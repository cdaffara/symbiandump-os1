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

#include "ctlbsprivfwstepsettings.h"
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
TVerdict CT_LbsPrivFwStepSettings::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepSettings::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStepSettings::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepSettings::doTestStepPostabmleL()"));

	return TestStepResult();
	}


CT_LbsPrivFwStepSettings::~CT_LbsPrivFwStepSettings()
	{
	}


CT_LbsPrivFwStepSettings::CT_LbsPrivFwStepSettings(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_Settings);
	}


CT_LbsPrivFwStepSettings* CT_LbsPrivFwStepSettings::New(CT_LbsPrivFwServer& aParent)
	{
	CT_LbsPrivFwStepSettings* testStep = new CT_LbsPrivFwStepSettings(aParent);
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


void CT_LbsPrivFwStepSettings::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepSettings::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);
	// Make 2 requestors
	CPosGSMPrivacyRequestInfo* reqInfo = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo);
	CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo2);
	switch(iTestCaseId)
		{
		case 1:
			{
			// Check that timeout is set correctly
			TTimeIntervalMicroSeconds32 input(10*1000);
			netPrivacy->SetConnectionTimeout(input);
			
			TTimeIntervalMicroSeconds32 output = netPrivacy->ConnectionTimeout();
			TEST(input.Int() == output.Int());
			break;
			}
		case 2:
			{
			// Check that setting invalid timeout results in a panic
			netPrivacy->SetConnectionTimeout(-1000);
			
			break;
			}
		case 3:
			{
			// Check that default timeout is 30 seconds
			TTimeIntervalMicroSeconds32 defaultTime(30000000);
			TTimeIntervalMicroSeconds32 output = netPrivacy->ConnectionTimeout();
			TEST(output.Int() == defaultTime.Int());
			break;
			}
		case 4:
			{
			// Check Requestor Name is set correctly
			
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			
			TPtrC outputName;
			CPosGSMPrivacyRequestInfo::TIdType outputType;
			reqInfo->GetRequestor(outputName, outputType);
			TEST(outputName.CompareC(KTxtRequestorName1) == 0);
			
			break;
			}
		case 5:
			{
			// Test that a Requestor string longer than 255 will cause a panic
			TInt reqId;
			TBuf<256> longString;
			if(iApiUsed  == ENetworkPrivacyAPI)
				{
				longString.Fill('b', 256);
				}
			else
				{
				longString.Fill('b', 64);
				}
			reqInfo2->SetRequestorL(longString, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			TRAPD(err, netPrivacy->NotifyLocationRequestL(*reqInfo2, reqId));
			if(err!= -6)
				{
				ERR_PRINTF1(_L("Test Failed - Large String did not result in a leave"));
				SetTestStepResult(EFail);
				}
			break;
			}
		case 6:
			{
			// Check Requestor Name is set correctly
			reqInfo->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			
			TPtrC outputName;
			CPosGSMPrivacyRequestInfo::TIdType outputType;
			reqInfo->GetRequestor(outputName, outputType);
			TEST(outputName.CompareC(KTxtRequestorName2) == 0);
			break;
			}
		case 7:
			{
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			
			TPtrC outputName;
			CPosGSMPrivacyRequestInfo::TIdType outputType;
			reqInfo->GetLCSClient(outputName, outputType);
			TEST(outputName.CompareC(KTxtLcsClientName1) == 0);
			break;
			}
		case 8:
			{
			TInt reqId;
			TBuf<256> longString;
			if(iApiUsed  == ENetworkPrivacyAPI)
				{
				longString.Fill('b', 256);
				}
			else
				{
				longString.Fill('b', 63);
				}
			reqInfo2->SetLCSClientL(longString, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			TRAPD(err, netPrivacy->NotifyLocationRequestL(*reqInfo2, reqId));
			if(err!= -6)
				{
				ERR_PRINTF1(_L("Test Failed - Large String did not result in a leave"));
				SetTestStepResult(EFail);
				}
			break;
			}
		case 9:
			{
			reqInfo->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			reqInfo->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
			
			TPtrC outputName;
			CPosGSMPrivacyRequestInfo::TIdType outputType;
			reqInfo->GetLCSClient(outputName, outputType);
			TEST(outputName.CompareC(KTxtLcsClientName2) == 0);
			break;
			}
		}
	CleanupStack::PopAndDestroy(reqInfo2);
	CleanupStack::PopAndDestroy(reqInfo);
	CleanupStack::PopAndDestroy(netPrivacy);
	return TestStepResult();
	}

