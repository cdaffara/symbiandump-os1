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
//

/**
 @file
 @internalComponent
*/

#include "cs_thread.h"
#include "C32LOG.H"
#include "CS_STD.H"
#include "cs_roles.h"
#include "cs_msgs.h"
#include "cs_glob.h"
#include <elements/nm_address_internal.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cflog.h>
#endif

#include <elements/cftransportmsg.h>

using namespace CommsFW;

#ifndef KLogSubSysSerComms
__CFLOG_STMT(_LIT8(KLogSubSysSerComms, "SerComms");)
#endif

//
//	CC32WorkerThread class definitions
//

CC32WorkerThread* CC32WorkerThread::NewL(TCFModuleInfo* aModuleInfo)
	{
	CleanupStack::PushL(TCleanupItem(DeleteHBufC8, aModuleInfo->iIniData));	
	CC32WorkerThread* self = new (ELeave) CC32WorkerThread;
	CleanupStack::PushL(self);
	self->ConstructL(aModuleInfo);
	CleanupStack::Pop(self);
	CleanupStack::PopAndDestroy();	// aModuleInfo->iIniData	
	return self;
	}

CC32WorkerThread::CC32WorkerThread()
#ifdef _DEBUG  
   : iFailType(RAllocator::ENone)
#endif
	{
	}

/**
The worker thread secondary construction will create the relevant Player/Dealer
instances needed as well as the channel handler to the Root Server. If and only if it
is the main thread it will also create the PitBoss.
@note If it has a Dealer and is not the main thread it is a WorkerDealer.
*/
void CC32WorkerThread::ConstructL(TCFModuleInfo* aModuleInfo)
	{
	TBool isDealer;
	TBool isPlayer;
	__CFLOG_OPEN;
	C32LOG1(KC32Bootup,_L8("CC32WorkerThread::ConstructL Determining roles"));
	DetermineRoleL(*aModuleInfo->iIniData, isDealer, isPlayer);

	iWorkerRegister = CC32WorkerRegister::NewL(WorkerId(), NULL);
	iTransport = CCommsTransport::NewL(*iWorkerRegister, NULL, NULL);
	iTransport->RegisterLegacyInterface(this);

	// initglobals allocates on heap and stores pointer in TLS, so
	// requires special cleanup if we leave beyond here - handled in RunC32Thread
	C32GlobalUtil::InitC32GlobalsL(this);
	
#ifdef _DEBUG
  	if(aModuleInfo->iIniData!=NULL)
  		{
	  	// Support for simulated heap allocation failures: place "AllocFail= type rate" in .CMI file
	  	// where:
	  	//	type == ERandom, ETrueRandom, EDeterministic, EFailNext
	  	//	rate == rate/chance of failure
	  	// See RAllocator documentation for details. Best to set this for the heap owner only, since the
	  	// last one processed will determine function for all heap users & it could get confusing.
	  	_LIT8(KAllocFailLabel, "AllocFail");
	  	TPtrC8 allocFail;
	  	if(GetVarFromIniData(*(aModuleInfo->iIniData), KNullDesC8, KAllocFailLabel, allocFail))
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
	  			C32LOG1(KC32Bootup, _L("Panic - Corrupt Ini data - AllocFail param"));
	  			Fault(EBadIniData);
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

  		}
#endif	// _DEBUG
	
	if(isDealer)
		{
		if(IsMainThread())
			{
			C32LOG(KC32Bootup, _L8("I am the Main Thread. Creating Dealer."));
			iDealer = CC32Dealer::NewL(this,*(aModuleInfo->iIniData));
			if(iDealer->ThreadManager())
				{
				if(iDealer->ThreadManager()->DefaultThread() == KMainThreadId)
					{
					/*
					The default thread which loads unlisted CSYs is the dealer/main thread (workerId=0),
					this isn't	normal in a multi-threaded configuration, but can happen due to 
					1. BAD configuration (would fault in debug builds to signal a corrupt configuration, but not in release)
					2. CMI files with no C32SerComms group field in it.
					3. CSYList=* field present in IniData section of dealer/main thread (workerId=0) [valid, but discouraged in multi-threaded conf]
					*/
					C32LOG(KC32Warning,_L8("Dealer is default thread for loading unlisted CSYs! A BAD configuration perhaps ?"));
					isPlayer=ETrue;
					}
				}
			}
		}
	else
		{
		SetDealerShutdownComplete(ETrue);
		}

	if(isPlayer)
		{
		C32LOG1(KC32Bootup, _L8("I am a Player, creating instance."));
		iPlayer = CC32Player::NewL(this);
		}
	else
		{
		SetPlayerShutdownComplete(ETrue);
		}


	// Start listening for binds, etc, from the RS
	C32LOG1(KC32Bootup, _L8("CC32WorkerThread::ConstructL Init RS ChannelHandler"));
	iChannelHandler = CCommChannelHandler::NewL(aModuleInfo->iRxQueues, aModuleInfo->iTxQueues, this);
	C32LOG1(KC32Bootup, _L8("CC32WorkerThread::ConstructL Done"));
	}

CC32WorkerThread::~CC32WorkerThread()
	{
	C32LOG2(KC32Shutdown, _L8("CC32WorkerThread(%08x)::~CC32WorkerThread()"), this);
	delete iTransport;
	delete iChannelHandler;
	delete iPlayer;
	if(IsMainThread())
		{
		delete iDealer;
		}
	delete iWorkerRegister;
	C32LOG2(KC32Shutdown, _L8("CC32WorkerThread(%08x)::~CC32WorkerThread() complete"), this);
	__CFLOG_CLOSE;
	}

/** Determine from inidata whether this worker is Dealer, Player or both. */
void CC32WorkerThread::DetermineRoleL(const TDesC8& aIniData, TBool &aIsDealer, TBool &aIsPlayer)
	{
	aIsDealer = EFalse;
	aIsPlayer = EFalse;
	// BC when No IniData present, this CPM is both Dealer and Player
	if(&aIniData==NULL)
		{
		aIsDealer = ETrue;
		aIsPlayer = ETrue;
		}
	else
		{
		TPtrC8 roleValue;
		// Role missing - Dealer
		if (!GetVarFromIniData(aIniData, KNullDesC8, KRoleLabel, roleValue))
			{
			aIsDealer = ETrue;
			}
		else
			{
			// Role present - player
			if (roleValue.CompareF(KPlayerRole)==0)
				{
				aIsPlayer = ETrue;
				}
			// Role present - dealer
			else if (roleValue.CompareF(KDealerRole)==0)
				{
				aIsDealer = ETrue;
				}
			// Role present - invalid arg
			else
				{
				C32LOG2(KC32Warning, _L8("Invalid Role in [IniData] section: %S"),&roleValue);
				__ASSERT_DEBUG(0,Fault(EBadIniData));
				}
			}

		// WorkerId missing
		TPtrC8 workerIdValue;
		if(!GetVarFromIniData(aIniData, KNullDesC8, KWorkerIdLabel, workerIdValue))
			{
			C32LOG1(KC32Warning, _L8("Corrupt [IniData]: WorkerId missing."));
			__ASSERT_DEBUG(0,Fault(EBadIniData));
			// leave if ini data corrupt rather than panic unless debug 
			User::Leave(KErrCorrupt);		
			}

		User::LeaveIfError(ConvertVal(workerIdValue, iWorkerId));
		// check for boundary values for WorkerId
		if(iWorkerId > TC32WorkerThreadPublicInfo::EMaxWorkerThreadId)
			{
			C32LOG2(KC32Warning, _L8("Corrupt [IniData]: Invalid WorkerId: %d"),iWorkerId);
			__ASSERT_DEBUG(0,Fault(EBadIniData));
			// leave if ini data corrup rather than panic if not debug
			User::Leave(KErrCorrupt);
			}

		if(iWorkerId == TC32WorkerThreadPublicInfo::EMainThread && aIsDealer == EFalse)
			{
			C32LOG1(KC32Warning, _L8("Worker Id zero with no dealer role. overriding"));
			aIsDealer = ETrue;
			}

		// CSYList present - Player
		TPtrC8 csyListValue;
		if(GetVarFromIniData(aIniData, KNullDesC8, KCSYListLabel, csyListValue))
			{
			aIsPlayer = ETrue;
			}
		}	// End of else block [if(aIniData==NULL)]
	}



/**
Check that our end sub-module address is correctly named: the sub-module name must be numeric and match our worker id.
*/
TInt CC32WorkerThread::DecodePeerId(const TCFSubModuleAddress* aSubModule1, const TCFSubModuleAddress* aSubModule2, TWorkerId& aPeerId)
	{
	TInt err = KErrNone;
	if(ConvertVal(aSubModule1->SubModule(), aPeerId) != KErrNone || aPeerId != WorkerId())
		{
		err = KErrCorrupt;	
		}
	else 
		{
		if(ConvertVal(aSubModule2->SubModule(), aPeerId) != KErrNone || 
			aPeerId > TC32WorkerThreadPublicInfo::EMaxWorkerThreadId)
			{
			err = KErrCorrupt;	
			}
		}
	return err;
	}


/**
Deals with binding requests from the Root Server. Note that although the Root Server allows
multiple identical bindings C32 does not allow this and will panic if the binding already exists.
Bindings are expressed in C32 by CCommsTransport. Since all sub-module
names are unique across all C32 instances (they are the individual owning worker ID converted to text)
they can be used here. I.e. the remote end sub-module is converted back to int and used to insert the
CCommsTransport into an array in the position corresponding to the remote end sub-module name/number.
As for responding to the bind request there are two cases:
    -# This worker is a "dumb" Player: Send bind response immediately.
	-# This worker is the Main thread: Send introduction messages to remote end and 
	postpone bind response until introduction response messages have arrived back.
@see CCommsTransport
*/
void CC32WorkerThread::CFBindMessageReceived(const TCFBindMsg& aMsg)
	{
    __CFLOG_SMADDR2(( KLogSubSysSerComms, KLogSubSysSerComms, _L8("W%d: CFBindMessageReceived %S <=> %S"),
		WorkerId(),
		&aMsg.SubModule1()->Printable(__FullModName1), 
		&aMsg.SubModule2()->Printable(__FullModName2) ));
	TWorkerId bindId;
	TInt err = DecodePeerId(aMsg.SubModule1(), aMsg.SubModule2(), bindId);
	ASSERT(bindId <=  TC32WorkerThreadPublicInfo::EMaxWorkerThreadId);
	if(err == KErrNone)
		{
		if(iTransport->PeerReachable(bindId))
			{
			C32LOG2(KC32Bootup, _L("%d Already exists. Error in configuration of cmi files"),bindId);
			//Must panic b/c the new module will be left in a half bound state and will tend to get out of hand
			__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransInvalidWorkerId));
			}
		else
			{
			err = iTransport->EstablishTransportToPeer(bindId, aMsg.ChannelInput(), aMsg.ChannelOutput());
			// Main thread introduces itself; workers wait passively for this
			if(err == KErrNone)
				{
				iTransport->SetLastRequestIdConcerningPeer(bindId, aMsg.Identifier());
				if(IsMainThread())
					{
					C32LOG1(KC32Bootup, _L("Sending Introduction Message to Worker"));
					DealerByRef().SendIntroductionToWorker(bindId);
					}
				}
			}
		}
	/* Main dealer only completes when it has received the introduction response messages from peers,
	dumb players respond immediately. */
	if(err!=KErrNone || !IsMainThread())
		{
		TCFBindCompleteMsg respMsg(aMsg.Identifier(), err);
		VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
		}
	}

void CC32WorkerThread::SetDealerShutdownComplete(TBool aComplete)
	{
	// Backsliding not permitted; cannot step back to !complete
	if(aComplete)
		{
		iDealerShutdownComplete = aComplete;
		MaybeCompleteUnbindings();
		}
	}

void CC32WorkerThread::SetPlayerShutdownComplete(TBool aComplete)
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
@see CC32WorkerThread::MaybeCompleteUnbinding
*/
void CC32WorkerThread::CFUnbindMessageReceived(const TCFUnbindMsg& aMsg)
	{
	// Mark the peer handler as unbinding
    __CFLOG_SMADDR2(( KLogSubSysSerComms, KLogSubSysSerComms, _L8("W%d: CFUnbindMessageReceived %S <=> %S"),
		WorkerId(),
		&aMsg.SubModule1()->Printable(__FullModName1), 
		&aMsg.SubModule2()->Printable(__FullModName2) ));

	TWorkerId unbindId;
	TInt err = DecodePeerId(aMsg.SubModule1(), aMsg.SubModule2(), unbindId);
	if(err == KErrNone && !iTransport->PeerReachable(unbindId))
		{
		__ASSERT_DEBUG(0, User::Panic(KCFTransportPanic, ECFTransAbsentWorker)); // shouldn't happen, but could possibly by race with thread panicked in shutdown?
		err = KErrNotFound;	
		}
	if(err == KErrNone)
		{
		iTransport->SetLastRequestIdConcerningPeer(unbindId, aMsg.Identifier());
		iTransport->SetDropTransportPending(unbindId, ETrue);
		MaybeCompleteUnbinding(unbindId);
		}
	else
		{
		TCFUnbindCompleteMsg respMsg(aMsg.Identifier(), err);
		VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
		}
	}

/** 
Can be called at any time, will poll all peer handlers for unbind "readiness" and make sure
they complete the unbind if possible. 
@see CC32WorkerThread::MaybeCompleteUnbinding
*/
void CC32WorkerThread::MaybeCompleteUnbindings()
	{
	if(iProlongBindingLife == 0)
		{
		for(TWorkerId player = TC32WorkerThreadPublicInfo::EMainThread; player <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId; ++player)
			{
			MaybeCompleteUnbinding(player);
			}
		}
	}

/** 
If the peer handler have been marked for unbind check whether local Dealer/Player is ready to 
unbind from this particular worker. If so, deletes handler and send unbind response to Root Server. 
@see CC32Dealer::CanUnbindFromWorker
@see CC32Player::CanUnbindFromWorker
@see TCFUnbindCompleteMsg
*/
void CC32WorkerThread::MaybeCompleteUnbinding(TWorkerId aWorker)
	{
	if(iTransport->PeerReachable(aWorker) && iTransport->IsDropTransportPending(aWorker))
		{
		if(!Dealer() || Dealer()->CanUnbindFromWorker(aWorker))
			{
			if(!Player() || Player()->CanUnbindFromWorker(aWorker))
				{
				C32LOG2(KC32Bootup, _L("CC32WorkerThread::MaybeCompleteUnbinding(%d) - dropping transport & unbinding"), aWorker);
				TCFUnbindCompleteMsg respMsg(iTransport->LastRequestIdConcerningPeer(aWorker), KErrNone);
				iTransport->DropTransportToPeer(aWorker);
				VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
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
@see CC32Dealer::ProcessShutdownRequest
@see CC32Player::ProcessShutdownRequest
@see CC32WorkerThread::SetShuttingDown
@see CC32WorkerThread::MaybeTriggerThreadShutdownCallback
*/
void CC32WorkerThread::CFShutdownMessageReceived(const CommsFW::TCFShutdownMsg& aMsg)
	{
	const CommsFW::TCFShutdownType type = aMsg.Type();
	C32LOG2(KC32Shutdown, _L("CFShutdownMessageReceived(%d)"), type);

	if(EOptional == type)
		{
		C32LOG1(KC32Shutdown, _L("Type is EOptional: ignoring"));
		return;
		}
	else if(EImmediate == type)
		{
		C32LOG1(KC32Shutdown, _L("Type is EImmediate"));
		DealerByRef().SetImmediateShutdownPresent();
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

void CC32WorkerThread::PostMessage(TWorkerId aWorkerId, TCFMessage& aMessage)
	{
	TCFLegacyMessagePacker::PackForPostage(aWorkerId, aMessage);
	iTransport->PostMessage(aMessage);
	}

/** 
Deal with incoming messages from other workers.
*/
void CC32WorkerThread::DispatchL(const TCFMessage& aMessage, TWorkerId aSenderId)
	{
	switch(aMessage.Code())
		{
	case TC32WorkerMsg::EMainIntroduction:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). EMainIntroduction"), aSenderId);
		ASSERT(!IsMainThread());
		const TC32WorkerMainIntroductionMsg& msg = reinterpret_cast<const TC32WorkerMainIntroductionMsg&>(aMessage);
		iDealer = msg.Dealer();
		iWorkerRegister->SetGlobalThreadRegister(&iDealer->WorkerDataGlobals());
		TC32WorkerThreadPublicInfo info;
		info.Init(WorkerId(), aSenderId == TC32WorkerThreadPublicInfo::EMainThread);
		info.iWorker = this;
		TC32WorkerMainIntroductionRespMsg respMsg(info);
#ifdef _DEBUG
  		respMsg.SetFailType(iFailType);
  		respMsg.SetFailRate(iFailRate);
#endif
		PostMessage(aSenderId, respMsg);
		break;
		}
	case TC32WorkerMsg::EMainIntroductionResp:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). EMainIntroductionResp"), aSenderId);
		ASSERT(IsMainThread());
		const TC32WorkerMainIntroductionRespMsg& msg = reinterpret_cast<const TC32WorkerMainIntroductionRespMsg&>(aMessage);
		DealerByRef().ProcessWorkerIntroductionResponse(msg);

		/* In contrast with the players, who send their bind response immediately to the RootServer, 
		the Main dealer has deferred sending the bind response until it has initialised the protocols 
		signalled in the introduction response message. This ultimately ensures that the configurator 
		only increments the sequence level when all protocols are actually initialised. */
		TCFBindCompleteMsg bindresp(iTransport->LastRequestIdConcerningPeer(aSenderId), KErrNone);
		VERIFY_RESULT(iChannelHandler->Send(bindresp), KErrNone);
		iTransport->SetLastRequestIdConcerningPeer(aSenderId, TCFCommsMessageId::KNullMessageId);
		// delete cpmLoader
		DealerByRef().DeleteCPMLoader(aSenderId);
		break;
		}

	case TC32PlayerMsg::EForwardRequest:
		{
		ASSERT(Player());
		TC32PlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TC32PlayerForwardRequestMsg&>(aMessage));
		C32LOG2(KC32CommsTrspMsg, _L8("DispatchL(%d). EForwardRequest "), aSenderId);
		Player()->ProcessMessageL(fwdReqMsg.Message(), fwdReqMsg.SubSession());
		break;
		}
	case TC32PlayerMsg::ESessionClose:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). ESessionClose"), aSenderId);
		CC32Player* player = Player();
		ASSERT(player);
		const TC32PlayerSessionCloseMsg& sessionCloseMsg = reinterpret_cast<const TC32PlayerSessionCloseMsg&>(aMessage);
		/* Only do something if the message is within the deadline and we're sure 
		   the session pointer is safe to use */
		TTime time;
		time.HomeTime();
		if(time.Int64()<=sessionCloseMsg.SessionCloseDeadline())
			{
			IncProlongBindingLife();
			player->CloseSession(sessionCloseMsg.Session());
			TC32PlayerSessionCloseRespMsg sessionCloseRespMsg(WorkerId(), sessionCloseMsg);
			PostMessage(aSenderId, sessionCloseRespMsg);
			DecProlongBindingLife();
			if(ShuttingDown())
				{
				player->MaybeSetPlayerShutdownComplete(EFalse);
				MaybeTriggerThreadShutdownCallback();
				}
			}
		break;
		}
	case TC32PlayerMsg::ESessionCloseResp:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). ESessionCloseResp"), aSenderId);
		ASSERT(IsMainThread());
		const TC32PlayerSessionCloseRespMsg& msg = reinterpret_cast<const TC32PlayerSessionCloseRespMsg&>(aMessage);
		ASSERT(msg.Session());
	
		/* Only do something if the message is within the deadline and we're sure 
		   the session pointer is safe to use */
		TTime time;
		time.HomeTime();
		if(time.Int64()<=msg.SessionCloseDeadline())
			{
			CCommSession* session = msg.Session();
			session->SessionCloseResp(msg.WorkerId());
			if(session->IsDisconnectListEmpty())
				{
				// Deleting the session may allow the binding to be removed, which will
				// cause the channel handler to be deleted
				Dealer()->DeleteSession(session);
				}
			}
		break;
		}
	case TC32PlayerMsg::ELoadCommModuleMsg:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). ELoadCommModuleMsg"), aSenderId);
		ASSERT(Player());
		TC32PlayerLoadCommModuleMsg msg(reinterpret_cast<const TC32PlayerLoadCommModuleMsg&>(aMessage));
		// make function call to player here
		Player()->LoadCommModule(msg.Message());
		break;
		}
	case TC32PlayerMsg::ELoadCommModuleSuccessResp:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). ELoadCommModuleResp"), aSenderId);
		ASSERT(IsMainThread());
		TC32PlayerLoadCommModuleSuccessResp msg(reinterpret_cast<const TC32PlayerLoadCommModuleSuccessResp&>(aMessage));
		// update CC32ThreadManager with CSerial*, portprefix and csyfilename for the csy loaded successfully
		// portprefix is retrieved using CSerial* (by switching heap if necessary) and csyfilename is obtained 
		// from message, after dealer after updating ThreadManager complete message with KErrNone, to emilinate 
		// possible race condition of player completing message and subsequent port open rerquest reaching 
		// ThreadManager before dealer gets a chance to update ThreadManager
		Dealer()->ProcessLoadCommModuleSuccessResponse(msg.Message(), msg.SerialPtr());
		break;
		}
	case TC32PlayerMsg::ELoadCommModuleFailureResp:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). ELoadCommModuleFailureResp"), aSenderId);
		ASSERT(IsMainThread());
		const TC32PlayerLoadCommModuleFailureResp msg(reinterpret_cast<const TC32PlayerLoadCommModuleFailureResp&>(aMessage));
		// remove CSY from session CSY container and de-allocate memory for this csy in ThreadManager as load failed
		Dealer()->ProcessLoadCommModuleFailureResponse(msg.Message(), msg.FailureReason());
		break;
		}
	case TC32PlayerMsg::EUnLoadCommModuleMsg:
		{
		C32LOG2(KC32Detail, _L("DispatchL(%d). EUnLoadCommModuleMsg"), aSenderId);
		ASSERT(Player());
		TC32PlayerUnLoadCommModuleMsg msg(reinterpret_cast<const TC32PlayerUnLoadCommModuleMsg&>(aMessage));
		Player()->ProcessUnLoadCommModuleMsg(msg.SerialPtr());
		break;
		}
	case TC32PlayerMsg::EGetPortInfoMsg:
		{
		C32LOG2(KC32CommsTrspMsg, _L("DispatchL(%d). EGetPortInfoMsg"), aSenderId);
		ASSERT(Player());
		TC32PlayerGetPortInfoMsg msg(reinterpret_cast<const TC32PlayerGetPortInfoMsg&>(aMessage));
		// make function call to player here
		Player()->PortInfo(msg.Message(), msg.SerialPtr());
		break;
		}
	default:
		{
		C32LOG1(KC32Warning, _L("Unknown Transport message received. Do Nothing"));
		break;
		}

		}
	}

/**
The ProcessMessageL function which takes care of incoming messages doesnt TRAP anything,
instead we count on dealing with failures here in the RunError. This simplifies the
ProcessMessageL function and avoids the cost of another TRAP
@see CC32Player::ProcessMessageL
*/
void CC32WorkerThread::OnDispatchLeave(const TCFMessage& aMessage, TWorkerId /*aSenderId*/, TInt aError)
	{
	C32LOG3(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave- Dispatch Left!! for message %x with error %d"),&aMessage,aError);
	switch(aMessage.Code())
		{
		case TC32PlayerMsg::EForwardRequest:
			{
			TC32PlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TC32PlayerForwardRequestMsg&>(aMessage));
			if(aError != KErrNone && aError != KErrBadDescriptor) 
				{   
				fwdReqMsg.Message().Complete(aError); 
		        C32LOG1(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave, EForwardRequest - completing message."));
				}
			break;
			}
		case TC32PlayerMsg::ELoadCommModuleMsg:
			{
			TC32PlayerLoadCommModuleMsg loadMsg(reinterpret_cast<const TC32PlayerLoadCommModuleMsg&>(aMessage));
			if(aError != KErrNone && aError != KErrBadDescriptor) 
				{   
				loadMsg.Message().Complete(aError); 
		        C32LOG1(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave, ELoadCommModuleMsg - completing message."));
				}
			break;
			}
		case TC32PlayerMsg::ELoadCommModuleSuccessResp:
			{
			TC32PlayerLoadCommModuleSuccessResp loadSuccessMsg(reinterpret_cast<const TC32PlayerLoadCommModuleSuccessResp&>(aMessage));
			if(aError != KErrNone && aError != KErrBadDescriptor) 
				{   
				loadSuccessMsg.Message().Complete(aError); 
		        C32LOG1(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave, ELoadCommModuleSuccessResp - completing message."));
				}
			break;
			}
		case TC32PlayerMsg::ELoadCommModuleFailureResp:
			{
			TC32PlayerLoadCommModuleFailureResp loadFailMsg(reinterpret_cast<const TC32PlayerLoadCommModuleFailureResp&>(aMessage));
			if(aError != KErrNone && aError != KErrBadDescriptor) 
				{   
				loadFailMsg.Message().Complete(aError); 
		        C32LOG1(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave, ELoadCommModuleFailureResp - completing message."));
				}
			break;
			}
		case TC32PlayerMsg::EGetPortInfoMsg:
			{
			TC32PlayerGetPortInfoMsg portInfoMsg(reinterpret_cast<const TC32PlayerGetPortInfoMsg&>(aMessage));
			if(aError != KErrNone && aError != KErrBadDescriptor) 
				{   
				portInfoMsg.Message().Complete(aError); 
		        C32LOG1(KC32Warning,_L8("CC32WorkerThread::OnDispatchLeave, EGetPortInfoMsg - completing message."));
				}
			break;
			}
		}
	}


TBool CC32WorkerThread::ShuttingDown() const
	{
	return iWorkerShuttingDown;
	}

void CC32WorkerThread::SetShuttingDown()
	{
	iWorkerShuttingDown = ETrue;
	}

void CC32WorkerThread::SessionShutdownComplete()
	{
	if(IsMainThread())
		{
		DealerByRef().SessionShutdownComplete();
		}
	else
		{
		TriggerThreadShutdownCallback();
		}
	}

void CC32WorkerThread::TriggerThreadShutdownCallback()
	{
	C32LOG1(KC32Shutdown, _L("CC32WorkerThread::TriggerThreadShutdownCallback()") );
	CC32DataInTls()->iShutdownWatchDog->Shutdown();
	}

void CC32WorkerThread::MaybeTriggerThreadShutdownCallback()
	{
	if(!ShuttingDown())
		{
		return;
		}
	TBool doTrigger = DealerByRef().TestImmediateShutdownPresent();
	C32LOG4(KC32Shutdown, _L("MaybeTriggerThreadShutdownCallback() - immediate=%d, dealerComplete=%d, playerComplete=%d"), 
		doTrigger, DealerShutdownComplete(), PlayerShutdownComplete());
	if(!doTrigger && DealerShutdownComplete())
		{
		if(!PlayerShutdownComplete() && Player())
			{
			Player()->MaybeSetPlayerShutdownComplete(EFalse);
			}
		if(PlayerShutdownComplete())
			{
			// Check if all bindings have gone
			doTrigger = ETrue;
			for(TWorkerId player = TC32WorkerThreadPublicInfo::EMainThread; player <= TC32WorkerThreadPublicInfo::EMaxWorkerThreadId; ++player)
				{
				if(player != WorkerId() && iTransport->PeerReachable(player))
					{
					C32LOG2(KC32Bootup, _L("MaybeTriggerThreadShutdownCallback() - peer %d binding remains"), player);
					doTrigger = EFalse;
					break;
					}
				}
			}
		}
	if(doTrigger)
		{
		if(WorkerId() == TC32WorkerThreadPublicInfo::EMainThread)
			{
			DealerByRef().ShutdownIfReady();
			}
		else
			{
			TriggerThreadShutdownCallback();
			}
		}
	}



LOCAL_C void LoadDeviceDrivers(void)
/**
 * On the target, load all the EUART LDDs and PDDs that can be found
 */
	{
#if defined (__EPOC32__)
	//Load Pdd's
	_LIT(CommPddName, "EUART");
	_LIT(CommPddName0, "EUART0");
	_LIT(CommPddName1, "EUART1");
	_LIT(CommPddName2, "EUART2");
	_LIT(CommPddName3, "EUART3");
	_LIT(CommPddName4, "EUART4");
	_LIT(CommPddName5, "EUART5");
	_LIT(CommPddName6, "EUART6");
	_LIT(CommPddName7, "EUART7");
	_LIT(CommPddName8, "EUART8");
	_LIT(CommPddName9, "EUART9");
	User::LoadPhysicalDevice(CommPddName);
	User::LoadPhysicalDevice(CommPddName0);
	User::LoadPhysicalDevice(CommPddName1);
	User::LoadPhysicalDevice(CommPddName2);
	User::LoadPhysicalDevice(CommPddName3);
	User::LoadPhysicalDevice(CommPddName4);
	User::LoadPhysicalDevice(CommPddName5);
	User::LoadPhysicalDevice(CommPddName6);
	User::LoadPhysicalDevice(CommPddName7);
	User::LoadPhysicalDevice(CommPddName8);
	User::LoadPhysicalDevice(CommPddName9);
	
	// Load logical device driver
	_LIT(CommLddName, "ECOMM.LDD");
	User::LoadLogicalDevice(CommLddName);
#endif // #ifdef __EPOC32__
	}


void CC32WorkerThread::DeleteHBufC8(TAny* aHBufC)
	{
	delete static_cast<HBufC8*>(aHBufC);
	}


/**
The C32 thread. This is where control will resume when the RootServer starts a C32 instance.
This function creates the worker thread object and starts the active scheduler.
*/
EXPORT_C TInt CC32WorkerThread::ThreadEntryPoint(TAny* aArg)
	{
	return RunC32Thread(static_cast<TCFModuleInfo*>(aArg));
	}

EXPORT_C TInt CC32WorkerThread::RunC32Thread(TCFModuleInfo* aArg)
/**
 * The comm server thread.
 */
	{	
	CTrapCleanup* tc=CTrapCleanup::New();
	if (!tc)
		{
		return KErrNoMemory;
		}
		
	LoadDeviceDrivers();

	CCommScheduler* cs = CCommScheduler::New();

	TCFModuleInfo* moduleInfo = aArg;
	
	TRAPD(res, CC32WorkerThread::NewL(moduleInfo)); // takes ownership of aArg and eventually deletes
	
	if(res != KErrNone)
		{
		// memory added to the TLS won't have been deleted since it isn't referred to be the cleanupstack
		// due to it being deemed to be "owned" by the thread via the TLS. So we delete it here
		CTLSRedirector* tls = CRedirectorInTls();
		if (tls != NULL)
			{
			// The CC32Data destructor does not own any data that it points to, so does not delete the data,
			// So we have to delete watchdog directly
			
			CC32Data* globals = CC32DataInTls();
			if (globals != NULL)
				{
				delete globals->iShutdownWatchDog;
				}
			delete tls;
			Dll::SetTls(NULL); // so that logger sees it is gone
			}

		delete cs;
		delete tc;
		
		return res;
		}

	RThread::Rendezvous(KErrNone);
		
	CActiveScheduler::Start();
	
	delete cs;
	delete tc;	
		
	return KErrNone;
	}


//
// CCommChannelHandler class definitions
//
CCommChannelHandler::CCommChannelHandler(CC32WorkerThread* aWorkerThread)
: CCFModuleChannelHandler(CActive::EPriorityStandard),
  iWorkerThread(aWorkerThread)
	{
	}

CCommChannelHandler* CCommChannelHandler::NewL(RCFChannel::TMsgQueues aRxQueues,
																   RCFChannel::TMsgQueues aTxQueues, 
																   CC32WorkerThread* aWorkerThread)
	{
	CCommChannelHandler* pHandler = new (ELeave) CCommChannelHandler(aWorkerThread);
	CleanupStack::PushL(pHandler);
	pHandler->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pHandler);
	return pHandler;
	}

TInt CCommChannelHandler::Send(const CommsFW::TCFMessage &aMessage)
	{
	return inherited::Send(aMessage);
	}


/**
This will be called by the handler when a shutdown message have been received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CCommChannelHandler::CFMessageShutdown(const TCFShutdownMsg& aMessage)
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
void CCommChannelHandler::CFMessageDiscover(const TCFDiscoverMsg& aMessage)
	{
	ASSERT(aMessage.Size() > 0);
	TBuf8<4> subname;
	_LIT8(subconst, "%d");
	subname.Format(subconst, iWorkerThread->WorkerId());
	aMessage.SubModuleNames()->Copy(subname);
	TCFDiscoverRespMsg respMsg(aMessage.Identifier(), 1, EFalse);
	VERIFY_RESULT(Send(respMsg), KErrNone);
	}

/** 
This will be called by the handler when a Bind message have been received from 
the Root Server. Delegating the call to the Worker Thread.
*/
void CCommChannelHandler::CFMessageBind(const TCFBindMsg& aMessage)
	{
	iWorkerThread->CFBindMessageReceived(aMessage);
	}

/**
This will be called by the handler when a Unbind message have has received from
the Root Server. Delegating the call to the Worker Thread.
*/
void CCommChannelHandler::CFMessageUnbind(const TCFUnbindMsg& aMessage)
	{
	iWorkerThread->CFUnbindMessageReceived(aMessage);
	}


