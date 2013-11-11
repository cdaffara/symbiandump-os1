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
// This contains ESock Test cases from section 17
// Testing read from socket with KSockReadPeek flag set
// 
//


#include <e32base.h>

#include "SocketTestSection17.h"


// Test step 17.1
const TDesC& CSocketTest17_1::GetTestName()
	{
	_LIT(ret,"Test17.1");
	return ret;
	}

enum TVerdict CSocketTest17_1::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Use a socket handle for a host resolver operation [PANIC STEP]."));
	
    Logger().WriteFormat(_L("- connecting to the socket server"));
	RSocketServ sockServ;
    TInt nRes = OptimalConnect(sockServ);
    TESTL(nRes == KErrNone);
	RSocket sock;
	Logger().WriteFormat(_L("- opening a null socket"));
	nRes = sock.Open(sockServ);
    TESTL(nRes == KErrNone);
    CleanupClosePushL(sock);
	
    Logger().WriteFormat(_L("- abusing the socket as a host resolver; should panic with \"eSock:0\""));
	RHostResolver* hr = reinterpret_cast<RHostResolver*>(&sock);
	TName name;
	hr->GetHostName(name);
	
    CleanupStack::PopAndDestroy(2); //sockServ, sock
	SetTestStepResult(EFail);
	return EFail;
	}


// Test step 17.2
const TDesC& CSocketTest17_2::GetTestName()
	{
	_LIT(ret,"Test17.2");
	return ret;
	}

enum TVerdict CSocketTest17_2::InternalDoTestStepL( void )
	{
	Logger().WriteFormat(_L("Use a stale socket handle [PANIC STEP]."));
	
    Logger().WriteFormat(_L("- connecting to the socket server, creating & getting local name of null socket"));
	RSocketServ sockServ;
    TInt nRes = OptimalConnect(sockServ);
    TESTL(nRes == KErrNone);
	RSocket sock;
	Logger().WriteFormat(_L("- opening connection to 'Dummy Protocol 2'"));
	nRes = sock.Open(sockServ, _L("Dummy Protocol 2"));
    TESTL(nRes == KErrNone);
    CleanupClosePushL(sock);
	TSockAddr addr;
	Logger().WriteFormat(_L("- getting local name of socket'"));
	sock.LocalName(addr);
	
    Logger().WriteFormat(_L("- copying original socket to duplicateSock"));
	RSocket duplicateSock(sock);
	Logger().WriteFormat(_L("- getting local name of duplicateSock"));
	duplicateSock.LocalName(addr);
    CleanupStack::Pop(&sock); //sock
    Logger().WriteFormat(_L("- closing original socket (sock)"));
    sock.Close();
	RSocket newSock;
	Logger().WriteFormat(_L("-opening a null socket on newsock"));
	nRes = newSock.Open(sockServ);
    TESTL(nRes == KErrNone);
    CleanupClosePushL(newSock);
	
    Logger().WriteFormat(_L("- getting local name of duplicate socket; should panic with \"eSock:0\""));
	duplicateSock.LocalName(addr);
	
    CleanupStack::PopAndDestroy(2); //sockServ, newSock
	SetTestStepResult(EFail);
	return EFail;
	}


