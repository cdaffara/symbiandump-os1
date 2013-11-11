// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 16
// Testing read from socket with KSockReadPeek flag set
// 
//


#include <e32base.h>

#include "SocketTestSection16.h"


// Test step 16.1
const TDesC& CSocketTest16_1::GetTestName()
	{
	_LIT(ret,"Test16.1");
	return ret;
	}

enum TVerdict CSocketTest16_1::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Testing read from socket with KSockReadPeek flag set in different situations."));
	
	RSocketServ sockServ;
	
	RSocket     sock_UdpIn;
    RSocket     sock_UdpOut;
    TInt        nRes;
	
    _LIT8(KSendData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"); // Sample testdata to send in these tests
	
    CleanupClosePushL(sockServ);
    CleanupClosePushL(sock_UdpIn);
    CleanupClosePushL(sock_UdpOut);
	
    Logger().WriteFormat(_L("connecting to the socket server..."));
    nRes = sockServ.Connect();
    TESTL(nRes == KErrNone);
	
    //-- open UDP datagram sockets 
	Logger().WriteFormat(_L("opening sockets..."));
    TESTL(sock_UdpIn.Open (sockServ, KAfInet, KSockDatagram, KProtocolInetUdp) == KErrNone);
	TESTL(sock_UdpOut.Open(sockServ, KAfInet, KSockDatagram, KProtocolInetUdp) == KErrNone);
	
    TRequestStatus  rqStat_in, rqStat_out;
	
    //-- local host address, port #7 - the data will be sent via this address
    TInetAddr       inetAddrTo(7);
    TInetAddr       inetAddrFrom;
	
    TBuf8<100>      tempBuf, tempBuf1;
    
    const  TInt     KIPHeaderSize = 20; //-- IP header size
    const  TInt     KUdpHeaderSize= 8;  //-- UDP header size
	
	
    inetAddrTo.Input(_L("127.0.0.2"));
    sock_UdpIn.Bind(inetAddrTo); //-- bind socket to address
	
	
    //###################################################################################################
    //--- Test normal situation with peeking data from the UDP socket, when the data arrived before peeking
    //###################################################################################################
	
	
    //--- 1. send the data to the udp socket
    sock_UdpOut.SendTo(KSendData, inetAddrTo, 0, rqStat_out);
	User::WaitForRequest(rqStat_out);
	Logger().WriteFormat(_L("UDP sent %d bytes of data, error code:%d\n"), KSendData().Size(), rqStat_out.Int());
    TESTL(rqStat_out.Int() == KErrNone);
	
	
    //--- 2. Peek data from UDP socket and check the size (taking into account the KIpHeaderIncluded flag)
    sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf.Size() == KIPHeaderSize + KUdpHeaderSize + KSendData().Size());
	
    //--- 3. Peek data from UDP socket and check the size (taking into account the KIpHeaderIncluded flag)
    sock_UdpIn.RecvFrom(tempBuf1, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf1.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf1.Size() == KIPHeaderSize + KUdpHeaderSize + KSendData().Size());
	
    //-- check, the data shall be the same
    TESTL( tempBuf.CompareF(tempBuf1) ==0);
	
    //--- 4. read data from socket and check the size
	
	sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, 0, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP read real %d bytes data from the socket, error code:%d\n"), tempBuf.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf.Size() == KSendData().Size());
	
    //###################################################################################################
    //--- Test the situation with peeking data from the UDP socket, when the data arrived after read request.
    //###################################################################################################
	
	
    //--- 1. Issue a read request from the socket with KSockReadPeek flag set
    Logger().WriteFormat(_L("Issuing reading request with KSockReadPeek flag set"));
    sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
	
	//--- 2. send data 
	sock_UdpOut.SendTo(KSendData, inetAddrTo, 0, rqStat_out);
	User::WaitForRequest(rqStat_out);
	Logger().WriteFormat(_L("UDP sent %d bytes of data, error code:%d\n"), KSendData().Size(), rqStat_out.Int());
	TESTL(rqStat_out.Int() == KErrNone);
	
	
    //--- 3. Wait for peeking data to complete, check the result
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf.Size() == KIPHeaderSize + KUdpHeaderSize + KSendData().Size());
	
    
    sock_UdpIn.RecvFrom(tempBuf1, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf1.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf1.Size() == KIPHeaderSize + KUdpHeaderSize + KSendData().Size());
	
    //-- check, the data shall be the same
    TESTL( tempBuf.CompareF(tempBuf1) ==0);
	
    //--- 4. read data from socket and check the size
	
	sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, 0, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP read real %d bytes data from the socket, error code:%d\n"), tempBuf.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
    TESTL(tempBuf.Size() == KSendData().Size());
	
	
    //###################################################################################################
    //--- Test the situation with peeking data from the UDP socket cancellation
    //###################################################################################################
	
    //--- 1. Issue a read request from the socket with KSockReadPeek flag set
    Logger().WriteFormat(_L("Issuing reading request with KSockReadPeek flag set"));
    sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
    
    Logger().WriteFormat(_L("Cancel receiving"));
    sock_UdpIn.CancelRecv();
	
	//--- 2. send data 
	sock_UdpOut.SendTo(KSendData, inetAddrTo, 0, rqStat_out);
	User::WaitForRequest(rqStat_out);
	Logger().WriteFormat(_L("UDP sent %d bytes of data, error code:%d\n"), KSendData().Size(), rqStat_out.Int());
	TESTL(rqStat_out.Int() == KErrNone);
	
	
    //--- 3. Wait for peeking data to complete, check the result
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf.Size(), rqStat_in.Int());
    TESTL((rqStat_in.Int() == KErrCancel) || (rqStat_in.Int() == KErrNone));
    
    //--- 4. Peek the data
    sock_UdpIn.RecvFrom(tempBuf, inetAddrFrom, KIpHeaderIncluded|KSockReadPeek, rqStat_in);
	User::WaitForRequest(rqStat_in);
	Logger().WriteFormat(_L("UDP peeked %d bytes of data, error code:%d\n"), tempBuf1.Size(), rqStat_in.Int());
    TESTL(rqStat_in.Int() == KErrNone);
	
	
	
	//-- close and destroy everything
	CleanupStack::PopAndDestroy(3); //sockServ, sock_UdpIn, sock_UdpOut
	
	SetTestStepResult(EPass);
	return EPass;
	}


