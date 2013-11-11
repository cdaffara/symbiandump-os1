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


#ifndef __MMFCONTROLLERFRAMEWORKCLASSES_H__
#define __MMFCONTROLLERFRAMEWORKCLASSES_H__

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif


// fwd ref
class CMMFControllerImplementationInformation;
class CLogonMonitor;


/**
@internalComponent
*/
#define KMMFControllerProxyVersion TVersion(7,1,1)

/**
@internalComponent
*/
const TInt KMMFControllerProxyMaxHeapSize = 0x1000000;//16MB

/**
@internalComponent
*/
const TInt KMMFControllerProxyMaxCachedMessages = 10;

/**
@internalComponent

Signals that the message is destined for the controller proxy
*/
const TInt KMMFObjectHandleControllerProxy = -1;

/**
@internalComponent

The first valid object handle.  The MMF Object container will
begin iterating from here when generating object handles.
*/
const TInt KMMFObjectHandleFirstValid = 1;

/**
@internalComponent
*/
const TInt KMmfControllerThreadShutdownTimeout = 20000000;	// 20 seconds

/**
@internalComponent

The UID and messages associated with the controller proxy interface.
*/
const TUid KUidInterfaceMMFControllerProxy = {0x101F77E7};

/**
@internalComponent

Struct to allow RMMFController to pass several parameters to new CControllerProxyServer threads.
*/
struct TControllerProxyServerParams 
	{
	RServer2*	iServer;
	TBool		iUsingSharedHeap;
	};
	

/**
@internalComponent
*/
enum TMMFControllerProxyMessages
	{
	EMMFControllerProxyLoadControllerPluginByUid,
	EMMFControllerProxyReceiveEvents,
	EMMFControllerProxyCancelReceiveEvents
	};

/**
@internalComponent

Used to Kill the controller thread either immediately or after a timeout.
Used by the controller thread on startup to prevent orphaning if no sessions are created to it.
*/
class CMMFControllerProxyShutdown : public CTimer
	{
	enum {EMMFControllerProxyShutdownDelay=1000000};	// 1s
public:

	/**
	Create a new shutdown timer.

	This method can leave with one of the system-wide error codes.

	@since 7.0s
	*/
	static CMMFControllerProxyShutdown* NewL();

	/**
	Start the shutdown timer.  The RunL of this active object will be called after
	EMMFControllerProxyShutdownDelay microseconds.

	@since 7.0s
	*/
	void Start();

	/**
	Shut down the controller thread immediately.
	
	Calls CActiveScheduler::Stop().

	@since 7.0s
	*/
	void ShutdownNow();
private:
	CMMFControllerProxyShutdown();
	void ConstructL();

	/**
	Calls ShutdownNow().

	@since 7.0s
	*/
	void RunL();
	};


/**
@internalComponent

Used to hold on to an TMMFMessage so we can complete it asynchronously to 
send an event to the client.

@since 7.0s
*/
class CMMFEventReceiver : public CBase
	{
public:

	/**
	Constructs a new event receiver.
	
	This method may leave with one of the system-wide error codes.

	@param  aMessage
	        The message will be completed when an event occurs.

	@return A pointer to the newly created event receiver.

	@since  7.0s
	*/
	static CMMFEventReceiver* NewL(const TMMFMessage& aMessage);

	/**
	Destructor.

	Completes the message with KErrCancel if the message hasn't already been completed.

	@since 7.0s
	*/
	~CMMFEventReceiver();

	/**
	Sends an event to the client.

	@param  aEvent
	        The event.

	@since  7.0s
	*/
	void SendEvent(const TMMFEvent& aEvent);
private:

	/**
	Constructor.

	@param  aMessage
	        The message will be completed when an event occurs.

	@since  7.0s
	*/
	CMMFEventReceiver(const TMMFMessage& aMessage);
private:
	/** 
	The message will be completed when an event occurs.
	*/
	TMMFMessage iMessage;
	};

/**
@internalComponent

The controller proxy server.

The main server inside the controller thread, responsible for creating and destroying the single
session used to transmit messages from the client to the server.

Every controller plugin runs in its own thread, and has its own controller proxy server.

@since 7.0s
*/
class CMMFControllerProxyServer : public CMmfIpcServer
	{
public:
	/**
	Construct the server.

	This method may leave with one of the system-wide error codes.

	@return The newly created server.

	@since  7.0s
	*/
	static CMMFControllerProxyServer* NewL(RServer2* aServer2 );

	/**
	Destructor.

	@since 7.0s
	*/
	~CMMFControllerProxyServer();

	/**
	Static thread function.

	The address of this function is passed into RThread::Create.

	Unpackages the startup parameters and calls DoStartThreadL().

	@param  aAny
	        A pointer to the packaged startup parameters.

	@return One of the system-wide error codes.

	@since 7.0s
	*/
	IMPORT_C static TInt StartThread(TAny* aParam);

	/**
	Called by the active scheduler when the ServiceL of the session leaves.
	Completes the message with the error and restarts the server.

	@param  aError
	        The error that the session ServiceL left with.

	@return  KErrNone
	@since 7.0s
	*/
	TInt RunError(TInt aError);

	/**
	Signals that the session has been created.

	Stops the shutdown timer.

	@since 7.0s
	*/
	void SessionCreated();

	/**
	Signals that the session has been destroyed.
	Causes the server to shut down immediately.

	@since 7.0s
	*/
	void SessionDestroyed();
private:

	/**
	Thread startup code.

	Creates the cleanup stack, installs the active scheduler and creates the server.
	Once all this is completed successfully, it signals the success back to the client.

	This function may leave with one of the system-wide error codes.

	@param  aParams
	        Used to signal startup success back to the client.

	@since 7.0s
	*/
	static void DoStartThreadL(TAny* aParam);

	/**
	Creates a new session.  Only one session may be created with the ControllerProxyServer.

	This function may leave with one of the system-wide error codes.

	@param  aVersion
	        The version number of the session.

	@return A pointer to the new session.

	@since 7.0s
	*/
	CMmfIpcSession* NewSessionL(const TVersion& aVersion) const;

	/** 
	Constructor.

	@since 7.0s
	*/
	CMMFControllerProxyServer();

	/**
	Second phase constructor.

	@since 7.0s
	*/
	void ConstructL(RServer2* aServer2);
	
	/**
	Renaming Controller Proxy Server name
	
	@since 9.2
	*/
	static void RenameControllerProxyThread();

private:
	/** 
	The timer used to shut down the server in case the client fails to connect a session.
	*/
	CMMFControllerProxyShutdown* iShutdownTimer;
	/** 
	Indicates whether we have a session connected.  Only one session is allowed to connect to the server.
	*/
	TBool iHaveSession;
	};

class CMMFController;

/**
@internalComponent

The controller proxy session.

Only one session can be connected to a controller proxy server.

@since 7.0s
*/
class CMMFControllerProxySession : public CMmfIpcSession, public MAsyncEventHandler
	{
public:

	/**
	Construct the session.

	This method may leave with one of the system-wide error codes.

	@return The newly created server.

	@since 7.0s
	*/
	static CMMFControllerProxySession* NewL();

	/**
	Second phase constructor called by the CServer base class.

	This function may leave with one of the system-wide error codes.

	@param  aServer
	        A reference to the server to which this session is attached.

	@since  7.0s
	*/
	void CreateL(const CMmfIpcServer& aServer);

	/**
	Destructor.
	*/
	~CMMFControllerProxySession();

	/**
	Called by the CServer baseclass when a request has been made by the client.

	This function may leave with on of the system-wide error codes. If
	a leave occurs, the message will be automatically completed by the
	RunError() of the Controller Proxy Server.

	@param  aMessage
	        The request to be handled.  The controller proxy session will create
	        a TMMFMessage from this, and pass on the request to the controller
	        base class to handle.

	@since	7.0s
	*/
	void ServiceL(const RMmfIpcMessage& aMessage);

	/**
	Derived from MAsyncEventHandler.

	@see MAsyncEventHandler

	@since	7.0s
	*/
	TInt SendEventToClient(const TMMFEvent& aEvent);
private:
	/**
	Constructor
	*/
	CMMFControllerProxySession();

	/**
	Handle a request from the client to register to receive events from the controller framework.

	This function may leave with one of the system-wide error codes.

	@param  aMessage
	        The request to be handled.

	@return ETrue if the message will be completed now, EFalse if the message will be completed 
	        later.
	*/
	TBool ReceiveEventsL(TMMFMessage& aMessage);

	/**
	Handle a request from the client to stop receiving events from the controller framework.

	This function may leave with one of the system-wide error codes.

	@param  aMessage
	        The request to be handled.

	@return ETrue if the message will be completed now, EFalse if the message will be completed later.
	*/
	TBool CancelReceiveEvents(TMMFMessage& aMessage);

	/**
	Handle a request from the client to load a controller plugin.

	This function may leave with one of the system-wide error codes.

	@param  aMessage
	        The request to be handled.

	@return ETrue if the message will be completed now, EFalse if the message will be completed later.
	*/
	TBool LoadControllerL(TMMFMessage& aMessage);
private:
	/** 
	The controller plugin. 
	*/
	CMMFController* iController;
	/** 
	A pointer to the server. 
	*/
	CMMFControllerProxyServer* iServer;
	/** 
	The event receiver.  Used to send events to the client.
	*/
	CMMFEventReceiver* iEventReceiver;
	/** 
	The events waiting to be sent to the client.
	*/
	RArray<TMMFEvent> iEvents;
	};

#endif //__MMFCONTROLLERFRAMEWORKCLASSES_H__
