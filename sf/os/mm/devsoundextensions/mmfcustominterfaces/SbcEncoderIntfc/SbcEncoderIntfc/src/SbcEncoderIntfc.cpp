/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface definition for BT SBC Encoder configuration.
*
*/



// INCLUDE FILES
#include "SbcEncoderIntfcProxy.h"
#include "SbcEncoderIntfc.h"
#include <mdaaudioinputstream.h>
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

/**
 * CSbcEncoderIntfc::NewL
 *
 * Creates the custom interface.
 * Calls the custom interface method on the devsound passed in.
 * Devsound will return the handle to the custom interface proxy.
 *
 * (other items were commented in a header).
 *
 */
EXPORT_C CSbcEncoderIntfc* 
CSbcEncoderIntfc::NewL(CMMFDevSound& aDevSound)
    {
    CSbcEncoderIntfcProxy* sbcEncoderIntfcProxy =
        (CSbcEncoderIntfcProxy*)aDevSound.CustomInterface(KUidSbcEncoderIntfc);

    if (!sbcEncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }

    return sbcEncoderIntfcProxy;
    }

/**
 * CSbcEncoderIntfc::NewL
 *
 * Creates the custom interface.
 * Calls the custom interface method on the CMdaAudioInputStream passed in.
 * The CMdaAudioInputStream utility will return the handle to the custom 
 * interface proxy.
 *
 * (other items were commented in a header).
 *
 */
EXPORT_C CSbcEncoderIntfc* 
CSbcEncoderIntfc::NewL(CMdaAudioInputStream& aUtility)
    {
    CSbcEncoderIntfcProxy* sbcEncoderIntfcProxy =
        (CSbcEncoderIntfcProxy*)aUtility.CustomInterface(KUidSbcEncoderIntfc);

    if (!sbcEncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }

    return sbcEncoderIntfcProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
