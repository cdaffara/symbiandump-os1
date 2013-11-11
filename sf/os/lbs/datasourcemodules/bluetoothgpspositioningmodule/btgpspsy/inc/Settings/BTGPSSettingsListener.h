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




#ifndef BTGPSSETTINGSLISTENER_H
#define BTGPSSETTINGSLISTENER_H

#include <e32base.h>
#include "BTGPSSettingsApi.h"

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface class for updating PSY settings. Listeners use this interface
*  get PSY night mode setting change from Central Repository
*  
*/
class MBTGPSSettingsListener
    {
    public:
        /**
        * Updates Nokia BT GPS device nightmode status.
        * @param aStatus New night mode status.
        */
	    virtual void HandleNightModeChange( 
	        const TBTGPSSettingsApi::TNightModeState aStatus ) = 0;
    };
#endif
// End of File
