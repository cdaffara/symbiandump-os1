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
// LBS Bluetooth GPS Configuration API implementation
//



/**
 @file
 @internalComponent
*/

#ifndef LBS_BTGPSCONFIGIMPL_H
#define LBS_BTGPSCONFIGIMPL_H

#include <e32base.h>
#include <centralrepository.h>

#include <lbs/lbsbtgpsconfig.h>
#include "lbsbtgpsdevicerecord.h"


/** LBS Bluetooth GPS Configuration API implementation

@see CLbsBtGpsConfig

@internalComponent
*/
class CLbsBtGpsConfigImpl : public CActive
	{
public:

	static CLbsBtGpsConfigImpl* NewL(MLbsBtGpsConfigObserver* aObserver);
	virtual ~CLbsBtGpsConfigImpl();

public:

	// Callled directly from external API:
	TInt GetDeviceCount(TInt& aCount);
	TInt AddDevice(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey);
	TInt RemoveDevice(TLbsBtGpsEntryKey aKey);
	TInt EmptyDeviceList();
	TInt ReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition);
	void GetDeviceListL(RPointerArray<TLbsBtGpsDeviceInfo>& aList);

	virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
	// Called only from internal API:
	TInt UpdateDevice(const TLbsBtGpsDeviceRecord& aDeviceRecord);
	void GetDeviceRecordListL(RPointerArray<TLbsBtGpsDeviceRecord>& aRecordList);

protected:
	
	virtual void DoCancel();
	virtual void RunL();

	void AddDeviceL(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey);
	void RemoveDeviceL(TLbsBtGpsEntryKey aKey);
	void EmptyDeviceListL();
	void ReorderDeviceL(TLbsBtGpsEntryKey aKey, TInt aNewPosition);
	void UpdateDeviceL(const TLbsBtGpsDeviceRecord& aDeviceRecord);
	
	TInt StartTransactionL(CRepository::TTransactionMode aMode);
	TInt EndTransaction();

	TInt ReadDeviceRecord(TInt aIndex, TLbsBtGpsDeviceRecord& aDeviceRecord);
	TInt WriteDeviceRecord(TInt aIndex, const TLbsBtGpsDeviceRecord& aDeviceRecord);
	TInt MoveDeviceRecord(TInt aOldIndex, TInt aNewIndex);
	
private:

	CLbsBtGpsConfigImpl(MLbsBtGpsConfigObserver* aObserver);
	void ConstructL();

	/** Prohibit copy constructor */
	 CLbsBtGpsConfigImpl(const CLbsBtGpsConfigImpl&);
	/** Prohibit assigment operator */
	 CLbsBtGpsConfigImpl& operator= (const CLbsBtGpsConfigImpl&);

private:

	/** Pointer to observer object (optional, can be NULL) */
	MLbsBtGpsConfigObserver* iObserver;
	
	/** Central repository object */
	CRepository* iRepos;
	
	};

#endif //LBS_BTGPSCONFIGIMPL_H
