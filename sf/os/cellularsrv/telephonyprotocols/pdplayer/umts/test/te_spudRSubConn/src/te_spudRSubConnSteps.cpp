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
// te_spudNetworkSideSteps.cpp
// Implements the integration tests for the SPUD using RawIpNif
// 
//

/**
 @file
 @internalComponent
*/


#include "te_spudRSubConnSteps.h"
#include <simtsy.h>
#include <commdbconnpref.h>
#include <nifvar.h> // Nifman Progress notifications
#include <es_enum.h> 
#include <c32comm.h> // uniquely for the call to StartC32WithCMISuppressions
#include <e32property.h>
#include <ip_subconparams.h>

namespace
	{
	const TInt KMaxMsgLen = 128;
	}
/**
 Implements the pure virtual doTestStepPreambleL defined in CTestStep. 
 Used to disable the phone book synchronizer
 which may cause the tests to fail.
 
 
 @return EPass if successful.
 @leave If Phonebook synchronizer disabling fails. 
 */
TVerdict CSpudRSubConnTestStepBase::doTestStepPreambleL()
	{	
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	TESTL(KErrNone == err || KErrAlreadyExists == err);
	INFO_PRINTF1(_L("Test Step Preamble: disabled Phonebook Synchronizer."));	
	return EPass;
	}
	

/**
 Implements the pure virtual doTestStepL defined in CTestStep. 
 Allows the base class to execute before any of the derived 
 tests is called.
 
 @leave If any of the called methods leaves.
 */
TVerdict CSpudRSubConnTestStepBase::doTestStepL()
	{
	// Tell SIM.TSY which test it should load. 
	// If this is removed, SIM TSY will load test 0.
	
	TInt simTsyTestNum = -1;
	if (!GetIntFromConfig(ConfigSection(), _L("SimTsyTestNum"), simTsyTestNum))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(simTsyTestNum >= 0);
	
	TInt ret0 =  RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,simTsyTestNum);
    if (ret0 != KErrNone)
    	{
    	ret0 =  RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,simTsyTestNum);
    	}

		
	TRAPD(esockConnErr, TestL(iESock.Connect(), _L("RSockeServ::Connect")));
	if(KErrNone != esockConnErr)
		{
		INFO_PRINTF1(_L("ESock thread is dead. Most likely, it was crashed by the previous test case. Check the Root Server logs."));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
	
	ASSERT(!CActiveScheduler::Current()); // We should not have an AS at this point.
	CActiveScheduler* testSched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(testSched);
	CActiveScheduler::Install(testSched);
	
	
	// Test sequence itself:
	TVerdict testResult = EFail;		
	TRAPD(err, testResult = RunTestStepL());
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Test Step Completion."));
		SetTestStepResult(testResult);
		}
	else
		{
		ERR_PRINTF2(_L("Test Step Failure: the step left with [%d]"), err);
		SetTestStepResult(EFail);
		}
	
	// It's ok to close everything more than once, or if not opened...
	iPrimCtxSock1.Close();
	iScndCtxSock1.Close();
	iSpudScndCtx1.Close();
	iSpud.Close(); // SPUD will linger, we don't care.
	iESock.Close();
	
	CActiveScheduler::Install(NULL); // uninstall the test scheduler
	CleanupStack::PopAndDestroy(testSched);
	
	return TestStepResult();	
	}




	




/**
Logs a message and leaves on error 

@param aErrCode error code to check
@param aMsg message to log 
@leave if aError is other than KErrNone
*/
void CSpudRSubConnTestStepBase::TestL(TInt aErrCode, const TDesC& aMsg)
	{	
	TestL(aErrCode, KErrNone, aMsg);
	}

/**
Used to verify that an error code is what expected, and log the associated comment

@param aErrCode the error code to check
@param aExpErrCode the expected error code
@param aMsg the message to log before testing
@leave if aErrCode != aExpErrCode
*/	
void CSpudRSubConnTestStepBase::TestL(TInt aErrCode, TInt aExpErrCode, const TDesC& aMsg)
	{	
	if(aExpErrCode == aErrCode)
		{
		INFO_PRINTF3(_L("%S: err[%d], as expected. OK."), &aMsg, aErrCode);
		}
	else
		{
		ERR_PRINTF5(_L("%S FAILED: err[%d], expected [%d]. Leaving with [%d])."), &aMsg, aErrCode, aExpErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	}

/**
Used to verify that a boolean is as expected, and log the associated comment

@param aBool the boolean to check
@param aMsg the message to log before testing
@leave if aBool == EFalse
*/	
void CSpudRSubConnTestStepBase::TestBooleanTrueL(TBool aBool, const TDesC& aMsg)
	{
	if(aBool)
		{
		INFO_PRINTF2(_L("%S: Value is true, as expected. OK."), &aMsg);
		}
	else
		{
		ERR_PRINTF2(_L("Failed, %S: Value is false. Leaving with KErrGeneral)."), &aMsg);
		User::Leave(KErrGeneral);
		}
	}
	
	

/**
Open and Start an interface */
void CSpudRSubConnTestStepBase::OpenAndStartInterfaceL(RConnection& aConn, TCommDbConnPref& aPref, TRequestStatus& aSt, const TDesC& aLogMsg)
	{
	INFO_PRINTF3(_L("%S: Opening and Starting RConnection on IAP=%d."), &aLogMsg, aPref.IapId());
	TInt openErr = aConn.Open(iESock, KAfInet);
	TestL(openErr, KErrNone, _L("Opening RConnection"));
	aConn.Start(aPref, aSt);	
	}
	
void CSpudRSubConnTestStepBase::WaitForCompletionL(TRequestStatus& aSt, TInt aExpErrCode, const TDesC& aLogMsg)
	{
	INFO_PRINTF3(_L("%S: Waiting for completion with %d"), &aLogMsg, aExpErrCode);
	User::WaitForRequest(aSt);
	INFO_PRINTF3(_L("%S completed with status= %d"), &aLogMsg, aSt.Int());
	TestL(aSt.Int(), aExpErrCode, aLogMsg);
	}

void CSpudRSubConnTestStepBase::ReceiveOnSocketL(RSocket& aSocket, const TDesC& aLogMsg)
	{
	INFO_PRINTF2(_L("%S: Waiting to receive."), &aLogMsg);
	TRequestStatus recvSt;
	TBuf8<256> recvBuf;
	TInetAddr srcAddr;
	aSocket.RecvFrom(recvBuf, srcAddr, 0x0, recvSt);
	WaitForCompletionL(recvSt, KErrNone, _L("Received on Socket"));	
	}
	

void CSpudRSubConnTestStepBase::VerifyBuffersEqualL(const TDesC8& aBuf1, const TDesC8& aBuf2)
	{
	if(0 != aBuf1.Compare(aBuf2))
		{
		INFO_PRINTF1(_L("Buffer 1 is not equal to Buffer 2. FAILURE."));
		User::Leave(KErrCorrupt);		
		}
	}

void CSpudRSubConnTestStepBase::WriteReadOnSocketL(RSocket& aSocket, const TDesC& aLogMsg)
	{
	INFO_PRINTF2(_L("%S: WriteReadOnSocketL"), &aLogMsg);
	
	TRequestStatus wSt1;
   	TBuf8<KMaxMsgLen> writeBuf(_L8("WRITE READ"));
   	
   	aSocket.Write(writeBuf, wSt1);
    WaitForCompletionL(wSt1, KErrNone, _L("Write on Socket"));
    
    TBuf8<KMaxMsgLen> readBuf;
	TRequestStatus rSt;
	aSocket.Read(readBuf, rSt);
	WaitForCompletionL(rSt, KErrNone, _L("Read on Socket")); 
	
	VerifyBuffersEqualL(writeBuf, readBuf);
	}


void CSpudRSubConnTestStepBase::SendRecvOnSocketL(RSocket& aSocket, TInetAddr& aDstAddr, const TDesC& aLogMsg)
	{
	INFO_PRINTF2(_L("%S: SendRecvOnSocketL"), &aLogMsg);
		
	TBuf8<KMaxMsgLen> sendBuf(_L8("SEND RECEIVE"));
	TRequestStatus sendSt;
    aSocket.SendTo(sendBuf, aDstAddr, 0x0, sendSt);
	WaitForCompletionL(sendSt, KErrNone, _L("Send on Socket"));
			
	TBuf8<KMaxMsgLen> recvBuf;		
	TRequestStatus recvSt;
	aSocket.RecvFrom(recvBuf, aDstAddr, 0x0, recvSt);
	WaitForCompletionL(recvSt, KErrNone, _L("Receive on Socket"));
	
	VerifyBuffersEqualL(recvBuf, sendBuf);
	}
	
void CSpudRSubConnTestStepBase::RecvSendOnSocketL(RSocket& aSocket, const TDesC& aLogMsg)
	{
	INFO_PRINTF2(_L("%S: **Bounce: RecvSendOnSocket"), &aLogMsg);
	
	
	TRequestStatus st;
	TBuf8<KMaxMsgLen> recvBuf;
	TInetAddr srcAddr;
	aSocket.RecvFrom(recvBuf, srcAddr, 0x0, st);
	WaitForCompletionL(st, KErrNone, _L("**Bounce: Receive on Socket"));	
		
	aSocket.SendTo(recvBuf, srcAddr, 0x0, st);
	WaitForCompletionL(st, KErrNone, _L("**Bounce: Send on Socket"));	
	}


void CSpudRSubConnTestStepBase::OpenAndBindSocketL(RSocket& aSock, RConnection& aIface, TUint aLocalPort, const TDesC& aLogMsg)
	{
	INFO_PRINTF3(_L("%S: Opening and Binding to port= %d"), &aLogMsg, aLocalPort);
	TInt ret = aSock.Open(iESock, 
    	KAfInet, 
        KSockDatagram, 
        KProtocolInetUdp,
        aIface);
        
    TestL(ret, KErrNone, _L("Open UDP Socket"));
	
	TInetAddr localAddr;
	localAddr.SetPort(aLocalPort);
	TestL(aSock.Bind(localAddr), KErrNone, _L("Bind Socket"));
	}
//


/**
Used in lieu of proper network-based server 
Open an interface, opens a socket on this interface, receives-sends udp (bounces) */
TVerdict CLoopbackPpp1::RunTestStepL()
	{
	// Figure out our name, so that the logging makes sense:
	TBuf<64> ourName(_L("BOUNCE"));
	ourName.AppendFormat(_L("[%x]"), this);
	
	TInt iapId = -1;
	if (!GetIntFromConfig(ConfigSection(), _L("IapId"), iapId))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(iapId > 0);
		
	TCommDbConnPref pref;
    pref.SetIapId(iapId);
	
	RConnection iface;
	if (KErrNone != iface.Open(iESock, KAfInet))
	    {
	    return EFail;
	    }
    TRequestStatus startSt;
    iface.Start(pref, startSt);
    User::WaitForRequest(startSt);
	INFO_PRINTF4(_L("%S: Interface on Iap ID =%d started with error=%d."), &ourName, iapId, startSt.Int());	
	User::LeaveIfError(startSt.Int());	
	
	TInt localPort = 0x0;
	if (!GetIntFromConfig(ConfigSection(), _L("UdpEchoPort"), localPort))
	    {
	    User::Leave(KErrNotFound);
	    }
    ASSERT(localPort > 0x0); 
    
    RSocket bounceSock;
    OpenAndBindSocketL(bounceSock, iface, static_cast<TUint>(localPort), ourName);
    
    TInt iterations = 0;
    if (!GetIntFromConfig(ConfigSection(), _L("Iterations"), iterations))
        {
        User::Leave(KErrNotFound);
        }
    ASSERT(iterations > 0);
    
    for(TInt iter = 0; iter < iterations; ++iter)
	    {
	    RecvSendOnSocketL(bounceSock, ourName);
	    }
    
   	bounceSock.Close();
	iface.Close(); // Leave interface lingering, it will be nuked when the peer PPP terminates.
	return EPass;		
	}


TBool CSpudRSubConnTestStepBase::ApplyQoSParametersL(RSubConnection& aPdpContext)
	{
	TInt qosParamSet = 0;
    if (!GetIntFromConfig(ConfigSection(), _L("QoSParamSet"), qosParamSet))
        {
        User::Leave(KErrNotFound);
        }
		
	ASSERT(qosParamSet >= 0);

	switch(qosParamSet)
		{
		case 0:
			return EFalse; // no qos parameters specified
		case 2:
			ApplyQoSParametersSet2L(aPdpContext);
			return ETrue;
		default:
			ASSERT(EFalse);
			return EFalse;
		}
	}

void CSpudRSubConnTestStepBase::ApplyQoSParametersSet2L(RSubConnection& aPdpContext)
	{
	RSubConParameterBundle qosParams;
	CleanupClosePushL(qosParams);

	// Create a container for QoS sub connection parameters (Param bundle takes ownership)
	CSubConParameterFamily* qosFamily = CSubConParameterFamily::NewL(qosParams, KSubConQoSFamily);
		
	// set class parameter
	CSubConQosIPLinkR99ParamSet* ipParamsReq = CSubConQosIPLinkR99ParamSet::NewL(*qosFamily, CSubConParameterFamily::ERequested);
	//The requested QoS parameters.
	ipParamsReq->SetTrafficClass(RPacketQoS::ETrafficClassBackground);
	ipParamsReq->SetMaxBitrateUplink(10);
	ipParamsReq->SetMaxBitrateDownlink(50);
	ipParamsReq->SetGuaBitrateDownlink(20); 
	ipParamsReq->SetGuaBitrateUplink(10);	
	ipParamsReq->SetMaxSduSize(1000);		
	ipParamsReq->SetResidualBitErrorRatio(RPacketQoS::EBERFourPerThousand);
	ipParamsReq->SetSDUErrorRatio(RPacketQoS::ESDUErrorRatioOnePerThousand);
	ipParamsReq->SetErroneousSDUDelivery(RPacketQoS::EErroneousSDUDeliveryNotRequired);				
	ipParamsReq->SetTrafficHandlingPriority(RPacketQoS::ETrafficPriorityUnspecified);		
	ipParamsReq->SetTransferDelay(3000); 
	ipParamsReq->SetDeliveryOrder(RPacketQoS::EDeliveryOrderNotRequired);

	//The minimum Requested params
	CSubConQosIPLinkR99ParamSet* ipParamsMinReq = CSubConQosIPLinkR99ParamSet::NewL(*qosFamily, CSubConParameterFamily::EAcceptable);
	
	ipParamsMinReq->SetTrafficClass(RPacketQoS::ETrafficClassBackground);
	ipParamsMinReq->SetGuaBitrateDownlink(20);
	ipParamsMinReq->SetGuaBitrateUplink(8);
	ipParamsMinReq->SetMaxBitrateDownlink(20);
	ipParamsMinReq->SetMaxBitrateUplink(10); 
	ipParamsMinReq->SetMaxSduSize(200);		
	ipParamsMinReq->SetResidualBitErrorRatio(RPacketQoS::EBERFivePerHundred);
	ipParamsMinReq->SetSDUErrorRatio(RPacketQoS::ESDUErrorRatioOnePerTen);
	ipParamsMinReq->SetErroneousSDUDelivery(RPacketQoS::EErroneousSDUDeliveryNotRequired);		
	ipParamsMinReq->SetTrafficHandlingPriority(RPacketQoS::ETrafficPriorityUnspecified);
	ipParamsMinReq->SetTransferDelay(4000); 
	ipParamsMinReq->SetDeliveryOrder(RPacketQoS::EDeliveryOrderNotRequired);		
	
	TInt err = aPdpContext.SetParameters(qosParams);
	TestL(err, KErrNone, _L("Applied QoS Parameters"));
	CleanupStack::PopAndDestroy(&qosParams);
	}



/**
The test sequence:
Create the primary & secondary PDP contexts.
Send - Receive UDP over both of them.
Close everything 

@leave if a test fails or there is an unexpected error */ 
TVerdict CUdp1::RunTestStepL()
	{
	TRequestStatus spudStartReq;
	TCommDbConnPref spudPref;
	spudPref.SetIapId(2);
	OpenAndStartInterfaceL(iSpud, spudPref, spudStartReq, _L(">>>>>Starting SPUD NIF"));
	WaitForCompletionL(spudStartReq, KErrNone, _L(">>>>>Starting SPUD NIF"));
	
	TInt ret = iScndCtxSock1.Open(iESock, 
                 KAfInet, 
                 KSockDatagram, 
                 KProtocolInetUdp,
                 iSpud);
    TestL(ret, KErrNone, _L(">>>>>>>>Open UDP Socket, primary PDP context"));
     
   	// KSoUdpSynchronousSend option causes the UDP send operation to block when dynamic 
	// interface setup is in progress or when local flow control within the stack would otherwise 
	// cause the packet to be dropped. This is not strictly necessary since we've waited for KLinkLayerOpen,
	// but who knows what's the timing between TCP/IP stack, Nifman, ESock is like...
	TestL(iScndCtxSock1.SetOpt(KSoUdpSynchronousSend, KSolInetUdp, 1), KErrNone, _L("SetOpt Sync Send"));
	
	// We must bind to a local address, and connect to a remote address,
	// so that TFT is always generated with the same destination - source ports.
	// Otherwise, SIM.TSY will reject TFT proposed.
	
	TInt scndCtxSockLocalPort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("SecLocalPort"), scndCtxSockLocalPort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(scndCtxSockLocalPort > 0);
	
	TInetAddr localAddr;
	localAddr.SetPort(scndCtxSockLocalPort);
	TestL(iScndCtxSock1.Bind(localAddr), KErrNone, _L(">>>>>Bind the Secondary PDP ctx socket"));
	
	
	TInt scndCtxSockRemotePort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("SecRemotePort"),scndCtxSockRemotePort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(scndCtxSockRemotePort > 0);
	
	TPtrC scndCtxSockRemoteAddr;
	if (!GetStringFromConfig(ConfigSection(), _L("SecRemoteIpAddr"), scndCtxSockRemoteAddr))
	    {
	    User::Leave(KErrNotFound);
	    }

	TInetAddr dstAddr1;
	dstAddr1.SetPort(scndCtxSockRemotePort);
    dstAddr1.Input(scndCtxSockRemoteAddr);
    
    TRequestStatus connStatus;
    iScndCtxSock1.Connect(dstAddr1, connStatus);
	WaitForCompletionL(connStatus, KErrNone, _L(">>>>>>>Connect the Secondary PDP ctx socket"));
    
   
    TInt sockErr = iPrimCtxSock1.Open(iESock, 
                 KAfInet, 
                 KSockDatagram, 
                 KProtocolInetUdp,
                 iSpud);
    TestL(sockErr, KErrNone, _L(">>>>>>Open Primary PDP ctx socket"));
    
   
   
   	TInt openErr = iSpudScndCtx1.Open(iESock, 
    	RSubConnection::ECreateNew, 
        iSpud);
    TestL(openErr, KErrNone, _L(">>>>>>>Open RSubConnection on SPUD"));
    
    TRequestStatus subconNotifyStat;
    TNotificationEventBuf eventBuf;
    iSpudScndCtx1.EventNotification(eventBuf, ETrue,  subconNotifyStat);              
	
	TBool waitForQos = ApplyQoSParametersL(iSpudScndCtx1);
	
	TRequestStatus subconAddStat;
	iSpudScndCtx1.Add(iScndCtxSock1, subconAddStat);
    WaitForCompletionL(subconAddStat, KErrNone, _L(">>>>>>Transfer the Socket to 2ndary context"));
    
   
   	if(waitForQos)
		{
		WaitForCompletionL(subconNotifyStat, KErrNone, _L(">>>>>QoS Negotiation Completion on 2ndary context"));
   		} 
   	else
	   	{
		iSpudScndCtx1.CancelEventNotification();	   		
	   	} 
    //*************************************************************************************************
    User::After(3 * 1000000); // Give the loopback sockets a chance to bind & open,
    // else we can end up with ICMP Destination (port) Unreachable.
    // When used over WinTunnel, this is completely unnecessary.
    //*************************************************************************************************
    
    WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 1"));
    
    // Must address UDP on primary explicitly.
    TInt primCtxSockRemotePort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("PrimRemotePort"),primCtxSockRemotePort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(primCtxSockRemotePort > 0);
	
	TPtrC primCtxSockRemoteAddr;
	if (!GetStringFromConfig(ConfigSection(), _L("PrimRemoteIpAddr"), primCtxSockRemoteAddr))
	    {
	    User::Leave(KErrNotFound);
	    }
	    
    TInetAddr primDstAddr1;
    primDstAddr1.SetPort(primCtxSockRemotePort);
    primDstAddr1.Input(primCtxSockRemoteAddr);
    
    SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Send - Receive on Primary PDP Context, 1"));
   
   	
   	WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 2"));
   	SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Primary PDP Context, 2"));
    
   	SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Primary PDP Context, 3"));
   	WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 3"));
   	
   	iSpud.Stop(); // nukes both contexts
   	// Everything will be cleaned up later.
  	return EPass;
  	}

/**
The test sequence:
Create the primary context
Init secondary SubConnection
Set up QoS params on secondary SubConnection
Create the secondary PDP contexts on secondary SubConnection.
Send - Receive UDP over both of them.
Close everything 

@leave if a test fails or there is an unexpected error */ 
TVerdict CUdp2::RunTestStepL()
	{
	TRequestStatus spudStartReq;
	TCommDbConnPref spudPref;
	spudPref.SetIapId(2);
	OpenAndStartInterfaceL(iSpud, spudPref, spudStartReq, _L(">>>>>Starting SPUD NIF"));
	WaitForCompletionL(spudStartReq, KErrNone, _L(">>>>>Starting SPUD NIF"));
	
   	TInt openErr = iSpudScndCtx1.Open(iESock, 
    	RSubConnection::ECreateNew, 
        iSpud);
    TestL(openErr, KErrNone, _L(">>>>>>>Open RSubConnection on SPUD"));
    
    TRequestStatus subconNotifyStat;
    TNotificationEventBuf eventBuf;
    iSpudScndCtx1.EventNotification(eventBuf, ETrue,  subconNotifyStat);              
	
	TBool waitForQos = ApplyQoSParametersL(iSpudScndCtx1);
	
	// Socket Open on Secondary SubConnection
	TInt ret = iScndCtxSock1.Open(iESock, 
                 KAfInet, 
                 KSockDatagram, 
                 KProtocolInetUdp,
                 iSpudScndCtx1);
    
    TestL(ret, KErrNone, _L(">>>>>>>>Open UDP Socket, primary PDP context"));
     
   	// KSoUdpSynchronousSend option causes the UDP send operation to block when dynamic 
	// interface setup is in progress or when local flow control within the stack would otherwise 
	// cause the packet to be dropped. This is not strictly necessary since we've waited for KLinkLayerOpen,
	// but who knows what's the timing between TCP/IP stack, Nifman, ESock is like...
	TestL(iScndCtxSock1.SetOpt(KSoUdpSynchronousSend, KSolInetUdp, 1), KErrNone, _L("SetOpt Sync Send"));
	
	// We must bind to a local address, and connect to a remote address,
	// so that TFT is always generated with the same destination - source ports.
	// Otherwise, SIM.TSY will reject TFT proposed by GUQoS.
	
	TInt scndCtxSockLocalPort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("SecLocalPort"), scndCtxSockLocalPort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(scndCtxSockLocalPort > 0);
	
	TInetAddr localAddr;
	localAddr.SetPort(scndCtxSockLocalPort);
	TestL(iScndCtxSock1.Bind(localAddr), KErrNone, _L(">>>>>Bind the Secondary PDP ctx socket"));
	
	
	TInt scndCtxSockRemotePort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("SecRemotePort"),scndCtxSockRemotePort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(scndCtxSockRemotePort > 0);
	
	TPtrC scndCtxSockRemoteAddr;
	if (!GetStringFromConfig(ConfigSection(), _L("SecRemoteIpAddr"), scndCtxSockRemoteAddr))
	    {
	    User::Leave(KErrNotFound);
	    }

	TInetAddr dstAddr1;
	dstAddr1.SetPort(scndCtxSockRemotePort);
    dstAddr1.Input(scndCtxSockRemoteAddr);
    
    TRequestStatus connStatus;
    iScndCtxSock1.Connect(dstAddr1, connStatus);
	WaitForCompletionL(connStatus, KErrNone, _L(">>>>>>>Connect the Secondary PDP ctx socket"));
    
   	if(waitForQos)
		{
		WaitForCompletionL(subconNotifyStat, KErrNone, _L(">>>>>QoS Negotiation Completion on 2ndary context"));
   		} 
   	else
	   	{
		iSpudScndCtx1.CancelEventNotification();	   		
	   	} 

    TInt sockErr = iPrimCtxSock1.Open(iESock, 
                 KAfInet, 
                 KSockDatagram, 
                 KProtocolInetUdp,
                 iSpud);
    TestL(sockErr, KErrNone, _L(">>>>>>Open Primary PDP ctx socket"));
    
   
    //*************************************************************************************************
    User::After(3 * 1000000); // Give the loopback sockets a chance to bind & open,
    // else we can end up with ICMP Destination (port) Unreachable.
    // When used over WinTunnel, this is completely unnecessary.
    //*************************************************************************************************
    
    WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 1"));
    
    // Must address UDP on primary explicitly.
    TInt primCtxSockRemotePort = 0;
	if (!GetIntFromConfig(ConfigSection(), _L("PrimRemotePort"),primCtxSockRemotePort))
	    {
	    User::Leave(KErrNotFound);
	    }
	ASSERT(primCtxSockRemotePort > 0);

	TPtrC primCtxSockRemoteAddr;
	if (!GetStringFromConfig(ConfigSection(), _L("PrimRemoteIpAddr"), primCtxSockRemoteAddr))
	    {
	    User::Leave(KErrNotFound);
	    }

    TInetAddr primDstAddr1;
    primDstAddr1.SetPort(primCtxSockRemotePort);
    primDstAddr1.Input(primCtxSockRemoteAddr);
    
    SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Send - Receive on Primary PDP Context, 1"));
   
   	
   	WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 2"));
   	SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Primary PDP Context, 2"));
    
   	SendRecvOnSocketL(iPrimCtxSock1, primDstAddr1, _L(">>>>>>>Primary PDP Context, 3"));
   	WriteReadOnSocketL(iScndCtxSock1, _L(">>>>>>>Secondary PDP Context, 3"));
   	
   	iSpud.Stop(); // nukes both contexts
   	// Everything will be cleaned up later.
  	return EPass;
  	}
	
	

TVerdict CUpperFlowOnLowerNifDown::RunTestStepL()
	{
	// Start the primary
	TRequestStatus spudStartReq;
	TCommDbConnPref spudPref;
	spudPref.SetIapId(2);
	OpenAndStartInterfaceL(iSpud, spudPref, spudStartReq, _L(">>>>>Starting SPUD NIF"));
	
	// Start PPP peer for the primary
	RConnection primCtxPppPeer;
	TRequestStatus peerStartReq;
	TCommDbConnPref peerPref;
	peerPref.SetIapId(1);
	OpenAndStartInterfaceL(primCtxPppPeer, peerPref, peerStartReq, _L(">>>>Starting PPP Peer for the primary context"));
	
	WaitForCompletionL(spudStartReq, KErrNone, _L(">>>>>Starting SPUD NIF"));
	WaitForCompletionL(peerStartReq, KErrNone, _L(">>>>>Starting PPP Peer for SPUD primary context"));
	// Open socket on primary
	TInt sockErr = iPrimCtxSock1.Open(iESock, 
                 KAfInet, 
                 KSockDatagram, 
                 KProtocolInetUdp,
                 iSpud);
    
    TestL(sockErr, KErrNone, _L(">>>>>>>>Open UDP Socket, primary PDP context"));
    
    User::After(2 * 10000); // Let everything settle down.  
	// Stop PPP peer, triggerting LinkLayerDown on the primary
	TInt stopErr = primCtxPppPeer.Stop();
	ASSERT(KErrNone == stopErr); // If there is an error, the test was not setup correctly.
	
	// At this point contxt deletion request is outstanding: SIM TSY will not complete it for a while.
	// Send on primary socket: no way to verify where it blocked. Must inspect the logs.
	
	User::After(5 * 1000000); 	
	
	TInetAddr primDstAddr1;
    primDstAddr1.SetPort(1060);
    primDstAddr1.Input(_L("192.168.3.1"));
   	
   	TBuf8<KMaxMsgLen> sendBuf(_L8("A packet to the La-La Land."));
	TRequestStatus sendSt;
    
    iPrimCtxSock1.SendTo(sendBuf, primDstAddr1, 0x0, sendSt);
	WaitForCompletionL(sendSt, KErrNone, _L("Send on Socket")); // ESock completed the send, the packet 
	// still needs to percolate through the tcp/ip stack - GUQoS - SPUD.
	User::After(30 * 1000000);			
	// SPUD panics if the upper layer tries to send on a flowed-off context.
			
	return EPass; // If there was a failure, we'd leave or panic
	// Cleanup everything later
	}
   
 
   
/**
Brings up SPUD & PPP peer from the primary context, then stops Spud.
This test step is intended to be used in a combination with other steps, (including itself),
to verify how RConnection::Stop affects the behaviour of the system. */	
TVerdict CSpudPppPrimaryStop::RunTestStepL()
	{
	TRequestStatus spudStartReq;
	TCommDbConnPref spudPref;
	spudPref.SetIapId(2);
	OpenAndStartInterfaceL(iSpud, spudPref, spudStartReq, _L(">>>>>Starting SPUD NIF"));
			
	
	RConnection primCtxPppPeer;
	TRequestStatus peerStartReq;
	TCommDbConnPref peerPref;
	peerPref.SetIapId(1);
	OpenAndStartInterfaceL(primCtxPppPeer, peerPref, peerStartReq, _L(">>>>Starting PPP Peer for the primary context"));
	
	
	WaitForCompletionL(spudStartReq, KErrNone, _L(">>>>>Starting SPUD NIF"));
	WaitForCompletionL(peerStartReq, KErrNone, _L(">>>>>Starting PPP Peer for SPUD primary context"));

	TInt stopTypeInt = -1;  
	if(!GetIntFromConfig(ConfigSection(), _L("StopType"), stopTypeInt))
	    {
        User::Leave(KErrNotFound);
	    }
		
	RConnection::TConnStopType stopType = static_cast<RConnection::TConnStopType>(stopTypeInt);
	INFO_PRINTF2(_L("Stopping Spud with stop type= %d (0 = EStopNormal, 1 = EStopAuthoritative)"), stopType);
	ASSERT(RConnection::EStopNormal == stopType || RConnection::EStopAuthoritative == stopType);
	TInt stopErr = iSpud.Stop(stopType);
	INFO_PRINTF2(_L("Stopped Spud with error = %d"), stopErr);
	ASSERT(KErrNone == stopErr); // If we have an errror, test was not set up / executed correctly.

	// if stop was authoritative ppp wont do the leaving handshake, so peer also needs to be stopped
	if (stopType == RConnection::EStopAuthoritative)
		{
		TInt stopErr = primCtxPppPeer.Stop(stopType);
		INFO_PRINTF2(_L("Stopped peer with error = %d"), stopErr);
		ASSERT(KErrNone == stopErr); // If we have an errror, test was not set up / executed correctly.
		}
	else
		{
		primCtxPppPeer.Close(); // No need to stop, peer should be shutting down as a result of Stop on Spud.
		}
	return EPass;
	}
	
	
	
	
