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
// EtelPacketTest_Rel5.cpp
// This contains EtelPacket TestCase Testing Release5 (RPacketService functionality testing)
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest_Rel5.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTest_Rel5::CEtelPacketTest_Rel5()
{
	// store the name of this test case
	SetTestStepName(_L("Release_5_Service"));
}

// destructor
CEtelPacketTest_Rel5::~CEtelPacketTest_Rel5()
{
}

enum TVerdict CEtelPacketTest_Rel5::doTestStepL( void )
/**
 * Test step Rel5 calls the 3G Packet service (RPacketService) methods
 */
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);

	RPacketService packetService;
	ret=packetService.Open(phone);

	RPacketContext r5Context;
	TName contextName;
	ret=r5Context.OpenNewContext(packetService, contextName);
	
	TRequestStatus reqStatus, notifyStatus;
	
	//Set configuration 
	RPacketContext::TContextConfig_R5 rel5ContextConfig;
	TEST(rel5ContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckgRel5(rel5ContextConfig);

	rel5ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel5ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel5ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel5ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;;
	rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
	rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
	rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	rel5ContextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	rel5ContextConfig.iPFI = DPCKTTSY_PFICONTEXT_SMS;
	rel5ContextConfig.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV42;
	rel5ContextConfig.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC1144;

	packetService.SetDefaultContextParams(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	packetService.GetDefaultContextParams(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(rel5ContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT2);
	TEST(rel5ContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS2);
	TEST(rel5ContextConfig.iPdpType==DPCKTTSY_PDP_TYPE2);
	TEST(rel5ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED);
	TEST(rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername==DPCKTTSY_USER_NAME1);
	TEST(rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword==DPCKTTSY_PASSWORD1);
	TEST(rel5ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol==DPCKTTSY_PROTOCOL_TYPE1_0);
	TEST(rel5ContextConfig.iNWIContext==DPCKTTSY_NWICONTEXT_TRUE);
	TEST(rel5ContextConfig.iPFI==DPCKTTSY_PFICONTEXT_SMS);
	TEST(rel5ContextConfig.iPdpDataCompression==DPCKTTSY_DATACOMPRESSION_EV44);
	TEST(rel5ContextConfig.iPdpHeaderCompression==DPCKTTSY_HEADERCOMPRESSION_ERFC2507);	

	packetService.NotifyContextActivationRequested(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	packetService.NotifyContextActivationRequested(reqStatus, contextConfigPckgRel5);
	packetService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	//notify
	RPacketContext::TContextConfig_R5 rel5NotifyContextConfig;
	TPckg<RPacketContext::TContextConfig_R5> contextNotifyConfigPckgRel5(rel5NotifyContextConfig);
	
	r5Context.NotifyConfigChanged(notifyStatus, contextNotifyConfigPckgRel5);
	User::WaitForRequest(notifyStatus);
	TEST(notifyStatus.Int()==KErrNone);
	TEST(rel5NotifyContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TEST(rel5NotifyContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TEST(rel5NotifyContextConfig.iPdpType==DPCKTTSY_PDP_TYPE1);
	TEST(rel5NotifyContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED);
	TEST(rel5NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iUsername==DPCKTTSY_USER_NAME1);
	TEST(rel5NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iPassword==DPCKTTSY_PASSWORD1);
	TEST(rel5NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol==DPCKTTSY_PROTOCOL_TYPE1_0);
	TEST(rel5NotifyContextConfig.iNWIContext==DPCKTTSY_NWICONTEXT_TRUE);
	TEST(rel5NotifyContextConfig.iPFI==DPCKTTSY_PFICONTEXT_SIG);
	TEST(rel5NotifyContextConfig.iPdpDataCompression==DPCKTTSY_DATACOMPRESSION_EV42);
	TEST(rel5NotifyContextConfig.iPdpHeaderCompression==DPCKTTSY_HEADERCOMPRESSION_ERFC1144);
	
	//notify cancel
	r5Context.NotifyConfigChanged(notifyStatus, contextConfigPckgRel5);
	r5Context.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
	User::WaitForRequest(notifyStatus);
	TEST(notifyStatus.Int()==KErrCancel);
	
	//setconfig
	rel5ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel5ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel5ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel5ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	rel5ContextConfig.iPdpDataCompression = DPCKTTSY_DATACOMPRESSION_EV42;
	rel5ContextConfig.iPdpHeaderCompression = DPCKTTSY_HEADERCOMPRESSION_ERFC1144;
	
	r5Context.SetConfig(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	//setconfig cancel
	r5Context.SetConfig(reqStatus, contextConfigPckgRel5);
	r5Context.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	// GetConfig
	r5Context.GetConfig(reqStatus, contextConfigPckgRel5);
	User::WaitForRequest(reqStatus);

	TEST(rel5ContextConfig.iAccessPointName == DPCKTTSY_ACCESS_POINT2);
	TEST(rel5ContextConfig.iPdpAddress == DPCKTTSY_PDP_ADDRESS2);
	TEST(rel5ContextConfig.iPdpType == DPCKTTSY_PDP_TYPE2);
	TEST(rel5ContextConfig.iPdpDataCompression == DPCKTTSY_DATACOMPRESSION_EV44);
	TEST(rel5ContextConfig.iPdpHeaderCompression == DPCKTTSY_HEADERCOMPRESSION_ERFC2507);
	
	//cancel
	r5Context.GetConfig(reqStatus, contextConfigPckgRel5);
	r5Context.CancelAsyncRequest(EPacketContextGetConfig);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	r5Context.Close();
	packetService.Close();
	phone.Close();

	return TestStepResult();
}
