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
// Contains IPC Test 05
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>
#include <ss_std.h>

#include <elements/sd_std.h>

#include "Test05IPCIllegalOperations.h"

using namespace Den;

const TDesC& CTest05IPCIllegalOperations::GetTestName()
	{
	_LIT(ret,"IPCTest05");
	return ret;
	}
// constructor
CTest05IPCIllegalOperations::CTest05IPCIllegalOperations()
	{
    iThreadStackSize = 1024;
	}


// This is thread that should fail as it attempts to accept
// with connected socket
TInt CTest05IPCIllegalOperations::fAcceptWithConnectedSocket(TAny* aInput)
	{
	TInt err;
	RSocketServ aSockServer;
	TRequestStatus aStatAccept, aStatConnect;
	RSocket aNewConn, aServerSock;
	RSocket aClientSock;
	TSockAddr aAddr;
    CTest05IPCIllegalOperations* pTestObject =
								(CTest05IPCIllegalOperations*)aInput;
	
	//-------------------substep 01-----------------------------	
	// Connect to Socket server, and create client/socket connection 
	if ( (err = OptimalConnect(aSockServer)) != KErrNone )
		return err;

	if ( (err=aServerSock.Open(aSockServer, pTestObject->iProt)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}

	if ( (err=aServerSock.SetLocalPort(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}

	if ( (err=aServerSock.Listen(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}
	
	if ( (err=aNewConn.Open(aSockServer)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}
	aServerSock.Accept(aNewConn,aStatAccept);
	
	if ( (err=aClientSock.Open(aSockServer, pTestObject->iProt)) != KErrNone)
		{
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}

	aAddr.SetPort(1);
	aClientSock.Connect(aAddr, aStatConnect);
	User::WaitForRequest(aStatConnect);
	User::WaitForRequest(aStatAccept);

	err = aStatAccept.Int();
    if (err != KErrNone)
		{
		aClientSock.Close();
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}
	
    err = aStatConnect.Int();
    if (err != KErrNone)
		{
		aClientSock.Close();
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}
	
	//-------------------substep 02-----------------------------	
	// Try to assosiate the same blank socket (that is already connected
	// to the client socket) to the server socket. 
	// This should fail and panic this thread. 
	// Main thread will then examine the exit status of this thread.
	aServerSock.Accept(aNewConn, aStatAccept); 
	
	User::WaitForRequest(aStatAccept);
	err = aStatAccept.Int();
	
	aClientSock.Close();
	aNewConn.Close();
	aServerSock.Close();
	aSockServer.Close();
	return err;
	}

// This is thread that should fail as it attempts to accept
// with unopened socket
TInt CTest05IPCIllegalOperations::fAcceptWithUnopendSocket(TAny* aInput)
	{
	TInt err;
	RSocketServ aSockServer;
	RSocket aNewConn, aServerSock;
	TRequestStatus aStat;
    CTest05IPCIllegalOperations* pTestObject =
								(CTest05IPCIllegalOperations*)aInput;
	
	//-------------------substep 01-----------------------------	
	// Connect to Socket server, and create server socket
	if ( (err = OptimalConnect(aSockServer)) != KErrNone )
		return EFail;

	if ( (err=aServerSock.Open(aSockServer, pTestObject->iProt)) != KErrNone)
		{
		aSockServer.Close();
		return err;
		}
	
	if ( (err=aServerSock.SetLocalPort(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}

	if ( (err=aServerSock.Listen(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		return err;
		}
	
	//-------------------substep 02-----------------------------	
	// Try to accept connection with a blank socket that is not opened.
	// This should fail and panic this thread. 
	// Main thread will then examine the exit status of this thread.
	aServerSock.Accept(aNewConn, aStat);
	User::WaitForRequest(aStat);
	aServerSock.Close();
	aSockServer.Close();
	return KErrNone;
	}

// 
enum TVerdict CTest05IPCIllegalOperations::InternalDoTestStepL(void)
	{
	//-------------------substep 01-----------------------------	
	_LIT(aLog01,"01 Create and start thread that will try to accept null socket:");
	Logger().Write(aLog01);

	RThread aThread;
	CleanupClosePushL(aThread);
	
	TRequestStatus aStatus;
	TInt err = aThread.Create(_L("IPCTest05Thread1"), 
		fAcceptWithUnopendSocket, 
		KDefaultStackSize, 
		KDefaultHeapSize, 
		KDefaultHeapSize, 
		(TAny*)this,
		EOwnerProcess);

	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create thread. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}

	aThread.Logon(aStatus);
	TBool aOldPanic = User::JustInTime();
	User::SetJustInTime(EFalse);
	aThread.Resume();
	
	//-------------------substep 02-----------------------------	
	User::WaitForRequest(aStatus);
	User::SetJustInTime(aOldPanic);
	
	TPtrC testCategory;
	testCategory.Set(aThread.ExitCategory());
	TExitType exitType = aThread.ExitType();
	TInt reason = aThread.ExitReason();
	
	if (testCategory == KESockClientPanic &&
			exitType == EExitPanic &&
			reason == ESockBadHandle)
		{
		_LIT(aLog, "02 Thread panic: Category='%S', type=%d, reason=%d");
		Logger().WriteFormat(aLog, &testCategory, exitType, reason);
		}
	else
		{
		_LIT(aLog, "02 Thread panic: Category='%S', type=%d, reason=%d <- wrong panic");
		Logger().WriteFormat(aLog, &testCategory, exitType, reason);
		User::Leave(EFail);
		}
	CleanupStack::PopAndDestroy(&aThread);
	
	//-------------------substep 03-----------------------------	
	_LIT(aLog03,"03 Create and start thread that will try to accept already connected socket:");
	Logger().Write(aLog03);
	
	CleanupClosePushL(aThread);
	err = aThread.Create(_L("IPCTest05Thread2"), 
		fAcceptWithConnectedSocket, 
					   KDefaultStackSize, 
					   KDefaultHeapSize, 
					   KDefaultHeapSize, 
					   (TAny*)this,
					   EOwnerProcess);
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create thread. err = %d");
		Logger().WriteFormat(aLog, err);
		User::Leave(EFail);
		}
	aThread.Logon(aStatus);
	aOldPanic = User::JustInTime();
	User::SetJustInTime(EFalse);
	aThread.Resume();
	
	//-------------------substep 04-----------------------------	
	User::WaitForRequest(aStatus);
	User::SetJustInTime(aOldPanic);

	testCategory.Set(aThread.ExitCategory());
	exitType = aThread.ExitType();
	reason = aThread.ExitReason();
	
	if (testCategory == KESockClientPanic &&
			exitType == EExitPanic &&
			reason == EAcceptTwice)
		{
		_LIT(aLog, "04 Thread panic: Category='%S', type=%d, reason=%d");
		Logger().WriteFormat(aLog, &testCategory, exitType, reason);
		}
	else
		{
		_LIT(aLog, "04 Thread panic: Category='%S', type=%d, reason=%d <- wrong panic");
		Logger().WriteFormat(aLog, &testCategory, exitType, reason);
		User::Leave(EFail);
		}
	CleanupStack::PopAndDestroy(&aThread);
	
	return EPass;
	}

