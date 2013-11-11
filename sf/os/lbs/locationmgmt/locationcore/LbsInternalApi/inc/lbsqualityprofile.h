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

/**
 @file
 @internalComponent 
*/

#ifndef __LBSQUALITYPROFILE_H__
#define __LBSQUALITYPROFILE_H__

#include <e32std.h>
#include <lbs/lbsadmin.h>
// jcm #include <lbs/lbslocdatasourcegpsbase.h>


/** Quality Profile data class.

A quality profile contains a set of data that can be used
to set the required quality when a location request is made.
Typically the required quality includes a minimum horizontal
accuracy and a desired time to first fix.

Within the LBS system there can be multiple quality profiles
defined, to allow a number of profiles to be used for different
types of location request.

The LbsAdmin API defines a number of settings to store the 
Id of the quality profile to be used in various types of
location request.

@internalcomponent
@released

@see TLbsQualityProfileId
*/
class TQualityProfile
	{
public:
	IMPORT_C TQualityProfile();
	
	IMPORT_C TLbsQualityProfileId QualityProfileId() const;
	IMPORT_C TReal32 MinHorizontalAccuracy() const;
	IMPORT_C TReal32 MinVerticalAccuracy() const;
	IMPORT_C TTimeIntervalMicroSeconds MaxFixTime() const;
	IMPORT_C TTimeIntervalMicroSeconds MeasurementInitialFixTime() const;
	IMPORT_C TTimeIntervalMicroSeconds MeasurementIntermediateFixTime() const;
	IMPORT_C TTimeIntervalMicroSeconds FinalNetPositionLag() const;
	
	IMPORT_C void SetQualityProfileId(TLbsQualityProfileId aId);
	IMPORT_C void SetMinHorizontalAccuracy(TReal32 aHorizontalAccuracy);
	IMPORT_C void SetMinVerticalAccuracy(TReal32 aVerticalAccuracy);
	IMPORT_C void SetMaxFixTime(TTimeIntervalMicroSeconds aMaxFixTime);
	IMPORT_C void SetMeasurementInitialFixTime(TTimeIntervalMicroSeconds aMeasurementInitialFixTime);
	IMPORT_C void SetMeasurementIntermediateFixTime(TTimeIntervalMicroSeconds aMeasurementIntermediateFixTime);
	IMPORT_C void SetFinalNetPositionLag(TTimeIntervalMicroSeconds aFinalNetPositionLag);
	
protected:
	/** Id of this profile */
	TLbsQualityProfileId iProfileId;
	/** Minimum horizonal accuracy */
	TReal32 iMinHorizontalAccuracy;
	/** Minimum vertical accuracy */
	TReal32 iMinVerticalAccuracy;
	/** The timeout used for a standard location request.  */
	TTimeIntervalMicroSeconds iMaxFixTime;
	/** The timeout used for the initial 'hybrid' location request ('alpha2'). */
	TTimeIntervalMicroSeconds iMeasurementInitialFixTime;
	/** The timeout used for each 'hybrid' location request after the initial one ('t'). */
	TTimeIntervalMicroSeconds iMeasurementIntermediateFixTime;
	/** The time allowed for the network to produce a final position after maxfix time has expired */
	TTimeIntervalMicroSeconds iFinalNetPositionLag;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[32];
	};


#endif // __LBSQUALITYPROFILE_H__
