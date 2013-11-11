/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef PSYTESTERCONSTANTS_H
#define PSYTESTERCONSTANTS_H

#include <e32std.h>

// CONSTANTS

// General constants
const TReal64 KLatitude = 3.3338;
const TReal64 KLongitude = 3.3338;
const TReal32 KAltitude = TReal32(9000.17);
const TReal32 KHorizontalAccuracy = TReal32(149.368);
const TReal32 KVerticalAccuracy = TReal32(149.368);
const TTime KTime = TTime(149);
const TReal32 KSpeed = TReal32(999.7);
const TReal32 KSpeedAccuracy = TReal32(234.32);
const TPositionModuleId KModuleId = {1238};

// Speed capability constants
const TReal32 KHorizontalSpeed = TReal32(9191.97);
const TReal32 KHorizontalSpeedError = TReal32(1.97);
const TReal32 KVerticalSpeed = TReal32(9191.97);
const TReal32 KVerticalSpeedError = TReal32(9191.97);

// Direction capability constants
const TReal32 KHeading = TReal32(9191.97);
const TReal32 KHeadingError = TReal32(9191.97);

// Compass capability constants
const TReal32 KTrueCourse = TReal32(9191.97);
const TReal32 KTrueCourseError = TReal32(9191.97);
const TReal32 KMagneticCourse = TReal32(9191.97); 
const TReal32 KMagneticVariation = TReal32(9191.97); 
const TReal32 KOrientation = TReal32(9191.97); 
const TReal32 KOrientationError = TReal32(9191.97);

// Adress capability constants
_LIT(KCountry, "AAAA");
_LIT(KCountryCode, "AAAA");
_LIT(KState, "AAAA");
_LIT(KCity, "AAAA");
_LIT(KDistrict, "AAAA");
_LIT(KStreet, "AAAA");
_LIT(KStreetExtension, "AAAA");
_LIT(KLocationName, "AAAA");
_LIT(KPostalCode, "AAAA");
_LIT(KLocality, "AAAA");
_LIT(KCrossing1, "AAAA");
_LIT(KCrossing2, "AAAA");

// Media capability constants
const TUint8 KMediaLinks = 98;
_LIT(KMediaLinksStart, "AAAA");

// Building capability constants
_LIT(KBuildingName, "AAAA");
_LIT(KBuildingFloor, "AAAA");
_LIT(KBuildingRoom, "AAAA");
_LIT(KBuildingZone, "AAAA");
_LIT(KBuildingTelephone, "AAAA");

// NMEA capability constants
const TUint8 KNMEASentences = 98;
_LIT(KNMEASentencesStart, "AAAA");

// Satellite capability constants
const TInt8 KSatelliteNumInView = 47;
const TInt8 KSatelliteNumUsed = 27;
const TTime KSatelliteTime = TTime(47);
const TReal32 KSatelliteHorizontalDoP = TReal32(47.47);
const TReal32 KSatelliteVerticalDoP = TReal32(47.47);
const TReal32 KSatelliteTimeDoP = TReal32(47.47);
const TReal32 KSatellitePositionDoP = TReal32(47.47);
const TReal32 KSatelliteSeaLevelAltitude = TReal32(-47.47);
const TReal32 KSatelliteGeoidalSeparation = TReal32(-47.47);
const TInt KSatelliteId = 2342;
const TReal32 KAzimuth = TReal32(-232.2);
const TReal32 KElevation = TReal32(-232.2);
const TBool KIsUsed = ETrue;
const TInt KSignalStrength = -194;

const TInt KMaxSentenceLength = 82; // The maximum length of an NMEA sentence

#endif      // PSYTESTERCONSTANTS_H   
            
// End of File
