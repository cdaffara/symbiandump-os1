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
// LbsQualityProfileApiSubscriber.cpp
// 
//

#include <e32base.h>
#include <e32property.h>

#include <lbs/lbslocerrors.h>
#include "lbsdevloggermacros.h"
#include "LbsInternalInterface.h"
#include "lbsqualityprofile.h"

#include "lbsqualityprofileapiimpl.h"

/**
@file
@internalComponent
@released
*/


//********************************************************************************************************
// Class CLbsQualityProfileApiImplImpl - Public Methods
//********************************************************************************************************

/** Static constructor.
@return A pointer to the new CLbsQualityProfileApiImpl object.
*/
CLbsQualityProfileApiImpl* CLbsQualityProfileApiImpl::NewL()
	{
	LBSLOG(ELogP5, "CLbsQualityProfileApiImpl::NewL()");
	CLbsQualityProfileApiImpl* self = new(ELeave)CLbsQualityProfileApiImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Second phase constructor 
*/	
void CLbsQualityProfileApiImpl::ConstructL()
	{
	LBSLOG(ELogP1, "CLbsQualityProfileApiImpl::ConstructL()");
	}		


/** Default constructor 
*/
CLbsQualityProfileApiImpl::CLbsQualityProfileApiImpl()
	{
	LBSLOG(ELogP1, "CLbsQualityProfileApiImpl::CLbsQualityProfileApiImpl()");
	}


/** Default destructor
*/
CLbsQualityProfileApiImpl::~CLbsQualityProfileApiImpl()
	{
	LBSLOG(ELogP1, "CLbsQualityProfileApiImpl::~CLbsQualityProfileApiImpl()");
	}


/** Retrieve an LBS Quality Profile real value setting.

This is called by one of overloaded DoGet methods after validating the request.

@param aProfileId The identity of the quality profile.
@param aSetting The setting to be read.
@param aRealValue   The value of the setting (returned).
@return KErrNone if successful, one of the system wide error codes if failed to read the property value.
@see TLbsQualityProfileApiSetting
*/
TInt CLbsQualityProfileApiImpl::GetValidated(const TLbsQualityProfileId& aProfileId, 
											const TLbsQualityProfileApiSetting& aSetting, 
											TReal32& aRealValue)
	{	
	// Get the quality profile information and retrieve the data requested.
	TQualityProfile profile;
	TInt err = LbsQualityProfile::GetQualityProfileById(aProfileId, profile);
	if (err == KErrNone)
		{	
		if(KLbsQualityProfileHorizontalAccuracy == aSetting)
			{
			aRealValue = profile.MinHorizontalAccuracy();
			}
		else if(KLbsQualityProfileVerticalAccuracy == aSetting)
			{
			aRealValue = profile.MinVerticalAccuracy();
			}
		else
			{
			err = KErrNotSupported;
			}
		}
	LBSLOG2(ELogP5, "   RealValue: %f", aRealValue);
	return err;
	}


/** Retrieve an LBS Quality Profile time value setting.

This is called by one of overloaded DoGet methods after validating the request.

@param aProfileId The identity of the quality profile.
@param aSetting The setting to be read.
@param aTimeValue   The value of the setting (returned).
@return KErrNone if successful, one of the system wide error codes if failed to read the property value.
@see TLbsQualityProfileApiSetting
*/
TInt CLbsQualityProfileApiImpl::GetValidated(const TLbsQualityProfileId& aProfileId, 
											const TLbsQualityProfileApiSetting& aSetting, 
											TTimeIntervalMicroSeconds& aTimeValue)
	{
	// Get the quality profile information and retrieve the data requested.
	TQualityProfile profile;
	TInt err = LbsQualityProfile::GetQualityProfileById(aProfileId, profile);
	if (err == KErrNone)
		{
		if(KLbsQualityProfileMaxFixTime == aSetting)
			{
			aTimeValue = profile.MaxFixTime();
			}
		else if (KLbsQualityProfileMeasurementInitialFixTime == aSetting)
			{
			aTimeValue = profile.MeasurementInitialFixTime();
			}
		else if(KLbsQualityProfileMeasurementIntermediateFixTime == aSetting)
			{
			aTimeValue = profile.MeasurementIntermediateFixTime();
			}
		else if( KLbsQualityProfileFinalNetPositionLag == aSetting)
			{
			aTimeValue = profile.FinalNetPositionLag();
			}
		else
			{
			err = KErrNotSupported;
			}
			
		}
	LBSLOG2(ELogP5, "   TimeValue: %f", aTimeValue.Int64());
	return err;
	}



