// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// defines the workerThread class and assoicates. These operate at a level below the roles.h
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef CS_THREAD_H
#define CS_THREAD_H

#include <cfutil.h>
#include <cfextras.h>
#include <elements/cftransport.h>
#include <cs_panic.h>
#include "cs_common.h"


class CCommChannelHandler;

typedef CommsFW::CWorkerThreadRegister<TC32WorkerThreadRegister, KMaxWorkerThreadId> CC32WorkerRegister;


/**
@class
CC32WorkerThread
The Worker Thread is the holder of the relevant C32 objects in the CPM instance,
e.g. the Dealer and the Player (depending on configuration). It is also
in charge of inter-thread communication as it holds the object enabling communication
with the RootServer (CCommChannelHandler) and a list of objects for
communication with other Worker threads (CCommsTransport).

@see CCommChannelHandler
@see CCommsTransport
*/
NONSHARABLE_CLASS(CC32WorkerThread) : public CBase, public CommsFW::MLegacyMessageReceiver
	{
public:
	static CC32WorkerThread* NewL(CommsFW::TCFModuleInfo* aModuleInfo);
	~CC32WorkerThread();

	inline CC32Dealer* Dealer() const;
	inline CC32Player* Player() const;
	inline CommsFW::TWorkerId WorkerId() const;
	inline CC32Dealer& DealerByRef() const;

	/**
	Use this to discover whether this Worker Thread is EMainThread ("C32_Main") which is
	the main Dealer.
	@see TWorkerThreadInfo
	*/
	TBool IsMainThread() const
	    {
	    return WorkerId()==TC32WorkerThreadRegister::EMainThread;
	    }

	/**
	This is the main thread function used by the RootServer when creating a new C32 thread.
	The RootServer will know the DLL ordinal for it, as specified in the .CMI file, and this is where
	an C32 instance starts and ends operation (unless it PANICs).
	@param aArg Will be the module info structure from the RootServer.
	@see CommsFW::TCFModuleInfo
	*/
	IMPORT_C static TInt ThreadEntryPoint(TAny* aArg);
	IMPORT_C static TInt RunC32Thread(CommsFW::TCFModuleInfo* aModuleInfo);

	void SetDealerShutdownComplete(TBool aComplete);
	inline TBool DealerShutdownComplete() const;
	void SetPlayerShutdownComplete(TBool aComplete);
	inline TBool PlayerShutdownComplete() const;

	TBool ShuttingDown() const;
	void SetShuttingDown();
	void SessionShutdownComplete();
	void MaybeTriggerThreadShutdownCallback();
	void TriggerThreadShutdownCallback();

	// RS control binding processing & message forwarding
	void CFBindMessageReceived(const CommsFW::TCFBindMsg& aMsg);
	void CFUnbindMessageReceived(const CommsFW::TCFUnbindMsg& aMsg);
	void CFShutdownMessageReceived(const CommsFW::TCFShutdownMsg& aMsg);


	// Test whether a message
	TBool PeerReachable(CommsFW::TWorkerId aPeerId) const
		{
		return iTransport->PeerReachable(aPeerId);
		}
	// Send a message to a peer
    void PostMessage(CommsFW::TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage);

	// From MLegacyMessageReceiver
	void DispatchL(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId);
	void OnDispatchLeave(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId, TInt aFirstDispatchLeaveReason);

#ifdef _DEBUG
  	inline RAllocator::TAllocFail AllocFailType() const;
  	inline TInt AllocFailRate() const;
#endif

protected:
	void ConstructL(CommsFW::TCFModuleInfo* aModuleInfo);
	CC32WorkerThread();
private:
	void DetermineRoleL(const TDesC8& aIniData, TBool& aIsDealer, TBool& aIsPlayer);
	void ProcessIniDataL();
	TInt DecodePeerId(const CommsFW::TCFSubModuleAddress* aSubModule1, const CommsFW::TCFSubModuleAddress* aSubModule2, CommsFW::TWorkerId& aPeerId);
	void MaybeCompleteUnbindings();
	void MaybeCompleteUnbinding(CommsFW::TWorkerId aWorker);
	static void DeleteHBufC8(TAny* aHBufC);

	/**
	@see ::iProlongBindingLife
	*/
	void IncProlongBindingLife()
		{
		++iProlongBindingLife;
		}

	/**
	@see ::iProlongBindingLife
	*/
	void DecProlongBindingLife()
		{
		--iProlongBindingLife;
		}

private:

	/** This is the handler used for bi-directional communication with the Root Server. */
	CCommChannelHandler* iChannelHandler;

	CC32WorkerRegister* iWorkerRegister;    // allows transport to find other threads
	CommsFW::CCommsTransport* iTransport;
	/**
	Indentification of this thread. No other instance must have the same id.
	@see TWorkerThreadInfo
	*/
	CommsFW::TWorkerId iWorkerId;

	CC32Dealer* iDealer;

	CC32Player* iPlayer;

	/** Set when a CommsFW::TCFShutdownMsg is received from the Root Server. */
	TBool iWorkerShuttingDown;

	/**
	Set by the Dealer instance to signal to the Worker Thread that it has
	finished with the shutdown bookkeeping and is ready to be deleted.
	This essentially means no sessions remain.
	*/
	TBool iDealerShutdownComplete;

	/**
	Set by the Player instance to signal to the Worker Thread that it has
	finished with the shutdown bookkeeping and is ready to be deleted.
	*/
	TBool iPlayerShutdownComplete;

	/**
	If the value of this TInt is larger than 0 any unbind requests will not be served,
	but postponed until iProlongBindingLife is 0. This is to ensure that if e.g. a
	TWorkerMsg::ECleanupDeadPeer or TPlayerMsg::ESessionClose is received they will be fully
	served before completing any unbind and thus deleting channel handlers.
	*/
	TInt iProlongBindingLife;

#ifdef _DEBUG
  	RAllocator::TAllocFail iFailType;
  	TInt iFailRate;
#endif

	};



NONSHARABLE_CLASS (CCommChannelHandler) : public CommsFW::CCFModuleChannelHandler
/** Main Comms Channel traffic handler for the serialserver.
The adapter responsible for communicating with the Root Server over the channel given with the
CommsFW::TCFModuleInfo structure delivered via the parameter to the main thread function.
@internalComponent
*/
	{
	typedef CommsFW::CCFModuleChannelHandler inherited;
public:
	static CCommChannelHandler* NewL(CommsFW::RCFChannel::TMsgQueues aRxQueues,
									 CommsFW::RCFChannel::TMsgQueues aTxQueues, CC32WorkerThread* aWorkerThread);
	TInt Send(const CommsFW::TCFMessage& aMessage);
private:
	CCommChannelHandler(CC32WorkerThread* aWorkerThread);
	virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
	virtual void CFMessageDiscover(const CommsFW::TCFDiscoverMsg& aMessage);
	virtual void CFMessageBind(const CommsFW::TCFBindMsg& aMessage);
	virtual void CFMessageUnbind(const CommsFW::TCFUnbindMsg& aMessage);
private:
	/** Pointer back to the worker thread owning this instance. */
	CC32WorkerThread* iWorkerThread;

	};

#include "cs_thread.inl"

#endif // CS_THREAD_H


