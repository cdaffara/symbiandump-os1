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

#include <e32math.h>
#include <lbs/lbsgpsmeasurement.h>
#include "LbsInternal.h"

//-----------------------------------------------------------------------------
// TPositionGpsMeasurementData
//-----------------------------------------------------------------------------
/**
Default constructor for a TPositionGpsMeasurementData.
*/
EXPORT_C TPositionGpsMeasurementData::TPositionGpsMeasurementData()
	:
	iSatelliteId(-1),
	iCarrierNoiseRatio(0),
	iDoppler(0),
	iWholeGpsChips(0),
	iFractionalGpsChips(0),
	iMultiPathIndicator(EMultiPathNotMeasured),
	iPseudoRangeRmsError(0)
	{
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/**
The ID of the satellite measured.
@see SetSatelliteId
@return The Satellite ID
*/
EXPORT_C TInt TPositionGpsMeasurementData::SatelliteId() const
	{
	return iSatelliteId;
	}

/**
Set the ID of the satellite measured.
@see SatelliteId
@param aSatelliteId The ID of the satellite measured
*/
EXPORT_C void TPositionGpsMeasurementData::SetSatelliteId(TInt aSatelliteId)
	{
	iSatelliteId = aSatelliteId;
	}

/**
Returns the carrier to noise ratio from the satellite measurement.
@see SetCarrierNoiseRatio
@return The carrier noise ratio for the satellite signal
*/
EXPORT_C TInt TPositionGpsMeasurementData::CarrierNoiseRatio() const
	{
	return iCarrierNoiseRatio;
	}

/**
Sets the carrier noise ratio for the satellite signal.
For more information see: 3GPP TS25.331.
@see CarrierNoiseRatio
@param aCarrierNoiseRatio The carrier noise ratio to be assigned
*/
EXPORT_C void TPositionGpsMeasurementData::SetCarrierNoiseRatio(TInt aCarrierNoiseRatio)
	{
	iCarrierNoiseRatio = aCarrierNoiseRatio;
	}


/**
Returns the Doppler from the satellite measurement.
Result should be multiplied by 0.2 to obtain actual frequency
For more information see: 3GPP TS25.331.
@see SetDoppler
@return The Doppler - Value should be multiplied by 0.2 to obtain actual frequency
*/
EXPORT_C TInt TPositionGpsMeasurementData::Doppler() const
	{
	return iDoppler;
	}

/**
Sets the Doppler from the satellite measurement.
Value should be actual value divided by 0.2.
For more information see: 3GPP TS25.331.
@see SetDoppler
@param aDoppler Actual doppler frequency divided by 0.2
*/
EXPORT_C void TPositionGpsMeasurementData::SetDoppler(TInt aDoppler)
	{
	iDoppler = aDoppler;
	}


/**
Returns the whole Gps chips from the satellite measurement.
For more information see: 3GPP TS25.331.
@return The whole Gps chips
*/
EXPORT_C TInt TPositionGpsMeasurementData::WholeGpsChips() const
	{
	return iWholeGpsChips;
	}

/**
Sets the whole Gps chips from the satellite measurement.
For more information see: 3GPP TS25.331.
@see WholeGpsChips
@see SetFractionalGpsChips
@param aWholeGpsChips The whole Gps chips to be set
*/
EXPORT_C void TPositionGpsMeasurementData::SetWholeGpsChips(TInt aWholeGpsChips)
	{
	iWholeGpsChips = aWholeGpsChips;
	}

/**
Returns the fractional value of the GPS code-phase measurement. Scale factor 2-10.
For more information see: 3GPP TS25.331.
@see SetFractionalGpsChips
@see WholeGpsChips
@return The FractionalGpsChips. Scale factor 2^10
*/
EXPORT_C TInt TPositionGpsMeasurementData::FractionalGpsChips() const
	{
	return iFractionalGpsChips;
	}

/**
Assigns the fractional value of the GPS code-phase measurement. Scale factor 2-10.
For more information see: 3GPP TS25.331.
@see FractionalGpsChips
@see SetWholeGpsChips
@param aFractionalGpsChips The fractional Gps chips to be assigned.
*/
EXPORT_C void TPositionGpsMeasurementData::SetFractionalGpsChips(TInt aFractionalGpsChips)
	{
	iFractionalGpsChips = aFractionalGpsChips;
	}

/**
Returns the MultipathIndicator from the satellite measurement.
This is an enumeration indicating the level of multi path error.
For more information see: 3GPP TS25.331.
@see SetMultiPathIndicator
@see TMultiPathIndicator
@return The MultiPathIndicator
*/
EXPORT_C TPositionGpsMeasurementData::TMultiPathIndicator TPositionGpsMeasurementData::MultiPathIndicator() const
	{
	return iMultiPathIndicator;
	}

/**
Sets the MultipathIndicator from the satellite measurement.
This is an enumeration indicating the level of multi path error.
For more information see: 3GPP TS25.331.
@see MultiPathIndicator
@see TMultiPathIndicator
@param aMultipathIndicator The MultipathIndicator to be set
*/
EXPORT_C void TPositionGpsMeasurementData::SetMultiPathIndicator(TMultiPathIndicator aMultiPathIndicator)
	{
	iMultiPathIndicator = aMultiPathIndicator;
	}

/**
Returns a bitmap indicating the pseudo range RMS Error from the satellite measurement.
For more information see: 3GPP TS25.331.
@see SetPseudoRangeRmsError
@return The PseudoRangeRmsError
*/
EXPORT_C TUint TPositionGpsMeasurementData::PseudoRangeRmsError() const
	{
	return iPseudoRangeRmsError;
	}

/**
Sets a bitmap indicating the pseudo range RMS error from the satellite measurement.
For more information see: 3GPP TS25.331.
@see PseudoRangeRmsError
@param aPseudoRangeRmsError The pseudo range RMS error from the satellite measurement
*/
EXPORT_C void TPositionGpsMeasurementData::SetPseudoRangeRmsError(TUint aPseudoRangeRmsError)
	{
	iPseudoRangeRmsError = aPseudoRangeRmsError;
	}


//-----------------------------------------------------------------------------
// TPositionGpsMeasurementInfo
//-----------------------------------------------------------------------------
/**
TPositionGpsMeasurementInfo constructor.
@see TPositionGpsMeasurementData
@see TPositionInfoBase
*/
EXPORT_C TPositionGpsMeasurementInfo::TPositionGpsMeasurementInfo()
	:
	iGpsTimeOfWeek(0),
	iNumMeasurements(0),
    iCalcPossible(0)
	{
	iPosClassType |= EPositionGpsMeasurementInfoClass;
	iPosClassSize = sizeof(TPositionGpsMeasurementInfo);
	}

/**
Gets measurement data by index into TPositionGpsMeasurementInfo 
@see NumMeasurements
@see AppendMeasurementData
@param aIndex The index of the measurement data requested.
       An index of zero specifies the first set of measurement data 
@param aMeasurementData Holds the returned measurement data
@return KErrNone if successful
@return KErrNotFound if parameter aIndex is out of range
*/
EXPORT_C TInt TPositionGpsMeasurementInfo::GetMeasurementData
	(
	TUint aIndex,
	TPositionGpsMeasurementData& aMeasurementData
	) const
	{
	if (aIndex >= iNumMeasurements)
		{
		return KErrNotFound;
		}
	
	aMeasurementData = iMeasurementData[aIndex];
	return KErrNone;
	}


/**
Adds the supplied measurement data to the current list
@see NumMeasurements
@see GetMeasurementData
@param aMeasurementData The data to append
@return KErrNone if successful
@return KErrOverflow if the number measurements held would exceed KPositionGpsMaxMeasurements
*/
EXPORT_C TInt TPositionGpsMeasurementInfo::AppendMeasurementData
	(
	const TPositionGpsMeasurementData& aMeasurementData
	)
	{
	if (iNumMeasurements >= KPositionGpsMaxMeasurements)
		{
		return KErrOverflow;
		}

	iMeasurementData[iNumMeasurements++] = aMeasurementData;

	return KErrNone;
	}

/**
The number of measurements held
@see GetMeasurementData
@return The number of measurements
*/
EXPORT_C TInt TPositionGpsMeasurementInfo::NumMeasurements() const
	{
	return iNumMeasurements;
	}

/**
Remove all satellite measurement data
@see NumMeasurements
@see GetMeasurementData
*/
EXPORT_C void TPositionGpsMeasurementInfo::ClearMeasurementData()
	{
	iNumMeasurements = 0;
	}

/**
Returns the GPS Time of Week in milliseconds

Note that RRC and RRLP encode this using a different algorithm in their 
respective GPS Measurement Position Response message elements.

For RRC the actual value in milliseconds is sent, in the range 0 to 604800000.

For RRLP the value sent is a 24 bit representation of actual GPS TOW in ms
in the range 0 to 14399999. This value should be set to (GpsTimeOfWeek() 
mod 14400000), ie the remainder of division by 14400000.

@see SetGpsTimeOfWeek
@return The GPS Time of Week
*/
EXPORT_C TInt TPositionGpsMeasurementInfo::GpsTimeOfWeek() const
	{
	return iGpsTimeOfWeek;
	}

/**
Sets the GPS Time of Week

The Time of Week in milliseconds, with no conversion specific
to the location protocol in use (RRC, RRLP or other).

@see GpsTimeOfWeek
@param aGpsTimeOfWeek The GPS Time of Week to assign
*/
EXPORT_C void TPositionGpsMeasurementInfo::SetGpsTimeOfWeek(TInt aGpsTimeOfWeek)
	{
	iGpsTimeOfWeek = aGpsTimeOfWeek;
	}

EXPORT_C TInt TPositionGpsMeasurementInfo::GetGpsTimingData(TGpsTimingMeasurementData& aGpsTimingData) const
 /**
 Populates aGpsTimingData with the GPS Timing Measurement Data
 @return KErrNotFound if the data has not been set, KErrNone otherwise
 */
    {
    if (iGpsTimingMeasurementData.DataType() == TGpsTimingMeasurementData::EGpsTimingDataTypeUndefined)
        {
        return KErrNotFound;
        }
     
     aGpsTimingData = iGpsTimingMeasurementData;
     return KErrNone;
     }
 
 EXPORT_C void TPositionGpsMeasurementInfo::SetGpsTimingData(const TGpsTimingMeasurementData& aGpsTimingData)
 /**
 Sets the GPS Timing Measurement Data
 @param aGpsTimingData the populated GPS Timing Measurement Data to be set
  */
     {
     iGpsTimingMeasurementData = aGpsTimingData;
     }


EXPORT_C TBool TPositionGpsMeasurementInfo::PositionCalculationPossible() const
/**
Returns true if the measurement info is complete enough so it should be possible
to calculate a position.
*/
    {
    return iCalcPossible;
    }

EXPORT_C void  TPositionGpsMeasurementInfo::SetPositionCalculationPossible(TBool aCalcPossible)
/**
The indicator is typically set by an AGPS positioning module and, if set to true, 
instructs the Location subsystem to complete Terminal Assisted positioning 
without waiting for a positioning timeout to expire.

@param [in] aCalcPossible The calculation indicator.
*/
    {
    iCalcPossible=aCalcPossible;
    }
