// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/** @file
 *
 * Implements CCommSession, CCommSubSession
 */

#include "CS_STD.H"
#include "C32LOG.H"
#include "cs_thread.h"
#include "cs_roles.h"
#include "cs_msgs.h"
#include "cs_glob.h"
#include <comms-infras/c32startcli.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <c32comm_internal.h>
#endif


class CCommSubSession;

void PanicClient(TInt aPanic, const RMessagePtr2& aMessage)
	{
	if(aMessage.Handle() != 0)
		{
		aMessage.Panic(KCommServerName(), aPanic);
		}
	}






TWorkerId CCommSession::WorkerId() const
	{
	return Dealer().WorkerId();
	}


/**
@param aMessage IPC message received from client.
@param aSubSess Subsession that needs to deal with this message.
*/
void CCommSession::ForwardMessageL(const RMessage2 &aMessage, CCommSubSession& aSubSess)
	{
	ForwardMessageL(aMessage, &aSubSess, aSubSess.Player().WorkerId());
	}

/** Determine whether to deal with message directly or to send it on a channel.
@param aMessage IPC message received from client.
@param aWorkerId Id of worker that can handle this message.
*/
void CCommSession::ForwardMessageL(const RMessage2& aMessage, CCommSubSession* aSS, TWorkerId aWorker)
	{
	ASSERT(aWorker <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);

	CC32WorkerThread& owner = C32WorkerThread();

	/**
	While forwarding the message to player, Add player to disconnect list. While closing the session
	the disconnect list is used and session close message is forwarded to player. Only when session
	close response is received, the session is destroyed.
	*/
	// If the Player is co-resident in this worker thread
	if(owner.WorkerId()==aWorker)
		{
		AddPlayerToDisconnectList(aWorker);
		owner.Player()->ProcessMessageL(aMessage, aSS);
		DontCompleteCurrentRequest();
		}
	else if(owner.PeerReachable(aWorker))	// Forward to Player thread
		{
		AddPlayerToDisconnectList(aWorker);
		TC32PlayerForwardRequestMsg msg(aMessage, aSS);
		owner.PostMessage(aWorker, msg);
		DontCompleteCurrentRequest();
		const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
		forwardedMsg.NullHandle();
		}
	else
		{
		C32LOG1(KC32Warning, _L8("CCommSession::ForwardMessageL() Peer Thread Unreachable"));
		DontCompleteCurrentRequest();
		__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
		}
	}

void CCommSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if (aError==KErrBadDescriptor)
		{
		// this completes message as well so we have to handle it in ServiceL
		PanicClient(EBadDescriptor,aMessage);
		return;
		}
	C32LOG1(KC32Detail, _L8("CCommSession::ServiceError() ServiceL leave occured"));
	inherited::ServiceError(aMessage,aError);
	}

void CCommSession::ServiceL(const RMessage2& aMessage)
/**
 * Handle messages for this session.
 *
 * @param aMessage handle to the IPC message from the client
 */
	{
	C32LOG5(KC32Detail,_L8("CCommSession::ServiceL(), Session : 0x%x, IPC: %d (%S). Message: %08x"), this, aMessage.Function(), &TC32Log::C32RequestStr(aMessage.Function()),aMessage.Handle());
	iComplete = ETrue;
	const CC32WorkerThread& owner=C32WorkerThread();
	CC32Dealer& c32Dealer = owner.DealerByRef();

	if (c32Dealer.StartupFailed())
		{
		SafeComplete(aMessage, KErrNotReady);
		return;
		}

	// TestImmediateShutdownPresent is only set when EImmediate shutdown is present, which is
	// used only in testing phase.
	if(c32Dealer.TestImmediateShutdownPresent())
		{
		User::Leave(KErrServerTerminated);
		}

	if((aMessage.Function()==ECommOpen)
	   ||
	   (aMessage.Function()==ECommOpenWhenAvailable))
		{
		NewPortL(aMessage);
		return;
		}

#if defined (_DEBUG)
	switch (aMessage.Function())
		{
		case ECommDbgMarkHeap:
			__UHEAP_MARK;
			SafeComplete(aMessage, KErrNone);
			return;
		case ECommDbgCheckHeap:
			__UHEAP_CHECK(aMessage.Int0());
			SafeComplete(aMessage, KErrNone);
			return;
		case ECommDbgMarkEnd:
			__UHEAP_MARKENDC(aMessage.Int0());
			SafeComplete(aMessage, KErrNone);
			return;
		case ECommDbgFailNext:
  			// We set the fail point for all heaps, rather than just the current Dealer. This could lead to a failure not related
  			// directly to whatever the client test code is trying to exercise but it all helps find bugs
  			c32Dealer.SetFailNextForAllHeaps(aMessage.Int0());
			SafeComplete(aMessage, KErrNone);
			return;
		}
#endif


	switch ((aMessage.Function()))
		{
		case ECommLoadCommModule:
			{
			TFileName fullCSYFilename;
			TInt ret = Read(0,aMessage,fullCSYFilename);
			if (ret != KErrNone)
				{
				C32LOG2(KC32Warning, _L8("ServiceL: LoadCommModule  Read returned %d instead of KErrNone, cannot proceed"), ret);
				PanicClient(EBadDescriptor,aMessage);
				return;
				}

			ret = AddCSYExtension(fullCSYFilename,aMessage);
			if(ret != KErrNone)
				{
				C32LOG2(KC32Warning, _L8("ServiceL: LoadCommModule AddCSYExtension returned %d instead of KErrNone, cannot proceed"), ret);
				return;
				}

 		 	CommsFW::TWorkerId worker;
	 		TBuf8<KMaxFileName> fileName8;
			fileName8.Copy(fullCSYFilename);

 		    TBool found = iThreadManager->FindThreadByFileName(fileName8, worker);
	 		if(!found)
		 		{
		 		worker = iThreadManager->iDefaultThreadIndex;
		 		}

 		 	if(c32Dealer.WorkerExists(worker))
 		 		{
 		 		LoadCommModuleL(aMessage,worker,!found,fullCSYFilename);
 		 		}
 		 	else
 		 		{
				C32LOG2(KC32Dealer,_L8("ServiceL: LoadCommModule requires worker %d. This worker does not exist so starting"),worker);
 		 		ret = c32Dealer.LoadCPMOnLoadCommModule(worker);
 		 		if ((ret!=KErrNone) && (ret!=KErrInUse))
 		 			{
 		 			// only likely return codes here are KErrNoMemory or KErrNotFound if
 		 			// the RS server could not be found - which means system is probably in pretty bad state (ie, no memory)
 		 			// luckily at this point there isn't anything to clean up!
 		 			SafeComplete(aMessage,ret);
 		 			}
 		 		else
 		 			{
	 		 		ret = c32Dealer.ParkRequest(this, aMessage);
	 		 		if(ret != KErrNone)
	 		 			{
	 		 			SafeComplete(aMessage, ret);
	 		 			}
 		 			}
 		 		}
 		 	return;
 		 	}
		case ECommCloseCommModule:
			CloseCommModuleL(aMessage);
			return;
		case ECommPortInfoByName:
			{
			TPortName name;
			TInt ret = Read(1,aMessage,name);
			if (ret != KErrNone)
				{
				C32LOG2(KC32Warning, _L8("ServiceL: LoadCommModule  Read returned %d instead of KErrNone, cannot proceed"), ret);
				PanicClient(EBadDescriptor,aMessage);
				return;
				}
			PortInfoL(aMessage,name);
			return;
			}
		case ECommPortInfoByNumber:		// original msg is not forwarded as global as aMessage.Int2() is not valid in player, instead CSerial* is wrapped in TC32PlayerGetPortInfoMsg
			PortInfo(aMessage,aMessage.Int2());
			return;
		case ECommNumPorts:				// get information from ThreadManager in dealer
			NumPorts(aMessage);
			return;
		case ECommStartServerThread:	// KErrNotSupported
			C32LOG2(KC32Dealer, _L8("WARNING: deprecated function ECommStartServerThread called, CCommSession(%08x)"), this);
			SafeComplete(aMessage, KErrNotSupported);
			return;
		}

	// obtain subsession* from aMessage.Int3()
	CCommSubSession *p = SubSessionFromHandle(aMessage.Int3(), CCommSubSession::ECPort);

	if (aMessage.Function()==ECommClose)
		{
		if (p==NULL)	// not a valid aMessage.Int3()
			{
			SafeComplete(aMessage, KErrBadHandle);
			return;
			}
		else
			{
			CloseSubSessionL(aMessage, CCommSubSession::ECPort);
			return;
			}
		}

	if (p==NULL)	// not a valid aMessage.Int3()
		{
		PanicClient(EBadCommHandle, aMessage);
		return;
		}

	// Its OK to proceed with the dispatch of other requests
	switch (aMessage.Function())
        {
	    case ECommRead:
		case ECommReadCancel:
		case ECommQueryReceiveBuffer:
		case ECommResetBuffers:
		case ECommWrite:
		case ECommWriteCancel:
		case ECommBreak:
		case ECommBreakCancel:
		case ECommCancel:
		case ECommConfig:
		case ECommSetConfig:
		case ECommCaps:
		case ECommSetMode:
		case ECommGetMode:
		case ECommSignals:
		case ECommSetSignalsToMark:
		case ECommSetSignalsToSpace:
		case ECommReceiveBufferLength:
		case ECommSetReceiveBufferLength:
		case ECommSetAccess:
		case ECommOpenWhenAvailableCancel:
	#ifdef _DEBUG
		case ECommDebugState:
	#endif

		// Extensions to the CCommSession starts from here

		case ECommNotifySignals:
		case ECommNotifyFlowControl:
		case ECommNotifySignalsCancel:
		case ECommNotifyFlowControlCancel:
		case ECommGetFlowControl:
		case ECommNotifyConfigChange:
		case ECommNotifyConfigChangeCancel:
		case ECommNotifyBreak:
		case ECommNotifyBreakCancel:
		case ECommGetRole:
		case ECommNotifyDataAvailable:
		case ECommNotifyDataAvailableCancel:
		case ECommNotifyOutputEmpty:
		case ECommNotifyOutputEmptyCancel:
			ForwardMessageL(aMessage, *p);
			break;

		// Extensions to the CCommSession ends to here

		default:
			SafeComplete(aMessage, KErrNotSupported);
			break;

		}
	C32LOG(KC32Detail,_L8("CCommSession::ServiceL() end"));
	}

TInt CCommSession::DisconnectPlayers()
	{
	return iDisconnectPlayers;
	}

CCommSubSession* CCommSession::SubSessionFromHandle(TUint aHandle, CCommSubSession::TSubSessionType aType) const
    {
	iSubSessions.Lock();
	CCommSubSession* subSess = iSubSessions.At(aHandle, aType);
	iSubSessions.Unlock();
	return subSess;
	}

CCommSession::CC32SessionCloseTimer::CC32SessionCloseTimer(CCommSession* aSession)
: CTimer(EPriorityLow),
  iSession(aSession)
	{
	}

CCommSession::CC32SessionCloseTimer* CCommSession::CC32SessionCloseTimer::NewL(CCommSession* aSession)
	{
	CC32SessionCloseTimer* self = new (ELeave) CCommSession::CC32SessionCloseTimer(aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCommSession::CC32SessionCloseTimer::Start()
	{
	CActiveScheduler::Add(this);
	After(KC32SessionCloseDelay);
	}

void CCommSession::CC32SessionCloseTimer::RunL()
	{
	Deque();
	if(iStatus == KErrNone)
		{
		C32LOG3(KC32Detail, _L8("CCommSession::CC32SessionCloseTimer::RunL session = %08x, DisconnectList = %08x. Will panic in debug."), iSession,	iSession->DisconnectPlayers());

		// After sending a sessionclose message to the player the timeout is set and it is in the order
		// of seconds. IF the player doesn't respond (may be unloaded during shutdown, see below comment
		// about stackable csy) and we get to this place, it is a defect and we'd like to catch that in
		// debug mode.

		// NOTE FOR STACKABLE CSY WRITERS
		// The session from csy loading another csy was not closed properly and hence when shutdown was
		// commanded, player CPMs unload, while the session remains. When EDisconnect message is received
		// by the comms server from kernel, session close message is sent to player CPM which has already
		// been unloaded and thus this timer expires. Ensure that all sessions from within csy is closed
		// before shutdown is called

		ASSERT(0);

		CC32Dealer* dealer = const_cast<CC32Dealer*>(&iSession->Dealer());
		dealer->DeleteSession(iSession);
		}
	}

void CCommSession::AddPlayerToDisconnectList(TWorkerId aPlayerId)
	{
	__ASSERT_COMPILE(TC32WorkerThreadPublicInfo::EMaxWorkerThreadId < 32);	// Using a TUint32 as a flag container
	ASSERT(aPlayerId <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);
	__FLOG_STMT(
		if(!IsPlayerInDisconnectList(aPlayerId))
			{
			C32LOG3(KC32Detail, _L8("CCommSession(%08x):\tAddPlayerToDisconnectList(%d)"), this, aPlayerId );
			}
		)
	iDisconnectPlayers |= (1 << aPlayerId);
	}

void CCommSession::RemovePlayerFromDisconnectList(TWorkerId aPlayerId)
	{
	ASSERT(aPlayerId <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);
	__FLOG_STMT(
		if(IsPlayerInDisconnectList(aPlayerId))
			{
			C32LOG3(KC32Detail, _L8("CCommSession(%08x):\tRemovePlayerFromDisconnectList(%d)"), this, aPlayerId );
			}
		)
	iDisconnectPlayers &= ~(1 << aPlayerId);
	}

TBool CCommSession::IsDisconnectListEmpty() const
	{
	C32LOG2(KC32Detail, _L8("CCommSession::IsDisconnectListEmpty %d "), iDisconnectPlayers );
	return iDisconnectPlayers == 0;
	}

TBool CCommSession::IsPlayerInDisconnectList(TWorkerId aPlayerId) const
	{
	// A potential optimisation for the future would be to actively check whether the player still has subsessions here, rather
	// than blinding sending the session close message
	ASSERT(aPlayerId <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);
	return iDisconnectPlayers & (1 << aPlayerId);
	}

void CCommSession::ForgetSubSession(CCommSession* aSelf, CCommSubSession* aSubSession, TInt aSubSessionHandle, TAny*)
	{
	(void) aSubSession;	// suppress UREL warnings
	VERIFY_RESULT(aSelf->iSubSessions.Remove(aSubSessionHandle), aSubSession);
	}

void CCommSession::CountSubSessions(CCommSession*, CCommSubSession*, TInt, TAny* aArg)
	{
	TInt* counter = reinterpret_cast<TInt*>(aArg);
	++*counter;
	}

CCommSession::CCommSession(CC32ThreadManager* aThreadManager)
/**
 * C'Tor - must pass client to CSession
 *
 * @param aClient handle to the Clients thread
 * @param aPortManager pointer to th port manager
 */
	:CSession2(),
	iThreadManager(aThreadManager)
	{
	C32GlobalUtil::NewSession();		// increment iNumSessions
	C32LOG1(KC32Detail, _L8("CCommSession::CCommSession()"));
	}

CCommSession* CCommSession::NewL(CC32ThreadManager* aThreadManager)
	{
	CCommSession* self = new (ELeave) CCommSession(aThreadManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCommSession::ConstructL()
	{
	iSessionCloseTimer = CCommSession::CC32SessionCloseTimer::NewL(this);
	iSubSessions.InitialiseL();			// initialise CC32SubSessionIx
	}

void CCommSession::CloseSubSessionL(const RMessage2& aMessage, CCommSubSession::TSubSessionType aType)
	{
	iSubSessions.Lock();
	CCommSubSession* subSess = iSubSessions.At(aMessage.Int3(), aType);
	C32LOG5(KC32Detail, _L8("CCommSession(%08x):\tCloseSubSession(%08x, %d) - subSess %08x"), this, aMessage.Int3(), aType, subSess);
	ASSERT(subSess);
	iSubSessions.Unlock();
	ForwardMessageL(aMessage, *subSess);
	}

TInt CCommSession::AddCSYToSessionL(const TDesC& aCSYFileName, TBool& aIsDuplicate)
	{
	aIsDuplicate=EFalse;

	TInt k = 0;
	while (k < iCsyCon.Count() && !aIsDuplicate)
		{
		if (aCSYFileName.CompareF(*iCsyCon[k])==0)
			{
			aIsDuplicate=ETrue;
			}
		k++;
		}

	HBufC* name = aCSYFileName.AllocL();
	return iCsyCon.Append(name);
	}

TInt CCommSession::RemoveCSYFromSession(const TDesC& aCSYFileName, TBool& aIsLast)
	{
	TBool aFound=EFalse;
	TInt k = 0;
	while (k < iCsyCon.Count())
		{
		if (iCsyCon[k]->CompareF(aCSYFileName)==0)
			{
            // coverity [dead_error_condition] aFound can be true on later iterations of the while loop
            if (aFound)
				{
				aIsLast=EFalse;
				}
			else
				{
				delete iCsyCon[k];
				iCsyCon.Remove(k);
				aIsLast=ETrue;
				aFound=ETrue;
				return KErrNone;
				}
			}
		k++;
		}
	return KErrNotFound;
	}

CC32WorkerThread& CCommSession::C32WorkerThread() const
	{
	return Dealer().WorkerThread();
	}

void CCommSession::LoadCommModuleL(const RMessage2& aMessage, CommsFW::TWorkerId aWorker, TBool aDefaulted, const TDesC& aFilename)
/**
 Load a comm module
 We assume that the worker we need is loaded if we've come this far.
 aFilename - full filename of CSY
 aDefaulted - True if the supplied filename is for a CSY that is not known via CSYList values in CMI file.
 In this case aWorker will be the default thread
 */
	{
	TBool isDuplicate;		// used both for AddCSYToSessionL and RemoveCSYFromSession (where it is used as isLast)
	TInt ret;
	// Add csy to session, if present we still add csyfilename to session CSY container to keep accesscount but do not
	// forward message, (preserve legacy behaviour where c32 allowed multiple load attempts with KErrNone)
	ret = AddCSYToSessionL(aFilename,isDuplicate);
	if(ret != KErrNone)
		{
		C32LOG2(KC32Warning,_L8("CCommSession::LoadCommModuleL() failed to append CSY to csy container returned %d "), ret);
		SafeComplete(aMessage, ret);
		return;
		}
	// if no memory, or other error, return as such to client
	if (isDuplicate)
		{
		ret = iThreadManager->IncrementCountOnLoad(aFilename);	// increment count for multiple load attempt
		__ASSERT_DEBUG(ret == KErrNone, Fault(EBadState,_L("CCommSession::LoadCommModuleL(): duplicate entry of CSY but not found in Thread Manager !")));
		SafeComplete(aMessage,KErrNone);
		}
	else
		{
		if (aDefaulted)
			{
			C32LOG2(KC32Dealer,_L("CCommSession::LoadCommModuleL(): CSY (%S) not found in Threadmanager so adding"),&aFilename);
			// add csy record since it is unknown, probably after-marked csy
			TBuf8<KMaxFileName> fileName8;   // huge stack usage, but no other way
			fileName8.Copy(aFilename);

			CCSYInfo* csyPtr = NULL;
			TRAP(ret,csyPtr = CCSYInfo::NewL(fileName8, aWorker));
			if (ret==KErrNone)
				{
				ret = iThreadManager->iCSYList.Append(csyPtr);
				if (ret!=KErrNone)
					{
					// failed to add so undo and abort
					delete csyPtr;
					(void)RemoveCSYFromSession(aFilename,isDuplicate);
					SafeComplete(aMessage,ret);
					return;
					}
				}
            else
				{
				// probably OOM
				(void)RemoveCSYFromSession(aFilename,isDuplicate);
				SafeComplete(aMessage,ret);
				return;
				}
			}

		// if this module hasn't been loaded before, it won't have had its portname set. As
		// the portname could be quite long we need to allocate it now to its max size
		// since we can't afford to run out of memory after player responds with the portname after load.

		ret = iThreadManager->GrowCSYPortNameToMaximumIfNotLoaded(aFilename);

		if (ret == KErrNoMemory)
			{
			C32LOG2(KC32Dealer,_L("CCommSession::LoadCommModuleL(): CSY (%S).Ran out of memory growing portname - cleaning up"),&aFilename);
			(void)RemoveCSYFromSession(aFilename,isDuplicate);
			if (aDefaulted)
				{
				C32LOG3(KC32Dealer,_L("CCommSession::LoadCommModuleL(): CSY (%S), cleanup for abort: This was a newly-added default record (at index %d), so deleting"),&aFilename,iThreadManager->iCSYList.Count()-1);
				// we just appended csy record, so remove it
				delete iThreadManager->iCSYList[iThreadManager->iCSYList.Count()-1];
				iThreadManager->iCSYList.Remove(iThreadManager->iCSYList.Count()-1);
				}
			SafeComplete(aMessage,ret);
			return;
			}
		else
			{
			__ASSERT_DEBUG(ret==KErrNone,Fault(EBadState,_L("CCommSession::LoadCommModuleL(): Panicking due to failure to grow CSY portname for unexpected reason: %d" ),ret));
			}


		// entry for unlisted CSY created now, increment count for this CSY, count as in number of times its loaded
		ret = iThreadManager->IncrementCountOnLoad(aFilename);
		__ASSERT_DEBUG(ret==KErrNone,Fault(EBadState,_L("CCommSession::LoadCommModuleL(): Panicking due to failure to increment count for CSY: %d" ),ret));

		// send Message to player
		CC32WorkerThread& owner = C32WorkerThread();
		if(aWorker==TC32WorkerThreadPublicInfo::EMainThread)
			{
			// co-resident player, make direct function call, avoid ProcessMessageL(), as aMessage.Int2() points to global index
			AddPlayerToDisconnectList(aWorker);
			owner.Player()->LoadCommModule(aMessage);
			DontCompleteCurrentRequest();
			}
		else if (owner.PeerReachable(aWorker))
			{
			// send the message via transports wrapped in TC32PlayerLoadCommModuleMsg
			AddPlayerToDisconnectList(aWorker);
			TC32PlayerLoadCommModuleMsg respMsg(aMessage);
			owner.PostMessage(aWorker, respMsg);
			DontCompleteCurrentRequest();
			const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
			forwardedMsg.NullHandle();
			}
		else
			{
			C32LOG2(KC32Warning,_L8("CCommSession::LoadCommModuleL() Peer Thread Unreachable.  RMessage2& = %08x. Will panic in debug"), aMessage.Handle());
			DontCompleteCurrentRequest();
			__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
			}

		}
	}

void CCommSession::SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode)
	{
	if(iComplete)
		{
		::SafeComplete(aMessage, aCompletionCode);
		}
	}

void CCommSession::DontCompleteCurrentRequest()
	{
	iComplete = EFalse;
	}

void CCommSession::CloseCommModuleL(const RMessage2& aMessage)
/**
 * Close a comm module
 */
	{
	TFullName name;      // not a TPortName for backwards compat reasons
	Read(0,aMessage,name);

	TFileName csyfilename;
	TInt ret = iThreadManager->MapPortprefixToCSYFileName(name, csyfilename);	// fill csyfilename
	if (ret!=KErrNone)
		{
		SafeComplete(aMessage, KErrNotFound);
		return;
		}

	TBool isLast;
	TInt res = RemoveCSYFromSession(csyfilename,isLast);	// can return either KErrNone or KErrNotFound in case when csy is not found listed

	if (res==KErrNone)
		{
		// decrement the LoadCount for CSY
		iThreadManager->DecrementCountOnCSYUnLoad(csyfilename);
		if (isLast)
			{
			// if CSY is the last one for the session, then remove from session and forward mesage to player
			// unload message is forwarded "per session basis", so every session sends unload message to player,
			// if this csy was previously loaded by this session, unload message is sent to player either when
			// unloadcommmodule is explicitly called or when session is closed
			CommsFW::TWorkerId worker;
	 		TBuf8<KMaxFileName> fileName8;
			fileName8.Copy(csyfilename);

			__ASSERT_ALWAYS(iThreadManager->FindThreadByFileName(fileName8,worker),Fault(EBadState)); //  find worker
			ForwardMessageL(aMessage, NULL, worker);
			}
		else
			{
			SafeComplete(aMessage, KErrNone);
			}
		}
	else
		{
		// old api used to just return and not panic during an attempt to unload a module not loaded
		// so we do too, altho we log to record this unsavoury behavior
		C32LOG(KC32Warning, _L8("CCommSession::CloseCommModuleL(), Attempt to Close an unloaded module. Continuing due to old API allowing this unadvised behavior."));
		SafeComplete(aMessage, res);	// res = KErrNotFound
		}

	}

void CCommSession::PortInfoL(const RMessage2& aMessage,const TPortName& aName)
/**
 * Write back the port info to the client for a specified port
 *
 * @param aName name of the port/filename to get information about
 */
	{
	TWorkerId worker;
	if(iThreadManager->FindThreadOfActiveCSYByName(aName, worker))
		{
		ForwardMessageL(aMessage, NULL, worker);
		}
	else
		{
		SafeComplete(aMessage, KErrNotFound);
		}
	}

// PortInfo overload requires the CSerial* to be forwarded to player, as the
// aMessage.Int2() has the global index which is not valid in player.
void CCommSession::PortInfo(const RMessage2& aMessage,TInt aNumber)
/**
 * Write back the port info to the client for a specified port
 * aNumber is the client supplied index of the CSY
 * @param aNumber number of the port to get information about
 */
	{
	// check if the aNumber is sane i.e. within the # of CSYs loaded in C32
	if (aNumber < 0)
		{
		SafeComplete(aMessage,KErrArgument);
		return;
		}

	TInt workerId = iThreadManager->FindThreadByGlobalIndex(aNumber);
	if (workerId == KErrNotFound)
		{
		SafeComplete(aMessage, KErrTooBig); // preserve old API behaviour
		}
	else
		{
		CC32WorkerThread& owner = C32WorkerThread();
		CommsFW::TWorkerId thisworker = owner.WorkerId();
		CSerial* s = iThreadManager->FindSerialObjectByGlobalIndex(aNumber);
		// Find Thread
		if(workerId==thisworker)
			{
			// co-resident player, make direct function call, avoid ProcessMessageL(), as aMessage.Int2() points to global index
			AddPlayerToDisconnectList(workerId);
			owner.Player()->PortInfo(aMessage, s);
			DontCompleteCurrentRequest();
			}
		else if (owner.PeerReachable(workerId))
			{
			// send the message via transports wrapped in TC32PlayerGetPortInfoMsg
			C32LOG4(KC32Dealer, _L8("CCommSession::PortInfo(aNumber) Sending Peer Thread %d request for info on CSerial %x which is for CSY at active idx %d"),workerId,s,aNumber);
			AddPlayerToDisconnectList(workerId);
			TC32PlayerGetPortInfoMsg respMsg(aMessage, s);
			owner.PostMessage(workerId, respMsg);
			DontCompleteCurrentRequest();
			const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
			forwardedMsg.NullHandle();
			}
		else
			{
			C32LOG2(KC32Warning, _L8("CCommSession::PortInfo(aNumber) Peer Thread %d Unreachable. Will panic in debug."),workerId);
			DontCompleteCurrentRequest();
			__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
			}
		}
	}

void CCommSession::NumPorts(const RMessage2& aMessage)
/**
 * Write back the number of CSYs loaded to the client
 */
	{
	// returns the number of CSYs loaded in all sessions
	TPckgBuf<TInt> num;

	TInt idx = 0;
	TInt count = 0;
	while (idx < iThreadManager->iCSYList.Count())
		{
		if (iThreadManager->iCSYList[idx]->IsLoaded())
			{
			count++;
			}
		idx++;
		}

	C32LOG2(KC32Detail, _L8("CCommSession::NumPorts = %d "), count);
	num() = count;
	Write(0,aMessage,num);
	SafeComplete(aMessage, KErrNone);
	}

CCommSession::~CCommSession()
/**
 *	D'tor - clean up and go home.
 */
	{
	C32LOG1(KC32Detail, _L8("CCommSession::~CCommSession()"));

	delete iSessionCloseTimer;

	iCsyCon.ResetAndDestroy();		// delete session CSY container

	C32GlobalUtil::SessionClosing();	// decrement iNumSessions

	}

void CCommSession::ProcessSubSessions(TWorkerId aPeerId, TSubSessionProcessor aSubSessionProcessor, TAny* aPtr)
	{
	iSubSessions.Lock();
	CC32SubSessionIx::TIter iter(iSubSessions);

	CCommSubSession* ss;
	TInt ssHandle;
	while((ss = iter.Next(ssHandle)) != NULL)
		{
		if(aPeerId == TC32WorkerThreadPublicInfo::ENullWorkerId || aPeerId == ss->Player().WorkerId())
			{
			aSubSessionProcessor(this, ss, ssHandle, aPtr);
			}
		}
	iSubSessions.Unlock();
	}

void CCommSession::NewPortL(const RMessage2& aMessage)
/**
 * Ask the port manager to open a port in the CSY which is then added
 * to this session's port list. If another session has already opened
 * the same port, the port manager will still give us a reference if
 * the port is not being used exclusively.
 *
 * @param aMessage handle to the IPC message from the client
 */
	{
	C32LOG1(KC32Detail, _L8("CCommSession::NewPort()"));

	TFullName name;      // not a TPortName for backwards compat reasons
	TUint port;
	TInt len;

	TInt ret = ExtractPortNameAndNumber(aMessage, name, port, len);
	if(ret != KErrNone)
		{
		C32LOG2(KC32Detail, _L8("CCommSession::NewPort() - returning KErrBadName to client. Offending portname was: %S"),&name);
        // Bad Handle, (session) dealer completes.
        // it would be nicer to complete with value of "ret", but as old C32 always completed with KErrBadName
        // it seems an easy way to reduce compatibility problems by continuing this
		SafeComplete(aMessage, KErrBadName);
		return;
		}
	else
		{
		// message details are valid, forward the message based on portname "COMM"
		// lookup in ThreadManager, fails if no port-prefix found (if RComm::Open called directly without loading the CSY)
		TWorkerId worker;
		if(iThreadManager->FindThreadByPortPrefix(TPtrC(name.Left(len)), worker))
			{
			ForwardMessageL(aMessage, NULL, worker);
			}
		else
			{
			C32LOG2(KC32Dealer, _L("CCommSession::NewPort() - returning KErrNotFound to client due to no loaded CSY for supplied port: %S"),&name);
			SafeComplete(aMessage, KErrNotFound);
			}
		}
	}

TInt CCommSession::ExtractPortNameAndNumber(const RMessagePtr2& aMessage, TDes& aPortName, TUint& aPortNumber, TInt& aLength)
/**
Extract the port name and number from RMessage
aPortName on return is full name including double-colon and number. The length is not policed for compliance with
TPortName type since this would break a backwards compatibility behaviour.
aLength is length of the actual name part without the double-colon and port number.
*/
	{
  	Read(0,aMessage, aPortName);

	_LIT(KDoubleColon, "::");
	aLength = aPortName.Find(KDoubleColon);
	if (aLength == KErrNotFound)
		{
		return KErrNotFound;
		}
	// extract the numeric value after ::
	TInt numPos = aLength + KDoubleColon.iTypeLength;
	if (numPos == aPortName.Length())
		{
		C32LOG1(KC32Warning, _L("CCommSession::ExtractPortNameAndNumber() - no port number mentioned to open"));
		return KErrBadName; // kerrbadname preserves old c32 return code for this function
		}

	TPtrC numPtr(&aPortName[numPos], aPortName.Length() - numPos);
	TLex lexer(numPtr);
	TInt ret = lexer.Val(aPortNumber);

	return ret;
	}

/** Sends SessionClosed to all no-resident players. Add self to Dealers list of disconnecting sessions.
*/
void CCommSession::Disconnect()
	{
	C32LOG2(KC32Detail, _L8("CCommSession(%08x):\tDisconnect()"), this );

	// Remove all subsessions from the session, ie just as if the client had closed them all individually
	ProcessSubSessions(TC32WorkerThreadPublicInfo::ENullWorkerId, ForgetSubSession, NULL);

	CC32WorkerThread& owner = C32WorkerThread();
	TWorkerId self = owner.WorkerId();

	// remove any parked requests if any as this session is going down.
	owner.Dealer()->RemoveParkedRequestsOnSessionClose(this);

	// run thru our list of csys we've loaded for this session and remove each one
	// and send thru to player to unload each one. As the list allows duplicates,
	// we only send unload for each csy when its last ref for this session
	for(TInt i = iCsyCon.Count() -1; i >= 0; i--)
		{
		// find worker
		TWorkerId worker;
 		TBuf8<KMaxFileName> fileName8;
		fileName8.Copy(*iCsyCon[i]);

		TBool found = iThreadManager->FindThreadByFileName(fileName8, worker);	// TBool found is re-used for testing last csy in session CSY container
		__ASSERT_DEBUG(found, Fault(EBadState,_L("CCommSession::Disconnect: Csy not found in ThreadManager !")));
		// find CSerial object
		CSerial* s = iThreadManager->GetSerialObjectFromCSYFileName(*iCsyCon[i]);
#ifdef _DEBUG
		if (s == NULL)
			{
#ifdef __FLOG_ACTIVE
			iThreadManager->DumpThreadInfoAndCSYLists();
#endif
			TPtrC csyFilename = *iCsyCon[i];
			Fault(EBadState,_L("CCommSession::Disconnect: CSerial not found for CSY (%S) in ThreadManager !"),&csyFilename);
			}
#endif

		TInt res = iThreadManager->DecrementCountOnCSYUnLoad(*iCsyCon[i]);

		TBool isLast;
		res = RemoveCSYFromSession((*iCsyCon[i]), isLast);
		// if its last entry for this csy in TCSYFileNameContainer, send the message to player
		if(res == KErrNone)
			{
			if(isLast)	// last CSY in session CSY container - same TBool found re-used
				{
				if(worker==self)
					{
					// co-resident player, make direct function call, avoid ProcessMessageL(), as aMessage.Int2() points to global index
					owner.Player()->ProcessUnLoadCommModuleMsg(s);
					}
				else if (owner.PeerReachable(worker))
					{
					// send the message via transports wrapped in TC32PlayerUnLoadCommModuleMsg
					TC32PlayerUnLoadCommModuleMsg respMsg(s);
					owner.PostMessage(worker, respMsg);
					}
				else
					{
					C32LOG2(KC32Warning, _L8("CCommSession::Disconnect() Peer Thread (%d) Unreachable. Will Panic in Debug"), worker);
					__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
					}
				}
			}
		}

	TTime time;
	time.HomeTime();
	time+=TTimeIntervalMicroSeconds32(KC32SessionClosePlayerDeadline);
	for(TWorkerId player = TC32WorkerThreadPublicInfo::EMainThread; player <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId; ++player)
		{
		if(IsPlayerInDisconnectList(player))
			{
			if(player == self)
				{
				RemovePlayerFromDisconnectList(player);
				owner.Player()->CloseSession(this);
				}
			else if(owner.PeerReachable(player))
				{
				/* Sending closed session message to involved player, dont care if it times out. After this we just wait
				anyway for a while and then obliterate it.*/
				C32LOG3(KC32Detail, _L8("CCommSession(%08x):\tDisconnect() Sending SessionClose to W%d"), this, player);
				TC32PlayerSessionCloseMsg msg(this, time.Int64());
				owner.PostMessage(player, msg);
				}
			else
				{
				C32LOG(KC32Warning,_L8("CCommSession::Disconnect: Peer not reachable. Will panic in debug."));
				__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker));
				}
			}
		}
	if(!IsDisconnectListEmpty())
		{
		C32LOG3(KC32Detail, _L8("CCommSession(%08x):\tDisconnect() Starting timer [discoList = 0x%x]"), this, iDisconnectPlayers );
		iSessionCloseTimer->Start();
		}
	}

void CCommSession::Disconnect(const RMessage2& aMessage)
	{
	iDisconnectMessage = aMessage;
	C32LOG3(KC32Detail, _L("CCommSession(%08x):\tDisconnect(const RMessage2& = %08x)"), this, aMessage.Handle() );

	Disconnect();

	// If co-resident Player was only one then complete immediately
	if(IsDisconnectListEmpty())
		{
		C32LOG2(KC32Detail, _L8("CCommSession(%08x)::Disconnect(const RMessage2&) inherited disconnect"), this );
		inherited::Disconnect(aMessage);
		}
	}

/** The player has handled the SessionClose message and has cleaned up */
void CCommSession::SessionCloseResp(TWorkerId aPlayerId)
	{
	RemovePlayerFromDisconnectList(aPlayerId);
	if(IsDisconnectListEmpty())
		{
		C32LOG2(KC32Detail, _L8("CCommSession::SessionCloseResp(%08x), Stopping the timer"),this);
		iSessionCloseTimer->Cancel();
		// Dealer()->DeleteSession(...) is called by the caller of this function after this
		}
	}


TInt CCommSession::Write(TInt aPos, const RMessagePtr2& aMessage , const TDesC8& aDes, TInt aOffset)
/**
 * Write and kill the client if it leaves.
 *
 * Copies data from an 8 bit descriptor in the server address space to the client
 * thread's address space. The target location must be a valid modifiable descriptor.
 * Data is copied from the source descriptor to the specified offset position within
 * the target descriptor data area. The length of data copied is the length of the
 * source descriptor. The length of the target descriptor is set to the length of
 * the source descriptor plus the value of the offset.
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data type at this location must be a modifiable descriptor, i.e. aTDes8 type.
 * @param aDes    An 8 bit descriptor in the server address space. This is the source of the copy operation.
 * @param aOffset The offset from the start of the target descriptor data area where copying is to begin.
 *                This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the WriteL() leaves
 */
	{
	//C32LOG4(KC32Detail, _L8("CCommSession::Write() Data = (%s), Pos (%d) Offset (%d)"), aDes.Ptr(), aPos, aOffset);

	TInt ret = aMessage.Write(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CCommSession::Read(TInt aPos, const RMessagePtr2& aMessage , TDes8& aDes, TInt aOffset)
/**
 * Read and kill the client if it leaves.
 *
 * Copies data from the client thread's address space into an 8 bit descriptor
 * in the server address space. The source data must be a valid descriptor.
 * Data is copied from the specified offset position within the source descriptor
 * data area. The length of data copied is the length of source descriptor data
 * minus the offset value. If the offset value is greater than the length of the
 * source descriptor, then no data is copied. The length of data copied is limited
 * to the maximum length of the target descriptor.
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data at this pointer must be a descriptor, i.e. a TDesC8 type.
 * @param aDes    An 8 bit descriptor in the server address space. This is the target
 *                of the copy operation.
 * @param aOffset The offset from the start of the source descriptor data area from where
 *                copying is to begin. This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the ReadL() leaves
 */
	{
	C32LOG3(KC32Detail, _L8("CCommSession::Read(), Pos (%d), Offset (%d)"), aPos, aOffset);

	TInt ret = aMessage.Read(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Detail, _L8("Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CCommSession::Write(TInt aPos, const RMessagePtr2& aMessage , const TDesC16& aDes, TInt aOffset)
/**
 * Write and kill the client if it leaves.
 *
 * (see CCommSession::Write() with 8-bit descriptor)
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data type at this location must be a modifiable descriptor, i.e. aTDes16 type.
 * @param aDes    A 16 bit descriptor in the server address space. This is the source of the copy operation.
 * @param aOffset The offset from the start of the target descriptor data area where copying is to begin.
 *                This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the WriteL() leaves
 */
	{

	//C32LOG4(KC32Detail, _L8("CCommSession::Write(), Data = (%s), Pos (%d), Offset (%d)"), aDes.Ptr(), aPos, aOffset);

	TInt ret = aMessage.Write(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}


TInt CCommSession::Read(TInt aPos, const RMessagePtr2& aMessage , TDes16& aDes, TInt aOffset)
/**
 * Read and kill the client if it leaves.
 *
 * (see CCommSession::Write() with 8-bit descriptor)
 *
 * @param aPtr    A pointer to a valid address within the client thread's address space.
 *                The data at this pointer must be a descriptor, i.e. a TDesC16 type.
 * @param aDes    A 16 bit descriptor in the server address space. This is the target
 *                of the copy operation.
 * @param aOffset The offset from the start of the source descriptor data area from where
 *                copying is to begin. This value must be greater than or equal to zero.
 *
 * @panic This function will panic the client if the ReadL() leaves
 */
	{
	C32LOG3(KC32Detail, _L8("CCommSession::Read(), Pos (%d), Offset (%d)"), aPos, aOffset);

	TInt ret = aMessage.Read(aPos, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Detail, _L8("Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,aMessage);
		}
	return ret;
	}

/**
Checks for a null'd handle before attempting complete.
If handle is null'd then don't complete as this will panic server.

@param aMessage Message to complete
@param aCompletionCode Completion code.
*/
void SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode)
	{
	if(!aMessage.IsNull())
		{
		C32LOG3(KC32Dealer,_L8("CCommSession::SafeComplete(%08x) Completion code: %d"),aMessage.Handle(),aCompletionCode);
		aMessage.Complete(aCompletionCode);
		}
	else
		{
		C32LOG3(KC32Detail,_L8("CCommSession::SafeComplete(%08x) - cannot complete message since Null! Completion code was meant to be: %d"),aMessage.Handle(),aCompletionCode);
		}
	}



TInt AddCSYExtension(TFileName& aFilename, const RMessage2& aMessage)
// implements variation whereby filename is 16-bit, and we want to panic client if the extension won't fit
// KErrNone if no probs or extension already present, KErrBadDescriptor if too short
	{
	TInt r=aFilename.LocateReverse('.');
 	if (r==KErrNotFound)
		{
		if (aFilename.Length()>KMaxFileName - (TInt)KCSYExtension.iTypeLength)	//< valid filename is checked here
			{
			PanicClient(EBadDescriptor,aMessage);
			return KErrBadDescriptor;
			}
		aFilename.Append(KCSYExtension);
		}
	return KErrNone;
	}


//
// CCommSubSession
//

/**
Constructor
*/
CCommSubSession::CCommSubSession(CCommSession* aSession, CPort* aPort, CC32Player* aPlayer)
: iPlayer(aPlayer),
  iAccessCount(1),
  iPort(aPort),
  iSession(aSession)
	{
	C32LOG4(KC32Detail, _L8("CCommSubSession(%08x):\tCPort Port(%08x):\tCCommSubSession Session(%08x)"), this, iPort, iSession );
	}

#ifdef _DEBUG
CCommSubSession::~CCommSubSession()
	{
	C32LOG3(KC32Detail, _L8("CCommSubSession(%08x):\t~CCommSubSession Session(%08x)"), this, iSession );
	}
#endif

void CCommSubSession::RemoveAndDestroy()
	{
	// Remove from the Player's container and delete
	C32LOG3(KC32Detail, _L8("CCommSubSession(%08x):\tRemoveAndDestroy(), session=%08x"), this, iSession );
	CC32Player::TSubSessionContainer& subSessions = Player().SubSessions();
	TInt sessIdx = subSessions.Find(this);
	if(sessIdx >= 0)
		{
		subSessions.Remove(sessIdx);
		if(subSessions.Count() == 0)
			{
			CC32WorkerThread& workerThread = Player().WorkerThread();
			if(workerThread.ShuttingDown())
				{
				workerThread.MaybeTriggerThreadShutdownCallback();
				}
			}
		}
	delete this;
	}

CCommSubSession* CCommSubSession::NewL(CCommSession *aSession, CPort* aPort, CC32Player* aPlayer)
	{
	CCommSubSession *s=new (ELeave) CCommSubSession(aSession, aPort, aPlayer);
	return s;
	}

// EOF - CS_SES.CPP

