// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:Test case to test the execution behaviour of CCmdCheckUserDrive,CCustomCmdClearStartupReason,
//							CCustomCmdDeleteTempFiles,CCmdInitRamDrive and CCustomCmdValidateRTC custom commands.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TCUSTCMD_STEP_INT_EXECUTE_H__
#define __TCUSTCMD_STEP_INT_EXECUTE_H__

#include "tcustcmd_const.h" 
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmsystemwideproperty.h>
				 
_LIT(KTestExecuteCustCmd, "CTestExecuteCustCmd");

class CTestExecuteCustCmd : public CTestStep
	{
public:
	CTestExecuteCustCmd();
	~CTestExecuteCustCmd();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:	
	void TestHandleValidateRTC();
	void TestHandleCheckUserDrive();
	void TestHandleClearStartupReason();
	void TestCustCmdDeleteTempFiles();
	void TestHandleInitRamDrive();
	void TestOOMCustCmd(TInt aCmdType);
	//Helper functions
	void CreateAndExecuteCustCmd(TInt aSwpVal);

private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	CAsyncCallBack* iAsyncStopScheduler;
	TRequestStatus iSwpStatus;
	};

#endif	// __TCUSTCMD_STEP_INT_EXECUTE_H__
