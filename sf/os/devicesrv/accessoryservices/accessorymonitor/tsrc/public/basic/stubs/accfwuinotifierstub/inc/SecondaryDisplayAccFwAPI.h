/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The set of messages provided to Secondary Display software by
*              : AccFW subsystem.
*
*/

#ifndef __SECONDARYDISPLAYACCFWAPI_H
#define __SECONDARYDISPLAYACCFWAPI_H

// INCLUDES
#include <e32base.h>

/*
* ==============================================================================
* This file contains the following sections:
*   - Dialog definitions
*   - Command definitions
*   - Event definitions
*   - Parameter definitions
* ==============================================================================
*/

// The category UID for the messages in this header file.
//
const TUid KCatAccFw = {0x10205030}; 

/*
* ==============================================================================
* Dialogs shown by AccFW subsystem. These messages are handled using the
* Secondary Display support in Avkon.
* ==============================================================================
*/
enum TSecondaryDisplayAccFwDialogs
    {
    /**
    * No note. Error condition if this comes to CoverUI
    */
    ECmdNoNote = 0,
    
    /**
    * A command for showing the "Accessory not supported" note on secondary display.
    */
    ECmdShowAccessoryNotSupportedNote,

    /**
    * A command for showing the "Accessory selection dialog" dialog on secondary display.
    */
    ECmdShowAccessorySelectionDialog,
   
    };
#endif      // SECONDARY_DISPLAY_ACCFW_API_H
            
// End of File
