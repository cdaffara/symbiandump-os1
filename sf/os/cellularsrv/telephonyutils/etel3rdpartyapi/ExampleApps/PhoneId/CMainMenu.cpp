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
Constructor. Called by factory constructor CMainMenu::NewLC()

@param Console to which output is printed
*/
CMainMenu::CMainMenu(CConsoleBase& aConsole)
	: CBaseMenuSync(aConsole)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CMainMenu::ConstructL()
	{
	CBaseMenuSync::ConstructL();
	iPhoneId = CPhoneId::NewL(this);
	iSubscriberId = CSubscriberId::NewL(this);
	}

/**
Destructor.
Deletes owned objects.
*/
CMainMenu::~CMainMenu()
	{
	delete iPhoneId;
	delete iSubscriberId;
	}

/**
Starts the request to get the phone ID and handles errors if they are returned.
*/
void CMainMenu::RunL()
	{
	switch(iState)
		{
	case EStart:
		iState = EGetPhoneId;
		SetActive();
		CompleteOwnRequest(KErrNone);
		break;
	case EEnd:
		CActiveScheduler::Stop();
		break;
	case EGetPhoneId:
		iState = EGetSubscriberId;
		TRAPD(errPhone, iPhoneId->StartRequestL());
		if (errPhone != KErrNone)
			{
			iConsole->Printf(_L("Request left with error code "));
			iConsole->Printf(_L("%d\n"), errPhone);
			return;
			}
		break;
	case EGetSubscriberId:
		iState = EEnd;
		TRAPD(errSub, iSubscriberId->StartRequestL());
		if (errSub != KErrNone)
			{
			iConsole->Printf(_L("%d\n"), errSub);
			return;
			}
		break;
		}
	}

/**
Cancels the outstanding asynchronous request.
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
