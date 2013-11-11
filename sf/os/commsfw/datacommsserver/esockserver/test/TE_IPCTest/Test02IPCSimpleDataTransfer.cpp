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

// Test system includes
#include "Test02IPCSimpleDataTransfer.h"


const TDesC& CTest02IPCSimpleDataTransfer::GetTestName()
	{
	_LIT(ret,"IPCTest02");
	return ret;
	}

enum TVerdict CTest02IPCSimpleDataTransfer::InternalDoTestStepL(void)
	{
	
	//-------------------substep 00-----------------------------	
	_LIT(aLog00,"00 Open socket server:");Logger().Write(aLog00);
	
	TInt err;
	RSocketServ ss;
	CleanupClosePushL(ss);
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
    if (aConnectStat != KErrNone)
		{
		_LIT(aLog,"Error:Client socket not connected. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}
    
    err = aAcceptStat.Int();
    if (aAcceptStat != KErrNone)
		{
		_LIT(aLog,"Error:Connection not accepted on server side. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}
    
    if (verdict == EFail)
    	User::Leave(EFail);
	
    CleanupStack::PopAndDestroy(&aServerSock);
	
	//-------------------substep 02-----------------------------	
	_LIT(aLog02,"02 Send 0x100 bytes to the client. Check the content on arrival:");
	Logger().Write(aLog02);
	
	TBuf8<0x100>* aOut, *aIn;
	CleanupStack::PushL( aOut = new (ELeave) TBuf8<0x100>);
	CleanupStack::PushL( aIn  = new (ELeave) TBuf8<0x100>);
	aOut->SetMax();
	aOut->Fill('a');
	aIn->SetMax();
	aIn->Fill('b');
	
	TRequestStatus aWriteStat, aReadStat;
	aClientSock.Read(*aIn, aReadStat);
	aNewConn.Write(*aOut, aWriteStat);
	
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
		_LIT(aLog,"Error:Data received on clent side corrupted. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 03-----------------------------	
	_LIT(aLog03_1,"03 Send 0x100 bytes to the client partially (a byte after byte). Then ...");
	Logger().Write(aLog03_1);
	_LIT(aLog03_2,"   ...receive them on client side and check the content.");
	Logger().Write(aLog03_2);

	TInt i;
	aIn->Fill('b');
	aClientSock.Read(*aIn, aReadStat);
	
	aOut->SetLength(1);
	for (i=0;i<0x100;i++)
		{
		aNewConn.Write(*aOut, aWriteStat);
		User::WaitForRequest(aWriteStat);
		err = aWriteStat.Int();
		if (err != KErrNone)
			{
			_LIT(aLog,"Error:Failed to write. err = %d");
			Logger().WriteFormat(aLog, err);
			User::Leave(EFail);
			}
		}
	
	User::WaitForRequest(aReadStat);
	err = aReadStat.Int();
    if (err != KErrNone)
		{
		_LIT(aLog,"Error:Failed to read. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	aOut->SetMax();
	if (aIn->Compare(*aOut))
		{
		_LIT(aLog,"Error:Data received on clent side corrupted. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 04-----------------------------	
	_LIT(aLog04_1,"04 Send 0x100 bytes to the server. Then...");
	Logger().Write(aLog04_1);
	_LIT(aLog04_2,"   ...receive them partially(byte after byte) on the server side and check the content.");
	Logger().Write(aLog04_2);
	
	aIn->Fill('b');
	aClientSock.Write(*aOut, aWriteStat);
	
	User::WaitForRequest(aWriteStat);
	err = aWriteStat.Int();
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Failed to write. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	TBuf8<0x01> aInSmall;
	for (i=0;i<0x100;i++)
		{
		aNewConn.Read(aInSmall, aReadStat);
		User::WaitForRequest(aReadStat);
		err = aReadStat.Int();
		if (err != KErrNone)
			{
			_LIT(aLog,"Error:Failed to read. err = %d");
			Logger().WriteFormat(aLog, err);
			User::Leave(EFail);
			}
		(*aIn)[i] = aInSmall[0];
		}
	
	if (aIn->Compare(*aOut))
		{
		_LIT(aLog,"Error:Data received on clent side corrupted. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	
	//-------------------substep 05-----------------------------	
	_LIT(aLog05,"05 Cleanup stack. Close all sockets.  Close socket server");
	Logger().Write(aLog05);
    CleanupStack::PopAndDestroy(5, &ss);
	return EPass;
	}
	

