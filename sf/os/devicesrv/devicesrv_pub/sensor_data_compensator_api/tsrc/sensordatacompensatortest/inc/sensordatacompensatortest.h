/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CSensorDataCompensatorTest class declaration.
*
*/


#ifndef SENSORDATACOMPENSATORTEST_H
#define SENSORDATACOMPENSATORTEST_H

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

// SYSTEM INCLUDES
#include <stiftestmodule.h>
#include <sensordatacompensator.h>

// FORWARD DECLARATIONS
class TCaseInfo;
class CSensorDataCompensatorTest;


// DESCRIPTION
// This a Test Module interface template
// that does not really do anything.
typedef TInt ( CSensorDataCompensatorTest::*TestFunction )( TTestResult& );

NONSHARABLE_CLASS( CSensorDataCompensatorTest ) : public CTestModuleBase
    {
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CSensorDataCompensatorTest* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CSensorDataCompensatorTest();

public: // Functions from base classes

    // @see CTestModuleBase
    virtual TInt GetTestCasesL( const TFileName& aTestCaseFile, 
        RPointerArray<TTestCaseInfo>& aTestCases );

    // @see CTestModuleBase
    virtual TInt RunTestCaseL( const TInt aCaseNumber, 
        const TFileName& aTestCaseFile,
        TTestResult& aResult );

protected: // New functions

    TInt CreateAndDestroyCompensator( TTestResult& aResult );
    TInt CreateCompensatorL();
    TInt DestroyCompensatorL();
    TInt CheckChannelTypeL( TTestResult& aResult );
    TInt CheckCompensationTypeL( TTestResult& aResult );
    TInt CompensateL( TTestResult& aResult );

private:

    /**
    * C++ default constructor.
    */
    CSensorDataCompensatorTest( CTestModuleIf& aTestModuleIf );

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

    /**
    * Function returning test case name and pointer to test case function
    */
    const TCaseInfo Case( const TInt aCaseNumber ) const;

private: // Data
    
    CActiveScheduler* iActiveScheduler;
    TestFunction iMethod;
    CSensorDataCompensator* iCompensatorApi;
    TSensrvChannelDataTypeId iDataType;
    TInt iCompensationType;

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

    TCaseInfo( const TText* a ) : iCaseName( ( TText* ) a )
        {
        };
    };

#endif // SENSORDATACOMPENSATORTEST_H
            
