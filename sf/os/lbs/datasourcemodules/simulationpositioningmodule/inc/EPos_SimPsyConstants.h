/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef SIMPSYCONSTANTS_H
#define SIMPSYCONSTANTS_H

#include <e32std.h>
#include <lbs.h>

// CONSTANTS
// Number of data items in Simulation Movement mode configuration file
const TInt KNoOfSimulatedDataItems = 10;
// The maximum length of an NMEA sentence
const TInt KMaxSentenceLength = 82;
// Error code for simulated failed request
const TInt KErrorCodeForFailedRequest = KPositionQualityLoss;

const TReal KToSeconds = 1000000;
const TInt KToMicroSeconds = 1000000;
const TReal KEarthRadius = 6371010;
const TReal KToRadians = KPi/180;
const TReal KToDegrees = 180/KPi;
const TReal KLongitudeUpperBound = 180.0;
const TReal KFullCircle = 360.0;
const TInt KTimesToRandom = 20;

// CONSTANTS
_LIT(KPosSimulationFileExtension, ".sps");
_LIT(KPosNMEAFileExtension, ".nme");

#endif      // SIMPSYCONSTANTS_H

// End of File
