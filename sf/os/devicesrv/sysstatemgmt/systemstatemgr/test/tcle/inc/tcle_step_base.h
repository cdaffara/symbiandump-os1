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
 

#ifndef __TCLE_STEP_BASE_H__
#define __TCLE_STEP_BASE_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcommand.h>

#include "clesrv.h"
#include "clecli.h"

#include "tcle_cleclient.h"
#include "ssmcletestapp.h"

enum TCleTestCommandType
	{
	EProcess,
	EApp,
	ECustom
	};
	
enum TDwfsListType
	{
	ENormal = 0,
	EIgnoreAll = 1,
	EDontRendezvous = 2
	};
	
struct TCleTestResult
	{
	TTime iStartTime;
	TInt iStartCount;
	TInt iStartHour;
	TInt iStartSecond;
	TInt iStartMinute;
	TInt iResult;
	};

class CCleTestStepBase : public CTestStep
	{
public:
	CCleTestStepBase();
	~CCleTestStepBase();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

protected:
	CSsmCommandList* CreateGoodCommandListL(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, TInt aCount);
	CSsmCommandList* CreateBadCommandListL(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, TInt aCount, TDwfsListType aDwfsListType = ENormal);
	MSsmCommand* CreateGoodCommandLC(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, const TDesC& aLogPrefix, TInt aDelay, TInt aTimeout, TInt aFailures, TInt aRetries);
	MSsmCommand* CreateBadCommandLC(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, const TDesC& aLogPrefix, TInt aDelay, TInt aTimeout, TInt aFailures, TInt aRetries, TCleTestFailHow aFailHow, TCmdErrorSeverity aSeverity = ECmdIgnoreFailure);
	void ExecuteCommandListL(CSsmCommandList* aCmdList, TInt aExpectedResult = KErrNone);
	TTime GetTimeFromFileL(RFs aFs, const TDesC& aFileName);
	TInt GetRunCountFromFileL(RFs aFs, const TDesC& aFileName);
	void GetTestResultsL(RFs aFs, TInt aCount, TSsmExecutionBehaviour aExecutionBehaviour = ESsmFireAndForget);
	void DeleteLogFiles(RFs aFs, TInt aCount);
	TInt FindAndKill(const TDesC& aProcessName );
	void StripExtension( TDes& aFilename );
protected:
	RCleSessionTest iCleCli;
	RArray<TCleTestResult> iTestResults;
	};

_LIT(KFinal, "final");

#endif //__TCLE_STEP_BASE_H__
