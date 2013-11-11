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

/**
Factory constructor.

@param  aConsole Console to which output is printed.
@return          Instance of CMainMenu class.
*/
CMainMenu* CMainMenu::NewLC(CConsoleBase& aConsole)
	{
	CMainMenu* self = new(ELeave) CMainMenu(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Constructor called by CMainMenu::NewLC(). Sets iSingleString to ETrue 
by default.

@param Console to which output is printed
*/
CMainMenu::CMainMenu(CConsoleBase& aConsole)
	: CBaseMenuAsync(aConsole)
	, iCurrentTone(_L(""))
	, iCurrentToneLog(_L(""))
	, iTheNumber(KTheNumber)
	{
	iSingleString = ETrue;
	}

/**
Second phase constructor.
*/
void CMainMenu::ConstructL()
	{
	CBaseMenuAsync::ConstructL();
	iPhoneId = CPhoneId::NewL(this);

	iFlightModeInfo = CFlightModeInfo::NewL(this);
	iNetworkRegInfo = CNetworkRegInfo::NewL(this);

	iDialCall = CDialCall::NewL(this);
	iSendDTMF = CSendDTMF::NewL(this);
	iHangup = CHangup::NewL(this);
	iLineStatus = CLineStatus::NewL(this);
	}

/**
Destructor.  Deletes the owned objects.
*/
CMainMenu::~CMainMenu()
	{
	delete iTelephony;
	delete iPhoneId;

	delete iFlightModeInfo;
	delete iNetworkRegInfo;

	delete iDialCall;
	delete iHangup;
	delete iSendDTMF;
	delete iCallStatus;
	delete iLineStatus;
	}

/**
Provides functionality for member objects to notify the Menu object
(their owner) that they have completed their request.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecComplete(TTelISVExampleType aDerivedType)
	{
	if	(iState==ESetNotifier || aDerivedType == KDialCall)
		{
		switch(aDerivedType)
			{
		case KSendDTMF:
			iLastOperation = iSendDTMF;
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
		case KHangup:
			iLastOperation = iHangup;
			break;
		case KCallStatus:
			iLastOperation = iCallStatus;
			break;
			}
		TRAPD(errNotify, iLastOperation->RequestNotificationL());
		if (errNotify != KErrNone)
			{
			iConsole->Printf(_L("Notification Request for TTelISVExampleType"));
			iConsole->Printf(_L("%d left with error code "), aDerivedType);
			iConsole->Printf(_L("%d\n"), errNotify);
			return;
			}
			
		// Check the type of iLastOperation to see
		// what it has been cast to.
		switch(iLastOperation->GetExampleType())
			{
		case KFlightModeInfo:
			iState = EGetNetworkRegStatus;
			iLastOperation = iNetworkRegInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KDialCall:
			iCallId = reinterpret_cast<CDialCall*>
			          (iDialCall)->iCallId;
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
		case KCallStatus:
			iConsole->ClearScreen();
			iConsole->Printf(KMenuMsg);
			iConsole->Printf(KDTMFQuestion);
			iConsole->Printf(KiSingleString);
			iConsole->Printf(KAsChar);
			GetInput();
			break;
		case KSendDTMF:
			iConsole->ClearScreen();
			iConsole->Printf(KMenuMsg);
			iConsole->Printf(KHangupMsg);
			GetInput();
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
Provides functionality for member objects to notify the Menu object
(their owner) that they have been notified of a change.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecNotify(TTelISVExampleType aDerivedType)
	{
	switch(aDerivedType)
		{
	case KNetworkRegInfo:
		TRAPD(err, iNetworkRegInfo->RequestNotificationL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"),err);
			return;
			}
		iState = EGetLineStatus;
		iLastOperation = iLineStatus;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KLineStatus:
		// Waiting for dial request
		TRAP(err, iLineStatus->RequestNotificationL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"),err);
			return;
			}
		iConsole->ClearScreen();
		iConsole->Printf(KMenuMsg);
		iConsole->Printf(KDialMsg);
		GetInput();
		break;
	case KCallStatus:
		delete iCallStatus;
		iCallStatus = NULL;
		iState = EGetLineStatus;
		iLastOperation = iLineStatus;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case KSendDTMF:
		iConsole->ClearScreen();
		iConsole->Printf(KMenuMsg);
		iConsole->Printf(KAsChar);
		GetInput();
		break;
	default:
		break;
		}
	}

/**
Handles user's key presses.
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
			}
		break;
	case EEnd:
		CActiveScheduler::Stop();
		break;
	case EGetLineStatus:
	case EGetCallStatus:
	case EGetFlightModeInfo:
	case EGetNetworkRegStatus:
		iState = ESetNotifier;
		TRAPD(err, iLastOperation->StartRequestL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), err);
			return;
			}
		break;
	case EHangup:
		iState = ESetNotifier;
		TRAPD(errArg, iLastOperation->StartRequestL(iCallId));
		if (errArg != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errArg);
			return;
			}
		break;
	case EDialCall:
		iState = ESetNotifier;
		TRAPD(errArg3, iLastOperation->StartRequestL(KTheNumber));
		if (errArg3 != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errArg3);
			return;
			}
		break;
	case ESendDTMF:
		iState = ESetNotifier;
		if (iSingleString)
			{
			TRAPD(errArg2, iLastOperation->StartRequestL(iTheNumber))
			if (errArg2 != KErrNone)
				{
				iConsole->Printf(_L("Request left with error code "));
				iConsole->Printf(_L("%d\n"), errArg2);
				return;
				}
			}
		else
			{
			TRAPD(errArg2, iLastOperation->StartRequestL(iCurrentTone))
			if (errArg2 != KErrNone)
				{
				iConsole->Printf(_L("Request left with error code "));
				iConsole->Printf(_L("%d\n"), errArg2);
				return;
				}
			}
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
			iNetworkRegInfo->Cancel();
			iFlightModeInfo->Cancel();
			if (iCallStatus != NULL)
				{
				iCallStatus->Cancel();
				}
			if (iLineStatus->IsActive())
				{
				iLineStatus->Cancel();
				}
			CActiveScheduler::Stop();
			break;
		case 'D':
		case 'd':
			if (iCallStatus == NULL)
				{
				iState = EDialCall;
				iLastOperation = iDialCall;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else
				{
				// Print correct menu
				GetInput();
				break;
				}
		case 'H':
		case 'h':
			if (iCallStatus != NULL)
				{
				iSingleString = ETrue;
				iState = EHangup;
				iLastOperation = iHangup;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else
				{
				// Print correct menu
				GetInput();
				break;
				}
		case 'S':
		case 's':
			if (iCallStatus != NULL && iSingleString)
				{
				iConsole->Printf(_L("Be Right Back\n"));
				iSingleString = ETrue;
				iCurrentToneLog.Append(iTheNumber);
				iCurrentToneLog.Append('#');
				iState = ESendDTMF;
				iLastOperation = iSendDTMF;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else
				{
				// Print correct menu
				GetInput();
				break;
				}
		// Send number
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '*':
			if (iCallStatus != NULL)
				{
				iSingleString = EFalse;
				iCurrentTone.Delete(0,1);
				iCurrentTone.Append(c);
				iCurrentToneLog.Append(iCurrentTone);
				iState = ESendDTMF;
				iLastOperation = iSendDTMF;
				SetActive();
				CompleteOwnRequest(KErrNone);
				break;
				}
			else
				{
				// Print correct menu
				GetInput();
				break;
				}

		case '#':
			if (iCallStatus != NULL && !iSingleString)
				{
				iConsole->Printf(_L("You sent "));
				iConsole->Printf(iCurrentToneLog);
				iConsole->Printf(KNewLine);
				iCurrentToneLog.Delete(0, iCurrentToneLog.Length());
				iConsole->Printf(KMenuMsg);
				iConsole->Printf(KHangupMsg);
				// Key presses finished!
				// Call is active, DTMF has been indicated to be
				// finished as # character has been received.
				}
			else
				{
				// Print correct menu
				GetInput();
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
Cancels outstanding asynchonous request.
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
