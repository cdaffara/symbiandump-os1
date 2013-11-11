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



#ifndef MPOSPSYFIXSTATELISTENER_H
#define MPOSPSYFIXSTATELISTENER_H

#include <e32base.h>
#include "epos_cpospsyfixstatemanager.h"

class TPositionInfoBase;


/**
* The MPosPsyFixStateListener is the interface to receive PSY fix state 
* change event.
*/
class MPosPsyFixStateListener
    {
    public: 
              
        /**
        * Notify the listener that fix state of a PSY is changed.
        *
        * @param aModuleId The module ID of the PSY
        * @param aFixState The new fix state of the PSY.
        */
        virtual void PsyFixStateChanged( 
            TPositionModuleId aModuleId,
            CPosPsyFixStateManager::TPsyFixState aFixState ) = 0; 

    };

#endif      // MPOSPSYFIXSTATELISTENER_H  
            
// End of File
