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
* Description:  Definition of integration test cases
*
*/


// INCLUDE FILES
#include "tftestmacros.h"

#ifndef RD_STARTUP_CHANGE
#include <sysstartup.h>
#endif // RD_STARTUP_CHANGE

#include "ctfatestenvironment.h"
#include <psvariables.h>
#include "dsyenums.h"
#include "dsytesttooltrace.h"
#include "ctfdosserverplugin.h"
#include "ctfdosservercontrol.h"
#include "ctfdosservercontroltestcaseparam.h"
#include "ctfdosservercontroltestcase.h"

// LOCAL FUNCTIONS
#ifndef __ACCESSORY_FW    
static void AccessoryUseCaseSuiteL( CTFATestSuite* aSuite );
static void BTAudioUseCaseSuiteL( CTFATestSuite* aSuite );
static void AudioUseCaseSuiteL( CTFATestSuite* aSuite );
static void AudioParameterSuiteL( CTFATestSuite* aSuite );

#endif // __ACCESSORY_FW    

#ifndef RD_STARTUP_CHANGE
static void SelfTestUseCaseSuiteL( CTFATestSuite* aSuite );
static void MtcUseCaseSuiteL( CTFATestSuite* aSuite );
static void HelperUseCaseSuiteL( CTFATestSuite* aSuite );
static void SysUtilsUseCaseSuiteL( CTFATestSuite* aSuite );
static void BluetoothSAPSuiteL( CTFATestSuite* aSuite );
#endif // RD_STARTUP_CHANGE

static void SAEUseCaseSuiteL( CTFATestSuite* aSuite );

static void ExtensionUseCaseSuiteL( CTFATestSuite* aSuite );

#ifndef __ACCESSORY_FW    
/***************************** AUDIO *********************************/    
// I0002.01 DSY [Handsfree On]
// Sets handsfree mode on.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( ESAIhfOn )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeOnStates[4] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.02 DSY [Handsfree Off]
// Sets handsfree mode off.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( ESAIhfOff )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeOffStates[4] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.03 DSY [Invalid Handsfree 100]
// Tries to set handsfree mode to invalid value. The current value must not change.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( 100 )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeInvalidStates[5] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetHandsfreeMode, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.07 DSY [Output Volume 0]
// Sets output volume to 0.
// Tested function:
//   - RDosAudio::SetOutputVolume( 0 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolume0States[4] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.08 DSY [Output Volume 9]
// Sets output volume to 9.
// Tested function:
//   - RDosAudio::SetOutputVolume( 9 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolume9States[4] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.09 DSY [Invalid Output Volume 100]
// Tries to set output volume to invalid value. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( 100 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalid100States[5] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.10 DSY [Open and Close Audio]
// Opens and closes the audio service.
// Tested function:
//   - RDosAudio::Open()
//   - RDosAudio::Close()
const TTFDosServerControlTestCaseState KTFAudioOpenCloseStates[2] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.11 DSY [Handsfree On Twice]
// Sets handsfree mode on twice.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( ESAIhfOn )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeOnTwiceStates[6] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.12 DSY [Handsfree Off Twice]
// Sets handsfree mode off twice.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( ESAIhfOff )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeOffTwiceStates[6] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, EPSIhfOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.13 DSY [Invalid Handsfree 100 Twice]
// Tries to set handsfree mode to invalid value twice. The current value must not change.
// Tested function:
//   - RDosAudio::SetHandsfreeMode( 100 )
const TTFDosServerControlTestCaseState KTFAudioHandsfreeInvalidTwiceStates[8] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetHandsfreeMode, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetHandsfreeMode, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetHandsfreeMode, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.17 DSY [Output Volume 0 Twice]
// Sets output volume to 0 twice.
// Tested function:
//   - RDosAudio::SetOutputVolume( 0 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolume0TwiceStates[6] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.18 DSY [Output Volume 9 Twice]
// Sets output volume to 9 twice.
// Tested function:
//   - RDosAudio::SetOutputVolume( 9 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolume9TwiceStates[6] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.19 DSY [Invalid Output Volume 100 Twice]
// Tries to set output volume to invalid value twice. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( 100 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalid100TwiceStates[8] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.22 DSY [Invalid Output Volume -1]
// Tries to set output volume to invalid value. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( -1 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalidNeg1States[5] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, -1, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.23 DSY [Invalid Output Volume 10]
// Tries to set output volume to invalid value. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( 10 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalid10States[5] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 10, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.24 DSY [Invalid Output Volume -1 Twice]
// Tries to set output volume to invalid value twice. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( -1 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalidNeg1TwiceStates[8] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, -1, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, -1, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.25 DSY [Invalid Output Volume 10 Twice]
// Tries to set output volume to invalid value twice. The current value must not change.
// Tested function:
//   - RDosAudio::SetOutputVolume( 10 )
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeInvalid10TwiceStates[8] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 10, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAudio_SetOutputVolume, 10, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0002.26 DSY [Output Volume 0 - 9]
// Sets and gets output volumes.
// Tested function:
//   - RDosAudio::SetOutputVolume
const TTFDosServerControlTestCaseState KTFAudioOutputVolumeStates[20] =
    {
        { ETFDosAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 2, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 2, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 4, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 4, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 6, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 6, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 9, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 6, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 6, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 4, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 4, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 2, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 2, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_SetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_GetOutputVolume, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
#endif // __ACCESSORY_FW    


/***************************** SAE *********************************/   
// I0010.01 DSY [Open and Close SAE]
// Opens and closes the SAE service.
// Tested function:
//   - RDosSae::Open()
//   - RDosSae::Close()
const TTFDosServerControlTestCaseState KTFSAEOpenCloseStates[2] =
    {
        { ETFDosSae_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSae_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.04 DSY [Start SAE]
// Starts sae.
// Tested function:
//   - RDosSae::StartSae()
const TTFDosServerControlTestCaseState KTFSAEStartSAEStates[3] = 
    {
        { ETFDosSae_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSae_StartSae, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSae_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

#ifndef RD_STARTUP_CHANGE
/***************************** SysUtils *********************************/
// I0008.01 DSY [Open and Close SysUtils]
// Opens and closes the SysUtils service.
// Tested function:
//   - RDosSysUtils::Open()
//   - RDosSysUtils::Close()
const TTFDosServerControlTestCaseState KTFSysUtilsOpenCloseStates[2] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.05 DSY [SIM Language]
// Gets SIM language.
// Tested function:
//   - RDosSysUtils::GetSimLanguage()
const TTFDosServerControlTestCaseState KTFSysUtilsGetSimLanguageStates[3] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_GetSimLanguage, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_IgnoreParameters },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.02 DSY [Set Alarm]
// Sets a DOS alarm 60*24*365 minutes from current time.
// Tested function:
//  - RDosSysUtils::SetDosAlarm()
const TTFDosServerControlTestCaseState KTFSysUtilsSetAlarmStates[3] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 60*24*365, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.03 DSY [Cancel Alarm]
// Cancels current DOS alarm.
// Tested function:
//  - RDosSysUtils::SetDosAlarm()
const TTFDosServerControlTestCaseState KTFSysUtilsCancelAlarmStates[3] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.04 DSY [Set and Cancel Alarm]
// Sets alarm twice, cancels it twice.
// Tested function:
//  - RDosSysUtils::SetDosAlarm()
const TTFDosServerControlTestCaseState KTFSysUtilsSetCancelAlarmStates[6] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 60*24*365, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 60*24*365*2, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.05 DSY [Invalid Alarm Neg.]
// Sets a DOS alarm ~10 years before current time. Setter should return KErrGeneral
// result if the CMT clock is not ~10 years behind the APE clock.
// Tested function:
//  - MDosSysUtilsBaseDSY::SetDosAlarm()
const TTFDosServerControlTestCaseState KTFSysUtilsSetAlarmInvalidStates[3] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, -( 60*24*365*10 ), 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.06 DSY [Invalid Alarm Sec]
// Sets a DOS alarm 60*24*365 minutes after current time and sets the seconds in the alarm to 30.
// Since alarm cannot contain seconds, setter should return KErrGeneral.
// Tested function:
//  - RDosSysUtils::SetDosAlarm()
const TTFDosServerControlTestCaseState KTFSysUtilsSetAlarmInvalidSecondsStates[3] =
    {
        { ETFDosSysUtils_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_SetDosAlarm, 60*24*365, 30, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSysUtils_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

/***************************** Helper *********************************/   
// I0010.02 DSY [Open and Close Helper]
// Opens and closes the helper service.
// Tested function:
//   - RDosHelper::Open()
//   - RDosHelper::Close()
const TTFDosServerControlTestCaseState KTFHelperOpenCloseStates[2] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.06 DSY [Startup Reason]
// Gets the startup reason. 
// Tested function:
//   - RDosHelper::GetStartupReason()
const TTFDosServerControlTestCaseState KTFHelperGetStartupReasonStates[3] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetStartupReason, ENormal, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.07 DSY [SW Startup Reason]
// Sets / gets SW startup reasons.
// Tested function:
//   - RDosHelper::SetSWStartupReason()
const TTFDosServerControlTestCaseState KTFHelperSetSWStartupReasonStates[15] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWNone, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWNone, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWRestoreFactorySet, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWRestoreFactorySet, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWLangSwitch, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWLangSwitch, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWWarrantyTransfer, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWWarrantyTransfer, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWChargerConnected, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWChargerConnected, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWRestoreFactorySetDeep, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetSWStartupReason, ESWRestoreFactorySetDeep, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_SetSWStartupReason, ESWNone, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.08 DSY [Hidden Reset]
// Gets hidden reset status.
// Tested function:
//   - RDosHelper::HiddenReset()
const TTFDosServerControlTestCaseState KTFHelperHiddenResetStates[3] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_HiddenReset, EFalse, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0007.01 DSY [RTC Status]
// Gets the RTC status. 
// Tested function:
//   - RDosHelper::GetRTCStatus()
const TTFDosServerControlTestCaseState KTFHelperGetRTCStatusStates[3] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GetRTCStatus, ETrue, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0004.01 DSY [Grip Event]
// Generates a grip event.
// Tested function:
//   - RDosHelper::GenerateGripEvent
/*const TTFDosServerControlTestCaseState KTFHelperGripEventStates[3] =
    {
        { ETFDosHelper_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_GenerateGripEvent, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosHelper_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
*/

/***************************** MTC *********************************/
// I0010.03 DSY [Open and Close MTC]
// Opens and closes the MTC service.
// Tested function:
//   - RDosMtc::Open()
//   - RDosMtc::Close()
const TTFDosServerControlTestCaseState KTFMtcOpenCloseStates[2] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.09 DSY [Offline State True]
// Sets offline state flag to true.
// Tested function:
//   - RDosMtc::SetStateFlag( EDosOfflineStateFlagTrue )
const TTFDosServerControlTestCaseState KTFMtcStateFlagTrueStates[4] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetStateFlag, EDosOfflineStateFlagTrue, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_GetStateFlag, EDosOfflineStateFlagTrue, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.10 DSY [Offline State False]
// Sets offline state flag to false.
// Tested function:
//   - RDosMtc::SetStateFlag( EDosOfflineStateFlagFalse )
const TTFDosServerControlTestCaseState KTFMtcStateFlagFalseStates[4] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetStateFlag, EDosOfflineStateFlagFalse, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_GetStateFlag, EDosOfflineStateFlagFalse, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.11 DSY [Invalid Offline State 100]
// Tries to set offline state flag to invalid value. The current value must not change.
// Tested function:
//   - RDosMtc::SetStateFlag( EDosOfflineStateFlagFalse )
const TTFDosServerControlTestCaseState KTFMtcStateFlagInvalidStates[5] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_GetStateFlag, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosMtc_SetStateFlag, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_GetStateFlag, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0010.12 DSY [RF Inactive and Normal States]
// Sets MTC state to RF inactive and then normal.
// Tested function:
//   - RDosMtc::SetState( ESysApMtcRfInactiveState )
//   - RDosMtc::SetState( ESysApMtcNormalState )
const TTFDosServerControlTestCaseState KTFMtcStateRFInactiveNormalStates[4] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetState, ESysApMtcRfInactiveState, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetState, ESysApMtcNormalState, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

/***************************** SelfTest *********************************/
// I0009.01 DSY [Open and Close Self Test]
// Opens and closes the self test service.
// Tested function:
//   - RDosSelfTest::Open()
//   - RDosSelfTest::Close()
const TTFDosServerControlTestCaseState KTFSelfTestOpenCloseStates[2] =
    {
        { ETFDosSelfTest_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSelfTest_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0009.02 DSY [Perform Self Test]
// Performs self test.
// Tested function:
//   - RDosSelfTest::PerformSelfTest()
const TTFDosServerControlTestCaseState KTFSelfTestPerformStates[3] =
    {
        { ETFDosSelfTest_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSelfTest_PerformSelfTest, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSelfTest_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
#endif // RD_STARTUP_CHANGE


#ifndef __ACCESSORY_FW 
/***************************** BT Audio *********************************/
// I0003.01 DSY [Open and Close BT Audio]
// Opens and closes the BT audio service.
// Tested function:
//   - RDosBTAudio::Open()
//   - RDosBTAudio::Close()
const TTFDosServerControlTestCaseState KTFBTAudioOpenCloseStates[2] =
    {
        { ETFDosBTAudio_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosBTAudio_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
#endif // __ACCESSORY_FW 

/***************************** Extension *********************************/  
// I0005.01 DSY [Open and Close Extension]  
// Opens and closes the extension service.
// Tested function:
//   - RDosExtension::Open()
//   - RDosExtension::Close()
const TTFDosServerControlTestCaseState KTFExtensionOpenCloseStates[2] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0005.02 DSY [Invalid Call Function Sync]  
// Tests sync CallFunction with invalid parameter.
// Tested function:
//   - RDosExtension::CallFunction() ( sync )
const TTFDosServerControlTestCaseState KTFExtensionInvalidSyncCallFunctionStates[3] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_CallFunction, 854632, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0005.03 DSY [Invalid Call Function Async]  
// Tests async CallFunction with invalid parameter.
// Tested function:
//   - RDosExtension::CallFunction() ( async )
const TTFDosServerControlTestCaseState KTFExtensionInvalidAsyncCallFunctionStates[3] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_CallFunctionAndComplete, 854632, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

#ifndef __ACCESSORY_FW    
/***************************** Accessory *********************************/ 
// I0001.01 DSY [Loopset On]
// Sets loopset mode on.
// Tested function:
//   - RDosAccessory::SetLoopSetModeState( ESAAccLpsOn )
const TTFDosServerControlTestCaseState KTFAccessoryLoopsetOnStates[4] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_SetLoopSetModeState, EPSAccLpsOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetLoopSetModeState, EPSAccLpsOn, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0001.02 DSY [Loopset Tty]
// Sets loopset mode to tty.
// Tested function:
//   - RDosAccessory::SetLoopSetModeState( ESAAccTty )
const TTFDosServerControlTestCaseState KTFAccessoryLoopsetTtyStates[4] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_SetLoopSetModeState, EPSAccTty, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetLoopSetModeState, EPSAccTty, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0001.03 DSY [Loopset Off]
// Sets loopset mode off. 
// Tested function:
//   - RDosAccessory::SetLoopSetModeState( ESAAccLpsOff )
const TTFDosServerControlTestCaseState KTFAccessoryLoopsetOffStates[4] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_SetLoopSetModeState, EPSAccLpsOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetLoopSetModeState, EPSAccLpsOff, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0001.04 DSY [Invalid Loopset 100]
// Tries to set loopset mode to invalid value. The current value must not change
// Tested function:
//   - RDosAccessory::SetLoopSetModeState( 100 )
const TTFDosServerControlTestCaseState KTFAccessoryLoopsetInvalidStates[5] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetLoopSetModeState, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_StoreParameters },
        { ETFDosAccessory_SetLoopSetModeState, 100, 0, KErrGeneral, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetLoopSetModeState, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_UseParameters },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0001.05 DSY [Open and Close Accessory]
// Opens and closes the accessory service.
// Tested function:
//   - RDosAccessory::Open()
//   - RDosAccessory::Close()
const TTFDosServerControlTestCaseState KTFAccessoryOpenCloseStates[2] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0001.08 DSY [Accessory Mode]
// Tests GetAccessoryMode, which should return EDosAccNotConnected
// Tested function:
//   - RDosAccessory::GetAccessoryMode
const TTFDosServerControlTestCaseState KTFAccessoryGetAccessoryModeStates[3] =
    {
        { ETFDosAccessory_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_GetAccessoryMode, EDosAccNotConnected, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosAccessory_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

#endif // __ACCESSORY_FW    

#ifndef RD_STARTUP_CHANGE
/***************************** BT SAP *********************************/ 
// I0012.01 DSY [BT SAP On]
// Sets BT SAP state on
// Tested function:
//   - RDosMtc::SetState( 5 )
const TTFDosServerControlTestCaseState KTFMtcBTSapOnStates[3] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetState, 5, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// I0012.02 DSY [BT SAP Off]
// Sets BT SAP state off
// Tested function:
//   - RDosMtc::SetState( ESysApMtcNormalState )
const TTFDosServerControlTestCaseState KTFMtcBTSapOffStates[3] =
    {
        { ETFDosMtc_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_SetState, ESysApMtcNormalState, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosMtc_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
#endif // RD_STARTUP_CHANGE



// C++ default constructor can NOT contain any code, that
// might leave.
//
CTFDosServerPlugin::CTFDosServerPlugin( TAny* aInitParams )
: iInitParams( ( CTFStubModuleInterface::TInterfaceInitParams* ) aInitParams )
    {
    }

// Two-phased constructor.
CTFDosServerPlugin* CTFDosServerPlugin::NewL( TAny* aInitParams )
    {
    return new ( ELeave ) CTFDosServerPlugin( aInitParams );
    }

// Destructor.
CTFDosServerPlugin::~CTFDosServerPlugin()
    {
    iInitParams = NULL;
    }


CTFStub* CTFDosServerPlugin::GetStubL( void )
    {
    return new ( ELeave ) CTFDosServerControl();
    }


void CTFDosServerPlugin::BuildTestSuiteL( CTFATestSuite* aRootSuite )
    {
    // Memory checks are disabled since DSY runs in separate thread and heap.
    aRootSuite->Environment().SetTestCaseFlags( ETFMayLeakMemory | ETFMayLeakRequests | ETFMayLeakHandles );
    TF_START_TEST_DECLARATION_L( aRootSuite );
        // Use case test cases
        TF_ADD_TEST_SUITE_L( _L( "Use Case Tests" ) );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "SAE" ), SAEUseCaseSuiteL );

#ifndef __ACCESSORY_FW
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Audio" ), AudioUseCaseSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Audio Param" ), AudioParameterSuiteL );
#endif // __ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "SysUtils" ), SysUtilsUseCaseSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Helper" ), HelperUseCaseSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Self Test" ), SelfTestUseCaseSuiteL );
#endif // RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "BT Audio" ), BTAudioUseCaseSuiteL );
#endif // __ACCESSORY_FW

            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Extension" ), ExtensionUseCaseSuiteL );
            
#ifndef __ACCESSORY_FW
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Accessory" ), AccessoryUseCaseSuiteL );
#endif // __ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "MTC" ), MtcUseCaseSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "BT SAP" ), BluetoothSAPSuiteL );
#endif // RD_STARTUP_CHANGE
        TF_END_TEST_SUITE();
    TF_END_TEST_DECLARATION();
    }


MTFStubTestCase* CTFDosServerPlugin::GetStubTestCaseL( TInt aTestCaseId )
    {
    TF_START_STUB_TEST_CASES( aTestCaseId );

#ifndef __ACCESSORY_FW
    //AUDIO
    // I0002.10 DSY [Open and Close Audio]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10110, KTFAudioOpenCloseStates ) ;

    // I0002.01 DSY [Handsfree On]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10101, KTFAudioHandsfreeOnStates ) ;

    // I0002.02 DSY [Handsfree Off]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10102, KTFAudioHandsfreeOffStates ) ;

    // I0002.03 DSY [Invalid Handsfree 100]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10103, KTFAudioHandsfreeInvalidStates ) ;

    // I0002.07 DSY [Output Volume 0]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10107, KTFAudioOutputVolume0States ) ;

    // I0002.08 DSY [Output Volume 9]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10108, KTFAudioOutputVolume9States ) ;

    // I0002.09 DSY [Invalid Output Volume 100]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10109, KTFAudioOutputVolumeInvalid100States ) ;

    // I0002.11 DSY [Handsfree On Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10111, KTFAudioHandsfreeOnTwiceStates ) ;

    // I0002.12 DSY [Handsfree Off Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10112, KTFAudioHandsfreeOffTwiceStates ) ;

    // I0002.13 DSY [Invalid Handsfree 100 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10113, KTFAudioHandsfreeInvalidTwiceStates ) ;
    
    // I0002.17 DSY [Output Volume 0 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10117, KTFAudioOutputVolume0TwiceStates ) ;
    
    // I0002.18 DSY [Output Volume 9 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10118, KTFAudioOutputVolume9TwiceStates ) ;
    
    // I0002.19 DSY [Invalid Output Volume 100 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10119, KTFAudioOutputVolumeInvalid100TwiceStates ) ;

    // I0002.22 DSY [Invalid Output Volume -1]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10122, KTFAudioOutputVolumeInvalidNeg1States ) ;

    // I0002.23 DSY [Invalid Output Volume 10]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10123, KTFAudioOutputVolumeInvalid10States ) ;

    // I0002.24 DSY [Invalid Output Volume -1 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10124, KTFAudioOutputVolumeInvalidNeg1TwiceStates ) ;

    // I0002.25 DSY [Invalid Output Volume 10 Twice]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10125, KTFAudioOutputVolumeInvalid10TwiceStates ) ;

    // I0002.20 DSY [Audio Regression]
    // Case 10120: Uses other test cases

    // I0002.21 DSY [Audio Randomizer]
    // Case 10121: Uses other test cases

    // I0002.26 DSY [Output Volume 0 - 9]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10126, KTFAudioOutputVolumeStates ) ;
#endif // __ACCESSORY_FW

    // SAE
    // I0010.01 DSY [Open and Close SAE]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10301, KTFSAEOpenCloseStates ) ;

    // I0010.04 DSY [Start SAE]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10302, KTFSAEStartSAEStates ) ;

#ifndef RD_STARTUP_CHANGE
    // SYSUTILS
    // I0008.01 DSY [Open and Close SysUtils]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10401, KTFSysUtilsOpenCloseStates ) ;

    // I0010.05 DSY [SIM Language]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10402, KTFSysUtilsGetSimLanguageStates ) ;

    // I0007.02 DSY [Set Alarm]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10403, KTFSysUtilsSetAlarmStates ) ;

    // I0007.03 DSY [Cancel Alarm]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10404, KTFSysUtilsCancelAlarmStates ) ;

    // I0007.04 DSY [Set and Cancel Alarm]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10405, KTFSysUtilsSetCancelAlarmStates ) ; 

    // I0007.05 DSY [Invalid Alarm Neg.]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10406, KTFSysUtilsSetAlarmInvalidStates ) ; 

    // I0007.06 DSY [Invalid Alarm Sec]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10407, KTFSysUtilsSetAlarmInvalidSecondsStates ) ; 

    //HELPER
    // I0010.02 DSY [Open and Close Helper]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10501, KTFHelperOpenCloseStates ) ;

    // I0010.06 DSY [Startup Reason]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10502, KTFHelperGetStartupReasonStates ) ;

    // I0010.07 DSY [SW Startup Reason]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10503, KTFHelperSetSWStartupReasonStates ) ;

    // I0010.08 DSY [Hidden Reset]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10504, KTFHelperHiddenResetStates ) ;

    // I0007.01 DSY [RTC Status]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10505, KTFHelperGetRTCStatusStates ) ;

    // I0004.01 DSY [Grip Event]
  //  TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10506, KTFHelperGripEventStates ) ;

    //MTC
    // I0010.03 DSY [Open and Close MTC]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10601, KTFMtcOpenCloseStates ) ;

    // I0010.09 DSY [Offline State True]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10602, KTFMtcStateFlagTrueStates ) ;
    
    // I0010.10 DSY [Offline State False]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10603, KTFMtcStateFlagFalseStates ) ;

    // I0010.11 DSY [Invalid Offline State 100]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10604, KTFMtcStateFlagInvalidStates ) ;

    // I0010.12 DSY [RF Inactive and Normal States]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10605, KTFMtcStateRFInactiveNormalStates ) ;

    // SELFTEST
    // I0009.01 DSY [Open and Close Self Test]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10701, KTFSelfTestOpenCloseStates ) ;

    // I0009.02 DSY [Perform Self Test]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10702, KTFSelfTestPerformStates ) ;
#endif // RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW 

    //BT AUDIO
    // I0003.01 DSY [Open and Close BT Audio]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10801, KTFBTAudioOpenCloseStates ) ;
#endif // __ACCESSORY_FW 


    // EXTENSION
    // I0005.01 DSY [Open and Close Extension]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10901, KTFExtensionOpenCloseStates ) ;

    // I0005.02 DSY [Invalid Call Function Sync]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10902, KTFExtensionInvalidSyncCallFunctionStates ) ;

    // I0005.03 DSY [Invalid Call Function Async]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 10903, KTFExtensionInvalidAsyncCallFunctionStates ) ;


#ifndef __ACCESSORY_FW
    // ACCESSORY
    // I0001.05 DSY [Open and Close Accessory]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11005, KTFAccessoryOpenCloseStates ) ;

    // I0001.01 DSY [Loopset On]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11001, KTFAccessoryLoopsetOnStates ) ;

    // I0001.02 DSY [Loopset Tty]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11002, KTFAccessoryLoopsetTtyStates ) ;

    // I0001.03 DSY [Loopset Off]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11003, KTFAccessoryLoopsetOffStates ) ;

    // I0001.04 DSY [Invalid Loopset 100]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11004, KTFAccessoryLoopsetInvalidStates ) ;

    // I0001.08 DSY [Accessory Mode]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 11008, KTFAccessoryGetAccessoryModeStates ) ;

#endif // __ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
    // I0012.01 DSY [BT SAP On]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 12001, KTFMtcBTSapOnStates ) ;
    
    // I0012.02 DSY [BT SAP Off]
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 12002, KTFMtcBTSapOffStates ) ;
#endif // RD_STARTUP_CHANGE

    TF_END_STUB_TEST_CASES();
    }

#ifndef __ACCESSORY_FW
static void AudioUseCaseSuiteL( CTFATestSuite* aSuite ) 
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10110, _L( "I0002.10 DSY [Open and Close Audio]" ) );
    TF_ADD_TEST_CASE_L( 10101, _L( "I0002.01 DSY [Handsfree On]" ) );
    TF_ADD_TEST_CASE_L( 10102, _L( "I0002.02 DSY [Handsfree Off]" ) );
    TF_ADD_TEST_CASE_L( 10103, _L( "I0002.03 DSY [Invalid Handsfree 100]" ) );
    TF_ADD_TEST_CASE_L( 10107, _L( "I0002.07 DSY [Output Volume 0]" ) );
    TF_ADD_TEST_CASE_L( 10108, _L( "I0002.08 DSY [Output Volume 9]" ) );
    TF_ADD_TEST_CASE_L( 10109, _L( "I0002.09 DSY [Invalid Output Volume 100]" ) );
    TF_ADD_TEST_CASE_L( 10111, _L( "I0002.11 DSY [Handsfree On Twice]" ) );
    TF_ADD_TEST_CASE_L( 10112, _L( "I0002.12 DSY [Handsfree Off Twice]" ) );
    TF_ADD_TEST_CASE_L( 10113, _L( "I0002.13 DSY [Invalid Handsfree 100 Twice]" ) );
    TF_ADD_TEST_CASE_L( 10117, _L( "I0002.17 DSY [Output Volume 0 Twice]" ) );
    TF_ADD_TEST_CASE_L( 10118, _L( "I0002.18 DSY [Output Volume 9 Twice]" ) );
    TF_ADD_TEST_CASE_L( 10119, _L( "I0002.19 DSY [Invalid Output Volume 100 Twice]" ) );
    TF_ADD_TEST_CASE_L( 10122, _L( "I0002.22 DSY [Invalid Output Volume -1]" ) );
    TF_ADD_TEST_CASE_L( 10123, _L( "I0002.23 DSY [Invalid Output Volume 10]" ) );
    TF_ADD_TEST_CASE_L( 10124, _L( "I0002.24 DSY [Invalid Output Volume -1 Twice]" ) );
    TF_ADD_TEST_CASE_L( 10125, _L( "I0002.25 DSY [Invalid Output Volume 10 Twice]" ) );

    TF_ADD_TEST_SUITE_REGRESSION_L( 10120, _L( "I0002.20 DSY [Audio Regression]" ), 3 );
    TF_ADD_TEST_SUITE_RANDOMIZE_L( 10121, _L( "I0002.21 DSY [Audio Randomizer]" ), 200 );
    }
    
static void AudioParameterSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10126, _L( "I0002.26 DSY [Output Volume 0 - 9]" ) );
    }
#endif // __ACCESSORY_FW

static void SAEUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10301, _L( "I0010.01 DSY [Open and Close SAE]" ) );
    // NOTE: In HW SAE has already called StartSAE during boot.
#ifdef TESTS_NO_UI
    TF_ADD_TEST_CASE_L( 10302, _L( "I0010.04 DSY [Start SAE]" ) );
#endif
    }

#ifndef RD_STARTUP_CHANGE
static void SysUtilsUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10401, _L( "I0008.01 DSY [Open and Close SysUtils]" ) );
    TF_ADD_TEST_CASE_L( 10402, _L( "I0010.05 DSY [SIM Language]" ) );
    TF_ADD_TEST_CASE_L( 10403, _L( "I0007.02 DSY [Set Alarm]" ) );
    TF_ADD_TEST_CASE_L( 10404, _L( "I0007.03 DSY [Cancel Alarm]" ) );
    TF_ADD_TEST_CASE_L( 10405, _L( "I0007.04 DSY [Set and Cancel Alarm]" ) );
    TF_ADD_TEST_CASE_L( 10406, _L( "I0007.05 DSY [Invalid Alarm Neg.]" ) );
    TF_ADD_TEST_CASE_L( 10407, _L( "I0007.06 DSY [Invalid Alarm Sec]" ) );

    TF_ADD_TEST_SUITE_REGRESSION_L( 10408, _L( "I0007.07 DSY [SysUtils Regression]" ), 3 );
    TF_ADD_TEST_SUITE_RANDOMIZE_L( 10409, _L( "I0007.08 DSY [SysUtils Randomizer]" ), 200 );
    }

static void HelperUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10501, _L( "I0010.02 DSY [Open and Close Helper]" ) );
    TF_ADD_TEST_CASE_L( 10502, _L( "I0010.06 DSY [Startup Reason]" ) );
    TF_ADD_TEST_CASE_L( 10503, _L( "I0010.07 DSY [SW Startup Reason]" ) );
    TF_ADD_TEST_CASE_L( 10504, _L( "I0010.08 DSY [Hidden Reset]" ) );
    TF_ADD_TEST_CASE_L( 10505, _L( "I0007.01 DSY [RTC Status]" ) );
    //TF_ADD_TEST_CASE_L( 10506, _L( "I0004.01 DSY [Grip Event]" ) );

    TF_ADD_TEST_SUITE_REGRESSION_L( 10507, _L( "I0010.13 DSY [Helper Regression]" ), 3 );
    TF_ADD_TEST_SUITE_RANDOMIZE_L( 10508, _L( "I0010.14 DSY [Helper Randomizer]" ), 200 );
    }

static void MtcUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10601, _L( "I0010.03 DSY [Open and Close MTC]" ) );
    TF_ADD_TEST_CASE_L( 10602, _L( "I0010.09 DSY [Offline State True]" ) );
    TF_ADD_TEST_CASE_L( 10603, _L( "I0010.10 DSY [Offline State False]" ) );
    TF_ADD_TEST_CASE_L( 10604, _L( "I0010.11 DSY [Invalid Offline State 100]" ) );
    TF_ADD_TEST_CASE_L( 10605, _L( "I0010.12 DSY [RF Inactive and Normal States]" ) );

    TF_ADD_TEST_SUITE_REGRESSION_L( 10606, _L( "I0010.15 DSY [MTC Regression]" ), 3 );
    TF_ADD_TEST_SUITE_RANDOMIZE_L( 10607, _L( "I0010.16 DSY [MTC Randomizer]" ), 200 );
    }

static void SelfTestUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10701, _L( "I0009.01 DSY [Open and Close Self Test]" ) );
#ifdef __WINS__
    TF_ADD_TEST_CASE_L( 10702, _L( "I0009.02 DSY [Perform Self Test]" ) );
#endif
    }
#endif // RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
static void BTAudioUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10801, _L( "I0003.01 DSY [Open and Close BT Audio]" ) );
    }
#endif // __ACCESSORY_FW

static void ExtensionUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 10901, _L( "I0005.01 DSY [Open and Close Extension]" ) );
    TF_ADD_TEST_CASE_L( 10902, _L( "I0005.02 DSY [Invalid Call Function Sync]" ) );
    TF_ADD_TEST_CASE_L( 10903, _L( "I0005.03 DSY [Invalid Call Function Async]" ) );
    }

#ifndef __ACCESSORY_FW
static void AccessoryUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 11005, _L( "I0001.05 DSY [Open and Close Accessory]" ) );
    TF_ADD_TEST_CASE_L( 11001, _L( "I0001.01 DSY [Loopset On]" ) );
    TF_ADD_TEST_CASE_L( 11002, _L( "I0001.02 DSY [Loopset Tty]" ) );
    TF_ADD_TEST_CASE_L( 11003, _L( "I0001.03 DSY [Loopset Off]" ) );
    TF_ADD_TEST_CASE_L( 11004, _L( "I0001.04 DSY [Invalid Loopset 100]" ) );
    TF_ADD_TEST_CASE_L( 11008, _L( "I0001.08 DSY [Accessory Mode]" ) ) ;

    TF_ADD_TEST_SUITE_REGRESSION_L( 11006, _L( "I0001.06 DSY [Accessory Regression]" ), 3 );
    TF_ADD_TEST_SUITE_RANDOMIZE_L( 11007, _L( "I0001.07 DSY [Accessory Randomizer]" ), 200 );
    }

#endif // __ACCESSORY_FW

#ifndef RD_STARTUP_CHANGE
#ifdef __BT_SAP
static void BluetoothSAPSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 12001, _L( "I0012.01 DSY [BT SAP On]" ) ) ;
    TF_ADD_TEST_CASE_L( 12002, _L( "I0012.02 DSY [BT SAP Off]" ) ) ;
    }
#else
#pragma message ( "BT SAP Disabled" )
static void BluetoothSAPSuiteL( CTFATestSuite* /*aSuite*/ )
    {
    }
#endif
#endif // RD_STARTUP_CHANGE

// End of File
