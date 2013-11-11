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
// This class wraps up all the functionality concerned with creating and using an Active 
// Scheduler within the std test environment.  Tests that want to use an Active Scheduler 
// should derive from this class.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TCMD_STEP_BASE_H__
#define __TCMD_STEP_BASE_H__

#include <test/testexecutestepbase.h>
#include <barsread2.h>
#include <ssm/ssmcommand.h>
#include <s32mem.h> 

#include "ssmdebug.h"

const TInt KTempDataLength = 1024;
//this supports default priority for v9.5 (without CR1515 changes)
const TUint16 KDefaultPriority = 10000;

class CActiveScheduler;
class CAsyncCallBack;
class CCmdTestList;
class CResourceFile;

class CCmdTestBase : public CTestStep
	{
public:
	CCmdTestBase(const TDesC& aStepName);
	~CCmdTestBase();

	//from CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL() = 0;
	virtual TVerdict doTestStepPostambleL();
	
	void StartScheduler();
	void StopScheduler();

	// cancelling commands
	void CreateCancelOnCommands(CCmdTestList* aCmdList);

	// stopping the scheduler
	void EnableStopSchedulerCallback();		// Enable it if you want it

	// Resource files
	RResourceReader& LoadResourcesL(const TDesC& aRscFileBuffer, TInt aStructId);
	void FreeResources();
	CResourceFile* ResourceFile();
	
	//Helper functions for start app/procses commands
	TInt FindAndKill(const TDesC& aProcessName);
	void StripExtension(TDes& aFilename);

public:
	CCmdTestList* iTestList;	// cmd list helper class
	
private:

	virtual void doTestNewLWithGoodDataL() = 0;
	virtual void doTestNewLWithBadDataL() = 0;
	virtual void doTestInternalizeLWithGoodDataL() = 0;
	virtual void doTestInternalizeLWithBadDataL() = 0;
	virtual void doTestResourceReaderWithGoodDataL() = 0;
	virtual void doTestResourceReaderWithMissingDataL() = 0;
	virtual void doTestResourceReaderWithBadDataL() = 0;
	virtual void doTestCommandTypeL() = 0;
	virtual void doTestCmdFailureIgnoreSeverityL() = 0;
	virtual void doTestCmdFailureHighSeverityL() = 0;
	virtual void doTestCancelCmdIgnoreSeverityL() = 0;
	virtual void doTestCancelCmdHighSeverityL() = 0;

private:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	CAsyncCallBack* iAsyncCancelCommands;
	
	TBool iActivateCancel;	// activate the cancel scheduler callback or not
	RFs iFs;				// for resource file reading
	CResourceFile* iFile;	// for resource file reading
	RResourceReader iReader;// for resource file reading
	RProcess iSsmProcess;	// State Manager process handle
	RProcess iSupProcess;	// Utility Server process handle
	};

// Global callbacks
extern TInt StopSchedulerCallBack(TAny* aCCmdTestBase);
extern TInt CancelCommandCallBack(TAny* aCmdList);
	
#endif	// __TCMD_STEP_BASE_H__
