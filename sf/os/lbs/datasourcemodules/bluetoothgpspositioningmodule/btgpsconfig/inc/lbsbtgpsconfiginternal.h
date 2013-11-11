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
// LBS Bluetooth GPS Configuration API: Internal version
//



/**
 @file
 @internalTechnology
*/

#ifndef LBS_BTGPSCONFIGINTERNAL_H
#define LBS_BTGPSCONFIGINTERNAL_H

#include <lbs/lbsbtgpsconfig.h>

class TLbsBtGpsDeviceRecord;

/** LBS Bluetooth GPS Configuration API: Internal version

This is an internal version of the LBS Bluetooth GPS Configuration API for use by LBS
internal components. It allows extra functionality which external clients should not 
have access to.

@see CLbsBtGpsConfig

@internalTechnology
*/
NONSHARABLE_CLASS(CLbsBtGpsConfigInternal) : public CLbsBtGpsConfig
	{
public:

	IMPORT_C static CLbsBtGpsConfigInternal* NewL();
	IMPORT_C static CLbsBtGpsConfigInternal* NewL(MLbsBtGpsConfigObserver& aObserver);
	IMPORT_C virtual ~CLbsBtGpsConfigInternal();

public:

	IMPORT_C TInt UpdateDevice(const TLbsBtGpsDeviceRecord& aDeviceRecord);
	IMPORT_C void GetDeviceRecordListL(RPointerArray<TLbsBtGpsDeviceRecord>& aRecordList);
	
protected:

	CLbsBtGpsConfigInternal();
	void ConstructL(MLbsBtGpsConfigObserver* aObserver);

private:

	/** Prohibit copy constructor */
	CLbsBtGpsConfigInternal(const CLbsBtGpsConfigInternal&);
	/** Prohibit assigment operator */
	CLbsBtGpsConfigInternal& operator= (const CLbsBtGpsConfigInternal&);

	};

#endif //LBS_BTGPSCONFIGINTERNAL_H
