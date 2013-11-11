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
* Description:  Channel data types definitions
*
*/



#ifndef SENSRVMAGNETOMETERSENSOR_H
#define SENSRVMAGNETOMETERSENSOR_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 

// MAGNETOMETER RELATED CHANNELS 

/**
* - Name:          Magnetometer XYZ-axis data channel type
* - Type:          Rawdata
* - Datatype:      TSensrvMagnetometerAxisData
* - Description:   Magnetometer x-, y-, z-axis data  
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdMagnetometerXYZAxisData = 0x2000BEE0;


// MAGNETOMETER RELATED PROPERTIES

/**
* - Name:        Name of the property
* - Type:        Defines type of the property (TInt/TReal/TBuf)
* - Scope:       Defines a property scope. Property can be defined for a channel, 
*                for a specific item in a channel or for a server related to 
*                channel.
* - Mandatory:   Defines is property mandatory
* - Capability:  Capabilities needed to set this property
* - Description: Description of the property
*
*/

/**
* - Name:         Auto calibration active
* - Type:         TInt 
* - Scope:        Channel item property
* - Mandatory:    No
* - Capability:   None
* - Description:  Indicates is auto calibration active.
*                 Value is one if calibration is activated, zero otherwise.
*/
const TSensrvPropertyId KSensrvPropAutoCalibrationActive = 0x00001006;

/**
* - Name:         Calibration status
* - Type:         TInt 
* - Scope:        Channel item property
* - Mandatory:    No
* - Capability:   None
* - Description:  Indicates the calibration level.
*                 Calibration level scales between minimum and maximum value.
*                 Maximum indicates that calibration level is at its best
*                 level. Minimum indicates that calibration is undefined.
*                 Possible values: 0=Not calibrated, 1=Low, 2=Moderate, 3=High accuracy.
*/
const TSensrvPropertyId KSensrvPropCalibrationLevel = 0x00001007;

/**
* - Name:         Auto calibration supported
* - Type:         TInt 
* - Scope:        Channel item property
* - Mandatory:    No
* - Capability:   None
* - Description:  Scalar – Support for Calibration(1-Calibration Supported,
*									0-Calibration not supported)
*/

const TSensrvPropertyId KSensrvPropIdAutoCalibrationSupported = 0x00001008;

// MAGNETOMETER RELATED DATATYPES

/**
* Magnetometer axis data type
*/
class TSensrvMagnetometerAxisData 
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x2000BEE0;

    /**
    * Channel data type index numbers
    */  
    enum TSensrvMagnetometerAxisDataIndexes
        {
        ETimeStamp = 0,
        EAxisX,
        EAxisY,
        EAxisZ,
        EAxisXCalibrated,
        EAxisYCalibrated,
        EAxisZCalibrated
        };

public:

    /**
    * - Item name:   Sampling time.
    * - Item Index:  0
    * - Conditions:  None
    * - Description: Timestamp for a sample.
    */   
    TTime iTimeStamp;    

    /**
    * - Item name:   Magnetometer x-axis 
    * - Item Index:  1
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from x-axis 
    */
    TInt iAxisXRaw;

    /**
    * - Item name:   Magnetometer y-axis 
    * - Item Index:  2
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from y-axis 
    */ 
    TInt iAxisYRaw;

    /**
    * - Item name:   Magnetometer z-axis 
    * - Item Index:  3
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from z-axis 
    */
    TInt iAxisZRaw;
    
    /**
    * - Item name:   Magnetometer x-axis 
    * - Item Index:  1
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from x-axis 
    */
    TInt iAxisXCalibrated;

    /**
    * - Item name:   Magnetometer y-axis 
    * - Item Index:  2
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from y-axis 
    */ 
    TInt iAxisYCalibrated;

    /**
    * - Item name:   Magnetometer z-axis 
    * - Item Index:  3
    * - Conditions:  Single limit and range 
    * - Description: Magnetometer values from z-axis 
    */
    TInt iAxisZCalibrated;
    };

#endif //SENSRVMAGNETOMETERSENSOR_H

// End of File
