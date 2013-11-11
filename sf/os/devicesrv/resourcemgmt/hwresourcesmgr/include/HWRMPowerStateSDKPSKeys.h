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
* The API provides the P&S power state keys that are updated by the HW Resource 
* Manager Server. These keys provide up-to-date information on the charging 
* status, battery level and battery state of the device. Any application may 
* subscribe to these P&S keys that are updated to receive notifications of 
* any change in state.
* This interface relies on the Publish and Subscribe (P&S) interface provided
* by Symbian. P&S is used by HW Resource Manager to globally publish the power
* state information.
* The P&D UID is #KPSUidHWRMPowerState and following keys are provided:
* - #KHWRMBatteryLevel
* - #KHWRMBatteryStatus
* - #KHWRMChargingStatus
*
*/




/**
 @file hwrmpowerstatesdkpskeys.h
 @publishedAll
 @released
*/

#ifndef HWRMPOWERSTATESDKPSKEYS_H
#define HWRMPOWERSTATESDKPSKEYS_H

#include <e32std.h>

/**
* P&S UID for all power state information keys provided by this API.
*
* @publishedAll
* @released
*/
const TUid KPSUidHWRMPowerState = { 0x10205041 }; 


/**
* P&S key to represent the battery level of the device. It can also be related
* to the number of battery bars displayed. This property is updated only when
* battery level changes. Valid values are defined by the enum EPSHWRMBatteryLevel
* and range from -1 to 7. <br>
* If an error has occurred or the battery level has not yet been initialized, 
* the enumeration value of #EBatteryLevelUnknown is used. In all other cases 
* the battery level is used with level 0 being the lowest (battery empty) and 
* level 7 the highest (battery full).
*
* @see EPSHWRMBatteryLevel
*
* @publishedAll
* @released
*/
const TUint32 KHWRMBatteryLevel = 0x00000001;

/**
* Battery level of device. Can be related to the number of battery bars 
* displayed by the device.
*
* @publishedAll
* @released
*/
enum EPSHWRMBatteryLevel
    {
    /**
    Uninitialized or some other error
    */
    EBatteryLevelUnknown        = -1,   
    /**
    Battery level 0. The lowest battery level.
    */
    EBatteryLevelLevel0         = 0,   
    /**
    Battery level 1.
    */
    EBatteryLevelLevel1         = 1,
    /**
    Battery level 2.
    */
    EBatteryLevelLevel2         = 2,
    /**
    Battery level 3.
    */
    EBatteryLevelLevel3         = 3,
    /**
    Battery level 4.
    */
    EBatteryLevelLevel4         = 4,
    /**
    Battery level 5.
    */
    EBatteryLevelLevel5         = 5,
    /**
    Battery level 6.
    */
    EBatteryLevelLevel6         = 6,
    /**
    Battery level 7. The highest battery level.
    */
    EBatteryLevelLevel7         = 7     
    };


/**
* Battery status of device. 
* This property may be updated to same status as previously, to indicate
* the note in question needs to be shown again.
*
* #EBatteryStatusLow update comes approximately ten times before 
* battery is completely empty and it comes at approximately
* 10 minute intervals. If phone is in-call mode, update comes at 
* approximately 1 minute intervals.
*
* #EBatteryStatusEmpty comes slightly before phone must power down 
* automatically because of insufficient battery power.  
*
* @see EPSHWRMBatteryStatus
*
* @publishedAll
* @released
*/
const TUint32 KHWRMBatteryStatus = 0x00000002;

/**
 * Battery status of device. 
 *
 * This Property is a replacement for the existing property KHWRMBatteryStatus.
 * It is strictly recommended to use the new property by any up-coming applications.
 * Legacy property, KHWRMBatteryStatus, will be deprecated in the very next release.
 *
 * @see EPSHWRMBatteryStatus
 *
 * @publishedAll
 * @released
 */
const TUint32 KHWRMExtendedBatteryStatus = 0x00000004;

/**
* Battery status of device.
*
* @publishedAll
* @released
*/
enum EPSHWRMBatteryStatus
    {
    /**
		Uninitialized or some other error e.g.server not intialized
    */
		EBatteryStatusUnknown        							= -1,
		/**
		* e.g. Chemistry not supported like LiFePo or general battery not supported error
		* Show "charging not allowed" note to user
		*/
		EBatteryStatusNotSupported								= -2,
		/**
		* e.g. Pirated Battery with wrong DBI Chip / DBI authentication failed
		* Show "charging not allowed" note to user
		*/
		EBatteryStatusDbiAuthFailed        				= -3,    
    /**
    This can also be used during charging
    */
    EBatteryStatusOk             = 0,  
    /**
    Show note to user "Battery low"
    */
    EBatteryStatusLow            = 1,  
    /**
    Show "recharge battery" note to user
    */
    EBatteryStatusEmpty          = 2   
    };

/**
* Charging status of device.
* This property is updated only when charging status changes.
*
* @see EPSHWRMChargingStatus
*
* @publishedAll
* @released
*/
const TUint32 KHWRMChargingStatus = 0x00000003;

/**
 * Charging status of device.
 *
 * This Property is a replacement for the existing property KHWRMChargingStatus.
 * It is strictly recommended to use the new property by any up-coming applications.
 * Legacy property, KHWRMChargingStatus, will be deprecated in the very next release.
 *
 * @see EPSHWRMChargingStatus
 *
 * @publishedAll
 * @released
 */
const TUint32 KHWRMExtendedChargingStatus = 0x00000005;

/**
* Charging status of device.
*
* @publishedAll
* @released
*/
enum EPSHWRMChargingStatus
    {
    /**
    Some error has occurred when charger is connected or charging.    
    */
    EChargingStatusError                        = -1,
    /**
     * Illegal charger is connected, device cannot charge
     */
    EChargingStatusIllegalChargerError          = -2,
    /**
     * Charger is under voltage, device cannot charge
     */
    EChargingStatusChargerUnderVoltageError     = -3,
    /**
     * Charger is over voltage, device cannot charge
     */
    EChargingStatusChargerOverVoltageError      = -4,
    /**
     * Wait for battery temperature to normalize before charging to proceed
     */
    EChargingStatusTempTooHotError              = -5,
    /**
     * Wait for battery temperature to normalize for charging
     */
    EChargingStatusTempTooColdError             = -6,
    /**
    Charger not connected/uninitialized
    */
    EChargingStatusNotConnected       = 0,  
    /**
    Device is charging
    */
    EChargingStatusCharging           = 1,  
    /**
    Charger is connected, device not charging
    */
    EChargingStatusNotCharging        = 2,  
    /**
    Charging almost completed
    */
    EChargingStatusAlmostComplete     = 3,  
    /**
    Charging completed
    */
    EChargingStatusChargingComplete   = 4,  
    /**
    Charging continued after brief interruption
    */
    EChargingStatusChargingContinued  = 5   
    };


#endif      // HWRMPOWERSTATESDKPSKEYS_H

// End of File
