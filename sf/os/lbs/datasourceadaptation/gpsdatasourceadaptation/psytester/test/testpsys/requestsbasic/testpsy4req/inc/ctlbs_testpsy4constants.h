/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __CT_LBS_TESTPSY4CONSTANTS_H__
#define __CT_LBS_TESTPSY4CONSTANTS_H__

// CONSTANTS
const TReal32 KHorizontalAcc = 10;
const TReal32 KVerticalAcc = 10;
const TReal64 KLatitude = 65.3;
const TReal64 KLongitude = 11.4;

const TReal32 KAltitude = 2;

// Speed Capabilities
const TReal32 KHorizontalSpeed = 10;
const TReal32 KHorizontalSpeedError = 1;
const TReal32 KVerticalSpeed = 10; 
const TReal32 KVerticalSpeedError = 1;

// Direction Capabilities
const TReal32 KHeading = 10;
const TReal32 KHeadingError = 1;

// Satellite Capabilities
const TInt8 KSatelliteNumInView = 2;
const TInt8 KSatelliteNumUsed =	2;

//KSatelliteTime
const TReal32 KSatelliteHorizontalDoP = 10;
const TReal32 KSatelliteVerticalDoP = 10;
const TReal32 KSatelliteTimeDoP	= 10;
const TReal32 KSatellitePositionDoP = 10;
const TReal32 KSatelliteWGS84Altitude = 10;
const TReal32 KSatelliteGeoidalSeparation = 10;

//TPositionSatteliteInfo
const TInt KSatelliteId=30;
const TReal32 KAzimuth = 10;
const TBool KIsUsed = 1;
const TInt KSignalStrength = 10;
const TReal32 KElevation = 10;


#endif      // __CT_LBS_TESTPSY4CONSTANTS_H__
            
// End of File
