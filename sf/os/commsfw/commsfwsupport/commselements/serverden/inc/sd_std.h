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

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_DEN_COMMONSTD_H
#define SYMBIAN_DEN_COMMONSTD_H

#include <cflog.h>
#include <cfshared.h>
#include <elements/h_structures.h>
#include <elements/sd_thread.h>
#include <elements/sd_apiextensionregister.h>
#include <elements/nm_interfaces.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenStdH, "ElemSvrDenStdH");
#endif

namespace Den
{


/**
@internalComponent
*/
typedef TUint TSessionUniqueId;
typedef TUint TSubSessionUniqueId;
typedef TUint TScopeUniqueId;

class CCommonPlayer;
class CWorkerSession;
class CCommonSessionProxy;
class TIpcExtensionRoutingMessageFromSession;
class TIpcExtensionRoutingMessageFromSubSession;

_LIT(KDenFaultPanic, "Den");
_LIT(KWorkerClientPanic, "Worker client");

/** Supplementing RMessage2 with additional methods for transferring handle ownership and completing with
logging
*/
class RSafeMessage : public RMessage2
/**
@internalTechnology
*/
	{
public:
	/** Only a default constructor is supported; safe messages get initialised by adopting their handle from
	another instance, by downcasting from a received RMessage2 (ugly but safe since safe messages never add
	members or virtuals), or (rarely) by duplicating another instance, which diminishes safety.
	*/
	RSafeMessage()
		{
		iHandle = KNullHandle;
		}

	IMPORT_C ~RSafeMessage();

	IMPORT_C void Complete(TInt aReason) const;

	IMPORT_C void PanicClient(const TDesC& aCategory, TInt aReason) const;

	IMPORT_C void Adopt(RSafeMessage& aSrc);
	IMPORT_C void Duplicate(const RSafeMessage& aSrc);
	IMPORT_C void ResetHandle();

    IMPORT_C void ReadL(TInt aSrcParamIndex,TDes8& aDes,TInt anOffset=0) const;
	IMPORT_C TInt Read(TInt aSrcParamIndex,TDes8& aDes,TInt anOffset=0) const;

    IMPORT_C void WriteL(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0) const;
    IMPORT_C TInt Write(TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset=0) const;

    IMPORT_C void WriteL(TInt aDstParamIndex,const TPckg<TUint32>& aDes,TInt anOffset = 0) const;
	IMPORT_C TInt Write(TInt aDstParamIndex,const TPckg<TUint32>& aDes,TInt anOffset = 0) const;

    IMPORT_C void ReadL(TInt aSrcParamIndex,TDes16& aDes,TInt anOffset=0) const;
    IMPORT_C TInt Read(TInt aSrcParamIndex,TDes16& aDes,TInt anOffset=0) const;

    IMPORT_C void WriteL(TInt aDstParamIndex,const TDesC16& aDes,TInt anOffset=0) const;
    IMPORT_C TInt Write(TInt aDstParamIndex,const TDesC16& aDes,TInt anOffset=0) const;


	IMPORT_C TInt GetDesLengthL(TInt aParam) const;
	IMPORT_C TInt GetDesMaxLengthL(TInt aParam) const;

private:
	// Conventional assignment and copy construction is not supported because of the likelihood of completing the
	// aliased message handle twice, eg storing a copy from a ServiceL() and then leaving, so completing the request
	// both from the error handler and the storing object's destructor
	RSafeMessage(const RSafeMessage&);
	RSafeMessage(const RMessage2&);
	RSafeMessage& operator=(const RSafeMessage&);
	RSafeMessage& operator=(const RMessage2&);
	};

class RSettableParameterMessage : public RSafeMessage
/**
@internalTechnology
*/
	{
public:
	inline RSettableParameterMessage(TInt aFunction, TInt aInt0 = 0, TInt aInt1 = 0, TInt aInt2 = 0, TInt aInt3 = 0)
		{
		iFunction = aFunction;
		iArgs[0] = aInt0;
		iArgs[1] = aInt1;
		iArgs[2] = aInt2;
		iArgs[3] = aInt3;
		}

	inline void SetInt0(TInt aInt0)
		{
		iArgs[0] = aInt0;
		}

	inline void SetInt1(TInt aInt1)
		{
		iArgs[1] = aInt1;
		}

	inline void SetInt2(TInt aInt2)
		{
		iArgs[2] = aInt2;
		}

	inline void SetInt3(TInt aInt3)
		{
		iArgs[3] = aInt3;
		}
	};

/** The panic numbers for panics caused by internal faults in Den
@internalComponent */

//TODO:
enum TDenFault
	{
	ECommonBadState = 0,
	ECommonNegativeSessionCount = 1,  //< Negative Session Count Den  Fault
	ECommonBadDescriptorRead = 2,     //< Read Bad Descriptor Den  Fault
	ECommonBadDescriptorWrite = 3,    //< Write Bad Descriptor Den  Fault
	ECommonBadDescriptorLength = 4,   //< Bad Descriptor Length Den  Fault
	ECommonCorruptIniData = 5,        //< Corrupt In Data Den  Fault
	ECommonBadSubSessionUniqueId = 6  //< Bad Subsession Unique Id Den  Fault
	};

class TPlayerRole : public Messages::TClientType
/**
Can be sent in a message and read as TClientType (for storage in iClients)

@internalComponent
*/
	{
public:
	inline TPlayerRole() //Needed for deserialisation
	:	Messages::TClientType()
		{
		}

	inline TBool HasRole(TUint32 aRole) const
		{
		return iClientType & aRole;
		}

	inline TUint32 Role() const
		{
		return iClientType;
		}

	inline TUint32 Kindex() const
		{
		return iClientFlags;
		}

protected:
	//This class is meant for derivation only, hence the useful ctr protected
	inline TPlayerRole(TUint32 aRole, TUint32 aKindex)
	:	Messages::TClientType(aRole, aKindex)
		{
		}
	};

/** Describes the type of configuration completed when a TWorkerConfigurationComplete is broadcast. (Although logically
associated with the message class putting it there introduces undesirable dependencies)
*/
enum TConfigurationCompletionType
/**
@internalComponent
*/
	{
	EModuleInitialisation,
	ETierMapping,
	};

/**
	Type identifier for the derived sub-session objects. Used e.g. in containers to identify
	what is stored.
	@see CWorkerSubSession::Type()
*/
class TSubSessInfo
/**
@internalComponent
*/
	{
public:
	enum TSubSessionType
		{
		ENull = 0,
		EAny = 1
		};

	TSubSessInfo(TUint8 aType)
	:	iType(aType)
		{
		}

	TUint8 iType;
	};

class CWorkerSubSession : public CBase
/**
@internalComponent
*/
	{
	friend class CWorkerSession;
	friend class CCommonPlayer;

public:
	// Reference counting methods formerly supplied by CObject.
    IMPORT_C virtual ~CWorkerSubSession();

	IMPORT_C void GetOwnerInfo(TProcessId& aProcessId, TUidType& aUidType, TThreadId& aThreadId) const;
	inline CWorkerSession* Session();
	inline const CWorkerSession* Session() const;
	inline CCommonPlayer& Player() const;
	inline CCommonSessionProxy* SessionProxy();
	inline const CCommonSessionProxy* SessionProxy() const;
	inline void SetSessionProxy(CCommonSessionProxy* aProxy);

	virtual void DeleteMe() = 0;

	void CleanupDeadWorker(CommsFW::TWorkerId aWorkerId);
	virtual TSubSessInfo Type() const = 0;

	// Some subsessions have opaque "names" that a client can retrieve to use as a reference
	// outside the owning session, eg for transferring a socket between sessions.
	IMPORT_C static void ComposeSubSessionName(const CWorkerSubSession* aSubSess, TName& aName);
	static TInt DecomposeSubSessionName(const TName& aName, CommsFW::TWorkerId& aWorkerId, CWorkerSubSession*& aSubSess);
	IMPORT_C static TInt FetchSubSessionFromName(const TName& aName, TSubSessInfo aType, const CCommonWorkerThread& aWorker, CWorkerSubSession*& aSubSess);


	IMPORT_C void DontCompleteCurrentRequest() const;

	IMPORT_C void SetReturn(TInt aReturnValue/*,TInt anError=0,TUid anOwner=TUid::Null()*/) const;

	TBool IsClosing() const { return iClosing; }

	/**
		Derived objects must implement this to make sure that all RMessage2 resources are completed when called. It
		is used in various close/clean-up scenarios.
	*/
	virtual void FinalCompleteAllBlockedMessages(TInt aResult) = 0;

//	virtual inline const RMessage2& Message() const; migrate to using SafeMessage
	IMPORT_C const RSafeMessage& SafeMessage() const;

	/**
		Derived objects must implement this to continue the processing of the current message.
	*/
	virtual void ProcessMessageL() = 0;

	inline const TSubSessionUniqueId& UniqueId() const
		{
		return iSubSessionUniqueId;
		}

	IMPORT_C void SetUniqueId(const TSubSessionUniqueId& aUniqueId);

	inline TScopeUniqueId NextScopeUniqueId()
		{
		return static_cast<TScopeUniqueId>(iNextScopeUniqueId++);
		}

	/** Special non-zero subsession pointer, used in a very few cases to select additional behaviour on a forwarded request. For example on
	creating a subsession this flags that tier resolution is first required
	*/
	static CWorkerSubSession* KFlagSpecialForwardCase();
	inline void SetSession(CWorkerSession* aProxy);

	IMPORT_C void StoreOwnerInfo();

protected:
	// API extension support
	IMPORT_C TIpcExtensionRoutingMessageFromSubSession* IsSubSessionIpcExtensionSupported(TInt aInterfaceId, TDes8& aBuff);
	IMPORT_C void OpenSubSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void SendReceiveSubSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void CloseSubSessionIpcExtension(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void CloseAllSubSessionIpcExtensions();
	virtual TIpcExtensionRoutingMessageFromSubSession* DoIsSubSessionIpcExtensionSupported(TInt /*aInterfaceId*/, TDes8& /*aBuff*/)
		{
		//Not supported by default
		return NULL;
		}

protected:
	IMPORT_C CWorkerSubSession(CWorkerSession* aSession, CCommonPlayer* aPlayer, TSubSessionUniqueId aSubSessionUniqueId);
	IMPORT_C void ConstructL();

	void SetClosing()
		{
		iClosing = ETrue;
		}

	IMPORT_C void CompleteMessage(const RMessage2& aMessage, TInt aResult);
	void RemoveAndDestroy();

	IMPORT_C void PanicClient(TInt aPanic);
	IMPORT_C void PanicClient(const TDesC& aCategory, TInt aPanic);

protected:
	CWorkerSession* iSession;
	/**
	The thread id of the creator of the subsession, part of the OwnerInfo. This can be misleading,
	since sessions can be shared between threads of a client - to really know what thread is
	responsible for the current operator query its RMessage2
	*/
	TThreadId iThreadId;
	TBool iClosing : 1;		//< Set once the client directly closes the object (closing can be asynchronous. Remembering this state avoids a double close if the session then closes)
	CCommonPlayer* iPlayer;		//< The Player responsible for this sub-session instance.
	TSubSessionUniqueId iSubSessionUniqueId; //Sub session unique id.

private:
	TApiExtRegister iApiExtRegister;   //Register of opened interfaces (on this subsession only)
	CCommonSessionProxy* iSessionProxy;
	TUint iNextScopeUniqueId;
	};

// Subsession index

/**
class CSubSessionIx
This sub-session container will maintain its own array of TEntry cells in memory as arrays provide for
quick access in some situations. There will be two types of cells in the array; cells carrying pointers
to sub-sessions and a virtual list of free cells.
This layout means that with a handle, the entry holding the sub-session can be looked up in the container
directly and if needing to insert a new entry the first free cell is indicated by iFreeListHead.
The array will grow with EIndexGranularity each time it runs out of free entries. However, it maintains a high
watermark and will never shrink.
The handles are defined by TSubSessionHandle, which in the current architectures is a 32 bit word, constructed
as needed. It is simple to deduct from the context so there is no need to store it.

0        8        16              31
|--------|--------|----------------|
   Type     Magic       Index

The three components comprising the handle are:
    - Type	- 8 bit value. One of the members of CSockSubsession::TSubSessionType.
    - Magic	- 8 bit value. Ensures that handles are not re-used immediately. Each time an entry is re-cycled this value increments, rolling around when reaching max.
    - Index	- 16 bit value. The position in the array.
@see CSockSubsession::TSubSessionType
*/
class CSubSessionIx : public CBase
/**
@internalComponent
*/
	{
	friend class TIter;

public:
	typedef TInt TSubSessionHandle;
	enum { KTypeBits = 8, KMagicBits = 8, KIndexBits = 16 };
	enum { KIndexLimit = 1 << KIndexBits };
	enum { KMagicMask = (1 << KMagicBits) - 1 };

public:
	IMPORT_C ~CSubSessionIx();

	void InitialiseL();
	inline void Lock() const;
	inline void Unlock() const;

	inline TInt ActiveCount() const;
	IMPORT_C CWorkerSubSession* At(TInt aHandle, TSubSessInfo aType) const;
	IMPORT_C TInt Find(CWorkerSubSession* aSubSession, TSubSessionHandle& aHandle) const;

	TInt Add(CWorkerSubSession* aSubSession, TSubSessionHandle& aHandle);
	IMPORT_C CWorkerSubSession* Replace(TSubSessionHandle aHandle, CWorkerSubSession* aSubSession);
	IMPORT_C CWorkerSubSession* Remove(TSubSessionHandle aHandle);

	NONSHARABLE_CLASS(TIter)
		{
	public:
		TIter(CSubSessionIx& aContainer);
		void SetToFirst();
		CWorkerSubSession* operator++(TInt);
		CWorkerSubSession* Next(TSubSessionHandle& aHandle);
	private:
		TInt iPos;
		CSubSessionIx& iContainer;
		};

private:
	enum { EIndexGranularity = 8 };

	/** class CSubSessionIx::TEntry
	One entry in the container. If iType is CWorkerSubSession::ENull, then it is a member of the free-list
	and iNextFree is the index of the next free TEntry instance otherwise it is an iObject* and points
	to a sub-session instance.
	iType can be one those defined in CPlayerSubsession::TSubSessionType.
	@see CSockSubsession::TSubSessionType
	*/
	class TEntry
		{
	public:
		union
			{
			CWorkerSubSession* iObject;
			TInt iNextFree;
			};
		TUint8 iType;
		TUint16 iMagic;
		};
private:
	inline TSubSessionHandle MakeHandle(TInt aIndex, TInt aMagic, TInt aType) const
		{
		return (((aIndex << KMagicBits) | aMagic) << KTypeBits) | aType;
		}
	inline TInt IndexFromHandle(TInt aHandle) const
		{
		return aHandle >> (KTypeBits + KMagicBits);
		}
	IMPORT_C TEntry* At(TSubSessionHandle aHandle) const;
	TInt ExpandArray();

private:
	TEntry* iIx;
	TInt iSize;
	TInt iActiveCount;			//< Number of active sub-sessions stored in this container.
	TInt iFreeListHead;			//< Pointer to the first item/cell in the virtual free list.
	mutable RWorkerLock iLock;
	};


class CWorkerSession: public CSession2
/**
@internalComponent
*/
	{
	friend class CCommonPitBoss;
	friend class CCommonDealer;

public:
	IMPORT_C ~CWorkerSession();
	IMPORT_C void ConstructL(TProcessId aProcessId);
	// Called by worker when it has received a SessionCloseRespMsg
	IMPORT_C void SessionCloseResp(CommsFW::TWorkerId aPlayerId);
	TBool IsPlayerInDisconnectList(CommsFW::TWorkerId aPlayerId) const;
	IMPORT_C TBool IsDisconnectListEmpty() const;

	void CleanupDeadWorker(CommsFW::TWorkerId aPeerId);
	void CompleteDisconnect();

	IMPORT_C static void PanicClient(const TDesC& aCategory, TInt aPanic);

	IMPORT_C void DontCompleteCurrentRequest();
	IMPORT_C void SetReturn(TInt aReturn /*,TInt anError=0,TUid anOwner=TUid::Null()*/);

	void Disconnect();
	// Override of default behaviour for session disconnection
	IMPORT_C void Disconnect(const RMessage2& aMessage);

	inline const TSessionUniqueId& UniqueId() const
		{
		return iSessionUniqueId;
		}

#ifdef SYMBIAN_TRACE_ENABLE
	TInt DisconnectPlayers();
#endif

	IMPORT_C static void LogSubSession(CWorkerSession* aSession, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);
		// Apply a callback function to subsessions of the session, either matching the worker id, or all
	// if aPeerId == ENullWorkerId
	typedef void (*TSubSessionProcessor)(CWorkerSession* aSession, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);
	IMPORT_C void ProcessSubSessions(CommsFW::TWorkerId aPeerId, TSubSessionProcessor aSubSessionProcessor, TAny* aArg);
	static void CountSubSessions(CWorkerSession* aSession, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);

//	[MZ]: Note, needs to be reimplemented properly:
//	IMPORT_C static void DebugParkRequestsL(CWorkerSession* aSelf, TInt aSubSessHandle, TInt aCount);
//	IMPORT_C static void DebugReleaseParkedRequests(CWorkerSession* aSelf);

protected:
	IMPORT_C CWorkerSession(TUidType aUid, TSessionUniqueId aSessionUniqueId);

	// Callbacks used with ProcessSubSessions()
	static void ForgetSubSession(CWorkerSession* aSession, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);
	static void OrphanSubSession(CWorkerSession* aSession, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);

	IMPORT_C void ForwardMessageL(const RMessage2& aMessage, CWorkerSubSession* aSS, CommsFW::TWorkerId aWorker);
	IMPORT_C void ForwardMessageL(const RMessage2& aMessage, CWorkerSubSession& aSubSess);

	IMPORT_C void OrphanSubSessions();
	IMPORT_C CWorkerSubSession* SubSessionFromHandle(TUint aHandle, TSubSessInfo aType) const;

	/* Disconnect handling: before processing EDisConnect message to destroy session
	the Players with subsessions in the session are given the opportunity to cleanup.
	Players are tracked in a dirty-bit manner: they're added to the disconnect list
	when first referenced by the session and never removed (too much bookwork) */
	void AddPlayerToDisconnectList(CommsFW::TWorkerId aPlayerId);
	void RemovePlayerFromDisconnectList(CommsFW::TWorkerId aPlayerId);

	IMPORT_C void ParkIfIndeterminateRequest(const RMessage2& aMessage, TInt aErrorIfConfigured);

	inline const RMessage2& Message() const;
	inline RSafeMessage& SafeMessage() const;
	inline RSafeMessage& SafeMessage(const RMessage2& aMessage) const;

protected:
	// API extension support
	IMPORT_C TIpcExtensionRoutingMessageFromSession* IsSessionIpcExtensionSupported(TInt aInterfaceId, TDes8& aBuff);
	IMPORT_C void OpenSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void SendReceiveSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void CloseSessionIpcExtension(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification);
	IMPORT_C void CloseAllSessionIpcExtensions();
	virtual TIpcExtensionRoutingMessageFromSession* DoIsSessionIpcExtensionSupported(TInt /*aInterfaceId*/, TDes8& /*aBuff*/)
		{
		//Not supported by default
		return NULL;
		}

public:
	IMPORT_C void GetOwnerInfo(TProcessId& aProcId, TUidType& aUidType) const;
	inline CCommonDealer* Dealer() const;
	inline CCommonWorkerThread& WorkerThread() const;
	inline CommsFW::TWorkerId WorkerId() const;
	inline CCommonPitBoss& PitBoss() const;
	inline CSubSessionIx& SubSessions();
	inline const CSubSessionIx& SubSessions() const;

protected:
	CSubSessionIx iSubSessions;	//< Container of subsessions belonging to this session. They can potentially belong to different Players.

	TBool iComplete;
	TInt  iReturn;

	const RMessage2* iOurMessage;

	RProcess iProcess;
	TUidType iUidType;

	// Session death
	TUint32 iDisconnectPlayers;	    // Set of Players to notify of session death
	RMessage2 iDisconnectMessage;	// EDisConnect Message to complete session destruction

private:
	TSessionUniqueId iSessionUniqueId; //Session unique id.
	TApiExtRegister iApiExtRegister;   //Register of opened interfaces (on this session only)
	};

class CCommonServer : public CPolicyServer
/**
class CCommonServer
@internalComponent
*/
	{
	friend class CWorkerSession; //to acess iNumSessions which should be removed along with this line

private:
	enum {EMaxServerNameLength=32};
	typedef TBuf<EMaxServerNameLength> TServerName;

public:
	enum {EPriority=0};

public:
	IMPORT_C ~CCommonServer();
	IMPORT_C TBool CanShutdown();

	inline CCommonWorkerThread& WorkerThread() const;
	inline CommsFW::TWorkerId WorkerId() const;

	TServerName& ServerName()
		{
		return iServerName;
		}

	void SetDealer(CCommonDealer* aDealer)
		{
		iDealer = aDealer;
		}

	CCommonDealer* Dealer()
		{
		return iDealer;
		}

	TDblQueIter<CSession2>& SessionIterator()
		{
		return iSessionIter;
		}

	static void FreeMemory(TAny*);

protected:
	IMPORT_C virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	virtual CSession2* DoNewSessionL(const TProcessId& aProcessId, const TUidType& aProcessUid) const = 0;
	virtual TVersion CurrentVersion() const = 0;

	void DoFreeMemory();
	IMPORT_C CCommonServer(TInt aPriority, CCommonWorkerThread* aOwnerThread, const TPolicy& aPolicy, TServerType aType, const TDesC& aName);

private:
	CCommonWorkerThread* iOwnerThread;
	CCommonDealer* iDealer;
	mutable TInt iNumSessions;	//counter of the sessions TODO - use the iSessionIter for that!!!
	TServerName iServerName;
	};


#include <elements/sd_std.inl>

} //namespace Den

#endif
//SYMBIAN_DEN_COMMONSTD_H


