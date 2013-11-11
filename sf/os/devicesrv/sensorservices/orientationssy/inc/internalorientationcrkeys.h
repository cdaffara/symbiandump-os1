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
* Description:  Definitions for internal cenrep keys.
*
*/


#ifndef INTERNALORIENTATIONCRKEYS_H
#define INTERNALORIENTATIONCRKEYS_H

/**
* Orientation SSY internal CR keys.
*
* These CR keys are used in configuring the orientation angles of different
* orientation states.
*/
const TUid KCRUidOrientationSsySettings = {0x10282F0F};

/**
* Specifies an angle for changing orientation from state display up to display
* right up. The value defines how much the midpoint angle (45 degrees) has to
* be exceeded in degrees for the orientation transaction to happen. Positive
* values widens the orientation area and negative values diminishes the
* orientation area. Range of possible values is from 44 to -44
* degrees.
* 
* Value is an integer.
*
* Default value: 10
*/
const TUint32 KOriStateDisplayUpToDisplayRightUpKey = 0x00000001;

/**
* Specifies the timer for changing orientation from state display up to display
* right up. The value defines how long is the time where orientation events are
* observed before the actual orientation happens. The value is defined in
* milliseconds.
* 
* Value is an integer.
*
* Default value: 150
*/
const TUint32 KOriStateDisplayUpToDisplayRightUpTimerKey = 0x00000002;

/**
* Specifies an angle for changing orientation from state display up to display
* left up. The value defines how much the midpoint angle (45 degrees) has to
* be exceeded in degrees for the orientation transaction to happen. Positive
* values widens the orientation area and negative values diminishes the
* orientation area. Range of possible values is from 44 to -44
* degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpToDisplayLeftUpKey = 0x00000003;

/**
* Specifies the timer for changing orientation from state display up to display
* left up. The value defines how long is the time where orientation events are
* observed before the actual orientation happens. The value is defined in 
* milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpToDisplayLeftUpTimerKey = 0x00000004;

/**
* Specifies an angle for changing orientation from state display up to display
* upwards. The value defines how much the midpoint angle (45 degrees) has to
* be exceeded in degrees for the orientation transaction to happen. Positive
* values widens the orientation area and negative values diminishes the
* orientation area. Range of possible values is from 44 to -44
* degrees.
* 
* Value is an integer.
*
* Default value: 10
*/
const TUint32 KOriStateDisplayUpToDisplayUpwardsKey = 0x00000005;

/**
* Specifies the timer for changing orientation from state display up to display
* upwards. The value defines how long is the time where orientation events are
* observed before the actual orientation happens. The value is defined in 
* milliseconds.
* 
* Value is an integer.
*
* Default value: 50
*/
const TUint32 KOriStateDisplayUpToDisplayUpwardsTimerKey = 0x00000006;

/**
* Specifies an angle for changing orientation from state display up to display
* downwards. The value defines how much the midpoint angle (45 degrees) has to
* be exceeded in degrees for the orientation transaction to happen. Positive
* values widens the orientation area and negative values diminishes the
* orientation area. Range of possible values is from 44 to -44
* degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpToDisplayDownwardsKey = 0x00000007;

/**
* Specifies the timer for changing orientation from state display up to display
* downwards. The value defines how long is the time where orientation events are
* observed before the actual orientation happens. The value is defined in 
* milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpToDisplayDownwardsTimerKey = 0x00000008;

/**
* Specifies an angle for changing orientation from state display up to display
* down. The value defines how much the midpoint angle (45 degrees) has to
* be exceeded in degrees for the orientation transaction to happen. Positive
* values widens the orientation area and negative values diminishes the
* orientation area. Range of possible values is from 44 to -44
* degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpToDisplayDownKey = 0x00000009;

/**
* Specifies the timer for changing orientation from state display up to display
* down. The value defines how long is the time where orientation events are
* observed before the actual orientation happens. The value is defined in 
* milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpToDisplayDownTimerKey = 0x0000000A;

/**
* Specifies an angle for changing orientation from state display down to
* display right up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownToDisplayRightUpKey = 0x0000000B;

/**
* Specifies an angle for changing orientation from state display down to
* display right up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownToDisplayRightUpTimerKey = 0x0000000C;

/**
* Specifies an angle for changing orientation from state display down to
* display left up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownToDisplayLeftUpKey = 0x0000000D;

/**
* Specifies an angle for changing orientation from state display down to
* display left up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownToDisplayLeftUpTimerKey = 0x0000000E;

/**
* Specifies an angle for changing orientation from state display down to
* display upwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownToDisplayUpwardsKey = 0x0000000F;

/**
* Specifies an angle for changing orientation from state display down to
* display upwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownToDisplayUpwardsTimerKey = 0x00000010;

/**
* Specifies an angle for changing orientation from state display down to
* display downwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownToDisplayDownwardsKey = 0x00000011;

/**
* Specifies an angle for changing orientation from state display down to
* display downwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownToDisplayDownwardsTimerKey = 0x00000012;

/**
* Specifies an angle for changing orientation from state display down to
* display up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownToDisplayUpKey = 0x00000013;

/**
* Specifies an angle for changing orientation from state display down to
* display up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownToDisplayUpTimerKey = 0x00000014;

/**
* Specifies an angle for changing orientation from state display upwards to
* display right up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: -5
*/
const TUint32 KOriStateDisplayUpwardsToDisplayRightUpKey = 0x00000015;

/**
* Specifies an angle for changing orientation from state display upwards to
* display right up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 150
*/
const TUint32 KOriStateDisplayUpwardsToDisplayRightUpTimerKey = 0x00000016;

/**
* Specifies an angle for changing orientation from state display upwards to
* display left up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpwardsToDisplayLeftUpKey = 0x00000017;

/**
* Specifies an angle for changing orientation from state display upwards to
* display left up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpwardsToDisplayLeftUpTimerKey = 0x00000018;

/**
* Specifies an angle for changing orientation from state display upwards to
* display up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: -5
*/
const TUint32 KOriStateDisplayUpwardsToDisplayUpKey = 0x00000019;

/**
* Specifies an angle for changing orientation from state display upwards to
* display up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 150
*/
const TUint32 KOriStateDisplayUpwardsToDisplayUpTimerKey = 0x0000001A;

/**
* Specifies an angle for changing orientation from state display upwards to
* display downwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpwardsToDisplayDownwardsKey = 0x0000001B;

/**
* Specifies an angle for changing orientation from state display upwards to
* display downwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpwardsToDisplayDownwardsTimerKey = 0x0000001C;

/**
* Specifies an angle for changing orientation from state display upwards to
* display down. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayUpwardsToDisplayDownKey = 0x0000001D;

/**
* Specifies an angle for changing orientation from state display upwards to
* display down. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayUpwardsToDisplayDownTimerKey = 0x0000001E;

/**
* Specifies an angle for changing orientation from state display downwards to
* display right up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownwardsToDisplayRightUpKey = 0x0000001F;

/**
* Specifies an angle for changing orientation from state display downwards to
* display right up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownwardsToDisplayRightUpTimerKey = 0x00000020;

/**
* Specifies an angle for changing orientation from state display downwards to
* display left up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownwardsToDisplayLeftUpKey = 0x00000021;

/**
* Specifies an angle for changing orientation from state display downwards to
* display left up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownwardsToDisplayLeftUpTimerKey = 0x00000022;

/**
* Specifies an angle for changing orientation from state display downwards to
* display downwards. TThe value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownwardsToDisplayUpwardsKey = 0x00000023;

/**
* Specifies an angle for changing orientation from state display downwards to
* display downwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownwardsToDisplayUpwardsTimerKey = 0x00000024;

/**
* Specifies an angle for changing orientation from state display downwards to
* display up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownwardsToDisplayUpKey = 0x00000025;

/**
* Specifies an angle for changing orientation from state display downwards to
* display up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownwardsToDisplayUpTimerKey = 0x00000026;

/**
* Specifies an angle for changing orientation from state display downwards to
* display down. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayDownwardsToDisplayDownKey = 0x00000027;

/**
* Specifies an angle for changing orientation from state display downwards to
* display down. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayDownwardsToDisplayDownTimerKey = 0x00000028;

/**
* Specifies an angle for changing orientation from state display left up to 
* display right up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayLeftUpToDisplayRightUpKey = 0x00000029;

/**
* Specifies an angle for changing orientation from state display left up to 
* display right up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayLeftUpToDisplayRightUpTimerKey = 0x0000002A;

/**
* Specifies an angle for changing orientation from state display left up to 
* display up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayLeftUpToDisplayUpKey = 0x0000002B;

/**
* Specifies an angle for changing orientation from state display left up to 
* display up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayLeftUpToDisplayUpTimerKey = 0x0000002C;

/**
* Specifies an angle for changing orientation from state display left up to 
* display upwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayLeftUpToDisplayUpwardsKey = 0x0000002D;

/**
* Specifies an angle for changing orientation from state display left up to 
* display upwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayLeftUpToDisplayUpwardsTimerKey = 0x0000002E;

/**
* Specifies an angle for changing orientation from state display left up to 
* display downwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayLeftUpToDisplayDownwardsKey = 0x0000002F;

/**
* Specifies an angle for changing orientation from state display left up to 
* display downwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayLeftUpToDisplayDownwardsTimerKey = 0x00000030;

/**
* Specifies an angle for changing orientation from state display left up to 
* display down. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayLeftUpToDisplayDownKey = 0x00000031;

/**
* Specifies an angle for changing orientation from state display left up to 
* display down. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayLeftUpToDisplayDownTimerKey = 0x00000032;

/**
* Specifies an angle for changing orientation from state display right up to 
* display up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: -5
*/
const TUint32 KOriStateDisplayRightUpToDisplayUpKey = 0x00000033;

/**
* Specifies an angle for changing orientation from state display right up to 
* display up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 150
*/
const TUint32 KOriStateDisplayRightUpToDisplayUpTimerKey = 0x00000034;

/**
* Specifies an angle for changing orientation from state display right up to 
* display left up. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayRightUpToDisplayLeftUpKey = 0x00000035;

/**
* Specifies an angle for changing orientation from state display right up to 
* display left up. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayRightUpToDisplayLeftUpTimerKey = 0x00000036;

/**
* Specifies an angle for changing orientation from state display right up to 
* display upwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 10
*/
const TUint32 KOriStateDisplayRightUpToDisplayUpwardsKey = 0x00000037;

/**
* Specifies an angle for changing orientation from state display right up to 
* display upwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 50
*/
const TUint32 KOriStateDisplayRightUpToDisplayUpwardsTimerKey = 0x00000038;

/**
* Specifies an angle for changing orientation from state display right up to 
* display downwards. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayRightUpToDisplayDownwardsKey = 0x00000039;

/**
* Specifies an angle for changing orientation from state display right up to 
* display downwards. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayRightUpToDisplayDownwardsTimerKey = 0x0000003A;

/**
* Specifies an angle for changing orientation from state display right up to 
* display down. The value defines how much the midpoint angle (45 degrees)
* has to be exceeded in degrees for the orientation transaction to happen.
* Positive values widens the orientation area and negative values diminishes
* the orientation area. Range of possible values is from 44 to -44 degrees.
* 
* Value is an integer.
*
* Default value: 30
*/
const TUint32 KOriStateDisplayRightUpToDisplayDownKey = 0x0000003B;

/**
* Specifies an angle for changing orientation from state display right up to 
* display down. The value defines how long is the time where orientation
* events are observed before the actual orientation happens. The value is
* defined in milliseconds.
* 
* Value is an integer.
*
* Default value: 300
*/
const TUint32 KOriStateDisplayRightUpToDisplayDownTimerKey = 0x0000003C;

#endif  // INTERNALORIENTATIONCRKEYS_H
            
// End of File
