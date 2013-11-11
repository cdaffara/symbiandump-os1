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



#ifndef TESTPSYCONSTANTS_H
#define TESTPSYCONSTANTS_H

// CONSTANTS
const TTimeIntervalMicroSeconds KTimerDelay = 100000;

const TReal32 KHorizontalAcc = 10;
const TReal32 KVerticalAcc = 100;
const TReal64 KLatitude = 65.3;
const TReal64 KLongitude = 11.4;

const TReal32 KAltitude = 2;

// Satellite Capabilities
const TInt8 KSatelliteNumInView2 = 2;
const TInt8 KSatelliteNumInViewError = 127;

const TInt8 KSatelliteNumUsed2 = 2;
const TInt8 KSatelliteNumUsed3 = 3;
const TInt8 KSatelliteNumUsedError = -1;

const TReal32 KSatelliteHorizontalDoP = 10;
const TReal32 KSatelliteVerticalDoP = 10;
const TReal32 KSatelliteTimeDoP	= 10;
const TReal32 KSatellitePositionDoP = 10;
const TReal32 KSatelliteWGS84Altitude = 10;
const TReal32 KSatelliteGeoidalSeparation = 10;

// Direction Capabilities
const TReal32 KHeading = 10;
const TReal32 KHeadingError = 1;


// NMEA Capabilities
const TUint8 KNMEASentences2 = 2;
_LIT8(KNMEASentencesStart2, "$GPGSV,3,2,12,24,32,051,,22,21,292,,01,20,315,,14,20,321,*71$GPGSV,3,2,12,24,32,051,,22,21,292,,01,20,315,,14,20,321,*71");
const TUint8 KNMEASentences1 = 1;
_LIT8(KNMEASentencesStart1, "$GPGSV,3,2,12,24,32,051,,22,21,292,,01,20,315,,14,20,321,*71");
const TUint8 KNMEASentences5 = 5;

// Media Capabilities
_LIT8(KURL, "https://www.aftonbladet.se");


#endif      // TESTPSYCONSTANTS_H   
            
// End of File
