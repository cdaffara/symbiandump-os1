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
* Description:  
*
*/


#ifndef SENSRVTESTCHANNELDATATYPES_H
#define SENSRVTESTCHANNELDATATYPES_H

#include <e32base.h>
#include <sensrvtypes.h> 



const TInt KSensrvTestEventChannelType(0x1efaefed);

/**
* Test event data type
*/
class TSensrvTestEventData 
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x1efaefed;

    /**
    * Channel data type index numbers
    */
    const class Index
        {
    public:  
        enum TSensrvTestEventData
          {
          iTimeStampFromEventPlugin = 0,
          iTimeStampFromAccelerometerPlugin
          };    
        };

public:
    /**
    * - Item name:   Sampling time.
    * - Item Index:  0
    * - Description: Timestamp for a sample.
    */   
    TTime iTimeStampFromEventPlugin;    
    
    /**
    * - Item name:   Accelerometer Sampling time.
    * - Item Index:  0
    * - Description: Timestamp for a Accelerometer sample.
    */   
    TTime iTimeStampFromAccelerometerPlugin;  
    };


#endif //SENSRVTESTCHANNELDATATYPES_H

// End of File


