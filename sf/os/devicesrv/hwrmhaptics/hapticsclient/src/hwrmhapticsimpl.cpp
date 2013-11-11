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
* Description:  Implementation of haptics client.
*
*/


#include <hwrmhapticspacketizer.h>
#include <hwrmhapticseffectdatagetter.h>

#include "hwrmhapticsimpl.h"
#include "hwrmhapticstrace.h"
#include "hwrmhapticssession.h" // common SendReceive
#include "hwrmhapticsclientserver.h"  // Cmd Ids
#include "hwrmhapticsivtdatacache.h"
#include "hwrmhapticsstatusobserver.h"
#include "hwrmhapticsclientserver.h"

#if defined(_DEBUG)
_LIT( KPanic, "HWRMHapticsClient" );
#endif

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsImpl::CHWRMHapticsImpl()
    : iDeviceHandle( 0 ), iOpenedActuator( EHWRMLogicalActuatorLast ),
      iEffectHandlePckg( 0 )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::CHWRMHapticsImpl()" ) ) );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ConstructL( 
    MHWRMHapticsObserver* aHapticsCallback, 
    MHWRMHapticsActuatorObserver* aActuatorCallback )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ConstructL()" ) ) );

    User::LeaveIfError( iClient.Connect() );
    
    FinalizeL( aHapticsCallback, aActuatorCallback );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ConstructL() - return" ) ) );    
    }
    
// ---------------------------------------------------------------------------
// Asynchronous construction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ConstructL(
    MHWRMHapticsObserver* aHapticsCallback,
    MHWRMHapticsActuatorObserver* aActuatorCallback,
    TRequestStatus& aStatus )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ConstructL() - ASYNC" ) ) );

    iClient.Connect( aStatus );

    FinalizeL( aHapticsCallback, aActuatorCallback );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ConstructL() - ASYNC - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsImpl::~CHWRMHapticsImpl()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::~CHWRMHapticsImpl()" ) ) );

    if( iEffectDataGetter )
        {
        delete iEffectDataGetter;
        iEffectDataGetter = NULL;
        } 

    if( iPacketizer )
        {
        delete iPacketizer;
        iPacketizer = NULL;
        } 

    if ( iIVTDataCache )
        {
        delete iIVTDataCache;
        iIVTDataCache = NULL;
        }

    iReqData.Close();
    
    // Inform session that we are cleaning up now. Trace errors.
    if ( iClient.Handle() )
        {
        TInt err = iClient.ExecuteOperation( EHWRMHapticsCleanup );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::~CHWRMHapticsImpl - Cleanup error code: %d" ), err ) );
        }

    // destroy observer after cleanup command, since it may receive
    // a response from the server during cleanup command execution
    if ( iStatusObserver )
        {
        delete iStatusObserver; // will also cancel active object
        iStatusObserver = NULL;
        }

    iClient.Close();
    
    REComSession::FinalClose();

    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::~CHWRMHapticsImpl - return" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Opens an actuator using the given logical actuator type.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::OpenActuatorL( THWRMLogicalActuators aActuator )
    {
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_OPENACTUATORL 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::OpenActuatorL() - aActuator(%d)" ), aActuator ) );

    // check that aActuator contains one of enumeration value
    TBool validActuator = EFalse;
    for ( TInt n = EHWRMLogicalActuatorAny; n < EHWRMLogicalActuatorLast && !validActuator; n <<= 1 )
        {
        if ( aActuator == n )
            {
            validActuator = ETrue;
            }
        }
    if ( !validActuator )
        {
        User::Leave( KErrArgument );
        }

    // leave if some actuator is already opened
    if ( iOpenedActuator != EHWRMLogicalActuatorLast )
        {
        if ( aActuator == iOpenedActuator )
            {
            User::Leave( KErrAlreadyExists );
            }
        else
            {
            User::Leave( KErrInUse );
            }
        }

    if( iPacketizer )
        {
        delete iPacketizer;
        iPacketizer = NULL;
        }

    // create the packetizer instance
    iPacketizer = CHWRMHapticsPacketizer::NewL( aActuator );

    // get request data for opening an actuator
    iReqData.Close();
    User::LeaveIfError( iPacketizer->EncOpenDeviceReq( aActuator, iReqData ) );
    
    // initialize device handle before command execution
    iDeviceHandle = 0;

    // device handle is out-param to command
    TPckg<TInt> deviceHandlePckg( iDeviceHandle );

    User::LeaveIfError( iClient.ExecuteOperation( 
                    EHWRMHapticsOpenActuator, 
                    TIpcArgs( &iReqData, &deviceHandlePckg, aActuator ) ) );
    
    // opened actuator successfully, store the actuator type
    iOpenedActuator = aActuator;
    
    API_TRACE( _L( "CHWRMHapticsImpl::OpenActuatorL() - return" ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_OPENACTUATORL 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Fetches the supported logical actuator types.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::SupportedActuators( TUint32& aActuators )
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::SupportedActuators() - aActuators(%d)" ), aActuators ) );
    
    // data package, which should include the fetched data on return
    TPckg<TUint32> actuatorInfoPckg( aActuators );

    // send command to server
    TInt err = iClient.ExecuteOperation( EHWRMHapticsSuppActuators,
                                         TIpcArgs( &actuatorInfoPckg ) );

    API_TRACE( ( _L( "CHWRMHapticsImpl::iSupportedActuators - %d" ), aActuators ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::SupportedActuators() - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Reserves haptics feature exclusively for this client.
// This is a convenience frontend to the below ReserveHapticsL
// with TBool argument overload.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ReserveHapticsL()
    {
    ReserveHapticsL( EFalse );
    }

// ---------------------------------------------------------------------------
// Reserves haptics feature exclusively for this client.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ReserveHapticsL( TBool aForceNoCCoeEnv )
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::ReserveHapticsL(0x%x)" ), aForceNoCCoeEnv ) );
    
    // We have not used CONE solution here because it is on middleware
    // layer. While hwrmhaptics is on OS layer.  We shall find OS layer
    // replacement for observing foreground/background events.

    /* 
    if ( !aForceNoCCoeEnv )
        {
        // Check that CCoeEnv exists
        CCoeEnv* env = CCoeEnv::Static();
        if ( env )
            {
            // Check that we are on foreground (i.e. we have keyboard focus.)
            // There can be windows on top of our application that 
            // have explicitly disabled keyboard focus,
            // but then again the foreground/background observer will not 
            // kick in in those cases either.
            // -> We can use focus to determine foreground status here.
            if ( env->WsSession().GetFocusWindowGroup() != 
                 env->RootWin().Identifier() )
                {
                COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ReserveHapticsL - Application not focused, reserve denied." ) ) );
                User::Leave( KErrNotReady );
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::ReserveHapticsL - No CCoeEnv available" ) ) );
            User::Leave( KErrBadHandle );
            }
        }
    */

    // Do the actual reservation
    User::LeaveIfError( DoReserveHaptics( aForceNoCCoeEnv ) );

    API_TRACE( ( _L( "CHWRMHapticsImpl::ReserveHapticsL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Releases haptics feature if it was previously reserved for this client.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ReleaseHaptics()
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::ReleaseHaptics()" ) ) );
    
    DoReleaseHaptics();
    iAutoReserve = EFalse;

    API_TRACE( ( _L( "CHWRMHapticsImpl::ReleaseHaptics - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Returns the current haptics status.
// ---------------------------------------------------------------------------
//
MHWRMHapticsObserver::THWRMHapticsStatus 
CHWRMHapticsImpl::HapticsStatus() const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::HapticsStatus()" )) );
    
    MHWRMHapticsObserver::THWRMHapticsStatus status = 
        MHWRMHapticsObserver::EHWRMHapticsStatusAvailable;
    
    if ( iStatusObserver )
        {
        // ask the current status from the observer
        status = iStatusObserver->CurrentStatus();
        }
    else
        {
        TPckg<MHWRMHapticsObserver::THWRMHapticsStatus> statusPckg( status );
        
        // no observer available, request status from the server
        iClient.ExecuteOperation( EHWRMHapticsGetStatus,
                                  TIpcArgs( &statusPckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::HapticsStatus - return 0x%x" ), status ) );
    
    return status;
    }

// ---------------------------------------------------------------------------
// SetDeviceProperty() - TInt overload for aDevicePropertyValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::SetDeviceProperty( TInt aDevicePropertyType, 
                                          TInt aDevicePropertyValue )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - TInt overloaded - aDevicePropertyType(%d), aDevicePropertyValue(%d)" ), aDevicePropertyType, aDevicePropertyValue ) );

    TInt err( KErrNone );
    switch ( aDevicePropertyType )
        {
        case CHWRMHaptics::EHWRMHapticsPriority:
            if ( aDevicePropertyValue < KHWRMHapticsMinDevicePriority ||
                 aDevicePropertyValue > KHWRMHapticsMaxDevicePriority )
                {
                API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - argument is not in range (EHWRMHapticsPriority)" ) ) );
                err = KErrArgument;
                }
            break;    
        case CHWRMHaptics::EHWRMHapticsDisableEffects:
            // any value is accepted
            break;
        case CHWRMHaptics::EHWRMHapticsStrength: // flow through
        case CHWRMHaptics::EHWRMHapticsMasterStrength:
            if ( aDevicePropertyValue < KHWRMHapticsMinStrength || 
                 aDevicePropertyValue > KHWRMHapticsMaxStrength )
                {
                API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - argument is not in range (EHWRMHaptics(Master)Strength)" ) ) );
                err = KErrArgument;
                }
            break;    
        default:             
            API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - invalid property type" ) ) );
            err = KErrArgument;
        }
    
    iReqData.Close(); 
    
    // encode request package
    if ( KErrNone == err )
        {
        if ( aDevicePropertyType == CHWRMHaptics::EHWRMHapticsDisableEffects )
            {
            err = iPacketizer->EncSetDevicePropertyBoolReq( 
                    iDeviceHandle, aDevicePropertyValue,
                    aDevicePropertyType, iReqData );
            }
        else
            {
            err = iPacketizer->EncSetDevicePropertyIntReq( 
                    iDeviceHandle, aDevicePropertyValue,
                    aDevicePropertyType, iReqData );
            }
    
        // send command to haptics server
        if ( KErrNone == err )
            {
            err = iClient.ExecuteOperation( EHWRMHaptics, 
                                            TIpcArgs( &iReqData ) );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - return %d" ), err ) );    

    return err;
    }

// ---------------------------------------------------------------------------
// SetDeviceProperty() - TDesC8 overload for aDevicePropertyValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::SetDeviceProperty( TInt aDevicePropertyType, 
                                          const TDesC8& aDevicePropertyValue )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - const TDesC8 overloaded - aDevicePropertyType(%d), aDevicePropertyValue( 0x%x)" ), aDevicePropertyType, &aDevicePropertyValue ) );

    TInt err( KErrNone );

    // check property type, only license key setting supported (for
    // string value type properties)
    if( CHWRMHaptics::EHWRMHapticsLicensekey != aDevicePropertyType )
        {
        API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - invalid property type" ) ) );
        err = KErrArgument;
        }
    
    
    if ( KErrNone == err )
        {
        // encode request package
        iReqData.Close(); 
        err = iPacketizer->EncSetDevicePropertyStringReq(
                                    iDeviceHandle,
                                    aDevicePropertyValue,
                                    aDevicePropertyType,
                                    iReqData );

        // send command to haptics server
        if ( KErrNone == err )
            {            
            err = iClient.ExecuteOperation( 
                       EHWRMHapticsSetLicenseProp, 
                       TIpcArgs( &iReqData, aDevicePropertyValue.Length() ) );                     
	         }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::SetDeviceProperty() - const TDesC8 overloaded - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// GetDeviceProperty() - TInt overload for aDevicePropertyValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetDeviceProperty( TInt aDevicePropertyType, 
                                          TInt& aDevicePropertyValue )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceProperty() - TInt overloaded - aDevicePropertyType(%d), aDevicePropertyValue(%d)" ), aDevicePropertyType, aDevicePropertyValue ) );

    TInt err( KErrNone );
    iReqData.Close();

    if ( aDevicePropertyType == CHWRMHaptics::EHWRMHapticsDisableEffects )
        {
        err = iPacketizer->EncGetDevicePropertyBoolReq( 
                   iDeviceHandle, aDevicePropertyType, iReqData );
        }
    else
        {
        err = iPacketizer->EncGetDevicePropertyIntReq( 
                   iDeviceHandle, aDevicePropertyType, iReqData );
        }    
       
    if ( KErrNone == err )
        {
        TPckg<TInt> devicePropertyValuePckg( aDevicePropertyValue );
        
        err = iClient.ExecuteOperation( 
            EHWRMHaptics, 
            TIpcArgs( &iReqData, &devicePropertyValuePckg ) );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceProperty() - TInt overloaded - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// GetDeviceProperty() - TDes8 overload for aDevicePropertyValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetDeviceProperty( TInt aDevicePropertyType, 
                                          TDes8& aDevicePropertyValue )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceProperty() - TDes8 overloaded - aDevicePropertyType(%d), aDevicePropertyValue(0x%x)" ), aDevicePropertyType, &aDevicePropertyValue ) );

    TInt err( KErrNone );

    if( aDevicePropertyValue.MaxLength() < MaxPropertyStringLength() ) 
        {
        err = KErrArgument;
        }
    
    if ( KErrNone == err )
        {
        // encode the request
        iReqData.Close();
        err = iPacketizer->EncGetDevicePropertyStringReq( 
                    iDeviceHandle, aDevicePropertyType, iReqData );
    
        if ( KErrNone == err )
            {
            err = iClient.ExecuteOperation( 
                    EHWRMHaptics, 
                    TIpcArgs( &iReqData, &aDevicePropertyValue ) );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceProperty() - TDes8 overloaded - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// GetDeviceCapability() - TInt overload for aDeviceCapabilityValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetDeviceCapability( TInt aDeviceCapabilityType, 
                                            TInt& aDeviceCapabilityValue )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceCapability() - aDeviceCapabilityType(%d), aDeviceCapabilityValue(%d)" ), aDeviceCapabilityType, aDeviceCapabilityValue ) );
    
    // default return value in case actuator has not been opened
    TInt err = KErrNotReady;
    
    if ( iOpenedActuator != EHWRMLogicalActuatorLast )
        {
        iReqData.Close();
        err = iPacketizer->EncGetDeviceCapabilityIntReq(
                 iOpenedActuator, aDeviceCapabilityType, iReqData );
        
        TPckg<TInt> deviceCapabilityValuePckg( aDeviceCapabilityValue );
        
        if ( KErrNone == err )
            {
            err = iClient.ExecuteOperation( 
                EHWRMHaptics, 
                TIpcArgs( &iReqData, &deviceCapabilityValuePckg ) );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceCapability() - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// GetDeviceCapability() - TDes8 overload for aDeviceCapabilityValue parameter
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetDeviceCapability( TInt aDeviceCapabilityType, 
                                            TDes8& aDeviceCapabilityValue )
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceCapability() - aDeviceCapabilityType(%d), aDeviceCapabilityValue(0x%x)" ), aDeviceCapabilityType, &aDeviceCapabilityValue ) );
    
    // default return value in case actuator has not been opened
    TInt err = KErrNotReady;
    
    if ( iOpenedActuator != EHWRMLogicalActuatorLast )
        {
        if( aDeviceCapabilityValue.MaxLength() < MaxCapabilityStringLength() ) 
            {
            err = KErrArgument;
            }
        else
            {
            iReqData.Close();
            err = iPacketizer->EncGetDeviceCapabilityStringReq( 
                    iOpenedActuator, aDeviceCapabilityType, iReqData );
            
            if ( KErrNone == err )
                {
                err = iClient.ExecuteOperation( 
                        EHWRMHaptics, 
                        TIpcArgs( &iReqData, &aDeviceCapabilityValue ) );
                }
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetDeviceCapability() - TInt overloaded - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Retrieves the status of an effect (playing, not playing, paused).
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectState( TInt aEffectHandle, 
                                       TInt& aEffectState )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectState() - aEffectHandle(%d) - aEffectState(%d)" ), aEffectHandle, aEffectState ) );

    TInt err( KErrNone ); 

    iReqData.Close();
    err = iPacketizer->EncGetEffectStateReq( 
            iDeviceHandle, aEffectHandle, iReqData );
    
    if ( KErrNone == err ) 
        {
        TPckg<TInt> effectStatePckg( aEffectState );
        err = iClient.ExecuteOperation( EHWRMHaptics, 
                                        TIpcArgs( &iReqData, 
                                                  &effectStatePckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectState() - return" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Creates a new streaming effect and returns a new handle for it.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::CreateStreamingEffect( TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_CREATESTREAMING 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::CreateStreamingEffect() - aEffectHandle(%d)" ), aEffectHandle ) );

    TInt err( KErrNone );

    iReqData.Close();
    err = iPacketizer->EncCreateStreamingEffectReq( iDeviceHandle, iReqData );
    
    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHaptics, 
                                        TIpcArgs( &iReqData, 
                                                  &effectHandlePckg ) );
        }
        
    API_TRACE( ( _L( "CHWRMHapticsImpl::CreateStreamingEffect() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_CREATESTREAMING 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Plays a streaming sample given the buffer defining the effect.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayStreamingSample( TInt aEffectHandle, 
                                            const TDesC8& aStreamingSample )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLE_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSample() - SYNC - aEffectHandle(%d), aStreamingSample(0x%x)" ), aEffectHandle, &aStreamingSample ) );

    TInt err( KErrNone );
    if( aStreamingSample.Size() > MaxStreamingSampleSize() )
        {
        err = KErrArgument;
        }
    
    iReqData.Close();
    
    if ( KErrNone == err ) 
        {
        err = iPacketizer->EncPlayStreamingSampleReq( 
                iDeviceHandle, aStreamingSample, aEffectHandle, iReqData );
        }

    if ( KErrNone == err ) 
        {
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect, 
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSample() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLE_SYNC 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Plays a streaming sample given the buffer defining the effect. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayStreamingSample( TInt aEffectHandle, 
                                            const TDesC8& aStreamingSample, 
                                            TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLE_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSample() - ASYNC -aEffectHandle(%d), aStreamingSample(0x%x), aStatus(%d)" ), aEffectHandle, &aStreamingSample, aStatus.Int() ) );

    if( aStreamingSample.Size() > MaxStreamingSampleSize() )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrArgument );
        }
    else
        {
        iReqData.Close();
            
        if ( KErrNone == iPacketizer->EncPlayStreamingSampleReq( 
             iDeviceHandle, aStreamingSample, aEffectHandle, iReqData ) )
            {
            iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect, 
                                           TIpcArgs( &iReqData ),
                                           aStatus );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSample() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLE_ASYNC 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Plays a streaming sample with a time offset given the parameters.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayStreamingSampleWithOffset( 
                                            TInt aEffectHandle, 
                                            const TDesC8& aStreamingSample,
                                            TInt aOffsetTime )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLEWITHOFFSET_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSampleWithOffset() - SYNC - aEffectHandle(%d), aStreamingSample(0x%x), aOffsetTime(%d)" ), aEffectHandle, &aStreamingSample, aOffsetTime ) );

    TInt err( KErrNone );
    if( aStreamingSample.Size() > MaxStreamingSampleSize() )
        {
        err = KErrArgument;
        }
    else 
        {
        iReqData.Close();
        err = iPacketizer->EncPlayStreamingSampleWithOffsetReq( 
                iDeviceHandle, aStreamingSample, aOffsetTime, 
                aEffectHandle, iReqData );
    
        if ( KErrNone == err )
            {
            err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect, 
                                            TIpcArgs( &iReqData ) );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSampleWithOffset() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLEWITHOFFSET_SYNC 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Plays a streaming sample with a time offset given the parameters. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayStreamingSampleWithOffset( 
                                            TInt aEffectHandle, 
                                            const TDesC8& aStreamingSample,
                                            TInt aOffsetTime, 
                                            TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLEWITHOFFSET_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSampleWithOffset() - ASYNC - aEffectHandle(%d), aStreamingSample(0x%x), aOffsetTime(%d), aStatus(%d)" ), aEffectHandle, &aStreamingSample, aOffsetTime, aStatus.Int() ) );
    
    if( aStreamingSample.Size() > MaxStreamingSampleSize() )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrArgument );
        }
    else
        {
        iReqData.Close();

        if ( KErrNone == iPacketizer->EncPlayStreamingSampleWithOffsetReq( 
                            iDeviceHandle, aStreamingSample, aOffsetTime, 
                            aEffectHandle, iReqData ) )
            {
            iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect, 
                                           TIpcArgs( &iReqData ),
                                           aStatus );
            }
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayStreamingSampleWithOffset() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYSTREAMINGSAMPLEWITHOFFSET_ASYNC 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Destroys the streaming effect so it is not recognized by the API.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::DestroyStreamingEffect( TInt aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_DESTROYSTREAMING 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::DestroyStreamingEffect() - aEffectHandle(%d)" ), aEffectHandle ) );

    TInt err( KErrNone );
    iReqData.Close(); 
    err = iPacketizer->EncDestroyStreamingEffectReq( 
                iDeviceHandle, aEffectHandle, iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics,
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::DestroyStreamingEffect() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_DESTROYSTREAMING 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Modifies playing MagSweep type effect. Sync version.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::ModifyPlayingMagSweepEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsMagSweepEffect& aEffect )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYMAGSWEEP_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingMagSweepEffect() - SYNC - aEffectHandle(%d)" ), aEffectHandle ) );

    iReqData.Close();
    TInt err = iPacketizer->EncModifyPlayingMagSweepEffectReq( 
                iDeviceHandle, aEffectHandle, aEffect, iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics,
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingMagSweepEffect() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYMAGSWEEP_SYNC 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Modifies playing MagSweep type effect. Async version.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ModifyPlayingMagSweepEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsMagSweepEffect& aEffect, 
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYMAGSWEEP_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingMagSweepEffect() - ASYNC - aEffectHandle(%d), aStatus(%d)" ), aEffectHandle, aStatus.Int() ) );
   
    iReqData.Close();
    TInt err = iPacketizer->EncModifyPlayingMagSweepEffectReq( 
                iDeviceHandle, aEffectHandle, aEffect, iReqData );

    if ( KErrNone == err )
        {
        iClient.ExecuteAsyncOperation( EHWRMHaptics,
                                       TIpcArgs( &iReqData ),
                                       aStatus );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingMagSweepEffect() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYMAGSWEEP_ASYNC 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Modifies playing Periodic type effect. Sync version.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::ModifyPlayingPeriodicEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsPeriodicEffect& aEffect )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYPERIODIC_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingPeriodicEffect() - SYNC - aEffectHandle(%d)" ), aEffectHandle ) );

    iReqData.Close();
    TInt err = iPacketizer->EncModifyPlayingPeriodicEffectReq(
                iDeviceHandle, aEffectHandle, aEffect, iReqData );

    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics,
                                        TIpcArgs( &iReqData ) );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingPeriodicEffect() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYPERIODIC_SYNC 0" ) ) );

    return err;
    }
 
// ---------------------------------------------------------------------------
// Modifies playing Periodic type effect. Async version.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ModifyPlayingPeriodicEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsPeriodicEffect& aEffect, 
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYPERIODIC_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingPeriodicEffect() - ASYNC - aEffectHandle(%d), aStatus(%d)" ), aEffectHandle, aStatus.Int() ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncModifyPlayingPeriodicEffectReq( 
                iDeviceHandle, aEffectHandle, aEffect, iReqData );
    
    if ( KErrNone == err )
        {
        iClient.ExecuteAsyncOperation( EHWRMHaptics, 
                                       TIpcArgs( &iReqData ),
                                       aStatus );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::ModifyPlayingPeriodicEffect() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_MODIFYPERIODIC_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Loads the given data buffer to be used for playing effects. Returns 
// a filehandle, which can be used to reference to that data. If the data has
// already been loaded before, the data is already located in the cache, and
// it is not loaded again. The filehandle of the data is returned in any
// successful case.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::LoadEffectData( const TDesC8& aData, TInt& aFileHandle )
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::LoadEffectData()" ) ) );

    TInt err = KErrNone;
    aFileHandle = iIVTDataCache->ClientFileHandle( aData );
    if ( aFileHandle <= 0 )
        {
        // store effect data to cache
        err = iIVTDataCache->AddCacheItem( aData, aFileHandle );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::LoadEffectData() - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Delete effect data from internal data cache referenced by file handle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::DeleteEffectData( TInt aFileHandle )
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::DeleteEffectData()" ) ) );
    
    TInt err = iIVTDataCache->RemoveCacheItem( aFileHandle );
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::DeleteEffectData() - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Delete all effect datas from internal data cache.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::DeleteAllEffectData()
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::DeleteAllEffectData()" ) ) );
    
    iIVTDataCache->Reset();
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::DeleteAllEffectData() - return" ) ) );

    return KErrNone;
    }
 
// ---------------------------------------------------------------------------
// Plays an effect defined in loaded effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayEffect( TInt aFileHandle, TInt aEffectIndex, 
                                   TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_HANDLE_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - SYNC - NO_IVT_DATA - aEffectIndex(%d), aEffectHandle(%d)" ), aEffectIndex, aEffectHandle ) );

    TInt err( KErrNone );
    TBool alreadyLoaded( iIVTDataCache->IsLoaded( aFileHandle ) );
    iReqData.Close();

    // load data, if not currently loaded
    if( alreadyLoaded )
        {
        err = iPacketizer->EncPlayEffectNoDataReq( iDeviceHandle,
                                                   aEffectIndex, 
                                                   iReqData );
        }
    else
        {
        const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
        if ( cachedIVTData )
            {
            err = iPacketizer->EncPlayEffectIncludeEffectDataReq(
                    iDeviceHandle, *cachedIVTData, aEffectIndex, iReqData );
            }
         else
            {
            err = KErrBadHandle;
            }
        }

    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect, 
                                        TIpcArgs( &iReqData,
                                                  &effectHandlePckg ) );
        }

    if ( KErrNone == err && !alreadyLoaded )
        {
        // loaded new data to player
        iIVTDataCache->UpdateCacheItem( aFileHandle, ETrue );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - SYNC - NO_IVT_DATA - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_HANDLE_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Plays an effect defined in loaded data data buffer. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayEffect( TInt aFileHandle, TInt aEffectIndex, 
                                   TInt& aEffectHandle, 
                                   TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_HANDLE_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - ASYNC - NO_IVT_DATA - aEffectIndex(%d), aEffectHandle(%d), aStatus(%d)" ), aEffectIndex, aEffectHandle, aStatus.Int() ) );

    TInt err( KErrNone );
    TBool alreadyLoaded( iIVTDataCache->IsLoaded( aFileHandle ) );
    iReqData.Close();
    
    // load data, if not currently loaded
    if( alreadyLoaded )
        {
        err = iPacketizer->EncPlayEffectNoDataReq( iDeviceHandle,
                                                   aEffectIndex, 
                                                   iReqData );
        }
    else
        {
        const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
        if ( cachedIVTData )
            {
            err = iPacketizer->EncPlayEffectIncludeEffectDataReq(
                    iDeviceHandle,*cachedIVTData, aEffectIndex, iReqData );
            }
         else
            {
            err = KErrBadHandle;
            }
        }
        
    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        TIpcArgs ipcArgs( &iReqData, &iEffectHandlePckg );
        
        if ( alreadyLoaded ||
             !iIVTDataCache->UpdateCacheItemListener( 
                aFileHandle, aStatus, &iClient, ipcArgs ) )
            {
            // make async call with the client's status
            iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                           ipcArgs,
                                           aStatus );
            }
        }
        
    // complete request, if error occured
    if ( err )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, err );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - ASYNC - NO_IVT_DATA - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_HANDLE_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Repeatedly plays a Timeline effect defined in loaded effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayEffectRepeat( TInt aFileHandle, 
                                         TInt aEffectIndex, 
                                         TUint8 aRepeat, 
                                         TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_HANDLE_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - SYNC - NO_IVT_DATA - aEffectIndex(%d), aRepeat(%d), aEffectHandle(%d)" ), aEffectIndex, aRepeat, aEffectHandle ) );

    TInt err( KErrNone );
    TBool alreadyLoaded( iIVTDataCache->IsLoaded( aFileHandle ) );
    iReqData.Close();
    
    // load data, if not currently loaded
    if( alreadyLoaded )
        {
        err = iPacketizer->EncPlayEffectRepeatNoDataReq( iDeviceHandle,
                                                         aEffectIndex,
                                                         aRepeat, 
                                                         iReqData );
        }
    else
        {
        const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
        if ( cachedIVTData )
            {
            err = iPacketizer->EncPlayEffectRepeatIncludeEffectDataReq( 
             iDeviceHandle, *cachedIVTData, aEffectIndex, aRepeat, iReqData );
            }
         else
            {
            err = KErrBadHandle;
            }
        }

    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect,
                                        TIpcArgs( &iReqData,
                                                  &effectHandlePckg ) );
        }
     
    if ( KErrNone == err && !alreadyLoaded )
        {
        // loaded new data to player
        iIVTDataCache->UpdateCacheItem( aFileHandle, ETrue );
        }
            
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - SYNC - NO_IVT_DATA - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_HANDLE_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Repeatedly plays a Timeline effect defined in loaded effect data buffer.
// Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayEffectRepeat( TInt aFileHandle, 
                                         TInt aEffectIndex, 
                                         TUint8 aRepeat, 
                                         TInt& aEffectHandle, 
                                         TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_HANDLE_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - ASYNC - NO_IVT_DATA - aEffectIndex(%d), aRepeat(%d), aEffectHandle(%d), aStatus(%d)" ), aEffectIndex, aRepeat, aEffectHandle, aStatus.Int() ) );

    TInt err( KErrNone );
    TBool alreadyLoaded( iIVTDataCache->IsLoaded( aFileHandle ) );
    iReqData.Close();

    // load data, if not currently loaded
    if( alreadyLoaded )        
        {
        err = iPacketizer->EncPlayEffectRepeatNoDataReq( iDeviceHandle,
                                                         aEffectIndex,
                                                         aRepeat,
                                                         iReqData );
        }
    else
        {
        const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
        if ( cachedIVTData )
            {
            err = iPacketizer->EncPlayEffectRepeatIncludeEffectDataReq(
             iDeviceHandle, *cachedIVTData, aEffectIndex, aRepeat, iReqData );
            }
        else
            {
            err = KErrBadHandle;
            }
        }    

    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        TIpcArgs ipcArgs( &iReqData, &iEffectHandlePckg );
        
        if ( alreadyLoaded ||
             !iIVTDataCache->UpdateCacheItemListener( 
                aFileHandle, aStatus, &iClient, ipcArgs ) )
            {
            // make async call if packages were created successfully
            iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                           ipcArgs,
                                           aStatus );
            }
        }

    if ( err )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, err );
        }
        
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - ASYNC - NO_IVT_DATA - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_HANDLE_ASYNC 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Plays an effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayEffect( const TDesC8& aData, 
                                   TInt aEffectIndex, 
                                   TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_DATA_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - SYNC - aData(0x%x), aEffectIndex(%d), aEffectHandle(%d)" ), &aData, aEffectIndex, aEffectHandle ) );

    iReqData.Close();
    TInt err = iPacketizer->EncPlayEffectIncludeEffectDataReq(
                    iDeviceHandle, aData, aEffectIndex, iReqData );
    
    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect,
                                        TIpcArgs( &iReqData, 
                                                  &effectHandlePckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_DATA_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Plays an effect defined in effect data buffer. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayEffect( const TDesC8& aData, 
                                   TInt aEffectIndex, 
                                   TInt& aEffectHandle, 
                                   TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_DATA_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayIVTEffect() - ASYNC - aData(0x%x), aEffectIndex(%d), aEffectHandle(%d), aStatus(%d)" ), &aData, aEffectIndex, aEffectHandle, aStatus.Int() ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncPlayEffectIncludeEffectDataReq( 
                    iDeviceHandle, aData, aEffectIndex, iReqData );
    
    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        
        // make async call if packages were created successfully
        iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                       TIpcArgs( &iReqData,
                                                 &iEffectHandlePckg ),
                                       aStatus );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffect() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECT_DATA_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Repeatedly plays a Timeline effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayEffectRepeat( const TDesC8& aData, 
                                         TInt aEffectIndex, 
                                         TUint8 aRepeat, 
                                         TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_DATA_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - SYNC - aData(0x%x), aEffectIndex(%d), aRepeat(%d), aEffectHandle(%d)" ), &aData, aEffectIndex, aRepeat, aEffectHandle ) );

    iReqData.Close();
    TInt err = iPacketizer->EncPlayEffectRepeatIncludeEffectDataReq( 
                iDeviceHandle, aData, aEffectIndex, aRepeat, iReqData );
    
    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect,
                                        TIpcArgs( &iReqData, 
                                                  &effectHandlePckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_DATA_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Repeatedly plays a Timeline effect defined in effect data buffer. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayEffectRepeat( const TDesC8& aData, 
                                         TInt aEffectIndex, 
                                         TUint8 aRepeat, 
                                         TInt& aEffectHandle, 
                                         TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_DATA_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - ASYNC - aData(0x%x), aEffectIndex(%d), aRepeat(%d), aEffectHandle(%d), aStatus(%d)" ), &aData, aEffectIndex, aRepeat, aEffectHandle, aStatus.Int() ) );

    iReqData.Close();
    TInt err = iPacketizer->EncPlayEffectRepeatIncludeEffectDataReq(
                iDeviceHandle, aData, aEffectIndex, aRepeat, iReqData );
        
    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        
        // make async call if packages were created successfully
        iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                       TIpcArgs( &iReqData,
                                                 &iEffectHandlePckg ),
                                       aStatus );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayEffectRepeat() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYEFFECTREPEAT_DATA_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Plays a MagSweep effect given the parameters defining the effect.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayMagSweepEffect( 
        const THWRMHapticsMagSweepEffect& aEffect, 
        TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYMAGSWEEP_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayMagSweepEffect() - SYNC - aEffectHandle(%d)" ), aEffectHandle ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncPlayMagSweepEffectReq( 
                    iDeviceHandle, aEffect, iReqData );
 
    if ( KErrNone == err )
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect, 
                                        TIpcArgs( &iReqData, 
                                                  &effectHandlePckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayMagSweepEffect() - SYNC - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYMAGSWEEP_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Plays a MagSweep effect given the parameters defining the effect. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayMagSweepEffect( 
        const THWRMHapticsMagSweepEffect& aEffect,
        TInt& aEffectHandle, 
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYMAGSWEEP_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayMagSweepEffect() - ASYNC - aEffectHandle(%d), aStatus(%d)" ), aEffectHandle, aStatus.Int() ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncPlayMagSweepEffectReq( 
                    iDeviceHandle, aEffect, iReqData );

    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        
        // make async call if packages were created successfully
        iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                       TIpcArgs( &iReqData,
                                                 &iEffectHandlePckg ),
                                       aStatus );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayMagSweepEffect() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYMAGSWEEP_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Plays a Periodic effect given the parameters defining the effect.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PlayPeriodicEffect( 
        const THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYPERIODIC_SYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayPeriodicEffect() - SYNC - aEffectHandle(%d)" ), aEffectHandle ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncPlayPeriodicEffectReq( 
                        iDeviceHandle, aEffect, iReqData );
    
    if ( KErrNone == err)
        {
        TPckg<TInt> effectHandlePckg( aEffectHandle );
        err = iClient.ExecuteOperation( EHWRMHapticsPlayEffect,
                                        TIpcArgs( &iReqData, 
                                                  &effectHandlePckg ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayPeriodicEffect() - SYNC - return %d"), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYPERIODIC_SYNC 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Plays a Periodic effect given the parameters defining the effect. Async.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::PlayPeriodicEffect( 
        const THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle, 
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        TRequestStatus* tmp = &aStatus;
        User::RequestComplete( tmp, EPanicNotReady );

        return;
        }

    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYPERIODIC_ASYNC 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayPeriodicEffect() - ASYNC - aEffectHandle(%d), aStatus(%d)" ), aEffectHandle, aStatus.Int() ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncPlayPeriodicEffectReq( 
                    iDeviceHandle, aEffect, iReqData );
    
    if ( KErrNone == err )
        {
        // reset effect handle package
        ResetIntPckg( aEffectHandle );
        
        // make async call if packages were created successfully
        iClient.ExecuteAsyncOperation( EHWRMHapticsPlayEffect,
                                       TIpcArgs( &iReqData, 
                                                 &iEffectHandlePckg ),
                                       aStatus );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PlayPeriodicEffect() - ASYNC - return" ) ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PLAYPERIODIC_ASYNC 0" ) ) );
    }
 
// ---------------------------------------------------------------------------
// Pauses an effect, which is currently playing. The effect is identified
// using the given effect handle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::PausePlayingEffect( TInt aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "CHWRMHapticsImpl::PausePlayingEffect() - aEffectHandle(%d)" ), aEffectHandle ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PAUSEEFFECT 1" ) ) );

    iReqData.Close();
    TInt err = iPacketizer->EncPausePlayingEffectReq(
                        iDeviceHandle, aEffectHandle, iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics,
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::PausePlayingEffect() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_PAUSEEFFECT 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Resumes playing a paused effect. The effect is identified using the 
// given effect handle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::ResumePausedEffect( TInt aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_RESUMEEFFECT 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::ResumePausedEffect() - aEffectHandle(%d)" ), aEffectHandle ) );

    iReqData.Close();
    TInt err = iPacketizer->EncResumePausedEffectReq( 
                    iDeviceHandle, aEffectHandle, iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics, 
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::ResumePausedEffect() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_RESUMEEFFECT 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Stops playing an effect, which is currently playing (or paused). 
// The effect is identified using the given effect handle.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::StopPlayingEffect( TInt aEffectHandle )
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_STOPEFFECT 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::StopPlayingEffect() - aEffectHandle(%d)" ), aEffectHandle ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncStopPlayingEffectReq( 
                    iDeviceHandle, aEffectHandle, iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics,
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::StopPlayingEffect() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_STOPEFFECT 0" ) ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Stops all effects, which are currently playing (or in paused state) by this
// haptics client.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::StopAllPlayingEffects()
    {
    __ASSERT_DEBUG( iPacketizer, User::Panic( KPanic, EPanicNotReady ) );
    if ( !iPacketizer )
        {
        return EPanicNotReady;
        }
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_STOPALLEFFECTS 1" ) ) );
    API_TRACE( ( _L( "CHWRMHapticsImpl::StopAllPlayingEffects()" ) ) );
    
    iReqData.Close();
    TInt err = iPacketizer->EncStopAllPlayingEffectsReq( iDeviceHandle, 
                                                         iReqData );
    
    if ( KErrNone == err )
        {
        err = iClient.ExecuteOperation( EHWRMHaptics, 
                                        TIpcArgs( &iReqData ) );
        }
    
    API_TRACE( ( _L( "CHWRMHapticsImpl::StopAllPlayingEffects() - return %d" ), err ) );
    API_TRACE( ( _L( "e_HWRMHAPTICS_CLIENT_STOPALLEFFECTS 0" ) ) );
    
    return err;
    }
 
// ---------------------------------------------------------------------------
// Get number of effects from loaded effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectCount( TInt aFileHandle, TInt& aCount ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectCount()" ) ) );
    
    TInt err = KErrBadHandle;
        
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect count
        err = iEffectDataGetter->GetEffectCount( *cachedIVTData, aCount );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectCount() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Get duration of an effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectDuration( TInt aFileHandle,
                                          TInt aEffectIndex,
                                          TInt& aEffectDuration ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectDuration()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect duration
        err = iEffectDataGetter->GetEffectDuration( *cachedIVTData,
                                                     aEffectIndex,
                                                     aEffectDuration );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectDuration() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the index of an effect defined in effect data buffer
// given the name of the effect.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectIndexFromName ( TInt aFileHandle,
                                                const TDesC8& aEffectName,
                                                TInt& aEffectIndex ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectIndexFromName()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
            // get the effect index
            err = iEffectDataGetter->GetEffectIndexFromName( *cachedIVTData,
                                                             aEffectName,
                                                             aEffectIndex );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectIndexFromName() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the type of an effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectType( TInt aFileHandle,
                                      TInt aEffectIndex,
                                      TInt& aEffectType ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectType()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect type
        err = iEffectDataGetter->GetEffectType( *cachedIVTData, 
                                                aEffectIndex,
                                                aEffectType );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectType() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the name of an effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetEffectName( TInt aFileHandle,
                                      TInt aEffectIndex,
                                      TDes8& aEffectName ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectName()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect name
        err = iEffectDataGetter->GetEffectName( *cachedIVTData, 
                                                aEffectIndex,
                                                aEffectName );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetEffectName() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the parameters of a MagSweep effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetMagSweepEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        THWRMHapticsMagSweepEffect& aEffect ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetMagSweepEffectDefinition()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect definition
        err = iEffectDataGetter->GetMagSweepEffectDefinition( *cachedIVTData, 
                                                              aEffectIndex,
                                                              aEffect );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetMagSweepEffectDefinition() - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets the parameters of a Periodic effect defined in effect data buffer.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::GetPeriodicEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        THWRMHapticsPeriodicEffect& aEffect ) const
    {
    API_TRACE( ( _L( "CHWRMHapticsImpl::GetPeriodicEffectDefinition()" ) ) );
    
    TInt err = KErrBadHandle;
    
    const HBufC8* cachedIVTData = iIVTDataCache->IVTData( aFileHandle );
    if ( cachedIVTData )
        {
        // get the effect definition
        err = iEffectDataGetter->GetPeriodicEffectDefinition( *cachedIVTData, 
                                                              aEffectIndex,
                                                              aEffect );
        }

    API_TRACE( ( _L( "CHWRMHapticsImpl::GetPeriodicEffectDefinition() - return %d" ), err ) );
  
    return err;
    }

// ---------------------------------------------------------------------------
// Getter for the value used for representing infinite repeats
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::InfiniteRepeat() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->InfiniteRepeat();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for the value used for representing infinite duration
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::InfiniteDuration() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->InfiniteDuration();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for maximum effect name length
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::MaxEffectNameLength() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->MaxEffectNameLength();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for maximum device name length
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::MaxDeviceNameLength() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->MaxDeviceNameLength();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for maximum capability string length
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::MaxCapabilityStringLength() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->MaxCapabilityStringLength();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for maximum property string length
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::MaxPropertyStringLength() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->MaxPropertyStringLength();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for maximum streaming sample size
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::MaxStreamingSampleSize() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->MaxStreamingSampleSize();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Getter for default device priority
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::DefaultDevicePriority() const
    {
    TInt retValue = KErrNotReady;
    
    if ( iPacketizer )
        {
        retValue = iPacketizer->DefaultDevicePriority();
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// Reserve haptics resource.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsImpl::DoReserveHaptics( TBool aForceNoCCoeEnv )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::DoReserveHaptics(0x%x)" ), aForceNoCCoeEnv ) );
    
    TInt err = KErrNone;
    
    // reserve only if not reserved already
    if ( !iReserved )
        {
        // Execute reservation
        err = iClient.ExecuteOperation( EHWRMHapticsReserve, 
                                        TIpcArgs( aForceNoCCoeEnv ) );

        // Reserve successful, mark as reserved
        iReserved = ETrue;
        }
    
    if ( !aForceNoCCoeEnv )
        {
        iAutoReserve = ETrue;
        }
    
    COMPONENT_TRACE( _L( "CHWRMHapticsImpl::DoReserveHaptics - return" ) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Releases haptics by sending corresponding command to haptics server.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::DoReleaseHaptics()
    {  
    // Only release if reserved
    if ( iReserved )
        {
        // Execute release
        TInt err = iClient.ExecuteOperation( EHWRMHapticsRelease );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsImpl::DoReleaseHaptics - ipc ret: %d" ), err ) );
        if ( KErrNone == err )
            {
            iReserved = EFalse;
            }
        }    
    }

// ---------------------------------------------------------------------------
// Recreates the given integer package using the given value.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::ResetIntPckg( TInt& aNewValue )
    {
    // re-construct the buffer
    (void) new (&iEffectHandlePckg) TPckg<TInt>( aNewValue );
    }

// ---------------------------------------------------------------------------
// Finalizes the construction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsImpl::FinalizeL(
                        MHWRMHapticsObserver* aHapticsCallback,
                        MHWRMHapticsActuatorObserver* aActuatorCallback )
    {
    // create status observer only if a callback for it has been received
    if ( aHapticsCallback || aActuatorCallback )
        {
        iStatusObserver = CHWRMHapticsStatusObserver::NewL( aHapticsCallback,
                                                            aActuatorCallback,
                                                            &iClient );
        }
    
    // create IVT-data cache
    iIVTDataCache = CHWRMHapticsIVTDataCache::NewL();

    // create effect data getter
    iEffectDataGetter = CHWRMHapticsEffectDataGetter::NewL();
    }

// End of file
