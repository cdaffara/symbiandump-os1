/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EPOS_MPOSMODULESETTINGSLISTENER_H
#define EPOS_MPOSMODULESETTINGSLISTENER_H

#include <lbs/epos_mposmodulesobserver.h>

/**
* Listener interface to receive module settings change event.
*
*/
class MPosModuleSettingsListener
    {
    public:  
        /**
        * Called when a change has been detected in location settings.
        *
        * To be able to receive this notification, the observer object must
        * first be registered by calling
        * MPosModuleSettingsManager::AddListenerL() .
        *
        * @param aEvent Information about the location settings event.
        * @see TPosModulesEvent
        */
        virtual void HandleModuleSettingsChangedL(
            TPosModulesEvent aEvent ) = 0;

    };  

#endif  // EPOS_MPOSMODULESETTINGSLISTENER_H

// End of File
