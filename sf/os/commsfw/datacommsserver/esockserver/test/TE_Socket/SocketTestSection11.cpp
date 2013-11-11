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
// This contains ESock Test cases from section 11
// 
//

// EPOC includes
#include <e32base.h>


// Test system includes
#include "SocketTestSection11.h"


// Test step 11.1
const TDesC &CSocketTest11_1::GetTestName()
	{
	_LIT(ret,"Test11.1");
	return ret;
	}

enum TVerdict CSocketTest11_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Successful Socket Transfer"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = OptimalConnect(ss1);
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Creating second connection to socket server"));
	RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening socket on first connection to dummy protocol 1"));
	RSocket sock1;
	RSocket tran1;
	ret = sock1.Open(ss1, _L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting name of socket"));
	TName socketname;
	ret = sock1.Name(socketname);
	Logger().WriteFormat(_L("Name returned %S, name '%S'"), 
		&EpocErrorToText(ret), &socketname);
	TESTL(KErrNone == ret);
    //---------------------------------------------------------------------
    //-- if SYMBIAN_NETWORKING_PLATSEC symbol is defined, we are using EKA2 and secured RSocket::Transfer()
    //-- in this case the transfer shall be enabled by setting appropriate socket option with "pulling" process capabilities
	
    //-- define the capabilities set for the socket "pulling" process (this one, actually)
    //-- this process must have at least NetworkServices and NetworkControl capabilities
    _LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
	
    
    //-- this transfer attempt shall fail with KErrPermissionDenied because the transfer hasn't been enabled
    Logger().WriteFormat(_L("Transferring the socket to another session shall fail with KErrPermissionDenied"));
    ret = tran1.Transfer(ss2, socketname);
    Logger().WriteFormat(_L("RSocket::Transfer() returned %S"), &EpocErrorToText(ret));
    TESTL(KErrPermissionDenied == ret);
    
    //-- Test setting new socket options 
    Logger().WriteFormat(_L("Testing setting options KSOEnableTransfer, KSODisableTransfer.."));
    ret = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTL(KErrNone == ret);
    
    ret = sock1.SetOpt(KSODisableTransfer, KSOLSocket);
    TESTL(KErrNone == ret);
    
    //-- this transfer attempt shall also fail, because we set KSOCancelTransfer above
    ret = tran1.Transfer(ss2, socketname);
    TESTL(KErrPermissionDenied == ret);
    Logger().WriteFormat(_L("OK"));
    
    //-- enable socket transfer (see lines below) with PlatSec check. Set socket option with receiving process capabilities    
    ret = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTL(KErrNone == ret);
	
    
    //---------------------------------------------------------------------
	Logger().WriteFormat(_L("Transferring first socket to second session"));
	ret = tran1.Transfer(ss2, socketname);
	Logger().WriteFormat(_L("Transfer returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Checking Info on first socket fails"));
	TProtocolDesc protoInfo;
	ret = sock1.Info(protoInfo);
	Logger().WriteFormat(_L("Info returned %S"), &EpocErrorToText(ret));
	TESTL(KErrBadHandle == ret);
	sock1.Close();
	
	Logger().WriteFormat(_L("Checking Info on transferred socket"));
	ret = tran1.Info(protoInfo);
	Logger().WriteFormat(_L("Info returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	tran1.Close();
	
	CleanupStack::Pop(&ss2);
	CleanupStack::Pop(&ss1);
	
	ss1.Close();
	ss2.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 11.2
const TDesC &CSocketTest11_2::GetTestName()
	{
	_LIT(ret,"Test11.2");
	return ret;
	}

enum TVerdict CSocketTest11_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Null Socket Transfer Fails"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = OptimalConnect(ss1);
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Creating second connection to socket server"));
	RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening null socket on second connection"));
	RSocket sock2;
	ret = sock2.Open(ss2);
	CleanupClosePushL(sock2);
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting name of socket"));
	TName socketname;
	ret = sock2.Name(socketname);
	Logger().WriteFormat(_L("Name returned %S, name '%S'"), 
		&EpocErrorToText(ret), &socketname);
	TESTL(KErrNone == ret);
	
	//---------------------------------------------------------------------
    //-- if SYMBIAN_NETWORKING_PLATSEC symbol is defined, we are using EKA2 and secured RSocket::Transfer()
    //-- in this case the transfer shall be enabled by setting appropriate socket option with "pulling" process capabilities
	
    //-- define the capabilities set for the socket "pulling" process (this one, actually)
    //-- this process must have at least NetworkServices and NetworkControl capabilities
    _LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
    
    //-- enable socket transfer (see lines below) with PlatSec check. Set socket option with receiving process capabilities    
    ret = sock2.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTL(KErrNotReady == ret);
	
    
    //---------------------------------------------------------------------
	
	Logger().WriteFormat(_L("Attempting to transfer second socket"));
	RSocket tran2;
	ret = tran2.Transfer(ss1, socketname);
	CleanupClosePushL(tran2);
	Logger().WriteFormat(_L("Transfer returned %S"), &EpocErrorToText(ret));
	TESTL(KErrPermissionDenied == ret);
	
	Logger().WriteFormat(_L("Checking Info on second socket fails"));
	TProtocolDesc protoInfo;
	ret = sock2.Info(protoInfo);
	Logger().WriteFormat(_L("Info returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);
	
	CleanupStack::Pop(&tran2);
	CleanupStack::Pop(&sock2);
	CleanupStack::Pop(&ss2);
	CleanupStack::Pop(&ss1);
	
	ss1.Close();
	sock2.Close();
	tran2.Close();
	ss2.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}

// Test step 11.3
const TDesC &CSocketTest11_3::GetTestName()
	{
	_LIT(ret,"Test11.3");
	return ret;
	}

enum TVerdict CSocketTest11_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Successful Transfer from Main dealer to other dealer"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Creating second optimal connection to socket server"));
	RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening null socket on second connection"));
	RSocket sock1;
	ret = sock1.Open(ss2,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Getting name of socket"));
	TName socketname;
	ret = sock1.Name(socketname);
	Logger().WriteFormat(_L("Name returned %S, name '%S'"), 
		&EpocErrorToText(ret), &socketname);
	TESTL(KErrNone == ret);
	
	//---------------------------------------------------------------------
    //-- if SYMBIAN_NETWORKING_PLATSEC symbol is defined, we are using EKA2 and secured RSocket::Transfer()
    //-- in this case the transfer shall be enabled by setting appropriate socket option with "pulling" process capabilities
	
    //-- define the capabilities set for the socket "pulling" process (this one, actually)
    //-- this process must have at least NetworkServices and NetworkControl capabilities
    _LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
    
    //-- enable socket transfer (see lines below) with PlatSec check. Set socket option with receiving process capabilities    
    ret = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTL(KErrNone == ret);
	
    
    //---------------------------------------------------------------------
	
	Logger().WriteFormat(_L("Attempting to transfer socket"));
	RSocket tran1;
	ret = tran1.Transfer(ss2, socketname);
	Logger().WriteFormat(_L("Transfer returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Checking Info on first socket fails"));
	TProtocolDesc protoInfo;
	ret = sock1.Info(protoInfo);
	Logger().WriteFormat(_L("Info returned %S"), &EpocErrorToText(ret));
	TESTL(KErrBadHandle == ret);
	sock1.Close();
	
	Logger().WriteFormat(_L("Checking Info on transferred socket"));
	ret = tran1.Info(protoInfo);
	Logger().WriteFormat(_L("Info returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	tran1.Close();
	
	CleanupStack::Pop(&ss2);
	ss1.Close();
	CleanupStack::Pop(&ss1);
	ss2.Close();
	
	return verdict;
	}


// Test step 11.4
const TDesC &CSocketTest11_4::GetTestName()
	{
	_LIT(ret,"Test11.4");
	return ret;
	}

enum TVerdict CSocketTest11_4::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Test Purpose: Attempt transfer of fake socket name"));
	
	// connect to esock
	Logger().WriteFormat(_L("Attempting to connect to socket server"));
    RSocketServ ss1;
	TInt ret = ss1.Connect();
	CleanupClosePushL(ss1);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Creating second optimal connection to socket server"));
	RSocketServ ss2;
	ret = OptimalConnect(ss2);
	CleanupClosePushL(ss2);
	Logger().WriteFormat(_L("Connect returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Opening null socket on second connection"));
	RSocket sock1;
	ret = sock1.Open(ss2,_L("Dummy Protocol 1"));
	Logger().WriteFormat(_L("Open returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNone == ret);
	
	
	//---------------------------------------------------------------------
    //-- if SYMBIAN_NETWORKING_PLATSEC symbol is defined, we are using EKA2 and secured RSocket::Transfer()
    //-- in this case the transfer shall be enabled by setting appropriate socket option with "pulling" process capabilities
	
    //-- define the capabilities set for the socket "pulling" process (this one, actually)
    //-- this process must have at least NetworkServices and NetworkControl capabilities
    _LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
    
    //-- enable socket transfer (see lines below) with PlatSec check. Set socket option with receiving process capabilities    
    ret = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
    TESTL(KErrNone == ret);
	
    
    //---------------------------------------------------------------------
	
	Logger().WriteFormat(_L("Attempting to transfer socket to good thread# but fake addr 5 ADEADF8"));
	RSocket tran1;
	// Try OK thread #, bad address
	ret = tran1.Transfer(ss2, _L("5 ADEADF8"));
	Logger().WriteFormat(_L("Transfer returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);

	// Try thread # out of range
	Logger().WriteFormat(_L("Attempting to transfer socket to out-of-range thread# 9 ADEADF8"));
	ret = tran1.Transfer(ss2, _L("9 ADEADF8"));
	Logger().WriteFormat(_L("Transfer returned %S"), &EpocErrorToText(ret));
	TESTL(KErrNotFound == ret);

	sock1.Close();
	
	tran1.Close();
	
	CleanupStack::Pop(&ss2);
	ss1.Close();
	CleanupStack::Pop(&ss1);
	ss2.Close();
	
	return verdict;
	}



