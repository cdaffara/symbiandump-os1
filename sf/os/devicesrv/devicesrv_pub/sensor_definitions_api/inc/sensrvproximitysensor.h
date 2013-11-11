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



#ifndef SENSRVPROXIMITYSENSOR_H
#define SENSRVPROXIMITYSENSOR_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 

// PROXIMITY RELATED CHANNELS

/**
* - Name: Proximity data channel
* - Type: Event
* - Datatype: TSensrvProximityMonitorData
* - Description: Proximity status 
*/
const TSensrvChannelTypeId KSensrvChannelTypeIdProximityMonitor = 0x2000E585;

// PROXIMITY RELATED DATATYPES

/**
* Proximity monitoring data type
*/
class TSensrvProximityData
	{
public:
	/**
	* Channel data type Id number
	*/ 
	static const TSensrvChannelDataTypeId KDataTypeId = 0x2000E585;

    /**
    * Channel data type index numbers
    */ 
    enum TSensrvProximityDataIndexes
        {
        ETimeStamp = 0,
        EState
        };

    /**
    * Possible values for proximito state
    */ 
    enum TProximityState
        {
        EProximityUndefined = 0,
        EProximityIndiscernible,
        EProximityDiscernible
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
    * - Item name: Proximity state
    * - Item Index: 0
    * - Conditions: None
    * - Description: -
    */ 
    TProximityState iProximityState;

};

#endif //SENSRVPROXIMITYSENSOR_H

// End of File
