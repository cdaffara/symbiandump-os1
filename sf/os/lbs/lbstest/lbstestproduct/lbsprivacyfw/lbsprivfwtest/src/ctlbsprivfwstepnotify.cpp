// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctlbsprivfwstepnotify.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"


_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

_LIT( KTxtRequestorName2,	"Requester2" );
_LIT( KTxtLcsClientName2,	"LcsClient2" );

_LIT( KTxtRequestorName3,	"Requester3" );
_LIT( KTxtLcsClientName3,	"LcsClient3" );

_LIT( KTxtRequestorName4,	"Requester4" );
_LIT( KTxtLcsClientName4,	"LcsClient4" );

_LIT( KTxtRequestorName5,	"Requester5" );
_LIT( KTxtLcsClientName5,	"LcsClient5" );

_LIT( KTxtRequestorName6,	"Requester6" );
_LIT( KTxtLcsClientName6,	"LcsClient6" );

_LIT( KTxtRequestorName7,	"Requester7" );
_LIT( KTxtLcsClientName7,	"LcsClient7" );

_LIT( KTxtRequestorName8,	"Requester8" );
_LIT( KTxtLcsClientName8,	"LcsClient8" );

_LIT( KTxtRequestorName9,	"Requester9" );
_LIT( KTxtLcsClientName9,	"LcsClient9" );



/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepNotify::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepNotify::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepNotify::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepNotify::doTestStepPostabmleL()"));

	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepNotify::~CT_LbsPrivFwStepNotify()
	{
	}


CT_LbsPrivFwStepNotify::CT_LbsPrivFwStepNotify(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Notify);
	}


CT_LbsPrivFwStepNotify* CT_LbsPrivFwStepNotify::New(CT_LbsPrivFwServer& aParent)
	{
	CT_LbsPrivFwStepNotify* testStep = new CT_LbsPrivFwStepNotify(aParent);
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


void CT_LbsPrivFwStepNotify::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepNotify::doTestStepL()
	{
	switch(iTestCaseId)
		{
		case 1:
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo);
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo);
			SubmitNotifyLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
				// 3x3 requests
		case 2:
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			//Connect to Network Privacy API again
			CPrivFwApiWrapper* netPrivacy2 = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy2);
			
			//Connect to Network Privacy API again
			CPrivFwApiWrapper* netPrivacy3 = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy3);
			
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
						
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);
						
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy2);
			
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy3);
			
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);
						
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy2);
			
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy3);
			
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);
						
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy2);
			
			reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			reqInfo1->SetRequestorL(KTxtRequestorName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy3);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
		
			CleanupStack::PopAndDestroy(netPrivacy3);
			CleanupStack::PopAndDestroy(netPrivacy2);
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		case 3:
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo);
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo);
			SubmitNotifyLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy);
			
			reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo);
			reqInfo->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo);
			SubmitNotifyLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy);
			
			reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo);
			reqInfo->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo);
			SubmitNotifyLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		
		}
	return TestStepResult();
	}


void CT_LbsPrivFwStepNotify::SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy)
	{
	CT_LbsPrivFwNotifyRequest* req = CT_LbsPrivFwNotifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	

	//Issue the notification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}
