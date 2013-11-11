// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __TAMASTARTER_STEP_STANDALONE_H__)
#define __TAMASTARTER_STEP_STANDALONE_H__

#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>

#include <test/testexecutestepbase.h>
#include <cinidata.h>

/**
CAppfwkAmaStarterTestStepStandalone encapsulates the functionality required 
when test AMAs are launched and compare it against the expected result file.
*/
class CAppfwkAmaStarterTestStepStandalone : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepStandalone(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepStandalone();
	
	//from CTestStep	
	TVerdict doTestStepL();	

private:
	void RunStandaloneTests();
	void TestCase1L();
	void TestCase2L();
	void TestCase3L();
	void TestCase4L();
	void TestCase5L();
	void TestCase6L();

private:	
	void LaunchExe(const TDesC& aFilename, const TDesC& aArguments, TInt aExitReason);
	void CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored);

	};

_LIT(KAppfwkAmaStarterStandalone,"AmaStarterStandalone");

#endif


