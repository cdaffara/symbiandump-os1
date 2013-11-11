// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains ESock Test cases from section 29
// 
//


// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <es_sock.h>
#include <nifman.h>
#include <commdbconnpref.h>

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include "es_dummy.h"
#else
#include <test/es_dummy.h>
#endif

#include <es_sock.h>
#include <es_mbuf.h>

#include "EsockTestSection29.h"

// Test system includes
#include <comms-infras/esocktestutils.h>


TVerdict CEsockTest29::easyTestStepL()
	{

	// Open Sockets, one to manipulate mBufPool and one to send/revieve with tcp or udp
	TESTL( OpenSockets() == KErrNone );

	// Bind Test Socket To local Address
	TInetAddr local;
	SetPort( local, KPort, KLocal );
	iTestSocket.Bind( local );
	
	// Connect Test Socket to LoopBack Address
	TInetAddr loopBack;
	SetPort( loopBack, KPort, KLoopback );
	ConnectSocketL( iTestSocket, loopBack );
	
	// Gobble all the Buf's in the MBuf Pool return the number of mBufs in Use
	TInt totalUsedBufs = iDummySocket.SetOpt( KDummyOptionSetGobbleMBufs, 0, 0 );
	TInt bufsToBeFreed = 0;
	
	// Data to be used during socket Send
	GetIntFromConfig(KSection29, KBufSize, iBufSize);
	TBool keepSending = ETrue;
	TBuf8<4500> recvBuf;
	TBuf8<4500> sendData;
	sendData.Fill(TChar('z'), iBufSize );
	TRequestStatus sendStatus;
	TRequestStatus recvStatus;
	const TInt KOneSecond = 1000000;
	
	
	
	for( TInt freeBufCounter = 0; freeBufCounter < totalUsedBufs && keepSending; freeBufCounter += bufsToBeFreed )
	// OOB loop that will gradually release more MBufs
		{
		
		// release some mBufs and send
		iDummySocket.SetOpt( KDummyOptionSetFreeSomeMBufs, bufsToBeFreed , 0);
		iTestSocket.Send( sendData, 0, sendStatus );
		
		// Wait for a second to allow time for server/protocol to send
		User::After( KOneSecond );
		
		// Cancel the send and start to recv
		iTestSocket.CancelSend();
		User::WaitForRequest(sendStatus);
		
		iTestSocket.Recv(recvBuf, 0, recvStatus);
		
		// Wait for a second to allow time for server/protocol to recieve
		User::After( KOneSecond );
		iTestSocket.CancelRecv();
		User::WaitForRequest( recvStatus );
		
		if( freeBufCounter % 5 == 0 )
			{
			INFO_PRINTF2( _L("freeBufs now >= %d "),  freeBufCounter );
			}
		
		if( recvBuf.Compare(sendData) == 0 )
		// if we have recieved data then exit the loop
			{
			keepSending = EFalse;
			}
			
		bufsToBeFreed = 2;
		
		}
	
	// Now do a regular send for good measure to make sure everything is ok
	sendData.Fill(TChar('c'), iBufSize );
	INFO_PRINTF1( _L("End of Loop, Now sending....") );
	iTestSocket.Send( sendData, 0, sendStatus );
	User::WaitForRequest( sendStatus );
	INFO_PRINTF2( _L("Send Returned %d, now recieving...."), sendStatus.Int() );
	iTestSocket.Recv( recvBuf, 0, recvStatus );
	User::After( KOneSecond ); 
	iTestSocket.CancelRecv();
	User::WaitForRequest( recvStatus );
	
	INFO_PRINTF2( _L("Recieve returned %d ") , recvStatus.Int() );
	if( recvBuf.Compare(sendData) != 0 )
		{
		return EFail;
		}
	
	return EPass;
	
	}


TInt CEsockTest29::OpenSockets()
// Func to open the sockets
	{
	iSockServ.Connect();
	GetStringFromConfig(KSection29, KTestSocketType, iProtocol);
	TInt err = OpenDummySocket();
	if(err != KErrNone)	
		{
		return err;
		}
	if( iProtocol.Compare(KUdp) == 0 )
		{
		err = OpenTestUdpSocket();
		INFO_PRINTF1( _L("UDP Test Socket Opened") );
		}
	else
		{
		err = OpenTestTcpSocket();	
		INFO_PRINTF1( _L("TCP Test Socket Opened") );	
		}
	return err;
	}


CEsockTest29::~CEsockTest29()
	{
	iTestSocket.Close();
	iDummySocket.Close();
	iSockServ.Close();
	}

	
TInt CEsockTest29::OpenTestTcpSocket()
	{
	return iTestSocket.Open(iSockServ, KAfInet, KSockStream, KProtocolInetTcp);
	}

	
TInt CEsockTest29::OpenTestUdpSocket()
	{
	return iTestSocket.Open(iSockServ, KAfInet, KSockDatagram, KProtocolInetUdp);
	}


TInt CEsockTest29::OpenDummySocket()
	{
	return iDummySocket.Open(iSockServ,_L("Dummy Protocol 3"));
	}


void CEsockTest29::SetPort(TInetAddr &aAddr, const TInt aPort, const TDesC &aIPAddr)
	{
	aAddr.SetPort(aPort);
	aAddr.Input(aIPAddr);
	}
	

void CEsockTest29::ConnectSocketL( RSocket &aSocket, TInetAddr &aAddr )
// Connect aSocket to aAddr
	{
	TRequestStatus connectSocketStatus;
 	aSocket.Connect( aAddr, connectSocketStatus );
 	User::WaitForRequest( connectSocketStatus );
 	if(connectSocketStatus != KErrNone)
	 	{
	 	User::Leave(KErrCouldNotConnect);
	 	}
 	}
 	
const TDesC& CEsockTest29::GetTestName()
	{
	// store the name of this test case
	_LIT(ret,"Test29");
	return ret;
	}
	

/* --------------------------------------------------------------------------------------------
	ESOCK Test 29_1 uses the above Loop with a data size of 100 (less than MTU) and a Udp Socket
	changing the OOB loop to do this is done with the use of ini files
   -------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------
	ESOCK Test 29_2 uses the above Loop with a data size of 4500 (Greater than MTU) and a Udp Socket
  ------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------
	ESOCK Tests 29_3 to 29_9 are left open for further OOB loop tests
  ------------------------------------------------------------------------------------------------*/

	
	
/* Test 29.10 */

// Test step 29.10
const TDesC& CEsockTest29_10::GetTestName()
	{
	_LIT(ret,"Test29.10");
	return ret;
	}

CEsockTest29_10::~CEsockTest29_10()
	{
	}

enum TVerdict CEsockTest29_10::easyTestStepL( void )
	{
	SetTestStepResult(EFail);
	TInt ret;
	const TUint KOneSecond = 1000000;
	
	Logger().WriteFormat(_L("Send/ Recv on PDummy3 socket but no avail MBuf Memory, mBuf becomes avail after send"));
	RSocketServ ss;
	ret = ss.Connect();
	TESTL(KErrNone == ret);	
    
    // open dummy prot 3
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 3"));
	TESTL(KErrNone == ret);
	
	// connect socket
	TSockAddr addr;
	TRequestStatus connStat;
	sock.Connect(addr, connStat);
	User::WaitForRequest(connStat);
	TESTL(KErrNone == connStat.Int());
	
	// send packet 1
	TRequestStatus stat;
	Logger().WriteFormat(_L("Attempting to create a packet to receive later"));	
	_LIT8(sendDataOne, "First Send");
	sock.Send(sendDataOne,0,stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	// wait for packets to go through esock
	User::After(KOneSecond * 2);
	// gobble mBufs
	TInt nFree = 0;
	sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
	Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));
	while (nFree > 0)
		{
		ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
		sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
		}
	Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetGobbleMBufs returned %d"), ret);
	
	// we are running in high priority and allocating in a loop.
	// so kernel may not be able to enlarge the pools.
	// wait for kernel to enlarge the pool
	// We have to come out from the loop as well.
    User::After(KOneSecond * 2);
	
    sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
    Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));
    while (nFree > 0)
        {
        ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
        sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
        }
    Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetGobbleMBufs returned %d"), ret);
    // Once again
    User::After(KOneSecond * 2);
    sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
    Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));
    while (nFree > 0)
        {
        ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
        sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
        }
    Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetGobbleMBufs returned %d"), ret);
    // Once again
    User::After(KOneSecond * 2);
    sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
    Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));
    while (nFree > 0)
        {
        ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
        sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
        }
    // Once again
    User::After(KOneSecond * 2);
    sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
    Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));
    while (nFree > 0)
        {
        ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
        sock.GetOpt(KDummyOptionGetMBufFreeSpace, 0, nFree);
        }

	
	// send packet 2
	User::After(KOneSecond);
	TRequestStatus stat2;
	_LIT8(sendDataTwo, "Second Send");
	Logger().WriteFormat(_L("Sending Data - Should never complete"));             
	sock.Send(sendDataTwo,0,stat2);

	User::After(KOneSecond);
	Logger().WriteFormat(_L("Now cancel the Send"));     
	sock.CancelSend();
	User::WaitForRequest(stat2);
	TESTL(stat2 == KErrCancel);
	
	Logger().WriteFormat(_L("Receiving Data -- expected to pick up sendDataOne"));
	TBuf8<100> buf;          
	TRequestStatus stat3;
	sock.Recv(buf, 0, stat3);
	User::After(KOneSecond);
	User::WaitForRequest(stat3);
	TESTL(buf.Compare(sendDataOne) == 0);
		
	// send packet 3
	_LIT8(sendDataThree, "Third Send");
	Logger().WriteFormat(_L("Sending Data ... again"));             
	TRequestStatus stat4;
	sock.Send(sendDataThree,0,stat4);
	User::After(1000);
			
	// free memory
	Logger().WriteFormat(_L("Now free memory - should get send and receive completion"));	
	Logger().WriteFormat(_L("Attempting to set KDummyOptionSetFreeMBufs Option in Protocol"));	
	ret = sock.SetOpt(KDummyOptionSetFreeMBufs, 0, 0);
	Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetFreeMBufs returned %d"), ret);
	TESTL(KErrNone == ret);	

	Logger().WriteFormat(_L("Sending Data - Should now complete"));             
	User::WaitForRequest(stat4);
	TESTL(stat4.Int() == KErrNone);

	// recieve data and compare contents to sent data
	Logger().WriteFormat(_L("Receiving Data"));
	sock.Recv(buf, 0, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Recv has returned %d"), stat.Int());
	TBuf<100> buf16;
	buf16.Copy(buf);
	if(buf.Compare(sendDataThree) == 0)
		{
		SetTestStepResult(EPass);
		}
		
	Logger().WriteFormat(_L("Data Recv'ed is '%S'"), &buf16);
	sock.Close();
	ss.Close();
	return TestStepResult();
	}

/* Test 29.11 */

// Test step 29.11
const TDesC& CEsockTest29_11::GetTestName()
	{
	_LIT(ret,"Test29.11");
	return ret;
	}

CEsockTest29_11::~CEsockTest29_11()
	{
	}

enum TVerdict CEsockTest29_11::easyTestStepL( void )
	{
	TVerdict verdict = EFail;
	TInt ret;
	
	Logger().WriteFormat(_L("Send/ Recv on PDummy3 socket but no avail MBuf Memory, mBuf becomes avail after send"));
	RSocketServ ss;
	ret = ss.Connect();
	TESTL(KErrNone == ret);	
	CleanupClosePushL(ss);
    
    // open dummy prot 3
	Logger().WriteFormat(_L("Openning Dummy Protocol 3"));             
	RSocket sock;                                                  
	ret = sock.Open(ss,_L("Dummy Protocol 3"));
	TESTL(KErrNone == ret);
	
	// connecti socket
	TSockAddr addr;
	TRequestStatus stat;
	sock.Connect(addr, stat);
	User::WaitForRequest(stat);
	TESTL(KErrNone == stat.Int());	
	
	// drain MBuf Pool
	Logger().WriteFormat(_L("Attempting to set KDummyOptionSetGobbleMBufs Option in Protocol"));	
	ret = sock.SetOpt(KDummyOptionSetGobbleMBufs, 0, 0);
	Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetFreeMBufs returned %d"), ret);
	
	// send data and wait for 1/10 seconds
	Logger().WriteFormat(_L("Sending Data"));     
	_LIT8( sendData, "bla bla bla bla");        
	sock.Send(sendData,0,stat);
	const TUint KTenMilliSecs = 10000; 
	User::After(KTenMilliSecs);
	
	// free all mbufs
	Logger().WriteFormat(_L("Attempting to set KDummyOptionSetFreeMBufs Option in Protocol"));	
	ret = sock.SetOpt(KDummyOptionSetFreeMBufs, 0, 0);
	TESTL(ret == KErrNone);
	Logger().WriteFormat(_L("Socket::SetOpt KDummyOptionSetFreeMBufs returned %d"), ret);

	// wait for send to return
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Send has returned %d"), stat.Int());
	if(stat.Int() != KErrNone)
		{
		verdict = EFail;
		}
	
	// recieve data and verify that it is the same as send data
	TBuf8<20> recvBuf;
	TRequestStatus recvStatus;
	sock.Recv(recvBuf, 0, recvStatus);
	User::WaitForRequest(recvStatus);
	Logger().WriteFormat(_L("recieving data on PDummy3 has returned %d"), recvStatus.Int());
	if(recvStatus.Int() != KErrNone)
		{
		verdict = EFail;
		}
	Logger().WriteFormat(_L("Comparing Recieved data and Sent data"), recvStatus.Int());		
	if(recvBuf.Compare(sendData) == 0)
		{
		verdict = EPass; 
		}
	
	sock.Close();
	CleanupStack::PopAndDestroy(&ss);
    SetTestStepResult(verdict);
	return verdict;

	}

