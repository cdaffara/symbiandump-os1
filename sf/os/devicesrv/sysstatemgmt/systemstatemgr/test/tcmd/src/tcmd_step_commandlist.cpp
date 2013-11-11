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


#include "tcmd_step_commandlist.h"
#include "ssmcommandutilprovider.h"

#include <ssm/ssmcommandlist.h>
#include <s32mem.h>

CCommandListTest::~CCommandListTest()
	{
	}

CCommandListTest::CCommandListTest()
	{
	SetTestStepName(KTCommandList);
	}

/** */
TVerdict CCommandListTest::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("*** >CCommandListTest::doTestStepPreambleL"));
	INFO_PRINTF1(_L("*** CCommandListTest::doTestStepPreambleL>"));
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CCommandListTest::doTestStepL()
	{
	INFO_PRINTF1(_L("*** >CCommandListTest::doTestStepL"));

	TInt err = KErrNone;

	__UHEAP_MARK;
	TRAP(err, Test1L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, Test2L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, Test3L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, Test4L());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("*** CCommandListTest::doTestStepL>"));
	return TestStepResult();
	}

/** */
TVerdict CCommandListTest::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("*** >CCommandListTest::doTestStepPostambleL"));
	INFO_PRINTF1(_L("*** CCommandListTest::doTestStepPostambleL>"));
	return CTestStep::doTestStepPostambleL();
	}

// simple construct and delete shouldn't leak memory
void CCommandListTest::Test1L()
	{
	INFO_PRINTF1(_L("*** Starting Test1L"));
	CSsmCommandList* commandList = CSsmCommandList::NewL();
	delete commandList;
	}

// check set/get DelayBetweenCommands works ok
void CCommandListTest::Test2L()
	{
	INFO_PRINTF1(_L("*** Starting Test2L"));
	CSsmCommandList* commandList = CSsmCommandList::NewL();
	const TInt expectedDelay = 1234; // delay in milliseconds
	commandList->SetDelayBetweenCommands(expectedDelay);
	const TInt actualDelay = commandList->DelayBetweenCommands();
	TEST(actualDelay == expectedDelay);
	delete commandList;
	}

// default for DelayBetweenCommands should be zero
void CCommandListTest::Test3L()
	{
	INFO_PRINTF1(_L("*** Starting Test3L"));
	CSsmCommandList* commandList = CSsmCommandList::NewL();
	const TInt expectedDelay = 0; // delay in milliseconds
	const TInt actualDelay = commandList->DelayBetweenCommands();
	TEST(actualDelay == expectedDelay);
	delete commandList;
	}

// delay value should survive an externalise/internalise sequence
void CCommandListTest::Test4L()
	{
	INFO_PRINTF1(_L("*** Starting Test4L"));
	CSsmCommandUtilProvider* commandUtilProvider = CSsmCommandUtilProvider::NewL();
	CleanupStack::PushL(commandUtilProvider);

	CSsmCommandList* commandList = CSsmCommandList::NewLC();
	const TInt expectedDelay = 1234; // delay in milliseconds
	commandList->SetDelayBetweenCommands(expectedDelay);

	// Externalize to buffer
	CBufFlat* buf = CBufFlat::NewL(256);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);
	commandList->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);

	// Internalize from buffer
	CSsmCommandList* commandList2 = CSsmCommandList::NewLC();
	RBufReadStream readStream(*buf);
	CleanupClosePushL(readStream);
	commandList2->InternalizeL(readStream, *commandUtilProvider);
	CleanupStack::PopAndDestroy(&readStream);

	const TInt actualDelay = commandList2->DelayBetweenCommands();
	TEST(actualDelay == expectedDelay);

	CleanupStack::PopAndDestroy(commandList2);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(commandList);
	CleanupStack::PopAndDestroy(commandUtilProvider);
	}

