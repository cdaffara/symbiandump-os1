// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// \file mnetwork.cpp
// Basic GSM Network and Operator information Implementation file.
// This file contains the implementation of the CATNetworkInfo, CATDetectNetwork,
// CNotifyNetworkRegistrationStatusChange and CCurrentNetworkChangedNotify classes.
// Find the current Network Operator in Numeric, Long and Short formats.
// NB - Some phones do not fully support all the formats, if any. The commands are 
// sent in sequence starting with the query command (AT+COPS?). All the phones currently
// supported implement this command. Valid responses are set in the TMobilePhoneLocationAreaV1
// and the TMobilePhoneNetworkInfoV1 structures and the Network's status is set to Current.
// 
//

#include <et_phone.h>
#include "mnetwork.h"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"
#include "mPHONE.H"

const TInt KSettlingPause=1;	//< This pause is used at the end of the network information queries.  The Nokia 8210 has been shown to require this, if it is not to error the next command.

_LIT8(KSetAndReadOperatorCommandNumeric, "AT+COPS=3,2;+COPS?\r");
_LIT8(KSetAndReadOperatorCommandShort, "AT+COPS=3,1;+COPS?\r");
_LIT8(KSetAndReadOperatorCommandLong, "AT+COPS=3,0;+COPS?\r");

_LIT8(KGetCurrentOperatorCommand, "AT+COPS?\r");
_LIT8(KOperatorResponse,"+COPS:");

_LIT8(KDetectNetworkCommand,"AT+COPS=?");


static TInt NetworkIdL(const TDesC8& aCode,
					   RMobilePhone::TMobilePhoneNetworkCountryCode& aCountryCode, 
					   RMobilePhone::TMobilePhoneNetworkIdentity& aNetworkIdentity)
	{
/**
 * Utility function to translate the 5 digit ASCII network identification returned by the ME
 * into Mobile Country Code (aCountryCode) and a Mobile Network Code (aNetworkIdentity) strings.
 * Tbe format returned by the ME is XXXYY, where XXX represents the Mobile Country Code and YY
 * represents the Mobile Network Code.
 */
	if (aCode.Length()!=5)
		{
		return KErrGeneral;
		}
	
	aCountryCode.SetLength(3);
	aCountryCode[0] = aCode[0];
	aCountryCode[1] = aCode[1];
	aCountryCode[2] = aCode[2];

	aNetworkIdentity.SetLength(2);
	aNetworkIdentity[0] = aCode[3];
	aNetworkIdentity[1] = aCode[4];
	return KErrNone;
	}


CATNetworkInfo* CATNetworkInfo::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard two-phase constructor.
 */
	{
	CATNetworkInfo* netInfo = new(ELeave) CATNetworkInfo(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(netInfo);
	netInfo->ConstructL();
	CleanupStack::Pop();
	return netInfo;
	}

CATNetworkInfo::CATNetworkInfo(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	,iPhoneGlobals(aPhoneGlobals)
	{}

CATNetworkInfo::~CATNetworkInfo()
/**
 * Standard Destructor.
 * Note the NULLing of the iATNetworkInfo pointer.  This should prevent any unwanted calling
 * of this function from the "end of command" sequence after the class' destruction.
 */
	{
	iIo->RemoveExpectStrings(this);
	iPhoneGlobals->iATNetworkInfo=NULL;
	}

void CATNetworkInfo::Start(TTsyReqHandle aTsyReqHandle,TAny* aParam)
/**
 * This is the standard entry point for retrieving the Network Information.
 */
	{
	LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tStarting Operator Info."));
	iReqHandle = aTsyReqHandle;
	if (aParam!=NULL)
		{
		TTsyNetworkInfo* info = static_cast<TTsyNetworkInfo*>(aParam);
		iNetworkInfo.iNetworkInfoPckg = info->iNetworkInfoPckg;
		iNetworkInfo.iArea = info->iArea;
		}

	//
	// This object is not always started thru the CATBase::ExecuteCommand
	// So we sometimes have to manually control the flow control of iEventSignalActive
	iPhoneGlobals->iEventSignalActive = ETrue;
	iCancelled=EFalse;

	if (iReqHandle==0)
		iTelObject->FlowControlSuspend();
	StartGetOperator();
	}

void CATNetworkInfo::CheckOperator()
/**
 * This function is called whenever an EventSignal Completes.  It is a second entry point for the command.
 * It checks CPhoneGlobals iPhoneStatus.iNetworkChanged and calls this function if it detects that the
 * networking information has changed and therefore the CPhoneGlobals data may be out of date and needs
 * to be refreshed.
 */
	{
	Start(0,0);
	}


void CATNetworkInfo::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * 
 * \par EATGetCurrentOperatorWriteComplete
 * Wait for the "AT+COPS?" write to complete
 *
 * \par EATGetCurrentOperatorReadComplete
 * Parse the "AT+COPS?" response.  If it gets an "ERROR" back it assume that no COPS commands are
 * supported, so complete the request with an error.  If it is supported, parse the result and proceed
 * to retrieve the numeric network code with "AT+COPS=3,2;+COPS?"
 *
 * \par EATNumericOperatorWaitForWriteComplete
 * Wait for the "AT+COPS=3,2;+COPS?" to complete
 *
 * \par EATSetNumericOperatorReadComplete
 * Parse the numeric network code if available.  Proceed to retrieve the short network code with
 * "AT+COPS=3,1;+COPS?".
 *
 * \par EATShortOperatorWaitForWriteComplete
 * Wait for the "AT+COPS=3,1;+COPS?" write to complete.
 *
 * \par EATSetShortOperatorReadComplete
 * Parse the short network name if available.  Proceed to retrieve the long network name with
 * "AT+COPS=3,0;+COPS?".
 *
 * \par EATLongOperatorWaitForWriteComplete
 * Wait for the "AT+COPS=3,0;+COPS?" write to complete.
 *
 * \par EATSetLongOperatorReadComplete
 * Parse the long network name if available.  Proceed to complete the request. 
 * Check for outstanding current network operator notifications at this point. 
 *
 * \par EATWaitForStopState
 * This state is used when stopping the state machine to ensure that a pending modem response has been
 * retrieved before freeing up the Multimode TSY to send more commands.  Prematurely stopping the state machine
 * is usually forced by a client cancel request.
 *
 * \par EATWaitForSettlingTimeout
 * This state is used to provide a pause at the end of the network information queries. The Nokia 8210
 * has been shown to require this, if it is not to arbitrarily error the next command.
 */
	{
	if ((aSource==ETimeOutCompletion)&&(iState!=EATWaitForSettlingTimeout))
		{
		LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tTimeout Error during Operator read"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case EATGetCurrentOperatorWriteComplete:
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetCurrentOperatorReadComplete;
			}
			break;

		case EATGetCurrentOperatorReadComplete:
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\t+COPS? not supported"));
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tSet Operator Command not supported")); // An assumption
				Complete(ret, aSource);	// No point in sending the Set Command.
				}
			else if (iCancelled)
				{
				Complete(KErrCancel, aSource);	// Cancel state machine in safe place
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\t+COPS? supported"));
				TRAPD(ret,ParseOperatorResponseL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tError parsing +COPS?"));
					}								
				iTxBuffer.Copy(KSetAndReadOperatorCommandNumeric); // AT+COPS=3,2;+COPS?
				iIo->Write(this, iTxBuffer);
				iIo->SetTimeOut(this, 5000);
				iState=EATNumericOperatorWaitForWriteComplete;
				}
			}
			break;		

		case EATNumericOperatorWaitForWriteComplete:
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATSetNumericOperatorReadComplete;
			}
			break;

		case EATSetNumericOperatorReadComplete:
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (iCancelled)
				{
				Complete(KErrCancel, aSource);	// Cancel state machine in safe place
				return;
				}
			else if (ret != KErrNone) 
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tAT+COPS=3,2 not supported"));				
				}				
			else	// Set Operator Command was successful, Read the Current Operator
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\t	AT+COPS=3,2 supported"));
				TRAPD(ret,ParseOperatorResponseL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tError parsing +COPS=3,2"));
					}					
				}
			iTxBuffer.Copy(KSetAndReadOperatorCommandShort); // AT+COPS=3,1;+COPS?
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATShortOperatorWaitForWriteComplete; 
			}
			break;

		case EATShortOperatorWaitForWriteComplete:
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATSetShortOperatorReadComplete;
			}
			break;

		case EATSetShortOperatorReadComplete:
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
				{
				iIo->WriteAndTimerCancel(this);
				TInt ret(ValidateExpectString());
				RemoveStdExpectStrings();
				if (iCancelled)
					{
					Complete(KErrCancel, aSource);	// Cancel state machine in safe place
					return;
					}
				else if (ret != KErrNone)
					{
					LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tAT+COPS=3,1 not supported"));					
					}
				else// Set Operator Command was successful, Read the Current Operator
					{	
					LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\t	AT+COPS=3,1 supported"));
					TRAPD(ret,ParseOperatorResponseL());
					if (ret != KErrNone)
						{
						LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tError parsing +COPS=3,1"));
						}						
					}
				iTxBuffer.Copy(KSetAndReadOperatorCommandLong); // AT+COPS=3,0;+COPS?
				iIo->Write(this, iTxBuffer);
				iIo->SetTimeOut(this, 5000);
				iState=EATLongOperatorWaitForWriteComplete; 
				}
			break;

		case EATLongOperatorWaitForWriteComplete:
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATSetLongOperatorReadComplete;
			}
			break;

		case EATSetLongOperatorReadComplete:
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\t+COPS=3,0 not supported"));
				}
			else	// Set Operator cmd was successful, Read the Current Operator
				{
				LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tAT+COPS=3,0 supported"));
				TRAPD(ret,ParseOperatorResponseL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tError parsing +COPS=3,0"));
					}						
				}

			// Check outstanding current network operator notifications
			iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,ECurrentNetworkChanged);

			iIo->SetTimeOut(this, KSettlingPause*1000);
			iState=EATWaitForSettlingTimeout;
			}
			break;

		case EATWaitForSettlingTimeout:
			__ASSERT_ALWAYS((aSource==ETimeOutCompletion),Panic(EATCommand_IllegalCompletionWriteExpected));
			Complete(KErrNone,aSource);
			break;
	
		case EATWaitForStopState:
			__ASSERT_ALWAYS(aSource==EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			Complete(KErrCancel, aSource);
			}
			break;

		default:
			break;
		}//switch
	}//EventSignal

void CATNetworkInfo::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function is used to prematurely stop the state machine.  This would usually occur following a
 * client cancel request.
 */
{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("MMTsy:\tCATNetworkInfo:\tCancel called."));
	iCancelled=ETrue;
}


void CATNetworkInfo::Complete(TInt aError,TEventSource aSource)
/**
 * This function completes the client's request.
 */
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	if (aSource==EWriteCompletion)
		iIo->Read();
	iState = EATNotInProgress;
	LOGTEXT2(_L8("MMTsy:CATNetworkInfo:\tCATNetworkInfo completed with error code : %d"), aError);
	//
	// BEWARE: The next command will be started from inside ReqCompleted or FlowControlResume
	// 
	//
	// This object is not always started thru the CATBase::ExecuteCommand
	// So we sometimes have to manually control the flow control of iEventSignalActive
	iPhoneGlobals->iEventSignalActive = EFalse;

	if (iReqHandle)
		{
		// Explicit call from a client
		if (aError==KErrNone)
			{
			GetCurrentNetworkInfo(&iNetworkInfo); 
			}
		iTelObject->ReqCompleted(iReqHandle, aError);
		}
	else
		{
		// CheckOperator() call from CATCommands::Complete
		iTelObject->FlowControlResume();
		}
	CATCommands::Complete(aError,aSource);
	}

void CATNetworkInfo::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
/**
 * This function handles I/O errors that may occur during any of the stages of the state machine.
 */
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		if (iReqHandle)
			{
			iTelObject->ReqCompleted(iReqHandle, aStatus);
			}
		else
			{
			// CheckOperator() call from CATCommands::Complete
			iTelObject->FlowControlResume();
			}
		iState = EATNotInProgress;
		}
	}

void CATNetworkInfo::StartGetOperator()
/**
 * This function kicks off the state machine by transmitting the first command, "AT+COPS?" and
 * setting the state.
 */
	{
	iPhoneGlobals->iPhoneStatus.iNetworkChanged=EFalse;
	InitializeNetworkInfo();		// Initialize the TNetworkInfo struct that will contain the mobile's responses
	iTxBuffer.Copy(KGetCurrentOperatorCommand);
	iIo->Write(this, iTxBuffer);
	iIo->SetTimeOut(this, 5000);
	iState=EATGetCurrentOperatorWriteComplete;
	}

void CATNetworkInfo::InitializeNetworkInfo()
/**
 * This function is used to initialize (to zero) the network information elements in the CPhoneGlobals
 * structure.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkinfo = iPhoneGlobals->iPhoneStatus.iCurrentNetwork;

	networkinfo.iMode = RMobilePhone::ENetworkModeUnknown;
	networkinfo.iCountryCode.Zero();		// Unknown, MCC
	networkinfo.iNetworkId.Zero();			// MNC
	networkinfo.iCdmaSID.Zero();			// Unused CDMA field
	networkinfo.iAnalogSID.Zero();			// Unused CDMA field
	networkinfo.iShortName.Zero();
	networkinfo.iLongName.Zero();
	networkinfo.iStatus = RMobilePhone::ENetworkStatusUnknown; 
	networkinfo.iDisplayTag.Zero();
	}

void CATNetworkInfo::ParseOperatorResponseL()
/**
 * This function is used to parse the ME responses to commands to retrieve numeric, short name 
 * or long name network information.  Possible responses are:
 * +COPS: mode, 0, "long format name"
 * +COPS: mode, 1, "short format name"
 * +COPS: mode, 2, "operator ID in hex"
 * +COPS: mode  (no network information available)
 *
 * Relevant information is retrieved and written into the CPhoneGlobals class.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfo = iPhoneGlobals->iPhoneStatus.iCurrentNetwork; 

	ParseBufferLC();	// Parse the buffer and create a linked list with the phone's response

	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	// The 1st item in the list should be the +COPS: response
	entry=iter++;
	if (entry && entry->iResultPtr.MatchF(KOperatorResponse)!=0)
		User::Leave(KErrGeneral);

	// The next item is the mode (this information is not used)
	entry=iter++;
	if (!entry)
		User::Leave(KErrGeneral);  
	
	// The next item should be the format. If there is no network information then this item
	// would be empty. The network status is consequently updated.
	entry=iter++;
	if (!entry)  // Not connected, so the response is "+COPS: 0"
		networkInfo.iStatus=RMobilePhone::ENetworkStatusUnknown;
	else	
		{
		TInt format=CATParamListEntry::EntryValL(entry);
		
		// If the format is available, the network name is the next item in the list 
		entry=iter++;
		if (!entry)
			User::Leave(KErrGeneral);
		switch (format)
			{
		case 0:
			if((entry->iResultPtr).Length() > networkInfo.iLongName.MaxLength())
				networkInfo.iLongName.Copy((entry->iResultPtr).Mid(0,networkInfo.iLongName.MaxLength()));			
			else
				networkInfo.iLongName.Copy(entry->iResultPtr);
			break;
		case 1:
			if((entry->iResultPtr).Length() > networkInfo.iShortName.MaxLength())
				networkInfo.iShortName.Copy((entry->iResultPtr).Mid(0,networkInfo.iShortName.MaxLength()));			
			else 
				networkInfo.iShortName.Copy(entry->iResultPtr);	
			break;
		case 2:
			User::LeaveIfError(NetworkIdL(entry->iResultPtr,networkInfo.iCountryCode, networkInfo.iNetworkId));
			break;
		default:
			User::Leave(KErrGeneral);
			break;
			}

		// We've got an answer, so this must be the current network
		networkInfo.iStatus=RMobilePhone::ENetworkStatusCurrent;
		}
	CleanupStack::PopAndDestroy();
	}

void CATNetworkInfo::GetCurrentNetworkInfo(CATNetworkInfo::TTsyNetworkInfo* aNetworkInfo)
/**
 * This function checks to see if the network registration information in CPhoneGlobals is valid,
 * and, if it is, it copies the CPhoneGlobals network information into the waiting client request
 * buffer.
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfo = (*(aNetworkInfo->iNetworkInfoPckg))();
	RMobilePhone::TMobilePhoneLocationAreaV1& areaInfo = *(aNetworkInfo->iArea);
	RMobilePhone::TMobilePhoneRegistrationStatus status = iPhoneGlobals->iPhoneStatus.iRegistrationStatus;

	if (status == RMobilePhone::ERegisteredOnHomeNetwork
		|| status == RMobilePhone::ERegisteredRoaming
		|| status == RMobilePhone::ERegistrationUnknown) // AT+CREG? is not supported, but info obtained during initialization is still valid. NickyM
		{
		RMobilePhone::TMobilePhoneNetworkInfoV1& info = iPhoneGlobals->iPhoneStatus.iCurrentNetwork;
		RMobilePhone::TMobilePhoneLocationAreaV1& locationInfo = iPhoneGlobals->iPhoneStatus.iLocationArea;

		networkInfo.iStatus			= info.iStatus;
		networkInfo.iCountryCode	= info.iCountryCode;	
		networkInfo.iNetworkId		= info.iNetworkId;
		networkInfo.iCdmaSID		= info.iCdmaSID;   // for CDMA
		networkInfo.iAnalogSID		= info.iAnalogSID; // for CDMA
		networkInfo.iShortName		= info.iShortName;
		networkInfo.iLongName		= info.iLongName;
		networkInfo.iDisplayTag		= info.iLongName;		// This TSY handles the display tag as the same as longname
		networkInfo.iMode			= iPhoneGlobals->iNetworkMode;
		areaInfo.iAreaKnown			= locationInfo.iAreaKnown;
		areaInfo.iLocationAreaCode	= locationInfo.iLocationAreaCode;
		areaInfo.iCellId			= locationInfo.iCellId;
		}
	else
		{
		networkInfo.iStatus			= RMobilePhone::ENetworkStatusUnknown;
		networkInfo.iShortName.Zero();			// Should this be .Zero() ?
		networkInfo.iLongName.Zero();			// Should this be .Zero() ?
		areaInfo.iAreaKnown			= EFalse;	
		areaInfo.iLocationAreaCode	= 0;
		areaInfo.iCellId			= 0;

		}
	return;
	}


// class CNotifyNetworkRegistrationStatusChange

CNotifyNetworkRegistrationStatusChange* CNotifyNetworkRegistrationStatusChange::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject, 
																					 RMobilePhone::TMobilePhoneRegistrationStatus* aRegStatus)
	{
	return new(ELeave) CNotifyNetworkRegistrationStatusChange(aReqHandle,aTelObject,aRegStatus);
	}

CNotifyNetworkRegistrationStatusChange::CNotifyNetworkRegistrationStatusChange(TTsyReqHandle aReqHandle,CTelObject* aTelObject, 
																			   RMobilePhone::TMobilePhoneRegistrationStatus* aRegStatus)
	: CNotifyBase(aReqHandle,aTelObject)
	{
	iRegStatus = aRegStatus;
	}

CNotifyNetworkRegistrationStatusChange::~CNotifyNetworkRegistrationStatusChange()
	{}

TBool CNotifyNetworkRegistrationStatusChange::CheckAndCompleteNotification(CTelObject* aTelObject, TEvent aEvent, TEvent /*aLastEvent*/)
/**
 * This function completes a Registration status Notification.
 */
	{	
	if (aEvent!=ERegistrationStatusChanged)
		return EFalse;

	LOGTEXT2(_L8("Event %d:\tRegistration Status Changed Notification completed"),aEvent);
	*iRegStatus = reinterpret_cast<CPhoneMobile*>(aTelObject)->RegistrationStatus();
	iTelObject->ReqCompleted(iReqHandle,KErrNone);
	return ETrue;
	}

// class CCurrentNetworkChangedNotify

CNotifyCurrentNetworkChange* CNotifyCurrentNetworkChange::NewL(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TInt* aInfo)
	{	
	return new(ELeave) CNotifyCurrentNetworkChange(aReqHandle,aTelObject,aInfo);
	}

CNotifyCurrentNetworkChange::CNotifyCurrentNetworkChange(TTsyReqHandle aReqHandle,CTelObject* aTelObject,TInt* aInfo)
	: CNotifyBase(aReqHandle,aTelObject)
	{
	CATNetworkInfo::TTsyNetworkInfo* info = reinterpret_cast<CATNetworkInfo::TTsyNetworkInfo*>(aInfo);
	iNetworkInfo.iNetworkInfoPckg = info->iNetworkInfoPckg;
	iNetworkInfo.iArea = info->iArea;
	}

CNotifyCurrentNetworkChange::~CNotifyCurrentNetworkChange()
	{}

TBool CNotifyCurrentNetworkChange::CheckAndCompleteNotification(CTelObject* aTelObject,TEvent aEvent,TEvent /*aLastEvent*/)
/**
 * This function completes a Current Network Change Notification.
 */
	{	
	if (aEvent!=ECurrentNetworkChanged)
		return EFalse; 

	LOGTEXT2(_L8("Event %d:\tCurrent Network Changed Notification completed"),aEvent);
//	REINTERPRET_CAST(CPhoneMobile*,aTelObject)->CurrentNetworkInfo(&iNetworkInfo);
	reinterpret_cast<CPhoneMobile*>(aTelObject)->CurrentNetworkInfo(&iNetworkInfo);
	iTelObject->ReqCompleted(iReqHandle,KErrNone);
	return ETrue;
	}

// Class CATDetectNetwork
//
// Detect all available networks.

CATDetectNetwork::CATDetectNetwork(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATDetectNetwork* CATDetectNetwork::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard 2 phase constructor.
 */
	{
	CATDetectNetwork* self = new(ELeave) CATDetectNetwork(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATDetectNetwork::~CATDetectNetwork()
/**
 * Destructor.
 */
	{
	iIo->RemoveExpectStrings(this);
	}

void CATDetectNetwork::Start(TTsyReqHandle aTsyReqHandle,TAny* aParam)
/**
 * This is the standard entry point for retrieving the detected Operator Information.
 */
	{
	LOGTEXT(_L8("MMTsy:\tCATDetectNetwork:\tStarting Network detection"));
	if (aParam != NULL)
		{
		TTsyDetectedNetworks*  info = static_cast<TTsyDetectedNetworks* >(aParam);
		iDetectedNetworks.iBufSize = info->iBufSize;
		iDetectedNetworks.iBufPtr = info->iBufPtr;
		}

	iReqHandle = aTsyReqHandle;

	WriteExpectingResults(KDetectNetworkCommand, 3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL,Panic(EGeneral));
	iState=EATDetectWaitForWriteComplete;
	}

void CATDetectNetwork::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * 
 * \par EATDetectWaitForWriteComplete
 * Wait for the "AT+COPS=?" write to complete
 *
 * \par EATDetectReadComplete
 * Reads the Modem's response to the AT+COPS=? command.  If it gets an "ERROR" back it resets the 
 * timer and sets a new state (EATDetectExtendedReadComplete) and waits for another response.
 *
 * \par EATDetectExtendedReadComplete
 * Reads the phone's response to the AT+COPS=? command and parses the response. Completes the
 * request.
 *
 * \par EATWaitForStopState
 * This state is used when stopping the state machine to ensure that a pending modem response has been
 * retrieved before freeing up the Multimode TSY to send more commands.  Prematurely stopping the state machine
 * is usually forced by a client cancel request.
 */
	{
	if (aSource==ETimeOutCompletion)
		{
		if (iState!=EATDetectExtendedReadComplete)
			{
			LOGTEXT(_L8("MmTsy:\tCATDetectNetwork:\tTimeout Error during Network Detect read"));
			RemoveStdExpectStrings();
			Complete(KErrTimedOut,aSource);
			return;
			}
		else
			{
			// the phone really does not support +COPS=?, so report that error
			RemoveStdExpectStrings();
			Complete(KErrNotSupported,EReadCompletion);
			return;
			}
		}

	switch(iState)
		{
	case EATDetectWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{
		iIo->WriteAndTimerCancel(this);
		iIo->SetTimeOut(this,45 * KOneSecondPause); // Some phones take a long time to detect all the networks
		AddStdExpectStrings();
		iState=EATDetectReadComplete;
		}
		break;

	case EATDetectReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		iIo->WriteAndTimerCancel(this);
		if (iIo->FoundChatString()==iErrorExpectString)
			{
			LOGTEXT(_L8("Modem returned ERROR - waiting for further response"));
			iIo->ClearCurrentLine();
			iIo->SetTimeOut(this,45 * KOneSecondPause);
			iState = EATDetectExtendedReadComplete;
			break;
			}
		// fall through ...

	case EATDetectExtendedReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		iIo->WriteAndTimerCancel(this);
		{
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret==KErrNone)
			TRAP(ret,ParseResponseL());
		Complete(ret,aSource); // The return value is set in the Complete methode
		}
		break;

	case EATWaitForStopState:
		__ASSERT_ALWAYS(aSource==EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
		{
		iIo->WriteAndTimerCancel(this);
		Complete(KErrCancel, aSource);
		}
		break;

	default:
		break;
		}//switch
	}//EventSignal

void CATDetectNetwork::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function is used to prematurely stop the state machine.  This would usually occur following a
 * client cancel request.
 */
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("MMTsy:\tCATDetectNetwork:\tCancel called."));
	switch(iState)
		{
		case EATNotInProgress:
		case EATDetectWaitForWriteComplete:
			{
			LOGTEXT2(_L8("Current state TSY is cancelling from %d"), iState);
			Complete(KErrCancel, EReadCompletion);
			}
			break;

		default:
			{
			LOGTEXT(_L8("MmTsy:\tCATDetectNetwork:\tStop, now waiting for expected modem response"));
			AddStdExpectStrings();
			iIo->SetTimeOut(this);
			iState = EATWaitForStopState;
			}
			break;
		}
	}


void CATDetectNetwork::Complete(TInt aError,TEventSource aSource)
/**
 * This function completes the client request.
 */
	{
	iState = EATNotInProgress;
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	CATCommands::Complete(aError,aSource);

	LOGTEXT2(_L8("MMTsy:CATDetectNetwork:\tCATDetectNetwork completed with error code : %d"), aError);
	if (aError==KErrCancel)
		{
		CPhoneMobile* phone = static_cast<CPhoneMobile*>(iTelObject);
		phone->CompleteDetectedNetworksCancel(iReqHandle);
		}
	else
		iTelObject->ReqCompleted(iReqHandle, aError);
	}

void CATDetectNetwork::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
/**
 * This function handles I/O errors that may occur during any of the stages of the state machine.
 */
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iState = EATNotInProgress;
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		}
	}

void CATDetectNetwork::ParseResponseL()
/**
 * This function parses the phone's response to the Detect Networks command.
 */
	{
	// should be
	//
	// +COPS: (status,"longname","shortname",code),... ,,(list),(list)
	//
	// but SH888 gives
	//
	// +COPS: (status,"longname","shortname",code)<cr><lf>
	// +COPS: ...
	//

	CMobilePhoneNetworkList* list=CMobilePhoneNetworkList::NewL();
	CleanupStack::PushL(list);

	ParseBufferLC();

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;	
	// This TSY only supports GSM mode so we can hardcode this
	info.iMode=RMobilePhone::ENetworkModeGsm;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;
	if (entry)
		{
		if (entry->iResultPtr!=KOperatorResponse)
			User::Leave(KErrGeneral);
		//
		entry=iter++;
		if (!entry)
			User::Leave(KErrGeneral);
		for (;;)
			{
			info.iStatus = RMobilePhone::TMobilePhoneNetworkStatus(CATParamListEntry::EntryValL(entry));
			//
			entry=iter++;
			if (!entry)
				User::Leave(KErrGeneral);
			info.iLongName.Copy(entry->iResultPtr);		
			//
			entry=iter++;
			if (!entry)
				User::Leave(KErrGeneral);
			if((entry->iResultPtr).Length() > info.iShortName.MaxLength())
				info.iShortName.Copy((entry->iResultPtr).Mid(0,info.iShortName.MaxLength()));			
			else
				info.iShortName.Copy(entry->iResultPtr);
			//
			entry=iter++;
			if (!entry)
				User::Leave(KErrGeneral);
			User::LeaveIfError(NetworkIdL(entry->iResultPtr,info.iCountryCode, info.iNetworkId));  // MNC and MCC	
			
			list->AddEntryL(info); 
		
			entry=iter++;
			if (!entry)
				break;
			if (entry->iResultPtr.Length()==0)
				break;
			if (entry->iResultPtr!=KOperatorResponse)
				continue;
			entry=iter++;
			if (!entry)
				break;
			}
		}

	CleanupStack::PopAndDestroy();			// results

	*(iDetectedNetworks.iBufPtr) = list->StoreLC();	 // Stream the list to optimice the size of it.
	*(iDetectedNetworks.iBufSize) = (*(iDetectedNetworks.iBufPtr))->Size(); // Store the size of the streamed list to be passed back to the client.   

	CleanupStack::Pop();					// pop the CBufBase allocated by StoreLC
	CleanupStack::PopAndDestroy();			// pop AND detroy the list.

	} // End of ParseResponseL()  



