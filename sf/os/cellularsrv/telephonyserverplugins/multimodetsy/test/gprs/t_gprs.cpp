// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// \file t_gprs.cpp
// This file implements all GPRS and Context test cases. The test cases are built
// for using against a Motorola GPRS and Ericsson T39/520 phones.
// All test cases that are in tgprsinter are also in this file.
// 
//

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include "f32file.h"
#include <etel.h>
#include <autotest.h>
#include <es_sock.h>
#include <in_sock.h>
#include "tmain.h"
#include <etelQoS.h>
#include <pcktcs.h>
// Globals
CAutoTest* gAutoTest;

//GLREF_D     RConnection		conn;


// Support functions
LOCAL_C void TestConfigParams(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2);
LOCAL_C TInt ListRoutes(RSocket& aSocket);


void WaitWithTimeout(TRequestStatus& aStatus)
/**	Waits untill end or request or time out
 * @param request itself
 * @param  number Of microseconds
 */
	{
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal() ;

	const TInt KOneSecondInMicroSeconds=1000000L;
	timer.After(timerStatus,120*KOneSecondInMicroSeconds);		// x second timer

	User::WaitForRequest(aStatus, timerStatus);

	// Cancel timer if it did not complete
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		aStatus=-999;		// Hopefully -999 reported in the test result log will make it obvious
								    // that the request was cancelled due to it taking to long
		gTest.Printf(_L("User::WaitForRequest cancelled as it did not complete within a reasonable time\n"));
		}

	timer.Close() ;
	}


LOCAL_C void  GprsDefaultContextParamsTest()
/**
 * This tests the default context params
 */
    {
    RPacketContext::TContextConfigGPRS     setDefaultParams;
    RPacketContext::TContextConfigGPRS     getDefaultParams;

	// Some default params to set, does not matter what it is.
    _LIT(KAccesspoint, "www.Symbian.com");  
    _LIT(KPpdpaddress, "192.168.111.100");

    gTest(gPacketService.Open(gPhone ) == KErrNone);
    
    setDefaultParams.iPdpType = RPacketContext::EPdpTypeIPv4;
    setDefaultParams.iAccessPointName = KAccesspoint;
    setDefaultParams.iPdpAddress = KPpdpaddress;
    setDefaultParams.iPdpCompression = 1;

    TPckg<RPacketContext::TContextConfigGPRS>    setcontextConfigPckg(setDefaultParams);
    TPckg<RPacketContext::TContextConfigGPRS>    getcontextConfigPckg(getDefaultParams);
    gTest(gPacketService.SetDefaultContextParams(setcontextConfigPckg) == KErrNone);
    gTest(gPacketService.GetDefaultContextParams(getcontextConfigPckg) == KErrNone);
    TestConfigParams(setDefaultParams, getDefaultParams);

    gPacketService.Close();
    User::After(500000);
    }


LOCAL_C void  GprsUnSupportedTest()
/**
 * Tests all unsupported GPRS API calls to the TSY.
 */
    {
    TRequestStatus status;
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gPacketService.RejectActivationRequest(status);
	 
	  User::WaitForRequest(status);
	  gTest(status.Int()== KErrNotSupported);

    gPacketService.RejectActivationRequest(status);
    gPacketService.CancelAsyncRequest(EPacketRejectActivationRequest);
    User::WaitForRequest(status);
    gTest(status.Int() == KErrNotSupported);

    RPacketService::TMSClass msClass;
    gPacketService.NotifyMSClassChange(status, msClass);
    User::WaitForRequest(status);
    gTest(status.Int() == KErrNotSupported);

    RPacketService::TDynamicCapsFlags dynCaps;
    gTest(gPacketService.GetDynamicCaps(dynCaps) == KErrNotSupported);

    gPacketService.NotifyDynamicCapsChange(status, dynCaps);
    User::WaitForRequest(status);
    gTest(status.Int() == KErrNotSupported);

    RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    gTest(gPacketService.SetAttachMode(attachMode) == KErrNotSupported);
    gTest(gPacketService.GetAttachMode(attachMode) == KErrNotSupported);

    RPacketService::TRegistrationStatus registrationStatus = RPacketService::ERegisteredRoaming;
    gPacketService.GetNtwkRegStatus(status, registrationStatus);
		User::WaitForRequest(status);
		gTest(status.Int() == KErrNone || status.Int() == KErrNotSupported);

		gPacketService.Close();
	
    User::After(500000);
    }


LOCAL_C void  GprsContextAttachTest()
/**
 * This tests the context attach and detach
 */
    {
    TRequestStatus                      stat1;
    TRequestStatus						stat2;
	TBuf<30>                            contextName;
    RPacketContext::TContextStatus        contextStatus;
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    
    gPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    gPacketContext[0].CancelAsyncRequest(EPacketContextNotifyStatusChange);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrCancel);

    gPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    
	gPacketContext[0].Activate(stat2);
	User::WaitForRequest(stat2);
	gTest(stat2.Int()== KErrNone);
    
	User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);
    gTest(contextStatus == RPacketContext::EStatusActivating);
    gPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);
    gTest(contextStatus == RPacketContext::EStatusActive);
    
    gPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    
		gPacketContext[0].Deactivate(stat2);
		User::WaitForRequest(stat2);
		gTest(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);
    gTest(contextStatus == RPacketContext::EStatusDeactivating);
    gPacketContext[0].NotifyStatusChange(stat1, contextStatus);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);
    gTest(contextStatus == RPacketContext::EStatusInactive);
    
    gPacketContext[0].Close();
    gPacketService.Close();
    User::After(500000);
    }


LOCAL_C void  GprsContextDeleteTest()
/**
 *  This tests the context delete.
 */
    {
    TRequestStatus						stat1;
		TBuf<30>                            contextName;
    RPacketContext::TContextStatus		contextStatus;

    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    
		gPacketContext[0].Delete(stat1);
		User::WaitForRequest(stat1);
		gTest(stat1.Int() == KErrNone);
    
		gPacketContext[0].Activate(stat1);
		User::WaitForRequest(stat1);
		gTest(stat1.Int() == KErrAccessDenied);

    gTest(gPacketContext[0].GetStatus(contextStatus) == KErrNone);
    gTest(contextStatus == RPacketContext::EStatusDeleted);

    gPacketContext[0].Close();
    gPacketService.Close();
    User::After(KOneSecond);
    }   


LOCAL_C void  GprsContextLoanComport()
/**
 * This tests the comport loan.
 */
    {
	TRequestStatus						stat1;
    TBuf<30>                            contextName;
	RCall::TCommPort dataPort;

    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    
    gPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrEtelPortNotLoanedToClient);

    gPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNotReady);

    gPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);

    gPacketContext[0].LoanCommPort(stat1, dataPort);
	User::WaitForRequest(stat1); 
	gTest(stat1.Int() == KErrNone);

    gPacketContext[0].Activate(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrInUse);

    gPacketContext[0].RecoverCommPort(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);

    gPacketContext[0].Deactivate(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);

    gPacketContext[0].Close();
    gPacketService.Close();
    User::After(KOneSecond);
    }   


LOCAL_C void  GprsContextUnSupportedTest()
/**
 *  This tests all GPRS context API calls.
 */
    {
    TRequestStatus            stat1;
    TBuf<30>                  contextName;
    TInt                      err = 0;
	RPacketContext::TDataVolume volume;
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    
    gTest(gPacketContext[0].GetDataVolumeTransferred(volume) == KErrNotSupported);
    gPacketContext[0].NotifyDataTransferred(stat1, volume);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNotSupported);

    gTest(gPacketContext[0].GetLastErrorCause(err) == KErrNotSupported);

    gPacketContext[0].Close();
    gPacketService.Close();
    User::After(KOneSecond);

    }


LOCAL_C void  GprsMsClassTest()
/**
 * This tests the MS Class.
 */
    {
    TRequestStatus stat1;
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    
    RPacketService::TMSClass     currentClass;
    RPacketService::TMSClass     maxClass;
    gPacketService.GetMSClass(stat1, currentClass, maxClass);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
 
	if(GPhoneBrand == EMotorola)
		{
		gPacketService.SetMSClass(stat1, currentClass);
		User::WaitForRequest(stat1);
		gTest(stat1.Int() == KErrNone);
	    gTest(currentClass	== RPacketService::EMSClassSuspensionRequired);
		gTest(maxClass		== RPacketService::EMSClassSuspensionRequired);
		}

    gPacketService.Close();
    }


LOCAL_C void  GprsAttachTestL()
/**
 *  This tests the attach and detach.
 */
    {
    TRequestStatus stat1;
    RPacketService::TStatus status;

    User::LeaveIfError(gPacketService.Open(gPhone));
    
		gPacketService.Attach(stat1);
    User::WaitForRequest(stat1);
    User::LeaveIfError(stat1.Int());
    
		gPacketService.GetStatus(status);
		GLOBAL_CHECKPOINT_COMPARE(status,RPacketService::EStatusAttached,_L("Expected RPacketService::EStatusAttached"));

    gPacketService.Detach(stat1);
    User::WaitForRequest(stat1);
    User::LeaveIfError(stat1.Int());

    gPacketService.GetStatus(status);
		GLOBAL_CHECKPOINT_COMPARE(status,RPacketService::EStatusUnattached,_L("Expected RPacketService::EStatusUnattached"));

    gPacketService.Close();
    User::After(KOneSecond);
    }


LOCAL_C void  GprsNotificationTest()
/**
 * Tests Notify Context added, Tests Notify status
 */
    {
    TRequestStatus stat1,stat2,stat3;
    TBuf<20> context= _L("");
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    TBuf<40> contextId;

    gPacketService.NotifyContextAdded(stat1, contextId);
    gPacketService.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(stat1);
    gTest(stat1.Int()== KErrCancel);
    gPacketService.NotifyContextAdded(stat1, contextId);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, context) == KErrNone);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);

    RPacketService::TStatus gprsStatus;
    gPacketService.NotifyStatusChange(stat2, gprsStatus);
    gPacketService.CancelAsyncRequest(EPacketNotifyStatusChange);
    User::WaitForRequest(stat2);
    gTest(stat2.Int() == KErrCancel);

    gPacketService.NotifyStatusChange(stat2, gprsStatus);
    gPacketService.Attach(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    User::WaitForRequest(stat2);
    gTest(stat2.Int() == KErrNone);
    gTest(gprsStatus == RPacketService::EStatusAttached);

	User::After(1000000);	// Delay to allow phone to settle

    gPacketService.NotifyStatusChange(stat3, gprsStatus);
    gPacketService.Detach(stat1);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    User::WaitForRequest(stat3);
    gTest(stat3.Int() == KErrNone);
    gTest(gprsStatus == RPacketService::EStatusUnattached);

    gPacketContext[0].Close();
    gPacketService.Close();
    User::After(KOneSecond);
    }


LOCAL_C void  GprsContextConfigTest()
/**
 * Test context config.
 */
    {
    TRequestStatus stat1;
	gTest(gPacketService.Open(gPhone ) == KErrNone);
    TBuf<30> contextName;
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    gTest(gPacketContext[1].OpenNewContext(gPacketService, contextName) == KErrNone);
    
    RPacketContext::TContextConfigGPRS     Getparams;
    RPacketContext::TContextConfigGPRS     Setparams;
    TPckg<RPacketContext::TContextConfigGPRS>    GetcontextConfigPckg(Getparams);
    gPacketContext[0].GetConfig(stat1, GetcontextConfigPckg);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    Setparams = Getparams;

    TPckg<RPacketContext::TContextConfigGPRS>    SetcontextConfigPckg(Setparams);
    gPacketContext[0].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    gPacketContext[1].SetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    gPacketContext[0].GetConfig(stat1, SetcontextConfigPckg);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    TestConfigParams(Getparams, Setparams);

	TRequestStatus stat;
	gPacketContext[0].SetConfig(stat, SetcontextConfigPckg);
	gPacketContext[0].CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(stat);
	gTest(stat.Int() == KErrCancel);

	gPacketContext[0].GetConfig(stat, GetcontextConfigPckg);
	gPacketContext[0].CancelAsyncRequest(EPacketContextGetConfig);
	User::WaitForRequest(stat);
	gTest(stat.Int() == KErrCancel);


	Setparams.iPdpType = RPacketContext::TProtocolType(20); // Set out of range
	TPckg<RPacketContext::TContextConfigGPRS>    errorPackage(Setparams);
	gPacketContext[0].SetConfig(stat1, errorPackage);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNotSupported);


    gPacketContext[0].Close();
    gPacketContext[1].Close();
    gPacketService.Close();
    User::After(KOneSecond);
    }


LOCAL_C void  GprsContextConfigNotificationsTest()
/**
 * Test context notifications.
 */
    {
    TRequestStatus                      stat1, stat2;
    TBuf<30>                            contextName;
    RPacketContext::TContextConfigGPRS   params, notifyParams;
    TPckg<RPacketContext::TContextConfigGPRS>  contextConfigPckg(params);
    TPckg<RPacketContext::TContextConfigGPRS>  notifyContextConfigPckg(notifyParams);
    
    gTest(gPacketService.Open(gPhone ) == KErrNone);
    gTest(gPacketContext[0].OpenNewContext(gPacketService, contextName) == KErrNone);
    gTest(gPacketContext[1].OpenNewContext(gPacketService, contextName) == KErrNone);
    gPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);
    gPacketContext[0].CancelAsyncRequest(EPacketContextNotifyConfigChanged);
    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrCancel);

    gPacketContext[0].GetConfig(stat1, contextConfigPckg);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrNone);
    gPacketContext[0].NotifyConfigChanged(stat1, notifyContextConfigPckg);
    
	gPacketContext[0].SetConfig(stat2, contextConfigPckg);
	User::WaitForRequest(stat2);
	gTest(stat2.Int() == KErrNone);

    User::WaitForRequest(stat1);
    gTest(stat1.Int() == KErrNone);
    TestConfigParams(params, notifyParams);
	
    gPacketContext[0].Close();
    gPacketContext[1].Close();
    gPacketService.Close();
    User::After(KOneSecond);
    }


LOCAL_C void  OpenGprsMaxContextsTest()
/**
 *  Tests enumerate contexts, open new context, open existing context.
 */
    {
    TRequestStatus	stat1;
	TBuf<20> contextName = _L("");

    gTest(gPacketService.Open(gPhone ) == KErrNone);
    TInt countContexts;
    TInt maxAllowedContexts;
    gPacketService.EnumerateContexts(stat1, countContexts, maxAllowedContexts);
	User::WaitForRequest(stat1);
    gTest(countContexts == 0);
	if(GPhoneBrand == EMotorola)
		gTest(maxAllowedContexts == 3);
	else if(GPhoneBrand == EEricsson)
		gTest(maxAllowedContexts == 10);
    TInt i = 0;
    RPacketService::TContextInfo contextinfo;

    for(i = 0; i < maxAllowedContexts;i++)
        {
        RPacketContext contextPacket;
        TBuf<30> tmpContextName;
        tmpContextName = _L("GPRS_CONTEXT");
        tmpContextName.AppendNum(i+1);
        gTest(gPacketContext[i].OpenNewContext(gPacketService, contextName) == KErrNone);
        gPacketService.GetContextInfo(stat1, i, contextinfo);
		User::WaitForRequest(stat1);
        gTest(contextinfo.iName == tmpContextName);
        gTest(contextinfo.iStatus == RPacketContext::EStatusUnknown);
        gTest(contextPacket.OpenExistingContext(gPacketService, contextName) == KErrNone);
        gTest(contextName == tmpContextName); 
        contextPacket.Close();
        }
    gTest(gPacketContext[i+5].OpenNewContext(gPacketService, contextName) == KErrOverflow);
	
	gPacketService.GetContextInfo(stat1, i+1, contextinfo);
	User::WaitForRequest(stat1);
	gTest(stat1.Int() == KErrArgument);
    for(i = 0; i < maxAllowedContexts;i++)
        {
        gPacketContext[i].Close(); 
        }
    gPacketService.Close();
	User::After(KOneSecond);
    }



LOCAL_C void GprsCancelTest()
/**
 * This test will call all GPRS API calls 
 * that can be canceled with an outstanding request
 * to the phone.
 */ 
	{
	TRequestStatus	stat;
	
	gTest(gPacketService.Open(gPhone ) == KErrNone);

	gPacketService.Attach(stat);
	gPacketService.CancelAsyncRequest(EPacketAttach);
	User::WaitForRequest(stat);
	gTest(stat.Int() == KErrCancel);

	gPacketService.Detach(stat);
	gPacketService.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(stat);
	gTest(stat.Int() == KErrCancel);

	RPacketService::TMSClass currentClass;
	RPacketService::TMSClass maxClass;

	gPacketService.GetMSClass(stat, currentClass, maxClass);
	gPacketService.CancelAsyncRequest(EPacketGetMSClass);
	User::WaitForRequest(stat);
	gTest((stat.Int() == KErrCancel)||(stat.Int() == KErrNone));

	currentClass = RPacketService::EMSClassSuspensionRequired;
	gPacketService.SetMSClass(stat, currentClass);
	gPacketService.CancelAsyncRequest(EPacketSetMSClass);
	User::WaitForRequest(stat);
	gTest(stat.Int() == KErrCancel);

	gPacketService.Close();
	User::After(KOneSecond);
	}


LOCAL_C void  GprsDownloadWebbPageTestL()

/**
 * This test will open a socket and download a
 * page/file that is specified by KTestCfgHTTPServerIpAddress
 * and KTestCfgHTTPPageAddress
 *
 */

	{
	// Constants used for this test.
	const TInt KWebPort = 80;
	const TPtrC testAddr = testConfigParams->FindAlphaVar(KTestCfgHTTPServerIpAddress,KNullDesC);
	const TPtrC testPage = testConfigParams->FindAlphaVar(KTestCfgHTTPPageAddress,KNullDesC);
	
	HBufC8* iWebPage = HBufC8::NewMaxL(4096);

	
	gTest.Printf(_L("Send HTML request\n"));
	RSocket sock;
	gTest.Printf(_L("Open tcpip socket\n"));
	gTest(sock.Open(gSocketServer, KAfInet, KSockStream, KProtocolInetTcp) == KErrNone);

	TInetAddr addr(KWebPort);
	addr.Input(testAddr );
	TRequestStatus stat;

	gTest.Printf(_L("Connecting to socket\n"));
	sock.Connect(addr, stat);
	WaitWithTimeout(stat);
	User::LeaveIfError(stat.Int());

	// List the route to the server.
	gTest.Printf(_L("List the route to the server\n"));
	ListRoutes(sock);

	TPtr8 des(iWebPage->Des());
	des.Copy(_L("GET"));
	des.Append(testPage);
	des.Append(_L(" \r\n\r\n"));

	gTest.Printf(_L("Writing data to socket\n"));
	sock.Write(*iWebPage, stat);
	WaitWithTimeout(stat);
	User::LeaveIfError(stat.Int());
	gTest.Printf(_L("Sent request OK\n"));

	TSockXfrLength HTTPHeaderLen;
	TInt rcount = 0;
	TTime startTime;
	startTime.UniversalTime();
	FOREVER
		{
		des.Zero();
		sock.RecvOneOrMore(des,0,stat,HTTPHeaderLen);
		User::WaitForRequest(stat);//, readStat);
		if(stat!=KErrNone)
			break;
		rcount += iWebPage->Length();
		gTest.Console()->SetPos(0);
		gTest.Printf(_L("\rRecved:%10d Bytes"), rcount);
		}

	if(stat.Int() != KErrEof)
		User::Leave(KErrEof);

	// Calculate the number of bytes per second.
	TTime stopTime;
	stopTime.UniversalTime();
	TTimeIntervalMicroSeconds totalTime = stopTime.MicroSecondsFrom(startTime);
	TInt64 byteRate(rcount);
	byteRate *= KOneSecond;
	byteRate /=totalTime.Int64();
	gTest.Printf(_L("\n"));
	gTest.Printf(_L("Rate: %d B/s\n"), I64INT(byteRate));
	gTest.Printf(_L("Received HTML page OK\n"));

	// Close the socket.
	sock.Close();
	// Stop the agent so that the we will be disconnected from the network.
//	gGenericAgent.Stop();
	conn.Stop();
	
	// Delete the webpage buffer.
	delete iWebPage;
	User::After(KFiveSeconds);
	}


LOCAL_C void TestGprsfaxCancel()
/**
 * This function tests the opening of 2 call objects
 * and doing a hang-up on them while there is a 
 * GPRS object in the system.
 */
	{
    TBuf<100>                           Context;
    TRequestStatus stat1, stat2;
    Context = _L("");
    _LIT(KFaxLineName,"Fax");
    _LIT(KVoiceLineName,"Voice");

    gTest(gPacketService.Open(gPhone ) == KErrNone);
    RLine faxLine;
    RCall call;
	RCall call2;
    RLine voiceLine;
    TName callName;
    faxLine.Open(gPhone ,KFaxLineName);
    voiceLine.Open(gPhone ,KVoiceLineName);
    gTest(gPhone .Initialise()==KErrNone);
    faxLine.NotifyIncomingCall(stat1,callName);
	faxLine.NotifyIncomingCallCancel();
	voiceLine.NotifyIncomingCall(stat1,callName);
    voiceLine.NotifyIncomingCallCancel();
    User::WaitForRequest(stat1);
	User::WaitForRequest(stat2);
    gTest(call.OpenNewCall(voiceLine)==KErrNone);
    gTest(call.HangUp()==KErrNone);
    gTest(call2.OpenNewCall(faxLine)==KErrNone);
    gTest(call2.HangUp()==KErrNone);

    call.Close();
	call2.Close();
    voiceLine.Close();
    faxLine.Close();
    gPacketService.Close();
	}


LOCAL_C void  TestOpenContextByNameL()
/**
 * This test should verify that defect SHY-54SNHT is fixed.
 */
	{
	User::LeaveIfError(gPacketService.Open(gPhone));
  TBuf<30> contextName;
	User::LeaveIfError(gPacketContext[0].OpenNewContext(gPacketService,contextName));
	User::LeaveIfError(gPacketContext[1].OpenExistingContext(gPacketService,contextName));
	gPacketService.Close();
	}


LOCAL_C void  TestNetworkRegStatusL()
/**
 * This test does a basic test on the Get Current Network 
 * Registration Status functionality.
 */
	{
	User::LeaveIfError(gPacketService.Open(gPhone));
	RPacketService::TRegistrationStatus regStatus;
	TRequestStatus status;
	gPacketService.GetNtwkRegStatus(status,regStatus);
	User::WaitForRequest(status);
	if(status!=KErrNone && status!=KErrNotSupported)		// Do not consider KErrNotSupported a failure
		{
		gPacketService.Close();
		User::Leave(status.Int());
		}
	gPacketService.Close();
	}


LOCAL_C void TestGetStaticCapsL()
/**
 * Simple test for RPacketService::GetStaticCaps
 */
 	{
	User::LeaveIfError(gPacketService.Open(gPhone));

	//
	// Call RPacketService::GetStaticCaps with EPdpTypeIPv6
	// We would expect this call to fail with KErrNotSupported
	TUint caps(0);		// Create and initialise caps
	TInt ret=gPacketService.GetStaticCaps(caps,RPacketContext::EPdpTypeIPv6);
	GLOBAL_CHECKPOINT_COMPARE(ret,KErrNotSupported,_L("Expected KErrNotSupported for GetStaticCaps with EPdpTypeIPv6"));

	//
	// Call RPacketService::GetStaticCaps
	caps=0;		// Intialise caps
	User::LeaveIfError(gPacketService.GetStaticCaps(caps,RPacketContext::EPdpTypeIPv4));

	//
	// Validate caps returned by GetStaticCaps.
	// (we know what the values should be as they specified in the MMTSY design document
	//  are harcoded in the MMTSY)
	GLOBAL_CHECKPOINT_COMPARE(caps,RPacketService::KCapsSetDefaultContextSupported|RPacketService::KCapsMSClassSupported,_L("Unexpcted Static Caps"));

	gPacketService.Close();
	}


LOCAL_C void TestOpenExistingContextL()
/**
 * Simple test for RPacketService::OpenExistingContext.
 * This test should verify the fix for defect SHY-54SNHT.
 * If SHY-54SNHT has not been fixed an access violation will occur in Etel during the 
 * OPenExistingContext call.
 */
	{
	RPacketService packetService;
	User::LeaveIfError(packetService.Open(gPhone));

	//
	// Open context using an invalid context name (call should fail with KErrNotFound)
	RPacketContext context;
	_LIT(KUnknownContextName,"NonExistantContext");
	TInt ret=context.OpenExistingContext(packetService,KUnknownContextName);
	GLOBAL_CHECKPOINT_COMPARE(ret,KErrNotFound,_L("RPacketContext::OpenExistingContext did not fail when it should have"));

	packetService.Close();
	}



LOCAL_C void TestConfigParams(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2)
	{
	gTest(aArg1.iPdpType                      == aArg2.iPdpType);
    gTest(aArg1.iAccessPointName              == aArg2.iAccessPointName );
    gTest(aArg1.iPdpCompression               == aArg2.iPdpCompression);
	gTest(aArg1.iPdpAddress                   == aArg2.iPdpAddress);
	gTest(aArg1.iAnonymousAccessReqd          == aArg2.iAnonymousAccessReqd);
	}

const TAutoTestCase KGPRSTestCases[] =
/**
 * List of all test cases. It consists of a function pointer and a text label.
 */
	{   
  {GprsDefaultContextParamsTest,        _S("GprsDefaultContextParamsTest ")},
  {GprsUnSupportedTest,                 _S("GprsUnSupportedTest ")},  
  {GprsContextAttachTest,               _S("GprsContextAttachTest ")},        
  {GprsContextDeleteTest,               _S("GprsContextDeleteTest ")},        
  {GprsContextLoanComport,              _S("GprsContextLoanComport ")},       
  {GprsContextUnSupportedTest,          _S("GprsContextUnSupportedTest ")},   
  {GprsAttachTestL,                      _S("GprsAttachTest ")},
  {GprsNotificationTest,                _S("GprsNotificationTest ")},
  {GprsMsClassTest,                     _S("GprsMsClassTest ")},
  {GprsContextConfigTest,               _S("GprsContextConfigTest ")},
  {GprsContextConfigNotificationsTest,  _S("GprsContextConfigNotificationsTest ")},
  {OpenGprsMaxContextsTest,             _S("OpenGprsMaxContextsTest")},
	{GprsCancelTest,			          _S("GprsCancelTest ")},
	{TestGprsfaxCancel,					 _S("TestGprsfaxCancel ")},
	{TestOpenContextByNameL,					 _S("TestOpenContextByName")},
	{TestNetworkRegStatusL,					 _S("TestNetworkRegStatus")},
	{TestGetStaticCapsL,					 _S("TestGetStaticCaps")},
	{TestOpenExistingContextL,					 _S("TestOpenExistingContext")},
	{GprsDownloadWebbPageTestL,           _S("GprsDownloadWebbPageTest ")},
	};


GLDEF_C void AutoTestExecute()
    {
    _LIT(KGPRSTestReport, "gprs_testreport.txt");
    _LIT8(KGPRSCompInfo, "MM.TSY");
    GLOBAL_AUTOTEST_EXECUTE(KGPRSTestCases, KGPRSTestReport, KGPRSCompInfo, gTest );
    }


LOCAL_C TInt ListRoutes(RSocket& aSocket)
 
/**
 * This function lists all the hops that the packet 
 * needs to make to reach the port on the server.
 */

	{

	TPckgBuf<TSoInetRouteInfo> opt;
	TInt ret;
	ret = aSocket.SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl);
	if (ret!=KErrNone)
		return ret;

	gTest.Printf(_L("  Destination      NetMask        Gateway      Interface  State Metric Type\r\n"));
	while (ret==KErrNone)
		{
		ret = aSocket.GetOpt(KSoInetNextRoute, KSolInetRtCtrl, opt);
		if (ret!=KErrNone)
			continue;
		TBuf<40> ia, ga, da, nm;
		opt().iDstAddr.Output(da);
		opt().iNetMask.Output(nm);
		opt().iGateway.Output(ga);
		opt().iIfAddr.Output(ia);
		gTest.Printf(_L("%15S %15S %15S %15S %d %d %d\r\n"), &da, &nm, &ga, &ia, opt().iState, opt().iMetric, opt().iType);
		}
	return ret==KErrEof ? KErrNone : ret;
	}


