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



#include "gprsqos.h"
#include "mSLOGGER.H"
#include <pcktcs.h>
#include "atgprsqosprofile.h"
#include "ATIO.H"
#include <etelpckt.h>
#include "NOTIFY.H"
#include "Matstd.h"

_LIT8(KGetCGQREQCommand,"AT+CGQREQ?\r");
_LIT8(KGetCGQMINCommand,"AT+CGQMIN?\r");
_LIT8(KCGQmin, "AT+CGQMIN=%d,%d,%d,%d,%d,%d\r");
_LIT8(KCGQreq, "AT+CGQREQ=%d,%d,%d,%d,%d,%d\r");


 /**
 * @file
 * This file implements the CATGprsSetProfile class and the CATGprsGetProfile. These two classes are used by the 
 * GPRS AT TSY library. 
 * This state machine uses "AT+CGQREQ" "AT+CGQMIN" commands.
 */
CATGprsSetProfile* CATGprsSetProfile::NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsSetProfile* p=new(ELeave) CATGprsSetProfile(aCid, aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGprsSetProfile::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}


CATGprsSetProfile::CATGprsSetProfile(TInt aCid, CATIO* aIo, CTelObject *aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)	
	: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals), iCid(aCid)
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsClass::CATGprsClass called"));
	}


CATGprsSetProfile::~CATGprsSetProfile()
/**
 * Destructor.
 */ 
	{
	LOGTEXT(_L8("CATGprsSetProfile::~CATGprsSetProfile called"));
	}


void CATGprsSetProfile::Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig)
/**
 * This starts the sending of the set commands.
 *
 * @param aConfig config package.
 * @param aTsyReqHandle handle to the client.
 */
	{
	TPckg<RPacketQoS::TQoSGPRSRequested>* qoSProfileV1Pckg = (TPckg<RPacketQoS::TQoSGPRSRequested>*)aConfig;
	iGprsReqProfile	= &(*qoSProfileV1Pckg)();
	MakeupCGQMIN();
	LOGTEXT(_L8("CATGprsSetProfile:\tCATConfigGPRS::Start function called in TSY"));

	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iReqHandle = aTsyReqHandle;
	iState=EWaitForSetCGQMINComplete;
	Write(KGprsCommandTimeOut);
	}

void CATGprsSetProfile::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function cancels the outstanding read and sets the state to EWaitForDSR.
 *
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsSetProfile::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsSetProfile::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}


void CATGprsSetProfile::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 *
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	Complete(aStatus, aSource);
	}


void CATGprsSetProfile::Complete(TInt aErr, TEventSource aSource)
/**
 * This Function completes the get or set command from the client.
 *
 * @param aErr an error code to relay to client.
 */
	{
	LOGTEXT(_L8("CATGprsSetProfile::Complete"));
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this); 
	if (aErr == KErrNone)
		{
		((CGprsQoS*)iTelObject)->SetReqQoSProfile(iGprsReqProfile);
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketQoSProfileChanged);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aErr,aSource);		
	iTelObject->ReqCompleted(iReqHandle,aErr);

	iState=EATNotInProgress;
	}


void CATGprsSetProfile::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively in 
 * get and set states and are described below.
 *	
 * @par  aSource Source of function call.
 *
 * @par		EWaitForSetCGQMINComplete,
 * Wait for response from the phone on the set command.
 *
 * @par		EWaitForSetCGQMINOK,
 * Validate phone response and send set AT+CGQREQ command.
 *
 * @par		EWaitForSetCGQREQComplete,
 * Wait for response from the phone on the set command.
 *
 * @par		EWaitForSetCGQREQOK,
 * Validate response and Complete set command.
 */
	{
	LOGTEXT2(_L8("CATGprsSetProfile::EventSignal with iState %d"),iState);
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGprsSetProfile:\tTimeout Error during Config"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	switch(iState)
		{
		case EWaitForSetCGQMINComplete:
			{
			LOGTEXT(_L8("CATGprsSetProfile::EventSignal, EWaitForSetCGQMINComplete"));
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
			iState = EWaitForSetCGQMINOK;
			}
			break;
		case EWaitForSetCGQMINOK:
			{
			LOGTEXT(_L8("CATGprsSetProfile::EventSignal, EWaitForSetCGQMINOK"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret = ValidateExpectString();
			RemoveStdExpectStrings();
			if(ret)
				{
				Complete(ret, aSource);
				return;
				}
			MakeupCGQREQ();
			Write(KGprsCommandTimeOut);
			iState = EWaitForSetCGQREQComplete;
			}
			break;
		case EWaitForSetCGQREQComplete:
			{
			LOGTEXT(_L8("CATGprsSetProfile::EventSignal, EWaitForSetCGQREQComplete"));
			StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
			iState = EWaitForSetCGQREQOK;
			}
		break;
		case EWaitForSetCGQREQOK:
			{
			LOGTEXT(_L8("CATGprsSetProfile::EventSignal, EWaitForSetCGQREQOK"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret = ValidateExpectString();
			Complete(ret, aSource);
			}
			break;
		case EATNotInProgress:
			break;
		default:
			{
			LOGTEXT(_L8("CATGprsSetProfile::EventSignal, default. Panic"));
			Panic(EIllegalEvent);
			}
			break;
		}			
	}


void CATGprsSetProfile::MakeupCGQMIN()
/**
 * This Function creates the at set string for the AT+CGQMIN command.
 */
	{
	TInt precedence, delay, reliability, peakthru, meanthru = 0;

	switch (iGprsReqProfile->iMinPrecedence)
		{
		case RPacketQoS::EPriorityHighPrecedence:
			precedence = 1; // high precedence
		break;

		case RPacketQoS::EPriorityMediumPrecedence:
			precedence = 2; // normal precedence
		break;
		
		case RPacketQoS::EPriorityLowPrecedence:
			precedence = 3; // low precedence
		break;
		
		default:
			precedence = 0; // unspecified or unknown precedence
		break;
		};

	switch (iGprsReqProfile->iMinDelay)
		{
		case RPacketQoS::EDelayClass1:
			delay = 1;
		break;

		case RPacketQoS::EDelayClass2:
			delay = 2;
		break;

		case RPacketQoS::EDelayClass3:
			delay = 3;
		break;

		case RPacketQoS::EDelayClass4:
			delay = 4;
		break;

		default:
			delay = 0;
		break;
		};

	switch (iGprsReqProfile->iMinReliability)
		{
		case RPacketQoS::EReliabilityClass1:
			reliability = 1;
		break;

		case RPacketQoS::EReliabilityClass2:
			reliability = 2;
		break;

		case RPacketQoS::EReliabilityClass3:
			reliability = 3;
		break;

		case RPacketQoS::EReliabilityClass4:
			reliability = 4;
		break;

		case RPacketQoS::EReliabilityClass5:
			reliability = 5;
		break;

		default:
			reliability = 0; // unspecified or unknown reliability
		break;
		};

	switch (iGprsReqProfile->iMinPeakThroughput)
		{
		case RPacketQoS::EPeakThroughput1000:
			peakthru = 1;
		break;

		case RPacketQoS::EPeakThroughput2000:
			peakthru = 2;
		break;

		case RPacketQoS::EPeakThroughput4000:
			peakthru = 3;
		break;

		case RPacketQoS::EPeakThroughput8000:
			peakthru = 4;
		break;

		case RPacketQoS::EPeakThroughput16000:
			peakthru = 5;
		break;

		case RPacketQoS::EPeakThroughput32000:
			peakthru = 6;
		break;

		case RPacketQoS::EPeakThroughput64000:
			peakthru = 7;
		break;

		case RPacketQoS::EPeakThroughput128000:
			peakthru = 8;
		break;

		case RPacketQoS::EPeakThroughput256000:
			peakthru = 9;
		break;
		
		default:
			peakthru = 0; // unspecified or unknown peak throughput
		break;
		};

	switch (iGprsReqProfile->iMinMeanThroughput)
		{
		case RPacketQoS::EMeanThroughput100:
			meanthru = 2;
		break;

		case RPacketQoS::EMeanThroughput200:
			meanthru = 3;
		break;

		case RPacketQoS::EMeanThroughput500:
			meanthru = 4;
		break;

		case RPacketQoS::EMeanThroughput1000:
			meanthru = 5;
		break;

		case RPacketQoS::EMeanThroughput2000:
			meanthru = 6;
		break;

		case RPacketQoS::EMeanThroughput5000:
			meanthru = 7;
		break;

		case RPacketQoS::EMeanThroughput10000:
			meanthru = 8;
		break;

		case RPacketQoS::EMeanThroughput20000:
			meanthru = 9;
		break;
		
		case RPacketQoS::EMeanThroughput50000:
			meanthru = 10;
		break;

		case RPacketQoS::EMeanThroughput100000:
			meanthru = 11;
		break;

		case RPacketQoS::EMeanThroughput200000:
			meanthru = 12;
		break;
		case RPacketQoS::EMeanThroughput500000:
			meanthru = 13;
		break;

		case RPacketQoS::EMeanThroughput1000000:
			meanthru = 14;
		break;

		case RPacketQoS::EMeanThroughput2000000:
			meanthru = 15;
		break;

		case RPacketQoS::EMeanThroughput5000000:
			meanthru = 16;
		break;

		case RPacketQoS::EMeanThroughput10000000:
			meanthru = 17;
		break;

		case RPacketQoS::EMeanThroughput20000000:
			meanthru = 18;
		break;

		case RPacketQoS::EUnspecifiedMeanThroughput:
		case RPacketQoS::EMeanThroughput50000000:
			meanthru = 31;
		break;

		default:
			meanthru = 0; // unspecified or unknown mean throughput
		break;
		};


	iTxBuffer.Format(KCGQmin, iCid,	
			precedence,
			delay,
			reliability,
			peakthru,
			meanthru);
	}


void CATGprsSetProfile::MakeupCGQREQ()
/**
 * This Function creates the at set string for the AT+CGQREQ command.
 */
	{
	TInt precedence, delay, reliability, peakthru, meanthru = 0;

	switch (iGprsReqProfile->iReqPrecedence)
		{
		case RPacketQoS::EPriorityHighPrecedence:
			precedence = 1; // high precedence
		break;

		case RPacketQoS::EPriorityMediumPrecedence:
			precedence = 2; // normal precedence
		break;
		
		case RPacketQoS::EPriorityLowPrecedence:
			precedence = 3; // low precedence
		break;
		
		default:
			precedence = 0; // unspecified or unknown precedence
		break;
		};

	switch (iGprsReqProfile->iReqDelay)
		{
		case RPacketQoS::EDelayClass1:
			delay = 1;
		break;

		case RPacketQoS::EDelayClass2:
			delay = 2;
		break;

		case RPacketQoS::EDelayClass3:
			delay = 3;
		break;

		case RPacketQoS::EDelayClass4:
			delay = 4;
		break;

		default:
			delay = 0;
		break;
		};

	switch (iGprsReqProfile->iReqReliability)
		{
		case RPacketQoS::EReliabilityClass1:
			reliability = 1;
		break;

		case RPacketQoS::EReliabilityClass2:
			reliability = 2;
		break;

		case RPacketQoS::EReliabilityClass3:
			reliability = 3;
		break;

		case RPacketQoS::EReliabilityClass4:
			reliability = 4;
		break;

		case RPacketQoS::EReliabilityClass5:
			reliability = 5;
		break;

		default:
			reliability = 0; // unspecified or unknown reliability
		break;
		};

	switch (iGprsReqProfile->iReqPeakThroughput)
		{
		case RPacketQoS::EPeakThroughput1000:
			peakthru = 1;
		break;

		case RPacketQoS::EPeakThroughput2000:
			peakthru = 2;
		break;

		case RPacketQoS::EPeakThroughput4000:
			peakthru = 3;
		break;

		case RPacketQoS::EPeakThroughput8000:
			peakthru = 4;
		break;

		case RPacketQoS::EPeakThroughput16000:
			peakthru = 5;
		break;

		case RPacketQoS::EPeakThroughput32000:
			peakthru = 6;
		break;

		case RPacketQoS::EPeakThroughput64000:
			peakthru = 7;
		break;

		case RPacketQoS::EPeakThroughput128000:
			peakthru = 8;
		break;

		case RPacketQoS::EPeakThroughput256000:
			peakthru = 9;
		break;
		
		default:
			peakthru = 0; // unspecified or unknown peak throughput
		break;
		};

	switch (iGprsReqProfile->iReqMeanThroughput)
		{
		case RPacketQoS::EMeanThroughput100:
			meanthru = 2;
		break;

		case RPacketQoS::EMeanThroughput200:
			meanthru = 3;
		break;

		case RPacketQoS::EMeanThroughput500:
			meanthru = 4;
		break;

		case RPacketQoS::EMeanThroughput1000:
			meanthru = 5;
		break;

		case RPacketQoS::EMeanThroughput2000:
			meanthru = 6;
		break;

		case RPacketQoS::EMeanThroughput5000:
			meanthru = 7;
		break;

		case RPacketQoS::EMeanThroughput10000:
			meanthru = 8;
		break;

		case RPacketQoS::EMeanThroughput20000:
			meanthru = 9;
		break;
		
		case RPacketQoS::EMeanThroughput50000:
			meanthru = 10;
		break;

		case RPacketQoS::EMeanThroughput100000:
			meanthru = 11;
		break;

		case RPacketQoS::EMeanThroughput200000:
			meanthru = 12;
		break;
		case RPacketQoS::EMeanThroughput500000:
			meanthru = 13;
		break;

		case RPacketQoS::EMeanThroughput1000000:
			meanthru = 14;
		break;

		case RPacketQoS::EMeanThroughput2000000:
			meanthru = 15;
		break;

		case RPacketQoS::EMeanThroughput5000000:
			meanthru = 16;
		break;

		case RPacketQoS::EMeanThroughput10000000:
			meanthru = 17;
		break;

		case RPacketQoS::EMeanThroughput20000000:
			meanthru = 18;
		break;

		case RPacketQoS::EUnspecifiedMeanThroughput:
		case RPacketQoS::EMeanThroughput50000000:
			meanthru = 31;
		break;

		default:
			meanthru = 0; // unspecified or unknown mean throughput
		break;
		};

	iTxBuffer.Format(KCGQreq,iCid,			
			precedence,
			delay,
			reliability,
			peakthru,
			meanthru);
	}



//
//
// CATGprsGetProfile
//
//
//
//

CATGprsGetProfile* CATGprsGetProfile::NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 *  Standard 2 phase constructor.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsGetProfile* p=new(ELeave) CATGprsGetProfile(aCid, aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGprsGetProfile::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGprsGetProfile::CATGprsGetProfile(TInt aCid, CATIO* aIo, CTelObject *aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)	
	: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals), iCid(aCid)
/**
 * Constructor.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{}


CATGprsGetProfile::~CATGprsGetProfile()
/**
 * Destructor.
 */
	{}


void CATGprsGetProfile::Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig)
/**
 * This starts the sending of the get commands.
 * 
 * @param aTsyReqHandle handle to the client.
 * @param aConfig Pointer to a RPacketQoS::TQoSGPRSNegotiated
 */

	{
	TPckg<RPacketQoS::TQoSGPRSNegotiated>* qoSProfileV1Pckg = (TPckg<RPacketQoS::TQoSGPRSNegotiated>*)aConfig;
	iGprsNegProfile= &(*qoSProfileV1Pckg)();
	LOGTEXT(_L8("CATGprsGetProfile:\tCATConfigGPRS::Start function called in TSY"));

	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	iReqHandle = aTsyReqHandle;
	iState=EWaitForGetCGQMINComplete;
	Write(KGetCGQMINCommand, KGprsCommandTimeOut);
	}

void CATGprsGetProfile::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function cancels the outstanding read and sets the state to EWaitForDSR.
 *
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsGetProfile::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsGetProfile::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}

void CATGprsGetProfile::Complete(TInt aErr, TEventSource aSource)
/**
 * This Function completes the get or set command from the client.
 * @param aErr and error to relay to the client.
 */
	{
	LOGTEXT(_L8("CATGprsGetProfile::Complete"));
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this); 
	if(aErr == KErrNone)		
		{
		((CGprsQoS*)iTelObject)->SetNegQoSProfile(iGprsNegProfile);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aErr,aSource);		
	iTelObject->ReqCompleted(iReqHandle,aErr);
	
	iState=EATNotInProgress;
	}


void CATGprsGetProfile::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 *
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	Complete(aStatus, aSource);
	}



void CATGprsGetProfile::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively in 
 * get and set states and are described below.
 *	
 * @par  aSource Source of function call. 
 *
 *
 * @par		EWaitForGetCGQMINComplete,
 * Wait for response from the phone on the set command.
 *
 * @par		EWaitForGetCGQMINOK,
 * Validate phone response and send set AT+CGQREQ command.
 *
 * @par		EWaitForGetCGQREQComplete,
 * Wait for response from the phone on the set command.
 *
 * @par		EWaitForGetCGQREQOK *
 * Validate phone response and complete get command.
 *
 */
	{
	LOGTEXT2(_L8("CATGprsGetProfile::EventSignal with iState %d"),iState);
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGprsGetProfile:\tTimeout Error during Config"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	switch(iState)
		{
		case EWaitForGetCGQMINComplete:
				{
				StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
				iState = EWaitForGetCGQMINOK;
				}
		break;
		
		case EWaitForGetCGQMINOK:
				{
				__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
				TInt ret = ValidateExpectString();
				RemoveStdExpectStrings();
				if(ret)
					{
					Complete(ret, aSource);
					return;
					}
				TRAPD(err,ParseCGQMINResponseL());
				if (err != KErrNone)
					{
					LOGTEXT(_L8("ATGPRSCONFIG::\tError parsing +CGQMIN=?"));
					Complete(ret, aSource);
					return;
					}									
				Write(KGetCGQREQCommand, KGprsCommandTimeOut);
				iState = EWaitForGetCGQREQComplete;
				}
			break;
		
		case EWaitForGetCGQREQComplete:
				{
				StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
				iState = EWaitForGetCGQREQOK;
				}
			break;
		case EWaitForGetCGQREQOK:
				{
				__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
				TInt ret = ValidateExpectString();
				RemoveStdExpectStrings();
				if(ret)
					{
					Complete(ret, aSource);
					return;
					}
				TRAPD(err,ParseCGQREQResponseL());
				if (err != KErrNone)
					{
					LOGTEXT(_L8("ATGPRSCONFIG::\tError parsing +CGQREQ=?"));
					Complete(ret, aSource);
					return;
					}								
				iIo->RemoveExpectStrings(this);	
				Complete(ret, aSource);
				}
			break;		
		case EATNotInProgress:
			break;
		default:
				{
				LOGTEXT(_L8("CATGprsGetProfile::EventSignal, Default, panic"));
				Panic(EIllegalEvent);
				}
			break;
		}			
	}


void CATGprsGetProfile::ParseCGQMINResponseL()
/**
 * This Function parses the response from the get AT+CGQMIN command to the phone
 */
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	while(entry = iter++,entry!=NULL)
		{
		if (entry->iResultPtr.MatchF(KCGQMINResponseString)!=0)
			continue;
		entry = iter++;
		if(entry == NULL)
			User::Leave(KErrGeneral);
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
		User::Leave(KErrGeneral);
	// we are now pointing to the correct context, just parse the values
	// Get precedence
	TLex8 lex(entry->iResultPtr);
	TInt val;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iPrecedence = RPacketQoS::EUnspecifiedPrecedence;
			break;
		case 1:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityHighPrecedence;
			break;
		case 2:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityMediumPrecedence;
			break;
		case 3:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityLowPrecedence;
			break;
		default:
			iGprsNegProfile->iPrecedence = RPacketQoS::EUnspecifiedPrecedence;
		}
	// Get delay class
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iDelay = RPacketQoS::EUnspecifiedDelayClass;
			break;
		case 1:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass1;
			break;
		case 2:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass2;
			break;
		case 3:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass3;
			break;
		case 4:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass4;
			break;
		default:
			iGprsNegProfile->iDelay = RPacketQoS::EUnspecifiedDelayClass;
		}
	// Get reliability class
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));

	switch(val)
		{
		case 0:
			iGprsNegProfile->iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
			break;
		case 1:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass1;
			break;
		case 2:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass2;
			break;
		case 3:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass3;
			break;
		case 4:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass4;
			break;
		case 5:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass5;
			break;
		default:
			iGprsNegProfile->iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
		}

	
	// Get peak 
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
			break;
		case 1:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput1000;
			break;
		case 2:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput2000;
			break;
		case 3:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput4000;
			break;
		case 4:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput8000;
			break;
		case 5:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput16000;
			break;
		case 6:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput32000;
			break;
		case 7:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput64000;
			break;
		case 8:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput128000;
			break;
		case 9:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput256000;
			break;
		default:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
		}
	// Get mean
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));

	switch(val)
		{
		case 0:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
			break;
		case 2:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput100;
			break;
		case 3:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput200;
			break;
		case 4:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput500;
			break;
		case 5:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput1000;
			break;
		case 6:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput2000;
			break;
		case 7:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput5000;
			break;
		case 8:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput10000;
			break;
		case 9:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput20000;
			break;
		case 10:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput50000;
			break;
		case 11:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput100000;
			break;
		case 12:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput200000;
			break;
		case 13:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput500000;
			break;
		case 14:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput1000000;
			break;
		case 15:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput2000000;
			break;
		case 16:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput5000000;
			break;
		case 17:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput10000000;
			break;
		case 18:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput20000000;
			break;
		case 31:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput50000000;
			break;
		default:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
		}
	CleanupStack::PopAndDestroy();
	}


void CATGprsGetProfile::ParseCGQREQResponseL()
/**
 * This Function parses the response from the get CGQREQ command to the phone
 */
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	while(entry = iter++,entry!=NULL)
		{
		if (entry->iResultPtr.MatchF(KCGQREQResponseString)!=0)
			continue;
		entry = iter++;
		if(entry == NULL)
			User::Leave(KErrGeneral);
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
		User::Leave(KErrGeneral);
	// we are now pointing to the correct context, just parse the values
	// Get precedence
	TLex8 lex(entry->iResultPtr);
	TInt val;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iPrecedence = RPacketQoS::EUnspecifiedPrecedence;
			break;
		case 1:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityHighPrecedence;
			break;
		case 2:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityMediumPrecedence;
			break;
		case 3:
			iGprsNegProfile->iPrecedence = RPacketQoS::EPriorityLowPrecedence;
			break;
		default:
			iGprsNegProfile->iPrecedence = RPacketQoS::EUnspecifiedPrecedence;
		}

	// Get delay class
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iDelay = RPacketQoS::EUnspecifiedDelayClass;
			break;
		case 1:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass1;
			break;
		case 2:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass2;
			break;
		case 3:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass3;
			break;
		case 4:
			iGprsNegProfile->iDelay = RPacketQoS::EDelayClass4;
			break;
		default:
			iGprsNegProfile->iDelay = RPacketQoS::EUnspecifiedDelayClass;
		}


	// Get reliability class
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));

	switch(val)
		{
		case 0:
			iGprsNegProfile->iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
			break;
		case 1:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass1;
			break;
		case 2:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass2;
			break;
		case 3:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass3;
			break;
		case 4:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass4;
			break;
		case 5:
			iGprsNegProfile->iReliability = RPacketQoS::EReliabilityClass5;
			break;
		default:
			iGprsNegProfile->iReliability = RPacketQoS::EUnspecifiedReliabilityClass;
		}

	
	// Get peak 
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));
	switch(val)
		{
		case 0:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
			break;
		case 1:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput1000;
			break;
		case 2:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput2000;
			break;
		case 3:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput4000;
			break;
		case 4:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput8000;
			break;
		case 5:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput16000;
			break;
		case 6:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput32000;
			break;
		case 7:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput64000;
			break;
		case 8:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput128000;
			break;
		case 9:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EPeakThroughput256000;
			break;
		default:
			iGprsNegProfile->iPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
		}
	// Get mean
	entry = iter++;
	if(entry == NULL)
		User::Leave(KErrGeneral);
	lex = entry->iResultPtr;
	(void)User::LeaveIfError(lex.Val(val));

	switch(val)
		{
		case 0:
		case 1:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
			break;
//		case 1:
//			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
//			break;
//
//	Best Effort & Subscribed are now contained in the EUnspecifiedMeanThroughput 
//
		case 2:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput100;
			break;
		case 3:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput200;
			break;
		case 4:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput500;
			break;
		case 5:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput1000;
			break;
		case 6:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput2000;
			break;
		case 7:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput5000;
			break;
		case 8:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput10000;
			break;
		case 9:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput20000;
			break;
		case 10:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput50000;
			break;
		case 11:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput100000;
			break;
		case 12:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput200000;
			break;
		case 13:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput500000;
			break;
		case 14:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput1000000;
			break;
		case 15:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput2000000;
			break;
		case 16:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput5000000;
			break;
		case 17:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput10000000;
			break;
		case 18:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput20000000;
			break;
		case 31:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EMeanThroughput50000000;
			break;
		default:
			iGprsNegProfile->iMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
		}
	CleanupStack::PopAndDestroy();

	}


