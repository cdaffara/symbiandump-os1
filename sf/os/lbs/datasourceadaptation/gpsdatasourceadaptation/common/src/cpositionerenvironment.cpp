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
//

/**
 @file
 @InternalComponent
*/

#include <e32base.h>
#include <lbscommon.h>
#include <lbs/lbsadmin.h>
#include <centralrepository.h>
#include <lbs/positioningpluginfwinternalcrkeys.h>

#include <lbs/adaptationpsyprivatecrkeys.h> 
#include "lbsqualityprofile.h"
#include "cpositionerenvironment.h"
#include "LbsInternalInterface.h"

CPositionerEnvironment::~CPositionerEnvironment()
	{
	delete iAdmin;
	}
	
void CPositionerEnvironment::ConstructL()
	{
	iAdmin = CLbsAdmin::NewL();
	}
	
const CLbsAdmin& CPositionerEnvironment::Admin()
	{
	return *iAdmin;
	}

TInt CPositionerEnvironment::GetDefaultSelfLocateQualityFromAdminProfile(TPositionQuality& aQuality)
	{
	// Get the profile Id for self locate requests
	TLbsQualityProfileId profileId;
	TInt err = Admin().Get(KLbsSettingQualityProfileSelfLocate, profileId);
	if (err == KErrNone)
		{
		// Retrieve the quality profile that belongs to the given Id
		TQualityProfile quality;
		err = LbsQualityProfile::GetQualityProfileById(profileId, quality);
		if (err == KErrNone)
			{
			aQuality.SetHorizontalAccuracy(quality.MinHorizontalAccuracy());
			aQuality.SetVerticalAccuracy(quality.MinVerticalAccuracy());
			aQuality.SetTimeToFirstFix(quality.MaxFixTime());
			aQuality.SetTimeToNextFix(quality.MaxFixTime());
			}
		}
	
	return err;
	}


void CPositionerEnvironment::GetEnvCriteriaL(TPositionCriteria& aCriteria, TUid aCenRepId)
	{
	TInt err = KErrNone;
	CRepository* rep = CRepository::NewLC(aCenRepId);	
	
	TPositionQuality quality;
	TInt  valInt;
	TReal valReal;
	
	err = GetDefaultSelfLocateQualityFromAdminProfile(quality);
	if(err == KErrNone) // we got accuracy and fix times from quality profile. 
		{
		iProfileInUse = ETrue;
		}
	else	// get accuracy and fix times from module info
		{
		User::LeaveIfError(rep->Get(KPSYHorizontalAccuracy, valReal));
		quality.SetHorizontalAccuracy(valReal);
		User::LeaveIfError(rep->Get(KPSYVerticalAccuracy, valReal));
		quality.SetVerticalAccuracy(valReal);
		User::LeaveIfError(rep->Get(KPSYTimeToFirstFix, valInt));
		quality.SetTimeToFirstFix(valInt);
		User::LeaveIfError(rep->Get(KPSYTimeToNextFix, valInt));
		quality.SetTimeToNextFix(valInt);
		}

	// get rest of quality details from moduleinfo
	User::LeaveIfError(rep->Get(KPSYCostIndicator, valInt));
	quality.SetCostIndicator(static_cast<TPositionQuality::TCostIndicator>(valInt));
	User::LeaveIfError(rep->Get(KPSYPowerConsumption, valInt));
	quality.SetPowerConsumption(static_cast<TPositionQuality::TPowerConsumption>(valInt));
	
	aCriteria.SetRequiredQuality(quality);
	
	User::LeaveIfError(rep->Get(KPSYCapabilities, valInt));
	aCriteria.AddRequiredCapabilities(valInt);
	CleanupStack::PopAndDestroy(rep);
	}

TUint CPositionerEnvironment::GetInactivityTimeoutL(TUid aCenRepId)
	{
	TInt  valInt;

	CRepository* rep = CRepository::NewLC(aCenRepId);	
	User::LeaveIfError(rep->Get(KAdaptationPSYInactivityDelay, valInt));
	CleanupStack::PopAndDestroy(rep);
	
	return valInt;
	}

TBool CPositionerEnvironment::ProfileInUse()
	{
	return iProfileInUse ;
	}

// End of File
