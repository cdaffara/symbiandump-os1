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



// Defines global internals for C32. 

#ifndef CS_STD_H
#define CS_STD_H

/** @file
 *
 * @internalComponent
 */

#include <cs_port.h>
#include "COMMIPC.H"
#include <cfmsgs.h>
#include <cfshared.h>
#include <cfutil.h>
#include "cs_thread.h"	// CC32Workerthread, CCommChannelHandler
#include <e32cmn.h>				// RFastLock



#ifndef __WINS__
#define VERBOSE
#endif	// __WINS__

// time for player to close all owned sessions. Is sent in msg from dealer to player so that player can 
// determine if it should still process the message. If player doesn't see this msg until after timer 
// expires, player does not process msg. Also, dealer won't process response and close session if this 
// timer has expired before dealer sees response (I don't quite see why this needs to be the case? 
// Surely this is only a problem if session close delay has also expired? ). 8 secs is derived from 
// being "a long time" for this sort of operation.
const TUint32 KC32SessionCloseDelay = 16*1000*1000;
// time for session to clean up all subsessions from workers wrt this session. The client is normally 
// held inside their Close() call until c32 completes the close. Must be longer than player close deadline 
// since at expiration client is released and after that, csys accessing client memory will be dangerous. 
// Is 8 secs longer than session close player deadline since this is how long it takes some test CSYs to 
// perform their closure.
const TUint32 KC32SessionClosePlayerDeadline = 8*1000*1000;

// Short delay on asynchronous callback to allow for any others to run
// In practice, the yield itself is normally enough
const TInt KCommServerShutdownLatencyMicroSeconds = 100 * 1000;	

_LIT(KCommServer, "!CommServer");
_LIT(KCSYExtension, ".CSY" );
_LIT8(KCSYExtension8, ".CSY" );
_LIT8(KRoleLabel, "Role");
_LIT8(KDealerRole, "Dealer");
_LIT8(KPlayerRole, "Player");
//_LIT8(KDealerPlayerRole, "DealerPlayer");  // maybe one day
_LIT8(KWorkerIdLabel, "WorkerId");
_LIT8(KCSYListLabel, "CSYList");


void PanicClient(TInt aPanic, const RMessagePtr2& aMessage);
void SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode);
TInt AddCSYExtension(TFileName& aFilename, const RMessage2& aMessage);



/**
 * External representation of the state of a CPort object.
 */
struct TCommDebugInfo 
	{
	TInternalCommAccess iMode;  //< The current main access mode of the port
	TInt iAccessCount;          //< Number of open clients
	TCommRole iRole;            //< DCE or DTE
	TInt iOutstandingCommands;  //< Number of outstanding Opens, Closes, Waits etc.
						        //< (excludes SetAccess requests)
	};

class CCommSubSession;
class CC32Player;
class CPort;

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


NONSHARABLE_CLASS(CCommSubSession) : public CBase
/**
@class
CCommSubSession
@internalComponent
*/
	{
	friend class CCommSession;
	friend class CC32Player;
public:
	// Reference counting methods formerly supplied by CObject.
	inline void Open();
	inline void Close();
	inline TInt AccessCount() const;

#ifdef _DEBUG
	virtual ~CCommSubSession();
#endif

	inline CCommSession* Session() const;
	inline CC32Player& Player() const;

	/** 
		Type identifier for the derived sub-session objects. Used e.g. in containers to identify
		what is stored.

		@see CCommSubSession::Type()
	*/
	enum TSubSessionType 
		{ 
		ENull, 
		EAny, 
		ECPort 
		};

	TSubSessionType Type() const
		{
		return CCommSubSession::ECPort;
		}


	static CCommSubSession* NewL(CCommSession* aSession, CPort* aPort, CC32Player* aPlayer);

private:
	CCommSubSession(CCommSession* aSession, CPort* aPort, CC32Player* aPlayer);
	void RemoveAndDestroy();
private:
	CC32Player* iPlayer;	//< The Player responsible for this sub-session instance.
	TInt iAccessCount;		//< Reference counting on the sub-session. If it goes to 0 the sub-session will be destroyed.
	CPort* iPort;
	CCommSession* iSession;
	};

//
// CC32SubSessionIx - Subsession index
//

/**
@class
CC32SubSessionIx
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
    - Type	- 8 bit value. One of the members of CCommSubSession::TSubSessionType.
    - Magic	- 8 bit value. Ensures that handles are not re-used immediately. Each time an entry is re-cycled this value increments, rolling around when reaching max.
    - Index	- 16 bit value. The position in the array.
@see CCommSubSession::TSubSessionType
*/
NONSHARABLE_CLASS(CC32SubSessionIx) : public CBase
	{
public:
	typedef TInt TSubSessionHandle;
	enum { KTypeBits = 8, KMagicBits = 8, KIndexBits = 16 };
	enum { KIndexLimit = 1 << KIndexBits };
	enum { KMagicMask = (1 << KMagicBits) - 1 };
public:
	~CC32SubSessionIx();

	void InitialiseL();
	inline void Lock() const;
	inline void Unlock() const;
	
	CCommSubSession* At(TInt aHandle, CCommSubSession::TSubSessionType aType) const;
	TInt Find(CCommSubSession* aSubSession, TSubSessionHandle& aHandle) const;

	TInt Add(CCommSubSession* aSubSession, TSubSessionHandle& aHandle);
	CCommSubSession* Remove(TSubSessionHandle aHandle);

	NONSHARABLE_CLASS(TIter)
		{
	public:
		TIter(CC32SubSessionIx& aContainer);
		void SetToFirst();
		CCommSubSession* operator++(TInt);
		CCommSubSession* Next(TSubSessionHandle& aHandle);
	private:
		TInt iPos;
		CC32SubSessionIx& iContainer;
		};

	friend class TIter;
private:
	enum { EIndexGranularity = 8 };

	/** @class
	CC32SubSessionIx::TEntry
	One entry in the container. If iType is CCommSubSession::ENull, then it is a member of the free-list 
	and iNextFree is the index of the next free TEntry instance otherwise it is an iObject* and points 
	to a sub-session instance.
	iType can be one those defined in CCommSubSession::TSubSessionType.
	@see CCommSubSession::TSubSessionType
	*/
	class TEntry
		{
	public:
		union
			{
			CCommSubSession* iObject;
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
	TEntry* At(TSubSessionHandle aHandle) const;
	TInt ExpandArray();

private:
	class RWorkerLock : public RFastLock
		{
	public:
		// overriden functions
		inline void Wait();

		inline void Signal();
		// end overriden functions
		
		inline void AssertLockHeld() const;
		
		
#ifdef _DEBUG
	private:
		// RFastLock does not nest, ie if the current holder attempts to re-acquire they deadlock
		// We check for this in debug builds only as the cost of doing so likely negates the benefits
		// of using the class over & above semaphores
		TThreadId iHolder;
#endif
		};

private:
	TEntry* iIx;
	TInt iSize;
	TInt iActiveCount;			//< Number of active sub-sessions stored in this container.
	TInt iFreeListHead;			//< Pointer to the first item/cell in the virtual free list.
	mutable RWorkerLock iLock;
	};




/**
 * Main Comms Server object.  Constructed on process initialisation; never deleted.
 */
class CC32WorkerThread;
class CC32ThreadManager;
class CC32Server : public CPolicyServer
	{
public:
	enum {EPriority=1000}; //< priority of this Active Object
public:
	// from CServer
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	~CC32Server();
public:
	inline CC32WorkerThread& WorkerThread() const;
	inline CommsFW::TWorkerId WorkerId() const;
	
protected://virtuals from CPolicyServer
	CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
protected:
	CC32Server(CC32WorkerThread* aOwnerThread, TInt aPriority);
	void ConstructL(CC32ThreadManager* aThreadManager);

private:
	CC32WorkerThread* iOwnerThread;
	CC32ThreadManager* iThreadManager;
	TInt iThreadHandles;	

private:
	static const TUint iRangeCount;
	static const TInt iRanges[];
	static const TUint8 iElementsIndex[];
	static const CPolicyServer::TPolicyElement iElements[];
	static const CPolicyServer::TPolicy iPolicy;
	};


class CC32ThreadManager;
/**
 * A client session.
 * Corresponds to an RCommSession object in the client.
 */
class CCommSession : public CSession2
	{
	friend class CCommSubSession;
    friend class CC32Dealer;
	typedef CSession2 inherited;

public:
	static CCommSession* NewL(CC32ThreadManager* aThreadManager);
	virtual void ServiceL(const RMessage2 &aMessage);
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	virtual ~CCommSession();
	CCommSession(CC32ThreadManager* aThreadManager);
	void ConstructL();
	void LoadCommModuleL(const RMessage2& aMessage, CommsFW::TWorkerId aWorker, TBool defaulted, const TDesC& aFilename);
	void CloseCommModuleL(const RMessage2& aMessage);
	void NewPortL(const RMessage2& aMessage);

	void NumPorts(const RMessage2& aMessage);
	void PortInfo(const RMessage2& aMessage,TInt aPortNum);
	void PortInfoL(const RMessage2& aMessage,const TPortName& aPortName);
	TInt Write(TInt aPos, const RMessagePtr2& aMessage, const TDesC8& aDes, TInt aOffset=0);
	TInt Read(TInt aPos, const RMessagePtr2& aMessage, TDes8& aDes, TInt aOffset=0);
	TInt Write(TInt aPos, const RMessagePtr2& aMessage, const TDesC16& aDes, TInt aOffset=0);
	TInt Read(TInt aPos, const RMessagePtr2& aMessage, TDes16& aDes,TInt aOffset=0);
	TInt ExtractPortNameAndNumber(const RMessagePtr2& aMessage, TDes& aPortName, TUint& aPortNumber, TInt& aLength);
	
public:
	inline const CC32Dealer& Dealer() const;
	CC32WorkerThread& C32WorkerThread() const;
	CommsFW::TWorkerId WorkerId() const;
	inline CC32SubSessionIx& SubSessions();

	// RCommServ related APIs
	TInt AddCSYToSessionL(const TDesC& aCSYFileName, TBool& aIsDuplicate);		//< LoadCommModule
	TInt RemoveCSYFromSession(const TDesC& aCSYFileName, TBool& aIsLast);	//< used by UnLoadCommModule
	
	// Apply a callback function to subsessions of the session, either matching the worker id, or all
	// if aPeerId == ENullWorkerId
	typedef void (*TSubSessionProcessor)(CCommSession* aSession, CCommSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);
	void ProcessSubSessions(CommsFW::TWorkerId aPeerId, TSubSessionProcessor aSubSessionProcessor, TAny* aPtr);

	// Callbacks used with ProcessSubSessions()
	static void ForgetSubSession(CCommSession* aSession, CCommSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);
	static void CountSubSessions(CCommSession* aSession, CCommSubSession* aSubSession, TInt aSubSessionHandle, TAny* aArg);

	// Called by worker when it has received a SessionCloseRespMsg
	void SessionCloseResp(CommsFW::TWorkerId aPlayerId);

	typedef RPointerArray<HBufC> RCSYFileNameContainer; //< unlike ThreadManager, CSY names stored in 16-bit since in all cases they come in 16-bit
														//(ie, we never have to compare with ThreadManager CSY list, which is 8-bit since based from 8-bit ini file)
	
	void CompleteDisconnect();

	TBool IsPlayerInDisconnectList(CommsFW::TWorkerId aPlayerId) const;
	TBool IsDisconnectListEmpty() const;
	TInt DisconnectPlayers();

protected:
	void Disconnect();
	// Override of default behaviour for session disconnection
	virtual void Disconnect(const RMessage2& aMessage);

	void ForwardMessageL(const RMessage2& aMessage, CCommSubSession* aSS, CommsFW::TWorkerId aWorker);
	void ForwardMessageL(const RMessage2& aMessage, CCommSubSession& aSubSess);
	void SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode);
	void DontCompleteCurrentRequest();

private:
	CC32ThreadManager* iThreadManager;	//< ThreadManager for lookup, located in dealer
	CCommSubSession* SubSessionFromHandle(TUint aHandle, CCommSubSession::TSubSessionType aType) const;
	void CloseSubSessionL(const RMessage2& aMessage, CCommSubSession::TSubSessionType aType);

	void AddPlayerToDisconnectList(CommsFW::TWorkerId aPlayerId);
	void RemovePlayerFromDisconnectList(CommsFW::TWorkerId aPlayerId);
	
	/** 
	@class
	CCommSession::CC32SessionCloseTimer
	This timer times out after double the deadline given to the Players to return a Session Close response.
	*/
    NONSHARABLE_CLASS(CC32SessionCloseTimer) : public CTimer
    	{
	public:
	    static CC32SessionCloseTimer* NewL(CCommSession* aSession);
		void Start();
	protected:
	    void RunL();
	private:
	    CC32SessionCloseTimer(CCommSession* aSession);
	    CCommSession* iSession;
    	};

    /** Timer starting when SessionClosed message is sent to players.
	If not all the expected responses come back within the deadline set in the Session
	Close message, the CC32SessionCloseTimer will fire and the session will be deleted by 
	the Dealer. This is usually due to a Player somehow being unreliable and shouldn't 
	normally happen. The purpose of the timeout is to avoid blocking clients.
	*/
    CC32SessionCloseTimer* iSessionCloseTimer;
	TBool iComplete;				//< check if the current message can be completed
	CC32SubSessionIx iSubSessions;	//< Container of subsessions belonging to this session. 
									//< They can potentially belong to different Players.
	TUint32 iDisconnectPlayers;	    //< Set of Players to notify of session death	
	RMessage2 iDisconnectMessage;	//< EDisConnect Message to complete session destruction
	RCSYFileNameContainer iCsyCon;	//< CSYs loaded in this session, used to check on unload whether CSY was loaded. Duplicates allowed, order unimportant.
	};


/**
 * Active Scheduler class with specialised error handling.
 */
class CCommScheduler : public CActiveScheduler
	{
public:
	static CCommScheduler* New();
    void Error(TInt anError) const;
	};

/**
 * Owner object for all the CPort Objects.
 * There is one instance of this, owned by the CCommSession object.
 * It has no explicit destructor because the C32 thread will never finish unless panicked
 */
NONSHARABLE_CLASS (CPortManager) : public CBase
	{
public:
	static CPortManager* NewL();
	CSerial* LoadCommModuleL(const TDesC& aFileName);
	CSerial* GetSerialL(const TDesC& aName);
	CPort* GetPortL(const TDesC& aName, TUint aPort, CSerial *aSerial, TUint aMode, TUint aRole, CCommSession* aSession);
	TInt PortInfo(const TPortName& aName, TSerialInfo& aSerial);
	~CPortManager();

private:
	CObjectConIx* iContainer;
	CObjectCon* iPorts;
	CObjectCon* iProviders;

private:
	void ConstructL();

	};

#ifdef __MARM__
const TInt KCommTimerGranularity = 31000;
#else
const TInt KCommTimerGranularity = 80000;
#endif


#include "cs_std.inl"

#endif // CS_STD_H

