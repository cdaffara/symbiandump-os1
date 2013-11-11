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
*
*/




#ifndef HWRMLIGHTDOMAINCRKEYS_H
#define HWRMLIGHTDOMAINCRKEYS_H

#include <e32cmn.h>

/**
* Light Settings repository UID.
*
* @publishedPartner
* @released
*/
const TUid KCRUidLightSettings = {0x10200C8C};


/**
* Default value for light intensity as percentage.
*
* This value is used by HWRM server when calls that
* affect lights do not include intensity parameter.
* 
* Possible integer values:
*
* 0 - 100 Percentage
*
* Default value: 50
*
* @publishedPartner
* @released
*/
const TUint32 KLightIntensity                   = 0x00000001;

/**
* Default value for light sensor sensitivity.
* 
* This value is used by HWRM server when calls 
* default intensity is used and sensor is enabled.
*
* Possible integer values:
*
* 0 - 100 Percentage
*
* Default value: 50
*
* @publishedPartner
* @released
*/
const TUint32 KLightSensorSensitivity           = 0x00000002;

/**
* Boolen value is light sensor used or not.
*
* Possible integer values:
*
* 0 - Sensor is not in use
* 1 - Sensor is in use
*
* Default value: 1
*
* @publishedPartner
* @released
*/
const TUint32 KLightSensorInUse                 = 0x00000003;

/**
* Contains bitmask of supported targets.
*
* @see CHWRMExtendedLight::TLightTarget for target enumeration.
*
* @publishedPartner
* @released
*/
const TUint32 KLightTargetMask                  = 0x00000004;

/**
* Contains bitmask of targets that are under sensor control
*
* @see CHWRMLight::TLightTarget for target enumeration.
*
* @publishedPartner
* @released
*/
const TUint32 KLightSensorMask                  = 0x00000005;

/**
* Timeout for setting off the display lights.
* Value is in seconds. 
*
* @publishedPartner
* @released
*/
const TUint32 KDisplayLightsTimeout = 0x00000006;


#endif  // HWRMLIGHTDOMAINCRKEYS_H
            
// End of File
