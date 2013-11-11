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



#ifndef MAGNETICNORTHSENSOR_H
#define MAGNETICNORTHSENSOR_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 

// TILT RELATED CHANNELS

/**
* - Name:          Magnetic north channel data
* - Type:          Event
* - Datatype:      TSensrvMagneticNorthData
* - Description:   The magnetic north data describes the angle between the
*                  device and magnetic north.
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdMagneticNorthData = 0x2000BEDF;


// TILT RELATED DATATYPES

/**
* Tilt angel data type
*/
class TSensrvMagneticNorthData
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x2000BEDF;

    /**
    * Channel data type index numbers
    */
    enum TSensrvMagneticNorthDataIndexes
        {
        ETimeStamp = 0,
        EAngleFromMagneticNorth
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
    * - Item name:   Tilt angle
    * - Item Index:  1
    * - Conditions:  Single limit and range 
    * - Description: The angle between the magnetic north and the device heading.
    */
    TInt iAngleFromMagneticNorth;  
    };

#endif //MAGNETICNORTHSENSOR_H

// End of File
