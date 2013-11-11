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

#ifndef __MMFSUBTHREADBASE_IMPL_H__
#define __MMFSUBTHREADBASE_IMPL_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmf/common/mmfcontroller.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif


/**
@internalTechnology

Used to Kill the subthread either immediately or after a timeout.
Used by the subthread on startup to prevent orphaning if no sessions are created to it.
*/
class CMMFSubThreadShutdown : public CTimer
	{
	enum {EMMFSubThreadShutdownDelay=1000000};	// 1s
public:
	static CMMFSubThreadShutdown* NewL();
	CMMFSubThreadShutdown();
	void ConstructL();
	void Start();
	void ShutdownNow();
private:
	void RunL();
	};

/**
@internalTechnology

Subthread server base class.
Provides session counting and will kill the subthread immediately when the session count reaches zero.
Starts the shutdown timer on construction to prevent orphaning if no sessions are created.
*/
class CMMFSubThreadServer : public CMmfIpcServer
	{
public:
	virtual ~CMMFSubThreadServer();
	virtual void SessionCreated();
	virtual TInt RunError(TInt aError);
	virtual void ShutdownNow();
protected:
	virtual CMmfIpcSession* NewSessionL(const TVersion& aVersion) const = 0;
	CMMFSubThreadServer(TInt aPriority);
	void ConstructL();
private:
	CMMFSubThreadShutdown* iShutdownTimer;
	};

/**
@internalTechnology

Used to hold on to an RMessage so we can complete it asynchronously to send an event to the main thread.
*/
class CMMFSubThreadEventReceiver : public CBase
	{
public:
	static CMMFSubThreadEventReceiver* NewL(const RMmfIpcMessage& aMessage);
	~CMMFSubThreadEventReceiver();
	void SendEvent(const TMMFEvent& aEvent);
private:
	CMMFSubThreadEventReceiver(const RMmfIpcMessage& aMessage);
private:
	RMmfIpcMessage iMessage;
	TBool iNeedToCompleteMessage;
	};

/**
@internalTechnology

Subthread session base class.
Derived classes must implement the ServiceL() method.
*/
class CMMFSubThreadSession : public CMmfIpcSession, public MAsyncEventHandler
	{
public:
	virtual ~CMMFSubThreadSession();
	void CreateL(const CMmfIpcServer& aServer);
	virtual void ServiceL(const RMmfIpcMessage& aMessage) = 0;
	//from MAsyncEventHandler
	TInt SendEventToClient(const TMMFEvent& aEvent);
protected:
	CMMFSubThreadSession() {};
	TBool ReceiveEventsL(const RMmfIpcMessage& aMessage);
	TBool CancelReceiveEvents();
	TBool ShutDown();
protected:
	CMMFSubThreadServer* iServer;
private:
	CMMFSubThreadEventReceiver* iEventReceiver;
	RArray<TMMFEvent> iEvents;
	};



#endif

