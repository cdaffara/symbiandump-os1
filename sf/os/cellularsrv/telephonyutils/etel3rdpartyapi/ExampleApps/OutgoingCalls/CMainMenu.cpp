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
Constructor called by the factory constructor CMainMenu::NewLC().

@param Console to which output is printed
*/
CMainMenu::CMainMenu(CConsoleBase& aConsole)
	: CBaseMenuAsync(aConsole)
	{
	iCallWaitingOn = EFalse;
	iRinging = EFalse;
	iThenAnswer = EFalse;
	}

/**
Second phase constructor.
*/
void CMainMenu::ConstructL()
	{
	CBaseMenuAsync::ConstructL();
	iPhoneId = CPhoneId::NewL(this);
	iNetworkName = CCurrentNetworkName::NewL(this);
	iOperatorName = COperatorName::NewL(this);
  	iNetworkInfo = CCurrentNetworkInfo::NewL(this);

	iFlightModeInfo = CFlightModeInfo::NewL(this);
	iNetworkRegInfo = CNetworkRegInfo::NewL(this);
	iDialCall = CDialCall::NewL(this);
	iLineStatus = CLineStatus::NewL(this);

	iHangup = CHangup::NewL(this);
	iHold = CHold::NewL(this);
	iResume = CResume::NewL(this);

	iSwap = CSwap::NewL(this);

	iGetLockInfo = CGetLockInfo::NewL(this);
	iCallWaiting = CCallWaitingStatus::NewL(this);
	iAnswerIncomingCall = CAnswerIncomingCall::NewL(this);
	}

/**
Destructor.
*/
CMainMenu::~CMainMenu()
	{
	delete iTelephony;
	delete iPhoneId;

	delete iNetworkName;
	delete iNetworkInfo;
	delete iOperatorName;

	delete iDialCall;
	delete iHangup;
	delete iHold;
	delete iSwap;
	delete iResume;

	delete iFlightModeInfo;
	delete iNetworkRegInfo;

	delete iGetLockInfo;
	delete iCallWaiting;
	delete iAnswerIncomingCall;

	delete iLineStatus;
	delete iCallStatus;
	delete iCallStatus2;
	}

/**
Provides functionality for member objects to notify the Menu object (their 
owner) that they have completed their request.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecComplete(TTelISVExampleType aDerivedType)
	{
	if (IsActive())
		{
		Cancel();
		}
   if (	iState==ESetNotifier
			||  aDerivedType == KLineStatus
			||  aDerivedType == KHold)
		{
		switch(aDerivedType)
			{
		case KLineStatus:
			iLastOperation = iLineStatus;
			break;
		case KFlightModeInfo:
			iLastOperation = iFlightModeInfo;
			break;
		case KNetworkRegInfo:
			iLastOperation = iNetworkRegInfo;
			break;
		case KDialCall:
			iLastOperation = iDialCall;
			break;
		case KHold:
			iLastOperation = iHold;
			break;
		case KResume:
			iLastOperation = iResume;
			break;
		case KSwap:
			iLastOperation = iSwap;
			break;
		case KHangup:
			iLastOperation = iHangup;
			break;
		case KNetworkInfo:
			iLastOperation = iNetworkInfo;
			break;
		case KNetworkName:
			iLastOperation = iNetworkName;
			break;
		case KOperatorName:
			iLastOperation = iOperatorName;
			break;
		case KLockInfo:
			iLastOperation = iGetLockInfo;
			break;
		case KCallWaitingStatus:
			iLastOperation = iCallWaiting;
			break;
		case KAnswerIncomingCall:
			iLastOperation = iAnswerIncomingCall;
			break;
			}
		TRAPD(errNotify, iLastOperation->RequestNotificationL());
		if (errNotify != KErrNone)
			{
			iConsole->Printf(_L("Notification Request for TTelISVExampleType"));
			iConsole->Printf(_L("%d left with error code "), aDerivedType);
			iConsole->Printf(_L("%d\n"), errNotify);
			return;
			};
		// Check the type of iLastOperation to see what it has been cast to
		switch(iLastOperation->GetExampleType())
			{
		case KFlightModeInfo:
			iState = EGetNetworkRegStatus;
			iLastOperation = iNetworkRegInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KLineStatus:
			iRinging = ETrue;
			PrintCorrectMenu(GetCurrentDynamicCaps());
			GetInput();
			break;
		case KAnswerIncomingCall:
			if (iCallStatus == NULL)
				{
				iCallId = reinterpret_cast<CAnswerIncomingCall*>
						  (iAnswerIncomingCall)->iCallId;
				TRAPD(err, 	iCallStatus = CCallStatus::NewL(this, iCallId));
				if (err != KErrNone)
					{
					iConsole->Printf(_L("CallStatus1 construction left "));
					iConsole->Printf(_L("with error code %d\n"), err);
					return;
					}
				iLastCallStatus = iCallStatus;
				}
			else
				{
				iCallId2 = reinterpret_cast<CAnswerIncomingCall*>
				           (iAnswerIncomingCall)->iCallId;
				TRAPD(err, 	iCallStatus2 = CCallStatus::NewL(this, iCallId2));
				if (err != KErrNone)
					{
					iConsole->Printf(_L("CallStatus2 construction left "));
					iConsole->Printf(_L("with error code %d\n"), err);
					return;
					}
				iLastCallStatus =  iCallStatus2;
				}
			iState = EGetCallStatus;
			iLastOperation = iLastCallStatus;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KDialCall:
			if (iCallStatus == NULL)
				{
				iCallId = reinterpret_cast<CDialCall*>
				          (iDialCall)->iCallId;
				TRAPD(err, 	iCallStatus = CCallStatus::NewL(this, iCallId));
				if (err != KErrNone)
					{
					iConsole->Printf(_L("CallStatus1 construction left "));
					iConsole->Printf(_L("with error code %d\n"), err);
					return;
					}

				iLastCallStatus = iCallStatus;
				}
			else
				{
				iCallId2 = reinterpret_cast<CDialCall*>
				           (iDialCall)->iCallId;
				TRAPD(err, 	iCallStatus2 = CCallStatus::NewL(this, iCallId2));
				if (err != KErrNone)
					{
					iConsole->Printf(_L("CallStatus2 construction left "));
					iConsole->Printf(_L("with error code %d\n"), err);
					return;
					}
				iLastCallStatus =  iCallStatus2;
				}
			iState = EGetCallStatus;
			iLastOperation = iLastCallStatus;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallStatus:
		case KResume:
			PrintCorrectMenu(GetCurrentDynamicCaps());
			GetInput();
			break;
		case KSwap:
			if (iLastCallStatus == iCallStatus)
				{
				iLastCallStatus =  iCallStatus2;
				}
			else
				{
				iLastCallStatus =  iCallStatus;
				}
			PrintCorrectMenu(GetCurrentDynamicCaps());
			GetInput();
			break;
		case KHangup:
			// Do nothing; all operations on hangup event are handled by the
			// call status notification. In this way remote party hangups can be
			// detected.
			break;
		case KHold:
			if (iThenAnswer)
				{
				iThenAnswer = EFalse;
				iState = EAnswerIncomingCall;
				iLastOperation = iAnswerIncomingCall;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				PrintCorrectMenu(GetCurrentDynamicCaps());
				GetInput();
				}
			break;
		case KNetworkInfo:
			iState = EGetNetworkName;
			iLastOperation = iNetworkName;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KNetworkName:
			iState = EGetOperatorName;
			iLastOperation = iOperatorName;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KOperatorName:
			iState = ECallWaiting;
			iLastOperation = iCallWaiting;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KLockInfo:
			iState = EGetLineStatus;
			iLastOperation = iLineStatus;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KCallWaitingStatus:
			iCallWaitingOn = ETrue;
			iState = EGetLockInfo;
			iLastOperation = iGetLockInfo;
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
	}

/**
Provides functionality for member objects to notify the Menu object (their 
owner) that they have been notified of a change.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecNotify(TTelISVExampleType aDerivedType)
	{
	if (IsActive())
		{
		Cancel();
		}
	switch(aDerivedType)
		{
	case KNetworkRegInfo:
		{
		TRAPD(err, iNetworkRegInfo->RequestNotificationL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("%d\n"),err);
			return;
			}
		iState = EGetNetworkInfo;
		iLastOperation = iNetworkInfo;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
		}
	case KLineStatus:
		{
		TRAPD(err, iLineStatus->RequestNotificationL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"),err);
			return;
			}		PrintCorrectMenu(GetCurrentDynamicCaps());
		GetInput();
		break;
		}
	case KCallStatus:
		if (iLastCallStatus == iCallStatus)
			{
			// Delete the previous object call status object which has now been
			// hung up.
			delete iLastCallStatus;
			iCallStatus = NULL;
			iLastCallStatus = iCallStatus2;
			}
		else
			{
			delete iLastCallStatus;
			iCallStatus2 = NULL;
			iLastCallStatus = iCallStatus;
			}
		if (iLineStatus->IsActive())
			{
			iLineStatus->Cancel();
			}
		iState = EGetLineStatus;
		iLastOperation = iLineStatus;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KCallWaitingStatus:
		iCallWaitingOn = EFalse;
		iState = EGetLockInfo;
		iLastOperation = iGetLockInfo;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
		}
	}

/**
Handles key presses.
*/
void CMainMenu::RunL()
	{
	switch(iState)
		{
	case EStart:
		iState = EGetFlightModeInfo;
		iLastOperation = iPhoneId;
		TRAPD(errPhone, iLastOperation->StartRequestL());
		if (errPhone != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errPhone);
			return;
			};
		break;
	case EEnd:
		if (iLineStatus->IsActive())
			{
			iLineStatus->Cancel();
			}
		if (iNetworkRegInfo->IsActive())
			{
			iNetworkRegInfo->Cancel();
			}
		if (iNetworkInfo->IsActive())
			{
			iNetworkInfo->Cancel();
			}
		if (iFlightModeInfo->IsActive())
			{
			iFlightModeInfo->Cancel();
			}
		CActiveScheduler::Stop();
		break;
	case EAnswerIncomingCall:
	case ECallWaiting:
	case EGetLockInfo:
	case EGetNetworkInfo:
	case EGetNetworkName:
	case EGetOperatorName:
	case EGetFlightModeInfo:
	case EGetNetworkRegStatus:
	case EGetLineStatus:
	case EGetCallStatus:
		iState = ESetNotifier;
		TRAPD(err, iLastOperation->StartRequestL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), err);
			return;
			}
		break;
	case EHold:
	case EResume:
	case EHangup:
		CTelephony::TCallId temp;
		if (iLastCallStatus == iCallStatus)
			{
			temp = iCallId;
			}
		else
			{
			temp = iCallId2;
			}
		iState = ESetNotifier;
		TRAPD(errArg, iLastOperation->StartRequestL(temp));
		if (errArg != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errArg);
			return;
			}
		break;
	case ESwap:
		iState = ESetNotifier;
		TRAPD(errSwap, iLastOperation->StartRequestL(iCallId, iCallId2));
		if (errSwap != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errSwap);
			return;
			}
		break;
	case EDialCall:
		iState = ESetNotifier;
		TRAPD(errNum, iLastOperation->StartRequestL(KTheNumber));
		if (errNum != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errNum);
			return;
			}
		break;
	case EWaitingForKeyPress:
		{
		CTelephony::TCallCapsV1 CallCapsV1 = GetCurrentDynamicCaps();
		TUint32 crtlCaps = CallCapsV1.iControlCaps;
		TInt c = iConsole->KeyCode();
		switch(c)
			{
		// Exit
		case 'E':
		case 'e':
		// cancel notifications
			if (iCallStatus != NULL)
				{
				iCallStatus->Cancel();
				}
			if (iCallStatus2 != NULL)
				{
				iCallStatus2->Cancel();
				}
			iLineStatus->Cancel();
			iFlightModeInfo->Cancel();
			iNetworkRegInfo->Cancel();
			iNetworkInfo->Cancel();
			iGetLockInfo->Cancel();
			CActiveScheduler::Stop();
			break;
		// Hangup active call
		case EKeyEscape:
			if (iLastCallStatus != NULL)
				{
				iState = EHangup;
				iLastOperation = iHangup;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Swap active and on hold calls
		case 'S':
		case 's':
			if (crtlCaps & CTelephony::KCapsSwap
			&&  iCallStatus != NULL
			&&  iCallStatus2 !=NULL)
				{
				iState = ESwap;
				iLastOperation = iSwap;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Make another call
		case 'D':
		case 'd':
			if (iLastCallStatus == NULL
			||  crtlCaps & CTelephony::KCapsResume
			&& !iRinging)
				{
				iConsole->ClearScreen();
				iConsole->Printf(_L("Dialling Call\n"));
				iState = EDialCall;
				iLastOperation = iDialCall;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Put call on hold
		case 'H':
		case 'h':
			if (crtlCaps & CTelephony::KCapsHold)
				{
				iState = EHold;
				iLastOperation = iHold;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Resume on hold call
		case 'R':
		case 'r':
			if (crtlCaps & CTelephony::KCapsResume
			&&  iLastCallStatus != NULL )
				{
				iState = EResume;
				iLastOperation = iResume;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Answer incoming call
		case 'a':
		case 'A':
			if (iCallWaiting && iLastCallStatus != NULL
			&&  crtlCaps & CTelephony::KCapsResume)
				{
				iRinging = EFalse;
				iState = EAnswerIncomingCall;
				iLastOperation = iAnswerIncomingCall;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else if(iCallWaiting
			     && iLastCallStatus != NULL
			     && crtlCaps & CTelephony::KCapsHold)
				{
				iRinging = EFalse;
				iThenAnswer = ETrue;
				iState = EHold;
				iLastOperation = iHold;
				SetActive();
				CompleteOwnRequest(KErrNone);
				}
			else
				{
				GetInput();
				}
			break;
		// Simulate incoming call
		case 'i':
		case 'I':
			if(iCallWaiting
			&& iLastCallStatus != NULL
			&& (crtlCaps & CTelephony::KCapsResume ||
			    crtlCaps & CTelephony::KCapsHold))
				{
				TInt ret = RProperty::Set(KUidPSSimTsyCategory,
										  KPSSimTsyIncomingVoiceCall,
										  0);
				break;
				}
		default:
			GetInput();
			}
		}
		break;
	default:
		break;
		}
	}

/**
Gets dynamic call capabilities.
*/
CTelephony::TCallCapsV1 CMainMenu::GetCurrentDynamicCaps()
	{
	CTelephony::TCallId temp;
	CTelephony::TCallCapsV1 callCapsV1;
	CTelephony::TCallCapsV1Pckg callCapsV1Pckg(callCapsV1);
	if (iLastCallStatus == iCallStatus)
		{
		temp = iCallId;
		}
	else
		{
		temp = iCallId2;
		}
	// Retrieves the dynamic call capabilities for calls you dialled or answered with CTelephony
	iTelephony->GetCallDynamicCaps(temp, callCapsV1Pckg);
	return callCapsV1;
	}

/**
Prints options menu to the console.

@param aCallCapsV1 Stores the call capabilities
*/
void CMainMenu::PrintCorrectMenu(CTelephony::TCallCapsV1 aCallCapsV1)
	{
	iConsole->ClearScreen();
	iConsole->Printf(KMenuMsg);
	if (iRinging)
		{
		iConsole->Printf(_L("There is an incoming call\n"));
		iConsole->Printf(_L("How would you like to proceed?\n"));
		if (aCallCapsV1.iControlCaps & CTelephony::KCapsHold)
			{
			iConsole->Printf(KHoldMsg);
			iConsole->Printf(KAnswerMsg);
			iConsole->Printf(KHangupMsg);
			}
		else
			{
			iConsole->Printf(KResumeMsg);
			iConsole->Printf(KAnswerMsg);
			iConsole->Printf(KHangupMsg);
			}
		}
	else
		{
		if (iLastCallStatus == NULL)
			{
			iConsole->Printf(KDialMsg);
			}
		else if (aCallCapsV1.iControlCaps & CTelephony::KCapsSwap
		     &&  iCallStatus != NULL
		     &&  iCallStatus2 != NULL)
			{
			iConsole->Printf(KSwapMsg);
			iConsole->Printf(KHangupMsg);
			}
		else if (aCallCapsV1.iControlCaps & CTelephony::KCapsHold)
			{
			iConsole->Printf(KHoldMsg);
			iConsole->Printf(KIncomingMsg);
			iConsole->Printf(KHangupMsg);
			}
		else if (aCallCapsV1.iControlCaps & CTelephony::KCapsResume)
			{
			iConsole->Printf(KResumeMsg);
			iConsole->Printf(KDialMsg);
			iConsole->Printf(KIncomingMsg);
			iConsole->Printf(KHangupMsg);
			}
		}
	}

/**
Waits for console input.
*/
void CMainMenu::GetInput()
	{
	iConsole->Read(iStatus);
	iState = EWaitingForKeyPress;
	SetActive();
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
