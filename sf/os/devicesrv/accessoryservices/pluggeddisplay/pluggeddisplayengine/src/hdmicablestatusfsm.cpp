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
* Description:  Finite State Machine class for HDMI Cable Status.
*
*/

#include <f32file.h>
#include <e32std.h>
#include <accpolhdmiobjectcon.h>
#include <accpolhdmisink.h>
#include <accpolhdmiaudioformat.h>
#include <accpolhdmivideoformat.h>
#include <accpolhdmilatency.h>
#include <accpolhdmispeakerallocation.h>
#include <data_caging_path_literals.hrh>

#include "pdeconstants.h"
#include "accessorycontrolif.h"
#include "hdmicablestateidle.h"
#include "hdmicablestateconnected.h"
#include "hdmicablestaterejected.h"
#include "hdmicablestatusfsm.h"
#include "tvoutconfigforhdmi.h"
#include "pdasycmdhandlerinterface.h"
#include "edidhandler.h"
#include "trace.h"
#include "cleanupresetanddestroy.h"
#include "tvstandbyfigure.h"
#include "TVOutStandbyFigure.mbg"  //located in \Data folder


//The standby image
_LIT(KHDMIStandbyFigureName,"z:tvoutstandbyfigure.mbm");

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CHDMICableStatusFSM* CHDMICableStatusFSM::NewL(
        RAccessoryServer& aAccessoryServer, CPDEngine *aPdEngine )
    {
    FUNC_LOG;
    CHDMICableStatusFSM* self = new ( ELeave ) CHDMICableStatusFSM();
    CleanupStack::PushL( self );
    self->ConstructL( aAccessoryServer, aPdEngine );
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHDMICableStatusFSM::~CHDMICableStatusFSM()
    {
    FUNC_LOG;
    iCurrentStateId = EHDMICableStateUndefined;
    // Delete state objects
    for (TInt i = 0; i < EHDMICableStateMaxValue; i++ )
        {
        delete iStateArray[i];
        }
    if ( iAccessoryControlIf )
        {
        iAccessoryControlIf->Cancel();
        delete iAccessoryControlIf;
        }
    if ( iTVOutConfigForHDMI )
        {
        iTVOutConfigForHDMI->Cancel();
        delete iTVOutConfigForHDMI;
        }
    if ( iEDIDHandler )
        {
        iEDIDHandler->Cancel();
        delete iEDIDHandler;        
        }
    }


//------------------------------------------------------------------------------
//  From MPdeFSM.
//------------------------------------------------------------------------------
//
TBool CHDMICableStatusFSM::ProcessCommandL( 
    const TProcessCmdId aCommand,
    const TASYCmdParams& aCmdParams,
    MPDAsyCmdHandler& aAsyCmdHandler )
    {
    FUNC_LOG;
    TBool commandHandled( EFalse );
    TASYCommandParamRecord cmdParams( aCmdParams());
    if ( iAccessoryControlIf->GetUniqueID() ==  cmdParams.iGenericID.UniqueID() )
        {
        commandHandled = ETrue;
        iAsyCmdHandler = &aAsyCmdHandler;
        TUint32 name;
        cmdParams.iNameRecord.GetName( name);
        TInt retVal( KErrNotFound );
        TFSMMainAndSubState currentStates = CurrentMainAndSubstateIDs();
        switch ( aCommand )
            {
            case ECmdGetObjectValue:
                {
                INFO( "Received command ECmdGetObjectValue" );
                CAccPolHdmiObjectCon* hdmiObjectCon = CAccPolHdmiObjectCon::NewLC();
                
                if (    ( EHDMICableStateConnected == currentStates.iMainState ) &&
                        ( CHDMICableStateConnected::ESubStateConnected == currentStates.iSubstate ) )
                    {                    
                    if ( KAccVideoHdmiAttributes == name )
                        {
                        INFO( "Object name KAccVideoHdmiAttributes" );
                        retVal = KErrNone;
                        }
                    else if ( KAccVideoFormat == name )
                        {
                        INFO( "Object name KAccVideoFormat" );
                        retVal = KErrNone;
                        }
                    else
                        {
                        INFO_1( "Unexpected capability name %i", name );
                        }
                    if ( KAccVideoHdmiAttributes == name )
                        {
                        CAccPolHdmiSink* hdmiSink = iEDIDHandler->CreateHdmiSinkL();
                        if( hdmiSink )
                            {
                            CleanupStack::PushL( hdmiSink );
                            hdmiObjectCon->AddL( *hdmiSink );
                            CleanupStack::PopAndDestroy( hdmiSink );
                            }
                        }
                    
                    if ( ( KAccVideoHdmiAttributes == name ) ||
                         ( KAccVideoFormat == name ) )
                        {
                        RAccPolHdmiVideoFormatArray hdmiVideoFormatArray;
                        CleanupResetAndDestroyPushL( hdmiVideoFormatArray );
                        iEDIDHandler->CreateHdmiVideoFormatL( hdmiVideoFormatArray );
                        TInt count = hdmiVideoFormatArray.Count();
                        for ( TInt i = 0; i < count; ++i )
                            {
                            hdmiObjectCon->AddL( *hdmiVideoFormatArray[i] );
                            }
                        CleanupStack::PopAndDestroy( &hdmiVideoFormatArray );
                        }
                    
                    if ( KAccVideoHdmiAttributes == name )
                        {
                        RAccPolHdmiLatencyArray hdmiLatencyArray;
                        CleanupResetAndDestroyPushL( hdmiLatencyArray );
                        iEDIDHandler->CreateHdmiLatencyL( hdmiLatencyArray );
                        TInt count = hdmiLatencyArray.Count();
                        for( TInt i = 0; i < count; i++ )
                            {
                            hdmiObjectCon->AddL( *hdmiLatencyArray[i] );
                            }
                        CleanupStack::PopAndDestroy( &hdmiLatencyArray );
                        }
                    
                    if ( KAccVideoHdmiAttributes == name )
                        {
                        RAccPolHdmiAudioFormatArray hdmiAudioFormatArray;
                        CleanupResetAndDestroyPushL( hdmiAudioFormatArray );
                        iEDIDHandler->CreateHdmiAudioFormatL( hdmiAudioFormatArray );
                        TInt count = hdmiAudioFormatArray.Count();
                        for ( TInt i = 0; i < count; ++i )
                            {
                            hdmiObjectCon->AddL( *hdmiAudioFormatArray[i] );
                            }
                        CleanupStack::PopAndDestroy( &hdmiAudioFormatArray );
                        }
                    
                    if ( KAccVideoHdmiAttributes == name )
                        {
                        CAccPolHdmiSpeakerAllocation* hdmiSpeakerAllocation =
                            iEDIDHandler->CreateHdmiSpeakerAllocationL();
                        if( hdmiSpeakerAllocation )
                            {
                            CleanupStack::PushL( hdmiSpeakerAllocation );
                            hdmiObjectCon->AddL( *hdmiSpeakerAllocation );
                            CleanupStack::PopAndDestroy( hdmiSpeakerAllocation );
                            }
                        }                    
                    aAsyCmdHandler.ProcessResponse( *hdmiObjectCon, retVal );
                    }
                else
                    {
                    INFO( "HDMI Cable is not connected" );
                    aAsyCmdHandler.ProcessResponse( *hdmiObjectCon, KErrHardwareNotAvailable );                   
                    }
                CleanupStack::PopAndDestroy( hdmiObjectCon );
                }
                break;
            case ECmdGetValueBool:
                {
                INFO( "Received command ECmdGetValueBool" );
                TAccValueTypeTBool typeTBool;
                typeTBool.iValue = EFalse;
                if ( KAccVideoHdcpSupportStatus == name )
                    {
                    if (    ( EHDMICableStateConnected == currentStates.iMainState ) &&
                            ( CHDMICableStateConnected::ESubStateConnected == currentStates.iSubstate ) )
                         {
                         Input( EPDEIfAsyCommandHandler, EIfAsyCommandHandlerEventGetCopyProtectStatus );
                         // CopyProtectionStatusGot() will be called later and command is responded there
                         }
                    else
                         {
                         INFO( "HDMI Cable is not connected" );
                         iAsyCmdHandler->ProcessResponse( typeTBool, KErrHardwareNotAvailable );
                         }
                    }
                else
                    {
                    INFO( "Unsupported cabability name" );
                    iAsyCmdHandler->ProcessResponse( typeTBool, KErrNotSupported );                   
                    }
                }
                break;
            case ECmdSetValueBool:
                INFO( "Received command ECmdSetValueBool" );
                TAccValueTypeTBool typeTBool;
                typeTBool.iValue = EFalse;
                if ( KAccVideoHdcpSupport == name )
                    {
                    if (    ( EHDMICableStateConnected == currentStates.iMainState ) &&
                            ( CHDMICableStateConnected::ESubStateConnected == currentStates.iSubstate ) )
                        {
                        TBool requestedCopyProtectStatus = (TBool)cmdParams.iCmdValue;
                        if ( requestedCopyProtectStatus )
                            {
                            Input( EPDEIfAsyCommandHandler, EIfAsyCommandHandlerEventSetCopyProtectionOn );
                            }
                        else
                            {
                            Input( EPDEIfAsyCommandHandler, EIfAsyCommandHandlerEventSetCopyProtectionOff );                    
                            }
                        // CopyProtectionSettingDone() will be called later and command is responded there
                        }
                    else
                        {
                        INFO( "HDMI Cable is not connected" );
                        iAsyCmdHandler->ProcessResponse( typeTBool, KErrHardwareNotAvailable );
                        }
                    }
                else
                    {
                    INFO( "Unsupported cabability name" );
                    iAsyCmdHandler->ProcessResponse( typeTBool, KErrNotSupported );                   
                    }
                break;
            default:
                {
                INFO_1( "Unknown command %i", aCommand );
                }
                break;
            }        
        }
    return commandHandled;
    }

//------------------------------------------------------------------------------
//  From MPdeFSM.
//------------------------------------------------------------------------------
//
TInt CHDMICableStatusFSM::GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID )
    {
    FUNC_LOG;
    return iAccessoryControlIf->GetAccPolGenericID( aAccPolGenericID );
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::Start()
    {
    FUNC_LOG;
    // Step to the initial state.
    iCurrentStateId = EHDMICableStateIdle;
    iStateArray[iCurrentStateId]->Enter();
    return;
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMId CHDMICableStatusFSM::FSMID()
    {
    FUNC_LOG;
    TFSMId aFSMID( EPDEFSMIdHDMICableStatus );
    return aFSMID;   
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStatusFSM::CurrentStateID()
    {
    FUNC_LOG;
    TFSMState currentState( EHDMICableStateUndefined );
    if (    ( 0 <= iCurrentStateId ) && 
            ( EHDMICableStateMaxValue > iCurrentStateId ))
        {
        currentState = iCurrentStateId;
        }
    INFO_1( "iCurrentStateId %i", iCurrentStateId );
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::Input(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    if ( EHDMICableStateUndefined != iCurrentStateId )
        {
        if ( NULL != iStateArray[iCurrentStateId] )
            {
            iStateArray[iCurrentStateId]->Input( aInterfaceId, aEvent );
            }
        }  
    return;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
MFSMState* CHDMICableStatusFSM::CurrentState()
    {
    FUNC_LOG;
    MFSMState* currentState( NULL );
    if (    ( 0 <= iCurrentStateId ) && 
            ( EHDMICableStateMaxValue > iCurrentStateId ))
        {
        currentState = iStateArray[iCurrentStateId];
        }
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TPtrC CHDMICableStatusFSM::CurrentStateName()
    {
    FUNC_LOG;
    TPtrC currentStateName( KNullDesC );
    if ( (  0 <= iCurrentStateId ) && 
         (  EHDMICableStateMaxValue > iCurrentStateId ))
        {
        if ( NULL != iStateArray[iCurrentStateId] )
             {
             currentStateName.Set( iStateArray[iCurrentStateId]->Name() );
             }
        }
    return currentStateName;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMMainAndSubState CHDMICableStatusFSM::CurrentMainAndSubstateIDs()
    {
    FUNC_LOG;
    TFSMMainAndSubState state;
    state.iMainState = EHDMICableStateUndefined;
    state.iSubstate = 0;
    if ( ( 0 <= iCurrentStateId ) && 
            ( EHDMICableStateMaxValue > iCurrentStateId ))
        {
        state.iMainState = iCurrentStateId;
        state.iSubstate = iStateArray[iCurrentStateId]->SubId();
        }
    INFO_2( "Main state id: %i, substate id: %i", state.iMainState, state.iSubstate );
    return state;
    }

// ---------------------------------------------------------------------------
// From MFSMForState.
// ---------------------------------------------------------------------------
//
TBool CHDMICableStatusFSM::Transit( TFSMState aNextState )
    {
    FUNC_LOG;
    TBool retVal(EFalse);
    if ( (  0 <= aNextState ) && 
         (  EHDMICableStateMaxValue > aNextState ) )
        {
        iCurrentStateId = aNextState;
        if ( NULL != iStateArray[iCurrentStateId])
            {
            retVal = ETrue;
            iStateArray[iCurrentStateId]->Enter();
            }
        }
    if ( EFalse == retVal )
        {
        //Something is wrong
        INFO( "Incorrect nextstate");
        }   
    INFO_1( "TBool retVal %i", retVal );
    return retVal;
    }

// ---------------------------------------------------------------------------
// CopyProtectionSettingDone
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::CopyProtectionSettingDone(
        TInt aError,
        TBool aCurrentStatus )
    {
    FUNC_LOG;
    if ( iAsyCmdHandler )
        {
        INFO_1( "Copy Protection setting returned error code %i", aError );
        TAccValueTypeTBool typeTBool;
        typeTBool.iValue = aCurrentStatus;
        iAsyCmdHandler->ProcessResponse( typeTBool, aError );        
        iAsyCmdHandler = NULL;
        }    
    if ( KErrNone == aError )
        {
        CopyProtectionStatusChanged( aCurrentStatus );
        }        
    }   

// ---------------------------------------------------------------------------
// CopyProtectionStatusChanged
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::CopyProtectionStatusChanged( TBool aNewStatus )
    {
    FUNC_LOG;
    INFO_1( "Copy Protection new status %i", aNewStatus );
    TUint32 name( KAccVideoHdcpSupportStatus );
    TAccValueTypeTBool typeTBool;
    typeTBool.iValue = aNewStatus;
    iAccessoryControlIf->NotifyValueChange( name, typeTBool);
    }   

// ---------------------------------------------------------------------------
// CopyProtectionListenFailed
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::CopyProtectionListenFailed( TInt aError )
    {
    FUNC_LOG;
    TInt error( aError ); // Just to get rid of a compiler warning.
    if ( KErrNone != error )
        {
        INFO_1( "Copy Protection listening returned error code %i", error );        
        }
    }   

// ---------------------------------------------------------------------------
// CopyProtectionStatusGot
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::CopyProtectionStatusGot( TBool aCopyProtectionStatus )
    {
    FUNC_LOG;
    if ( iAsyCmdHandler )
        {
        INFO_1( "Copy Protection status %i", aCopyProtectionStatus );
        TAccValueTypeTBool typeTBool;
        typeTBool.iValue = aCopyProtectionStatus;
        iAsyCmdHandler->ProcessResponse( typeTBool, KErrNone );
        iAsyCmdHandler = NULL;
        }    
    }   

// ---------------------------------------------------------------------------
// SettingsChangedL
// ---------------------------------------------------------------------------
//
void CHDMICableStatusFSM::SettingsChangedL()
    {
    FUNC_LOG;
    TUint32 name( KAccVideoFormat );
    CAccPolHdmiObjectCon* hdmiObjectCon = CAccPolHdmiObjectCon::NewLC();
    TFSMMainAndSubState currentStates = CurrentMainAndSubstateIDs();
    if (    (hdmiObjectCon) &&
            ( EHDMICableStateConnected == currentStates.iMainState ) &&
            ( CHDMICableStateConnected::ESubStateConnected == currentStates.iSubstate ) )
        {
        RAccPolHdmiVideoFormatArray hdmiVideoFormatArray;
        CleanupResetAndDestroyPushL( hdmiVideoFormatArray );
        iEDIDHandler->CreateHdmiVideoFormatL( hdmiVideoFormatArray );
        for ( TInt i = 0; i < hdmiVideoFormatArray.Count(); ++i )
            {
            hdmiObjectCon->AddL( *hdmiVideoFormatArray[i] );
            }
        CleanupStack::PopAndDestroy( &hdmiVideoFormatArray );
        iAccessoryControlIf->NotifyValueChange( name, *hdmiObjectCon );
        }                
    CleanupStack::PopAndDestroy( hdmiObjectCon );
    }


// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CHDMICableStatusFSM::CHDMICableStatusFSM()
    {
    FUNC_LOG;
    }


// ----------------------------------------------------------------------------
// CHDMICableStatusFSM::SetStandbyFiguresSettingsL
//
// ----------------------------------------------------------------------------
//
void CHDMICableStatusFSM::SetStandbyFiguresSettingsL()
    {
    FUNC_LOG;
    
    TInt err(KErrNone);
    
    if( iTVOutConfigForHDMI->GetTvOutConfig() )
        {
        //Standby figure
        TParse parse;
        TSize figureSize;
        TStandByFigure figureSettings;
        
        parse.Set( KHDMIStandbyFigureName, &KDC_BITMAP_DIR, NULL );
        TFileName fileName( parse.FullName() );
    
        //  Create converter for main standby figure
        CTvStandbyFigure* mainImage = CTvStandbyFigure::NewLC(
                    fileName, EMbmGSTvoutStandbyFigureQgn_graf_tvout_logo );
                    
        //  Fill the data structure for main standby figure
        mainImage->GetFigureSize( figureSize );
    
        TUint16* RGBData = mainImage->RgbConvertL();
        figureSettings.iTable = RGBData;
        
        figureSettings.iColumns = figureSize.iWidth;
        figureSettings.iRows = figureSize.iHeight;
        
        CleanupStack::PushL(RGBData);
        figureSettings.iFigureType = TStandByFigure::EStandByFigure;
        err = iTVOutConfigForHDMI->GetTvOutConfig()->StandByFigure( figureSettings );
        INFO_1( "iTVOutConfigForHDMI->StandByFigure( figureSettings ) return code %i", err );        
        figureSettings.iFigureType = TStandByFigure::EProtectedContentFigure;    
        err = iTVOutConfigForHDMI->GetTvOutConfig()->StandByFigure( figureSettings );
        INFO_1( "iTVOutConfigForHDMI->StandByFigure( figureSettings ) return code %i", err );        

        CleanupStack::PopAndDestroy( 2 );        
        }
    User::LeaveIfError(err);    
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CHDMICableStatusFSM::ConstructL(
        RAccessoryServer& aAccessoryServer, CPDEngine *aPdEngine )
    {
    FUNC_LOG;
    
    iPdEngine = aPdEngine;
    
    // Create interface objecs
    iAccessoryControlIf = CAccessoryControlIf::NewL( *this, aAccessoryServer );
    iTVOutConfigForHDMI = CTVOutConfigForHDMI::NewL( *this );
    iEDIDHandler = CEDIDHandler::NewL( *this, *iTVOutConfigForHDMI );
    // Create state objects here.
    iStateArray[ EHDMICableStateIdle ] = CHDMICableStateIdle::NewL( 
            *this, *iTVOutConfigForHDMI, *iEDIDHandler );
    iStateArray[ EHDMICableStateConnected ] = CHDMICableStateConnected::NewL( 
            *iTVOutConfigForHDMI, *iEDIDHandler, *iAccessoryControlIf, *this );
    iStateArray[ EHDMICableStateRejected ] = CHDMICableStateRejected::NewL( 
                *this, *iTVOutConfigForHDMI, *iEDIDHandler  );
    
    SetStandbyFiguresSettingsL();
    
    return;
    }

// ======== GLOBAL FUNCTIONS ========

