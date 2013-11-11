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
* Description:  MediatorTest class member functions
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MediatorTest.h"
#include "mediatortestdata.h"

_LIT_SECURITY_POLICY_PASS(KAlwaysPassPolicy);

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
// CMediatorTest::CMediatorTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMediatorTest::CMediatorTest()
    {

    }

// -----------------------------------------------------------------------------
// CMediatorTest::ConstructL
// Symbian 2nd phase constructor can leave.
//
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CMediatorTest::ConstructL()
    {
    iLog = CStifLogger::NewL( KMediatorTestLogPath, 
                          KMediatorTestLogFile);

    // Sample how to use logging
    _LIT( KLogStart, "MediatorTest logging starts!" );
    iLog->Log( KLogStart );

    /* Create and install active scheduler */
    iMyActiveScheduler = new CActiveScheduler;
    CActiveScheduler::Install(iMyActiveScheduler);
    	
    iTestHelper = CTestCasePropertyWatch::NewL();
    iAnotherTestHelper = CTestCasePropertyWatch::NewL();
    iEventProvider = CMediatorEventProvider::NewL();
    iEventConsumer = CMediatorEventConsumer::NewL(iTestHelper);
    iNotifications = CMediatorNotifications::NewL();
    iCommandResponder = CMediatorCommandResponder::NewL( iTestHelper );
    iCommandInitiator = CMediatorCommandInitiator::NewL( iTestHelper );
    iDefaultVersion = TVersion(5,5,5);
	  //TInt initCountOfDomain = 0;
	  
    User::LeaveIfError( RProperty::Define( KMediatorStifTestPsKeys, KCurrentTestListVerificationVerdict, RProperty::EInt, KAlwaysPassPolicy, KAlwaysPassPolicy ));
	User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestListVerificationVerdict, KErrGeneral ) );
	
	User::LeaveIfError( RProperty::Define( KMediatorStifTestPsKeys, KCurrentTestList,  RProperty::EInt, KAlwaysPassPolicy, KAlwaysPassPolicy ));
	User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KCurrentTestList, KErrNotSupported ) );	
    
	User::LeaveIfError( RProperty::Define( KMediatorStifTestPsKeys, KReceivedTestLists,  RProperty::ELargeByteArray, KAlwaysPassPolicy, KAlwaysPassPolicy ));
	TTestItemListSet events;
	events.iCount = 0;
	TTestItemListSetPckgBuf eventsPckgBuf( events );
	User::LeaveIfError( RProperty::Set( KMediatorStifTestPsKeys, KReceivedTestLists, eventsPckgBuf ) );
    }

// -----------------------------------------------------------------------------
// CMediatorTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMediatorTest* CMediatorTest::NewL()
    {
    CMediatorTest* self = new (ELeave) CMediatorTest;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CMediatorTest::~CMediatorTest()
    {
    delete iLog;
    delete iEventProvider;
    delete iEventConsumer;
    delete iNotifications;
    delete iCommandResponder;
    delete iCommandInitiator;
    delete iTestHelper;
    delete iAnotherTestHelper;
    delete iMyActiveScheduler;
    }

// -----------------------------------------------------------------------------
// CMediatorTest::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMediatorTest::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {

        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();

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
// CMediatorTest::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CMediatorTest::RunTestCaseL( 
    const TInt aCaseNumber,   
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {

    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    _LIT( KLogStartTC, "Starting testcase [%S]" );
    iLog->Log( KLogStartTC, &tmp.iCaseName);

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

// -----------------------------------------------------------------------------
// CMediatorTest::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
// -----------------------------------------------------------------------------
//
TBool CMediatorTest::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt aCaseNumber, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& aFirstMemFailure, 
                                TInt& aLastMemFailure ) 
    {
    _LIT( KLogOOMTestQueryL, "CMediatorTest::OOMTestQueryL" );
    iLog->Log( KLogOOMTestQueryL );     

    aFirstMemFailure = Case( aCaseNumber ).iFirstMemoryAllocation;
    aLastMemFailure = Case( aCaseNumber ).iLastMemoryAllocation;

    return Case( aCaseNumber ).iIsOOMTest;

    }

// -----------------------------------------------------------------------------
// CMediatorTest::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: User may add implementation for OOM test environment initialization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CMediatorTest::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorTest::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: User may add implementation for OOM test warning handling. Usually no
// implementation is required.
// -----------------------------------------------------------------------------
//
void CMediatorTest::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorTest::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: User may add implementation for OOM test environment finalization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CMediatorTest::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CMediatorTest::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File
