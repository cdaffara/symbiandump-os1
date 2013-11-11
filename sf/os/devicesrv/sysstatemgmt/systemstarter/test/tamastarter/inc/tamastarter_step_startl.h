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

#if (!defined __TAMASTARTER_STEP_STARTL_H__)
#define __TAMASTARTER_STEP_STARTL_H__

#include <startupproperties.h>
#include <startup.hrh>
#include <dscstoredefs.h>
#include <dscstore.h>
#include <dscitem.h>
#include <sysmonclisess.h>

#include <test/testexecuteserverbase.h>
#include <cinidata.h>

/**
CAppfwkAmaStarterTestStepStartL encapsulates the functionality required 
when test AMAs are launched and compare it against the expected result file.
*/
class CAppfwkAmaStarterTestStepStart : public CTestStep
	{
public:	
	CAppfwkAmaStarterTestStepStart(const TDesC& aStepName);
	virtual ~CAppfwkAmaStarterTestStepStart();
	
	//from CTestStep	
	TVerdict doTestStepL();	

private:
	void TestCase1L();
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void TestCase2L();
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT
	void TestCase3L();

private:	
	void CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored);

	};

_LIT(KAppfwkAmaStarterStartL,"AmaStarterStartL");

#endif


