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

#ifndef LBSHOSTSETTINGSNOTIFIER_H_
#define LBSHOSTSETTINGSNOTIFIER_H_

#include <centralrepository.h>
#include <lbs/lbshostsettingsclasstypes.h>
#include "lbshostsettingsimpl.h"
#include <lbs/lbshostsettings.h>

class CLbsHostSettingsNotifier : public CActive
	{
public:
	static CLbsHostSettingsNotifier* NewL(TLbsHostStoreId aStoreId, CRepository& aRepository, MLbsHostSettingsStoreObserver* aObserver); 
	~CLbsHostSettingsNotifier();
	void RequestNotificationL();
	void SuppressNotification(TUid aSettingsId);
	
private:
	CLbsHostSettingsNotifier(TLbsHostStoreId aStoreId, CRepository& aRepository, MLbsHostSettingsStoreObserver* aObserver);
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	CRepository& iRepository;
	TLbsHostStoreId iStoreId;
	// We do not own iObserver so do not delete it.
	// We have a pointer instead of a reference because it may be NULL
	MLbsHostSettingsStoreObserver* iObserver;
	TUid iSuppressSettingsId;
	};

#endif // LBSHOSTSETTINGSNOTIFIER_H_
