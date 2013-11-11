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


#ifndef __TCMD_STEP_CCUSTOMCOMMAND_H__
#define __TCMD_STEP_CCUSTOMCOMMAND_H__

#include "tcmd_step_base.h"

_LIT(KTCCmdTestCustomCommand, "CmdCustomCommand");

class CCmdCustomCommand;

class CCmdTestCustomCommand : public CCmdTestBase
	{
public:
	CCmdTestCustomCommand();
	~CCmdTestCustomCommand();

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
	void doTestNormalExecutionL();
	void doTestSupportingClassesL();
	void doTestRetriesL();
	void doTestInitializeResourceHandleLeakL();

	CCmdCustomCommand* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority = KDefaultPriority);
	void CompareCommandsDataL(CCmdCustomCommand* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority = KDefaultPriority);
	void CreateCustCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aInfo, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout = 0);

private:
	CCmdCustomCommand* iTestCmd;
	};

#endif	// __TCMD_STEP_CCUSTOMCOMMAND_H__
