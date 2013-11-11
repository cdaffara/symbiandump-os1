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
* Description:  This module contains the implementation of CRemConControlTestCase class 
*	             member functions.
*
*/


// INCLUDE FILES
#include "ctfatesttimer.h"
#include "ctfatestlog.h"
#include "tftypes.h"
#include "siftrace.h"
#include "remconcontrol.h"
#include "remconcontroltestcase.h"
#include "remconcontroltestcaseparam.h"
    
CRemConControlTestCase::CRemConControlTestCase( CRemConControlTestCaseParam* aParameters )
: CTFStubTestCase( KTFStubTypeRemConControl )
, iParameters( aParameters )
    {    
    }
    
void CRemConControlTestCase::ConstructL( void )
    {
    
    iTimer = CTFATestTimer::NewL( *this );
    }
    
CRemConControlTestCase::~CRemConControlTestCase( void )
    {
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::~CRemConControlTestCase() - Enter " ) ) );
    delete iParameters;
    delete iTimer;
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::~CRemConControlTestCase() - Return " ) ) );
    }

void CRemConControlTestCase::Log( TInt aDepth )
    {
    }
    
void CRemConControlTestCase::InitL( void )
	{
	iCurrentStateIndex = 0;
    iCurrentState = iParameters->States()[iCurrentStateIndex];
    iCleanupWait = EFalse; // Added to run same test again.
	}

void CRemConControlTestCase::ActiveTestRunL( void )
    {
    
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::ActiveTestRunL() - Enter " ) ) );
    if ( iCleanupWait )
        {
        CActiveScheduler::Stop();
        }
    else
        {
        RunL();
        }
        
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::ActiveTestRunL() - Return " ) ) );        
    
    }

void CRemConControlTestCase::RunL( void )
    {
    
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::RunL() - Enter " ) ) );
    
   if ( iCurrentState.iFunction != 0 )
        {
        
        TRAPD( result, CallCurrentApiFunctionL() );
        StartNextState( result );
        
        }
    else
        {
        DoCompleteTest( KTFErrNoStub );
        }
        
        TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::RunL() - Return " ) ) );
    }
    
void CRemConControlTestCase::Teardown( void )
	{
	// If the test is terminated by a failing stub test case, 
	// the timer may be left active and thus must be cancelled. 
	iTimer->Cancel();
    iTimer->After( 200000 ); // 100-ms timeout to wait for pending events.
    iCleanupWait = ETrue;
    CActiveScheduler::Start();
	}

// Check that expected event was returned. If there are differences write them to output window 
// and return an error code. 
   
void CRemConControlTestCase::DoCompleteTest( TInt aResult )
    {
    iTimer->Cancel();
    CompleteTest( aResult ); // SAA framework method
    }
    
    
void CRemConControlTestCase::CallCurrentApiFunctionL( void )
    {
    //TRACE_ASSERT( iStub != NULL );
    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::CallCurrentApiFunctionL() - Enter " ) ) );
    if ( iStub != NULL )
        {
        TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::CallCurrentApiFunctionL() - JUUPAA " ) ) );
        
        STATIC_CAST( CRemConControl*, iStub )->CallApiFunctionL( iCurrentState );
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::CallCurrentApiFunctionL() - TUUPAA " ) ) );
        User::Leave( KTFErrNoStub );
        }
     TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::CallCurrentApiFunctionL() - Return " ) ) );        
     
    }
    
void CRemConControlTestCase::StartNextState( TInt aResult )
    {
    // Test case state failed  
    if ( aResult != KErrNone )
        {
	    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::StartNextState() - Test case failed: %d" ), aResult ) );
	    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::StartNextState() - Failed state: %d" ), iCurrentStateIndex + 1) );
	    //STATIC_CAST( CRemConControl*, iStub )->DeleteInstances();
        DoCompleteTest( aResult );
        }
    // Last test case state finished ?
    else if ( iCurrentStateIndex == iParameters->StateCount() - 1 )
        {
	    TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::StartNextState() - Test case complete" ) ) );
        DoCompleteTest( KErrNone );
        }
    // Start the next state
    else
        {
			 TEST_CASE_TRACE( ( _L( "CRemConControlTestCase::StartNextState() - Starting state %d" ), iCurrentStateIndex + 2 ) );
		     iCurrentStateIndex++;
		     iCurrentState = iParameters->States()[iCurrentStateIndex];
		     if ( !iTimer->IsActive() )
		     	{		         
		     	iTimer->After( 2000000 ); 
		     	}
		
	   }
    }     

