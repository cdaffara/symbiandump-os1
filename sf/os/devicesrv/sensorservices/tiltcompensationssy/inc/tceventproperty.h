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
* Description:  TTCEventProperty class declaration.
*
*/



#ifndef TCSEVENTPROPERTY_H
#define TCSEVENTPROPERTY_H

// INCLUDES
#include <sensrvproperty.h>

#include "tcevent.h"

/**
*  Property event. Property event contains the updated property.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCEventProperty ): public TTCEvent
    {
    public:
    
        /**
        * Default C++ constructor.
        *
        * @param aId
        */
        TTCEventProperty( const TSensrvProperty& aProperty );
        
    public:
    
        /** Property associated to this event */
        TSensrvProperty iProperty;
    };

#endif // TCSEVENTPROPERTY_H

// End of File
