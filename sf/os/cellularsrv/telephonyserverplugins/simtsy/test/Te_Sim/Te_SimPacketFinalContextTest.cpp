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

#include "Te_SimPacketFinalContextTest.h"

CSimPacketFinalContextTest::CSimPacketFinalContextTest() 
	{ 
	SetTestStepName(_L("FinalContextTest"));
	}

TVerdict CSimPacketFinalContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketFinalContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	TRequestStatus reqStatus;

	// test RPacketContext::OpenNewContext
	TName contextName;
	TName contextNameCompare;
	TESTL(iFirstPrimaryPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Context object"));

	// test RPacketContext::Activate
	StartNTRasSimulation();
	iFirstPrimaryPacketContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Activate test passed"));

	// test RPacketContext::InitialiseContext
	RPacketContext::TDataChannelV2 channelV2;
	RPacketContext::TDataChannelV2Pckg channelV2pckg(channelV2); 	
	iFirstPrimaryPacketContext.InitialiseContext(reqStatus, channelV2pckg);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.41"));	

	// test RPacketContext::LoanCommPort
	RCall::TCommPort commPort;
	iFirstPrimaryPacketContext.LoanCommPort(reqStatus, commPort);
	iFirstPrimaryPacketContext.CancelAsyncRequest(EPacketContextLoanCommPort);//does nothing
	User::WaitForRequest(reqStatus);
	TESTL(commPort.iCsy==DPCKTTSY_CSY);
	TESTL(commPort.iPort==DPCKTTSY_PORT);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.23"));
	INFO_PRINTF1(_L("Loan Comm Port test passed"));

	_LIT8(KWriteTestData,"SIMULATOR TSY PACKET TEST"); 
	_LIT(KPortName,"LOOPBACK::1");
	_LIT(KCsyName,"LOOPBACK.CSY");

	RCommServ cs;
	
	cs.Connect();
	cs.LoadCommModule(KCsyName);

	RComm port;
	TESTL(port.Open(cs,KPortName,ECommShared)==KErrNone);

	port.Write(reqStatus,KWriteTestData);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	port.Close();
	cs.Close();

	// test RPacketContext::RecoverCommPort
	iFirstPrimaryPacketContext.RecoverCommPort(reqStatus);
	iFirstPrimaryPacketContext.CancelAsyncRequest(EPacketContextRecoverCommPort);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.24"));
	INFO_PRINTF1(_L("Recover Comm Port test passed"));
	
	// test RPacketContext::Deactivate
	iFirstPrimaryPacketContext.Deactivate(reqStatus);
	User::WaitForRequest(reqStatus);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.25"));
	INFO_PRINTF1(_L("Deactivate test passed"));

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
	
	// test RPacketContext::Detach
	iPacketService.Detach(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);
	INFO_PRINTF1(_L("Packet Detach Successful"));

	// End of RPacketContext and RPacketService tests
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

