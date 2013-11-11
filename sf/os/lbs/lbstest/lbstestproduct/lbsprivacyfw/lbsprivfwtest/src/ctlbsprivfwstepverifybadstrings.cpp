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
// @file ctlbsprivfwstepverifybadstrings.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsprivfwstepverifybadstrings.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

_LIT( KTxtRequestorName2,	"Requester2" );
_LIT( KTxtLcsClientName2,	"LcsClient2" );


_LIT( KTxtLcsClientName3,	"LcsClient3" );

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepVerifyBadStrings::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepVerifyBadStrings::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepVerifyBadStrings::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepVerifyBadStrings::doTestStepPostabmleL()"));

	return TestStepResult();
	}


CT_LbsPrivFwStepVerifyBadStrings::~CT_LbsPrivFwStepVerifyBadStrings()
	{
	}


CT_LbsPrivFwStepVerifyBadStrings::CT_LbsPrivFwStepVerifyBadStrings(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_VerifyBadStrings);
	}


CT_LbsPrivFwStepVerifyBadStrings* CT_LbsPrivFwStepVerifyBadStrings::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepVerifyBadStrings* testStep = new CT_LbsPrivFwStepVerifyBadStrings(aParent);
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


void CT_LbsPrivFwStepVerifyBadStrings::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepVerifyBadStrings::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);
	
	// Make network privacy type requestorinfo identified by name
	// iArray takes ownership of this item.
	CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo1);

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;


	switch(iTestCaseId)
	{
	case 5:	
		{
	    			
		// Single client, multiple reqests, each with bad reqinfo
		
		reqInfo1->SetRequestorL(KTxtRequestorName1, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(6));
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); 
		
		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(7));
		reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);

		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); 


		HBufC* requestorName1=HBufC::NewL(256);
		TPtr pRequestorName1(requestorName1->Des());
		pRequestorName1.AppendFill('a', 256); 
		pRequestorName1[0]='1';

									
		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(pRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); 

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

	CleanupStack::PopAndDestroy(netPrivacy);
	
	return TestStepResult();
	}


void CT_LbsPrivFwStepVerifyBadStrings::SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint /*aResponseDelay*/)
	{
	CT_LbsPrivFwVerifyRequest* req = CT_LbsPrivFwVerifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

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
