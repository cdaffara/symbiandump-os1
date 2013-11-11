/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Fmtx watcher plugin implementation.
*
*/


#include <hwrmfmtxcommands.h>
#include <ecom/implementationproxy.h>
#include "hwrmfmtxwatcherplugin.h"
#include "hwrmfmtxaccobserver.h"
#include "hwrmfmtxusbobserver.h"
#include "hwrmfmtxplugintimer.h"
#ifdef INSECURE_AUDIO_POLICY_KEYS
#include "hwrmfmtxtestobserver.h"
#endif // INSECURE_AUDIO_POLICY_KEYS
#include "trace.h"

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x20012402, CHWRMFmtxWatcherPlugin::NewL)
    };

// ======== OTHER EXPORTED FUNCTIONS ========

// -----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::NewL
// -----------------------------------------------------------------------------
//
CHWRMFmtxWatcherPlugin* CHWRMFmtxWatcherPlugin::NewL()
    {
    FUNC_LOG;
    
    CHWRMFmtxWatcherPlugin* self = new(ELeave) CHWRMFmtxWatcherPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::NewL
// -----------------------------------------------------------------------------
//
CHWRMFmtxWatcherPlugin::~CHWRMFmtxWatcherPlugin()
    {
    FUNC_LOG;
    
    iObservers.ResetAndDestroy();
    iTimers.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::CHWRMFmtxWatcherPlugin
// -----------------------------------------------------------------------------
//
CHWRMFmtxWatcherPlugin::CHWRMFmtxWatcherPlugin()
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::ConstructL()
    {
    FUNC_LOG;
    
    iConnected.Set( EFmtxWatcherObserverAccessory ); // assume connected
    MHWRMFmtxConnObserver* observer = CHWRMFmtxAccObserver::NewL( *this );
    CleanupStack::PushL( observer );
    iObservers.AppendL( observer );
    CleanupStack::Pop( observer );
 
#ifndef __WINS__ // RUsbMan not created because it does not behave well in emulator
    iConnected.Set( EFmtxWatcherObserverUsb ); // assume connected
    observer = CHWRMFmtxUsbObserver::NewL( *this );
    CleanupStack::PushL( observer );
    iObservers.AppendL( observer );
    CleanupStack::Pop( observer );
#else // __WINS__
    iObservers.AppendL( NULL );
#endif // __WINS__

// Test observer should be always the last
#ifdef INSECURE_AUDIO_POLICY_KEYS
    observer = CHWRMFmtxTestObserver::NewL( *this );
    CleanupStack::PushL( observer );
    iObservers.AppendL( observer );
    CleanupStack::Pop( observer );
#endif // INSECURE_AUDIO_POLICY_KEYS

    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::ProcessCommandL( const TInt aCommandId,
                                              const TUint8 aTransId,
                                              TDesC8& /*aData*/ )
    {
    FUNC_LOG;
    
    INFO_LOG2( "CHWRMFmtxWatcherPlugin::ProcessCommandL, (0x%x, 0x%x)", 
        aCommandId, aTransId );

    TInt retVal( KErrNone );

    switch (aCommandId)
        {
        case HWRMFmTxCommand::EConnStatusCmdId:
            TRAP( retVal, GetStatusL() );
            break;
        
        case HWRMFmTxCommand::EStartObservingCmdId:
            TRAP( retVal, StartObservingL() );
            break;
            
        case HWRMFmTxCommand::EStopObservingCmdId:
            StopObserving();
            break;
            
        default :
            User::Leave( KErrNotSupported );
            break;
        }

    // Create new timer for response.
    CHWRMFmtxPluginTimer* timer = CHWRMFmtxPluginTimer::NewL( aCommandId, aTransId, retVal, *this );
    CleanupStack::PushL( timer );
    iTimers.AppendL( timer );
    CleanupStack::Pop( timer );
    INFO_LOG1( "CHWRMFmtxWatcherPlugin::ProcessCommandL, timers now %d", 
        iTimers.Count() );
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::CancelCommandL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::CancelCommandL(const TUint8 aTransId,
#if defined(FMTX_WATCHER_INFO_LOG_ENABLED)
                                            const TInt aCommandId)
#else
                                            const TInt /*aCommandId*/)
#endif
    {
    FUNC_LOG;
    
    INFO_LOG2( "CHWRMFmtxWatcherPlugin::CancelCommandL, (0x%x, 0x%x)", 
        aCommandId, aTransId );

    TInt count = iTimers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if ( iTimers[i]->TransId() == aTransId )
            {
            delete iTimers[i];
            iTimers.Remove(i);
            INFO_LOG1( "CHWRMFmtxWatcherPlugin::CancelCommandL, removed timer %d", i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::GenericTimerFired
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::GenericTimerFired(TInt aCommandId, const TUint8 aTransId, 
                                               TInt aRetVal)
    {
    FUNC_LOG;
    
    INFO_LOG3( "CHWRMFmtxWatcherPlugin: GenericTimerFired, (0x%x, 0x%x, %d)", 
        aCommandId, aTransId, aRetVal );

	TInt err = KErrNone;
	
    switch( aCommandId )
        {
        case HWRMFmTxCommand::EConnStatusCmdId:
        case HWRMFmTxCommand::EStartObservingCmdId:
            {
            HWRMFmTxCommand::TConnStatusData data;
            data.iErrorCode = aRetVal;
            data.iConnected = iConnected.Value() ? ETrue : EFalse;
            HWRMFmTxCommand::TConnStatusPackage pckg( data );
            TRAP(err, iResponseCallback->ProcessResponseL( aCommandId, aTransId, pckg ));
            break;
            }
            
        case HWRMFmTxCommand::EStopObservingCmdId:
            {
            HWRMFmTxCommand::TErrorCodeResponsePackage pckg( aRetVal );
            TRAP(err, iResponseCallback->ProcessResponseL( aCommandId, aTransId, pckg ));
            break;
            }
            
        default :
            {
            HWRMFmTxCommand::TErrorCodeResponsePackage pckg( KErrNotSupported );
            TRAP(err, iResponseCallback->ProcessResponseL( aCommandId, aTransId, pckg ));
            break;
            }
        }

    LOG_IF_ERROR1( err, "CHWRMFmtxWatcherPlugin::GenericTimerFired, err %d", err );

    // delete obsolete (if any) timers
    for( TInt i = (iTimers.Count()-1); i >= 0 ; i-- )
        {
        if ( !iTimers[i]->IsActive() )
            {
            delete iTimers[i];
            iTimers.Remove(i);
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::GetStatusL
// -----------------------------------------------------------------------------
//
TBool CHWRMFmtxWatcherPlugin::GetStatusL()
    {
    FUNC_LOG;

    TInt count = iObservers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        MHWRMFmtxConnObserver* observer = iObservers[i];
        if ( observer )
            {
            iConnected.Assign( i, observer->GetStatusL() );
            }
        }
    
    return iConnected.Value();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::StartObservingL
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::StartObservingL()
    {
    FUNC_LOG;

    TInt count = iObservers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        MHWRMFmtxConnObserver* observer = iObservers[i];
        if ( observer )
            {
            observer->StartObservingL();
            }
        }

    GetStatusL();
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::StopObserving
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::StopObserving()
    {
    FUNC_LOG;

    TInt count = iObservers.Count();
    for( TInt i = 0; i < count; i++ )
        {
        MHWRMFmtxConnObserver* observer = iObservers[i];
        if ( observer )
            {
            observer->StopObserving();
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmtxWatcherPlugin::HandleConnectionChange
// -----------------------------------------------------------------------------
//
void CHWRMFmtxWatcherPlugin::HandleConnectionChange( TFmtxWatcherObserverTypes aType,
                                                     TBool aConnected )
    {
    FUNC_LOG;

    iConnected.Assign( aType, aConnected );

    HWRMFmTxCommand::TConnStatusData data;
    data.iErrorCode = KErrNone;
    data.iConnected = iConnected.Value() ? ETrue : EFalse;
    HWRMFmTxCommand::TConnStatusPackage pckg( data );
    TRAPD( err, iResponseCallback->EventL( HWRMFmTxCommand::ETxWatcherConnIndId, pckg ) );
    
    LOG_IF_ERROR1( err, "CHWRMFmtxWatcherPlugin::HandleConnectionChange: err=%d", err );
    }
