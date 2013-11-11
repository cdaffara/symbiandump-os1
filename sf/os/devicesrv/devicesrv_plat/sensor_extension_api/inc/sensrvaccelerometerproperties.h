/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Property definitions for accelerometer.
*
*/


#ifndef SENSRVACCELEROMETERPROPERTIES_H
#define SENSRVACCELEROMETERPROPERTIES_H

//  INCLUDES
#include <e32base.h>
#include <sensrvaccelerometersensor.h>

/**
* - Name:         Axis threshold value
* - Type:         TReal
* - Scope:        Channel item property
* - Mandatory:    No
* - Capability:   None
* - Description:  Threshold value for a sensor channel
*/
const TSensrvPropertyId KSensrvPropIdAxisThresholdValue = 0x00001002;

/**
* - Name:          Wakeup event channel type
* - Type:          Event
* - Datatype:      TSensrvTappingData
* - Description:   Wakeup events 
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdAccelerometerWakeupData = 0x1020507F;

/**
* - Name:         Tap duration in milliseconds
* - Type:         TInt
* - Scope:        Channel property
* - Mandatory:    No
* - Capability:   None
* - Description:  Tapping duration setting
*/
const TSensrvPropertyId KSensrvPropIdTapDurationValue = 0x00001003; // deprecated, will be removed

#endif //SENSRVCHANNELPROPERTYNAMES_H

// End of File
