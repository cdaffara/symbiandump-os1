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
* Description:  TTCEventMagnetometer class declaration.
*
*/



#ifndef TCSEVENTMAGNETOMETERDATA_H
#define TCSEVENTMAGNETOMETERDATA_H

// INCLUDES
#include <sensrvmagnetometersensor.h>

#include "tcevent.h"

// FORWARD DECLARATIONS

// DATA TYPES

/**
*  Magnetometer event. Magnetometer event contains the new
*  magnetometer XYZ axis data.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCEventMagnetometer ): public TTCEvent
    {
    public:
    
        /**
        * Default C++ constructor.
        *
        * @param aId
        */
        TTCEventMagnetometer( const TSensrvMagnetometerAxisData& aData );
        
    public:
    
        /** Magnetometer data */
        TSensrvMagnetometerAxisData iData;
    };

#endif // TCSEVENTMAGNETOMETERDATA_H

// End of File
