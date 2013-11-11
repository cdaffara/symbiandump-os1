// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @released
*/

#include "cmdrendezvouswithestart.h"
#include "ssmdebug.h"


/**
 * Constructs and returns a new custom command, leaving on errors.
 * 
 * @internalComponent
 */
CCustomCmdRendezvousWithEStart* CCustomCmdRendezvousWithEStart::NewL()
	{
	CCustomCmdRendezvousWithEStart* self = new (ELeave) CCustomCmdRendezvousWithEStart();
	return self;
	}

CCustomCmdRendezvousWithEStart::CCustomCmdRendezvousWithEStart()
	{
	
	}

CCustomCmdRendezvousWithEStart::~CCustomCmdRendezvousWithEStart()
	{
	}

/**
 * Initializes this custom command.
 * 
 * @internalComponent
 */
TInt CCustomCmdRendezvousWithEStart::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	return KErrNone;
	}

/**
 * Destory this object and any resources allocated to it.
 * 
 * @internalComponent
 */
void CCustomCmdRendezvousWithEStart::Close()
	{
	}

void CCustomCmdRendezvousWithEStart::Release()
	{
	delete this;
	}

/**
 * Issues the required rendezvous.
 * 
 * @internalComponent
 */
void CCustomCmdRendezvousWithEStart::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	RProcess::Rendezvous(KErrNone);
	
	aStatus = KRequestPending;
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, KErrNone);
	}

/**
 * Cancels the requested execute
 * 
 * @internalComponent
 */
void CCustomCmdRendezvousWithEStart::ExecuteCancel()
	{
	// Nothing to do as request already completed in Execute()
	}
