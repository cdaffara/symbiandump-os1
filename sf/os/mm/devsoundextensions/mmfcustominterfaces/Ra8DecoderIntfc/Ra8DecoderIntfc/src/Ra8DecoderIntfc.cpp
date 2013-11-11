/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Custom Interface for RA8 decoder.
*
*/



// INCLUDE FILES
#include <sounddevice.h>
#include "Ra8CustomInterface.h"
#include "Ra8CustomInterfaceProxy.h"


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
 * CRa8CustomInterface::NewL
 * Creates the custom interface.
 * Calls the custom interface method on the devsound passed in.
 * Devsound will return the handle to the custom interface proxy.
 * (other items were commented in a header).
 */
EXPORT_C CRa8CustomInterface* CRa8CustomInterface::NewL(CMMFDevSound& aDevSound)
    {
    CRa8CustomInterfaceProxy* ra8DecoderProxy;
    ra8DecoderProxy = (CRa8CustomInterfaceProxy*)
                      aDevSound.CustomInterface(KUidRa8DecHwDeviceCI);
    
    if (!ra8DecoderProxy)
        {
        User::Leave(KErrNotFound);
        }
        
    return ra8DecoderProxy;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
