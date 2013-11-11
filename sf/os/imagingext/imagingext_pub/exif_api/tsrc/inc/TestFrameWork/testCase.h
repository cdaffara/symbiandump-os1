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



#ifndef __CPPUNIT_CTESTCASE_H
#define __CPPUNIT_CTESTCASE_H

#include <e32base.h>
#include "TestFrameWork/test.h"
#include <StifTestModule.h>
class CAssertFailure;
class CTestResult;
class CppUnitLog;

/*
 * A test case defines the fixture to run multiple tests. To define a test case
 * 1) implement a subclass of CTestCase
 * 2) define instance variables that store the state of the fixture
 * 3) initialize the fixture state by overriding setUp
 * 4) clean-up after a test by overriding tearDown.
 *
 * Each test runs in its own fixture so there can be no side effects 
 * among test runs. Here is an example:
 * 
 * class CMathTest : public CTestCase 
 * {
 *     public:
 *
 *     void setUpL () 
 *     {
 *         iValue1 = 2;
 *         iValue2 = 3;
 *     }
 *     
 *     private:
 *
 *     TInt iValue1, iValue2;
 * }
 * 
 * For each test implement a method which interacts with the fixture.
 * Verify the expected results with assertions specified
 * by calling assert on the expression you want to test:
 * 
 *    protected: 
 *    void testAddL ()
 *    {
 *        TInt result = value1 + value2;
 *        assertL (result == 5);
 *    }
 * 
 * The tests to be run can be collected into a CTestSuite:
 * 
 * public: 
 * static CMathTest::suiteL ()
 * {
 *      CTestSuite *suiteOfTests = CTestSuite::NewL(_L8("aSuite"));
 *      suiteOfTests->addTestL(CTestCaller<MathTest>::NewL(_L8("testAddL"), testAddL));
 *      return suiteOfTests;
 * }
 *
 * see CTestSuite and CTestCaller
 *
 */


class CTestCase : public MTest, public CBase 
	{
public:

     ~CTestCase ();

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

protected:

     virtual void ConstructL (const TDesC8& aName);

     void AssertL (TBool aCondition, 
                           const TDesC8& aConditionExpression,
                           TInt  aLineNumber,
                           const TDesC8& aFileName);

     void AssertEqualsL (TInt  aExpected, 
                                 TInt  aActual,
                                 TInt  aLineNumber,
                                 const TDesC8& aFileName);

     void AssertEqualsL (TReal aExpected,
                                 TReal aActual, 
                                 TReal aDelta, 
                                 TInt  aLineNumber,
                                 const TDesC8& aFileName);

     void AssertEqualsL (const TDesC8&  aExpected, 
                                 const TDesC8&  aActual,
                                 TInt           aLineNumber,
                                 const          TDesC8& aFileName);

     void AssertEqualsL (const TDesC16& aExpected, 
                                 const TDesC16& aActual,
                                 TInt           aLineNumber,
                                 const          TDesC8& aFileName);

	 void AllocFailureSimulation (TBool aSwitchedOn);

    virtual void setUpL () = 0;
    virtual void executeTestL () { }
    virtual void tearDown () = 0;

	 CTestCase ();

private:

	TInt ExecuteImplL ();

    HBufC8* NotEqualsMessageLC (const TDesC8& aExpected,
                                const TDesC8& aActual);

    HBufC8* NotEqualsMessageLC (const TDesC16& aExpected,
                                const TDesC16& aActual);

	void AssertFailureToTlsL (const TDesC8& aMessage,
	                          TInt  aLineNumber,
	                          const TDesC8& aFileName);

	CAssertFailure* AssertFailureFromTlsL ();

	TInt HeapCellsReservedByAssertFailure ();

    // data
    HBufC8* iName;
	RHeap::TAllocFail iAllocFailureType;
	TUint iAllocFailureRate;
	};


// A set of macros which allow us to get the line number
// and file name at the point of an assertion failure:

#undef assertL
#define assertL(condition)\
        (this->AssertL ((condition), TPtrC8((TText8*)(#condition)),\
                        __LINE__, TPtrC8((TText8*)__FILE__)))

// Macros for primitive value comparisons
#define assertTIntsEqualL(expected,actual)\
        (this->AssertEqualsL ((expected), (actual),\
                              __LINE__, TPtrC8((TText8*)__FILE__)))

#define assertTRealsEqualL(expected,actual,delta)\
        (this->AssertEqualsL ((expected), (actual), (delta),\
                              __LINE__,TPtrC8((TText8*)__FILE__)))


// Macros for descriptor comparisons
#define assertTDesC8sEqualL(expected,actual)\
        (this->AssertEqualsL ((expected), (actual),\
                              __LINE__, TPtrC8((TText8*)__FILE__)))

#define assertTDesC16sEqualL(expected,actual)\
        (this->AssertEqualsL ((expected), (actual),\
                              __LINE__, TPtrC8((TText8*)__FILE__)))

#if defined(_UNICODE)
#define assertTDesCsEqualL(expected,actual) assertTDesC16sEqualL(expected,actual)
#else
#define assertTDesCsEqualL(expected,actual) assertTDesC8sEqualL(expected,actual)
#endif


#endif
