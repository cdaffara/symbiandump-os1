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
// Contains IPC Test 01
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes

#include "Test01IPCStreamConnection.h"

const TDesC& CTest01IPCStreamConnection::GetTestName()
	{
	_LIT(ret,"IPCTest01");
	return ret;
	}

enum TVerdict CTest01IPCStreamConnection::InternalDoTestStepL(void)
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
	_LIT(aLog01,"01 Open server socket, set port  & listen:");
	Logger().Write(aLog01);
	RSocket aServerSock;
	CleanupClosePushL(aServerSock);
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
	
	//-------------------substep 02-----------------------------	
	_LIT(aLog02,"02 Create blank socket and marry it (with accept) to the server socket:");
	Logger().Write(aLog02);
	RSocket aNewConn;
	CleanupClosePushL(aNewConn);
	if ( (err=aNewConn.Open(ss)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open blank socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	TRequestStatus aAcceptStat;
	aServerSock.Accept(aNewConn, aAcceptStat);
	
	//-------------------substep 03-----------------------------	
	_LIT(aLog03,"03 Create client socket and connect it to the server");
	Logger().Write(aLog03);
	RSocket aClientSock;
	CleanupClosePushL(aClientSock);
	if ( (err=aClientSock.Open(ss, iProt)) != KErrNone)
		{
		_LIT(aLog,"Error:Could not open client socket. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	TSockAddr aAddr;
	aAddr.SetPort(1);
	TRequestStatus aConnectStat;
	aClientSock.Connect(aAddr, aConnectStat);
	
	//-------------------substep 04-----------------------------	
	_LIT(aLog04,"04 Check the status of the connection at the both ends:");
	Logger().Write(aLog04);
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
	
	//-------------------substep 05-----------------------------	
	_LIT(aLog05,"05 Close all sockets.  Close socket server:"); Logger().WriteFormat(aLog05);
	CleanupStack::PopAndDestroy(4, &ss);
	return verdict;
	}
	

