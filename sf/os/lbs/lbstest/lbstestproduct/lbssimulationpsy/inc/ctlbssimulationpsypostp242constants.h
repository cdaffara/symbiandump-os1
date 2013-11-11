/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_SIMULATION_PSY_POS_TP242_CONSTANTS_H 
#define __CT_LBS_SIMULATION_PSY_POS_TP242_CONSTANTS_H

const TReal32 KHorizontalAcc = 40;
const TReal32 KVerticalAcc = 60;
const TReal64 KLatitude = -6.0;
const TReal64 KLongitude = -61.0;
const TReal32 KAltitude = 200;

// Speed Capabilities
const TReal32 KHorizontalSpeed = 1.852F;

// Direction Capabilities
const TReal32 KHeading = 256.5F;

// Compass Capabilities
const TReal32 KTrueCourse = 256.5F;
const TReal32 KMagneticVariation = 3.3F;

// NMEA Capabilities
const TUint8 KNMEASentences = 1;
_LIT(KNMEASentencesStart, "$GPGSV,3,2,12,24,32,051,,22,21,292,,01,20,315,,14,20,321,*71");

// Satellite Capabilities
const TInt8 KSatelliteNumInView = 6;
const TInt8 KSatelliteNumUsed =	5;

const TReal KSatelliteTime = 150010.10;
const TInt KSatelliteDate = 241203; 
const TReal32 KSatelliteHorizontalDoP = 4.0;
const TReal32 KSatelliteVerticalDoP = 5.5;
const TReal32 KSatelliteCalVerticalDoP = 0.9f;
const TReal32 KSatelliteCalPositionDoP = 4.1f;
const TReal32 KSatelliteTimeDoP	= 10;
const TReal32 KSatellitePositionDoP = 3.5;
const TReal32 KSatelliteAltitude = 166.5;      
const TReal32 KSatelliteGeoidalSeparation = 33.5;  //  From GGA


const TReal32 KElevation1   = 11;
const TReal32 KAzimuth1     = 111;
const TInt KSignalStrength1 = 11;
const TBool KIsUsed1        = 1;

const TReal32 KAzimuth2     = 222;
const TInt KSignalStrength2 = 22;
const TBool KIsUsed2        = 1;

const TReal32 KElevation3   = 3;
const TInt KSignalStrength3 = 33;
const TBool KIsUsed3        = 1;

const TReal32 KElevation4   = 44;
const TReal32 KAzimuth4     = 444;
const TInt KSignalStrength4 = 0;
const TBool KIsUsed4        = 1;

const TReal32 KElevation5   = -5;
const TReal32 KAzimuth5     = 555;
const TInt KSignalStrength5 = 55;
const TBool KIsUsed5        = 1;

const TReal32 KElevation6   = 66;
const TReal32 KAzimuth6     = 666;
const TInt KSignalStrength6 = 66;
const TBool KIsUsed6        = 0;

#endif      // __CT_LBS_SIMULATION_PSY_POS_TP242_CONSTANTS_H   
            
// End of File
