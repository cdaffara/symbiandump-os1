// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These macros and templates extend the tef testsuite support.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __EXTENDTEF_H__
#define __EXTENDTEF_H__

#include <test/tefunit.h>

// Adds a test suite that is not called CreateSuiteL so that one class can describe multiple subsets of tests.
#define ADD_TEST_SUITE_SUB(subname) \
	_LIT( KTest ## subname, #subname); \
	lTestSuite->AddL( CreateSuiteSub##subname##L( KTest##subname));

// Calls a subfunction to add more members to the current test suite at the current level. Can be the same function as used to create nested tests.
#define ADD_TEST_SUITE_SUB_FLAT(subname) \
	_LIT( KTest ## subname, #subname); \
	CreateSuiteSub##subname##L( KTest##subname,lTestSuite);

// Allows the class suite to be added with a name different to the class name, so class names can be computational
#define ADD_TEST_SUITE_MODULE_SUB_AS(module,subname,as) \
	_LIT( KTest ## as, #as); \
	lTestSuite->AddL( module::CreateSuiteSub##subname##L( KTest##as));

/** Use in a subsuite to distinguish between ADD_TEST_SUITE_SUB and ADD_TEST_SUITE_SUB_FLAT caller
 *	ClassName is required - it defines the name of the class containing the referenced methods. 
 *	aTestSuite is optional - it should be the function parameter aTestSuite or the keyword NULL 
 **/
#define SUB_SUITE_OPT(ClassName,aTestSuite) \
	CTestSuite* lTestSuite = aTestSuite? (CTestSuite*)aTestSuite: CTestSuite::NewL(aName); \
	typedef ClassName	ThisThis;

// Create a dotted name subtest in the script that represents a parameter to the named test
// This should be called from a ADD_TEST_SUITE_SUB with the same name as the test method,
// so that entries can be inserted of the form CMyTestClass.MyTest.EColor256
#define ADD_TEST_STEP_PARAM_1(method,param)	\
	_LIT( KTest ## method ## param, #param); \
	AddTestCaseParam(lTestSuite, KTest ## method ## param, method, param);

// Insert a test that takes a parameter and a range of dotted subtests that set that parameter
// This allows tests of the form CMyTestClass.MyLoopTest.8
#define ADD_TEST_STEP_PARAM_RANGE(method,minparam,maxparam)	\
	_LIT(KTest ## method,#method); \
	AddTestCaseParamRangeL(lTestSuite,KTest ## method,&ThisThis::method,minparam,maxparam);

// Insert a test that takes a parameter and a range of dotted subtests that set that parameter
// This allows tests of the form CMyTestClass.MyLoopTest.8
#define ADD_TEST_STEP_PARAM_RANGE_STEP(method,minparam,maxparam,step)		 \
	_LIT( KTest ## method , #method); \
	AddTestCaseParamRangeL(lTestSuite, KTest ## method, &ThisThis::method, minparam,maxparam,step);

// Insert a test that takes a boolean parameter and a range of dotted subtests that set that parameter 
// Names for dotted tests include "true", "yes", "on", "1"
// This allows tests of the form CMyTestClass.EnableFeature.on
#define ADD_TEST_STEP_PARAM_BOOL(method)	\
	_LIT(KTest ## method,#method); \
	AddTestCaseParamBoolL(lTestSuite,KTest ## method,&ThisThis::method);

// Insert a test that that just requires calling a function
#define ADD_WSGCE_TEST_STEP(classname,method) \
	_LIT(KTest ## method,#method); \
	AddTestCase(lTestSuite,KTest ## method,&classname::method);

// Insert an asyncronus test that that just requires calling a function
#define ADD_WSGCE_ASYNC_TEST_STEP(classname,method) \
	_LIT(KTest ## method,#method); \
	AddAsyncTestCase(lTestSuite,KTest ## method,&classname::method);

// Insert a test that that just requires calling a function
#define ADD_THIS_TEST_STEP(method) \
	_LIT(KTest ## method,#method); \
	AddTestCase(lTestSuite,KTest ## method,&ThisThis::method);

// Insert an asyncronus test that that just requires calling a function
#define ADD_THIS_ASYNC_TEST_STEP(method) \
	_LIT(KTest ## method,#method); \
	AddAsyncTestCase(lTestSuite,KTest ## method,&ThisThis::method);

/*********************************************************************************
 * Base class for derived doers that use the Setup/Shutdown
 *********************************************************************************/
template <class Fixture>
class CTestCaseDoTemplate : public CTestCase
	{
private:
	
public:
	CTestCaseDoTemplate(const TDesC &aName);
	virtual ~CTestCaseDoTemplate();

	virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger);
	virtual void DoRunL(Fixture*aFixture)=0;

protected:
	};

/*********************************************************************************
 * Derived dorun to run a test with a parameter
 *********************************************************************************/
template <class Fixture,class Param>
class CTestCaseParamTemplate : public CTestCaseDoTemplate<Fixture>
	{
private:
	typedef void (Fixture::*TestMethod)(Param aParam);
	
public:
	CTestCaseParamTemplate(const TDesC &aName, TestMethod aTestMethod, Param aParam)
	:	CTestCaseDoTemplate<Fixture>(aName),
		iTest(aTestMethod),iParam(aParam)
	{}
	virtual void DoRunL(Fixture*aFixture)
		{
		(aFixture->*iTest)(iParam);
		}

protected:
	TestMethod	iTest;
	Param iParam;
	};

/***************************************
 * 
 * Implementation of template <class Fixture,class Param> class CTestCaseTemplate
 *
 ***************************************/
template <class Fixture>
CTestCaseDoTemplate<Fixture>::CTestCaseDoTemplate(const TDesC &aName)
: CTestCase(aName)
	{
	}
template <class Fixture>
CTestCaseDoTemplate<Fixture>::~CTestCaseDoTemplate()
	{
	}
template <class Fixture>
void CTestCaseDoTemplate<Fixture>::RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger)
/**
 * Creates and destroys the test fixture, but runs the DoRunL in the derived class, which in turn runs the test. 
 *
 * @param aConfig - Test configuration
 * @param aLogger - Test logger
 */
	{
	Fixture* iFixture = new (ELeave) Fixture();
	CleanupStack::PushL(iFixture);
	
	// Must set the Logger and Config now
	iFixture->SetLoggerL( aLogger );
	iFixture->SetConfigL( aConfig );
	
	// Setup code
	iFixture->SetupL();
	
	// Run the test
	TRAPD( err, DoRunL(iFixture) );

	// Log the result
	CTEFLogger::LogResult( iName, err, aLogger );

	// Now the test case has been logged we need to leave
	// again if that is what the test did
	if( err != KErrTEFUnitPass )
		{
		User::Leave(err);
		}

	// Teardown code
	iFixture->TearDownL();

	CleanupStack::PopAndDestroy();
	}
/**
 * Support method to add a test that takes a specific value as its parameter.
 * Can be used from ADD_TEST_STEP_PARAM_1, or from the methods below
 **/
template <class Param,class Fixture>
inline void AddTestCaseParam(CTestSuite* aTestSuite, const TDesC& aName, void (Fixture::*aTestMethod)(Param aParam),Param aParam)
	{
	CTestCaseParamTemplate<Fixture,Param> *testCaseTemplate = new (ELeave) CTestCaseParamTemplate<Fixture,Param>(aName, aTestMethod,aParam);
	CleanupStack::PushL(testCaseTemplate);
	aTestSuite->AddL( testCaseTemplate );
	CleanupStack::Pop(testCaseTemplate);
	}
/**
 * Adds a test with a range of parameter values. Usually launched from ADD_TEST_STEP_PARAM_RANGE
 * 
 **/
template <class Param,class Fixture>
inline void AddTestCaseParamRangeL(CTestSuite* aTestSuite, const TDesC& aName, void (Fixture::*aTestMethod)(Param aParam),TInt aParamMin,TInt aParamMax,TInt aDelta=1)
	{
	CTestSuite* lTestSuite = CTestSuite::NewL(aName);
	CleanupStack::PushL(lTestSuite);
	__ASSERT_ALWAYS(aDelta!=0 && aParamMin!=aParamMax,User::Invariant());
	__ASSERT_ALWAYS((aDelta>0) == (aParamMax>aParamMin),User::Invariant());
	for (TInt count=aParamMin;count!=aParamMax;count+=aDelta)
		{
		TBuf<10> name2;
		name2.AppendNum(count);
		CTestCaseParamTemplate<Fixture,Param> *testCaseTemplate = new (ELeave) CTestCaseParamTemplate<Fixture,Param>(name2, aTestMethod,Param(count));
		CleanupStack::PushL(testCaseTemplate);
		lTestSuite->AddL( testCaseTemplate );
		CleanupStack::Pop(testCaseTemplate);
		}
	aTestSuite->AddL(lTestSuite);
	CleanupStack::Pop(lTestSuite);
	}

/**
 * Adds a test with a range of parameter values. Usually launched from ADD_TEST_STEP_PARAM_BOOL
 * 
 **/
template <class Param,class Fixture>
inline void AddTestCaseParamBoolL(CTestSuite* aTestSuite, const TDesC& aName, void (Fixture::*aTestMethod)(Param aParam))
	{
	class CTestSuiteBool: public CTestSuite
		{
		public:
		static CTestSuite* NewL(const TTestName& aName)
			{
			CTestSuite* lTestSuite = new (ELeave) CTestSuiteBool(aName);
			CleanupStack::PushL(lTestSuite);
			lTestSuite->ConstructL();
			CleanupStack::Pop();
			return lTestSuite;
			}
		protected:
		CTestSuiteBool(const TTestName& aName):	CTestSuite(aName)
			{	}
		virtual void RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger)
			{	//run "all the tests"
			// Log that the runner is currently in this suite
			CTEFLogger::LogTraverse( iName, aLogger );
			TInt lError=KErrNone;
			for(TInt lIndex = 0; lIndex < Count() && lIndex<2; lIndex++)
				{
				TRAPD(err,TestL(lIndex)->RunL(aConfig, aLogger));
				if(err != KErrTEFUnitPass)
					{
					lError=err;
					}
				}
			if(lError != KErrTEFUnitPass)
				{
				User::Leave(lError);
				}
			}
		
		};
	typedef Fixture	ThisThis;
	CTestSuite* lTestSuite = CTestSuiteBool::NewL(aName);
	CleanupStack::PushL(lTestSuite);
#define yes 1
#define true 1
#define on 1
#define off 0
#define no 0
#define false 0
	
#define Yes 1
#define True 1
#define On 1
#define Off 0
#define No 0
#define False 0
	
#define YES 1
#define ON 1
#define OFF 0
#define NO 0


#define ETrue 1
#define EFalse 0
	//The first two MUST be a true and a false so running "all" will run them both
	ADD_TEST_STEP_PARAM_1(aTestMethod,ETrue);
	ADD_TEST_STEP_PARAM_1(aTestMethod,EFalse);

	ADD_TEST_STEP_PARAM_1(aTestMethod,1);
	ADD_TEST_STEP_PARAM_1(aTestMethod,0);
	
	ADD_TEST_STEP_PARAM_1(aTestMethod,yes);
	ADD_TEST_STEP_PARAM_1(aTestMethod,true);
	ADD_TEST_STEP_PARAM_1(aTestMethod,on);
	ADD_TEST_STEP_PARAM_1(aTestMethod,no);
	ADD_TEST_STEP_PARAM_1(aTestMethod,false);
	ADD_TEST_STEP_PARAM_1(aTestMethod,off);

	ADD_TEST_STEP_PARAM_1(aTestMethod,Yes);
	ADD_TEST_STEP_PARAM_1(aTestMethod,True);
	ADD_TEST_STEP_PARAM_1(aTestMethod,On);
	ADD_TEST_STEP_PARAM_1(aTestMethod,No);
	ADD_TEST_STEP_PARAM_1(aTestMethod,False);
	ADD_TEST_STEP_PARAM_1(aTestMethod,Off);

	ADD_TEST_STEP_PARAM_1(aTestMethod,YES);
	ADD_TEST_STEP_PARAM_1(aTestMethod,TRUE);
	ADD_TEST_STEP_PARAM_1(aTestMethod,ON);
	ADD_TEST_STEP_PARAM_1(aTestMethod,NO);
	ADD_TEST_STEP_PARAM_1(aTestMethod,FALSE);
	ADD_TEST_STEP_PARAM_1(aTestMethod,OFF);

#undef yes 
#undef true
#undef on 
#undef off
#undef no 
#undef false

#undef Yes
#undef True
#undef On
#undef Off
#undef No
#undef False
	
#undef YES
#undef ON
#undef OFF
#undef NO


#undef ETrue
#undef EFalse

	aTestSuite->AddL(lTestSuite);
	CleanupStack::Pop(lTestSuite);
	}

void TefUnitFailLeaveL();
	
#undef 	__ASSERT_SHARED
#define __MAKE_L_STRING(S)	L##S
/**
 * Redefining this macro allows the condition text of the assert to be logged when the fail is reported as well as the file and line number
 * This allows simple issues to be diagnosed without resorting to the source.
 **/
#define __ASSERT_SHARED(aFunction, aMessage) \
	if(!aFunction) \
		{ \
		struct Log	\
			{	/*This wrapper is provided purely to hide the temp string from the debugger.*/ \
			static const TLitC<sizeof("ASSERT FAILED: " #aFunction)>&	Str()\
				{\
				_LIT(aMessage, "ASSERT FAILED: " __MAKE_L_STRING(#aFunction) ); \
				return aMessage;\
				}\
			};\
		Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrAll, Log::Str() );\
		TefUnitFailLeaveL();\
		}
#define ASSERT_NOT_EQUALS(aExpected, aActual) \
	__ASSERT_SHARED(!AssertEquals(aExpected, aActual) , KAssertFailedEquals);

template <class Pointer>
inline TBool AssertTrue(Pointer* aCondition)
	{
	return AssertTrue((TBool)aCondition);
	}


#define LOG_AND_RETURN_IF_GOT_GCE										\
		{																\
		if (GCEIsSupported())											\
			{															\
			INFO_PRINTF1(_L("Test skipped: GCE support is loaded"));	\
			User::Panic(_L("GCE.Wrong.Mode"),1);						\
			return;														\
			}															\
		}
		
#define LOG_AND_PANIC_IF_NOT_GCE											\
		{																	\
		if (!GCEIsSupported())												\
			{																\
			INFO_PRINTF1(_L("Test skipped: GCE support is not loaded"));	\
			User::Panic(_L("GCE.Wrong.Mode"),1);							\
			return;															\
			}																\
		}


#define _LIT2(NAME,STRING)											\
    class NAME														\
    	{															\
    public:															\
    	static const TLitC<sizeof(L##STRING)/2>& S()			\
    		{														\
    		_LIT(singleton,STRING);								\
    		return singleton;										\
    		}														\
    	}	;


#endif	//__EXTENDTEF_H__
