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



#ifndef EPOS_MPOSMODULESTATUSLISTENER_H
#define EPOS_MPOSMODULESTATUSLISTENER_H

// FORWARD DECLARATION
class TPositionQuality;

// CLASS DECLARATION

/**
* Listener interface to receive module status change event.
*
*/
class MPosModuleStatusListener
    {
    public:  
        /**
        * Called when a change has been detected in location status.
        *
        * To be able to receive this notification, the observer object must
        * first be registered by calling 
        * MPosModuleStatusManager::AddListenerL() .
        *
        * @param[out] aStatusEvent contains, on successful completion, information regarding
        * the status event and the new status of of the specified positioning module.
        */
        virtual void HandleModuleStatusChanged(
            TPositionModuleStatusEventBase& aStatusEvent ) = 0;
    };  

#endif  // EPOS_MPOSMODULESTATUSLISTENER_H

// End of File
