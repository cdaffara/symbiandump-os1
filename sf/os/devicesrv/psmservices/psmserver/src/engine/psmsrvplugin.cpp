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
* Description:  PSM Plug-in loader
*
*/


#include <ecom/ecom.h>
#include <psmpluginbase.h>
#include "psmsrvplugin.h"
#include "psmsettingshandler.h"
#include "psmmanager.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmSrvPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSrvPlugin* CPsmSrvPlugin::NewL( CPsmManager& aManager, TUid aImplUid )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NewL()" ) ) );

    CPsmSrvPlugin* self = CPsmSrvPlugin::NewLC( aManager, aImplUid );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSrvPlugin::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSrvPlugin* CPsmSrvPlugin::NewLC( CPsmManager& aManager, TUid aImplUid )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NewLC()" ) ) );

    CPsmSrvPlugin* self = new( ELeave ) CPsmSrvPlugin( aManager, aImplUid );
    
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NewLC - return 0x%x" ), self ) );

    return self;
    }
    

// -----------------------------------------------------------------------------
// CPsmSrvPlugin::CPsmSrvPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmSrvPlugin::CPsmSrvPlugin( CPsmManager& aManager, TUid aImplUid ) :
    iManager( aManager ),
    iImplUid( aImplUid )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmSrvPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmSrvPlugin::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::ConstructL()" ) ) );

    // First, create settings handler
    iSettingsHandler = CPsmSettingsHandler::NewL( iManager.SettingsProvider().Mode() );

    // Then, initialize plugin
    iActivePlugin = CPsmPluginBase::NewL( iImplUid, *iSettingsHandler );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmSrvPlugin::~CPsmSrvPlugin()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::~CPsmSrvPlugin()" ) ) );

    if ( iActivePlugin )
        {
        delete iActivePlugin;
        iActivePlugin = NULL;
        }

    delete iSettingsHandler;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::~CPsmSrvPlugin - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvPlugin::NotifyModeChange
// -----------------------------------------------------------------------------
//
void CPsmSrvPlugin::NotifyModeChange( const TPsmsrvMode aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NotifyModeChange()" ) ) );

    iSettingsHandler->SetMode( aMode );

    iActivePlugin->NotifyModeChange( aMode );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvPlugin::NotifyModeChange - return" ) ) );
    }

// End of file
