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
* Description:   ?Description
*
*/



// INCLUDE FILES
#include <e32math.h>
#include <f32file.h>
#include "PlatformVerTest.h"
#include <versioninfo.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

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

_LIT( KS60ProductIDFiles, "z:\\system\\install\\series60v*.sis" );


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
// CPlatformVerTest::Case
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
const TCaseInfo CPlatformVerTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    PlatformVerTest.cpp file and to PlatformVerTest.h 
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
        //FUNCENTRY( PrintTest ),
        ENTRY( "[PlatformVer] GetPlatformVersion", GetPlatformVersion )

        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( PrintTest, ETrue, 1, 3 ),
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
// CPlatformVerTest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CPlatformVerTest::PrintTest( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );
           
    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );
    
    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CPlatformVerTest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CPlatformVerTest::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for( TInt i=0; i<10; i++)
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i);
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CPlatformVerTest::DoPlatformVersionTest
// -----------------------------------------------------------------------------
//
TInt CPlatformVerTest::DoPlatformVersionTest(
        VersionInfo::TVersionBase& aVersion, RFs* aFs )
    {
    TInt fileErr( KErrNone );
    CDir* productIds = NULL;
    TFindFile find( iFs );
    TInt ret( find.FindWildByPath( KS60ProductIDFiles, NULL, productIds ) );
    if ( ret == KErrNone && productIds && productIds->Count() )
        {
        fileErr = KErrNone;
        }
    else
        {
        fileErr = KErrNotSupported;
        }
    if ( aFs )
        {
        ret = VersionInfo::GetVersion( aVersion, *aFs );
        }
    else
        {
        ret = VersionInfo::GetVersion( aVersion );
        }
    if ( fileErr == ret )
        {
        ret = KErrNone;
        }
    delete productIds;
    return ret;
    }

// -----------------------------------------------------------------------------
// CPlatformVerTest::GetPlatformVersion
// Testing relations between retrieving paths via direct methods and 
// via GetRootPathTest( aPath) method.
// Note: This case results KErrGeneral even if just one of the path pairs differ 
// -----------------------------------------------------------------------------
//
TInt CPlatformVerTest::GetPlatformVersion( TTestResult& aResult )
    {
    VersionInfo::TPlatformVersion version1;
    TInt err( DoPlatformVersionTest( version1 ) );

    VersionInfo::TPlatformVersion version2;
    if ( err == KErrNone )
        {
        err = DoPlatformVersionTest( version2, &iFs );
        }

    _LIT( KDescription, "GetPlatformVersion executed. " );
    _LIT( KTextNone, "NONE )" );

    TBuf<100> description;
    description.Copy( KDescription );
  
    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    if( err == KErrNone )
        {
        description.Append( KTextNone );
        }

    aResult.SetResult( err, description );
  
    return KErrNone;
    }

//  End of File
