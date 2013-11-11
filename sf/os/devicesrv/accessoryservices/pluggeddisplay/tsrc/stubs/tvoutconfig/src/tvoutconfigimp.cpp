/*
 * Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implements the all the virtual functions derived from
 CTvOutConfig  TVOutConfig Dummy Implementation.
 *
 */

//- Include Files  ----------------------------------------------------------

#include "tvoutconfigimp.h"
#include "tvoutstubeventlistener.h"

#include <e32std.h>
#include <AccPolGenericIdDefinitions.h>
#include <AccPolCommonNameValuePairs.h>
#include <AccConfigFileParser.h>
#include <AccConGenericID.h>
#include "trace.h"

//- External Data -----------------------------------------------------------

//- External Function Prototypes --------------------------------------------

//- Constants ---------------------------------------------------------------

//- Macros ------------------------------------------------------------------

//- Global and Local Variables ----------------------------------------------

//- Local Function Prototypes -----------------------------------------------

//- Local Functions ---------------------------------------------------------

//- Member Functions --------------------------------------------------------

//=============================================================================

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
CTvOutConfigImp::CTvOutConfigImp() :
    CTvOutConfig(), iHdmiCableListenerStatus( NULL )
    {
    FUNC_LOG;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
CTvOutConfigImp::~CTvOutConfigImp()
    {
    FUNC_LOG;
	
    iAccControl.CloseSubSession();
    iAccServer.Close();
	
    iConfigurationChunk.Close();
    iConfigSemaphore.Close();
    delete iCableConnectStatusListener;
	delete iAnalogCableConnectStatusListener;
    delete iCopyProtectStatusListener;
    delete iSettingChangeListener;
	iAllDviMode.Reset();
	iSupportedMode.Reset();
    }

//---------------------------------------------------------------------------
/**
 Dummy implementation.
 */
void CTvOutConfigImp::ConstructL()
    {
    FUNC_LOG;
    
    TInt err = iConfigSemaphore.OpenGlobal( 
            KTvOutConfigGlobalSemaphoreName,
            EOwnerProcess );
    if ( KErrNone != err )
        {
        err = iConfigSemaphore.CreateGlobal(
                KTvOutConfigGlobalSemaphoreName,
                0,
                EOwnerProcess );        
        err = iConfigSemaphore.OpenGlobal( 
                KTvOutConfigGlobalSemaphoreName,
                EOwnerProcess );
        User::LeaveIfError( err );
        err = iConfigurationChunk.CreateGlobal(
                KTvOutConfigGlobalChunkName,
                sizeof( STvOutConfigSharedData ), 
                sizeof( STvOutConfigSharedData ),
                EOwnerProcess );        
        err = iConfigurationChunk.OpenGlobal(
                KTvOutConfigGlobalChunkName,
                EFalse, // == Read | Write
                EOwnerProcess );
        User::LeaveIfError( err );
        TUint8* basePtr = iConfigurationChunk.Base();
        User::LeaveIfNull( basePtr );
        iConfig = (STvOutConfigSharedData*)basePtr;
        Mem::FillZ(iConfig, sizeof( STvOutConfigSharedData ) );
        iConfig->TagAssign();
        iConfigSemaphore.Signal();
        }
    else
        {
        err = iConfigurationChunk.OpenGlobal(
                KTvOutConfigGlobalChunkName,
                EFalse, // == Read | Write
                EOwnerProcess );
        User::LeaveIfError( err );
        TUint8* basePtr = iConfigurationChunk.Base();
        User::LeaveIfNull( basePtr );
        iConfig = (STvOutConfigSharedData*)basePtr;
        iConfig->TagCheckL();
        }

    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlEnable,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlDisable,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlSetConfig,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlCopyProtect,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlCableConnect,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlAnalogCableConnect,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlCableConnect,
        RProperty::EInt );
    RProperty::Define( KTVOutStatusControlCategory,
        KTVOutStatusControlConfigChange,
        RProperty::EInt );
    
    RProperty::Define( KTVOutStatusControlCategory,
       KTVOutStatusControlConfigChangeImmediateError,
        RProperty::EInt );
    
    RProperty::Define( KTVOutControlCategory,
       KTVOutControlCEAMode,
        RProperty::EInt );
    RProperty::Define( KTVOutControlCategory,
       KTVOutControlDMTMode,
        RProperty::EInt );
    

    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlEnable,
        KErrNone );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlDisable,
        KErrNone );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlSetConfig,
        KErrNone );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlCopyProtect,
        0 );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlCableConnect,
        0 );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlAnalogCableConnect,
        0 );
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlConfigChange,
        0 ); 
    RProperty::Set( KTVOutStatusControlCategory,
        KTVOutStatusControlConfigChangeImmediateError,
        0 );
    RProperty::Set( KTVOutControlCategory,
        KTVOutControlCEAMode,
        0 );
    RProperty::Set( KTVOutControlCategory,
        KTVOutControlDMTMode,
        0 );
    
    iCableConnectStatusListener = CTVOutStubEventListener::NewL( *this,
        KTVOutStatusControlCategory,
        KTVOutStatusControlCableConnect );
    iAnalogCableConnectStatusListener = CTVOutStubEventListener::NewL( *this,
        KTVOutStatusControlCategory,
        KTVOutStatusControlAnalogCableConnect );
    iCopyProtectStatusListener = CTVOutStubEventListener::NewL( *this,
        KTVOutStatusControlCategory,
        KTVOutStatusControlCopyProtect );
    iSettingChangeListener = CTVOutStubEventListener::NewL( *this,
        KTVOutStatusControlCategory,
        KTVOutStatusControlConfigChange );

	iAllDviMode.Reset();
	iSupportedMode.Reset();
	for(TInt i = ECeaFixedModeNone+1; i < ELastCeaFixedModeValue; i++)
		{
		TSupportedHdmiDviMode mode;

		Mem::FillZ( ( TAny* )&mode, sizeof( mode ) );
		mode.iStandardModeType = TSupportedHdmiDviMode::ECea;
		mode.iStandardMode = i;
		iAllDviMode.Append( mode );
		}
	for(TInt i = EDmtFixedModeNone+1; i < ELastDmtFixedModeValue; i++)
		{
		TSupportedHdmiDviMode mode;

		Mem::FillZ( ( TAny* )&mode, sizeof( mode ) );
		mode.iStandardModeType = TSupportedHdmiDviMode::EDmt;
		mode.iStandardMode = i;
		iAllDviMode.Append( mode );
		}

    err = iAccServer.Connect();
	User::LeaveIfError( err );
	err = iAccControl.CreateSubSession( iAccServer );
	User::LeaveIfError( err );
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 @return Symbian error code, KErrNotSupported if not supported,
 KErrArgument if aTvSettings was ignored,
 KErrNotFound when Tv-out driver not found.
 KErrNotReady - Disable()/Enable() sequence is needed */
TInt CTvOutConfigImp::SetConfig( const TTvSettings& aTvSettings )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    iConfigSemaphore.Wait();
    if( KErrNone == iConfig->iConfigSetTvError )
        {
        iConfig->iTvSettings = aTvSettings;
        iConfig->iConfigSet = ETrue;
        }
    else
        {
        err = iConfig->iConfigSetTvError;
        iConfig->iConfigSetTvError = KErrNone;
        }        
    iConfigSemaphore.Signal();
    return err;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::StandByFigureMaxSizeInBytes( TUint& aMaxFigureSize )
    {
    FUNC_LOG;
    
    aMaxFigureSize = 256;
    
    return KErrNone;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::StandByFigure( const TStandByFigure& aStandByFigure )
    {
    FUNC_LOG;
    
    
    return KErrNone;
    }

//---------------------------------------------------------------------------
/** Enables Tv-out. Latest SetConfig() settings are used.
 @pre SetConfig() is called without errors.
 @return Symbian Error Code, KErrNotSupported if not supported,
 KErrNotReady if SetConfig() failed,
 KErrAlreadyExists if was enabled,
 KErrServerBusy if previous task is going,
 try again later, not immediately */
TInt CTvOutConfigImp::Enable()
    {
    FUNC_LOG;
    TInt value( 0 );

    TInt err = RProperty::Get( KTVOutStatusControlCategory,
        KTVOutStatusControlEnable,
        value );

    iConfigSemaphore.Wait();
    if( err == KErrNone )
        {
        if( ( iConfig->iHdmiConfigSet || iConfig->iConfigSet )
            && !iConfig->iEnabled ) // Config must be set and not already enabled
            {
            if( value == KErrNone ) // Check if requested to fail
                {
                iConfig->iEnabled = ETrue;
                }
            if( KErrServerBusy == value )
                {
                if( iServerBusyReturnedOnce )
                    {
                    // Let the retrial succeed
                    iServerBusyReturnedOnce = EFalse;
                    iConfig->iEnabled = ETrue;
                    value = KErrNone;
                    }
                else
                    {
                    iServerBusyReturnedOnce = ETrue;
                    }
                }
            err = value;
            }
        else if( !iConfig->iHdmiConfigSet && !iConfig->iConfigSet ) // If config not set, return KErrNotReady
            {
            err = KErrNotReady;
            }
        else if( iConfig->iEnabled ) //  If already enabled, return KErrAlreadyExists
            {
            err = KErrAlreadyExists;
            }
        else
            {
            err = KErrServerBusy; //If P&S key any other than KErrNone
            }
        }
    // Only one return point per function. You see semaphore has to be released!
    iConfigSemaphore.Signal();
    return err;
    }

//---------------------------------------------------------------------------
/** Disables Tv-out.
 @pre Enable() is called without errors.
 @return Symbian Error Code, KErrNotSupported if not supported,
 KErrNotReady if SetConfig() failed,
 KErrAlreadyExists if was disabled,
 KErrServerBusy if previous task is going,
 try again later, not immediately */
TInt CTvOutConfigImp::Disable()
    {
    FUNC_LOG;
    TInt value( 0 );

    TInt err = RProperty::Get( KTVOutStatusControlCategory,
        KTVOutStatusControlDisable,
        value );

    iConfigSemaphore.Wait();
    if( err == KErrNone )
        {
        if( iConfig->iEnabled ) // Must be enabled
            {
            if( value == KErrNone ) // Check if requested to fail
                {
                iConfig->iEnabled = EFalse;
                }
            if( KErrServerBusy == value )
                {
                if( iServerBusyReturnedOnce )
                    {
                    // Let the retrial succeed
                    iServerBusyReturnedOnce = EFalse;
                    iConfig->iEnabled = EFalse;
                    value = KErrNone;
                    }
                else
                    {
                    iServerBusyReturnedOnce = ETrue;
                    }
                }
            err = value;
            }
        else if( !iConfig->iHdmiConfigSet && !iConfig->iConfigSet ) // If config not set, return KErrNotReady
            {
            err = KErrNotReady;
            }
        else if( !iConfig->iEnabled ) //  If already disabled, return KErrAlreadyExists
            {
            err = KErrAlreadyExists;
            }
        else
            {
            err = KErrServerBusy; //If P&S key any other than KErrNone
            }
        }
    iConfigSemaphore.Signal();
    // Only one return point per function. You see semaphore has to be released!
    return err;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TBool CTvOutConfigImp::Enabled()
    {
    FUNC_LOG;
    TBool ret( EFalse );
    iConfigSemaphore.Wait();
    ret = iConfig->iEnabled;
    iConfigSemaphore.Signal();
    return ret;
    }

//---------------------------------------------------------------------------
/** Adds listener for tv-out enable/disable situations.
 Check always aListenerRequest.Int(), it is KErrNone when normal operation.
 @param  aListenerRequest Will be completed without errors when tv-out
 changing state to on or off.
 @return Symbian error code */
TInt CTvOutConfigImp::OnOffListener( TRequestStatus& /*aListenerRequest*/)
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::OnOffListenerCancel()
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::CopyProtectionStatusListener( TRequestStatus& aListenerRequest )
    {
    FUNC_LOG;
    aListenerRequest = KRequestPending;
    iCopyProtectListenerStatus = &aListenerRequest;
    return KErrNone;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::CopyProtectionStatusCancel()
    {
    FUNC_LOG;
    if( iCopyProtectListenerStatus )
        {
        User::RequestComplete( iCopyProtectListenerStatus, KErrCancel );
        iCopyProtectListenerStatus = NULL;
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------
/** Adds listener for HDMI connected/disconnected situations.
 Check always aListenerRequest.Int(), it is KErrNone when normal operation.
 @param  aListenerRequest Will be completed without errors when HDMI cable connected/disconnected
 @return Symbian error code
 */
TInt CTvOutConfigImp::HdmiCableListener( TRequestStatus& aListenerRequest )
    {
    FUNC_LOG;
    aListenerRequest = KRequestPending;
    iHdmiCableListenerStatus = &aListenerRequest;
    return KErrNone;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::HdmiCableListenerCancel()
    {
    FUNC_LOG;
    if( iHdmiCableListenerStatus )
        {
        User::RequestComplete( iHdmiCableListenerStatus, KErrCancel );
        iHdmiCableListenerStatus = NULL;
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 @return Symbian error code, KErrNotSupported if not supported,
 KErrArgument if aTvSettings was ignored,
 KErrNotFound when Tv-out driver not found.
 KErrNotReady - Disable()/Enable() sequence is needed */
TInt CTvOutConfigImp::SetConfig( const THdmiDviTimings& aHdmiDviSettings )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    iConfigSemaphore.Wait();
    if( KErrNone == iConfig->iConfigSetHdmiDviError )
        {
        iConfig->iSetHdmiDviSettings = aHdmiDviSettings;
        iConfig->iHdmiConfigSet = ETrue;
        }
    else
        {
        err = iConfig->iConfigSetHdmiDviError;
        iConfig->iConfigSetHdmiDviError = KErrNone;
        }        
    iConfigSemaphore.Signal();
    return err;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TBool CTvOutConfigImp::HdmiCableConnected()
    {
    FUNC_LOG;
    TBool retVal( EFalse );

    iConfigSemaphore.Wait();
    retVal = iConfig->iConnected;
    iConfigSemaphore.Signal();
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TBool CTvOutConfigImp::CopyProtectionStatus()
    {
    FUNC_LOG;
    
    TBool retVal( EFalse );
    if( Enabled() )
        {
        iConfigSemaphore.Wait();
        retVal = iConfig->iCopyProtected;
        iConfigSemaphore.Signal();
        }
    
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::CopyProtection( TBool aCopyProtectEnabled )
    {
    FUNC_LOG;
    
    TInt err = KErrNone;
    if( CopyProtectionStatus() != aCopyProtectEnabled )
        {
        iConfigSemaphore.Wait();
        iConfig->iCopyProtected = aCopyProtectEnabled;
        iConfigSemaphore.Signal();
        }
    
    return err;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetNumberOfHdmiModes( TUint& aNumberOfModes )
    {
    FUNC_LOG;
    TInt retVal( KErrNone ), ceamask( 0 ), dmtmask( 0 );

	RProperty::Get( KTVOutControlCategory, KTVOutControlCEAMode, ceamask );
    RProperty::Get( KTVOutControlCategory, KTVOutControlDMTMode, dmtmask );

    aNumberOfModes = 0;
	if( (ceamask == 0) && (dmtmask == 0) )
		{
		aNumberOfModes = iAllDviMode.Count();
		}
	else
		{
	    iSupportedMode.Reset();
		for( TInt i=0; i < 32; i++ )
			{
			TSupportedHdmiDviMode mode;

			if( ceamask && (ceamask & (1 << i)) )
				{
				Mem::FillZ( ( TAny* )&mode, sizeof( mode ) );
				mode.iStandardModeType = TSupportedHdmiDviMode::ECea;
				mode.iStandardMode = i+1;
				iSupportedMode.Append( mode );				
				}
			
			if( dmtmask && (dmtmask & (1 << i)) )
				{
				Mem::FillZ( ( TAny* )&mode, sizeof( mode ) );
				mode.iStandardModeType = TSupportedHdmiDviMode::EDmt;
				mode.iStandardMode = i+1;
				iSupportedMode.Append( mode );				
				}
			}
		aNumberOfModes = iSupportedMode.Count();
		}
	
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetSupportedHdmiMode( TUint aModeNumber, TSupportedHdmiDviMode& aReadMode)
    {
    FUNC_LOG;
    TInt retVal( KErrNone ), ceamask( 0 ), dmtmask( 0 );

	RProperty::Get( KTVOutControlCategory, KTVOutControlCEAMode, ceamask );
    RProperty::Get( KTVOutControlCategory, KTVOutControlDMTMode, dmtmask );

	if( (ceamask == 0) && (dmtmask == 0) && (aModeNumber < iAllDviMode.Count()) )
		{
		aReadMode = iAllDviMode[aModeNumber];
		}
	else if( aModeNumber < iSupportedMode.Count() )
		{
		aReadMode = iSupportedMode[aModeNumber];
		}
	else
		{
		retVal = KErrNotReady;
		}
	
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetTvHwCapabilities( TTvSettings::TOutput /*aConnector*/,
    THwCapabilities& /*aReadCapabilities*/)
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetConfig( TTvSettings& aTvSettings )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    iConfigSemaphore.Wait();
    if( iConfig->iConfigSet )
        {
        aTvSettings = iConfig->iTvSettings;
        }
    else
        {
        retVal = KErrNotReady;
        }
    iConfigSemaphore.Signal();
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetConfig( THdmiDviTimings& aTvSettings )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    iConfigSemaphore.Wait();
    if( iConfig->iHdmiConfigSet )
        {
        aTvSettings = iConfig->iSetHdmiDviSettings;
        }
    else
        {
        retVal = KErrNotReady;
        }
    iConfigSemaphore.Signal();
    return retVal;
    }

//---------------------------------------------------------------------------
/**
 @see TvOutConfig.h
 */
TInt CTvOutConfigImp::GetStandByFigure( TStandByFigure& /*aStandByFigure*/)
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 Set list of available Tv-configurations
 @param  aAnalogConfigs  Available Analog Tv configurations
 @param  aHdmiConfigs    Available HDMI/DVI Tv configurations
 @return Symbian error code
 */
TInt CTvOutConfigImp::SetAvailableTvConfigList( const RArray<TTvSettings>& aAnalogConfigs,
    const RArray<THdmiDviTimings>& aHdmiConfigs )
    {
    FUNC_LOG;
    TInt value( 0 );

    TInt err = RProperty::Get( KTVOutStatusControlCategory,
        KTVOutStatusControlSetConfig,
        value );

    if( err == KErrNone )
        {
        if( value == KErrNone )
            {
            iConfigSemaphore.Wait();
            TInt count = aHdmiConfigs.Count();
            if( count )
                {
                iConfig->iHdmi.iCount = count;
                iConfig->iHdmiConfigSet = ETrue;
                for( TInt i = 0; i < count; ++i )
                    {
                    iConfig->iHdmi.iConfigs[i] = aHdmiConfigs[i];
                    }
                }
            count = aAnalogConfigs.Count();
            if( count )
                {
                iConfig->iAnalog.iCount = count;
                iConfig->iConfigSet = ETrue;
                for( TInt i = 0; i < aAnalogConfigs.Count(); ++i )
                    {
                    iConfig->iAnalog.iConfigs[i] = aAnalogConfigs[i];
                    }
                }
            iConfigSemaphore.Signal();
            }
        err = value;
        }
    return err;
    }

//---------------------------------------------------------------------------
/**
 Get list of available Tv-configurations
 @param  aAnalogConfigs  Available Analog Tv configurations
 @param  aHdmiConfigs    Available HDMI/DVI Tv configurations
 @return Symbian error code
 */
TInt CTvOutConfigImp::GetAvailableTvConfigList( RArray<TTvSettings>& aAnalogConfigs,
    RArray<THdmiDviTimings>& aHdmiConfigs )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    iConfigSemaphore.Wait();
    if( iConfig->iHdmiConfigSet || iConfig->iConfigSet )
        {
        for( TInt i = 0; i < iConfig->iHdmi.iCount; ++i )
            {
            aHdmiConfigs.Append( iConfig->iHdmi.iConfigs[i] );
            }
        for( TInt i = 0; i < iConfig->iAnalog.iCount; ++i )
            {
            aAnalogConfigs.Append( iConfig->iAnalog.iConfigs[i] );
            }
        }
    else
        {
        retVal = KErrNotReady;
        }
    iConfigSemaphore.Signal();

    return retVal;
    }

//---------------------------------------------------------------------------
/**
 Listener for SetAvailableTvConfigList()
 Check always aListenerRequest.Int(), it is KErrNone when normal operation.
 @param  aListenerRequest Will be completed without errors when SetAvailableTvConfigList is called
 @return Symbian error code
 */
TInt CTvOutConfigImp::AvailableTvConfigListListener( TRequestStatus& /*aListenerRequest*/)
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 Cancels listener, @see AvailableTvConfigListListener
 @return Symbian error code
 */
TInt CTvOutConfigImp::AvailableTvConfigListListenerCancel()
    {
    FUNC_LOG;
    return KErrNotReady;
    }

//---------------------------------------------------------------------------
/**
 Adds listener for changed settings
 Check always aListenerRequest.Int(), it is KErrNone when normal operation.
 @param  aListenerRequest Will be completed without errors when HDMI cable connected/disconnected
 @return Symbian error code
 */
TInt CTvOutConfigImp::SettingsChangedListener( TRequestStatus& aListenerRequest )
    {
    FUNC_LOG;
    TInt value( 0 );
    TInt err = RProperty::Get( KTVOutStatusControlCategory,
        KTVOutStatusControlConfigChangeImmediateError,
        value );
    
    if ( !err )
        {
        err = value;
        }
    if ( !err )
        {
        aListenerRequest = KRequestPending;
        iSettingsChangedListenerStatus = &aListenerRequest;
        }
    return err;
    }

//---------------------------------------------------------------------------
/**
 Cancels listener, @see SettingsChangedListener
 @return Symbian error code
 */
TInt CTvOutConfigImp::SettingsChangedListenerCancel()
    {
    FUNC_LOG;
    if( iSettingsChangedListenerStatus )
        {
        User::RequestComplete( iSettingsChangedListenerStatus, KErrCancel );
        iSettingsChangedListenerStatus = NULL;
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------
/**
 Receive and event from the listener

 */
void CTvOutConfigImp::SendEvent( const TUid aCategory,
    const TInt aEvent,
    const TInt aArg )
    {
    FUNC_LOG;
    /*TInt previousevent = KErrNone;
    
    iConfigSemaphore.Wait();
    previousevent = iConfig->iPreviousEvent;
    iConfig->iPreviousEvent = aEvent;
    iConfigSemaphore.Signal();*/
    
    if( (KTVOutStatusControlCategory == aCategory) ) //&& (previousevent != aEvent) )
        {
        if( KTVOutStatusControlCableConnect == aEvent )
            {
            EventHdmiCableConnect( aArg );	    
            }
        else if( KTVOutStatusControlCopyProtect == aEvent )
            {
            EventCopyProtectionStatus( aArg );
            }
        else if( KTVOutStatusControlConfigChange == aEvent )
            {
            EventSettingChange( aArg );       
            }
		else if( KTVOutStatusControlAnalogCableConnect == aEvent )
			{
			EventAnalogCableConnect( aArg );
			}
        }        
    }

//---------------------------------------------------------------------------
/**
 Process HDMI cable connect event

 */
void CTvOutConfigImp::EventHdmiCableConnect( const TInt aArg )
	{
    FUNC_LOG;
    iConfigSemaphore.Wait();
    if( aArg )
        {
        iConfig->iConnected = ETrue;
        }
    else
        {
        iConfig->iConnected = EFalse;
        }
    iConfigSemaphore.Signal();
	if ( iHdmiCableListenerStatus )
		{
        User::RequestComplete( iHdmiCableListenerStatus, KErrNone );
		iHdmiCableListenerStatus = NULL;
		}
	}

//---------------------------------------------------------------------------
/**
 Process Analog cable connect event

 */

void CTvOutConfigImp::EventAnalogCableConnect( const TInt aArg )
	{
    FUNC_LOG;
    TBool cvideoconnected = EFalse;
    
    RProcess process;
    TSecureId KRequiredSecureId((TUint32)0x10205030);

    if( process.SecureId() != KRequiredSecureId )
    	{
    		INFO_1("***************It is NOT Accessory server thread - 0x%x", (TUint32)process.SecureId());
    		return;
    	}
 		INFO_1("*************It is Accessory server thread - 0x%x", (TUint32)process.SecureId());
        
/*   TSecureId accid((TUint32)0x10205030);
    TSecureId secid = User::CreatorSecureId();
    
    if( secid != accid )
    	{
    		INFO_1("***************It is NOT Accessory server thread - 0x%x", (TUint32)secid);
    		return;
    	}
 		INFO_1("*************It is Accessory server thread - 0x%x", (TUint32)secid);
 		*/
    	
    iConfigSemaphore.Wait();
    cvideoconnected = iConfig->iCVideoConnected;
    
    if( aArg && (!cvideoconnected) )
        {
        // Connected
		TInt err = KErrNone;
		
		CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL(
			_L("Configuration file") );
		CleanupStack::PushL( accConfigFileParser );
		
		CAccConGenericID* genericID = CAccConGenericID::NewL();
		CleanupStack::PushL( genericID );
		
		RArray<TAccPolNameValueRecord> nameValueArray;
		CleanupClosePushL( nameValueArray );
		TBuf<KHWModelIDMaxLength> hwModelID( _L("c-video") );
		TAccPolNameRecord nameRecord;
		TAccPolValueRecord valueRecord;
		
		TBuf<KHWModelIDMaxLength> HWModelID( _L("c-video") );
		
		//Set GenericID header
		TAccPolGIDHeader genericIdHeader;
		genericIdHeader.iAccessoryDeviceType = KDTAVDevice;
		genericIdHeader.iPhysicalConnection = KPCWired;
		genericIdHeader.iApplicationProtocol = 0x0;
		genericIdHeader.iCapabilitiesSubblocks = KSBAudioSubblock | KSBVideoSubblock;
		genericIdHeader.iHWModelID = hwModelID;
		genericIdHeader.iHWDeviceID = 0x0;
		
		//Set "Audio Output Type" capability
		TAccPolNameValueRecord
			nameValueRecordAudioOutputType( KAccAudioOutputType,
				EAccAudioOutPutTypePublic,
				EAPVInt,
				EAPVPolicy );
		nameValueArray.AppendL( nameValueRecordAudioOutputType );
		
		//Set "Audio stereo" capability
		TAccPolNameValueRecord nameValueRecordAudioStereo( KAccStereoAudio,
			EAccAudioOutPutTypePublic,
			EAPVInt,
			EAPVPolicy );
		nameValueArray.AppendL( nameValueRecordAudioStereo );
		
		//Set "Line out Type" capability
		TAccPolNameValueRecord nameValueRecordAudioOut( KAccAudioOut,
			TAccAudioOutLineout,
			EAPVInt,
			EAPVPolicy );
		nameValueArray.AppendL( nameValueRecordAudioOut );
		
		//Set "Video Output Type" capability
		TAccPolNameValueRecord nameValueRecordVideoOut( KAccVideoOut,
			EAccVideoOutCVideo,
			EAPVInt,
			EAPVPolicy );
		nameValueArray.AppendL( nameValueRecordVideoOut );
		
		accConfigFileParser->ParseGenericIDL( genericID,
			genericIdHeader,
			nameValueArray );
		
    //iConfigSemaphore.Wait();
    iConfig->iCVideoConnected = ETrue;
    //iConfigSemaphore.Signal();
    
		TRequestStatus status;
		iAccControl.ConnectAccessory( status, genericID, EFalse );
		User::WaitForRequest( status );
		err = status.Int();

		if( KErrNone == err )
			{
			iCVideoGenericID = genericID->GenericID();
			}
			
			// Cleanup
			CleanupStack::PopAndDestroy( &nameValueArray );
			CleanupStack::PopAndDestroy( genericID );
			CleanupStack::PopAndDestroy( accConfigFileParser );
        }
    else if( (!aArg) && cvideoconnected )
        {
        // Disconnected
    //iConfigSemaphore.Wait();
    iConfig->iCVideoConnected = EFalse;
    //iConfigSemaphore.Signal();
    
	    TRequestStatus status;
	    iAccControl.DisconnectAccessory( status, iCVideoGenericID );
	    User::WaitForRequest( status );
		}
    iConfigSemaphore.Signal();

	}

//---------------------------------------------------------------------------
/**
 Process Copy Protection status event

 */
void CTvOutConfigImp::EventCopyProtectionStatus( const TInt aArg )
    {
    FUNC_LOG;
    iConfigSemaphore.Wait();
    if( aArg )
        {
        iConfig->iCopyProtected = ETrue;
        }
    else
        {
        iConfig->iCopyProtected = EFalse;
        }
    iConfigSemaphore.Signal();
    if ( iCopyProtectListenerStatus )
        {
        User::RequestComplete( iCopyProtectListenerStatus, KErrNone );
        iCopyProtectListenerStatus = NULL;
        }
    }

//---------------------------------------------------------------------------
/**
 Setting change event

 */
void CTvOutConfigImp::EventSettingChange( const TInt /*aArg*/ )
    {
    FUNC_LOG;
    if ( iSettingsChangedListenerStatus ) 
        {
        User::RequestComplete( iSettingsChangedListenerStatus, KErrNone );
        iSettingsChangedListenerStatus = NULL;
        }
    }

//---------------------------------------------------------------------------
/**
 Tag assign for shared memory chunk

 */
EXPORT_C void STvOutConfigSharedData::TagAssign()
    {
    FUNC_LOG;
    iBinaryProtection.iSizeOfThisStruct = sizeof( STvOutConfigSharedData );
    for ( TInt i = 0; i < 10; ++i )
        {
        iBinaryProtection.iTag[i] = i;
        }
    }

//---------------------------------------------------------------------------
/**
 Tag checking for shared memory chunk

 */
EXPORT_C void STvOutConfigSharedData::TagCheckL()
    {
    FUNC_LOG;
    TBool comparisonOK( ETrue );
    for ( TInt i = 0; i < 10; ++i )
        {
        if ( iBinaryProtection.iTag[i] != i )
            {
            comparisonOK = EFalse;
            break;
            }
        }
    if (    ( !comparisonOK ) ||
            ( sizeof( STvOutConfigSharedData ) != iBinaryProtection.iSizeOfThisStruct ) )   
        {
        // Recompilation needed after structure change has changed
        User::Leave( KErrCouldNotConnect );
        }
    }

//---------------------------------------------------------------------------
/**
 Zero load data, not tag at the end.

 */
EXPORT_C void STvOutConfigSharedData::ZeroLoadDataNotTag()
    {
    Mem::FillZ( this, ( sizeof( STvOutConfigSharedData ) - sizeof( iBinaryProtection ) ) );
    }


// End of File
