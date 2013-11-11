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
* Description:   Interface definition for Ilbc encoder interface.
*
*/



// INCLUDE FILES
#include "IlbcEncoderIntfcProxy.h"
#include "IlbcEncoderIntfc.h"
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
// CIlbcEncoderIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the devsound passed in.
// Devsound will return the handle to the custom interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcEncoderIntfc* 
CIlbcEncoderIntfc::NewL(CMMFDevSound& aDevSound)
    {
    CIlbcEncoderIntfcProxy* ilbcEncoderIntfcProxy;
    ilbcEncoderIntfcProxy = 
        (CIlbcEncoderIntfcProxy*)aDevSound.CustomInterface(KUidIlbcEncoderIntfc);
    if (!ilbcEncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return ilbcEncoderIntfcProxy;
    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the CMdaAudioInputStream passed in.
// The CMdaAudioInputStream utility will return the handle to the custom 
// interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcEncoderIntfc* 
CIlbcEncoderIntfc::NewL(CMdaAudioInputStream& aUtility)
    {
    CIlbcEncoderIntfcProxy* ilbcEncoderIntfcProxy;
    ilbcEncoderIntfcProxy = 
        (CIlbcEncoderIntfcProxy*)aUtility.CustomInterface(KUidIlbcEncoderIntfc);
    if (!ilbcEncoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return ilbcEncoderIntfcProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
