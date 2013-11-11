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
// Te_SimPacketNetworkNotifyConfigChangeChangeTest.cpp
// 
//

/**
 @file
*/

#include "Te_SimPacketNetworkNotifyConfigChangeTest.h"

const TInt KOneSecond=1000000;

CSimPacketNetworkNotifyConfigChangeTest::CSimPacketNetworkNotifyConfigChangeTest()
	{ 
	SetTestStepName(_L("NetworkNotifyConfigChangeTest"));
	}

TVerdict CSimPacketNetworkNotifyConfigChangeTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketNetworkNotifyConfigChangeTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(22);

	// Open phone
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	// Open packet service
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet Service object"));

	// Open context
	TName contextName;
	TName contextNameCompare;
	TESTL(iPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened a Context object"));
	
	//setup params to test setting R5 context config params
	RPacketContext::TContextConfig_R5* rel5ContextConfig = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(rel5ContextConfig);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5(*rel5ContextConfig);

	rel5ContextConfig->iAccessPointName = R5_CONTEXT_GGSN;
	rel5ContextConfig->iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	rel5ContextConfig->iPdpType = R5_CONTEXT_PROTOCOL;
	rel5ContextConfig->iUseEdge = R5_CONTEXT_USE_EDGE;
	rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	rel5ContextConfig->iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	rel5ContextConfig->iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	rel5ContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	rel5ContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	rel5ContextConfig->iProtocolConfigOption.iId = R5_CONTEXT_ID;
	rel5ContextConfig->iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	rel5ContextConfig->iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

	TBuf8<R5_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamData(R5_CONTEXT_ADDITIONAL_PARAM);	
	additionalParamData.SetLength(R5_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoId(R5_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.Zero();

	TPtr8 pcoPtr(const_cast<TUint8*>(miscBuffer.Ptr()), RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtr(const_cast<TUint8*>(additionalParamData.Ptr()),
		additionalParamData.Length(), additionalParamData.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLV(pcoPtr,0);	

	theTLV.AppendItemL(pcoId,additionalParamDataPtr);

	miscBuffer.SetLength(pcoPtr.Length());

	rel5ContextConfig->iProtocolConfigOption.iMiscBuffer = miscBuffer;

	// Test setting R5 Context Config Params
	TRequestStatus reqStatus;
	iPacketContext.SetConfig(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Test notification of change in R5 context config params
	RPacketContext::TContextConfig_R5* rel5ContextConfig_2 = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(rel5ContextConfig_2);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5_2(*rel5ContextConfig_2);

	TRequestStatus notifyStatus;
	iPacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5_2);
	
	//Activate the context
	StartNTRasSimulation();
	iPacketContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//This NotifyConfigChanged is triggered by context activation
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	
	//Repost NotifyConfigChanged request to capture network induced event
	iPacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5_2);

	//Set up the buffer which contains expected new miscbuffer
	TBuf8<R5_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamData2(R5_CONTEXT_ADDITIONAL_PARAM2);	
	additionalParamData2.SetLength(R5_CONTEXT_ADDITIONAL_PARAM_LEN2);

	RPacketContext::TPcoId pcoId2(R5_CONTEXT_PCO_ID2);

	RPacketContext::TMiscProtocolBuffer miscBuffer2;

	miscBuffer2.Zero();
	
	TPtr8 pcoPtr2(const_cast<TUint8*>(miscBuffer2.Ptr()), RPacketContext::KMiscProtocolBufferLength);

	TPtr8 additionalParamDataPtr2(const_cast<TUint8*>(additionalParamData2.Ptr()),
		additionalParamData2.Length(), additionalParamData2.Length());

	// Attach TTlv to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLV2(pcoPtr2,0);	

	theTLV2.AppendItemL(pcoId2,additionalParamDataPtr2);

	miscBuffer2.SetLength(pcoPtr2.Length());

	// Wait for the notification to complete
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	CHECKPOINTL(notifyStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.82"));

	TESTL(rel5ContextConfig_2->iAccessPointName == R5_CONTEXT_GGSN2);
	TESTL(rel5ContextConfig_2->iPdpAddress == R5_CONTEXT_PDP_ADDRESS2);
	TESTL(rel5ContextConfig_2->iPdpType == R5_CONTEXT_PROTOCOL2);
	TESTL(rel5ContextConfig_2->iUseEdge == R5_CONTEXT_USE_EDGE2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1_2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2_2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iId == R5_CONTEXT_ID2);
	TESTL(rel5ContextConfig_2->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION2);
	TESTL(rel5ContextConfig_2->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION2);
	rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer2.Length());
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer == miscBuffer2);
	INFO_PRINTF1(_L("Network Induced NotifyConfigChanged Test Passed"));

	//Repost NotifyConfigChanged request to capture next network induced event
	iPacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5_2);
		
	//Timer is set to 15Sec. Test Reducing ETimerCallBackPcktContextConfigChange timer
	SignalSimTsyToReduceTimerL(ETimerIdPcktContextConfigChange);

	// Wait for the notification to complete
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	CHECKPOINTL(notifyStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.83"));
	
	TESTL(rel5ContextConfig_2->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(rel5ContextConfig_2->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(rel5ContextConfig_2->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(rel5ContextConfig_2->iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(rel5ContextConfig_2->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(rel5ContextConfig_2->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());
	TESTL(rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer == miscBuffer);
	INFO_PRINTF1(_L("Network Induced NotifyConfigChanged - Reduced Timer. Test Passed"));
	
	//Repost NotifyConfigChanged request to capture next network induced event
	iPacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5_2);

	iPacketContext.Deactivate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	//Since we have deactived the next event should not occur
	WaitWithTimeout(notifyStatus, 10*KOneSecond);
	CHECKPOINTL(notifyStatus.Int(), KRequestPending,CHP_CNTXT_CASE("E.84"));
	
	//Cancel pending request
	iPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("Notify event timed out due to context deactivation. Test Passed"));

	
	CleanupStack::PopAndDestroy(rel5ContextConfig_2);
	CleanupStack::PopAndDestroy(rel5ContextConfig);
	
	// Close
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	TInt temp = RThread().RequestCount();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

