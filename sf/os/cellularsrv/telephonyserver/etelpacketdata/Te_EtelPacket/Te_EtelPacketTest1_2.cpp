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
// EtelPacketTest1_2.cpp
// This contains EtelPacket TestCase Test 1.2  (RPacketContext functionality)
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest1_2.h"
#include "Te_etelpckt_defs.h"

// constructor
CEtelPacketTest1_2::CEtelPacketTest1_2()
{
	// store the name of this test case
	SetTestStepName(_L("Context"));
}

// destructor
CEtelPacketTest1_2::~CEtelPacketTest1_2()
{
}

// do Test step 1.1
enum TVerdict CEtelPacketTest1_2::doTestStepL( void )
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT(ret,KErrNone,CHP_OPEN_PHONE);

	RPacketService gprs;
	ret=gprs.Open(phone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.1"));

	RPacketContext gprsContext;
	TName contextName;
	ret=gprsContext.OpenNewContext(gprs, contextName);
	CHECKPOINT(ret,KErrNone,CHP_CNTXT_CASE("B.1"));

	User::After(100000L);

	RPacketContext aContext;
	aContext.OpenExistingContext(gprs, contextName);
	CHECKPOINT(contextName, DPCKTTSY_CONTEXT_NAME, CHP_CNTXT_CASE("B.2"));

	aContext.Close();
	TRequestStatus reqStatus;
//Set configuration 
	RPacketContext::TContextConfigGPRS contextConfig;
	CHECKPOINT(contextConfig.ExtensionId(),TPacketDataConfigBase::KConfigGPRS,CHP_CNTXT_CASE("B.4"));
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
	contextConfig.iNWIContext = DPCKTTSY_NWICONTEXT_FALSE;

	gprsContext.SetConfig(reqStatus, contextConfigPckg);
  	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.4"));

	gprsContext.SetConfig(reqStatus, contextConfigPckg);
	gprsContext.CancelAsyncRequest(EPacketContextSetConfig);
  	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.4"));

  //get context
	gprsContext.GetConfig(reqStatus,contextConfigPckg);
  	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iAccessPointName,DPCKTTSY_ACCESS_POINT2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT(contextConfig.iAnonymousAccessReqd,DPCKTTSY_ANONYMOUS_ACCESS2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iPdpAddress,DPCKTTSY_PDP_ADDRESS2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT(contextConfig.iPdpCompression,DPCKTTSY_COMPRESSION2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT(contextConfig.iPdpType,DPCKTTSY_PDP_TYPE2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iPdpAddress,DPCKTTSY_PDP_ADDRESS2,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT(contextConfig.iUseEdge,DPCKTTSY_EGPRS_NOTREQUIRED,CHP_CNTXT_CASE("B.5")); // Added 12/07/02 - CR: BGIN5ADD6S
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iPassword,DPCKTTSY_PASSWORD1,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_CNTXT_CASE("B.5"));
	CHECKPOINT_COMP_STR(contextConfig.iNWIContext,DPCKTTSY_NWICONTEXT_TRUE,CHP_CNTXT_CASE("B.5"));

	gprsContext.GetConfig(reqStatus, contextConfigPckg);
	gprsContext.CancelAsyncRequest(EPacketContextGetConfig);
  	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.5"));

  //notify config changed
	gprsContext.NotifyConfigChanged(reqStatus, contextConfigPckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT_COMP_STR(contextConfig.iAccessPointName,DPCKTTSY_ACCESS_POINT1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT(contextConfig.iAnonymousAccessReqd,DPCKTTSY_ANONYMOUS_ACCESS1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT_COMP_STR(contextConfig.iPdpAddress,DPCKTTSY_PDP_ADDRESS1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT(contextConfig.iPdpCompression,DPCKTTSY_COMPRESSION1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT(contextConfig.iPdpType,DPCKTTSY_PDP_TYPE1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT(contextConfig.iUseEdge,DPCKTTSY_EGPRS_REQUIRED,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iUsername,DPCKTTSY_USER_NAME1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT_COMP_STR(contextConfig.iProtocolConfigOption.iAuthInfo.iPassword,DPCKTTSY_PASSWORD1,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT(contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol,DPCKTTSY_PROTOCOL_TYPE1_0,CHP_CNTXT_CASE("B.6"));
	CHECKPOINT_COMP_STR(contextConfig.iNWIContext,DPCKTTSY_NWICONTEXT_TRUE,CHP_CNTXT_CASE("B.6"));

	gprsContext.NotifyConfigChanged(reqStatus, contextConfigPckg);
	gprsContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.6"));
	
 // Activate
	gprsContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.7"));

	gprsContext.Activate(reqStatus);
	gprsContext.CancelAsyncRequest(EPacketContextActivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.7"));
// Deactivate
	gprsContext.Deactivate(reqStatus);
	gprsContext.CancelAsyncRequest(EPacketContextDeactivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.8"));

	gprsContext.Deactivate(reqStatus);
	gprsContext.CancelAsyncRequest(EPacketContextDeactivate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.8"));

// Delete
	gprsContext.Delete(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.9"));

	gprsContext.Delete(reqStatus);
	gprsContext.CancelAsyncRequest(EPacketContextDelete);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.9"));
// Comm Port
	RCall::TCommPort dataPort;
	gprsContext.LoanCommPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.10"));
	CHECKPOINT_COMP_STR(dataPort.iCsy,DPCKTTSY_CSY,CHP_CNTXT_CASE("B.10"));
	CHECKPOINT_COMP_STR(dataPort.iPort,DPCKTTSY_PORT,CHP_CNTXT_CASE("B.10"));

	gprsContext.LoanCommPort(reqStatus, dataPort);
	gprsContext.CancelAsyncRequest(EPacketContextLoanCommPort);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.10"));
//recover comm port
	gprsContext.RecoverCommPort(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.11"));

	gprsContext.RecoverCommPort(reqStatus);
	gprsContext.CancelAsyncRequest(EPacketContextRecoverCommPort);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.11"));

	ExtendedTest1( reqStatus, gprsContext);

	gprs.Close();
	phone.Close();

	return TestStepResult();
}

// some test code moved here to prevent stack overflow
enum TVerdict CEtelPacketTest1_2::ExtendedTest1(TRequestStatus& reqStatus, RPacketContext& gprsContext)
{
	// Context Status
	RPacketContext::TContextStatus contextStatus;
	CHECKPOINT(gprsContext.GetStatus(contextStatus),KErrNone,CHP_CNTXT_CASE("B.12"));
	CHECKPOINT(contextStatus,DPCKTTSY_CONTEXT_STATUS1,CHP_CNTXT_CASE("B.12"));
//
	gprsContext.NotifyStatusChange(reqStatus, contextStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.13"));
	CHECKPOINT(contextStatus,DPCKTTSY_CONTEXT_STATUS2,CHP_CNTXT_CASE("B.13"));

	gprsContext.NotifyStatusChange(reqStatus, contextStatus);
	gprsContext.CancelAsyncRequest(EPacketContextNotifyStatusChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.13"));
// Data Volume Transferred
	RPacketContext::TDataVolume volume;
	CHECKPOINT(gprsContext.GetDataVolumeTransferred(volume),KErrNone,CHP_CNTXT_CASE("B.14"));
	CHECKPOINT(volume.iBytesSent,DPCKTTSY_BYTES_SENT1,CHP_CNTXT_CASE("B.14"));
	CHECKPOINT(volume.iOverflowCounterSent,DPCKTTSY_OVERFLOW_COUNTER_SENT1,CHP_CNTXT_CASE("B.14"));
	CHECKPOINT(volume.iBytesReceived,DPCKTTSY_BYTES_RCVD1,CHP_CNTXT_CASE("B.14"));
	CHECKPOINT(volume.iOverflowCounterReceived,DPCKTTSY_OVERFLOW_COUNTER_RCVD1,CHP_CNTXT_CASE("B.14"));

	// Data Transferred
	TUint rcvdGranularity=DPCKTTSY_RCVD_GRANULARITY;
	TUint sentGranularity=DPCKTTSY_SENT_GRANULARITY;

	gprsContext.NotifyDataTransferred(reqStatus, volume, rcvdGranularity, sentGranularity);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.15"));
	CHECKPOINT(volume.iBytesSent,DPCKTTSY_BYTES_SENT2,CHP_CNTXT_CASE("B.15"));
	CHECKPOINT(volume.iOverflowCounterSent,DPCKTTSY_OVERFLOW_COUNTER_SENT2,CHP_CNTXT_CASE("B.15"));
	CHECKPOINT(volume.iBytesReceived,DPCKTTSY_BYTES_RCVD2,CHP_CNTXT_CASE("B.15"));
	CHECKPOINT(volume.iOverflowCounterReceived,DPCKTTSY_OVERFLOW_COUNTER_RCVD2,CHP_CNTXT_CASE("B.15"));

	gprsContext.NotifyDataTransferred(reqStatus, volume, rcvdGranularity, sentGranularity);
	gprsContext.CancelAsyncRequest(EPacketContextNotifyDataTransferred);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.15"));

	//current connection speed
	TUint rate=0;
	gprsContext.GetConnectionSpeed(reqStatus,rate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.16"));
	CHECKPOINT(rate,DPCKTTSY_CONNECTION_RATE,CHP_CNTXT_CASE("B.16"));
//speed change notification
	gprsContext.NotifyConnectionSpeedChange(reqStatus, rate);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("B.17"));
	CHECKPOINT(rate,DPCKTTSY_CONNECTION_RATE,CHP_CNTXT_CASE("B.17"));

	gprsContext.NotifyConnectionSpeedChange(reqStatus, rate);
	gprsContext.CancelAsyncRequest(EPacketContextNotifyConnectionSpeedChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.17"));
// Last Error Cause
	TInt error;
	CHECKPOINT(gprsContext.GetLastErrorCause(error),KErrNone,CHP_CNTXT_CASE("B.18"));
	CHECKPOINT(error,DPCKTTSY_LAST_ERROR_CAUSE,CHP_CNTXT_CASE("B.18"));

// JoeF : 16-Nov-01 : Added the following test below. This tests a new API method recently added to the RPacketContext class
// GetProfileName
	TName myQoSProfile;
	CHECKPOINT(gprsContext.GetProfileName(myQoSProfile), KErrNone, CHP_CNTXT_CASE("B.19"));
	CHECKPOINT_COMP_STR(myQoSProfile, DPCKTTSY_PROFILE_NAME, CHP_CNTXT_CASE("B.19"));

// Change Request: NDOE-58VF4Q. Added asychronous request (and cancel) for the following
// method: GetDataVolumeTransferred().
// Data Volume Transferred - Asynchronous variant 
	RPacketContext::TDataVolume volume2;
	gprsContext.GetDataVolumeTransferred(reqStatus, volume2);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(), KErrNone, CHP_CNTXT_CASE("B.14B"));
	CHECKPOINT(volume2.iBytesSent,DPCKTTSY_BYTES_SENT3,CHP_CNTXT_CASE("B.14B"));
	CHECKPOINT(volume2.iOverflowCounterSent,DPCKTTSY_OVERFLOW_COUNTER_SENT3,CHP_CNTXT_CASE("B.14B"));
	CHECKPOINT(volume2.iBytesReceived,DPCKTTSY_BYTES_RCVD3,CHP_CNTXT_CASE("B.14B"));
	CHECKPOINT(volume2.iOverflowCounterReceived,DPCKTTSY_OVERFLOW_COUNTER_RCVD3,CHP_CNTXT_CASE("B.14B"));

// Cancel Data Volume Transferred
	gprsContext.GetDataVolumeTransferred(reqStatus, volume2);
	gprsContext.CancelAsyncRequest(EPacketContextGetDataVolumeTransferred);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("B.14"));

	// End of RPacketContext tests
	gprsContext.Close();



	return TestStepResult();
}

