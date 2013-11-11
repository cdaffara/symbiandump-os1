/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include <e32std.h>
#include <ecom/ImplementationProxy.h>

#include "BTAsyStubMainServiceBase.h"
#include "BTAsyStubCmdHandlerBase.h"

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {       
        {{0x10200c7C},  (TProxyNewLPtr) CBTASYStubMainServiceBase::NewL},
        {{0x10200c7D},  (TProxyNewLPtr) CBTASYStubCmdHandlerBase::NewL}
    };


//
// ---------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy( TInt&)
// ---------------------------------------------------------
//  
// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
