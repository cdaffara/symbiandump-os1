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
// This contains ESock Test cases from section 7
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "EsockTestSection7.h"


// Test step 7.1
const TDesC& CEsockTest7_1::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.1");
	
	return ret;
	}

CEsockTest7_1::~CEsockTest7_1()
	{
	}

enum TVerdict CEsockTest7_1::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 2)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		
		ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open UDP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_1::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(2 == nSockets, nSockets);
	
	// Test IP options (KSolInetIp)

	RSocket &sock1 = iEsockSuite->GetSocketHandle(1);
	RSocket &sock2 = iEsockSuite->GetSocketHandle(2);
	
	// KSoIpTOS option, default value = 0
	
	TInt nRet;
	// get option and check its default value
	TInt ret = sock1.GetOpt(KSoIpTOS, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoIpTOS, KSolInetIp, 60);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoIpTOS, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 60, nRet);
	
	// restore default
	ret = sock1.SetOpt(KSoIpTOS, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoIpTOS, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoIpTOS, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoIpTOS, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock2.SetOpt(KSoIpTOS, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoIpTTL option - default value = 69
	
	// get option and check its default value
	ret = sock1.GetOpt(KSoIpTTL, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 69, nRet);
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoIpTTL, KSolInetIp, 255);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoIpTTL, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 255, nRet);
	
	// restore the default
	ret = sock1.SetOpt(KSoIpTTL, KSolInetIp, 69);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoIpTTL, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 69, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoIpTTL, KSolInetIp, 63);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoIpTTL, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 63, nRet);
	
	// restore the default
	ret = sock2.SetOpt(KSoIpTTL, KSolInetIp, 69);
	TESTEL(KErrNone == ret, ret);
	
	// KSoReuseAddr option - default value is disabled (0)
	
	// get option and check its default value
	ret = sock1.GetOpt(KSoReuseAddr, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoReuseAddr, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoReuseAddr, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore the default
	ret = sock1.SetOpt(KSoReuseAddr, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoReuseAddr, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoReuseAddr, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoReuseAddr, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore the default
	ret = sock2.SetOpt(KSoReuseAddr, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoNoInterfaceError option - supported
	
	// get/set option - sould be successful
	ret = sock1.GetOpt(KSoNoInterfaceError, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.SetOpt(KSoNoInterfaceError, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoNoInterfaceError, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.SetOpt(KSoNoInterfaceError, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	CloseSockets(2);
	
	return EPass;
	}
	
	
// Test step 7.2
const TDesC& CEsockTest7_2::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.2");
	
	return ret;
	}

CEsockTest7_2::~CEsockTest7_2()
	{
	}

enum TVerdict CEsockTest7_2::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 1)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open UDP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_2::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(1 == nSockets, nSockets);
	
	// Test UDP only options (KSolInetUdp)

	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	
	// KSoUdpReceiveICMPError option, default value is disabled (0)
	
	// get option and check its default value
	TInt nRet;
	TInt ret = sock.GetOpt(KSoUdpReceiveICMPError, KSolInetUdp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoUdpReceiveICMPError, KSolInetUdp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoUdpReceiveICMPError, KSolInetUdp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoUdpReceiveICMPError, KSolInetUdp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoIpOptions option is not supported
	
	// get/set - unsuccessful
	TBuf8<40> buf;
	ret = sock.GetOpt(KSoIpOptions, KSolInetIp, buf);			// Returned -5
	TESTEL(KErrNotSupported == ret, ret);
	ret = sock.SetOpt(KSoIpOptions, KSolInetIp, _L8("abc"));			// Returned -5
	TESTEL(KErrNotSupported == ret, ret);
	
	// KSoRawMode option, default value is disabled (0)
	
	// get option and check its default value
	ret = sock.GetOpt(KSoRawMode, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoRawMode, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoRawMode, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoRawMode, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoHeaderIncluded option, default value is disabled (0)
	
	// get option and check its default value
	ret = sock.GetOpt(KSoHeaderIncluded, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	// must set raw mode first
	ret = sock.SetOpt(KSoHeaderIncluded, KSolInetIp, 1);
	TESTEL(KErrNotSupported == ret, ret);
	ret = sock.SetOpt(KSoRawMode, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.SetOpt(KSoHeaderIncluded, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoHeaderIncluded, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoHeaderIncluded, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	ret = sock.SetOpt(KSoRawMode, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	
	CloseSockets(2);
	
	return EPass;
	}


// Test step 7.3
const TDesC& CEsockTest7_3::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.3");
	
	return ret;
	}

CEsockTest7_3::~CEsockTest7_3()
	{
	}

enum TVerdict CEsockTest7_3::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 1)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_3::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(1 == nSockets, nSockets);
	
	// Test TCP only options (KSolInetTcp)

	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	
	// KSoTcpSendWinSize option, default value is 16384
	
	// get option and check its default value
	TInt nRet;
	TInt ret = sock.GetOpt(KSoTcpSendWinSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 16384, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpSendWinSize, KSolInetTcp, 4096);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpSendWinSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 4096, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpSendWinSize, KSolInetTcp, 16384);
	TESTEL(KErrNone == ret, ret);
	
	
	// KSoTcpRecvWinSize option, default value is 49152
	
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpRecvWinSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 49152, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpRecvWinSize, KSolInetTcp, 16384);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpRecvWinSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 16384, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpRecvWinSize, KSolInetTcp, 49152);
	TESTEL(KErrNone == ret, ret);
	
	// KSoTcpMaxSegSize option, default value is 65535
	
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpMaxSegSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 65535, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpMaxSegSize, KSolInetTcp, 400);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpMaxSegSize, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 400, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpMaxSegSize, KSolInetTcp, 65535);
	TESTEL(KErrNone == ret, ret);
	
	// KSoTcpNoDelay option, default value is disabled (0)
	
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpNoDelay, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpNoDelay, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpNoDelay, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpNoDelay, KSolInetTcp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoTcpKeepAlive option, is supported
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpKeepAlive, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// get/set option - check it has been set correctly
	ret = sock.SetOpt(KSoTcpKeepAlive, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpKeepAlive, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	//Set the default
	ret = sock.SetOpt(KSoTcpKeepAlive, KSolInetTcp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoTcpAsync2MslWait option, not supported
	
	// get/set option - unsuccessful
	ret = sock.GetOpt(KSoTcpAsync2MslWait, KSolInetTcp, nRet);
	TESTEL(KErrNotSupported == ret, ret);
	ret = sock.SetOpt(KSoTcpAsync2MslWait, KSolInetTcp, 1);
	TESTEL(KErrNotSupported == ret, ret);
	
	// KSoTcpNextSendUrgentData option, default value is disabled (0)
	
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpNextSendUrgentData, KSolInetTcp, 0);
	TESTEL(KErrNone == ret, ret);
	
	// KSoTcpOobInline option, default value is disabled (0)
	
	// get option and check its default value
	ret = sock.GetOpt(KSoTcpOobInline, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// set new value - check it has been set correctly
	ret = sock.SetOpt(KSoTcpOobInline, KSolInetTcp, 1);
	TESTEL(KErrNone == ret, ret);
	ret = sock.GetOpt(KSoTcpOobInline, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// restore default
	ret = sock.SetOpt(KSoTcpOobInline, KSolInetTcp, 0);
	TESTEL(KErrNone == ret, ret);
	
	CloseSockets(2);
	
	return EPass;
	}
	
	
// Test step 7.4
const TDesC& CEsockTest7_4::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.4");
	
	return ret;
	}

CEsockTest7_4::~CEsockTest7_4()
	{
	}

enum TVerdict CEsockTest7_4::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 2)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		
		ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open UDP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_4::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(2 == nSockets, nSockets);
	
	RSocket &sock1 = iEsockSuite->GetSocketHandle(1);
	RSocket &sock2 = iEsockSuite->GetSocketHandle(2);
	
	// KSoIp6UnicastHops option, default value is 69
	
	// get option and check its default value
	TInt nRet;
	TInt ret = sock1.GetOpt(KSoIp6UnicastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 69, nRet);
	
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoIp6UnicastHops, KSolInetIp, 40);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoIp6UnicastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 40, nRet);
	
	// restore default
	ret = sock1.SetOpt(KSoIp6UnicastHops, KSolInetIp, 69);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoIp6UnicastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 69, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoIp6UnicastHops, KSolInetIp, 30);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoIp6UnicastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 30, nRet);
	
	// restore default
	ret = sock2.SetOpt(KSoIp6UnicastHops, KSolInetIp, 69);
	
	// KSoIp6MulticastHops option, default value is 1
	
	// get option and check its default value
	ret = sock1.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoIp6MulticastHops, KSolInetIp, 3);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 3, nRet);
	
	// restore default
	ret = sock1.SetOpt(KSoIp6MulticastHops, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoIp6MulticastHops, KSolInetIp, 4);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 4, nRet);
	
	// restore default
	ret = sock2.SetOpt(KSoIp6MulticastHops, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// KSoIp6MulticastLoop option, default value is enabled (1)
	
	// get option and check its default value
	ret = sock1.GetOpt(KSoIp6MulticastLoop, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// set new value - check it has been set correctly
	ret = sock1.SetOpt(KSoIp6MulticastHops, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	ret = sock1.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// restore default
	ret = sock1.SetOpt(KSoIp6MulticastHops, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	// repeat for UDP socket
	ret = sock2.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 1, nRet);
	
	// set new value - check it has been set correctly
	ret = sock2.SetOpt(KSoIp6MulticastHops, KSolInetIp, 0);
	TESTEL(KErrNone == ret, ret);
	ret = sock2.GetOpt(KSoIp6MulticastHops, KSolInetIp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// restore default
	ret = sock2.SetOpt(KSoIp6MulticastHops, KSolInetIp, 1);
	TESTEL(KErrNone == ret, ret);
	
	CloseSockets(2);
	
	return EPass;
	}
	
	
// Test step 7.5
const TDesC& CEsockTest7_5::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.5");
	
	return ret;
	}

CEsockTest7_5::~CEsockTest7_5()
	{
	}

enum TVerdict CEsockTest7_5::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 1)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_5::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(1 == nSockets, nSockets);
	
	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	
	// attempt to set read-only socket options
	// KSoTcpReadBytesPending and KSoTcpSocketListening
	
	// attempt to set new value - check it is not successful
	TInt nRet;
	TInt ret = sock.SetOpt(KSoTcpReadBytesPending, KSolInetTcp, 40);
	TESTEL(KErrNotSupported == ret, ret);
	
	// check option can be retrieved
	ret = sock.GetOpt(KSoTcpReadBytesPending, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	// attempt to set new value - check it is not successful
	ret = sock.SetOpt(KSoTcpListening, KSolInetTcp, 1);
	TESTEL(KErrNotSupported == ret, ret);
	
	// check option can be retrieved
	ret = sock.GetOpt(KSoTcpListening, KSolInetTcp, nRet);
	TESTEL(KErrNone == ret, ret);
	TESTEL(nRet == 0, nRet);
	
	CloseSockets(2);
	
	return EPass;
	}


// Test step 7.6
const TDesC& CEsockTest7_6::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.6");
	
	return ret;
	}

CEsockTest7_6::~CEsockTest7_6()
	{
	}

enum TVerdict CEsockTest7_6::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 2)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		
		ret = OpenSockets(KAfInet, KSockDatagram, KProtocolInetUdp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open UDP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_6::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(2 == nSockets, nSockets);
	
	// attempt to set a UDP specific option for a TCP socket
	
	// attempt to enable KSoRawMode value - check it is not successful
	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	TInt ret = sock.SetOpt(KSoRawMode, KSolInetIp, 1);
	TESTEL(KErrNotSupported == ret, ret);
	
	// attempt to set a TCP specific option for a UDP socket
	
	// attempt to set the KSoTcpSendWinSize value - check it is not successful
	sock = iEsockSuite->GetSocketHandle(2);
	ret = sock.SetOpt(KSoTcpSendWinSize, KSolInetIp, 4096);
	TESTEL(KErrNotSupported == ret, ret);

	CloseSockets(2);
	
	return EPass;
	}


// Test step 7.7
const TDesC& CEsockTest7_7::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test7.7");
	
	return ret;
	}

CEsockTest7_7::~CEsockTest7_7()
	{
	}

enum TVerdict CEsockTest7_7::easyTestStepPreambleL()
	{
	if (iEsockSuite->GetSocketListCount() != 1)
		{
		CloseSockets();
		
		TInt ret = OpenSockets(KAfInet, KSockStream, KProtocolInetTcp);
		if (KErrNone != ret)
			{
			Logger().WriteFormat(_L("Open TCP socket failed, returned %d"), ret);
			return EFail;
			}
		}
	return EPass;
	}

enum TVerdict CEsockTest7_7::easyTestStepL()
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	TESTEL(1 == nSockets, nSockets);
	
	// set options to an invalid value
	
	RSocket &sock = iEsockSuite->GetSocketHandle(1);
	
	// attempt to set KSoIpTTL to invalid value
	// - check it is not successful
	TInt ret = sock.SetOpt(KSoIpTTL, KSolInetIp, 5000);
	TESTEL(KErrArgument == ret, ret);
	
	CloseSockets(2);
	
	return EPass;
	}

