// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Locker.cpp - all the active connector code
// 
//

#include <cs_port.h>
#include "btcomm.h"
#include "btcommactive.h"

CBTPortLocker *CBTPortLocker::NewL(CBTPortProxy *aParent)
/**
	Create new CBTPortLocker.
	This function creates a new CBTPortLocker active object.
**/
	{
	CBTPortLocker *cc;
	cc=new (ELeave) CBTPortLocker();
	CleanupStack::PushL(cc);
	cc->InitL(aParent);
	CleanupStack::Pop();
	CActiveScheduler::Add(cc);
	return cc;
	}

void CBTPortLocker::InitL(CBTPortProxy *aParent)
	{
	iPortProxy=aParent;
	}

CBTPortLocker::~CBTPortLocker()
/**
	CBTPortLocker destructor.	
**/
	{
	Cancel();
	}

void CBTPortLocker::RunL()
/**
	CBTPortLocker RunL.
	Active status cleared.  We can now call the DoLockedAction 
	function on the corresponding port proxy object.
**/
	{
    if(iStatus==KErrNone)
        iPortProxy->DoLockedAction();
	}

void CBTPortLocker::LockSession()
/**
	CBTPortLocker LockSession.
	This function initiates the process of making the session exclusive
	on the socket server.
**/
	{
    if(!IsActive())
        {
		// We need to complete this request either by actually getting a
		// lock on ESock if we've got a handle, or by just completing the 
		// request if we don't.  It's fine to do the DoLockedAction() 
		// without a locker if we don't have an ESock handle, as we can't
		// be making any requests to ESock.
		if(iPortProxy->iSockServ.Handle())
			{
			iPortProxy->iSockServ.SetExclusiveMode(iStatus);
			SetActive();
			}
		else
			{
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	iLockOn=ETrue;
	}

void CBTPortLocker::UnlockSession()
/**
	CBTPortLocker UnlockSession.
**/
	{
    if(iPortProxy->iSockServ.Handle())
        iPortProxy->iSockServ.ClearExclusiveMode();
	iLockOn=EFalse;
	}

void CBTPortLocker::DoCancel()
/**
	CBTPortLocker DoCancel.
	Nothing to cancel.
**/
	{
	}

CBTPortLocker::CBTPortLocker() 
	: CActive (1020)
/**
	CBTPortLocker private c'tor.
	Note that this active object is set with a priority
	of 1020 which is higher than any other active object 
	running on the c32 active scheduler (note IrCOMM locker
	has a priority of 1000).  This means that this active 
	object will always be first in the scheduler's
	list of completed actions when it is completes.
**/
	{
	}

TBool CBTPortLocker::IsSessionLocked()
	{
	return iLockOn;
	}
