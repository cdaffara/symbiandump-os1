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
//

/**
 @file
*/

#include "Te_SimPacketAdditionalParamsTest.h"

CSimPacketAdditionalParamsTest::CSimPacketAdditionalParamsTest()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("AdditionalParamsContextTest"));
	}

TVerdict CSimPacketAdditionalParamsTest::doTestStepL()
/**
The test step containing test cases for DEF082819 and DEF082817.
*/
	{
	INFO_PRINTF1(_L("BeginAdditionalParamsContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(16);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// GPRS RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is an additional parameter specified
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);

	RPacketContext::TContextConfigGPRS gprsContextNotify;
	TESTL(gprsContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS(gprsContextNotify);

	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	TBuf8<GPRS_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamDataGprs_2(GPRS_PACKET_CONTEXT_ADDITIONAL_PARAM);
	additionalParamDataGprs_2.SetLength(GPRS_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoIdGprs_2(GPRS_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferGprs_2;
	miscBufferGprs_2.Zero();
	
	TPtr8 pcoPtrGprs_2(const_cast<TUint8*>(miscBufferGprs_2.Ptr()), 
			 RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrGprs_2(const_cast<TUint8*>(additionalParamDataGprs_2.Ptr()),
		additionalParamDataGprs_2.Length(), additionalParamDataGprs_2.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVGprs_2(pcoPtrGprs_2,0);

	theTLVGprs_2.AppendItemL(pcoIdGprs_2,additionalParamDataPtrGprs_2);

	miscBufferGprs_2.SetLength(pcoPtrGprs_2.Length());

	gprsContextConfig.iProtocolConfigOption.iMiscBuffer = miscBufferGprs_2;

	iPacketContext.NotifyConfigChanged(notifyStatus, contextNotifyPckgGPRS);
	iPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig GPRS with Additional Parameter Test Passed"));

	TESTL(gprsContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs_2.Length());
	TESTL(gprsContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferGprs_2);
	
	INFO_PRINTF1(_L("NotifyConfigChanged GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.52"));
	
	// Test GPRS RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigGPRS gprsContextConfig_2;
	TESTL(gprsContextConfig_2.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS_2(gprsContextConfig_2);
	
	iPacketContext.GetConfig(reqStatus, contextConfigPckgGPRS_2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(gprsContextConfig_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextConfig_2.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextConfig_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextConfig_2.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextConfig_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs_2.Length());
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer == miscBufferGprs_2);

	INFO_PRINTF1(_L("GetConfig GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.53"));

	
	// Clean up for next set of tests
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();

	// Test RPacketService::Open
	ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName rel4ContextName;
	TName rel4ContextNameCompare;

	TESTL(iPacketContext.OpenNewContext(iPacketService, rel4ContextName)==KErrNone);
	rel4ContextNameCompare.Append(KSimPrimaryPacketContextName);
	rel4ContextNameCompare.AppendNum(1);
	TESTL(rel4ContextName.Compare(rel4ContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened R99_R4 context object"));

	// Rel4_R99 RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is an additional parameter specified
	RPacketContext::TContextConfigR99_R4 contextConfigRel4;
	TESTL(contextConfigRel4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigRel4Pckg(contextConfigRel4);

	RPacketContext::TContextConfigR99_R4 rel4ContextNotify;
	TESTL(rel4ContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> rel4ContextNotifyPckg(rel4ContextNotify);

	contextConfigRel4.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfigRel4.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfigRel4.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfigRel4.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	contextConfigRel4.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	contextConfigRel4.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	TBuf8<R99_R4_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamDataR4_2(R99_R4_PACKET_CONTEXT_ADDITIONAL_PARAM);
	additionalParamDataR4_2.SetLength(R99_R4_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoIdR4_2(R99_R4_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR4_2;
	miscBufferR4_2.Zero();

	TPtr8 pcoPtrR4_2(const_cast<TUint8*>(miscBufferR4_2.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR4_2(const_cast<TUint8*>(additionalParamDataR4_2.Ptr()),
		additionalParamDataR4_2.Length(), additionalParamDataR4_2.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR4_2(pcoPtrR4_2,0);

	theTLVR4_2.AppendItemL(pcoIdR4_2,additionalParamDataPtrR4_2);

	miscBufferR4_2.SetLength(pcoPtrR4_2.Length());

	contextConfigRel4.iProtocolConfigOption.iMiscBuffer = miscBufferR4_2;

	iPacketContext.NotifyConfigChanged(notifyStatus, rel4ContextNotifyPckg);
	iPacketContext.SetConfig(reqStatus, contextConfigRel4Pckg);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(notifyStatus);

	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig Rel99_R4 with Additional Parameter Test Passed"));
	
	TESTL(rel4ContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(rel4ContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(rel4ContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	rel4ContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(rel4ContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	
	INFO_PRINTF1(_L("NotifyConfigChanged Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.64"));
	
	// Test R99_R4 RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigR99_R4 contextConfigR99_R4;
	TESTL(contextConfigR99_R4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigR99_R4Pckg(contextConfigR99_R4);
	
	iPacketContext.GetConfig(reqStatus, contextConfigR99_R4Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);	
	TESTL(contextConfigR99_R4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(contextConfigR99_R4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(contextConfigR99_R4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	INFO_PRINTF1(_L("GetConfig Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.65"));

	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

CSimPacketAdditionalParamsTest2::CSimPacketAdditionalParamsTest2()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("AdditionalParamsContextTest2"));
	}

TVerdict CSimPacketAdditionalParamsTest2::doTestStepL()
/**
The test step containing test cases for DEF082819 and DEF082817.
*/
	{
	INFO_PRINTF1(_L("BeginAdditionalParamsContextTest2"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(17);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// Test GPRS RPacketService::SetDefaultContextParams
	// When no additional params are specified
	RPacketContext::TContextConfigGPRS defaultConfigGPRS_3;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS_3(defaultConfigGPRS_3);

	defaultConfigGPRS_3.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigGPRS_3.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	defaultConfigGPRS_3.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigGPRS_3.iPdpCompression = DPCKTTSY_COMPRESSION1;
	defaultConfigGPRS_3.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigGPRS_3.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigGPRS_3.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	defaultConfigGPRS_3.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	defaultConfigGPRS_3.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	defaultConfigGPRS_3.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	defaultConfigGPRS_3.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	defaultConfigGPRS_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	defaultConfigGPRS_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	defaultConfigGPRS_3.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgGPRS_3) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params GPRS when no additional params specified test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.54"));

	// Test GPRS RPacketService::GetDefaultContextParams
	// when there are no additional parameters
	RPacketContext::TContextConfigGPRS defaultConfigGPRS_4;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS_4(defaultConfigGPRS_4);
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgGPRS_4) == KErrNone);
	TESTL(defaultConfigGPRS_4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigGPRS_4.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(defaultConfigGPRS_4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigGPRS_4.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(defaultConfigGPRS_4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(defaultConfigGPRS_4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	INFO_PRINTF1(_L("GetDefault Params GPRS where there are no additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.55"));

	// Test R5 RPacketService::SetDefaultContextParams
	// When no additional params are specified
	RPacketContext::TContextConfig_R5 defaultConfigR5_3;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5_3(defaultConfigR5_3);

	defaultConfigR5_3.iAccessPointName = R5_CONTEXT_GGSN;
	defaultConfigR5_3.iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	defaultConfigR5_3.iPdpType = R5_CONTEXT_PROTOCOL;
	defaultConfigR5_3.iUseEdge = R5_CONTEXT_USE_EDGE;
	defaultConfigR5_3.iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	defaultConfigR5_3.iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	defaultConfigR5_3.iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	defaultConfigR5_3.iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	defaultConfigR5_3.iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	defaultConfigR5_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	defaultConfigR5_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	defaultConfigR5_3.iProtocolConfigOption.iId = R5_CONTEXT_ID;
	defaultConfigR5_3.iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	defaultConfigR5_3.iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

	ret = iPacketService.SetDefaultContextParams(defaultConfigPckgR5_3);

	INFO_PRINTF1(_L("SetDefault Params R5 when there are no additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.56"));

	// Test R5 RPacketService::GetDefaultContextParams
	// when there are no additional params
	RPacketContext::TContextConfig_R5 defaultConfigR5_4;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5_4(defaultConfigR5_4);

	ret = iPacketService.GetDefaultContextParams(defaultConfigPckgR5_4);

	TESTL(defaultConfigR5_4.iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(defaultConfigR5_4.iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(defaultConfigR5_4.iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(defaultConfigR5_4.iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(defaultConfigR5_4.iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(defaultConfigR5_4.iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(defaultConfigR5_4.iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	INFO_PRINTF1(_L("GetDefault Params R5 with no additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.57"));

	// Test UMTS RPacketService::GetDefaultContextParams
	// when no additional parameters are specified.

	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4_3;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4_3(defaultConfigR99_R4_3);

	defaultConfigR99_R4_3.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigR99_R4_3.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigR99_R4_3.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigR99_R4_3.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	defaultConfigR99_R4_3.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgR99_R4_3) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params R99_R4 with no additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.58"));
	
	// Test RPacketService::GetDefaultContextParams
	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4_4;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4_4(defaultConfigR99_R4_4);
	
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgR99_R4_4) == KErrNone);
	TESTL(defaultConfigR99_R4_4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigR99_R4_4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigR99_R4_4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(defaultConfigR99_R4_4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);

	INFO_PRINTF1(_L("GetDefault Params R99_R4 with no additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.59"));

	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

CSimPacketAdditionalParamsTest3::CSimPacketAdditionalParamsTest3()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("AdditionalParamsContextTest3"));
	}

TVerdict CSimPacketAdditionalParamsTest3::doTestStepL()
/**
The test step containing test cases for DEF082819 and DEF082817.
*/
	{
	INFO_PRINTF1(_L("BeginAdditionalParamsContextTest3"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(17);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// GPRS RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there are no additional parameters specified
	RPacketContext::TContextConfigGPRS gprsContextConfig_3;
	TESTL(gprsContextConfig_3.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS_3(gprsContextConfig_3);

	RPacketContext::TContextConfigGPRS gprsContextNotify_3;
	TESTL(gprsContextNotify_3.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS_3(gprsContextNotify_3);

	gprsContextConfig_3.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig_3.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig_3.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig_3.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig_3.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig_3.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	gprsContextConfig_3.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig_3.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig_3.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig_3.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig_3.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig_3.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	iPacketContext.NotifyConfigChanged(notifyStatus, contextNotifyPckgGPRS_3);
	iPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS_3);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig GPRS with no Additional Parameter Test Passed"));
	
	TESTL(gprsContextNotify_3.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify_3.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify_3.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify_3.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify_3.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextNotify_3.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);

	INFO_PRINTF1(_L("NotifyConfigChanged GPRS with no Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.60"));

	// Test GPRS RPacketContext::GetConfig with no additional parameter 
	RPacketContext::TContextConfigGPRS gprsContextConfig_4;
	TESTL(gprsContextConfig_4.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS_4(gprsContextConfig_4);
	
	iPacketContext.GetConfig(reqStatus, contextConfigPckgGPRS_4);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(gprsContextConfig_4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextConfig_4.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextConfig_4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextConfig_4.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextConfig_4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextConfig_4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);

	INFO_PRINTF1(_L("GetConfig GPRS with no Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.61"));

   // Clear up
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();

	// Test RPacketService::Open
	ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName rel4ContextName;
	TName rel4ContextNameCompare;

	TESTL(iPacketContext.OpenNewContext(iPacketService, rel4ContextName)==KErrNone);
	rel4ContextNameCompare.Append(KSimPrimaryPacketContextName);
	rel4ContextNameCompare.AppendNum(1);
	TESTL(rel4ContextName.Compare(rel4ContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened R99_R4 context object"));

	// R99_R4 RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is an additional parameter specified
	RPacketContext::TContextConfigR99_R4 contextConfigR99_R4_2;
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgR99_R4_2(contextConfigR99_R4_2);

	RPacketContext::TContextConfigR99_R4 contextConfigNotifyR99_R4_2;
	TESTL(contextConfigNotifyR99_R4_2.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigNotifyPckgR99_R4_2(contextConfigNotifyR99_R4_2);

	contextConfigR99_R4_2.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfigR99_R4_2.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfigR99_R4_2.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfigR99_R4_2.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	contextConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	contextConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	contextConfigR99_R4_2.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	contextConfigR99_R4_2.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	contextConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	contextConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	contextConfigR99_R4_2.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	iPacketContext.NotifyConfigChanged(notifyStatus, contextConfigNotifyPckgR99_R4_2);
	iPacketContext.SetConfig(reqStatus, contextConfigPckgR99_R4_2);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig Rel99_R4 with no Additional Parameter Test Passed"));
	
	TESTL(contextConfigNotifyR99_R4_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(contextConfigNotifyR99_R4_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(contextConfigNotifyR99_R4_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(contextConfigNotifyR99_R4_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	INFO_PRINTF1(_L("NotifyConfigChanged Rel99_R4 with no Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.62"));

	// Test R99_R4 RPacketContext::GetConfig with no additional parameter 
	RPacketContext::TContextConfigR99_R4 contextConfigR99_R4_3;
	TESTL(contextConfigR99_R4_2.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigR99_R4Pckg_2(contextConfigR99_R4_3);
	
	iPacketContext.GetConfig(reqStatus, contextConfigR99_R4Pckg_2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);	
	TESTL(contextConfigR99_R4_3.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(contextConfigR99_R4_3.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(contextConfigR99_R4_3.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(contextConfigR99_R4_3.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	INFO_PRINTF1(_L("GetConfig Rel99_R4 with no Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.63"));

	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}
	
CSimPacketDefaultAdditionalParamsTest::CSimPacketDefaultAdditionalParamsTest()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("DefaultAdditionalParamsContextTest"));
	}

TVerdict CSimPacketDefaultAdditionalParamsTest::doTestStepL()
/**
The test step containing test cases for DEF082819 and DEF082817.
*/
	{
	INFO_PRINTF1(_L("BeginDefaultAdditionalParamsContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(16);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// Test GPRS RPacketService::SetDefaultContextParams
	// When additional params are specified
	RPacketContext::TContextConfigGPRS defaultConfigGPRS;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS(defaultConfigGPRS);

	defaultConfigGPRS.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigGPRS.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	defaultConfigGPRS.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigGPRS.iPdpCompression = DPCKTTSY_COMPRESSION1;
	defaultConfigGPRS.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigGPRS.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	defaultConfigGPRS.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	defaultConfigGPRS.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	defaultConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	defaultConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	defaultConfigGPRS.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	TBuf8<GPRS_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamDataGprs(GPRS_PACKET_CONTEXT_ADDITIONAL_PARAM);
	additionalParamDataGprs.SetLength(GPRS_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoId(GPRS_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferGprs;
	miscBufferGprs.Zero();

	TPtr8 pcoPtrGprs(const_cast<TUint8*>(miscBufferGprs.Ptr()), 
			RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrGprs(const_cast<TUint8*>(additionalParamDataGprs.Ptr()),
		additionalParamDataGprs.Length(), additionalParamDataGprs.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVGprs(pcoPtrGprs,0);

	theTLVGprs.AppendItemL(pcoId,additionalParamDataPtrGprs);

	miscBufferGprs.SetLength(pcoPtrGprs.Length());

	defaultConfigGPRS.iProtocolConfigOption.iMiscBuffer = miscBufferGprs;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgGPRS) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params GPRS when additional params specified test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.46"));

	// Test GPRS RPacketService::GetDefaultContextParams
	// when there are additional parameters
	RPacketContext::TContextConfigGPRS defaultConfigGPRS_2;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS_2(defaultConfigGPRS_2);
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgGPRS_2) == KErrNone);
	TESTL(defaultConfigGPRS_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigGPRS_2.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(defaultConfigGPRS_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigGPRS_2.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(defaultConfigGPRS_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	defaultConfigGPRS_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);
	INFO_PRINTF1(_L("GetDefault Params GPRS where there are additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.47"));

	// Test R5 RPacketService::SetDefaultContextParams
	// When additional params are specified
	RPacketContext::TContextConfig_R5 defaultConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5(defaultConfigR5);

	defaultConfigR5.iAccessPointName = R5_CONTEXT_GGSN;
	defaultConfigR5.iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	defaultConfigR5.iPdpType = R5_CONTEXT_PROTOCOL;
	defaultConfigR5.iUseEdge = R5_CONTEXT_USE_EDGE;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	defaultConfigR5.iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	defaultConfigR5.iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	defaultConfigR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	defaultConfigR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	defaultConfigR5.iProtocolConfigOption.iId = R5_CONTEXT_ID;
	defaultConfigR5.iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	defaultConfigR5.iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

	TBuf8<R5_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamDataR5(R5_CONTEXT_ADDITIONAL_PARAM);
	additionalParamDataR5.SetLength(R5_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoIdR5(R5_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR5;
	
	miscBufferR5.Zero();
	
 	TPtr8 pcoPtrR5(const_cast<TUint8*>(miscBufferR5.Ptr()), 
			 RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR5(const_cast<TUint8*>(additionalParamDataR5.Ptr()),
		additionalParamDataR5.Length(), additionalParamDataR5.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR5(pcoPtrR5,0);

	theTLVR5.AppendItemL(pcoIdR5,additionalParamDataPtrR5);

	miscBufferR5.SetLength(pcoPtrR5.Length());

	defaultConfigR5.iProtocolConfigOption.iMiscBuffer = miscBufferR5;
	ret = iPacketService.SetDefaultContextParams(defaultConfigPckgR5);

	INFO_PRINTF1(_L("SetDefault Params R5 when there are additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.48"));

	// Test R5 RPacketService::GetDefaultContextParams
	// when there are additional params
	RPacketContext::TContextConfig_R5 defaultConfigR5_2;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5_2(defaultConfigR5_2);

	ret = iPacketService.GetDefaultContextParams(defaultConfigPckgR5_2);

	TESTL(defaultConfigR5_2.iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(defaultConfigR5_2.iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(defaultConfigR5_2.iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(defaultConfigR5_2.iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(defaultConfigR5_2.iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(defaultConfigR5_2.iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	defaultConfigR5_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR5.Length());
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iMiscBuffer == miscBufferR5);
	INFO_PRINTF1(_L("GetDefault Params R5 with additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.49"));
	
	// Test UMTS RPacketService::GetDefaultContextParams
	// when an additional parameter is specified.

	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4(defaultConfigR99_R4);

	defaultConfigR99_R4.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigR99_R4.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigR99_R4.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigR99_R4.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	defaultConfigR99_R4.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	defaultConfigR99_R4.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	defaultConfigR99_R4.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	TBuf8<R99_R4_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamDataR4(R99_R4_PACKET_CONTEXT_ADDITIONAL_PARAM);
	additionalParamDataR4.SetLength(R99_R4_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoIdR4(R99_R4_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR4;
	miscBufferR4.Zero();

	TPtr8 pcoPtrR4(const_cast<TUint8*>(miscBufferR4.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR4(const_cast<TUint8*>(additionalParamDataR4.Ptr()),
		additionalParamDataR4.Length(), additionalParamDataR4.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR4(pcoPtrR4,0);

	theTLVR4.AppendItemL(pcoIdR4,additionalParamDataPtrR4);

	miscBufferR4.SetLength(pcoPtrR4.Length());

	defaultConfigR99_R4.iProtocolConfigOption.iMiscBuffer = miscBufferR4;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgR99_R4) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params R99_R4 test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.50"));

	// Test RPacketService::GetDefaultContextParams
	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4_2;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4_2(defaultConfigR99_R4_2);
	
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgR99_R4_2) == KErrNone);
	TESTL(defaultConfigR99_R4_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigR99_R4_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigR99_R4_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	defaultConfigR99_R4_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4.Length());
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iMiscBuffer == miscBufferR4);
	
	INFO_PRINTF1(_L("GetDefault Params R99_R4 test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.51"));

	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

CSimPacketBinaryAdditionalParamsTest1::CSimPacketBinaryAdditionalParamsTest1()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("BinaryAdditionalParamsContextTest1"));
	}

TVerdict CSimPacketBinaryAdditionalParamsTest1::doTestStepL()
/**
The test step containing test cases for DEF082820
*/
	{
	INFO_PRINTF1(_L("BeginBinaryAdditionalParamsContextTest1"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(18);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// GPRS RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is a mix binary and ASCII additional parameter 
	// specified
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);

	RPacketContext::TContextConfigGPRS gprsContextNotify;
	TESTL(gprsContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS(gprsContextNotify);

	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	_LIT8(KBinaryAdditionalParam1,"abc\\def\\\\");
	TBuf8<9> additionalParamDataGprs(KBinaryAdditionalParam1);

	RPacketContext::TPcoId pcoIdGprs(GPRS_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferGprs;
	miscBufferGprs.Zero();
	
	TPtr8 pcoPtrGprs(const_cast<TUint8*>(miscBufferGprs.Ptr()), 
			 RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrGprs(const_cast<TUint8*>(additionalParamDataGprs.Ptr()),
		additionalParamDataGprs.Length(), additionalParamDataGprs.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVGprs(pcoPtrGprs,0);

	theTLVGprs.AppendItemL(pcoIdGprs,additionalParamDataPtrGprs);

	miscBufferGprs.SetLength(pcoPtrGprs.Length());

	gprsContextConfig.iProtocolConfigOption.iMiscBuffer = miscBufferGprs;

	iPacketContext.NotifyConfigChanged(notifyStatus, contextNotifyPckgGPRS);
	iPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig GPRS with Additional Parameter Test Passed"));

	TESTL(gprsContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(gprsContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);
	
	INFO_PRINTF1(_L("NotifyConfigChanged GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.66"));
	
	// Test GPRS RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigGPRS gprsContextConfig_2;
	TESTL(gprsContextConfig_2.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS_2(gprsContextConfig_2);
	
	iPacketContext.GetConfig(reqStatus, contextConfigPckgGPRS_2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(gprsContextConfig_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextConfig_2.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextConfig_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextConfig_2.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextConfig_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);

	INFO_PRINTF1(_L("GetConfig GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.67"));

	// Clean up for next test case
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();

	// Test RPacketService::Open
	ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName rel4ContextName;
	TName rel4ContextNameCompare;

	TESTL(iPacketContext.OpenNewContext(iPacketService, rel4ContextName)==KErrNone);
	rel4ContextNameCompare.Append(KSimPrimaryPacketContextName);
	rel4ContextNameCompare.AppendNum(1);
	TESTL(rel4ContextName.Compare(rel4ContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened R99_R4 context object"));

	// Rel4_R99 RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is an additional  mixed binary and ASCII parameter 
	// specified
	RPacketContext::TContextConfigR99_R4 contextConfigRel4;
	TESTL(contextConfigRel4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigRel4Pckg(contextConfigRel4);

	RPacketContext::TContextConfigR99_R4 rel4ContextNotify;
	TESTL(rel4ContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> rel4ContextNotifyPckg(rel4ContextNotify);

	contextConfigRel4.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfigRel4.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfigRel4.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfigRel4.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	contextConfigRel4.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	contextConfigRel4.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	const TUint8 binaryAdditionalParam2[10] = 
			{'a','b','c',0x0A,'d','e','f',0xFF,'\\',0x00};
	
	TBuf8<9> additionalParamDataR4_2(binaryAdditionalParam2);

	RPacketContext::TPcoId pcoIdR4_2(R99_R4_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR4_2;
	miscBufferR4_2.Zero();

	TPtr8 pcoPtrR4_2(const_cast<TUint8*>(miscBufferR4_2.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR4_2(const_cast<TUint8*>(additionalParamDataR4_2.Ptr()),
		additionalParamDataR4_2.Length(), additionalParamDataR4_2.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR4_2(pcoPtrR4_2,0);

	theTLVR4_2.AppendItemL(pcoIdR4_2,additionalParamDataPtrR4_2);

	miscBufferR4_2.SetLength(pcoPtrR4_2.Length());

	contextConfigRel4.iProtocolConfigOption.iMiscBuffer = miscBufferR4_2;

	iPacketContext.NotifyConfigChanged(notifyStatus, rel4ContextNotifyPckg);
	iPacketContext.SetConfig(reqStatus, contextConfigRel4Pckg);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(notifyStatus);

	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig Rel99_R4 with Additional Parameter Test Passed"));
	
	TESTL(rel4ContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(rel4ContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(rel4ContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	rel4ContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(rel4ContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	
	INFO_PRINTF1(_L("NotifyConfigChanged Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.68"));
	
	// Test R99_R4 RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigR99_R4 contextConfigR99_R4;
	TESTL(contextConfigR99_R4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigR99_R4Pckg(contextConfigR99_R4);
	
	iPacketContext.GetConfig(reqStatus, contextConfigR99_R4Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);	
	TESTL(contextConfigR99_R4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(contextConfigR99_R4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(contextConfigR99_R4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	INFO_PRINTF1(_L("GetConfig Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.69"));

	
	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}


CSimPacketBinaryAdditionalParamsTest2::CSimPacketBinaryAdditionalParamsTest2()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("BinaryAdditionalParamsContextTest2"));
	}
	
TVerdict CSimPacketBinaryAdditionalParamsTest2::doTestStepL()
/**
The test step containing test cases for DEF082820
*/
	{
	INFO_PRINTF1(_L("BeginBinaryAdditionalParamsContextTest2"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(19);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));
	
	TRequestStatus reqStatus, notifyStatus;

	// RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is a completely binary additional parameter 
	// specified
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);

	RPacketContext::TContextConfigGPRS gprsContextNotify;
	TESTL(gprsContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS(gprsContextNotify);

	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;
	
	const TUint8 binaryAdditionalParam3[7] = 
			{0xAB,0x01,0xCD,0x02,0xEF,0x03,0x00};
	
	TBuf8<6> additionalParamDataGprs(binaryAdditionalParam3);

	RPacketContext::TPcoId pcoIdGprs(GPRS_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferGprs;
	miscBufferGprs.Zero();
	
	TPtr8 pcoPtrGprs(const_cast<TUint8*>(miscBufferGprs.Ptr()), 
			 RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrGprs(const_cast<TUint8*>(additionalParamDataGprs.Ptr()),
		additionalParamDataGprs.Length(), additionalParamDataGprs.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVGprs(pcoPtrGprs,0);

	theTLVGprs.AppendItemL(pcoIdGprs,additionalParamDataPtrGprs);

	miscBufferGprs.SetLength(pcoPtrGprs.Length());

	gprsContextConfig.iProtocolConfigOption.iMiscBuffer = miscBufferGprs;

	iPacketContext.NotifyConfigChanged(notifyStatus, contextNotifyPckgGPRS);
	iPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig GPRS with Additional Parameter Test Passed"));
	
	TESTL(gprsContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(gprsContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);
	
	INFO_PRINTF1(_L("NotifyConfigChanged GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.70"));
	
	// Test GPRS RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigGPRS gprsContextConfig_2;
	TESTL(gprsContextConfig_2.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS_2(gprsContextConfig_2);
	
	iPacketContext.GetConfig(reqStatus, contextConfigPckgGPRS_2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(gprsContextConfig_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextConfig_2.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextConfig_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextConfig_2.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextConfig_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(gprsContextConfig_2.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);

	INFO_PRINTF1(_L("GetConfig GPRS with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.71"));

	// Clean up for next test case
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();

	// Test RPacketService::Open
	ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName rel4ContextName;
	TName rel4ContextNameCompare;

	TESTL(iPacketContext.OpenNewContext(iPacketService, rel4ContextName)==KErrNone);
	rel4ContextNameCompare.Append(KSimPrimaryPacketContextName);
	rel4ContextNameCompare.AppendNum(1);
	TESTL(rel4ContextName.Compare(rel4ContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened R99_R4 context object"));

	// Rel4_R99 RPacketContext::SetConfig and
	// RPacketContext::NotifyConfigChanged
	// when there is an additional mix binary and ASCII parameter 
	// specified which only actually contains ASCII chars.
	RPacketContext::TContextConfigR99_R4 contextConfigRel4;
	TESTL(contextConfigRel4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigRel4Pckg(contextConfigRel4);

	RPacketContext::TContextConfigR99_R4 rel4ContextNotify;
	TESTL(rel4ContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> rel4ContextNotifyPckg(rel4ContextNotify);

	contextConfigRel4.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfigRel4.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfigRel4.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfigRel4.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	contextConfigRel4.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	contextConfigRel4.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	contextConfigRel4.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	contextConfigRel4.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	_LIT8(KBinaryAdditionalParam4,"abcDEFghi012345");
	
	TBuf8<15> additionalParamDataR4_2(KBinaryAdditionalParam4);

	RPacketContext::TPcoId pcoIdR4_2(R99_R4_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR4_2;
	miscBufferR4_2.Zero();

	TPtr8 pcoPtrR4_2(const_cast<TUint8*>(miscBufferR4_2.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR4_2(const_cast<TUint8*>(additionalParamDataR4_2.Ptr()),
		additionalParamDataR4_2.Length(), additionalParamDataR4_2.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR4_2(pcoPtrR4_2,0);

	theTLVR4_2.AppendItemL(pcoIdR4_2,additionalParamDataPtrR4_2);

	miscBufferR4_2.SetLength(pcoPtrR4_2.Length());

	contextConfigRel4.iProtocolConfigOption.iMiscBuffer = miscBufferR4_2;

	iPacketContext.NotifyConfigChanged(notifyStatus, rel4ContextNotifyPckg);
	iPacketContext.SetConfig(reqStatus, contextConfigRel4Pckg);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(notifyStatus);

	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("SetConfig Rel99_R4 with Additional Parameter Test Passed"));
	
	TESTL(rel4ContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(rel4ContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(rel4ContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(rel4ContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	rel4ContextNotify.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(rel4ContextNotify.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	
	INFO_PRINTF1(_L("NotifyConfigChanged Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.72"));
	
	// Test R99_R4 RPacketContext::GetConfig with additional parameter 
	RPacketContext::TContextConfigR99_R4 contextConfigR99_R4;
	TESTL(contextConfigR99_R4.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigR99_R4Pckg(contextConfigR99_R4);
	
	iPacketContext.GetConfig(reqStatus, contextConfigR99_R4Pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);	
	TESTL(contextConfigR99_R4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(contextConfigR99_R4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(contextConfigR99_R4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4_2.Length());
	TESTL(contextConfigR99_R4.iProtocolConfigOption.iMiscBuffer == miscBufferR4_2);
	INFO_PRINTF1(_L("GetConfig Rel99_R4 with Additional Parameter Test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.73"));
	
	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}	
	
CSimPacketDefaultBinaryAdditionalParamsTest::CSimPacketDefaultBinaryAdditionalParamsTest()
/**
Default constructor. Initialises the test step name.
*/
	{
	SetTestStepName(_L("DefaultBinaryAdditionalParamsContextTest"));
	}

TVerdict CSimPacketDefaultBinaryAdditionalParamsTest::doTestStepL()
/**
The test step containing test cases for DEF082820
*/
	{
	INFO_PRINTF1(_L("BeginDefaultBinaryAdditionalParamsContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(18);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// Test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened packet object"));

	// Test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	TRequestStatus reqStatus, notifyStatus;

	// Test GPRS RPacketService::SetDefaultContextParams
	// When additional mixed binary and ASCII param is specified
	// which contains backslashes at beginning and end of
	// the data item.
	RPacketContext::TContextConfigGPRS defaultConfigGPRS;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS(defaultConfigGPRS);

	defaultConfigGPRS.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigGPRS.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	defaultConfigGPRS.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigGPRS.iPdpCompression = DPCKTTSY_COMPRESSION1;
	defaultConfigGPRS.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigGPRS.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	defaultConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	defaultConfigGPRS.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	defaultConfigGPRS.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	defaultConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	defaultConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	defaultConfigGPRS.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	_LIT8(KBinaryAdditionalParam1,"\\\\PQR\\\\\\");
	TBuf8<8> additionalParamDataGprs(KBinaryAdditionalParam1);

	RPacketContext::TPcoId pcoId(GPRS_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferGprs;
	miscBufferGprs.Zero();

	TPtr8 pcoPtrGprs(const_cast<TUint8*>(miscBufferGprs.Ptr()), 
			RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrGprs(const_cast<TUint8*>(additionalParamDataGprs.Ptr()),
		additionalParamDataGprs.Length(), additionalParamDataGprs.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVGprs(pcoPtrGprs,0);

	theTLVGprs.AppendItemL(pcoId,additionalParamDataPtrGprs);

	miscBufferGprs.SetLength(pcoPtrGprs.Length());

	defaultConfigGPRS.iProtocolConfigOption.iMiscBuffer = miscBufferGprs;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgGPRS) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params GPRS when additional params specified test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.73"));

	// Test GPRS RPacketService::GetDefaultContextParams
	// when there are additional parameters
	RPacketContext::TContextConfigGPRS defaultConfigGPRS_2;
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS_2(defaultConfigGPRS_2);
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgGPRS_2) == KErrNone);
	TESTL(defaultConfigGPRS_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigGPRS_2.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(defaultConfigGPRS_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigGPRS_2.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(defaultConfigGPRS_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	defaultConfigGPRS_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferGprs.Length());
	TESTL(defaultConfigGPRS_2.iProtocolConfigOption.iMiscBuffer == miscBufferGprs);
	INFO_PRINTF1(_L("GetDefault Params GPRS where there are additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.74"));

	// Test R5 RPacketService::SetDefaultContextParams
	// When a mixed additional param item is specified
	// which contains 128 bits of binary data.
	RPacketContext::TContextConfig_R5 defaultConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5(defaultConfigR5);

	defaultConfigR5.iAccessPointName = R5_CONTEXT_GGSN;
	defaultConfigR5.iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	defaultConfigR5.iPdpType = R5_CONTEXT_PROTOCOL;
	defaultConfigR5.iUseEdge = R5_CONTEXT_USE_EDGE;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	defaultConfigR5.iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	defaultConfigR5.iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	defaultConfigR5.iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	defaultConfigR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	defaultConfigR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	defaultConfigR5.iProtocolConfigOption.iId = R5_CONTEXT_ID;
	defaultConfigR5.iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	defaultConfigR5.iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

	const TUint8 binaryDefaultAdditionalParamR5[17] =
			{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x00};
			
	TBuf8<16> additionalParamDataR5(binaryDefaultAdditionalParamR5);

	RPacketContext::TPcoId pcoIdR5(R5_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR5;
	
	miscBufferR5.Zero();
	
 	TPtr8 pcoPtrR5(const_cast<TUint8*>(miscBufferR5.Ptr()), 
			 RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR5(const_cast<TUint8*>(additionalParamDataR5.Ptr()),
		additionalParamDataR5.Length(), additionalParamDataR5.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR5(pcoPtrR5,0);

	theTLVR5.AppendItemL(pcoIdR5,additionalParamDataPtrR5);

	miscBufferR5.SetLength(pcoPtrR5.Length());

	defaultConfigR5.iProtocolConfigOption.iMiscBuffer = miscBufferR5;
	ret = iPacketService.SetDefaultContextParams(defaultConfigPckgR5);

	INFO_PRINTF1(_L("SetDefault Params R5 when there are additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.75"));

	// Test R5 RPacketService::GetDefaultContextParams
	// when there are additional params
	RPacketContext::TContextConfig_R5 defaultConfigR5_2;
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5_2(defaultConfigR5_2);

	ret = iPacketService.GetDefaultContextParams(defaultConfigPckgR5_2);

	TESTL(defaultConfigR5_2.iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(defaultConfigR5_2.iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(defaultConfigR5_2.iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(defaultConfigR5_2.iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(defaultConfigR5_2.iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(defaultConfigR5_2.iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	defaultConfigR5_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR5.Length());
	TESTL(defaultConfigR5_2.iProtocolConfigOption.iMiscBuffer == miscBufferR5);
	INFO_PRINTF1(_L("GetDefault Params R5 with additional params test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.76"));
	
	// Test UMTS RPacketService::GetDefaultContextParams
	// when an additional mixed binary and ASCII parameter is 
	// specified which contains a both binary non-printable chars
	// and back slashes

	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4(defaultConfigR99_R4);

	defaultConfigR99_R4.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigR99_R4.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigR99_R4.iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigR99_R4.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	defaultConfigR99_R4.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	defaultConfigR99_R4.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	defaultConfigR99_R4.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	const TUint8 binaryDefaultAdditionalParam[12] = 
			{'\\',0x0A,'\\',0x0B,'\\',0x0C,'\\',0x0D,'\\',0x0E,'\\',0x00};

	TBuf8<11> additionalParamDataR4(binaryDefaultAdditionalParam);

	RPacketContext::TPcoId pcoIdR4(R99_R4_PACKET_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBufferR4;
	miscBufferR4.Zero();

	TPtr8 pcoPtrR4(const_cast<TUint8*>(miscBufferR4.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtrR4(const_cast<TUint8*>(additionalParamDataR4.Ptr()),
		additionalParamDataR4.Length(), additionalParamDataR4.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLVR4(pcoPtrR4,0);

	theTLVR4.AppendItemL(pcoIdR4,additionalParamDataPtrR4);

	miscBufferR4.SetLength(pcoPtrR4.Length());

	defaultConfigR99_R4.iProtocolConfigOption.iMiscBuffer = miscBufferR4;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgR99_R4) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params R99_R4 test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.77"));

	// Test RPacketService::GetDefaultContextParams
	RPacketContext::TContextConfigR99_R4 defaultConfigR99_R4_2;
	TPckg<RPacketContext::TContextConfigR99_R4> defaultConfigPckgR99_R4_2(defaultConfigR99_R4_2);
	
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgR99_R4_2) == KErrNone);
	TESTL(defaultConfigR99_R4_2.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigR99_R4_2.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigR99_R4_2.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);
	defaultConfigR99_R4_2.iProtocolConfigOption.iMiscBuffer.SetLength(miscBufferR4.Length());
	TESTL(defaultConfigR99_R4_2.iProtocolConfigOption.iMiscBuffer == miscBufferR4);
	
	INFO_PRINTF1(_L("GetDefault Params R99_R4 test Passed"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.78"));

	// Clear up	
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}
