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


#ifndef __TCMD_STEP_FINALISEDRIVES_H__
#define __TCMD_STEP_FINALISEDRIVES_H__

#include "tcmd_step_base.h"

class CCmdFinaliseDrives;

_LIT(KTCCmdTestFinaliseDrives, "CCmdTestFinaliseDrives");

class CCmdTestFinaliseDrives : public CCmdTestBase
	{
public:
	CCmdTestFinaliseDrives();
	~CCmdTestFinaliseDrives();

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
	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TInt aExpectedErr, TBool aCancel);
	void CompareCommandsDataL(CCmdFinaliseDrives* aTestCmd, TCmdErrorSeverity aSeverity);
	CCmdFinaliseDrives* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity);

private:

	CCmdFinaliseDrives* iTestCmd;
	};

#endif	// __TCMD_STEP_FINALISEDRIVES_H__
