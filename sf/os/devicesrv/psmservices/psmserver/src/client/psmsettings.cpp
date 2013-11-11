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


#include "psmsettings.h"
#include "psmsettingsimpl.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPsmSettings* CPsmSettings::NewL()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettings::NewL()" ) ) );
    CPsmSettings* self = CPsmSettings::NewLC();
    CleanupStack::Pop( self );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettings::NewL - return 0x%x" ), self ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSettings::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPsmSettings* CPsmSettings::NewLC()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettings::NewLC()" ) ) );
    CPsmSettingsImpl* self = new ( ELeave ) CPsmSettingsImpl;
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettings::NewLC - return 0x%x" ), self ) );
    return self;
    }

// End of file
