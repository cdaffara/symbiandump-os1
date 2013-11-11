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

/**
 @file
*/

#include "Te_SimPacketRel5ContextTest.h"

CSimPacketRel5ContextTest::CSimPacketRel5ContextTest() 
	{ 
	SetTestStepName(_L("Rel5ContextTest"));
	}

TVerdict CSimPacketRel5ContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketRel5ContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iRel5PacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	TName contextName;	
	TESTL(iRel5PacketContext.OpenNewContext(iRel5PacketService, contextName)==KErrNone);
	
	INFO_PRINTF1(_L("AddMediaAuthorization test"));
	// Test valid media authorization with valid return code
	INFO_PRINTF1(_L("Test valid media authorization, return valid return code"));
	RPacketContext::CTFTMediaAuthorizationV3* test1MediaAuthV3 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(test1MediaAuthV3);
	RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier iFlowIdentifier;
	((*test1MediaAuthV3).iAuthorizationToken).Copy(SIMTSY_AUTH_TOKEN_TEST_1);
	iFlowIdentifier.iMediaComponentNumber = SIMTSY_COMP_NUMBER1_TEST_1;
	iFlowIdentifier.iIPFlowNumber = SIMTSY_IPFLOW_NUMBER1_TEST_1;
	(*test1MediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	iFlowIdentifier.iMediaComponentNumber = SIMTSY_COMP_NUMBER2_TEST_1;
	iFlowIdentifier.iIPFlowNumber = SIMTSY_IPFLOW_NUMBER2_TEST_1;
	(*test1MediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	
	TRequestStatus reqStatus;
	iRel5PacketContext.AddMediaAuthorizationL(reqStatus, *test1MediaAuthV3);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Valid media authorization Test Passed"));

	// Test unmatched media authorization, returning KErrCorrupt
	INFO_PRINTF1(_L("Test unmatched media authorization, return invalid return code"));
	RPacketContext::CTFTMediaAuthorizationV3* test2MediaAuthV3 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(test2MediaAuthV3);

	((*test2MediaAuthV3).iAuthorizationToken).Copy(SIMTSY_AUTH_TOKEN_TEST_2);
	iFlowIdentifier.iMediaComponentNumber = SIMTSY_COMP_NUMBER1_TEST_2;
	iFlowIdentifier.iIPFlowNumber = SIMTSY_IPFLOW_NUMBER1_TEST_2;
	(*test2MediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	iFlowIdentifier.iMediaComponentNumber = SIMTSY_COMP_NUMBER2_TEST_2;
	iFlowIdentifier.iIPFlowNumber = SIMTSY_IPFLOW_NUMBER2_TEST_2;
	(*test2MediaAuthV3).iFlowIds.Append(iFlowIdentifier);
	
	iRel5PacketContext.AddMediaAuthorizationL(reqStatus, *test2MediaAuthV3);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrCorrupt);
	INFO_PRINTF1(_L("Corrupt media authorization Test Passed"));

	INFO_PRINTF1(_L("RemoveMediaAuthorization test"));	
	// Test removal of media authorization, configured to return not found
	INFO_PRINTF1(_L("Test removal media authorization, return not found"));
	iRel5PacketContext.RemoveMediaAuthorization(reqStatus, (*test2MediaAuthV3).iAuthorizationToken);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotFound);
	INFO_PRINTF1(_L("Not found remove media authorization Test Passed"));

	INFO_PRINTF1(_L("AddMediaAuthorization and RemoveMediaAuthorization cancel test"));
	// Test cancellation of AddMediaAuthorizationL
	INFO_PRINTF1(_L("Test cancellation of AddMediaAuthorizationL"));
	iRel5PacketContext.AddMediaAuthorizationL(reqStatus, *test1MediaAuthV3);
	iRel5PacketContext.CancelAsyncRequest(EPacketAddMediaAuthorization);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Cancellation of add media authorization Test Passed"));

	// Test cancellation of RemoveMediaAuthorizationL
	INFO_PRINTF1(_L("Test cancellation of RemoveMediaAuthorizationL"));
	iRel5PacketContext.RemoveMediaAuthorization(reqStatus, (*test2MediaAuthV3).iAuthorizationToken);
	iRel5PacketContext.CancelAsyncRequest(EPacketRemoveMediaAuthorization);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotFound);
	INFO_PRINTF1(_L("Cancellation of remove media authorization Test Passed"));

	CleanupStack::PopAndDestroy(test2MediaAuthV3);
	CleanupStack::PopAndDestroy(test1MediaAuthV3);
	


	// UMTS Rel5
	RPacketContext::TContextConfig_R5* rel5ContextConfig = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(rel5ContextConfig);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5(*rel5ContextConfig);

	TRequestStatus notifyStatus;

	//Test notification of change in R5 context config params
	iRel5PacketContext.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5);

	RPacketContext::TContextConfig_R5* rel5ContextConfig_2 = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(rel5ContextConfig_2);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5_2(*rel5ContextConfig_2);

	//setup params to test setting R5 context config params
	rel5ContextConfig_2->iAccessPointName = R5_CONTEXT_GGSN;
	rel5ContextConfig_2->iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	rel5ContextConfig_2->iPdpType = R5_CONTEXT_PROTOCOL;
	rel5ContextConfig_2->iUseEdge = R5_CONTEXT_USE_EDGE;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	rel5ContextConfig_2->iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	rel5ContextConfig_2->iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	rel5ContextConfig_2->iProtocolConfigOption.iId = R5_CONTEXT_ID;
	rel5ContextConfig_2->iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	rel5ContextConfig_2->iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

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

	rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer = miscBuffer;

	// Test setting R5 Context Config Params
	iRel5PacketContext.SetConfig(reqStatus, contextConfigPckgRel5_2);

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.4"));
	INFO_PRINTF1(_L("SetConfig Rel5 test passed"));

	// Wait for the notification to complete
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	CHECKPOINTL(notifyStatus.Int(),KErrNone,CHP_IMS_CASE("P.3"));

	TESTL(rel5ContextConfig->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(rel5ContextConfig->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(rel5ContextConfig->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(rel5ContextConfig->iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(rel5ContextConfig->iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(rel5ContextConfig->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(rel5ContextConfig->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	rel5ContextConfig->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());
	TESTL(rel5ContextConfig->iProtocolConfigOption.iMiscBuffer == miscBuffer);
	INFO_PRINTF1(_L("NotifyConfigChanged Rel5 Test Passed"));


	//Test setting an R5 Config Param and placing GPRS, R4 and R5 Config param change notify requests
	//Set R5 Notify
	TRequestStatus rel5Status;
	RPacketContext::TContextConfig_R5* rel5ContextConfig_3 = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(rel5ContextConfig_3);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5_3(*rel5ContextConfig_3);
	iRel5PacketContext.NotifyConfigChanged(rel5Status, contextConfigPckgRel5_3);

	//Set GPRS Notify
	TRequestStatus gprsStatus;
	RPacketContext::TContextConfigGPRS* gprsContextConfig = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(gprsContextConfig);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(*gprsContextConfig);

	RPacketContext iRel5PacketContext2;
	TESTL(iRel5PacketContext2.OpenExistingContext(iRel5PacketService, contextName)==KErrNone);
	iRel5PacketContext2.NotifyConfigChanged(gprsStatus, contextConfigPckgGPRS);

	//Set Rel99/R4 Notify
	TRequestStatus rel99Status;	
	RPacketContext::TContextConfigR99_R4* rel99ContextConfig = new(ELeave) RPacketContext::TContextConfigR99_R4;
	CleanupStack::PushL(rel99ContextConfig);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgRel99(*rel99ContextConfig);

	RPacketContext iRel5PacketContext3;
	TESTL(iRel5PacketContext3.OpenExistingContext(iRel5PacketService, contextName)==KErrNone);
	iRel5PacketContext3.NotifyConfigChanged(rel99Status, contextConfigPckgRel99);

	//setup params to test setting R5 context config params
	rel5ContextConfig_2->iAccessPointName = R5_CONTEXT_GGSN2;
	rel5ContextConfig_2->iPdpAddress = R5_CONTEXT_PDP_ADDRESS2;
	rel5ContextConfig_2->iPdpType = R5_CONTEXT_PROTOCOL2;
	rel5ContextConfig_2->iUseEdge = R5_CONTEXT_USE_EDGE2;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT2;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME2;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD2;
	rel5ContextConfig_2->iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE2;
	rel5ContextConfig_2->iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE2;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1_2;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2_2;
	rel5ContextConfig_2->iProtocolConfigOption.iId = R5_CONTEXT_ID2;
	rel5ContextConfig_2->iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION2;
	rel5ContextConfig_2->iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION2;

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

	rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer = miscBuffer2;

	//set R5 Context Config Params
	iRel5PacketContext.SetConfig(reqStatus, contextConfigPckgRel5_2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Wait for the notifications to complete
	User::WaitForRequest(gprsStatus);
	CHECKPOINTL(gprsStatus.Int(),KErrNone,CHP_IMS_CASE("P.5"));

	User::WaitForRequest(rel99Status);
	CHECKPOINTL(rel99Status.Int(),KErrNone,CHP_IMS_CASE("P.6"));

	User::WaitForRequest(rel5Status);
	TESTL(rel5Status.Int()==KErrNone);

	//check that the GPRS notify values are correct
	TESTL(gprsContextConfig->iAccessPointName == R5_CONTEXT_GGSN2);
	TESTL(gprsContextConfig->iAnonymousAccessReqd == RPacketContext::ENotApplicable);
	TESTL(gprsContextConfig->iPdpAddress == R5_CONTEXT_PDP_ADDRESS2);
	TESTL(gprsContextConfig->iPdpCompression == RPacketContext::KPdpHeaderCompression);
	TESTL(gprsContextConfig->iPdpType == R5_CONTEXT_PROTOCOL2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1_2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2_2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iId == R5_CONTEXT_ID2);	

	//check that the Rel99 notify values are correct
	TESTL(rel99ContextConfig->iAccessPointName == R5_CONTEXT_GGSN2);
	TESTL(rel99ContextConfig->iPdpAddress == R5_CONTEXT_PDP_ADDRESS2);
	TESTL(rel99ContextConfig->iPdpType == R5_CONTEXT_PROTOCOL2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1_2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2_2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iId == R5_CONTEXT_ID2);


	//check that the Rel5 notify values are correct
	TESTL(rel5ContextConfig_3->iAccessPointName == R5_CONTEXT_GGSN2);
	TESTL(rel5ContextConfig_3->iPdpAddress == R5_CONTEXT_PDP_ADDRESS2);
	TESTL(rel5ContextConfig_3->iPdpType == R5_CONTEXT_PROTOCOL2);
	TESTL(rel5ContextConfig_3->iUseEdge == R5_CONTEXT_USE_EDGE2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1_2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2_2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iId == R5_CONTEXT_ID2);
	TESTL(rel5ContextConfig_3->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION2);
	TESTL(rel5ContextConfig_3->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION2);
	rel5ContextConfig_3->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer2.Length());
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iMiscBuffer == miscBuffer2);
	INFO_PRINTF1(_L("NotifyConfigChanged Rel5 Test Passed"));


	//Test cancelling the async Setconfig request
	iRel5PacketContext.SetConfig(reqStatus, contextConfigPckgRel5_2);
	iRel5PacketContext.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.7")); //not supporting cancel of setconfig umts
	INFO_PRINTF1(_L("SetConfig Cancel Rel5 test passed"));


	//setup params to test setting R5 context config params
	rel5ContextConfig_2->iAccessPointName = R5_CONTEXT_GGSN;
	rel5ContextConfig_2->iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	rel5ContextConfig_2->iPdpType = R5_CONTEXT_PROTOCOL;
	rel5ContextConfig_2->iUseEdge = R5_CONTEXT_USE_EDGE;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	rel5ContextConfig_2->iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	rel5ContextConfig_2->iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	rel5ContextConfig_2->iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	rel5ContextConfig_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	rel5ContextConfig_2->iProtocolConfigOption.iId = R5_CONTEXT_ID;
	rel5ContextConfig_2->iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	rel5ContextConfig_2->iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;
	rel5ContextConfig_2->iProtocolConfigOption.iMiscBuffer = miscBuffer;

	//Set R5 Context Config Params
	iRel5PacketContext.SetConfig(reqStatus, contextConfigPckgRel5_2);

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Test Getting GPRS Context Config Params when the Params set were R5
	iRel5PacketContext2.GetConfig(gprsStatus, contextConfigPckgGPRS);
	User::WaitForRequest(gprsStatus);
	CHECKPOINTL(gprsStatus.Int(),KErrNone,CHP_IMS_CASE("P.8"));

	TESTL(gprsContextConfig->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(gprsContextConfig->iAnonymousAccessReqd == RPacketContext::ENotApplicable);
	TESTL(gprsContextConfig->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(gprsContextConfig->iPdpCompression == 
			(RPacketContext::KPdpHeaderCompression | RPacketContext::KPdpDataCompression));
	TESTL(gprsContextConfig->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(gprsContextConfig->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(gprsContextConfig->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(gprsContextConfig->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(gprsContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(gprsContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(gprsContextConfig->iProtocolConfigOption.iId == R5_CONTEXT_ID);

	//Test Getting R99 Context Config Params when the Params set were R5
	iRel5PacketContext3.GetConfig(rel99Status, contextConfigPckgRel99);
	User::WaitForRequest(rel99Status);
	CHECKPOINTL(rel99Status.Int(),KErrNone,CHP_IMS_CASE("P.9"));

	TESTL(rel99ContextConfig->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(rel99ContextConfig->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(rel99ContextConfig->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(rel99ContextConfig->iProtocolConfigOption.iId == R5_CONTEXT_ID);

	//Test Getting R5 Conext Config Params when the Params set were R5
	iRel5PacketContext.GetConfig(rel5Status, contextConfigPckgRel5_3);
	User::WaitForRequest(rel5Status);
	CHECKPOINTL(rel5Status.Int(),KErrNone,CHP_IMS_CASE("P.10"));

	TESTL(rel5ContextConfig_3->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(rel5ContextConfig_3->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(rel5ContextConfig_3->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(rel5ContextConfig_3->iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(rel5ContextConfig_3->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(rel5ContextConfig_3->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	rel5ContextConfig_3->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());
	TESTL(rel5ContextConfig_3->iProtocolConfigOption.iMiscBuffer == miscBuffer);

	INFO_PRINTF1(_L("GetConfig R5 test passed"));

	CleanupStack::PopAndDestroy(rel99ContextConfig);
	CleanupStack::PopAndDestroy(gprsContextConfig);
	CleanupStack::PopAndDestroy(rel5ContextConfig_3);
	CleanupStack::PopAndDestroy(rel5ContextConfig_2);
	CleanupStack::PopAndDestroy(rel5ContextConfig);

	// Now test the getting of DNS server names
	RPacketContext::TDnsInfoV2 dnsInfoV2;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoV2pckg(dnsInfoV2);

	iRel5PacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	User::WaitForRequest(reqStatus); 
	
	TESTL(dnsInfoV2.iPrimaryDns==R5_CONTEXT_DNS1);
	TESTL(dnsInfoV2.iSecondaryDns==R5_CONTEXT_DNS2);

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_IMS_CASE("P.11"));
	INFO_PRINTF1(_L("GetDnsInfo Test Passed")); 	

	iRel5PacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	iRel5PacketContext.CancelAsyncRequest(EPacketContextGetDNSInfo);
	User::WaitForRequest(reqStatus); 
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfoCancel Test Passed"));

	// End of Rel5PacketContext and Rel5PacketService tests
	iRel5PacketContext.Close();
	iRel5PacketContext2.Close();
	iRel5PacketContext3.Close();
	iRel5PacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

