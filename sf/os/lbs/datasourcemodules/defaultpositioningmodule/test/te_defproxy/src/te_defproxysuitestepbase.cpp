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
 @file Te_defproxySuiteStepBase.cpp
 @internalTechnology
*/

#include "te_defproxysuitestepbase.h"

// Device driver constants

// Set PSY6 Delay
void CTe_defproxySuiteStepBase::SetPsy6Delay(HPositionGenericInfo& aGenInfo, TTimeIntervalMicroSeconds aDelay)
    {
    aGenInfo.SetRequestedField(KPSY6FieldDelayProcessing);
    aGenInfo.SetValue(KPSY6FieldDelayProcessing, aDelay);
    }

// Set PSY6 Result
void CTe_defproxySuiteStepBase::SetPsy6Result(HPositionGenericInfo& aGenInfo, TInt32 aResult)
    {
    aGenInfo.SetRequestedField(KPSY6FieldRequestedResult);
    aGenInfo.SetValue(KPSY6FieldRequestedResult, aResult);
    }

// Get PSY6 Event Log
TQueryEvent* CTe_defproxySuiteStepBase::GetPsy6EventLogL(HPositionGenericInfo& aGenInfo)
    {
    TPtrC8 ptr;
    User::LeaveIfError(aGenInfo.GetValue(KPSY6FieldEventLog, ptr));
    return reinterpret_cast<TQueryEvent*>(const_cast<TUint8*>(ptr.Ptr()));
    }



TVerdict CTe_defproxySuiteStepBase::doTestStepPreambleL()
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
    iBackupList = CPosModuleIdList::NewL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_defproxySuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	RestoreModulesL();
	return TestStepResult();
	}

CTe_defproxySuiteStepBase::~CTe_defproxySuiteStepBase()
	{
	delete iScheduler;
	delete iBackupList;
    delete iModulesSettings;
	}

CTe_defproxySuiteStepBase::CTe_defproxySuiteStepBase()
	{
    KUidLcfPsy1.iUid = KLcfPsy1UidValue;
    KUidLcfPsy3.iUid = KLcfPsy3UidValue;
    KUidLcfPsy6.iUid = KLcfPsy6UidValue;

    KLeavingPsy.iUid = KLeavingPsyUidValue;
    KInstaPsy.iUid = KInstaPsyUidValue;
	}

void CTe_defproxySuiteStepBase::InitPsySettingsL(TInt aCount, ...)
    {
    //disable all
    DisableAllModulesL();

    //Enable module according to the input parameters
    TUint32 psyUid;
    TInt availability;
    VA_LIST list;
    VA_START(list, aCount);
    for(TInt i=0; i<aCount; i++)
        {
        psyUid = VA_ARG(list, TUint32);
        availability = VA_ARG(list, TInt);
        SetupModuleL(TUid::Uid(psyUid), availability, i);
        }
    User::After(KSmallDelay);
    }

void CTe_defproxySuiteStepBase::InitPsyListInDefaultProxyL()
    {
    //Remove last working GPS PSY from CenRep
    CRepository* repository = CRepository::NewL(TUid::Uid(0x101f7a7f));
    CleanupStack::PushL(repository);
    repository->Set(0x10000001, KNullDesC());
    CleanupStack::PopAndDestroy(repository);
    //Change the priority of a PSY that is never used in this test driver
    ToggleModuleL(KNppPsy,1);
    ToggleModuleL(KNppPsy,0);
    }

void CTe_defproxySuiteStepBase::ConfigPsyL(TPositionModuleId aModuleId, TInt aCount, ...)
    {

    VA_LIST list;
    VA_START(list, aCount);
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    genInfo->SetRequestedField(KIntGpsPsy1ConfigPsyUid);
    genInfo->SetValue(KIntGpsPsy1ConfigPsyUid, TInt32(aModuleId.iUid));

    genInfo->SetRequestedField(KIntGpsPsy1ConfigItemsNumber);
    genInfo->SetValue(KIntGpsPsy1ConfigItemsNumber, TInt32(aCount));
        

    for(TInt i=0; i<aCount; i++)
        {
        TPsyConfig config;
        config = VA_ARG(list, TPsyConfig);
        
        genInfo->SetRequestedField(KIntGpsPsy1ConfigItemsNumber+1+i);
        TPckg<TPsyConfig> configPkg(config);
        genInfo->SetValue(KIntGpsPsy1ConfigItemsNumber+1+i, configPkg);
        }

    TRequestStatus status;
    iPositioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    CleanupStack::PopAndDestroy(genInfo);
    }

void CTe_defproxySuiteStepBase::ConfigPsyL(RPositioner& aPositioner, TPositionModuleId aModuleId, TInt aCount, ...)
    {
    VA_LIST list;
    VA_START(list, aCount);
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
    genInfo->SetRequestedField(KIntGpsPsy1ConfigPsyUid);
    genInfo->SetValue(KIntGpsPsy1ConfigPsyUid, TInt32(aModuleId.iUid));

    genInfo->SetRequestedField(KIntGpsPsy1ConfigItemsNumber);
    genInfo->SetValue(KIntGpsPsy1ConfigItemsNumber, TInt32(aCount));
        

    for(TInt i=0; i<aCount; i++)
        {
        TPsyConfig config;
        config = VA_ARG(list, TPsyConfig);
        
        genInfo->SetRequestedField(KIntGpsPsy1ConfigItemsNumber+1+i);
        TPckg<TPsyConfig> configPkg(config);
        genInfo->SetValue(KIntGpsPsy1ConfigItemsNumber+1+i, configPkg);
        }

    TRequestStatus status;
    aPositioner.NotifyPositionUpdate(*genInfo, status);
    User::WaitForRequest(status);
    User::LeaveIfError(status.Int());
    CleanupStack::PopAndDestroy(genInfo);
    }

void CTe_defproxySuiteStepBase::StandardPrepareL()
    {
    iModulesSettings = CPosModules::OpenL();
       
    // setup modules in order : Dummy1, Psy6, Dummy3
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

    CPosModuleIdList* idList = iModulesSettings->ModuleIdListLC();
    for (TInt i = 0; i < idList->Count(); i++)
        {
        TPositionModuleInfo info;
        iModulesSettings->GetModuleInfoL((*idList)[i], info);
        switch (info.ModuleId().iUid)
            {
            case KLcfPsy1UidValue:
            case KLcfPsy3UidValue:
            case KLcfPsy6UidValue:
                if (!info.IsAvailable())
                    {
                    moduleUpdate->SetUpdateAvailability(ETrue);
                    iModulesSettings->UpdateModuleL(info.ModuleId(), *moduleUpdate);
                    }
                break;
            default:
                if (info.IsAvailable())
                    {
                    iBackupList->AppendL(info.ModuleId());
                    moduleUpdate->SetUpdateAvailability(EFalse);
                    iModulesSettings->UpdateModuleL(info.ModuleId(), *moduleUpdate);
                    }
                break;
            }
        }
    CleanupStack::PopAndDestroy(2, moduleUpdate);
    
    // setup order
    SetModulePriorityL(KUidLcfPsy1, 0);
    SetModulePriorityL(KUidLcfPsy6, 1);
    SetModulePriorityL(KUidLcfPsy3, 2);

    User::After(KSecond);

    TInt res;
    res = iServer.Connect();
    CheckExpectedResult(res, KErrNone, KFailedConnectLocServer);
    res = iPositioner.Open(iServer);
    CheckExpectedResult(res, KErrNone, KFailedConnectDefProxy);
    res = iPositioner.SetRequestor(
    		CRequestor::ERequestorService,
    		CRequestor::EFormatApplication,
    		KTdDefProxyTestName);
    CheckExpectedResult(res, KErrNone, KFailedSetRequestor);

    //Init time
    iTime1.UniversalTime();
    iTime2.UniversalTime();
    }

void CTe_defproxySuiteStepBase::StandardCleanup()
    {
    iPositioner.Close();
    iServer.Close();
    }

void CTe_defproxySuiteStepBase::SetModulePriorityL(TPositionModuleId aModuleId, TInt aPriority)
    {
    if (aPriority >= 0)
        {
        iModulesSettings->SetModulePriorityL(aModuleId, aPriority);
        }
    // slight delay here
    User::After(KSmallDelay);
    }

void CTe_defproxySuiteStepBase::ToggleModuleL(TPositionModuleId aModuleId, TBool aEnabled)
    {
    CPosModuleIdList* idList = iModulesSettings->ModuleIdListLC();
    TInt index = idList->Find(aModuleId);
    
    if(KErrNotFound == index)
        {
        ERR_PRINTF1(KRequestedModuleNotFound);
        SetTestStepResult(EFail);
        User::Leave(index);
        }

    CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
    update->SetUpdateAvailability(aEnabled);
    iModulesSettings->UpdateModuleL(aModuleId, *update);

    CleanupStack::PopAndDestroy(2, idList);

    // slight delay here
    User::After(KSmallDelay);
    }

void CTe_defproxySuiteStepBase::ToggleModuleVisibleL(TPositionModuleId aModuleId, TBool aVisible)
    {
    CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
    update->SetUpdateVisibility(aVisible);
    iModulesSettings->UpdateModuleL(aModuleId, *update);

    CleanupStack::PopAndDestroy(update);

    // slight delay here
    User::After(KSmallDelay);
    }

void CTe_defproxySuiteStepBase::SetupModuleL(TPositionModuleId aModuleId, TBool aEnabled, TInt aPriority)
    {
    // availablity
    ToggleModuleL(aModuleId, aEnabled);
    // priority
    SetModulePriorityL(aModuleId, aPriority);
    }

void CTe_defproxySuiteStepBase::DisableAllModulesL()
    {
    CPosModuleIdList* idList = iModulesSettings->ModuleIdListLC();

    CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
    update->SetUpdateAvailability(EFalse); // disabled

    for (TInt i = 0; i < idList->Count(); i++)
        {
        TPositionModuleInfo info;
        iModulesSettings->GetModuleInfoL((*idList)[i], info);
        if (info.IsAvailable())
            {
            iModulesSettings->UpdateModuleL((*idList)[i], *update);
            }
        }
    CleanupStack::PopAndDestroy(2, idList);

    // slight delay here
    User::After(KSmallDelay);
    }

void CTe_defproxySuiteStepBase::RestoreModulesL()
	{
	CPosModuleUpdate* update = CPosModuleUpdate::NewLC();
	update->SetUpdateAvailability(ETrue); // enabled
	for (TInt i = 0; i < iBackupList->Count(); i++)
        {
        TPositionModuleInfo info;
        iModulesSettings->GetModuleInfoL((*iBackupList)[i], info);
        if (!info.IsAvailable())
            {
            iModulesSettings->UpdateModuleL((*iBackupList)[i], *update);
            }
        }
	CleanupStack::PopAndDestroy(update);
	
	// slight delay here
	User::After(KSmallDelay);
	}

void CTe_defproxySuiteStepBase::CheckExpectedResult(TInt aResult, TInt aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        ERR_PRINTF2(KValueReturned, aResult);
        ERR_PRINTF2(KValueExpected, aExpectedResult);
        SetTestStepResult(EFail);
        }
    }

void CTe_defproxySuiteStepBase::CheckExpectedResult(TUid aResult, TUid aExpectedResult, const TDesC& aLabel)
    {
    if (aResult!=aExpectedResult)
        {
        ERR_PRINTF1(aLabel);
        SetTestStepResult(EFail);
        }
    }

void CTe_defproxySuiteStepBase::PositionRequestWithCheck(TPositionInfo& _aInfo, TInt _aResult, TUid _aModuleId)
    {
    iPositioner.NotifyPositionUpdate(_aInfo, iStatus);
    User::WaitForRequest(iStatus);
    CheckExpectedResult(iStatus.Int(), _aResult, KWrongRequestResult);
    CheckExpectedResult((_aInfo).ModuleId(), _aModuleId, KWrongModuleIdReturned);
    }


