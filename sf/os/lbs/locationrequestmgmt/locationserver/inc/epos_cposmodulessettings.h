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



#ifndef EPOS_CPOSMODULESSETTINGS_H
#define EPOS_CPOSMODULESSETTINGS_H

//  INCLUDES
#include <lbs.h>
#include "epos_mposmodulesettingsmanager.h"
#include <lbs/epos_mposmodulesobserver.h>

class CPosModules;

/**
*  Class manages module settings.
*/
class CPosModuleSettings : public CBase,
                           public MPosModulesObserver,
                           public MPosModuleSettingsManager
    {
    public:  // Constructors and destructor
        static CPosModuleSettings* NewL();
        virtual ~CPosModuleSettings();

    public: // New functions
        CPosModules& PosModules();

    public:	// From MPosModuleSettingsManager
        virtual void AddListenerL(MPosModuleSettingsListener& aListener );
        virtual void RemoveListener(MPosModuleSettingsListener& aListener);
        virtual CPosModuleIdList* ModuleIdListL();
        virtual void GetModuleInfoL(TPositionModuleId aModuleId, TPositionModuleInfoBase& aInfo);
        virtual TBool IsModuleVisibleL(TPositionModuleId aModuleId);

    protected:  // From MPosModulesObserver
        virtual void HandleSettingsChangeL( TPosModulesEvent aEvent );

    private:
        CPosModuleSettings();
        void ConstructL();
        CPosModuleSettings& operator= ( const CPosModuleSettings& );

    private:    // Data
        //Listen array
        RPointerArray < MPosModuleSettingsListener > iListenerArray;
        
        //Module settings API
        CPosModules* iModules;
        
    };

#endif      // EPOS_CPOSMODULESSETTINGS_H

// End of File
