// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_locsrvSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_locsrvsuitestepbase.h"

// Device driver constants

TInt SetAuthenticRequestor(RPositioner& aPositioner)
{
    // set service
    return aPositioner.SetRequestor(DUMMY_REQUESTOR_TYPE_ACCEPTED1, DUMMY_REQUESTOR_FORMAT_ACCEPTED1, DUMMY_REQUESTOR_NAME_ACCEPTED1);
}

TVerdict CTe_locsrvSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	iScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(iScheduler);
    SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_locsrvSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	return TestStepResult();
	}

CTe_locsrvSuiteStepBase::~CTe_locsrvSuiteStepBase()
	{
	delete iScheduler;
	delete iModulesSettings;
	}

CTe_locsrvSuiteStepBase::CTe_locsrvSuiteStepBase()
	{
	}

void CTe_locsrvSuiteStepBase::StandardPrepareL()
{
    // Setup modules:
    // LcfDummyPsy1,3 and LcfPsy6 should be enabled, other - disabled
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    CPosModuleIdList* idList = modules->ModuleIdListLC();
    for (TInt i = 0; i < idList->Count(); i++)
        {
        TPositionModuleInfo info;
        modules->GetModuleInfoL((*idList)[i], info);
        switch (info.ModuleId().iUid)
            {
            case KLcfPsy1UidValue:
            case KLcfPsy3UidValue:
            case KLcfPsy6UidValue:
                if (!info.IsAvailable())
                    {
                    moduleUpdate->SetUpdateAvailability(ETrue);
                    modules->UpdateModuleL(info.ModuleId(), *moduleUpdate);
                    }
                break;
            default:        
                if (info.IsAvailable())
                    {
                    moduleUpdate->SetUpdateAvailability(EFalse);
                    modules->UpdateModuleL(info.ModuleId(), *moduleUpdate);
                    }
                break;
            }
        }
    CleanupStack::PopAndDestroy(3, modules);
    User::After(KSecondsToMicro);
}

void CTe_locsrvSuiteStepBase::StandardCleanup()
{
}

void CTe_locsrvSuiteStepBase::SetupPsyPriorityL(TUid aPsyUid, TInt aPriority)
{
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    // enable module
    moduleUpdate->SetUpdateAvailability(ETrue);
    modules->UpdateModuleL(aPsyUid, *moduleUpdate);

    // set module priority
    modules->SetModulePriorityL(aPsyUid, aPriority);
    CleanupStack::PopAndDestroy(2, modules);   // moduleUpdate, modules

    User::After(KSecond);
}

void CTe_locsrvSuiteStepBase::DisableAllModulesL()
{
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleIdList* idList = modules->ModuleIdListLC();

    CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
    update->SetUpdateAvailability(EFalse); // disabled

    for (int i = 0; i < idList->Count(); i++)
    {
        modules->UpdateModuleL((*idList)[i], *update);
    }
    CleanupStack::PopAndDestroy(3, modules);

    User::After(KSecond);
}

void CTe_locsrvSuiteStepBase::TogglePsyL(TUid aPsyUid, TBool aEnabled)
{
		INFO_PRINTF1(_L("TogglePsyL() ++"));
    CPosModules* modules = CPosModules::OpenL();
    CleanupStack::PushL(modules);
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    // toggle module
    INFO_PRINTF2(_L("TogglePsyL with psy 0x%x"), aPsyUid);
    moduleUpdate->SetUpdateAvailability(aEnabled);
    modules->UpdateModuleL(aPsyUid, *moduleUpdate);

    CleanupStack::PopAndDestroy(2);   // moduleUpdate, modules

    User::After(KSecond);
    	
    INFO_PRINTF1(_L("TogglePsyL() --"));
}

void CTe_locsrvSuiteStepBase::CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        ERR_PRINTF2(KValueReturned, aResult);
        ERR_PRINTF2(KValueExpected, aExpectedResult);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckExpectedResult(TUid aResult, TUid aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckExpectedResult(TTime aResult, TTime aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckExpectedResult(const TDesC& aResult, const TDesC& aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckExpectedResult(TTimeIntervalMicroSeconds aResult, TTimeIntervalMicroSeconds aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckExpectedRealResult(TReal32 aResult, TReal32 aExpectedResult, const TDesC& aLabel)
    {
    if (Math::IsNaN(aResult) || Math::IsNaN(aExpectedResult))
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        return;
        }
    if (Abs(aResult - aExpectedResult) > KErrorToleranceValue)
        {
        ERR_PRINTF1(KFailedCheck);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::CheckResult(TBool aResult)
    {
    if (!aResult)
        {
        ERR_PRINTF1(KFailedCheck);
        SetTestStepResult(EFail);
        }
    }

void CTe_locsrvSuiteStepBase::PositionRequestWithCheck(TPositionInfo& _aInfo, TInt _aResult, TUid _aModuleId)
    {
    iPositioner.NotifyPositionUpdate(_aInfo, iStatus);
    User::WaitForRequest(iStatus);
    CheckExpectedResult(iStatus.Int(), _aResult, KWrongRequestResult);
    CheckExpectedResult((_aInfo).ModuleId(), _aModuleId, KWrongModuleIdReturned);
    }

void CTe_locsrvSuiteStepBase::PositionRequestWithCheck(TPositionInfo& _aInfo, TRequestStatus& aStatus, TInt _aResult, TUid _aModuleId, RPositioner aPositioner)
    {
    aPositioner.NotifyPositionUpdate(_aInfo, aStatus);
    User::WaitForRequest(aStatus);
    CheckExpectedResult(aStatus.Int(), _aResult, KWrongRequestResult);
    CheckExpectedResult((_aInfo).ModuleId(), _aModuleId, KWrongModuleIdReturned);
    }

//
// PANIC TEST SUPPORT
//

void TurnJITBackOn(TAny* /*aPtr*/)
{
    User::SetJustInTime(ETrue);
}

TInt PanicThreadFunction(TAny* aPtr)
{
    CTrapCleanup* cs = CTrapCleanup::New();
    if (cs == NULL)
        return KErrNoMemory;

    // we may need an ActiveScheduler 
    CActiveScheduler scheduler;
    CActiveScheduler::Install(&scheduler);

    SPanicThreadFunctionData* data = reinterpret_cast<SPanicThreadFunctionData*>(aPtr);
    TInt result = KErrNone;
    TRAPD(leave, result = data->iPanicFunction(data->iPtr));  // should panic

    delete cs;
    if (leave)
        {
        User::Panic(KLeaveInPanicThread, leave);
        }
    else
        {
        User::Exit(result);
        }
    return result;
}

TInt CTe_locsrvSuiteStepBase::DoPanicTestL(TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
                  TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue)

{
#ifdef __WINS__
    User::SetJustInTime(EFalse);
    CleanupStack::PushL(TCleanupItem(TurnJITBackOn));
#endif
    RThread panickingThread;

    SPanicThreadFunctionData threadData;
    threadData.iPanicFunction = aThreadFunction;
    threadData.iPtr = NULL;

    // check that thread does not already exist
    TInt result = KErrNone;
    TBuf<24> threadName(KPanicThreadName);
    
    result = panickingThread.Create(
                threadName,
                PanicThreadFunction,
                KDefaultStackSize,
                KTestHeapMinSize,
                KTestHeapMaxSize,
                &threadData,
                EOwnerProcess);

    CheckExpectedResult(result, KErrNone, KUnexpectedErrorCreatingPanicThread);
    User::LeaveIfError(result);

    CleanupClosePushL(panickingThread);

    RTimer timeoutTimer;
    User::LeaveIfError(timeoutTimer.CreateLocal());
    CleanupClosePushL(timeoutTimer);

    TRequestStatus timerStatus;
    timeoutTimer.After(timerStatus, aTimeoutValue);

    TRequestStatus threadStatus;

    panickingThread.Rendezvous(threadStatus);
    if (threadStatus != KRequestPending)
        {
        // logon failed - thread is not yet running, so cannot have terminated
        User::WaitForRequest(threadStatus);              // eat signal
        panickingThread.Kill(threadStatus.Int());        // abort startup
        }
    else
        {
        panickingThread.Resume();
        User::WaitForRequest(threadStatus, timerStatus);
        }

    TInt exitReason = KErrNone;
    if (threadStatus == KRequestPending)
    {
        ERR_PRINTF1(KThreadDidntDieKillIt);
        panickingThread.Kill(KErrTimedOut);
        User::WaitForRequest(threadStatus);
    }
    else // (timerStatus == KRequestPending)
    {
        // stop timer
        timeoutTimer.Cancel();
        User::WaitForRequest(timerStatus);
    }

    exitReason = panickingThread.ExitReason();
    TExitCategoryName exitCat = panickingThread.ExitCategory();
    
    switch(panickingThread.ExitType()) 
    {
        case EExitKill:
            INFO_PRINTF2(KThreadWasKilled, exitReason);
            break;
        case EExitTerminate:
            INFO_PRINTF2(KThreadWasTerminated, exitReason);
            break;
        case EExitPanic:
            // check exit reasons
            if (exitCat.Compare(aExpectedExitCat) != 0)
                {
                INFO_PRINTF3(KUnexpectedPanicCategory, &exitCat, &aExpectedExitCat);
                }
            CheckExpectedResult(exitReason, aExpectedExitReason, KUnexpectedPanicReason);
            break;
        default:
            CheckExpectedResult((TInt)EExitPanic, threadStatus.Int(), KUnexpectedThreadExitType);
            break;
    }

    CleanupStack::PopAndDestroy(&timeoutTimer);
    CleanupStack::PopAndDestroy(&panickingThread);
#ifdef __WINS__
    CleanupStack::PopAndDestroy();  //TurnJITBackOn
#endif
    // wait a bit to make sure this thread is over
    const TTimeIntervalMicroSeconds32 KThreadDelay = 1000 * 1000; // 1 s
    User::After(KThreadDelay);

    return exitReason;
}
