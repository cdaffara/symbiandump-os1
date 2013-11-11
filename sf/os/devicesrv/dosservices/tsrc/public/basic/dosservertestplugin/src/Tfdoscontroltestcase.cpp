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
* Description:
*
*/


// INCLUDE FILES
#include <ctfatesttimer.h>
#include <ctfatestlog.h>
#include <tftypes.h>
#include "siftrace.h"
#include "ctfdoscontrol.h"
#include "ctfdoscontroltestcase.h"
#include "ctfdoscontroltestcaseparam.h"

CTFDosControlTestCaseParam::CTFDosControlTestCaseParam( void )
    {
    }


void CTFDosControlTestCaseParam::ConstructL( const TTFDosServerControlTestCaseState* aStates, TInt aStateCount )
    {
    TRACE_ASSERT( aStates != NULL );
    TRACE_ASSERT( aStateCount > 0 );
    iStates = REINTERPRET_CAST( TTFDosServerControlTestCaseState*, User::Alloc( sizeof ( TTFDosServerControlTestCaseState ) * aStateCount ) );
    User::LeaveIfNull( iStates );
    Mem::Copy( iStates, aStates, sizeof ( TTFDosServerControlTestCaseState ) * aStateCount );
    iStateCount = aStateCount;
    }

    
CTFDosControlTestCaseParam::~CTFDosControlTestCaseParam( void )
    {
    User::Free( iStates );
    iStates = NULL;
    }
    
    
const TTFDosServerControlTestCaseState* CTFDosControlTestCaseParam::States( void ) const
    {
    return iStates;
    }
    
    
TInt CTFDosControlTestCaseParam::StateCount( void ) const
    {
    return iStateCount;
    }
    
    
CTFDosControlTestCase::CTFDosControlTestCase( CTFDosControlTestCaseParam* aParameters )
: CTFStubTestCase( KTFStubTypeDosServerControl )
, iParameters( aParameters )
    {
    TRACE_ASSERT( aParameters != NULL );
    if ( aParameters != NULL )
        {
        TRACE_ASSERT( aParameters->States() != NULL );
        TRACE_ASSERT( aParameters->StateCount() > 0 );
        }
    }
    
    
void CTFDosControlTestCase::ConstructL( void )
    {
    iTimer = CTFATestTimer::NewL( *this );
    }
    
    
CTFDosControlTestCase::~CTFDosControlTestCase( void )
    {
    delete iParameters;
    delete iTimer;
    }


void CTFDosControlTestCase::Log( TInt aDepth )
    {
    _LIT( KStart, "DosServer control component, %d states" );
    _LIT( KState, "Flags: %d %d %d %d %d" );
    Logger().WriteList( aDepth, KStart, iParameters->StateCount() );
    for ( TInt i = 0; i < iParameters->StateCount(); i++ )
        {
        Logger().WriteList( aDepth + 1, KState, 
            iParameters->States()[i].iDosFunction,
            iParameters->States()[i].iArg1,
            iParameters->States()[i].iArg2,
            iParameters->States()[i].iExpectedResult,
            iParameters->States()[i].iCompletionEvent );
        }
    }
    
    
void CTFDosControlTestCase::InitL( void )
	{
    iCleanupWait = EFalse;
	iCurrentStateIndex = 0;
    iCurrentState = iParameters->States()[iCurrentStateIndex];
	}


void CTFDosControlTestCase::ActiveTestRunL( void )
    {
    if ( iCleanupWait )
        {
        CActiveScheduler::Stop();
        }
    else
        {
        RunL();
        }
    }
    

void CTFDosControlTestCase::RunL( void )
    {
    TInt result = KErrNone;    
    if ( iCurrentState.iDosFunction != 0 )
        {
        if ( iCurrentState.iDosFunction == ETFDosEvent_Synchronized )
            {
            // Wait event to start next state!
            }
        else
            {
            TRAP( result, CallCurrentDosFunctionL() );
            StartNextState( result );    
            }            
        }  
    }
    
    
void CTFDosControlTestCase::Teardown( void )
	{
	// If the test is terminated by a failing stub test case, 
	// the timer may be left active and thus must be cancelled. 
	iTimer->Cancel();
    iTimer->After( 100000 ); // 100-ms timeout to wait for pending events.
    iCleanupWait = ETrue;
    CActiveScheduler::Start();
	}
    

void CTFDosControlTestCase::NotifyDosEvent( TInt aEvent, TInt aParameter )
    {
    TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::NotifyDosEvent(%d, %d)"), aEvent, aParameter ) );
    
    iCurrentState = iParameters->States()[iCurrentStateIndex];
    
    if ( !IsComplete() )
        {
        if ( aEvent == iCurrentState.iCompletionEvent && iCurrentState.iArg1 == aParameter )
            {
            StartNextState( KErrNone );
            }
            else
            {
            DoCompleteTest( KTFErrDosUnexpectedEvent );    
            }        
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::NotifyDosEvent() - Test already finished" ) ) );
        }
    }
    
    
void CTFDosControlTestCase::DoCompleteTest( TInt aResult )
    {
    iTimer->Cancel();
    CompleteTest( aResult );
    }
    
    
void CTFDosControlTestCase::CallCurrentDosFunctionL( void )
    {
    TRACE_ASSERT( iStub != NULL );
    if ( iStub != NULL )
        {
        STATIC_CAST( CTFDosControl*, iStub )->CallDosFunctionL( iCurrentState );
        }
    else
        {
        User::Leave( KTFErrNoStub );
        }
    }
    
    
CTFDosControlTestCaseParam& CTFDosControlTestCase::Parameters( void )
    {
    return *iParameters;
    }
    
    
TTFDosFunction CTFDosControlTestCase::CurrentDosFunction( void ) const
    {
    return iCurrentState.iDosFunction;
    }
    
    
TInt CTFDosControlTestCase::CurrentArg1( void ) const
    {
    return iCurrentState.iArg1;
    }
    
    
TInt CTFDosControlTestCase::CurrentStateIndex( void ) const
    {
    return iCurrentStateIndex;
    }
    
    
CTFATestTimer* CTFDosControlTestCase::Timer( void )
    {
    return iTimer;
    }
    
    
void CTFDosControlTestCase::StartNextState( TInt aResult )
    {
    if ( aResult != KErrNone )
        {
	    TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::StartNextState() - Test case failed: %d" ), aResult ) );
	    TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::StartNextState() - Failed state: %d" ), iCurrentStateIndex ) );
        DoCompleteTest( aResult );

        }
    else if ( iCurrentStateIndex == iParameters->StateCount() - 1 )
        {
	    TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::StartNextState() - Test case complete" ) ) );
        DoCompleteTest( KErrNone );
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "CTFDosServerControlTestCase::StartNextState() - Starting state %d" ), iCurrentStateIndex + 1 ) );
        iCurrentStateIndex++;
        iCurrentState = iParameters->States()[iCurrentStateIndex];        
        if ( !iTimer->IsActive() )
            {
        	iTimer->After( 100000 ); // Timer calls RunL
    	    }
        }
    }   
    
TInt CTFDosControlTestCase::CheckResult( TInt aResult )
    {    
    
    TInt result(KErrNone);
          
    if ( aResult == iParameters->States()[iCurrentStateIndex].iExpectedResult )
        {                        
        result = KErrNone;    
        }
        else
        {
        result = aResult;            
        }
        
    return result;
    }
    
    
