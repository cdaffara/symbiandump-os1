// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
@file
@test
@internalComponent
*/

#ifndef TTMSGRAPHICSSTEP_
#define TTMSGRAPHICSSTEP_

#include <test/testexecutestepbase.h>
#include <w32std.h>
#include <flogger.h>
#include <badesca.h> 

// default name that a test will be called if not specifically set in the calling object
_LIT(KUnknownSYMTestCaseIDName, "@SYMTestCaseID Unnamed test");
_LIT(KUndefinedSYMTestCaseIDName, "@SYMTestCaseID is not defined");
_LIT(KNotATestSYMTestCaseIDName, "This is not a test");

class CTMSDataColumn
{
public:
TBuf8<256> iTheID;
TBuf8<50> iTheResult;
};

class CTTMSGraphicsStep : public CTestStep
	{
public:
	IMPORT_C CTTMSGraphicsStep();
	IMPORT_C virtual ~CTTMSGraphicsStep();
	IMPORT_C void CloseTMSGraphicsStep();
	IMPORT_C void MQCTest(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void MQCTestL(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void SetTestStepID(const TDesC& aStepName);
	IMPORT_C void SetOverallTestStepID(const TDesC& aStepName);
	IMPORT_C void RecordTestResultL();
	IMPORT_C void MultipleResultsForSameID(TBool aShowMultipleResults);
	IMPORT_C void MQCTestWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine);
	
	void DebugLogL(const TDesC8 &aText, const TDesC8 &aText1);
protected:
	void CleanUpAndWriteResults();
	TBuf8<KMaxTestExecuteNameLength> iCurrentTestStepName;
	TBool iStepPassFail;
	TBool iShowMultipleResults;
	RArray<CTMSDataColumn> iArrayTMSData;
	};

#endif /*TTMSGRAPHICSSTEP_*/
