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




#ifndef BTGPSNOKNIGHTMODEHANDLER_H
#define BTGPSNOKNIGHTMODEHANDLER_H

//  INCLUDES

#include <e32base.h>
#include "BTGPSSettingsApi.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;

// CLASS DECLARATION

/**
*  Handler for Nokia BT GPS night mode feature. This class will set
*  set mode to connected Nokia BT GPS device according to setting. It will
*  check the response from Nokia BT GPS device and retry to set the night
*  mode setting if not correct response is received within timeout.
*
*/
class CBTGPSNokNightModeHandler: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        * @param aConnectManager Reference to connect manager
        * @param aNightModeStatus Noght mode status in setting
        */
        static CBTGPSNokNightModeHandler * NewL(
            CBTGPSConnectManager& aConnectManager,
            TBTGPSSettingsApi::TNightModeState aNightModeStatus);

        /**
        * Destructor
        */
        virtual ~CBTGPSNokNightModeHandler();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSNokNightModeHandler(
            CBTGPSConnectManager& aConnectManager,
            TBTGPSSettingsApi::TNightModeState aNightModeStatus);
        
        /**
        * Timer call back
        */
        void TimerTick();
        
        /**
        * Static timer callback
        */
        static TInt TimerCallback(TAny* aAny);
        
        /**
        * Send night mode setting message
        */
        void SetNightModeSetting();
        
    private:
        //Connect manager
        CBTGPSConnectManager& iConnectManager;
        
        //Night mode status
        TBTGPSSettingsApi::TNightModeState iNightModeStatus;
        
        //Timer
        CPeriodic* iTimer;
    };
#endif
// End of File

