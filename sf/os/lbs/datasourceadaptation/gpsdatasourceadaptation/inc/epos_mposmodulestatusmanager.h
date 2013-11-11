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



#ifndef EPOS_MPOSMODULESTATUSMANAGER_H
#define EPOS_MPOSMODULESTATUSMANAGER_H

#include <lbs.h>

class MPosModuleStatusListener;

/**
*  This is an interface through which a PSY can get module settings and listen
*  to module settings change.
* 
*/
class MPosModuleStatusManager
    {
    public:
        /**
        * Add a listener to get the module status change event. 
        *
        * The listener must be removed when the client does not want to get
        * module status change event any more.
        *
        * @param aListener The reference to listener class.
        */
        virtual void AddListenerL(
            MPosModuleStatusListener& aListener ) = 0;

        /**
        * Remove a listener of module status chagne event.
        *
        * If the listener has not been added, this function does nothing.
        *
        * @param aListener The reference to the listener class.
        */
        virtual void RemoveListener(
            MPosModuleStatusListener& aListener) = 0;

        /**
        * Obtains information about the specified positioning module.
        *
        * @param[out] aStatus contains, on successful completion, the status of
        * the specified positioning module
        * @param aModuleId the unique identifier (UID) of a positioning module
        * @return a symbian OS error code.
        * @return KErrNotFound is returned if the specified moduleId is not valid.
        */
        virtual TInt GetModuleStatus(
            const TPositionModuleId& aModuleId,
            TPositionModuleStatusBase& aStatus ) const = 0;
    };  

#endif  // EPOS_MPOSMODULESTATUSMANAGER_H
