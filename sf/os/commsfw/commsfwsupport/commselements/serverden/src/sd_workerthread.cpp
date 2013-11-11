// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "sd_log.h"
#include <ecom/ecom.h>
#include "sd_roles.h"
#include "sd_rootserverchannelhandler.h"
#include "sd_thread.h"
#include "sd_msgs.h"
#include "sd_mintercept.h"
#include <elements/cfmacro.h>
#include <elements/cftransportmsg.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenWrkrTC, "ElemSvrDenWrkrTC");
#endif

using namespace Den;
using namespace CommsFW;
using namespace Elements;

CWorkerRootServChannelHandler::CWorkerRootServChannelHandler(CCommonWorkerThread* aWorkerThread)
:	CCFModuleChannelHandler(CActive::EPriorityStandard),
	iWorkerThread(aWorkerThread)
	{
	}

EXPORT_C CWorkerRootServChannelHandler* CWorkerRootServChannelHandler::NewL(RCFChannel::TMsgQueues aRxQueues,
																   RCFChannel::TMsgQueues aTxQueues,
																   CCommonWorkerThread* aWorkerThread)
	{
	CWorkerRootServChannelHandler* pHandler = new (ELeave) CWorkerRootServChannelHandler(aWorkerThread);
	CleanupStack::PushL(pHandler);
	pHandler->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pHandler);
	return pHandler;
	}

EXPORT_C TInt CWorkerRootServChannelHandler::Send(const TCFMessage &aMessage)
	{
	return inherited::Send(aMessage);
	}


/**
This will be called by the handler when a shutdown message have been received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CWorkerRootServChannelHandler::CFMessageShutdown(const TCFShutdownMsg& aMessage)
	{
	iWorkerThread->CFShutdownMessageReceived(aMessage);
	}

/**
This will be called by the handler when a Discovery message have been received from
the Root Server. It will send a response, telling the Root Server that it has a single
sub-module/binding point which is named after the WorkerID.
As each Worker has a unique ID each worker thus also has a unique sub-module name to bind to,
which potentially allows for simpler handling code and easy to interpret logs.
*/
void CWorkerRootServChannelHandler::CFMessageDiscover(const TCFDiscoverMsg& aMessage)
	{
	// ESOCKV3 CPMs use a single binding point for all messages
	__ASSERT_DEBUG(aMessage.Size() > 0, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 1));
	TBuf<4> subname;
	_LIT(subconst, "%d");
	subname.Format(subconst, iWorkerThread->WorkerId());
	aMessage.SubModuleNames()->Copy(subname);
	TCFDiscoverRespMsg respMsg(aMessage.Identifier(), 1, EFalse);
	VERIFY_RESULT(Send(respMsg), KErrNone);
	}

/**
This will be called by the handler when a Bind message have been received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CWorkerRootServChannelHandler::CFMessageBind(const TCFBindMsg& aMessage)
	{
	iWorkerThread->CFBindMessageReceived(aMessage);
	}

/**
This will be called by the handler when a Unbind message have has received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CWorkerRootServChannelHandler::CFMessageUnbind(const TCFUnbindMsg& aMessage)
	{
	(void) aMessage;
	iWorkerThread->CFUnbindMessageReceived(aMessage);
	}

/**
This will be called by the handler when a transport message has been received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CWorkerRootServChannelHandler::CFMessageForward(const TCFForwardMsg& aMessage)
	{
	iWorkerThread->CFMessageForward(aMessage);
	}


// CCommonWorkerThread class definitions
//

EXPORT_C void CCommonWorkerThread::DeleteHBufC8(TAny* aHBufC)
	{
	delete static_cast<HBufC8*>(aHBufC);
	}

EXPORT_C CCommonWorkerThread::CCommonWorkerThread()
:	iFailType(RAllocator::ENone),
	iFailRate(0)
	{
	}

EXPORT_C CCommonWorkerThread::~CCommonWorkerThread()
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread(%08x)::~CCommonWorkerThread()"), this));
	delete iChannelHandler;
	delete iPlayer;
	delete iDealer;
	delete iShutdownWatchDog;

	if(IsMainThread())
		{
		delete iPitBoss;
		}

	delete iWorkerRegister;
	Messages::TlsGlobals::UnInstallGlobals();
	delete iTransport;

#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	if(IsMainThread())
		{
		// We created and took ownership of the message intercept register so lets be the one to get rid of it
		delete &CMessageInterceptRegister::GetGlobal();
		}
	CMessageInterceptRegister::SetGlobal(NULL);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void CCommonWorkerThread::ConstructL(TCFModuleInfo& aModuleInfo, CShutdownWatchDog& aShutdownWatchDog)
	{
	__ASSERT_DEBUG(iShutdownWatchDog==NULL, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 2));
	iShutdownWatchDog = &aShutdownWatchDog;

	COMMONLOG((iWorkerId, KECommonBootingTag, _L8("CCommonWorkerThread::ConstructL Determining roles")));
	TBool isDealer;
	TBool isPlayer;
	TPlayerRole playerRole;
	DetermineRoleL(aModuleInfo.iIniData, isDealer, isPlayer, playerRole);

	iWorkerRegister = CWorkerRegister::NewL(WorkerId(), NULL);
	iGlobals = &Messages::TlsGlobals::InstallGlobalsL();

	iTransport = CCommsTransport::NewL(*iWorkerRegister, iGlobals->VirtualCtor(), NULL); //walker);
	iGlobals->SetTransportSender(&iTransport->GetSender());
	iGlobals->SetTransportReceiver(&iTransport->GetReceiver());

	iTransport->RegisterLegacyInterface(this);

	if(isDealer)
		{
		COMMONLOG((iWorkerId, KECommonBootingTag, _L8("Creating Dealer.")));
		iDealer = DoCreateDealerL(this, playerRole);

		if(IsMainThread())
			{
			COMMONLOG((iWorkerId, KECommonBootingTag, _L8("Creating PitBoss.")));
			iPitBoss = DoCreatePitBossL(this);
			}		
		}
	else
		{
		SetDealerShutdownComplete(ETrue);
		}

	if(isPlayer)
		{
		COMMONLOG((iWorkerId, KECommonBootingTag, _L8("I am a Player for roles #%08x, creating instance."), playerRole.Role()));
		iPlayer = DoCreatePlayerL(this, playerRole);
		}
	else
		{
		SetPlayerShutdownComplete(ETrue);
		}

#if defined (_DEBUG)
	// Support for simulated heap allocation failures: place "AllocFail= type rate" in .CMI file
	// where:
	//	type == ERandom, ETrueRandom, EDeterministic, EFailNext
	//	rate == rate/chance of failure
	// See RAllocator documentation for details. Best to set this for the heap owner only, since the
	// last one processed will determine function for all heap users & it could get confusing.
	_LIT8(KAllocFailLabel, "AllocFail");
	TPtrC8 allocFail;
	if(GetVarFromIniData(*(aModuleInfo.iIniData), KNullDesC8, KAllocFailLabel, allocFail))
		{
		TLex8 lex(allocFail);
		TPtrC8 failTypeTok = lex.NextToken();
		const struct
			{
			const TText8* iLabel;
			RAllocator::TAllocFail iType;
			} failModes[] =
			{
				{ _S8("ERandom"), RAllocator::ERandom },
				{ _S8("ETrueRandom"), RAllocator::ETrueRandom },
				{ _S8("EDeterministic"), RAllocator::EDeterministic },
				{ _S8("EFailNext"), RAllocator::EFailNext }
			};
		TInt i;
		for(i = sizeof(failModes) / sizeof(failModes[0]) - 1; i >= 0; --i)
			{
			if(TPtrC8(failModes[i].iLabel).CompareF(failTypeTok) == 0)
				{
				break;
				}
			}
		TInt rate = 0;
		lex.SkipSpace();
		if(i < 0 || lex.Val(rate) != KErrNone)
			{
			// Already in Debug mode so just log and panic
			COMMONLOG((iWorkerId, KECommonBootingTag, _L8("Panic - Corrupt Ini data ")));
			User::Panic(KDenFaultPanic, ECommonCorruptIniData);
			}

		iFailType = failModes[i].iType;
		iFailRate = rate;
		}

	/*
	For debug builds, ensure that the array inside the cleanup stack will never
	need to allocate any memory. This aids checking for leaked cells across a
	sequence of calls that is heap-balanced.
	*/
		{
		const TInt KStretchExtent = 10;
		TRAP_IGNORE(
			for(TInt i = 0; i < KStretchExtent; i++)
				{
				CleanupStack::PushL((TAny*)1);
				}
			CleanupStack::Pop(KStretchExtent);
			)
		}
#endif

	// Start listening for binds, etc, from the RS
	COMMONLOG((iWorkerId, KECommonBootingTag, _L8("CWorkerThread::ConstructL Init RS ChannelHandler")));
	iChannelHandler = CWorkerRootServChannelHandler::NewL(aModuleInfo.iRxQueues, aModuleInfo.iTxQueues, this);

#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	if(iDealer)
		{
		if(IsMainThread())
			{
			// Create our process wide esock debug message intercept register singleton here
			// We are the main thread and so in charge of esock debug
			// We are only going to ever create one so something wrong if already exists
			CMessageInterceptRegister* msgIntercept = CMessageInterceptRegister::NewL();
			CMessageInterceptRegister* current = CMessageInterceptRegister::SetGlobal(msgIntercept);
			__ASSERT_DEBUG(!current, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 3));

			// Can also register our debug message interface here
			// The main thread is the only place we will be dispatching them
			iGlobals->RegisterInterfaceL(TMIMessages::ImplementationUid(),
				                         TMIMessages::ImplementationProxyTableSize(),
				                         TMIMessages::ImplementationProxyTable());
			}
		}
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
	}

EXPORT_C void CCommonWorkerThread::DestroyDealer()
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread::DestroyDealer()")));
	delete iDealer;
	iDealer = NULL;
	}

EXPORT_C void CCommonWorkerThread::DestroyPlayer()
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread::DestroyPlayer()")) );
	delete iPlayer;
	iPlayer = NULL;
	}


/** Determine from inidata whether this worker is Dealer, Player or both. */
EXPORT_C void CCommonWorkerThread::DetermineRoleL(HBufC8* aIniData, TBool& aIsDealer, TBool& aIsPlayer, TPlayerRole& aPlayerRole)
	{
	_LIT8(KRoleLabel, "Role");
	_LIT8(KDealerRole, "Dealer");
	_LIT8(KPlayerRole, "Player");
	_LIT8(KDealerPlayerRole, "DealerPlayer");
	_LIT8(KWorkerIdLabel, "WorkerId");
	_LIT8(KDefaultOptimalDealer, "DefaultOptimalDealer");

	__ASSERT_DEBUG(iShutdownWatchDog, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 4)); //Have you called CCommonWorkerThread::ConstructL?
	TPtrC8 roleValue;
	TPtrC8 workerIdValue;
	if(aIniData==NULL || !CommsFW::GetVarFromIniData(*aIniData, KNullDesC8, KRoleLabel, roleValue) ||
		!CommsFW::GetVarFromIniData(*aIniData, KNullDesC8, KWorkerIdLabel, workerIdValue))
		{
		COMMONLOG((WorkerId(), KECommonBootingTag, _L8("Corrupt IniData.")));
#ifdef _DEBUG
			User::Panic(KDenFaultPanic, ECommonCorruptIniData);
#endif
		// leave if ini data corrupt rather than panic unless debug
		User::Leave(KErrCorrupt);
		}
	aIsDealer = EFalse;
	aIsPlayer = EFalse;
	if(roleValue.CompareF(KDealerPlayerRole)==0)
		{
		aIsDealer = ETrue;
		aIsPlayer = ETrue;
		}
	else if(roleValue.CompareF(KDealerRole)==0)
		{
		aIsDealer = ETrue;
		}
	else if(roleValue.CompareF(KPlayerRole)==0)
		{
		aIsPlayer = ETrue;
		}
	else
		{
		COMMONLOG((WorkerId(), KECommonBootingTag, _L8("Corrupt IniData, no role recognised - Must be Dealer, Player or DealerPlayer")));

#ifdef _DEBUG
		User::Panic(KDenFaultPanic,ECommonCorruptIniData);
#endif
		// leave if ini data corrupt rather than panic if not debug
		User::Leave(KErrCorrupt);
		}

	User::LeaveIfError(CommsFW::ConvertVal(workerIdValue, iWorkerId));
	if(iWorkerId > TWorkerThreadPublicInfo::EMaxWorkerThreadId)
		{
		COMMONLOG((WorkerId(), KECommonBootingTag, _L8("Corrupt IniData, invalid WorkerId.")));
#ifdef _DEBUG
		User::Panic(KDenFaultPanic, ECommonCorruptIniData);
#endif
		// leave if ini data corrupt rather than panic if not debug
		User::Leave(KErrCorrupt);
		}

	CommsFW::GetVarFromIniData(*aIniData, KNullDesC8, KDefaultOptimalDealer, iDefaultOptimalDealer);

	if(aIsPlayer)
		{
		DoDeterminePlayerRoleL(aIniData, aPlayerRole);
		}
	}

/**
Check that our end sub-module address is correctly named: the sub-module name must be numeric and match our worker id.
*/
EXPORT_C TInt CCommonWorkerThread::DecodePeerId(const TCFSubModuleAddress* aSubModule1, const TCFSubModuleAddress* aSubModule2, TWorkerId& aPeerId)
	{
	TInt err = KErrNone;
	if(CommsFW::ConvertVal(aSubModule1->SubModule(), aPeerId) != KErrNone || aPeerId != WorkerId())
		{
		err = KErrCorrupt;
		}
	else
		{
		if(CommsFW::ConvertVal(aSubModule2->SubModule(), aPeerId) != KErrNone ||
			aPeerId > TWorkerThreadPublicInfo::EMaxWorkerThreadId)
			{
			err = KErrCorrupt;
			}
		}
	return err;
	}

/**
Deals with binding requests from the Root Server. Note that although the Root Server allows
multiple identical bindings ESock does not allow this and will panic if the binding already exists.
Bindings are expressed in ESock by worker transport objects stored in an array. Since all sub-module
names are unique across all instances (they are the individual owning worker ID converted to text)
they can be used here. I.e. the remote end sub-module is converted back to int and used to insert the
CWorkerTransport into an array in the position corresponding to the remote end sub-module name/number.
As for responding to the bind request there are two cases:
    -# This worker is a "peer Player: Immediately introduce ourselves.
	-# This worker is the Main thread: Wait to receive its introduction message.
@see CWorkerTransport
*/

EXPORT_C void CCommonWorkerThread::SetDealerShutdownComplete(TBool aComplete)
	{
	// Backsliding not permitted; cannot step back to !complete
	if(aComplete)
		{
		iDealerShutdownComplete = aComplete;
		MaybeCompleteUnbindings();
		}
	}

EXPORT_C void CCommonWorkerThread::SetPlayerShutdownComplete(TBool aComplete)
	{
	// Backsliding not permitted; cannot step back to !complete
	if(aComplete)
		{
		iPlayerShutdownComplete = aComplete;
		MaybeCompleteUnbindings();
		}
	}

/**
The unbind requests are only responded to once the channel is presumed to be idle:
   - for Dealers this means no sessions remain with subsessions on that Player
   - for Players this means no subsessions remain for that Dealer
So the peer handler is marked as pending unbind but the unbind response is not sent back
until both Dealer and Player (if any) confirm idleness, which has to be checked whenever
closing sessions or subsessions.
Perhaps this could/should switch to using ref counts?
Once unbind is pending the Dealer refrains from creating any new subsessions which use it.
@see CCommonWorkerThread::MaybeCompleteUnbinding
*/
void CCommonWorkerThread::CFUnbindMessageReceived(const CommsFW::TCFUnbindMsg& aMsg)
	{
	// Get the bound module names for logging purposes.
	#if defined _DEBUG || defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
		TCFFullModuleName fullModName1;
		TCFFullModuleName fullModName2;

		aMsg.SubModule1()->Printable(fullModName1);
		aMsg.SubModule2()->Printable(fullModName2);
	#endif

	// Mark the peer handler as unbinding
    COMMONLOG((WorkerId(), KECommonBootingTag, _L8("W%d: CFUnbindMessageReceived %S <=> %S"),
		WorkerId(),
		&fullModName1,
		&fullModName2 ));
	TWorkerId unbindId;
	TInt err = DecodePeerId(aMsg.SubModule1(), aMsg.SubModule2(), unbindId);
	if(err == KErrNone && !iTransport->PeerReachable(unbindId))
		{
		// This can happen if a module fails during startup (e.g., it is a duplicate protocol
		// which has been forcibly killed by the pitboss before it completed its binding) so
		// we log the failure and handle it silently unless a debugger is attached since it is
		// normally the result of a previous failure.  This enables the module cleanup code to
		// work properly and avoids a situation where the server is left in a bad state if we
		// paniced the ESock_Main thread instead.
		COMMONLOG((WorkerId(), KECommonErrorTag, _L8("ERROR: %S <=> %S cannot be unbound: W%d peer is no longer reachable"), &fullModName1, &fullModName2, unbindId));
		#ifdef _DEBUG
			// Add ASCII delimiters.
			fullModName1.Append('\0');
			fullModName2.Append('\0');

			RDebug::Printf("CCommonWorkerThread::CFUnbindMessageReceived ERROR: %s <=> %s cannot be unbound: W%d peer is no longer reachable", fullModName1.Ptr(), fullModName2.Ptr(), unbindId);
		#endif
		__CF_BREAK_IF_DEBUGGER();
		err = KErrNotFound;
		}
	if(err == KErrNone)
		{
		iTransport->SetLastRequestIdConcerningPeer(unbindId, aMsg.Identifier());
		iTransport->SetDropTransportPending(unbindId, ETrue);
		MaybeCompleteUnbinding(unbindId, aMsg.PeerIsDead());
		}
	else
		{
		CommsFW::TCFUnbindCompleteMsg respMsg(aMsg.Identifier(), err);
		VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
		}
	}

/**
Can be called at any time, will poll all peer handlers for unbind "readiness" and make sure
they complete the unbind if possible.
@see CCommonWorkerThread::MaybeCompleteUnbinding
@return EFalse if one or more bindings remain
*/
EXPORT_C TBool CCommonWorkerThread::MaybeCompleteUnbindings()
	{
	if(iProlongBindingLife <= 0)
		{
		for(TWorkerId player = TWorkerThreadPublicInfo::EMainThread; player <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++player)
			{
			// We always have a transport to ourself & it needs no unbinding
			if(player != WorkerId())
				{
				MaybeCompleteUnbinding(player, EFalse);
				if(iTransport->PeerReachable(player))
					{
					return EFalse;
					}
				}
			}
		return ETrue;
		}
	return EFalse;	// as long as we're in nested prolong a binding *could* remain
	}

/**
If the peer handler have been marked for unbind check whether local Dealer/Player is ready to
unbind from this particular worker. If so, deletes handler and send unbind response to Root Server.
@see CCommonDealer::CanUnbindFromWorker
@see CCommonPlayer::CanUnbindFromWorker
@see TCFUnbindCompleteMsg
*/
EXPORT_C void CCommonWorkerThread::MaybeCompleteUnbinding(TWorkerId aWorker, TBool aPeerIsDead)
	{
	if(iTransport->PeerReachable(aWorker) && iTransport->IsDropTransportPending(aWorker))
		{
		if(aPeerIsDead || !Dealer() || Dealer()->CanUnbindFromWorker(aWorker))
			{
			if(aPeerIsDead || !Player() || Player()->CanUnbindFromWorker(aWorker))
				{
				COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonWorkerThread::MaybeCompleteUnbinding(%d) - dropping transport & unbinding"), aWorker));
				// Let the peer know that we've unbound - if it has management responsibility towards us then
				// it will be waiting. All possible roles unbind simultaneously (although the message could be used
				// more selectively)
				DoCompleteUnbinding(aWorker);
				}
			}
		}
	}


/**
A shutdown message can be of several types:
    -# EImmediate: We shutdown immediately even if leaking resources, although doing a best effort to cleanup. However, SymbianOS doesnt allow the server to gracefully terminate sessions so certain things cant be cleaned up.
	-# EOptional: Ignored.
	-# EGraceful: The module only unloads once no resources remain, which means for the Dealer no sessions and for the Player no subsessions. The shutdown request arrives after the unbind requests.
@see CommsFW::TCFShutdownMsg
@see CommsFW::TCFShutdownType
@see CCommonDealer::ProcessShutdownRequest
@see CCommonPlayer::ProcessShutdownRequest
@see CCommonWorkerThread::SetShuttingDown
@see CCommonWorkerThread::MaybeTriggerThreadShutdownCallback
*/
void CCommonWorkerThread::CFShutdownMessageReceived(const CommsFW::TCFShutdownMsg& aMsg)
	{
	const CommsFW::TCFShutdownType type = aMsg.Type();
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CFShutdownMessageReceived(%d)"), type));

	if(CommsFW::EOptional == type)
		{
		COMMONLOG((WorkerId(), KECommonServerTag, _L8("Type is EOptional: ignoring")));
		return;
		}
	else if(CommsFW::EImmediate == type)
		{
		COMMONLOG((WorkerId(), KECommonServerTag, _L8("Type is EImmediate")));
		PitBoss().SetImmediateShutdownPresent();
		}

	SetShuttingDown();

	if(Dealer())
		{
		Dealer()->ProcessShutdownRequest(type);
		}
	if(Player())
		{
		Player()->ProcessShutdownRequest(type);
		}

	MaybeTriggerThreadShutdownCallback();
	}

EXPORT_C void CCommonWorkerThread::PostMessage(TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage)
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread:\tPostMessage, to %d. message code:%d"), aWorkerId, aMessage.Code()));
	TCFLegacyMessagePacker::PackForPostage(aWorkerId, aMessage);
	iTransport->PostMessage(aMessage);
	}


EXPORT_C void CCommonWorkerThread::CompleteSessionClose(CWorkerSession* aSession)
	{
	TPlayerSessionCloseRespMsg sessionCloseRespMsg(WorkerId(), aSession);
	TWorkerId id=aSession->WorkerId();
	PostMessage(id, sessionCloseRespMsg);
	DecProlongBindingLife();
	if(ShuttingDown())
		{
		Player()->MaybeSetPlayerShutdownComplete(EFalse);
		MaybeTriggerThreadShutdownCallback();
		}
	}

EXPORT_C void CCommonWorkerThread::SendIntroMessage(const CommsFW::TCFModuleName& aPeerName, CommsFW::TWorkerId aPeerId)
	{
	// Introduce ourselves to this new peer.
	TWorkerThreadPublicInfo info;
	// Only the PitBoss needs to own duplicate refs to the delicate bits; if other threads do then should they crash it will
	// prevent proper cleanup of the current thread
	info.Init(WorkerId(), aPeerId == TWorkerThreadPublicInfo::EMainThread);
	info.iWorker = this;

	TWorkerIntroductionMsg introMsg(info);

	if(IsMainThread())
		{
		PitBoss().BindMessageReceived(aPeerName, aPeerId);

#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
		CMessageInterceptRegister& msgIntercept = CMessageInterceptRegister::GetGlobal();
		introMsg.SetMessageInterceptGlobal(&msgIntercept);
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
		}

#ifdef _DEBUG
	introMsg.SetFailType(AllocFailType());
	introMsg.SetFailRate(AllocFailRate());
#endif

	DoFillIntroMessage(aPeerId, introMsg);
	PostMessage(aPeerId, introMsg);
	}

/**
Deal with incoming messages from other workers. If the message is not a known type it might
be a generic transport message and an attempt to use a transporthandler ECOM plugin is made.
*/
EXPORT_C void CCommonWorkerThread::DispatchL(const CommsFW::TCFMessage& aMessage, TWorkerId aSenderId)
	{
	switch(aMessage.Code())
		{
		case TWorkerMsg::EWorkerIntroduction:
			{
			//COMMONLOG
			const TWorkerIntroductionMsg& msg = reinterpret_cast<const TWorkerIntroductionMsg&>(aMessage);
			const TWorkerThreadPublicInfo& info = msg.WorkerInfo();

			if(info.iWorkerId == TWorkerThreadPublicInfo::EMainThread)
				{
				iPitBoss = &info.iWorker->PitBoss();
				iWorkerRegister->SetGlobalThreadRegister(&iPitBoss->WorkerDataGlobals());
				DoProcessWorkerIntroductionL(msg);

#ifdef __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE
				// The main thread is telling us about the location of the message intercept register
				// Something's wrong if we already have some record of it
				CMessageInterceptRegister* current = CMessageInterceptRegister::SetGlobal(msg.MsgInterceptRegister());
				__ASSERT_DEBUG(!current, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 5));
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

				TCFModuleName dummyName;	// we're not the main thread so don't need to use the real module name
				SendIntroMessage(dummyName, TWorkerThreadPublicInfo::EMainThread);
				}
			else
				{
				if(IsMainThread())
					{
					TRAPD(err, PitBoss().ProcessWorkerIntroductionL(msg));

					// Now that the worker has greeted we can report the binding as having started (or error it
					// if we didn't like the greeting)
					TCFBindCompleteMsg bindresp(iTransport->LastRequestIdConcerningPeer(aSenderId), err);
					VERIFY_RESULT(iChannelHandler->Send(bindresp), KErrNone);
					iTransport->SetLastRequestIdConcerningPeer(aSenderId, TCFCommsMessageId::KNullMessageId);
					}
				}
			break;
			}

		case TWorkerMsg::EConfigurationComplete:
			{
			__ASSERT_DEBUG(!IsMainThread(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 6));	// talking to ourself indicates derangement
			//
			if(Dealer())
				{
				Dealer()->ProcessConfigurationComplete(reinterpret_cast<const TWorkerConfigurationComplete&>(aMessage).Type());
				}
			break;
			}
		case TWorkerMsg::ECleanupDeadPeer:
			{
			__ASSERT_DEBUG(!IsMainThread(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 7));
			IncProlongBindingLife();

			// Delete the data handler for the peer - little point in processing any further messages
			TWorkerId peerId = reinterpret_cast<const TWorkerCleanupDeadPeerMsg&>(aMessage).WorkerId();
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("PeerWorkerMessageReceived(%d). ECleanupDeadPeer W%d"), aSenderId, peerId));

			if(Dealer())
				{
				Dealer()->CleanupDeadWorker(peerId);
				}

			if(Player())
				{
				Player()->CleanupDeadWorker(peerId);
				}

	// Now done as a part of the unbinding (below)
	//		// Users of the transport to the dead thread may need to clean-up
	//		iTransportUsers.NotifyPeerDeath(peerId);

			if(iTransport->PeerReachable(peerId))
				{
				MaybeCompleteUnbinding(peerId, ETrue);
				if(iTransport->PeerReachable(peerId))
					{
					COMMONLOG((iWorkerId, KECommonServerTag, _L8("Destroying peer #%d handler"),peerId));
					iTransport->DropTransportToPeer(peerId);
					}
				}

			// Report completion
			if(iTransport->PeerReachable(aSenderId))
				{
				TWorkerCleanupDeadPeerRespMsg respMsg(peerId);
				PostMessage(aSenderId, respMsg);
				}
			else
				{
				// This can happen if a module fails during startup (e.g., it is a duplicate protocol
				// which has been forcibly killed by the pitboss before it completed its binding) so
				// we log the failure and handle it silently unless a debugger is attached since it is
				// normally the result of a previous failure.  This enables the module cleanup code to
				// work properly and avoids a situation where the server is left in a bad state if we
				// paniced the ESock_Main thread instead.
				COMMONLOG((WorkerId(), KECommonErrorTag, _L8("ERROR: Impossible to send worker cleanup response: W%d sender is no longer reachable"), aSenderId));
				#ifdef _DEBUG
					RDebug::Printf("CCommonWorkerThread::DispatchL ERROR: Impossible to send worker cleanup response: W%d sender is no longer reachable", aSenderId);
				#endif
				__CF_BREAK_IF_DEBUGGER();
				}
			DecProlongBindingLife();
			MaybeTriggerThreadShutdownCallback();
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("ECleanupDeadPeer handling done for W%d"), peerId));
			break;
			}
		case TWorkerMsg::ECleanupDeadPeerResp:
			{
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("PeerWorkerMessageReceived(%d). ECleanupDeadPeerResp"), aSenderId));
			__ASSERT_DEBUG(IsMainThread(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 8));
			const TWorkerCleanupDeadPeerRespMsg& msg = reinterpret_cast<const TWorkerCleanupDeadPeerRespMsg&>(aMessage);
			PitBoss().HandleWorkerCleanupCompletionByPeer(msg.WorkerId(), aSenderId);
			break;
			}
		case TPlayerMsg::EForwardRequest:
			{
			__ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 9));
			TPlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TPlayerForwardRequestMsg&>(aMessage));
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("PeerWorkerMessageReceived(%d). EForwardRequest(%d)"), aSenderId, fwdReqMsg.SafeMessage().Function()));

			// If ProcessMessageL() leaves it's the responsibility of the scheduler to complete the message
			Player()->ProcessMessageL(fwdReqMsg.SafeMessage(), fwdReqMsg.SubSession());
			break;
			}
		case TPlayerMsg::ESessionClose:
			{
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("PeerWorkerMessageReceived(%d). ESessionClose"), aSenderId));
			CCommonPlayer* player = Player();
			__ASSERT_DEBUG(player, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 10));

			const TPlayerSessionCloseMsg& sessionCloseMsg = reinterpret_cast<const TPlayerSessionCloseMsg&>(aMessage);
			CCommonSessionProxy* sessProxy = player->FindSessionProxy(sessionCloseMsg.Session());
			if(sessProxy)
				{
				sessProxy->BeginSessionClose();
				}
			else
				{
				CompleteSessionClose(sessionCloseMsg.Session());
				}
			break;
			}
		case TPlayerMsg::ESessionCloseResp:
			{
			COMMONLOG((iWorkerId, KECommonServerTag, _L8("PeerWorkerMessageReceived(%d). ESessionCloseResp"), aSenderId));
			__ASSERT_DEBUG(Dealer(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 11));
			const TPlayerSessionCloseRespMsg& msg = reinterpret_cast<const TPlayerSessionCloseRespMsg&>(aMessage);
			__ASSERT_DEBUG(msg.Session(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 12));

			CWorkerSession* session = msg.Session();
			session->SessionCloseResp(msg.WorkerId());
			if(session->IsDisconnectListEmpty())
				{
				// Deleting the session may allow the binding to be removed, which will
				// cause the channel handler to be deleted
				Dealer()->DeleteSession(session);
				}
			break;
			}
		default:
			{
			//Unrecognised internally, ask derived worker thread
			if(!DoDispatchL(aMessage, aSenderId))
				{
				//Unrecognised message!
				__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 13)); //TODO: nice panic code
				}
			}
		};
	}

/**
The DispatchL function which takes care of incoming legacy messages doesnt TRAP anything,
instead we count on dealing with failures here in the OnDispatchLeave, which is called from
the transport implementation. 
@see CCommonWorkerThread::DispatchL
*/
EXPORT_C void CCommonWorkerThread::OnDispatchLeave(const TCFMessage& aMessage, TWorkerId /*aSenderId*/, TInt aError)
	{
    COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread::OnDispatchLeave() [aMessage.Code=%d] [aError=%d]]"), aMessage.Code(), aError));

	__ASSERT_DEBUG(aError!=KErrNone, User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 14));
	switch(aMessage.Code())
		{
        case TPlayerMsg::EForwardRequest:
            {
            if(aError != KErrBadDescriptor)
                {
                TPlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TPlayerForwardRequestMsg&>(aMessage));
                // Should we also test Message().IsNull(). Should be no aliasing of it now?
                COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread:\tOnDispatchLeave, Complete message(%08x) with %d."), fwdReqMsg.SafeMessage().Handle(), aError));
                __ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ElemSvrDenWrkrTC, 15));
                if (Player()->ShouldCompleteCurrentRequest())
                    {
                    fwdReqMsg.SafeMessage().Complete(aError);
                    }
                }
            break;
            }
		}
	}

EXPORT_C CCommonWorkerDealer* CCommonWorkerThread::WorkerDealer() const
	{
	ASSERT(iWorkerId != TWorkerThreadPublicInfo::EMainThread);
	if (!iDealer->MainDealer())
		{
		return static_cast<CCommonWorkerDealer*>(iDealer);
		}
	else
		{
		return NULL;
		}
	}

EXPORT_C TBool CCommonWorkerThread::ShuttingDown() const
	{
	return iWorkerShuttingDown;
	}

EXPORT_C void CCommonWorkerThread::SetShuttingDown()
	{
	iWorkerShuttingDown = ETrue;
	DoSetShuttingDown();
	}

EXPORT_C void CCommonWorkerThread::SessionShutdownComplete()
	{
	if(IsMainThread())
		{
		PitBoss().SessionShutdownComplete();
		}
	else
		{
		TriggerThreadShutdownCallback();
		}
	}

void CCommonWorkerThread::TriggerThreadShutdownCallback()
	{
	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonWorkerThread::TriggerThreadShutdownCallback()")));
	iShutdownWatchDog->Shutdown(PitBoss().TestImmediateShutdownPresent());
	}

EXPORT_C void CCommonWorkerThread::MaybeTriggerThreadShutdownCallback()
	{
	if(!ShuttingDown())
		{
		return;
		}
	TBool doTrigger = PitBoss().TestImmediateShutdownPresent();
	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("MaybeTriggerThreadShutdownCallback() - immediate=%d, dealerComplete=%d, playerComplete=%d"),
		doTrigger, DealerShutdownComplete(), PlayerShutdownComplete()));
	if(!doTrigger && DealerShutdownComplete())
		{
		if(!PlayerShutdownComplete() && Player())
			{
			Player()->MaybeSetPlayerShutdownComplete(EFalse);
			}
		if(PlayerShutdownComplete())
			{
			// In the multi-plane world shutdown has to be initiated to cue some of the cleanups necessary before unbinding can be done. The
			// 9.1-era code blocked shutdown until the bindings had gone but we need them for final cleanup exchanges.
			doTrigger = ETrue;
			}
		}
	if(doTrigger)
		{
		if(WorkerId() == TWorkerThreadPublicInfo::EMainThread)
			{
			PitBoss().ShutdownIfReady();
			}
		else
			{
			TriggerThreadShutdownCallback();
			}
		}
	}

void CCommonWorkerThread::DropTransportToPeer(TInt aPeer)
	{
	iTransport->DropTransportToPeer(aPeer);
	}


/**
This is the RedShirt function. If a worker thread dies the RedShirt will be run by the PitBoss in
a separate thread and attempt to access memory structures of the dead thread and and assure
that all is cleaned up. Notify all its own transport user ECOM plugins of shutdown. Call the
Player cleanup function on the dead thread.
*/
EXPORT_C TInt CCommonWorkerThread::PostMortemCleanupThreadEntry(TAny* aArg)
	{
	CCommonWorkerThread* self = reinterpret_cast<CCommonWorkerThread*>(aArg);
	self->DoPostMortemCleanup();
	if(self->Player())
		{
		self->Player()->PostMortemCleanup();
		}
	return KErrNone;
	}

/**
@see ::iProlongBindingLife
*/
EXPORT_C void CCommonWorkerThread::IncProlongBindingLife()
	{
	++iProlongBindingLife;
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread::IncProlongBindingLife() iPBL => %d"), iProlongBindingLife));
	}

/**
@see ::iProlongBindingLife
*/
EXPORT_C void CCommonWorkerThread::DecProlongBindingLife()
	{
	--iProlongBindingLife;
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonWorkerThread::DecProlongBindingLife() iPBL => %d"), iProlongBindingLife));
	}

//
// CShutdownWatchDog
//

CShutdownWatchDog::CShutdownWatchDog(CCommonWorkerThread* aWorker, const TCallBack& aShutDownCb)
: CPeriodic(-20/*ECAsyncDeferredPriority*/),
  iWorker(aWorker),
  iShutDownCb(aShutDownCb)
/**
Constructor
*/
	{
	}

/**
Short delay on asynchronous callback to allow for any others to run
@internalComponent
*/
const TInt KCommonServerShutdownLatencyMicroSeconds = 100 * 1000;
/**
Short delay on asynchronous callback to allow for any others to run
@internalComponent
*/
const TInt KCommonServerShutdownRetryIntervalMicroSeconds = 500 * 1000;


EXPORT_C void CShutdownWatchDog::Shutdown(TBool aImmediate)
/**
Que ourselves to shut the socket server down after a suitable delay.
*/
	{
	COMMONLOG((iWorker->WorkerId(), KECommonServerTag, _L8("W%d: CShutdownWatchDog::Shutdown()"), iWorker->WorkerId()));

	iImmediate = aImmediate;
	if (!IsAdded())
		{
		CActiveScheduler::Add(this);
		}
	if(!IsActive())
		{
		Start(KCommonServerShutdownLatencyMicroSeconds, KCommonServerShutdownRetryIntervalMicroSeconds, TCallBack(CShutdownWatchDog::TryShutdown, this));
		}
	}

TInt CShutdownWatchDog::TryShutdown(TAny* aSelf)
/**
Call the socket server to shut itself down - this must only be done once there are no active sessions.
*/
	{
	CShutdownWatchDog* self = reinterpret_cast<CShutdownWatchDog*>(aSelf);

#ifdef SYMBIAN_TRACE_ENABLE
	TInt workerId = self->iWorker->WorkerId();
#endif
	COMMONLOG((workerId, KECommonServerTag, _L8("W%d: CShutdownWatchDog::TryShutdown entry"), workerId));

	// For a graceful shutdown we need:
	//	(1) all interfaces registered after interaction with peers deregistered
	//	(2) all bindings to peers dropped - this can only happen after (1) because most interfaces are registered as a consequence of interaction
	//		with peers, which is done across bindings. Generally interfaces are only deregistered once we know a peer has stopped using it, and
	//		generally we only know this because of a message received from them
	// For a more urgent shutdown we don't care, but of course other workers might hang, crash, etc.
	// Because individual interfaces aren't tracked we detect (1) by waiting for the number of remaining interfaces to match that at the start.
	if(!self->iImmediate)
		{
		if(!self->iWorker->MaybeCompleteUnbindings())
			{
			COMMONLOG((workerId, KECommonServerTag, _L8("W%d: CShutdownWatchDog::TryShutdown - bindings remaining, will try again..."), workerId));
			return 0;
			}
		}

	// End of the line
	self->iWorker->PitBoss().PeerWorkerExiting(self->iWorker->WorkerId());
	// Give Dealers a chance to orphan subsessions before Players destroy them
	self->iWorker->DestroyDealer();
	// Need to delete any orphaned subsessions before shutting down NifMan
	self->iWorker->DestroyPlayer();

	// Complete any final unbinds against peer workers (may have been deferred to allow divided Player roles to cleanup)
	self->iWorker->MaybeCompleteUnbindings();

	//Custom stuff
	self->iShutDownCb.CallBack();

	COMMONLOG((workerId, KECommonServerTag, _L8("SocketServer W%d: objects destroyed, waiting to drop Scheduler"), workerId));

	CActiveScheduler::Stop();
	COMMONLOG((workerId, KECommonServerTag, _L8("W%d: CShutdownWatchDog::TryShutdown Finished"), workerId));
	// Don't need to delete self; the now dead globals owned us & have already deleted us
	return 0;
	}

EXPORT_C CShutdownWatchDog* CShutdownWatchDog::NewL(CCommonWorkerThread* aWorker, const TCallBack& aShutDownCb)
	{
	CShutdownWatchDog* t = new(ELeave) CShutdownWatchDog(aWorker, aShutDownCb);
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop(t);
	return t;
	}


