/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exif Library Test DLL
*
*/



// INCLUDE FILES


#include "ExifLibTestStif.h"

// ================= MEMBER FUNCTIONS =========================================


CTestModule::CTestModule()
    {
    }


void CTestModule::ConstructL()
    {  
    iTestSuite = CTestSuite::NewL( _L8( "ExifLibTest" ) );

    iTestSuite->addTestL( CExifLibTestInc::suiteL() );

     /* Install an active scheduler */
    iScheduler = new(ELeave)CActiveScheduler;
    CActiveScheduler::Install(iScheduler);

    }


CTestModule* CTestModule::NewL()
    {

    // Construct new CTestModule instance.
    CTestModule* self = new ( ELeave ) CTestModule();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
  
    }


CTestModule::~CTestModule()
    {
    delete iTestSuite;
    delete iScheduler;
    }


EXPORT_C TInt SetRequirements( CTestModuleParam*& aTestModuleParam, 
                                TUint32& aParameterValid )
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


    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 2*16384; // 2*16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 8*1048576;// 8M heap max

    aTestModuleParam = param;

    return KErrNone;

    }



/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetTestCases

    Description: GetTestCases is used to inquire test cases 
    from the test module. Because this test module have hard coded test cases
    (i.e cases are not read from file), paramter aConfigFile is not used.

    This function loops through all cases defined in Cases() function and 
    adds corresponding items to aTestCases array.

    Parameters: const TFileName&  : in: Configuration file name. Not used                                                       
                RPointerArray<TTestCaseInfo>& aTestCases: out: 
                      Array of TestCases.
    
    Return Values: KErrNone: No error

    Errors/Exceptions: Function leaves if any memory allocation operation fails

    Status: Proposal
    
-------------------------------------------------------------------------------
*/      
TInt CTestModule::GetTestCasesL( const TFileName& /*aConfigFile*/, 
                                     RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; i< iTestSuite->CountTestCases(); i++ )
        {
 
	    // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
    
	    // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

	    // Set number for the testcase.
	    // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

	    // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( iTestSuite->TestCaseName(i) );

	    // Append TTestCaseInfo to the testcase array. After appended 
	    // successfully the TTestCaseInfo object is owned (and freed) 
	    // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );
        
	    // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );

        }
    
    return KErrNone;
  
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: RunTestCase

    Description: Run a specified testcase.

    Function runs a test case specified by test case number. Test case file
    parameter is not used.

    If case number is valid, this function runs a test case returned by
    function Cases(). 
  
    Parameters: const TInt aCaseNumber: in: Testcase number 
                const TFileName& : in: Configuration file name. Not used
                TTestResult& aResult: out: Testcase result
    
    Return Values: KErrNone: Testcase ran.
                   KErrNotFound: Unknown testcase

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModule::RunTestCaseL( const TInt aCaseNumber, 
                                    const TFileName& /*aConfig*/,
                                    TTestResult& aResult )
    {

    // Return value
    TInt execStatus = KErrNone;
    iTestSuite->ExecuteTestL(aResult, aCaseNumber);

    // Return case execution status (not the result of the case execution)
    return execStatus;
      
    }
    

// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------
   
    Function: LibEntryL

    Description: Polymorphic Dll Entry Point
    
    Test framework calls this function to obtain new instance of test module
    class. 

    Parameters:    None
    
    Return Values: CTestModule*    Pointer to CTestModule instance
    
    Errors/Exceptions: Leaves if CTestModule::NewL leaves
    
    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CTestModule* LibEntryL()
    {
    return CTestModule::NewL();

    }


