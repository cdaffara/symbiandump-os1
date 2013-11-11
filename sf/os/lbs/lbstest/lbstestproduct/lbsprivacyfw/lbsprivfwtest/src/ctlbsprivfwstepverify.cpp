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

#include "ctlbsprivfwstepverify.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

#include <lbs/lbsadmin.h>

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

_LIT( KTxtRequestorName10,	"Requester10" );
_LIT( KTxtLcsClientName10,	"LcsClient10" );

TTimeIntervalMicroSeconds responseDelay=0;


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepVerify::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepVerify::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepVerify::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepVerify::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepVerify::~CT_LbsPrivFwStepVerify()
	{
	}


CT_LbsPrivFwStepVerify::CT_LbsPrivFwStepVerify(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Verify);
	}


CT_LbsPrivFwStepVerify* CT_LbsPrivFwStepVerify::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepVerify* testStep = new CT_LbsPrivFwStepVerify(aParent);
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


void CT_LbsPrivFwStepVerify::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepVerify::doTestStepL()
	{
		
	// make additional client(s) that use the other API
	TLbsCTApiUsed alternateApi = (iApiUsed == ENetworkPrivacyAPI ? EPrivacyRequestAPI : ENetworkPrivacyAPI);
	
	// To Pops and cleans up the specified number 
	TUint popcount=0;
    
	// Make network privacy type requestorinfo identified by name
	// iArray takes ownership of this item.
	CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo1);
	

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;

	switch(iTestCaseId)
	{
	case 1:	
		{					
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy);
	    //To Pops and cleans up the specified number 
	    popcount =1;
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...
		}
	break;
	
	case 2:	
		{	
		reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo1);
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy);		
		//To Pops and cleans up the specified number 
		popcount=1;
		SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...
		}
		break;
	
	case 3:	
		{	
		 //Client 1
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
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		
		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		//Client 2
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy2 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy2);
		CPosGSMPrivacyRequestInfo* reqInfo4 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo4);
		reqInfo4->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo4);
		SubmitVerifyLocationRequest(reqInfo4, gsmInfoType, iApiObserver, netPrivacy2, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo5 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo5);
		reqInfo5->SetRequestorL(KTxtRequestorName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo5->SetLCSClientL(KTxtLcsClientName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo5->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo5);
		SubmitVerifyLocationRequest(reqInfo5, gsmInfoType, iApiObserver, netPrivacy2, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		CPosGSMPrivacyRequestInfo* reqInfo6 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo6);
		reqInfo6->SetRequestorL(KTxtRequestorName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo6->SetLCSClientL(KTxtLcsClientName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo6->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo6);
		SubmitVerifyLocationRequest(reqInfo6, gsmInfoType, iApiObserver, netPrivacy2, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		//Client 3
		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy3 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy3);
		CPosGSMPrivacyRequestInfo* reqInfo7 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo7);
		reqInfo7->SetRequestorL(KTxtRequestorName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo7->SetLCSClientL(KTxtLcsClientName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo7->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
		CleanupStack::Pop(reqInfo7);
		SubmitVerifyLocationRequest(reqInfo7, gsmInfoType, iApiObserver, netPrivacy3, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo8 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo8);
		reqInfo8->SetRequestorL(KTxtRequestorName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo8->SetLCSClientL(KTxtLcsClientName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo8->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo8);
		SubmitVerifyLocationRequest(reqInfo8, gsmInfoType, iApiObserver, netPrivacy3, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		CPosGSMPrivacyRequestInfo* reqInfo9 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo9);
		reqInfo9->SetRequestorL(KTxtRequestorName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo9->SetLCSClientL(KTxtLcsClientName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo9->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo9);
		SubmitVerifyLocationRequest(reqInfo9, gsmInfoType, iApiObserver, netPrivacy3, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...
	
		//To Pops and cleans up the specified number 
		popcount=3;
		}
		break;
	
	case 4:	
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
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...
		}
		break;
		
	case 6:	
		{
	    // This case creates 10 clients and sends 1 privacy request per client
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
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy2 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy2);
		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy2, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

 		
 		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy3 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy3);
 		CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo3);
		reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo3->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo3);
		SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy3, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy4 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy4);
		CPosGSMPrivacyRequestInfo* reqInfo4 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo4);
		reqInfo4->SetRequestorL(KTxtRequestorName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetLCSClientL(KTxtLcsClientName4, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo4->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo4);
		SubmitVerifyLocationRequest(reqInfo4, gsmInfoType, iApiObserver, netPrivacy4, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy5 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy5);
		CPosGSMPrivacyRequestInfo* reqInfo5 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo5);
		reqInfo5->SetRequestorL(KTxtRequestorName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo5->SetLCSClientL(KTxtLcsClientName5, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo5->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo5);
		SubmitVerifyLocationRequest(reqInfo5, gsmInfoType, iApiObserver, netPrivacy5, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy6 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy6);
		CPosGSMPrivacyRequestInfo* reqInfo6 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo6);
		reqInfo6->SetRequestorL(KTxtRequestorName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo6->SetLCSClientL(KTxtLcsClientName6, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo6->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo6);
		SubmitVerifyLocationRequest(reqInfo6, gsmInfoType, iApiObserver, netPrivacy6, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy7 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy7);
		CPosGSMPrivacyRequestInfo* reqInfo7 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo7);
		reqInfo7->SetRequestorL(KTxtRequestorName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo7->SetLCSClientL(KTxtLcsClientName7, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo7->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo7);
		SubmitVerifyLocationRequest(reqInfo7, gsmInfoType, iApiObserver, netPrivacy7, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy8 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy8);
		CPosGSMPrivacyRequestInfo* reqInfo8 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo8);
		reqInfo8->SetRequestorL(KTxtRequestorName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo8->SetLCSClientL(KTxtLcsClientName8, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo8->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo8);
		SubmitVerifyLocationRequest(reqInfo8, gsmInfoType, iApiObserver, netPrivacy8, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy9 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy9);
		CPosGSMPrivacyRequestInfo* reqInfo9 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo9);
		reqInfo9->SetRequestorL(KTxtRequestorName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo9->SetLCSClientL(KTxtLcsClientName9, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo9->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo9);
		SubmitVerifyLocationRequest(reqInfo9, gsmInfoType, iApiObserver, netPrivacy9, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		//Connect to Network Privacy API
	    CPrivFwApiWrapper* netPrivacy10 = CPrivFwApiWrapper::NewL(iApiUsed);
	    CleanupStack::PushL(netPrivacy10);
		CPosGSMPrivacyRequestInfo* reqInfo10 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo10);
		reqInfo10->SetRequestorL(KTxtRequestorName10, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo10->SetLCSClientL(KTxtLcsClientName10, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo10->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo10);
		SubmitVerifyLocationRequest(reqInfo10, gsmInfoType, iApiObserver, netPrivacy10, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...	
		
		//To Pops and cleans up the specified number 
		popcount=10;
		}
		break;

	case 9:	
		{	
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
									EResponseTypeAccepted,responseDelay); // I want this to happen...

		
		CPrivFwApiWrapper* alternatePrivacyApi = CPrivFwApiWrapper::NewL(alternateApi);
	    CleanupStack::PushL(alternatePrivacyApi);
		CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
		CleanupStack::PushL(reqInfo2);
		reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		CleanupStack::Pop(reqInfo2);
		SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, alternatePrivacyApi, 
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeRejected,responseDelay); // I want this to happen...		
        // To Pops and cleans up the specified number 
		popcount=2;
		}
		break;
	 case 8:	
		{	
		responseDelay = 5*1000000; // 5 secs response delay to check the buffer in controller 
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
									EResponseTypeAccepted,responseDelay); // I want this to happen...

				
		//Client 2
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
									CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
									EResponseTypeAccepted,responseDelay); // I want this to happen...
	
				
		// To Pops and cleans up the specified number 
	    popcount=2;	 
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
	
	CleanupStack::PopAndDestroy(popcount);
	
	return TestStepResult();
	}


void CT_LbsPrivFwStepVerify::SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision,TTimeIntervalMicroSeconds aResponseDelay)
	
	{
		
	CT_LbsPrivFwVerifyRequest* req = CT_LbsPrivFwVerifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);

	//Set after what time notifier/controller should response
	req->SetResponseDelay(aResponseDelay);
	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

