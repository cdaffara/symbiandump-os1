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
// @file ctlbsprivfwstepctlrclosedown.cpp
// This is the class implementation for the LBS privacy controller close down step
// 
//

#include "ctlbsprivfwstepctlrclosedown.h"
#include <lbs/lbsloccommon.h>
#include "ctlbsprivfwctlrclosedownrequest.h"
#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepCtlrCloseDown::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepCtlrCloseDown::doTestStepPreambleL()"));
	CT_LbsPrivFwStepBase::doTestStepPreambleL();

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepCtlrCloseDown::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepCtlrCloseDown::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CT_LbsPrivFwStepCtlrCloseDown::~CT_LbsPrivFwStepCtlrCloseDown()
	{
	if (iNetPrivacy)
		{
		delete iNetPrivacy;
		}
	}
    
CT_LbsPrivFwStepCtlrCloseDown::CT_LbsPrivFwStepCtlrCloseDown(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_CtlrCloseDown);
	}


CT_LbsPrivFwStepCtlrCloseDown* CT_LbsPrivFwStepCtlrCloseDown::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepCtlrCloseDown* testStep = new CT_LbsPrivFwStepCtlrCloseDown(aParent);
// Note the lack of ELeave.
// This means that having insufficient memory will return NULL;

if (testStep)
	{
	TInt err = KErrNone;

	TRAP(err, testStep->ConstructL());
	if (err)
		{
		delete testStep;
		testStep = NULL;
		}
	}
	
return testStep;
}


void CT_LbsPrivFwStepCtlrCloseDown::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepCtlrCloseDown::doTestStepL()
	{
	//Connect to Network Privacy API
	// Note that this is a member variable so that we can control when we delete the client from lbs
	iNetPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	
	// Make network privacy type requestorinfo identified by name
	// iArray takes ownership of this item.
	CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;


	switch(iTestCaseId)
	{
	case 1:	
		{					
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		//Issue a request - Takes ownership of CPosGSMPrivacyRequestInfo
		SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, iNetPrivacy);
		}
	break;

		
	default:
		INFO_PRINTF1(_L("FAIL: Test not implemented or test case ID is unknown."));
		SetTestStepResult(EFail);
	break;
	}
	
	iNotifierObserver->Start(); 
	iApiObserver->Start();
	
	CActiveScheduler::Start();

	return TestStepResult();
	}


void CT_LbsPrivFwStepCtlrCloseDown::ProcessRequestComplete(TInt aRequestId, TInt aCompletionCode)
	{
//	Override base version of this method to delete the network client as soon as the request has completed
	
//	Uncomment when req ids are the same
	TInt arrayIndex = FindArrayIndex(aRequestId); // Use this once request Ids are the same.
	//TInt	arrayIndex = iRequestIdArray.Find(aRequestId);
	//ASSERT(arrayIndex != KErrNotFound);
	
	iArray[arrayIndex]->ProcessPrivRequestComplete(aRequestId, aCompletionCode);
	CheckShutdown();

	delete iNetPrivacy;
	iNetPrivacy = NULL;
	}



void CT_LbsPrivFwStepCtlrCloseDown::SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy)
	{
CT_LbsPrivFwCtlrCloseDownRequest* req = CT_LbsPrivFwCtlrCloseDownRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	

	//Issue request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}
