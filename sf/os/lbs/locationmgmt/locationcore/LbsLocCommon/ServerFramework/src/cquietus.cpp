// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cquietus.h"

/** static method to create CQuietus object

@return The singleton CQuietus object
@internalTechnology
@released
 */
CQuietus* CQuietus::NewL()
	{
	CQuietus* self = new(ELeave) CQuietus();
	return self;
	}

/** destructor, cancel any outstanding request

@internalTechnology
@released
 */
CQuietus::~CQuietus()
	{
	Cancel();
	}
	
/** constructor, create CActive object with standard priority and add itself to AS

@return The singleton CQuietus object
@internalTechnology
@released
 */
CQuietus::CQuietus() : CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/** setactive itself and issue a message to force complete the request to enter into RunL

@internalTechnology
@released
 */
void CQuietus::CoupDeGrace()
	{
	// kill out active scheduler. thsi can only be done under a RunL, so we'll
	// force ourselves into the active scheduler Q and kill ourseleves when we run
	SetActive();
	TRequestStatus* trs = &iStatus;
	User::RequestComplete(trs, KErrNone);
	}

/** All it does is to kill the process!

@internalTechnology
@released
 */
void CQuietus::RunL()
	{
	// kill the controlling AS
	CActiveScheduler::Stop();
	}

/** Do the cancel job if cancel is issued to cancel an outstanding request

@internalTechnology
@released
 */
void CQuietus::DoCancel()
	{
	// no operation!!
	// This operationin NOT CANCELABLE by defination and design
	// No this is NOT a bug :)
	}
