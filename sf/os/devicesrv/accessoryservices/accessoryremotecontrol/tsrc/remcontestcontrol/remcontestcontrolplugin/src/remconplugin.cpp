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
* Description:  This module contains the implementation of CXXXPlugin class 
*	             member functions.
*
*/


// INCLUDE FILES
#include "tftestmacros.h"
#include "ctfatestenvironment.h"
#include "siftrace.h"
#include "remconplugin.h"
#include "remconcontrol.h"
#include "remconcontroltestcaseparam.h"
#include "remconcontroltestcase.h"
#include "remcontestcases.h"

// LOCAL FUNCTIONS

// These lines are test case specific and have to be modified
static void KeyEventUseCaseSuiteL( CTFATestSuite* aSuite );

// C++ default constructor can NOT contain any code, that
// might leave.
//
CRemConPlugin::CRemConPlugin( TAny* aInitParams )
: iInitParams( ( CTFStubModuleInterface::TInterfaceInitParams* ) aInitParams )
	{
	}

// Two-phased constructor.
CRemConPlugin* CRemConPlugin::NewL( TAny* aInitParams )
	{
	return new ( ELeave ) CRemConPlugin( aInitParams );
	}

// Destructor.
CRemConPlugin::~CRemConPlugin()
	{
    iInitParams = NULL;
	}

CTFStub* CRemConPlugin::GetStubL( void )
    {
    return new ( ELeave ) CRemConControl();
    }

void CRemConPlugin::BuildTestSuiteL( CTFATestSuite* aRootSuite )
    {
    
        // Memory checks are disabled since DSY runs in separate thread and heap.
    aRootSuite->Environment().SetTestCaseFlags( ETFMayLeakMemory | ETFMayLeakRequests | ETFMayLeakHandles );

    TF_START_TEST_DECLARATION_L( aRootSuite ); 
    TF_ADD_TEST_SUITE_L( _L( "RemCon Test Suite" ) ); 
    TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Key Events" ), KeyEventUseCaseSuiteL );
    
    
    TF_END_TEST_SUITE();        
    TF_END_TEST_DECLARATION();  
    }

MTFStubTestCase* CRemConPlugin::GetStubTestCaseL( TInt aTestCaseId )
    {
    TF_START_STUB_TEST_CASES( aTestCaseId );  
    
    TF_REMCON_CONTROL_TEST_CASE_L( 1,  KPocPressStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 2,  KPocReleaseStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 3,  KInvalidPocStates ); 

    TF_REMCON_CONTROL_TEST_CASE_L( 4,  KSynchronizeClickStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 5,  KInvalidSyncStates ); 

    TF_REMCON_CONTROL_TEST_CASE_L( 6,  KVolumeUpPressStates );     
    TF_REMCON_CONTROL_TEST_CASE_L( 7,  KVolumeUpReleaseShortStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 8,  KVolumeUpReleaseLongStates );     
    TF_REMCON_CONTROL_TEST_CASE_L( 9,  KVolumeDownPressStates );     
    TF_REMCON_CONTROL_TEST_CASE_L( 10, KVolumeDownReleaseShortStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 11, KVolumeDownReleaseLongStates );     
    TF_REMCON_CONTROL_TEST_CASE_L( 12, KInvalidVolumeStates );     
    TF_REMCON_CONTROL_TEST_CASE_L( 13, KMediaPlayAndPauseStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 14, KMediaStopStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 15, KMediaForwardPressStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 16, KMediaForwardReleaseStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 17, KMediaRewindPressStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 18, KMediaRewindReleaseStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 19, KMediaNextTrackStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 20, KMediaPreviousTrackStates ); 
	TF_REMCON_CONTROL_TEST_CASE_L( 21, KInvalidMediaStates );   
	TF_REMCON_CONTROL_TEST_CASE_L( 22, KMuteStates );   
		  
	TF_REMCON_CONTROL_TEST_CASE_L( 23, KAddressOutgoingCommandStates );      	
	TF_REMCON_CONTROL_TEST_CASE_L( 24, KPermitOutgoingCommandStates );      	
	TF_REMCON_CONTROL_TEST_CASE_L( 25, KCancelOutgoingCommandStates );   
		
	TF_REMCON_CONTROL_TEST_CASE_L( 26, KDialCallStates );      		
    TF_REMCON_CONTROL_TEST_CASE_L( 27, KAnswerCallStates );      		
    TF_REMCON_CONTROL_TEST_CASE_L( 28, KEndCallShortPressStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 29, KAnswerEndCallStates );      		
    TF_REMCON_CONTROL_TEST_CASE_L( 30, KRejectWiredCall );      		
    TF_REMCON_CONTROL_TEST_CASE_L( 31, KVoiceDialStates ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 32, KMultiPartyCallingStates );      		
    TF_REMCON_CONTROL_TEST_CASE_L( 33, KDTMFStates );      	
    TF_REMCON_CONTROL_TEST_CASE_L( 34, KSpeedDialStates );
    TF_REMCON_CONTROL_TEST_CASE_L( 35, KVoiceDialWired ); 
    TF_REMCON_CONTROL_TEST_CASE_L( 36, KSupportedStates );      	        
    TF_REMCON_CONTROL_TEST_CASE_L( 37, KLastNumberRedialStates );        

    TF_REMCON_CONTROL_TEST_CASE_L( 38, KPermitOutgoingResponseStates );
    TF_REMCON_CONTROL_TEST_CASE_L( 39, KAddressIncomingNotifyStates );
     
    TF_REMCON_CONTROL_TEST_CASE_L( 40, KGetSourceCommandInfoStates_Wired );
     
    TF_END_STUB_TEST_CASES();   
    }

// Test case specific method
static void KeyEventUseCaseSuiteL( CTFATestSuite* aSuite )
{
	TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 1,  _L( "1 ACCREMCON [Poc Press Event]" ) );
    TF_ADD_TEST_CASE_L( 2,  _L( "2 ACCREMCON [Poc Release Event]" ) );
    TF_ADD_TEST_CASE_L( 3,  _L( "3 ACCREMCON [Poc Invalid Event]" ) );

    TF_ADD_TEST_CASE_L( 4,  _L( "4 ACCREMCON [Synchronize Click Event]" ) );    
    TF_ADD_TEST_CASE_L( 5,  _L( "5 ACCREMCON [Synchronize Invalid Event]" ) );

    TF_ADD_TEST_CASE_L( 6,  _L( "6 ACCREMCON [Volume Up Press Event]" ) );
    TF_ADD_TEST_CASE_L( 7,  _L( "7 ACCREMCON [Volume Up Release Short Event]" ) );
    TF_ADD_TEST_CASE_L( 8,  _L( "8 ACCREMCON [Volume Up Release Long Event]" ) );
    
    TF_ADD_TEST_CASE_L( 9,   _L( "9 ACCREMCON [Volume Down Press Event]" ) );
    TF_ADD_TEST_CASE_L( 10,  _L( "10 ACCREMCON [Volume Down Release Short Event]" ) );
    TF_ADD_TEST_CASE_L( 11,  _L( "11 ACCREMCON [Volume Down Release Long Event]" ) );
	TF_ADD_TEST_CASE_L( 12,  _L( "12 ACCREMCON [Volume Invalid Event]" ) );

    TF_ADD_TEST_CASE_L( 13, _L( "13 ACCREMCON [Media Play And Pause Event]" ) );
    TF_ADD_TEST_CASE_L( 14, _L( "14 ACCREMCON [Media Stop Event]" ) );
    TF_ADD_TEST_CASE_L( 15, _L( "15 ACCREMCON [Media Forward Press Event]" ) );
    TF_ADD_TEST_CASE_L( 16, _L( "16 ACCREMCON [Media Forward Release Event]" ) );
    TF_ADD_TEST_CASE_L( 17, _L( "17 ACCREMCON [Media Rewind Press Event]" ) );
    TF_ADD_TEST_CASE_L( 18, _L( "18 ACCREMCON [Media Rewind Release Event]" ) );
    TF_ADD_TEST_CASE_L( 19, _L( "19 ACCREMCON [Media Next Track Event]" ) );
    TF_ADD_TEST_CASE_L( 20, _L( "20 ACCREMCON [Media Previous Track Event]" ) );
    TF_ADD_TEST_CASE_L( 21, _L( "21 ACCREMCON [Media Invalid Event]" ) );
	TF_ADD_TEST_CASE_L( 22, _L( "22 ACCREMCON [Mute]" ) );

	TF_ADD_TEST_CASE_L( 23, _L( "23 ACCREMCON [Command Address Outgoing]" ) );
	TF_ADD_TEST_CASE_L( 24, _L( "24 ACCREMCON [Command Permit Outgoing]" ) );
	TF_ADD_TEST_CASE_L( 25, _L( "25 ACCREMCON [Command Cancel Outgoing]" ) );
	
	TF_ADD_TEST_CASE_L( 26, _L( "26 ACCREMCON [BT Dial Call]" ) );
	TF_ADD_TEST_CASE_L( 27, _L( "27 ACCREMCON [BT Answer Call]" ) );
	TF_ADD_TEST_CASE_L( 28, _L( "28 ACCREMCON [BT End Call]" ) );
	TF_ADD_TEST_CASE_L( 29, _L( "29 ACCREMCON [Answer/End Call]" ) );
	TF_ADD_TEST_CASE_L( 30, _L( "30 ACCREMCON [Reject Wired Call]" ) );
	TF_ADD_TEST_CASE_L( 31, _L( "31 ACCREMCON [Voice Dial]" ) );
	TF_ADD_TEST_CASE_L( 32, _L( "32 ACCREMCON [Multiparty call]" ) );
	TF_ADD_TEST_CASE_L( 33, _L( "33 ACCREMCON [DTMF]" ) );
	TF_ADD_TEST_CASE_L( 34, _L( "34 ACCREMCON [BT Speed Dial]" ) );
	TF_ADD_TEST_CASE_L( 35, _L( "35 ACCREMCON [Wired Voice Dial]" ) );
	TF_ADD_TEST_CASE_L( 36, _L( "36 ACCREMCON [Not Supported]" ) );	
	TF_ADD_TEST_CASE_L( 37, _L( "37 ACCREMCON [Last Number Redial]" ) );
	
	TF_ADD_TEST_CASE_L( 38, _L( "38 ACCREMCON [Response Permit Outgoing]" ) );
    TF_ADD_TEST_CASE_L( 39, _L( "39 ACCREMCON [Notify Address Incoming]" ) );
    
    TF_ADD_TEST_CASE_L( 40, _L( "40 ACCREMCON [Get Source Command - Wired]" ) );	
}
// End of File
