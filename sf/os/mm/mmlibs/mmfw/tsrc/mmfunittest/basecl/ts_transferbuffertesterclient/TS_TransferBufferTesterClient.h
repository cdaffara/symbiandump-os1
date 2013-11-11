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


#ifndef TS_TRANSFERBUFFERTESTERCLIENT_H
#define TS_TRANSFERBUFFERTESTERCLIENT_H

#include <e32std.h>
#include <e32base.h>
#include <e32twin.h>
#include <E32TransBuf.h>

#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif

// server name
_LIT(KTransferBufferTesterServerName,"TS_TransferBufferTesterServer");
#ifdef __WINS__
_LIT(KTransferBufferTesterServerImg,"TS_TransferBufferTesterServer");	//<DLL/EXE name
#else
_LIT(KTransferBufferTesterServerImg,"z:\\system\\libs\\TS_TransferBufferTesterServer.exe");	//<DLL/EXE name
#endif
const TUid KTransferBufferTesterServerUid3={0x101f7d35};
#ifdef __WINS__
const TInt KTransferBufferTesterServerStackSize=0x2000;			//  8KB
const TInt KTransferBufferTesterServerInitHeapSize=0x1000;		//  4KB
const TInt KTransferBufferTesterServerMaxHeapSize=0x1000000;	// 16MB
#endif

// A version must be specified when creating a session with the server
const TUint KTransferBufferTesterServerMajorVersionNumber=7;
const TUint KTransferBufferTesterServerMinorVersionNumber=1;
const TUint KTransferBufferTesterServerBuildVersionNumber=1;



// opcodes used in message passing between client and server
enum TTransferBufferTesterRequest
	{
	ECreateTransferBuffer,
	EMapBufferIntoClientWindowHandle,
	EMapOutBuffer,
	EWriteTestData,
	EGetThreadId
	};


//Provides server startup parameters.
class TTransferBufferTesterServerStart
	{
public:
	TTransferBufferTesterServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	TTransferBufferTesterServerStart();
	TInt GetCommand();
	void SignalL();
public:
	TThreadId iId;
	TRequestStatus* iStatus;
	};


//Data passing stuff
typedef TPckgBuf<TRect> TRectBuf;

class RTransferBuffer;
class RTransferWindow;

/**
*
* RTransferBufferTesterClient
*
* This is the client used to test transfer buffers
* Its purpose is to create, populate and map RTransferBuffers
* into a client window from one process to another
* 
* @lib "ts_transferbuffertesterclient.lib"
*/
class RTransferBufferTesterClient : public RMmfSessionBase
	{
public:
	IMPORT_C RTransferBufferTesterClient();
	IMPORT_C TInt Connect();
	
	IMPORT_C TInt CreateTransferBuffer(TUint aSize);
	IMPORT_C TInt MapBufferIntoClientWindowHandle(TInt aClientWindowHandle);
	IMPORT_C TInt WriteTestData(TPtr8& aTestData);
	IMPORT_C TInt GetServerThreadId(TThreadId& aId);
	IMPORT_C void Close();
private:
	HBufC8* iBuffer;
	};



#endif
