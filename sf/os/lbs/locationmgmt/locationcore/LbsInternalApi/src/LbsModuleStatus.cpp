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
#include <lbscommon.h>
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "LbsInternalCommon.h"


/** Module status data key start offset in RProperty object */
const TUint KLbsModuleStatusDataStartKeyOffset = KLbsModuleStatusKayBase;

/** Module status data structure */
struct TLbsModuleStatusData
	{
	/** class for storing a position module's status */
	TUint64 iPositionModuleStatusBytes[(sizeof(TPositionModuleStatus)+(sizeof(TUint64)-1))/sizeof(TUint64)];	

	TInt iEventDeviceStatusCount;
	TInt iEventDataQualityStatusCount;
	};

/** Descriptor package of the module status data */
typedef TPckg<TLbsModuleStatusData> TLbsModuleStatusDataPckg;
/** Constant descriptor package of the module status data */
typedef TPckgC<TLbsModuleStatusData> TLbsModuleStatusDataPckgC;

/** Position module array, support GPS and Network location manager */
const TUid KModuleIdArray[] = 
	{
		{KLbsGpsLocManagerUidValue},
		{KLbsNetLocManagerUidValue}
	};

/** Position module array count */
const TInt KModuleIdArrayCount = sizeof(KModuleIdArray)/sizeof(TUid);

/** Get the module status key offset from position module ID

@param aModuleId Module ID.
@param aKey A reference to the module status key.
@return Symbian Location service Error code
@released
@internalComponent
 */
 TInt ModuleStatusKeyFromModuleId(TUid aModuleId, TUint& aKey)
	{
	if(aModuleId==KLbsGpsLocManagerUid)
		{
		aKey = KLbsModuleStatusDataStartKeyOffset;
		return KErrNone;
		}
	
	if(aModuleId==KLbsNetLocManagerUid)
		{
		aKey = KLbsModuleStatusDataStartKeyOffset+1;
		return KErrNone;
		}
	
	return KErrNotFound;
	}

/** Get the module status key offset from position module ID, possibly leave

@param aModuleId Module ID.
@param aKey A reference to the module status key.
@return Symbian Location service Error code
@released
@internalComponent
 */
 TUint ModuleStatusKeyFromModuleIdL(TUid aModuleId)
	{
	TUint key;
	User::LeaveIfError(ModuleStatusKeyFromModuleId(aModuleId, key));
	return key;
	}

/** RLbsModuleStatus constructor

@released
@internalComponent
 */
 EXPORT_C RLbsModuleStatus::RLbsModuleStatus()
:	iEventDeviceStatusCount(0),
	iEventDataQualityStatusCount(0)
	{
	}

/** Open the RProperty object of the specified module by module ID and initialization process UID

@param aModuleId Module ID.
@param aInitializationProcessUid The initialization process UID.
@released
@internalComponent
 */
 EXPORT_C void RLbsModuleStatus::OpenL(TUid aModuleId)
	{
	__ASSERT_DEBUG(iProperty.Handle()==NULL, User::Invariant());		//Handle already open.

	TUint key = ModuleStatusKeyFromModuleIdL(aModuleId);

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KModuleStatusCategoryKey, categoryUid);
		
	User::LeaveIfError(iProperty.Attach(categoryUid, key));
	}

/** Close the RProperty object of the module 

@released
@internalComponent
 */
EXPORT_C void RLbsModuleStatus::Close()
	{
	iProperty.Close();
	}
	
/** Initialize the moduel status data. Check the security of the process, set the available keys according to the module array to 
RProperty object. The secure ID of the property is set to the initialization process secure ID.

@released
@internalComponent
 */
EXPORT_C void RLbsModuleStatus::InitializeL(const TPositionModuleStatus& aStatusData)
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);
 
	for(TInt i=KModuleIdArrayCount-1; i>=0; i--)
		{
		TUint key = ModuleStatusKeyFromModuleIdL(KModuleIdArray[i]);
		TInt err = RProperty::Define(key, RProperty::EByteArray, KReadPolicy, KWritePolicy, sizeof(TLbsModuleStatusData));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		
		TLbsModuleStatusData data;
		Mem::Copy(data.iPositionModuleStatusBytes, &aStatusData, sizeof(TPositionModuleStatus));

		data.iEventDeviceStatusCount = 0;
		data.iEventDataQualityStatusCount = 0;
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), key, TLbsModuleStatusDataPckgC(data)));
		}
		
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KModuleStatusCategoryKey, RProcess().SecureId());
	}
	
/** Delete all keys defined in RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsModuleStatus::ShutDownL()
	{
	for(TInt i=KModuleIdArrayCount-1; i>=0; i--)
		{
		TUint key = ModuleStatusKeyFromModuleIdL(KModuleIdArray[i]);
		User::LeaveIfError(RProperty::Delete(key));
		}
	}


/** Get module status
No need to connect to call this function - to be used by RPositionServer::GetModuleStatus.
The caller's contract is that it should ensure that the blob passed is of 
type EPositionModuleStatusClass

@param aBlob TAny* pointer to a TPositionModuleStatus object.
@param aBlobSize size in bytes of the blob
@param aModuleId The position module ID.
@param aInitializationProcessUid The initialization process UID.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsModuleStatus::GetModuleStatus(TAny* aBlob, TInt aBlobSize,TPositionModuleId aModuleId)
	{
	TUint key;
	TInt err = ModuleStatusKeyFromModuleId(aModuleId, key);
	if(err!=KErrNone)
		return err;
	
	TUid categoryUid;
	err = LbsInternalCommon::GetCategoryUid(KModuleStatusCategoryKey, categoryUid);
	if (err != KErrNone)
		return err;
	
	TLbsModuleStatusData data;
	TLbsModuleStatusDataPckg pckg(data);
	err = RProperty::Get(categoryUid, key, pckg);
	if(err!=KErrNone)
		return err;
	
	Mem::Copy(aBlob, data.iPositionModuleStatusBytes, aBlobSize);

	return KErrNone;
	}
						
/** Set module status
The caller's contract is that it should ensure that the blob passed is of 
type EPositionModuleStatusClass

@param aBlob TAny* pointer to a TPositionModuleStatus object.
@param aBlobSize size in bytes of the blob
@param aModuleStatus A reference to the TPositionModuleStatusBase object.
@param aOccurredEventsSinceLastSet A constant TPositionModuleStatusEventBase::TModuleEvent value.
@return Symbian Location service Error code
@see TPositionModuleStatusEventBase
@released
@internalComponent
 */
 EXPORT_C TInt RLbsModuleStatus::SetModuleStatus(const TAny* aBlob, TInt aBlobSize,
							const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet)

	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	if((aOccurredEventsSinceLastSet&
		(TPositionModuleStatusEventBase::EEventDeviceStatus|TPositionModuleStatusEventBase::EEventDataQualityStatus))
		!=aOccurredEventsSinceLastSet)
		return KErrNotSupported;	//This would imply that there's an event other than EEventDeviceStatus or EEventDataQualityStatus
	
	if(aOccurredEventsSinceLastSet&TPositionModuleStatusEventBase::EEventDeviceStatus)
		iEventDeviceStatusCount++;
	if(aOccurredEventsSinceLastSet&TPositionModuleStatusEventBase::EEventDataQualityStatus)
		iEventDataQualityStatusCount++;
	
	TLbsModuleStatusData data;
	data.iEventDeviceStatusCount = iEventDeviceStatusCount;
	data.iEventDataQualityStatusCount = iEventDataQualityStatusCount;

	Mem::Copy(data.iPositionModuleStatusBytes,aBlob, aBlobSize);

	return iProperty.Set(TLbsModuleStatusDataPckgC(data));
	}

/** Get module status
To be used by RPositionServer::NotifyModuleStatusEvent.
The caller's contract is that it should ensure that the blob passed is of 
type EPositionModuleStatusClass

@param aBlob TAny* pointer to a TPositionModuleStatus object.
@param aBlobSize size in bytes of the blob
@param aOccurredEventsSinceLastGet A constant TPositionModuleStatusEventBase::TModuleEvent value.
@return Symbian Location service Error code
@see TPositionModuleStatusEventBase
@released
@internalComponent
 */
 EXPORT_C TInt RLbsModuleStatus::GetModuleStatus(TAny* aBlob, TInt aBlobSize,
				TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastGet) const

	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	TLbsModuleStatusData data;
	TLbsModuleStatusDataPckg pckg(data);
	
	TInt err = const_cast<RLbsModuleStatus*>(this)->iProperty.Get(pckg);
	if(err!=KErrNone)
		return err;
	
	Mem::Copy(aBlob, data.iPositionModuleStatusBytes, aBlobSize);

	aOccurredEventsSinceLastGet = 0;
	if(data.iEventDeviceStatusCount != iEventDeviceStatusCount)
		{
		aOccurredEventsSinceLastGet|=TPositionModuleStatusEventBase::EEventDeviceStatus;
		iEventDeviceStatusCount = data.iEventDeviceStatusCount;
		}
	if(data.iEventDataQualityStatusCount != iEventDataQualityStatusCount)
		{
		aOccurredEventsSinceLastGet|=TPositionModuleStatusEventBase::EEventDataQualityStatus;
		iEventDataQualityStatusCount = data.iEventDataQualityStatusCount;
		}
	return err;
	}
	

/** Set request for module status change notification by subscribe to the RProperty object
To be used by RPositionServer::NotifyModuleStatusEvent.

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
 */
 EXPORT_C void RLbsModuleStatus::NotifyModuleStatusChange(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	if((iEventDeviceStatusCount==0)&&(iEventDataQualityStatusCount==0))
		{
		//Do a dummy get to initialize the the counts the first time round.
		TLbsModuleStatusData data;
		TLbsModuleStatusDataPckg pckg(data);

		TInt err = iProperty.Get(pckg);
		if(err!=KErrNone)
			{
			TRequestStatus* statPtr = &aStatus;
			User::RequestComplete(statPtr, err);
			return;
			}
	
		iEventDeviceStatusCount = data.iEventDeviceStatusCount;
		iEventDataQualityStatusCount = data.iEventDataQualityStatusCount;
		}
	
	iProperty.Subscribe(aStatus);
	}

/** Cancel the module status change notification request. Stop the notification publish from the RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsModuleStatus::CancelNotifyModuleStatusChange()
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Cancel();
	
	iEventDeviceStatusCount=0;
	iEventDataQualityStatusCount=0;
	}

