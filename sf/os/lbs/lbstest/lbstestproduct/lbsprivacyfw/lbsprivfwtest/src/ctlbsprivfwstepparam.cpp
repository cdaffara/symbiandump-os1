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
// @file ctlbsprivfwstepparam.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsprivfwstepparam.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"


_LIT( KTxtRequestorName1,	"1Requester" );
_LIT( KTxtLcsClientName1,	"1LcsClient" );

_LIT( KTxtRequestorName2,	"2Requester" );
_LIT( KTxtLcsClientName2,	"2LcsClient" );

_LIT( KTxtRequestorName3,	"3Requester" );
_LIT( KTxtLcsClientName3,	"3LcsClient" );

_LIT( KTxtRequestorName4,	"4Requester" );
_LIT( KTxtLcsClientName4,	"4LcsClient" );

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepParam::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepParam::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepParam::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepParam::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepParam::~CT_LbsPrivFwStepParam()
	{
	}


CT_LbsPrivFwStepParam::CT_LbsPrivFwStepParam(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Param);
	}


CT_LbsPrivFwStepParam* CT_LbsPrivFwStepParam::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepParam* testStep = new CT_LbsPrivFwStepParam(aParent);
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


void CT_LbsPrivFwStepParam::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepParam::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
	TPosNetworkPrivacyRequestInfoType suplInfoType = ERequestInfoTypeSupl;
    
	switch(iTestCaseId)
	{
	
	//	GSM cases
	case 1:
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeMSISDN);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeURL);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeSIPURL);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		}
		break;
		
		
	case 2:
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeMSISDN);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeURL);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		SubmitNotifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy);

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeSIPURL);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		SubmitNotifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy);		
		}
		break;
		
		
	case 3:
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(8));
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(9));
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		}
		break;
		
		
	case 4:
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(10));
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, static_cast<CPosGSMPrivacyRequestInfo::TIdType>(11));
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);		
		}
		break;




	//	SUPL cases
	case 5:
		{
		CPosSUPLPrivacyRequestInfo* reqInfo1 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, suplInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...

		CPosSUPLPrivacyRequestInfo* reqInfo2 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosSUPLPrivacyRequestInfo::EIdTypeURL);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, suplInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...

		CPosSUPLPrivacyRequestInfo* reqInfo3 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, suplInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		}
		break;
		
		
	case 6:
		{
		CPosSUPLPrivacyRequestInfo* reqInfo1 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitNotifyLocationRequest(reqInfo1, suplInfoType, iApiObserver, netPrivacy);

		CPosSUPLPrivacyRequestInfo* reqInfo2 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosSUPLPrivacyRequestInfo::EIdTypeURL);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		SubmitNotifyLocationRequest(reqInfo2, suplInfoType, iApiObserver, netPrivacy);

		CPosSUPLPrivacyRequestInfo* reqInfo3 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		SubmitNotifyLocationRequest(reqInfo3, suplInfoType, iApiObserver, netPrivacy);		
		}
		break;
		
		
	case 7:
		{
		CPosSUPLPrivacyRequestInfo* reqInfo1 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, static_cast<CPosSUPLPrivacyRequestInfo::TIdType>(8));
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, static_cast<CPosSUPLPrivacyRequestInfo::TIdType>(9));
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, suplInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		}
		break;
		
		
	case 8:
		{
		CPosSUPLPrivacyRequestInfo* reqInfo1 = CPosSUPLPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, static_cast<CPosSUPLPrivacyRequestInfo::TIdType>(10));
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, static_cast<CPosSUPLPrivacyRequestInfo::TIdType>(11));
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitNotifyLocationRequest(reqInfo1, suplInfoType, iApiObserver, netPrivacy);		
		}
		break;




	// Other parameter tests
	case 9:	
		{
		HBufC* requestorName1=HBufC::NewL(255);
		CleanupStack::PushL(requestorName1);
		HBufC* clientName1=HBufC::NewL(255);
		CleanupStack::PushL(clientName1);
		
		TPtr pRequestorName1(requestorName1->Des());
		TPtr pClientName1(clientName1->Des());
		
		if(iApiUsed == ENetworkPrivacyAPI)
			{
			pRequestorName1.AppendFill('a', 255); 
			pClientName1.AppendFill('b', 255); 
			}
		else
			{
			pRequestorName1.AppendFill('a', 63); 
			pClientName1.AppendFill('b', 63); 			
			}
		pRequestorName1[0]='1';
		pClientName1[0]='1';
		
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(pRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(pClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted);

		CleanupStack::PopAndDestroy(clientName1);
		CleanupStack::PopAndDestroy(requestorName1);
		}
		break;
	
	case 10:	
		{
		HBufC* requestorName1=HBufC::NewL(255);
		CleanupStack::PushL(requestorName1);
		HBufC* clientName1=HBufC::NewL(255);
		CleanupStack::PushL(clientName1);
		
		TPtr pRequestorName1(requestorName1->Des());
		TPtr pClientName1(clientName1->Des());
		
		if(iApiUsed == ENetworkPrivacyAPI)
			{
			pRequestorName1.AppendFill('a', 255); 
			pClientName1.AppendFill('b', 255); 
			}
		else
			{
			pRequestorName1.AppendFill('a', 63); 
			pClientName1.AppendFill('b', 63); 			
			}
		pRequestorName1[0]='1';
		pClientName1[0]='1';
		
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(pRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(pClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);
		
		CleanupStack::PopAndDestroy(clientName1);
		CleanupStack::PopAndDestroy(requestorName1);
		}
		break;
		

		
	
	
	case 11: // Four request type parameters using VerifyLocationRequest()
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		// Note that we have not set up a request type so it will take the default value of ERequestTypeUnknown.
		// The Network Privacy API will pass this through without problems but the Privacy Request API should
		// reject the request returning KErrArgument
		TInt err = KErrNone;
		err = SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		if (err != KErrNone)
			{
			if (iApiUsed != EPrivacyRequestAPI || err != KErrArgument)
				{
				INFO_PRINTF2(_L("FAIL: Expected a return code of KErrArgument, received %d"), err);
				SetTestStepResult(EFail);
				}
			}

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		err = SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestSingleShot, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestPeriodic);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		err = SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestPeriodic, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}

		CPosGSMPrivacyRequestInfo* reqInfo4 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo4);
		reqInfo4->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetRequestType(TLbsExternalRequestInfo::ERequestAreaEvent);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo4);
		err = SubmitVerifyLocationRequest(reqInfo4, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted); // Network wants this to happen...
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestAreaEvent, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}
		}
		break;
		
		
	case 12: // Four request type parameters using NotifyLocationRequest()
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		// Note that we have not set up a request type so it will take the default value of ERequestTypeUnknown.
		// The Network Privacy API will pass this through without problems but the Privacy Request API should
		// reject the request returning KErrArgument
		TInt err = KErrNone;
		err = SubmitNotifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy);
		if (err != KErrNone)
			{
			if (iApiUsed != EPrivacyRequestAPI || err != KErrArgument)
				{
				INFO_PRINTF2(_L("FAIL: Expected a return code of KErrArgument, received %d"), err);
				SetTestStepResult(EFail);
				}
			}

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		err = SubmitNotifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestSingleShot, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestPeriodic);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		err = SubmitNotifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy);		
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestPeriodic, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}
		
		CPosGSMPrivacyRequestInfo* reqInfo4 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo4);
		reqInfo4->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetRequestType(TLbsExternalRequestInfo::ERequestAreaEvent);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo4);
		err = SubmitNotifyLocationRequest(reqInfo4, gsmInfoType, iApiObserver, netPrivacy);		
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestAreaEvent, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}
		}
		break;
	
	case 13: // Four request type parameters using NotifyVerificationTimeout()
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		// Note that we have not set up a request type so it will take the default value of ERequestTypeUnknown.
		// The Network Privacy API will pass this through without problems but the Privacy Request API should
		// reject the request returning KErrArgument
		TInt err = KErrNone;
		err = SubmitNotifyVerificationTimeout(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionRejected,
									EResponseTypeRejected, 1);
		if (err != KErrNone)
			{
			if (iApiUsed != EPrivacyRequestAPI || err != KErrArgument)
				{
				INFO_PRINTF2(_L("FAIL: Expected a return code of KErrArgument, received %d"), err);
				SetTestStepResult(EFail);
				}
			}

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo2);
		err = SubmitNotifyVerificationTimeout(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
											CPosNetworkPrivacy::EDecisionRejected,
											EResponseTypeRejected, 2);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestSingleShot, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(TLbsExternalRequestInfo::ERequestPeriodic);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo3);
		err = SubmitNotifyVerificationTimeout(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
											CPosNetworkPrivacy::EDecisionRejected,
											EResponseTypeRejected, 3);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestPeriodic, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}

		CPosGSMPrivacyRequestInfo* reqInfo4 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo4);
		reqInfo4->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetRequestType(TLbsExternalRequestInfo::ERequestAreaEvent);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo4);
		err = SubmitNotifyVerificationTimeout(reqInfo4, gsmInfoType, iApiObserver, netPrivacy, 
											CPosNetworkPrivacy::EDecisionRejected,
											EResponseTypeRejected, 4);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("FAIL: In ERequestAreaEvent, received a return code of %d"), err);
			SetTestStepResult(EFail);
			}
		}
		break;
		
		
	case 14: // Out of range request type parameter using Network Privacy API
		{
		CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo1);
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(TLbsExternalRequestInfo::ERequestTypeMaxEnum + 2);

		// Should never get here since the SetRequestType call panics
		INFO_PRINTF1(_L("FAIL: SetRequestType call did not panic"));
		SetTestStepResult(EFail);
		CleanupStack::PopAndDestroy(netPrivacy);
		return TestStepResult();
		}
		
		
	case 15: // Out of range request type parameter using Privacy Request API
		{
		if (iApiUsed == EPrivacyRequestAPI)
			{
			TLbsPrivacyRequest privacy;
			TLbsExternalRequestInfo requestInfo;
			
			privacy.SetRequestAdvice(TLbsPrivacyRequest::ERequestAdviceVerify);
			privacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionAllow);

			requestInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
			requestInfo.SetRequesterId(KTxtRequestorName1);
			requestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatLogicalName);
			requestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
			// don't set up the client name - makes the validation check more complicated
			//requestInfo.SetClientName(KTxtLcsClientName1);
			//requestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatLogicalName);
			//requestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
			requestInfo.SetRequestSource(TLbsExternalRequestInfo::ERequestSourceNetwork);
			
			TUint32 requestId;
			TInt err;

			// make a request with a dodgy request type
			requestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestTypeMaxEnum + 2);
			err = netPrivacy->NewPrivacyRequest(requestId, privacy, requestInfo );
			if (err != KErrArgument)
				{
				INFO_PRINTF2(_L("FAIL: Illegal Request Type - expected a return code of KErrNotSupported, received %d"), err);
				SetTestStepResult(EFail);
				}

			// make a request with an illegal Format indicator
			requestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
			requestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMIN);
			err = netPrivacy->NewPrivacyRequest(requestId, privacy, requestInfo );
			if (err != KErrArgument)
				{
				INFO_PRINTF2(_L("FAIL: Illegal Format Indicator - expected a return code of KErrArgument, received %d"), err);
				SetTestStepResult(EFail);
				}

			// this test doesn't actually make a request so don't wait for a request to be processed
			CleanupStack::PopAndDestroy(netPrivacy);
			return TestStepResult();
			}
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


TInt CT_LbsPrivFwStepParam::SubmitVerifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	(void)aResponseDelay;
	TInt err = KErrNone;
	
	CT_LbsPrivFwVerifyRequest* req = CT_LbsPrivFwVerifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	//Issue the verification request
	TRAP(err, req->Start());
	if (err != KErrNone)
		{// abandon this request
		delete req;
		}
	else
		{// Add the request to the array (which takes ownership)
		iArray.AppendL(req);
		}
	return err;
	}

TInt  CT_LbsPrivFwStepParam::SubmitNotifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy)
	{
	TInt err = KErrNone;
	CT_LbsPrivFwNotifyRequest* req = CT_LbsPrivFwNotifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	//Issue the notification request
	TRAP(err, req->Start());
	if (err != KErrNone)
		{// abandon this request
		delete req;
		}
	else
		{// Add the request to the array (which takes ownership)
		iArray.AppendL(req);
		}
	return err;
	}

TInt  CT_LbsPrivFwStepParam::SubmitNotifyVerificationTimeout(CPosNetworkPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy,
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TInt aRequestId,
		TUint aResponseDelay)
	{
	TInt err = KErrNone;
	CT_LbsPrivFwTimeoutRequest* req = CT_LbsPrivFwTimeoutRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		
	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	// Set time delay before handling request
	req->SetResponseDelay(aResponseDelay);
	// Make the request submit an additional reponse after timeout
	req->SetBehaviourType(CT_LbsPrivFwTimeoutRequest::EReqInvalidTimeoutId);
	// Set the request ID to be a value that has not been used - but is still a valid value (i.e >=1)
	req->SetRequestId(aRequestId);
	//Issue the NotifyVerificationTimeout request
	TRAP(err, req->Start());
	if (err != KErrNone)
		{// abandon this request
		delete req;
		}
	else
		{// Add the request to the array (which takes ownership)
		iArray.AppendL(req);
		}
	return err;
	}


