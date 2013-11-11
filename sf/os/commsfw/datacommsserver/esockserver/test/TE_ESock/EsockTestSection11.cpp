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
// This contains ESock Test cases from section 11
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection11.h"
#include "esockopenserver.h"


// Test step 11.1
const TDesC& CEsockTest11_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test11.1");
	return ret;
	}

CEsockTest11_1::~CEsockTest11_1()
	{
	}

enum TVerdict CEsockTest11_1::easyTestStepL()
	{
	// get ip address to connect to
	TInetAddr addr;
	TESTL(GetIpAddressFromConfig(_L("Test_Common"), _L("ipAddress"), addr));
	
	// get port number to connect to
	TInt port;
	TESTL(GetIntFromConfig(_L("Test_11.1"), _L("port"), port));
	
	// set remote address port
	addr.SetPort(port);
	
	// open a TCP socket
	RSocket sock;
	CleanupClosePushL(sock);
	TInt nRet = sock.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	
	// connect to the remote address
	TRequestStatus stat;
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTEL(stat==KErrNone, stat.Int());
	
	// get the remote name of the connected socket
	TInetAddr addr1;
	sock.RemoteName(addr1);
	
	// check this matches the address we connected to
	TESTL(addr.CmpAddr(addr1));
	
	CleanupStack::PopAndDestroy(1, &sock);
	return EPass;
	}


// Test step 11.2
const TDesC& CEsockTest11_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test11.2");
	
	return ret;
	}

CEsockTest11_2::~CEsockTest11_2()
	{
	}

enum TVerdict CEsockTest11_2::easyTestStepL()
	{
	TInt nRet;
	TProtocolDesc protocolInfo;
	
	// Get protocol information
	
	// open a TCP socket, retrieve the protocol info
	// and check the protocol ID
	RSocket sock1;
	CleanupClosePushL(sock1);
	nRet = sock1.Open(iEsockSuite->iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == nRet, nRet);
	TESTL(sock1.Info(protocolInfo)==KErrNone);
	TESTL(protocolInfo.iProtocol==KProtocolInetTcp);
	CleanupStack::PopAndDestroy(1, &sock1);
	
	// open a UDP socket, retrieve the protocol info
	// and check the protocol ID
	RSocket sock2;
	CleanupClosePushL(sock2);
	nRet = sock2.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp);
	TESTEL(KErrNone == nRet, nRet);
	TESTL(sock2.Info(protocolInfo)==KErrNone);
	TESTL(protocolInfo.iProtocol==KProtocolInetUdp);
	CleanupStack::PopAndDestroy(1, &sock2);
	
	// open an ICMP socket, retrieve the protocol info
	// and check the protocol ID
	RSocket sock3;
	CleanupClosePushL(sock3);
	nRet = sock3.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetIcmp);
	TESTEL(KErrNone == nRet, nRet);
	TESTL(sock3.Info(protocolInfo)==KErrNone);
	TESTL(protocolInfo.iProtocol==KProtocolInetIcmp);
	CleanupStack::PopAndDestroy(1, &sock3);
	
	// open an IP socket, retrieve the protocol info
	// and check the protocol ID
	RSocket sock4;
	CleanupClosePushL(sock4);
	nRet = sock4.Open(iEsockSuite->iSocketServer, KAfInet, KSockDatagram, KProtocolInetIp);
	TESTEL(KErrNone == nRet, nRet);
	TESTL(sock4.Info(protocolInfo)==KErrNone);
	TESTL(protocolInfo.iProtocol==KProtocolInetIp);
	CleanupStack::PopAndDestroy(1, &sock4);
	
	return EPass;
	}


// Test step 11.3
const TDesC& CEsockTest11_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test11.3");
	
	return ret;
	}

CEsockTest11_3::~CEsockTest11_3()
	{
	}

enum TVerdict CEsockTest11_3::easyTestStepL()
	{
	TInetAddr addrLocal, addrRemote;
	TRequestStatus wstat, wstat2, wstat3, rstat;
	TInt sockIndex1, sockIndex2, sockIndex3;
	
	const TInt KBufSize = 4024;
	// Constructs an empty 8 bit modifiable buffer descriptor. It contains no data.
	typedef TBuf8<KBufSize> TBuffer;
	
	// get local ip address
	TESTL(GetIpAddressFromConfig(_L("Test_11.3"), _L("ipAddressLocal"), addrLocal));
	
	// get ip address to connect to (usually loopback)
	TESTL(GetIpAddressFromConfig(_L("Test_11.3"), _L("ipAddressRemote"), addrRemote));
	
	// open socket and listen for connect requests
	TESTL(KErrNone == OpenListeningSocketL(addrLocal, sockIndex1));
	
	// open active socket and make connect request
	TESTL(KErrNone == OpenActiveSocketL(addrRemote, sockIndex2));
	
	// accept connect request
	TESTL(KErrNone == AcceptConnectionL(sockIndex3, sockIndex1));
	
	TBuffer* wtemp=new (ELeave) TBuffer;
	CleanupStack::PushL(wtemp);
	TBuffer& wbuf=*wtemp;
	
	TBuffer* rtemp=new (ELeave) TBuffer;
	CleanupStack::PushL(rtemp);
	TBuffer& rbuf=*rtemp;
	
	wbuf.SetMax();
	StripeDes(wbuf, 0, wbuf.Length(), '@', 'Z');
	
	iEsockSuite->GetSocketHandle(sockIndex2).Send(wbuf, 0, wstat);
	iEsockSuite->GetSocketHandle(sockIndex2).Send(wbuf, 0, wstat2);
	iEsockSuite->GetSocketHandle(sockIndex2).Send(wbuf, 0, wstat3);
	iEsockSuite->GetSocketHandle(sockIndex2).CancelAll();
	User::WaitForRequest(wstat);
	User::WaitForRequest(wstat2);
	User::WaitForRequest(wstat3);
	TESTEL(wstat==KErrNone || wstat==KErrCancel, wstat.Int());
	TESTEL(wstat2==KErrNone || wstat2==KErrCancel, wstat2.Int());
	TESTEL(wstat3==KErrNone || wstat3==KErrCancel, wstat3.Int());
	
	Logger().WriteFormat(_L("stat1 %d stat2 %d stat3 %d"),wstat.Int(), wstat2.Int(), wstat3.Int());
	
	iEsockSuite->GetSocketHandle(sockIndex3).Recv(rbuf, 0, rstat);
	iEsockSuite->GetSocketHandle(sockIndex3).CancelAll();
	User::WaitForRequest(rstat);
	TESTEL(rstat==KErrNone || rstat==KErrCancel, rstat.Int());
	
	iEsockSuite->GetSocketHandle(sockIndex2).Shutdown(RSocket::ENormal, wstat);
	iEsockSuite->GetSocketHandle(sockIndex3).Shutdown(RSocket::ENormal, rstat);
	iEsockSuite->GetSocketHandle(sockIndex2).CancelAll();
	iEsockSuite->GetSocketHandle(sockIndex3).CancelAll();
	User::WaitForRequest(wstat);
	User::WaitForRequest(rstat);
	TESTEL(wstat == KErrNone, wstat.Int());
	TESTEL(rstat == KErrNone, rstat.Int());

	CleanupStack::PopAndDestroy(2, wtemp);
	
	// shutdown the client socket - do not wait for completion
	CloseSockets(2);
	return EPass;
	}


// Test step 11.4
const TDesC& CEsockTest11_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test11.4");
	
	return ret;
	}

CEsockTest11_4::~CEsockTest11_4()
	{
	}

enum TVerdict CEsockTest11_4::easyTestStepL()
	{
	//
	// Out Of Memory Test on open() RSocket //
	//

	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("TE_ESock: test 11.4"));
	Logger().WriteFormat(_L("RSocket Open"));

#if defined (_DEBUG_SOCKET_FUNCTIONS)

	RSocketServ socketHelper;
	CleanupClosePushL(socketHelper);
	TInt ret = socketHelper.Connect();
	TESTEL(KErrNone == ret, ret);

	// Create a socket on the helper to get the DND up & running
	RSocket sockHelper;
	CleanupClosePushL(sockHelper);
	ret = sockHelper.Open(socketHelper, KAfInet, KSockStream, KProtocolInetTcp);
	TESTEL(KErrNone == ret, ret);
	// Short wait for DND settle down
    User::After(5000000);
	socketHelper.__DbgMarkHeap();

	RSocketServ socketServer;
	CleanupClosePushL(socketServer);
	ret = socketServer.Connect();
	TESTEL(KErrNone == ret, ret);
	
	// See if we can crash the server:
	RSocket sock;
	CleanupClosePushL(sock);
	TInt failure = 0;

	verdict = EInconclusive;
	TInt prevResult = KErrNoMemory;
	TInt prevOccurs = 0;
	while (EInconclusive == verdict)
		{
		socketServer.__DbgFailNext(failure++);
		ret = sock.Open(socketServer, KAfInet, KSockStream, KProtocolInetTcp);
		
		if ((prevResult != ret) || (++prevOccurs >= 1000))
			{
			Logger().WriteFormat(_L("%d loop(s), open socket returned %d"), prevOccurs, prevResult);
			if (KErrNone == ret)
				{
				verdict = EPass;
				}
			else if (KErrServerTerminated == ret)
				{
				verdict = EFail;
				}
			else if (prevResult != ret)
				{
				prevResult = ret;
				prevOccurs = 1;
				}
			else
				{
				prevOccurs = 0;
				}
			}
		};
	
	socketServer.__DbgFailNext(-1);
	// wait for protocol families to unload - it's via an async callback and dependent upon the IP stack, which
	// may also be killing off DND. So a big delay is needed. If it proves unreliable in future then the really
	// strong approach would be shutting down ESOCK - it'll panic if it has allocations outstanding
    User::After(5000000);
	socketHelper.__DbgMarkEnd(0);
	CleanupStack::PopAndDestroy(4, &socketHelper);

#else
	Logger().WriteFormat(_L("Test disabled on release build."));
#endif
	
	return verdict;
	}

