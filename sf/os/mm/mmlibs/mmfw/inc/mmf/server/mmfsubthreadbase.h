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

#ifndef __MMFSUBTHREADBASE_H__
#define __MMFSUBTHREADBASE_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmf/common/mmfcontroller.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfsubthreadbaseimpl.h>
#endif

/**
@publishedAll
@released

Defines the maximum number of event messages that will be held server-side
while waiting for the client to request the next message in the queue.
*/
static const TInt KMMFSubThreadMaxCachedMessages = 4;

/**
@publishedAll
@released

Defines the maximum heap size paramater used when creating the datapath subthread.
*/
static const TInt KMMFSubThreadMaxHeapSize = 0x100000;//1MB


/**
@publishedAll
@released

ITC message ID's used by the client to send commands to the datapath subthread server.
*/
enum TMMFSubThreadMessageIds
	{
  	/**
	Message ID for message to request next event from the sub thread server.
	*/
	EMMFSubThreadReceiveEvents,
	/**
	Message ID for message to cancel a previous request to receive an event from the sub thread server.
	*/
	EMMFSubThreadCancelReceiveEvents,
	/**
	Message ID for message to request that the sub thread shuts itself down.
	*/
	EMMFSubThreadShutdown,
	/**
	Message ID for message to request the datapath subthread load a datapath.
	*/
	EMMFDataPathProxyLoadDataPathBy,
	/**
	Message ID for message to request the datapath subthread load a datapath with a specified 
	media ID.
	*/
	EMMFDataPathProxyLoadDataPathByMediaId,
	/**
	Message ID for message to request the datapath subthread load a datapath with a specified codec.
	*/
	EMMFDataPathProxyLoadDataPathByCodecUid,
	/**
	Message ID for message to request the datapath subthread load a datapath with a specified media 
	ID and codec.
	*/
	EMMFDataPathProxyLoadDataPathByMediaIdCodecUid,
	/**
    Message ID for message to add a data source to the datapath.
	*/
	EMMFDataPathProxyAddDataSource,
	/**
	Message ID for message to add a data sink to the datapath.
	*/
	EMMFDataPathProxyAddDataSink,
	/**
	Message ID for message to prime the datapath.
	*/
	EMMFDataPathProxyPrime,
	/**
	Message ID for message to start the datapath playing.
	*/
	EMMFDataPathProxyPlay,
	/**
	Message ID for message to pause the datapath.
	*/
	EMMFDataPathProxyPause,
	/**
	Message ID for message to stop the datapath.
	*/
	EMMFDataPathProxyStop,
	/**
	Message ID for message to get the datapath's position.
	*/
	EMMFDataPathProxyGetPosition,
	/**
	Message ID for message to set the datapath's position.
	*/
	EMMFDataPathProxySetPosition,
	/**
	Message ID for message to set the datapath's play window.
	*/
	EMMFDataPathProxySetPlayWindow,
	/**
	Message ID for message to clear the datapath's play window.
	*/
	EMMFDataPathProxyClearPlayWindow,
	/**
	Message ID for message to get the datapath's current state.
	*/
	EMMFDataPathProxyState,
	/**
	Unused.
	*/
	EMMFAudioPolicyProxyGetAudioPolicy
	};


class RMMFSubThreadBase; // declared here.
/**
@publishedAll

Base class for clients to MMF sub threads.
Provides functionality to start the sub thread and transmit events from subthread to main thread.
*/
NONSHARABLE_CLASS( RMMFSubThreadBase ): public RMmfSessionBase
	{
public:
	RMMFSubThreadBase(TTimeIntervalMicroSeconds32 aShutdownTimeout) : iShutdownTimeout(aShutdownTimeout) {};
	/**
	Returns the id of the subthread, allowing a client to logon to the thread to receive notification of its death.
	*/
	TThreadId SubThreadId() {return iSubThread.Id();};
	/**
	Allows a client to receive events from the subthread.
	*/
	IMPORT_C void ReceiveEvents(TMMFEventPckg& aEventPckg, TRequestStatus& aStatus);
	IMPORT_C TInt CancelReceiveEvents();
	/**
	Signal to the subthread to exit.
	Note: This function will not return until the subthread has exited, or a timeout has occurred.
	*/
	IMPORT_C void Shutdown();
protected:
	/**
	Should be called by derived classes to start the subthread.
	*/
	TInt DoCreateSubThread(const TDesC& aName, TThreadFunction aFunction, TBool aUseNewHeap = EFalse);
	void Panic(TMMFSubThreadPanicCode aPanicCode);
protected:
	RThread iSubThread;
	TTimeIntervalMicroSeconds32 iShutdownTimeout;
private:
	/**
	Used to determine the success of a logon.  If the status is not pending, the logon has failed
	and the thread should be closed.
	*/
	TRequestStatus iLogonStatus;
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};

#endif

