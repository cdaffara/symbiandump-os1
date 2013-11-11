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
// EtelPacketTest_Rel99.cpp
// This contains EtelPacket TestCase Testing Release99 (RPacketService functionality testing)
// Symbian OS includes
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest_Rel99.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTest_Rel99::CEtelPacketTest_Rel99()
{
	// store the name of this test case
	SetTestStepName(_L("Release_99_Service"));
}

// destructor
CEtelPacketTest_Rel99::~CEtelPacketTest_Rel99()
{
}

enum TVerdict CEtelPacketTest_Rel99::doTestStepL( void )
/**
 * Test step Rel99 calls the 3G Packet service (RPacketService) methods
 */
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);

	RPacketService packetService;
	ret=packetService.Open(phone);

	RPacketContext r99Context;
	TName contextName;
	ret=r99Context.OpenNewContext(packetService, contextName);
	
	TRequestStatus reqStatus, notifyStatus;
	
	//Set configuration 
	RPacketContext::TContextConfigR99_R4 rel99ContextConfig;
	TEST(rel99ContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckgRel99(rel99ContextConfig);

	rel99ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel99ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel99ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
	rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	rel99ContextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	rel99ContextConfig.iPFI = DPCKTTSY_PFICONTEXT_SMS;

	packetService.SetDefaultContextParams(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	packetService.GetDefaultContextParams(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(rel99ContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT2);
	TEST(rel99ContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS2);
	TEST(rel99ContextConfig.iPdpType==DPCKTTSY_PDP_TYPE2);
	TEST(rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iUsername==DPCKTTSY_USER_NAME1);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iPassword==DPCKTTSY_PASSWORD1);
	TEST(rel99ContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol==DPCKTTSY_PROTOCOL_TYPE1_0);
	TEST(rel99ContextConfig.iNWIContext==DPCKTTSY_NWICONTEXT_TRUE);
	TEST(rel99ContextConfig.iPFI==DPCKTTSY_PFICONTEXT_SMS);

	packetService.NotifyContextActivationRequested(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	packetService.NotifyContextActivationRequested(reqStatus, contextConfigPckgRel99);
	packetService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	//notify
	RPacketContext::TContextConfigR99_R4 rel99NotifyContextConfig;
	TPckg<RPacketContext::TContextConfigR99_R4> contextNotifyConfigPckgRel99(rel99NotifyContextConfig);
	
	r99Context.NotifyConfigChanged(notifyStatus, contextNotifyConfigPckgRel99);
	User::WaitForRequest(notifyStatus);
	TEST(notifyStatus.Int()==KErrNone);
	TEST(rel99NotifyContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TEST(rel99NotifyContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TEST(rel99NotifyContextConfig.iPdpType==DPCKTTSY_PDP_TYPE1);
	TEST(rel99NotifyContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED);
	TEST(rel99NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iUsername==DPCKTTSY_USER_NAME1);
	TEST(rel99NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iPassword==DPCKTTSY_PASSWORD1);
	TEST(rel99NotifyContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol==DPCKTTSY_PROTOCOL_TYPE1_0);
	TEST(rel99NotifyContextConfig.iNWIContext==DPCKTTSY_NWICONTEXT_TRUE);
	TEST(rel99NotifyContextConfig.iPFI==DPCKTTSY_PFICONTEXT_SIG);
	
	//notify cancel
	r99Context.NotifyConfigChanged(notifyStatus, contextConfigPckgRel99);
	r99Context.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
	User::WaitForRequest(notifyStatus);
	TEST(notifyStatus.Int()==KErrCancel);
	
	//setconfig
	rel99ContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	rel99ContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	rel99ContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	rel99ContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	
	r99Context.SetConfig(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	//setconfig cancel
	r99Context.SetConfig(reqStatus, contextConfigPckgRel99);
	r99Context.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	// GetConfig - UMTS Rel99
	r99Context.GetConfig(reqStatus, contextConfigPckgRel99);
	User::WaitForRequest(reqStatus);

	TEST(rel99ContextConfig.iAccessPointName == DPCKTTSY_ACCESS_POINT2);
	TEST(rel99ContextConfig.iPdpAddress == DPCKTTSY_PDP_ADDRESS2);
	TEST(rel99ContextConfig.iPdpType == DPCKTTSY_PDP_TYPE2);
	
	//cancel
	r99Context.GetConfig(reqStatus, contextConfigPckgRel99);
	r99Context.CancelAsyncRequest(EPacketContextGetConfig);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	r99Context.Close();
	packetService.Close();
	phone.Close();
	return TestStepResult();
}
