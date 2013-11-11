// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "Te_EtelSatTestStepBase.h"
#include "TE_EtelSatcontrol.h"

CTestSatControl::CTestSatControl()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatControl"));
	}

enum TVerdict CTestSatControl::doTestStepL()
	{
	INFO_PRINTF1(_L("**************************************"));
	INFO_PRINTF1(_L("Test RSat Control Functionality"));
	INFO_PRINTF1(_L("**************************************"));

	RPhone phone;
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RSat sat;
	ret=sat.Open(phone);
	TEST(ret==KErrNone);

	// local variables used throughout the gprs tests
	TRequestStatus reqStatus;
	iTestCount=1;

	// Call Control - Address
	RSat::TCallControlV1* callControl = new (ELeave) RSat::TCallControlV1;
	CleanupStack::PushL(callControl);
	RSat::TCallControlV1Pckg* callControlPckg = new (ELeave) RSat::TCallControlV1Pckg(*callControl);
	CleanupStack::PushL(callControlPckg);

	sat.NotifyCallControlRequest(reqStatus,*callControlPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TControlResult* result = new (ELeave) RSat::TControlResult;
	CleanupStack::PushL(result);

	callControl->GetCcGeneralResult(*result);
	TEST(*result==DSATTSY_CC_RESULT);

	RSat::TCallControlType* ccType = new (ELeave) RSat::TCallControlType;
	CleanupStack::PushL(ccType);
	callControl->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcAddress);

	RSat::TCallSetUpParams* address = new (ELeave) RSat::TCallSetUpParams;
	CleanupStack::PushL(address);

	TEST(callControl->GetCallSetUpDetails(*address)==KErrNone);
	TEST(address->iCcp1==DSATTSY_CCP1);
	TEST(address->iSubAddress==DSATTSY_SUBADDRESS);
	TEST(address->iCcp2==DSATTSY_CCP2);
	TEST(address->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(address->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(address->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);

	RSat::TAlphaIdValidity* validity = new (ELeave) RSat::TAlphaIdValidity;
	CleanupStack::PushL(validity);
	RSat::TAlphaId* alphaId = new (ELeave) RSat::TAlphaId;
	CleanupStack::PushL(alphaId);

	callControl->GetAlphaId(*validity, *alphaId);
	TEST(*validity==DSATTSY_NO_ALPHAID);
	TEST(alphaId->iStatus==DSATTSY_ALPHAID_STATUS);
	TEST(alphaId->iAlphaId==DSATTSY_NULL_BUF);
	RSat::TBCRepeatIndicator bcRepeatIndicator;
	callControl->GetBCRepeatIndicator(bcRepeatIndicator);

	TEST(bcRepeatIndicator==DSATTSY_BC_REPEAT_INDICATOR_SEQUENTIAL);

	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus, *callControlPckg);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);


	// Call Control - SS String
	RSat::TSsString* ssString = new (ELeave) RSat::TSsString;
	CleanupStack::PushL(ssString);

	sat.NotifyCallControlRequest(reqStatus,*callControlPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	callControl->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcSsString);

	RSat::TUssdString ussdString;
	TEST(callControl->GetSendUssdDetails(ussdString)==KErrNotSupported);
	TEST(callControl->GetCallSetUpDetails(*address)==KErrNotSupported);

	TEST(callControl->GetSendSsDetails(*ssString)==KErrNone);
	TEST(ssString->iTypeOfNumber==DSATTSY_TON);
	TEST(ssString->iNumberPlan==DSATTSY_NPI);
	TEST(ssString->iSsString==DSATTSY_SS_STRING);

	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus,*callControlPckg);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);

	// Call Control - USSD String & Call originator
	RSat::TCallControlV2* callControlV2 = new (ELeave) RSat::TCallControlV2;
	CleanupStack::PushL(callControlV2);
	RSat::TCallControlV2Pckg* callControlV2Pckg = new (ELeave) RSat::TCallControlV2Pckg(*callControlV2);
	CleanupStack::PushL(callControlV2Pckg);

	sat.NotifyCallControlRequest(reqStatus,*callControlV2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	callControlV2->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcUssdString);

	TEST(callControlV2->GetCallSetUpDetails(*address)==KErrNotSupported);
	TEST(callControlV2->GetSendSsDetails(*ssString)==KErrNotSupported);

	TEST(callControlV2->GetSendUssdDetails(ussdString)==KErrNone);
	TEST(ussdString.iDcs==DSATTSY_DCS);
	TEST(ussdString.iUssdString==DSATTSY_USSD_STRING);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyCallControlRequest - USSD String - passed"),iTestCount++);

	TName callNameV2;
	callControlV2->GetCallName(callNameV2);
	TEST(callNameV2 == DSATTSY_CALL_NAME);

	RSat::TActionOriginator* callOriginatorV2 = new (ELeave) RSat::TActionOriginator;
	CleanupStack::PushL(callOriginatorV2);
	callControlV2->GetActionOriginator(*callOriginatorV2);
	TEST(*callOriginatorV2 == DSATTSY_ACTION_ORIGINATOR);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyCallControlRequest - V2 passed"), iTestCount++);

	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus,*callControlV2Pckg);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);

	// Call Control - PDP Context Activation Parameters and BC Repeat Indicator Fallback Mode

	RSat::TCallControlV5* callControlV5 = new (ELeave) RSat::TCallControlV5;
	CleanupStack::PushL(callControlV5);
	RSat::TCallControlV5Pckg* callControlPckgV5 = new (ELeave) RSat::TCallControlV5Pckg(*callControlV5);
	CleanupStack::PushL(callControlPckgV5);

	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	callControlV5->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcPDPParameters);

	callControlV5->GetBCRepeatIndicator(bcRepeatIndicator);
	TEST(bcRepeatIndicator==DSATTSY_BC_REPEAT_INDICATOR_FALLBACK);

	TPdpParameters  pdpParams;
	callControlV5->GetPdpParameters(pdpParams);
	TEST(pdpParams==DSATTSY_PDP_PARAMETERS);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyCallControlRequest PDP Context Activation Parameters and BC Repeat Indicator Fallback Mode - passed"),iTestCount++);

	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV5);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);

	// Call Control - PDP Context Activation Parameters and BC Repeat Indicator Service Change and Fallback Mode

	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	callControlV5->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcPDPParameters);

	callControlV5->GetBCRepeatIndicator(bcRepeatIndicator);
	TEST(bcRepeatIndicator==DSATTSY_BC_REPEAT_INDICATOR_SERVICECHANGEANDFALLBACK);

	callControlV5->GetPdpParameters(pdpParams);
	TEST(pdpParams==DSATTSY_PDP_PARAMETERS2);


	INFO_PRINTF2(_L("Test %d - RSat::NotifyCallControlRequest - PDP Context Activation Parameters and BC Repeat Indicator Service Change and Fallback Mode - passed"),iTestCount++);

	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV5);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);
	CleanupStack::PopAndDestroy(2);

	// Call Control with V6 Parameters
	RSat::TCallControlV6* callControlV6 = new (ELeave) RSat::TCallControlV6;
	CleanupStack::PushL(callControlV6);
	TEST(callControlV6->ExtensionId() == RSat::KSatV6);
	RSat::TCallControlV6Pckg* callControlPckgV6 = new (ELeave) RSat::TCallControlV6Pckg(*callControlV6);
	CleanupStack::PushL(callControlPckgV6);
	
	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV6);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	callControlV6->GetCallControlType(*ccType);
	TEST(*ccType==RSat::ECcPDPParameters);

	callControlV6->GetBCRepeatIndicator(bcRepeatIndicator);
	TEST(bcRepeatIndicator==DSATTSY_BC_REPEAT_INDICATOR_SERVICECHANGEANDFALLBACK);

	callControlV6->GetPdpParameters(pdpParams);
	TEST(pdpParams==DSATTSY_PDP_PARAMETERS2);
	
	RSat::TCallParamOrigin callParamOrigin;
	callControlV6->GetCallParamOrigin(callParamOrigin);
	TEST(callParamOrigin == DSATTSY_CC_CALL_PARAM_ORIGIN);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyCallControlRequest Call originator - passed"),iTestCount++);	
	
	// Call Control Cancel
	sat.NotifyCallControlRequest(reqStatus,*callControlPckgV6);
	sat.CancelAsyncRequest(ESatNotifyCallControlRequest);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::CallControlCancel passed"),iTestCount++);
	CleanupStack::PopAndDestroy(2);

	// MO SM Control
	RSat::TMoSmControlV1* moSmControl = new (ELeave) RSat::TMoSmControlV1;
	CleanupStack::PushL(moSmControl);
	RSat::TMoSmControlV1Pckg* moSmControlPckg = new (ELeave) RSat::TMoSmControlV1Pckg(*moSmControl);
	CleanupStack::PushL(moSmControlPckg);

	sat.NotifyMoSmControlRequest(reqStatus,*moSmControlPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(moSmControl->iRpAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(moSmControl->iRpAddress.iNumberPlan==DSATTSY_NPI);
	TEST(moSmControl->iRpAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(moSmControl->iTpAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(moSmControl->iTpAddress.iNumberPlan==DSATTSY_NPI);
	TEST(moSmControl->iTpAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(moSmControl->iResult==DSATTSY_CONTROL_RESULT);
	TEST(moSmControl->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(moSmControl->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyMoSmControlRequest passed"),iTestCount++);

	// MO SM Control Cancel
	sat.NotifyMoSmControlRequest(reqStatus,*moSmControlPckg);
	sat.CancelAsyncRequest(ESatNotifyMoSmControlRequest);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::MoSmControlCancel passed"),iTestCount++);


	CleanupStack::PopAndDestroy(13);

	sat.Close();
	phone.Close();

	return TestStepResult();
	}


