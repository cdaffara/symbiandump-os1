// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CMainMenu.h"
#include <simtsy.h>

/**
Factory constructor.

@param  aConsole Console to which output is printed
@return          Instance of CMainMenu class
*/
CMainMenu* CMainMenu::NewLC(CConsoleBase& aConsole)
	{
	CMainMenu* self = new(ELeave) CMainMenu(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Constructor called by CMainMenu::NewLC().

@param Console to which output is printed
*/
CMainMenu::CMainMenu(CConsoleBase& aConsole)
	: CBaseMenuAsync(aConsole)
	{
	iCount = 0;
	iMessages = 0;
	iRecievedCalls = 0;
	iCallWaitingOn = EFalse;
	}

/**
Second phase constructor.
*/
void CMainMenu::ConstructL()
	{
	CBaseMenuAsync::ConstructL();
	iPhoneId = CPhoneId::NewL(this);

	// Create active objects using their factory constructors.
	iCallForwarding = CCallForwardingStatus::NewL(this);
	iCallBarring = CCallBarringStatus::NewL(this);
	iFlightModeInfo = CFlightModeInfo::NewL(this);
	iIdentityService = CIdentityServiceStatus::NewL(this);
	iCallWaiting = CCallWaitingStatus::NewL(this);

	iLineStatus = CLineStatus::NewL(this);
	iAnswerIncomingCall = CAnswerIncomingCall::NewL(this);
	iCallInfo = CCallInfo::NewL(this);
	iDummyAnswer = CDummyAnswer::NewL(this);
	iHangup = CHangup::NewL(this);

	iCallStatus = NULL;

	}

/**
Destructor.
Deletes owned objects.
*/
CMainMenu::~CMainMenu()
	{
	delete iTelephony;
	delete iPhoneId;

	delete iFlightModeInfo;
	delete iCallForwarding;
	delete iCallBarring;
	delete iIdentityService;
	delete iCallWaiting;

	delete iAnswerIncomingCall;
	delete iHangup;
	delete iCallStatus;
	delete iCallInfo;
	delete iDummyAnswer;
	delete iLineStatus;
	}

/**
Provides functionality for member objects to notify the Menu object (their 
owner) that they have completed their request.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecComplete(TTelISVExampleType aDerivedType)
	{
   if (iState==ESetNotifier)
		{
		switch(aDerivedType)
			{
		case KLineStatus:
			iLastOperation = iLineStatus;
			break;
		case KCallInfo:
			iLastOperation = iCallInfo;
			break;
		case KCallStatus:
			iLastOperation = iCallStatus;
			break;
		case KAnswerIncomingCall:
			iLastOperation = iAnswerIncomingCall;
			break;
		case KHangup:
			iLastOperation = iHangup;
			break;
		case KCallForwardingStatus:
			iLastOperation = iCallForwarding;
			break;
		case KCallBarringStatus:
			iLastOperation = iCallBarring;
			break;
		case KFlightModeInfo:
			iLastOperation = iFlightModeInfo;
			break;
		case KIdentityServiceStatus:
			iLastOperation = iIdentityService;
			break;
		case KCallWaitingStatus:
			iLastOperation = iCallWaiting;
			break;
			}
			
		// Request to be told about particular events occurring.		
		TRAPD(errNotify, iLastOperation->RequestNotificationL());
		if (errNotify != KErrNone)
			{
			iConsole->Printf(_L("Notification Request for TTelISVExampleType"));
			iConsole->Printf(_L("%d left with error code "), aDerivedType);
			iConsole->Printf(_L("%d\n"), errNotify);
			return;
			};
			
		// Check the type of iLastOperation to see what it has been cast to.
		switch(iLastOperation->GetExampleType())
			{
		case KLineStatus:
			iConsole->ClearScreen();
			iConsole->Printf(KMenuMsg);
			CallsCount();
			iConsole->Printf(KAnyMsg);
			GetInput();
			break;
		case KAnswerIncomingCall:
			iCallId = reinterpret_cast<CAnswerIncomingCall*>
			          (iAnswerIncomingCall)->iCallId;
			TRAPD(err, 	iCallStatus = CCallStatus::NewL(this, iCallId));
			if (err != KErrNone)
				{
				iConsole->Printf(_L("CallStatus construction left "));
				iConsole->Printf(_L("with error code %d\n"), err);
				return;
				}
			iState = EGetCallStatus;
			iLastOperation = iCallStatus;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallInfo:
			iState = EAnswerIncomingCall;
			iLastOperation = iAnswerIncomingCall;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallStatus:
			iState = EDummyAnswer;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KFlightModeInfo:
			if (IsActive())
				{
				Cancel();
				}
			iState = ECallBarring;
			iLastOperation = iCallBarring;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallBarringStatus:
			iState = ECallForwarding;
			iLastOperation = iCallForwarding;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallForwardingStatus:
			iState = EIdentityService;
			iLastOperation = iIdentityService;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KIdentityServiceStatus:
			iState = ECallWaiting;
			iLastOperation = iCallWaiting;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallWaitingStatus:
			iCallWaitingOn = EFalse;
			iState = EGetLineStatus;
			iLastOperation = iLineStatus;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		default:

			break;
			}
		}
	else if(aDerivedType == KPhoneId)
		{
		iLastOperation = iFlightModeInfo;
		SetActive();
		CompleteOwnRequest(KErrNone);
		}
	else if (aDerivedType == KNotType)
		{
		iConsole->ClearScreen();
		iConsole->Printf(KMenuMsg);
		CallsCount();
		iConsole->Printf(KHangupMsg);
		GetInput();
		}
	}

/**
Provides functionality for member objects to notify the Menu object (their
owner) that they have been notified of a change.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecNotify(TTelISVExampleType aDerivedType)
	{
	switch(aDerivedType)
		{
	case KAnswerIncomingCall:
		iCallId = reinterpret_cast<CAnswerIncomingCall*>
		          (iAnswerIncomingCall)->iCallId;
		TRAPD(err, iCallStatus = CCallStatus::NewL(this, iCallId));
		if (err != KErrNone)
			{
			iConsole->Printf(_L("CallStatus construction left "));
			iConsole->Printf(_L("with error code %d\n"), err);
			return;
			}
		iState = EGetCallStatus;
		iLastOperation = iCallStatus;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KCallWaitingStatus:
		iCallWaitingOn = ETrue;
		iState = EGetLineStatus;
		iLastOperation = iLineStatus;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KLineStatus:
		if (iCallStatus != NULL)
			{
			iConsole->Printf(_L("Call Waiting!\n"));
			iRecievedCalls++;
			iConsole->ClearScreen();
			iConsole->Printf(KMenuMsg);
			CallsCount();
			iConsole->Printf(KHangupMsg);
			GetInput();
			}
		else
			{
			// Request to be told about particular events occuring 
			TRAPD(err, iLineStatus->RequestNotificationL());
			if (err != KErrNone)
				{
				iConsole->Printf(_L("Request left with error code "));
				iConsole->Printf(_L("%d\n"),err);
				return;
				}
			iRecievedCalls++;
			iState = EGetCallInfo;
			iLastOperation = iCallInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			}
		break;
	case KCallStatus:
		if (iFlightModeInfo->IsActive())
			{
			iFlightModeInfo->Cancel();
			}
		delete iCallStatus;
		iCallStatus = NULL;
 	  	iLineStatus->Cancel();
		iState = EStart;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KCallForwardingStatus:
		iState = ECallForwarding;
		iLastOperation = iCallForwarding;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
		}
	}

/**
Responds appropriately depending on what key the user has pressed.
*/
void CMainMenu::RunL()
	{
	switch(iState)
		{
	case EStart:
		iState = EGetFlightModeInfo;
		if (iCount)
			{
			iCount = 0;
			iLastOperation = iFlightModeInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			}
		else
			{
			iLastOperation = iPhoneId;
			TRAPD(errPhone, iLastOperation->StartRequestL());
			if (errPhone != KErrNone)
				{
				iConsole->Printf(_L("Request left with error code "));
				iConsole->Printf(_L("%d\n"), errPhone);
				return;
				};
			}
		break;
	case EEnd:
		iFlightModeInfo->Cancel();
		CActiveScheduler::Stop();
		break;
	case ECallWaiting:
	case EGetFlightModeInfo:
	case ECallForwarding:
	case ECallBarring:
	case EIdentityService:
	case EGetLineStatus:
	case EGetCallInfo:
	case EGetCallStatus:
	case EAnswerIncomingCall:
		iState = ESetNotifier;
		TRAPD(errAnswer, iLastOperation->StartRequestL());
		if (errAnswer != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errAnswer);
			return;
			};
		break;
	case EHangup:
		iDummyAnswer->Cancel();
		TRAPD(errArg, iLastOperation->StartRequestL(iCallId));
		if (errArg != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errArg);
			return;
			}
		break;
	case EDummyAnswer:
		iMessages++;
		iState = EHangup;
		TRAPD(errDummy, iDummyAnswer->StartCount(1000000));
		if (errDummy != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errDummy);
			return;
			};
		break;
	case EWaitingForKeyPress:
		{
		TInt c = iConsole->KeyCode();
		switch(c)
			{
		case 'E':
		case 'e':
		case EKeyEscape:
			// Cancel notifications
			if (iAnswerIncomingCall->IsActive())
				{
				iAnswerIncomingCall->Cancel();
				}
			if (iHangup->IsActive())
				{
				iHangup->Cancel();
				}
			if (iCallStatus != NULL && iCallStatus->IsActive())
				{
				iCallStatus->Cancel();
				}
			iFlightModeInfo->Cancel();
			iLineStatus->Cancel();
			CActiveScheduler::Stop();
			break;
		case 'h':
		case 'H':
			if (iDummyAnswer->IsActive())
				{
				iState = EHangup;
				iLastOperation = iHangup;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else
				{
				GetInput();
				break;
				}
		case 'i':
		case 'I':
			{
			CTelephony::TPhoneLine line = CTelephony::EVoiceLine;
			CTelephony::TCallStatusV1 iLineStatusV1;
  			CTelephony::TCallStatusV1Pckg iLineStatusV1Pckg(iLineStatusV1);
  			iTelephony->GetLineStatus(line, iLineStatusV1Pckg);
			CTelephony::TCallStatus voiceLineStatus = iLineStatusV1.iStatus;
			if (iCount >= 1
				&& iCallWaitingOn
				&& voiceLineStatus != CTelephony::EStatusRinging)
				{
				iCount++;
				TInt ret = RProperty::Set(KUidPSSimTsyCategory,
										  KPSSimTsyIncomingVoiceCall,
										  0);
				break;
				}
			else
				{
				GetInput();
				break;
				}
			}
		default:
		// Pressed a non suggested key to simulate an incoming call
			if (iCount == 0)
				{
				// For SIM TSY to simulate an incoming call
				iCount = 1;
		   		TInt ret = RProperty::Set(KUidPSSimTsyCategory,
		   								  KPSSimTsyIncomingVoiceCall,
		   								  0);
		   		}
		   	else
		   		{
					GetInput();
			   	}
				break;
			}
		}
		break;
	default:
		break;
		} // switch(iState)
	}

/**
Calculates the number of people who did not leave a message and displays result
to the console. Result = Number of received calls - Number of messages received
*/
void CMainMenu::CallsCount()
	{
	iConsole->Printf(_L("You have %d Messages\n"), iMessages);
	TInt difference = iRecievedCalls - iMessages;
	if (difference > 0)
		{
		iConsole->Printf(_L("%d people called and left no message\n"),
		                 difference);
		}
	}

/**
Cancels outstanding asynchronous request.
*/
void CMainMenu::DoCancel()
	{
	if(iState == EStart)
		{
		CompleteOwnRequest(KErrCancel);
		}
	else
		{
		iConsole->ReadCancel();
		}
	}
