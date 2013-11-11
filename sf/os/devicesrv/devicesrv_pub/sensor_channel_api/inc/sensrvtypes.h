/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Type definitions file for Sensor Server
*
*/



#ifndef SENSRVTYPES_H
#define SENSRVTYPES_H

#include <e32base.h>
#include <sensrvproperty.h>

// Forward declare channel info
class TSensrvChannelInfo;

/**
* Max length of the vendor Id
*
* @see TSensrvChannelInfo
*/
const TInt KSensrvVendorIdLength = 16;

/**
* Max length of the location string
*
* @see TSensrvChannelInfo
*/
const TInt KSensrvLocationLength = 16;

/**
* Item index for properties that are not data item specific
*
* @see TSensrvProperty
*/
const TInt KSensrvItemIndexNone = -1;

// DATA TYPES

/**
* Type definition for a channel id used by TSensrvChannelInfo. Its value is assigned by the Sensor
* Server and uniquely identifies a channel for the client session. When the client session has ended
* the channel id is no longer valid.
*
* @see TSensrvChannelInfo
* @see CSsyControl
* @see MSsyCallback
* @see MSsyChannelDataProvider
* @see MSsyPropertyProvider
*/
typedef TUint32 TSensrvChannelId;

/**
* Type definition for a channel type used by TSensrvChannelInfo. This id identifies the type of channel
* * that the object represents and therefore the content of the channel. This value is a uid and is unique
* among all channels. See sensor channel specific header files. A client can use this value as one of the
* search criteria when finding channels.
*
* @see TSensrvChannelInfo
*/
typedef TUint32 TSensrvChannelTypeId;

/**
* Typedef for datatype Id used by TSensrvChannelInfo. This identifies the data type for the data that the
* channel provides. This value is a uid and is unique among all channels. See sensor channel specific header
* files.
*
* @see TSensrvChannelInfo
*/
typedef TUint32 TSensrvChannelDataTypeId;


/**
* Undefined Channel id for use when finding channels
*
* @see CSensrvChannelFinder
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdUndefined = 0x00000000;

/**
* RArray based channel Id list
*
* @see MSsyCallback
* @see MSsyPropertyProvider
*/
typedef RArray<TSensrvChannelId> RSensrvChannelList;

/**
* RArray based property list
*
* @see CSensrvChannel
* @see MSsyPropertyProvider
*/
typedef RArray<TSensrvProperty> RSensrvPropertyList;

/**
* RArray based channel info list
*
* @see CSensrvChannelFinder
* @see MSsyCallback
*/
typedef RArray<TSensrvChannelInfo> RSensrvChannelInfoList;

/**
* Indication of whether a call to CSensrvChannel::SetPropertyL() is likely to succeed
*
* @see CSensrvChannel
* @see MSensrvPropertyListener
*/
enum TSetPropertySuccessIndicator
    {
    ESetPropertyIndicationUnknown = 0,
    /**
    * Setting of a property is certain to succeed because client has the highest priority
    * of all clients that have open channels on that sensor device
    */
    ESetPropertyIndicationAvailable,
    /**
    * Setting of a property may not succeed because:
    * 1. Client has the highest priority on its own channel however there are multiple
    * clients with the same priority and control is granted on a first come first served
    * basis.
    * 2. Client has highest priority on its own channel however there are higher priority
    * clients on other channels of the device that may be affected by the setting, in which
    * case it will not be allowed.
    * 3. Client has highest priority on its channel however there are same priority clients
    * on other channels of the device that may be affected by the setting, in which case it
    * will not be allowed
    */
    ESetPropertyIndicationPossible,
    /**
    * Setting of a property will not succeed because:
    * 1. There are clients on the same channel with higher priority
    * 2. A client of the same priority is already setting properties on channel.
    */
    ESetPropertyIndicationUnavailable
    };

/**
* The quantity of channel values. Defines the quantity the channel is measuring.
* Licensee defined values must be between ESensrvQuantityLicenseeBase and
* ESensrvQuantityLicenseeEnd
*
* @see TSensrvChannelInfo
*/
enum TSensrvQuantity
    {
    /** Channel doesn't provide quantity information*/
    ESensrvQuantityNotUsed = -1,
    /** Quantity is not defined */
    ESensrvQuantityNotdefined = 0,
    /** Channel measures acceleration */
    ESensrvQuantityAcceleration = 10,
    /** Channel measures tapping events */
    ESensrvQuantityTapping = 11,
    /** Channel measures phone orientation */
    ESensrvQuantityOrientation = 12,
    /** Channel measures phone rotation */
    ESensrvQuantityRotation = 13,
    /** Channel measures phone direction */
    ESensrvQuantityMagnetic = 14,
    /** Channel measures degrees */
    ESensrvQuantityAngle = 15,
    /** Channel measures phone proximity events*/
    ESensrvQuantityProximity = 16,
    /** Start of licensee quantity range definitions */
    ESensrvQuantityLicenseeBase = 8192,
    /** End of licensee quantity range definitions */
    ESensrvQuantityLicenseeEnd = 12287
    };

/**
* The context type of a sensor.
* Licensee defined values must be between ESensrvContextTypeLicenseeBase and
* ESensrvContextTypeLicenseeEnd
*
* @see TSensrvChannelInfo
*/
enum TSensrvContextType
    {
    /** Channel doesn't provide sensor context type information */
    ESensrvContextTypeNotUsed = -1,
    /** Context type is not defined */
    ESensrvContextTypeNotDefined = 0,
    /**
    * Sensor is measuring a physical quantity of the phones environment e.g. pressure, temperature,
    * sound intensity, humidity
    */
    ESensrvContextTypeAmbient = 1,
    /**
    * Sensor is measuring a physical quantity that the phone itself is undergoing e.g. acceleration,
    * rotation, orientation
    */
    ESensrvContextTypeDevice = 2,
    /**
    * Sensor is measuring a physical quantity that is user stimulated e.g. body temperature, body mass,
    * heart rate
    */
    ESensrvContextTypeUser = 3,
    /** Start of licensee context range definitions */
    ESensrvContextTypeLicenseeBase = 8192,
    /** End of licensee context range definitions */
    ESensrvContextTypeLicenseeEnd = 12287
    };

/**
* The connection type of a sensor.
* Licensee defined values must be between ESensrvConnectionTypeLicenseeBase and
* ESensrvConnectionTypeLicenseeEnd
*
* @see KSensrvSensorConnectionType
*/
enum TSensrvConnectionType
    {
    /** Connection type is not defined */
    ESensrvConnectionTypeNotDefined = 0,
    /** Sensor is embedded in the phone */
    ESensrvConnectionTypeEmbedded,
    /** Sensor is attached to phone by wire */
    ESensrvConnectionTypeWired,
    /** Sensor is attached to phone wirelessly */
    ESensrvConnectionTypeWireless,
    /** Start of licensee Connection type range definitions */
    ESensrvConnectionTypeLicenseeBase = 8192,
    /** End of licensee Connection type range definitions */
    ESensrvConnectionTypeLicenseeEnd = 12287
    };

/**
* The unit of the data measured by the sensor channel
* Licensee defined values must be between ESensrvChannelUnitLicenseeBase and
* ESensrvChannelUnitLicenseeEnd
*
* @see KSensrvPropIdChannelUnit
*/
enum TSensrvChannelUnit
    {
    /** Channel Unit is not defined */
    ESensrvChannelUnitNotDefined = 0,
    /** Acceleration, meter per square second (m/s^2) */
    ESensevChannelUnitAcceleration = 10,
    /** Acceleration, gravitational constant (G) */
    ESensrvChannelUnitGravityConstant = 11,
    /** ESensrvChannelUnitMagneticFluxDensity, magnetic field density, Tesla (T) */
    ESensrvChannelUnitMagneticFluxDensity = 12,
    /** Start of licensee Channel Unit range definitions */
    ESensrvChannelUnitLicenseeBase = 8192,
    /** End of licensee Channel Unit range definitions */
    ESensrvChannelUnitLicenseeEnd = 12287
    };

/**
* The format of the data measured by the sensor channel
* Licensee defined values must be between ESensrvChannelDataFormatLicenseeBase and
* ESensrvChannelDataFormatLicenseeEnd
*
* @code
* Scaled format example:
* Measure range for the accelerometer, KSensrvPropIdMeasureRange: -2g to 2g.
* KSensrvPropIdScaledRange defines following values:
* Range: Min: -127  Max: 127
*
* Example values for the data item and their absolute values:
* Data item: -64 = > -64/127 * 2g = -1.01g
* Data item:  32 = > 32/127 * 2g = 0.51g
* Data item: 127 = > 127/127 * 2g = 2g
* @endcode
*
* @see KSensrvPropIdChannelDataFormat
*/
enum TSensrvChannelDataFormat
    {
    /** Data Fomat is not defined */
    ESensrvChannelDataFormatNotDefined = 0,
    /** Value of the data item represents actual value of the measured quantity */
    ESensrvChannelDataFormatAbsolute,
    /**
    * Value of the data item represents a relative value which is scaled between the maximum
    * and minimum values of the measured quantity
    */
    ESensrvChannelDataFormatScaled,
    /** Start of licensee Data Format range definitions */
    ESensrvChannelDataFormatLicenseeBase = 8192,
    /** End of licensee Data Format range definitions */
    ESensrvChannelDataFormatLicenseeEnd = 12287
    };

/**
* The error code received by a sensor server listener
*
* @see MSensrvChannelConditionListener
* @see MSensrvChannelListener
* @see MSensrvDataListener
* @see MSensrvPropertyListener
*/
enum TSensrvErrorSeverity
    {
    /** Error Severity is not defined */
    ESensrvErrorSeverityNotDefined = 0,
    /** The channel has a temporary failure but listening has been successfully continued */
    ESensrvErrorSeverityMinor,
    /** The channel has a fatal error and the channel was closed  */
    ESensrvErrorSeverityFatal
    };

/**
* The channel change is detected by the Channel Listener
*
* @see MSensrvChannelListener
*/
enum TSensrvChannelChangeType
    {
    /** Channel Change not defined */
    ESensrvChannelChangeTypeNotDefined = 0,
    /** Channel was removed */
    ESensrvChannelChangeTypeRemoved,
    /** Channel was added */
    ESensrvChannelChangeTypeAdded
    };

/**
* The TSensrvPower represents sensor power state
*
* Possible values:
* - ESensrvPowerPowerDown, sensor is currently powered down
* - ESensrvPowerPowerUp, sensor currently powered up
*/
enum TSensrvPower
	{
	/** Sensor is powered down */
	ESensrvPowerPowerDown = 0,
	/** Sensor is powered up */
	ESensrvPowerPowerUp
	};

/**
* The TSensrvAvailability represents sensor channel availability
*
* Possible values:
* - ESensrvAvailabilityFalse, the channel is not available, which is caused by
*   either sensor being powered down, or a dependency with another channel
*   prevents the channel for being available
* - ESensrvAvailabilityTrue, the channel is available
*/
enum TSensrvAvailability
	{
    /** Channel not available */
	ESensrvAvailabilityFalse = 0,
	/** Channel available */
	ESensrvAvailabilityTrue
	};


#endif //SENSRVTYPES_H

// End of File
