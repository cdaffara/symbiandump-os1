// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains implementations of the RConnectionUpsStep classes that test the
// IP Networking User Prompting Functionality added in PREQ1116.
// Each class encapsulates a complete test case and is derived from TE_RConnectionUpsStep
//
//

/**
 @file 
*/

#include "TE_RConnectionUpsTestSteps.h"
#include <test/upstestnotifierproperties.h>
#include <comms-infras/esocktestutils.h>
#include <test/es_dummy.h>

// Location of DummyNif in Network Table in rconnectionudp.xml
const TInt KDummyNifNetworkId = 3;


enum TVerdict TE_RConnectionUPSCloseInterfaces::doTestStepPreambleL()
{
	return EPass;
}

enum TVerdict TE_RConnectionUPSCloseInterfaces::doTestStepPostambleL()
{
	return EPass;
}

TE_RConnectionUPSCloseInterfaces::~TE_RConnectionUPSCloseInterfaces()
{
}

enum TVerdict TE_RConnectionUPSCloseInterfaces::doTestStepL(void)
/**
 * This test step results in any open interfaces being stopped.
 * The test step is usually invoked after every test case, when ESOCK needs
 * to be shut down and restarted. Running this step speeds up the ESOCK
 * shutdown process.
 */
{
	RSocketServ ss;
	
	ss.Connect();
	CleanupClosePushL(ss);
	
	TInt ret = KErrNone;
	if (KErrNone != WaitForAllInterfacesToCloseL(ss))
		{
		INFO_PRINTF1(_L("Previous test case has left connections up that have not closed. Stopping them."));
		TBuf<200> errDesc;
		if ((ret = ESockTestUtils::StopAllInterfaces(errDesc)) != KErrNone)
			{
			INFO_PRINTF3(_L("StopAllInterfaces() ret=%d\n%S"), ret, &errDesc);
			}			
		}
	ss.Close();
	CleanupStack::Pop();
	
	if (ret != KErrNone)
			{
			return EFail;
			}
	
	return EPass;
}


enum TVerdict TE_RConnectionUPSSetNotifFileOverride::doTestStepPreambleL()
{
	return EPass;
}

enum TVerdict TE_RConnectionUPSSetNotifFileOverride::doTestStepPostambleL()
{
	return EPass;
}

TE_RConnectionUPSSetNotifFileOverride::~TE_RConnectionUPSSetNotifFileOverride()
{
}

enum TVerdict TE_RConnectionUPSSetNotifFileOverride::doTestStepL(void)
/**
 * This test step is used set a P&S property which causes 
 * the UPS test Notifier to override any setting which is 
 * has fetched from its configuration file, with the P&S
 * values set by this test harness.
 *
 * This test step is necessary to enable running of this
 * test harness in regression test environments where a
 * UPS test notifier config file is present.
 */

{
	User::LeaveIfError(RProperty::Set(KUidPSUPSTestNotifCategory, KUtFileOverride, KFileOverride));
	return EPass;
}


enum TVerdict TE_RConnectionUPSUnsetNotifFileOverride::doTestStepPreambleL()
{
	return EPass;
}

enum TVerdict TE_RConnectionUPSUnsetNotifFileOverride::doTestStepPostambleL()
{
	return EPass;
}

TE_RConnectionUPSUnsetNotifFileOverride::~TE_RConnectionUPSUnsetNotifFileOverride()
{
}

enum TVerdict TE_RConnectionUPSUnsetNotifFileOverride::doTestStepL(void)
/**
 * This test step is used to unset a P&S property which causes 
 * the UPS test Notifier to override any setting which is 
 * has fetched from its configuration file, with the P&S
 * values set by this test harness.
 *
 * This test step results in the UPS test notifier using settings
 * from its configuration file, if one is present
 */
{
	User::LeaveIfError(RProperty::Set(KUidPSUPSTestNotifCategory, KUtFileOverride, KNoFileOverride));
	return EPass;
}



/*************************************
 *                                   *
 *      Implicit API Call Tests      *
 *                                   *
 *************************************/

TE_RConnectionUPSTest1a::~TE_RConnectionUPSTest1a()
{
}

enum TVerdict TE_RConnectionUPSTest1a::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect() on a TCP socket
 */
{	
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;
		
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest1a



TE_RConnectionUPSTest1b::~TE_RConnectionUPSTest1b()
{
}

enum TVerdict TE_RConnectionUPSTest1b::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() on a UDP socket
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest1b



TE_RConnectionUPSTest1c::~TE_RConnectionUPSTest1c()
{
}

enum TVerdict TE_RConnectionUPSTest1c::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByName()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest1c



TE_RConnectionUPSTest1d::~TE_RConnectionUPSTest1d()
{
}

enum TVerdict TE_RConnectionUPSTest1d::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByAddress()
 */ 
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionUPSTest1d



TE_RConnectionUPSTest2a::~TE_RConnectionUPSTest2a()
{
}

enum TVerdict TE_RConnectionUPSTest2a::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect() followed by another Implicit
 * RSocket::Connect()
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;	
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	
	RSocket sock2;
	err = OpenTcpSocket(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock2);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock2
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest2a



TE_RConnectionUPSTest2b::~TE_RConnectionUPSTest2b()
{
}

enum TVerdict TE_RConnectionUPSTest2b::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() followed by another Implicit
 * RSocket::SendTo()
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	
	RSocket sock2;
	err = OpenUdpSocketL(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop(); //sock2

	DestroyUdpSocket(sock);
	CleanupStack::Pop(); //sock

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest2b



TE_RConnectionUPSTest2c::~TE_RConnectionUPSTest2c()
{
}

enum TVerdict TE_RConnectionUPSTest2c::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByName() followed by an Implicit
 * Asynchronous RHostResolver::GetByName()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	

	RHostResolver hr2;
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeNameRequest(hr2, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr2);
	CleanupStack::Pop(); //hr2

	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest2c



TE_RConnectionUPSTest2d::~TE_RConnectionUPSTest2d()
{
}

enum TVerdict TE_RConnectionUPSTest2d::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByAddress() followed by an Implicit
 * Asynchronous RHostResolver::GetByAddress()
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RHostResolver hr;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);


	RHostResolver hr2;
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeAddressRequest(hr2, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);


	DestroyHostResolver(hr2);
	CleanupStack::Pop(); //hr2
	
	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();

} // TE_RConnectionUPSTest2d



TE_RConnectionUPSTest3a::~TE_RConnectionUPSTest3a()
{
}

enum TVerdict TE_RConnectionUPSTest3a::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect(), close RSocket, followed by
 * Implicit RSocket2::Connect()
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RSocket is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	//Close the first Socket object
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock


	RSocket sock2;
	err = OpenTcpSocket(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	err = DestroyTcpSocket(sock2);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock2

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest3a



TE_RConnectionUPSTest3b::~TE_RConnectionUPSTest3b()
{
}

enum TVerdict TE_RConnectionUPSTest3b::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo(), close RSocket, followed by
 * Implicit RSocket2::SendTo()
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RSocket is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	//Close the first Socket object
	DestroyUdpSocket(sock);
	CleanupStack::Pop(); //sock


	RSocket sock2;
	err = OpenUdpSocketL(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop(); //sock2

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest3b



TE_RConnectionUPSTest3c::~TE_RConnectionUPSTest3c()
{
}

enum TVerdict TE_RConnectionUPSTest3c::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByName(), close RHostResolver,
 * followed by implicit Asynchronous RHostResolver2::GetByName()
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RHostResolver is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
		
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	//Close the first HostResolver object
	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr
	

	RHostResolver hr2;	
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeNameRequest(hr2, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr2);
	CleanupStack::Pop(); //hr2
	
	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss
	
	return TestStepResult();
} // TE_RConnectionUPSTest3c


TE_RConnectionUPSTest3d::~TE_RConnectionUPSTest3d()
{
}

enum TVerdict TE_RConnectionUPSTest3d::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByAddress(), close RHostResolver,
 * followed by implicit Asynchronous RHostResolver2::GetByAddress()
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RHostResolver is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
		
	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr
		
	
	RHostResolver hr2;	
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeAddressRequest(hr2, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr2);
	CleanupStack::Pop(); //hr2
	
	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss
	
	return TestStepResult();
} // TE_RConnectionUPSTest3d


TE_RConnectionUPSTest4a::~TE_RConnectionUPSTest4a()
{
}

enum TVerdict TE_RConnectionUPSTest4a::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect(), followed by an Implicit 
 * RHostResolver::GetByName()
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;	
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RHostResolver hr;	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest4a


TE_RConnectionUPSTest4c::~TE_RConnectionUPSTest4c()
{
}

enum TVerdict TE_RConnectionUPSTest4c::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByName(), followed by an Implicit 
 * RHostResolver2::GetByAddress()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);


	RHostResolver hr2;
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeAddressRequest(hr2, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr2);
	CleanupStack::Pop(); //hr2

	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest2c



TE_RConnectionUPSTest5a::~TE_RConnectionUPSTest5a()
{
}

enum TVerdict TE_RConnectionUPSTest5a::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect(), followed by an Implicit 
 * RHostResolver::GetByAddress()
 */

{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;	
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RHostResolver hr;
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop(); //hr
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop(); //sock

	CloseSocketServer(ss);
	CleanupStack::Pop(); //ss

	return TestStepResult();
} // TE_RConnectionUPSTest5a



TE_RConnectionUPSTest6a::~TE_RConnectionUPSTest6a()
{
}

enum TVerdict TE_RConnectionUPSTest6a::doTestStepL(void)
/**
 * Test prompting behaviour when calling: 
 * Implicit RSocket::Connect(), followed by RConnection::Start
 */
{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;
		
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn;
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);

	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest6a



TE_RConnectionUPSTest6b::~TE_RConnectionUPSTest6b()
{
}

enum TVerdict TE_RConnectionUPSTest6b::doTestStepL(void)
/**
 * Test prompting behaviour when calling: 
 * Implicit RSocket::SendTo(), followed by RConnection::Start
 */

{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn;
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest6b



TE_RConnectionUPSTest6c::~TE_RConnectionUPSTest6c()
{
}

enum TVerdict TE_RConnectionUPSTest6c::doTestStepL(void)
/**
 * Test prompting behaviour when calling: 
 * Implicit Synchronous RHostResolver::GetByName(), followed by RConnection::Start
 */

{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
		
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn;

	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));

	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest6c



TE_RConnectionUPSTest6d::~TE_RConnectionUPSTest6d()
{
}

enum TVerdict TE_RConnectionUPSTest6d::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit Synchronous RHostResolver::GetByAddress(), followed 
 * by RConnection::Start
 */

{
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn;
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionUPSTest6d


TE_RConnectionUPSTest7a::~TE_RConnectionUPSTest7a()
{
}

enum TVerdict TE_RConnectionUPSTest7a::doTestStepL(void)
{
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::Connect(), close RSocket, Delete UPS decision
 * Database, followed by RSocket2::Connect()
 *
 * This test should behave differently when run in NetworkLifetime
 * mode and decision from Notifier is Always/Never or Session based - 
 * Closing the initial RSocket clears the decision which gets stored
 * in the NetUPS component, while deleting the UPS Decision DB should
 * clear any persistent storage of any decision from the UPS DB
 */
 
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;
		
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();
	
	//Delete entries in the DecisionDB
	err = DeleteUpsDB();
	

	RSocket sock2;

	err = OpenTcpSocket(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);

	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock2, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock2);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest7a



TE_RConnectionUPSTest7b::~TE_RConnectionUPSTest7b()
{
}

enum TVerdict TE_RConnectionUPSTest7b::doTestStepL(void)
{
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo(), close RSocket, Delete UPS decision
 * Database, followed by RSocket2::SendTo()
 *
 * This test should behave differently when run in NetworkLifetime
 * mode and decision from Notifier is Always/Never or Session based - 
 * Closing the initial RSocket clears the decision which gets stored
 * in the NetUPS component, while deleting the UPS Decision DB should
 * clear any persistent storage of any decision from the UPS DB
 */
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	//Delete entries in the DecisionDB
	err = DeleteUpsDB();
	

	RSocket sock2;

	err = OpenUdpSocketL(sock2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock2);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest7b



TE_RConnectionUPSTest7c::~TE_RConnectionUPSTest7c()
{
}

enum TVerdict TE_RConnectionUPSTest7c::doTestStepL(void)
{
/**
 * Test prompting behaviour when calling:
 * Implicit RHostResolver::GetByName(), close RSocket, Delete UPS decision
 * Database, followed by RHostResolver2::GetByName()
 *
 * This test should behave differently when run in NetworkLifetime
 * mode and decision from Notifier is Always/Never or Session based - 
 * Closing the initial RHostResolver clears the decision which gets stored
 * in the NetUPS component, while deleting the UPS Decision DB should
 * clear any persistent storage of any decision from the UPS DB
 */
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	//Delete entries in the DecisionDB
	err = DeleteUpsDB();
	

	RHostResolver hr2;

	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);

	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr2, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyHostResolver(hr2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest7c



TE_RConnectionUPSTest7d::~TE_RConnectionUPSTest7d()
{
}

enum TVerdict TE_RConnectionUPSTest7d::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RHostResolver::GetByAddress(), close RSocket, Delete UPS decision
 * Database, followed by RHostResolver2::GetByAddress()
 *
 * This test should behave differently when run in NetworkLifetime
 * mode and decision from Notifier is Always/Never or Session based - 
 * Closing the initial RHostResolver clears the decision which gets stored
 * in the NetUPS component, while deleting the UPS Decision DB should
 * clear any persistent storage of any decision from the UPS DB
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	//Delete entries in the DecisionDB
	err = DeleteUpsDB();
	

	RHostResolver hr2;
	
	err = OpenHostResolver(hr2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr2);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetAddress()
	err = MakeSynchronousAddressRequest(hr2, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest7d





/*************************************
 *                                   *
 *      Explicit API Call Tests      *
 *                                   *
 *************************************/

TE_RConnectionUPSTest10::~TE_RConnectionUPSTest10()
{
}

enum TVerdict TE_RConnectionUPSTest10::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));

	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest10



TE_RConnectionUPSTest11::~TE_RConnectionUPSTest11()
{
}

enum TVerdict TE_RConnectionUPSTest11::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an explicit RSocket::SendTo();
 */
{	
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest11



TE_RConnectionUPSTest12::~TE_RConnectionUPSTest12()
{
}

enum TVerdict TE_RConnectionUPSTest12::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an explicit RSocket::Connect();
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
		
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenTcpSocketExplicit(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest12



TE_RConnectionUPSTest13::~TE_RConnectionUPSTest13()
{
}

enum TVerdict TE_RConnectionUPSTest13::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an explicit RHostResolver::GetByName();
 */
{
	// Test203 TE_RConnectionNtRasTestSteps.cpp te_rconnectionhostresolvertests_Connection.script
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RHostResolver hr;
	
	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();


	return TestStepResult();
} // TE_RConnectionUPSTest13



TE_RConnectionUPSTest14::~TE_RConnectionUPSTest14()
{
}

enum TVerdict TE_RConnectionUPSTest14::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an explicit 
 * RHostResolver::GetByAddress();
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RHostResolver hr;
	
	err = OpenHostResolverExplicit(hr, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest14



TE_RConnectionUPSTest15::~TE_RConnectionUPSTest15()
{
}

enum TVerdict TE_RConnectionUPSTest15::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an explicit RSocket::SendTo(),
 * followed by an explicit RSocket2::SendTo()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);


	RSocket sock2;
	
	err = OpenUdpSocketExplicitL(sock2, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	err = TestUdpDataPathL(sock2, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock2);
	CleanupStack::Pop();
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest15



TE_RConnectionUPSTest16::~TE_RConnectionUPSTest16()
{
}

enum TVerdict TE_RConnectionUPSTest16::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by RConnection2::Start(),
 * followed by an explicit RSocket::SendTo() using RConnection2
 */
{	
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn2);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest16



TE_RConnectionUPSTest17::~TE_RConnectionUPSTest17()
{
}


enum TVerdict TE_RConnectionUPSTest17::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start(), close RConnection, RConnection2::Start(),
 * followed by an explicit RSocket::SendTo() on RConnection2
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RConnection is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn);
	CleanupStack::Pop();
	

	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn2);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest17


TE_RConnectionUPSTest18::~TE_RConnectionUPSTest18()
{
}


enum TVerdict TE_RConnectionUPSTest18::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start(), close RConnection, Delete UPS Decision DB,
 * Followed by an RConnection2::Start(), followed by an explicit 
 *
 * This test should behave differently when run in NetworkLifetime
 * mode and decision from Notifier is Always/Never or Session based - 
 * Closing the initial RConnection clears the decision which gets stored
 * in the NetUPS component, while deleting the UPS Decision DB should
 * clear any persistent storage of any decision from the UPS DB
 */
{

	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	DeleteUpsDB();

	
	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn2);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn2);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest18



TE_RConnectionUPSTest19::~TE_RConnectionUPSTest19()
{
}

enum TVerdict TE_RConnectionUPSTest19::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit RSocket::SendTo();
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest19


TE_RConnectionUPSTest20::~TE_RConnectionUPSTest20()
{
}


enum TVerdict TE_RConnectionUPSTest20::doTestStepL(void)
{
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit RSocket::Connect();
 */
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest20



TE_RConnectionUPSTest21::~TE_RConnectionUPSTest21()
{
}

enum TVerdict TE_RConnectionUPSTest21::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit 
 * RHostResolver::GetByName()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RHostResolver hr;
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest21



TE_RConnectionUPSTest22::~TE_RConnectionUPSTest22()
{
}

enum TVerdict TE_RConnectionUPSTest22::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit 
 * RHostResolver::GetByAddress()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);


	RHostResolver hr;
	
	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Get the NotifyCount before the RHostResolver::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RHostResolver::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest22



TE_RConnectionUPSTest23::~TE_RConnectionUPSTest23()
{
}

enum TVerdict TE_RConnectionUPSTest23::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit RSocket::SendTo(), 
 * followed by RConnection2::Start(), followed by 
 * RConnection3::Start()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	

	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn2);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn3;
	
	err = OpenConnection(conn3, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn3);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn3);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn3);
	CleanupStack::Pop();
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest23



TE_RConnectionUPSTest24::~TE_RConnectionUPSTest24()
{
}

enum TVerdict TE_RConnectionUPSTest24::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start() followed by an implicit RSocket::SendTo(), 
 * followed by RConnection2::Start(). Close all RSockets and RConnections,
 * followed by RConnection3::Start().
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	RConnection conn1;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	err = OpenConnection(conn1, ss);
	TESTEL(KErrNone == err, err); 
	CleanupClosePushL(conn1);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	// Start 2 connections simultaneously - simulate 
	// starting the connections from 2 different processes simultaneously.
	
	TRequestStatus status;
	TRequestStatus status1;
	conn.Start(status);
	conn1.Start(status1);
	//_PRINTF1(_L("Starting Connection"));
	User::WaitForRequest(status);
	User::WaitForRequest(status1);
	TESTL(KErrNone==CheckResult(status.Int()));
	TESTL(KErrNone==CheckResult(status1.Int()));
	
	CloseConnection(conn1);		
	CleanupStack::Pop();
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RSocket sock;
	
	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn2);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	RConnection conn3;
	
	err = OpenConnection(conn3, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn3);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn3);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn3);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest24



TE_RConnectionUPSTest25::~TE_RConnectionUPSTest25()
{
}

enum TVerdict TE_RConnectionUPSTest25::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start(), followed by RConnection2::Attach()
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	
	RConnection conn2;

	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	TUint numOfConnections;
	TPckgBuf<TConnectionInfo> connInfo;
	
	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);
	
	err = conn.GetConnectionInfo(1, connInfo);
	TESTEL(KErrNone == err, err);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Attach()
	err = AttachNormal(conn2, connInfo);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest25



TE_RConnectionUPSTest26::~TE_RConnectionUPSTest26()
{
}

enum TVerdict TE_RConnectionUPSTest26::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * RConnection::Start(), close RConnection (the interface should still
 * stay up) followed by RConnection2::Attach()
 *
 * This test should behave differently in NetworkLifetime Mode
 * when Notifier response is Session based, since decision is no
 * longer valid once first RConnection is closed i.e. once all client 
 * associations with connections cease to exist
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start()
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn);
	CleanupStack::Pop();
			
	RConnection conn2;
	
	err = OpenConnection(conn2, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn2);
	
	TUint numOfConnections;
	TPckgBuf<TConnectionInfo> connInfo;
	
	// need to call Enumerate() before GetConnectionInfo() to set up array used there
	err = EnumerateConnections(conn2, numOfConnections);
	TESTEL(KErrNone == err, err);
	TESTEL(1 == numOfConnections, numOfConnections);
	
	err = conn2.GetConnectionInfo(1, connInfo);
	TESTEL(KErrNone == err, err);
	
	//Get the NotifyCount before the RConnection::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Attach()
	err = AttachNormal(conn2, connInfo);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn2);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();

} // TE_RConnectionUPSTest26




/*************************************
 *                                   *
 *     Test steps to be run in       *
 *  Multi-Threaded / MultiProcess    *
 *              Manner               *
 *                                   *
 *************************************/

enum TVerdict TE_RConnectionUPSPreMulti::doTestStepPreambleL()
/**
 * Basic Preamble - Only get the UPS test config data and not the
 * additional RConnection test config data
 */
{
	TESTL(GetConfigData() == KErrNone);
	return TestStepResult();
}

TE_RConnectionUPSPreMulti::~TE_RConnectionUPSPreMulti()
{
}

enum TVerdict TE_RConnectionUPSPreMulti::doTestStepL(void)
/**
 * Gets and stores the NotifyCount
 * This test step is intended to be the first step to be run
 * when performing multi-threaded / multi-process 
 * tests
 */
{
	//Get the NotifyCount before the main test step is run
	TInt notifyCount = GetNotifyCount();
	
	INFO_PRINTF1(_L(" "));
	INFO_PRINTF2(_L("NotifyCount before executing MultiThreaded test steps = ( %d )"), notifyCount);
	INFO_PRINTF1(_L(" "));
	
	//Store the NotifyCount
	SetStoredNotifyCount(notifyCount);
	return EPass;
}


enum TVerdict TE_RConnectionUPSPostMulti::doTestStepPreambleL()
/**
 * Basic Preamble - Only get the UPS test config data and not the
 * additional RConnection test config data
 */
{
	TESTL(GetConfigData() == KErrNone);
	return TestStepResult();
}

TE_RConnectionUPSPostMulti::~TE_RConnectionUPSPostMulti()
{
}

enum TVerdict TE_RConnectionUPSPostMulti::doTestStepL(void)
/**
 * Retieves the stored NotifyCount from before the main test step was run
 * and gets the NotifyCount from after the main test step was run and
 * tests that the expected number of Prompts occurred
 */
{

	//Get the stored NotifyCount from before the main test step was run
	TInt notifyCount1 = GetStoredNotifyCount();

	//Get the NotifyCount from after the main test step being run
	TInt notifyCount2 = GetNotifyCount();
	
	INFO_PRINTF1(_L(" "));
	INFO_PRINTF2(_L("NotifyCount after executing MultiThreaded test steps = ( %d )"), notifyCount2);
	
	TInt err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	return TestStepResult();
}


TE_RConnectionUPSTest27a::~TE_RConnectionUPSTest27a()
{
}

enum TVerdict TE_RConnectionUPSTest27a::doTestStepL(void)
/**
 * Performs an Implicit RSocket::Connect()
 * This test step is intended to be run in a Multi-Threaded / 
 * Multi-Process manner.
 */
{	
	DoTestStartDelay();
	
	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RSocket sock;
		
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenTcpSocket(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call Connect()
	err = ConnectTcpSocket(sock, iEchoServerAddr);
	TESTL(KErrNone == CheckResult(err));
	
	err = DestroyTcpSocket(sock);
	TESTEL((KErrNone == err || KErrPermissionDenied == err), err);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest27a



TE_RConnectionUPSTest27b::~TE_RConnectionUPSTest27b()
{
}

enum TVerdict TE_RConnectionUPSTest27b::doTestStepL(void)
/**
 * Performs an Implicit RSocket::SendTo()
 * This test step is intended to be run in a Multi-Threaded / 
 * Multi-Process manner.
 */
{
	DoTestStartDelay();

	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest27b



TE_RConnectionUPSTest27c::~TE_RConnectionUPSTest27c()
{
}

enum TVerdict TE_RConnectionUPSTest27c::doTestStepL(void)
/**
 * Performs an Implicit RHostResolver::GetByName()
 * This test step is intended to be run in a Multi-Threaded / 
 * Multi-Process manner.
 */
{
	DoTestStartDelay();

	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByName()
	err = MakeSynchronousNameRequest(hr, iTestName);
	TESTL(KErrNone == CheckResult(err));
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest27c



TE_RConnectionUPSTest27d::~TE_RConnectionUPSTest27d()
{
}

enum TVerdict TE_RConnectionUPSTest27d::doTestStepL(void)
/**
 * Performs an Implicit RHostResolver::GetByAddress()
 * This test step is intended to be run in a Multi-Threaded / 
 * Multi-Process manner.
 */
{
	DoTestStartDelay();

	TInt err;

	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RHostResolver hr;

	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenHostResolver(hr, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(hr);
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call GetByAddress()
	err = MakeSynchronousAddressRequest(hr, iLookupAddress);
	TESTL(KErrNone == CheckResult(err));
	
	DestroyHostResolver(hr);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionUPSTest27d



TE_RConnectionUPSTest28::~TE_RConnectionUPSTest28()
{
}

enum TVerdict TE_RConnectionUPSTest28::doTestStepL(void)
/**
 * Performs an RConnection::Start()
 * This test step is intended to be run in a Multi-Threaded / 
 * Multi-Process manner.
 */
{
	DoTestStartDelay();

	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));

	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();

} // TE_RConnectionUPSTest28


TE_RConnectionUPSTest29::~TE_RConnectionUPSTest29()
{
}

enum TVerdict TE_RConnectionUPSTest29::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() specifying a local loopback
 * address - 127.0.0.1
 *
 * SendTo() to a local address should never result in prompting 
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	TInetAddr sendAddr(KInetAddrLoop, iEchoPortNum);
	
	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, sendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest29



TE_RConnectionUPSTest30::~TE_RConnectionUPSTest30()
{
}

enum TVerdict TE_RConnectionUPSTest30::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() specifying a local loopback
 * address - ::1
 *
 * SendTo() to a local address should never result in prompting 
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	TInetAddr sendAddr(KInet6AddrLoop, iEchoPortNum);
	
	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, sendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest30



TE_RConnectionUPSTest31::~TE_RConnectionUPSTest31()
{
}

enum TVerdict TE_RConnectionUPSTest31::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() specifying a local address of interface
 *
 * SendTo() to a local address should never result in prompting 
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RConnection::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	CloseConnection(conn);
	CleanupStack::Pop();

	
	RSocket sock;
	
	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);

	_LIT(KInterfaceName,"dummy");

	//get local interface address
	TUint32 localAddr = GetInterfaceAddress(sock, KInterfaceName());
	TInetAddr sendAddr(localAddr, iEchoPortNum);
	
	//Implicit SendTo()/RecvFrom() local loopback address
	err = TestUdpDataPathL(sock, sendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest31



TE_RConnectionUPSTest32::~TE_RConnectionUPSTest32()
{
}

enum TVerdict TE_RConnectionUPSTest32::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Explicit RSocket::SendTo() specifying a local address of interface
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	_LIT(KInterfaceName,"dummy");

	//get local interface address
	TUint32 localAddr = GetInterfaceAddress(sock, KInterfaceName());
	TInetAddr sendAddr(localAddr, iEchoPortNum);
	
	//Explicit SendTo()/RecvFrom() local loopback address
	err = TestUdpDataPathL(sock, sendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest32



TE_RConnectionUPSTest33::~TE_RConnectionUPSTest33()
{
}

enum TVerdict TE_RConnectionUPSTest33::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Implicit RSocket::SendTo() specifying an address
 * that is scoped to the NetworkId of the interface
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());

	RSocketServ ss;
	RSocket sock;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);

	err = OpenUdpSocketL(sock, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount beforethe RSocket::...() API is called
	TInt notifyCount1 = GetNotifyCount();

	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Explicitly set the NetworkID of the interface - Scoped Address
	iDummyNifSendAddr.SetScope(KDummyNifNetworkId);

	//Call SendTo() and RecvFrom()
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();

	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);

	DestroyUdpSocket(sock);
	CleanupStack::Pop();

	CloseSocketServer(ss);
	CleanupStack::Pop();

	return TestStepResult();
} // TE_RConnectionUPSTest33


TE_RConnectionUPSTest34::~TE_RConnectionUPSTest34()
{
}

enum TVerdict TE_RConnectionUPSTest34::doTestStepL(void)
/**
 * Test prompting behaviour when calling:
 * Explicit RSocket::SendTo() specifying an address
 * that is scoped to the NetworkId of the interface
 */
{
	TInt err;
	
	//Delete entries in the DecisionDB
	TESTL(KErrNone == CheckAndDeleteUpsDB());
	
	RSocketServ ss;
	RConnection conn;
	
	err = OpenSocketServer(ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(ss);
	
	err = OpenConnection(conn, ss);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(conn);
	
	//Get the NotifyCount before the RConnection::...() API is called
	TInt notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Call RConnection::Start();
	err = StartConnection(conn);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	TInt notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	
	RSocket sock;
	
	err = OpenUdpSocketExplicitL(sock, ss, conn);
	TESTEL(KErrNone == err, err);
	CleanupClosePushL(sock);
	
	//Get the NotifyCount before the RSocket::...() API is called
	notifyCount1 = GetNotifyCount();
	
	//Set the Test Notifier Response
	err = SetNotifierResponse();
	TESTEL(KErrNone == err, err);
	
	//Explicitly set the NetworkID of the interface - Scoped Address
	iDummyNifSendAddr.SetScope(KDummyNifNetworkId);
	
	err = TestUdpDataPathL(sock, iDummyNifSendAddr);
	TESTL(KErrNone == CheckResult(err));
	
	//Get the NotifyCount after the RSocket::...() API is called
	notifyCount2 = GetNotifyCount();
	
	//Check the Prompt Occurred as expected
	err = CheckNotifyCount(notifyCount1, notifyCount2);
	TEST(KErrNone == err);
	
	DestroyUdpSocket(sock);
	CleanupStack::Pop();
	
	CloseConnection(conn);
	CleanupStack::Pop();
	
	CloseSocketServer(ss);
	CleanupStack::Pop();
	
	return TestStepResult();
} // TE_RConnectionUPSTest34

TE_RConnectionUPSTest35::~TE_RConnectionUPSTest35()
{
}

enum TVerdict TE_RConnectionUPSTest35::doTestStepL(void)
/**
 */
{
    TInt err;
    
    RSocketServ ss;
    RConnection conn;
    
    err = OpenSocketServer(ss);
    TESTEL(KErrNone == err, err);
    CleanupClosePushL(ss);
    
    RSocket sock;
    err = sock.Open(ss, KDummyAddrFamily, KSockDatagram, KDummyOne);
    INFO_PRINTF2(_L("Dummy socket open returned %d"), err);
    User::LeaveIfError(err);
    CleanupClosePushL(sock);

    err = sock.SetOpt(KDummyOptionIssueNoBearer, 0, 0);
    INFO_PRINTF2(_L("SetOpt(KDummyOptionIssueNoBearer) returned %d"), err);
    User::LeaveIfError(err);

    // Wait for the NoBearer() to take effect.  Actually, we are just waiting to see if ESock Server
    // crashes before attempting to open a second socket to check.
    const TInt KMicrosecondsPerSecond = 1000000;
    User::After(1 * KMicrosecondsPerSecond);

    RSocket sock2;
    err = sock2.Open(ss, KDummyAddrFamily, KSockDatagram, KDummyOne);
    if (err != KErrNone)
        {
        INFO_PRINTF2(_L("Error %d opening second dummy socket"), err);
        }
    User::LeaveIfError(err);
    sock2.Close();
    
    INFO_PRINTF1(_L("Dummy socket close"));

    CleanupStack::PopAndDestroy();      // sock
    
    CloseSocketServer(ss);
    CleanupStack::Pop();                // ss
    
    return TestStepResult();
} // TE_RConnectionUPSTest35


