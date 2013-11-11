/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class definition of plugin custom interface factory interface.
*
*/



// Include files
#include <e32base.h>
#include <implementationproxy.h>
#include "ciplatformfactoryplugin.h"
#include "ciplatformfactoryplugin.hrh"

// ---------------------------------------------------------------------------
// Plugin creation function pointer for ECom framework
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KUidCCIPlatformFactoryPlugin, CCIPlatformFactoryPlugin::NewL)
    };

// ---------------------------------------------------------------------------
// Implementation table export for ECom framework
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }
