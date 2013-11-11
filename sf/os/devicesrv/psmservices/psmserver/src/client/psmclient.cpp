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
* Description:  Power save mode client class
*
*/


#include "psmclient.h"
#include "psmclientimpl.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPsmClient* CPsmClient::NewL( MPsmClientObserver& aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClient::NewL()" ) ) );
    CPsmClient* self = CPsmClient::NewLC( aObserver );
    CleanupStack::Pop( self );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClient::NewL - return 0x%x" ), self ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CPsmClient::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPsmClient* CPsmClient::NewLC( MPsmClientObserver& aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClient::NewLC()" ) ) );
    CPsmClientImpl* self = new ( ELeave ) CPsmClientImpl( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClient::NewLC - return 0x%x" ), self ) );
    return self;
    }

// End of file
