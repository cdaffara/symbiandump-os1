// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest1_1Extended.cpp
// This contains Extended EtelPacket TestCase 1.1 tests (RPacketService functionality 
// testing). These extended tests have been added to new files due to stack overflow in
// the EtelPackettest1_1 files.
// Change Request: NDOE-58VF4Q. 
// Asynchronous variants of the following methods have been created:
// SetAttachMode(), GetAttachMode(), SetDefaultContextParams, GetDefaultContextParams
// The synchronous versions of these methods are also called in this file. This is 
// essential if the dummy TSY (dtsy) code is to work properly.
// File created 10-04-02.
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest1_1Extended.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTest1_1Extended::CEtelPacketTest1_1Extended()
{
	// store the name of this test case
	SetTestStepName(_L("Extended_Service"));
}

// destructor
CEtelPacketTest1_1Extended::~CEtelPacketTest1_1Extended()
{
}


enum TVerdict CEtelPacketTest1_1Extended::doTestStepL( void )
/**
 * Test step Extended_Service tests new asynchronous Packet service (RPacketService) methods
 */
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT_EX(ret==KErrNone,CHP_OPEN_PHONE);
	RPacketService gprs;
	ret=gprs.Open(phone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.1"));
	
	TRequestStatus reqStatus;
	
// Set Attach Mode
	RPacketService::TAttachMode attachMode=DPCKTTSY_ATTACH_MODE1;
	CHECKPOINT(gprs.SetAttachMode(attachMode),KErrNone,CHP_SRVS_CASE("A.4"));

// Set Attach mode - Asynchronous variant 
	RPacketService::TAttachMode attachModeAsync=DPCKTTSY_ATTACH_MODE2;
	gprs.SetAttachMode(reqStatus, attachModeAsync);
	User::WaitForRequest(reqStatus);
	

// Cancel Set Attach mode
	gprs.SetAttachMode(reqStatus, attachModeAsync);
	gprs.CancelAsyncRequest(EPacketSetAttachMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.4B"));

// Get Attach Mode
	CHECKPOINT(gprs.GetAttachMode(attachMode),KErrNone,CHP_SRVS_CASE("A.5"));
	CHECKPOINT(attachMode,DPCKTTSY_ATTACH_MODE2,CHP_SRVS_CASE("A.5"));

// Get Attach mode - Asynchronous variant 
	gprs.GetAttachMode(reqStatus, attachModeAsync);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(), KErrNone, CHP_SRVS_CASE("A.5B"));
	CHECKPOINT(attachModeAsync,DPCKTTSY_ATTACH_MODE1,CHP_SRVS_CASE("A.5B"));

// Cancel Get Attach mode
	gprs.GetAttachMode(reqStatus, attachModeAsync);
	gprs.CancelAsyncRequest(EPacketGetAttachMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.5B"));
	
// Notify Attach mode change
	gprs.NotifyAttachModeChange(reqStatus, attachModeAsync);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.5C"));
	CHECKPOINT(attachModeAsync,DPCKTTSY_ATTACH_MODE2,CHP_SRVS_CASE("A.5C"));
	
// Notify Attach mode change & Cancel
	gprs.NotifyAttachModeChange(reqStatus, attachModeAsync);
	gprs.CancelAsyncRequest(EPacketNotifyAttachModeChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.5C"));

// Set Default Context Parameters
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg(contextConfig);

	contextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	contextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	contextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	contextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	contextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	contextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	contextConfig.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME1;
	contextConfig.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD1;
	contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	CHECKPOINT(gprs.SetDefaultContextParams(contextConfigPckg),KErrNone,CHP_SRVS_CASE("A.19"));

// Set Default Context Parameters - Asynchronous variant
	RPacketContext::TContextConfigGPRS contextConfigAsync;
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgAsync(contextConfigAsync);

	contextConfigAsync.iAccessPointName = DPCKTTSY_ACCESS_POINT2;
	contextConfigAsync.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS2;
	contextConfigAsync.iPdpAddress = DPCKTTSY_PDP_ADDRESS2;
	contextConfigAsync.iPdpCompression = DPCKTTSY_COMPRESSION2;
	contextConfigAsync.iPdpType = DPCKTTSY_PDP_TYPE2;
	contextConfigAsync.iUseEdge = DPCKTTSY_EGPRS_NOTREQUIRED;
	contextConfigAsync.iProtocolConfigOption.iAuthInfo.iUsername = DPCKTTSY_USER_NAME2;
	contextConfigAsync.iProtocolConfigOption.iAuthInfo.iPassword = DPCKTTSY_PASSWORD2;
	contextConfigAsync.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE2_0;
	contextConfigAsync.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	
	gprs.SetDefaultContextParams(reqStatus, contextConfigPckgAsync);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrNone, CHP_SRVS_CASE("A.19B"));
	
// Cancel Set Default Context Parameters
	gprs.SetDefaultContextParams(reqStatus, contextConfigPckgAsync);
	gprs.CancelAsyncRequest(EPacketSetDefaultContextParams);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.19B"));

// Get Default Context Parameters
	CHECKPOINT(gprs.GetDefaultContextParams(contextConfigPckg),KErrNone,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iAccessPointName,DPCKTTSY_ACCESS_POINT2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iAnonymousAccessReqd,DPCKTTSY_ANONYMOUS_ACCESS2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iPdpAddress,DPCKTTSY_PDP_ADDRESS2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iPdpCompression,DPCKTTSY_COMPRESSION2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iPdpType,DPCKTTSY_PDP_TYPE2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iUseEdge,DPCKTTSY_EGPRS_NOTREQUIRED,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iPassword,DPCKTTSY_PASSWORD1,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_SRVS_CASE("A.20"));

// Get Default Context Parameters - Asynchronous variant

	gprs.GetDefaultContextParams(reqStatus, contextConfigPckgAsync);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrNone, CHP_SRVS_CASE("A.20B"));

	CHECKPOINT_COMP_STR(contextConfigAsync.iAccessPointName,DPCKTTSY_ACCESS_POINT1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iAnonymousAccessReqd,DPCKTTSY_ANONYMOUS_ACCESS1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT_COMP_STR(contextConfigAsync.iPdpAddress,DPCKTTSY_PDP_ADDRESS1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iPdpCompression,DPCKTTSY_COMPRESSION1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iPdpType,DPCKTTSY_PDP_TYPE1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iUseEdge,DPCKTTSY_EGPRS_REQUIRED,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT_COMP_STR(contextConfigAsync.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT_COMP_STR(contextConfigAsync.iProtocolConfigOption.iAuthInfo.iPassword ,DPCKTTSY_PASSWORD1,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_SRVS_CASE("A.20B"));
	CHECKPOINT(contextConfigAsync.iNWIContext,DPCKTTSY_NWICONTEXT_TRUE,CHP_SRVS_CASE("A.20B"));

// Cancel Get Default Context Parameters
	gprs.GetDefaultContextParams(reqStatus, contextConfigPckgAsync);
	gprs.CancelAsyncRequest(EPacketGetDefaultContextParams);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.20B"));
	
// End of RPacketService tests.
	gprs.Close();
	phone.Close();

	return TestStepResult();
}
