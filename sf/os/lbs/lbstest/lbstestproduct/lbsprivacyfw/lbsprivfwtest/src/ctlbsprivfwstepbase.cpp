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
// @file ctlbsprivfwstepbase.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsprivfwstepbase.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"



TBool CT_LbsPrivFwStepBase::NeedSessionTerminate()
    {
    return iSPPMInUse;
    }


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepBase::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepBase::doTestStepPreambleL()"));
	
	TInt testCaseId;
	
	if(!GetIntFromConfig(ConfigSection(),KTe_testCaseId,testCaseId))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	iTestCaseId = testCaseId;
	
    GetBoolFromConfig(ConfigSection(),KTe_SPPMInUse,iSPPMInUse);
	
	TInt apiUsed;
	_LIT(KTe_apiUsed, "api");
	if(!GetIntFromConfig(ConfigSection(),KTe_apiUsed,apiUsed))
		{
		// By default use the Network Privacy API
		apiUsed = ENetworkPrivacyAPI;
		}
	iApiUsed = static_cast<TLbsCTApiUsed>(apiUsed);

	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepBase::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepBase::doTestStepPostabmleL()"));

	return TestStepResult();
	}


CT_LbsPrivFwStepBase::~CT_LbsPrivFwStepBase()
	{
	delete iApiObserver;
	delete iNotifierObserver;
	delete iProxy;
  
	iArray.ResetAndDestroy();
	}


CT_LbsPrivFwStepBase::CT_LbsPrivFwStepBase(CT_LbsPrivFwServer& aParent) : iParent(aParent), iArray(10)
	{
	}

void CT_LbsPrivFwStepBase::ConstructL()
	{
	// Create the base class objects.
	iApiObserver = CT_LbsPrivFwApiDataObserver::NewL(this);
	iNotifierObserver = CT_LbsPrivFwNotifierDataObserver::NewL(this);
	iNotifierObserver->SetLogger(Logger());
	DeterminePrivacyHandlerL();
	iProxy = CPrivFwProxy::NewL(iPrivacyHandlerType, *iNotifierObserver);
	
	//iRequestIdArray.Reset();

	}


TVerdict CT_LbsPrivFwStepBase::doTestStepL()
	{
	return TestStepResult();
	}


TInt CT_LbsPrivFwStepBase::FindArrayIndex(TUint aReqId)
	{
	TInt arraySize = iArray.Count();
	for(int i = 0; i < arraySize; i++)
		{
		if(iArray[i]->RequestId() == aReqId)
			{
			return i;
			}
		}
	return -1;
	}


TInt CT_LbsPrivFwStepBase::FindArrayIndexByClientName(const TLbsTestRequestInfo& aTestInfo)
	{
	TBuf<KLbsMaxClientNameSize2> testClientName;
	aTestInfo.GetClientName(testClientName);
	//Find client name and put into testClientName
	//
	TInt arraySize = iArray.Count();
	for(int i = 0; i < arraySize; i++)
		{
		CPosNetworkPrivacyRequestInfo& netReq = iArray[i]->RequestInfo();
		TPosNetworkPrivacyRequestInfoType netReqType = iArray[i]->RequestInfoType();
		TPtrC clientName;
		switch(netReqType)
			{
			case ERequestInfoTypeGsm:
				{
				CPosGSMPrivacyRequestInfo& gsmReq = static_cast<CPosGSMPrivacyRequestInfo&>(netReq);
				CPosGSMPrivacyRequestInfo::TIdType unused;
				gsmReq.GetLCSClient(clientName, unused);
				
				break;
				}
			case ERequestInfoTypeSupl:
				{
				CPosSUPLPrivacyRequestInfo& suplReq = static_cast<CPosSUPLPrivacyRequestInfo&>(netReq);
				CPosSUPLPrivacyRequestInfo::TIdType unused;
				suplReq.GetLCSClient(clientName, unused);
				break;
				}
			}
		// If testClientName is smaller than clientName in length, make sure its
		// found at offset 0, otherwise its an error. Standard error codes will fail 
		// here too e.g. KErrNotFound.
		if(clientName.Find(testClientName) == 0)
			{
			return i;
			}
		}
	return -1;
	}


void CT_LbsPrivFwStepBase::CheckShutdown()
	{
	TInt arraySize = iArray.Count();
	TInt terminals = 0;
	for(int i = 0; i < arraySize; i++)
		{
		if(iArray[i]->IsRequestComplete())
			{
			terminals++;
			}
		else
			{
			break;
			}
		}
	if (terminals == arraySize)
		{
		CActiveScheduler::Stop();
		}
	}

// helper methods to check if the named process is running
TBool CT_LbsPrivFwStepBase::IsProcessRunning(const TDesC& aProcessName)
	{
	TFindProcess findProcess(aProcessName);
	TFullName fullName;
	RProcess process;
	TBool processIsRunning = EFalse;
	TInt err = KErrNone;
	
	while (err = findProcess.Next(fullName), err == KErrNone)
		{
		TInt processOpenErr = process.Open(findProcess);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			process.Close();
			if (exitType == EExitPending)
				{
				// found a running instance of the process,
				processIsRunning = ETrue;
				break;
				}
			}
		}
	return processIsRunning;
	}


TBool CT_LbsPrivFwStepBase::IsProcessStopped(const TDesC& aProcessName)
	{
	return !IsProcessRunning(aProcessName);
	}


// From MApiDataObserver
void CT_LbsPrivFwStepBase::ProcessApiData(TInt aRequestId, TInt aCompletionCode)
	{
	TInt arrayIndex = FindArrayIndex(aRequestId);
	iArray[arrayIndex]->ProcessPrivApiData(aRequestId, aCompletionCode);
	CheckShutdown();
	}


// From MNotifierDataObserver
void CT_LbsPrivFwStepBase::ProcessNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData)
	{
	//iRequestIdArray.Append(aRequestId);
	TInt arrayIndex = FindArrayIndexByClientName(aPrivNotifierData);
	iArray[arrayIndex]->ProcessPrivNotifierData(aRequestId, aPrivNotifierData);
	CheckShutdown();
	}

void CT_LbsPrivFwStepBase::ProcessRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
//	Uncomment when req ids are the same
	TInt arrayIndex = FindArrayIndex(aRequestId); // Use this once request Ids are the same.
	//TInt	arrayIndex = iRequestIdArray.Find(aRequestId);
	//ASSERT(arrayIndex != KErrNotFound);
	
	iArray[arrayIndex]->ProcessPrivRequestComplete(aRequestId, aCompletionCode);
	CheckShutdown();
	}

void CT_LbsPrivFwStepBase::ProcessNotiferCtrlCloseDown()
	{
	TInt arrayIndex = 0;
	iArray[arrayIndex]->ProcessPrivNotiferCtrlCloseDown();
	CheckShutdown();
	}


void CT_LbsPrivFwStepBase::DeterminePrivacyHandlerL()
	{

	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	TInt err;
	
	CLbsAdmin::TPrivacyHandler tempPrivacyHandlerType;
	err = adminApi->Get(KLbsSettingPrivacyHandler, tempPrivacyHandlerType);	
	User::LeaveIfError(err);
	
	switch(tempPrivacyHandlerType)
		{
		case CLbsAdmin::EPrivacyHandleByNotifier:
			iPrivacyHandlerType = EPrivacyHandleByNotifier;
		break;
		
		case CLbsAdmin::EPrivacyHandleByController:
			iPrivacyHandlerType = EPrivacyHandleByController;
		break;
		
		case CLbsAdmin::EPrivacyHandleByAdvancedNotifier:
			iPrivacyHandlerType = EPrivacyHandleByQnN;
		break;
		
		case CLbsAdmin::EPrivacyHandlerUnknown:
			// Leave if there's any error.
			User::Leave(KErrNotFound);
		break;

		default:
			// Leave if there's any error.
			User::Leave(KErrNotFound);
		break;		
		}

	
	CleanupStack::PopAndDestroy(adminApi);
	}
