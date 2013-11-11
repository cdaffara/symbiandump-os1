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

#include "Te_SimPacketContextTest.h"

CSimPacketContextTest::CSimPacketContextTest()
	{ 
	SetTestStepName(_L("ContextTest"));
	}

TVerdict CSimPacketContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	// test GetContextInfo - get context info where no contexts created
	TInt index = 0;
	RPacketService::TContextInfo contextInfo;
	TRequestStatus reqStatus, notifyStatus;

	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	iPacketService.CancelAsyncRequest(EPacketGetContextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrArgument); 
	INFO_PRINTF1(_L("Get Info Test Passed - KErrArgument as no context created"));

	TBuf<40> contextId;
	iPacketService.NotifyContextAdded(notifyStatus, contextId);
	iPacketService.CancelAsyncRequest(EPacketNotifyContextAdded);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()== KErrCancel);
	
	TName contextName;
	iPacketService.NotifyContextAdded(notifyStatus, contextName);

	// test RPacketContext::OpenNewContext
	
	TName contextNameCompare;
	ret = iFirstPrimaryPacketContext.OpenNewContext(iPacketService, contextName);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.2"));
	INFO_PRINTF1(_L("Opened Context object"));
		 
	User::WaitForRequest(notifyStatus);
	
	TName anotherContextName;
	TName anotherContextNameCompare;
	TESTL(iSecondPrimaryPacketContext.OpenNewContext(iPacketService, anotherContextName)==KErrNone);
	anotherContextNameCompare.Append(KSimPrimaryPacketContextName);
	anotherContextNameCompare.AppendNum(2);
	TESTL(anotherContextName.Compare(anotherContextNameCompare)==KErrNone);
	CHECKPOINTL(ret,KErrNone,CHP_CNTXT_CASE("E.3"));
	INFO_PRINTF1(_L("Opened another Primary Context object")); 

	// test RPacketService::GetContextInfo - context has been created
	index = 1;
	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	iPacketService.CancelAsyncRequest(EPacketGetContextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.6"));
	INFO_PRINTF1(_L("Get Info Test Passed"));

	// test RPacketService::GetContextInfo() - on deactivated context
	iFirstPrimaryPacketContext.Deactivate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Deactivated context successfully"));
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.7"));

	index = 0;
	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("Get Info Test Passed")); 

	// test RPacketContext::GetStatus
	RPacketContext::TContextStatus contextStatus;
	TESTL(iFirstPrimaryPacketContext.GetStatus(contextStatus) == KErrNone);
	TESTL(contextStatus==RPacketContext::EStatusInactive);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.27"));
	INFO_PRINTF1(_L("GetStatus on inactive context successful"));

	// test RPacketContext::Activate
	iFirstPrimaryPacketContext.Activate(reqStatus);
	iFirstPrimaryPacketContext.CancelAsyncRequest(EPacketContextActivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrCancel);
	CHECKPOINTL(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("E.22"));
	INFO_PRINTF1(_L("Activation Cancel test passed"));

	// test RPacketContext::GetStatus
	TESTL(iFirstPrimaryPacketContext.GetStatus(contextStatus) == KErrNone);
	TESTL(contextStatus==RPacketContext::EStatusInactive);

	// test RPacketContext::NotifyStatusChange 

	// test RPacketContext::Activate
	StartNTRasSimulation();
	iFirstPrimaryPacketContext.NotifyStatusChange(notifyStatus, contextStatus);
	iFirstPrimaryPacketContext.Activate(reqStatus);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.21"));
	TESTL(notifyStatus.Int()==KErrNone);

	INFO_PRINTF1(_L("Activate test passed"));
	// test RPacketContext::GetContextInfo
	index = 0;
	iPacketService.GetContextInfo(reqStatus, index, contextInfo);
	iPacketService.CancelAsyncRequest(EPacketGetContextInfo);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.6"));
	INFO_PRINTF1(_L("Get Info Test Passed"));

	// test RPacketContext::GetStatus
	TESTL(iFirstPrimaryPacketContext.GetStatus(contextStatus) == KErrNone);
	TESTL(contextStatus==RPacketContext::EStatusActive);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.28"));
	INFO_PRINTF1(_L("GetStatus on active context successful"));

	// test cancelling RPacketService::Deactivate() 
	iFirstPrimaryPacketContext.Deactivate(reqStatus);
	iFirstPrimaryPacketContext.CancelAsyncRequest(EPacketContextDeactivate);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("Cancelled Deactivate context successfully"));
	CHECKPOINTL(reqStatus.Int(),KErrCancel,CHP_CNTXT_CASE("E.26"));

	// test RPacketContext::Deactivate
	iFirstPrimaryPacketContext.Deactivate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Deactivate test passed"));

	// Activate PRIMARY2
	StartNTRasSimulation();
	iSecondPrimaryPacketContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Activate PRIMARY2 tested successfully"));

	// test RPacketContext::GetStatus
	TESTL(iSecondPrimaryPacketContext.GetStatus(contextStatus) == KErrNone);
	TESTL(contextStatus==RPacketContext::EStatusActive);
	INFO_PRINTF1(_L("GetStatus on active PRIMARY2 context successful"));

	TUint8 addrArray[RPacketContext::KIPAddressSize] = {0,0,0,0,
														0,0,0,0,
														0,0,255,255,
														102,23,224,132};
	TUint8 subArray[RPacketContext::KIPAddressSize] =	{0,0,0,0,
														0,0,0,0,
														0,0,255,255,
														255,255,255,255};
	RPacketContext::TPacketFilterV2 packetFilterV2;
	packetFilterV2.iId = 0;
	packetFilterV2.iEvaluationPrecedenceIndex = 1;

	for(TInt x = 0;  x<RPacketContext::KIPAddressSize; x++)
		{
		packetFilterV2.iSrcAddr[x] = addrArray[x];
		packetFilterV2.iSrcAddrSubnetMask[x] = subArray[x];
		}

	packetFilterV2.iProtocolNumberOrNextHeader = 1;
	packetFilterV2.iSrcPortMin = 0;
	packetFilterV2.iSrcPortMax = 1;
	packetFilterV2.iDestPortMin = 2;
	packetFilterV2.iDestPortMax = 0;
	packetFilterV2.iIPSecSPI = 0;
	packetFilterV2.iTOSorTrafficClass = 1;
	packetFilterV2.iFlowLabel = 2;

	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);

	iSecondPrimaryPacketContext.AddPacketFilter(reqStatus, packetFilterV2pckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("AddPacketFilter PRIMARY2 Test Passed"));

	// test RPacketContext::ModifyActiveContext
	iSecondPrimaryPacketContext.ModifyActiveContext(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("ModifyActiveContext PRIMARY2 Test Passed"));

	RCall::TCommPort commPort;
	iSecondPrimaryPacketContext.LoanCommPort(reqStatus, commPort);
	iSecondPrimaryPacketContext.CancelAsyncRequest(EPacketContextLoanCommPort);//does nothing
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(commPort.iCsy==DPCKTTSY_CSY);
	TESTL(commPort.iPort==DPCKTTSY_PORT);
	INFO_PRINTF1(_L("Loan Comm Port test passed"));
	
	_LIT8(KWriteTestData,"SIMULATOR TSY PACKET TEST"); 
	_LIT(KPortName,"LOOPBACK::1");
	_LIT(KCsyName,"LOOPBACK.CSY");

	RCommServ cs;
	
	TInt r=cs.Connect();
	r=cs.LoadCommModule(KCsyName);

	RComm port;
	TESTL(port.Open(cs,KPortName,ECommShared)==KErrNone);

	port.Write(reqStatus,KWriteTestData);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	port.Close();
	cs.Close();

	// test RPacketContext::RecoverCommPort
	iSecondPrimaryPacketContext.RecoverCommPort(reqStatus);
	iSecondPrimaryPacketContext.CancelAsyncRequest(EPacketContextRecoverCommPort);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Recover Comm Port test passed"));

	// test RPacketContext::Deactivate
	iFirstPrimaryPacketContext.Deactivate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Deactivate test passed"));


	// test RPacketContext::Detach
	iPacketService.Detach(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);
	INFO_PRINTF1(_L("Packet Detach Cancelled Successfully"));

	// test RPacketContext::Delete
	iFirstPrimaryPacketContext.Delete(reqStatus);
	iFirstPrimaryPacketContext.CancelAsyncRequest(EPacketContextDelete);
	User::WaitForRequest(reqStatus);
	if (reqStatus.Int()==KErrNone)
		{
		INFO_PRINTF1(_L("Delete request completed before its cancelation"));
		}
	else if (reqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF1(_L("Delete Cancel test passed"));
		// successfully canceled, reissue the async. request again
		iFirstPrimaryPacketContext.Delete(reqStatus);
		User::WaitForRequest(reqStatus);
		}
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Delete test passed"));

	iFirstPrimaryPacketContext.Close();
	iSecondPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

