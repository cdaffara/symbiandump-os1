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

#include <ss_std.h>
#include <comms-infras/sockmes.h>
#include <ss_glob.h>
#include "ss_msgs.h"
#include <nifman.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_roles.h>
#include "ss_subconn.h"
#include <ss_sock.h>
#include "SS_rslv.H"
#include "SS_conn.H"
#include "ss_connectionsession.h"
#include <comms-infras/ss_api_ext.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#endif

#include "ss_apiext_messages.h"
#include <comms-infras/ss_tiermanagerutils.h>

#include <comms-infras/ss_nodemessages_internal_esock.h>

#include <comms-infras/esockdebugmessages.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#include <comms-infras/commsbufponddbg.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING
#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_SES, "ESockSSockS_SES.");
#endif

#define MSG_PRM(prmIndex)		(prmIndex)

using namespace NetInterfaces;
using namespace ESock;
//using namespace Elements;
using namespace Messages;
using namespace Den;
using namespace CommsFW;

class RNullableMessage : public RMessage2
	{
public:
	void NullHandle() const
		{
		// Although this casting away of const looks awful it's in keeping with the ability to Complete() const
		// messages - perhaps iHandle should have been mutable?
		const_cast<RNullableMessage&>(*this).iHandle = 0;
		}
	};

/**
Constructor
*/
CSockSession::CSockSession(TUidType aUid, TSessionUniqueId aSessionUniqueId)
:	CWorkerSession(aUid, aSessionUniqueId)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSession(%08x):\tConstructor"), this) );
	SocketServer::NewSession();
	}

/*static*/ CSockSession* CSockSession::NewL(TProcessId aProcessId, TUidType aUid, TSessionUniqueId aSessionUniqueId)
	{

	CSockSession* self = new (ELeave) CSockSession(aUid, aSessionUniqueId);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessId);
	CleanupStack::Pop(self);
	return self;
	}

void CSockSession::ConstructL(TProcessId aProcessId)
	{
#ifdef SYMBIAN_TRACE_ENABLE
	// Document the owner of the session
	TLogTextBuf buf;
	buf.Format(_L8("CSockSession %08x:\tConstructL()"), this);
	RProcess cliProc;
	if (cliProc.Open(aProcessId) == KErrNone)
		{
		TName cliName = cliProc.Name();
		TInt index = cliName.LocateReverse('[');
		if (index >= 1)
			{
			cliName.SetLength(index);
			}
		TUint processId = cliProc.Id();
		iProcessName.Copy(cliName);
		buf.AppendFormatIgnoreOverflow(_L8(" pid=%x \"%S\""), processId, &iProcessName);
		// Remove the "...[nnn]nnn" clutter from the name stored in the session
		// (unless the name happens to begin with "[").
		cliProc.Close();
		}
	LOG(ESockLog::Printf(KESockServerTag, buf) );
#endif
	CWorkerSession::ConstructL(aProcessId);
	}

/**
Destroy
*/
CSockSession::~CSockSession()
	{
	LOG( ESockLog::Printf(KESockServerTag, _L8("~CSockSession(%08x):\t\"%S\""), this, &iProcessName) );

	if(iOptimalDealer)
		{
		iOptimalDealer->RemoveEligiblePid(iProcess.Id());
		}

	SocketServer::SessionClosing();
	}

CSockManData* CSockSession::SockManGlobals() const
    {
    return WorkerThread().SockManGlobals();
    }

TInt CSockSession::CheckPolicy(const TSecurityPolicy& aPolicy, const char *aDiagnostic)
/**
Check the security policy of a process.
Called from a socket or resolver provider to check whether the process conforms to the security policy passed as
argument.
@see MProvdSecurityChecker
*/
	{
	return aPolicy.CheckPolicy(iProcess, aDiagnostic) ? KErrNone : KErrPermissionDenied;
	}


/**
 Handle messages for this server. In a few cases the messages will be dealt with accordingly,
 but in most cases they will be 1) forwarded to seperate player or 2) be handled by ProcessMessageL
 if the Player is co-resident in this thread.
 @param aMessage Standard IPC request from client side
*/
void CSockSession::ServiceL(const RMessage2& aMessage)
	{
	LOG(
		TBuf8<64> messName;
		ESockLog::IPCMessName((TSockMess) aMessage.Function(), messName);
		ESockLog::Printf(KESockServerTag, _L8("CSockSession(%08x):\tServiceL, Message(%08x) [%S] \"%S\" int3=%08x"),
				this, aMessage.Handle(), &messName, &iProcessName, SubSessionFromHandle(aMessage.Int3(), TCFSubSessInfo(TCFSubSessInfo::EAny)));
		);

	const CWorkerThread& owner=WorkerThread();
	const CPitBoss& pitBoss = owner.PitBoss();

	if(pitBoss.TestImmediateShutdownPresent())
		{
		User::Leave(KErrServerTerminated);
		}

	iComplete=ETrue; 		// Whether to complete RMessage2 immediately
	iReturn=KErrNone;		// Default return value if completing RMessage2 here
    iOurMessage = &aMessage;

	CWorkerSubSession* ss = NULL;

	TWorkerId worker=TWorkerThreadPublicInfo::ENullWorkerId;
	const TInt function = aMessage.Function();

#if defined(_DEBUG_SOCKET_FUNCTIONS)
	if(function != ESSDbgControl && iDebugParking.iNumOutstanding > 0)
		{
		// Weakness here; if it's a session request then the 4th arg may be random stack junk. Major mitigation is that almost certainly they're in UDEB which gives near-certainty
		// of it being the stack fill pattern (eg 0xCCCCCCCC) so a collision is very unlikely to be seen (and only in the few cases where debug parking is employed). To improve this
		// need to test whether it's a sub-session operation; reorganising the IPC list will help this
		if(iDebugParking.iSubSessHandle == 0 || iDebugParking.iSubSessHandle == aMessage.Int3())
			{
			User::LeaveIfError(Dealer()->ParkRequest(this, aMessage, CCommonDealer::EDebugParking));
			--iDebugParking.iNumOutstanding;
			DontCompleteCurrentRequest();
			return;
			}
		}
#endif
	switch(function)
        {
    /* Cases handled without a player */
    case ESSRequestOptimalDealer:
     	RequestOptimalDealerL();
    	break;

	case ESSInstallExtension:
		LOG(ESockLog::Printf(_L8("WARNING: unsupported function ESSInstallExtension called, CSockSession(%08x)"), this));
		SetReturn(KErrNotSupported);
		break;

    case ESSExclusiveMode:
		LOG(ESockLog::Printf(_L8("WARNING: unsupported function ESSExclusiveMode called, CSockSession(%08x)"), this));
		SetReturn(KErrNone);
        break;

    case ESSClearExclusiveMode:
		LOG(ESockLog::Printf(_L8("WARNING: unsupported function ESSClearExclusiveMode called, CSockSession(%08x)"), this));
		SetReturn(KErrNone);
        break;

 	case ESSNumProtocols:
		NumProtocolsL();
		break;

	case ESSProtocolInfo:
		ProtocolInfoL();
		break;

	case ESSProtocolInfoByName:
		ProtocolInfoByNameL();
		break;

    /* Cases for a Player to deal with, protocol level */
	case ESSProtocolStart:
	case ESSProtocolStop:
	case ESoCreate:
	case ESRCreate:
		if(pitBoss.GetWorkerForProtocol(aMessage.Int0(),aMessage.Int1(),aMessage.Int2(), worker))
			{
			ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), worker);
			}
		else
			{
			ParkIfIndeterminateRequest(aMessage, KErrBadName);
			}
		break;

	case ESoCreateWithConnection:
		{
		TPckgBuf<TSockOpen> argPkg;
		SafeMessage().ReadL(MSG_PRM(0),argPkg);
		ss = CConnectionFromHandle(argPkg().iHandle);
		if(!ss)
		    {
		    PanicClient(ESockBadHandle);
		    }
		else
			{
			// Forward the message to the connection's (control) thread. It's best placed to decide which of its data threads it really belongs in
			ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), ss->Player().WorkerId());
			}
		}
		break;
	case ESoCreateWithSubConnection:
		{
	    TPckgBuf<TSockOpen> argPkg;
	    SafeMessage().ReadL(MSG_PRM(0),argPkg);
	    ss = CSubConnectionFromHandle(argPkg().iHandle);
	    if (!ss)
		    {
		    PanicClient(ESockBadHandle);
		    }
		else
			{
			// Forward the message to the subconnection's (control) thread. It's best placed to decide which of its data threads it really belongs in
			ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), ss->Player().WorkerId());
			}
		break;
		}
	case EHRCreateWithConnection:
	case EHRCreate:
	case ENDCreate:
		if(pitBoss.GetWorkerForProtocol(aMessage.Int0(), KUndefinedSockType, aMessage.Int1(), worker))
			{
			ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), worker);
			}
		else
			{
			ParkIfIndeterminateRequest(aMessage, KErrBadName);
			}
		break;

	case ESoCreateNull:
		// Attempt to pass to co-resident player
		if(owner.Player())
			{
			worker = WorkerId();
			}
		else
			{ // No co-resident player, pass to NullSocket handling player known to PitBoss
			VERIFY(pitBoss.GetWorkerForNullSocket(worker));
			}
		ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), worker);
		break;

	case ECNCreate:
        ForwardTierRequestL(aMessage);
		break;

	// Connection Messages
	case ECNCreateWithName:
		{
		// The cloned connection must be created on the same [control] Player that owns the original
		TName name;
		aMessage.ReadL(0, name);
		User::LeaveIfError(CSockSubSession::FetchSubSessionFromName(name, TCFSubSessInfo(TCFSubSessInfo::EConnection), owner, ss));
		ForwardMessageL(aMessage, *ss);	// can't be indeterminate request; already created the clone source
		break;
		}

    /* All else (cases for a Player to deal with, subsession level) */

// socket messages

	case ESoClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::ESocket));
		break;

	case ESoConnect:
	case ESoShutdown:
	case ESoSendToNoLength:
	case ESoSendNoLength:
	case ESoSendTo:
	case ESoSend:
	case ESoWrite:
	case ESoRecvFromNoLength:
	case ESoRecvNoLength:
	case ESoRecvOneOrMore:
    case ESoRecvOneOrMoreNoLength:
	case ESoRecvFrom:
	case ESoRecv:
	case ESoRead:
	case ESoBind:
	case ESoAccept:
	case ESoListen:
	case ESoSetOpt:
	case ESoGetOpt:
	case ESoIoctl:
	case ESoGetDiscData:
	case ESoGetLocalName:
	case ESoGetRemoteName:
	case ESoReference:
		ss=CSocketFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;
	case ESoCancelRecv:
	case ESoCancelSend:
	case ESoCancelIoctl:
	case ESoCancelConnect:
	case ESoCancelAccept:
	case ESoCancelAll:
		ss=CSocketFromHandle(aMessage.Int3());
		if(ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			LOG(ESockLog::Printf(_L8("CSockSession::ServiceL: BAD ESoCancelAll")));
			}
		break;

	case ESoSocketInfo:
		ss=CSocketFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			SetReturn(KErrBadHandle);
			}
		break;

	case ESoTransfer:
		TransferSocketL();
		break;

// Host resolver message types

	case EHRClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::EHostResolver));
		break;

	case EHRCancel:
		ss=CHostResolverFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			LOG( ESockLog::Printf(_L8("CSockSession::ServiceL: BAD EHRCancel")) );
			}
		break;

    case EHRGetByName:
	case EHRNext:
	case EHRGetByAddress:
	case EHRGetHostName:
	case EHRSetHostName:
	case EHrQuery:
	case EHrQueryNext:
	case EHRSetOpt:
		ss=CHostResolverFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;

// Service resolver message types

	case ESRClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::EServiceResolver));
		break;

	case ESRGetByName:
	case ESRGetByNumber:
	case ESRRegisterService:
	case ESRRemoveService:
		ss=CServiceResolverFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;
	case ESRCancel:
		ss=CServiceResolverFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			LOG( ESockLog::Printf(_L8("CSockSession::ServiceL: BAD ESRCancel")) );
			}
		break;

// Net database message types

	case ENDClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::ENetDatabase));
		break;

	case ENDQuery:
	case ENDAdd:
	case ENDRemove:
		ss=CNetDatabaseFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;
	case ENDCancel:
		ss=CNetDatabaseFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			LOG( ESockLog::Printf(_L8("CSockSession::ServiceL: BAD ENDCancel")) );
			}
		break;
    case ECNClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::EConnection));
		break;
	case ECNStart:
	case ECNSetStartPrefs:
	case ECNStop:
	case ECNReference:
	case ECNProgress:
	case ECNProgressNotification:
	case ECNCancelProgressNotification:
	case ECNLastProgressError:
	case ECNServiceChangeNotification:
	case ECNCancelServiceChangeNotification:
	case ECNGetIntSetting:
	case ECNGetBoolSetting:
	case ECNGetDes8Setting:
	case ECNGetDes16Setting:
	case ECNGetLongDesSetting:
	case ECNEnumerateConnections:
	case ECNGetConnectionInfo:
	case ECNIoctl:
	case ECNCancelIoctl:
	case ECNControl:
	case ECNAttach:
	case ECNAllInterfaceNotification:
	case ECNCancelAllInterfaceNotification:
	case ECNCancelAllSubConnectionNotification:
	case ECNEnumerateSubConnections:
	case ECNAllSubConnectionNotification:
	case ECNWaitForIncoming:
	case ECNCancelWaitForIncoming:

	case ESCPSStop:
	case ESCPSProgressNotification:
	case ESCPSCancelProgressNotification:
	case ESCPSDataTransferred:
	case ESCPSDataTransferredCancel:
	case ESCPSDataSentNotificationRequest:
	case ESCPSDataSentNotificationCancel:
	case ESCPSDataReceivedNotificationRequest:
	case ESCPSDataReceivedNotificationCancel:
	case ESCPSIsSubConnectionActiveRequest:
	case ESCPSIsSubConnectionActiveCancel:

	case ESCPSGetSubConnectionInfo:
		ss=CConnectionFromHandle(aMessage.Int3());
		if (ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;

	case ESCCreate:
	    {
    	TPckgBuf<TSubConnOpen> argPkg;
	    SafeMessage().ReadL(MSG_PRM(0),argPkg);
	    ss=CConnectionFromHandle(argPkg().iHandle);
	    if (ss)
		    {
			ForwardMessageL(aMessage, *ss);
		    }
		else
		    {
		    PanicClient(ESockBadHandle);
			}
		break;
		}
	case ESCClose:
		CloseSubSessionL(aMessage, TCFSubSessInfo(TCFSubSessInfo::ESubConnection));
		break;

	case ESCAddSocket:
	case ESCRemoveSocket:
	case ESCSetParameters:
	case ESCGetParameters:
	case ESCGetParametersLength:
	case ESCEventNotificationSetup:
	case ESCEventNotification:
	case ESCEventAllNotifications:
	case ESCEventNotificationCancel:
	case ESCControl:
    case ESCStart:
    case ESCStop:
		ss=CSubConnectionFromHandle(aMessage.Int3());
		if (ss)
		    {
			ForwardMessageL(aMessage, *ss);
			}
		else
		    {
			PanicClient(ESockBadHandle);
		    }
		break;

// Server debug messages
#if defined (_DEBUG_SOCKET_FUNCTIONS)
	case ESSDbgMarkHeap:
	case ESSDbgCheckHeap:
	case ESSDbgMarkEnd:
	case ESSDbgFailNext:
	case ESSDbgFailNextMbuf:
	case ESSDbgSetMbufPoolLimit:
	case ESSDbgCheckMbuf:
	case ESSDbgMbufFreeSpace:
	case ESSDbgMbufTotalSpace:
	case ESSDbgCheckFailNext:
	    {
		if(!pitBoss.ModuleConfigurationComplete())
			{
			if (Dealer()->ParkRequest(this, aMessage, CCommonDealer::EIndeterminateDuringBoot) != KErrNone)
				{
				//Something terrible has just happened.
				//We could not park the request.
				//Since this is a debug call it is probably a good idea to panic here,
				//hopefully providing a chance to see what is going wrong.
				__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockS_SES, 1));
				}
			else
				{
				DontCompleteCurrentRequest();
				}
			}
		else
			{
			SSDbgFunctionL(aMessage);
			}
		break;
        }
#endif		// _DEBUG_SOCKET_FUNCTIONS

	default:
		ss = SubSessionFromHandle(aMessage.Int3(), TCFSubSessInfo(TCFSubSessInfo::EAny));

		if(ss)
			{
			ForwardMessageL(aMessage, *ss);
			}
		else
			{
			PanicClient(ESockBadHandle);
			}
		break;
	}

	// Message handlers can change the state of the iMessage if they want to hold onto the message.
	// They can also write a return value to iReturn.
	if (iComplete)
		{
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSession(%08x):\tServiceL, Complete message(%08x) with %d."), this, aMessage.Handle(), iReturn) );
		aMessage.Complete(iReturn);
		}
	}

#if defined (_DEBUG_SOCKET_FUNCTIONS)
void CSockSession::DispatchDebugMessageL(const RMessage2& aMessage)
	{
	Elements::RResponseMsg responseMsg(aMessage, aMessage.Int0(), 1, 2);

	// Construct our message in place
	TBuf8<ESockDebug::KMaxIpcMsgLength> msgDst;
	ESockDebug::TControlMsg* msg = static_cast<ESockDebug::TControlMsg*>(responseMsg.ReadClientReqMsg(msgDst));
	__ASSERT_ALWAYS(msg, Fault(EDebugSupport));

	// Self dispatch our message and return any error code
	TPckgBuf<TInt> returnValue;
	TInt& debugStatus = returnValue();
	debugStatus = msg->DispatchL(this);
	SafeMessage(aMessage).WriteL(MSG_PRM(0), returnValue);
	}


#endif // _DEBUG_SOCKET_FUNCTIONS

#if defined (_DEBUG_SOCKET_FUNCTIONS)
void CSockSession::SSDbgFunctionL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case ESSDbgMarkHeap:
			__UHEAP_MARK;
			break;
		case ESSDbgCheckHeap:
			__UHEAP_CHECK(aMessage.Int0());
			break;
		case ESSDbgMarkEnd:
			__UHEAP_MARKENDC(aMessage.Int0());
			break;
		case ESSDbgFailNext:
        	{
			const CWorkerThread& owner=WorkerThread();
			CPitBoss& pitBoss = owner.PitBoss();

			// We set the fail point for all heaps, rather than just the current Dealer. This could lead to a failure not related
			// directly to whatever the client test code is trying to exercise but it all helps find bugs
			pitBoss.SetFailNextForAllHeaps(aMessage.Int0());
			break;
    	    }
		case ESSDbgCheckFailNext:
			{
			// Report whether any heap has ended "FailNext" mode set by ESSDbgFailNext, which generally indicates that some code
			// under test hasn't explored all allocation points
			const CWorkerThread& owner=WorkerThread();
			CPitBoss& pitBoss = owner.PitBoss();
			SetReturn(pitBoss.TestFailNextForAllHeaps());
			break;
			}
	// MBuf specific allocation fails and checks.
		case ESSDbgFailNextMbuf:
			{
#ifdef SYMBIAN_ZERO_COPY_NETWORKING			
			RCommsBufPond pond=TCommsBufPondTLSOp::Get();
			if (pond.IsNull())
				{
				PanicClient(EMbufManagerNotLoaded);
				}
			else
				{
				TCommsBufPondDbg pondDbg(pond); 
				pondDbg.__DbgSetFailAfter(aMessage.Int0());
				}
#else
          	CMBufManager* mbufMan=CMBufManager::Context();
            if (mbufMan==NULL)
				{
			    PanicClient(EMbufManagerNotLoaded);
		        }
		        else
	            {
                mbufMan->__DbgSetFailAfter(aMessage.Int0());
                }			
#endif // SYMBIAN_ZERO_COPY_NETWORKING				
			break;
			}
		case ESSDbgSetMbufPoolLimit:
			{
#ifdef SYMBIAN_ZERO_COPY_NETWORKING				
			RCommsBufPond pond=TCommsBufPondTLSOp::Get();
			if (pond.IsNull())
				{
				PanicClient(EMbufManagerNotLoaded);
				}
			else
				{
				TCommsBufPondDbg pondDbg(pond); 
				pondDbg.__DbgSetPoolLimit(aMessage.Int0());
				}
#else
          	CMBufManager* mbufMan=CMBufManager::Context();
            if (mbufMan==NULL)
				{
			    PanicClient(EMbufManagerNotLoaded);
		        }
		    else
		    	{
		    	mbufMan->__DbgSetPoolLimit(aMessage.Int0());	
		    	}
		
#endif // SYMBIAN_ZERO_COPY_NETWORKING					
			break;
			}
		case ESSDbgCheckMbuf:
			{
#ifdef 	SYMBIAN_ZERO_COPY_NETWORKING		
			RCommsBufPond pond=TCommsBufPondTLSOp::Get();
			if (pond.IsNull())
				{
				PanicClient(EMbufManagerNotLoaded);
				}
			else
				{
				TCommsBufPondDbg pondDbg(pond); 								
				if (pondDbg.__DbgGetBufTotal()-pondDbg.__DbgGetBufSpace()!=aMessage.Int0())
					{
					Fault(EBadMbufCheck);						
					}
				}
#else
          	CMBufManager* mbufMan=CMBufManager::Context();
            if (mbufMan==NULL)
				{
			    PanicClient(EMbufManagerNotLoaded);
		        }
		    else
		    	{
				if (mbufMan->__DbgGetBufTotal()-mbufMan->__DbgGetBufSpace()!=aMessage.Int0())
						Fault(EBadMbufCheck);
		    	}		
#endif				
			break;
			}
		case ESSDbgMbufFreeSpace:
			{
#ifdef 	SYMBIAN_ZERO_COPY_NETWORKING				
			RCommsBufPond pond=TCommsBufPondTLSOp::Get();
			if (pond.IsNull())
				{
				PanicClient(EMbufManagerNotLoaded);
				}
			else
				{
				TPckgBuf<TInt> c;
				TInt& space=c();
				TCommsBufPondDbg pondDbg(pond); 	
				space=pondDbg.__DbgGetBufSpace();
				SafeMessage().WriteL(MSG_PRM(0),c);
				}
#else
          	CMBufManager* mbufMan=CMBufManager::Context();
            if (mbufMan==NULL)
				{
			    PanicClient(EMbufManagerNotLoaded);
		        }
		    else
		    	{
				TPckgBuf<TInt> c;
				TInt& space=c();
				space=mbufMan->__DbgGetBufSpace();
				SafeMessage().WriteL(MSG_PRM(0),c);
		    	}		
#endif				
			break;
			}
		case ESSDbgMbufTotalSpace:
			{
#ifdef 	SYMBIAN_ZERO_COPY_NETWORKING			
			RCommsBufPond pond=TCommsBufPondTLSOp::Get();
			if (pond.IsNull())
				{
				PanicClient(EMbufManagerNotLoaded);
				}
			else
				{
				TPckgBuf<TInt> c;
				TInt& size=c();
				TCommsBufPondDbg pondDbg(pond); 				
				size=pondDbg.__DbgGetBufTotal();
				SafeMessage().WriteL(MSG_PRM(0),c);
				}
#else
          	CMBufManager* mbufMan=CMBufManager::Context();
            if (mbufMan==NULL)
				{
			    PanicClient(EMbufManagerNotLoaded);
		        }
		    else
		    	{
				TPckgBuf<TInt> c;
				TInt& size=c();
				size=mbufMan->__DbgGetBufTotal();
				SafeMessage().WriteL(MSG_PRM(0),c);
		    	}		
#endif //	 SYMBIAN_ZERO_COPY_NETWORKING			
			}
			break;

	// Bad message
		default:
			PanicClient(ESockBadHandle);
		}
	}
#endif
// _DEBUG_SOCKET_FUNCTIONS



/**
Return a CSocket given a client's handle
*/
CSocket* CSockSession::CSocketFromHandle(TUint aHandle)
    {
	return static_cast<CSocket*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::ESocket)));
    }

/**
Return a Flow and SCPR CommsIds given a socket's handle
*/
TBool CSockSession::FlowAndSCPRFromSocketHandle(TUint aHandle, Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR)
    {
    SubSessions().Lock();

    TInt found = EFalse;

	CSocket* sock = static_cast<CSocket*>(iSubSessions.At(aHandle, TCFSubSessInfo(TCFSubSessInfo::ESocket)));
	if (sock)
    	{
    	found = sock->GetFlowAndSCPR(aFlow, aSCPR);
    	}
		
	SubSessions().Unlock();

	return found;
    }

/**
Find a CNameResolver from a clients handle
*/
CHostResolver* CSockSession::CHostResolverFromHandle(TUint aHandle)
    {
	return static_cast<CHostResolver*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::EHostResolver)));
    }

/**
Find a CServiceresolver from a clients handle
*/
CServiceResolver* CSockSession::CServiceResolverFromHandle(TUint aHandle)
    {
	return static_cast<CServiceResolver*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::EServiceResolver)));
    }

/**
Find a CNetDatabase from a handle
*/
CNetDatabase* CSockSession::CNetDatabaseFromHandle(TUint aHandle)
    {
	return static_cast<CNetDatabase*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::ENetDatabase)));
    }

/**
Find a CConnection from a handle
*/
ESock::CConnection* CSockSession::CConnectionFromHandle(TUint aHandle)
    {
	return static_cast<ESock::CConnection*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::EConnection)));
    }

/**
find a CSubConnection from a handle

*/
CSubConnection* CSockSession::CSubConnectionFromHandle(TUint aHandle)
    {
	return static_cast<CSubConnection*>(SubSessionFromHandle(aHandle, TCFSubSessInfo(TCFSubSessInfo::ESubConnection)));
    }

void CSockSession::CloseSubSessionL(const RMessage2& aMessage, TSubSessInfo aType)
	{
	CWorkerSubSession* subSess = NULL;
	
		{
		SubSessions().Lock();

		subSess = iSubSessions.At(aMessage.Int3(), aType);
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSession(%08x):\tCloseSubSession(%08x, %d) - subSess %08x"), this, aMessage.Int3(), aType.iType, subSess));
		if(subSess)
			{
			VERIFY_RESULT(iSubSessions.Remove(aMessage.Int3()), subSess);
			}
		
		SubSessions().Unlock();
		}

	if(subSess)
		{
		ForwardMessageL(aMessage, *subSess);
		}
	else
		{
		SetReturn(KErrBadHandle);	// Close() is always safe
		}
	}


/**
Search for an optimal dealer. If one is found set the iOptimalDealer member, register
the requesting PID on the optimal dealer and return the dealer server name to the client.
*/
void CSockSession::RequestOptimalDealerL()
	{
	/* Wont accept this if we're shutting down or if we've
	already requested optimal dealer (successfully) before */
	if(SocketServer::IsShuttingDown())
		{
		User::Leave(KErrServerTerminated);
		}
	else if(iOptimalDealer)
		{
		PanicClient(EConnectingAlready);
		}
	else // Ok, find optimal dealer and register it with session, tell nice client...
		{
		TSessionPref pref;
		TPckg<TSessionPref> package(pref);
		SafeMessage().ReadL(MSG_PRM(0), package);
		if(PitBoss().FindOptimalDealer(pref, iOptimalDealer))
			{
			// Add PID to table of authorised clients.
			iOptimalDealer->AddEligiblePidL(iProcess.Id());
			SafeMessage().WriteL(MSG_PRM(1), iOptimalDealer->ServerName());
			SetReturn(KErrNone);
			LOG(ESockLog::Printf(KESockSessDetailTag, _L("CSockSession(%08x):\tRequestOptimalDealerL %S for 0x%X"), this, &iOptimalDealer->ServerName(), static_cast<TUint>(iEligiblePid)) );
			}
		else
			{
			LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSession(%08x):\tRequestOptimalDealerL KErrNotFound"), this) );
			iOptimalDealer=NULL;
			// Optimal Dealer requests don't get parked during boot - there's a risk of creating a hard-to-debug startup deadlock
			// which outweighs the performance benefit 
			SetReturn(KErrNotFound);
			}
		}
	}

/**
Determine the owning Player and forward the TransferSocket message to it.
*/
void CSockSession::TransferSocketL()
	{
	TName name;
	SafeMessage().ReadL(MSG_PRM(0), name);
	CWorkerSubSession* socket;
	TInt err = CSocket::FetchSubSessionFromName(name, TCFSubSessInfo(TCFSubSessInfo::ESocket), WorkerThread(), socket);
	if(err == KErrNone)
		{
		ForwardMessageL(*iOurMessage, *socket);
		}
	User::Leave(KErrNotFound);	// name bad or Player exited
	}

/**
Handle a request for the number of loaded protocols.
*/
void CSockSession::NumProtocolsL(void)
	{
	if(!PitBoss().ModuleConfigurationComplete())
		{
		ParkIfIndeterminateRequest(Message(), KErrNone);
		}
	else
		{
		TUint num=PitBoss().GetNumProtocols();
		TPtrC8 d((TUint8 *)&num,sizeof(num));
		if (SafeMessage().Write(MSG_PRM(0),d) != KErrNone)
		    {
		    DontCompleteCurrentRequest();
		    }
		}
	}

/**
Get info for a protocol by index.
*/
void CSockSession::ProtocolInfoL(void)
	{
	TWorkerId worker;
	if(!PitBoss().GetWorkerForProtocol(Message().Int1(), worker))
		{
		ParkIfIndeterminateRequest(Message(), KErrNotFound);
		}
	else
		{
		ForwardMessageL(Message(), TPlayerForwardRequestMsg::UnusedParam(), worker);
		}
	}

/**
Get protocol info by name.
*/
void CSockSession::ProtocolInfoByNameL( )
	{
	TProtocolName name;
	SafeMessage().ReadL(MSG_PRM(1),name);

	TWorkerId worker;
	if(!PitBoss().GetWorkerForProtocolByName(name, worker))
		{
		ParkIfIndeterminateRequest(Message(), KErrNotFound);
		}
	else
		{
		ForwardMessageL(Message(), TPlayerForwardRequestMsg::UnusedParam(), worker);
		}
	}

/**
Panic the client.
*/
void CSockSession::PanicClient(TESockPanic aPanic)
	{
	SafeMessage().PanicClient(KESockClientPanic, (TInt) aPanic);
	DontCompleteCurrentRequest();
	}


#ifdef SYMBIAN_NETWORKING_PERFMETRICS
void CSockSession::IncludePerformanceData(TInt aDeltaClientRxBytes, TInt aDeltaClientRxBuffBytes, TInt aDeltaClientTxBytes)
	{
	SockManGlobals()->IncludePerformanceData(aDeltaClientRxBytes, aDeltaClientRxBuffBytes, aDeltaClientTxBytes);
	}
#endif

/**
Constructor
*/
CSockSubSession::CSockSubSession(CSockSession* aSession, CPlayer* aPlayer, const Den::TSubSessionUniqueId aSubSessionUniqueId)
:	CWorkerSubSession(aSession, aPlayer, aSubSessionUniqueId)
	{
	}

void CSockSubSession::ConstructL(CProtocolBase* aProtocol)
	{
	CWorkerSubSession::ConstructL();
	if(aProtocol)
		{
		CSockSessionProxy* sp = static_cast<CSockSessionProxy*>(SessionProxy());
		sp->AddProtocolL(aProtocol);
		}
	}

CSockSession* CSockSubSession::Session()
	{
	return static_cast<CSockSession*>(Den::CWorkerSubSession::Session());
	}

const CSockSession* CSockSubSession::Session() const
	{
	return static_cast<const CSockSession*>(Den::CWorkerSubSession::Session());
	}

void CSockSubSession::DeleteMe()
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSubSession %08x:\tDeleteMe() - iSubSessionUniqueId %08X, iType %d, iSession %08x"), this, iSubSessionUniqueId, Type().iType, iSession) );

	switch(Type().iType)
		{
		case TCFSubSessInfo::ESocket:
			static_cast<CSocket*>(this)->InitiateDestruction();
			break;
		case TCFSubSessInfo::EHostResolver:
			static_cast<CHostResolver*>(this)->InitiateDestruction();
			break;
		case TCFSubSessInfo::EServiceResolver:
			{
			// Remove the subsession from the session's subsession list.
		    if(iSession)
				{
				iSession->SubSessions().Lock();

				CSubSessionIx::TSubSessionHandle handle;
				if(iSession->SubSessions().Find(this, handle) == KErrNone)
					{
					iSession->PitBoss().RemoveSubSession(handle, iSession);
					}
				
				iSession->SubSessions().Unlock();
				}

			delete this;
			}
			break;
		case TCFSubSessInfo::ENetDatabase:
			{
			// Remove the subsession from the session's subsession list.
		    if(iSession)
				{
				iSession->SubSessions().Lock();

				CSubSessionIx::TSubSessionHandle handle;
				if(iSession->SubSessions().Find(this, handle) == KErrNone)
					{
					iSession->PitBoss().RemoveSubSession(handle, iSession);
					}
				
				iSession->SubSessions().Unlock();
				}

			delete this;
			}
			break;
		case TCFSubSessInfo::EConnection:
			//Mimic the client sending ECNClose (without RMessage2)
			{
			const TNodeId& c = static_cast<CConnection&>(*this).Id();
			RClientInterface::OpenPostMessageClose(TNodeCtxId(ECNClose, c), TNodeCtxId(ECNClose, c), ESock::TCFInternalEsock::TSubSess(ECNClose,RMessage2()).CRef());
			}
			break;
		case TCFSubSessInfo::ESubConnection:
			//Mimic the client sending ESCClose (without RMessage2)
			{
			const TNodeId& sc = static_cast<CSubConnection&>(*this).Id();
			RClientInterface::OpenPostMessageClose(sc, TNodeCtxId(ESCClose, sc), ESock::TCFInternalEsock::TSubSess(ESCClose,RMessage2()).CRef());
			}
			break;
		default:
			//Please support your new subsession type
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockS_SES, 2));
		}

	}

CSockSubSession::~CSockSubSession()
	{
	LOG(ESockLog::Printf(KESockServerTag, _L8("CSockSubSession(%08x):\t~CSockSubSession Session(%08x)"), this, iSession) );
	
	// A subsession has a session proxy unless OOM prevented it
	if(SessionProxy())
		{
		SessionProxy()->NotifySubSessionDestroyed();
		}
	}

//Default implementation - must be specialised if used
void CSockSubSession::CommsApiExtBindIfaceL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	}

//Default implementation - must be specialised if used
void CSockSubSession::CommsApiExtIfaceSendReceiveL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	}

//Default implementation - must be specialised if used
void CSockSubSession::CloseExtensionInterface(const RMessage2& /*aMessage*/)
	{
	}

//Default implementation - must be specialised if used
void CSockSubSession::CancelAndCloseAllClientExtIfaces()
	{
	}

/** Client request to forwarded to the thread of the tier manager for the given tier. Possibilities:
(1) Mapping of tier to worker thread already known; can forward directly
(2) Mapping unknown & mappings loaded; fail request
(3) Mapping unknown & mappings not yet loaded; park request and request tier resolver to load mappings
(4) Tier resolver not yet located (ie during boot); request parked as indeterminate

Because only the main thread is guaranteed to have a binding to the tier resolver, the request for
mappings is always made by it, ie a secondary Dealer sends the request to load mappings to it. After
adding the mappings the main thread then messages all other workers to tell them to unpark any requests
awaiting the tier mappings.

The last thing added is the dummy "tiers loaded" flag, which serves to indicate to Dealers that resolution
is complete.
*/
void CSockSession::ForwardTierRequestL(const RMessage2& aMessage)
	{
    TWorkerId worker;
    TUint family = aMessage.Int0();
    TUint protocol = aMessage.Int2();
    TUid tierUid = TierManagerUtils::MapTierIdsL(TUid::Uid(family), protocol);
    CPitBoss& pitBoss = PitBoss();
	if(pitBoss.GetWorkerForTier(tierUid.iUid, worker))
		{
		//ForwardMessageL(aMessage, reinterpret_cast<CSockSubSession*>(tierUid.iUid), worker);
		ForwardMessageL(aMessage, TPlayerForwardRequestMsg::NormalCreationFlag(), worker); 
		}
	else
		{
		// No mapping found; either it's a bad id or we've yet to load mappings
		TInt err = KErrNone;
		if(PitBoss().TierMappingsLoaded())
			{
			err = KErrBadName;
			}
		else
			{
			err = Dealer()->ParkRequest(this, aMessage, CCommonDealer::EAwaitingTierToWorkerMapping);
			}
		if(err == KErrNone)
			{
			if(WorkerId() != TWorkerThreadPublicInfo::EMainThread)
				{
				TWorkerLoadTierMappings msg;
				WorkerThread().PostMessage(TWorkerThreadPublicInfo::EMainThread, msg);
				}
			else
				{
				PitBoss().RequestLoadTierMapping();
				}
			}
		if(err == KErrNone)
			{
			DontCompleteCurrentRequest();
			}
		else
			{
			SetReturn(err);
			}
		}
	}

CWorkerThread& CSockSession::WorkerThread() const
	{
	return static_cast<CWorkerThread&>(CWorkerSession::WorkerThread());
	}


CPitBoss& CSockSession::PitBoss() const
	{
	return static_cast<CPitBoss&>(CWorkerSession::PitBoss());
	}

void CSockSession::Disconnect(const RMessage2& aMessage)
	{
	LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CSockSession(%08x):\tDisconnect \"%S\""), this, &iProcessName) );
	CWorkerSession::Disconnect(aMessage);
	}
