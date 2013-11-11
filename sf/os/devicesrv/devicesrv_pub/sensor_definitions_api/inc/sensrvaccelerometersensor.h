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



#ifndef SENSRVACCELEROMETERSENSOR_H
#define SENSRVACCELEROMETERSENSOR_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 

// ACCELEROMETER RELATED CHANNELS 

/**
* - Name:          Accelerometer XYZ-axis data channel type
* - Type:          Rawdata
* - Datatype:      TSensrvAccelerometerAxisData
* - Description:   Accelerometer x-, y-, z-axis data  
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdAccelerometerXYZAxisData = 0x1020507E;

// ACCELEROMETER RELATED PROPERTIES

/**
* - Name:         Axis active
* - Type:         TInt 
* - Scope:        Channel item property
* - Mandatory:    No
* - Capability:   None
* - Description:  Indicates is sensor axis activated.
*                 Value is one if the axis is activated, zero otherwise.
*/
const TSensrvPropertyId KSensrvPropIdAxisActive = 0x00001001;

// ACCELEROMETER RELATED DATATYPES

/**
* Accelerometer axis data type
*/
class TSensrvAccelerometerAxisData 
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x1020507E;

    /**
     * Channel data type index numbers
     */
	enum TSensrvAccelerometerAxisDataIndexes
          {
           ETimeStamp = 0,
           EAxisX,
           EAxisY,
           EAxisZ
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
    * - Item name:   Accelerometer x-axis 
    * - Item Index:  1
    * - Conditions:  Single limit and range 
    * - Description: Accelerometer values from x-axis 
    */
    TInt iAxisX;  

    /**
    * - Item name:   Accelerometer y-axis 
    * - Item Index:  2
    * - Conditions:  Single limit and range 
    * - Description: Accelerometer values from y-axis 
    */ 
    TInt iAxisY;  

    /**
    * - Item name:   Accelerometer z-axis 
    * - Item Index:  3
    * - Conditions:  Single limit and range 
    * - Description: Accelerometer values from z-axis 
    */
    TInt iAxisZ;  
    };

#endif //SENSRVACCELEROMETERSENSOR_H

// End of File
