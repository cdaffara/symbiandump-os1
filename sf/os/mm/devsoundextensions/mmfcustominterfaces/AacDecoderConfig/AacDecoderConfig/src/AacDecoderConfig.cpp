/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface definition for AAC decoder configuration interface.
*
*/



// INCLUDE FILES
#include "AacDecoderConfigProxy.h"
#include "AacDecoderConfig.h"
#include <mdaaudiooutputstream.h>
#include <sounddevice.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAacDecoderConfig::NewL
// Creates the custom interface.
// Calls the custom interface method on the devsound passed in.
// Devsound will return the handle to the custom interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CAacDecoderConfig* CAacDecoderConfig::NewL(CMMFDevSound& aDevSound)
    {
    CAacDecoderConfigProxy* aacDecoderConfigProxy;
    aacDecoderConfigProxy = 
       (CAacDecoderConfigProxy*)aDevSound.CustomInterface(KUidAacDecoderConfig);
    if (!aacDecoderConfigProxy)
        {
        User::Leave(KErrNotFound);
        }
    return aacDecoderConfigProxy;
    }

// -----------------------------------------------------------------------------
// CAacDecoderConfig::NewL
// Creates the custom interface.
// Calls the custom interface method on the CMdaAudioOutputStream passed in.
// The CMdaAudioOutputStream utility will return the handle to the custom 
// interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CAacDecoderConfig* 
CAacDecoderConfig::NewL(CMdaAudioOutputStream& aUtility)
    {
    CAacDecoderConfigProxy* aacDecoderConfigProxy;
    aacDecoderConfigProxy = 
        (CAacDecoderConfigProxy*)aUtility.CustomInterface(
                                          KUidAacDecoderConfig);
    if (!aacDecoderConfigProxy)
        {
        User::Leave(KErrNotFound);
        }
    return aacDecoderConfigProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
