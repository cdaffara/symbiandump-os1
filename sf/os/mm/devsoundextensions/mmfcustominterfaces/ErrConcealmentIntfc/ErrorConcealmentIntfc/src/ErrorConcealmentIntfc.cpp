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
* Description:   Interface definition for error concealment interface.
*
*/



// INCLUDE FILES
#include "ErrorConcealmentIntfcProxy.h"
#include "ErrorConcealmentIntfc.h"
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
// CErrorConcealmentIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the devsound passed in.
// Devsound will return the handle to the custom interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CErrorConcealmentIntfc* 
CErrorConcealmentIntfc::NewL(CMMFDevSound& aDevSound)
    {
    CErrorConcealmentIntfcProxy* errorConcealmentIntfcProxy;
    errorConcealmentIntfcProxy = 
        (CErrorConcealmentIntfcProxy*)aDevSound.CustomInterface(
                                                KUidErrorConcealmentIntfc);
    if (!errorConcealmentIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return errorConcealmentIntfcProxy;
    }

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfc::NewL
// Creates the custom interface.
// Calls the custom interface method on the CMdaAudioOutputStream passed in.
// The CMdaAudioOutputStream utility will return the handle to the custom 
// interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CErrorConcealmentIntfc* 
CErrorConcealmentIntfc::NewL(CMdaAudioOutputStream& aUtility)
    {
    CErrorConcealmentIntfcProxy* errorConcealmentIntfcProxy;
    errorConcealmentIntfcProxy = 
        (CErrorConcealmentIntfcProxy*)aUtility.CustomInterface(
                                               KUidErrorConcealmentIntfc);
    if (!errorConcealmentIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return errorConcealmentIntfcProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
