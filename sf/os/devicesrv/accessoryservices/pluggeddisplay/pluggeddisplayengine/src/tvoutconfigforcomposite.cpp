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
 * CPDConfigurationListenerForHDMI class implementation.
 *
 */


// INCLUDE FILES
#include <tvoutconfig.h>
#include <centralrepository.h>
#include <accessoriescrkeys.h>

#include "edidhandler.h"
#include "pdeconstants.h"
#include "multifinitestatemachine.h"
#include "tvoutconfigforcomposite.h"
#include "myasynconeshottimer.h"
#include "trace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

//Default progressive sequence
const TTvSettings::TScanMode KDefaultScan = TTvSettings::EInterlacedScan;

//Default output type
const TTvSettings::TOutput KDefaultOutput = TTvSettings::ECompositeVideo;

//Default detection mode
const TTvSettings::TDetectionType KDefaultDetectionMode = 
                                  TTvSettings::EAutoDetectOff;

// Pixel Aspect Ratio parameters
//PAL 4:3
const TUint8 KPxlAspect4d3PalNumerator   = 255; // 768 but scaled to 8-bit value
const TUint8 KPxlAspect4d3PalDenominator = 233; // 702 but scaled to 8-bit value
//PAL 16:9
const TUint8 KPxlAspect16d9PalNumerator  = 255; // 1024 but scaled to 8-bit value
const TUint8 KPxlAspect16d9PalDenominator= 175; // 702 but scaled to 8-bit value
//NTSC 4:3
const TUint8 KPxlAspect4d3NtscNumerator   = 232; // 647 but scaled to 8-bit value
const TUint8 KPxlAspect4d3NtscDenominator = 255; // 711 but scaled to 8-bit value
//NTSC 16:9
const TUint8 KPxlAspect16d9NtscNumerator  = 255; // 863 but scaled to 8-bit value
const TUint8 KPxlAspect16d9NtscDenominator = 210; // 711 but scaled to 8-bit value

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::NewL
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForComposite* CTVOutConfigForComposite::NewL( MFSMForBody& aFSM )
    {
    FUNC_LOG;
    
    CTVOutConfigForComposite* self = new(ELeave) CTVOutConfigForComposite( aFSM );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::CTVOutConfigForComposite
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForComposite::CTVOutConfigForComposite( MFSMForBody& aFSM ):
    CActive( CActive::EPriorityLow ),
    iFSM( aFSM )
    {
    FUNC_LOG;
    
    // Nothing
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::SetConfig
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::SetConfig( TBool aSettingAfterCableConnected )
    {
    FUNC_LOG;
    
    iSettingAfterCableConnected = aSettingAfterCableConnected;
    iStatus = KRequestPending;
    iNextOpt = EOptSetting;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );            
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ListenSettingsChanges
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::ListenSettingsChanges()
    {
    FUNC_LOG;
    Cancel();    
    iNextOpt = EOptStartListenForSettingsChanges;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::GetTvOutConfig
//
// ----------------------------------------------------------------------------
//
CTvOutConfig* CTVOutConfigForComposite::GetTvOutConfig()
    {
    FUNC_LOG;
    return iTvOutConfig; 
    }


// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::Enable
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::Enable()
    {
    FUNC_LOG;
    
    iStatus = KRequestPending;
    iNextOpt = EOptEnableTv;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::Disable
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::Disable()
    {
    FUNC_LOG;
    
    iStatus = KRequestPending;
    iNextOpt = EOptDisableTv;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    }

// -----------------------------------------------------------------------------
// CTVOutConfigForComposite::UpdateOverscanValues
// -----------------------------------------------------------------------------
//
TBool CTVOutConfigForComposite::UpdateOverscanValues()
    {
    FUNC_LOG;

    // Overscan from cenrep
    TInt hOverscan = 0;
    TInt vOverscan = 0;
    TInt err = KErrNone;
	TBool valChanged = EFalse;
    
    // Horizontal
    err = iRepository->Get( KSettingsTvoutHorizontalOverscan, hOverscan );
    if( err != KErrNone )
        {
        hOverscan = 0;
        }
    
    // Vertical
    err = iRepository->Get( KSettingsTvoutVerticalOverscan, vOverscan );
    if( err != KErrNone )
        {
        vOverscan = 0;
        }

	if( (iHOverscan != hOverscan) || (iVOverscan != vOverscan) )
		{
		valChanged = ETrue;
		}
    
    // Update overscan values
    iHOverscan = hOverscan;
    iVOverscan = vOverscan;

	INFO_3( "Overscan Values: %d,%d Changed:%d", iHOverscan, iVOverscan, valChanged );

	return valChanged;
    }

// -----------------------------------------------------------------------------
// CTVOutConfigForComposite::InitializeDriverL
// -----------------------------------------------------------------------------
//
TInt CTVOutConfigForComposite::InitializeDriverL()
    {
    FUNC_LOG;
    
    TTvSettings settings;
    TInt value = 0;
    TInt err = KErrNone;
    
    value = TvSystemL();
    ConvertTvSystem( value );
    settings.iTvSystem = ( TTvSettings::TSystem ) value;

    value = AspectRatioL();
    ConvertAspectRatio( value );
    settings.iRatio = ( TTvSettings::TAspectRatio ) value;

    settings.iScanMode = KDefaultScan;
    settings.iOutput = KDefaultOutput;
    settings.iAutoDetection = KDefaultDetectionMode;

    value = FlickerFilterL();
    ConvertFlickerFilter( value );
    settings.iFlickerFilter = ( TTvSettings::TFlickerFilterType ) value;

	// Update overscan
    settings.iHorizontalOverScan = iHOverscan;
    settings.iVerticalOverScan = iVOverscan;
    
    // Update the PAR
    UpdatePixelAspectRatio( settings );

    if ( err == KErrNone )
        {
        RArray<THdmiDviTimings> newHdmiSettingsArray;
        CleanupClosePushL( newHdmiSettingsArray );
        RArray<TTvSettings> newTvSettingsArray;
        CleanupClosePushL( newTvSettingsArray );

        err = newTvSettingsArray.Append( settings );
        INFO_1( "Settings.iTvSystem %i", settings.iTvSystem );
        INFO_1( "Settings.iRatio %i", settings.iRatio );
        INFO_1( "Settings.iScanMode %i", settings.iScanMode );
        INFO_1( "Settings.iOutput %i", settings.iOutput );
        INFO_1( "Settings.iFlickerFilter %i", settings.iFlickerFilter );
        INFO_1( "Settings.iAutoDetection %i", settings.iAutoDetection );
        INFO_1( "Settings.iHorizontalOverScan %i", settings.iHorizontalOverScan );
        INFO_1( "Settings.iVerticalOverScan %i", settings.iVerticalOverScan );
        iTvOutConfig->SetAvailableTvConfigList( newTvSettingsArray, newHdmiSettingsArray );
        CleanupStack::PopAndDestroy( &newTvSettingsArray );
        CleanupStack::PopAndDestroy( &newHdmiSettingsArray );
        
        iSettingAfterCableConnected = EFalse;
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CTVOutConfigForComposite::TvSystem
// -----------------------------------------------------------------------------
//
TInt CTVOutConfigForComposite::TvSystemL()
    {
    FUNC_LOG;
    
    TInt value = 0;
    User::LeaveIfError( iRepository->Get( KSettingsTvSystemInfo, value ) );
    
    return value;
    }

// -----------------------------------------------------------------------------
// CTVOutConfigForComposite::AspectRatioL
// -----------------------------------------------------------------------------
//
TInt CTVOutConfigForComposite::AspectRatioL()
    {
    FUNC_LOG;
    
    TInt value = 0;
    User::LeaveIfError( iRepository->Get( KSettingsTvAspectRatio, value ) );
    
    return value;
    }

// -----------------------------------------------------------------------------
// CTVOutConfigForComposite::FlickerFilter
// -----------------------------------------------------------------------------
//
TInt CTVOutConfigForComposite::FlickerFilterL()
    {
    FUNC_LOG;
    
    TInt value = 0;
    User::LeaveIfError( iRepository->Get( KSettingsTvoutFlickerFilter, value ) );

    return value;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::ConvertFlickerFilter
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::ConvertFlickerFilter( TInt& aValue )
    {
    FUNC_LOG;
    
    switch( aValue )   
        {
        case 0: // Off
            {
            aValue = TTvSettings::EFilterDisabled;
            break;
            }
        case 1: // On
            {
            aValue = TTvSettings::EFilterEnabled;
            break;
            }
        default:
            {
            aValue = TTvSettings::EUndefFlicker;
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::ConvertAspectRatio
// 
// 
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::ConvertAspectRatio( TInt& aValue )
    {
    FUNC_LOG;
    
    switch( aValue )
        {
        case 0: // 4:3
            {
            aValue = TTvSettings::E4d3;
            break;
            }
        case 1: // 16:9
            {
            aValue = TTvSettings::E16d9;
            break;
            }
        default:
            {
            aValue = TTvSettings::EUndefRatio;
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::ConvertTvSystem
// 
// 
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::ConvertTvSystem( TInt& aValue )
    {
    FUNC_LOG;
    
    switch( aValue )
        {
        case 0: // PAL
            {
            aValue = TTvSettings::EPAL;
            break;
            }
        case 1: // PALM
            {
            aValue = TTvSettings::EPALM;
            break;
            }
        case 2: // NTSC
            {
            aValue = TTvSettings::ENTSC;
            break;
            }
        default:
            {
            aValue = TTvSettings::EUndefSystem;
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::UpdatePixelAspectRatio
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::UpdatePixelAspectRatio( TTvSettings& aSettings )
    {
    FUNC_LOG;
    
    switch ( aSettings.iTvSystem )
        {
        case TTvSettings::EPAL:
        case TTvSettings::EPALM:
            {
            if( aSettings.iRatio == TTvSettings::E16d9 )
                {
                aSettings.iPixelAspectRatioNumerator = KPxlAspect16d9PalNumerator;
                aSettings.iPixelAspectRatioDenominator = KPxlAspect16d9PalDenominator;
                }
            else
                {
                aSettings.iPixelAspectRatioNumerator = KPxlAspect4d3PalNumerator;
                aSettings.iPixelAspectRatioDenominator = KPxlAspect4d3PalDenominator;                
                }
            break;
            }
        case TTvSettings::ENTSC:
            {
            if( aSettings.iRatio == TTvSettings::E16d9 )
                {
                aSettings.iPixelAspectRatioNumerator = KPxlAspect16d9NtscNumerator;
                aSettings.iPixelAspectRatioDenominator = KPxlAspect16d9NtscDenominator;
                }
            else
                {
                aSettings.iPixelAspectRatioNumerator = KPxlAspect4d3NtscNumerator;
                aSettings.iPixelAspectRatioDenominator = KPxlAspect4d3NtscDenominator;                
                }
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::ConstructL
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::ConstructL()
    {
    FUNC_LOG;

    iMyAsyncOneShotTimer = new (ELeave) CMyAsyncOneShotTimer( CActive::EPriorityLow, *this );
    iRepository = CRepository::NewL( KCRUidTvoutSettings );
    iTvOutConfig = CTvOutConfig::NewL();

    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::~CTVOutConfigForComposite
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForComposite::~CTVOutConfigForComposite()
    {
    FUNC_LOG;
    
    if ( iTvOutConfig )
        {
        Cancel();
        delete iTvOutConfig;
        }
    if ( iMyAsyncOneShotTimer )
        {
        if ( iMyAsyncOneShotTimer->IsActive() )
            {
            iMyAsyncOneShotTimer->Cancel();      
            }
        delete iMyAsyncOneShotTimer;
        }
    delete iRepository;    
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::DoCancel
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::DoCancel()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    if ( iMyAsyncOneShotTimer->IsActive() )
        {
        iMyAsyncOneShotTimer->Cancel();
        }
    if ( iNextOpt == EOptListenForSettingsChanges )
        {
        err = iTvOutConfig->SettingsChangedListenerCancel ();
        }
    if ( KErrNone != err )
        {
        INFO_1("Error status %d", err);
        }
    iNextOpt = EOptIdle;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::RunL
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::RunL()
    {

    FUNC_LOG;

    switch ( iNextOpt )
        {
        case EOptEnableTv:
            {
            INFO("EOptEnableTv");
            TInt retVal = iTvOutConfig->Enable();
            INFO_1("iTvOutConfig->Enable() retVal: %d", retVal);
            switch ( retVal )
                {
                case KErrNone:
                case KErrAlreadyExists:    
                    {
                    iNextOpt = EOptUndefined;
                    iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventEnabled );                     
                    }
                    break;
                case KErrServerBusy:
                    {
                    iTimeOutReason = iNextOpt;
                    iNextOpt = EOptIdle;
                    iMyAsyncOneShotTimer->Call();
                    }
                    break;
                case KErrNotSupported:
                case KErrNotReady:
                default:
                    {
                    iNextOpt = EOptUndefined;
                    iFSM.Input(
                             EPDEIfTVOutConfig,
                             EIfTVOutConfigEventEnableFailed );                                         
                    }
                    break;
                }            
            break;
            }
        case EOptDisableTv:
            {
            INFO("EOptDisableTv");
            TInt retVal = iTvOutConfig->Disable();
            INFO_1("iTvOutConfig->Disable() retVal: %d", retVal);
            switch ( retVal )
                {
                case KErrNone:
                case KErrAlreadyExists:    
                    {
                    iNextOpt = EOptUndefined;
                    iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventDisabled );                     
                    }
                    break;
                case KErrServerBusy:
                    {
                    iTimeOutReason = iNextOpt;
                    iNextOpt = EOptIdle;
                    iMyAsyncOneShotTimer->Call();
                    }
                    break;
                case KErrNotSupported:
                case KErrNotReady:
                default:
                    {
                    iNextOpt = EOptUndefined;
                    iFSM.Input(
                             EPDEIfTVOutConfig,
                             EIfTVOutConfigEventDisableFailed );                                         
                    }
                    break;
                }            
            break;
            }
        case EOptStartListenForSettingsChanges:
            {
            INFO("EOptStartListenForSettingsChanges");
            iStatus = KRequestPending;
            TInt err = iTvOutConfig->SettingsChangedListener( iStatus );
            INFO("EOptStartListenForSettingsChanges - Stage 1");
            if ( KErrNone != err )
                {
                INFO_1("iTvOutConfig->SettingsChangedListener( iStatus ) error status %i", err);
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenFailed );
                }
            else
                {
                INFO("EOptStartListenForSettingsChanges - Stage 2");
                iNextOpt = EOptListenForSettingsChanges;
                SetActive();
                INFO("EOptStartListenForSettingsChanges - Stage 3");
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenStarted );
                INFO("EOptStartListenForSettingsChanges - Stage 4");
                }
            break;
            }
        case EOptListenForSettingsChanges:
            {
            INFO("EOptListenForSettingsChanges");
            iNextOpt = EOptIdle;
            TInt err( iStatus.Int() );
            if ( KErrNone != err )
                {
                INFO_1("Error status %d", err);
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenFailed );
                }
            else
                {
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChanged );
                }
            break;
            }            
        case EOptSetting:
            {
            INFO("EOptSetting");
            TInt retVal( KErrNone );
            TRAP( retVal,  InitializeDriverL() );
            if (KErrNone != retVal )
                {
                INFO_1( "InitializeDriverL() leaved with error code %i", retVal );        
                if ( ( KErrNotReady == retVal ) &&
                     ( !iDisabledOnceBeforeSetting ) )
                    {
                    iDisabledOnceBeforeSetting = ETrue;
                    iStatus = KRequestPending;
                    iNextOpt = EOptDisableTVBeforeSetting;
                    SetActive();
                    TRequestStatus* status = &iStatus;
                    User::RequestComplete( status, KErrNone );        
                    }
                else
                    {
                    iDisabledOnceBeforeSetting = EFalse;
                    iSettingAfterCableConnected = EFalse;
                    iFSM.Input(
                             EPDEIfTVOutConfig,
                             EIfTVOutConfigEventSetFailed );                                         
                    }
                }
            else
                {
                iDisabledOnceBeforeSetting = EFalse;
                iSettingAfterCableConnected = EFalse;
                iNextOpt = EOptUndefined;
                iFSM.Input(
                         EPDEIfTVOutConfig,
                         EIfTVOutConfigEventSet );                                                 
                }
            break;
            }
        case EOptDisableTVBeforeSetting:
            {
            INFO("EOptDisableTVBeforeSetting");
            TInt retVal = iTvOutConfig->Disable();
            INFO_1("iTvOutConfig->Disable() retVal: %d", retVal);
            switch ( retVal )
                {
                case KErrNone:
                case KErrAlreadyExists:    
                    {
                    iNextOpt = EOptUndefined;
                    SetConfig( iSettingAfterCableConnected );
                    }
                    break;
                case KErrServerBusy:
                    {
                    User::After( EWaitTimeInMsForTvOutConfigBusyServer );
                    iStatus = KRequestPending;
                    iNextOpt = EOptDisableTVBeforeSetting;
                    SetActive();
                    TRequestStatus* status = &iStatus;
                    User::RequestComplete( status, KErrNone );                            
                    }
                    break;
                case KErrNotSupported:
                case KErrNotReady:
                default:
                    {
                    iNextOpt = EOptUndefined;
                    iFSM.Input(
                             EPDEIfTVOutConfig,
                             EIfTVOutConfigEventSetFailed );                                         
                    }
                    break;
                }            
            break;
            }
        default:
            INFO_1("Default case, iNextOpt = %i", iNextOpt);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForComposite::OneShotTimeout
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForComposite::OneShotTimeout()
    {
    FUNC_LOG;
    
    iNextOpt = iTimeOutReason;
    iTimeOutReason = EOptUndefined;
    RunL();
    }
