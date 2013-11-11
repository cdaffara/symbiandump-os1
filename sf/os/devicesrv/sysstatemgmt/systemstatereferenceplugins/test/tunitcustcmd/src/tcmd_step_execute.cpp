// Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Unit test case to test functionalities of CmdCheckUserDrive,
//							CCustomCmdClearStartupReason,CCustomCmdDeleteTempFiles,CCmdInitRamDrive and CCustomCmdValidateRTC custom commands.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tcmd_step_execute.h"
#include <e32property.h>
const TUint32 KRtcAdaptationPluginPropertyKey = 0x2000D76C;
const TUid KPropertyCategory={0x2000D75B};

/**
 This function is wrapped in  Callback and used to stop the CActiveScheduler
 when all other activities have been peformed
 */
static TInt StopScheduler(TAny* aCmdValidateRTCTest)
    {
    CTestCustomCmdExecute*  cmdValidateRTCTest = static_cast<CTestCustomCmdExecute*>(aCmdValidateRTCTest);
    cmdValidateRTCTest->StopActiveScheduler();
    return KErrNone;
    }

void CTestCustomCmdExecute::StopActiveScheduler()
    {
    if(iStatus != KRequestPending)
        {
        CActiveScheduler::Stop();
        }
    else
        {
        iAsyncStopScheduler->CallBack();
        }
    }

CTestCustomCmdExecute::~CTestCustomCmdExecute()
    {
    delete iCmdEnv;
    iFs.Close();
    delete iAsyncStopScheduler;
    delete iActiveScheduler;
    }

CTestCustomCmdExecute::CTestCustomCmdExecute()
    {
    SetTestStepName(KTestCustomCmdExecute);
    }

/**
 Create common resources for this test 
 Install CActiveScheduler necessary for the tests
 */
TVerdict CTestCustomCmdExecute::doTestStepPreambleL( )
    {
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install (iActiveScheduler );

    iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
    TCallBack stop(StopScheduler, this);
    iAsyncStopScheduler->Set (stop );
		
    User::LeaveIfError(iFs.Connect());
 
    iCmdEnv = CSsmCustomCommandEnv::NewL(iFs);
    
    return CTestStep::doTestStepPreambleL ( );
    }

/**
Test CaseID         DEVSRVS-SSREFPLUGINS-CUSTCMD-0028
 */
void CTestCustomCmdExecute::TestCustomCmdValidateRTC()
    {
    INFO_PRINTF1(_L("TestCustomCmdValidateRTC started"));
    // Setting the P and S key will route the request to the reference plugins instead of the actual plugins
    TInt err = RProperty::Define(KPropertyCategory, KRtcAdaptationPluginPropertyKey, RProperty::EInt);
    TEST(KErrNone == err || KErrAlreadyExists == err);
    err = RProperty::Set(KPropertyCategory, KRtcAdaptationPluginPropertyKey, 1);
    TEST(KErrNone == err);
    
    //Create ValidateRTC custom command
    MSsmCustomCommand* customCmdValidateRTC = SsmCustomCmdFactory::CmdValidateRTCNewL();
    customCmdValidateRTC->Initialize(iCmdEnv);
    ExecuteCommand(customCmdValidateRTC);
    // In the reference plugins, the API completes with KErrNotSupported. Hence we check for the same
    TEST(KErrNotSupported == iStatus.Int());
    ExecuteCommand(customCmdValidateRTC);
    customCmdValidateRTC->ExecuteCancel();
    // In the reference plugins, the API completes with KErrNotSupported. Hence we check for the same
    TEST(KErrNotSupported == iStatus.Int());
    //Close the command
    customCmdValidateRTC->Close();
    //Releasing the comand will delete itself.
    customCmdValidateRTC->Release();
    INFO_PRINTF3(_L("TestCustomCmdValidateRTC completed with %d Expected %d"),iStatus.Int(),KErrNone);
    err = RProperty::Delete(KPropertyCategory, KRtcAdaptationPluginPropertyKey);
    TEST(KErrNone == err);
    }

/**
Test CaseID 		DEVSRVS-SSREFPLUGINS-CUSTCMD-0028
 */
void CTestCustomCmdExecute::TestCustomCmdCheckUserDrive()
	{
	INFO_PRINTF1(_L("TestCustomCmdCheckUserDriveL started"));
	//Create ChectUserDrive custom command
	MSsmCustomCommand* customCmdCheckUserDrive = SsmCustomCmdFactory::CmdCheckUserDriveNewL();
	customCmdCheckUserDrive->Initialize(iCmdEnv);
	ExecuteCommand(customCmdCheckUserDrive);
    TEST(KErrNone == iStatus.Int());
    ExecuteCommand(customCmdCheckUserDrive);
    customCmdCheckUserDrive->ExecuteCancel();
    TEST(KErrNone == iStatus.Int());
    //Close the command
    customCmdCheckUserDrive->Close();
    //Delete the custom command
    customCmdCheckUserDrive->Release();
    INFO_PRINTF3(_L("TestCustomCmdCheckUserDriveL completed with %d Expected %d"),iStatus.Int(),KErrNone);
	}

/**
Test CaseID         DEVSRVS-SSREFPLUGINS-CUSTCMD-0028
 */
void CTestCustomCmdExecute::TestCustomCmdClearStartupReason()
    {
    INFO_PRINTF1(_L("TestCustomCmdClearStartupReason started"));
    //Create ClearStartupReason custom command
    MSsmCustomCommand* customCmdClearStartupReason = SsmCustomCmdFactory::CmdClearStartupReasonNewL();
    customCmdClearStartupReason->Initialize(iCmdEnv);
    ExecuteCommand(customCmdClearStartupReason);
    TEST(KErrNone == iStatus.Int());
    ExecuteCommand(customCmdClearStartupReason);
    customCmdClearStartupReason->ExecuteCancel();
    TEST(KErrNone == iStatus.Int());
    //Close the command
    customCmdClearStartupReason->Close();
    customCmdClearStartupReason->Release();
    INFO_PRINTF3(_L("TestCustomCmdClearStartupReason completed with %d Expected %d"),iStatus.Int(),KErrNone);
    }

/**
Test CaseID         DEVSRVS-SSREFPLUGINS-CUSTCMD-0028
 */
void CTestCustomCmdExecute::TestCustomCmdDeleteTempFiles() 
    {
    INFO_PRINTF1(_L("TestCustomCmdDeleteTempFilesL started"));
    //Create DeleteTempFiles custom command
    MSsmCustomCommand* customCmdDeleteTempFiles = SsmCustomCmdFactory::CmdDeleteTempFilesNewL();
    customCmdDeleteTempFiles->Initialize(iCmdEnv);
    ExecuteCommand(customCmdDeleteTempFiles);
    TEST(KErrNone == iStatus.Int());
    ExecuteCommand(customCmdDeleteTempFiles);
    customCmdDeleteTempFiles->ExecuteCancel();
    TEST(KErrNone == iStatus.Int());
    //Close the command
    customCmdDeleteTempFiles->Close();
    //Delete the custom command
    customCmdDeleteTempFiles->Release();
    INFO_PRINTF3(_L("TestCustomCmdDeleteTempFilesL completed with %d Expected %d"),iStatus.Int(),KErrNone);
    }

/**
Test CaseID         DEVSRVS-SSREFPLUGINS-CUSTCMD-0028
 */
void CTestCustomCmdExecute::TestCustomCmdInitRamDrive()
    {
    INFO_PRINTF1(_L("TestCustomCmdInitRamDrive started"));
    //Create InitRamDrive custom command
    MSsmCustomCommand* customCmdInitRamDrive = SsmCustomCmdFactory::CmdInitRamDriveNewL();
    customCmdInitRamDrive->Initialize(iCmdEnv);
    ExecuteCommand(customCmdInitRamDrive);
    TEST(KErrNone == iStatus.Int());
    ExecuteCommand(customCmdInitRamDrive);
    customCmdInitRamDrive->ExecuteCancel();
    TEST(KErrNone == iStatus.Int());
    //Close the command
    customCmdInitRamDrive->Close();
    //Delete the custom command
    customCmdInitRamDrive->Release();
    INFO_PRINTF3(_L("TestCustomCmdInitRamDrive completed with %d Expected %d"),iStatus.Int(),KErrNone);
    }

void CTestCustomCmdExecute::ExecuteCommand(MSsmCustomCommand *aCustCmd)
		{
    TBufC8<1> dummy;
    //Command parameter is not used inside the Execute. So passing any dummy data should be fine
    //Execute the command
    aCustCmd->Execute(dummy, iStatus);
    //this callback will not run until the test is finished because it has priority idle
    iAsyncStopScheduler->CallBack ( );
    //run custom command
    iActiveScheduler->Start( );
	}
	
/**
 Clean up if necessary
 */
TVerdict CTestCustomCmdExecute::doTestStepPostambleL( )
    {
    return CTestStep::doTestStepPostambleL ( );
    }

TVerdict CTestCustomCmdExecute::doTestStepL()
    {
    __UHEAP_MARK;
    TestCustomCmdInitRamDrive();
    TestCustomCmdDeleteTempFiles();
    TestCustomCmdCheckUserDrive();
    TestCustomCmdClearStartupReason();
    TestCustomCmdValidateRTC();
    __UHEAP_MARKEND;
    return TestStepResult();
    }
