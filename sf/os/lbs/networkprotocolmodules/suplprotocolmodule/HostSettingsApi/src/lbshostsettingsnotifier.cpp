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
// An Active Object that requests and handles change notification requests from the CenRep 
// 
//

/**
 @file
 @internalAll
 @deprecated 
*/

#include <e32base.h>
#include <e32debug.h>
#include "lbshostsettingsnotifier.h"
#include <lbs/lbshostsettingsclasstypes.h>
#include "lbshostsettingsconsts.h"

const TUid KInvalidSettingsId = {-1};

/**
Create new instance of notifier

@param aStoreId		Host settings store ID
@param aRepository	Repository
@param aObserver	Observer that is to receive notifications
@return New notifier
*/
CLbsHostSettingsNotifier* CLbsHostSettingsNotifier::NewL(TLbsHostStoreId aStoreId, CRepository& aRepository, MLbsHostSettingsStoreObserver* aObserver)
	{
	ASSERT(aObserver);
	CLbsHostSettingsNotifier* self = new(ELeave) CLbsHostSettingsNotifier(aStoreId, aRepository, aObserver);
	return self;
	}

/**
Constructor. Add notifier to the current active scheduler.
*/
CLbsHostSettingsNotifier::CLbsHostSettingsNotifier(TLbsHostStoreId aStoreId, CRepository& aRepository, MLbsHostSettingsStoreObserver* aObserver) : 
	CActive(EPriorityStandard), iRepository(aRepository), iStoreId(aStoreId), iObserver(aObserver), iSuppressSettingsId(KInvalidSettingsId)
	{
	CActiveScheduler::Add(this);
	}

/**
Destructor. Cancel notifications.
*/
CLbsHostSettingsNotifier::~CLbsHostSettingsNotifier()
	{
	Cancel();
	}

/**
Request notification about changes to LBS Host Settings from CenRep

@leave Error code from CenRep
*/
void CLbsHostSettingsNotifier::RequestNotificationL()
	{
	// Only one request for notification can be made at a time.
	// Any other requests are ignored.
	if (!IsActive())
		{
		// We only want one notificaton for each account that is modified,
		// not one for every record (i.e. CenRep key). So request notifications
		// for timestamp records only, since these are updated by every
		// change made via the API. This also conveniently excludes changes
		// to metadata.
		User::LeaveIfError(iRepository.NotifyRequest(static_cast<TUint32>(ETimestamp) << KRecordTypeShift, KRecordTypeMask, iStatus));
		SetActive();
		}
	}

/**
Suppress the next notification for the specified settings ID.
This method is called when an account has been modified by the
CLbsHostSettingsStore instance that created this notifier, in order
to prevent the observer receiving notifications of its own changes.

@param settingsId	Settings id to ignore
*/
void CLbsHostSettingsNotifier::SuppressNotification(TUid aSettingsId)
	{
	iSuppressSettingsId = aSettingsId;
	}
	
/**
Implements CActive. Handle a notification from CenRep.
*/
void CLbsHostSettingsNotifier::RunL()
	{
	// A notification has been recieved from the CenRep.
	// The ID of the changed setting is in iStatus.
	TInt status = iStatus.Int();
	TUid updatedSettingsId = TUid::Uid(status & KSettingIdMask);
	
	// Renew request before invoking callback to avoid missing any changes
	RequestNotificationL();
	
	// Notify the observer.
	// Ignore repository wide resets (KInvalidNotificationId) and changes
	// made by the same client session.
	if (status != NCentralRepositoryConstants::KInvalidNotificationId)
		{
		ASSERT((status & KRecordTypeMask) == (static_cast<TUint32>(ETimestamp) << KRecordTypeShift));
		if (updatedSettingsId == iSuppressSettingsId)
			{
			iSuppressSettingsId = KInvalidSettingsId;
			}
		else
			{
			iObserver->LbsHostSettingsUpdated(KErrNone, iStoreId, updatedSettingsId);
			}
		}
	}

/**
Implements CActive. Cancel notifications from CenRep.
*/
void CLbsHostSettingsNotifier::DoCancel()
	{
	iRepository.NotifyCancelAll();
	}

/**
Implements CActive. Handles leave from RunL().
*/
TInt CLbsHostSettingsNotifier::RunError(TInt aError)
	{
	iObserver->LbsHostSettingsUpdated(aError, iStoreId, KNullUid);
	return KErrNone;
	}
