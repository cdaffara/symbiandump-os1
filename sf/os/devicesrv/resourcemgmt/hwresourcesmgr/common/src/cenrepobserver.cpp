// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cenrepobserver.h
// 
//

#include "cenrepobserver.h"
#include <centralrepository.h>

CCenRepListener* CCenRepListener::NewL( MCenRepObserver& aCallback, CRepository& aSession, TUint32 aId)
	
	{
	CCenRepListener* self = new(ELeave) CCenRepListener(aCallback, aSession, aId);
	return self;
	}

      

void CCenRepListener::StartListeningL()
	{
	if(!IsActive())
		{
		TInt err = iSession.NotifyRequest(iId, iStatus);
		User::LeaveIfError(err);
		SetActive();		
		}
	}


void CCenRepListener::StopListening()
	{
	Cancel();
	}



CCenRepListener::~CCenRepListener()
	{
	Cancel();
	}
   
void CCenRepListener::RunL()
	{
	TInt err(KErrNone);
	TInt newVal(0);

	// Careful here! If request completes successfully, iStatus is populated with the ID of the changed key.
	// Otherwise, it will contain a standard error code.
	
	if (iStatus == iId) // check that the notification is for the key that we are watching
		{
		err = iSession.Get(iId, newVal);
		}
	else if(iStatus < KErrNone) // standard error code if an error occurred
		{
		err = iStatus.Int();
		}
	else // we have received a notification for a key that we are not interested in - there's something wrong in cenrep
		{
		iCallback.HandleNotifyError(iId, KErrArgument, this);
		// return without reissuing the request, as "only one notification per setting per CRepository may be active at any one time."
		return;
		}
		
	StartListeningL();

	// now err can only be an error code (<=0)
	if (err == KErrNone)
		{
		iCallback.HandleNotifyInt(iId, newVal);
		}
	else
		{
		iCallback.HandleNotifyError(iId, err, this);
		}
	}
   
void CCenRepListener::DoCancel()
	{
	iSession.NotifyCancel(iId);
	}

CCenRepListener::CCenRepListener( MCenRepObserver& aCallback, CRepository& aSession, TUint32 aId)
	: CActive(EPriorityNormal), iCallback(aCallback), iSession(aSession), iId(aId)
	{
	CActiveScheduler::Add(this);
	}
                                                   
