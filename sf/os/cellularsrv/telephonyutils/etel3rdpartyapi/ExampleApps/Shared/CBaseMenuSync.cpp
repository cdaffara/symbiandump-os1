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

#include "CBaseMenuSync.h"

/**
Completes a request so that the RunL() code of the active object is executed.

@param aErr The error code to pass to User::RequestComplete().
*/
void CBaseMenuSync::CompleteOwnRequest(TInt aErr)
	{
	TRequestStatus* s = &iStatus;
	
	// Complete an asynchronous request
	User::RequestComplete(s, aErr);
	}

/**
Starts (this) active object making it active so it can take input and perform
required functionality.
*/
void CBaseMenuSync::Start()
	{
	PostOwnRequest();
	SetActive();
	CompleteOwnRequest(KErrNone);
	}

/**
Sets its own status to KRequestPending.
*/
void CBaseMenuSync::PostOwnRequest()
	{
	iStatus = KRequestPending;
	}

/**
Ends execution of the active object.
*/
void CBaseMenuSync::Terminate()
	{
	iState = EEnd;
	CompleteOwnRequest(KErrNone);
	}

/**
Constructor.

@param aConsole Reference to console object to which output will be displayed
*/
CBaseMenuSync::CBaseMenuSync(CConsoleBase& aConsole)
	: CActive(EPriorityUserInput),
	  iState(EStart),
	  iConsole(&aConsole)
	{
	CActiveScheduler::Add(this);
	}

/**
Sets the active object state to active.
*/
void CBaseMenuSync::ExecComplete()
	{
	SetActive();
	CompleteOwnRequest(KErrNone);
	}

/**
Second phase constructor.
*/
void CBaseMenuSync::ConstructL()
	{
	iTelephony = CTelephony::NewL();
	}

/**
Destructor.
*/
CBaseMenuSync::~CBaseMenuSync()
	{
	delete iTelephony;
	}
