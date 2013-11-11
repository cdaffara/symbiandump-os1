/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Plugged Display ASY Ecom proxy declarations.
 *
 */

// SYSTEM INCLUDES
#include <ecom/implementationproxy.h>

// USER INCLUDES
#include "pdasymainservice.h"
#include "pdasycmdhandler.h"
#include "pdasyconst.hrh"

// ======== MEMBER FUNCTIONS ========

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        PD_ASY_MAIN_SERVICE_IMPL_UID, 
        CPDAsyMainService::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( 
        PD_ASY_CMD_HANDLER_IMPL_UID, 
        CPDAsyCmdHandler::NewL ),
    };

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of file
