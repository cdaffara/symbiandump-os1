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
// Defines the classes that describe the two roles in the system: dealer, player
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef CS_ROLES_H
#define CS_ROLES_H


#include <cfmsgs.h>
#include <cfshared.h>
#include <cfutil.h>
#include <e32cmn.h>
#include <c32comm.h>
#include "CS_STD.H"
#include "cs_thread.h"
#include "cs_msgs.h"
#include <comms-infras/c32startcli.h>

class CC32Player;
class CC32Dealer;
class CC32WorkerThread;

class TC32WorkerMainIntroductionRespMsg;



class CC32WorkerDataGlobals : public CommsFW::CWorkerThreadDataGlobals<TC32WorkerThreadRegister, KMaxWorkerThreadId>
	{
public:
	static CC32WorkerDataGlobals* NewL();

	inline CC32WorkerThread* WorkerThread(TWorkerId aWorker) const;
	};

NONSHARABLE_CLASS(CCSYInfo) : public CBase
// Store the information the dealer needs to know about a single CSY.
// These records are owned by the CC32ThreadManager
{
public:
	static CCSYInfo* NewL(const TDesC8& aCSYName, TInt aThreadNum);
    static TInt CompareCSYInfo(const CCSYInfo& aFirst,const CCSYInfo& aSecond);	// supplied to search functions
	~CCSYInfo();
	inline void SetWorkerId(TInt aThreadNum);
	inline TInt WorkerId() const;
	inline const HBufC8* GetCSYName()  const; // guaranteed to be non-null if NewL called.
	void SetCSYHandle(CSerial* aSerialHandle);
	inline CSerial* GetCSYHandle() const;
	void SetPortName(const TDesC& aPortName);
	TInt SetPortNameSizeToMaximum();
	inline TPtrC GetPortName() const;
	inline void IncAccessCount();
	inline void DecAccessCount();
	inline TInt AccessCount() const;
	inline TBool IsLoaded() const;
	inline void SetLoadState(const TBool aLoaded);
	inline void SetLoadIndex(TInt aIndex);
	inline TInt LoadIndex() const;
		
private:
	CCSYInfo(TInt aThreadNum);
	void ConstructL(const TDesC8& aCSYName);
private:	
	HBufC8* iCSYName;	//< Name of file to load CSY. Set by CMI file
	RBuf iCSYPortPrefix; 	// CSY Internal Name. Set on CSY load by dealer when load success msg recvd from player.
	                        // needs to accomodate TFullName length strings even though c32 has TPortName type since old C32
	                        // didn't police length and so existing csys may accept a long portname as its portname.
	CSerial* iCSYHandle; //< Set when CSY is loaded so we can transpose the index number the client supplies from the above GlobalIndexNumber to the local index inside the thread the CSY lives in.
	TWorkerId iThreadNum; //< Thread that this CSY will live in
	TBool iLoaded;			//< State of CSY whether loaded or not
	TInt iAccessCount;		//< Reference Counting for CSY
	TInt iLoadIndex;       // valid when csy is loaded. Supports index order of csys for getportInfo(index)
};




NONSHARABLE_CLASS(CC32ThreadManager) : public CBase
// Manages the Dealer's primary data structure which it uses to help it forward messages. ThreadManager stores
// information about the threads not covered by the workerThreadRegister including module name and which CSYs
// it should contain. ThreadManager also tracks the loading of CSYs.
// It does not track the running state of threads since the PeerReachable facility of the transport and
// the WorkerPresent facility of the global data covers this case.
{
	friend class CCommSession;
public:
	~CC32ThreadManager();
    static CC32ThreadManager* NewL(CC32Dealer* aDealer);
	void LoadCMIDataL(const TDesC8& aCSYList);
	void SetDefaults();
	

    // START - lookup functions
	TBool FindThreadByPortPrefix(const TDesC& aPortName, TWorkerId& aWorker) const;
	TInt MapPortprefixToCSYFileName(const TDesC& aPortName, TDes& aCSYFileName); //< mapping of PortPrefix to CSYFileName
	TBool FindThreadByFileName(const TDesC8& aCSYFileName, TWorkerId &aWorkerId) const;
	TBool FindThreadOfActiveCSYByName(const TDesC& aName, TWorkerId& aWorkerId) const;	//< RCommServ::GetPortInfo(aName) version, TWorkerId filled with worker
	TInt FindThreadByGlobalIndex(TInt aIndex) const; // must use TInt rather than TWorkerID so error code can be returned if necessary

	CSerial* FindSerialObjectByGlobalIndex(const TInt aIndex) const;
	const HBufC8* GetModuleName(TWorkerId aWorkerId);
	CSerial* GetSerialObjectFromCSYFileName(const TDesC& aCSYFileName);
	TInt GetIndexFromPortPrefix(const TDesC& aPortName, TInt& aIndex);
	CSerial* GetSerialObjectByIndex(TInt aIndex);
	CSerial* GetSerialL(const TDesC& aPortName);
	// END - lookup functions
	
	TInt IncrementCountOnLoad(const TDesC& aCSYFileName);	 //< increments iAccessCount in CCSYInfo on every Load request
	TInt DecrementCountOnCSYUnLoad(const TDesC& aCSYFileName);	//< decrements iAccessCount in CCSYInfo on every CSY UnLoad request (can be explicit UnLoadCommModule or implicit unload of CSY on session close)
	TInt GrowCSYPortNameToMaximumIfNotLoaded(const TDesC& aCSYFileName);  // so that biggest possible string exists before we begin load
	
	TWorkerId DefaultThread();


	void UpdateThreadManagerOnCSYLoad(const TDesC& aCSYFileName, const TDesC& aPortPrefix, CSerial* aSerial);
	void UpdateThreadManagerOnCSYUnload(const TDesC8& aCSYFileName8);
	void UpdateThreadManagerOnCSYLoadFailure(const TDesC& aCSYFilename);	
	CC32Dealer* Dealer();
	
	TInt RequestLoadModule(CommsFW::TWorkerId aWorker);
	
    void ProcessModuleLoadFailed(CommsFW::TWorkerId aWorker);
	void ProcessModuleLoadSuccess(CommsFW::TWorkerId aWorker);

private:


	NONSHARABLE_CLASS(CC32ThreadInfo) : public CBase
		{
	public:
		static CC32ThreadInfo* NewL(const TDesC8& aModuleName);
		static TBool CompareThreadInfo(const CC32ThreadInfo& aFirst,const CC32ThreadInfo& aSecond);
		void SetModuleNameL(const TDesC8& aModuleName); // set possibly never used
		inline const HBufC8* ModuleName() const;
		~CC32ThreadInfo();
	private:
		CC32ThreadInfo();
		void ConstructL(const TDesC8& aModuleName);
	private:
		HBufC8*	iModuleName;
		};
		
	
	NONSHARABLE_CLASS(CC32ParsedIniData) : public CBase
		{
		friend class CC32ThreadManager;
	public:
		static CC32ParsedIniData* NewL(const TDesC8& aIniData, const TBool aIsDealer);
		void ConstructL(const TDesC8& aIniData, const TBool aIsDealer);
		inline TBool IsDefaultThread() const;
		inline TBool IsPlayer() const; 
		inline TInt WorkerId() const;
		inline void SetWorkerId(const TInt aThreadNum);
		inline TInt NumCSYs() const;
		~CC32ParsedIniData();
	private:
		CC32ParsedIniData();
		void DigestIniDataL(TDes8& aIniData);
	private:
		TBool iIsDefault; // true if CSYList includes an asterix (*).
		TBool iIsPlayer;  // true if "Role=Player" was found
		TInt iCMIWorkerId;
		RArray<TPtrC8> iCSYs; // these point to the data in the supplied iniData
		};

	NONSHARABLE_CLASS(CCPMLoader) : public CActive
	// single class to track loading of a CPM
	// exists from when request is first made to when either Worker communicates with Dealer, or load timer expires
	// Player loading is a highly rare occurance.
		{
	public:
		static CCPMLoader* NewL();
		~CCPMLoader();
		void ConstructL();
		void RequestModuleLoad(CC32ThreadManager* aThreadMgr, CommsFW::TWorkerId aWorker, const TCFModuleName& aName); 
	private:
		void RunL();
		void DoCancel();
		CCPMLoader();
		TCFModuleName iName; // keep a copy of string we send since server may read long after we make request
		RRsConfigurator iCfgSvr;
		CC32ThreadManager* iThreadManager; //use thread manager pointer & workerid to update TM if load fails
		TWorkerId iWorkerId;		
		};


private:
	CC32ThreadManager(CC32Dealer* aDealer);
#ifdef __FLOG_ACTIVE
	void DumpThreadInfoAndCSYLists();
#endif
private:   
	RPointerArray< CCSYInfo > iCSYList; //< list of all csys we know about, loaded or otherwise

	RPointerArray<CC32ThreadInfo> iC32ThreadList; //< Index is the thread number, value is module name of thread, and a few other things
	TWorkerId iDefaultThreadIndex;	//< For finding where to create unknown CSYS
	CCPMLoader* iCPMLoader[KMaxWorkerThreadId+1];  // AOs that start load via configurator of new cpm. soaks up response
	CC32Dealer* iDealer;   			// used for CPMLoader and debug printing

	TInt iNumLoadedModules;  // used to calculate loadIndex for a CSY when CSY first loaded
};


/** 
@class
CC32Dealer
The Dealer is an implementation of the Symbian OS server (CServer2) and as such one of its most 
important possessions is the list of CSession2's kept by the CServer2. On a conceptual level 
one can say that the Dealer owns the CCommSessions due to their inherited relationship.

The Dealer thread must be started by the Configurator before any worker threads. The C32
instance loading it must be the only one having the WorkerId 0 (TC32WorkerThreadPublicInfo::EMainThread).
The Dealer has two main responsibilities:
    -# Maintain global data structures, accessible to all Workers to which the Dealer pointer is published 
    (through the TWorkerMainIntroductionMsg). Mostly the workers will access this data using Dealer access 
    functions which are thread-safe [AddSubSession() and RemoveSubSession()], taking special measures
    to guarantee this where needed.
    -# Manage Worker thread : Registering Workers when they start.

All C32 thread instances can have their own heap if set through CMI file.
Sometimes it happens that one Worker needs to do 
something that might cause allocation or freeing on another workers heap, such as inserting a 
sub-session into a Dealers session. So the Dealer offers a function that can determine whether 
a different heap is needed (maybe the two workers have different heaps, maybe they share one heap)
and does the switch.

Sub-sessions are also supported in the form of the CCommSubSession.
These are owned by the provider container (CC32SubSessionIx) in the CCommSession.

It also has AddSubSession() and RemoveSubSession() methods used by the Player, assuring this 
happens on the right heap.
*/

NONSHARABLE_CLASS(CC32Dealer) : public CC32Server
	{
	typedef CC32Server inherited;
public:
	static CC32Dealer* NewL(CC32WorkerThread* aOwnerThread, const TDesC8& aDealerIniData);
	~CC32Dealer();

	const TDesC& ServerName() const;
	void DeleteSession(CCommSession* aSession);
	TInt SubsessionCountInPlayer(CommsFW::TWorkerId aPeerId);
	TBool CanShutdown();
	TBool CanUnbindFromWorker(TWorkerId aWorker);
	void ProcessShutdownRequest(CommsFW::TCFShutdownType aType);
	TInt Read(TInt aPos, const RMessagePtr2& aMessage , TDes16& aDes, TInt aOffset);
	void ProcessFailedPlayerLoad(CommsFW::TWorkerId aWorker);
	void ShutdownIfReady();
	
	inline void SetImmediateShutdownPresent();
	inline TBool TestImmediateShutdownPresent() const;
	inline void StartShutdown();
	inline TBool StartupFailed() const;


public:
	void SendIntroductionToWorker(CommsFW::TWorkerId aWorkerId);
	void ProcessWorkerIntroductionResponse(const TC32WorkerMainIntroductionRespMsg& aMsg);
	void DeleteCPMLoader(CommsFW::TWorkerId aWorkerId);	

	void ProcessLoadCommModuleSuccessResponse(const RMessage2& aMessage, CSerial* aSerial);
	void ProcessLoadCommModuleFailureResponse(const RMessage2& aMessage, TInt aFailReason);	
	
	void SessionShutdownComplete();
	
	TBool WorkerExists(TWorkerId aId) const;
	inline TBool WorkerDataGlobalsExist() const;
    inline CC32WorkerDataGlobals& WorkerDataGlobals() const;
    
    void PostMessage(CommsFW::TWorkerId aWorkerId, CommsFW::TCFMessage& aMessage);
	CC32Dealer* Dealer(CommsFW::TWorkerId aWorkerId) const;
	CC32Player* Player(CommsFW::TWorkerId aWorkerId) const;

	RAllocator* MaybeSwitchHeap(CommsFW::TWorkerId aForeignWorkerId);
#ifdef _DEBUG
 	void SetFailNextForAllHeaps(TInt aFailNext);
#endif

	// SubSession container (CC32SubSessionIx) manipulation - used by Player
	TInt AddSubSession(CCommSubSession* aSubSession, CCommSession* aSession, TInt& aHandle);
	void RemoveSubSession(TInt aHandle, CCommSession* aSession);
	TInt FindSubSession(CCommSubSession* aSubSession, CCommSession* aSession, TInt& aHandle);
	TInt LoadCPMOnLoadCommModule(CommsFW::TWorkerId aWorker);
	void ProcessParkedRequest();
	void RemoveParkedRequestsOnSessionClose(CCommSession* aSession);
	TInt ParkRequest(CCommSession* aSession, const RMessage2& aMessage);
	CC32ThreadManager* ThreadManager() const
		{
		return iThreadManager;
		}
	
protected:
	CC32Dealer(CC32WorkerThread* aOwnerThread);
	void ConstructL(const TDesC8& aDealerIniData);
	
	void ProcessSubSessions(TWorkerId aPeerId, CCommSession::TSubSessionProcessor aSubSessionProcessor, TAny* aPtr);
	 
protected:
	class TSessionMessagePair
		{
	public:
		TSessionMessagePair(CCommSession* aSession, const RMessage2& aMessage)
		: iSession(aSession),
		  iMessage(aMessage)
	 		{
	 		}
	public:
		CCommSession* iSession;
		RMessage2 iMessage;
	 	};
	typedef RArray<TSessionMessagePair> RParkedRequestArray;
	RParkedRequestArray iParkedRequests;

private:
	void FreeWorkerReferences(TWorkerId aWorkerId);
private:
	CC32WorkerThread* iOwnerThread;
	CC32WorkerDataGlobals* iWorkerDataGlobals;
	CC32ThreadManager* iThreadManager;
		
	void AddPendingIntroductionResponse();
	void RemovePendingIntroductionResponse();

	/** 
	Incremented when a request to bind to another C32 instance has been received and decremented when a
	binding is complete.
	When the reference configurator configuration level have reached the point signifying that all core 
	components have been configured and this TInt is 0, it is safe to delete data structures used only during
	startup.
	*/
	TInt iPendingIntroResponses;

	/** 
	ETrue if the Dealer  is in the process of shutting down. Set when optional or graceful shutdown 
	is received by C32_Main from the RootServer. 
	*/

	/** 
	Set ETrue when immediate shutdown is received by the dealer (acting as pitboss) from the RootServer or if 
	optional/graceful shutdown has been received at some point and the last session is closed. 
	*/
	TBool iSessionShutdownComplete;

	/** 
	Set ETrue when immediate shutdown is received from the RootServer. 
	*/
	TBool iImmediateShutdownMark;

	/**
	If we are unable to start to a reliable state, mark ourselves as such and don't start the server.
	Such a scenario may be low memory during startup. This allows the phone to at least continue
	booting and we just sit lame until the next boot.
	*/
	TBool iFailedStartup;

	};



/** 
@class
CC32Player
The main responsibility of the Player is receiving the provider operations/queries 
from the Dealer and serving them using the protocol plug-ins. 
It is also responsible for adding/removing sub-sessions from the provider container 
in the related session object as well as keeping a list of all sub-sessions handled 
by the Player instance (iSubSessions).
*/
NONSHARABLE_CLASS(CC32Player) : public CBase
	{
public:
	static CC32Player* NewL(CC32WorkerThread* aOwnerThread);
	~CC32Player();

	void ProcessMessageL(const RMessage2& aMsg, CCommSubSession* aSubSession);

	inline CC32WorkerThread& WorkerThread() const;
	inline TWorkerId WorkerId() const;
	inline CC32Dealer& CC32DealerByRef() const;
	
	typedef RPointerArray<CCommSubSession> TSubSessionContainer;
	inline TSubSessionContainer& SubSessions();

	TBool CanUnbindFromWorker(TWorkerId aWorker);

	void MaybeSetPlayerShutdownComplete(TBool aForceShutdownNow);
	void ProcessShutdownRequest(CommsFW::TCFShutdownType aType);

	void CloseSession(CCommSession* aSession);

	void DontCompleteCurrentRequest();
	void SafeComplete(const RMessagePtr2& aMessage, TInt aCompletionCode);
	
	inline const RMessage2& Message() const;
	void CloseAllOwnedSubSessions(CCommSession* aSession);
	void ProcessUnLoadCommModuleMsg(CSerial* aSerial);

//
//	CC32Player functions specific to C32 APIs
//
	void LoadCommModule(const RMessage2& aMessage);
	void CloseCommModule(const RMessage2& aMessage);
	void NewPortL(const RMessage2& aMessage);
	TInt ExtractPortNameAndNumber(const RMessagePtr2& aMessage, TDes& aPortName, TUint& aPortNumber, TInt& aLength);	//< duplicate function as in CCommSession
	void PortInfo(const RMessage2& aMessage, CSerial* aSerialPtr);
	void PortInfo(const RMessage2& aMessage,const TPortName& aPortName);	//< msg is forwarded to player owning CSY which writes the TSerialInfo back to client using Write() API in player
	CPort* CPortFromSubSession(CCommSubSession* aSubSession);

	TInt Write(TInt aPos, const RMessagePtr2& aMessage, const TDesC8& aDes, TInt aOffset=0);
	TInt Read(TInt aPos, const RMessagePtr2& aMessage, TDes8& aDes, TInt aOffset=0);
	TInt Write(TInt aPos, const RMessagePtr2& aMessage, const TDesC16& aDes, TInt aOffset=0);
	TInt Read(TInt aPos, const RMessagePtr2& aMessage, TDes16& aDes,TInt aOffset=0);
	
private:
	CC32Player(CC32WorkerThread* aOwnerThread);
	void ConstructL();

	TInt WriteSubSessionHandle(TInt aHandle);
	void CloseSubSession(const RMessage2& aMessage, CCommSubSession* aSubSession);

private:
	CC32WorkerThread* iOwnerThread;					//< Thread owning this Player
	TBool iComplete;								//< check if current message can be completed
	CCommSession* iSession;							//< Current session, which might possibly belong to Dealer in different thread - only valid within current ProcessL().
	const RMessage2* iCurrentMessage;				//< Current RMessage - only valid within current ProcessMessageL().
	TSubSessionContainer iSubSessions;				//< All sub-sessions belonging to this Player.
	CPortManager* iPortManager;						//< CPortManager in this player
	};

#include "cs_roles.inl"


#endif // CS_ROLES_H


