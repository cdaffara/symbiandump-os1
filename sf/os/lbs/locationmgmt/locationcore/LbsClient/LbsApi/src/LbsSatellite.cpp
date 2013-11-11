// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <lbssatellite.h>
#include "LbsInternal.h"

//-----------------------------------------------------------------------------
// TSatelliteData
//-----------------------------------------------------------------------------
EXPORT_C TSatelliteData::TSatelliteData()
/**
Default constructor for a TSatelliteData.
Note the azimuth and elevation are both initially NaN.

*/
:	iSatelliteId(-1),
	iIsUsed(EFalse),
	iSignalStrength(0)
	{
	TRealX nan;
	nan.SetNaN();

	iAzimuth = nan;
	iElevation = nan;
	
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

EXPORT_C TInt TSatelliteData::SatelliteId() const
/**
The satellite ID
@return The Satellite ID
 */
	{
	return iSatelliteId;
	}

EXPORT_C void TSatelliteData::SetSatelliteId(TInt aSatelliteId)
/**
The satellite ID to be set
@param aSatelliteId The satellite ID to be set
 */
	{
	iSatelliteId = aSatelliteId;
	}

EXPORT_C TReal32 TSatelliteData::Azimuth() const
/**
Returns the satellite azimuth
@return The satellite azimuth
 */
	{
	return iAzimuth;
	}

EXPORT_C void TSatelliteData::SetAzimuth(TReal32 aAzimuth)
/**
Sets the satellite azimuth
@param aAzimuth The satellite azimuth to be set
 */
	{
	iAzimuth = aAzimuth;
	}

EXPORT_C TReal32 TSatelliteData::Elevation() const
/**
Returns the satellite elevation
@return The satellite elevation
 */
	{
	return iElevation;
	}

EXPORT_C void TSatelliteData::SetElevation(TReal32 aElevation)
/**
Sets the satellite elevation to be set
@param aElevation The satellite elevation to be set
 */
	{
	iElevation = aElevation;
	}

EXPORT_C TBool TSatelliteData::IsUsed() const
/**
Returns Whether this satellite is used
@return Whether this satellite is used
 */
	{
	return iIsUsed;
	}

EXPORT_C void TSatelliteData::SetIsUsed(TBool aIsUsed)
/**
sets whether this satellite will be used
@param aIsUsed Whether this satellite will be used
 */
	{
	iIsUsed = aIsUsed;
	}

EXPORT_C TInt TSatelliteData::SignalStrength() const
/**
Returns the signal strength
@return The signal strength
 */
	{
	return iSignalStrength;
	}

EXPORT_C void TSatelliteData::SetSignalStrength(TInt aSignalStrength)
/**
Sets the signal strength
@param aSignalStrength The signal strength to set
 */
	{
	iSignalStrength = aSignalStrength;
	}


//-----------------------------------------------------------------------------
// TPositionSatelliteInfo
//-----------------------------------------------------------------------------
EXPORT_C TPositionSatelliteInfo::TPositionSatelliteInfo()
/**
TPositionSatelliteInfo constructor.

Note the the horizontal and vertical dilution of precision values are initially NaN.

*/
:	iNumSatellitesInView(0),
	iNumSatellitesUsed(0),
	iSatelliteTime(0)
	{
	TRealX nan;
	nan.SetNaN();

	iHorizontalDoPValue = nan;
	iVerticalDoPValue = nan;
	iTimeDoPValue = nan;

	iPosClassType |= EPositionSatelliteInfoClass;
	iPosClassSize = sizeof(TPositionSatelliteInfo);
	}

EXPORT_C TInt TPositionSatelliteInfo::GetSatelliteData(TUint aIndex,
                                                       TSatelliteData& aSatelliteData) const
/**
Gets satellite data by index into TSatelliteData 
@param aIndex The index of the satellite whose data is requested
@param aSatelliteData The TSatelliteData structure where the data will be placed
@return KErrNone if successful, KErrNotFound otherwise
 */
	{
	if (aIndex >= iNumSatellitesInView)
		{
		return KErrNotFound;
		}
	
	aSatelliteData = iSatellitesInView[aIndex];
	return KErrNone;
	}

EXPORT_C TInt TPositionSatelliteInfo::AppendSatelliteData(const TSatelliteData& aSatelliteData)
/**
Appends satellite data
@param aSatelliteData The data to append
@return KErrNone if successful, KErrOverflow otherwise
 */
	{
	if (iNumSatellitesInView >= KPositionMaxSatellitesInView)
		{
		return KErrOverflow;
		}

	iSatellitesInView[iNumSatellitesInView++] = aSatelliteData;

	if(aSatelliteData.IsUsed())
		++iNumSatellitesUsed;

	return KErrNone;
	}

EXPORT_C TInt TPositionSatelliteInfo::NumSatellitesInView() const
/**
The number of satellites in view
@return The number of satellites in view
 */
	{
	return iNumSatellitesInView;
	}

EXPORT_C void TPositionSatelliteInfo::ClearSatellitesInView()
/**
Reset the number of satellites in view

*/
	{
	iNumSatellitesInView = 0;
	iNumSatellitesUsed =0;
	}

EXPORT_C TInt TPositionSatelliteInfo::NumSatellitesUsed() const
/**
Returns the number of satellites used in the calculation
@return The number of satellites used in the calculation
 */
	{
	return iNumSatellitesUsed;
	}

EXPORT_C TTime TPositionSatelliteInfo::SatelliteTime() const
/**
Returns the time according to the satellite
@return The time according to the satellite
 */
	{
	return iSatelliteTime;
	}

EXPORT_C void TPositionSatelliteInfo::SetSatelliteTime(TTime aTime)
/**
Sets the satellite time
@param aTime The satellite time to be set
 */
	{
	iSatelliteTime = aTime;
	}

EXPORT_C TReal32 TPositionSatelliteInfo::HorizontalDoP() const
/**
Returns the horizontal dilution of precision
@return The horizontal dilution of precision
 */
	{
	return iHorizontalDoPValue;
	}

EXPORT_C TReal32 TPositionSatelliteInfo::VerticalDoP() const
/**
Returns the vertical dilution of precision
@return The vertical dilution of precision
 */
	{
	return iVerticalDoPValue;
	}

EXPORT_C TReal32 TPositionSatelliteInfo::TimeDoP() const
/**
Returns the time dilution of precision
@return The time dilution of precision
 */
	{
	return iTimeDoPValue;
	}

EXPORT_C void TPositionSatelliteInfo::SetHorizontalDoP(TReal32 aDoPValue)
/**
Sets the horizontal dilution of precision
@param aDoPValue The horizontal dilution of precision to be set
 */
	{
	iHorizontalDoPValue = aDoPValue;
	}

EXPORT_C void TPositionSatelliteInfo::SetVerticalDoP(TReal32 aDoPValue)
/**
Sets the vertical dilution of precision
@param aDoPValue The vertical dilution of precision to be set
 */
	{
	iVerticalDoPValue = aDoPValue;
	}

EXPORT_C void TPositionSatelliteInfo::SetTimeDoP(TReal32 aDoPValue)
/**
Sets the time dilution of precision
@param aDoPValue The time dilution of precision to be set
 */
	{
	iTimeDoPValue = aDoPValue;
	}

