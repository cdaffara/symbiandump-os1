// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TGRAPHICSHARNESS_H__
#define __TGRAPHICSHARNESS_H__

#include <test/testexecutestepbase.h>
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>


/**
  A main interface for implementation test cases
 */
class MTestCases
	{
public:
	virtual void RunTestCaseL(TInt aCurTestCase) = 0; // this function needs to be implemented by the derived class
	virtual TInt RunTestCaseLeft(TInt aError) = 0;
	};

class CTestManager;

/**
  The abstract class is base for test programs, encapsulates the MTestCases interface.
	
  All test cases must be implemented in derived classes.  
*/
class CTGraphicsBase : public CBase, public MTestCases
	{
friend class CTGraphicsStep;
public:
	IMPORT_C static const TDesC& ColorModeName(TDisplayMode aMode);
	inline static const TDesC& ColorModeName(TInt aMode);
	IMPORT_C static const TDesC& RotationName(CFbsBitGc::TGraphicsOrientation aOrientation);
	inline static const TDesC& RotationName(TInt aOrientation);
	IMPORT_C static void SaveScreenShotL(CFbsScreenDevice* aScdv);
	IMPORT_C CTGraphicsBase(CTestStep* aStep);
	IMPORT_C ~CTGraphicsBase();
	CTestExecuteLogger&	Logger() {return iStep->Logger();}
	// LEGACY methods
	inline void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine); 
	inline void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
		
	inline void testSoftFailWinscw(TBool aCondition, const TText8* aFile, TInt aLine);
		
protected:
	IMPORT_C void TestComplete();	//test program has to call this function in order to
									//stop the consequence of test cases and quit the step
	IMPORT_C void SetSelfDrive(TBool aSelfDrive);
	IMPORT_C void CaseComplete();
	virtual void ConstructL(){}
	IMPORT_C void ResetCounter(TInt aNewCurrentTestCase = 0);
private:
	void Execute();
	void InitializeL();
	IMPORT_C TInt RunTestCaseLeft(TInt aError);
protected:	
	CTestStep* iStep;
private:
	CTestManager* iTestManager;
	};

/** Base class for test step */
class CTGraphicsStep : public CTTMSGraphicsStep 
	{
public:
	CTGraphicsStep(){}
	virtual ~CTGraphicsStep(){}
public:
	void SetLogHeapInfo(TBool aLogHeapInfo) {iLogHeapInfo = aLogHeapInfo;}
protected:
	virtual CTGraphicsBase* CreateTestL() = 0;
	//allocate all test rersources here
	virtual void TestSetupL(){}
	//delete all allocated resourses here
	virtual void TestClose(){}
//from CTestStep
	IMPORT_C virtual TVerdict doTestStepL();
private:
	void ExecuteL();
	TInt GetScreenSize(TSize& aSize) const;
	void SetupCleanup(CTrapCleanup*& tc);
	void CloseAllPanicWindows(TInt aId, RWsSession& ws) const;
	void LogHeapInfo(RWsSession &aWs, TBool aStart);
private:
	TBool iLogHeapInfo;	// Only perform wserv heap memory checks when this is true (commonly for wserv testing only)
	};

#define TEST2(a, b)  			testBooleanTrue((a == b), (TText8*)__FILE__, __LINE__)
#define TEST_SOFTFAIL_WINSCW(a)	testSoftFailWinscw((a), (TText8*)__FILE__, __LINE__)

inline const TDesC& CTGraphicsBase::ColorModeName(TInt aMode)
	{return ColorModeName(reinterpret_cast<TDisplayMode&>(aMode));}
inline const TDesC& CTGraphicsBase::RotationName(TInt aOrientation)
	{return RotationName(reinterpret_cast<CFbsBitGc::TGraphicsOrientation&>(aOrientation));}

// Macro for defining test step constructor and creating a test.
// Work for the following name convention:
// step class - CTXXStep
// test class - CTXX,
//  where XX any name

#define __CONSTRUCT_STEP__(a) \
	CT##a##Step::CT##a##Step() \
	{ \
	SetTestStepName(KT##a##Step); \
	} \
	 \
CTGraphicsBase* CT##a##Step::CreateTestL() \
	{ \
	return new (ELeave) CT##a (this); \
	}

// Macros to automatically test error condition and report when test fails.
// Essentially same as TESTE(a,b) MACRO that CTestStep uses, except error to
// check is also error that is reported.
#define TESTNOERROR(a) \
	{\
	TInt b = a;\
	TBool c = (b == KErrNone);\
	testBooleanTrueWithErrorCode((c), (b), (TText8*)__FILE__, __LINE__);\
	}
	
inline void CTGraphicsBase::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine) 
	{
	((CTGraphicsStep*)iStep)->MQCTest(aCondition, aFile, aLine);
	}

inline void CTGraphicsBase::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
	{
	((CTGraphicsStep*)iStep)->MQCTestWithErrorCode(aCondition, aErrorCode, aFile, aLine);
	}

inline void CTGraphicsBase::testSoftFailWinscw(TBool aCondition, const TText8* aFile, TInt aLine)
		{
#ifdef __WINS__
		if (!aCondition)
			{
			_LIT(KMessage,"ERROR: Test Failed but is ignored on WINSCW");
			Logger().LogExtra(aFile, aLine, ESevrErr, KMessage);
			}
#else // __WINS__
		testBooleanTrue(aCondition, aFile, aLine);
#endif //__WINS__
		}
#endif
