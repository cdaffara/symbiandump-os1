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
#include <lbscommon.h>

/**
We only support the GPS module for power mode advice - to add more module just extend this array
and the PowerModeAdviceKeyFromModuleIdL funciton.
*/
const TUid KModuleIdArray[] = {KLbsGpsLocManagerUidValue};
const TInt KModuleIdArrayCount = sizeof(KModuleIdArray)/sizeof(TUid);


/** Function searches for the aModuleId in the KPowerModeAdviceModuleIdToKeyMap and returns the
Associated key value.
Leaves with KErrNotSupported if aModuleId can't be found

@param aModuleId Position module ID
@return Power mode advice key base offset.
@released
@internalComponent
*/
TUint PowerModeAdviceKeyFromModuleIdL(TUid aModuleId)
	{
	if(aModuleId==KLbsGpsLocManagerUid)
		return KLbsPowerModeAdviceKeyBase;
	
	User::Leave(KErrNotSupported);
	return 0;	//Put this in to shut the compiler up.
	}

/** Initialize the power mode advice data. Check the security of the process, set the available keys to 
RProperty object. The secure ID of the property is set to 
the initialization process secure ID.
We don't make an attempt to 'roll back' if there's an error - we just leave the data hanging about.
This should never happen in real life though.

@released
@internalComponent
*/
EXPORT_C void RLbsManagerPowerModeAdvice::InitializeL()
	{
	//We only support the gps manager for power advice - just extend this array and
	//PowerModeAdviceKeyFromModuleIdL if we want to support other managers.
	
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	for(TInt i=KModuleIdArrayCount-1; i>=0; i--)
		{
		TUint key = PowerModeAdviceKeyFromModuleIdL(KModuleIdArray[i]);
		TInt err = RProperty::Define(key, RProperty::EInt, KReadPolicy, KWritePolicy);
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), key, CLbsLocationSourceGpsBase::EPowerModeUnknown));
		}

	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KManagerPowerModeAdviceCategoryKey, RProcess().SecureId());
	}

/** Delete all keys defined in RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsManagerPowerModeAdvice::ShutDownL()
	{
	for(TInt i=KModuleIdArrayCount-1; i>=0; i--)
		{
		TUint key = PowerModeAdviceKeyFromModuleIdL(KModuleIdArray[i]);
		User::LeaveIfError(RProperty::Delete(key));
		}
	}
	
/**  Nothing here.

@released
@internalComponent
 */
 EXPORT_C RLbsManagerPowerModeAdvice::RLbsManagerPowerModeAdvice()
	{}

/**  Open the RProperty object of the specified module by module ID and initialization process UID

@param aModuleId The position Module ID.
@param aInitializationProcessUid The initialization process UID.
@released
@internalComponent
 */
 EXPORT_C void RLbsManagerPowerModeAdvice::OpenL(TUid aModuleId)
	{
	__ASSERT_DEBUG(iProperty.Handle()==NULL, User::Invariant());		//Handle already open.
	
	TUint key = PowerModeAdviceKeyFromModuleIdL(aModuleId);

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KManagerPowerModeAdviceCategoryKey, categoryUid);
			
	User::LeaveIfError(iProperty.Attach(categoryUid, key));
	}

/** Close the RProperty object of the module 

@released
@internalComponent
 */
EXPORT_C void RLbsManagerPowerModeAdvice::Close()
	{
	iProperty.Close();
	}

/** Get power mode advice data.

@param aPowerModeAdvice A reference to TPowerMode storing the returned power mode advice data.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsManagerPowerModeAdvice::GetPowerModeAdvice(CLbsLocationSourceGpsBase::TPowerMode& aPowerModeAdvice) const
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return const_cast<RLbsManagerPowerModeAdvice*>(this)->iProperty.Get(reinterpret_cast<TInt&>(aPowerModeAdvice));
	}

/** Set power mode advice data.

@param aPowerModeAdvice A reference to TPowerMode storing the power mode advice data.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsManagerPowerModeAdvice::SetPowerModeAdvice(const CLbsLocationSourceGpsBase::TPowerMode& aPowerModeAdvice)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return iProperty.Set(reinterpret_cast<const TInt&>(aPowerModeAdvice));
	}
	
/** Set request for power mode advice notification by subscribe to the RProperty object

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
 */
 EXPORT_C void RLbsManagerPowerModeAdvice::NotifyPowerModeAdviceUpdate(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Subscribe(aStatus);
	}

/** Cancel the power mode advice notification request. Stop the notification publish from the RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsManagerPowerModeAdvice::CancelNotifyPowerModeAdviceUpdate()
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.

	iProperty.Cancel();
	}


