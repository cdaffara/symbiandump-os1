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



#ifndef SENSRVILLUMINATIONSENSOR_H
#define SENSRVILLUMINATIONSENSOR_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 


// ILLUMINATION RELATED CHANNELS

/**
* - Name:          Ambient light event channel type
* - Type:          Event
* - Datatype:      TSensrvAmbientLightData
* - Description:   Ambient light events
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdAmbientLightData = 0x2000BF16;


// ILLUMINATION RELATED DATATYPES

class TSensrvAmbientLightData
    {
public:

    /**
    * Brighness constants for ambien light sensor data channel.
    */
    static const TUint8 KAmbientLightVeryDark 	= 0;
    static const TUint8 KAmbientLightDark 	    = 20;
    static const TUint8 KAmbientLightTwilight 	= 40;
    static const TUint8 KAmbientLightLight      = 60;
    static const TUint8 KAmbientLightBright     = 80;
    static const TUint8 KAmbientLightSunny      = 100;

    /**
    * Channel data type Id number
    */
    static const TSensrvChannelDataTypeId KDataTypeId = 0x2000BF16;

	/**
    * Channel data type index numbers
    */
	enum TSensrvAmbientLightDataIndexes
        {
        ETimeStamp = 0,
        EAmbientLight
        };

public:

    /**
    * - Item name:   Sampling time
    * - Item Index:  0
    * - Description: Timestamp for a sample
    */
    TTime iTimeStamp;

    /**
    * - Item name:   Ambient light
    * - Item Index:  1
    * - Description: Percentage value (0..100) of Ambient light sensor's brightness.
    */
    TUint8 iAmbientLight;
    };


#endif //SENSRVILLUMINATIONSENSOR_H

// End of File
