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
// Description:
//CCmdRunCleTest to test the commands reconstructed by the CLE 

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/


#ifndef TCMD_RUNCLECMDTEST_H_
#define TCMD_RUNCLECMDTEST_H_

#include <test/testexecutestepbase.h>
#include <ssm/ssmcommand.h>

_LIT(KTCCmdRunCleTest, "CCmdRunCleTest");

class CActiveScheduler;
class CAsyncCallBack;
class CSsmStartupProperties;

class CCmdRunCleTest : public CTestStep
    {
public:
    CCmdRunCleTest();
    ~CCmdRunCleTest();

    //from CTestStep
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
        
private:
    void doTestRunClePublishSwpL();
    void doTestRunCleCustomCommandL();
    void doTestRunCleCmdLoadSupL();
    void doTestRunCleCmdpublishSystemStateL();
    void doTestRunCleCcmdreqSwpchangeL();
    void doTestRunCleCmdAmaStarterL();
    void doTesTRunCleCmdMultipleWaitL();
    void doTestRunCmdPowerOffL();
    void doTestRunCmdCreateSwpL();
    void doTestRunCleCmdPersistHalAttributesL();
    void doTestRunCleCmdFinaliseDrivesL();
    void doTestRunCleCmdSetPAndSKeyL();
    void doTestRunCleCmdWaitForApparcInitL();
    void doTestRunCleCmdStartAppL();
    void doTestRunCleFandFAppWithMonitorL();
    void doTestRunCleCmdStartProcessL();
    void doTestRunCleFandFProcWithMonitorL();
    void doTestRunCleFandFAlreadyRunWithMonitorL();
    //helper function
    TInt RunTestWithCleAndWaitL(MSsmCommand * aCommand, TBool aMultipleWait = 0);
    TInt FindAndKill(const TDesC& aProcessName);
    void StripExtension(TDes& aFilename);
    void SetStartupPropertiesL(const TDesC& aFileName, const TDesC& aArgs, TSsmExecutionBehaviour aExecutionBehaviour = ESsmWaitForSignal, TSsmCommandType aSsmCommandType = ESsmCmdUndefined, TInt aNumOfRetries = 0, TInt32 aTimeout = 0);
    void Reset();
private:
    CActiveScheduler* iActiveScheduler;
    CAsyncCallBack* iAsyncStopScheduler;
    CSsmStartupProperties* iStartupProperties;
    RSemaphore iSem;
    MSsmCommand *iCmd;
    };
#endif /* TCMD_RUNCLECMDTEST_H_ */
