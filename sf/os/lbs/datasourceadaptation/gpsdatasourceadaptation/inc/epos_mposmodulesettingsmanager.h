/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EPOS_MPOSMODULESETTINGSMANAGER_H
#define EPOS_MPOSMODULESETTINGSMANAGER_H

#include <lbs.h>

class MPosModuleSettingsListener;
class CPosModuleIdList;

/**
*  This is an interface through which a PSY can get module settings and listen
*  to module settings change.
* 
*/
class MPosModuleSettingsManager
    {
    public:
        /**
        * Add a listener to get the module settings change event. 
        *
        * The listener must be removed when the client does not want to get
        * module settings change event any more.
        *
        * @param aListener The reference to listener class.
        */
        virtual void AddListenerL(
            MPosModuleSettingsListener& aListener ) = 0;

        /**
        * Remove a listener of module settings chagne event.
        *
        * If the listener has not been added, this function does nothing.
        *
        * @param aListener The reference to the listener class.
        */
        virtual void RemoveListener(
            MPosModuleSettingsListener& aListener) = 0;

        /**
        * Retrieves a list of the existing modules in priority order.
        * Client takes ownership of returned object.
        *
        * Modules marked as invisible are not included in this list.
        *
        * @return List of visible modules
        * @see ModuleIdListLC()
        */
        virtual CPosModuleIdList* ModuleIdListL() = 0;

        /**
        * Retrieves the attributes of a module.
        *
        * If the supplied position module info class is not supported, this
        * method will leave with code KErrNotSupported . Currently only
        * TPositionModuleInfo is supported.
        *
        * @param[in] aModuleId The ID of the module to retrieve information for.
        * @param[out] aInfo On return contains the module information.
        *
        * @leave KErrNotSupported Not supported module info class object is
        *                         is supplied as parameter.
        * @leave KErrNotFound No module with given ID is installed.
        *
        */
        virtual void GetModuleInfoL(
            TPositionModuleId aModuleId,
            TPositionModuleInfoBase& aInfo
            ) = 0;

        /**
        * Get module visibility status.
        *
        * @param[in] aModuleId The ID of the module to retrieve visiblity.
        * @return ETrue if the module is visible
        *
        * @leave KErrNotFound No module with given ID is installed.
        */
        virtual TBool IsModuleVisibleL(
            TPositionModuleId aModuleId
            ) = 0;
    };  

#endif  // EPOS_MPOSMODULESETTINGSMANAGER_H
