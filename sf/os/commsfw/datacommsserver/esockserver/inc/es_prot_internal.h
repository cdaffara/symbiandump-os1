// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalTechnology
*/

#ifndef __ES_PROT_INTERNAL_H__
#define __ES_PROT_INTERNAL_H__

#include <e32base.h>
#include <es_sock.h>

/**
@internalComponent
*/
enum TConnStartType 
	{
	EConnStartExplicit,
	EConnStartImplicit
	};

/**
@internalComponent
*/
enum TESOCKActivePriority
	{
	ESSExclusivePriority=100000,
	ECommReceivePriority=200,		//< Must always run.
	ECommTransmitPriority=100,
	ECAsyncImmediatePriority=75,	//< I.e. before a timer event goes off.
	ESocketTimerPriority=50,
	EActiveIpcPriority=20,
	EActiveHighPriority=10,
	EActiveMedPriority=0,
	EActiveLowPriority=-10,
	ECAsyncDeferredPriority=-20,
	EShutDownPriority=-100000,		//< Lowest priority for shutdown, so that other async cleanup behaviours can complete first
	};

/**
@internalComponent
*/
enum TProtocolServiceInfo
	{
	ESocketSupport=0x00000001,		//< Can make sockets
	ETransport=0x00000002,			//< Can act as a transport - ie can be bound to
	EPreferMBufChains=0x00000004,	//< Will process MBufChains (I.e. won't convert them to descriptors)
	EPreferDescriptors=0x00000008,	//< Will process Descriptors (I.e. won't convert them to MBufChains)
	ECantProcessMBufChains=0x00000010,	//< Will Panic if asked to process an MBufChain
	ENeedMBufs=0x00000020,			//< Uses MBufChains internally.
	EUseCanSend=0x00000040,			//< Uses the inter-protocol CanSend upcall.
	EInterface=0x00000080,			//< Is a CInterfaceBase rather than CProtocolBase
	};


/**
@internalTechnology
*/
enum TProtocolType
	{
	ENormalProtocol,				//< Normal protocol
	EInterfaceProtocol				//< Has EInterface flags set and supports GetBinderL()
	};

/** Ensure that the requested length for datagrams is bigger than any real one. This ensures that
naive providers deliver them without truncation, so allowing the client to perform continuation reads.
@internalTechnology
*/
const TUint KGetDataWholeDatagram = 0x40000000;


/**
Definition of argument to CConnectionProvdBase::SetUsageProfile()
Bitmap masks

@internalTechnology
*/
const TUint KConnProfileNone = 0;


/**
Definition of argument to CConnectionProvdBase::SetUsageProfile()
Bitmap masks

@internalTechnology
*/
const TUint KConnProfileLong = 1;

/**
Definition of argument to CConnectionProvdBase::SetUsageProfile()
Bitmap masks

@internalTechnology
*/
const TUint KConnProfileMedium = 2;


class SocketTimer 
/**
@internalComponent
*/
	{
public:
	IMPORT_C static void Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TDeltaTimerEntry& aTimer);
	IMPORT_C static void Remove(TDeltaTimerEntry& aTimer);
	};

class CProtocolBase;
class SocketServExt
/**
@internalTechnology
*/
    {
public:
	IMPORT_C static CProtocolBase* FindAndLoadProtocolL(const TDesC& aName, TProtocolType aType=ENormalProtocol);
	IMPORT_C static CProtocolBase* FindAndLoadProtocolL(TUint aAddrFamily, TUint aSockType, TUint aProtocol);
	IMPORT_C static void InstallExtensionL(const TDesC& aDllName, const TDesC& aArgs);
	IMPORT_C static void OpenSession();
	IMPORT_C static void CloseSession();
	IMPORT_C static void InstallSchedulerWaitHook(TCallBack* aCall);
	};

class CSocketServExtRef;
class CSocketServExtBase : public CBase
/**
Base class for extensions

@internalTechnology
*/
	{
public:
	IMPORT_C virtual ~CSocketServExtBase();
protected:
	IMPORT_C CSocketServExtBase();
public:	
	virtual void InstallL(const TDesC& aArgs)=0;
	virtual void Remove()=0;
	//
private:
	friend class CSocketServExtRef;
	CSocketServExtRef* iExtRef;
	};


/**
KSoOwnerInfo

Definition for internal SetOption() from ESock to Provider to communicate
socket owner information.

@internalTechnology
@released
*/
const TInt KSOLProvider = 2;

const TUint KSoOwnerInfo = 1 | KSocketInternalOptionBit;

/**
KSoConnectionInfo

Definition for internal SetOption() from ESock to Provider to communicate
connection selection information.  Note that the information is opaque
to ESock and comes from Nifman in the form of a TSoIfConnectionInfo class.
@internalTechnology
*/
const TInt KSoConnectionInfo = 2 | KSocketInternalOptionBit;

#ifdef SYMBIAN_NETWORKING_UPS
/**
KSoGetErrorCode

Definition for internal GetOption() from ESock to Host Resolver Provider to retrieve
any specific error code needed to error the subsession with in certain
circumstances.
@internalTechnology
*/
const TUint KSoGetErrorCode = 3 | KSocketInternalOptionBit;

/**
KSoSetPlatSecApi

Definition for internal SetOption() from ESock to Socket/Host Resolver Provider to send
a class instance that can be used to dynamically retrieve thread and process id information.
@internalTechnology
*/
const TUint KSoSetPlatSecApi = 4 | KSocketInternalOptionBit;
#endif //SYMBIAN_NETWORKING_UPS

/**
UID of Unicode Protocol Modules

@internalTechnology
*/
const TInt KUidUnicodeProtocolModule = 0x10003d38;

/** @internalTechnology */
const TInt KUidEsockExtension = 0x10000387;


#endif // __ES_PROT_INTERNAL_H__

