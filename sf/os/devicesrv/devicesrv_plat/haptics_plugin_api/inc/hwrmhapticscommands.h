/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics plugin API commands
*
*/

#ifndef HWRMHAPTICSCOMMANDS_H
#define HWRMHAPTICSCOMMANDS_H

/*
* ==============================================================================
* This file contains the following sections:
*   - Command definitions for Haptics plugin API
*   - Command parameter definitions for Haptics plugin API
*   - Response parameter definitions for Haptics plugin API
* ==============================================================================
*/

// Include plugin service so that including only 
// this header is necessary to use Haptics plugin API.
   
#include <hwrmhapticspluginservice.h>
#include <hwrmhapticsrespdata.h>

// use this constant while allocating max length for RHWRMHapticsRespData
const TInt KHWRMHapticsRespMaxSize = 128;

/**
*  Encapsulates the Haptics command enumerations.
*
*  @since S60 5.1
*/
namespace HWRMHapticsCommand
    {
    // request
    typedef RBuf8 RHWRMHapticsReqData;

    // response
    typedef RBuf8 RHWRMHapticsRespData; 
    
    /*
    * ========================================================================
    * Command definitions
    * All commands contain a command id and a set of parameters.
    * The set of parameters is internalized into a RHWRMHapticsReqData
    * descriptor.
    *
    * This interface is asynchronous. A command is interpreted of being 
    * completed after a response for that command is received.
    * The actual response data is internalized into a RHWRMHapticsRespData
    * descriptor.
    * ========================================================================
    */
    enum THapticsCmd
        {
        /**
         * Not a valid command Id. Used only for sanity checks.
         */
        ENoCommandId = 3000,
        /**
        * Haptics command id. Used for all haptics commands.
        */
        EHapticsCmdId 
        };
    }

#endif      // HWRMHAPTICSCOMMANDS_H   
            
// End of File
