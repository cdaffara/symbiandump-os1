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
#include "psmpluginloader.h"
#include "psmsrvplugin.h"
#include "psmmanager.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmPluginLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmPluginLoader* CPsmPluginLoader::NewL( CPsmManager& aManager )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NewL()" ) ) );

    CPsmPluginLoader* self = CPsmPluginLoader::NewLC( aManager );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmPluginLoader* CPsmPluginLoader::NewLC( CPsmManager& aManager )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NewLC()" ) ) );

    CPsmPluginLoader* self = new( ELeave ) CPsmPluginLoader( aManager );

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::CPsmPluginLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmPluginLoader::CPsmPluginLoader( CPsmManager& aManager ) :
    iManager( aManager )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmPluginLoader::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmPluginLoader::~CPsmPluginLoader()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::~CPsmPluginLoader()" ) ) );

    iPluginArray.ResetAndDestroy();

    REComSession::FinalClose();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::~CPsmPluginLoader - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::InitializePluginsL
// -----------------------------------------------------------------------------
//
static void ResetAndDestroyArray( TAny* aPtr )
    {
    RImplInfoPtrArray* array = static_cast< RImplInfoPtrArray* >( aPtr );
    array->ResetAndDestroy();
    array->Close();
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::InitializePluginsL
// -----------------------------------------------------------------------------
//
void CPsmPluginLoader::InitializePluginsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::InitializePluginsL()" ) ) );

    // List implementations
    RImplInfoPtrArray implInfoArray;
    TCleanupItem cleanupItem( ResetAndDestroyArray, &implInfoArray );
    CleanupStack::PushL( cleanupItem );
    
    REComSession::ListImplementationsL( KPsmPluginControlInterfaceUid, implInfoArray );

    // Get number of implementations
    const TInt pluginCount( implInfoArray.Count() );

    // Loop implementation info and create instances
    for ( TInt i = 0; i < pluginCount; i++ )
        {
        // Get next implementation info
        CImplementationInformation* info = 
            static_cast<CImplementationInformation*>( implInfoArray[i] );
            
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::InitializePluginsL - create plugin 0x%x started" ),
            info->ImplementationUid().iUid) );

        // Create new instance of the plugin
        CPsmSrvPlugin* plugin = NULL;
        TRAPD(err, plugin = CPsmSrvPlugin::NewL( iManager, info->ImplementationUid() ));
        if( err == KErrNone )
            {
            // Add plugin to array
            CleanupStack::PushL( plugin );
            User::LeaveIfError( iPluginArray.Append( plugin ) );
            CleanupStack::Pop( plugin );
            }
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::InitializePluginsL - create plugin 0x%x ended with code %d" ),
            info->ImplementationUid().iUid, err ) );
        }

    // Reset and destroy implementations infos
    CleanupStack::PopAndDestroy(&implInfoArray);

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::InitializePluginsL - return plugin count: %d" ), pluginCount ) );
    }

// -----------------------------------------------------------------------------
// CPsmPluginLoader::NotifyPlugins
// -----------------------------------------------------------------------------
//
void CPsmPluginLoader::NotifyPlugins()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NotifyPlugins()" ) ) );

    // Current mode
    const TPsmsrvMode mode( iManager.SettingsProvider().Mode() );

    // Loop plugins and notify
    for ( TInt i = 0; i < iPluginArray.Count(); i++ )
        {
        iPluginArray[ i ]->NotifyModeChange( mode );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmPluginLoader::NotifyPlugins - return" ) ) );
    }

// End of file
