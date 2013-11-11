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
// EtelPacketTest.cpp
// This contains EtelPacket TestCase Test 1.1  (RPacketService functionality testing)
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest1_1.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTest1_1::CEtelPacketTest1_1()
{
	// store the name of this test case
	SetTestStepName(_L("Service"));
}

// destructor
CEtelPacketTest1_1::~CEtelPacketTest1_1()
{
}

// do Test step 1.1
enum TVerdict CEtelPacketTest1_1::doTestStepL( void )
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT_EX(ret==KErrNone,CHP_OPEN_PHONE);
	RPacketService gprs;
	ret=gprs.Open(phone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.1"));
	
	TRequestStatus reqStatus;

	TBuf16<20> contextId;

	gprs.NotifyContextAdded(reqStatus, contextId);
   	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.21"));
	CHECKPOINT_COMP_STR(contextId,DPCKTTSY_CONTEXT_ID,CHP_SRVS_CASE("A.21"));

	gprs.NotifyContextAdded(reqStatus, contextId);
	gprs.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.21"));
// Attach
	gprs.Attach(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.2"));
	
	gprs.Attach(reqStatus);
	gprs.CancelAsyncRequest(EPacketAttach);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.2"));
//Detach
	gprs.Detach(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3"));
	
	gprs.Detach(reqStatus);
	gprs.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.3"));
//Gprs status
	RPacketService::TStatus gprsStatus;
	CHECKPOINT(gprs.GetStatus(gprsStatus),KErrNone,CHP_SRVS_CASE("A.6"));
	CHECKPOINT(gprsStatus,DPCKTTSY_PACKET_STATUS1,CHP_SRVS_CASE("A.6"));

	gprs.NotifyStatusChange(reqStatus,gprsStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.7"));
	CHECKPOINT(gprsStatus,DPCKTTSY_PACKET_STATUS2,CHP_SRVS_CASE("A.7"));

	gprs.NotifyStatusChange(reqStatus,gprsStatus);
	gprs.CancelAsyncRequest(EPacketNotifyStatusChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.7"));
//Context Activation
	RPacketContext::TContextConfigGPRS contextNotify;
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckg(contextNotify);

	gprs.NotifyContextActivationRequested(reqStatus, contextNotifyPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.8"));
	CHECKPOINT(contextNotify.iPdpType,DPCKTTSY_PDP_TYPE1,CHP_SRVS_CASE("A.8"));
	CHECKPOINT_COMP_STR(contextNotify.iPdpAddress,DPCKTTSY_PDP_ADDRESS1,CHP_SRVS_CASE("A.8"));
	CHECKPOINT_COMP_STR(contextNotify.iAccessPointName,DPCKTTSY_ACCESS_POINT1,CHP_SRVS_CASE("A.8"));
	CHECKPOINT(contextNotify.iUseEdge,DPCKTTSY_EGPRS_REQUIRED,CHP_SRVS_CASE("A.8"));
	CHECKPOINT_COMP_STR(contextNotify.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_SRVS_CASE("A.8"));
	CHECKPOINT_COMP_STR(contextNotify.iProtocolConfigOption.iAuthInfo.iPassword,DPCKTTSY_PASSWORD1,CHP_SRVS_CASE("A.8"));
	CHECKPOINT(contextNotify.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_SRVS_CASE("A.8"));
	CHECKPOINT(contextNotify.iNWIContext,DPCKTTSY_NWICONTEXT_TRUE,CHP_SRVS_CASE("A.8"));

	gprs.NotifyContextActivationRequested(reqStatus, contextNotifyPckg);
	gprs.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.8"));
	
//reject
	gprs.RejectActivationRequest(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.9"));

	gprs.RejectActivationRequest(reqStatus);
	gprs.CancelAsyncRequest(EPacketRejectActivationRequest);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.9"));
//Enumerate Contexts
	TInt contextCount, maxAllowed;
	gprs.EnumerateContexts(reqStatus,contextCount, maxAllowed);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.10"));
	CHECKPOINT(contextCount,DPCKTTSY_NUMBER_OF_CONTEXTS,CHP_SRVS_CASE("A.10"));
	CHECKPOINT(maxAllowed,DPCKTTSY_MAX_NUMBER_OF_CONTEXTS,CHP_SRVS_CASE("A.10"));
//Context Info
	TInt index = 0;
	RPacketService::TContextInfo contextInfo;
	gprs.GetContextInfo(reqStatus, index, contextInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.11"));
	CHECKPOINT_COMP_STR(contextInfo.iName,DPCKTTSY_CONTEXT_INFO_NAME0,CHP_SRVS_CASE("A.11"));
	CHECKPOINT(contextInfo.iStatus,DPCKTTSY_CONTEXT_INFO_STATUS0,CHP_SRVS_CASE("A.11"));

	gprs.GetContextInfo(reqStatus, index, contextInfo);
	gprs.CancelAsyncRequest(EPacketGetContextInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.11"));

	index++;
// Registration Status
	RPacketService::TRegistrationStatus registrStatus;
	gprs.GetNtwkRegStatus(reqStatus,registrStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(registrStatus,DPCKTTSY_REGISTRATION_STATUS1,CHP_SRVS_CASE("A.22"));

	gprs.GetNtwkRegStatus(reqStatus,registrStatus);
	gprs.CancelAsyncRequest(EPacketGetNtwkRegStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.22"));
//reg status notification
	gprs.NotifyChangeOfNtwkRegStatus(reqStatus, registrStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.12"));
	CHECKPOINT(registrStatus,DPCKTTSY_REGISTRATION_STATUS2,CHP_SRVS_CASE("A.12"));

	gprs.NotifyChangeOfNtwkRegStatus(reqStatus, registrStatus);
	gprs.CancelAsyncRequest(EPacketNotifyChangeOfNtwkRegStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.12"));
// MS Class
	RPacketService::TMSClass msClass1, msClass2;

	gprs.GetMSClass(reqStatus, msClass1, msClass2);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.13"));
	CHECKPOINT(msClass1,DPCKTTSY_CURRENT_MS_CLASS,CHP_SRVS_CASE("A.13"));
	CHECKPOINT(msClass2,DPCKTTSY_MAX_MS_CLASS,CHP_SRVS_CASE("A.13"));

	gprs.GetMSClass(reqStatus, msClass1, msClass2);
	gprs.CancelAsyncRequest(EPacketGetMSClass);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.13"));
// set ms class
	msClass1 = DPCKTTSY_SET_MS_CLASS;
	gprs.SetMSClass(reqStatus, msClass1);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.14"));

	gprs.SetMSClass(reqStatus, msClass1);
	gprs.CancelAsyncRequest(EPacketSetMSClass);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.14"));

//ms class change ntf
	gprs.NotifyMSClassChange(reqStatus, msClass1);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.15"));
	CHECKPOINT(msClass1,DPCKTTSY_NEW_MS_CLASS,CHP_SRVS_CASE("A.15"));

	gprs.NotifyMSClassChange(reqStatus, msClass1);
	gprs.CancelAsyncRequest(EPacketNotifyMSClassChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.15"));
// Static Caps
	TUint caps;
	
	CHECKPOINT(gprs.GetStaticCaps(caps, DPCKTTSY_PDP_TYPE1),KErrNone,CHP_SRVS_CASE("A.16"));
	CHECKPOINT(caps,DPCKTTSY_MISC_CAPS,CHP_SRVS_CASE("A.16"));
// Dynamic Caps
	RPacketService::TDynamicCapsFlags dynamicCaps;

	CHECKPOINT(gprs.GetDynamicCaps(dynamicCaps),KErrNone,CHP_SRVS_CASE("A.17"));
	CHECKPOINT(dynamicCaps,DPCKTTSY_DYNAMIC_CAPS1,CHP_SRVS_CASE("A.17"));
//dynamic caps chamge notification
	gprs.NotifyDynamicCapsChange(reqStatus, dynamicCaps);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.18"));
	CHECKPOINT(dynamicCaps,DPCKTTSY_DYNAMIC_CAPS2,CHP_SRVS_CASE("A.18"));

	gprs.NotifyDynamicCapsChange(reqStatus, dynamicCaps);
	gprs.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.18"));

// Attach Mode
	RPacketService::TAttachMode attachMode=DPCKTTSY_ATTACH_MODE1;
	CHECKPOINT(gprs.SetAttachMode(attachMode),KErrNone,CHP_SRVS_CASE("A.4"));

	CHECKPOINT(gprs.GetAttachMode(attachMode),KErrNone,CHP_SRVS_CASE("A.5"));
	CHECKPOINT(attachMode,DPCKTTSY_ATTACH_MODE2,CHP_SRVS_CASE("A.5"));

	ExtendedTest1( phone, gprs, reqStatus);

	return TestStepResult();
}


// moved here to prevent stack overflow
enum TVerdict CEtelPacketTest1_1::ExtendedTest1(RPhone& phone, 	RPacketService& gprs, TRequestStatus& reqStatus)
{
	
// Default Context Params
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
	contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = DPCKTTSY_PROTOCOL_TYPE1_0;
	contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_TRUE;
	CHECKPOINT(gprs.SetDefaultContextParams(contextConfigPckg),KErrNone,CHP_SRVS_CASE("A.19"));

	CHECKPOINT(gprs.GetDefaultContextParams(contextConfigPckg),KErrNone,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iAccessPointName,DPCKTTSY_ACCESS_POINT2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iAnonymousAccessReqd,DPCKTTSY_ANONYMOUS_ACCESS2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iPdpAddress,DPCKTTSY_PDP_ADDRESS2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iPdpCompression,DPCKTTSY_COMPRESSION2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iPdpType,DPCKTTSY_PDP_TYPE2,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iUseEdge,DPCKTTSY_EGPRS_NOTREQUIRED,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_SRVS_CASE("A.20"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iPassword,DPCKTTSY_PASSWORD1,CHP_SRVS_CASE("A.20"));
	CHECKPOINT(contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_SRVS_CASE("A.20"));

// JoeF : 16-Nov-01 : Added the following tests below. These test new API methods recently added to the RPacketService class.
// Get Preferred Bearer
	RPacketService::TPreferredBearer myBearer;
	TInt ret=gprs.GetPreferredBearer(myBearer);
	CHECKPOINT(ret, KErrNone, CHP_SRVS_CASE("A.23"));
	CHECKPOINT_COMP_STR(myBearer, DPCKTTSY_PREFERRED_BEARER1, CHP_SRVS_CASE("A.23"));
	
//async Get Preferred Bearer
	RPacketService::TPreferredBearer myAsyncBearer;
	gprs.GetPreferredBearer(reqStatus, myAsyncBearer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT_COMP_STR(myAsyncBearer, DPCKTTSY_PREFERRED_BEARER2, CHP_SRVS_CASE("A.23A"));
	
//async Get Preferred Bearer & Cancel
	gprs.GetPreferredBearer(reqStatus, myAsyncBearer);
	gprs.CancelAsyncRequest(EPacketGetPrefBearer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.23A"));

// Set Preferred Bearer
	gprs.SetPreferredBearer(reqStatus, myBearer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrNone, CHP_SRVS_CASE("A.24"));

	gprs.SetPreferredBearer(reqStatus, myBearer);
	gprs.CancelAsyncRequest(EPacketSetPrefBearer);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.23"));

// End of RPacketService tests.
	gprs.Close();
	phone.Close();

	return TestStepResult();
}

