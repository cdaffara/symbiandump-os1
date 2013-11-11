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
* Description: ExifLibTest
*
*/



#ifndef __CPPUNIT_CTESTSUITE_H
#define __CPPUNIT_CTESTSUITE_H

#include <e32base.h>
#include "TestFrameWork/test.h"

class TestResult;
class CppUnitLog;

/*
* A CTestSuite is a Composite of MTests.
* It runs a collection of test cases.
*
* see MTest and CTestCaller
*/


class CTestSuite : public MTest, public CBase
    {
    public:
        
        static CTestSuite* NewLC(const TDesC8& aName);
        static CTestSuite* NewL(const TDesC8& aName);
        ~CTestSuite ();
        
        void addTestL (MTest *aTest);
        
        // From MTest:
        void ExecuteL (TTestResult& aResult);
        
        // From MTest:
        TInt CountTestCases ();
        
        // From MTest:
        const TDesC8& Name ();
        
        
        // From MTest:
        void ExecuteTestL(TTestResult& aResult,
            TInt aIndex);
        
        // From MTest:
        const TDesC8& TestCaseName (TInt aIndex);
        
    private:
        
        void ConstructL (const TDesC8& aName);
        CTestSuite () { }
        
        RPointerArray<MTest> iTests;
        HBufC8 *iName;
    };

#endif
