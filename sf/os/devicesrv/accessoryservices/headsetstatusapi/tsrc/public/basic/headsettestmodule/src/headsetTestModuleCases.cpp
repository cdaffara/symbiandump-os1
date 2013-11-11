/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <e32base.h>
#include "headsetTestModule.h"
#include <HeadsetStatus.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
_LIT( KEnter, "Enter" );
_LIT( KOnGoing, "On-going" );
_LIT( KExit, "Exit" );

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CheadsetTestModule::Case
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
const TCaseInfo CheadsetTestModule::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    headsetTestModule.cpp file and to headsetTestModule.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        ENTRY( "HeadsetStatus test", &CheadsetTestModule::HeadsetStatusTestL ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( PrintTest, ETrue, 1, 3 ),
        
        ENTRY( "SetObserver test",      &CheadsetTestModule::SetObserverL ),
        ENTRY( "StartObserving test",   &CheadsetTestModule::StartObservingL ),
        ENTRY( "StopObserving test",    &CheadsetTestModule::StopObservingL )
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::HeadsetStatusChangedL
// -----------------------------------------------------------------------------
//
void CheadsetTestModule::HeadsetStatusChangedL( TBool /*aHeadsetConnected*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::HeadsetStatusError
// -----------------------------------------------------------------------------
//
void CheadsetTestModule::HeadsetStatusError( TInt /*aError*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::HeadsetStatusTestL
// -----------------------------------------------------------------------------
//
TInt CheadsetTestModule::HeadsetStatusTestL( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KStatusTest, "HeadsetStatusTest" );

    TestModuleIf().Printf( 0, KStatusTest, KEnter );
    CHeadsetStatus* api = CHeadsetStatus::NewL();
    CleanupStack::PushL(api);
    TInt err(KErrNone);
    TBool connected(EFalse);
    err = api->HeadsetConnectedL(connected);
    TestModuleIf().Printf( 1, KStatusTest, KOnGoing );
    CleanupStack::PopAndDestroy(api);
    TestModuleIf().Printf( 0, KStatusTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if ((err == KErrNone) && (connected))
        {
        _LIT( KDescription, "Headset connected" );
        aResult.SetResult( err, KDescription );
        }
    else if ((err == KErrNone) && (!connected))     
        {
        _LIT( KDescription, "Headset not connected" );
        aResult.SetResult( err, KDescription );
        }
    else 
        {
        _LIT( KDescription, "Error reading headset status" );
        aResult.SetResult( err, KDescription );
        }    
    

    // Case was executed
    return err;

    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::SetObserverL
// -----------------------------------------------------------------------------
//
TInt CheadsetTestModule::SetObserverL( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    _LIT( KStatusTest, "SetObserverTest" );

    // Enter
    TestModuleIf().Printf( 0, KStatusTest, KEnter );
    CHeadsetStatus* obj = CHeadsetStatus::NewL();
    CleanupStack::PushL( obj );
    
    // Test
    TestModuleIf().Printf( 1, KStatusTest, KOnGoing );
    obj->SetObserver( this );
    
    // Exit
    CleanupStack::PopAndDestroy(); // obj
    TestModuleIf().Printf( 0, KStatusTest, KExit );

    // Check results
    aResult.SetResult( err, _L("Observer set without errors") );
    
    // Case executed
    return err;
    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::StartObservingL
// -----------------------------------------------------------------------------
//
TInt CheadsetTestModule::StartObservingL( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    _LIT( KStatusTest, "StartObservingTest" );

    // Enter
    TestModuleIf().Printf( 0, KStatusTest, KEnter );
    CHeadsetStatus* obj = CHeadsetStatus::NewL();
    CleanupStack::PushL( obj );
    
    // Test
    TestModuleIf().Printf( 1, KStatusTest, KOnGoing );
    obj->SetObserver( this );
    TL( !obj->IsObserving() );
    obj->StartObserving();
    TL( obj->IsObserving() );
    
    // Exit
    CleanupStack::PopAndDestroy(); // obj
    TestModuleIf().Printf( 0, KStatusTest, KExit );

    // Check results
    aResult.SetResult( err, _L("Observing started without errors") );
    
    // Case executed
    return err;
    }

// -----------------------------------------------------------------------------
// CheadsetTestModule::StopObservingL
// -----------------------------------------------------------------------------
//
TInt CheadsetTestModule::StopObservingL( 
    TTestResult& aResult )
    {
    TInt err( KErrNone );
    _LIT( KStatusTest, "StopObservingTest" );

    // Enter
    TestModuleIf().Printf( 0, KStatusTest, KEnter );
    CHeadsetStatus* obj = CHeadsetStatus::NewL();
    CleanupStack::PushL( obj );
    
    // Test
    TestModuleIf().Printf( 1, KStatusTest, KOnGoing );
    obj->SetObserver( this );
    obj->StartObserving();
    obj->StopObserving();
    TL( !obj->IsObserving() );
    
    // Exit
    CleanupStack::PopAndDestroy(); // obj
    TestModuleIf().Printf( 0, KStatusTest, KExit );

    // Check results
    aResult.SetResult( err, _L("Observing stopped without errors") );
    
    // Case executed
    return err;
    }

//  End of File
