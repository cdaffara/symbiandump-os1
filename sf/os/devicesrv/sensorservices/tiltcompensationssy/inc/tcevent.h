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
* Description:  TTCEventData class declaration.
*
*/



#ifndef TCSEVENT_H
#define TCSEVENT_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

// DATA TYPES

/**
* Tilt SSY events.
*/
enum TTCEventId
    {
    EEventIdNull,
    EEventIdHandleOpenChannel,
    EEventIdHandleCloseChannel,
    EEventIdHandleStartChannelData,
    EEventIdHandleStopChannelData,
    EEventIdHandleForceBufferFull,
    EEventIdHandleAccelerometer,
    EEventIdHandleMagnetometer,
    EEventIdHandlePropertyChanged,
    EEventIdHandleTimerExpriredSend,
    EEventIdLast
    };
    
// CLASS DECLARATION

/**
*  Event base class.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( TTCEvent )
    {
    public:
    
    public:
    
        /**
        * Returns event ID.
        * 
        * @return TTCEventId
        */
        TTCEventId Id() const;
            
    protected:
    
        /**
        * Default C++ constructor.
        *
        * @param aId
        */
        TTCEvent( TTCEventId aId );
        
    private: // Data
    
        /** Event ID */
        TTCEventId iId;
    };

#endif // TCSEVENT_H

// End of File
