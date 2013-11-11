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
#include "lbsqualityprofile.h"


/** Quality profile count key start offset in RProperty object. */
const TUint KLbsQualityProfileCountKeyOffset = KLbsQualityProfileKeyBase;	
/** Quality profile data key start offset in RProperty object */
const TUint KLbsModuleInfoDataStartKeyOffset = KLbsQualityProfileKeyBase + 1;


//
// TQualityProfile
//

EXPORT_C TQualityProfile::TQualityProfile() :
	iProfileId(0),
	iMaxFixTime(0),
	iMeasurementInitialFixTime(0),
	iMeasurementIntermediateFixTime(0),
	iFinalNetPositionLag(0)
	{
	TRealX nan;
	nan.SetNaN();
	
	iMinHorizontalAccuracy = nan;
	iMinVerticalAccuracy = nan;
	}
	
EXPORT_C TLbsQualityProfileId TQualityProfile::QualityProfileId() const
	{
	return iProfileId;
	}

EXPORT_C TReal32 TQualityProfile::MinHorizontalAccuracy() const
	{
	return iMinHorizontalAccuracy;
	}
	
EXPORT_C TReal32 TQualityProfile::MinVerticalAccuracy() const
	{
	return iMinVerticalAccuracy;
	}
	
EXPORT_C TTimeIntervalMicroSeconds TQualityProfile::MaxFixTime() const
	{
	return iMaxFixTime;
	}

EXPORT_C TTimeIntervalMicroSeconds TQualityProfile::MeasurementInitialFixTime() const
	{
	return iMeasurementInitialFixTime;
	}

EXPORT_C TTimeIntervalMicroSeconds TQualityProfile::MeasurementIntermediateFixTime() const
	{
	return iMeasurementIntermediateFixTime;
	}
	
EXPORT_C TTimeIntervalMicroSeconds TQualityProfile::FinalNetPositionLag() const
	{
	return iFinalNetPositionLag;
	}

EXPORT_C void TQualityProfile::SetQualityProfileId(TLbsQualityProfileId aId)
	{
	iProfileId = aId;
	}
	
EXPORT_C void TQualityProfile::SetMinHorizontalAccuracy(TReal32 aMinHorizontalAccuracy)
	{
	iMinHorizontalAccuracy = aMinHorizontalAccuracy;
	}
	
EXPORT_C void TQualityProfile::SetMinVerticalAccuracy(TReal32 aMinVerticalAccuracy)
	{
	iMinVerticalAccuracy = aMinVerticalAccuracy;
	}
	
EXPORT_C void TQualityProfile::SetMaxFixTime(TTimeIntervalMicroSeconds aMaxFixTime)
	{
	iMaxFixTime = aMaxFixTime;
	}

EXPORT_C void TQualityProfile::SetMeasurementInitialFixTime(TTimeIntervalMicroSeconds aMeasurementInitialFixTime)
	{
	iMeasurementInitialFixTime = aMeasurementInitialFixTime;
	}

EXPORT_C void TQualityProfile::SetMeasurementIntermediateFixTime(TTimeIntervalMicroSeconds aMeasurementIntermediateFixTime)
	{
	iMeasurementIntermediateFixTime = aMeasurementIntermediateFixTime;
	}
	
EXPORT_C void TQualityProfile::SetFinalNetPositionLag(TTimeIntervalMicroSeconds aFinalNetPositionLag)
	{
	iFinalNetPositionLag = aFinalNetPositionLag;
	}	
	
//
// LbsQualityProfile
//

/** Initialize the quality profile data. 

Check the security of the process, set the available keys to 
RProperty object, both the key for info data and count. The secure ID of the 
property is set to the initialization process secure ID.

@param aQualityProfileArray RArray containing all of the TQualityProfiles 
                            to be published.

@see TQualityProfile

@internalComponent
@released
*/
EXPORT_C void LbsQualityProfile::InitializeL(RArray<TQualityProfile>& aQualityProfileArray)
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	//First, set the data...
	const TInt count = aQualityProfileArray.Count();
	for(TInt i = 0; i < count; i++)
		{
		const TUint key = KLbsModuleInfoDataStartKeyOffset + i;
		TInt err = RProperty::Define(key, RProperty::EByteArray, 
									 KReadPolicy, KWritePolicy, 
									 sizeof(TQualityProfile));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		
		TPckg<TQualityProfile> pckg(aQualityProfileArray[i]);
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
										  key, 
										  pckg));
		}
	
	//Next, set the count.
	TInt err = RProperty::Define(KLbsQualityProfileCountKeyOffset, 
								 RProperty::EInt, 
								 KReadPolicy, 
								 KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
		
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
									  KLbsQualityProfileCountKeyOffset, 
									  count));
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KQualityProfileCategoryKey, RProcess().SecureId());
	}

/** Delete all keys defined in RProperty object.

@internalComponent
@released
*/
EXPORT_C void LbsQualityProfile::ShutDownL()
	{
	TInt err;
	TUint count;
	err = GetNumQualityProfiles(count);
	if(err)
		{
		if(err == KErrNotFound)
			{
			return; // not a problem; it's just not there.
			}
		User::Leave(err);
		}
	 
	for(TInt i = 0; i < count; i++)
		{
		const TUint key = KLbsModuleInfoDataStartKeyOffset + i;
		User::LeaveIfError(RProperty::Delete(key));
		}
		
	User::LeaveIfError(RProperty::Delete(KLbsQualityProfileCountKeyOffset));
	}

/** Get the number of quality profiles available.

@param aNumQualityProfiles Return the number of quality profiles.

@return Symbian Location service Error code

@internalComponent
@released
*/
EXPORT_C TInt LbsQualityProfile::GetNumQualityProfiles(TUint& aNumQualityProfiles)
	{
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KQualityProfileCategoryKey, 
												 categoryUid);
	if (err == KErrNone)
		{
		err = RProperty::Get(categoryUid, 
							 KLbsQualityProfileCountKeyOffset, 
							 reinterpret_cast<TInt&>(aNumQualityProfiles));
		}
		
	return err;
	}
	
/** Get quality profile data by index.

@param aIndex Index of quality profile to retrieve
@param aQualityProfile 

@return KErrNone if successful, KErrArgument if aIndex is out of range, otherwise
		one of the standard Symbian error codes.

@see TQualityProfile

@internalComponent
@released
 */
EXPORT_C TInt LbsQualityProfile::GetQualityProfileByIndex(TInt aIndex, 
												  		  TQualityProfile& aQualityProfile)
	{
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KQualityProfileCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		
	TUint count;
	err = GetNumQualityProfiles(count);
	if(err != KErrNone)
		{
		return err;
		}
	
	if( (aIndex < 0) || (aIndex >= count))
		{
		return KErrArgument;
		}
		
	TPckg<TQualityProfile> pckg(aQualityProfile);
	err = RProperty::Get(categoryUid, 
						 KLbsModuleInfoDataStartKeyOffset + aIndex, 
						 pckg);
	return err;
	}

/** Get module info data by module ID.
Only may be used to get the TPositionModuleInfo

@param aModuleId Position module ID.
@param aBlob TAny* pointer to TPositionModuleInfo object.
@param aBlobSize size , in bytes, of blob.
@param aInitializationProcessUid The initialization process UID.

@return Symbian Location service Error code

@see TLbsQualityProfileId
@see TQualityProfile

@internalComponent
@released
 */
EXPORT_C TInt LbsQualityProfile::GetQualityProfileById(TLbsQualityProfileId aId,
													   TQualityProfile& aQualityProfile)
	{
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KQualityProfileCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		
	// Get the number of quality profiles
	TUint count;
	err = GetNumQualityProfiles(count);
	if(err != KErrNone)
		{
		return err;
		}
	
	TQualityProfile profile;
	TPckg<TQualityProfile> pckg(profile);
	for(TInt i =0; i < count; i++)
		{
		TInt err = RProperty::Get(categoryUid, 
								  KLbsModuleInfoDataStartKeyOffset + i, 
								  pckg);
		if(err != KErrNone)
			{
			return err; //Something strange going on - should never happen.
			}

		if(profile.QualityProfileId() == aId)
			{
			aQualityProfile = profile;
			return KErrNone;	
			}
		}
	
	return KErrNotFound;
	}


