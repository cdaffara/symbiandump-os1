/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* This contains CTestStep which is the base class for all test steps.  
* 
*
*/



/**
 @file testexecutestepbase.h
*/
#ifndef TESTEXECUTESTEPBASE_H
#define TESTEXECUTESTEPBASE_H

#include <e32base.h>
#include <f32file.h>
#include <cinidata.h>
#include "tefexportconst.h"
#include "testlog.h"


class CTestStep : public CBase
/**
@publishedPartner
@test
*/
	{
public:
	// Destructor
	IMPORT_C virtual ~CTestStep();

	// Set a user-friendly test step name referred for a test step object from the script file
	IMPORT_C void SetTestStepName(const TDesC& aStepName);

	// Set the actual test step error obtained to execution of test step to compare the expected error with the actual
	IMPORT_C void SetTestStepError(const TInt& aErrorCode);

	// Initialise the test step during cnstruction
	void InitialiseL(const TDesC& aStepArgs, const TDesC& aServerName, TBool& aSharedDataMode);

	// Set the result of test step execution  to one of the TVerdict enum codes
	inline void SetTestStepResult(TVerdict aVerdict) {iTestStepResult = aVerdict;};

	// Returns the test step result at any point during execution of test step
	inline TVerdict TestStepResult() const {return iTestStepResult;};

	// Returns the user-friendly test step name
	inline const TDesC& TestStepName() const {return iTestStepName;};

	// Returns any error code set during the test step execution
	inline const TInt& TestStepError() const {return iTestStepError;};

	// Test Step virtuals - Test Implementation methods
	IMPORT_C virtual enum TVerdict	doTestStepL() = 0; 
	IMPORT_C virtual enum TVerdict	doTestStepPreambleL();
	IMPORT_C virtual enum TVerdict	doTestStepPostambleL();

	// LEGACY methods
	IMPORT_C void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine );
	IMPORT_C void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode,
												const TText8* aFile, TInt aLine );
	IMPORT_C void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode,
													const TText8* aFile, TInt aLine, TBool IgnoreStepResult );
    IMPORT_C void TestCheckPointCompareL(TInt aVal, TInt aExpectedVal, const TDesC& aText, const TText8* aFile, TInt aLine);

	// Retrieve the test case name/id
	IMPORT_C TBool TestCaseName(TDes& aTestCaseName);

	inline void SetLogger(CTestLog* logger) {iLogger = logger;};
	inline CTestLog* Logger() {return iLogger;};

protected:
	IMPORT_C CTestStep();
private:
	CTestLog*                      iLogger;

	TVerdict							iTestStepResult;
	TBuf<KMaxTestExecuteNameLength>	    iTestStepName;		// Name of the test step
	TBuf<KMaxTestExecuteNameLength>		iServerName;		// Name of the test server
	TInt								iTestStepError;		// Expected Test step Error
	};

// LEGACY
// Autotest macros mapped to CTestStep functions

// Check a boolean is true
#define TESTL(a) testBooleanTrueL((a), (TText8*)__FILE__, __LINE__) 
#define TEST(a)  testBooleanTrue((a), (TText8*)__FILE__, __LINE__) 

#define TEST_ERROR_CODE 84	

#endif
