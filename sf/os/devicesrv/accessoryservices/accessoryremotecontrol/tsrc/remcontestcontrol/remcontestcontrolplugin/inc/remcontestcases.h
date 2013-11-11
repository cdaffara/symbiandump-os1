/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the ?classname class.
*
*/

#include "remcontypes.h"
#include <remconextensionapi.h>

/**************************POC*******************************************************/
//
// ACCREMCON [Poc Press Event]
//  


const TRemConControlTestCaseState KPocPressStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventRepeat }, 0, (TText8*) L"", ETrue, ETFRemConEvent_None },        
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
        { ETFRemCon_PocButtonDownResponse,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None},
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
    };

//
// ACCREMCON [Poc Release Event]
//  
const TRemConControlTestCaseState KPocReleaseStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocUp },
        { ETFRemCon_PocButtonUpResponse,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocUp },
    };
   
//
// ACCREMCON [Poc Invalid Event]
//  
const TRemConControlTestCaseState KInvalidPocStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue,ETFRemConEvent_None },
    };

/**************************Synchronize**************************************************/
//
// ACCREMCON [Synchronize Click Event]
//  
const TRemConControlTestCaseState KSynchronizeClickStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSync, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_Sync },
        { ETFRemCon_SyncButtonResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_Sync },
    };

//
// ACCREMCON [Synchronize Invalid Event]
//  
const TRemConControlTestCaseState KInvalidSyncStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSync, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },        
    };

/**************************Volume*******************************************************/	

//
// ACCREMCON [Volume Up Press Event]
//  
const TRemConControlTestCaseState KVolumeUpPressStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_LaunchAudioClient,            {}, 0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeUp, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventNone },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_VolumeUp},

        /*{ ETFRemCon_CloseAudioClient,            {}, 0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Volume Up Release Short Event]
//  
const TRemConControlTestCaseState KVolumeUpReleaseShortStates[] =
    {   
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeUp, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_VolumeUp },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Volume Up Release Long Event]
//  
const TRemConControlTestCaseState KVolumeUpReleaseLongStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeUp, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_VolumeUp },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Volume Down Press Event]
//  
const TRemConControlTestCaseState KVolumeDownPressStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeDown, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventNone },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_VolumeDown },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Volume Down Release Short Event]
//  
const TRemConControlTestCaseState KVolumeDownReleaseShortStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeDown, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_VolumeDown },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Volume Down Release Long Event]
//  
const TRemConControlTestCaseState KVolumeDownReleaseLongStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeDown, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_VolumeDown},
        { ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }
    };

//
// ACCREMCON [Volume Invalid Event]
//  
const TRemConControlTestCaseState KInvalidVolumeStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
    	{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeVolumeDown, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
    	{ ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_None},

    	/*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };


/**************************Media*******************************************************/	
    							
//
// ACCREMCON [Media Play And Pause Event]
//  
const TRemConControlTestCaseState KMediaPlayAndPauseStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodePlayPause, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},2, (TText8*) L"", ETrue, ETFRemConEvent_CoreApiPlaybackSpeedX1},

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };
                                    
//
// ACCREMCON [Media Stop Event]
//
const TRemConControlTestCaseState KMediaStopStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeStop, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},2, (TText8*) L"", ETrue, ETFRemConEvent_CodeStop },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };
    
//
// ACCREMCON [Media Forward Press Event]
//
const TRemConControlTestCaseState KMediaForwardPressStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeForward, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventRepeat },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_CodeForward},

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Media Forward Release Event]
//
const TRemConControlTestCaseState KMediaForwardReleaseStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeForward, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_CodeForward},

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };
    
//
// ACCREMCON [Media Rewind Press Event]
//
const TRemConControlTestCaseState KMediaRewindPressStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeBack, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventRepeat },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },        
        { ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_CodeBack},

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Media Rewind Release Event]
//
const TRemConControlTestCaseState KMediaRewindReleaseStates[] =
    {
    	{ ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

    	/*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

    	{ ETFRemCon_Subscribe,                      { ERemConKeyCodeBack, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventLongPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},1, (TText8*) L"", ETrue, ETFRemConEvent_CodeBack},

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

    };

//
// ACCREMCON [Media Next Track Event]
//
const TRemConControlTestCaseState KMediaNextTrackStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeForward, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},2, (TText8*) L"", ETrue, ETFRemConEvent_CodeForward },

        /*{ ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }*/

	};
//
// ACCREMCON [Media Previous Track Event]
//
const TRemConControlTestCaseState KMediaPreviousTrackStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },

        /*{ ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },*/

        { ETFRemCon_Subscribe,                      { ERemConKeyCodeBack, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},2, (TText8*) L"", ETrue, ETFRemConEvent_CodeBack },
        { ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }
	};    	

//
// ACCREMCON [Media Invalid Button Event]
//  
const TRemConControlTestCaseState KInvalidMediaStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodePlayPause, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }
    };
    
/**************************MUTE******************************************************/	

//
// ACCREMCON [Mute]
//  
const TRemConControlTestCaseState KMuteStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_LaunchAudioClient,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeMute, ERemConKeyModifierNone,ERemConKeyStateUp, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemCon_CheckClientKey,                {},2, (TText8*) L"", ETrue, ETFRemConEvent_Mute },
        { ETFRemCon_CloseAudioClient,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None }
    };

/**************************TSP*******************************************************/	

//
// ACCREMCON [Command Address Outgoing]
//  
const TRemConControlTestCaseState KAddressOutgoingCommandStates[] =
    {
        { ETFRemConEvent_NormalHandler,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_AddressOutgoingCommand,            {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemConEvent_CheckOutgoingCommandResponse, {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },        
    };

//
// ACCREMCON [Command Permit Outgoing]
//  
const TRemConControlTestCaseState KPermitOutgoingCommandStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_PermitOutgoingCommand,                 {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
    };

//
// ACCREMCON [Command Cancel Outgoing]
//  
const TRemConControlTestCaseState KCancelOutgoingCommandStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_CancelOutgoingCommand,                  {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
    };

//
// ACCREMCON [Response Permit Outgoing]
//  
const TRemConControlTestCaseState KPermitOutgoingResponseStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventRepeat }, 0, (TText8*) L"", ETrue, ETFRemConEvent_None },        
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
        { ETFRemCon_PocButtonDownResponse,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None},
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
    };

//
// ACCREMCON [Notify Address Incoming]
//  
const TRemConControlTestCaseState KAddressIncomingNotifyStates[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_NotifyCommandSubscribe,         {  ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventShortPress },0, (TText8*) L"", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue,ETFRemConEvent_None },
    };

/************************** CALL HANDLING *******************************************************/

//
// ACCREMCON [BT Dial Call]
//  
const TRemConControlTestCaseState KDialCallStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,0, EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtDialCall, (TText8*) L"3391413", EFalse, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,                {},0,(TText8*) L"", EFalse, ETFRemConEvent_DialCall  },
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"", EFalse, ETFRemConEvent_None  },
    };

//
// ACCREMCON [BT Answer Call]
//  
const TRemConControlTestCaseState KAnswerCallStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateRinging,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtAnswerCall, (TText8*) L"2", EFalse, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_AnswerCall  },
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"",    EFalse, ETFRemConEvent_None  },
    };

//
// ACCREMCON [BT End Call]
//  
const TRemConControlTestCaseState KEndCallShortPressStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {}, EPSCTsyCallStateConnected,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtEndCall, (TText8*) L"2", EFalse, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {}, 0, (TText8*) L"",                             EFalse, ETFRemConEvent_EndCall  },
        { ETFRemConEvent_SetCallState,              {}, EPSCTsyCallStateNone,(TText8*) L"",      EFalse, ETFRemConEvent_None  },
    };

//
// ACCREMCON [Answer Call or End Call]
//  
// send/end key pressed, depending on call state: answer call or end call
// Applies to wired and BT
const TRemConControlTestCaseState KAnswerEndCallStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        // test answer call
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateRinging,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtAnswerEnd, (TText8*) L"2", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_AnswerEnd  },
       	// test end call
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateConnected,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtAnswerEnd, (TText8*) L"2", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_AnswerEnd  },
    	// command sent when call in idle state  
    	{ ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      {  ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventShortPress }, ERemConExtAnswerEnd, (TText8*) L"2", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_AnswerEnd  },
    	
    };

//
// ACCREMCON [End Call Long Press: Reject wired call]
//  
// send/end key LONG pressed, an incoming voice call (=ringing); reject wired call
const TRemConControlTestCaseState KRejectWiredCall[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateRinging,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventLongPress }, ERemConExtEndCall, (TText8*) L"1", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                             EFalse, ETFRemConEvent_EndCall  },
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"",      EFalse, ETFRemConEvent_None  },
    };
    
//
// ACCREMCON [End Call Long Press: Voice Dial for Wired]
//  
// send/end key LONG pressed, phone in an idle state; voice dial for wired
const TRemConControlTestCaseState KVoiceDialWired[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"", EFalse, ETFRemConEvent_None  },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateUp, ERemConKeyEventLongPress }, ERemConExtEndCall, (TText8*) L"1", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                             EFalse, ETFRemConEvent_EndCall  },
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"",      EFalse, ETFRemConEvent_None  },
    };

//
// ACCREMCON [Voice Dial]
//  
// send/end key has been long time down = voice dial
const TRemConControlTestCaseState KVoiceDialStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {},EPSCTsyCallStateNone,(TText8*) L"",      EFalse, ETFRemConEvent_None       },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodeSendEnd, ERemConKeyModifierNone, ERemConKeyStateDown, ERemConKeyEventLongPress },ERemConExtVoiceDial, (TText8*) L"1", ETrue, ETFRemConEvent_None },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",EFalse, ETFRemConEvent_VoiceDial  },
    };

//
// ACCREMCON [Last Number Redial]
//  
const TRemConControlTestCaseState KLastNumberRedialStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {}, 0, (TText8*) L"",                               EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,              {}, EPSCTsyCallStateNone,  (TText8*) L"",      EFalse, ETFRemConEvent_None              },
        { ETFRemCon_Subscribe,                      {}, ERemConExtLastNumberRedial, (TText8*) L"", EFalse, ETFRemConEvent_None              },
        { ETFRemConEvent_CheckEvent,             {}, 0, (TText8*) L"",                               EFalse, ETFRemConEvent_LastNumberRedial  },
    };

//
// ACCREMCON [Multiparty call]
//  
const TRemConControlTestCaseState KMultiPartyCallingStates[] =
    {
        { ETFRemConEvent_CmdHandler,             {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,           {}, EPSCTsyCallStateNone,  (TText8*) L"", EFalse, ETFRemConEvent_None               },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,     (TText8*) L"1", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,     (TText8*) L"12", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,     (TText8*) L"21", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,     (TText8*) L"", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,    (TText8*) L"1x", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
        { ETFRemCon_Subscribe,                   {}, ERemConExt3WaysCalling,     (TText8*) L"x1", EFalse, ETFRemConEvent_None               },
        { ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                           EFalse, ETFRemConEvent_MultiPartyCalling  },
    };

//
// ACCREMCON [DTMF]
//  
const TRemConControlTestCaseState KDTMFStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,  {}, EPSCTsyCallStateNone,  (TText8*) L"",   EFalse, ETFRemConEvent_None               },
        { ETFRemCon_Subscribe,          {}, ERemConExtGenerateDTMF,     (TText8*) L"10",  EFalse, ETFRemConEvent_None               },
		{ ETFRemConEvent_CheckEvent,             {},0, (TText8*) L"",                 EFalse, ETFRemConEvent_GenerateDTMF       },
    };
    
//
// ACCREMCON [BT Speed Dial]
//  
const TRemConControlTestCaseState KSpeedDialStates[] =
    {
        { ETFRemConEvent_CmdHandler,                {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,  {}, EPSCTsyCallStateNone,  (TText8*) L"",      EFalse, ETFRemConEvent_None               },
        { ETFRemCon_Subscribe,          {}, ERemConExtSpeedDial,        (TText8*) L"1", EFalse, ETFRemConEvent_None               },

        { ETFRemConEvent_CheckEvent,             {}, 0,(TText8*) L"",                    EFalse, ETFRemConEvent_None          },

    };

//
// ACCREMCON [Not supported]
//  
const TRemConControlTestCaseState KSupportedStates[] =
    {
        { ETFRemConEvent_CmdHandler,    {},0, (TText8*) L"", EFalse, ETFRemConEvent_None }, 
        { ETFRemConEvent_SetCallState,  {}, EPSCTsyCallStateNone,  (TText8*) L"",   EFalse, ETFRemConEvent_None               },
        { ETFRemCon_Subscribe,  		{  ERemConKeyCode4, ERemConKeyModifierNone, ERemConKeyStateDown, ERemConKeyEventRepeat }, ERemConExtGenerateDTMF, (TText8*) L"2", ETrue, ETFRemConEvent_None  },
        { ETFRemConEvent_CheckEvent,    {},0, (TText8*) L"",                 EFalse, ETFRemConEvent_None       },
    };    

//
// ACCREMCON [Poc Press Event - GetCommandSourceInfo]
//  
const TRemConControlTestCaseState KGetSourceCommandInfoStates_Wired[] =
    {
        { ETFRemConEvent_NormalHandler,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_None },
        { ETFRemCon_Subscribe,                      { ERemConKeyCodePOC, ERemConKeyModifierNone,ERemConKeyStateDown, ERemConKeyEventRepeat }, 0, (TText8*) L"", ETrue, ETFRemConEvent_None },        
        { ETFRemConEvent_CheckEvent,                {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
        { ETFRemConEvent_CheckCmdSource,             {}, ERemConExtCmdSourceUnKnown, (TText8*) L"",                           EFalse, ETFRemConEvent_None  },
        { ETFRemCon_PocButtonDownResponse,          {},0, (TText8*) L"", ETrue, ETFRemConEvent_None},
        { ETFRemConEvent_CheckResponse,             {},0, (TText8*) L"", ETrue, ETFRemConEvent_PocDown },
    };
