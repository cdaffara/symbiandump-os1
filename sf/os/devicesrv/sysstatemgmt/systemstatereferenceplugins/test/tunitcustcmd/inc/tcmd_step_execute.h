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
// Description: Unit test case to test functionalities of CCmdCheckUserDrive,CCustomCmdClearStartupReason,CCustomCmdDeleteTempFiles,CCmdInitRamDrive and CCustomCmdValidateRTC custom commands.
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TCMD_STEP_EXECUTE_H__
#define __TCMD_STEP_EXECUTE_H__

#include <f32file.h>
#include <test/testexecutestepbase.h>
#include "ssmcustomcmdfactory.h"
_LIT(KTestCustomCmdExecute, "CTestCustomCmdExecute");

class CTestCustomCmdExecute: public CTestStep
    {
public:
    CTestCustomCmdExecute();
    ~CTestCustomCmdExecute();

    //from CTestStep
    TVerdict doTestStepPreambleL();
    TVerdict doTestStepL();
    TVerdict doTestStepPostambleL();
    
    void StopActiveScheduler();
private:
    void TestCustomCmdValidateRTC();
    void TestCustomCmdCheckUserDrive();
    void TestCustomCmdClearStartupReason();
    void TestCustomCmdInitRamDrive();
    void TestCustomCmdDeleteTempFiles();
   
    //Helper functions
    void ExecuteCommand(MSsmCustomCommand *aCustCmd);
private:
    CActiveScheduler* iActiveScheduler;
    CAsyncCallBack* iAsyncStopScheduler; 
    TRequestStatus iStatus;
    RFs iFs;
    CSsmCustomCommandEnv* iCmdEnv;
    };

#endif  //  __TCMD_STEP_EXECUTE_H__
