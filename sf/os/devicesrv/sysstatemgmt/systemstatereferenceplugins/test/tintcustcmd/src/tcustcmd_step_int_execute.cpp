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
// Description:Test case to test the execution behaviour of CCmdCheckUserDrive,CCustomCmdClearStartupReason,CCustomCmdDeleteTempFiles,CCmdInitRamDrive and CCustomCmdValidateRTC custom commands.
//

#include "tcustcmd_step_int_execute.h"


enum TestSwpValForCustCmd
	{
	//create and execute CmdCheckUserDrive custom command
	EValForCmdCheckUserDrive = 66,
	//create and execute CmdInitRamDrive custom command
	EValForCmdInitRamDrive = 77,
	//create and execute CmdDeleteTempFiles custom command
	EValForCmdDeleteTempFiles = 88,
	//create and execute CmdClearStartupReason custom command
	EValForCmdCmdClearStartupReason = 99,
	//create and execute CmdValidateRTC custom command
	EValForCmdCmdValidateRTC = 100
	};
	
CTestExecuteCustCmd ::~CTestExecuteCustCmd ()
	{
	delete iAsyncStopScheduler;
	delete iActiveSchedulerWait;
	delete iActiveScheduler;
	}

CTestExecuteCustCmd ::CTestExecuteCustCmd ()
	{
	SetTestStepName(KTestExecuteCustCmd);
	}

TVerdict CTestExecuteCustCmd ::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;

	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	
	return TestStepResult();
	}
			
TVerdict CTestExecuteCustCmd ::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	__UHEAP_MARK;
	TestHandleCheckUserDrive();
	TestHandleInitRamDrive();
	TestHandleValidateRTC();
	TestHandleClearStartupReason();
	TestCustCmdDeleteTempFiles();
	TestOOMCustCmd(EValForCmdCheckUserDrive);
	TestOOMCustCmd(EValForCmdInitRamDrive);
	TestOOMCustCmd(EValForCmdDeleteTempFiles);
	TestOOMCustCmd(EValForCmdCmdClearStartupReason);
	TestOOMCustCmd(EValForCmdCmdValidateRTC);
	__UHEAP_MARKEND;
	return TestStepResult();
	}
	
/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0027
 */
void CTestExecuteCustCmd ::TestHandleValidateRTC()
	{	
	INFO_PRINTF1(_L("TestHandleValidateRTC"));
	CreateAndExecuteCustCmd(EValForCmdCmdValidateRTC);
	TEST(iSwpStatus.Int() == KErrNone);    
	INFO_PRINTF3(_L("TestHandleValidateRTC completed with %d Expected %d"),iSwpStatus.Int(),KErrNone);
	}

/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0027
 */	
void CTestExecuteCustCmd ::TestHandleCheckUserDrive()
	{	
	INFO_PRINTF1(_L("TestHandleCheckUserDrive"));
	CreateAndExecuteCustCmd(EValForCmdCheckUserDrive);
	TEST(iSwpStatus.Int() == KErrNone);    
	INFO_PRINTF3(_L("TestHandleCheckUserDrive completed with %d Expected %d"),iSwpStatus.Int(),KErrNone);
	}

/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0027
 */	
void CTestExecuteCustCmd ::TestHandleClearStartupReason()
{	
	INFO_PRINTF1(_L("TestHandleClearStartupReason"));
	CreateAndExecuteCustCmd(EValForCmdCmdClearStartupReason);
	TEST(iSwpStatus.Int() == KErrNone);    
	INFO_PRINTF3(_L("TestHandleClearStartupReason completed with %d Expected %d"),iSwpStatus.Int(),KErrNone);
	}
	
/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0027
 */	
void CTestExecuteCustCmd ::TestCustCmdDeleteTempFiles()
	{	
	INFO_PRINTF1(_L("TestCustCmdDeleteTempFiles"));
	CreateAndExecuteCustCmd(EValForCmdDeleteTempFiles);
	TEST(iSwpStatus.Int() == KErrNone);    
	INFO_PRINTF3(_L("TestCustCmdDeleteTempFiles completed with %d Expected %d"),iSwpStatus.Int(),KErrNone);
	}

/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0027
 */	
void CTestExecuteCustCmd ::TestHandleInitRamDrive()
	{	
	INFO_PRINTF1(_L("TestHandleInitRamDrive"));
	CreateAndExecuteCustCmd(EValForCmdInitRamDrive);
  TEST(iSwpStatus.Int() == KErrNone);    
	INFO_PRINTF3(_L("TestHandleInitRamDrive completed with %d Expected %d"),iSwpStatus.Int(),KErrNone);
	}

void CTestExecuteCustCmd::CreateAndExecuteCustCmd(TInt aSwpVal)
	{
	INFO_PRINTF1(_L("CreateAndExecuteCustCmd"));
	
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create SwP
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp(KTestSwp, aSwpVal);
	RProperty property;
	TEST(KErrNone == property.Define(KCustomcmdServerSID, swp.Key(), RProperty::EInt));
	TEST(KErrNone == property.Set(KCustomcmdServerSID, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	TESTE(aSwpVal == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	client.RegisterSwpMapping(KTestSwp, KTestSwpPolicyCustomCmd);
	
	//Let client request swp changes
	INFO_PRINTF1(_L("Request swp change"));
	swp.Set(KTestSwp, -99);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);				
	TEST(status == KRequestPending);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);	
    
	RSsmSystemWideProperty swpClient;
	//Connect to the swp.
	TInt err = swpClient.Connect(swp.Key());
	TEST(KErrNone == err);
	swpClient.Subscribe(iSwpStatus);
	User::WaitForRequest(iSwpStatus);
	TEST(KErrNone == property.Get(KCustomcmdServerSID, swp.Key(), value));
	// clean-up
	swpClient.Close();
	TEST(KErrNone == property.Delete(KCustomcmdServerSID, swp.Key()));
	TESTE(99 != value, value);
	property.Close();
	client.Close();
	INFO_PRINTF1(_L("CreateAndExecuteCustCmd completed"));
	}	
		
TVerdict CTestExecuteCustCmd ::doTestStepPostambleL()
	{	
	return TestStepResult();
	}

/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0029
 */
void CTestExecuteCustCmd::TestOOMCustCmd(TInt aCmdType)
	{
    switch(aCmdType)
        {
         case EValForCmdCheckUserDrive:
             INFO_PRINTF1(_L("*** Starting Test CmdCheckUserDrive custom command in OOM"));
         break;
         case EValForCmdInitRamDrive:
             INFO_PRINTF1(_L("*** Starting Test InitRamDrive custom command in OOM"));
         break;
         case EValForCmdDeleteTempFiles:
             INFO_PRINTF1(_L("*** Starting Test DeleteTempFiles custom command in OOM"));
         break;
         case EValForCmdCmdClearStartupReason:
             INFO_PRINTF1(_L("*** Starting Test ClearStartupReason custom command in OOM"));
         break;
         case EValForCmdCmdValidateRTC:
             INFO_PRINTF1(_L("*** Starting Test ValidateRTC custom command in OOM"));
         break;
         default:
         break;
         }       
	TInt failRate = 1;
	for (;;failRate++)
	  {
	   __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	   __UHEAP_MARK;
       TRAPD(ret, CreateAndExecuteCustCmd(aCmdType));
               
       TEST((ret==KErrNone || ret==KErrNoMemory));
       __UHEAP_MARKEND;
       if (ret==KErrNone)
           {
           break;
           }
        }
    __UHEAP_RESET;
	INFO_PRINTF2(_L(" Executed %d number of times."),failRate);    
	}			
