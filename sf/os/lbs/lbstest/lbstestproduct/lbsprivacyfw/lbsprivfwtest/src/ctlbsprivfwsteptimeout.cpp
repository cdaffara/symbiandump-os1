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

#include "ctlbsprivfwsteptimeout.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );
_LIT( KTxtRequestorName2,	"Requester2" );
_LIT( KTxtLcsClientName2,	"LcsClient2" );

const TInt	KNoReplyDelay = 0;


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepTimeout::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepTimeout::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepTimeout::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepTimeout::doTestStepPostabmleL()"));

	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepTimeout::~CT_LbsPrivFwStepTimeout()
	{
	}


CT_LbsPrivFwStepTimeout::CT_LbsPrivFwStepTimeout(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Timeout);
	}


CT_LbsPrivFwStepTimeout* CT_LbsPrivFwStepTimeout::New(CT_LbsPrivFwServer& aParent)
	{
	CT_LbsPrivFwStepTimeout* testStep = new CT_LbsPrivFwStepTimeout(aParent);
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


void CT_LbsPrivFwStepTimeout::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepTimeout::doTestStepL()
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
			
			// We do not need to set a replay delay - qnn will reply instantly, others will not replay at all
			CleanupStack::Pop(reqInfo);
			SubmitTimeoutLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy, 
					EResponseTypeRejected, KNoReplyDelay); // I want this to happen...
		
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		case 2:
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
			
			// We do not need to set a replay delay - qnn will reply instantly, others will not replay at all
			CleanupStack::Pop(reqInfo);
			SubmitTimeoutLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					EResponseTypeAccepted, KNoReplyDelay);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
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
			
			// We do not need to set a replay delay - qnn will reply instantly, others will not replay at all
			CleanupStack::Pop(reqInfo);
			SubmitTimeoutLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionRejected,
					EResponseTypeRejected, KNoReplyDelay); 
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		case 4:
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
			
			CleanupStack::Pop(reqInfo);
			SubmitComplexTimeoutLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					CPosNetworkPrivacy::EDecisionRejected,
					EResponseTypeAccepted, // How it should be reponded to
					KNoReplyDelay);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		case 5:
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
			
			CleanupStack::Pop(reqInfo);
			SubmitComplexTimeoutLocationRequest(reqInfo, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted, // Default timeout behaviour
					CPosNetworkPrivacy::EDecisionAccepted, // How it should be timed out
					EResponseTypeAccepted, // How it should be responded to
					KNoReplyDelay);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}

		case 6:// Test case to exercise an unknown requestId which is within valid requestId range (>=1)
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionRejected);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit an additional reponse after timeout
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqInvalidTimeoutId);
			// Set the request ID to be a value that has not been used - but is still a valid value (i.e >=1)
			req->SetRequestId(3);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		
		case 7://Multiple external Verification Requests using single client and  Cancel one Privacy Verification Request
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CleanupStack::Pop(reqInfo1);
			SubmitTimeoutLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					EResponseTypeAccepted, KNoReplyDelay);
			
						
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo2);
						
			reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CleanupStack::Pop(reqInfo2);
			SubmitComplexTimeoutLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					CPosNetworkPrivacy::EDecisionRejected,
					EResponseTypeAccepted, // How it should be reponded to
					KNoReplyDelay);
								
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
			
		case 8://Multiple external Verification Requests using single client and  Cancel one Privacy Verification Request
			{
			
			TUint responseDelay=5*1000000; //5 sec
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo1);
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CleanupStack::Pop(reqInfo1);
			SubmitTimeoutLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					EResponseTypeAccepted, responseDelay);
			
						
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo2);
						
			reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			
			CleanupStack::Pop(reqInfo2);
			SubmitComplexTimeoutLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy,
					CPosNetworkPrivacy::EDecisionAccepted,
					CPosNetworkPrivacy::EDecisionRejected,
					EResponseTypeAccepted, // How it should be responded to
					responseDelay);
								
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);

			break;
			}
					
		// Controller specific test
		case 101:
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
			
			CleanupStack::Pop(reqInfo);
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionRejected);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit an additional reponse after timeout
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqRespondBeforeComplete);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		// Controller specific test
		case 102:
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
			
			CleanupStack::Pop(reqInfo);
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionRejected);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit an additional reponse after timeout
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqRespondAfterComplete);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}

		case 103:// Network Privacy API: Test case to exercise an unknown requestId which is > max positive TInt32 value so invalid
				 // Privacy API: Test case to exercise an unknown requestId which is valid
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionRejected);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit an additional reponse after timeout
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqInvalidTimeoutId);
			TInt requestId;
			if(iApiUsed == ENetworkPrivacyAPI)
				{
				// Set the request ID to be a value that is invalid.
				requestId = -1;
				}
			else
				{
				// Set the request ID to be a value that is unknown (has not been used before) but is valid.
				requestId = 6121;
				}
			req->SetRequestId(requestId);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
			
		case 201: // Notifier specific test - Send response claiming to have timed out - with Accept timeout strategy
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeAccepted);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionAccepted);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit a response claiming to have timed out, rather than actually timing out
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqSendTimeoutError);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
			
		case 202: // Notifier specific test - Send response claiming to have timed out - with Reject timeout strategy
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionRejected);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit a response claiming to have timed out, rather than actually timing out
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqSendTimeoutError);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
			
		case 203: // Notifier specific test - Send response claiming to have timed out - with Unknown timeout strategy
			{
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// Make network privacy type requestorinfo identified by name
			// iArray takes ownership of this item.
			CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
			TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			
			CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, netPrivacy, iApiObserver, iPrivacyHandlerType, this);		
			req->SetRequestInfo(reqInfo, gsmInfoType);
			// Set what the outcome should be
			req->SetDesiredResponse(EResponseTypeRejected);
			// Set what should happen if a timeout occurs
			req->SetDecision(CPosNetworkPrivacy::EDecisionNotAvailable);
			// Set time delay before handling request
			req->SetResponseDelay(KNoReplyDelay);
			// Make the request submit a response claiming to have timed out, rather than actually timing out
			req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqSendTimeoutError);
			//Issue the verification request
			req->Start();
			// Add the request to the array (which takes ownership)
			iArray.AppendL(req);
			
			iNotifierObserver->Start(); 
			iApiObserver->Start();
			
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(netPrivacy);
			break;
			}
		}
	
	return TestStepResult();
	}



// With timeout decision
void CT_LbsPrivFwStepTimeout::SubmitComplexTimeoutLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		CPosNetworkPrivacy::TRequestDecision aTimeoutDecision,
		TResponseType aIntendedDecision, 
		TUint aResponseDelay)
	{
	// Change this to cancel object
	CT_LbsPrivFwComplexTimeoutRequest* req = CT_LbsPrivFwComplexTimeoutRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		
	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	// Set HOW the request should be timed out
	req->SetTimeoutDecision(aTimeoutDecision);
	// Set time delay before handling request
	req->SetResponseDelay(aResponseDelay);
	//Issue the verification request
	req->Start();
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}


// With timeout decision
void CT_LbsPrivFwStepTimeout::SubmitTimeoutLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		
	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	// Set time delay before handling request
	req->SetResponseDelay(aResponseDelay);
	//Issue the verification request
	req->Start();
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

// Without timeout decision
void CT_LbsPrivFwStepTimeout::SubmitTimeoutLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		
	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set time delay before handling request
	req->SetResponseDelay(aResponseDelay);
	//Issue the verification request
	req->Start();
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}
