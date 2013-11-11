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

#include "Te_SimPacketTest.h"

CSimPacketsTest::CSimPacketsTest() 
	{ 
	SetTestStepName(_L("PacketTest"));
	}

TVerdict CSimPacketsTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(9);

	INFO_PRINTF1(_L("RPacketService Functionality"));

	// test RPacketService::Open
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	//TESTL(iPacketService.Open(iPhone)==KErrNone);
	ret = iPacketService.Open(iPhone);
	INFO_PRINTF1(_L("Opened Packet object"));
	CHECKPOINTL(ret,KErrNone,CHP_SRVS_CASE("E.1"));

	RPacketService::TRegistrationStatus regStatus;
	TRequestStatus stat0;
	TRequestStatus reqStatus;

	// test network RPacketService::GetNtwkRegStatus
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Testing Get Current Registration Status"));
	iPacketService.GetNtwkRegStatus(stat0, regStatus);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(regStatus==RPacketService::ENotRegisteredSearching);
	INFO_PRINTF1(_L("GETNTWK:  Searching for network"));

	// test RPacketService::NotifyChangeOfNtwkRegStatus
	INFO_PRINTF1(_L("Testing Registration Status Change Notification"));
	iPacketService.NotifyChangeOfNtwkRegStatus(stat0, regStatus);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(regStatus==RPacketService::ENotRegisteredSearching);
	INFO_PRINTF1(_L("NOTIFY NTWK: Registered on home network"));

	// test RPacketService::NotifyChangeOfNtwkRegStatus
	INFO_PRINTF1(_L("Testing Registration Status Change Notification"));
	iPacketService.NotifyChangeOfNtwkRegStatus(stat0, regStatus);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(regStatus==RPacketService::ERegisteredOnHomeNetwork);
	INFO_PRINTF1(_L("NOTIFY NTWK: Roaming"));

	// test RPacketService::GetStatus
	RPacketService::TStatus packetStatus;
	TRequestStatus notifyStatus;
	TESTL(iPacketService.GetStatus(packetStatus)==KErrNone);
	TESTL(packetStatus==RPacketService::EStatusUnattached);
	INFO_PRINTF1(_L("GET: Packet status is EStatusUnattached"));

	// test RPacketService::NotifyStatusChange
	iPacketService.NotifyStatusChange(notifyStatus,packetStatus);
	iPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrCancel);
	TESTL(packetStatus==RPacketService::EStatusUnattached);
	INFO_PRINTF1(_L("NOTIFY CANCEL: Packet status is EStatusUnattached"));

    // put defaultConfigGPRS on heap to avoid chkstk problem in CCover build
	RPacketContext::TContextConfigGPRS* defaultConfigGPRS = new(ELeave) RPacketContext::TContextConfigGPRS;
    CleanupStack::PushL(defaultConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPckgGPRS(*defaultConfigGPRS);

	defaultConfigGPRS->iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	defaultConfigGPRS->iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	defaultConfigGPRS->iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	defaultConfigGPRS->iPdpCompression = DPCKTTSY_COMPRESSION1;
	defaultConfigGPRS->iPdpType = DPCKTTSY_PDP_TYPE1;
	defaultConfigGPRS->iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	defaultConfigGPRS->iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	defaultConfigGPRS->iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	defaultConfigGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	defaultConfigGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	defaultConfigGPRS->iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgGPRS) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params GPRS test Passed"));

	// test RPacketService::GetDefaultContextParams
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgGPRS) == KErrNone);
	TESTL(defaultConfigGPRS->iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigGPRS->iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(defaultConfigGPRS->iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigGPRS->iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(defaultConfigGPRS->iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(defaultConfigGPRS->iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);
	INFO_PRINTF1(_L("GetDefault Params GPRS test Passed"));

	// test config UMTS RPacketService::GetDefaultContextParams
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

	TESTL(iPacketService.SetDefaultContextParams(defaultConfigPckgR99_R4) == KErrNone);
	INFO_PRINTF1(_L("SetDefault Params R99_R4 test Passed"));

	// test RPacketService::GetDefaultContextParams
	TESTL(iPacketService.GetDefaultContextParams(defaultConfigPckgR99_R4) == KErrNone);
	TESTL(defaultConfigR99_R4.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(defaultConfigR99_R4.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(defaultConfigR99_R4.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER2);
	TESTL(defaultConfigR99_R4.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID2);

	INFO_PRINTF1(_L("GetDefault Params Rel99 test Passed"));

	//Test R5 RPacketService::SetDefaultContextParams
	RPacketContext::TContextConfig_R5* defaultConfigR5 = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(defaultConfigR5);
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5(*defaultConfigR5);

	defaultConfigR5->iAccessPointName = R5_CONTEXT_GGSN;
	defaultConfigR5->iPdpAddress = R5_CONTEXT_PDP_ADDRESS;
	defaultConfigR5->iPdpType = R5_CONTEXT_PROTOCOL;
	defaultConfigR5->iUseEdge = R5_CONTEXT_USE_EDGE;
	defaultConfigR5->iProtocolConfigOption.iAuthInfo.iProtocol = R5_CONTEXT_AUTH_PROT;
	defaultConfigR5->iProtocolConfigOption.iAuthInfo.iUsername = R5_CONTEXT_USERNAME;
	defaultConfigR5->iProtocolConfigOption.iAuthInfo.iPassword = R5_CONTEXT_PASSWORD;
	defaultConfigR5->iProtocolConfigOption.iChallenge = R5_CONTEXT_CHALLENGE;
	defaultConfigR5->iProtocolConfigOption.iResponse = R5_CONTEXT_RESPONSE;
	defaultConfigR5->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = R5_CONTEXT_DNS1;
	defaultConfigR5->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = R5_CONTEXT_DNS2;
	defaultConfigR5->iProtocolConfigOption.iId = R5_CONTEXT_ID;
	defaultConfigR5->iPdpHeaderCompression = R5_CONTEXT_HEADER_COMPRESSION;
	defaultConfigR5->iPdpDataCompression = R5_CONTEXT_DATA_COMPRESSION;

	TBuf8<R5_CONTEXT_ADDITIONAL_PARAM_LEN> additionalParamData(R5_CONTEXT_ADDITIONAL_PARAM);	
	additionalParamData.SetLength(R5_CONTEXT_ADDITIONAL_PARAM_LEN);

	RPacketContext::TPcoId pcoId(R5_CONTEXT_PCO_ID);

	RPacketContext::TMiscProtocolBuffer miscBuffer;
	miscBuffer.Zero();
	
	TPtr8 pcoPtr(const_cast<TUint8*>(miscBuffer.Ptr()), 
		RPacketContext::KMiscProtocolBufferLength);	

	TPtr8 additionalParamDataPtr(const_cast<TUint8*>(additionalParamData.Ptr()),
		additionalParamData.Length(), additionalParamData.Length());

	// Attach TTlv  to the buffer
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> theTLV(pcoPtr,0);	

	theTLV.AppendItemL(pcoId,additionalParamDataPtr);

	miscBuffer.SetLength(pcoPtr.Length());

	defaultConfigR5->iProtocolConfigOption.iMiscBuffer = miscBuffer;
	ret = iPacketService.SetDefaultContextParams(defaultConfigPckgR5);	

	CHECKPOINTL(ret,KErrNone,CHP_IMS_CASE("P.1"));
	INFO_PRINTF1(_L("SetDefault Params R5 test Passed"));
	
	CleanupStack::PopAndDestroy(defaultConfigR5);

	//Test R5 RPacketService::GetDefaultContextParams
	RPacketContext::TContextConfig_R5* defaultConfigR5_2 = new(ELeave) RPacketContext::TContextConfig_R5;
	CleanupStack::PushL(defaultConfigR5_2);
	TPckg<RPacketContext::TContextConfig_R5> defaultConfigPckgR5_2(*defaultConfigR5_2);

	ret = iPacketService.GetDefaultContextParams(defaultConfigPckgR5_2);
	CHECKPOINTL(ret,KErrNone,CHP_IMS_CASE("P.2"));

	TESTL(defaultConfigR5_2->iAccessPointName == R5_CONTEXT_GGSN);
	TESTL(defaultConfigR5_2->iPdpAddress == R5_CONTEXT_PDP_ADDRESS);
	TESTL(defaultConfigR5_2->iPdpType == R5_CONTEXT_PROTOCOL);
	TESTL(defaultConfigR5_2->iUseEdge == R5_CONTEXT_USE_EDGE);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iAuthInfo.iProtocol == R5_CONTEXT_AUTH_PROT);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iAuthInfo.iUsername == R5_CONTEXT_USERNAME);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iAuthInfo.iPassword == R5_CONTEXT_PASSWORD);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iChallenge == R5_CONTEXT_CHALLENGE);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iResponse == R5_CONTEXT_RESPONSE);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iDnsAddresses.iPrimaryDns == R5_CONTEXT_DNS1);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iDnsAddresses.iSecondaryDns == R5_CONTEXT_DNS2);
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iId == R5_CONTEXT_ID);
	TESTL(defaultConfigR5_2->iPdpHeaderCompression == R5_CONTEXT_HEADER_COMPRESSION);
	TESTL(defaultConfigR5_2->iPdpDataCompression == R5_CONTEXT_DATA_COMPRESSION);
	defaultConfigR5_2->iProtocolConfigOption.iMiscBuffer.SetLength(miscBuffer.Length());
	TESTL(defaultConfigR5_2->iProtocolConfigOption.iMiscBuffer == miscBuffer);
	INFO_PRINTF1(_L("GetDefault Params Rel5 test Passed"));

	CleanupStack::PopAndDestroy(defaultConfigR5_2);

	// test RPacketService::Attach
	iPacketService.Attach(reqStatus);
	iPacketService.CancelAsyncRequest(EPacketAttach);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrCancel);
	INFO_PRINTF1(_L("Packet Attach Cancelled Successfully"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.9"));

	// test RPacketService::GetStatus
	TESTL(iPacketService.GetStatus(packetStatus)==KErrNone);
	TESTL(packetStatus==RPacketService::EStatusUnattached);
	INFO_PRINTF1(_L("GET: Packet status is EStatusUnattached"));

	// test RPacketService::NotifyStatusChange
	iPacketService.NotifyStatusChange(notifyStatus,packetStatus);

	// test RPacketService::Attach
	iPacketService.Attach(reqStatus);
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	TESTL(reqStatus==KErrNone);
	TESTL(packetStatus==RPacketService::EStatusAttached);
	INFO_PRINTF1(_L("NOTIFY: Packet status is EStatusAttached"));
	INFO_PRINTF1(_L("Packet Attached to network"));
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.8"));

	// test RPacketService::Detach
	iPacketService.Detach(reqStatus);
	iPacketService.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrCancel);
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.10"));
	INFO_PRINTF1(_L("Packet Detach Cancelled Successfully"));

	// test RPacketService::GetStatus
	TESTL(iPacketService.GetStatus(packetStatus)==KErrNone);
	TESTL(packetStatus==RPacketService::EStatusAttached);
	INFO_PRINTF1(_L("GET: Packet status is EStatusAttached 2"));

	// test RPacketService::NotifyStatusChange
	iPacketService.NotifyStatusChange(notifyStatus,packetStatus);

	// test RPacketService::Detach
	iPacketService.Detach(reqStatus);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	TESTL(packetStatus==RPacketService::EStatusUnattached);
	INFO_PRINTF1(_L("NOTIFY: Packet status is EStatusUnattached"));
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.11"));
	INFO_PRINTF1(_L("Packet Detached from network"));

	// test RPacketService::EnumerateContexts
	TInt contextCount, maxAllowed;
	iPacketService.EnumerateContexts(reqStatus,contextCount, maxAllowed);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(contextCount==0);
	TESTL(maxAllowed==11);
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.4"));
	INFO_PRINTF2(_L("Enumerate Context: %d"), contextCount);

	// test RPacketService::GetContextInfo
	TInt index = 0;
	RPacketService::TContextInfo contextInfo;
	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrArgument); // So far no context created
	CHECKPOINTL(reqStatus.Int(),KErrArgument,CHP_CNTXT_CASE("E.5"));

	// test RPacketService::GetMSClass
	RPacketService::TMSClass msClass1, msClass2;
	iPacketService.GetMSClass(reqStatus, msClass1,msClass2);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(msClass1==RPacketService::EMSClassSuspensionRequired);
	TESTL(msClass2==RPacketService::EMSClassSuspensionRequired);
	INFO_PRINTF1(_L("GetMSClass successful"));

	iPacketService.GetMSClass(reqStatus, msClass1, msClass2);
	iPacketService.CancelAsyncRequest(EPacketGetMSClass);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); //can't be cancelled in the simtsy so it GetMSClass should complete.
	INFO_PRINTF1(_L("GetMSClass Cancel does not cancel"));

	// test RPacketService::SetMSClass
	msClass1 = RPacketService::EMSClassSuspensionRequired;

	iPacketService.SetMSClass(reqStatus, RPacketService::EMSClassSuspensionRequired);
	iPacketService.CancelAsyncRequest(EPacketSetMSClass);//no implementation-does nothing
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	iPacketService.SetMSClass(reqStatus, RPacketService::EMSClassDualMode);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	INFO_PRINTF1(_L("SetMSClass successful"));

	iPacketService.SetMSClass(reqStatus, msClass1);
	iPacketService.CancelAsyncRequest(EPacketSetMSClass);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("SetMSClass Cancel does not cancel"));

	// test RPacketService::NotifyMSClassChange
	iPacketService.NotifyMSClassChange(reqStatus, msClass1);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	INFO_PRINTF1(_L("NotifyMSClass not supported successful"));

	iPacketService.NotifyMSClassChange(reqStatus, msClass1);
	iPacketService.CancelAsyncRequest(EPacketNotifyMSClassChange);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotSupported);
	INFO_PRINTF1(_L("NotifyMSClass cancel not supported successful"));

	// test RPacketService::GetStaticCaps
	TUint caps;
	TESTL(iPacketService.GetStaticCaps(caps, DPCKTTSY_PDP_TYPE1)==KErrNone);
	TESTL(caps==SIM_PACKET_SERVICE_CAPS);
	INFO_PRINTF1(_L("Get Static Caps test passed"));

	// RPacketService::GetDynamicCaps is tested by te_simpacket

	// test RPacketService::SetAttachMode
	RPacketService::TAttachMode attachMode=RPacketService::EAttachWhenNeeded;
	TESTL(iPacketService.SetAttachMode(attachMode)==KErrNone);

	// test RPacketService::GetAttachMode
	TESTL(iPacketService.GetAttachMode(attachMode)==KErrNone);
	TESTL(attachMode==RPacketService::EAttachWhenNeeded);
	
	// test RPacketService::NotifyAttachModeChange
	RPacketService::TAttachMode notifyAttachMode;
	iPacketService.NotifyAttachModeChange(reqStatus, notifyAttachMode);
	TESTL(iPacketService.SetAttachMode(attachMode)==KErrNone);
	User::WaitForRequest(reqStatus);	
	TESTL(reqStatus==KErrNone);
	TESTL(notifyAttachMode==RPacketService::EAttachWhenNeeded);	
	
	// test RPacketService::NotifyAttachModeChange & Cancel
	iPacketService.NotifyAttachModeChange(reqStatus, notifyAttachMode);
	iPacketService.CancelAsyncRequest(EPacketNotifyAttachModeChange);
	User::WaitForRequest(reqStatus);	
	TESTL(reqStatus==KErrCancel);

    CleanupStack::PopAndDestroy(1);

	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

