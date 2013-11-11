// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//  Test steps
//

/**
 @file
 @internalComponent
*/


#include <stdlib.h>
#include <string.h>
//#include <profiler.h>
#include <test/testexecutelog.h>
#include <in_sock.h>
#include "te_mbufgobblertest.h"
#include "mbufgobblertestflags.h"


CMbufGobblerStartupTest::CMbufGobblerStartupTest(CMbufGobblerTestServer* /*aOwner*/)
	{
	SetTestStepName(KTestMbufGobblerStartupTest);
	}

CMbufGobblerStartupTest::~CMbufGobblerStartupTest()
	{
	}
    
TVerdict CMbufGobblerStartupTest::doTestStepL()
	{
#ifdef _DEBUG
    SetTestStepResult( EFail );    
    
    User::LeaveIfError(MbufGobblerTestFlags::ActivateTestFlags());

	INFO_PRINTF1(_L("  .. opening socket server.."));
	RSocketServ sockServ;
    User::LeaveIfError(sockServ.Connect());
    CleanupClosePushL(sockServ);
    
	INFO_PRINTF1(_L("  .. opening connection.."));
	RConnection conn;
    User::LeaveIfError(conn.Open(sockServ));
    CleanupClosePushL(conn);
    User::LeaveIfError(conn.Start()); // start default connection (defined in commsdat)
    
	INFO_PRINTF1(_L("Checking Tier up.."));
    TInt result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KTierUp);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("Tier started ok [%d]."),result);
    
	INFO_PRINTF1(_L("Checking MCPr up.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KMCPrUp);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("MCPr started ok [%d]."),result);

	INFO_PRINTF1(_L("Checking CPr up.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KCPrUp);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("CPr started ok [%d]."),result);

	INFO_PRINTF1(_L("Checking SCPr up.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KSCPrUp);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("SCPr started ok [%d]."),result);

	INFO_PRINTF1(_L("Checking Binder up.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KBind);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("Binder up ok [%d]."),result);

	INFO_PRINTF1(_L("  .. opening socket.."));
    RSocket sock;
	User::LeaveIfError(sock.Open(sockServ,KAfInet,KSockDatagram,KProtocolInetUdp,conn));
    CleanupClosePushL(sock);
	
	INFO_PRINTF1(_L("  .. sending a packet.."));
	TBuf8<128> pkt;
	pkt.SetLength(128);
	TInetAddr dest;
	dest.Input(_L("11.22.33.44"));
	dest.SetPort(100);
	TRequestStatus status;
	sock.SendTo(pkt,dest,0,status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	
	INFO_PRINTF1(_L("Checking Send occurred.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KBinderSend);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("Send occurred ok [%d]."),result);

	INFO_PRINTF1(_L("Checking Receive occurred.."));
    result = MbufGobblerTestFlags::GetResult(MbufGobblerTestFlags::KBinderReceive);
    User::LeaveIfError(result);
	INFO_PRINTF2(_L("Receive occurred ok [%d]."),result);

	// Remove this call if its high capability requirement is a problem
	INFO_PRINTF1(_L("  .. stopping connection.."));
	User::LeaveIfError(conn.Stop());
	
    CleanupStack::Pop(); // RSocket close
    CleanupStack::Pop(); // RConnection close
    CleanupStack::Pop(); // RSocketServ close

	// return success / failure
    SetTestStepResult( EPass );
#else // _DEBUG
    SetTestStepResult( EPass );    
	INFO_PRINTF1(_L("  .. results for this component only useful in UDEB mode."));
#endif    
	return TestStepResult();
	}

