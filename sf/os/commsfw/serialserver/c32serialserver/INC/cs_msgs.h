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

#ifndef CS_MSGS_H
#define CS_MSGS_H

#include <cfmsgs.h>
#include <elements/cftransport.h>
#include <cfshared.h>
#include <cfutil.h>



/** Messages from Main Thread to Worker Threads (with Resp messages being responses
sent back from Worker)
*/
class TC32WorkerMsg : public CommsFW::TCFMessage
	{
public:
	enum
		{
		/** Number of messages reserved for C32 internal usage. Currently bigger than the actual
		number of messages to provide space for more in the future since player msgs being after this block.
		The number is fairly arbitrary and is a guess at being "big enough" to accomodate a
		reasonable multiplicity of future messages given we only use a couple at the moment.
		@see TC32WorkerMsg::TCode::ELasTC32WorkerMsgCode
		*/
		KC32MessageCodeReserveSize = 15
		};
	enum TCode {

		/** This is the first message between worker thread and the main thread. It is sent from
		the main thread when receiving a bind request and starts the initialisation sequence.
		The receiver is expected to respond with a EMainIntroductionResp message.
		@see TC32WorkerMsg::EMainIntroductionResp
		*/
		EMainIntroduction = CommsFW::TCFCommsMessage::ECodePrivateFlag,

		/** This is sent from a worker to the main thread in response to the
		introduction message from the main thread. Once received the main thread will notify
		the Root Server that the binding is complete.
		@see TC32WorkerMsg::EMainIntroduction
		*/
		EMainIntroductionResp,

		/** Dummy trailer code to give reference point for starting a following sequence of message codes. */
		ELasTC32WorkerMsgCode = CommsFW::TCFCommsMessage::ECodePrivateFlag + KC32MessageCodeReserveSize
		};
	inline TC32WorkerMsg(TInt aCode)
		{
		Code(aCode);
		}
	};

class CC32Dealer;

/**
@class
TC32WorkerMainIntroductionMsg
@see TC32WorkerMsg::EMainIntroduction
*/
class TC32WorkerMainIntroductionMsg : public TC32WorkerMsg
	{
public:
	enum {KCode = EMainIntroduction};//< Message code
	inline TC32WorkerMainIntroductionMsg(CC32Dealer* aDealer)
	: TC32WorkerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iDealer=aDealer;
		}
	inline CC32Dealer* Dealer() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iDealer;
		}
private:
	struct TParams
		{
		CC32Dealer* iDealer;					//< The Dealer
		};
	};

/**
@class
TC32WorkerMainIntroductionRespMsg
@see TC32WorkerMsg::EMainIntroductionResp
*/
class TC32WorkerMainIntroductionRespMsg : public TC32WorkerMsg
	{
public:
	enum {KCode = TC32WorkerMsg::EMainIntroductionResp};//< Message code
	inline TC32WorkerMainIntroductionRespMsg(const TC32WorkerThreadPublicInfo& aWorkerInfo)
	: TC32WorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iWorkerInfo=aWorkerInfo;
#ifdef _DEBUG
  		params->iFailType = RAllocator::ENone;
  		params->iFailRate = 0;
#endif
		}

	inline const TC32WorkerThreadPublicInfo& WorkerInfo() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iWorkerInfo;
		}
#ifdef _DEBUG
  	inline void SetFailType(RAllocator::TAllocFail aFailType)
  		{
  		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
  		params->iFailType = aFailType;
  		}
  	inline RAllocator::TAllocFail FailType() const
  		{
  		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
  		return params->iFailType;
  		}
  	inline void SetFailRate(TInt aFailRate)
  		{
  		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
  		params->iFailRate = aFailRate;
  		}
 	inline TInt FailRate() const
  		{
  		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
  		return params->iFailRate;
  		}
#endif

private:
	struct TParams
		{
		TC32WorkerThreadPublicInfo iWorkerInfo;	//< Crucial information the Dealer needs about the worker thread/server
#ifdef _DEBUG
  		RAllocator::TAllocFail iFailType;
  		TInt iFailRate;
#endif
		};
	};




/**
@class
TC32PlayerMsg
Messages from a Dealer to a Player in another thread
*/
class TC32PlayerMsg : public CommsFW::TCFMessage
	{
public:
	enum TCode {
		/** This is the most common message. It is used to forward IPC requests
		from a Dealer to a Player. */
		EForwardRequest = TC32WorkerMsg::ELasTC32WorkerMsgCode + 1,

		/** When a session is closed by a client the Dealer will send this message to all Players
		that might have relations (sub-sessions) to this session.
		@note The receiver is expected to response with a ESessionCloseResp message.
		@see TC32PlayerMsg::ESessionCloseResp
		*/
		ESessionClose,

		/** Notify the Dealer that cleanup related to the ESessionClose message is completed.
		@see TC32PlayerMsg::ESessionClose
		*/
		ESessionCloseResp,

		/** This message is sent from dealer to player on load request. Response message is sent from
		player back.
		@see TC32PlayerMsg::ELoadCommModuleSuccessResp
		@see TC32PlayerMsg::ELoadCommModuleFailureResp
		*/
		ELoadCommModuleMsg,

		/** This message is sent from the player to dealer in response to ELoadCommModuleMsg
		msg. On success, CSerial* with RMessage2 is sent to dealer. CSerial* is used by dealer to
		update the ThreadManager entry of PortPrefix, while csyfilename is retrieved from message again.
		After updation, the message is completed by dealer
		*/
		ELoadCommModuleSuccessResp,

		/** This message is sent from player to dealer when CSY load fails. The load failure
		result is also sent with the message which the dealer completes the message with.
		*/
		ELoadCommModuleFailureResp,

		/**
		Message sent from deale to player to unload Comm Module
		*/
		EUnLoadCommModuleMsg,
		/**
		This is the message used specifically for RCommServ::GetPortInfo (aIndex) API as
		global index which is sent via aMessage is no longer valid in player, which maintains
		its local index.
		*/
		EGetPortInfoMsg,
		};
	inline TC32PlayerMsg(TInt aCode)
		{
		Code(aCode);
		}
	};

/**
@class
TC32PlayerForwardRequestMsg
@see TC32PlayerMsg::EForwardRequest
*/
class TC32PlayerForwardRequestMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::EForwardRequest};//< Message code
	inline TC32PlayerForwardRequestMsg(const RMessage2& aMsg, CCommSubSession* aSubSession):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iMsg=aMsg;
		params->iSubSession=aSubSession;
		}
	inline const RMessage2& Message() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iMsg;
		}
	inline CCommSubSession* SubSession() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSubSession;
		}
private:
	struct TParams
		{
		/** The IPC message to process. The Player must complete this itself when ready.
		When sent to a Player the Dealer will have forgotten completely about it and left
		the responsibility to the Player.
		*/
		RMessage2 iMsg;
		CCommSubSession* iSubSession;
		};
	};

/**
@class
TC32PlayerSessionCloseMsg
@see TC32PlayerMsg::ESessionClose
*/
class TC32PlayerSessionCloseMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::ESessionClose};//< Message code
	inline TC32PlayerSessionCloseMsg(CCommSession* aSession, const TInt64& aSessionCloseDeadline):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iSession=aSession;
		params->iSessionCloseDeadline=aSessionCloseDeadline;
		}

	inline CCommSession* Session() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSession;
		}
	inline TInt64 SessionCloseDeadline() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSessionCloseDeadline;
		}
private:
	struct TParams
		{
		CCommSession* iSession;			//< The session that is being closed.

		/** The session pointer is only valid if TTime.HomeTime() returns a value smaller than
		the value stored here, otherwise the Dealer will have timed out and deleted the session. */
		TInt64 iSessionCloseDeadline;
		};
	};

/**
@class
TC32PlayerSessionCloseRespMsg
@see TC32PlayerMsg::ESessionCloseResp
*/
class TC32PlayerSessionCloseRespMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::ESessionCloseResp};//< Message code
	inline TC32PlayerSessionCloseRespMsg(CommsFW::TWorkerId aPlayerId, const TC32PlayerSessionCloseMsg& aCloseMsg):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iSession=aCloseMsg.Session();
		params->iPlayerId=aPlayerId;
		params->iSessionCloseDeadline=aCloseMsg.SessionCloseDeadline();
		}
	inline CCommSession* Session() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSession;
		}
	inline CommsFW::TWorkerId WorkerId() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iPlayerId;
		}
	inline TInt64 SessionCloseDeadline() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSessionCloseDeadline;
		}
private:
	struct TParams
		{
		CCommSession* iSession;			//< The session which was closed
		CommsFW::TWorkerId iPlayerId;			//< Id of the sending worker thread

		/** Must contain the session close deadline from the session close
        message which this message is responding to. */
		TInt64 iSessionCloseDeadline;
		};
	};

/**
@class
TC32PlayerLoadCommModuleMsg
@see TC32PlayerMsg::ELoadCommModuleMsg
*/
class TC32PlayerLoadCommModuleMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::ELoadCommModuleMsg};//< Message code
	inline TC32PlayerLoadCommModuleMsg(const RMessage2& aMsg):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iMsg=aMsg;
		}
	inline const RMessage2& Message() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iMsg;
		}
private:
	struct TParams
		{
		/** The IPC message to process. The Player must complete this itself when ready.
		When sent to a Player the Dealer will have forgotten completely about it and left
		the responsibility to the Player.
		*/
		RMessage2 iMsg;
		};
	};

/**
@class
TC32PlayerLoadCommModuleSuccessResp
@see TC32PlayerMsg::ELoadCommModuleSuccessResp
*/
class TC32PlayerLoadCommModuleSuccessResp : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::ELoadCommModuleSuccessResp};//< Message code
	inline TC32PlayerLoadCommModuleSuccessResp(const RMessage2& aMsg, CSerial* aSerial):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iMsg=aMsg;
		params->iSerial=aSerial;
		}
	inline const RMessage2& Message() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iMsg;
		}
	inline CSerial* SerialPtr() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSerial;
		}
private:
	struct TParams
		{
		/** The IPC message to process. The Player must complete this itself when ready.
		When sent to a Player the Dealer will have forgotten completely about it and left
		the responsibility to the Player.
		*/
		RMessage2 iMsg;
		CSerial* iSerial;	//< used in PortInfo(aIndex)
		};
	};


/**
@class
TC32PlayerLoadCommModuleMsg
@see TC32PlayerMsg::ELoadCommModuleFailureResp
*/
class TC32PlayerLoadCommModuleFailureResp : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::ELoadCommModuleFailureResp};//< Message code
	inline TC32PlayerLoadCommModuleFailureResp(const RMessage2& aMsg, TInt aFailReason):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iMsg=aMsg;
		params->iFailReason=aFailReason;
		}
	inline const RMessage2& Message() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iMsg;
		}
	inline TInt FailureReason() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iFailReason;
		}
private:
	struct TParams
		{
		RMessage2 iMsg;
		TInt iFailReason;
		};
	};
/*
@class
TC32PlayerUnLoadCommModuleMsg
@see TC32PlayerMsg::EUnLoadCommModuleMsg
*/
class TC32PlayerUnLoadCommModuleMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::EUnLoadCommModuleMsg};//< Message code
	inline TC32PlayerUnLoadCommModuleMsg(CSerial* aSerial):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iSerial=aSerial;
		}
	inline CSerial* SerialPtr() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSerial;
		}
private:
	struct TParams
		{
		CSerial* iSerial; //< CSerial to call Close()
		};
	};


/**
@class
TC32PlayerGetPortInfoMsg
@see TC32PlayerMsg::EGetPortInfoMsg
*/
class TC32PlayerGetPortInfoMsg : public TC32PlayerMsg
	{
public:
	enum {KCode = TC32PlayerMsg::EGetPortInfoMsg};//< Message code
	inline TC32PlayerGetPortInfoMsg(const RMessage2& aMsg, CSerial* aSerial):TC32PlayerMsg(KCode)
		{
		TParams* params = reinterpret_cast<TParams*>(TCFMessage::ParamBlock());
		params->iMsg=aMsg;
		params->iSerial=aSerial;
		}
	inline const RMessage2& Message() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iMsg;
		}
	inline CSerial* SerialPtr() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(TCFMessage::ParamBlock());
		return params->iSerial;
		}
private:
	struct TParams
		{
		/** The IPC message to process. The Player must complete this itself when ready.
		When sent to a Player the Dealer will have forgotten completely about it and left
		the responsibility to the Player.
		*/
		RMessage2 iMsg;
		CSerial* iSerial;
		};
	};

#endif	// CS_MSGS_H

