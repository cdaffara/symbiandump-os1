/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*  %version:  bh1dsgas#4 %, % date_modified: % by %derived_by: ravindrr %
*
*/



#ifndef HWRMTARGETMODIFIERPLUGINTESTS_H
#define HWRMTARGETMODIFIERPLUGINTESTS_H

// INCLUDES
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "StifTestModule.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
class CHwrmTargetModifierPluginTests;
class TCaseInfo;
// DESCRIPTION
// This a Test Module interface template 
// that does not really do anything.

typedef TInt (CHwrmTargetModifierPluginTests::* TestFunction)(TTestResult&);    

NONSHARABLE_CLASS(CHwrmTargetModifierPluginTests) 
        :public CTestModuleBase
    {
    public: // Enumerations
       
    private: // Enumerations
       
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHwrmTargetModifierPluginTests* NewL();
        
        /**
        * Destructor.
        */
        ~CHwrmTargetModifierPluginTests();

    public: // New functions
       
    public: // Functions from base classes
        
        /**
        * Init is used to initialize the Test Module. The Test Modules 
        * can use the configure file to read parameters for Test Module 
        * initialization but they can also have their own configure file 
        * or some other routine to initialize themselves.
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * Test cases are inquired from the Test Module by calling GetTestCases. 
        * Test cases are read from the config file specified by aConfigFile. 
        * If the Test Module does not use config files for test case definition 
        * it does not use aConfigFile parameter. Test cases are appended 
        * to CArrayPtrFlat<RTestEngine::TTestCaseInfo> that is a list 
        * consisting of several TTestCaseInfo objects.
        * TTestCaseInfo class defines individual test cases and, if needed, 
        * a test set where the test case belongs to. TTestCaseInfo is 
        * defined in Table 9.
        * The Test Framework will be the owner of the data in the 
        * RPointerArray and it does the memory deallocation.
        */
        TInt GetTestCasesL( const TFileName& aConfigFile, 
            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * RunTestCase is used to run an individual test case specified 
        * by aTestCase. Test cases that can be run may be requested from 
        * Test Module by GetTestCases method before calling RunTestCase.
        * This method is synchronous and test case result is returned as 
        * reference parameter aResult value. The function shall return 
        * KErrNone if case is succesfully started.
        * If test case is not available (i.e invalid configuration file or
        * or case number), then function shall return the error code (i.e
        * (KErrNotFound)                
        */
        TInt RunTestCaseL( const TInt aCaseNumber, 
            const TFileName& aConfig,
            TTestResult& aResult );
        
    protected: // New functions

        TInt CreateAndDestroy(TTestResult& aResult);
        TInt GetFullSystemTarget(TTestResult& aResult);
        TInt ModifyTarget(TTestResult& aResult);

    protected: // Functions from base classes

    public:
        
    private:

		 /**
        * C++ default constructor.
        */
        CHwrmTargetModifierPluginTests();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

		/**
        * Function returning test case name and pointer to test case function
        */
		const TCaseInfo Case ( const TInt aCaseNumber ) const;

    public: // Data
        
    protected: // Data
        
    private: // Data
			
			// Pointer to test (function) to be executed
			TestFunction iMethod;			

    public: // Friend classes
        
    protected: // Friend classes
        
    private: // Friend classes
      
    };

// Function pointer related internal definitions

#ifndef __GCC32__
    #define GETPTR
#else
    #define GETPTR &
#endif  


// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfoInternal
    {
    public:
        const TText* iCaseName;
        TestFunction iMethod;
    };

// An internal structure containing a test case name and
// the pointer to function doing the test
class TCaseInfo
    {
    public:
        TPtrC iCaseName;    
        TestFunction iMethod;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {        
        };

    };


#endif // HWRMTARGETMODIFIERPLUGINTESTS_H
            
// End of File
