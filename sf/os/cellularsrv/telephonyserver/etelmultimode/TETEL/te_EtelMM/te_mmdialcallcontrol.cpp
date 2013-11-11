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
// This file contains class definitions of TestExecute test steps
// which test that the RMobileCall::DialCallControl API can only
// be called if the application has the correct SID and capabilities
// to call it.
// 
//

/**
 @file
*/

#include "testdef.h"
#include "te_mmdialcallcontrol.h"


CTestDialCallControlCorrectSIDCorrectCap::CTestDialCallControlCorrectSIDCorrectCap()
/** 
Each test step initialises it's own name.
*/
	{
	SetTestStepName(_L("TestDialCallControlCorrectSIDCorrectCap"));
	}

enum TVerdict CTestDialCallControlCorrectSIDCorrectCap::doTestStepL()
/**
@SYMTestCaseID TEL-ETEL-NA-2038
@SYMComponent  telephony_etel
@SYMTestCaseDesc To check client has the required SID and Capability to successfully call the RMobileCall::DialCallControl API.
@SYMTestPriority High
@SYMTestActions  Call the RMobileCall::DialCallControl API with required SID and required Capability and check the return value of the API
@SYMTestExpectedResults KErrNone when SID and capabilities are both correct.
@SYMTestType CT
*/
	{
	TInt iTestCount = 1;
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);

	RMobileCall mmCall;
	TName callName;
	ret=mmCall.OpenNewCall(mmLine,callName);
    TEST(ret==KErrNone);
	
	RMobileCall::TMobileCallParamsV7 mmCallParamsV7;
	TEST(mmCallParamsV7.ExtensionId() == RMobileCall::KETelMobileCallParamsV7);

	mmCallParamsV7.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParamsV7.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParamsV7.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParamsV7.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParamsV7.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParamsV7.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	mmCallParamsV7.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	mmCallParamsV7.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	mmCallParamsV7.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	mmCallParamsV7.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	mmCallParamsV7.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	mmCallParamsV7.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
	mmCallParamsV7.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	mmCallParamsV7.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	mmCallParamsV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
	
	RMobileCall::TMobileCallParamsV7Pckg mmCallParamsV7Pckg(mmCallParamsV7);
	
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber(DMMTSY_PHONE_TEL_NUMBER);
	
	TRequestStatus reqStatus;
	mmCall.DialCallControl(reqStatus, mmCallParamsV7Pckg, telNumber);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl correct SID correct caps passed"), iTestCount++);
	
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();

	return TestStepResult();
	}

CTestDialCallControlIncorrectSIDCorrectCap::CTestDialCallControlIncorrectSIDCorrectCap()
/** 
Each test step initialises it's own name.
*/
	{
	SetTestStepName(_L("TestDialCallControlIncorrectSIDCorrectCap"));
	}

enum TVerdict CTestDialCallControlIncorrectSIDCorrectCap::doTestStepL()
/**
@SYMTestCaseID TEL-ETEL-NA-2039
@SYMComponent  telephony_etel
@SYMTestCaseDesc To check client has the required SID and Capability to successfully call the RMobileCall::DialCallControl API
@SYMTestPriority High
@SYMTestActions Call the RMobileCall::DialCallControl with incorrect SID but required Capability and check the return value of the API
@SYMTestExpectedResults KErrPermissionDenied returned when SID is incorrect.
@SYMTestType CT
*/
	{
	TInt iTestCount = 1;
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);

	RMobileCall mmCall;
	TName callName;
	ret=mmCall.OpenNewCall(mmLine,callName);
    TEST(ret==KErrNone);
	
	RMobileCall::TMobileCallParamsV7 mmCallParamsV7;
	TEST(mmCallParamsV7.ExtensionId() == RMobileCall::KETelMobileCallParamsV7);

	mmCallParamsV7.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParamsV7.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParamsV7.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParamsV7.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParamsV7.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParamsV7.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	mmCallParamsV7.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	mmCallParamsV7.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	mmCallParamsV7.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	mmCallParamsV7.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	mmCallParamsV7.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	mmCallParamsV7.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
	mmCallParamsV7.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	mmCallParamsV7.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	mmCallParamsV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
	
	RMobileCall::TMobileCallParamsV7Pckg mmCallParamsV7Pckg(mmCallParamsV7);
	
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber(DMMTSY_PHONE_TEL_NUMBER);
	
	TRequestStatus reqStatus;
	mmCall.DialCallControl(reqStatus, mmCallParamsV7Pckg, telNumber);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrPermissionDenied);
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl incorrect SID correct caps passed"), iTestCount++);
	
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	return TestStepResult();
	}

CTestDialCallControlCorrectSIDIncorrectCap::CTestDialCallControlCorrectSIDIncorrectCap()
/** 
Each test step initialises it's own name.
*/
	{
	SetTestStepName(_L("TestDialCallControlCorrectSIDIncorrectCap"));
	}

enum TVerdict CTestDialCallControlCorrectSIDIncorrectCap::doTestStepL()
/**
@SYMTestCaseID TEL-ETEL-NA-2040
@SYMComponent  telephony_etel
@SYMTestCaseDesc To check client has the required SID and Capability to successfully call the RMobileCall::DialCallControl API
@SYMTestPriority High
@SYMTestActions  Call the RMobileCall::DialCallControl API with correct SID and incorrect Capability and check the return value of the API
@SYMTestExpectedResults KErrPermissionDenied when capability is incorrect.
@SYMTestType CT
*/
	{
	TInt iTestCount = 1;
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);

	RMobileCall mmCall;
	TName callName;
	ret=mmCall.OpenNewCall(mmLine,callName);
    TEST(ret==KErrNone);
	
	RMobileCall::TMobileCallParamsV7 mmCallParamsV7;
	TEST(mmCallParamsV7.ExtensionId() == RMobileCall::KETelMobileCallParamsV7);

	mmCallParamsV7.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParamsV7.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParamsV7.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParamsV7.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParamsV7.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParamsV7.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	mmCallParamsV7.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	mmCallParamsV7.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	mmCallParamsV7.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	mmCallParamsV7.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	mmCallParamsV7.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	mmCallParamsV7.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
	mmCallParamsV7.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	mmCallParamsV7.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	mmCallParamsV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
	
	RMobileCall::TMobileCallParamsV7Pckg mmCallParamsV7Pckg(mmCallParamsV7);
	
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber(DMMTSY_PHONE_TEL_NUMBER);
	
	TRequestStatus reqStatus;
	mmCall.DialCallControl(reqStatus, mmCallParamsV7Pckg, telNumber);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrPermissionDenied);
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl correct SID incorrect caps passed"), iTestCount++);
	
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	return TestStepResult();
	}

CTestDialCallControlIncorrectSIDIncorrectCap::CTestDialCallControlIncorrectSIDIncorrectCap()
/** 
Each test step initialises it's own name.
*/
	{
	SetTestStepName(_L("TestDialCallControlIncorrectSIDIncorrectCap"));
	}

enum TVerdict CTestDialCallControlIncorrectSIDIncorrectCap::doTestStepL()
/**
@SYMTestCaseID TEL-ETEL-NA-2041
@SYMComponent  telephony_etel
@SYMTestCaseDesc To check client has the required SID and Capability to successfully call the RMobileCall::DialCallControl API
@SYMTestPriority High
@SYMTestActions  Call the RMobileCall::DialCallControl API with incorrect SID and incorrect Capability and check the return value of the API
@SYMTestExpectedResults KErrPermissionDenied returned when both SID and capabilities are both wrong.
@SYMTestType CT
*/
	{
	TInt iTestCount = 1;
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);

	RMobileCall mmCall;
	TName callName;
	ret=mmCall.OpenNewCall(mmLine,callName);
    TEST(ret==KErrNone);
	
	RMobileCall::TMobileCallParamsV7 mmCallParamsV7;
	TEST(mmCallParamsV7.ExtensionId() == RMobileCall::KETelMobileCallParamsV7);

	mmCallParamsV7.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParamsV7.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParamsV7.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParamsV7.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParamsV7.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParamsV7.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	mmCallParamsV7.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	mmCallParamsV7.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	mmCallParamsV7.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	mmCallParamsV7.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	mmCallParamsV7.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	mmCallParamsV7.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
	mmCallParamsV7.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	mmCallParamsV7.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	mmCallParamsV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
	
	RMobileCall::TMobileCallParamsV7Pckg mmCallParamsV7Pckg(mmCallParamsV7);
	
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber(DMMTSY_PHONE_TEL_NUMBER);
	
	TRequestStatus reqStatus;
	mmCall.DialCallControl(reqStatus, mmCallParamsV7Pckg, telNumber);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrPermissionDenied);
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl incorrect SID incorrect caps passed"), iTestCount++);
	
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	return TestStepResult();
	}

