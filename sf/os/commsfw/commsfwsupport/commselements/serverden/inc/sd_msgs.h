// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_DEN_MSGS_H
#define SYMBIAN_DEN_MSGS_H

#include <elements/cftransportmsg.h>
#include <elements/nm_interfaces.h>
#include <elements/sd_mintercept.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenMsgH, "ElemSvrDenMsgH");
#endif

namespace Den
{

/** Messages from Main Thread to Worker Threads (with Resp messages being responses
sent back from Worker)
*/
class TWorkerMsg : public CommsFW::TCFMessage
	{
public:
	enum
		{
		/** Number of messages reserved for ESock internal usage. Currently bigger than the actual
		number of messages to provide space for more in the future.
		@see TWorkerMsg::TCode::ELastWorkerMsgCode
		*/
		KWorkerMessageCodeReserveSize = 15
		};

	enum TCode
		{
		/** This is the first message sent on a binding by any worker thread. It identifies the basics
		about the worker
		*/
		EWorkerIntroduction = CommsFW::TCFCommsMessage::ECodePrivateFlag,

		/** This is the second message sent on a binding by a worker. It identifies a standard set of
		cookies. In the future it's expected that the legacy transport will only be used for the above
		initial intro message which will carry a boot-strapping cookie and all other cookies will
		be identified directly to that one
		*/
		EWorkerCookies,

		/** This is sent from the main thread to all workers when a significant configuration event
		happens, as indicated in the message (eg the boot of modules)
		*/
		EConfigurationComplete,

		/** This requests that the tier mappings are loaded from CommsDat. A worker can send it to the
		PitBoss and the PitBoss can send it to the tier resolver. In the latter case the tier resolver
		responds with TWorkerTierMappingsLoaded
		*/
		ELoadTierMappings,

		/** The tier resolver reports through this that the previously requested load of the tier mappings
		is complete
		*/
		ETierMappingsLoaded,

		/** This is sent from the PitBoss to a worker thread to notify that another worker has
		died and the receiver is expected to clean up any data structures/states related to that
		dead worker (if any).
		The receiver is expected to respond with a ECleanupDeadPeerResp message.
		@see TWorkerMsg::ECleanupDeadPeerResp
		*/
		ECleanupDeadPeer,

		/** Response from worker to main thread that cleanup after dead peer has been completed.
		@see TWorkerMsg::ECleanupDeadPeer
		*/
		ECleanupDeadPeerResp,

		/** Notification that a player role management responsibility has ended (eg a Player data
		flows signals the Player hosting subconnections when it unbinds, which can only happen when
		all flows have ceased dependency upon the sub-connections
		@see TWorkerMsg::TPlaneUnboundMsg
		*/
		EPlaneUnbound,

		/** Carries data that did not fit into EWorkerIntroduction
		*/
		EWorkerIntroduction2,

		/** Introduction message from main thread to workers to prompt objects to join their broker in the main thread
		*/
		EFactoryContainerIntroduction,

		/** Introduces a factory container object broker in the main thread to a worker thread
		*/
		EBrokerIntroduction,

		/** Dummy trailer code to give reference point for starting a following sequence of message codes. */
		ELastWorkerMsgCode = CommsFW::TCFCommsMessage::ECodePrivateFlag + KWorkerMessageCodeReserveSize
		};

	inline TWorkerMsg(TInt aCode)
		{
		Code(aCode);
		}
	};

/**
class TWorkerIntroductionMsg
*/
class TWorkerIntroductionMsg : public TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::EWorkerIntroduction};//< Message code
	enum {KMaxIntroductionInfoSize = sizeof(Messages::TNodeId)};

	inline TWorkerIntroductionMsg(const TWorkerThreadPublicInfo& aWorkerInfo)
	:	TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		params->iWorkerInfo = aWorkerInfo;
		params->iDebugMsgIntercept = NULL;
		params->iFailType = RAllocator::ENone;
		params->iFailRate = 0;
		params->iIntroductionInfo[0] = 0; //To mark as empty
		}

	inline const TWorkerThreadPublicInfo& WorkerInfo() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iWorkerInfo;
		}

	inline void IntroductionInfo(TDes8& aIntroductionInfo) const
		{
		TInt size = aIntroductionInfo.MaxSize() - aIntroductionInfo.Size();
		__ASSERT_DEBUG(size <= KMaxIntroductionInfoSize, User::Panic(KSpecAssert_ElemSvrDenMsgH, 1));
		const TParams* params=reinterpret_cast<const TParams*>(ParamBlock());
		aIntroductionInfo.Copy(&params->iIntroductionInfo[0], KMaxIntroductionInfoSize);
		}

	inline void SetIntroductionInfo(const TDesC8& aIntroductionInfo)
		{
		__ASSERT_DEBUG(aIntroductionInfo.Size() <= KMaxIntroductionInfoSize, User::Panic(KSpecAssert_ElemSvrDenMsgH, 2));

		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		__ASSERT_DEBUG(params->iIntroductionInfo[0] == 0, User::Panic(KSpecAssert_ElemSvrDenMsgH, 3));
		Mem::Copy(params->iIntroductionInfo, aIntroductionInfo.Ptr(), aIntroductionInfo.Size());
		}

#ifdef _DEBUG
	inline void SetFailType(RAllocator::TAllocFail aFailType)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iFailType = aFailType;
		}
	inline RAllocator::TAllocFail FailType() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iFailType;
		}
	inline void SetFailRate(TInt aFailRate)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iFailRate = aFailRate;
		}
	inline TInt FailRate() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iFailType;
		}
#endif

#if defined (__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)
	inline void SetMessageInterceptGlobal(CMessageInterceptRegister* aMsgInterceptRegister)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iDebugMsgIntercept = reinterpret_cast<TAny*>(aMsgInterceptRegister);
		}
	inline CMessageInterceptRegister* MsgInterceptRegister() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return reinterpret_cast<CMessageInterceptRegister*>(params->iDebugMsgIntercept);
		}
#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE

private:
	struct TParams /** Message parameters */
		{
		TWorkerThreadPublicInfo iWorkerInfo;	//< Crucial information about the worker thread/server
		// Pointer to the process wide singleton created by the main thread
		TAny* iDebugMsgIntercept;
		RAllocator::TAllocFail iFailType;
		TInt iFailRate;
		TUint8 iIntroductionInfo[KMaxIntroductionInfoSize];
		};
	__ASSERT_COMPILE(sizeof(TParams) <= CommsFW::KCFMessageParamBlockSize);
	};

/**
class TWorkerConfigurationComplete
@see TWorkerMsg::EConfigurationComplete
*/
class TWorkerConfigurationComplete : public TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::EConfigurationComplete};/**Message code*/
	inline TWorkerConfigurationComplete(TConfigurationCompletionType aType)
	: TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		params->iType = aType;
		}
	inline TConfigurationCompletionType Type() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iType;
		}
private:
	struct TParams /** Message parameters */
		{
		TConfigurationCompletionType iType;
		};
	};

/**
class TWorkerCleanupDeadPeerMsg
@see TWorkerMsg::ECleanupDeadPeer
*/
class TWorkerCleanupDeadPeerMsg : public TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::ECleanupDeadPeer};//< Message code
	inline TWorkerCleanupDeadPeerMsg(CommsFW::TWorkerId aWorkerId)
	: TWorkerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iWorkerId = aWorkerId;
		}
	inline CommsFW::TWorkerId WorkerId() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iWorkerId;
		}
private:
	struct TParams /** Message parameters */
		{
		CommsFW::TWorkerId iWorkerId;	//< Identifying which dead peer to cleanup
		};
	};

/**
class TWorkerCleanupDeadPeerRespMsg
@see TWorkerMsg::ECleanupDeadPeerResp
*/
class TWorkerCleanupDeadPeerRespMsg : public TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::ECleanupDeadPeerResp};//< Message code
	inline TWorkerCleanupDeadPeerRespMsg(CommsFW::TWorkerId aWorkerId)
	: TWorkerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iWorkerId = aWorkerId;
		}
	inline CommsFW::TWorkerId WorkerId() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iWorkerId;
		}
private:
	struct TParams /** Message parameters */
		{
		CommsFW::TWorkerId iWorkerId;	//< Identifying which dead peer the cleanup was related to.
		};
	};

class TPlaneUnboundMsg : public TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::EPlaneUnbound};//< Message code
	inline TPlaneUnboundMsg(TPlayerRole aPlayerRole)
	:	Den::TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		params->iPlayerRole = aPlayerRole;
		}

	inline const TPlayerRole& PlayerRole() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iPlayerRole;
		}
private:
	struct TParams /** Message parameters */
		{
		TPlayerRole iPlayerRole;
		};
	};

/**
class TPlayerMsg
Messages from a Dealer to a Player in another thread
*/
class TPlayerMsg : public CommsFW::TCFMessage
	{
public:
	enum TCode
		{
		/** This is the most common message. It is used to forward IPC requests
		from a Dealer to a Player. */
		EForwardRequest = TWorkerMsg::ELastWorkerMsgCode + 1,

		/** When a session is closed by a client the Dealer will send this message to all Players
		that might have relations (sub-sessions) to this session.
		@note The receiver is expected to response with a ESessionCloseResp message.
		@see TPlayerMsg::ESessionCloseResp
		*/
		ESessionClose,

		/** Notify the Dealer that cleanup related to the ESessionClose message is completed.
		@see TPlayerMsg::ESessionClose
		*/
		ESessionCloseResp,
		};

	inline TPlayerMsg(TInt aCode)
		{
		Code(aCode);
		}
	};

/**
class TPlayerForwardRequestMsg
@see TPlayerMsg::EForwardRequest
*/
class TPlayerForwardRequestMsg : public TPlayerMsg
	{
public:
	enum {KCode = TPlayerMsg::EForwardRequest};//< Message code
	inline TPlayerForwardRequestMsg(const RMessage2& aMsg, CWorkerSubSession* aSubSession):TPlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iMsg.Adopt(static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(aMsg)));
		params->iSubSession=aSubSession;
		}
	/* Create requests have no meaningful subsession pointer (it gets created through the request) but can need
	to communicate additional status. This is carried through these magic subsession consts
	*/
	inline static CWorkerSubSession* UnusedParam()
		{
		return reinterpret_cast<CWorkerSubSession*>(NULL);
		}
	inline static CWorkerSubSession* NormalCreationFlag()
		{
		return reinterpret_cast<CWorkerSubSession*>(NULL);
		}
	inline static CWorkerSubSession* AbnormalCreationFlag()
		{
		return reinterpret_cast<CWorkerSubSession*>(-1);
		}
	inline const RSafeMessage& SafeMessage() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iMsg;
		}
	inline CWorkerSubSession* SubSession() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iSubSession;
		}
private:
	struct TParams /** Message parameters */
		{
		/** The IPC message to process. The Player must complete this itself when ready.
		When sent to a Player the Dealer will have forgotten completely about it and left
		the responsibility to the Player.
		*/
		RSafeMessage iMsg;
		CWorkerSubSession* iSubSession;
		};

	__ASSERT_COMPILE(sizeof(TParams) <= CommsFW::KCFMessageParamBlockSize);
	};

/**
class TPlayerSessionCloseMsg
@see TPlayerMsg::ESessionClose
*/
class TPlayerSessionCloseMsg : public TPlayerMsg
	{
public:
	enum {KCode = TPlayerMsg::ESessionClose};//< Message code
	inline TPlayerSessionCloseMsg(CWorkerSession* aSession):TPlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iSession=aSession;
		}

	inline CWorkerSession* Session() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iSession;
		}
private:
	struct TParams /** Message parameters */
		{
		CWorkerSession* iSession;			//< The session that is being closed.
		};
	};

/**
class TPlayerSessionCloseRespMsg
@see TPlayerMsg::ESessionCloseResp
*/
class TPlayerSessionCloseRespMsg : public TPlayerMsg
	{
public:
	enum {KCode = TPlayerMsg::ESessionCloseResp};//< Message code
	inline TPlayerSessionCloseRespMsg(CommsFW::TWorkerId aPlayerId, CWorkerSession* aSession)
	: TPlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(ParamBlock());
		params->iSession=aSession;
		params->iPlayerId=aPlayerId;
		}
	inline CWorkerSession* Session() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iSession;
		}
	inline const CommsFW::TWorkerId WorkerId() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iPlayerId;
		}
private:
	struct TParams /** Message parameters */
		{
		CWorkerSession* iSession;	//< The session which was closed
		CommsFW::TWorkerId iPlayerId;			//< Id of the sending worker thread
		};
	};

class TWorkerTransportMsg : public CommsFW::TCFMessage
	{
public:
	inline TWorkerTransportMsg(TInt aCode)
		{
		Code(aCode);
		TWorkerTransportParams* params = reinterpret_cast<TWorkerTransportParams*>(ParamBlock());
		params->iStatus = NULL;
		}
	inline TWorkerTransportMsg(TInt aCode, TRequestStatus& aStatus)
		{
		Code(aCode);
		TWorkerTransportParams* params = reinterpret_cast<TWorkerTransportParams*>(ParamBlock());
		params->iStatus = &aStatus;
		}
	inline TRequestStatus& Status() const
		{
		const TWorkerTransportParams* params = reinterpret_cast<const TWorkerTransportParams*>(ParamBlock());
		return *params->iStatus;
		}
	inline TRequestStatus* GetStatusPtr() const
		{
		const TWorkerTransportParams* params = reinterpret_cast<const TWorkerTransportParams*>(ParamBlock());
		return params->iStatus;
		}
	inline void SetStatusPtr(TRequestStatus* aStatus)
		{
		TWorkerTransportParams* params = reinterpret_cast<TWorkerTransportParams*>(ParamBlock());
		params->iStatus = aStatus;
		}
protected:
	struct TWorkerTransportParams /** Message parameters */
		{
		TRequestStatus* iStatus;	//< Pointer to request status to set when the request is complete.
		};
	};

} //namespace Den

#endif
//SYMBIAN_DEN_MSGS_H


