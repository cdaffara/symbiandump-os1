/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Power save mode manager class
*
*/


#include <ecom/ecom.h>
#include <psmsrvdomaincrkeys.h>
#include <centralrepository.h>
#include "psmmodechangeobserver.h"
#include "psmmanager.h"
#include "psmpluginloader.h"
#include "psmsettingshandler.h"
#include "psmtrace.h"
#include "psmdefines.h"
#include "psmtypes.h"

// -----------------------------------------------------------------------------
// CPsmManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmManager* CPsmManager::NewL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NewL()" ) ) );

    CPsmManager* self = CPsmManager::NewLC();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmManager* CPsmManager::NewLC()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NewLC()" ) ) );

    CPsmManager* self = new( ELeave ) CPsmManager();

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmManager::CPsmManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmManager::CPsmManager() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPsmManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmManager::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::ConstructL()" ) ) );

    // Get current mode from CenRep
    TInt mode = 0;
    CRepository* cenrep = CRepository::NewLC( KCRUidPowerSaveMode );
    cenrep->Get( KPsmCurrentMode, mode );
    CleanupStack::PopAndDestroy( cenrep );

    // Create an instance of the settings handler. Manager has only one instance
    iSettingsHandler = CPsmSettingsHandler::NewL( (TPsmsrvMode)mode );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmManager::~CPsmManager()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::~CPsmManager()" ) ) );

    Cancel();

    if ( iSettingsHandler )
        {
        delete iSettingsHandler;
        }

    if ( iPluginLoader )
        {
        delete iPluginLoader;
        }    
    
    // Close mode observers array
    iModeObservers.Close();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::~CPsmManager - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::NotifyPowerSaveModeChangeL
// Starts mode change actions
// -----------------------------------------------------------------------------
//
void CPsmManager::NotifyPowerSaveModeChangeL( TInt aPsmMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NotifyPowerSaveModeChangeL()" ) ) );

    iPowerSaveModeChangeActive = ETrue;
    iModeChangeCancel = EFalse;

    if ( !iPluginLoader )
        {
        // Create plugin loader
        iPluginLoader = CPsmPluginLoader::NewL( *this );
        }
    else
        {
        // Check are there any pending plugins
        if ( iPluginLoader->PluginCount() )
            {
            // There are pending plugins, delete those
            delete iPluginLoader;
            iPluginLoader = NULL;
            iPluginLoader = CPsmPluginLoader::NewL( *this );
            }
        }

    // Settings handler already exists, update mode
    iSettingsHandler->SetMode( ( TPsmsrvMode ) aPsmMode );

    // Start loading plugins
    iPluginLoader->InitializePluginsL();

    // Notify plugins to change mode
    iPluginLoader->NotifyPlugins();

    // continue process asynchronously
    IssueRequest();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NotifyPowerSaveModeChangeL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::CancelPowerSaveModeChangeL
// Starts mode change actions
// -----------------------------------------------------------------------------
//
void CPsmManager::CancelPowerSaveModeChangeL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::CancelPowerSaveModeChangeL()" ) ) );

    if ( iPowerSaveModeChangeActive )
        {
        // Change mode normally to previous mode
        NotifyPowerSaveModeChangeL( iSettingsHandler->PreviousMode() );

        // Indicates that no need to notify observers
        iModeChangeCancel = ETrue;
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::CancelPowerSaveModeChangeL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmManager::IssueRequest
// ---------------------------------------------------------------------------
//
void CPsmManager::IssueRequest()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::IssueRequest()" ) ) );
    // Provides synchronous function calls to be handled as asynchronous
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus *s = &iStatus;
	    User::RequestComplete( s, KErrNone );
        }
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::IssueRequest() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::RunL
// -----------------------------------------------------------------------------
//
void CPsmManager::RunL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::RunL()" ) ) );

    // Change cenrep values
    TRAPD( cenrepErr, iSettingsHandler->ChangeCenRepSettingsL() );

    ERROR_TRACE( ( _L( "PSM Server - CPsmManager::RunL: CenRep change error: %i" ), cenrepErr ) );

    TInt completeCode( KErrNone );

    if ( KErrNone != cenrepErr )
        {
        // Indicate to mode change originator that mode is changed but with some failures
        // i.e. mode successfully changed only partially
        completeCode = KErrCompletion;
        }

    // Update mode to CenRep
    CRepository* cenrep = CRepository::NewLC( KCRUidPowerSaveMode );
    cenrep->Set( KPsmCurrentMode, iSettingsHandler->Mode() );
    CleanupStack::PopAndDestroy( cenrep );

    // If mode change was cancelled, no need to notify observers
    if ( !iModeChangeCancel )
        {
        // Notify observers
        NotifyObservers( completeCode );
        }

    // PSM change no longer active
    iPowerSaveModeChangeActive = EFalse;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::RunL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::NotifyObservers
// -----------------------------------------------------------------------------
//
void CPsmManager::NotifyObservers( const TInt aError ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NotifyObservers()" ) ) );

    // Notify mode observers
    for ( TInt i = 0; i < iModeObservers.Count(); i++ )
        {
        iModeObservers[i]->NotifyPsmModeChangeComplete( aError );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::NotifyObservers - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::DoCancel
// -----------------------------------------------------------------------------
//
void CPsmManager::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::DoCancel()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::DoCancel - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::RunError
// -----------------------------------------------------------------------------
//
TInt CPsmManager::RunError( TInt aError )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::RunError()" ) ) );

    // RunError should never occur as there are no leaving possibilities in RunL.
    // Keeping RunError for possible future use

    NotifyObservers( aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPsmManager::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CPsmManager::RegisterObserverL( MPsmModeChangeObserver* aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::RegisterObserverL()" ) ) );
    iModeObservers.AppendL( aObserver );    
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::RegisterObserverL() return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::UnregisterObserver
// -----------------------------------------------------------------------------
//
void CPsmManager::UnregisterObserver( MPsmModeChangeObserver* aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::UnregisterObserver()" ) ) );

    // Loop observers and remove if found
    for ( TInt i = 0; i < iModeObservers.Count(); i++ )
        {
        if ( aObserver == iModeObservers[ i ] )
            {
            iModeObservers.Remove( i );
            }
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::UnregisterObserver - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmManager::SettingsProvider
// -----------------------------------------------------------------------------
//
MPsmSettingsProvider& CPsmManager::SettingsProvider()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmManager::SettingsProvider()" ) ) );
    return *iSettingsHandler; 
    }

#ifdef _DEBUG 
// -----------------------------------------------------------------------------
// CPsmManager::ClosePluginLoader
// For testing purpose 
// -----------------------------------------------------------------------------
void CPsmManager::ClosePluginLoader()
    {
    if(iPluginLoader)
        {
        delete iPluginLoader;
        iPluginLoader=NULL;
        }
    }

// -----------------------------------------------------------------------------
// CPsmManager::CompressModeObserversArray
// For testing purpose 
// -----------------------------------------------------------------------------
void CPsmManager::CompressModeObserversArray()
    {
    iModeObservers.Compress();
    }
#endif // _DEBUG 
// End of file
