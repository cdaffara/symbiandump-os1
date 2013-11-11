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
// Contains IPC Test 02
//
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <ss_std.h>

// Test system includes
#include "Test06IPCTestSelect.h"


const TDesC& CTest06IPCTestSelect::GetTestName()
	{
	_LIT(ret,"IPCTest06");
	return ret;
	}

enum TVerdict CTest06IPCTestSelect::InternalDoTestStepL(void)
	{
	_LIT8(KTestMsg, "Hello");
	
	//-------------------substep 00-----------------------------	
	Logger().Write(_L("00 Open socket server:"));
	
	TInt err;
	RSocketServ ss;
	CleanupClosePushL(ss);
	if ( (err = OptimalConnect(ss)) != KErrNone )
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), err);
		User::Leave(EFail);
		}

	//-------------------substep 01-----------------------------	
	Logger().Write(_L("01 Open server socket, set local port & set to listen:"));

	RSocket aClientSock, aNewConn, aServerSock;
	CleanupClosePushL(aClientSock);
	CleanupClosePushL(aServerSock);
	CleanupClosePushL(aNewConn);
	
	TRequestStatus aAcceptStat, aConnectStat;
	TSockAddr aAddr;
	
	if ( (err=aServerSock.Open(ss, iProt)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open server socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	if ( (err=aServerSock.SetLocalPort(257)) != KErrTooBig)
		{
		_LIT(aLog,"Error:Set illegal port returned %d instead of %d"); 
		Logger().WriteFormat(aLog, err, KErrTooBig);
		User::Leave(EFail);
		}
	
	if ( (err=aServerSock.SetLocalPort(1)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not set port. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	if ( (err=aServerSock.Listen(1)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not set up socket to listen. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 02-----------------------------	
	Logger().Write(_L("02 Set server socket to non-blocking mode:"));
	if ( (err=aServerSock.SetOpt(KSONonBlockingIO, KSOLSocket)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not set socket option. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 03-----------------------------	
	Logger().Write(_L("03 Open blank socket, assosiate it to server socket (with accept)...:"));
	Logger().Write(_L("   ...and check that return status is WOULD BLOCK:"));
	if ( (err=aNewConn.Open(ss)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open blank socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	aServerSock.Accept(aNewConn, aAcceptStat);
    User::WaitForRequest(aAcceptStat);
    err = aAcceptStat.Int();
    if (err != KErrWouldBlock)
		{
		_LIT(aLog,"Error:Accept method returned wrong value %d instead of %d"); 
		Logger().WriteFormat(aLog, err, KErrWouldBlock);
		User::Leave(EFail);
		}
	
	//-------------------substep 04-----------------------------	
	Logger().Write(_L("04 Assosiate local buffer as status buffer on server socket,..."));
	Logger().Write(_L("   ...with initial value READ + WRITE + EXCEPTION:"));
	TPckgBuf<TUint> aSbuf, aCbuf;
	aSbuf()=KSockSelectRead | KSockSelectWrite | KSockSelectExcept;
	aServerSock.Ioctl(KIOctlSelect, aAcceptStat, &aSbuf, KSOLSocket);
	
    //-------------------substep 05-----------------------------	
	Logger().Write(_L("05 Associate local buffer as status buffer on client socket,..."));
	Logger().Write(_L("   ...with initial value WRITE:"));

	// Create client socket.
	if ( (err=aClientSock.Open(ss, iProt)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	// Set client socket to non-blocking mode.
	if ( (err=aClientSock.SetOpt(KSONonBlockingIO, KSOLSocket)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not set socket option. err = %d");
		Logger().WriteFormat(aLog, err);
		aNewConn.Close();
		User::Leave(EFail);
		}
	
	// Connect client socket to server socket.
	aAddr.SetPort(1);
	aClientSock.Connect(aAddr, aConnectStat);
	User::WaitForRequest(aConnectStat);
	err = aConnectStat.Int();
    if ( (err != KErrNone) && (err != KErrWouldBlock))
		{
		_LIT(aLog,"Error:Connection method returned wrong value: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	aCbuf()=KSockSelectWrite;
    aClientSock.Ioctl(KIOctlSelect, aConnectStat, &aCbuf, KSOLSocket);
	User::WaitForRequest(aConnectStat);
	err = aConnectStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Ioctl method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    //-------------------substep 06-----------------------------	
	Logger().Write(_L("06 Check client status buffer to be WRITE..."));
	Logger().Write(_L("   ...and server status buffer to be READ:"));

	if ( aCbuf() != KSockSelectWrite)
		{
		_LIT(aLog,"Error:Wrong value of buffer: %d");
		Logger().WriteFormat(aLog, aCbuf());
		User::Leave(EFail);
		}

    User::WaitForRequest(aAcceptStat);
	err = aAcceptStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Ioctl method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    if (aSbuf() != KSockSelectRead)
		{
		_LIT(aLog,"Error:Wrong value of buffer: %d");
		Logger().WriteFormat(aLog, aSbuf());
		User::Leave(EFail);
		}
	
	//-------------------substep 07-----------------------------	
	Logger().Write(_L("07 Assosiate blank socket to the server socket:"));
	
	// Re-open blank client socket.
	aNewConn.Close();
	if ( (err=aNewConn.Open(ss)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
		
	aServerSock.Accept(aNewConn, aAcceptStat);
	User::WaitForRequest(aAcceptStat);
	err = aAcceptStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Accept method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 08-----------------------------	
	Logger().Write(_L("08 Chack that blank socket is in non blocking mode as well"));
	if ( (err=aNewConn.GetOpt(KSONonBlockingIO, KSOLSocket, aSbuf)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not get socket option. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
    if (!aSbuf())
		{
		_LIT(aLog,"Error:Wrong value of option: %d");
		Logger().WriteFormat(aLog, aSbuf());
		User::Leave(EFail);
		}
	
	//-------------------substep 09-----------------------------	
	Logger().Write(_L("09 Try to read on server side and check that it would block"));
	TRequestStatus aReadStat, aWriteStat;
	TBuf8<5> aReadbuf;
	aReadbuf.SetMax();
	aNewConn.Read(aReadbuf, aReadStat);
	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrWouldBlock)
		{
		_LIT(aLog,"Error:Read method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 10-----------------------------	
	Logger().Write(_L("10 Assosiate local buffer as status buffer of blank socket,..."));
	Logger().Write(_L("   ...with initial value: READ + EXCEPTION"));
	aSbuf()=KSockSelectRead | KSockSelectExcept;
	aNewConn.Ioctl(KIOctlSelect, aAcceptStat, &aSbuf, KSOLSocket);
	
	//-------------------substep 11-----------------------------	
	Logger().Write(_L("11 Write a message on the client side, and chack that"));
	Logger().Write(_L("   ...status of the other side of connection is now: READ"));

	aClientSock.Write(KTestMsg, aWriteStat);
	User::WaitForRequest(aWriteStat);
	err = aWriteStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Write method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	User::WaitForRequest(aAcceptStat);
	err = aAcceptStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Ioctl method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    if (aSbuf()!=KSockSelectRead)
		{
		_LIT(aLog,"Error:Wrong value of option: %d");
		Logger().WriteFormat(aLog, aSbuf());
		User::Leave(EFail);
		}

    //-------------------substep 12-----------------------------	
	Logger().Write(_L("12 Read the message on server side and check the content:"));
	aReadbuf.SetMax();
	
	aNewConn.Read(aReadbuf, aReadStat);
	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Read method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    if (aReadbuf != KTestMsg)
		{
		_LIT(aLog,"Error:Read/Write mismatch");
		Logger().Write(aLog);
		User::Leave(EFail);
		}
	
	//-------------------substep 13-----------------------------	
	Logger().Write(_L("13 Try to read again on server side and check that ..."));
	Logger().Write(_L("   ...the status of the action is: WOULD BLOCK"));
	aReadbuf.SetMax();

	aNewConn.Read(aReadbuf, aReadStat);
	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrWouldBlock)
		{
		_LIT(aLog,"Error:Read method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 14-----------------------------	
	Logger().Write(_L("14 Write the message on the client side:"));
	aClientSock.Write(KTestMsg, aWriteStat);
	User::WaitForRequest(aWriteStat);
	err = aWriteStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Write method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 15-----------------------------	
	Logger().Write(_L("15 Get SelectPoll option of the server side and check..."));
	Logger().Write(_L("   ...it is READ + WRITE"));

	if ( (err=aNewConn.GetOpt(KSOSelectPoll, KSOLSocket, aSbuf)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not get socket option. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
 
	if (aSbuf()!=(KSockSelectRead | KSockSelectWrite))
		{
		_LIT(aLog,"Error:Wrong value of option: %d");
		Logger().WriteFormat(aLog, aSbuf());
		User::Leave(EFail);
		}

	//-------------------substep 16-----------------------------	
	Logger().Write(_L("16 Assosiate local buffer as status buffer on server side,..."));
	Logger().Write(_L("   ...with initial value: READ + EXCEPTION. Chack that it stays READ only"));

	aSbuf()=KSockSelectRead | KSockSelectExcept;
	aNewConn.Ioctl(KIOctlSelect, aAcceptStat, &aSbuf, KSOLSocket);
	User::WaitForRequest(aAcceptStat);
	err = aAcceptStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Ioctl method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    if (aSbuf() != KSockSelectRead)
		{
		_LIT(aLog,"Error:Wrong value of option: %d");
		Logger().WriteFormat(aLog, aSbuf());
		User::Leave(EFail);
		}
	
	//-------------------substep 17-----------------------------	
	Logger().Write(_L("17 Read the message on server side and check the content:"));

	aReadbuf.SetMax();
	aNewConn.Read(aReadbuf, aReadStat);
	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Read method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

    if (aReadbuf != KTestMsg)
		{
		_LIT(aLog,"Error:Read/Write mismatch");
		Logger().Write(aLog);
		User::Leave(EFail);
		}
	
	//-------------------substep 18-----------------------------	
	Logger().Write(_L("18 Close the server side. Access server side and the check that..."));
	Logger().Write(_L("   ...status of the action is DISCONNECTED:"));
	CleanupStack::PopAndDestroy(&aNewConn);
	aCbuf()=KSockSelectRead | KSockSelectExcept;
	
	aClientSock.Ioctl(KIOctlSelect, aConnectStat, &aCbuf, KSOLSocket);
	User::WaitForRequest(aConnectStat);
	err = aConnectStat.Int();
    if (err != KErrDisconnected)
		{
		_LIT(aLog,"Error:Ioctl method returned wrong status: %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 19-----------------------------	
	Logger().Write(_L("19 Close all socket and socket server:"));
	CleanupStack::PopAndDestroy(3, &ss);
	return EPass;
	}

