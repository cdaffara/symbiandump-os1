//tcmd_step_ssmcommand.h

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


#ifndef __TCMD_STEP_SSMCOMMAND_H__
#define __TCMD_STEP_SSMCOMMAND_H__

#include <ssm/ssmstartupproperties.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmswp.h>
#include "e32property.h" 
#include "tcmd_swppolicy_builtincmd.hrh"

_LIT(KTCCmdTestSsmCommand, "CCmdTestSsmCommand");
const TUint KSwpTestKey1=0xE5325221;
const TUid KMySID={0x2000D75B}; // ssmserver SID 
_LIT(KTestSwpPolicyCmd, "tcmd_swppolicy_builtincmd.dll");
	
class CCmdTestSsmCommand: public CTestStep
	{
public:
	CCmdTestSsmCommand();
	~CCmdTestSsmCommand();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
private:
	void TestWaitForApparcInit();
	void TestMultipleWaitCommand();	
	void TestMultipleWaitCommandCancel();
	void TestStartApp(TSwpBuiltInCmdValues aCmdValue);
	void TestStartAppCancel(TSwpBuiltInCmdValues aCmdValue);
	void TestStartProc(TSwpBuiltInCmdValues aCmdValue);
	void TestStartProcCancel(TSwpBuiltInCmdValues aCmdValue);
	void TestFinaliseDrive(TSwpBuiltInCmdValues aCmdValue);
	void TestFinaliseDriveCancel(TSwpBuiltInCmdValues aCmdValue);
	void TestPersisthalAttributesL(TSwpBuiltInCmdValues aCmdValue);
	void TestPersisthalAttributesCancelL(TSwpBuiltInCmdValues aCmdValue);
	void TestPowerOff(TSwpBuiltInCmdValues aCmdValue);
	void TestPowerOffCancel(TSwpBuiltInCmdValues aCmdValue);
	void TestAMAStarterCommandL(TSwpBuiltInCmdValues aCmdValue);
	void TestAMAStarterCommandCancelL(TSwpBuiltInCmdValues aCmdValue);
private:	
	void CreateAndExecuteCmd(TSwpBuiltInCmdValues aCmdValue);
	void CreateAndExecuteCancelCmd(TSwpBuiltInCmdValues aCmdValue);
	void CreateAndExecuteCmd1(TSwpBuiltInCmdValues aCmdValue);
	void CreateAndExecuteCancelCmd1(TSwpBuiltInCmdValues aCmdValue);
public:
	//Helper functions to find app/procses
	TInt FindAndKill(const TDesC& aProcessName);	
	void StripExtension( TDes& aFilename );
private:
	CActiveScheduler* iActiveScheduler;
	CActiveSchedulerWait* iActiveSchedulerWait;
	TRequestStatus iStatus;
	TRequestStatus iCancelReqStatus;
	RSsmStateManager iClient;
	};

#endif	// __TCMD_STEP_SSMCOMMAND_H__
