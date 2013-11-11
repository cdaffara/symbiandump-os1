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



#ifndef HEADSETTESTMODULE_H
#define HEADSETTESTMODULE_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

#include <HeadsetStatus.h>

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

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CheadsetTestModule;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CheadsetTestModule::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CheadsetTestModule::* TestFunction)(TTestResult&);

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
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };


// CLASS DECLARATION

/**
*  This a headsetTestModule class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CheadsetTestModule): public CTestModuleBase,
                                       public MHeadsetStatusObserver
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CheadsetTestModule* NewL();

        /**
        * Destructor.
        */
        virtual ~CheadsetTestModule();

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
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
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
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

    protected:  // Functions from base classes

        /**
        * From MHeadsetStatusObserver
        */
        void HeadsetStatusChangedL( TBool aHeadsetConnected );

        /**
        * From MHeadsetStatusObserver
        */
        void HeadsetStatusError( TInt aError );
        
    private:
    
        CActiveScheduler* iActiveScheduler;

        /**
        * C++ default constructor.
        */
        CheadsetTestModule();

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
        * @since    3.1
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt HeadsetStatusTestL( TTestResult& aResult );

        /**
        * Tests:
        *   SetObserver
        * @since    3.1
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt SetObserverL( TTestResult& aResult );
        
        /**
        * Tests:
        *   StartObserving
        *   IsObserving
        * @since    3.1
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObservingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   StopObserving
        *   IsObserving
        * @since    3.1
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StopObservingL( TTestResult& aResult );
        
    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 
    };

#endif      // HEADSETTESTMODULE_H

// End of File