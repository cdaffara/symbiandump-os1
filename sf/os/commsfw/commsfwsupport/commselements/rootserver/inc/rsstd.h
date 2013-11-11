// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__RSSTD_H__)
#define __RSSTD_H__

#include <cflog.h>
#include <cfshared.h>
#include <commschan.h>
#include <rscommon.h>
#include <e32property.h>

class CRootServerSession;
class CRootServer;
class CBindManager;

NONSHARABLE_CLASS(CRSTransientModuleInfo) : public CBase
/** Helper class/structure used by the RCFThread.
@internalComponent
*/
	{
	public:
	~CRSTransientModuleInfo();
	CRSTransientModuleInfo(const CommsFW::RCFChannel::TMsgQueues& aTxQueues, const CommsFW::RCFChannel::TMsgQueues& aRxQueues,
						   RootServer::TRSHeapType aThreadHeapType, TThreadFunction aLibEntry, HBufC8* aIniDataOnRSHeap,
                           TThreadPriority aPriority, TUint32 aControlFlags);

	static TInt ModuleThreadStartup(TAny* aArg);

	private:
	CommsFW::TCFModuleInfo iCFModuleInfo;
	RootServer::TRSHeapType iThreadHeapType;
	TThreadFunction iLibEntry;
	HBufC8* iIniDataOnRSHeap;
	RHeap* iHeapPtr;
	TThreadPriority iPriority;
	TUint32 iControlFlags;
	};

/** MNotifySuddenDeath is used by CModuleUndertaker to notify registered
sessions of Sudden Death.
@internalComponent
*/
class MNotifySuddenDeath
    {
public:
    virtual void SuddenDeath(TInt aError)=0;
    };

class CNotifySuddenDeathContainer:public CBase
    {
public:
    CNotifySuddenDeathContainer(MNotifySuddenDeath* aNotifier);
    void Dequeue();
    void Notify(TInt aReason);
	TBool Match(MNotifySuddenDeath* aNotifier) const;
public:
    TDblQueLink iLink;			// DblQue link element
private:
    MNotifySuddenDeath* iNotifier; 	// Reference to notifier
    };

/** The CModuleUnderTaker class monitors a thread for sudden death. If it happens it will call
iNotifier->SuddenDeath().
@internalComponent
*/
NONSHARABLE_CLASS(CModuleUndertaker) : public CActive
    {
public:
    ~CModuleUndertaker();
    static CModuleUndertaker* NewL(MNotifySuddenDeath& iNotifier, RThread& aThread);
    void Logon();
protected:
    void DoCancel();
    void RunL();
    void LogThreadDeath();
private:
    CModuleUndertaker(MNotifySuddenDeath& iNotifier, RThread& aThread);
    MNotifySuddenDeath& iNotifier;
    RThread& iThread;
    };

/** This is the main module orchestrating thread creation, loading and unloading, The class
is asynchronously activated from three active objects: its own derivative, the
iModuleTimer * member and the iUndertaker member. The type of action taken on a particular
input is decided by its internal state, defined by iState.
@internalTechnology
*/
class CCommsProviderModule : public CActive, public MNotifySuddenDeath
    {
public:
    IMPORT_C ~CCommsProviderModule();

    TInt Load(TRequestStatus& aStatus);			// Start module in new thread
    TInt CancelLoad();
    TInt Unload();								// Unload module
    TInt CancelUnload(RootServer::TRSUnLoadType aType);
	RThread GetThread() const;
	inline const CommsFW::TCFModuleNameF& GetName() const;		// Return name
	inline const TFileName& GetDll() const;			// Return name of dll
	inline RootServer::TRSModuleState GetState() const;
#ifdef __CFLOG_ACTIVE
	const TText8* GetStateName() const;
#endif
#if defined _DEBUG || defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
	static const TText8* GetUnloadTypeName(RootServer::TRSUnLoadType aType);
#endif
	inline RHeap* GetHeapPtr() const;
	inline const CommsFW::RCFChannel& GetSendChannel() const;
	inline const CommsFW::RCFChannel& GetRecvChannel() const;
    IMPORT_C virtual void SuddenDeath(TInt aReason);   // Called by iUndertaker upon thread death
	inline TBool IsSticky() const;
	inline TBool IsServer() const;
#ifdef SYMBIAN_C32ROOT_API_V3
	inline TUint32 ControlFlags() const;
	inline void SetControlFlags(TUint32 aFlags);
#endif

    // Functions for handling Sudden Death notification
    void RegisterSuddenDeathNotifierL(MNotifySuddenDeath* aNotifier);
	void UnregisterSuddenDeathNotifier(MNotifySuddenDeath* aNotifier);

	// Failure to respond promptly converts it into half-dead zombie
	void Inhume();
    void CreateThreadL();
public:
   	TDblQueLink iLink;	// DblQue link element

protected:
    IMPORT_C CCommsProviderModule( RThread& aThread );
    IMPORT_C void ConstructL(CRootServer* aRootServer, const RootServer::TRSStartModuleParams& aParams, HBufC8* aIniData);

private:
    virtual TInt CreateModuleThread(const TDesC& aName, TThreadFunction aOtherThreadFunction,
						TInt aStackSize, RHeap* aHeap, TAny* aModuleArgs) = 0;
private:
    void NotifyOfDeath( TInt aReason );
    TBool UidMatch(const TUid& aUid) const;
    IMPORT_C void RunL();
    void DoCancel() {};
    RootServer::TRSModuleState iState;					// Current state of module (running, starting, etc)

protected:
	/** Whether RootServer should deny client shutdown requests.
	 */
	TBool iIsSticky;

	/** Whether module is being loaded as a server.
	 */
	TBool iIsServer;

	/** Additional module control flags
	    @see TRSStartModuleParamContainer::TControlFlags
	 */
	TUint32 iControlFlags;

    /** Saved for asynchronous stuff.
	 */
    TRequestStatus* iReqStatus;

    /** Name of module, defined by privileged app.
	 */
    CommsFW::TCFModuleNameF iName;

    /** Filename of DLL containing this module.
	 */
    TFileName iDll;

    /** Handle on the DLL, kept open as long as the module is active.
	 */
    RLibrary iLibrary;
    HBufC8* iIniData;

    /** Priority of thread.
	 */
    TThreadPriority iPriority;

    /** Stack size of thread.
	 */
    TInt iStackSize;

    /** Min heap size of thread.
	 */
    TInt iMinHeapSize;

    /** Max heap size of thread.
	 */
    TInt iMaxHeapSize;

    /** Ordinal for main threadfunction in dll.
	 */
	TInt iThreadFunctionOrdinal;

	/** EDefaultHeap, EShareHeap(with another loaded module), ENewHeap.
	 */
    RootServer::TRSHeapType iHeapType;

    /** If sharing a heap, then this is the module to share with.
	 */
    CommsFW::TCFModuleNameF iShareHeapWith;

    /** Thread handle of running module context.
	 */
    RThread& iThread;

    /** Active object listening for sudden death.
	 */
    CModuleUndertaker* iUndertaker;

    /** Access to the root server public functions.
	 */
    CRootServer* iRootServer;

    /** Sending comms channel.
	 */
	CommsFW::RCFChannel iSend;

	/** Receiving comms channel.
	 */
	CommsFW::RCFChannel iRecv;

	/** Registered sudden death notifiers.
	 */
	TDblQue<CNotifySuddenDeathContainer> iNotifiers;

	/** Exitreason when module has exited.
	 */
	TInt iExitReason;
    CRSTransientModuleInfo* iThreadStartupInfo;
    RHeap* iHeapPtr;
    };

class CRootServer : public CPolicyServer, public MRootServerNotify
/** The main class implementing the Rootserver.
@internalTechnology
*/
    {
public:
	IMPORT_C ~CRootServer();

    // Create new CRootServerSession instance
    IMPORT_C virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

    // Create comms provider module and put it in list
    CCommsProviderModule* CreateCpmL(const RootServer::TRSStartModuleParams& aParams, HBufC8* aIniData);

    RHeap* FindHeap( const CommsFW::TCFModuleNameF& aName );

    // Retrieve Server or Provider reference by name
    CCommsProviderModule* FindCpm(const CommsFW::TCFModuleNameF& aName);

    // Sudden Death notification. Called by Undertaker's. Call all members of iNotifier.
    IMPORT_C void SuddenDeath(TInt aReason);

    // Information, module count
    IMPORT_C TInt CpmCount();

    // Information, Enumeration
    TInt EnumerateModules(TInt aStartAt, CommsFW::TCFModuleNameF& aName);
    TInt EnumerateSubModules(const CommsFW::TCFModuleNameF& aModuleName, TInt aStartAt, CommsFW::TCFSubModuleNameF& aName);

    TInt GetModuleInfo(const CommsFW::TCFModuleNameF& aName, RootServer::TRSModuleInfo& aInfo);

	IMPORT_C virtual void LazyLoadL();
	IMPORT_C virtual void ReceivedMessage(const CommsFW::TCFModuleName& aModule,
							              const CommsFW::TCFMessage& aNewMsg);	// Notification of messages from modules
	CBindManager* BindManager() const;	// returns a pointer to the bind manager

	TInt Shutdown();


	TInt Forward(const CommsFW::TCFModuleNameF& aModule, const RMessage2& aMessage);

	IMPORT_C virtual TBool IsCallerConfigurator(const CRootServerSession* aSession, const RMessage2& aMessage);
	IMPORT_C virtual void DisconnectSession(const CRootServerSession* aSession);

	TUid iDeathKey;
	TUid iLeakKey;
	TUid iProcessKey;

protected:
    IMPORT_C CRootServer(TInt aPriority, const TPolicy& aPolicy); 	// Construction
    IMPORT_C void ConstructL(const TRootServerStartupInfo& aRootServerStartupInfo); 			// Second level construction

protected:
    TDblQue<CCommsProviderModule> iModules;
	CBindManager* iBindMgr;
	RProperty iDeathProperty;
	TInt iDeathCount;

	__CFLOG_DECLARATION_MEMBER;

	static const TUint PolicyRangeCount;
	static const TInt PolicyRanges[];
	static const CPolicyServer::TPolicyElement PolicyElements[];
	static const TUint8 PolicyElementsIndex[];
	static const CPolicyServer::TPolicy Policy;

    TCommsProviderModuleCtor iCommsProviderModuleCtorL;

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	CPerfMetricStore* iPerfStore;
#endif
    };

NONSHARABLE_CLASS(CSessionHandler) : public CActive, public MNotifySuddenDeath
/** Active session response object. Used when the session makes an asynchronous request to
e.g. Binding Manager. Has access to public members of it's CRootServerSession parent.
@internalComponent
*/
    {
public:
    CSessionHandler( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
    ~CSessionHandler();
    CCommsProviderModule* GetModulePtr( const CommsFW::TCFModuleNameF& aName ) const;
	virtual TInt Start() = 0;

	virtual TBool CancelHandler( TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable = EFalse ) = 0;
	TDblQueLink iLink;	// DblQue link element

	enum TSHType
		{
		ESHTypeLoadCPM = 0,
		ESHTypeUnLoadCPM = 1,
		ESHTypeBinding = 2,
		ESHTypeUnBinding = 3
		};
	virtual TSHType HandlerType() = 0;

protected:
	virtual void CompleteClientRequest( TInt aRequest );
	virtual void RunL()=0;
	virtual void DoCancel()=0;
	void SuddenDeath(TInt aError);
    enum TSHState
		{
		ESHIdle = 0,
		ESHStarting = 1,
		ESHLoading = 2,
		ESHUnLoading = 3,
		ESHBinding = 4,
		ESHUnBinding = 5,
		ESHKilling = 6,
		ESHCompleting = 7
		};

	TBool iRequestCompleted;
    CRootServerSession* iSession;
	CBindManager* iBindMgr;
	TSHState iState;
	RMessage2 iMessage;
    };

NONSHARABLE_CLASS(CSessionHandlerLoadCPM) : public CSessionHandler
/** Small class to handle a pending operation for loading a module.
@internalComponent
*/
	{
public:
	static CSessionHandlerLoadCPM* NewL( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	~CSessionHandlerLoadCPM();
	TInt Start();
	void SetHandler( const CommsFW::TCFModuleNameF& aModule );
	TBool CancelHandler( TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable = EFalse );
		
	CSessionHandler* MatchPending( const CommsFW::TCFModuleNameF& aModule );
	virtual TSHType HandlerType();

private:
	CSessionHandlerLoadCPM( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	void ConstructL();
	void CompleteClientRequest( TInt aRequest );
	void RunL();
	void DoCancel();
	void CompleteModalWaiter();
	void SuddenDeath(TInt aError);
private:
	CommsFW::TCFModuleNameF iName;
	CActiveSchedulerWait* iLoadCompletionNotificationWaiter;
	};

NONSHARABLE_CLASS(CSessionHandlerUnLoadCPM) : public CSessionHandler
/** Small class to handle a pending operation for unloading a module.
@internalComponent
*/
	{
public:
	static CSessionHandlerUnLoadCPM* NewL( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	~CSessionHandlerUnLoadCPM();
	TInt Start();
	void SetHandler( const CommsFW::TCFModuleNameF& aModule, RootServer::TRSUnLoadType aType );
	TBool CancelHandler( TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable = EFalse );
	CSessionHandler* MatchPending( const CommsFW::TCFModuleNameF& aModule );
	virtual TSHType HandlerType();

private:
	CSessionHandlerUnLoadCPM( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	void ConstructL();
	void CompleteClientRequest( TInt aRequest );
	void RunL();
	void DoCancel();
	void SuddenDeath(TInt aError);
private:
	CommsFW::TCFModuleNameF iName;
	RootServer::TRSUnLoadType iType;
	CActiveSchedulerWait* iLoadCompletionNotificationWaiter;
	};

NONSHARABLE_CLASS(CSessionHandlerBinding) : public CSessionHandler
/** Small class to handle a pending operation for forbinding a module.
@internalComponent
*/
	{
public:
	static CSessionHandlerBinding* NewL( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	TInt Start();
	void SetHandler(const CommsFW::TCFSubModuleAddress& aFrom,
					const CommsFW::TCFSubModuleAddress& aTo, RootServer::TRSBindType& aBindType,
					TInt aForwardQLength, TInt aReverseQLength);
	TBool CancelHandler( TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable = EFalse );
	CSessionHandler* MatchPending( const CommsFW::TCFSubModuleAddress& aFrom, const CommsFW::TCFSubModuleAddress& aTo );
	virtual TSHType HandlerType();

private:
	CSessionHandlerBinding( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	void RunL();
	void DoCancel();
private:
	RootServer::TRSBindType iBindType;
	CommsFW::TCFSubModuleAddress iNameFrom;
	CommsFW::TCFSubModuleAddress iNameTo;
	TInt iForwardQLength;
	TInt iReverseQLength;
	};

NONSHARABLE_CLASS(CSessionHandlerUnBinding) : public CSessionHandler
/** Small class to handle a pending operation for unloading a module
@internalComponent */
	{
public:
	static CSessionHandlerUnBinding* NewL( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	TInt Start();
	void SetHandler( const CommsFW::TCFSubModuleAddress& aFrom, const CommsFW::TCFSubModuleAddress& aTo );
	TBool CancelHandler( TBool aCompleteRequest, TBool aWaitForCompletionIfUncancelable = EFalse );
	CSessionHandler* MatchPending( const CommsFW::TCFSubModuleAddress& aFrom, const CommsFW::TCFSubModuleAddress& aTo );

private:
	CSessionHandlerUnBinding( CRootServerSession* aSession, CBindManager* aBindMgr, const RMessage2& aMessage );
	void RunL();
	void DoCancel();
	virtual TSHType HandlerType();

private:
	CommsFW::TCFSubModuleAddress iNameFrom;
	CommsFW::TCFSubModuleAddress iNameTo;
	};

class CRootServerSession : public CSession2, public MNotifySuddenDeath
/** Required session object as required by the client/server framework.
Implements the "Secure IPC" API.
@internalComponent
*/
    {
public:
	IMPORT_C CRootServerSession(const CRootServer* aRootServer);
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
    IMPORT_C void SuddenDeath(TInt aError); // As required by MNotifySuddenDeath

	void RegisterHandler( CSessionHandler& aHandler );

 	void CloseSession();
 	CRootServer* RootServer();

    RMessage2& GetMessage();
    IMPORT_C virtual ~CRootServerSession();
    
protected:
	IMPORT_C void Disconnect(const RMessage2& aMessage);    
    virtual TInt IsCallerConfigurator(const RMessage2& aMessage);    

    TInt LoadCPML(const RMessage2& aMessage);
    TInt UnloadCPML(const RMessage2& aMessage);

	TInt GetModuleInfo(const RMessage2& aMessage);
    TInt EnumerateModules(const RMessage2& aMessage);
    TInt EnumerateSubModules(const RMessage2& aMessage);
	TInt CancelLoadCPM(const RMessage2& aMessage);
	TInt CancelUnloadCPM(const RMessage2& aMessage);

	TInt Forward(const RMessage2& aMessage);

    TInt EnumerateBindings(const RMessage2& aMessage);
    TInt BindL(const RMessage2& aMessage);
    TInt UnbindL(const RMessage2& aMessage);
	TInt CancelBind(const RMessage2& aMessage);
	TInt CancelUnbind(const RMessage2& aMessage);

	TInt Shutdown();

protected:
    CRootServer* iRootServer;
private:
    RMessage2 iMessage;
    TBool iSuddenDeathListener;
    RMessage2 iSuddenDeathMessage;
    TBool iRequestActive;
    TDblQue<CSessionHandler> iActiveHandlers;

	//Enumeration
    TInt iEnumerateModulesIndex;
    TInt iEnumerateSubModulesIndex;
    TInt iEnumerateBindingsIndex;

	CommsFW::TCFSubModuleAddress iEnumBindings;
	CommsFW::TCFSubModuleNameF iEnumSubs;
    };

#include "rsstd.inl"

#endif // __RSSTD_H__

