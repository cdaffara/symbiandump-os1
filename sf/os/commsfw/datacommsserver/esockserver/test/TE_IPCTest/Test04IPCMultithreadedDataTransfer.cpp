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
// Contains IPC Test 04
// 
//

// EPOC includes
#include <e32base.h>
#include <ss_std.h>

#include "Test04IPCMultithreadedDataTransfer.h"

const TInt KInactivityTimeout = 1000000;	// Use 1 second so that there can be many timeouts, but we'll beat the TEF's 100 second timeout

const TDesC& CTest04IPCMultithreadedDataTransfer::GetTestName()
	{
	_LIT(ret,"IPCTest04");
	return ret;
	}
// constructor
CTest04IPCMultithreadedDataTransfer::CTest04IPCMultithreadedDataTransfer()
	{
    iThreadStackSize = 1024;
    iSemName = _L("IPC sem");
    iTestString = _L8("This is IPC test string");
	}

// destructor
CTest04IPCMultithreadedDataTransfer::~CTest04IPCMultithreadedDataTransfer() 
	{
	}

// 
TInt CTest04IPCMultithreadedDataTransfer::fTransmitterThread(TAny* aInput)
	{
	TInt err;
	RSocketServ aSockServer;
	RSocket aClientSock;
	RSemaphore aSem;
	TSockAddr aAddr;
    CTest04IPCMultithreadedDataTransfer* pTestObject =
								(CTest04IPCMultithreadedDataTransfer*)aInput;
	
	//-------------------substep 01-----------------------------	
	// Connect to Socket server, open semaphore and create client socket end point
	pTestObject->iTransmitThreadResult.Format(_L("   Step1..."));
	pTestObject->iTransmitThreadResult.Append(_L("Connect to Sock Server..."));
	if ( (err = OptimalConnect(aSockServer)) != KErrNone ) 
		return err;
	pTestObject->iTransmitThreadResult.Append(_L("Open Semaphore..."));
	if ((err = aSem.OpenGlobal(pTestObject->iSemName)) != KErrNone) 
		{
		aSockServer.Close();
		return err;
		}
	pTestObject->iTransmitThreadResult.Append(_L("Open Client Socket..."));
	if ( (err=aClientSock.Open(aSockServer,pTestObject->iProt)) != KErrNone)
		{
		aSockServer.Close();
		aSem.Close();
		return err;
		}
	
	//-------------------substep 02-----------------------------	
	// Wait the signal from receiver thread to continue
	pTestObject->iTransmitThreadResult.Append(_L("Wait..."));
	aSem.Wait(); 
	
	//-------------------substep 03-----------------------------	
	// Connect to the server socket and send test message
	aAddr.SetPort(1);
	pTestObject->iTransmitThreadResult.Append(_L("Connect..."));
	aClientSock.Connect(aAddr,pTestObject->iTrStatus);
	User::WaitForRequest(pTestObject->iTrStatus);
	if (pTestObject->iTrStatus != KErrNone)
		{
		aClientSock.Close();
		aSockServer.Close();
		aSem.Close();
		return pTestObject->iTrStatus.Int();
 		}
	
	pTestObject->iTransmitThreadResult.Append(_L("Write..."));
	aClientSock.Write(pTestObject->iTestString,pTestObject->iTrStatus);
	User::WaitForRequest(pTestObject->iTrStatus);
	if(pTestObject->iTrStatus!=KErrNone)
		{
		aClientSock.Close();
		aSockServer.Close();
		aSem.Close();
		return pTestObject->iTrStatus.Int();
		}
	
	//-------------------substep 04-----------------------------	
	// Wait the signal from receiver thread to continue
	pTestObject->iTransmitThreadResult.Append(_L("Wait..."));
	aSem.Wait(); 
	
	//-------------------substep 05-----------------------------	
	// Close the socket. Close connection to Socket server.
	pTestObject->iTransmitThreadResult.Append(_L("Exit OK..."));
	aClientSock.Close();
	aSockServer.Close();
	aSem.Close();
	return KErrNone;
	}

//
TInt CTest04IPCMultithreadedDataTransfer::fReceiverThread(TAny* aInput)
	{
	TInt err;
	RSocketServ aSockServer;
	RSemaphore aSem;
	RSocket aNewConn, aServerSock;
    CTest04IPCMultithreadedDataTransfer* pTestObject =
								(CTest04IPCMultithreadedDataTransfer*)aInput;
	
	//-------------------substep 01-----------------------------	
	// Connect to Socket setver, open semaphore and create server socket end point
	pTestObject->iReceiveThreadResult.Format(_L("   Step1..."));
	
	pTestObject->iReceiveThreadResult.Append(_L("Connect to SockServer..."));
	if ( (err = OptimalConnect(aSockServer)) != KErrNone )   
		return err;
	pTestObject->iReceiveThreadResult.Append(_L("Open semaphore..."));
	if ((err = aSem.OpenGlobal(pTestObject->iSemName)) != KErrNone)
		{
		aSockServer.Close();
		return err;
		}
	pTestObject->iReceiveThreadResult.Append(_L("Open Server socket..."));
	if ( (err=aServerSock.Open(aSockServer,pTestObject->iProt)) != KErrNone)
		{
		aSockServer.Close();
		aSem.Close();
		return err;
		}
	if ( (err=aServerSock.SetLocalPort(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return err;
		}
	if ( (err=aServerSock.Listen(1)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return err;
		}
	pTestObject->iReceiveThreadResult.Append(_L("Open Blank Socket..."));
	if ( (err=aNewConn.Open(aSockServer)) != KErrNone)
		{
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return err;
		}
	
	//-------------------substep 02-----------------------------	
	// Send the signal to transmit thread to continue
	pTestObject->iReceiveThreadResult.Append(_L("Signal..."));
	aSem.Signal();
	
	//-------------------substep 03-----------------------------	
	// Read the message from the socket
	pTestObject->iReceiveThreadResult.Append(_L("Accept..."));
	aServerSock.Accept(aNewConn,pTestObject->iRecStatus);
	User::WaitForRequest(pTestObject->iRecStatus);
	if (pTestObject->iRecStatus != KErrNone)
		{
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return pTestObject->iRecStatus.Int();
		}
	int aLen = pTestObject->iTestString.Length(); 
	TAny* aPtr = User::Alloc(aLen);
	if (aPtr == NULL)
		{
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return KErrNoMemory;
		}
	TPtr8 aIn((TUint8*)aPtr,aLen,aLen);
	aIn.SetLength(aLen);
	
	pTestObject->iReceiveThreadResult.Append(_L("Read..."));
	aNewConn.Read(aIn,pTestObject->iRecStatus);
	User::WaitForRequest(pTestObject->iRecStatus);
	if (pTestObject->iRecStatus != KErrNone)
		{
		User::Free(aPtr);
		aNewConn.Close();
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return pTestObject->iRecStatus.Int();
		}
	aNewConn.Close();
	
	//-------------------substep 04-----------------------------	
	// Send the signal to transmit thread to continue
	pTestObject->iReceiveThreadResult.Append(_L("Signal..."));
	aSem.Signal();
	
	//-------------------substep 05-----------------------------	
	// Check the data received against test message that is sent
	pTestObject->iReceiveThreadResult.Append(_L("Compare data..."));
	if (aIn.Compare(pTestObject->iTestString)) 
		{
		User::Free(aPtr);
		aServerSock.Close();
		aSockServer.Close();
		aSem.Close();
		return EFail;
		}
	
	//-------------------substep 02-----------------------------	
	// Close the server socket. Close connection to Socket server. Free memory
	pTestObject->iReceiveThreadResult.Append(_L("Exit OK..."));
	aServerSock.Close();
	aSockServer.Close();
	User::Free(aPtr);
	aSem.Close();
	return KErrNone;
	}


//
TInt CTest04IPCMultithreadedDataTransfer::fInactivityTimeoutThread(TAny* aSelf)	
	{
	CTest04IPCMultithreadedDataTransfer* pTestObject = REINTERPRET_CAST(CTest04IPCMultithreadedDataTransfer*,aSelf);
	
	User::After(KInactivityTimeout);
	if (pTestObject->iRecStatus == KRequestPending)
		{
		pTestObject->iReceiveThreadResult.Append(_L("TIMEOUT..."));
		TRequestStatus* reqStat = &(pTestObject->iRecStatus);
		pTestObject->iThreadRec.RequestComplete(reqStat,KErrTimedOut);
		}
	
	if (pTestObject->iTrStatus == KRequestPending)
		{
		pTestObject->iTransmitThreadResult.Append(_L("TIMEOUT..."));
		TRequestStatus* reqStat = &(pTestObject->iTrStatus);
		pTestObject->iThreadTr.RequestComplete(reqStat,KErrTimedOut);
		}
	
	return KErrNone;
	}

	
// 
enum TVerdict CTest04IPCMultithreadedDataTransfer::InternalDoTestStepL(void)
	{
	
	//-------------------substep 01-----------------------------	
	_LIT(aLog01,"01 Create threads and semaphore:"); Logger().Write(aLog01);
	RThread aTimeoutThread;
	TRequestStatus aStatusRec,aStatusTr;
	
	TInt err=iThreadRec.Create(_L("testThreadRec"), 
		fReceiverThread, 
		KDefaultStackSize, 
		KDefaultHeapSize, 
		KDefaultHeapSize, 
		(TAny*)this,
		EOwnerProcess);
	
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create receiving thread. err = %d");
		Logger().WriteFormat(aLog, err);
		return EFail;
		}
	
	err=iThreadTr.Create(_L("testThreadTr"),
		fTransmitterThread, 
		KDefaultStackSize, 
		KDefaultHeapSize, 
		KDefaultHeapSize, 
		(TAny*)this,
		EOwnerProcess);
	
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create transmitting thread. err = %d");
		Logger().WriteFormat(aLog, err);
		return EFail;
		}
	
	RSemaphore aSem;
	err = aSem.CreateGlobal(iSemName,0);
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create semaphore. err = %d");
		Logger().WriteFormat(aLog, err);
		return EFail;
		}

	err=aTimeoutThread.Create(_L("testThreadTimeout"),
		fInactivityTimeoutThread, 
		KDefaultStackSize, 
		KDefaultHeapSize, 
		KDefaultHeapSize, 
		(TAny*)this,
		EOwnerProcess);
		
	if (err != KErrNone)
		{
		_LIT(aLog,"Error:Could not create timeout thread. err = %d");
		Logger().WriteFormat(aLog, err);
		return EFail;
		}
	
	//-------------------substep 02-----------------------------	
	_LIT(aLog02,"02 Resume threads and wait them to end:"); Logger().Write(aLog02);
	iThreadRec.Logon(aStatusRec);
	iThreadRec.Resume();
	iThreadTr.Logon(aStatusTr);
	iThreadTr.Resume();
	User::WaitForRequest(aStatusRec,aStatusTr);
	if (aStatusRec!=KErrNone && aStatusTr==KRequestPending)
		{
		/* If the receiver completes prematurely with an error before the 
		 * transmitter, the transmitter will hang waiting to be signalled.
		 * So we send 2 signals to ensure that the transmitter completes. */
		aSem.Signal();
		aSem.Signal();
		}
	/* kick off a timeout thread that ensures that the test doesn't hang.
	 * (This can happen when an OOM occurs at an unfavourable time.) */
	aTimeoutThread.Resume();
	User::WaitForRequest(aStatusRec,aStatusTr);
	// We've completed, so the timeout thread won't be needed any longer
	aTimeoutThread.Kill(KErrNone);
	aTimeoutThread.Close();
	aSem.Close();
	iThreadRec.Close();
	iThreadTr.Close();

	TVerdict verdict = EPass;
	Logger().Write(_L("   Info:Receiving thread log:"));
	Logger().Write(iReceiveThreadResult);
	err = aStatusRec.Int();
	if (KErrNone != err)
		{
		_LIT(aLog,"Error:Receiving thread has returned error. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}
	
	Logger().Write(_L("   Info:Transmitting thread log:"));
	Logger().Write(iTransmitThreadResult);
	err = aStatusTr.Int();
	if (KErrNone != err)
		{
		_LIT(aLog,"Error:Transmitting thread has returned error. err = %d");
		Logger().WriteFormat(aLog, err);
		verdict = EFail;
		}
	return verdict;
	}

