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
 @internalComponent - Internal Symbian test code
*/

#include "tgsa_step_fleximergesamesubstate.h"
#include <f32file.h>
#include <s32file.h>
_LIT(KTestProcess, "gsatestprocgood.exe");

/**
Old Test CaseID 		AFSS-GSA-0037
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0037
*/
void CGsaFlexiMergeSameSubstateTest::DoTestFlexiMergeSameSubstateL()
	{
	INFO_PRINTF1(_L("DoTestFlexiMergeSameSubstateL test started...."));
	TInt num = FindAndKill(KTestProcess);
	INFO_PRINTF2(_L("The  number of processes found were: %d"),num);
	TEST (num == 2);
	INFO_PRINTF1(_L("DoTestFlexiMergeSameSubstateL case completed\n"));
	}

//---------------- CGsaFlexiMergeSameSubstateTest step ---------------------------------------


CGsaFlexiMergeSameSubstateTest::~CGsaFlexiMergeSameSubstateTest()
	{
	}

CGsaFlexiMergeSameSubstateTest::CGsaFlexiMergeSameSubstateTest()
	{
	SetTestStepName(KTGsaGsaFlexiMergeSameSubstateStep);
	}

TVerdict CGsaFlexiMergeSameSubstateTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaFlexiMergeSameSubstateTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaFlexiMergeSameSubstateTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaFlexiMergeSameSubstateTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoTestFlexiMergeSameSubstateL());
	INFO_PRINTF2(_L("DoTestFlexiMergeSameSubstateL test completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaFlexiMergeSameSubstateTest completed!!"));
	return TestStepResult();
	}

/**
Helper function to find and kill the specified process
*/
TInt CGsaFlexiMergeSameSubstateTest::FindAndKill(const TDesC& aProcessName)
	{
	TFullName searchTerm(aProcessName);
	StripExtension(searchTerm);
	searchTerm += _L("*");
	TFindProcess find(searchTerm);
	TFullName name;
	TInt instancesFound = 0;
	while(find.Next(name) == KErrNone)
		{
		RProcess process;
		const TInt err = process.Open(find);

		if (KErrNone == err)
			{
			if (process.ExitType() == EExitPending)
				{
				instancesFound++;
				process.Kill(KErrCancel);
				process.Close();
				INFO_PRINTF2(_L("Process %S found and killed"), &aProcessName);
				}
			process.Close();
			}
		}
	return instancesFound;
	}

/**
Helper function to strip extension
*/
void CGsaFlexiMergeSameSubstateTest::StripExtension(TDes& aFilename)
	{
	TInt dot = aFilename.Find(_L("."));

	if(KErrNotFound != dot)
		{
		aFilename.SetLength(dot);
		}
	}

