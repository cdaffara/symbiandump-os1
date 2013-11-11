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
* Description:  Implementation of CTFDosServerControlTestCase class
*
*/


// INCLUDE FILES
#include "ctfatesttimer.h"
#include "ctfatestlog.h"
#include "tftypes.h"
#include "dsytesttooltrace.h"
#include "ctfdosservercontrol.h"
#include "ctfdosservercontroltestcase.h"
#include "ctfdosservercontroltestcaseparam.h"

const TInt KTFStateTransitionTimeout = 1000;
const TInt KTFTeardownTimeout = 100000;


CTFDosServerControlTestCaseParam::CTFDosServerControlTestCaseParam( void )
    {
    }


void CTFDosServerControlTestCaseParam::ConstructL( const TTFDosServerControlTestCaseState* aStates, TInt aStateCount )
    {
    TRACE_ASSERT( aStates != NULL );
    TRACE_ASSERT( aStateCount > 0 );
    iStates = REINTERPRET_CAST( TTFDosServerControlTestCaseState*, User::Alloc( sizeof ( TTFDosServerControlTestCaseState ) * aStateCount ) );
    User::LeaveIfNull( iStates );
    Mem::Copy( iStates, aStates, sizeof ( TTFDosServerControlTestCaseState ) * aStateCount );
    iStateCount = aStateCount;
    }

    
CTFDosServerControlTestCaseParam::~CTFDosServerControlTestCaseParam( void )
    {
    User::Free( iStates );
    iStates = NULL;
    }
    
    
const TTFDosServerControlTestCaseState* CTFDosServerControlTestCaseParam::States( void ) const
    {
    return iStates;
    }
    
    
TInt CTFDosServerControlTestCaseParam::StateCount( void ) const
    {
    return iStateCount;
    }
    
    
CTFDosServerControlTestCase::CTFDosServerControlTestCase( CTFDosServerControlTestCaseParam* aParameters )
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
    
    
void CTFDosServerControlTestCase::ConstructL( void )
    {
    iTimer = CTFATestTimer::NewL( *this );
    }
    
    
CTFDosServerControlTestCase::~CTFDosServerControlTestCase( void )
    {
    delete iParameters;
    delete iTimer;
    }


void CTFDosServerControlTestCase::Log( TInt aDepth )
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
    
    
void CTFDosServerControlTestCase::InitL( void )
    {
    iCleanupWait = EFalse;
    iCurrentStateIndex = 0;
    iCurrentState = iParameters->States()[iCurrentStateIndex];
    }


void CTFDosServerControlTestCase::ActiveTestRunL( void )
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
    

void CTFDosServerControlTestCase::RunL( void )
    {
    TInt result = KErrNone;
    TBool syncStart = -1;
    if ( iCurrentState.iDosFunction != 0 )
        {
        // If the state is the start of synchronized block, the current state
        // is changed to the state that represents the next event that should occur.
        if ( iCurrentState.iCompletionEvent == ETFDosEvent_Synchronized )
            {
            COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::RunL() - Starting synchronized block from state %d" ), iCurrentStateIndex + 1 ) );
            syncStart = iCurrentStateIndex++;
            TRACE_ASSERT( iCurrentStateIndex < iParameters->StateCount() );
            TTFDosServerControlTestCaseState state( iCurrentState );
            iCurrentState = iParameters->States()[iCurrentStateIndex];
            TRAP( result, STATIC_CAST( CTFDosServerControl*, iStub )->CallDosFunctionL( state ) );
            }
        else
            {
            TRAP( result, CallCurrentDosFunctionL() );
            }
        }
    // If the current state does not have a completion event, the transition to next state can be made
    // after the result and parameters have been checked.
    // If the current state is not a synchronization state and it contains a completion event,
    // the state change occurs when the event arrives.
    // If CallDSYFunctionL started a synchronized block, the current state should now
    // be the end of the synchronized block. Otherwise the events between the synchronized
    // states did not occur.
    if ( ( syncStart == -1 && iParameters->States()[iCurrentStateIndex].iCompletionEvent == ETFDosEvent_None ) ||
        ( syncStart != -1 && syncStart != iCurrentStateIndex && iParameters->States()[iCurrentStateIndex].iCompletionEvent == ETFDosEvent_Synchronized ) )
        {
        if ( syncStart != -1 )
            {
            COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::RunL() - End of synchronized block ( states %d-%d )" ), syncStart + 1, iCurrentStateIndex + 1 ) );
            }
        result = CheckResult( result, EFalse );
        StartNextState( result );
        }
    else 
        {
        // KErrNotSupported result from a synchronized block or a state with a completion event
        // is not interpreted as passed.
        if ( syncStart != -1 && result == KErrNone )
            {
            TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::RunL() - End of synchronized block missing" ) ) );
            result = KTFErrDosNoSyncEnd;
            }
        if ( result != KErrNone )
            {
            DoCompleteTest( result );
            }
        }
    }
    
    
void CTFDosServerControlTestCase::Teardown( void )
    {
    // If the test is terminated by a failing stub test case, 
    // the timer may be left active and thus must be cancelled. 
    iTimer->Cancel();
    iTimer->After( KTFTeardownTimeout ); // 100-ms timeout to wait for pending events.
    iCleanupWait = ETrue;
    CActiveScheduler::Start();
    }
    

void CTFDosServerControlTestCase::NotifyDosEvent( TInt aEvent, TInt aParameter )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent(%d, %d)" ), aEvent, aParameter ) );
    if ( !IsComplete() )
        {
        if ( aEvent == iParameters->States()[iCurrentStateIndex].iCompletionEvent && aParameter == iParameters->States()[iCurrentStateIndex].iExpectedResult )
            {
            TInt result = CheckResult( aParameter, ETrue );
            StartNextState( result );
            }
        else if ( aEvent == iParameters->States()[iCurrentStateIndex].iCompletionEvent && ( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_IgnoreEventParameters ) )
            {
            TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - Expected result was unspecified, result was %d" ), aParameter ) );
            StartNextState( KErrNone );
            }
        else
            {
            TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - Unexpected event: %d, %d" ), aEvent, aParameter ) );
            TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - Expected event was: %d, %d" ), iParameters->States()[iCurrentStateIndex].iCompletionEvent, iParameters->States()[iCurrentStateIndex].iExpectedResult ) );
            if ( aEvent != iParameters->States()[iCurrentStateIndex].iCompletionEvent && ( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_IgnoreUnexpectedEvents ) )
                {
                TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - IgnoreUnexpectedEvents flag is on" ) ) );
                }
            else
                {
                TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - Failed state: %d" ), iCurrentStateIndex + 1 ) );
                if ( aEvent != iParameters->States()[iCurrentStateIndex].iCompletionEvent )
                    {
                    DoCompleteTest( KTFErrDosUnexpectedEvent - aEvent );
                    }
                else
                    {
                    DoCompleteTest( KTFErrDosUnexpectedEventParameter - aParameter );
                    }
                }
            }
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::NotifyDosEvent() - Test already finished" ) ) );
        }
    }
    
    
void CTFDosServerControlTestCase::DoCompleteTest( TInt aResult )
    {
    iTimer->Cancel();
    CompleteTest( aResult );
    }
    
    
void CTFDosServerControlTestCase::CallCurrentDosFunctionL( void )
    {
    TRACE_ASSERT( iStub != NULL );
    if ( iStub != NULL )
        {
        STATIC_CAST( CTFDosServerControl*, iStub )->CallDosFunctionL( iCurrentState );
        }
    else
        {
        User::Leave( KTFErrNoStub );
        }
    }
    
    
CTFDosServerControlTestCaseParam& CTFDosServerControlTestCase::Parameters( void )
    {
    return *iParameters;
    }
    
    
TTFDosFunction CTFDosServerControlTestCase::CurrentDosFunction( void ) const
    {
    return iCurrentState.iDosFunction;
    }
    
    
TInt CTFDosServerControlTestCase::CurrentArg1( void ) const
    {
    return iCurrentState.iArg1;
    }
    
    
TInt CTFDosServerControlTestCase::CurrentStateIndex( void ) const
    {
    return iCurrentStateIndex;
    }
    
    
CTFATestTimer* CTFDosServerControlTestCase::Timer( void )
    {
    return iTimer;
    }
    
    
void CTFDosServerControlTestCase::StartNextState( TInt aResult )
    {
    if ( aResult != KErrNone )
        {
        TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::StartNextState() - Test case failed: %d" ), aResult ) );
        TEST_CASE_TRACE( ( _L( "    DSYTESTTEST - CTFDosServerControlTestCase::StartNextState() - Failed state: %d" ), iCurrentStateIndex + 1 ) );
        DoCompleteTest( aResult );
        }
    else if ( iCurrentStateIndex == iParameters->StateCount() - 1 )
        {
        DoCompleteTest( KErrNone );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::StartNextState() - Starting state %d" ), iCurrentStateIndex + 2 ) );
        iCurrentStateIndex++;
        iCurrentState = iParameters->States()[iCurrentStateIndex];
        // Timer is active if synchronized block was used
        if ( !iTimer->IsActive() )
            {
            iTimer->After( KTFStateTransitionTimeout ); // Timer calls RunL
            }
        }
    }
    
    
// Checks that the test results are expected:
//  - iExpectedResult matches unless IgnoreResult flag is set or result is KErrNotSupported
//  - Test parameters match unless IgnoreParameters flag is set
TInt CTFDosServerControlTestCase::CheckResult( TInt aResult, TBool aIsEvent )
    {
    TInt result = aResult;
    if ( !aIsEvent && ( aResult == KErrNotSupported ) )
        {
        // KErrNotSupported return value from DosServer function is interpreted as passed.
        // The parameters are not checked in this case since they may contain arbitrary values.
        TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - KErrNotSupported -> KErrNone" ) ) );
        result = KErrNone;
        }
    else
        {
        // If result is not KErrNotSupported it is compared to the expected result unless
        // IgnoreResult flag is set. If the result does not match, the test case fails
        if ( ( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_IgnoreResult ) || 
            ( aResult == iParameters->States()[iCurrentStateIndex].iExpectedResult ) )
            {
            if ( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_StoreParameters )
                {
                COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Parameters stored: %d, %d" ), iCurrentState.iArg1, iCurrentState.iArg2 ) );
                // StoreParameters flag tells that the values from current state are stored to
                // be used when a state with UseParameters is encountered
                iStoredArg1 = iCurrentState.iArg1;
                iStoredArg2 = iCurrentState.iArg2;
                result = KErrNone;
                }
            else if ( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_UseParameters )
                {
                if ( iStoredArg1 != iCurrentState.iArg1 )
                    {
                    TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Stored argument 1 ( %d ) did not match %d" ), iStoredArg1, iCurrentState.iArg1 ) );
                    result = KTFErrDosUnexpectedArg1 - iCurrentState.iArg1;
                    }
                else if ( iStoredArg2 != iCurrentState.iArg2 )
                    {
                    TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Stored argument 2 ( %d ) did not match %d" ), iStoredArg2, iCurrentState.iArg2 ) );
                    result = KTFErrDosUnexpectedArg2 - iCurrentState.iArg2;
                    }
                else
                    {
                    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Success" ) ) );
                    result = KErrNone;
                    }
                }
            // The parameters of the current state are changed by the DosServer function if it
            // contains output parameters. The changed parameters are compared to the expected
            // parameters and if they do not match, the test case is failed.
            else if ( !( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_IgnoreParameters ) && 
                       ( iParameters->States()[iCurrentStateIndex].iArg1 != iCurrentState.iArg1 ) )
                {
                TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Argument 1 changed from %d to %d" ), iParameters->States()[iCurrentStateIndex].iArg1, iCurrentState.iArg1 ) );
                result = KTFErrDosUnexpectedArg1 - iCurrentState.iArg1;
                }
            else if ( !( iParameters->States()[iCurrentStateIndex].iStateFlags & ETFDosFlags_IgnoreParameters ) && 
                       ( iParameters->States()[iCurrentStateIndex].iArg2 != iCurrentState.iArg2 ) )
                {
                TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Argument 2 changed from %d to %d" ), iParameters->States()[iCurrentStateIndex].iArg2, iCurrentState.iArg2 ) );
                result = KTFErrDosUnexpectedArg2 - iCurrentState.iArg2;
                }
            else
                {
                COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Success" ) ) );
                result = KErrNone;
                }
            }
        else
            {
            TEST_CASE_TRACE( ( _L( "    DSYTESTTOOL - CTFDosServerControlTestCase::CheckResult - Unexpected result: %d" ), aResult ) );
            result = KTFErrDosUnexpectedResult + aResult;
            }
        }
    return result;
    }
    
    
