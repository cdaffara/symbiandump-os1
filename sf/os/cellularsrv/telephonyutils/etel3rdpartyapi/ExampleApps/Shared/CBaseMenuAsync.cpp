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


#include "CBaseMenuAsync.h"

/**
Completes a request so that the RunL() code of the active object is executed.

@param aErr The error code to pass to User::RequestComplete().
*/
void CBaseMenuAsync::CompleteOwnRequest(TInt aErr)
	{
	TRequestStatus* status = &iStatus;
	
	// Complete an asynchronous request
	User::RequestComplete(status, aErr);
	}

/**
Starts (this) active object making it active so it can take
input and perform required functionality.
*/
void CBaseMenuAsync::Start()
	{
	PostOwnRequest();
	SetActive();
	CompleteOwnRequest(KErrNone);
	}

/**
Sets its own status to KRequestPending.
*/
void CBaseMenuAsync::PostOwnRequest()
	{
	iStatus = KRequestPending;
	}

/**
Ends execution of the active object.
*/
void CBaseMenuAsync::Terminate()
	{
	iState = EEnd;
	SetActive();
	CompleteOwnRequest(KErrNone);
	}

/**
Constructor.

@param aConsole Reference to console object to which output will be displayed
*/
CBaseMenuAsync::CBaseMenuAsync(CConsoleBase& aConsole)
	: CActive(EPriorityUserInput),
	  iState(EStart),
	  iConsole(&aConsole)
	{
	CActiveScheduler::Add(this);
	}

/**
Places a request for input to the console, reads the input character from the
keypad (keyboard etc.)
*/
void CBaseMenuAsync::GetInput()
	{
	// Gets a keystroke from the console window, asynchronously.
	iConsole->Read(iStatus);
	iState = EWaitingForKeyPress;
	SetActive();
	}

/**
Second phase constructor.
*/
void CBaseMenuAsync::ConstructL()
	{
	iTelephony = CTelephony::NewL();
	}
