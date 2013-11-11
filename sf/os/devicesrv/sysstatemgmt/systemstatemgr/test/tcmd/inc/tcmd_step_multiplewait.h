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
#ifndef __TCMD_STEP_MULTIPLEWAIT_H__
#define __TCMD_STEP_MULTIPLEWAIT_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"
#include "tcmd_mock_commandfactory.h"

_LIT(KTCCmdTestMultipleWait, "CCmdTestMultipleWait");
class CCmdMultipleWait;

class CCmdTestMultipleWait : public CCmdTestBase, public MSsmCommandFactory
	{
public:
	CCmdTestMultipleWait();
	~CCmdTestMultipleWait();

	//from CTestStep
	TVerdict doTestStepL();

private:
	//from CCmdTestBase
	void doTestNewLWithGoodDataL();
	void doTestNewLWithBadDataL();
	void doTestInternalizeLWithGoodDataL();
	void doTestInternalizeLWithBadDataL();
	void doTestResourceReaderWithGoodDataL();
	void doTestResourceReaderWithMissingDataL();
	void doTestResourceReaderWithBadDataL();
	void doTestCommandTypeL();
	void doTestCmdFailureIgnoreSeverityL();
	void doTestCmdFailureHighSeverityL();
	void doTestCancelCmdIgnoreSeverityL();
	void doTestCancelCmdHighSeverityL();

	CCmdMultipleWait* CreateCmdFromStreamL(TArray<MSsmCommand*> aDeferredList, TInt32 aTimeout);
private:
	struct TDeferredCmdConfiguration
		{
		TDeferredCmdConfiguration(TSsmCommandType aCmdType, TInt aCmdResourceId)
			: iCmdType(aCmdType), iCmdResourceId(aCmdResourceId)
			{}
		TSsmCommandType iCmdType;
		TInt			iCmdResourceId;
		};

	void CompareCommandsDataL(CCmdMultipleWait* aTestCmd, TInt32 aTimeout);
	void CreateDummyDeferredListL();
	void Reset();
	void CreateCmdAndExecuteL(TArray<TDeferredCmdConfiguration> aDeferredCmds, TInt aExpectedErr, TBool aCancel, TInt32 aMultipleWaitTimeout);
	void AddDeferredCommandsInListL(TDeferredCmdConfiguration aDeferredCmd);
	CSsmCommandBase* ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters);

private:
	RArray<TDeferredCmdConfiguration> iDeferredCmds;
	RPointerArray<MSsmCommand> iDeferredList;
	CCmdMultipleWait* iTestCmd;
	};

#endif	// __TCMD_STEP_MULTIPLEWAIT_H__
