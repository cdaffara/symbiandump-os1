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
// @file ctlbsprivfwstepnoinstall.cpp
// This is the class implementation for the LBS privacy notifier not installed test step
// 
//

#include "ctlbsprivfwstepnoinstall.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwverrequestwithnonotifier.h"
#include "ctlbsprivfwnotrequestwithnonotifier.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"


_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

const TInt K5Sec = 5 * 1000000; 


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepNoInstall::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepNoInstall::doTestStepPreambleL()"));
	CT_LbsPrivFwStepBase::doTestStepPreambleL();
	
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
TVerdict CT_LbsPrivFwStepNoInstall::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepNoInstall::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepNoInstall::~CT_LbsPrivFwStepNoInstall()
	{
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
	}


CT_LbsPrivFwStepNoInstall::CT_LbsPrivFwStepNoInstall(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_NoInstall);
	}


CT_LbsPrivFwStepNoInstall* CT_LbsPrivFwStepNoInstall::New(CT_LbsPrivFwServer& aParent)
	{
	CT_LbsPrivFwStepNoInstall* testStep = new CT_LbsPrivFwStepNoInstall(aParent);
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


void CT_LbsPrivFwStepNoInstall::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}


TVerdict CT_LbsPrivFwStepNoInstall::doTestStepL()
	{
	switch(iTestCaseId)
		{
		case 1: //Submit a notification request
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a notification request - Takes ownership of CPosGSMPrivacyRequestInfo
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);

			iNotifierObserver->Start(); 
			iApiObserver->Start();
		    iTimer->Start(K5Sec, 0, TCallBack(TimerCallBack, this));

			
			CActiveScheduler::Start();
			
		    if (iTimer)
		        {
		        iTimer->Cancel();
		        }
	
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
				
		case 2: //Submit a verification request
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosSUPLPrivacyRequestInfo* reqInfo1 = CPosSUPLPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType suplInfoType = ERequestInfoTypeSupl;
			
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			SubmitVerifyLocationRequest(reqInfo1, suplInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionAccepted,
										EResponseTypeAccepted);
		
			iNotifierObserver->Start(); 
			iApiObserver->Start();
		    iTimer->Start(K5Sec, 0, TCallBack(TimerCallBack, this));

			
			CActiveScheduler::Start();
			
		    if (iTimer)
		        {
		        iTimer->Cancel();
		        }
	
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		}
	return TestStepResult();
	}


void CT_LbsPrivFwStepNoInstall::SubmitNotifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy)
	{
	CT_LbsPrivFwNotRequestWithNoNotifier* req = CT_LbsPrivFwNotRequestWithNoNotifier::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	

	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

void CT_LbsPrivFwStepNoInstall::SubmitVerifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	(void)aResponseDelay;
	
	CT_LbsPrivFwVerRequestWithNoNotifier* req = CT_LbsPrivFwVerRequestWithNoNotifier::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

TInt CT_LbsPrivFwStepNoInstall::TimerCallBack(TAny* aPtr)
    {
    ((CT_LbsPrivFwStepNoInstall*)aPtr)->CheckShutdown();
    return 0; 
    }

