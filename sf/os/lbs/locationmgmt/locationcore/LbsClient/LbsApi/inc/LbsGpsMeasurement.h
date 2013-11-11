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

#ifndef __LBSGPSMEASUREMENT_H__
#define __LBSGPSMEASUREMENT_H__

#include <lbsposition.h>
#include <lbspositioninfo.h>
#include <lbs/lbsgpstimingmeasurement.h>


/**
Maximum number of GPS satellites in view

@publishedPartner
@released
 */
const TUint KPositionGpsMaxMeasurements = 16;


/**
Class TPositionGpsMeasurementData is used to contain measurement data about a
particular satellite

For more information see: 3GPP TS 25.331

@publishedPartner
@released
*/
class TPositionGpsMeasurementData
	{
public:
	enum TMultiPathIndicator 
		{
		/** Multi path error was not measured */
		EMultiPathNotMeasured = 0,
		/** Multi path error is below 5m */
		EMultiPathLow,
		/** Multi path error is between 5m below 43m (inclusive)*/
		EMultiPathMedium,
		/** Multi path error is above 43m */
		EMultiPathHigh	
		};
		
public:
	IMPORT_C TPositionGpsMeasurementData();

	IMPORT_C TInt SatelliteId() const;
	IMPORT_C void SetSatelliteId(TInt aSatelliteId);

	IMPORT_C TInt CarrierNoiseRatio() const;
	IMPORT_C void SetCarrierNoiseRatio(TInt aCarrierNoiseRatio);

	IMPORT_C TInt Doppler() const;
	IMPORT_C void SetDoppler(TInt aDoppler);

	IMPORT_C TInt WholeGpsChips() const;
	IMPORT_C void SetWholeGpsChips(TInt aWholeGpsChips);

	IMPORT_C TInt FractionalGpsChips() const;
	IMPORT_C void SetFractionalGpsChips(TInt aFactionalGpsChips);

	IMPORT_C TMultiPathIndicator MultiPathIndicator() const;
	IMPORT_C void SetMultiPathIndicator(TMultiPathIndicator aMultiPathIndicator);
	
	IMPORT_C TUint PseudoRangeRmsError() const;
	IMPORT_C void  SetPseudoRangeRmsError(TUint aPseudoRangeRmsError);

protected:
	/** The ID (in GPS, the PRN) */
	TInt    iSatelliteId;
	/** Carrier noise ratio */
	TInt   iCarrierNoiseRatio;
	/** Doppler shift(Hz). Multiple by 0.2*/
	TInt   iDoppler;
	/** Whole value of the UE GPS code-phase measurement.
        Increasing binary values of the field signify increasing measured pseudoranges */
	TInt   iWholeGpsChips;
	/** Fractional GPS chips. Scale factor 2^10 */
	TInt   iFractionalGpsChips; 				
	/** Multipath Indicator */
	TMultiPathIndicator  iMultiPathIndicator;
	/** Pseudorange RMS error bit map*/
	TUint  iPseudoRangeRmsError;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[20];
	};


/**
Class TPositionGpsMeasurementInfo is used to hold the complete set of satellite measurement data.

@publishedPartner
@released
*/
class TPositionGpsMeasurementInfo : public TPositionInfoBase
	{
public:
	IMPORT_C TPositionGpsMeasurementInfo();

	IMPORT_C TInt GetMeasurementData(TUint aIndex, TPositionGpsMeasurementData& aMeasurementData) const;
	IMPORT_C TInt AppendMeasurementData(const TPositionGpsMeasurementData& aMeasurementData);

	IMPORT_C TInt NumMeasurements() const;

	IMPORT_C void ClearMeasurementData();

	IMPORT_C TInt GpsTimeOfWeek() const;
	IMPORT_C void SetGpsTimeOfWeek(TInt aGpsTimeOfWeek);

	IMPORT_C TInt GetGpsTimingData(TGpsTimingMeasurementData& aGpsTimingData) const;
	IMPORT_C void SetGpsTimingData(const TGpsTimingMeasurementData& aGpsTimingData);

	IMPORT_C TBool PositionCalculationPossible() const;
	IMPORT_C void  SetPositionCalculationPossible(TBool aCalcPossible);

protected:
	/** Gps Time of Week in milli seconds */
	TInt iGpsTimeOfWeek;

	/** The number of measurements */
	TUint iNumMeasurements;
	/** The satellite measurements */
	TFixedArray<TPositionGpsMeasurementData, KPositionGpsMaxMeasurements> iMeasurementData;
	/** GPS/Network Timing Measurements */
    TGpsTimingMeasurementData iGpsTimingMeasurementData;
    /** Indicates if the measurement information is complete enough to calculate a position */
    TInt iCalcPossible;

private:
	/** No more reserved bytes left. */
	};

#endif //__LBSGPSMEASUREMENT_H__
