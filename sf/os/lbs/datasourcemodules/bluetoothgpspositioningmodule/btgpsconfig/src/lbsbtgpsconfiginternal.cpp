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

#include "lbsbtgpsconfiginternal.h"
#include "lbsbtgpsconfigimpl.h"
#include "lbsbtgpsdevicerecord.h"


/** Static constructor (without observer)
Creates an instance of the LBS Bluetooth GPS Configuration API.
This version is for clients that do not require configuration update notifications.

@return A newly constructed CLbsBtGpsConfig object.

@leave KErrPermissionDenied Client does not have the required capabilities.

@capability LocalServices

@see CLbsBtGpsConfig

@internalTechnology
@prototype
*/
EXPORT_C CLbsBtGpsConfigInternal* CLbsBtGpsConfigInternal::NewL()
	{
	CLbsBtGpsConfigInternal* self = new (ELeave) CLbsBtGpsConfigInternal();
    CleanupStack::PushL(self);
    self->ConstructL(NULL);
    CleanupStack::Pop(self);
    return self;
	}


/** Static constructor (with observer)
Creates an instance of the LBS Bluetooth GPS Configuration API, specifying a callback
to recieve update notifications.

@param aObserver [IN] Observer which will recieve update notifications.

@return A newly constructed CLbsBtGpsConfig object.

@leave KErrPermissionDenied Client does not have the required capabilities.

@capability LocalServices

@see CLbsBtGpsConfig

@internalTechnology
@prototype
*/
EXPORT_C CLbsBtGpsConfigInternal* CLbsBtGpsConfigInternal::NewL(MLbsBtGpsConfigObserver& aObserver)
	{
	CLbsBtGpsConfigInternal* self = new (ELeave) CLbsBtGpsConfigInternal();
    CleanupStack::PushL(self);
    self->ConstructL(&aObserver);
    CleanupStack::Pop(self);
    return self;
	}


/** Destructor 
@internalTechnology
@prototype
*/
EXPORT_C CLbsBtGpsConfigInternal::~CLbsBtGpsConfigInternal()
	{

	}


/** Updates the device information for a device in the list

@param aDeviceInfo [In] The updated device information. The key should identify an existing device in the list.

@return KErrNone if the operation completed succesfully.
		KErrNotFound if the specified device is not present in the list.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@internalTechnology
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfigInternal::UpdateDevice(const TLbsBtGpsDeviceRecord& aDeviceRecord)
	{
	return iImpl->UpdateDevice(aDeviceRecord);
	}


/** Retrieves the list of device records

@param aList [Out] Reference to an empty RPointerArray to populate with device records from the list.

@leave KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@internalComponent
*/
EXPORT_C void CLbsBtGpsConfigInternal::GetDeviceRecordListL(RPointerArray<TLbsBtGpsDeviceRecord>& aRecordList)
	{
	iImpl->GetDeviceRecordListL(aRecordList);
	}


/** Class constructor 
@internalComponent
@prototype
*/
CLbsBtGpsConfigInternal::CLbsBtGpsConfigInternal()
	{
	
	}


/** Second phase constructor 

@param aObserver [IN] Optional observer, which will recieve update notifications.

@internalComponent
@prototype
*/
void CLbsBtGpsConfigInternal::ConstructL(MLbsBtGpsConfigObserver* aObserver)
	{
	CLbsBtGpsConfig::ConstructL(aObserver);
	}
