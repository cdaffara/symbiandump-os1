// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "gprs.h"
#include "Gprscontext.h"
#include "mSLOGGER.H"
#include <pcktcs.h>
#include "ATGprsConfig.h"
#include "ATIO.H"
#include <etelpckt.h>
#include "NOTIFY.H"
#include "Matstd.h"

_LIT8(KGetCGDCONTCommand,"AT+CGDCONT?\r");
_LIT8(KIPType4	, "IP");
_LIT8(KIPType6	, "IP");
_LIT8(KX25		, "X25");


 /**
 * @file
 * This file implements the CATGPRSSetConfig class and the CATGPRSGetConfig. These two classes are used by the 
 * GPRS AT TSY library. 
 * This state machine uses "AT+CGQREQ" "AT+CGDCONT" "AT+CGQMIN" commands.
 */
CATGPRSSetConfig* CATGPRSSetConfig::NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 *  Standard 2 phase constructor.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGPRSSetConfig* p=new(ELeave) CATGPRSSetConfig(aCid, aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGPRSSetConfig::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}


CATGPRSSetConfig::CATGPRSSetConfig(TInt aCid, CATIO* aIo, CTelObject *aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)	
	: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals), iCid(aCid)
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsClass::CATGprsClass called"));
	}


CATGPRSSetConfig::~CATGPRSSetConfig()
/**
 * Destructor.
 */ 
	{
	LOGTEXT(_L8("CATGPRSSetConfig::~CATGPRSSetConfig called"));
	}


void CATGPRSSetConfig::Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig)
/**
 * This starts the sending of the set commands.
 *
 * @param aConfig config package.
 * @param aTsyReqHandle handle to the client.
 */
	{
	iReqHandle = aTsyReqHandle;
	TPckg<RPacketContext::TContextConfigGPRS>* contextConfigV1Pckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
	iContextConfigGPRS= &(*contextConfigV1Pckg)();
	TInt ret=MakeupCGDCONT();
	if(ret)
		{
		Complete(KErrNotSupported,ETimeOutCompletion);
		return;
		}
	
	LOGTEXT(_L8("CATGPRSSetConfig:\tCATConfigGPRS::Start function called in TSY"));
	
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iState=EWaitForSetCGDCONTComplete;
	Write(KGprsCommandTimeOut);	
	}

void CATGPRSSetConfig::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function cancels the outstanding read and sets the state to EWaitForDSR.
 *
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGPRSSetConfig::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGPRSSetConfig::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}


void CATGPRSSetConfig::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 *
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	Complete(aStatus, aSource);
	}


void CATGPRSSetConfig::Complete(TInt aErr, TEventSource aSource)
/**
 * This Function completes the get or set command from the client.
 * @param aErr an error code to relay to client.
 */
	{
	LOGTEXT(_L8("CATGPRSSetConfig::Complete"));
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this); 
	if (aErr == KErrNone)
		{
		((CGprsContext*)iTelObject)->SetConfig(iContextConfigGPRS);
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketContextConfigChanged);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aErr,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aErr);

	iState = EATNotInProgress;
	}


void CATGPRSSetConfig::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively in 
 * get and set states and are described below.
 *	
 * @par  aSource Source of function call.
 *
 * @par EWaitForDSR
 * Wait for the DSR command to complete. Also sends AT+CGDCONT set or get command to the phone.
 *
 * @par	EWaitForSetCGDCONTComplete,
 * Wait for response from the phone on the set command. 
 *
 * @par		EWaitForSetCGDCONTOK,
 * Validate phone response and send set AT+CGQMIN command.
 * 
 *
 */
	{
	LOGTEXT2(_L8("CATGPRSSetConfig::EventSignal with iState %d"),iState);
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGPRSSetConfig:\tTimeout Error during Config"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	switch(iState)
		{
		case EWaitForSetCGDCONTComplete:
			LOGTEXT(_L8("CATGPRSSetConfig::EventSignal, EWaitForSetCGDCONTComplete"));
			StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
			iState = EWaitForSetCGDCONTOK;
			break;

		case EWaitForSetCGDCONTOK:
			{
			// Called when OK is received
			LOGTEXT(_L8("CATGPRSSetConfig::EventSignal, EWaitForSetCGDCONTOK"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret = ValidateExpectString();
			Complete(ret, aSource);
			}
			break;

		case EATNotInProgress:
			break;
		default:
			LOGTEXT(_L8("CATGPRSSetConfig::EventSignal, default. Panic"));
			Panic(EIllegalEvent);
			break;
		}			
	}


TInt CATGPRSSetConfig::MakeupCGDCONT()
/**
 * This Function creates the at set string for the AT+CGDCONT command.
 */
	{
	const TInt KPDPTypeIdent=5;
	TBuf8<KPDPTypeIdent>						pdpType;		// PDP Type identifier
	TBuf8<RPacketContext::KGSNNameLength>		gsnName;			// Access point Name
	TBuf8<RPacketContext::KMaxPDPAddressLength>	pdpAddress;	// PDP pre-assigned address
	switch(iContextConfigGPRS->iPdpType)
		{
	
		case RPacketContext::EPdpTypeIPv4:
			pdpType.Format(KIPType4);
		break;

		case RPacketContext::EPdpTypeIPv6:
			pdpType.Format(KIPType6);
		break;

		case RPacketContext::EPdpTypeX25:
			pdpType.Format(KX25);
		break;

		default:
			return KErrUnknown;
		}

	gsnName.Copy(iContextConfigGPRS->iAccessPointName);
	pdpAddress.Copy(iContextConfigGPRS->iPdpAddress);
	const TBool dataCompression=iContextConfigGPRS->iPdpCompression & RPacketContext::KPdpDataCompression;
	const TBool headerCompression=iContextConfigGPRS->iPdpCompression & RPacketContext::KPdpHeaderCompression;

	iTxBuffer.Format(_L8("AT+CGDCONT=%d,\"%S\",\"%S\",\"%S\",%d,%d\r"), iCid, &pdpType,
			&gsnName, &pdpAddress,dataCompression,headerCompression);

	return KErrNone;
	}



CATGPRSGetConfig* CATGPRSGetConfig::NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGPRSGetConfig* p=new(ELeave) CATGPRSGetConfig(aCid, aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGPRSGetConfig::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGPRSGetConfig::CATGPRSGetConfig(TInt aCid, CATIO* aIo, CTelObject *aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)	
	: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals), iCid(aCid)
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{}


CATGPRSGetConfig::~CATGPRSGetConfig()
/**
 * Destructor.
 */
	{}


void CATGPRSGetConfig::Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig)
/**
 * This starts the sending of the get commands.
 * 
 * @param aTsyReqHandle handle to the client.
 * @param aConfig Pointer to a RPacketContext::TContextConfigGPRS. 
 */

	{
	LOGTEXT(_L8("CATGPRSGetConfig::Start called"));

	TPckg<RPacketContext::TContextConfigGPRS>* contextConfigV1Pckg = (TPckg<RPacketContext::TContextConfigGPRS>*)aConfig;
	iContextConfigV1 = &(*contextConfigV1Pckg)();
	
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iReqHandle = aTsyReqHandle;
	iState=EWaitForGetCGDCONTComplete;
	Write(KGetCGDCONTCommand, KGprsCommandTimeOut);
	}



void CATGPRSGetConfig::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function cancels the outstanding read and sets the state to EWaitForDSR.
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGPRSGetConfig::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGPRSGetConfig::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}

void CATGPRSGetConfig::Complete(TInt aErr, TEventSource	aSource)
/**
 * This Function completes the get or set command from the client.
 * @param aErr and error to relay to the client.
 */
	{
	LOGTEXT(_L8("CATGPRSGetConfig::Complete"));
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this); 
	if(aErr == KErrNone)		
		{
		((CGprsContext*)iTelObject)->SetConfig(iContextConfigV1);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aErr,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aErr);

	iState = EATNotInProgress;
	}

void CATGPRSGetConfig::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	Complete(aStatus, aSource);
	}

void CATGPRSGetConfig::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively in 
 * get and set states and are described below.
 *	
 * @par  aSource Source of function call. 
 *
 *
 * @par		EWaitForGetCGDCONTComplete,
 * Wait for response from the phone on the set command.
 *
 * @par		EWaitForGetCGDCONTOK,
 * Validate phone response and send set AT+CGQMIN command.
 *
 */
	{
	LOGTEXT2(_L8("CATGPRSGetConfig::EventSignal with iState %d"),iState);
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGPRSGetConfig:\tTimeout Error during Config"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	switch(iState)
		{
		case EWaitForGetCGDCONTComplete:
				{
				iIo->WriteAndTimerCancel(this);
				StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
				iState = EWaitForGetCGDCONTOK;
				}
			break;
		case EWaitForGetCGDCONTOK:
				{
				// Called when OK is received
				__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
				TInt ret = ValidateExpectString();
				if(ret)
					{	
					Complete(ret, aSource);
					return;
					}
				TRAP_IGNORE(ParseCGDCONTResponseL());
				Complete(ret, aSource);
				}
			break;
			

		case EATNotInProgress:
			break;
		
		default:
				{
				LOGTEXT(_L8("CATGPRSGetConfig::EventSignal, Default, panic"));
				Panic(EIllegalEvent);
				}
			break;
		}			
	}


void CATGPRSGetConfig::ParseCGDCONTResponseL()
/**
 * This Function parses the response from the get AT+CGDCONT? command to the phone
 */
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	while(entry = iter++,entry!=NULL)
		{
		if (entry->iResultPtr.MatchF(KCGDCONTResponseString)!=0)
			continue;
		entry = iter++;
		if(entry == NULL)
			User::Leave(KErrNotFound);
		TLex8 lex(entry->iResultPtr);
		TInt val;
		(void)User::LeaveIfError(lex.Val(val));
		if(iCid == val)
			{
			entry = iter++;
			break;
			}
		}
	if(entry == NULL)
		User::Leave(KErrNotFound);
	// we are now pointing to the correct context, just parse the values
	// Get the pdp type
	TPtrC8 result(entry->iResultPtr);
	if(result == KIPType4)	
		iContextConfigV1->iPdpType = RPacketContext::EPdpTypeIPv4;
	else if(result == KIPType6)//EPdpTypeIPv6,
		iContextConfigV1->iPdpType = RPacketContext::EPdpTypeIPv6;
	else if(result == KX25)
		iContextConfigV1->iPdpType = RPacketContext::EPdpTypeX25;
	else
		User::Leave(KErrNotSupported);		
	//Get the gsnName
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrNotFound);
	result.Set(entry->iResultPtr);
	iContextConfigV1->iAccessPointName.Copy(result);
	// Get pdp address
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrNotFound);
	result.Set(entry->iResultPtr);
	iContextConfigV1->iPdpAddress.Copy(result);
	// get compression
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	iContextConfigV1->iPdpCompression = CATParamListEntry::EntryValL(entry);
	CleanupStack::PopAndDestroy();
	}


