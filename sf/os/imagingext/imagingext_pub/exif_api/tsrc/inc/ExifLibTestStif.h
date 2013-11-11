/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __EXIFLIBTEST_H
#define __EXIFLIBTEST_H


//  INCLUDES

#include <e32base.h>

#include "ExifLibTestInc.h"
// CLASS DECLARATION
class CTestModule;
class TCaseInfo;

// Demo testmodule class definition.
NONSHARABLE_CLASS( CTestModule )
        :public CTestModuleBase
    {
    public: // Enumerations
    	// None

    private: // Enumerations
    	// None

    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestModule* NewL();
        
        /**
        * Destructor.
        */
        ~CTestModule();

    public: // New functions
        // None
        
    public: // Functions from base classes

        /**
        * Test cases are inquired from the Test Module by calling GetTestCases. 
        * Test cases are appended to RPointerArray<TTestCaseInfo>& aTestCases
        * that is a list consisting of several TTestCaseInfo objects.
        */
        TInt GetTestCasesL( const TFileName& aConfigFile, 
            RPointerArray<TTestCaseInfo>& aTestCases );
        /**
        * RunTestCase is used to run an individual test case.
        */
        TInt RunTestCaseL( const TInt aCaseNumber, 
                          const TFileName& aConfig,
                          TTestResult& aResult );
        
    protected: // New functions
    	// None
    	
    protected: // Functions from base classes
        // None
    	
    private:

        /**
        * C++ default constructor.
        */
        CTestModule();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();        

        /**
        * Function returning test case name and pointer to test case function
        */
        const TCaseInfo Case ( const TInt aCaseNumber ) const;
        
            
	public: //Data
	    // None
    	
	protected: // Data
        // None
    	
    private: // Data

       CTestSuite *iTestSuite ;
       CActiveScheduler* iScheduler;
    	
    public: // Friend classes
        // None
    	
    protected: // Friend classes
        // None
    	
    private: // Friend classes
        // None
    	
    };
#endif
