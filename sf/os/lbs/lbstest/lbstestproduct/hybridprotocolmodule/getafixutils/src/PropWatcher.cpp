/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
//
// PropWatcher.cpp
//
// (c) Symbian Ltd, 2006
//

#include "PropWatcher.h"


CPropWatcher* CPropWatcher::NewL(TUint aCategory, TUint32 aKey, MPropWatcherObserver& aObserver)
	{
    CPropWatcher* p = new(ELeave) CPropWatcher(aObserver);
    CleanupStack::PushL(p);
    p->ConstructL(aCategory, aKey);
    CleanupStack::Pop(p);
    return p;
    }

CPropWatcher::CPropWatcher(MPropWatcherObserver& aObserver)
:CActive(EPriorityNormal),
 iObserver(aObserver)
	{
	;
	}


void CPropWatcher::ConstructL(TUint aCategory, TUint32 aKey)
    {
    // An active scheduler must be installed.
    __ASSERT_DEBUG(NULL != CActiveScheduler::Current(), User::Invariant()); // No scheduler installed

    TInt e = iProperty.Attach(TUid::Uid(aCategory), aKey);
    User::LeaveIfError(e);		
    
    CActiveScheduler::Add(this);
    RunL();	// initial subscription and process current property value
    }

CPropWatcher::~CPropWatcher()
    {
    Cancel();
    iProperty.Close();
    }

void CPropWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

void CPropWatcher::RunL()
    {
    TInt status = iStatus.Int();
    
    // re-subscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
    SetActive();
	
    if (KErrNotFound == status)
        {
        // This means that the owner of the property (the test code) has deleted it
       	// as the test has ended. Just ignore it!
        return;
        }
    else
    	{
    	iObserver.OnChange(iProperty);
    }   } 


