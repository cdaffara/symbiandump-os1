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


#ifndef __TCMD_STEP_POWEROFF_H__
#define __TCMD_STEP_POWEROFF_H__

#include "tcmd_step_base.h"

_LIT(KTCCmdTestPowerOff, "CCmdTestPowerOff");
class CCmdPowerOff;

class CCmdTestPowerOff : public CCmdTestBase
	{
public:
	CCmdTestPowerOff();
	~CCmdTestPowerOff();

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

private:
	CCmdPowerOff* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent);
	void CompareCommandsDataL(CCmdPowerOff* aTestCmd, TCmdErrorSeverity aSeverity, TPowerState aPowerEvent);
	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent, TInt aExpectedErr, TBool aCancel);

private:
	CCmdPowerOff* iTestCmd;
	};

#endif	// __TCMD_STEP_POWEROFF_H__
