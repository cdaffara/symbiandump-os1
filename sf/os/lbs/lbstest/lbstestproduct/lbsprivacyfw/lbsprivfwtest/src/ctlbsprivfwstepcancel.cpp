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

#include "ctlbsprivfwstepcancel.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

_LIT( KTxtRequestorName2,	"Requester2" );
_LIT( KTxtLcsClientName2,	"LcsClient2" );

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
//TVerdict CT_LbsPrivFwStepCancel::doTestStepPreambleL()
//	{
//	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
//	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepNotify::doTestStepPreambleL()"));
//	CT_LbsPrivFwStepBase::doTestStepPreambleL();
//
//	return TestStepResult();
//	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
//TVerdict CT_LbsPrivFwStepCancel::doTestStepPostambleL()
//	{
//	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
//	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepCancel::doTestStepPostabmleL()"));
//
//	//SetTestStepResult(EPass);  // or EFail
//	return TestStepResult();
//	}


CT_LbsPrivFwStepCancel::~CT_LbsPrivFwStepCancel()
	{
	}


CT_LbsPrivFwStepCancel::CT_LbsPrivFwStepCancel(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Cancel);
	}


CT_LbsPrivFwStepCancel* CT_LbsPrivFwStepCancel::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepCancel* testStep = new CT_LbsPrivFwStepCancel(aParent);
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


void CT_LbsPrivFwStepCancel::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepCancel::doTestStepL()
	{

	/* Note: 
	 * the name "netPrivacy" does not accurately describe the object.  It is
	 * an historical name reflecting the first API under test.  There has since been an
	 * additional API (Privacy Request) and the object created could be either of the APIs
	 * dependant on the value of iApiUsed
	 */

	// make additional client(s) that use the other API
	TLbsCTApiUsed alternateApi = (iApiUsed == ENetworkPrivacyAPI ? EPrivacyRequestAPI : ENetworkPrivacyAPI);
	
	// To Pops and cleans up the specified number 
	TUint popcount=0;
        
	// Make network privacy type requestorinfo identified by name
	// iArray takes ownership of this item.
	CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo1);
	
	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;

	//TUint32	networkTimeout = KDefaultNetworkPrivacyTimeoutInSecond;
	TTimeIntervalMicroSeconds responseDelay=0;
	
	switch(iTestCaseId)
		{
		case 1:
			{
			responseDelay = 2*1000000; // 2 secs
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// To Pops and cleans up the specified number 
			popcount=1;
			
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterDialog);
										
			break;			
			}

		case 2:
			{
			responseDelay = 2*1000000; // 2 secs
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);																																								
			// To Pops and cleans up the specified number 
			popcount=1;			
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionAccepted, // If a timeout happens, do this... - NOTE: EDecisionNotAvailable doesnt work - This is the way its implemented in Nile - Bizarre
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterDialog,
										CPosNetworkPrivacy::ECancelReasonTimeout);
			break;
			}

		case 3:
			{
			responseDelay = 5*1000000; // 5 secs
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelNone); 


			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy2 = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy2);
			CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo2);

			reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo2);
			SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy2, 
										CPosNetworkPrivacy::EDecisionAccepted, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelBeforeDialog); 
			// To Pops and cleans up the specified number 
			popcount=2;
			break;			
			}

		case 4:
			{
			responseDelay = 2*1000000; // 2 secs
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// To Pops and cleans up the specified number 
			popcount=1;
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterComplete,
										CPosNetworkPrivacy::ECancelReasonTimeout); 
			break;			
			}

		case 5:
			{
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			// To Pops and cleans up the specified number 
			popcount=1;
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelInvalidRequestId); 
			break;			
			}

		case 6:
			{
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// To Pops and cleans up the specified number 
			popcount=1;
			SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelNotifyAfterDialog); 
			break;			
			}

		case 8:
			{
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			// Just call the cancel functions directly with a range of invalid request ids, and non-existent requests.
			for (TInt reqId = -1; reqId < 2; reqId++)
				{
				netPrivacy->CancelVerifyLocationRequest(reqId);
				netPrivacy->CancelVerifyLocationRequest(reqId, CPosNetworkPrivacy::ECancelReasonTimeout);
				}

			// Cleanup and return.
			CleanupStack::PopAndDestroy(netPrivacy);
			CleanupStack::PopAndDestroy(reqInfo1);
						
			return TestStepResult();
			}

        case 9:
			{
			//responseDelay = 2*1000000; // 2 secs
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
			
			//Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterDialog);
										
			//Connect to Privacy API
			CPrivFwApiWrapper* alternatePrivacyApi = CPrivFwApiWrapper::NewL(alternateApi);
			CleanupStack::PushL(alternatePrivacyApi);

			CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo2);

			reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo2);
			SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, alternatePrivacyApi, 
										CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterDialog); 
			
			// To Pops and cleans up the specified number 
			popcount=2;
			break;			
			}
        case 10://Multiple Privacy Verification Requests using single client and Cancel one Privacy Verification Request (Timeout) after dialog
			{
			responseDelay = 5*1000000; // 5 secs 
			reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo1);
		    	
		    //Connect to Network Privacy API
			CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
			CleanupStack::PushL(netPrivacy);
			
			SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionAccepted, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelNone); 


			CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
			CleanupStack::PushL(reqInfo2);

			reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
			CleanupStack::Pop(reqInfo2);
			SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
										CPosNetworkPrivacy::EDecisionAccepted, // If a timeout happens, do this...
										EResponseTypeAccepted,
										responseDelay,
										CT_LbsPrivFwCancelRequest::EReqCancelAfterDialog,
										CPosNetworkPrivacy::ECancelReasonTimeout); 
			
			
			// To Pops and cleans up the specified number 
			popcount=1;
			
			break;			
			}		
		default:
			User::Invariant();
		}
		


	iNotifierObserver->Start(); 
	iApiObserver->Start();
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(popcount);

	User::After(5*1000000); //Wait for 10 seconds

	return TestStepResult();
	}


void CT_LbsPrivFwStepCancel::SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, 
		TTimeIntervalMicroSeconds aResponseDelay,
		CT_LbsPrivFwCancelRequest::TReqCancelBehaviour aCancelBehaviour,
    	CPosNetworkPrivacy::TCancelReason aCancelReason )
	{
	CT_LbsPrivFwCancelRequest* req = CT_LbsPrivFwCancelRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	//Set after what time notifier/controller should response
	req->SetResponseDelay(aResponseDelay);
	//Set when user wants to cancel request
	req->SetCancelBehaviour(aCancelBehaviour);
	//Set Cancel Reason
	req->SetCancelReason(aCancelReason);
	//Set request type to ERequestVerifyCancel
	req->SetRequestType(ERequestVerify);
	
	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

void CT_LbsPrivFwStepCancel::SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy,
		TTimeIntervalMicroSeconds aResponseDelay,
		CT_LbsPrivFwCancelRequest::TReqCancelBehaviour aCancelBehaviour,
    	CPosNetworkPrivacy::TCancelReason aCancelReason )
	{
	CT_LbsPrivFwCancelRequest* req = CT_LbsPrivFwCancelRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);

	//Set after what time notifier/controller should response
	req->SetResponseDelay(aResponseDelay);
	//Set when user wants to cancel request
	req->SetCancelBehaviour(aCancelBehaviour);
	//Set Cancel Reason
	req->SetCancelReason(aCancelReason);
	//Set request type to ERequestVerifyCancel
	req->SetRequestType(ERequestNotify);

	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}
