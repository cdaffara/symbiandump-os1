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
// Socket server global (TLS) data
// Structure of the pseudo statics stored in Dll local storage.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __SS_GLOB_H__
#define __SS_GLOB_H__

#include <comms-infras/ss_common.h>
#include <elements/cftransport.h> // TCookie
#include <ss_pman.h>
#include <comms-infras/ss_factorycontainermap.h>
#include <comms-infras/cfperfmetrics.h>
#include <comms-infras/ss_apiext_register.h>

class CWorkerThread;
class CPlayer;

class CSocketServer;
class CMBufManager;

namespace ESock
{

class CCommsFactoryBase;
class CConnectionFactoryContainer;
class CSubConnectionFactoryContainer;
class CSubConnectionFlowFactoryContainer;
class CProtocolFamilyFactoryContainer;
class CProtocolIntfFactoryContainer;
class CConnectionServer;

enum EFactoryType
	{
	EProtocolFamilyFactory = 1,
	ESubConnectionFactory
	};

class CMetaConnectionFactoryContainer;
class CTierManagerFactoryContainer;
class CCFFactoryContainerBroker;
}

class CSockManData : public CBase
/**
@internalTechnology
*/
	{
public:
	// Some accessor functions to try to reduce the coupling
	IMPORT_C CPlayer* SelfPlayer();
	const CESockIniData* IniData();
	IMPORT_C CWorkerThread* SelfWorker() const;

#ifdef ESOCK_HOME_THREAD_CHECK_ENABLED
	void AssertOwnThread() const;
#endif

#ifdef _DEBUG
	void LogActiveProtocols();
#endif

public:
//TODO Move some of this stuff to Den::Globals?
	CWorkerThread* iWorkerThread;			//
	CSocketServer* iSocketServer;			//< Back pointer to the CServer (for shutdown)
	TProtocolRefList* iProtocols;			//< List of all socket types available
	TSglQue<CProtocolFamilyRef>* iProtocolFamilies;	//< List of all protocol families used
	CESockIniData* iEskData;
	TUint iNumProtocols;					//< Count of protocols.
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	RCommsBufPond iCommsBufPond;			//< The Comms buf pond
#else
	 CMBufManager* iMBufManager;             //< The MBuf manager
#endif
	CDeltaTimer* iTimer;					//< The global delta timer.
	TInt iNumFamilies;
	TBool iShutdownGracefully;              //< Flag that shutdown should be done when there's no more sessions or loaded families.
	HBufC *iDeps;							//< List of protocols requiring other protocols to be loaded first
	Messages::CGlobals& iTransportGlobals;

public:
	IMPORT_C static ESock::CCommsFactoryBase* InstallFactoryL( ESock::EFactoryType aFactoryType, const TDesC8& aName, TUid aFactoryUid );
	IMPORT_C Messages::TNodeId GetPlaneFC(const Den::TPlayerRole& aPlane);

	TBool ShutdownGracefully();
	void InstallFactoryContainersL();
	void UninstallFactoryContainers();

	ESock::CTierManagerFactoryContainer* iTierManagerFactories;
	ESock::CMetaConnectionFactoryContainer* iMetaConnectionFactories;
	ESock::CConnectionFactoryContainer* iConnectionFactories; // Connection Provider factories
	ESock::CSubConnectionFactoryContainer* iSubConnectionFactories; //< Sub-Connection Provider factories
	ESock::CSubConnectionFlowFactoryContainer* iSubConnectionFlowFactories; //< Sub-Connection Flow factories
	ESock::CProtocolFamilyFactoryContainer* iProtocolFamilyFactories; //< ProtocolFamily Provider factories
	ESock::CProtocolIntfFactoryContainer* iProtocolIntfFactories;

	ESock::RPlaneFCMap iFCMap;

	//Register of api extensions opened in this thread.
	//This register does not belong with the interfaces because their lifetime is implementation (or host) specific
	//This register also does not belong with the sessions, because they can disappear at any time (plus it would
	//not be possible to access the count of opened interfaces)
	ESock::TCommsApiExtRegister iCommsApiExtRegister;

	Messages::TNodeId iCommsFactoryContainerBroker;
	ESock::CCFFactoryContainerBroker* iCommsFactoryContainerBrokerSingleton;

	void AddFactoryContainerObjectBrokerClient(
		const Messages::TNodeId& aNodeId,
		const TCFPlayerRole& aType);

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	void IncludePerformanceData(TInt aDeltaClientRxBytes, TInt aDeltaClientRxBuffBytes, TInt aDeltaClientTxBytes);
private:
	static TBool AddToPerfLog(TAny* aSelf, TDes8& aBuffer, TDes8Overflow* aOverflowHandler);

	struct TXferStats
		{
		enum
			{
			KNumBuckets = 6 // power series from 128 - >65536
			};
		// No ctor; relying on embedding object to zero fill RAM
		void AccumulateXfer(TUint aValue);

		TUint iBuckets[KNumBuckets];
		TUint iTotal;
		TUint iCount;
		};
	TXferStats iRxStats;
	TXferStats iRxBuffStats;
	TXferStats iTxStats;
public:
#endif

	/** List of socket server extensions */
	TDblQue<CSocketServExtRef> iExtensions;
public:
	static CSockManData* NewL(Messages::CGlobals& aGlobals, CWorkerThread* aWorker);
	~CSockManData();
private:
	CSockManData(Messages::CGlobals& aGlobals, CWorkerThread* aWorker);
	void ConstructL();
	};

class SockManGlobals
/**
@internalTechnology
*/
	{
public:
	static IMPORT_C CSockManData* Get();
	static void Set(CSockManData * aGlobals);	//lint -sem(SockManGlobals::Set, custodial(1))
	};

/** Test that the current thread is the "home thread" */
#ifdef ESOCK_HOME_THREAD_CHECK_ENABLED
#define ASSERT_HOME_THREAD SockManGlobals::Get()->AssertOwnThread();
#else
#define ASSERT_HOME_THREAD
#endif

#endif
