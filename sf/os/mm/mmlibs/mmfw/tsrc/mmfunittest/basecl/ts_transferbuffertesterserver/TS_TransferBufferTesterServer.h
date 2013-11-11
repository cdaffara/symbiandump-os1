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


#ifndef TS_TRANSFERBUFFERTESTERSERVER_H
#define TS_TRANSFERBUFFERTESTERSERVER_H

#include "TS_TransferBufferTesterClient.h"
#include <E32TransBuf.h>

#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif

_LIT8(KTestData,"This is test data written server side");


class CTBTSession;

/**
 *
 * Utility to shutdown the transferbuffer tester server
 *
 */
class CTBTServerShutdown : public CTimer
	{
	enum {KTBTServerShutdownDelay=1000000};	// 1s
public:
	inline CTBTServerShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

/**
 *
 * Server used for transfer buffer testing
 * Creates RTransferBuffer/Window in a server process which can be accessed by
 * the client in the client proces
 *
 */
class CTBTServer : public CMmfIpcServer
	{
public:
	//construct / destruct
	static CMmfIpcServer* NewL();
	~CTBTServer();

	void AddSession();
	void DropSession();
private:
	CTBTServer();
	void ConstructL();
	//open/close a session
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;
	TInt RunError(TInt aError);

private:
	TInt iSessionCount;	// The number of sessions
	CTBTServerShutdown iShutdown;// A timer used to shut the server down after all clients have exited.
	};


/**
 *
 * Session for transfer buffer tester Server
 *
 */
class CTBTSession : public CMmfIpcSession
	{
public:
	CTBTSession();
	void CreateL(const CMmfIpcServer& aServer);
	void ConstructL();
	~CTBTSession();

	void ServiceL(const RMmfIpcMessage& aMessage);
	TInt RunError(TInt aError);

private:
	void SetOwnCopyOfMessageL(const RMmfIpcMessage& aMessage);
	void CompleteOwnCopyOfMessage(TInt aReason);
	void CreateTransferBufferL(const RMmfIpcMessage& aMessage);
	void MapBufferIntoClientWindowHandleL(const RMmfIpcMessage& aMessage);
	void WriteTestDataL(const RMmfIpcMessage& aMessage);
	void GetThreadIdL(const RMmfIpcMessage& aMessage);
private:
	CTBTServer* iServer;	//<pointer to owning server
	RMmfIpcMessage iMessage;
	TBool iCanCompleteMessage;
	RTransferWindow iTransferWindow;
	RTransferWindow iClientTransferWindowCopy;
	RTransferBuffer iTransferBuffer;
	RThread iClient;
	};


#endif
