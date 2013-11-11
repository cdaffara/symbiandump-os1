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
* Description:
*
*/


// INCLUDE FILES
#include <ecom/implementationproxy.h>
#include "RemConKeyEventConverter.h"
#include "RemConDebug.h"

// CONSTANTS
// Define the private interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x10205035, CRemConKeyEventConverter::NewL),
    };

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy (needed by ECom FW)
// Returns: const TImplementationProxy*: Pointer to implementation table.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    COM_TRACE_1( "[AccFW:RemConConverter] TImplementationProxy* ImplementationGroupProxy(0x%x)", aTableCount );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

//
// End of file
