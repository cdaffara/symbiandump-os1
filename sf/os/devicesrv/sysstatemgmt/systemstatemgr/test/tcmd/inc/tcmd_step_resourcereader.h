// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TCMD_STEP_RESOURCEREADER_H__
#define __TCMD_STEP_RESOURCEREADER_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmconditionalcallback.h>
#include <ssm/ssmcmd.hrh>

#include "ssmcommandlistinterface.h"
#include "tcmd_mock_commandfactory.h"

class RResourceReader;
class CSsmCommandBase;
class TSsmCommandParameters;
class CActiveSchedulerWithRunLCount;

_LIT(KTResourceReader,"ResourceReader");

class CResourceReaderTest : public CTestStep, public MSsmConditionalCallback, public MSsmCommandList, public MSsmCommandFactory
	{
public:
	CResourceReaderTest();
	~CResourceReaderTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	// from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);
	
	// from MSsmCommandList
	void AppendL(MSsmCommand* aCmd);
	TArray<MSsmCommand*> PendingDeferred() const;
	void SetDelayBetweenCommands(TInt aDelayBetweenCommands);
	void SetResourceFileNameL(const TDesC& aFileName);
	void SetConditionalCallback(MSsmConditionalCallback& aCallBack);

	class TThreadParams
		{
	public:
		enum TFunction
			{
			EThreadInitialise3,
			EThreadInitialise6,
			EThreadInitialise7,
			EThreadInitialise8,
			EThreadInitialise9,
			EThreadGet1,
			EThreadGet2,
			EThreadGet3,
			EThreadPrepare16,
			EThreadGetCommandListIds1
			};
		TThreadParams(CResourceReaderTest& aTestStep, TFunction aFunction) : iTestStep(aTestStep), iFunction(aFunction) {};
		CResourceReaderTest& iTestStep;
		TFunction iFunction;
		};

	//from MSsmCommandFactory
	CSsmCommandBase* ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters);
		
private:
	void TestConstructCases();
	void TestInitialiseCases();
	void TestPrepareCases();
	void TestCancelCases();
	void TestDelayCases();
	void TestGetCases();

private:
	enum TWhichPrepare
		{
		EPrepareNoCheck,
		EPrepareState,
		EPrepareSwp,
		EPrepareLeave,
		EConditional,
		EPrepareType
		};
	void TestConstruct1L();
	void TestConstruct2L();
	void TestOOM1L();
	void DoOOM1L();
	void TestOOM2L();
	void DoOOM2L();

	void TestInitialise1L();
	void TestInitialise2L();
	void TestInitialise3L();
	void ThreadInitialise3L();
	void TestInitialise4L();
	void TestInitialise5L();
	void TestInitialise6L();
	void ThreadInitialise6L();
	void TestInitialise7L();
	void ThreadInitialise7L();
	void TestInitialise8L();
	void ThreadInitialise8L();
	void TestInitialise9L();
	void ThreadInitialise9L();
	void TestInitialise10L();
	void TestInitialise11L();
	void TestInitialise12L();
	void TestInitialise13L();
	void TestInitialise14L();
	void TestInitialise15L();
	void TestNoMappings1L();
	void TestNoMappings2L();
	
	void TestPrepare1L();
	void TestPrepare2L();
	void TestPrepare3L();
	void TestPrepare4L();
	void TestPrepare5L();
	void TestPrepare6L();
	void TestPrepare7L();
	void TestPrepare8L();
	void TestPrepare9L();
	void TestPrepare10L();
	void TestPrepare11L();
	void TestPrepare12L();
	void TestPrepare13L();
	void TestPrepare14L();
	void TestPrepare15L();
	void TestPrepare16L();
	void ThreadPrepare16L();
	void TestPrepare17L();

	void TestCancel1L();
	void TestCancel2L();

	void TestGet1L();
	void ThreadGet1L();
	void TestGet2L();
	void ThreadGet2L();
	void TestGet3L();
	void ThreadGet3L();
	void TestGet4L();
	void TestGet5L();
	void ConfTestL();
	void TestGeneral1L();
	void TestCommandType1L();
	void TestGetCommandListIds1L();
	void ThreadGetCommandListIds1L();
	void TestGetCommandListIds2L();
	void TestGetCommandListIds3L();

	void TestDelay1L();
	void TestDelay2L();

private:
	void Run();
	void Run(TInt aRunLCount);

	TInt StartTestInThreadL(TThreadParams::TFunction aFunction);
	static TInt ThreadStartFn(TAny* aPtr);
	void ThreadDoTestL(TThreadParams::TFunction aFunction);
	void ThreadDispatchFunctionL(TThreadParams::TFunction aFunction);

private:
	RFs iFs;
	TThreadId iMainThreadId;
	CActiveSchedulerWithRunLCount* iActiveScheduler;
	CActiveSchedulerWithRunLCount* iThreadActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	CAsyncCallBack* iThreadAsyncStopScheduler;
	TWhichPrepare iWhichPrepare;
	TInt iCommandConstructCount;
	TInt iDelay;
	};

#endif
