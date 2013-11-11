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



#ifndef __CT_LBS_TESTPSY2CONSTANTS_H__
#define __CT_LBS_TESTPSY2CONSTANTS_H__

// CONSTANTS
const TReal32 KHorizontalAcc = 10;
const TReal32 KVerticalAcc = 100;
const TReal64 KLatitude = 65.3;
const TReal64 KLongitude = 11.4;

const TReal32 KAltitude = 2;


// Address Capabilities
_LIT(KCountry, "Sweden");
_LIT(KCountryCode,"S");
_LIT(KState, "-");
_LIT(KCity, "Gothenborg");
_LIT(KDistrict,"Gårda");
_LIT(KStreet, "Vädursgatan 6");
_LIT(KStreetExtension, "");
_LIT(KLocationName, "TietoEnator");
_LIT(KPostalCode, "412 50");
_LIT(KLocality, "Gårda");
_LIT(KCrossing1, "Fabriksgatan");
_LIT(KCrossing2, "Åvägen");

// Building Capabilities
_LIT(KBuildingName, "?");
_LIT(KBuildingFloor, "7");
_LIT(KBuildingRoom, "Yellow");
_LIT(KBuildingZone, "?");
_LIT(KBuildingTelephone, "031444444");

// NMEA Capabilities
const TUint8 KNMEASentences = 1;
_LIT8(KNMEASentencesStart, "$GPGSV,3,2,12,24,32,051,,22,21,292,,01,20,315,,14,20,321,*71");


// Media Capabilities
const TUint8 KMediaLinks = 3;
_LIT8(KMediaLink1, "https://www.aftonbladet.se");
_LIT8(KMediaLink2, "http://www.idg.se");
_LIT8(KMediaLink3, "http://www.svt.se");

#endif      // __CT_LBS_TESTPSY2CONSTANTS_H__
 
// End of File
