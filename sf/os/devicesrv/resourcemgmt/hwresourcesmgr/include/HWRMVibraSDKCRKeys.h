/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The API provides the vibra related Central Repository keys. 
* These keys provide information on the vibra setting controlled by
* profiles, maximum vibrating time supported by the device and 
* minimum time vibra cannot be started after it was forcibly 
* turned off when maximum time was reached. In addition keys
* provide tactile feedback setting controlled by profiles and 
* default intensity and duration for feedback vibration.
* This interface relies on the Central Repository (CenRep) interface provided
* by Symbian. 
* The CenRep UID is KCRUidVibraCtrl and following keys are provided:
* - KVibraCtrlProfileVibraEnabled
* - KVibraCtrlMaxTime
* - KVibraMinOffTime
*
*/




/**
 @file 
 @publishedAll
 @released
*/

#ifndef HWRMVIBRASDKCRKEYS_H
#define HWRMVIBRASDKCRKEYS_H

#include <e32std.h>

/**
* CenRep UID for all vibra related settings keys provided by this API.
*
* @publishedAll
* @released
*/
const TUid KCRUidVibraCtrl = {0x10200C8B};

/**
* Vibra setting controlled by profiles. 
* 
* Possible integer values:
*
* 0 = Vibra is off in profile (default value) <br>
* 1 = Vibra is on in profile <br>
*
* @publishedAll
* @released
*/
const TUint32 KVibraCtrlProfileVibraEnabled     = 0x00000001;

/**
* Maximum vibrating time supported by device. 
* 
* Value is milliseconds as an integer. 
* If value is zero, no maximum time is supported.
* Maximum supported time is TMaxTInt microseconds in milliseconds.
*
* Value is read-only.
*
* Default value: 10000.
*
* @publishedAll
* @released
*/
const TUint32 KVibraCtrlMaxTime                 = 0x00000002;

/**
* Minimum time vibra cannot be started after it was
* forcibly turned off when maximum time was reached.
* This can be used to enforce vibra cooling after long 
* use periods.
*
* Value is milliseconds as an integer.
*
* Value is read-only.
*
* Default value: 0
*
* @publishedAll
* @released
*/
const TUint32 KVibraMinOffTime                  = 0x00000003;

/**
* Tactile feedback setting controlled by Profiles. 
* 
* Possible integer values:
*
* 0 = Tactile feedback is off in profile (default value) <br>
* 1 = Tactile feedback is on in profile <br>
*
* @publishedAll
* @released
*/
const TUint32 KVibraCtrlProfileFeedbackEnabled  = 0x00000004;

/**
* Tactile feedback vibration default intensity. 
* 
* Possible default intensity values are from the range -100...100.
*
* Value is read-only.
*
* Default value: 100
*
* @publishedAll
* @released
*/
const TUint32 KVibraCtrlFeedbackIntensity       = 0x00000005;

/**
* Tactile feedback vibration default duration. 
* 
* Value is milliseconds as an integer.
*
* Value is read-only.
*
* Default value: 20
*
* @publishedAll
* @released
*/
const TUint32 KVibraCtrlFeedbackDuration        = 0x00000006;



#endif  // HWRMVIBRASDKCRKEYS_H
            
// End of File
