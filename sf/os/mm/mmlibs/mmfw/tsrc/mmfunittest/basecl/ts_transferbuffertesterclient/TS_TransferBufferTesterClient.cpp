// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32math.h>
#include "TS_TransferBufferTesterClient.h"


inline TTransferBufferTesterServerStart::TTransferBufferTesterServerStart(TRequestStatus& aStatus)
	:iId(RThread().Id()),iStatus(&aStatus)
	{aStatus=KRequestPending;}
inline TPtrC TTransferBufferTesterServerStart::AsCommand() const
	{return TPtrC(reinterpret_cast<const TText*>(this),sizeof(TTransferBufferTesterServerStart)/sizeof(TText));}

/**
 *
 * Procedure to start up the TS_TransferBufferTester Server
 * This runs in a separate process to the client (on target only)
 *
 */
static TInt StartServer()
// Start the server process/thread which lives in an EPOCEXE object
	{
	TRequestStatus started;
	TTransferBufferTesterServerStart start(started);

	const TUidType serverUid(KNullUid,KNullUid,KTransferBufferTesterServerUid3);

#ifdef __WINS__
/**	
In WINS the server is a DLL, the exported entrypoint returns a TInt
which represents the real entry-point for the server thread
**/
	RLibrary lib;
	TInt r=lib.Load(KTransferBufferTesterServerImg,serverUid);
	if (r!=KErrNone)
		return r;
	TLibraryFunction ordinal1=lib.Lookup(1);
	TThreadFunction serverFunc=reinterpret_cast<TThreadFunction>(ordinal1());
	//
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	// This uses Math::Random() to generate a 32-bit random number for the name
	//
	TName name(KTransferBufferTesterServerName);
	name.AppendNum(Math::Random(),EHex);
	RThread server;
	r=server.Create(name,serverFunc,
					KTransferBufferTesterServerStackSize,
					&start,&lib,NULL,
					KTransferBufferTesterServerInitHeapSize,KTransferBufferTesterServerMaxHeapSize,EOwnerProcess);
	lib.Close();	// if successful, server thread has handle to library now

#else
/**
EPOC is easy, we just create a new server process. Simultaneous launching
of two such processes should be detected when the second one attempts to
create the server object, failing with KErrAlreadyExists.
**/
	RProcess server;
	TInt r=server.Create(KTransferBufferTesterServerImg,start.AsCommand(),serverUid);

#endif
	if (r!=KErrNone)
		return r;

	TRequestStatus died;
	server.Logon(died);
	if (died!=KRequestPending)
		{
		// logon failed - server is not yet running, so cannot have terminated
		User::WaitForRequest(died);	// eat signal
		server.Kill(0);				// abort startup
		server.Close();
		return died.Int();
		}
	//
	// logon OK - start the server
	
	server.Resume();

	User::WaitForRequest(started,died);		// wait for start or death
	if (started==KRequestPending)
		{
		// server has died, never made it to the startup signal
		server.Close();
		return died.Int();
		}
	//
	// server started (at last). Cancel and consume the death-notification
	// before reporting success
	server.LogonCancel(died);
	server.Close();
	User::WaitForRequest(died);		// eat the signal (from the cancel)
	return KErrNone;
	}


EXPORT_C RTransferBufferTesterClient::RTransferBufferTesterClient()
	{
	iBuffer = NULL; //need to do this as this class is not CBase
	}

/**
 *
 * Client side procedure to connect to the TS_TransferBufferTesterServer
 *
 */
EXPORT_C TInt RTransferBufferTesterClient::Connect()
	{
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KTransferBufferTesterServerName,TVersion(KTransferBufferTesterServerMajorVersionNumber,
																KTransferBufferTesterServerMinorVersionNumber,
																KTransferBufferTesterServerBuildVersionNumber));
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

/**
 *
 * Client side procedure to instruct the server in the server process to create a RTransferWindow
 * and RTransferBuffer of size aSize
 *
 */
EXPORT_C TInt RTransferBufferTesterClient::CreateTransferBuffer(TUint aSize)
	{
	TInt error = SendReceive(ECreateTransferBuffer, TInt(aSize));
	return error;
	}

/**
 *
 * Client side procedure to instruct the server in the server process to map 
 * the server side buffer (created by CreateTransferBuffer and populated by WriteTestData) 
 * into the specified client RTransferWindow specified by aClientWindowHandle
 *
 */
EXPORT_C TInt RTransferBufferTesterClient::MapBufferIntoClientWindowHandle(TInt aClientWindowHandle)
	{
	TInt error =  SendReceive(EMapBufferIntoClientWindowHandle, aClientWindowHandle);
	return error;
	}

/**
 *
 * Utiltiy to get the server thread id
 *
 */
EXPORT_C TInt RTransferBufferTesterClient::GetServerThreadId(TThreadId& aId)
	{
	TPckgBuf<TThreadId> pckg;
	TInt error =  SendReceiveResult(EGetThreadId, pckg);
	aId = pckg();
	return error;
	}


/**
 *
 * Client side procedure to instruct the server in the server process to populate
 * the server RTransferBuffer and return that test data in aTestData
 * The test data is set in KTestData in TS_TransferBufferTesterServer.h
 * and is currently set to the following string: "This is test data written server side"
 *
 */
EXPORT_C TInt RTransferBufferTesterClient::WriteTestData(TPtr8& aTestData)
	{
	delete iBuffer;
	iBuffer = NULL;
	TRAPD(err, iBuffer = HBufC8::NewL(40));
	if (err != KErrNone)
		return err;
	TPtr8 ptr(iBuffer->Des());
	TInt error =  SendReceiveResult(EWriteTestData, ptr);
	if (!error) aTestData.Set(ptr);
	return error;
	}

/**
 *
 * Close RTransferBufferTesterClient
 *
 */
EXPORT_C void RTransferBufferTesterClient::Close()
	{
	if (iBuffer) delete iBuffer;
	RSessionBase::Close();
	}


GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

