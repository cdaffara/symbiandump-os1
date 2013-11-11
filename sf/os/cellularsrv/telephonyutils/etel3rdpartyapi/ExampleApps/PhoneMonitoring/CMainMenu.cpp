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
Constructor, called by the CMainMenu::NewLC() factory constructor.

@param aConsole Console to which output is printed.
*/
CMainMenu::CMainMenu(CConsoleBase& aConsole)
	: CBaseMenuAsync(aConsole)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CMainMenu::ConstructL()
	{
	CBaseMenuAsync::ConstructL();
	iPhoneId = CPhoneId::NewL(this);

	iBatteryInfo = CBatteryInfo::NewL(this);
	iSignalInfo = CSignalInfo::NewL(this);

	iFlightModeInfo = CFlightModeInfo::NewL(this);
	iGetIndicator = CGetIndicator::NewL(this);
	}

/**
Destructor.
Deletes owned objects.
*/
CMainMenu::~CMainMenu()
	{
	delete iTelephony;
	delete iPhoneId;

	delete	iBatteryInfo;
	delete iSignalInfo;

	delete iFlightModeInfo;
	delete iGetIndicator;
	}

/**
Provides functionality for member objects to notify the Menu object (their
owner) that they have completed their request.

@param aDerivedType Type of class derived from CISVAPIBase
*/
void CMainMenu::ExecComplete(TTelISVExampleType aDerivedType)
	{
	if(iState==ESetNotifier || aDerivedType == KFlightModeInfo)
		{
		switch(aDerivedType)
			{
		case KBatteryInfo:
			iLastOperation = iBatteryInfo;
			break;
		case KSignalInfo:
			iLastOperation = iSignalInfo;
			break;
		case KFlightModeInfo:
			iLastOperation = iFlightModeInfo;
			break;
		case KGetIndicator:
			iLastOperation = iGetIndicator;
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
			
		// Check the type of iLastOperation to see what it has been cast to.
		switch(iLastOperation->GetExampleType())
			{
		case KBatteryInfo:
			iState = EGetFlightModeInfo;
			iLastOperation = iFlightModeInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KSignalInfo:
			iConsole->Printf(KMenuMsg);
			GetInput();
			break;
		case KFlightModeInfo:
			Cancel();
			iState = EGetSignalInfo;
			iLastOperation = iSignalInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		case KGetIndicator:
			iState = EGetBatteryInfo;
			iLastOperation = iBatteryInfo;
			SetActive();
			CompleteOwnRequest(KErrNone);
			break;
		default:

			break;
			}
		}
	else if(aDerivedType == KPhoneId)
		{
		iLastOperation = iGetIndicator;
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
	switch(aDerivedType)
		{
	case KFlightModeInfo:
		if (IsActive())
			{
			Cancel();
			}
		if (iSignalInfo->IsActive())
			{
			iSignalInfo->Cancel();
			}
		iConsole->Printf(KMenuMsg);
		GetInput();
		break;
		}
	}

/**
Handles key presses and CMainMenu states.
*/
void CMainMenu::RunL()
	{
	switch(iState)
		{
	case EStart:
		iState = EGetIndicator;
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
		CActiveScheduler::Stop();
		break;
	case EGetIndicator:
	case EGetBatteryInfo:
	case EGetSignalInfo:
	case EGetFlightModeInfo:
		iState = ESetNotifier;
		TRAPD(err, iLastOperation->StartRequestL());
		if (err != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), err);
			return;
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
			iGetIndicator->Cancel();
			iBatteryInfo->Cancel();
			iSignalInfo->Cancel();
			iFlightModeInfo->Cancel();

			CActiveScheduler::Stop();
			break;
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

