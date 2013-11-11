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
* Description:  Implementation of CTFDosServerControl class
*
*/


// INCLUDE FILES
#ifndef RD_STARTUP_CHANGE
#include <sysstartup.h>
#endif // RD_STARTUP_CHANGE

#include "ctffactory.h"
#include "tftypes.h"
#include <psvariables.h>
#include "dsytesttooltrace.h"
#include "dsyenums.h"
#include "ctfdosservercontrol.h"
#include "ctfdosservercontroltestcase.h"
#include "ctfdoseventlistener.h"


// -----------------------------------------------------------------------------
// GetDosServerControl
// -----------------------------------------------------------------------------
EXPORT_C MTFDosServerControl* GetDosServerControl( void )
    {
    CTFStub* stub = CTFFactory::Stub( KTFStubTypeDosServerControl );
    return STATIC_CAST( CTFDosServerControl*, stub );
    }


// -----------------------------------------------------------------------------
// MTFDosServerControl::MTFDosServerControl
// -----------------------------------------------------------------------------
MTFDosServerControl::MTFDosServerControl( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// MTFDosServerControl::~MTFDosServerControl
// -----------------------------------------------------------------------------
MTFDosServerControl::~MTFDosServerControl( void )
    {
    }
    

CTFDosServerControl::CTFDosServerControl( void )
: CTFStub( KTFStubTypeDosServerControl )
    {
    }
    
//lint -e1740 Pointer not directly deleted by destructor
CTFDosServerControl::~CTFDosServerControl( void )
    {
    ResetDosServices();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::~CTFDosServerControl() - DosServer connection closed" ) ) );
    }
//lint +e1740    
    
void CTFDosServerControl::InitializeL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::InitializeL()" ) ) );

    User::LeaveIfError( iDosServer.Connect() );
    iEventListener = new ( ELeave ) CTFDosServerEventListener( iDosServer );
    TRegisterEvent eventList[] = 
        {
#ifndef RD_STARTUP_CHANGE       
            { KNotifySelfTestStatus, sizeof ( TInt ), EQueue },
            { KDoMtcInitiatedShutdown, sizeof ( TInt ), EQueue },
            { KPhonePowerOn, 0, EQueue },
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
            { KSimReady, sizeof ( TInt ), EQueue },
            { KSimChanged, sizeof ( TInt ), EQueue },
            { KSimLockStatus, sizeof ( TInt ), EQueue },
            { KSimState, sizeof ( TInt ), EQueue },
            { KCallsForwardingStatus, sizeof ( TInt ), EQueue },
            { KVoiceMailStatus, sizeof ( TInt ), EQueue },
            { KFaxMessage, sizeof ( TInt ), EQueue },
            { KEmailMessage, sizeof ( TInt ), EQueue },
#endif //RD_STARTUP_CHANGE

     //       { KNetworkBars, sizeof ( TInt ), EQueue },

#ifndef RD_STARTUP_CHANGE
            { KSecurityCode, sizeof ( TInt ), EQueue },
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
            { KCurrentSimOwnedStatus, sizeof ( TInt ), EQueue },
#endif //RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
            { KAudioLinkOpenReq, sizeof ( TInt ), EQueue },
            { KAudioLinkCloseReq, sizeof ( TInt ), EQueue },
            { KActivateVoiceRecognitionReq, sizeof ( TInt ), EQueue },
            { KAbortVoiceRecognitionReq, sizeof ( TInt ), EQueue },
            { KVoiceRecognitionStatusReq, sizeof ( TInt ), EQueue },
            { KQuerySpeedDialNumberReq, sizeof ( TInt ), EQueue },
            { KQueryLastDialledVoiceNumberReq, sizeof ( TInt ), EQueue },
            { KAccessoryModeChangedInd, sizeof ( TInt ), EQueue },
            { KBTAccessoryModeChangedInd, sizeof ( TInt ), EQueue },
            { KAccessorySpeakerVolumeSetInd, sizeof ( TInt ), EQueue },
            { KServiceLevelConnEstablishedInd, sizeof ( TInt ), EQueue },
            { KHeadsetButtonChanged, sizeof ( TInt ), EQueue },
            { KHandsfreeModeChanged, sizeof ( TInt ), EQueue },
            { KOutputVolumeLevelChanged, sizeof ( TInt ), EQueue },
#endif //__ACCESSORY_FW
            
#ifndef RD_STARTUP_CHANGE
            { KSmsStorageStatusChanged, sizeof ( TDosSmsStorageStatus ), EQueue },
#endif // RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
            { KAccessoryModeChanged, sizeof ( TInt ), EQueue }
#endif //__ACCESSORY_FW
        };
    iEventListener->StartListeningL( eventList, sizeof ( eventList ) / sizeof ( TRegisterEvent ) );
    iInitialized = ETrue;

    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::InitializeL - return" ) ) );
    }

void CTFDosServerControl::ResetDosServices( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerStub::ResetDosServices()" ) ) );

    if ( iEventListener != NULL )
        {
        iEventListener->Stop();
        delete iEventListener;
        }
#ifndef RD_STARTUP_CHANGE
    if ( iDosSysUtils != NULL )
        {
        iDosSysUtils->Close();
        iDosSysUtils = NULL;
        }
    if ( iDosHelper != NULL )
        {
        iDosHelper->Close();
        iDosHelper = NULL;
        }
    if ( iDosMtc != NULL )
        {
        iDosMtc->Close();
        iDosMtc = NULL;
        }
    if ( iDosSelfTest != NULL )
        {
        iDosSelfTest->Close();
        iDosSelfTest = NULL;
        }
#endif //RD_STARTUP_CHANGE 

    if ( iDosSae != NULL )
        {
        iDosSae->Close();
        iDosSae = NULL;
        }        
#ifndef __ACCESSORY_FW
    if ( iDosBTAudio != NULL )
        {
        iDosBTAudio->Close();
        iDosBTAudio = NULL;
        }
#endif //__ACCESSORY_FW
    if ( iDosExtension != NULL )
        {
        iDosExtension->Close();
        iDosExtension = NULL;
        }
#ifndef __ACCESSORY_FW
    if ( iDosAccessory != NULL )
        {
        iDosAccessory->Close();
        iDosAccessory = NULL;
        }
    if ( iDosAudio != NULL )
        {
        iDosAudio->Close();
        iDosAudio = NULL;
        }
#endif //__ACCESSORY_FW

    iDosServer.Close();
    iInitialized = EFalse;

    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerStub::ResetDosServices() - Success" ) ) );
    }

void CTFDosServerControl::CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::CallDosFunctionL(%d, %d, %d)" ), aParameter.iDosFunction, aParameter.iArg1, aParameter.iArg2 ) );

    // DosServer connection needs to be opened.
    if ( !iInitialized )
        {
        InitializeL();
        }

    TInt retval = KTFErrDosServiceMissing;
    switch ( aParameter.iDosFunction )
        {
        
#ifndef RD_STARTUP_CHANGE       
        case ETFDosSysUtils_Open:
            if ( iDosSysUtils == NULL )
                {
                iDosSysUtils = new ( ELeave ) RDosSysUtils();
                retval = iDosSysUtils->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosSysUtils_GetSimLanguage:
            TRACE_ASSERT( iDosSysUtils != NULL );
            if ( iDosSysUtils != NULL )
                {
                retval = iDosSysUtils->GetSimLanguage( aParameter.iArg1 );
                }
            break;
        case ETFDosSysUtils_SetDosAlarm:
            TRACE_ASSERT( iDosSysUtils != NULL );
            if ( iDosSysUtils != NULL )
                {
                if ( aParameter.iArg1 != 0 )
                    {
                    TTime time;
                    time.HomeTime();
                    if ( aParameter.iArg1 > 0 )
                        {
                        TTimeIntervalMinutes interval = aParameter.iArg1;
                        time += interval;
                        }
                    else
                        {
                        TTimeIntervalMinutes interval = 0 - aParameter.iArg1;
                        time -= interval;
                        }
                    time.RoundUpToNextMinute();
                    if ( aParameter.iArg2 != 0 )
                        {
                        TTimeIntervalSeconds interval = aParameter.iArg2;
                        time += interval;
                        }
                    retval = iDosSysUtils->SetDosAlarm( &time );
                    }
                else
                    {
                    retval = iDosSysUtils->SetDosAlarm( NULL );
                    }
                }
            break;
        case ETFDosSysUtils_PerformDosRfs:
            TRACE_ASSERT( iDosSysUtils != NULL );
            if ( iDosSysUtils != NULL )
                {
                }
            break;
        case ETFDosSysUtils_Close:
            TRACE_ASSERT( iDosSysUtils != NULL );
            if ( iDosSysUtils != NULL )
                {
                iDosSysUtils->Close();
                delete iDosSysUtils;
                iDosSysUtils = NULL;
                }
            retval = KErrNone;
            break;
    
        case ETFDosHelper_Open:
            if ( iDosHelper == NULL )
                {
                iDosHelper = new ( ELeave ) RDosHelper();
                retval = iDosHelper->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosHelper_GetStartupReason:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                retval = iDosHelper->GetStartupReason( aParameter.iArg1 );
                }
            break;
        case ETFDosHelper_GetSWStartupReason:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                TInt16 temp;
                retval = iDosHelper->GetSWStartupReason( temp );
                aParameter.iArg1 = temp;
                }
            break;
        case ETFDosHelper_SetSWStartupReason:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                retval = iDosHelper->SetSWStartupReason( ( TInt16 )aParameter.iArg1 );
                }
            break;
        case ETFDosHelper_HiddenReset:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                aParameter.iArg1 = iDosHelper->HiddenReset();
                retval = KErrNone;
                }
            break;
        case ETFDosHelper_GetRTCStatus:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                retval = iDosHelper->GetRTCStatus( aParameter.iArg1 );
                }
            break;
        /*case ETFDosHelper_GenerateGripEvent:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                retval = iDosHelper->GenerateGripEvent();
                }
            break;*/
        case ETFDosHelper_Close:
            TRACE_ASSERT( iDosHelper != NULL );
            if ( iDosHelper != NULL )
                {
                iDosHelper->Close();
                delete iDosHelper;
                iDosHelper = NULL;
                }
            retval = KErrNone;
            break;
    
        case ETFDosMtc_Open:
            if ( iDosMtc == NULL )
                {
                iDosMtc = new ( ELeave ) RDosMtc();
                retval = iDosMtc->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosMtc_PowerOn:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->PowerOn();
                }
            break;
        case ETFDosMtc_PowerOff:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->PowerOff();
                }
            break;
        case ETFDosMtc_DosSync:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->DosSync();
                }
            break;
        case ETFDosMtc_ResetGenerate:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->ResetGenerate();
                }
            break;
        case ETFDosMtc_SetState:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->SetState( aParameter.iArg1 );
                }
            break;
        case ETFDosMtc_SetStateFlag:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                retval = iDosMtc->SetStateFlag( ( TDosStateFlag )aParameter.iArg1 );
                }
            break;
        case ETFDosMtc_GetStateFlag:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                TDosStateFlag flag;
                retval = iDosMtc->GetStateFlag( flag );
                aParameter.iArg1 = flag;
                }
            break;
        case ETFDosMtc_Close:
            TRACE_ASSERT( iDosMtc != NULL );
            if ( iDosMtc != NULL )
                {
                iDosMtc->Close();
                delete iDosMtc;
                iDosMtc = NULL;
                }
            retval = KErrNone;
            break;
    
        case ETFDosSelfTest_Open:
            if ( iDosSelfTest == NULL )
                {
                iDosSelfTest = new ( ELeave ) RDosSelfTest();
                retval = iDosSelfTest->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosSelfTest_PerformSelfTest:
            TRACE_ASSERT( iDosSelfTest != NULL );
            if ( iDosSelfTest != NULL )
                {
                retval = iDosSelfTest->PerformSelfTest();
                }
            break;
        case ETFDosSelfTest_Close:
            TRACE_ASSERT( iDosSelfTest != NULL );
            if ( iDosSelfTest != NULL )
                {
                iDosSelfTest->Close();
                delete iDosSelfTest;
                iDosSelfTest = NULL;
                }
            retval = KErrNone;
            break;
#endif // RD_STARTUP_CHANGE

        case ETFDosSae_Open:
            if ( iDosSae == NULL )
                {
                iDosSae = new ( ELeave ) RDosSae();
                retval = iDosSae->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosSae_StartSae:
            TRACE_ASSERT( iDosSae != NULL );
            if ( iDosSae != NULL )
                {
                retval = iDosSae->StartSae();
                }
            break;
        case ETFDosSae_Close:
            TRACE_ASSERT( iDosSae != NULL );
            if ( iDosSae != NULL )
                {
                iDosSae->Close();
                delete iDosSae;
                iDosSae = NULL;
                }
            retval = KErrNone;
            break;

#ifndef __ACCESSORY_FW 
        case ETFDosBTAudio_Open:
            if ( iDosBTAudio == NULL )
                {
                iDosBTAudio = new ( ELeave ) RDosBTAudio();
                retval = iDosBTAudio->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosBTAudio_AttachAudioAccessoryReq:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_CancelAttachReq:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_DetachAudioAccessoryReq:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_CancelDetachReq:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_AudioLinkOpenResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_AudioLinkCloseResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_ActivateVoiceRecognitionResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_AbortVoiceRecognitionResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_VoiceRecognitionStatusResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_QuerySpeedDialNumberResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_QueryLastDialledVoiceNumberResp:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_AudioLinkOpenInd:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_AudioLinkCloseInd:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_VoiceRecognitionStatusInd:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                }
            break;
        case ETFDosBTAudio_Close:
            TRACE_ASSERT( iDosBTAudio != NULL );
            if ( iDosBTAudio != NULL )
                {
                iDosBTAudio->Close();
                delete iDosBTAudio;
                iDosBTAudio = NULL;
                }
            retval = KErrNone;
            break;
#endif // __ACCESSORY_FW 
    
        case ETFDosExtension_Open:
            if ( iDosExtension == NULL )
                {
                iDosExtension = new ( ELeave ) RTFDosExtension();
                retval = iDosExtension->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosExtension_CallFunction:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                retval = iDosExtension->CallSyncDosExtFunction( aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( aParameter.iArg2 ), ETrue );
                }
            break;
        case ETFDosExtension_CallFunctionAndComplete:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                TRequestStatus status;
                retval = iDosExtension->CallAsyncDosExtFunction( status, aParameter.iArg1, ( TAny* )&aParameter.iArg2, sizeof ( TInt ), ETrue );
                }
            break;
        case ETFDosExtension_Close:
            TRACE_ASSERT( iDosExtension != NULL );
            if ( iDosExtension != NULL )
                {
                iDosExtension->Close();
                delete iDosExtension;
                iDosExtension = NULL;
                }
            retval = KErrNone;
            break;
    
#ifndef __ACCESSORY_FW    
        case ETFDosAccessory_Open:
            if ( iDosAccessory == NULL )
                {
                iDosAccessory = new ( ELeave ) RDosAccessory();
                retval = iDosAccessory->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosAccessory_GetAccessoryMode:
            TRACE_ASSERT( iDosAccessory != NULL );
            if ( iDosAccessory != NULL )
                {
                TDosAccessoryMode acc;
                retval = iDosAccessory->GetAccessoryMode( acc );
                if ( retval == KErrNone )
                    {
                    aParameter.iArg1 = acc;
                    }
                }
            break;
        case ETFDosAccessory_SetLoopSetModeState:
            TRACE_ASSERT( iDosAccessory != NULL );
            if ( iDosAccessory != NULL )
                {
                retval = iDosAccessory->SetLoopSetModeState( ( EPSAccLpsMode )aParameter.iArg1 );
                }
            break;
        case ETFDosAccessory_GetLoopSetModeState:
            TRACE_ASSERT( iDosAccessory != NULL );
            if ( iDosAccessory != NULL )
                {
                EPSAccLpsMode acc;
                retval = iDosAccessory->GetLoopSetModeState( acc );
                if ( retval == KErrNone )
                    {
                    aParameter.iArg1 = acc;
                    }
                }
            break;
            
        case ETFDosAccessory_Close:
            TRACE_ASSERT( iDosAccessory != NULL );
            if ( iDosAccessory != NULL )
                {
                iDosAccessory->Close();
                delete iDosAccessory;
                iDosAccessory = NULL;
                }
            retval = KErrNone;
            break;
#endif // __ACCESSORY_FW    
            
    
#ifndef __ACCESSORY_FW    
        case ETFDosAudio_Open:
            if ( iDosAudio == NULL )
                {
                iDosAudio = new ( ELeave ) RDosAudio();
                retval = iDosAudio->Open( iDosServer );
                }
            else
                {
                retval = KErrNone;
                }
            break;
        case ETFDosAudio_SetHandsfreeMode:
            TRACE_ASSERT( iDosAudio != NULL );
            if ( iDosAudio != NULL )
                {
                retval = iDosAudio->SetHandsfreeMode( ( EPSHandsFreeMode )aParameter.iArg1 );
                }
            break;
        case ETFDosAudio_GetHandsfreeMode:
            TRACE_ASSERT( iDosAudio != NULL );
            if ( iDosAudio != NULL )
                {
                EPSHandsFreeMode hf;
                retval = iDosAudio->GetHandsfreeMode( hf );
                if ( retval == KErrNone )
                    {
                    aParameter.iArg1 = hf;
                    }
                }
            break;
        
        case ETFDosAudio_GetOutputVolume:
            TRACE_ASSERT( iDosAudio != NULL );
            if ( iDosAudio != NULL )
                {
                retval = iDosAudio->GetOutputVolume( aParameter.iArg1 );
                }
            break;
        case ETFDosAudio_SetOutputVolume:
            TRACE_ASSERT( iDosAudio != NULL );
            if ( iDosAudio != NULL )
                {
                retval = iDosAudio->SetOutputVolume( aParameter.iArg1 );
                }
            break;
        case ETFDosAudio_Close:
            TRACE_ASSERT( iDosAudio != NULL );
            if ( iDosAudio != NULL )
                {
                iDosAudio->Close();
                delete iDosAudio;
                iDosAudio = NULL;
                }
            retval = KErrNone;
            break;
#endif // __ACCESSORY_FW    
            
        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }
    User::LeaveIfError( retval );
    }

// Getters return 0, since they are not used in parameter tests
TInt CTFDosServerControl::ParameterCount( TTFDosFunction aFunction ) const
    {
    TInt retval = 0;
    switch ( aFunction )
        {
#ifndef RD_STARTUP_CHANGE
        case ETFDosSysUtils_Open:
        case ETFDosHelper_Open:
        case ETFDosMtc_Open:
        case ETFDosSelfTest_Open:
        case ETFDosSysUtils_Close:
        case ETFDosHelper_Close:
        case ETFDosMtc_Close:
        case ETFDosSelfTest_Close:
#endif // RD_STARTUP_CHANGE

        case ETFDosSae_Open:
        case ETFDosSae_Close:

#ifndef __ACCESSORY_FW
        case ETFDosBTAudio_Open:
        case ETFDosBTAudio_Close:
#endif // __ACCESSORY_FW

        case ETFDosExtension_Open:
        case ETFDosExtension_Close:
        
#ifndef __ACCESSORY_FW       
        case ETFDosAccessory_Open:
        case ETFDosAudio_Open:
        case ETFDosAccessory_Close:
        case ETFDosAudio_Close:
#endif // __ACCESSORY_FW        
            break;
            
#ifndef RD_STARTUP_CHANGE
        case ETFDosSysUtils_SetDosAlarm:
        case ETFDosSysUtils_PerformDosRfs:
            retval = 1;
            break;
        case ETFDosHelper_GetStartupReason:
        case ETFDosHelper_GetSWStartupReason:
            break;
        case ETFDosHelper_SetSWStartupReason:
            retval = 1;
            break;
        case ETFDosHelper_HiddenReset:
        case ETFDosHelper_GetRTCStatus:
        //case ETFDosHelper_GenerateGripEvent:
        case ETFDosMtc_PowerOn:
        case ETFDosMtc_PowerOff:
        case ETFDosMtc_DosSync:
        case ETFDosMtc_ResetGenerate:
            break;
        case ETFDosMtc_SetState:
        case ETFDosMtc_SetStateFlag:
            retval = 1;
            break;
        case ETFDosMtc_GetStateFlag:
        case ETFDosSelfTest_PerformSelfTest:
            break;
#endif // RD_STARTUP_CHANGE
        
        case ETFDosSae_StartSae:
            break;

#ifndef __ACCESSORY_FW 
        case ETFDosBTAudio_AttachAudioAccessoryReq:
            break;
        case ETFDosBTAudio_CancelAttachReq:
            break;
        case ETFDosBTAudio_DetachAudioAccessoryReq:
            break;
        case ETFDosBTAudio_CancelDetachReq:
            break;
        case ETFDosBTAudio_AudioLinkOpenResp:
            break;
        case ETFDosBTAudio_AudioLinkCloseResp:
            break;
        case ETFDosBTAudio_ActivateVoiceRecognitionResp:
            break;
        case ETFDosBTAudio_AbortVoiceRecognitionResp:
            break;
        case ETFDosBTAudio_VoiceRecognitionStatusResp:
            break;
        case ETFDosBTAudio_QuerySpeedDialNumberResp:
            break;
        case ETFDosBTAudio_QueryLastDialledVoiceNumberResp:
            break;
        case ETFDosBTAudio_AudioLinkOpenInd:
            break;
        case ETFDosBTAudio_AudioLinkCloseInd:
            break;
        case ETFDosBTAudio_VoiceRecognitionStatusInd:
            break;
#endif // __ACCESSORY_FW 
    
        case ETFDosExtension_CallFunction:
            break;
        case ETFDosExtension_CallFunctionAndComplete:
            break;
    
#ifndef __ACCESSORY_FW    
        case ETFDosAccessory_GetAccessoryMode:
            break;
        case ETFDosAccessory_SetLoopSetModeState:
            retval = 1;
            break;
        case ETFDosAccessory_GetLoopSetModeState:
            break;

        case ETFDosAudio_SetHandsfreeMode:
            retval = 1;
            break;
        case ETFDosAudio_GetHandsfreeMode:
            break;
        
        case ETFDosAudio_GetOutputVolume:
            break;
        case ETFDosAudio_SetOutputVolume:
            retval = 1;
            break;
#endif // __ACCESSORY_FW    
            
        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }
    return retval;
    }
    

TBool CTFDosServerControl::ParameterBounds( TTFDosFunction aFunction, TInt* aMin, TInt* aMax ) const
    {
    TBool retval = EFalse;
    TRACE_ASSERT( aMin != NULL && aMax != NULL );
    if ( aMin != NULL && aMax != NULL )
        {
        switch ( aFunction )
            {
#ifndef RD_STARTUP_CHANGE
            case ETFDosSysUtils_Open:
            case ETFDosHelper_Open:
            case ETFDosMtc_Open:
            case ETFDosSelfTest_Open:
            case ETFDosSysUtils_Close:
            case ETFDosHelper_Close:
            case ETFDosMtc_Close:
            case ETFDosSelfTest_Close:
#endif // RD_STARTUP_CHANGE

            case ETFDosSae_Open:
            case ETFDosExtension_Open:

#ifndef __ACCESSORY_FW    
            case ETFDosBTAudio_Open:
            case ETFDosBTAudio_Close:
            case ETFDosAccessory_Open:
            case ETFDosAudio_Open:
            case ETFDosAccessory_Close:
            case ETFDosAudio_Close:
#endif // __ACCESSORY_FW    

            case ETFDosSae_Close:
            case ETFDosExtension_Close:
                break;
    
#ifndef RD_STARTUP_CHANGE
            case ETFDosSysUtils_GetSimLanguage:
                break;
            case ETFDosSysUtils_SetDosAlarm:
                // Not supported, too many time values to test.
                break;
            case ETFDosSysUtils_PerformDosRfs:
                retval = ETrue;
                *aMin = ERfsNormal;
                *aMax = ERfsDeep;
                break;
    
            case ETFDosHelper_GetStartupReason:
                break;
            case ETFDosHelper_GetSWStartupReason:
                break;
            case ETFDosHelper_SetSWStartupReason:
                retval = ETrue;
                *aMin = ESWNone;
                *aMax = ESWRestoreFactorySetDeep;
                break;
            case ETFDosHelper_HiddenReset:
                break;
            case ETFDosHelper_GetRTCStatus:
                break;
            //case ETFDosHelper_GenerateGripEvent:
            //    break;
    
            case ETFDosMtc_PowerOn:
                break;
            case ETFDosMtc_PowerOff:
                break;
            case ETFDosMtc_DosSync:
                break;
            case ETFDosMtc_ResetGenerate:
                break;
            case ETFDosMtc_SetState:
                retval = ETrue;
                *aMin = ESysApMtcChargingState;
                *aMax = ESysApMtcRfInactiveState;
                break;
            case ETFDosMtc_SetStateFlag:
                retval = ETrue;
                *aMin = EDosOfflineStateFlagFalse;
                *aMax = EDosOfflineStateFlagTrue;
                break;
            case ETFDosMtc_GetStateFlag:
                break;
    
            case ETFDosSelfTest_PerformSelfTest:
                break;
#endif // RD_STARTUP_CHANGE
    
            case ETFDosSae_StartSae:
                break;

#ifndef __ACCESSORY_FW 
            case ETFDosBTAudio_AttachAudioAccessoryReq:
                break;
            case ETFDosBTAudio_CancelAttachReq:
                break;
            case ETFDosBTAudio_DetachAudioAccessoryReq:
                break;
            case ETFDosBTAudio_CancelDetachReq:
                break;
            case ETFDosBTAudio_AudioLinkOpenResp:
                break;
            case ETFDosBTAudio_AudioLinkCloseResp:
                break;
            case ETFDosBTAudio_ActivateVoiceRecognitionResp:
                break;
            case ETFDosBTAudio_AbortVoiceRecognitionResp:
                break;
            case ETFDosBTAudio_VoiceRecognitionStatusResp:
                break;
            case ETFDosBTAudio_QuerySpeedDialNumberResp:
                break;
            case ETFDosBTAudio_QueryLastDialledVoiceNumberResp:
                break;
            case ETFDosBTAudio_AudioLinkOpenInd:
                break;
            case ETFDosBTAudio_AudioLinkCloseInd:
                break;
            case ETFDosBTAudio_VoiceRecognitionStatusInd:
                break;
#endif // __ACCESSORY_FW 
    
            case ETFDosExtension_CallFunction:
                break;
            case ETFDosExtension_CallFunctionAndComplete:
                break;
    
#ifndef __ACCESSORY_FW    
            case ETFDosAccessory_GetAccessoryMode:
                break;
            case ETFDosAccessory_SetLoopSetModeState:
                retval = ETrue;
                *aMin = EPSAccLpsOff;
                *aMax = EPSAccTty;
                break;
            case ETFDosAccessory_GetLoopSetModeState:
                break;

            case ETFDosAudio_SetHandsfreeMode:
                retval = ETrue;
                *aMin = EPSIhfOff;
                *aMax = EPSIhfOn;
                break;
            case ETFDosAudio_GetHandsfreeMode:
                break;

            case ETFDosAudio_GetOutputVolume:
                break;
            case ETFDosAudio_SetOutputVolume:
                retval = ETrue;
                *aMin = 0;
                *aMax = 9;
                break;
#endif // __ACCESSORY_FW    
                
            default:
                TRACE_ASSERT_ALWAYS;
                break;
            }
        }
    else
        {
        retval = KErrArgument;
        }
    return retval;
    }

void CTFDosServerControl::NotifyDosEvent( TInt aEvent, TInt aParameter )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::NotifyDosEvent( %d, %d )" ), aEvent, aParameter ) );
    CTFDosServerControlTestCase* testCase = STATIC_CAST( CTFDosServerControlTestCase*, CurrentTestCase() );
    TRACE_ASSERT( testCase != NULL );
    if ( testCase != NULL )
        {
        testCase->NotifyDosEvent( aEvent, aParameter );
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControl::NotifyDosEvent - return" ) ) );
    }
    

TInt RTFDosExtension::CallSyncDosExtFunction( const TInt& aFunc, TAny* aParam = NULL, TInt aParLength = 0, TBool aAutoComplete = ETrue )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallSyncDosExtFunction(0x%x, 0x%x, 0x%x, 0x%x)" ), aFunc, aParam, aParLength, aAutoComplete ) );
    TExtensionParPckg package;
    package().iFunc = aFunc;
    package().iParLength = aParLength;
    package().iAutoComplete = aAutoComplete;
    TPtr8 ptr( ( TUint8* )aParam, aParLength, aParLength );
    TInt result = CallFunction( package, ptr );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallSyncDosExtFunction - return 0x%x" ), result ) );
    return result;
    }


TInt RTFDosExtension::CallAsyncDosExtFunction( TRequestStatus& aStatus, TInt aFunc, TAny* aParam = NULL, TInt aParLength = 0, TBool aAutoComplete = EFalse )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallAsyncDosExtFunction(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), &aStatus, aFunc, aParam, aParLength, aAutoComplete ) );
    TExtensionParPckg package;
    package().iFunc = aFunc;
    package().iParLength = aParLength;
    package().iAutoComplete = aAutoComplete;
    TPtr8 ptr( ( TUint8* )aParam, aParLength, aParLength );
    CallFunction( aStatus, package, ptr );
    User::WaitForRequest( aStatus );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - RTFDosExtension::CallAsyncDosExtFunction - return 0x%x" ), aStatus.Int() ) );
    return aStatus.Int();
    }
