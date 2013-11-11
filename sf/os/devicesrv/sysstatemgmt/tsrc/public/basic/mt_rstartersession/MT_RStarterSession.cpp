/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:	Unit test code for RStarterSession class
*
*/


//  CLASS HEADER
#include "MT_RStarterSession.h"

//  EXTERNAL INCLUDES
#include <stifparser.h>
#include <stiftestinterface.h>


//  INTERNAL INCLUDES
#include <starterclient.h>
#include <starter.hrh>

// ---------------------------------------------------------
// MT_RStarterSession::LibEntryL
// Returns: Poiter to MT_RStarterSession class
// ---------------------------------------------------------
EXPORT_C MT_RStarterSession* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
	RDebug::Printf("MT_RStarterSession* LibEntryL");
    MT_RStarterSession* libEntry( MT_RStarterSession::NewL( aTestModuleIf ) );
    return libEntry;
    }

// CONSTRUCTION
MT_RStarterSession* MT_RStarterSession::NewL(CTestModuleIf& aTestModuleIf)
    {
	  RDebug::Printf("MT_RStarterSession* NewL");
    MT_RStarterSession* self = new ( ELeave ) MT_RStarterSession( aTestModuleIf );
    return self;
    }

// Destructor (virtual by CBase)
MT_RStarterSession::~MT_RStarterSession()
    {
    Teardown();
    }

// Default constructor
MT_RStarterSession::MT_RStarterSession(CTestModuleIf& /*aTestModuleIf*/)
    {
    }

// -----------------------------------------------------------------------------
// MT_SysLangUtil::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
//
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo MT_RStarterSession::Case( const TInt aCaseNumber ) const
    {

    /**
    * To add new test cases, implement new test case function and add new
    * line to KCases array specify the name of the case and the function
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to
    *    CenRepNotifyHandlerTest.cpp file and to CenRepNotifyHandlerTest.h
    *    header file.
    * 2) Add entry to following KCases array either by using FUNCENTRY or
    *    ENTRY macro.
    * ENTRY macro takes two parameters: test case name and test case
    * function name.
    * FUNCENTRY macro takes only test case function name as a parameter and uses
    * that as a test case name and test case function name.
    */
	RDebug::Printf("MT_RStarterSession* case");
    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array       
        ENTRY( "1 [RStarterSession] Test ConectL", &MT_RStarterSession::T_RStarterSession_Connect ),
        ENTRY( "2 [RStarterSession] Test SetState to Normal", &MT_RStarterSession::T_RStarterSession_SetStateToNormal),
        ENTRY( "3 [RStarterSession] Test SetStateti Alarm", &MT_RStarterSession::T_RStarterSession_SetStateToAlarm),
        ENTRY( "4 [RStarterSession] Test SetState to Charging", &MT_RStarterSession::T_RStarterSession_SetStateToCharging),
        ENTRY( "5 [RStarterSession] Test SetState to OffLine", &MT_RStarterSession::T_RStarterSession_SetStateToOffLine),
        ENTRY( "6 [RStarterSession] Test SetState to BTSap", &MT_RStarterSession::T_RStarterSession_SetStateToBTSap),
        ENTRY( "7 [RStarterSession] Test ResetNetwork", &MT_RStarterSession::T_RStarterSession_ResetNetwork),       
        ENTRY( "8 [RStarterSession] Test IsRTCTimeValid", &MT_RStarterSession::T_RStarterSession_IsRTCTimeValid),
        ENTRY( "9 [RStarterSession] Test ActivateRfForEmergencyCall", &MT_RStarterSession::T_RStarterSession_ActivateRfForEmergencyCall ),
        ENTRY( "10 [RStarterSession] Test DeactivateRfAfterEmergencyCall", &MT_RStarterSession::T_RStarterSession_DeactivateRfAfterEmergencyCall),
        ENTRY( "11 [RStarterSession] Test EndSplashScreen", &MT_RStarterSession::T_RStarterSession_EndSplashScreen),
        ENTRY( "12 [RStarterSession] Test Reset", &MT_RStarterSession::T_RStarterSession_Reset),
        ENTRY( "13 [RStarterSession] Test Shutdown", &MT_RStarterSession:: T_RStarterSession_Shutdown)        
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[aCaseNumber].iCaseName );
    tmp.iMethod = KCases[aCaseNumber].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;

    return tmp;
    }

// -----------------------------------------------------------------------------
// MT_SysLangUtil::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation.
// -----------------------------------------------------------------------------
//
TInt MT_RStarterSession::GetTestCasesL( const TFileName& /*aConfig*/,
    RPointerArray<TTestCaseInfo>& aTestCases )
    {
	RDebug::Printf("MT_RStarterSession* gettestcase");
    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {

        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo;

        // PushL TTestCaseInfo to CleanupStack.
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended
        // successfully the TTestCaseInfo object is owned (and freed)
        // by the TestServer.
        User::LeaveIfError(aTestCases.Append ( newCase ) );

        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// MT_SysLangUtil::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified
// by aTestCase. Test cases that can be run may be requested from
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt MT_RStarterSession::RunTestCaseL( const TInt aCaseNumber,
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {

    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;
        execStatus  = ( this->*iMethod )( aResult );
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;
    }

//  METHODS
    
void MT_RStarterSession::SetupL(  )
    {
    TInt err = iRStarterSession.Connect();
    RDebug::Print(_L("MT_RStarterSession* SetupL Connected to RStarterSession with % error"),err);
    }
    
void MT_RStarterSession::Teardown(  )
    {
    iRStarterSession.Close();
    }
    
TInt MT_RStarterSession::T_RStarterSession_Connect( TTestResult& aResult )
    {
    RStarterSession session;
    TInt err = session.Connect();
    
    if ( err != KErrNone )
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_Connect" );
        aResult.SetResult( err, KDescription );
        return err;
        }
    session.Close();
    return err;
    }
    
TInt MT_RStarterSession::T_RStarterSession_SetStateToNormal( TTestResult& aResult )
    {
    SetupL();
    // State transition from Normal to normal
    TInt err = iRStarterSession.SetState( RStarterSession::ENormal );
    
    if ( (err != KErrArgument) && (err != KErrGeneral) && (err != KErrNone)) 
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_SetStateToNormal " );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return KErrNone;
    }

TInt MT_RStarterSession::T_RStarterSession_SetStateToAlarm( TTestResult& aResult )
    {
    SetupL();
    // State transition from normal to Alarm 
    TInt err = iRStarterSession.SetState( RStarterSession::EAlarm );
    
    if ( err != KErrNotSupported ) 
        {
        _LIT( KDescription, "Incorrect return value T_RStarterSession_SetStateToAlarm" );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return KErrNone;
    }

TInt MT_RStarterSession::T_RStarterSession_SetStateToCharging( TTestResult& aResult )
    {
    SetupL();
    // State transition from normal to ECharging
    TInt err = iRStarterSession.SetState( RStarterSession::ECharging );
    
    if ( err != KErrNotSupported ) 
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_SetStateEToCharging " );	
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return KErrNone;
    }

TInt MT_RStarterSession::T_RStarterSession_SetStateToOffLine( TTestResult& aResult )
    {
    SetupL();
    // State transition from normal to EOffline
    TInt err = iRStarterSession.SetState( RStarterSession::EOffline );
    
    if ( err != KErrNone ) 
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_SetStateTOOffLine " );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return err;
    }

TInt MT_RStarterSession::T_RStarterSession_SetStateToBTSap( TTestResult& aResult )
    {
    SetupL();
    // State transition from ENormal to EBTSap
    TInt err = iRStarterSession.SetState( RStarterSession::EBTSap );
    
    if ( err != KErrNone ) 
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_SetStateToBTSap " );	
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();    
    return err;
    }
    
TInt MT_RStarterSession::T_RStarterSession_Reset( TTestResult& aResult )
    {
    SetupL();
    // Other values will be tested in BAT testing (Manual testing)...
    iRStarterSession.Reset( (RStarterSession::TResetReason)-1 );
    // Make request and ignore return value
    _LIT( KDescription, "T_RStarterSession_Reset - Calling Reset()" );
    aResult.SetResult( KErrNone, KDescription );
    Teardown();
    return KErrNone;
    }
    
TInt MT_RStarterSession::T_RStarterSession_Shutdown(TTestResult& aResult  )
    {
    SetupL();
    iRStarterSession.Shutdown( );
    _LIT( KDescription, "T_RStarterSession_Shutdown - Calling Shutdown()" );
    aResult.SetResult( KErrNone, KDescription );
    Teardown();
    return KErrNone;
    }
    
TInt MT_RStarterSession::T_RStarterSession_ResetNetwork(TTestResult& aResult  )
    {
    SetupL();
    TInt err = iRStarterSession.ResetNetwork( );   
    if ( err != KErrNone )
        {
        _LIT( KDescription, "T_RStarterSession_ResetNetwork - Incorrect return value in " );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return err;
    }
    
TInt MT_RStarterSession::T_RStarterSession_IsRTCTimeValid(TTestResult& aResult  )
    {
    SetupL();
    TBool valid = iRStarterSession.IsRTCTimeValid();
    if ( !valid )
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_IsRTCTimeValid  " );
        aResult.SetResult( valid, KDescription );
        return valid;
        }
    Teardown();
    return KErrNone;
    }
    
TInt MT_RStarterSession::T_RStarterSession_ActivateRfForEmergencyCall(TTestResult& aResult  )
    {
    SetupL();
    TInt err = iRStarterSession.ActivateRfForEmergencyCall( );

    if (( err != KErrNone) && (err != KErrGeneral))
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_ActivateRfForEmergencyCall" );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return err;
    }
    
TInt MT_RStarterSession::T_RStarterSession_DeactivateRfAfterEmergencyCall( TTestResult& aResult )
    {
    SetupL();
    TInt err = iRStarterSession.DeactivateRfAfterEmergencyCall( );
    
    if (( err != KErrNone) && (err != KErrGeneral))
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_DeactivateRfAfterEmergencyCall" );
        aResult.SetResult( err, KDescription );
        return err;
        }
    // Activate RF again
    err = iRStarterSession.ActivateRfForEmergencyCall( );
    Teardown();
    return err;
    }
    
TInt MT_RStarterSession::T_RStarterSession_EndSplashScreen( TTestResult& aResult )
    {
    SetupL();
    TInt err = iRStarterSession.EndSplashScreen( );
    if ( err != KErrNone )
        {
        _LIT( KDescription, "Incorrect return value in T_RStarterSession_EndSplashScreen" );
        aResult.SetResult( err, KDescription );
        return err;
        }
    Teardown();
    return err;
    }

//END OF FILE
