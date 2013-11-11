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



#ifndef CPPUNIT_TESTCALLER_H
#define CPPUNIT_TESTCALLER_H

#include "TestFrameWork/testCase.h"


/* 
 * A test caller provides access to a test case method 
 * on a test case class.  Test callers are useful when 
 * you want to run an individual test or add it to a suite.
 * 
 * Here is an example:
 * 
 * class CMathTest : public CTestCase 
 * {
 *     public:
 *         void         setUpL ();
 *         void         tearDown ();
 *
 *     protected:
 *         void         testAddL ();
 *         void         testSubtractL ();
 * };
 *
 * CTest* CMathTest::suiteL () 
 * {
 *     CTestSuite *suite = CTestSuite::NewL();
 *     suite->addTestL(CTestCaller<MathTest>::NewL(_L("testAddL") testAddL));
 *     suite->addTestL(CTestCaller<MathTest>::NewL(_L("testSubtractL") testSubtractL));
 *     return suite;
 * }
 *
 * You can use a CTestCaller to bind any test method on a CTestCase
 * class, as long as it does not have parameters and returns void.
 * 
 * See CTestCase
 */


template <class Fixture> class CTestCaller : public CTestCase
	{
public:

    typedef void (Fixture::*TestMethod)();

    static CTestCaller* NewLC (const TDesC8& aName, TestMethod aTest);
    static CTestCaller* NewL (const TDesC8& aName, TestMethod aTest);
	~CTestCaller();

protected:
 
    // From CTestCase:
    void setUpL ()       { iFixture->setUpL (); }
    void executeTestL () { (iFixture->*iTest)(); } 
    void tearDown ()     { iFixture->tearDown (); }

private:

    CTestCaller (TestMethod aTest) : iTest(aTest) { }
    void ConstructL (const TDesC8& aName);

    TestMethod iTest;
    Fixture    *iFixture;
	};


template <class Fixture>
CTestCaller<Fixture>* CTestCaller<Fixture>::NewLC (const TDesC8& aName,
                                                   TestMethod aTest)
	{
	CTestCaller* self = new(ELeave) CTestCaller(aTest);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

template <class Fixture> 
CTestCaller<Fixture>* CTestCaller<Fixture>::NewL (const TDesC8& aName,
                                                  TestMethod aTest)
	{
	CTestCaller* self = NewLC(aName, aTest);
	CleanupStack::Pop();
	return self;
	}


template <class Fixture>
void CTestCaller<Fixture>::ConstructL (const TDesC8& aName)
	{
	CTestCase::ConstructL(aName);
	iFixture = new(ELeave)Fixture;
	}


template <class Fixture>
CTestCaller<Fixture>::~CTestCaller () 
	{
	delete iFixture;
	}

#endif
