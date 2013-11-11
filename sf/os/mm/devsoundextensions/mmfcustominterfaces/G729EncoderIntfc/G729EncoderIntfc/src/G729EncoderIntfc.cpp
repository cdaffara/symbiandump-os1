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
* Description:   Interface definition for G729 encoder interface.
*
*/



// INCLUDE FILES
#include "G729EncoderIntfcProxy.h"
#include "G729EncoderIntfc.h"
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

// -----------------------------------------------------------------------------
// CG729EncoderIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the devsound passed in.
// Devsound will return the handle to the custom interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CG729EncoderIntfc* 
CG729EncoderIntfc::NewL(CMMFDevSound& aDevSound)
    {
    CG729EncoderIntfcProxy* g729EncoderIntfcProxy;
    g729EncoderIntfcProxy = 
        (CG729EncoderIntfcProxy*)aDevSound.CustomInterface(
                                           KUidG729EncoderIntfc);
    if (!g729EncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return g729EncoderIntfcProxy;
    }

// -----------------------------------------------------------------------------
// CG729EncoderIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the CMdaAudioInputStream passed in.
// The CMdaAudioInputStream utility will return the handle to the custom 
// interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CG729EncoderIntfc* 
CG729EncoderIntfc::NewL(CMdaAudioInputStream& aUtility)
    {
    CG729EncoderIntfcProxy* g729EncoderIntfcProxy;
    g729EncoderIntfcProxy = 
        (CG729EncoderIntfcProxy*)aUtility.CustomInterface(
                                          KUidG729EncoderIntfc);
    if (!g729EncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return g729EncoderIntfcProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
