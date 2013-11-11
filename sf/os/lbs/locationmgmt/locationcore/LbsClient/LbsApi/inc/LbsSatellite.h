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

#ifndef __LBSSATELLITE_H__
#define __LBSSATELLITE_H__

#include <lbsposition.h>
#include <lbspositioninfo.h>


/**
Maximum number of satellites in view

@publishedAll
@released
 */
const TUint KPositionMaxSatellitesInView = 20;


class TSatelliteData
/**
This class is used to transfer data about a satellite

@publishedAll
@released
 */
	{
public:
	IMPORT_C TSatelliteData();

	IMPORT_C TInt SatelliteId() const;
	IMPORT_C void SetSatelliteId(TInt aSatelliteId);

	IMPORT_C TReal32 Azimuth() const;
	IMPORT_C void SetAzimuth(TReal32 aAzimuth);

	IMPORT_C TReal32 Elevation() const;
	IMPORT_C void SetElevation(TReal32 aElevation);

	IMPORT_C TBool IsUsed() const;
	IMPORT_C void SetIsUsed(TBool aIsUsed);

	IMPORT_C TInt SignalStrength() const;
	IMPORT_C void SetSignalStrength(TInt aSignalStrength);

protected:
	/** The ID (in GPS, the PRN) */
	TInt    iSatelliteId;
	/** The azimuth, in degrees */
	TReal32 iAzimuth;
	/** The elevation, in degrees */
	TReal32 iElevation;
	/** Whether the satellite is used */
	TBool   iIsUsed;
	/** The signal strength, in decibels */
	TInt    iSignalStrength;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};

class TPositionSatelliteInfo : public TPositionCourseInfo
/**
This class is used to store information about positions obtained by satellites.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionSatelliteInfo();

	IMPORT_C TInt GetSatelliteData(TUint aIndex, TSatelliteData& aSatelliteData) const;
	IMPORT_C TInt AppendSatelliteData(const TSatelliteData& aSatelliteData);

	IMPORT_C TInt NumSatellitesInView() const;
	IMPORT_C void ClearSatellitesInView();

	IMPORT_C TInt NumSatellitesUsed() const;

	IMPORT_C TTime SatelliteTime() const;
	IMPORT_C void SetSatelliteTime(TTime aTime);

	IMPORT_C TReal32 HorizontalDoP() const;
	IMPORT_C TReal32 VerticalDoP() const;
	IMPORT_C TReal32 TimeDoP() const;

	IMPORT_C void SetHorizontalDoP(TReal32 aDoPValue);
	IMPORT_C void SetVerticalDoP(TReal32 aDoPValue);
	IMPORT_C void SetTimeDoP(TReal32 aDoPValue);

protected:
	/** The number of satellites in view */
	TUint iNumSatellitesInView;
	/** The number of satellites used */
	TUint iNumSatellitesUsed;
	/** The time according to the satellite */
	TTime iSatelliteTime;
	/** The horizontal dilution of precision */
	TReal32 iHorizontalDoPValue;
	/** The vertical dilution of precision */
	TReal32 iVerticalDoPValue;
	/** The time dilution of precision */
	TReal32 iTimeDoPValue;
	/** The satellites in view */
	TFixedArray<TSatelliteData, KPositionMaxSatellitesInView> iSatellitesInView;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[24];
	};

#endif //__LBSSATELLITE_H__
