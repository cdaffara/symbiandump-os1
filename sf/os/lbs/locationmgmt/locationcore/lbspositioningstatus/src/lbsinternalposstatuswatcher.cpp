/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cmn.h>
#include <centralrepository.h>

#include "lbsrootcenrepdefs.h"
#include "lbsinternalposstatuswatcher.h"
#include "lbspositioningstatusprops.h"



CLbsInternalPosStatusWatcher* CLbsInternalPosStatusWatcher::NewL(const TLbsPositioningStatusType& aType, MLbsInternalPosStatusObserver& aObserver)
	{
    CLbsInternalPosStatusWatcher* me=new(ELeave) CLbsInternalPosStatusWatcher(aType, aObserver);
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop(me);
    return me;
	}

CLbsInternalPosStatusWatcher::~CLbsInternalPosStatusWatcher()
	{
	Cancel();
	iProperty.Close();
	}

CLbsInternalPosStatusWatcher::CLbsInternalPosStatusWatcher(const TLbsPositioningStatusType& aType, MLbsInternalPosStatusObserver& aObserver)
	: CActive(EPriorityStandard), iObserver(aObserver), iType(aType)
	{
	}

/**
The 2nd phase constructor. Attaches to the property, completes the async 
request to cause the RunL method to be called for the first time and check
if there has already been an update to the property.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CLbsInternalPosStatusWatcher::ConstructL()
	{
    // Get the CategoryUid from the cenrep file owned by LbsRoot.
    TInt category;
    CRepository* rep = CRepository::NewLC(KLbsCenRepUid);

    // attach to the property
	if (iType == ELbsPositioningStatusMO)
		{
		// Mobile Originated Status P&S Property
        User::LeaveIfError(rep->Get(KMoPositioningStatusAPIKey, category));
        TSecureId propOwnerSecureId(category);
		User::LeaveIfError(iProperty.Attach(propOwnerSecureId, KLbsMoPositioningStatusKey, EOwnerThread));
		}
	else if (iType == ELbsPositioningStatusNI)
		{
		// Network Initiated Status P&S Property
        User::LeaveIfError(rep->Get(KNiPositioningStatusAPIKey, category));
        TSecureId propOwnerSecureId(category);
		User::LeaveIfError(iProperty.Attach(propOwnerSecureId, KLbsNiPositioningStatusKey, EOwnerThread));
		}
	else
		{
		// Any other value indicates a programming error
		User::Leave(KErrArgument);
		}

    CleanupStack::PopAndDestroy(rep);

    CActiveScheduler::Add(this);
    // initial subscription and process current property value
    RunL();
	}

void CLbsInternalPosStatusWatcher::RunL()
	{
   // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();

    TInt newStatus;
    // property updated, get new value
    if (iProperty.Get(newStatus)==KErrNotFound)
        {
        // property deleted, there's no sessions of this type! 
        iObserver.OnInternalStatusUpdate(iType, 0);
        }
    else
        {
        // inform the positioning status observer
		iObserver.OnInternalStatusUpdate(iType, newStatus);
        }
	}

void CLbsInternalPosStatusWatcher::DoCancel()
	{
	iProperty.Cancel();
	}

TInt CLbsInternalPosStatusWatcher::RunError(TInt aError)
	{
	// TODO: Handle RunL leaving...
	return aError;
	}

