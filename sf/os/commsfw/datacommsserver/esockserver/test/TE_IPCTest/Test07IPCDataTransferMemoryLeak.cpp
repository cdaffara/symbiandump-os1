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
// Contains IPC Test 07
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "Test07IPCDataTransferMemoryLeak.h"

const TDesC& CTest07IPCDataTransferMemoryLeak::GetTestName()
	{
	_LIT(ret,"IPCTest07");
	return ret;
	}

enum TVerdict CTest07IPCDataTransferMemoryLeak::InternalDoTestStepL()
	{
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	// Server memory leak checking - this is a contentious thing because the
	// only sure "leak" is when dynamic memory is 100% orphaned, ie no pointer
	// from the root class to it, which we can't determine. The checking in this
	// code used to be at the level of the socket operations, ie like:
	//		iSockServer.__DbgMarkHeap();
	//		ret = socket.DoSomething();
	//		if(ret == KErrNoMemory)
	//			iSockServer.__DbgMarkCheck();
	// but this explicitly requires that upon a OOM condition the SAP will unwind
	// any allocations, such as recv queues, and this is not true and isn't stated
	// in any "contract" - it used to work purely because the recv queue was the
	// last thing allocated, but now the shim introduces a later allocation.
	
	// So here we move the heap mark/check outside all of the socket handling, on
	// the assumption that once all the SAPs are closed then all memory will have
	// been returned. This is still a weak assumption, eg what if the protocol keeps
	// a dynamic array of SAPs, then even once the SAPs have been freed this still
	// preserves a "high water mark" (unless the array is compressed)
	
	TInt err;
	RSocketServ heapSockServer;
	
	CleanupClosePushL (heapSockServer);
	
	if((err = OptimalConnect(heapSockServer)) != KErrNone)
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), err);
		User::Leave(EFail);
		}
	
//	heapSockServer.__DbgMarkHeap();
	
	//-------------------substep 00-----------------------------
	Logger().Write(_L("00 Open socket server:"));
	
	RSocketServ ss;
	CleanupClosePushL(ss);
	if((err = OptimalConnect(ss)) != KErrNone)
		{
		Logger().WriteFormat(_L("Connecting to socket server failed with %d"), err);
		User::Leave(EFail);
		}
	
	//-------------------substep 01-----------------------------
	Logger().Write(_L("01 Create a single client/server connection:"));
	
	RSocket clientSock, newConn, serverSock;
	TRequestStatus acceptStat, connectStat;
	TSockAddr addr;
	
	CleanupClosePushL (clientSock);
	CleanupClosePushL (newConn);
	CleanupClosePushL (serverSock);
	
	if((err = serverSock.Open(ss, iProt)) != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Could not open server socket. err = %d"), err);
		User::Leave(EFail);		
		}
	
	if((err = serverSock.SetLocalPort(1)) != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Could not set port. err = %d"), err);
		User::Leave(EFail);
		}
	
	if((err = serverSock.Listen(1)) != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Could not set up socket to listen. err = %d"), err);
		User::Leave(EFail);		
		}
	
	if((err = newConn.Open(ss)) != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Could not open blank socket. err = %d"), err);
		User::Leave(EFail);		
		}
	
	serverSock.Accept(newConn, acceptStat);
	
	if((err = clientSock.Open(ss, iProt)) != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Could not open socket. err = %d"), err);
		User::Leave(EFail);		
		}
	
	addr.SetPort(1);
	clientSock.Connect(addr, connectStat);
	
	User::WaitForRequest(connectStat);
	User::WaitForRequest(acceptStat);
	TVerdict verdict = EPass;
	
	err = connectStat.Int();
	if (err != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Client socket not connected. err = %d"), err);
		verdict = EFail;	       
		}
	
	err = acceptStat.Int();
	if (acceptStat != KErrNone)
		{
		Logger().WriteFormat(_L("Error: Connection not accepted on server side. err = %d"), err);
		verdict = EFail;		
		}
	
	if (verdict == EFail)
		User::Leave(EFail);
	
	//-------------------substep 02-----------------------------
	Logger().Write(_L("02 Set heap to fails after N=0 allocations..."));
	Logger().Write(_L("   ...Call Write method. If it returns error, check memory leak."));
	Logger().Write(_L("   ...Increase N and repeat the same until write method returns NoError:"));
	
	TBuf8<1> out, in;
	TInt failure = 0;
	TRequestStatus writeStat, readStat;
	
	in.SetMax();
	out.SetMax();
	
	clientSock.Read(in, readStat);
	
	do
		{
		ss.__DbgFailNext(failure);
		
		newConn.Write(out, writeStat);
		User::WaitForRequest(writeStat);
		
		failure++;
		}
        while(writeStat.Int() != KErrNone);
		
	// flush the hanging __DbgFailNext
	ss.__DbgFailNext(-1);
	Logger().WriteFormat(_L("Info: Loop has passed %d times"), failure - 1);
	
	User::WaitForRequest(readStat);
	err = readStat.Int();
	// Depending upon the number of lazy allocs the read may already have been non-fatally failed - try again now we're out of OOM
	if (err == KErrNoMemory)
		{
		clientSock.Read(in, readStat);
		User::WaitForRequest(readStat);
		err = readStat.Int();
		}
	if (err != KErrNone)
		{
		Logger().WriteFormat(_L("Read returned err=%d"), err);
		User::Leave(EFail);
		}
	
	//-------------------substep 03-----------------------------	
	Logger().Write(_L("03 Set heap to fails after N=0 allocations..."));
	Logger().Write(_L("   ...Call Read method. If it returns error, check memory leak."));
	Logger().Write(_L("   ...Increase N and repeat the same until read method returns NoError:"));
	failure = 0;
	
	newConn.Write(out, writeStat);
	
	do
		{
		ss.__DbgFailNext(failure);
		
		clientSock.Read(in, readStat);
		User::WaitForRequest(readStat);
		
		failure++;
		}
		while(readStat.Int() != KErrNone);
		
	// To flush the hanging __DbgFailNext
	ss.__DbgFailNext(-1);
	
	Logger().WriteFormat(_L("Info: Loop has passed %d times"), failure - 1);
	
	User::WaitForRequest(writeStat);
	err = writeStat.Int();
	if(err != KErrNone)
		{
		Logger().WriteFormat(_L("Write returned err=%d"), err);
		User::Leave(EFail);
		}
	
	//-------------------substep 05-----------------------------
	Logger().Write(_L("04 Cleanup stack. Close the sockets & socket server"));
	
	CleanupStack::PopAndDestroy(4);
	
//	heapSockServer.__DbgCheckHeap(0);
	
	CleanupStack::PopAndDestroy();
	
	Logger().WriteFormat(_L("Got here OK"));
	
	return EPass;
#else
	Logger().WriteFormat(_L("Info: Test Disabled on release build"));
	return EPass;
#endif
}

