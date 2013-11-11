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

#if (!defined __TAMASTARTER_STEP_SSC_H__)
#define __TAMASTARTER_STEP_SSC_H__

#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>

#include <test/testexecutestepbase.h>
#include <cinidata.h>

/**
CAppfwkAmaStarterTestStepSsc encapsulates the functionality required 
when test AMAs are launched and compare it against the expected result file.
*/
class CAppfwkAmaStarterTestStepSsc : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepSsc(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepSsc();
	
	//from CTestStep	
	TVerdict doTestStepL();	

private:
	void TestCase1L();
	void TestCase2L();
	void TestCase3L();
	void TestCase4L();
	void TestCase5L();
	void TestCase6L();

private:
	void CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored);
	};

_LIT(KAppfwkAmaStarterSsc,"AmaStarterSsc");

#endif


