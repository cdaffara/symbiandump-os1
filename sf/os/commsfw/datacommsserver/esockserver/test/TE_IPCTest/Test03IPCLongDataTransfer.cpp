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
// Contains IPC Test 03
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "Test03IPCLongDataTransfer.h"

const TDesC& CTest03IPCLongDataTransfer::GetTestName()
	{
	_LIT(ret,"IPCTest03");
	return ret;
	}

enum TVerdict CTest03IPCLongDataTransfer::InternalDoTestStepL(void)
	{
	
	//-------------------substep 00-----------------------------	
	_LIT(aLog00,"00 Open socket server:"); Logger().Write(aLog00);
	
	RSocketServ ss;
	CleanupClosePushL(ss);
	
	TInt err;
	if ( (err = OptimalConnect(ss)) != KErrNone )
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), err);
		User::Leave(EFail);
		}
	
	//-------------------substep 01-----------------------------	
	_LIT(aLog01,"01 Create a single client/server connection:");
	Logger().Write(aLog01);
	RSocket aClientSock, aNewConn, aServerSock;
	CleanupClosePushL(aNewConn);
	CleanupClosePushL(aClientSock);
	CleanupClosePushL(aServerSock);
	
	TRequestStatus aAcceptStat, aConnectStat;
	TSockAddr aAddr;
	
	if ( (err=aServerSock.Open(ss, iProt)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open server socket. err = %d");
		Logger().WriteFormat(aLog, err);
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
	
	if ( (err=aNewConn.Open(ss)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open blank socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	aServerSock.Accept(aNewConn, aAcceptStat);
	
	if ( (err=aClientSock.Open(ss, iProt)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open client socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	aAddr.SetPort(1);
	aClientSock.Connect(aAddr, aConnectStat);
	
	User::WaitForRequest(aConnectStat);
	User::WaitForRequest(aAcceptStat);
	TVerdict verdict = EPass;
	
    err = aConnectStat.Int();
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Client socket not connected. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}

	err = aAcceptStat.Int();
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Connection not accepted on server side. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}
    
    if (verdict == EFail)
    	User::Leave(EFail);
	
    CleanupStack::PopAndDestroy(&aServerSock);
	
	//-------------------substep 02-----------------------------	
	_LIT(aLog02,"02 Send 0x10000 bytes to the client. Check the content on arrival:");
	Logger().WriteFormat(aLog02);
	
	TRequestStatus aWriteStat, aReadStat;
	
	TBuf8<0x10000>* aOut, *aIn;
	CleanupStack::PushL( aOut = new (ELeave) TBuf8<0x10000>);
	CleanupStack::PushL( aIn  = new (ELeave) TBuf8<0x10000>);
	aOut->SetMax();
	aOut->Fill('a');
	aIn->SetMax();
	aIn->Fill('b');
	
	aClientSock.Read(*aIn, aReadStat);
	err = aReadStat.Int();
    if (err != KRequestPending)
		{
		Logger().WriteFormat(_L("Pending read failed.  err = %d"), err);
		User::Leave(EFail);
		}
		
	aNewConn.Write(*aOut,aWriteStat);
	User::WaitForRequest(aWriteStat);
	err = aWriteStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Failed to write. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Failed to read. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	if (aIn->Compare(*aOut))
		{
		_LIT(aLog,"Error:Data received on client side corrupted. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 03-----------------------------	
	_LIT(aLog05,"03 Cleanup: close all sockets and socket server.");
	Logger().Write(aLog05);
    CleanupStack::PopAndDestroy(5, &ss);
	return EPass;
	}
	

