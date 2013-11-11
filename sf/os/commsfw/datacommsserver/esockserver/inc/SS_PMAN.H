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

#ifndef __SS_PMAN_H__
#define __SS_PMAN_H__

#include <es_prot.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_prot_internal.h>
#endif

#include <comms-infras/api_ext_msg.h>

#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpond.h>
#endif 

namespace ESock
{
class CSocket;
class CHostResolver;
class CServiceResolver;
class CNetDatabase;
}
class CSocketServer;
class CProtocolBase;
class CMBufManager;
class CSockSession;
struct TProtocolDesc;
class CProtocolRef;
class CMBufManager;
class CDeltaTimer;
class CESockIniData;

  //
  // Housekeeping structures used internally by the Protocol manager.
  //
class CProtocolFamilyBase;
class CProtocolBase;

typedef TSglQue<CProtocolRef> TProtocolRefList;
NONSHARABLE_CLASS(CProtocolFamilyRef) : public CBase
/**
@internalComponent
*/
	{
#ifdef __FLOG_ACTIVE
	friend class TProtocolManagerLogger;
#endif
public:
	enum TManagerProtFamilyFlags
		{
		EInstalled=0x00000001,
		};
	TSglQueLink iLink;
public:
	~CProtocolFamilyRef();
	static CProtocolFamilyRef* GetProtocolFamilyL(TDesC& aName);
	void LoadL();
	CProtocolFamilyBase& ProtocolFamilyL(TBool aInstall=ETrue);
	inline void Close();
	inline void FamilyClosed();
private:
	CProtocolFamilyRef();
private:
	TInt iFlags;
	CProtocolFamilyBase* iProtocolFamily;
	HBufC* iLibFileName;
	};

class Nif;
NONSHARABLE_CLASS(CProtocolRef) : public CBase
/**
@internalComponent
*/
	{
friend class Nif;
public:
	enum TManagerProtocolFlags
		{
		EInited=0x00000001,
		EStarted=0x00000002,
		EBound=0x00000004,
		EThreePlaneCommsBased=0x00000008,
		};
	TSglQueLink iLink;

public:
	~CProtocolRef();
	TBool Matches(TUint anAddrFamily,TUint aSocketType,TUint aProtocol);
	static CProtocolRef* GetProtocolL(const TDesC& aName);
	static CProtocolRef* MakeRefL(const TDesC& aName,CProtocolFamilyRef* aFamily=NULL,TServerProtocolDesc* info=NULL);
	static CProtocolRef* MakeRefL(const TDesC& aName, TServerProtocolDesc* info);
	inline void LoadAndBindL();
	CProtocolBase* Protocol();
	inline CProtocolFamilyBase* Family();
	TServerProtocolDesc& Info();
	inline void ProtocolClosed();
	inline void Close();
	inline TPtrC Tag();
	inline void SetFlag(TUint aFlag){iFlags=aFlag;};
	inline TUint GetFlag(){return iFlags;};

private:
	void StartL();
	void ResolveBindingL();
	void LoadL(TBool aLoadForBinding);
	CProtocolRef();
	void DoBindingL(const CESockIniData& anIniFile);
	void ProcessBindListL(TPtrC aBindList, const CESockIniData& aIniFile);
	static void Cleanup(TAny* aFamily);
	static CProtocolRef* GetProtocolNoCreate(TProtocolRefList* aProtocolList, const TDesC& aTag);

private:
	TUid iUid;
	HBufC* iTag;
	TUint iFlags;
	TServerProtocolDesc iInfo;
	CProtocolBase* iProtocol;
	CProtocolFamilyRef* iFamily;
	};

class SocketServer;
class CSocketServExtBase;
NONSHARABLE_CLASS(CSocketServExtRef) : public CBase
/**
@internalComponent
*/
	{
	friend class CSockManData;
public:
	CSocketServExtRef();
	virtual ~CSocketServExtRef();
	void InstallL(const TDesC& aDllName, const TDesC& aParam);
	void Remove();
	void Close();
private:
	TDblQueLink iExtLink;
	RLibrary iLibrary;
	CSocketServExtBase* iExtension;
	CAsyncCallBack* iDestructor;
public:
	inline CSocketServExtBase* Extension() { return iExtension; }
	};

/**
@internalComponent
*/
typedef CSocketServExtBase* (*TSocketServExtInstallFn)();

#if !defined (__WINS__)
#define VERBOSE
#endif

#if defined (VERBOSE)
#define VERBOSE_ONLY(x) x
#else
#define VERBOSE_ONLY(x)
#endif

class CPlayer;
struct TServerProtocolDesc;

namespace ESock
{
class CTransportFlowShim;
}

class CInternalSocketImpl;
class ProtocolManager
/**
@internalComponent
*/
	{
	friend class SocketServer;
	friend class ESock::CTransportFlowShim;
	friend class ESock::CSocket;
	friend class CPlayer;

public:
	static void LoadProtocolL(TUint anAddrFamily, TUint aSocketType, TUint aProtocol, CPlayer* aPlayer);
	static void UnLoadProtocolL(TUint anAddrFamily, TUint aSocketType, TUint aProtocol, CPlayer* aPlayer);
	static ESock::CHostResolver* NewHostResolverL(TUint anAddrFamily, TUint aSocketType, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	static ESock::CServiceResolver* NewServiceResolverL(TUint anAddrFamily, TUint aSocketType, TUint aProtocolId, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	static ESock::CNetDatabase* NewNetDatabaseL(TUint anAddrFamily, TUint aSocketType, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
	static TUint NumProtocols(void);
	static TInt ProtocolInfo(TUint anIndex,TProtocolDesc& aProtocol);
	static TInt ProtocolInfo(const TDesC& aName,TProtocolDesc& aProtocol);
	static TInt ProtocolInfo(const TDesC& aName,TServerProtocolDesc& aProtocol);
	static CProtocolBase* FindAndLoadProtocolL(const TDesC& aName, TProtocolType aType=ENormalProtocol);
	static CProtocolBase* FindAndLoadProtocolL(TUint aAddrFamily,TUint aSockType,TUint aProtocol);
	static void TransferSocketL(ESock::CSocket* aSocket, CPlayer* aPlayer);
	static TBool GetDependency(const TDesC& aTag,TPtrC& aList);
	static void ProcessIniDataL();
	static void InitL();
	static CInternalSocketImpl* NewInternalSocketL ( TAny* aParams, const TDesC& aName );
	static CInternalSocketImpl* NewInternalSocketL ( TAny* aParams, TUint aAddrFamily, TUint aSockType, TUint aProtocol );
private:
	static void ShutDown(void);
	static CProtocolRef* FindProtocolL(TInt anAddrFamily,TUint aSocketType,TInt aProtocol);
	static void CleanupProtocol(TAny* aProtocol);
	static void AddDependencyL(const TDesC& aDependentsList, const TDesC& aModuleToLoad);
	static void SafeCleanupProtocol(TAny* aProtocolRef);
	};

class CWorkerThread;
class SocketServer
/**
@internalComponent
*/
	{
public:
	static void InitL(CWorkerThread* aWorker);
	static void ShutDown(void);
	static RHeap* Heap();
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	static RCommsBufPond GetCommsBufPond();
#else
	static CMBufManager* GetMBufManager();
#endif
	static CDeltaTimer* GetTimer();
	static CSocketServer* GetSocketServer();
	static void NewSession();
	static void SessionClosing();
	static TBool IsShuttingDown();
	static void ShutdownExtensions();
	static void InstallExtensionL(const TDesC& aDllName, const TDesC& aArgs);
	static IMPORT_C void __DbgForceLoadMBufManagerL();
	static IMPORT_C void __DbgForceKillMBufManager();
	};

#ifdef __MARM__
//const TInt KTimerGranularity = 100000;

/**
@internalComponent
*/
const TInt KTimerGranularity = 15500;
#else
const TInt KTimerGranularity = 80000;
#endif

#ifdef __FLOG_ACTIVE
// Dumping out the set of loaded families and protocols aids debugging shutdown hangs
NONSHARABLE_CLASS(TProtocolManagerLogger)
	{
public:
	static void LogLoadedInfo();
	};
#endif


#include <ss_pman.inl>

#endif
