/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel group definitions.
*
*/



#ifndef SENSRVGROUPS_H
#define SENSRVGROUPS_H

//  INCLUDES
#include <sensrvtypes.h>


/**
* Group identifier is a 16-bit number which represents a specific group. Group Id
* space is divided to three parts. Groups specific in the platform, groups specific
* in the sensor policy and proprietary group.
* 
* Spilitting is done in following way
*
* - Platform groups: 0x000-0xC000
* - Sensor policy groups: 0xC001 - 0xFFFE
* - proprietary group: 0xFFFF
*
* Group Id is used as a part of Id which is used to identify property. 
* It categorizes each Id to some specific group. Each of these Ids are are 32-bit 
* numbers. So 16 MSB represents group Id and 16 LSB represents Id inside of the group.
*/

/**
* - Name:          General group
* - Description:   Contains general definitions which are common for each channel type.
*/
const TInt KSensrvGeneralGroup = 0x0000;

/**
* - Name:          Accelerometer channels group
* - Description:   Contains accelometer related channels (including tapping-channels).
*/
const TInt KSensrvAccelerometerGroup = 0x0001;

/**
* - Name:          Naviscroll channels group
* - Description:   Contains naviscroll related channels.
*/
const TInt KSensrvNaviscrollGroup = 0x0002;

#endif //SENSRVGROUPS_H