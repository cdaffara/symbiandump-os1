// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimPacketRel99ContextTest.h"

CSimPacketRel99ContextTest::CSimPacketRel99ContextTest() 
	{ 
	SetTestStepName(_L("Rel99ContextTest"));
	}

TVerdict CSimPacketRel99ContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketRel99ContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iRel99PacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	TRequestStatus reqStatus;
	TRequestStatus notifyStatus;

	// test RPacketContext::OpenNewContext
	TName rel99ContextName;
	TName rel99ContextNameCompare;
	TESTL(iRel99PacketContext.OpenNewContext(iRel99PacketService, rel99ContextName)==KErrNone);
	rel99ContextNameCompare.Append(KSimPrimaryPacketContextName);
	rel99ContextNameCompare.AppendNum(1);
	TESTL(rel99ContextName.Compare(rel99ContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 Context object"));

	// UMTS Rel99
	RPacketContext::TContextConfigR99_R4 rel99ContextConfig;

	TESTL(rel99ContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgRel99(rel99ContextConfig);

	rel99ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel99ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel99ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL2;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME2;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD2;
	rel99ContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE2;
	rel99ContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE2;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER2;
	rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER2;
	rel99ContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID2;

	iRel99PacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel99);
	iRel99PacketContext.SetConfig(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.14"));
	INFO_PRINTF1(_L("SetConfig Rel99 test passed"));

	TESTL(rel99ContextConfig.iAccessPointName == DPCKTTSY_ACCESS_POINT1);
	TESTL(rel99ContextConfig.iPdpAddress == DPCKTTSY_PDP_ADDRESS1);
	TESTL(rel99ContextConfig.iPdpType == DPCKTTSY_PDP_TYPE1);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);

	INFO_PRINTF1(_L("NotifyConfigChanged Rel99 Test Passed"));

	iRel99PacketContext.SetConfig(reqStatus, contextConfigPckgRel99);
	iRel99PacketContext.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);  //not supporting cancel of setconfig umts
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.17"));
	INFO_PRINTF1(_L("SetConfig Cancel Rel99 test passed"));
	// UMTS Rel99 tests finished

	// GetConfig - UMTS Rel99
	iRel99PacketContext.GetConfig(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TESTL(rel99ContextConfig.iAccessPointName == DPCKTTSY_ACCESS_POINT1);
	TESTL(rel99ContextConfig.iPdpAddress == DPCKTTSY_PDP_ADDRESS1);
	TESTL(rel99ContextConfig.iPdpType == DPCKTTSY_PDP_TYPE1);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(rel99ContextConfig.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.20"));
	INFO_PRINTF1(_L("GetConfig UMTS test passed"));

	// now test the getting of DNS server names
	RPacketContext::TDnsInfoV2 dnsInfoV2;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoV2pckg(dnsInfoV2);

	iRel99PacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	User::WaitForRequest(reqStatus); 
	
	TESTL(dnsInfoV2.iPrimaryDns==SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(dnsInfoV2.iSecondaryDns==SIM_PACKET_CONTEXT_SECONDARYSERVER2);

	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfo Test Passed")); 	

	iRel99PacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	iRel99PacketContext.CancelAsyncRequest(EPacketContextGetDNSInfo);
	User::WaitForRequest(reqStatus); 
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfoCancel Test Passed"));

	iRel99PacketContext.Close();
	iRel99PacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

