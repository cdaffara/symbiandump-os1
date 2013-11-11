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
* Description:  headsetTestModule test module.
*
*/



#ifndef TSPCLIENTTESTMODULE_H
#define TSPCLIENTTESTMODULE_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <tspclientmapper.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// Logging path
_LIT( KheadsetTestModuleLogPath, "\\logs\\testframework\\headsetTestModule\\" ); 
// Log file
_LIT( KheadsetTestModuleLogFile, "headsetTestModule.txt" ); 

// Function pointer related internal definitions
#ifndef __GCC32__
#define GETPTR
#else
#define GETPTR &
#endif  
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// FORWARD DECLARATIONS
class CTspClientTestModule;

// DATA TYPES

//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CAccMonitoringTestModule::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CTspClientTestModule::* TestFunction)(TTestResult&);

// CLASS DECLARATION

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC 			iCaseName;
        TestFunction 	iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };

// CLASS DECLARATION

/**
*  This a AccMonitorTestModule class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CTspClientTestModule ): public CTestModuleBase
    {
    public:  // Constructors and destructor

	
		
        /**
        * Two-phased constructor.
        */
        static CTspClientTestModule* NewL();

        /**
        * Destructor.
        */
        virtual ~CTspClientTestModule();

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to Reset the 
        *       headsetTestModule. It is called once for every instance of 
        *       TestModuleheadsetTestModule after its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of headsetTestModule.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from headsetTestModule. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; OOMTestResetL may be used to Reset OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestResetL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */); 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );
    
    private:
    
        CActiveScheduler* iActiveScheduler;

        /**
        * C++ default constructor.
        */
        CTspClientTestModule();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Function returning test case name and pointer to test case function.
        * @since ?Series60_version
        * @param aCaseNumber test case number
        * @return TCaseInfo 
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;

        /**
        * Actual Hardcoded test case functions are listed below.
        */

        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetProcessIdPlayingL( TTestResult& aResult );

		/**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndMoveProcessIdPlayingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndRemoveProcessIdPlayingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetTwoAndSetFirstProcessIdPlayingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetProcessIdRegisteredL( TTestResult& aResult );

		/**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndMoveProcessIdRegisteredL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndRemoveProcessIdRegisteredL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetTwoAndSetFirstProcessIdRegisteredL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetProcessIdStoppedL( TTestResult& aResult );

		/**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndMoveProcessIdStoppedL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetAndRemoveProcessIdStoppedL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetTwoAndSetFirstProcessIdStoppedL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt CheckOutOfRangeL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        static TInt CheckTspTargetClients( TInt aMappingType, RArray< TProcessId >& aPidArray );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TBool CheckArrayZero( RArray< TProcessId >& aPidArray );
        
        /**
        * Tests:
        *   MultiThreadTestSyncL
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt MultiThreadTestSyncL( TTestResult& aResult );
        
        void StartThreadFunctionL();
        
        static TInt ThreadEntryFunction( TAny* aPtr );
        
        static TInt DoSyncLockTestL ( TProcessId aProcessId );
        
        
        void ResetRegisteredClients();
        
    private:    // Data
    
       	// Pointer to test (function) to be executed
        TestFunction iMethod;
        
        // Pointer to logger
        CStifLogger * iLog; 
    };

#endif // TSPCLIENTTESTMODULE_H

// End of File
