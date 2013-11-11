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

#include <e32base.h>

#include "lbsdevloggermacros.h"
#include "lbsqualityprofileapiimpl.h"


//********************************************************************************************************
// Class CLbsQualityProfileApi - Public Methods
//********************************************************************************************************

/** Static constructor.
@return A pointer to the new CLbsQualityProfileApi object.
*/
EXPORT_C CLbsQualityProfileApi* CLbsQualityProfileApi::NewL()
	{
	LBSLOG(ELogP9, "->S CLbsQualityProfileApi::NewL() LBSQualityProfile\n");
	CLbsQualityProfileApi* self = new(ELeave)CLbsQualityProfileApi();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Second phase constructor 
*/
void CLbsQualityProfileApi::ConstructL()
	{
	LBSLOG(ELogP1, "CLbsQualityProfileApi::ConstructL()");
	iImpl = CLbsQualityProfileApiImpl::NewL();
	}		


/** Default constructor 
*/
CLbsQualityProfileApi::CLbsQualityProfileApi():
iImpl(NULL)
	{
	LBSLOG(ELogP1, "CLbsQualityProfileApi::CLbsQualityProfileApi()");
	}


/** Default destructor
*/
CLbsQualityProfileApi::~CLbsQualityProfileApi()
	{
	LBSLOG(ELogP9, "->S CLbsQualityProfileApi::~() LBSQualityProfile\n");
	delete iImpl;
	}


//********************************************************************************************************
// Class CLbsQualityProfileApi - Private Methods
//********************************************************************************************************

/** Gets the Horizontal or Vertical accuracy settings.
@param aProfileId 	The identity of the quality profile.
@param aSetting 	The setting to be retrieved.
@param aRealValue   The value of the time setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsQualityProfileApiSetting

@internalComponent
*/
EXPORT_C TInt CLbsQualityProfileApi::DoGet(	const TLbsQualityProfileId& aProfileId,
											const TLbsQualityProfileApiSetting& aSetting, 
											TReal32& aRealValue) const
	{ 
	LBSLOG(ELogP9, "->S CLbsQualityProfileApi::Get(2,TReal32) LBSQualityProfile\n");
	
	TInt err = KErrArgument;
	
	if ((aSetting == KLbsQualityProfileHorizontalAccuracy) || (aSetting == KLbsQualityProfileVerticalAccuracy))
		{
		err =  iImpl->GetValidated(aProfileId, aSetting, aRealValue);
		}
	
	LBSLOG2(ELogP9, "  < TLbsQualityProfileId aProfileId  = %u\n", aProfileId);
	LBSLOG2(ELogP9, "  < TLbsQualityProfileApiSetting aSetting  = 0x%16X\n", aSetting);
	LBSLOG2(ELogP9, "  < TReal32 aRealValue  = %3.6g\n", aRealValue);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


/** Gets the Max Fix Time or Measurement Time settings.
@param aProfileId 	The identity of the quality profile.
@param aSetting 	The setting to be retrieved.
@param aTimeValue   The value of the time setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsQualityProfileApiSetting

@internalComponent
*/
EXPORT_C TInt CLbsQualityProfileApi::DoGet(	const TLbsQualityProfileId& aProfileId,
											const TLbsQualityProfileApiSetting& aSetting, 
											TTimeIntervalMicroSeconds& aTimeValue) const
	{ 
	LBSLOG(ELogP9, "->S CLbsQualityProfileApi::Get(3) LBSQualityProfile\n");
	
	TInt err = KErrArgument;
	
	if ((aSetting == KLbsQualityProfileMaxFixTime) 
		|| (aSetting == KLbsQualityProfileFinalNetPositionLag)
		|| (aSetting == KLbsQualityProfileMeasurementInitialFixTime)
		|| (aSetting == KLbsQualityProfileMeasurementIntermediateFixTime))
		{
		err = iImpl->GetValidated(aProfileId, aSetting, aTimeValue);
		}

	LBSLOG2(ELogP9, "  < TLbsQualityProfileId aProfileId  = %u\n", aProfileId);
	LBSLOG2(ELogP9, "  < TLbsQualityProfileApiSetting aSetting  = 0x%16X\n", aSetting);
	LBSLOG2(ELogP9, "  < TTimeIntervalMicroSeconds aTimeValue  = %ld\n", aTimeValue.Int64());
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}

