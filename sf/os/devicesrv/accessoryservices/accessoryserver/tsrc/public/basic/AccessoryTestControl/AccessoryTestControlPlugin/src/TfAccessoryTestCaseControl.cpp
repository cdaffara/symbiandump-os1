/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <accpolhdmiobjectcon.h>

#include "siftrace.h"
#include "CtfAccessoryTestControl.h"
#include "CtfAccessoryTestCaseControl.h"
#include "CtfAccessoryTestCaseParamControl.h"

CTFAccessoryTestCaseParamControl::CTFAccessoryTestCaseParamControl( void )
    {

    }

void CTFAccessoryTestCaseParamControl::ConstructL( const TTFAccessoryTestCaseStateControl* aStates,
    TInt aStateCount )
    {
    TRACE_ASSERT( aStates != NULL );
    TRACE_ASSERT( aStateCount> 0 );
    iStates = REINTERPRET_CAST( TTFAccessoryTestCaseStateControl*, User::Alloc( sizeof ( TTFAccessoryTestCaseStateControl ) * aStateCount ) );
    User::LeaveIfNull( iStates );
    Mem::Copy( iStates, aStates, sizeof ( TTFAccessoryTestCaseStateControl ) * aStateCount );
    iStateCount = aStateCount;
    }

CTFAccessoryTestCaseParamControl::~CTFAccessoryTestCaseParamControl( void )
    {
    User::Free( iStates );
    iStates = NULL;
    }

const TTFAccessoryTestCaseStateControl* CTFAccessoryTestCaseParamControl::States( void ) const
    {
    return iStates;
    }

TInt CTFAccessoryTestCaseParamControl::StateCount( void ) const
    {
    return iStateCount;
    }

CTFAccessoryTestCaseControl::CTFAccessoryTestCaseControl( CTFAccessoryTestCaseParamControl* aParameters ) :
    CTFStubTestCase( KTFStubTypeAccessoryControl ), iParameters( aParameters )
    {
    TRACE_ASSERT( aParameters != NULL );
    if ( aParameters != NULL )
        {
        TRACE_ASSERT( aParameters->States() != NULL );
        TRACE_ASSERT( aParameters->StateCount()> 0 );
        }
    }

void CTFAccessoryTestCaseControl::ConstructL( void )
    {
    iTimer = CTFATestTimer::NewL( *this );
    }

CTFAccessoryTestCaseControl::~CTFAccessoryTestCaseControl( void )
    {
    delete iParameters;
    delete iTimer;
    }

void CTFAccessoryTestCaseControl::Log( TInt /*aDepth*/)
    {
    }

void CTFAccessoryTestCaseControl::InitL( void )
    {
    iCleanupWait = EFalse;
    iCurrentStateIndex = 0;
    iCurrentState = iParameters->States()[iCurrentStateIndex];
    iStoredArg1 = KTFErrAccessoryUnexpectedArg1;
    iStoredArg2 = KTFErrAccessoryUnexpectedArg1;
    iStatusReceive = EFalse;
    }

void CTFAccessoryTestCaseControl::ActiveTestRunL( void )
    {
    if( iCleanupWait )
        {
        CActiveScheduler::Stop();
        }
    else
        {
        RunL();
        }
    }

void CTFAccessoryTestCaseControl::RunL( void )
    {
    TInt result = KErrNone;

    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::RunL() - enter" ) ) );

    if( ( iCurrentState.iAccessoryFunction != 0 ) )
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::RunL() - CallCurrentAccessoryFunctionL (%d)" ),iCurrentState.iAccessoryFunction ) );
            TRAP( result, CallCurrentAccessoryFunctionL() );
        }

    result = CheckResult( result, EFalse );

    StartNextState( result );

    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::RunL() - return" ) ) );
    }

void CTFAccessoryTestCaseControl::Teardown( void )
    {
    // If the test is terminated by a failing stub test case, 
    // the timer may be left active and thus must be cancelled. 
    iTimer->Cancel();
    iTimer->After( 100000 );
    iCleanupWait = ETrue;
    CActiveScheduler::Start();
    }

void CTFAccessoryTestCaseControl::DoCompleteTest( TInt aResult )
    {
    iTimer->Cancel();
    CompleteTest( aResult );
    }

void CTFAccessoryTestCaseControl::CallCurrentAccessoryFunctionL( void )
    {
    TRACE_ASSERT( iStub != NULL );
    if ( iStub != NULL )
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CallCurrentAccessoryFunctionL(void) - Stub %x" ), iStub ) );
        STATIC_CAST( CTFAccessoryTestControl*, iStub )->CallAccessoryFunctionL( iCurrentState );
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CallCurrentAccessoryFunctionL(void) - Leave - KErrNoStub" ) ) );
        User::Leave( KTFErrNoStub );
        }
    }

CTFAccessoryTestCaseParamControl& CTFAccessoryTestCaseControl::Parameters( void )
    {
    return *iParameters;
    }

TTFAccessoryFunction CTFAccessoryTestCaseControl::CurrentAccessoryFunction( void ) const
    {
    return iCurrentState.iAccessoryFunction;
    }

TInt CTFAccessoryTestCaseControl::CurrentArg1( void ) const
    {
    return iCurrentState.iArg1;
    }

TInt CTFAccessoryTestCaseControl::CurrentStateIndex( void ) const
    {
    return iCurrentStateIndex;
    }

CTFATestTimer* CTFAccessoryTestCaseControl::Timer( void )
    {
    return iTimer;
    }

void CTFAccessoryTestCaseControl::StartNextState( TInt aResult )
    {
    if( aResult != /*iParameters->States()[iCurrentStateIndex].iExpectedResult*/KErrNone )
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::StartNextState() - Test case failed: %d" ), aResult ) );
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::StartNextState() - Failed state: %d" ), iCurrentStateIndex) );
        STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
        DoCompleteTest( aResult );
        }
    else if( iCurrentStateIndex == iParameters->StateCount() - 1 )
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::StartNextState() - Test case complete" ) ) );
        DoCompleteTest( KErrNone );
        }
    else
        {
        TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::StartNextState() - Starting state %d" ), iCurrentStateIndex ) );
        if( !iTimer->IsActive() )
            {
            iTimer->After( iCurrentState.iTimer );
            }
        iCurrentStateIndex++;
        iCurrentState = iParameters->States()[iCurrentStateIndex];
        }
    }

// Checks that the test results are expected:
//  - iExpectedResult matches unless IgnoreResult flag is set
//  - Test parameters match unless IgnoreParameters flag is set
TInt CTFAccessoryTestCaseControl::CheckResult( TInt aResult, TBool /*isCompleted*/)
    {
    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckResult(%d)" ), aResult ) );
    TInt result( KErrNone );

    if( aResult == KErrNotSupported )
        {
        STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
        result = KErrNotSupported;
        }
    else if( aResult == KTFDontDeleteObserver )
        {
        result = KErrNone;
        }
    else
        {
        result = aResult;
        }

    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckResult - result: %d" ), result ) );
    return result;
    }

//
// Check request result
//    
void CTFAccessoryTestCaseControl::CheckRequest( TRequestStatus aStatus, TInt64 aValue, TUint32& aCapability, TInt* aErr )		
 {

    TTFAccessoryTestCaseStateControl aCurrentState =
        iParameters->States()[iCurrentStateIndex];

    if( ( aStatus != KRequestPending ) &&
        ( aStatus == aCurrentState.iExpectedResult ) )
        {

        if( aCapability )
            {
            if( iParameters->States()[iCurrentStateIndex].iArg3 == aCapability )
                {
                TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest expect: %Ld, value: %Ld  - Unexpected arg3 received" ), iParameters->States()[iCurrentStateIndex].iArg3, aValue ) );
                TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - current state index: %d" ), iCurrentStateIndex ) );
                STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                DoCompleteTest( KTFErrAccessoryUnexpectedArg3 );
                }
            }

        if( iParameters->States()[iCurrentStateIndex].iArg1 != aValue )
            {
 					TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - Unexpected status=%d received for func=%d RequestType=%d" ), aStatus.Int(), aCurrentState.iAccessoryFunction, aCurrentState.iRequestType ) );
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest expect: %Ld, value: %Ld  - Unexpected arg1 received" ), iParameters->States()[iCurrentStateIndex].iArg1, aValue ) );
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - current state index: %d" ), iCurrentStateIndex ) );
            STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
            DoCompleteTest( KTFErrAccessoryUnexpectedArg1 );
            }
 					
 				
 			}
 		else
 			{
 			if(aStatus == KRequestPending)
 			    {
 			     TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - KRequestPending=%d received for func=%d RequestType=%d" ), aStatus.Int(), aCurrentState.iAccessoryFunction, aCurrentState.iRequestType ) );
 			     if(aErr != NULL)
 			         {
 			          (*aErr) = KTFDontDeleteObserver;
 			         }
        }
    else
        {
 			    TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - Other than KRequestPending=%d received for func=%d RequestType=%d" ), aStatus.Int(), aCurrentState.iAccessoryFunction, aCurrentState.iRequestType ) );                        
        if( aCurrentState.iArg1 == KTFDontDeleteObserver )
            {
            //OK, This means that this message shouldn't be received
            }
        else
            {
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - Unexpected status=%d received for func=%d RequestType=%d" ), aStatus.Int(), aCurrentState.iAccessoryFunction, aCurrentState.iRequestType ) );
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckRequest - current state index: %d" ), iCurrentStateIndex ) );
            STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
            DoCompleteTest( KTFErrAccessoryUnexpectedStatus );
            }
        }
    }
    
}

void CTFAccessoryTestCaseControl::CheckObjectRequest( TRequestStatus aStatus,
    TAccValueTypeObject& aValue,
    TUint32& aCapability )
    {

    TTFAccessoryTestCaseStateControl currentState =
        iParameters->States()[iCurrentStateIndex];

    if( ( aStatus != KRequestPending ) &&
        ( aStatus == currentState.iExpectedResult ) )
        {

        if( aCapability )
            {
            if( currentState.iArg3 == aCapability )
                {
                TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckObjectsRequest expect: %Ld, value: %Ld  - Unexpected arg3 received" ), iParameters->States()[iCurrentStateIndex].iArg3, aValue ) );
                TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckObjectsRequest - current state index: %d" ), iCurrentStateIndex ) );
                STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                DoCompleteTest( KTFErrAccessoryUnexpectedArg3 );
                }            
            }
        else
            {
            CAccPolObjectCon* conFromASY = NULL;
            CAccPolObjectCon* conFromPS = NULL;
            switch( currentState.iArg3 )
                {
                case KAccAudioUnits:
                    {
                    TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: Check object for name KAccAudioUnits" ) ) );
                    conFromASY = CAccPolAudioTopologyObjectCon::NewLC();
                    conFromPS = CAccPolAudioTopologyObjectCon::NewLC();
                    break;
                    }
                case KAccVideoHdmiAttributes:
                    {
                    TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: Check object for name KAccVideoHdmiAttributes" ) ) );                    
                    conFromASY = CAccPolHdmiObjectCon::NewLC();
                    conFromPS = CAccPolHdmiObjectCon::NewLC();
                    break;
                    }
                case KAccVideoFormat:
                    {
                    TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: Check object for name KAccVideoFormat" ) ) );                    
                    conFromASY = CAccPolHdmiObjectCon::NewLC();
                    conFromPS = CAccPolHdmiObjectCon::NewLC();
                    break;
                    }
                default:
                    {
                    TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: Unknown name!" ) ) );                    
                    STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                    DoCompleteTest( KTFErrAccessoryUnexpectedResult );
                    break;
                    }
                }

            // Get object container from ASY
            RDesReadStream readStream( aValue.iValue );
            readStream.PushL();
            conFromASY->InternalizeL( readStream );
            CleanupStack::PopAndDestroy( &readStream );
            
            // Get object container from P&S
            CBufFlat* valueBuf = CBufFlat::NewL( 50 );
            CleanupStack::PushL( valueBuf );
            valueBuf->ResizeL( 256 );
            TPtr8 valueBufPtr( valueBuf->Ptr( 0 ) );                
            TInt err = RProperty::Get( KTFAccessoryTestProperty,
                KTFAccessoryObjectMethod,
                valueBufPtr );
            if( err == KErrNone )
                {
                readStream.Open( valueBufPtr );
                readStream.PushL();
                conFromPS->InternalizeL( readStream );
                CleanupStack::PopAndDestroy( &readStream );
                }
            else
                {
                TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: Failed to read P&S cat=KTFAccessoryTestProperty, key=KTFAccessoryObjectMethod, err=%d" ), err ) );
                STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                DoCompleteTest( KTFErrAccessoryUnexpectedResult );
                }

            // Validate: object count
            if( conFromASY->Count() != conFromPS->Count() )
                {
                TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: conFromASY <> conFromPS: Count does not match") ) );
                STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                DoCompleteTest( KTFErrAccessoryUnexpectedResult );
                }
            
            // Validate: Capability specific check
            switch( currentState.iArg3 )
                {
                case KAccVideoFormat:
                    {
                    // Check that video object can be found
                    CAccPolHdmiObjectCon* con =
                        static_cast<CAccPolHdmiObjectCon*>( conFromASY );
                    RAccPolHdmiVideoFormatArray array;
                    CleanupClosePushL( array );
                    con->GetHdmiVideoFormatObjectsL( array );
                    if( !array.Count() )
                        {
                        TEST_CASE_TRACE( ( _L("CTFAccessoryTestCaseControl::CheckObjectsRequest: conFromASY <> conFromPS: Count does not match") ) );
                        STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
                        DoCompleteTest( KTFErrAccessoryUnexpectedResult );
                        }
                    CleanupStack::PopAndDestroy( &array );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            
            // Clean up
            CleanupStack::PopAndDestroy( valueBuf );
            CleanupStack::PopAndDestroy( conFromPS );
            CleanupStack::PopAndDestroy( conFromASY );
            }
        }
    else
        {
        if( currentState.iArg1 == KTFDontDeleteObserver )
            {
            //OK, This means that this message shouldn't be received
            }
        else
            {
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckObjectsRequest - Unexpected status=%d received for func=%d RequestType=%d" ), aStatus.Int(), currentState.iAccessoryFunction, currentState.iRequestType ) );
            TEST_CASE_TRACE( ( _L( "CTFAccessoryTestCaseControl::CheckObjectsRequest - current state index: %d" ), iCurrentStateIndex ) );
            STATIC_CAST( CTFAccessoryTestControl*, iStub )->ResetAccessoryServer();
            DoCompleteTest( KTFErrAccessoryUnexpectedStatus );
            }
        }
    }

