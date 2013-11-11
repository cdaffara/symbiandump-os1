/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSFIXSTATETIMERLISTENER_H
#define MPOSFIXSTATETIMERLISTENER_H

#include <e32base.h>

class TPositionInfoBase;

/**
* The MPosFixStateTimerListener is 
*/
class MPosFixStateTimerListener
    {
    public: 
              
        /**
        * Notify the listener that timeshift timer timeout.
        *
        * @param aPsyId The ID of the PSY
        */
        virtual void TimeshiftTimerTimeout( 
            TPositionModuleId aPsyId ) = 0;
            
        /**
        * Notify the listener that unknown timer timeout.
        *
        * @param aPsyId The ID of the PSY
        */
        virtual void UnknownTimerTimeout(
            TPositionModuleId aPsyId ) = 0;

    };

#endif      // MPOSFIXSTATETIMERLISTENER_H  
            
// End of File
