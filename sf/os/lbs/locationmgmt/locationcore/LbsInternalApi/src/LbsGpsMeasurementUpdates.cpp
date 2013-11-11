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
//

#include <centralrepository.h>
#include <lbs/lbsgpsmeasurement.h>
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "LbsInternalCommon.h"

	
/** Gps measurement update data structure */
struct TLbsGpsMeasurementUpdateData
	{
	/** Gps measurement info data */
	TUint64 iGpsMeasurementInfo[(sizeof(TPositionGpsMeasurementInfo)+(sizeof(TUint64)-1))/sizeof(TUint64)];	
	/** Gps measurement update error */
	TInt iError;
	/** Actual time for Gps measurement update */
	TTime iActualTime;
	/** new - need to tell if the property has been defined but contains no 
	useful data, of if it has been populated with some real data.. 
	This is a purely internal field. */
	TBool iValidData; 
	TInt iBytesWritten;
	};

/** Descriptor package of the Gps measurement update data */
typedef TPckg<TLbsGpsMeasurementUpdateData> TLbsGpsMeasurementUpdateDataPckg;
/** Descriptor package of the Gps measurement update data */
typedef TPckgC<TLbsGpsMeasurementUpdateData> TLbsGpsMeasurementUpdateDataPckgC;


/** Initialize the GPS measurement update data. 

Creates the RProperty used to publish the most recent
GPS measurement update.

@see TLbsGpsMeasurementUpdateData
@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::InitializeL()
	{
	const TSecurityPolicy KReadPolicy(ECapabilityReadDeviceData,ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData,ECapabilityLocation);

	TLbsGpsMeasurementUpdateData defaultData;
	defaultData.iError = KErrNotFound;
	defaultData.iActualTime = 0;
	// indicate that the property is ok, but contains no data.
	defaultData.iValidData = EFalse;
	
	// Define the property and set the initial data
	TInt err = RProperty::Define(KLbsGpsMeasurementUpdatesKeyBase, 
								 RProperty::ELargeByteArray, 
								 KReadPolicy, 
								 KWritePolicy, 
								 sizeof(TLbsGpsMeasurementUpdateData));
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	TLbsGpsMeasurementUpdateDataPckgC pckg(defaultData);
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
									  KLbsGpsMeasurementUpdatesKeyBase, 
									  pckg));

	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KGpsMeasurementUpdatesCategoryKey, 
									   RProcess().SecureId());
	}

/**	Shutdown this interface and free any used resources.

@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::ShutDownL()
	{
	User::LeaveIfError(RProperty::Delete(KLbsGpsMeasurementUpdatesKeyBase));
	}

/**	

@released
@internalComponent
*/
EXPORT_C RLbsGpsMeasurementUpdates::RLbsGpsMeasurementUpdates()
	{
	}
	
/**	Open a connection to GPS measurement update interface.

@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::OpenL()
	{
	__ASSERT_DEBUG(iProperty.Handle() == NULL, User::Invariant());

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KGpsMeasurementUpdatesCategoryKey, categoryUid);
		
	User::LeaveIfError(iProperty.Attach(categoryUid, KLbsGpsMeasurementUpdatesKeyBase));
	}
	

/** Close the connection to the GPS measurement update interface.

@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::Close()
	{
	iProperty.Close();
	}

/** Get current GPS measurement info data published on this interface.

@param aBlob pointer to a TPositionGpsMeasurementInfo object.
@param aBlobSize size of blob.
@param aActualTime The actually time for the GPS measurement data.

@see TPositionGpsMeasurementInfo
@see RProperty::Get
@return KErrNone if successful, 
        KErrNotReady if the currently available data is not valid,
        KErrNotSupported if aBlobSize is not equal to sizeof(TPositionGpsMeasurementInfo),
        KErrArgument if aBlobSize is bigger than the size of the data published on the 
        interface, 
        or one of the standard Symbian error codes.

@released
@internalComponent
*/
EXPORT_C TInt RLbsGpsMeasurementUpdates::GetGpsMeasurementInfo(
		TAny* aBlob, 
		TUint aBlobSize, 
		TTime& aActualTime) const
	{
	__ASSERT_DEBUG(iProperty.Handle() != NULL, User::Invariant());
	
	TLbsGpsMeasurementUpdateData data;
	TLbsGpsMeasurementUpdateDataPckg pckg(data);
	TInt err = const_cast<RLbsGpsMeasurementUpdates*>(this)->iProperty.Get(pckg);
	if(err != KErrNone)
		{
		return err;
		}

	// is the data contained valid?
	if(data.iValidData == EFalse)
		{
		return KErrNotReady; // nothing has been published yet
		}
	
	aActualTime = data.iActualTime;

	if(sizeof(TPositionGpsMeasurementInfo) != aBlobSize) 
		{
		return KErrNotSupported;
		}

	// skip header (i.e leave type same as passed by getter) 
	// and copy as much data as has been requested  
	TUint8* destAddr = reinterpret_cast<TUint8*>(aBlob)+sizeof(TPositionClassTypeBase);
	TUint8* srcAddr = reinterpret_cast<TUint8*>(&data.iGpsMeasurementInfo[0]) + sizeof(TPositionClassTypeBase);

	if (aBlobSize > data.iBytesWritten)
		{
		return KErrArgument;
		}
	
	TUint8* endAddr2 = Mem::Copy(destAddr, srcAddr, aBlobSize-sizeof(TPositionClassTypeBase));

	// was there an error from the agps manager, pass this back
	if(data.iError != KErrNone)
		{
		return data.iError;
		}
	
	return err;
	}

/** Set the current GPS measurement info data on the interface.

@param aError Error code of setting RProperty key
@param aBlob pointer to a TPositionGpsMeasurementInfo object.
@param aBlobSize size of blob.
@param aActualTime The actually time for the position data.

@see TPositionGpsMeasurementInfo
@see RProperty::Set
@return KErrNone if successful, 
        KErrNotSupported if aBlobSize is greater than sizeof(TPositionGpsMeasurementInfo),
        or one of the standard Symbian error codes.

@released
@internalComponent
*/
EXPORT_C TInt RLbsGpsMeasurementUpdates::SetGpsMeasurementInfo(
 		const TInt& aError, 
 		const TAny* aBlob,
 		TUint aBlobSize, 
 		const TTime& aActualTime)
	{
	__ASSERT_DEBUG(iProperty.Handle() != NULL, User::Invariant());
		
	TLbsGpsMeasurementUpdateData data;
	data.iError = aError;
	data.iActualTime = aActualTime;
	// indicate that the property is ok, and now contains no data.
	data.iValidData = ETrue;

	// check that blob is no bigger than a TPositionGpsMeasurementInfo
	if (aBlobSize > sizeof(TPositionGpsMeasurementInfo))
		{
		return KErrNotSupported;
		}
	TUint8* endAddr = Mem::Copy(&data.iGpsMeasurementInfo, aBlob, aBlobSize);

	data.iBytesWritten = aBlobSize;
	TLbsGpsMeasurementUpdateDataPckgC pckg(data);
	TInt err = const_cast<RLbsGpsMeasurementUpdates*>(this)->iProperty.Set(pckg);
	return err;
	}
	
/** Set a request for notification when GPS measurement data is updated.

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::NotifyGpsMeasurementUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle() != NULL, User::Invariant());

	iProperty.Subscribe(aStatus);
	}

/** Cancel the GPS measurement data update notification request. 

@released
@internalComponent
*/
EXPORT_C void RLbsGpsMeasurementUpdates::CancelNotifyGpsMeasurementUpdate()
	{
	__ASSERT_DEBUG(iProperty.Handle() != NULL, User::Invariant());

	iProperty.Cancel();
	}

