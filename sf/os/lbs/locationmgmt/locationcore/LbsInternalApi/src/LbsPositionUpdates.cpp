
// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "LbsInternalCommon.h"

#include <lbs/lbsextendedsatellite.h>

/** The start key offset of position update data */
const TUint KLbsPositionUpdatesDataStartKeyOffset = KLbsPositionUpdatesKeyBase;

/** Position module uid array, support GPS and Network location managers, Network request handler and
Location Server. Each of those processes publishes location updates to one of the RLbsPositionUpdates objects */
const TUid KModuleUidArray[] = 
	{
		{KLbsGpsLocManagerUidValue},
		{KLbsNetLocManagerUidValue},
		{KLbsNetRequestHandlerUidValue},
		{KLbsLocServerUidValue}
	};
/** Position module uid array count */
const TInt KModuleUidArrayCount = sizeof(KModuleUidArray)/sizeof(TUid);
	
	
/** Position update data structure */
struct TLbsPositionUpdateData
	{
	/** Position satellie info data */
	TUint64 iSatInfo[(sizeof(TPositionExtendedSatelliteInfo)+(sizeof(TUint64)-1))/sizeof(TUint64)];	
	/** Indicates a conflict control message.  iError must not be KErrNone */
	TBool iConflictControl;
	/** Postion update error */
	TInt iError;
	/** Target time for position update */
	TTime iTargetTime;
	/** Actual time for position update */
	TTime iActualTime;
	/** Extra attribute bitfield used to flag extra information */
	TUint iAttributes;
	/** Flag to tell the Location Server the gps Mode that has been 
	sent to the GPS module. At the moment this info is only used by
	the Location Server to spot transitions of mode into Hybrid mode **/
	TUint iGpsMode;
	/** new - need to tell if the property has been defined but contains no 
	useful data, of if it has been populated with some real data.. 
	This is a purely internal field. */
	TBool iValidData; 
	TInt iBytesWritten; 
	};

/** Descriptor package of the position update data */
typedef TPckg<TLbsPositionUpdateData> TLbsPositionUpdateDataPckg;
/** Descriptor package of the position update data */
typedef TPckgC<TLbsPositionUpdateData> TLbsPositionUpdateDataPckgC;

/** Get the position update key offset from position module ID

@param aModuleId Module ID.
@return position update key offset
@released
@internalComponent
 */
TUint PositionUpdateKeyFromModuleIdL(TUid aModuleId)
	{
	if(aModuleId==KLbsGpsLocManagerUid)
		{
		return KLbsPositionUpdatesDataStartKeyOffset;
		}
	if(aModuleId==KLbsNetLocManagerUid)
		{
		return KLbsPositionUpdatesDataStartKeyOffset+1;
		}
	if(aModuleId==KLbsNetRequestHandlerUid)
		{
		return KLbsPositionUpdatesDataStartKeyOffset+2;
		}
	if(aModuleId==KLbsLocServerUid)
		{
		return KLbsPositionUpdatesDataStartKeyOffset+3;
		}
	User::Leave(KErrNotSupported);
	return 0;	//Put this in to shut the compiler up.
	}

/** Initialize the position update data. Check the security of the process, set the available keys according to the module array to 
RProperty object. The secure ID of the property is set to the initialization process secure ID.

@see TLbsPositionUpdateData
@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdates::InitializeL(const TPositionExtendedSatelliteInfo& aSatData)
	{
	const TSecurityPolicy KReadPolicy(ECapabilityReadDeviceData,ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData,ECapabilityLocation);

	TLbsPositionUpdateData defaultData;
	
	Mem::Copy(defaultData.iSatInfo, &aSatData, sizeof(TPositionExtendedSatelliteInfo));

	defaultData.iError = KErrNotFound;		//Right error???
	defaultData.iConflictControl = EFalse;
	defaultData.iTargetTime = 0;
	defaultData.iActualTime = 0;
	// indicate that the property is ok, but contains no data.
	defaultData.iValidData = EFalse;
	
	//First, set the data...
	for(TInt i=KModuleUidArrayCount-1; i>=0; i--)
		{
		const TUint key = PositionUpdateKeyFromModuleIdL(KModuleUidArray[i]);
		TInt err = RProperty::Define(key, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TLbsPositionUpdateData));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		
		TLbsPositionUpdateDataPckgC pckg(defaultData);
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), key, pckg));
		}

	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KPositionUpdatesCategoryKey, RProcess().SecureId());
	}

/**	Delete all keys defined in RProperty object.

@released
@internalComponent
 */
EXPORT_C void RLbsPositionUpdates::ShutDownL()
	{
	for(TInt i=KModuleUidArrayCount-1; i>=0; i--)
		{
		const TUint key = PositionUpdateKeyFromModuleIdL(KModuleUidArray[i]);
		User::LeaveIfError(RProperty::Delete(key));
		}
	}

/**	Nothing here.

@released
@internalComponent
 */
EXPORT_C RLbsPositionUpdates::RLbsPositionUpdates()
	{
	}
	
/**	Open the RProperty object of the specified module by module ID and initialization process UID

@param aModuleId The position module ID.
@param aInitializationProcessUid The initialization process UID.
@released
@internalComponent
 */
EXPORT_C void RLbsPositionUpdates::OpenL(TUid aModuleId)
	{
	__ASSERT_DEBUG(iProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	TUint key = PositionUpdateKeyFromModuleIdL(aModuleId);

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KPositionUpdatesCategoryKey, categoryUid);
		
	User::LeaveIfError(iProperty.Attach(categoryUid, key));
	}
	

/** Close the RProperty object of the module 

@released
@internalComponent
 */
EXPORT_C void RLbsPositionUpdates::Close()
	{
	iProperty.Close();
	}

/** Get position info data from RProperty published from modules

@param aBlob pointer to a TPositionInfoBase object.
@param aBlobSize size of blob.
@param aTargetTime The target time for the position data.
@param aActualTime The actually time for the position data.

@see TPositionInfoBase

@return Symbian Location service Error code

@released
@internalComponent
*/
EXPORT_C TInt RLbsPositionUpdates::GetPositionInfo(TBool& aConflictControl, TAny* aBlob, 
												   TUint aBlobSize, TTime& aTargetTime, 
												   TTime& aActualTime) const
	{
	TUint attributes;
	TUint gpsMode;
	return GetPositionInfo(aConflictControl, aBlob, aBlobSize, aTargetTime, aActualTime, attributes, gpsMode);
	}

/** Get position info data from RProperty published from modules

@param aBlob pointer to a TPositionInfoBase object.
@param aBlobSize size of blob.
@param aTargetTime The target time for the position data.
@param aActualTime The actually time for the position data.
@param aAttributes Extra bit field that carries extra information for this update.
                   The values to use are defined in TPositionInfoAttributes.

@see TPositionInfoAttributes
@see TPositionInfoBase

@return Symbian Location service Error code

@released
@internalComponent
*/
EXPORT_C TInt RLbsPositionUpdates::GetPositionInfo(TBool& aConflictControl, TAny* aBlob, 
												   TUint aBlobSize, TTime& aTargetTime, 
												   TTime& aActualTime, TUint& aAttributes,
												   TUint& aGpsMode) const
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	aConflictControl = EFalse;
	
	TLbsPositionUpdateData data;
	TLbsPositionUpdateDataPckg pckg(data);
	TInt err = const_cast<RLbsPositionUpdates*>(this)->iProperty.Get(pckg);
	if(err!=KErrNone)
		{
		return err;
		}
	// is the data contained valid?
	if(data.iValidData==EFalse)
		{
		return KErrNotReady; // nothing has been published yet
		}
		
	// Is it a special conflict control message
	aConflictControl = data.iConflictControl;
	
	// was there an error from the agps manager
	err = data.iError;
	if(err < KErrNone)
		{
		return data.iError;
		}
	aTargetTime = data.iTargetTime;
	aActualTime = data.iActualTime;
	aAttributes = data.iAttributes;
	aGpsMode = data.iGpsMode;
	
	if(sizeof(TPositionExtendedSatelliteInfo) != aBlobSize)
	    {	
	    if(sizeof(TPositionSatelliteInfo) != aBlobSize) 
	        {
		    if (sizeof(TPositionCourseInfo) != aBlobSize)
		        {
			    if (sizeof(TPositionInfo) != aBlobSize)
			        {
				    return KErrNotSupported;
			        }
		        }
	        }
	    }
	// skip header (i.e leave type same as passed by getter) 
	// and copy as much data as has been requested  
	TUint8* destAddr = reinterpret_cast<TUint8*>(aBlob)+sizeof(TPositionClassTypeBase);
	TUint8* srcAddr = reinterpret_cast<TUint8*>(&data.iSatInfo[0]) + sizeof(TPositionClassTypeBase);

	if (aBlobSize > data.iBytesWritten)
		{
		return KErrArgument;
		}
	
	TUint8* endAddr2 = Mem::Copy(destAddr, srcAddr, aBlobSize-sizeof(TPositionClassTypeBase));
	
	//If some error code is associated to this update, return that
	if(data.iError != KErrNone)
		{
		err = data.iError;
		}

	return err;	
	}

/** Legacy version of SetPositionInfo() that doesn't set the new aAttributes bitfield.

THIS FUNCTION IS ONLY HERE TO EASE THE BETA RELEASE OF GETAFIX WORK WHILST THE NEW
FUNCTION IS NEEDED FOR THE EXTRA FUNCTIONALITY IN CR1010 & CR1009.

AFTER THE BETA RELEASE THIS FUNCTION SHOULD BE REMOVED AND ALL LBS COMPONENTS THAT 
DEPEND ON IT UPDATED TO USE THE NEW FUNCTION.
*/
 EXPORT_C TInt RLbsPositionUpdates::SetPositionInfo(const TInt& aError, const TBool aConflictControl, 
 													const TAny* aBlob,TUint aBlobSize, 
 													const TTime& aTargetTime, const TTime& aActualTime)

	{
	TUint attributes(0);
	TUint gpsMode(2); // This overload is called by the Agps Manager when publishing Reference Location, therefore
					  // gpsMode EGpsModeTerminalBased is wanted. Also called after an early complete indication
					  // or if the Protocol Module has finished the session with KErrServerBusy, in which case
					  // the GPS mode is irrelevant.
	return SetPositionInfo(aError, aConflictControl, aBlob, aBlobSize, aTargetTime, aActualTime, attributes, gpsMode);
	}
	
	

/** Set position info data to RProperty, should be call by modules

@param aError Error code of setting RProperty key
@param aBlob pointer to a TPositionInfoBase object.
@param aBlobSize size of blob.
@param aTargetTime The target time for the position data.
@param aActualTime The actually time for the position data.
@param aAttributes Extra bit field that carries extra information for this update.
                   The values to use are defined in TPositionInfoAttributes.

@see TPositionInfoAttributes
@see TPositionInfoBase

@return Symbian Location service Error code
@released
@internalComponent
*/
EXPORT_C TInt RLbsPositionUpdates::SetPositionInfo(const TInt aError, const TBool aConflictControl, 
												   const TAny* aBlob, TUint aBlobSize, 
												   const TTime& aTargetTime, const TTime& aActualTime, 
												   const TUint& aAttributes, const TUint& aGpsMode)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.

		
	TLbsPositionUpdateData data;
	data.iConflictControl = aConflictControl;
	data.iError = aError;
	data.iTargetTime = aTargetTime;
	data.iActualTime = aActualTime;
	data.iAttributes = aAttributes;
	data.iGpsMode = aGpsMode;
	// indicate that the property is ok, and now contains no data.
	data.iValidData = ETrue;

	// check that blob is no bigger than a TPositionExtendedSatelliteInfo
	if (aBlobSize > sizeof(TPositionExtendedSatelliteInfo))
		{
		return KErrNotSupported;
		}
	TUint8* endAddr = Mem::Copy(&data.iSatInfo, aBlob, aBlobSize);

	data.iBytesWritten = aBlobSize;
	TLbsPositionUpdateDataPckgC pckg(data);
	TInt err = const_cast<RLbsPositionUpdates*>(this)->iProperty.Set(pckg);
	return err;	
	}
	
/** Set request for position update notification by subscribe to the RProperty object

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdates::NotifyPositionUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Subscribe(aStatus);
	}

/** Cancel the position update notification request. Stop the notification publish from the RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdates::CancelNotifyPositionUpdate()
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Cancel();
	}
	
/** Get position info data from RProperty published from modules
	this is similar to GetPositionInfo but the caller should always pass in a 
	TPositionExtendedSatelliteInfo. The type of the position returned will be whatever 
	has been published by SetPositionInfo.

@param aBlob pointer to a TPositionInfoBase object.
@param aBlobSize size of blob.
@param aTargetTime The target time for the position data.
@param aActualTime The actually time for the position data.
@return Symbian Location service Error code
@released
@internalComponent
 */
EXPORT_C TInt RLbsPositionUpdates::GetAsMuchAsCanPositionInfo(TBool& aConflictControl, TAny* aBlob, TUint aBlobSize, TTime& aTargetTime, TTime& aActualTime) const
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	if(sizeof(TPositionExtendedSatelliteInfo) != aBlobSize) 
		{
		return KErrNotSupported;
		}

	aConflictControl = EFalse;
	
	TLbsPositionUpdateData data;
	TLbsPositionUpdateDataPckg pckg(data);
	TInt err = const_cast<RLbsPositionUpdates*>(this)->iProperty.Get(pckg);
	if(err!=KErrNone)
		{
		return err;
		}
	// is the data contained valid?
	if(data.iValidData==EFalse)
		{
		return KErrNotReady; // nothing has been published yet
		}
		
	// Is it a special conflict control message
	aConflictControl = data.iConflictControl;
	
	aTargetTime = data.iTargetTime;
	aActualTime = data.iActualTime;
	
	TUint8* destAddr = reinterpret_cast<TUint8*>(aBlob);
	TUint8* srcAddr = reinterpret_cast<TUint8*>(&data.iSatInfo[0]);

	TUint8* endAddr2 = Mem::Copy(destAddr, srcAddr, aBlobSize);
	
	//If some error code is associated to this update, return that
	if(data.iError != KErrNone)
		{
		err = data.iError;
		}

	return err;	

	}
