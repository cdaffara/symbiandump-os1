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



#ifndef __CT_LBS_PSY_TEST_CONSTANTS_H__
#define __CT_LBS_PSY_TEST_CONSTANTS_H__


// CONSTANTS
const TReal32 KHorizontalAcc = 40;
const TReal32 KVerticalAcc = 40;

const TReal64 KLatitude = 2;
const TReal64 KLongitude = 3;
const TReal32 KAltitude = 200;

_LIT(KPositionTime, "20801027:161600");//YYYYMMDD:HHMMSS.MMMMMM

const TReal32 KSpeed = 20;
const TReal32 KHeading = 30;
const TReal32 KSpeedAcc = 2;
const TReal32 KHeadingAcc = 3;

const TInt KSatelliteId = 30;
const TReal32 KAzimuth = 30;
const TReal32 KElevation = 30;
const TBool KIsUsed = ETrue;
const TInt KSignalStrength = 10;

const TReal32 KAzimuthOdd = 35;
const TReal32 KElevationOdd = 35;
const TBool KIsUsedOdd = EFalse;
const TInt KSignalStrengthOdd = 5;

const TUint KNumberOfSatellitesInView = 12;
const TUint KNumberOfSatellitesUsed = 6;
_LIT(KSatelliteTime, "20021027:161600");
const TReal32 KHorizontalDoPValue = 1;
const TReal32 KVerticalDoPValue = 2;
const TReal32 KTimeDoPValue = 20;  






_LIT(KModuleName, "Multi PSY");


#endif      // __CT_LBS_PSY_TEST_CONSTANTS_H__   
            
// End of File
