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
// Location Based Services Quality Profile API
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_QUALITY_PROFILE_API_H
#define LBS_QUALITY_PROFILE_API_H

#include <e32base.h>
#include <lbs/lbsadmin.h>

class CLbsQualityProfileApiImpl;


/** Defined type for setting identifiers
*/
typedef TUint64 TLbsQualityProfileApiSetting;

/** No setting */
const TLbsQualityProfileApiSetting KLbsQualityProfileSettingNone			= 0x0000000000000000;

/** Horizontal Accuracy setting.
The default horizontal accuracy for a profile.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileHorizontalAccuracy		= 0x0000000000000001;

/** Vertical Accuracy setting.
The default vertical accuracy for a profile.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileVerticalAccuracy		= 0x0000000000000002;

/** Max Fix Time setting.
The default maximum fix time for a profile.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileMaxFixTime				= 0x0000000000000004;

/** Measurement Initial Time setting.
This is the timeout used for the initial location request when 
starting a 'hybrid' location request. 

Note: A 'hybrid' request is one where the network will accept either an
accurate GPS location or raw GPS measurement data as a response.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileMeasurementInitialFixTime
																			= 0x0000000000000008;
/** Measurement Intermediate Time setting.
This is the timeout used for the every location request except the first
when continuing a 'hybrid' location request.

Note: A 'hybrid' request is one where the network will accept either an
accurate GPS location or raw GPS measurement data as a response.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileMeasurementIntermediateFixTime	
																			= 0x0000000000000010;

/** Hybrid Time setting.
@deprecated Use KLbsQualityProfileMeasurementInitialFixTime instead.
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileHybridTime				= KLbsQualityProfileMeasurementInitialFixTime;

/**
This is the time allowed for the network to produce a final network position after maxfix time has passed
*/
const TLbsQualityProfileApiSetting KLbsQualityProfileFinalNetPositionLag		= 0x0000000000000020;


/** Location Based Services Quality Profile API.
A client uses this API to retrieve values stored in the Quality Profile 
installed for LBS.

One example client of this API is the Network Protocol Module.

The Quality Profile values represent vendor-specific quality criteria 
that are used in the course of handling location service requests.
*/
class CLbsQualityProfileApi : public CBase
	{
public:		
	IMPORT_C static CLbsQualityProfileApi* NewL();
	virtual ~CLbsQualityProfileApi();
	
	template <typename T>
		inline TInt Get(const TLbsQualityProfileId& aProfileId,
						const TLbsQualityProfileApiSetting& aSetting, 
						T& aValue) const;
	
private:
	CLbsQualityProfileApi();
	void ConstructL();
	
	IMPORT_C TInt DoGet(const TLbsQualityProfileId& aProfileId,  
						const TLbsQualityProfileApiSetting& aSetting, 
						TReal32& aRealValue) const;
	IMPORT_C TInt DoGet(const TLbsQualityProfileId& aProfileId,
						const TLbsQualityProfileApiSetting& aSetting, 
						TTimeIntervalMicroSeconds& aTimeValue) const;

private:
	/** pointer to the implementation */
	CLbsQualityProfileApiImpl* iImpl;
	};
	
#include <lbs/lbsqualityprofileapi.inl>

#endif //LBS_QUALITY_PROFILE_API_H
