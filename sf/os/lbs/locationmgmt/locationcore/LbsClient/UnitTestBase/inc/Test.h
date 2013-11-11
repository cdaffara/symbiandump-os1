// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the TTest class that is the base class for a single unit
// test case. Note that test code should never use TTest directly,
// it is sub-classed and instantiated by the TEST macro.
// 
//

/**
 @file Test.h
*/

#ifndef __TEST_H__
#define __TEST_H__

#include <e32std.h>
#include "testregistry.h"

const TInt KTestStackSizes = KDefaultStackSize;
const TInt KTestHeapMinSize = 0x4000;
const TInt KTestHeapMaxSize = 0x8000;
const TTimeIntervalMicroSeconds32 KDefaultTestTimeout = 5*1000*1000;	//5 seconds should be more than enough.

class TTest
	{
public:
	enum TResult
		{
		EPassed,
		EFailed
		};
public:
	TTest(const TText* aName, const TText* aTestScriptName, const TText* aTestSpecId, TInt aIntTestId);
	void DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
				  TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue,
				  const char* aFile, TInt aLineNo);
	inline virtual ~TTest();
	inline TPtrC Name();
	inline TPtrC TestScriptName();
	inline TPtrC TestSpecId();
	inline TInt IntTestId();
	inline void TestFailed(TInt aLineNumOfTestFailure);
	inline TResult Result();
	inline TInt LineNumOfTestFailure();
public:
	virtual void RunTestL() = 0;
private:
	// Un-owned
	const TText* iName;             // eg. TestGetNumModules
	const TText* iTestScriptName;   // eg. F:\lbs\PositionerUnitTest.cpp
	const TText* iTestSpecId;       // a four letter ID, eg. TTID
	TInt iIntTestId;                // a sub-test identifier
	TResult iResult;
	TInt iLineNumOfTestFailure;
	};

/**
 * A virtual destructor, just to keep the gcc compiler happy.
 */
TTest::~TTest()
	{
	}

TPtrC TTest::Name()
	{
	return iName;
	}

TPtrC TTest::TestScriptName()
	{
	return iTestScriptName;
	}

TPtrC TTest::TestSpecId()
	{
	return iTestSpecId;
	}

TInt TTest::IntTestId()
	{
	return iIntTestId;
	}

void TTest::TestFailed(TInt aLineNumOfTestFailure)
	{
	iResult = EFailed;
	iLineNumOfTestFailure = aLineNumOfTestFailure;
	}

TTest::TResult TTest::Result()
	{
	return iResult;
	}

TInt TTest::LineNumOfTestFailure()
	{
	return iLineNumOfTestFailure;
	}


/**
 * This macro defines a sub-class of TTest that will implement
 * a single unit test case. It then starts the function definition,
 * which must follow any invocation of this macro.
 */
#define TEST(aClassName, aScenario, aTestSpecId, aIntTestId) \
class TTest ## aClassName ## aScenario ## aIntTestId : public TTest \
	{    \
public:  \
	TTest ## aClassName ## aScenario ## aIntTestId()  \
	 : TTest(_S(#aScenario), (const TText*) EXP_WIDE_STR(__FILE__), _S(aTestSpecId), aIntTestId) {}  \
	virtual void RunTestL(); \
	}    \
	gTest ## aClassName ## aScenario ## aIntTestId; \
void TTest ## aClassName ## aScenario ## aIntTestId::RunTestL()


/**
 * The CHECK macros are to be used in the body of a test definition,
 * as they depend on methods of TTest.
 */
#define CHECK(aCondition) \
	{  \
	if (!(aCondition)) \
		{  \
		RTestRegistry::TestFailed(Name(), __FILE__, __LINE__, _S(#aCondition)); \
		this->TestFailed(__LINE__); \
		} \
	}

template <typename T1, typename T2>
void CheckEqual(TTest* aTest, const T1& aExpected, const T2& aActual,
				const char* aFile, TInt aLineNo)
	{
	if (aExpected != aActual)
		{
		RTestRegistry::TestFailed(aTest->Name(), aFile, aLineNo, aExpected, aActual);
		aTest->TestFailed(aLineNo);
		}
	}

#define CHECK_EQUAL(aExpected, aActual) \
	CheckEqual(this, aExpected, aActual, __FILE__, __LINE__)

#define TEST_FAILED(aReason) \
	{  \
	RTestRegistry::TestFailed(Name(), __FILE__, __LINE__, _S(#aReason)); \
	this->TestFailed(__LINE__); \
	}

#define DO_PANIC_TEST_L(aThreadFunction, aExpectedExitCat, aExpectedExitReason, \
	aTimeoutValue) \
	{ \
	TExitCategoryName exitName = TPtrC(aExpectedExitCat).Left(KMaxExitCategoryName); \
	DoPanicTestL(aThreadFunction, exitName, aExpectedExitReason, aTimeoutValue, __FILE__, __LINE__); \
	}

#endif // __TEST_H__
