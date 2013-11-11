/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TTCEventAccelerometer class declaration.
*
*/



#ifndef TCSEVENTACCELEROMETER_H
#define TCSEVENTACCELEROMETER_H

// INCLUDES
#include <sensrvchanneldatatypes.h>

#include "tcevent.h"

// FORWARD DECLARATIONS

// DATA TYPES

/**
*  Accelerometer event. Accelrometer event contains the new
*  accelerometer XYZ axis data.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCEventAccelerometer ): public TTCEvent
    {
    public:
    
        /**
        * Default C++ constructor.
        *
        * @since S60 5.0
        * @param aId
        * @return
        */
        TTCEventAccelerometer( const TSensrvAccelerometerAxisData& aData );
        
    public:
    
        /** Accelerometer data */
        TSensrvAccelerometerAxisData iData;
    };

#endif // TCSEVENTACCELEROMETER_H

// End of File
