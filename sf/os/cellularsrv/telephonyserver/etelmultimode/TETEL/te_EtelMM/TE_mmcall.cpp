// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmcall.h"
#include "testdef.h"

CTestCall::CTestCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCall"));
	}

enum TVerdict CTestCall::doTestStepL()
//
// Test Call Functions
//
    {
	iTestCount =1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Call Functionality"));
	
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

	TRequestStatus reqStatus;
	   	
   	// Test DialNoFdnCheck (Overloaded)

	TBuf<RMobilePhone::KMaxMobileTelNumberSize> TelNumber(DMMTSY_PHONE_TEL_NUMBER);
	
	// Provide standard multimode call parameter package for DialNoFdnCheck

	RMobileCall::TMobileCallParamsV1  mmCallParams;
	RMobileCall::TMobileCallParamsV1Pckg mmCallParamsPckg(mmCallParams);

	mmCallParams.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;

	// First Asynchronous
	mmCall.DialNoFdnCheck(reqStatus, TelNumber);
	User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone);
  
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (First async) passed"), iTestCount++);

	// First Asynchronous & cancel
	mmCall.DialNoFdnCheck(reqStatus, TelNumber);
	mmCall.CancelAsyncRequest(EMobileCallDialNoFdnCheck);

    User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

   	if (reqStatus.Int()==KErrCancel)
   		INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (First async & cancel (Cancelled Request)) passed"), iTestCount++);
   	else
   		INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (First async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Second Asynchronous
	mmCall.DialNoFdnCheck(reqStatus, mmCallParamsPckg, TelNumber);
	User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone);
  
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (Second async) passed"), iTestCount++);

	// Second Asynchronous & cancel
	mmCall.DialNoFdnCheck(reqStatus, mmCallParamsPckg, TelNumber);
	mmCall.CancelAsyncRequest(EMobileCallDialNoFdnCheck);

    User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

   	if (reqStatus.Int()==KErrCancel)
   		INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (Second async & cancel (Cancelled Request)) passed"), iTestCount++);
   	else
   		INFO_PRINTF2(_L("Test %d - RMobileCall::DialNoFdnCheck (Second async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test Set Call Parameters within Dial
	// Create a multimode call parameter package, send it as part of Dial
	mmCallParams.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;

//	TBuf<RMobilePhone::KMaxMobileTelNumberSize> number(DMMTSY_PHONE_TEL_NUMBER);
	iNumber = DMMTSY_PHONE_TEL_NUMBER;
	TEST(KErrNone==mmCall.Dial(mmCallParamsPckg, iNumber));
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (MM Call Params) passed"), iTestCount++);

	
	// Test Set Call using V2 Parameters within Dial
	// Create a multimode call parameter package, send it as part of Dial
	RMobileCall::TMobileCallParamsV2  mmCallParamsV2;

	mmCallParamsV2.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParamsV2.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParamsV2.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParamsV2.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParamsV2.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParamsV2.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParamsV2.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	mmCallParamsV2.iBearerMode = DMMTSY_CALL_NEW_BEARER;

	RMobileCall::TMobileCallParamsV2Pckg mmCallParamsV2Pckg(mmCallParamsV2);
	TEST(KErrNone==mmCall.Dial(mmCallParamsV2Pckg, iNumber));
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (MMv2 Call Params) passed"), iTestCount++);

	// Test Set Call using V7 Parameters within Dial
	// Create a multimode call parameter package, send it as part of Dial
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
	TEST(KErrNone==mmCall.Dial(mmCallParamsV7Pckg, iNumber));
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (MMv7 Call Params) passed"), iTestCount++);
	
	// Test RMobileCall::DialCallControl with RMobileCall::TMobileCallParamsV7
	
	// If this test fails, ensure that the line:
	// etel_KSatEngineSid 0x10281806
	// is present in \epoc32\data\epoc.ini
	TRequestStatus dialCallControlStatus;
	mmCall.DialCallControl(dialCallControlStatus, mmCallParamsV7Pckg, iNumber);
	User::WaitForRequest(dialCallControlStatus);
	TEST(KErrNone==dialCallControlStatus.Int());
	
	// Asynchronous and cancel
	mmCall.DialCallControl(dialCallControlStatus, mmCallParamsV7Pckg, iNumber);
	mmCall.CancelAsyncRequest(EMobileCallDialCallControl);
	User::WaitForRequest(dialCallControlStatus);
	TEST(KErrNone==dialCallControlStatus.Int() || KErrCancel==dialCallControlStatus.Int());
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl with RMobileCall::TMobileCallParamsV3 passed"), iTestCount++);
	
	// Test RMobileCall::DialCallControl with no number
	TBuf<1> noNumberBuf;
	noNumberBuf.Zero();
	mmCall.DialCallControl(dialCallControlStatus, mmCallParamsV7Pckg, noNumberBuf);
	User::WaitForRequest(dialCallControlStatus);
	TEST(KErrArgument==dialCallControlStatus.Int());	
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::DialCallControl with RMobileCall::TMobileCallParamsV3 and no number passed"), iTestCount++);
	
	//
	
	// Try to get just the core ETel API parts of the TCallParams
	RCall::TCallParams coreCallParams;
	RCall::TCallParamsPckg coreCallParamsPckg(coreCallParams);

	TEST(KErrNone==mmCall.GetCallParams(coreCallParamsPckg));

	TEST(coreCallParams.ExtensionId()==0);
	TEST(coreCallParams.iInterval==DMMTSY_CALL_INTERVAL);

	// Get the multimode call parameters this time
	RMobileCall::TMobileCallParamsV1  mmCallParams2;
	RMobileCall::TMobileCallParamsV1Pckg mmCallParams2Pckg(mmCallParams2);

	TEST(KErrNone==mmCall.GetCallParams(mmCallParams2Pckg));

	TEST(mmCallParams2.ExtensionId()==RMobileCall::KETelMobileCallParamsV1);
	TEST(mmCallParams2.iInterval==DMMTSY_CALL_INTERVAL);
	TEST(mmCallParams2.iIdRestrict==DMMTSY_CALL_CLI_RESTRICT_SETTING1);
	TEST(mmCallParams2.iCug.iExplicitInvoke==DMMTSY_CALL_CUG_INVOKE);
	TEST(mmCallParams2.iCug.iCugIndex==DMMTSY_CALL_CUG_INDEX);
	TEST(mmCallParams2.iCug.iSuppressOA==DMMTSY_CALL_CUG_OA);
	TEST(mmCallParams2.iCug.iSuppressPrefCug==DMMTSY_CALL_CUG_PREF);
	TEST(mmCallParams2.iAutoRedial==DMMTSY_CALL_AUTO_REDIAL_FLAG);

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCallParams (MM Call Params) passed"), iTestCount++);

	// Test GetMobileDataCallCaps
	RMobileCall::TMobileCallDataCapsV1 dataCaps, asyncDataCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCapsPckg(dataCaps);
	RMobileCall::TMobileCallDataCapsV1Pckg asyncDataCapsPckg(asyncDataCaps);

	TEST(mmCall.GetMobileDataCallCaps(dataCapsPckg) == KErrNone);
	ValidateCallDataCaps(dataCaps);

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileDataCallCaps (sync) passed"), iTestCount++);

	// Test NotifyMobileDataCallCapsChange
	// asynchronous
	mmCall.NotifyMobileDataCallCapsChange(reqStatus,asyncDataCapsPckg);
  	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateCallDataCaps(asyncDataCaps);

	// asynchronous & cancel
	mmCall.NotifyMobileDataCallCapsChange(reqStatus,asyncDataCapsPckg);
	mmCall.CancelAsyncRequest(EMobileCallNotifyMobileDataCallCapsChange);
  	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileDataCallCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		ValidateCallDataCaps(asyncDataCaps);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileDataCallCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Set Data Call Parameters within Dial
	RMobileCall::TMobileDataCallParamsV1 dataParams, retrieveDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg dataParamsPckg(dataParams);
	RMobileCall::TMobileDataCallParamsV1Pckg retrieveDataParamsPckg(retrieveDataParams);
	
	dataParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	dataParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	dataParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	dataParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	dataParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	dataParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	
	dataParams.iService = DMMTSY_DATA_CALL_SERVICE;
	dataParams.iSpeed = DMMTSY_DATA_CALL_SPEED;
	dataParams.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	dataParams.iQoS = DMMTSY_DATA_CALL_QOS;
	dataParams.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	dataParams.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
	dataParams.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;

	mmCall.Dial(reqStatus, dataParamsPckg, iNumber);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (Data Call Params) passed"), iTestCount++);
	
	// Test Get Data Call Params
	TEST(mmCall.GetCallParams(retrieveDataParamsPckg) == KErrNone);
	TEST(retrieveDataParams.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV1);

	TEST(retrieveDataParams.iIdRestrict==DMMTSY_CALL_CLI_RESTRICT_SETTING1);
	TEST(retrieveDataParams.iCug.iExplicitInvoke==DMMTSY_CALL_CUG_INVOKE);
	TEST(retrieveDataParams.iCug.iCugIndex==DMMTSY_CALL_CUG_INDEX);
	TEST(retrieveDataParams.iCug.iSuppressOA==DMMTSY_CALL_CUG_OA);
	TEST(retrieveDataParams.iCug.iSuppressPrefCug==DMMTSY_CALL_CUG_PREF);
	TEST(retrieveDataParams.iAutoRedial==DMMTSY_CALL_AUTO_REDIAL_FLAG);
	ValidateDataCallParams(retrieveDataParams, 0);
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCallParams (Data Call Params) passed"), iTestCount++);

	// Test Set Data Call Parameters within Dial
	RMobileCall::TMobileDataCallParamsV8 dataParams8, retrieveDataParams8;
	RMobileCall::TMobileDataCallParamsV8Pckg dataParamsPckg8(dataParams8);
	RMobileCall::TMobileDataCallParamsV8Pckg retrieveDataParamsPckg8(retrieveDataParams8);
	
	TEST(dataParams8.ExtensionId()== RMobileCall::KETelMobileDataCallParamsV8);
	TEST(dataParams8.iCallParamOrigin == RMobileCall::EOriginatorUnknown);
	TEST(dataParams8.iBCRepeatIndicator == RMobileCall::EBCAlternateMode);
	
	dataParams8.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	dataParams8.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	dataParams8.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	dataParams8.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	dataParams8.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	dataParams8.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	
	dataParams8.iService = DMMTSY_DATA_CALL_SERVICE;
	dataParams8.iSpeed = DMMTSY_DATA_CALL_SPEED;
	dataParams8.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	dataParams8.iQoS = DMMTSY_DATA_CALL_QOS;
	dataParams8.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	dataParams8.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
	dataParams8.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;
	dataParams8.iBearerMode = DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED;
		
	dataParams8.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	dataParams8.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	dataParams8.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	dataParams8.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	dataParams8.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
	dataParams8.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	dataParams8.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	dataParams8.iAlphaId = DMMTSY_CALL_ALPHA_ID;
		
		
	mmCall.Dial(reqStatus, dataParamsPckg8, iNumber);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (Data Call Params8) passed"), iTestCount++);
		
	// Test Get Data Call Params
	TEST(mmCall.GetCallParams(retrieveDataParamsPckg8) == KErrNone);
	TEST(retrieveDataParams8.ExtensionId()==RMobileCall::KETelMobileDataCallParamsV8);
	
	TEST(retrieveDataParams8.iIdRestrict==DMMTSY_CALL_CLI_RESTRICT_SETTING1);
	TEST(retrieveDataParams8.iCug.iExplicitInvoke==DMMTSY_CALL_CUG_INVOKE);
	TEST(retrieveDataParams8.iCug.iCugIndex==DMMTSY_CALL_CUG_INDEX);
	TEST(retrieveDataParams8.iCug.iSuppressOA==DMMTSY_CALL_CUG_OA);
	TEST(retrieveDataParams8.iCug.iSuppressPrefCug==DMMTSY_CALL_CUG_PREF);
	TEST(retrieveDataParams8.iAutoRedial==DMMTSY_CALL_AUTO_REDIAL_FLAG);
	
	ValidateDataCallParams(retrieveDataParams8, 0);
	TEST(retrieveDataParams8.iCallParamOrigin==DMMTSY_CALL_PARAM_ORIGIN);
	TEST(retrieveDataParams8.iSubAddress==DMMTSY_CALL_SUBADDRESS);
	TEST(retrieveDataParams8.iBearerCap1==DMMTSY_CALL_BEARER_CAP1);
	TEST(retrieveDataParams8.iBearerCap2==DMMTSY_CALL_BEARER_CAP2);
	TEST(retrieveDataParams8.iBCRepeatIndicator==DMMTSY_CALL_BC_REPEAT_INDICATOR);
	TEST(retrieveDataParams8.iIconId.iIdentifier==DMMTSY_CALL_ICON_ID_IDENTIFIER);
	TEST(retrieveDataParams8.iIconId.iQualifier==DMMTSY_CALL_ICON_ID_QUALIFIER);
	TEST(retrieveDataParams8.iAlphaId==DMMTSY_CALL_ALPHA_ID);
		
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCallParams (Data Call Params8) passed"), iTestCount++);

	// Test GetMobileDataCallRLPRange
	RMobileCall::TMobileDataRLPRangesV1 rlpRanges;
	RMobileCall::TMobileDataRLPRangesV1Pckg rlpRangesPckg(rlpRanges);

	// asynchronous
	mmCall.GetMobileDataCallRLPRange(reqStatus, DMMTSY_CALL_RLP_VERSION1, rlpRangesPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// extract the returned rlpRanges from the package
	rlpRanges = rlpRangesPckg();
	ValidateRLPRange(rlpRanges);	

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileDataCallRLPRange passed"), iTestCount++);
	
	// asynchronous & cancel
	mmCall.GetMobileDataCallRLPRange(reqStatus, DMMTSY_CALL_RLP_VERSION1, rlpRangesPckg);
	mmCall.CancelAsyncRequest(EMobileCallGetMobileDataCallRLPRange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileDataCallRLPRange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		// extract the returned rlpRanges from the package
		rlpRanges = rlpRangesPckg();
		ValidateRLPRange(rlpRanges);
		INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileDataCallRLPRange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	
	// Test Set Hcsd Parameters within Dial
	RMobileCall::TMobileHscsdCallParamsV1 hscsdParams, retrieveHscsdParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg hscsdParamsPckg(hscsdParams);
	RMobileCall::TMobileHscsdCallParamsV1Pckg retrieveHscsdParamsPckg(retrieveHscsdParams);
	
	hscsdParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	hscsdParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	hscsdParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	hscsdParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	hscsdParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	hscsdParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	
	hscsdParams.iService = DMMTSY_DATA_CALL_SERVICE;
	hscsdParams.iSpeed = DMMTSY_DATA_CALL_SPEED;
	hscsdParams.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	hscsdParams.iQoS = DMMTSY_DATA_CALL_QOS;
	hscsdParams.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	hscsdParams.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
	hscsdParams.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;
	
	hscsdParams.iInterval = DMMTSY_CALL_INTERVAL;
	hscsdParams.iWantedAiur = DMMTSY_CALL_HCSD_AIUR;
	hscsdParams.iWantedRxTimeSlots = DMMTSY_CALL_HCSD_TS;
	hscsdParams.iMaxTimeSlots = DMMTSY_CALL_HCSD_MAXTS;
	hscsdParams.iCodings = DMMTSY_CALL_HCSD_CODING;
	hscsdParams.iAsymmetry = DMMTSY_CALL_HCSD_ASYMMETRY;
	hscsdParams.iUserInitUpgrade = DMMTSY_CALL_HCSD_USER_IMI;

	// asynchronous
	mmCall.Dial(reqStatus, hscsdParamsPckg, iNumber);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	// Set the dyanmic HSCSD params
	mmCall.SetDynamicHscsdParams(reqStatus,hscsdParams.iWantedAiur,hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel
	mmCall.SetDynamicHscsdParams(reqStatus,hscsdParams.iWantedAiur,hscsdParams.iWantedRxTimeSlots);
	mmCall.CancelAsyncRequest(EMobileCallSetDynamicHscsdParams);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::SetDynamicHscsdParams (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::SetDynamicHscsdParams (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
	// Test Set Hscsd Params V7 within Dial
	RMobileCall::TMobileHscsdCallParamsV7 hscsdParamsV7;
	RMobileCall::TMobileHscsdCallParamsV7Pckg hscsdParamsV7Pckg(hscsdParamsV7);
	
	hscsdParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	hscsdParamsV7.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	hscsdParamsV7.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	hscsdParamsV7.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	hscsdParamsV7.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	hscsdParamsV7.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
		
	hscsdParamsV7.iService = DMMTSY_DATA_CALL_SERVICE;
	hscsdParamsV7.iSpeed = DMMTSY_DATA_CALL_SPEED;
	hscsdParamsV7.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	hscsdParamsV7.iQoS = DMMTSY_DATA_CALL_QOS;
	hscsdParamsV7.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	hscsdParamsV7.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
	hscsdParamsV7.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;

	hscsdParamsV7.iInterval = DMMTSY_CALL_INTERVAL;
	hscsdParamsV7.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	hscsdParamsV7.iWantedAiur = DMMTSY_CALL_HCSD_AIUR;
	hscsdParamsV7.iWantedRxTimeSlots = DMMTSY_CALL_HCSD_TS;
	hscsdParamsV7.iMaxTimeSlots = DMMTSY_CALL_HCSD_MAXTS;
	hscsdParamsV7.iCodings = DMMTSY_CALL_HCSD_CODING;
	hscsdParamsV7.iAsymmetry = DMMTSY_CALL_HCSD_ASYMMETRY;
	hscsdParamsV7.iUserInitUpgrade = DMMTSY_CALL_HCSD_USER_IMI;
	hscsdParamsV7.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	hscsdParamsV7.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	hscsdParamsV7.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	hscsdParamsV7.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	hscsdParamsV7.iAlphaId = DMMTSY_CALL_ALPHA_ID;
	
	// asynchronous
	mmCall.Dial(reqStatus, hscsdParamsV7Pckg, iNumber);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (HSCSDv7 Call Params) passed"), iTestCount++);

	// Test Get Hscsd Params
	TEST(mmCall.GetCallParams(retrieveHscsdParamsPckg) == KErrNone);
	TEST(retrieveHscsdParams.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV1);
	
	TEST(retrieveHscsdParams.iIdRestrict==DMMTSY_CALL_CLI_RESTRICT_SETTING1);
	TEST(retrieveHscsdParams.iCug.iExplicitInvoke==DMMTSY_CALL_CUG_INVOKE);
	TEST(retrieveHscsdParams.iCug.iCugIndex==DMMTSY_CALL_CUG_INDEX);
	TEST(retrieveHscsdParams.iCug.iSuppressOA==DMMTSY_CALL_CUG_OA);
	TEST(retrieveHscsdParams.iCug.iSuppressPrefCug==DMMTSY_CALL_CUG_PREF);
	TEST(retrieveHscsdParams.iAutoRedial==DMMTSY_CALL_AUTO_REDIAL_FLAG);
		
	ValidateHscsdParams(retrieveHscsdParams);
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCallParams (HSCSD params) passed"), iTestCount++);
	
	// Test Set Hscsd Params V8 within Dial
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams8, retrieveHscsdParams8;
	RMobileCall::TMobileHscsdCallParamsV8Pckg hscsdParamsPckg8(hscsdParams8);
	RMobileCall::TMobileHscsdCallParamsV8Pckg retrieveHscsdParamsPckg8(retrieveHscsdParams8);

	TEST(hscsdParams8.ExtensionId()== RMobileCall::KETelMobileHscsdCallParamsV8);
	TEST(hscsdParams8.iBCRepeatIndicator == RMobileCall::EBCAlternateMode);
		
	hscsdParams8.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	hscsdParams8.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	hscsdParams8.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	hscsdParams8.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	hscsdParams8.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	hscsdParams8.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
			
	hscsdParams8.iService = DMMTSY_DATA_CALL_SERVICE;
	hscsdParams8.iSpeed = DMMTSY_DATA_CALL_SPEED;
	hscsdParams8.iProtocol = DMMTSY_DATA_CALL_PROTOCOL;
	hscsdParams8.iQoS = DMMTSY_DATA_CALL_QOS;
	hscsdParams8.iRLPVersion = DMMTSY_DATA_CALL_RLP;
	hscsdParams8.iV42bisReq = DMMTSY_DATA_CALL_V42BIS;
	hscsdParams8.iUseEdge = DMMTSY_DATA_CALL_EGPRS_REQUIRED;
	
	hscsdParams8.iInterval = DMMTSY_CALL_INTERVAL;
	hscsdParams8.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	hscsdParams8.iWantedAiur = DMMTSY_CALL_HCSD_AIUR;
	hscsdParams8.iWantedRxTimeSlots = DMMTSY_CALL_HCSD_TS;
	hscsdParams8.iMaxTimeSlots = DMMTSY_CALL_HCSD_MAXTS;
	hscsdParams8.iCodings = DMMTSY_CALL_HCSD_CODING;
	hscsdParams8.iAsymmetry = DMMTSY_CALL_HCSD_ASYMMETRY;
	hscsdParams8.iUserInitUpgrade = DMMTSY_CALL_HCSD_USER_IMI;
	hscsdParams8.iBearerMode = DMMTSY_CALL_NEW_BEARER;
	hscsdParams8.iCallParamOrigin = DMMTSY_CALL_PARAM_ORIGIN;
	hscsdParams8.iIconId.iIdentifier = DMMTSY_CALL_ICON_ID_IDENTIFIER;
	hscsdParams8.iIconId.iQualifier = DMMTSY_CALL_ICON_ID_QUALIFIER;
	hscsdParams8.iAlphaId = DMMTSY_CALL_ALPHA_ID;
			
	hscsdParams8.iSubAddress = DMMTSY_CALL_SUBADDRESS;
	hscsdParams8.iBearerCap1 = DMMTSY_CALL_BEARER_CAP1;
	hscsdParams8.iBearerCap2 = DMMTSY_CALL_BEARER_CAP2;
	hscsdParams8.iBCRepeatIndicator = DMMTSY_CALL_BC_REPEAT_INDICATOR;
			
	// asynchronous
	mmCall.Dial(reqStatus, hscsdParamsPckg8, iNumber);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::Dial (HSCSDv8 Call Params) passed"), iTestCount++);
				
	// Test Get Hscsd Params
	TEST(mmCall.GetCallParams(retrieveHscsdParamsPckg8) == KErrNone);
	TEST(retrieveHscsdParams8.ExtensionId()==RMobileCall::KETelMobileHscsdCallParamsV8);

	TEST(retrieveHscsdParams8.iIdRestrict==DMMTSY_CALL_CLI_RESTRICT_SETTING1);
	TEST(retrieveHscsdParams8.iCug.iExplicitInvoke==DMMTSY_CALL_CUG_INVOKE);
	TEST(retrieveHscsdParams8.iCug.iCugIndex==DMMTSY_CALL_CUG_INDEX);
	TEST(retrieveHscsdParams8.iCug.iSuppressOA==DMMTSY_CALL_CUG_OA);
	TEST(retrieveHscsdParams8.iCug.iSuppressPrefCug==DMMTSY_CALL_CUG_PREF);
	TEST(retrieveHscsdParams8.iAutoRedial==DMMTSY_CALL_AUTO_REDIAL_FLAG);
	
	ValidateHscsdParams(retrieveHscsdParams8);
	TEST(retrieveHscsdParams8.iSubAddress == DMMTSY_CALL_SUBADDRESS);
	TEST(retrieveHscsdParams8.iBearerCap1 == DMMTSY_CALL_BEARER_CAP1);
	TEST(retrieveHscsdParams8.iBearerCap2 == DMMTSY_CALL_BEARER_CAP2);
	TEST(retrieveHscsdParams8.iBCRepeatIndicator == DMMTSY_CALL_BC_REPEAT_INDICATOR);
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCallParams (HSCSD params) passed"), iTestCount++);
				
	// Test Get HSCSD Call Information with version 1
	RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo, asyncHscsdInfo;
	RMobileCall::TMobileCallHscsdInfoV1Pckg hscsdInfoPckg(hscsdInfo);
	RMobileCall::TMobileCallHscsdInfoV1Pckg asyncHscsdInfoPckg(asyncHscsdInfo);

	TEST(mmCall.GetCurrentHscsdInfo(hscsdInfoPckg) == KErrNone);
	ValidateHscsdInfo(hscsdInfo);

	// Test Get HSCSD Call Information with version 7
	RMobileCall::TMobileCallHscsdInfoV7 hscsdInfoV7;
	RMobileCall::TMobileCallHscsdInfoV7Pckg hscsdInfoV7Pckg(hscsdInfoV7);
	TEST(mmCall.GetCurrentHscsdInfo(hscsdInfoV7Pckg) == KErrNone);
	
	ValidateHscsdInfo(hscsdInfoV7);
	TEST(hscsdInfoV7.iCallParamOrigin == DMMTSY_CALL_PARAM_ORIGIN);
	TEST(hscsdInfoV7.iIconId.iQualifier == DMMTSY_CALL_ICON_ID_QUALIFIER);
	TEST(hscsdInfoV7.iIconId.iIdentifier == DMMTSY_CALL_ICON_ID_IDENTIFIER);
	TEST(hscsdInfoV7.iAlphaId == DMMTSY_CALL_ALPHA_ID);
	TEST(hscsdInfoV7.iParamsCallControlModified == DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED);
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCurrentHscsdInfo (sync) passed"), iTestCount++);

	// Test Get HSCSD Call Information with version 8
	RMobileCall::TMobileCallHscsdInfoV8 hscsdInfoV8;
	RMobileCall::TMobileCallHscsdInfoV8Pckg hscsdInfoV8Pckg(hscsdInfoV8);
	TEST(mmCall.GetCurrentHscsdInfo(hscsdInfoV8Pckg) == KErrNone);
	
	ValidateHscsdInfo(hscsdInfoV8);
	TEST(hscsdInfoV8.iCallParamOrigin == DMMTSY_CALL_PARAM_ORIGIN);
	TEST(hscsdInfoV8.iIconId.iQualifier == DMMTSY_CALL_ICON_ID_QUALIFIER);
	TEST(hscsdInfoV8.iIconId.iIdentifier == DMMTSY_CALL_ICON_ID_IDENTIFIER);
	TEST(hscsdInfoV8.iAlphaId == DMMTSY_CALL_ALPHA_ID);
	TEST(hscsdInfoV8.iParamsCallControlModified == DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED);
	TEST(hscsdInfoV8.iSubAddress == DMMTSY_CALL_SUBADDRESS);
	TEST(hscsdInfoV8.iBearerCap1 == DMMTSY_CALL_BEARER_CAP1);
	TEST(hscsdInfoV8.iBearerCap2 == DMMTSY_CALL_BEARER_CAP2);
	TEST(hscsdInfoV8.iBCRepeatIndicator == DMMTSY_CALL_BC_REPEAT_INDICATOR);
	
	
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetCurrentHscsdInfo (sync) passed"), iTestCount++);

	
	// Test Notify Hscsd Info Change
	// asynchronous
	mmCall.NotifyHscsdInfoChange(reqStatus,asyncHscsdInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateHscsdInfo(asyncHscsdInfo);

	// asynchronous & cancel
	mmCall.NotifyHscsdInfoChange(reqStatus,asyncHscsdInfoPckg);
	mmCall.CancelAsyncRequest(EMobileCallNotifyHscsdInfoChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyHscsdInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		ValidateHscsdInfo(asyncHscsdInfo);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyHscsdInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	// Close Call
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

CTestCallb::CTestCallb()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCall-2nd-part"));
	}

enum TVerdict CTestCallb::doTestStepL()
//
// Test Call Functions
//
    {
	iTestCount =1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Call Functionality"));
//	LOGTEXT(_L8("Test Mobile Call Functionality"));
	
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

	TRequestStatus reqStatus;

   	// Test DialNoFdnCheck (Overloaded)
	
	// Provide stndard multimode call parameter package for DialNoFdnCheck

	RMobileCall::TMobileCallParamsV1  mmCallParams;

	mmCallParams.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;

//	test.Next(_L("OK: RMobileCall's HSCSD Call Info"));

	// Test Voice Fallback for Multimedia Calls

	TName falbackCallName;

	// asynchronous 

	mmCall.NotifyVoiceFallback(reqStatus, falbackCallName);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(falbackCallName.Compare(DMMTSY_VOICE_FALLBACK_CALL_NAME) == 0);

	// asynchronous & cancel

	mmCall.NotifyVoiceFallback(reqStatus, falbackCallName);
	mmCall.CancelAsyncRequest(EMobileCallNotifyVoiceFallback);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyVoiceFallback (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(falbackCallName.Compare(DMMTSY_VOICE_FALLBACK_CALL_NAME) == 0);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyVoiceFallback (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
//	test.Next(_L("OK: RMobileCall's Voice Fallback"));


	// Test SwitchAlternating Data Call

	// asynchronous

	mmCall.SwitchAlternatingCall(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.SwitchAlternatingCall(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallSwitchAlternatingCall);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::SwitchAlternatingCall (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::SwitchAlternatingCall (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	
	// Test Notify Alternating Call Switch

	// asynchronous 

	mmCall.NotifyAlternatingCallSwitch(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.NotifyAlternatingCallSwitch(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallNotifyAlternatingCallSwitch);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyAlternatingCallSwitch (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyAlternatingCallSwitch (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		
//	test.Next(_L("OK: RMobileCall's Alternating Calls"));

	// Call Control caps
	
	RMobileCall::TMobileCallCapsV1 callCaps;
	RMobileCall::TMobileCallCapsV1Pckg callCapsPckg(callCaps);

	// synchronous

	TEST(mmCall.GetMobileCallCaps(callCapsPckg)==KErrNone);
	TEST(callCaps.iCallControlCaps==DMMTSY_CALL_CONTROL_CAPS1);
	TEST(callCaps.iCallEventCaps==DMMTSY_CALL_EVENT_CAPS1);
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileCallCaps (sync) passed"), iTestCount++);

	// asynchronous

	mmCall.NotifyMobileCallCapsChange(reqStatus, callCapsPckg);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(callCaps.iCallControlCaps==DMMTSY_CALL_CONTROL_CAPS2);
	TEST(callCaps.iCallEventCaps==DMMTSY_CALL_EVENT_CAPS2);

	// asynchronous & cancel

	mmCall.NotifyMobileCallCapsChange(reqStatus, callCapsPckg);
	mmCall.CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileCallCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		TEST(callCaps.iCallControlCaps==DMMTSY_CALL_CONTROL_CAPS2);
		TEST(callCaps.iCallEventCaps==DMMTSY_CALL_EVENT_CAPS2);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileCallCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Call Status

	RMobileCall::TMobileCallStatus callStatus;

	// synchronous

	TEST(KErrNone==mmCall.GetMobileCallStatus(callStatus));
	TEST(callStatus==DMMTSY_CALL_STATUS1);
	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileCallStatus (sync) passed"), iTestCount++);
	
	// asynchronous

	mmCall.NotifyMobileCallStatusChange(reqStatus,callStatus);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(callStatus==DMMTSY_CALL_STATUS2);

	// asynchronous & cancel

	mmCall.NotifyMobileCallStatusChange(reqStatus,callStatus);
	mmCall.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileCallStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(callStatus==DMMTSY_CALL_STATUS2);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyMobileCallStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Call Hold
	
	// asynchronous

	mmCall.Hold(reqStatus);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.Hold(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallHold);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::Hold (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::Hold (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Call Resume

	// asynchronous

	mmCall.Resume(reqStatus);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.Resume(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallResume);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone  || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::Resume (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::Resume (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Call Swap

	// asynchronous

	mmCall.Swap(reqStatus);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.Swap(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallSwap);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::Swap (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::Swap (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	
	// Call Deflect

	RMobilePhone::TMobileAddress deflectAddress;
	RMobileCall::TMobileCallDeflect deflectType=DMMTSY_CALL_DEFLECT_TYPE;

	deflectAddress.iTypeOfNumber = DMMTSY_PHONE_TON;
	deflectAddress.iNumberPlan = DMMTSY_PHONE_NPI;
	deflectAddress.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;

	// asynchronous

	mmCall.Deflect(reqStatus,deflectType,deflectAddress);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.Deflect(reqStatus,deflectType,deflectAddress);
	mmCall.CancelAsyncRequest(EMobileCallDeflect);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::Deflect (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::Deflect (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


	// Call Transfer

	// asynchronous

	mmCall.Transfer(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.Transfer(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallTransfer);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::Transfer (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::Transfer (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Call Go one to one

	// asynchronous

	mmCall.GoOneToOne(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.GoOneToOne(reqStatus);
	mmCall.CancelAsyncRequest(EMobileCallGoOneToOne);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::GoOneToOne (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::GoOneToOne (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// asynchronous

	RMobileCall::TMobileCallEvent event;
	mmCall.NotifyCallEvent(reqStatus, event);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(event==DMMTSY_CALL_EVENT1);

	// asynchronous & cancel

	mmCall.NotifyCallEvent(reqStatus, event);
	mmCall.CancelAsyncRequest(EMobileCallNotifyCallEvent);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyCallEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(event==DMMTSY_CALL_EVENT1);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyCallEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Get the big block of call information

	RMobileCall::TMobileCallInfoV1 syncCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg syncCallInfoPckg(syncCallInfo);
	
	// synchronous

	TEST(KErrNone==mmCall.GetMobileCallInfo(syncCallInfoPckg));
	syncCallInfo = syncCallInfoPckg();
	
	ValidateMobileCallInfo(syncCallInfo);
	
	//repeat for V3
	RMobileCall::TMobileCallInfoV3 syncCallInfoV3;
	RMobileCall::TMobileCallInfoV3Pckg syncCallInfoPckgV3(syncCallInfoV3);
	
	TEST(KErrNone==mmCall.GetMobileCallInfo(syncCallInfoPckgV3));
	ValidateMobileCallInfo(syncCallInfoV3);
	
	TEST(syncCallInfoV3.iSecurity == DMMTSY_CALL_SECURITY);

	//repeat for V7
	RMobileCall::TMobileCallInfoV7 syncCallInfoV7;
	RMobileCall::TMobileCallInfoV7Pckg syncCallInfoPckgV7(syncCallInfoV7);
	TEST(syncCallInfoV7.ExtensionId() == KEtelExtMultimodeV7);
	
	TEST(KErrNone==mmCall.GetMobileCallInfo(syncCallInfoPckgV7));
	ValidateMobileCallInfo(syncCallInfoV7);
	
	TEST(syncCallInfoV7.iSecurity == DMMTSY_CALL_SECURITY);
	TEST(syncCallInfoV7.iCallParamOrigin == DMMTSY_CALL_ORIGIN);
	TEST(syncCallInfoV7.iIconId.iQualifier  == DMMTSY_CALL_ICON_ID_QUALIFIER);
	TEST(syncCallInfoV7.iIconId.iIdentifier  == DMMTSY_CALL_ICON_ID_IDENTIFIER);	
	TEST(syncCallInfoV7.iAlphaId == DMMTSY_CALL_ALPHA_ID);

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileCallInfo (sync) passed"), iTestCount++);

	//repeat for V8
	RMobileCall::TMobileCallInfoV8 syncCallInfoV8;
	RMobileCall::TMobileCallInfoV8Pckg syncCallInfoPckgV8(syncCallInfoV8);
	TEST(syncCallInfoV8.ExtensionId() == KEtelExtMultimodeV8);
	TEST(syncCallInfoV8.iBCRepeatIndicator == RMobileCall::EBCAlternateMode);
		
	TEST(KErrNone==mmCall.GetMobileCallInfo(syncCallInfoPckgV8));
	ValidateMobileCallInfo(syncCallInfoV8);
		
	TEST(syncCallInfoV8.iSubAddress == DMMTSY_CALL_SUBADDRESS);
	TEST(syncCallInfoV8.iBearerCap1 == DMMTSY_CALL_BEARER_CAP1);
	TEST(syncCallInfoV8.iBearerCap2  == DMMTSY_CALL_BEARER_CAP2);
	TEST(syncCallInfoV8.iBCRepeatIndicator  == DMMTSY_CALL_BC_REPEAT_INDICATOR);	

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetMobileCallInfo (sync) passed"), iTestCount++);

	
	// Close Call
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

CTestCallc::CTestCallc()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCall-3rd-part"));
	}

enum TVerdict CTestCallc::doTestStepL()
//
// Test Call Functions
//
    {
	iTestCount =1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Call Functionality"));
//	LOGTEXT(_L8("Test Mobile Call Functionality"));
	
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

	TRequestStatus reqStatus;

   	// Test DialNoFdnCheck (Overloaded)

	// Provide stndard multimode call parameter package for DialNoFdnCheck

	RMobileCall::TMobileCallParamsV1  mmCallParams;
	RMobileCall::TMobileCallParamsV1Pckg mmCallParamsPckg(mmCallParams);

	mmCallParams.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;

	// Notify Call Event


	//test.next(_L("OK: RMobileCall's MobileCallControl"));

	// Notify Remote Party Info change
	
	RMobileCall::TMobileCallRemotePartyInfoV1 remoteParty;
	RMobileCall::TMobileCallRemotePartyInfoV1Pckg remotePartyPckg(remoteParty);

	// asynchronous

	mmCall.NotifyRemotePartyInfoChange(reqStatus, remotePartyPckg);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	remoteParty=remotePartyPckg();
	ValidateRemotePartyInfoChange(remoteParty);

	// asynchronous & cancel

	mmCall.NotifyRemotePartyInfoChange(reqStatus, remotePartyPckg);
	mmCall.CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyRemotePartyInfoChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		remoteParty=remotePartyPckg();
		ValidateRemotePartyInfoChange(remoteParty);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyRemotePartyInfoChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	//test.next(_L("OK: RMobileCall's MobileCallInformation"));

	// Test Privacy functional unit

	RMobilePhone::TMobilePhonePrivacy asyncPrivacy;
	
	// Test Set Privacy Privacy

	TEST(mmCall.SetPrivacy(DMMTSY_CALL_SET_PRIVACY) == KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::SetPrivacy (sync) passed"), iTestCount++);

	// Test Notify Privacy Confirmation

	// asynchronous

	mmCall.NotifyPrivacyConfirmation(reqStatus, asyncPrivacy);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(asyncPrivacy == DMMTSY_CALL_SET_PRIVACY);

	// asynchronous & cancel

	mmCall.NotifyPrivacyConfirmation(reqStatus, asyncPrivacy);
	mmCall.CancelAsyncRequest(EMobileCallNotifyPrivacyConfirmation);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrNone)
		{
		TEST(asyncPrivacy == DMMTSY_CALL_SET_PRIVACY);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyPrivacyConfirmation (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyPrivacyConfirmation (async & cancel (Cancelled Request)) passed"), iTestCount++);

	//test.next(_L("OK: RMobileCall's Privacy Settings OK"));


	// Test Set Traffic Channel Setting

	TEST(mmCall.SetTrafficChannel(DMMTSY_CALL_TCH_REQUEST) == KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::SetTrafficChannelSetting (sync) passed"), iTestCount++);
	
	// Test Notify Traffic Channel Confirmation

	// asynchronous

	RMobileCall::TMobileCallTch asyncTCHType;

	mmCall.NotifyTrafficChannelConfirmation(reqStatus, asyncTCHType);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);
	TEST(asyncTCHType == DMMTSY_CALL_TCH_SETTING);

	// asynchronous & cancel

	mmCall.NotifyTrafficChannelConfirmation(reqStatus, asyncTCHType);
	mmCall.CancelAsyncRequest(EMobileCallNotifyTrafficChannelConfirmation);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int() == KErrCancel);
	if (reqStatus.Int() == KErrNone)
		{
		TEST(asyncTCHType == DMMTSY_CALL_TCH_SETTING);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyTrafficChannelConfirmation (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyTrafficChannelConfirmation (async & cancel (Cancelled Request)) passed"), iTestCount++);

	//test.next(_L("OK: RMobileCall's Traffic Channel OK"));

	// asynchronous
	// If this test fails, ensure that the lines:
	// etel_KPriorityClientSid 0x10281806
	// etel_KLocationServicesNetworkGatewaySid 0x10281806
	// are present in \epoc32\data\epoc.ini

	TBuf<RMobileENStore::KEmergencyNumberSize> aNumber(DMMTSY_EN_STORE_AMBULANCE_NUM);
	mmCall.DialEmergencyCall(reqStatus, aNumber);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.DialEmergencyCall(reqStatus, aNumber);
	mmCall.CancelAsyncRequest(EMobileCallDialEmergencyCall);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::DialEmergencyCall (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::DialEmergencyCall (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	//test.next(_L("OK: RMobileCall's MobileCallEmergency"));

	// Test ActivateCCBS

	// asynchronous

	TInt index;
 	mmCall.ActivateCCBS(reqStatus, index);

   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.ActivateCCBS(reqStatus, index=0);
	mmCall.CancelAsyncRequest(EMobileCallActivateCCBS);
   	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::ActivateCCBS (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::ActivateCCBS (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	
	// Test RejectCCBS

	TEST(mmCall.RejectCCBS() == KErrNone);
	INFO_PRINTF2(_L("Test %d - RMobileCall::RejectCCBS (sync) passed"), iTestCount++);



	// Test Get UUS Caps

	TUint32 uusCaps;

	mmCall.GetUUSCaps(uusCaps);
	TEST(mmCall.GetUUSCaps(uusCaps) == KErrNone);
	TEST(uusCaps == DMMTSY_CALL_UUS_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobileCall::GetUUSCaps (sync) passed"), iTestCount++);

	// Test Notify UUS Caps Change

	// asynchronous

	mmCall.NotifyUUSCapsChange(reqStatus, uusCaps);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.NotifyUUSCapsChange(reqStatus, uusCaps);
	mmCall.CancelAsyncRequest(EMobileCallNotifyUUSCapsChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyUUSCapsChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(uusCaps == DMMTSY_CALL_UUS_CAPS);
		INFO_PRINTF2(_L("Test %d - RMobileCall::NotifyUUSCapsChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test Dial With UUI
	// by passing UUI info into the Dial request

	// Using multimode call parameters - mmCallParams2 & number

	RMobileCall::TMobileCallUUSRequestV1 uusRequest;

	uusRequest.iServiceReq = DMMTSY_CALL_UUI_SERVICE_REQ;
	uusRequest.iUUI= DMMTSY_CALL_UUI_INFO_DIAL;

	RMobileCall::TMobileCallUUSRequestV1Pckg uusRequestPckg(uusRequest);

	// Test Activate UUS

	// asynchronous

	mmCall.ActivateUUS(reqStatus,uusRequestPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.ActivateUUS(reqStatus,uusRequestPckg);
	mmCall.CancelAsyncRequest(EMobileCallActivateUUS);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::ActivateUUS (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::ActivateUUS (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	RMobileCall::TMobileCallUUI uuiAnswerInfo(DMMTSY_CALL_UUI_INFO_ANSWER);

	// Test AnswerIncomingCallWithUUI

	mmCallParams.iInterval = DMMTSY_CALL_INTERVAL;
	mmCallParams.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	mmCallParams.iCug.iExplicitInvoke = DMMTSY_CALL_CUG_INVOKE;
	mmCallParams.iCug.iCugIndex = DMMTSY_CALL_CUG_INDEX;
	mmCallParams.iCug.iSuppressOA = DMMTSY_CALL_CUG_OA;
	mmCallParams.iCug.iSuppressPrefCug = DMMTSY_CALL_CUG_PREF;
	mmCallParams.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;

	mmCall.AnswerIncomingCallWithUUI(reqStatus, mmCallParamsPckg, uuiAnswerInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	mmCall.AnswerIncomingCallWithUUI(reqStatus, mmCallParamsPckg, uuiAnswerInfo);
	mmCall.CancelAsyncRequest(EMobileCallAnswerWithUUI);
	User::WaitForRequest(reqStatus);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerIncomingCallWithUUI (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerIncomingCallWithUUI (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test Send UUI

	// asynchronous
	RMobileCall::TMobileCallUUI uuiInfo(DMMTSY_CALL_UUI_INFO);

	mmCall.SendUUI(reqStatus, DMMTSY_CALL_UUI_MORE_INFO, uuiInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.SendUUI(reqStatus, DMMTSY_CALL_UUI_MORE_INFO, uuiInfo);
	mmCall.CancelAsyncRequest(EMobileCallSendUUI);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::SendUUI (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::SendUUI (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Test Receive UUI

	RMobileCall::TMobileCallUUI uui;

	// asynchronous

	mmCall.ReceiveUUI(reqStatus, uui);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(uui.Compare(DMMTSY_CALL_UUI_INFO) == 0);

	// asynchronous & cancel

	mmCall.ReceiveUUI(reqStatus, uui);
	mmCall.CancelAsyncRequest(EMobileCallReceiveUUI);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::ReceiveUUI (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(uui.Compare(DMMTSY_CALL_UUI_INFO) != 0);
		INFO_PRINTF2(_L("Test %d - RMobileCall::ReceiveUUI (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Test HangUp With UUI

	// asynchronous

	TBuf<RMobileCall::KMaxUUISize> uuiHangupInfo(DMMTSY_CALL_UUI_INFO_HANGUP);

	mmCall.HangupWithUUI(reqStatus, uuiHangupInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	mmCall.HangupWithUUI(reqStatus, uuiHangupInfo);
	mmCall.CancelAsyncRequest(EMobileCallHangupWithUUI);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::HangupWithUUI (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::HangupWithUUI (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	//test.next(_L("OK: RMobileCall's MobileCallUUI"));

	// Test AnswerMultimediaCallAsVoice
	TName aName ;
	// asynchronous

	mmCall.AnswerMultimediaCallAsVoice(reqStatus, mmCallParamsPckg, aName);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(aName.Compare(DMMTSY_LINE_VOICE_NAME)==0);

	// asynchronous & cancel

	mmCall.AnswerMultimediaCallAsVoice(reqStatus,mmCallParamsPckg,aName);
	mmCall.CancelAsyncRequest(EMobileCallAnswerMultimediaCallAsVoice);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerMultimediaCallAsVoice (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerMultimediaCallAsVoice (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	//test.next(_L("OK: RMobileCall's AnswerMultimediaCallAsVoice"));
	
	// Test notification of audio tones
	RMobileCall::TAudioToneV3 audioToneV3;
	RMobileCall::TAudioToneV3Pckg audioToneV3Pckg(audioToneV3);
	
	//asynchronous start notification
	mmCall.NotifyAudioToneEvent(reqStatus, audioToneV3Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(audioToneV3.iEvent == DMMTSY_AUDIO_START_TONE);
	TEST(audioToneV3.iTone == DMMTSY_AUDIO_CONGESTION_TONE);
	TEST(audioToneV3.iPlayedLocally = DMMTSY_AUDIO_PLAYED_LOCALLY);
	
	//asynchronous stop notification
	mmCall.NotifyAudioToneEvent(reqStatus, audioToneV3Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(audioToneV3.iEvent == DMMTSY_AUDIO_STOP_TONE);
	TEST(audioToneV3.iTone == DMMTSY_AUDIO_NO_TONE);
	TEST(audioToneV3.iPlayedLocally = DMMTSY_AUDIO_PLAYED_LOCALLY);
	
	//asynchronous & cancel
	mmCall.NotifyAudioToneEvent(reqStatus, audioToneV3Pckg);
	mmCall.CancelAsyncRequest(EMobileCallNotifyAudioToneEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileCall::EMobileCallNotifyAudioToneEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileCall::EMobileCallNotifyAudioToneEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	// Close Call
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}


CTestCallISV::CTestCallISV()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCall-ISV"));
	}


enum TVerdict CTestCallISV::doTestStepL()
//
// Test Call Functions
//
    {
	iTestCount =1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Call Functionality - ISV"));
 
	// Set up parameters for the call
	RMobileCall::TEtel3rdPartyMobileCallParamsV1 callParams1;
 	callParams1.iIdRestrict = DMMTSY_CALL_CLI_RESTRICT_SETTING1;
	callParams1.iAutoRedial = DMMTSY_CALL_AUTO_REDIAL_FLAG;
	RMobileCall::TEtel3rdPartyMobileCallParamsV1Pckg callParamsPckg1(callParams1);	
	TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber(DMMTSY_PHONE_TEL_NUMBER); 	
			
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

	TRequestStatus reqStatus;
	
	// Test Dial ISV functionality 
	mmCall.DialISV(reqStatus, callParamsPckg1, telNumber);
	User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone);
   	
   	if (reqStatus.Int()==KErrNone)
		{
		INFO_PRINTF2(_L("Test %d - RMobileCall::DialISV passed"), iTestCount++);
   		}
	
  	// Test Dial ISV Cancel functionality
  	mmCall.DialISV(reqStatus, callParamsPckg1, telNumber);
  	// Attempt to cancel call immediately. 
	mmCall.CancelAsyncRequest(EMobileCallDialISV);

    User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

   	if (reqStatus.Int()==KErrCancel)
		{
   		INFO_PRINTF2(_L("Test %d - RMobileCall::DialISV Cancel Request passed"), iTestCount++);
		}
   	else
   		{
   			if (reqStatus.Int()==KErrNone)
				{
   				INFO_PRINTF2(_L("Test %d - RMobileCall::DialISV - Request Not Cancelled "), iTestCount++);
				}
   		}
   	 
  
    // Answer incoming call 
    mmCall.AnswerIncomingCallISV(reqStatus, callParamsPckg1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	if (reqStatus.Int()==KErrNone)
		{
		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerIncomingCallISV passed"), iTestCount++);
		}
	
	// Test Answer Incoming ISV Cancel functionality
  	mmCall.AnswerIncomingCallISV(reqStatus, callParamsPckg1); 
  	// Attempt to cancel call immediately. 
	mmCall.CancelAsyncRequest(EMobileCallAnswerISV);

    User::WaitForRequest(reqStatus);
   	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

   	if (reqStatus.Int()==KErrCancel)
		{
   		INFO_PRINTF2(_L("Test %d - RMobileCall::AnswerIncomingCallISV Cancel Request passed"), iTestCount++);
		}
   	else
   		{
   			if (reqStatus.Int()==KErrNone)
				{
   				INFO_PRINTF2(_L("Test %d - RMobileCall:: AnswerIncomingCallISV - Request Not Cancelled "), iTestCount++);
				}
   		}
   	 

	// Close Call
	mmCall.Close();
	mmLine.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));


	return TestStepResult();
	}


void CTestCallc::ValidateRemotePartyInfoChange(const RMobileCall::TMobileCallRemotePartyInfoV1 &aRemoteParty)
	{
	TEST(aRemoteParty.iRemoteIdStatus==DMMTSY_CALL_REMOTEPARTYSTATUS2);
	TEST(aRemoteParty.iRemoteNumber.iTelNumber.Compare(DMMTSY_CALL_EMPTY_STRING)==0);
	TEST(aRemoteParty.iDirection==DMMTSY_CALL_REMOTEPARTYDIRECTION2);
	TEST(aRemoteParty.iCallingName.Compare(DMMTSY_CALL_EMPTY_STRING)==0);
	}

void CTestCallb::ValidateMobileCallInfo(const RMobileCall::TMobileCallInfoV1 &aSyncCallInfo)
	{
	TEST(aSyncCallInfo.iCallName.Compare(DMMTSY_CALL_NAME1)==0);
	TEST(aSyncCallInfo.iLineName.Compare(DMMTSY_LINE_VOICE_NAME)==0);
	TEST(aSyncCallInfo.iService==DMMTSY_CALL_SERVICE);
	TEST(aSyncCallInfo.iStatus==DMMTSY_CALL_STATUS4);
	TEST(aSyncCallInfo.iValid==DMMTSY_CALL_VALID_FLAG);
	TEST(aSyncCallInfo.iDuration.Int()==DMMTSY_CALL_DURATION1);
	TEST(aSyncCallInfo.iCallId==DMMTSY_CALL_ID1);
	TEST(aSyncCallInfo.iRemoteParty.iRemoteIdStatus==DMMTSY_CALL_REMOTEPARTYSTATUS1);
	TEST(aSyncCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Compare(DMMTSY_CALL_REMOTEPARTYNUMBER1)==0);
	TEST(aSyncCallInfo.iRemoteParty.iDirection==DMMTSY_CALL_REMOTEPARTYDIRECTION1);
	TEST(aSyncCallInfo.iRemoteParty.iCallingName.Compare(DMMTSY_CALL_REMOTEPARTYNAME)==0);
	TEST(aSyncCallInfo.iExitCode==DMMTSY_CALL_EXIT_CODE);
	TEST(aSyncCallInfo.iEmergency==DMMTSY_CALL_EMERGENCY_FLAG);
	TEST(aSyncCallInfo.iForwarded==DMMTSY_CALL_FORWARDED_FLAG);
	TEST(aSyncCallInfo.iPrivacy==DMMTSY_CALL_PRIVACY);
	TEST(aSyncCallInfo.iTch==DMMTSY_CALL_CHANNEL);
	TEST(aSyncCallInfo.iAlternatingCall==DMMTSY_CALL_ALTERNATING);
	TEST(aSyncCallInfo.iStartTime.Year() == DMMTSY_PHONE_NWTINFO_YEAR);
	TEST(aSyncCallInfo.iStartTime.Month() == DMMTSY_PHONE_NWTINFO_MONTH);
	TEST(aSyncCallInfo.iStartTime.Day() == DMMTSY_PHONE_NWTINFO_DAY);
	TEST(aSyncCallInfo.iStartTime.Hour() == DMMTSY_PHONE_NWTINFO_HOUR);
	TEST(aSyncCallInfo.iStartTime.Minute() == DMMTSY_PHONE_NWTINFO_MINUTE);
	TEST(aSyncCallInfo.iStartTime.Second() == DMMTSY_PHONE_NWTINFO_SECOND);
	TEST(aSyncCallInfo.iStartTime.MicroSecond() == DMMTSY_PHONE_NWTINFO_MICRO);
	}

void CTestCall::ValidateCallDataCaps(const RMobileCall::TMobileCallDataCapsV1 &aDataCaps)
	{
	TEST(aDataCaps.iSpeedCaps == DMMTSY_CALL_DATA_CAPS_SPEED);
	TEST(aDataCaps.iProtocolCaps == DMMTSY_CALL_DATA_CAPS_CARRIER);
	TEST(aDataCaps.iServiceCaps == DMMTSY_CALL_DATA_CAPS_SERVICE);
	TEST(aDataCaps.iHscsdSupport == DMMTSY_CALL_DATA_CAPS_HSCSD);
	TEST(aDataCaps.iMClass == DMMTSY_CALL_DATA_CAPS_MCLASS);
	TEST(aDataCaps.iMaxRxTimeSlots == DMMTSY_CALL_DATA_CAPS_MAXRXTS);
	TEST(aDataCaps.iMaxTxTimeSlots == DMMTSY_CALL_DATA_CAPS_MAXTXTS);
	TEST(aDataCaps.iTotalRxTxTimeSlots == DMMTSY_CALL_DATA_CAPS_TOTRXTXTS);
	TEST(aDataCaps.iCodingCaps == DMMTSY_CALL_DATA_CAPS_CODING);
	TEST(aDataCaps.iAsymmetryCaps == DMMTSY_CALL_DATA_CAPS_ASYMETRY);
	TEST(aDataCaps.iUserInitUpgrade == DMMTSY_CALL_DATA_CAPS_USERIMI);
	}

void CTestCall::ValidateRLPRange(const RMobileCall::TMobileDataRLPRangesV1 &aRLPRanges)
	{
	TEST(aRLPRanges.iIWSMax == DMMTSY_CALL_DATA_RLP_WSMAX);
	TEST(aRLPRanges.iIWSMin == DMMTSY_CALL_DATA_RLP_WSMIN);
	TEST(aRLPRanges.iMWSMax == DMMTSY_CALL_DATA_RLP_MWSMAX);
	TEST(aRLPRanges.iMWSMin == DMMTSY_CALL_DATA_RLP_MWSMIN);
	TEST(aRLPRanges.iN2Max == DMMTSY_CALL_DATA_RLP_N2MAX);
	TEST(aRLPRanges.iN2Min == DMMTSY_CALL_DATA_RLP_N2MIN);
	TEST(aRLPRanges.iT1Max == DMMTSY_CALL_DATA_RLP_T1MAX);
	TEST(aRLPRanges.iT1Min == DMMTSY_CALL_DATA_RLP_T1MIN);
	TEST(aRLPRanges.iT4Max == DMMTSY_CALL_DATA_RLP_T4MAX);
	TEST(aRLPRanges.iT4Min == DMMTSY_CALL_DATA_RLP_T4MIN);
	}

void CTestCall::ValidateHscsdParams(const RMobileCall::TMobileHscsdCallParamsV1& aHcsdParams)
	{
	TEST(aHcsdParams.iWantedAiur == DMMTSY_CALL_HCSD_AIUR);
	TEST(aHcsdParams.iWantedRxTimeSlots == DMMTSY_CALL_HCSD_TS);
	TEST(aHcsdParams.iMaxTimeSlots == DMMTSY_CALL_HCSD_MAXTS);
	TEST(aHcsdParams.iCodings == DMMTSY_CALL_HCSD_CODING);
	TEST(aHcsdParams.iAsymmetry == DMMTSY_CALL_HCSD_ASYMMETRY);
	TEST(aHcsdParams.iUserInitUpgrade == DMMTSY_CALL_HCSD_USER_IMI);
	}

void CTestCall::ValidateHscsdInfo(const RMobileCall::TMobileCallHscsdInfoV1& aHcsdInfo)
	{
	TEST(aHcsdInfo.iAiur == DMMTSY_CALL_HCSD_AIUR);
	TEST(aHcsdInfo.iRxTimeSlots == DMMTSY_CALL_HCSD_TS);
	TEST(aHcsdInfo.iTxTimeSlots == DMMTSY_CALL_HCSD_TS);
	TEST(aHcsdInfo.iCodings == DMMTSY_CALL_HCSD_CODING);
	}


