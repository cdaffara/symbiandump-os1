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


#ifndef ACCMONITORINGTESTMODULE_H
#define ACCMONITORINGTESTMODULE_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <AccMonitorInfo.h>
#include <AccMonConnectionEmulator.h>
#include <AccPolGenericID.h>
#include <AccMonitor.h>
#include <AccMonTestTimer.h>

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
class CAccMonitoringTestModule;
class TAccPolGenericID;
class CConnectionEmulator;


// DATA TYPES

//enum ?declaration
//typedef ?declaration
//extern ?data_type;
// A typedef for function that does the actual testing,
// function is a type 
// TInt CAccMonitoringTestModule::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (CAccMonitoringTestModule::* TestFunction)(TTestResult&);

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
NONSHARABLE_CLASS( CAccMonitoringTestModule ): public CTestModuleBase,
                                       		   public MAccMonitorObserver
    {
    public:  // Constructors and destructor

	
		
        /**
        * Two-phased constructor.
        */
        static CAccMonitoringTestModule* NewL();

        /**
        * Destructor.
        */
        virtual ~CAccMonitoringTestModule();

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

    protected:  // Functions from base classes

		/**
        * From MHeadsetStatusObserver
        */
        void ConnectedL( CAccMonitorInfo* aInfo );
        
        /**
        * From MHeadsetStatusObserver
        */
        void DisconnectedL( CAccMonitorInfo* aInfo );

        /**
        * From MHeadsetStatusObserver
        */
        void AccMonitorObserverError( TInt aError );
        
    private:
    
        CActiveScheduler* iActiveScheduler;

        /**
        * C++ default constructor.
        */
        CAccMonitoringTestModule();

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
        TInt AccMonitorTestNoL( TTestResult& aResult );

		/**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt AccMonitorTestOneL( TTestResult& aResult );
        
        /**
        * Tests:
        *   HeadsetConnectedL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt AccMonitorTestMultiL( TTestResult& aResult );
        
        /**
        * Tests:
        *   TestHasAccessoryCapaL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt TestHasAccessoryCapaL( TTestResult& aResult );
        
        /**
        * Tests:
        *   TestHasAccessoryDifferentCapasL
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt TestHasAccessoryDifferentCapasL( TTestResult& aResult );
        
        /**
        * Tests:
        *   SetObserver
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObservingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   StartObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObserving1L( TTestResult& aResult );
        
        /**
        * Tests:
        *   StartObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObserving2L( TTestResult& aResult );
        
        /**
        * Tests:
        *   StartObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObserving3L( TTestResult& aResult );
        
        /**
        * Tests:
        *   StartObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StartObserving4L( TTestResult& aResult );
        
        /**
        * Tests:
        *   StopObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt StopObservingL( TTestResult& aResult );
        
        /**
        * Tests:
        *   operator==
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
        TInt CompareInfosL( TTestResult& aResult );
		
		/**
        * Tests:
        *   GetValue function.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt GetValueTestL( TTestResult& aResult );
		
		/**
        * Tests:
        *   Run error.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt RunErrTestL( TTestResult& aResult );
		
		/**
        * Tests:
        *   Starts observing all capabilities.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt StartObserverAllCapasL( TTestResult& aResult );
		
		/**
        * Resets the arrays for correct comparison.
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		void ResetArrays();
		
		
		/**
        * Tests:
        *   GetValue function.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt ConnectHeadsetL( TTestResult& aResult );
		
		/**
        * Tests:
        *   GetValue function.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt ConnectBTHeadsetL( TTestResult& aResult );
		
		/**
        * Tests:
        *   GetValue function.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt DisconnectBTHeadsetL( TTestResult& aResult );
		
		/**
        * Tests:
        *   GetValue function.
        *   IsObserving
        * @since    4.0
        * @param    aResult: Test case result (PASS/FAIL)
        * @return   Symbian OS error code (test case execution error 
        *           that is not returned as test case result in aResult)
        */
		TInt DisconnectHeadsetL( TTestResult& aResult );
		
		TInt GetConnectedAccessoriesL( CAccMonitor* aApi, RConnectedAccessories& aArray, TInt& aAmount );
		
    private:    // Data
    
       	// Flag for leaving
       	TBool iLeaveFlag;
       	// Array of capabilities for wired headset
       	RAccMonCapabilityArray iWiredHeadsetAccessory;
       	
       	// Array of capabilities for wireless headset
       	RAccMonCapabilityArray iWirelessHeadsetAccessory;
       	
       	// Array of capabilities for wireless carkit
       	RAccMonCapabilityArray iWirelessCarKitAccessory;
       	
       	// Array of capabilities for wired carkit
       	RAccMonCapabilityArray iWiredCarKitAccessory;
       	
       	// Array of capabilities for tvout
       	RAccMonCapabilityArray iTVOutAccessory;
       	
       	// Array of capabilities for music stand
       	RAccMonCapabilityArray iMusicStandAccessory;
       	
       	// Array to save the capabilities from some connected accessory
   		CAccMonitorInfo* iConnectedAccessory;
   		
   		CConnectionEmulator* iConnectionEmulator;
        
        CAccMonTestTimer* iTestTimer;
        
        // Pointer to test (function) to be executed
        TestFunction iMethod;
        TBool iRunErrorCheck;
        TBool iDoLeave;
        TBool iWrongAccessory;
        
        // Pointer to logger
        CStifLogger * iLog; 
    };

#endif      // ACCMONITORINGTESTMODULE_H

// End of File