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
* Description:  General properties definition file.
*
*/


#ifndef SENSRVGENERALPROPERTIES_H
#define SENSRVGENERALPROPERTIES_H

#include <e32base.h>
#include <sensrvtypes.h>

// PROPERTY TEMPLATE

/*
* The following is a template that must be used when defining new properties
*
* Property Name: Name of the property
* Property Type: Defines type of the property (TInt/TReal/TBuf)
* Scope: Defines a property scope. Property can be defined for a channel, for a specific
* item in a channel or for a sensor related to a channel.
* Mandatory: Defines is property mandatory
* Capability: Capabilities needed to set this property
* Description: Description of the property
*/

// CONSTANTS

/**
* Property Name: Data Rate
* Type:         TInt
* Scope:        Channel property
* Mandatory:    Yes
* Capability:   None
* Description:  Current data rate (Hz) of the sensor channel.
*
* A Sensor channel which supports data rates within a given range can use one Data Rate
* property. The range is defined using the property's maximum and minimum value. E.g. A
* sensor channel supports a data range from 10Hz to 100Hz and all values within this range
* are feasible. To implement this use one Data Rate property with a minimum value 10 and a
* maximum value of 100.
*
* A Sensor channel which supports discrete data rates can use the Data Rate property as
* an array. E.g. A sensor channel supports the following data rates 10Hz, 40Hz and 50Hz.
* To implement this four different Data Rate properties are needed. The following table
* shows the content of the four properties, only mandatory attributes are shown.
*
* @code
* property ID     Array index     Value           Min Value       Max Value       Read only
* -----------     -----------     -----           ---------       ---------       ---------
* 0x00000002      -2              1               0               2               EFalse
* 0x00000002      0               10              n/a             n/a             ETrue
* 0x00000002      1               40              n/a             n/a             ETrue
* 0x00000002      2               50              n/a             n/a             ETrue
* @endcode
*
* The first property (first row in table above) is the header for the property array. It
* defines that this property is an array property. The attributes of this property are:
*
* Array index:
* -2 means that the property is an array property
* Value:
* 1 means that current value of the property is defined by the property with an array index of
* 1. The value is 40Hz in this example.
* Min value
* 0 is the start index of the property array
* Max value:
* 2 is the last index of the property array
* Read only:
* EFalse means that the properties current value can be changed,
*
* In this example the possible values of the property are 0, 1, and 2 which corresponds to data
* rates of 10Hz, 40Hz and 50Hz.
*/
const TSensrvPropertyId KSensrvPropIdDataRate = 0x00000002;

/**
* Name:         Power id property
* Type:         TInt
* Scope:        Channel item property
* Mandatory:    No
* Capability:   None
* Description:  Power Property of the sensor
*               Sensor power may either be down (ESensrvPowerPowerDown) or up (ESensrvPowerPowerUp).
*               When sensor power is down, it's channels will also be unavailable.
*               When sensor power is up, any given channel provided by the sensor may either be
*               available or unavailable. The availability property represents channel's availability.
*
* @see          TSensrvPower.
* @see          KSensrvPropIdAvailability
*/
const TSensrvPropertyId KSensrvPropIdPower = 0x00000003;

/**
* Name: Availability
* Type:         TInt.
* Scope:        Channel property
* Mandatory:    Yes
* Capability:   None
* Description:  Indicates if channel is available or not. There are channels
*               supported by a sensor that cannot be opened at the same time.
*               These so called dependant channnels are managed by client priorities.
*               Availability property informs if a channel is available at the moment.
*               Also, unavailable channel can be opened or be listened by client but
*               the channel become functional after availability changes.
*               Availability property values are defined in TSensrvAvailability
*               as ESensrvAvailabilityFalse (not available) and ESensrvAvailabilityTrue
*               (available).
*
* @see          TSensrvAvailability
*/
const TSensrvPropertyId KSensrvPropIdAvailability = 0x00000004;

/**
* Name: Measure Range
* Type: TReal or TInt
* Scope: Channel item property
* Mandatory: No
* Capability: None
* Description: The measure range of the sensor channel.
*
* The channel measure range is defined by the property's maximum and minimum values. The property
* value represents the resolution of the measure range. The measure range property can also be
* an array property.
*
* Example:
*
* An accelerometer xyz axis channel has a range of +/- 2g. Therefore the property would be a single
* property, its type would be ESensrvIntProperty and its value would be 2.
*
* An orientation channel has a range of scalar integers representing the current orientation of
* the mobile:  Undefined, Up, Down, Left, Right, Upward, Downward. Therefore the property would be an
* array property (count of 8 made up of 1 header + 7 scalar values), its type would be
* ESensrvIntProperty and the values of each member of the array would be one of the scalar values
* above. For an example of an array property see KSensrvPropIdDataRate.
*
* A rotation channel has a range of 0-360 degrees. Therefore the property would be an array property
* (count of 3 made up of 1 header + 1 min value + 1 max value), its type would be ESensrvIntProperty
* and the values of each member of the array would be 0 and 360.
*
* The sensor specific channel header file for each channel found in epoc32\include\sensors\channels
* will define the exact content of this property.
*
* @see ESensrvIntProperty
* @see TSensrvArrayIndex
* @see KSensrvPropIdDataRate
*/
const TSensrvPropertyId KSensrvPropIdMeasureRange = 0x00000005;

/**
* Name: Format of the channel data
* Type: TInt, see possible values from TSensrvChannelDataFormat
* Scope: Channel item property
* Mandatory: Yes
* Capability: None
* Description: A format which is used to present a data value.
*
* @see TSensrvChannelDataFormat
*/
const TSensrvPropertyId KSensrvPropIdChannelDataFormat = 0x000000006;

/**
* Name:        Data item scaled range
* Type:        TInt.
* Scope:       Channel item property
* Mandatory:   No
* Capability:  None
* Description: If the channel data is scaled it represents the scale of the returned data.
*
* The property value represents the minimum and maximum values of the scaled range. This property
* should is used when the KSensrvPropIdChannelDataFormat property value is ESensrvFormatScaled.
*
* Example:
*
* An accelerometer xyz axis channel has a scaled range of -128 to 127. Therefore the property would
* be an array property (count of 3 made up of 1 header + 1 min value + 1 max value), its type would
* be ESensrvIntProperty and the values of each member of the array would be -128 and 127.
*
* If the accelerometer xyz axis channel also has a measure range of +/- 2g given by
* KSensrvPropIdMeasureRange property. Then if a data value of 64 is read on one of the axis the g reading
* would be approximately +1g.
*
* Rotation and orientation channels would have KSensrvPropIdChannelDataFormat set to
* ESensrvChannelDataFormatAbsolute. Theerfore this property would not exist for these channels.
*
* The sensor specific channel header file for each channel found in epoc32\include\sensors\channels
* will define the exact content of this property.
*
* @see KSensrvPropIdChannelDataFormat
* @see KSensrvPropIdMeasureRange
*/
const TSensrvPropertyId KSensrvPropIdScaledRange = 0x000000007;

/**
* Name: Accuracy of the channel data
* Type: TReal
* Scope: Channel item property
* Mandatory: No
* Capability: None
* Description: The property value is the accuracy of the channel
*
* Example:
*
* An accelerometer xyz axis channel has an accuracy of +/-2mg. Therefore the property would be a
* single property, its type would be ESensrvIntProperty and its value would be 2.
*
* An orientation channel has a range of scalar integers representing the current orientation of
* the mobile and therefore has no accuracy associated with it. Therefore this property will not
* exist for this channel.
*
* A rotation channel has an accuracy of the nearest 15 degrees. Therefore the property would be
* a single property, its type would be ESensrvIntProperty and its value would be 15.
*
* The sensor specific channel header file for each channel found in epoc32\include\sensors\channels
* will define the exact content of this property.
*
* @see ESensrvIntProperty
*/
const TSensrvPropertyId KSensrvPropIdChannelAccuracy = 0x000000008;

/**
* Name: Channel Data Scale
* Type: TInt
* Scope: Channel item property
* Mandatory: Mandatory when KSensrvPropIdChannelDataFormat is set to ESensrvFormatGeneral
* Capability: None
* Description: The property value is the scale used for the measurement provided by this channel.
*
* The scale can be used as a substitute for prefixing the unit. The scale is expressed as an
* exponent of ten. If no scaling is needed, the scale MUST be zero.
*
* Either the unit or data values can be altered by the scaling factor. If, for example, the
* unit is meter and the scale is -3, either the unit can be interpreted as "mm" or the data
* value can be mulitplied by 10^(scale). i.e For a unit of Meter, a scale of -3 and a value of
* 10 the value canbe interpreted as either 10mm, or 0.01 Meter
*
* If the data type of the channel is int this results in a fixed-point representation in which
* the number of decimal places always remains the same. Using a fixed-point representation can
* have a favorable effect on performance. With floating point data type, the scaling factor is
* often used for convenience. With either very small, or very large values the presentation can
* be more readable when the values are presented in moderate numbers.
*
* Channel data values and the KSensrvMeasureRange property must be represented using the same
* scale.
*
* @see KSensrvPropIdChannelDataFormat
*/
const TSensrvPropertyId KSensrvPropIdChannelScale = 0x000000009;

/**
* Name: Channel item unit
* Type: TInt, see possible values from TSensrvChannelUnit
* Scope: Channel item property
* Mandatory: No
* Capability: None
* Description:  The property value is the unit, in which data values are presented.
*
* The recommended units are listed in TSensrvChannelUnit.
*
* @see TSensrvChannelUnit
*/
const TSensrvPropertyId KSensrvPropIdChannelUnit = 0x0000000010;

/**
* Name:         Sensor model
* Type:         Buffer
* Scope:        Sensor property
* Mandatory:    No
* Capability:   None
* Description: The property value is a sensor model description
*/
const TSensrvPropertyId KSensrvSensorModel = 0x0000000011;

/**
* Name:         Sensor connection type
* Type:         TInt.
* Scope:        Sensor property
* Mandatory:    No
* Capability:   None
* Description: Defines sensor connection type
*
* @see TSensrvConnectionType
*/
const TSensrvPropertyId KSensrvSensorConnectionType = 0x0000000012;

/**
* Name: Sensor description
* Type: Buffer
* Scope: Sensor property
* Mandatory: No
* Capability: None
* Description: The property value is a short description of the sensor
*
* If required this property can be used to hold a long description (>20 characters) of the sensor
* by making it an array property. Each member of the array can hold part of the longer description.
* Any framework client retrieving the property can then concatenate parts to make the full
* description. The sensor specific headers found in epoc32\include\sensors\channels will define
* whether this is a single property or an array property.
*/
const TSensrvPropertyId KSensrvSensorDescription = 0x0000000013;

/**
* - Name:         Compensation type of channel data
* - Type:         TInt, see possible values from TSensorCompensationType 
*                 enumeration declared in sensordatacompensationtypes.h.
* - Scope:        Channel property
* - Mandatory:    No   
* - Capability:   None
* - Description:  Indicates channel data compensation type.
*/
const TSensrvPropertyId KSensrvPropIdChannelDataCompensation = 0x0000000014;

#endif //SENSRVGENERALPROPERTIES_H

// End of File
