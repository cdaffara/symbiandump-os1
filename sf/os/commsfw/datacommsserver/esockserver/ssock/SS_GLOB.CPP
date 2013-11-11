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
#include "ss_glob.h"

#include <comms-infras/ss_roles.h>
#include <ss_std.h>
#include <comms-infras/ss_log.h>
#include <ss_protprov.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <elements/nm_common.h>	//for Messages::TlsGlobals
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_tiermanager_internal.h>
#include "ss_apiext_messages.h"
#include <comms-infras/ss_mcprnodemessages.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_GLOB, "ESockSSockS_GLOB");
#endif

using namespace ESock;
using namespace Messages;
using namespace Den;

CSockManData::CSockManData(CGlobals& aGlobals, CWorkerThread* aWorker)
:	iWorkerThread(aWorker),
	iTransportGlobals(aGlobals),
	iCommsApiExtRegister(aGlobals)
	{
	__ASSERT_DEBUG(!SockManGlobals::Get(), User::Panic(KSpecAssert_ESockSSockS_GLOB, 1));
	SockManGlobals::Set(this);
	}

CSockManData::~CSockManData()
	{
	SockManGlobals::Set(NULL);
	iFCMap.Close();
	delete iTimer;
	delete iEskData;
	delete iWorkerThread;
	}

CSockManData* CSockManData::NewL(CGlobals& aGlobals, CWorkerThread* aWorker)
	{
	CSockManData* globs = new(ELeave) CSockManData(aGlobals, aWorker);
	CleanupStack::PushL(globs);
	globs->ConstructL();
	CleanupStack::Pop(globs);
	return globs;
	}

void CSockManData::ConstructL()
	{
	iExtensions.SetOffset(_FOFF(CSocketServExtRef, iExtLink));

	//at the moment the CProtocolFamilyFactoryBase subclasses' instances are created along with
	//the protocol family being loaded (applies to the new protocols only)
	iTimer=CDeltaTimer::NewL(ESocketTimerPriority, KTimerGranularity);
	iTimer->Deque();	//PERF: re-add upon first use

//	globals->iNumSessions=0;
//	globals->iNumFamilies=0;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
	iCommsBufPond=TCommsBufPondTLSOp::Get();
#else
    iMBufManager=CMBufManager::Context();
#endif // SYMBIAN_ZERO_COPY_NETWORKING
	}

TBool CSockManData::ShutdownGracefully()
/**
@return Whether the socket server intends to shutdown (ETrue) at first opportunity
when there is no connections to it. If ETrue it doesnt accept new connections.
*/
    {
    return SelfWorker()->ShuttingDown();
    }

EXPORT_C Messages::TNodeId CSockManData::GetPlaneFC(const TPlayerRole& aPlane)
	{
	return iFCMap.GetPlaneFC(aPlane);
	}


void CSockManData::InstallFactoryContainersL()
	{
	__ASSERT_DEBUG( iWorkerThread->Player() , User::Panic(KSpecAssert_ESockSSockS_GLOB, 2));	// Only Players have factories
	CPlayer& player(*iWorkerThread->Player());

	if (!iTierManagerFactories && player.HasTierMgrPlane())
		{
		iTierManagerFactories = CTierManagerFactoryContainer::NewL();
		TNodeId fc(iTierManagerFactories->Id());
		iFCMap.AddPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane), fc);
		iTransportGlobals.AddPersistentItf(fc);
		}

	if (!iMetaConnectionFactories && player.HasMetaConnPlane())
		{
		iMetaConnectionFactories = CMetaConnectionFactoryContainer::NewL();
		TNodeId fc(iMetaConnectionFactories->Id());
		iFCMap.AddPlaneFC(TCFPlayerRole(TCFPlayerRole::EMetaConnPlane), fc);
		iTransportGlobals.AddPersistentItf(fc);
		}

	if (!iConnectionFactories && player.HasConnPlane())
		{
		iConnectionFactories = CConnectionFactoryContainer::NewL();
		TNodeId fc(iConnectionFactories->Id());
		iFCMap.AddPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane), fc);
		iTransportGlobals.AddPersistentItf(fc);

		// Install a new object broker for all of the factory containers in affiliated worker threads
		__ASSERT_DEBUG(!iCommsFactoryContainerBrokerSingleton, User::Panic(KSpecAssert_ESockSSockS_GLOB, 3));
		iCommsFactoryContainerBrokerSingleton = CCFFactoryContainerBroker::NewL();
		iTransportGlobals.AddPersistentItf(iCommsFactoryContainerBrokerSingleton->Id());

		// Keep this locally for factory requests in our own thread
		// Other workers learn about it through an advertisement message
		iCommsFactoryContainerBroker = iCommsFactoryContainerBrokerSingleton->Id();
		}

	if (!iSubConnectionFactories && player.HasSubConnPlane())
		{
		iSubConnectionFactories = CSubConnectionFactoryContainer::NewL();
		TNodeId fc(iSubConnectionFactories->Id());
		iFCMap.AddPlaneFC(TCFPlayerRole(TCFPlayerRole::ESubConnPlane), fc);
		iTransportGlobals.AddPersistentItf(fc);
		}

	LOG(ESockLog::Printf(_L("ProtocolFamilyFactories Initialisation")));

	if (!iProtocolFamilyFactories && player.HasDataPlane())
		{
		iProtocolFamilyFactories = CProtocolFamilyFactoryContainer::NewL();
		iTransportGlobals.AddPersistentItf(iProtocolFamilyFactories->Id());
		}

	if (!iSubConnectionFlowFactories&& player.HasDataPlane())
		{
		iSubConnectionFlowFactories = CSubConnectionFlowFactoryContainer::NewL();
		TNodeId fc(iSubConnectionFlowFactories->Id());
		iFCMap.AddPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane), fc);
		iTransportGlobals.AddPersistentItf(fc);
		// SubConnection Factory Container receives the cookie of the Flow Factory Container,
		// as the former hosts requests to initiate Flow creation.
		// iSubConnectionFactories->SetFlowFactoryCookieL(0, (*iSubConnectionFlowFactories)());
		}

	if (!iProtocolIntfFactories && player.HasDataPlane())
		{
		iProtocolIntfFactories = CProtocolIntfFactoryContainer::NewL();
		iTransportGlobals.AddPersistentItf(iProtocolIntfFactories->Id());
		}

	// Now add our own factory containers to the object broker if we are hosting it, as we won't
	// be told about the broker if we are hosting it and won't then get a chance to send our FCs to it
	if (player.HasConnPlane())
		{
		if (iTierManagerFactories)
			{
			AddFactoryContainerObjectBrokerClient(
				iTierManagerFactories->Id(),
				TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)
				);
			}

		if (iMetaConnectionFactories)
			{
			AddFactoryContainerObjectBrokerClient(
				iMetaConnectionFactories->Id(),
				TCFPlayerRole(TCFPlayerRole::EMetaConnPlane)
				);
			}

		if (iConnectionFactories)
			{
			AddFactoryContainerObjectBrokerClient(
				iConnectionFactories->Id(),
				TCFPlayerRole(TCFPlayerRole::EConnPlane)
				);
			}

		if (iSubConnectionFactories)
			{
			AddFactoryContainerObjectBrokerClient(
				iSubConnectionFactories->Id(),
				TCFPlayerRole(TCFPlayerRole::ESubConnPlane)
				);
			}

		if (iSubConnectionFlowFactories)
			{
			AddFactoryContainerObjectBrokerClient(
				iSubConnectionFlowFactories->Id(),
				TCFPlayerRole(TCFPlayerRole::EDataPlane)
				);
			}
		}

	// Register internal messages
	TCFMessage::RegisterL();
	TCFSafeMessage::RegisterL();
	TExtItfMsgTables::RegisterL();
	}

void CSockManData::AddFactoryContainerObjectBrokerClient(
	const Messages::TNodeId& aNodeId,
	const TCFPlayerRole& aType)
	{
	// Should only be adding them once
	__ASSERT_DEBUG(0 == iCommsFactoryContainerBrokerSingleton->CountClients<TDefaultClientMatchPolicy>(aType), User::Panic(KSpecAssert_ESockSSockS_GLOB, 4));

	iCommsFactoryContainerBrokerSingleton->AddClientL(
		aNodeId,
		aType
		);
	}

void CSockManData::UninstallFactoryContainers()
	{
	// De-Register internal messages
	TCFMessage::DeRegister();
	TCFSafeMessage::DeRegister();
	TExtItfMsgTables::DeRegister();

	delete iCommsFactoryContainerBrokerSingleton;
	delete iSubConnectionFactories;
	delete iConnectionFactories;
	delete iProtocolIntfFactories;
	delete iProtocolFamilyFactories;
	delete iSubConnectionFlowFactories;
	delete iMetaConnectionFactories;
	delete iTierManagerFactories;
	}

// Support self-awareness when logging
EXPORT_C CWorkerThread* CSockManData::SelfWorker() const
	{
	return iWorkerThread;
	}

// Support self-awareness when logging
EXPORT_C CPlayer* CSockManData::SelfPlayer()
	{
	return iWorkerThread->Player();
	}

void SockManGlobals::Set(CSockManData * aGlobals)
	{
    Dll::SetTls(aGlobals);
    }

EXPORT_C CSockManData* SockManGlobals::Get()
	{
	CSockManData * d=(CSockManData *)Dll::Tls();
	return d;
    }

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
void CSockManData::IncludePerformanceData(TInt aDeltaClientRxBytes, TInt aDeltaClientRxBuffBytes, TInt aDeltaClientTxBytes)
	{
	if(aDeltaClientRxBytes >= 0)
		{
		iRxStats.AccumulateXfer(aDeltaClientRxBytes);
		}
	if(aDeltaClientRxBuffBytes >= 0)
		{
		iRxBuffStats.AccumulateXfer(aDeltaClientRxBuffBytes);
		}
	if(aDeltaClientTxBytes >= 0)
		{
		iTxStats.AccumulateXfer(aDeltaClientTxBytes);
		}
	}


TBool CSockManData::AddToPerfLog(TAny* aSelf, TDes8& aBuffer, TDes8Overflow* aOverflowHandler)
	{
	CSockManData* self = static_cast<CSockManData*>(aSelf);
	_LIT8(KFormat, "ESock Rx: tot %u; cnt %u; %u, %u, %u, %u, %u, %u; RxBuff: tot %u; %u, %u, %u, %u, %u, %u, Tx: tot %u; cnt %u; %u; %u, %u, %u, %u, %u");
	TXferStats& rx = self->iRxStats;
	TXferStats& rxB = self->iRxBuffStats;
	TXferStats& tx = self->iTxStats;
	aBuffer.AppendFormat(KFormat, aOverflowHandler,
		rx.iTotal, rx.iCount, rx.iBuckets[0], rx.iBuckets[1], rx.iBuckets[2], rx.iBuckets[3], rx.iBuckets[4], rx.iBuckets[5],
		rxB.iTotal, rxB.iBuckets[0], rxB.iBuckets[1], rxB.iBuckets[2], rxB.iBuckets[3], rxB.iBuckets[4], rxB.iBuckets[5],
		tx.iTotal, tx.iCount, tx.iBuckets[0], tx.iBuckets[1], tx.iBuckets[2], tx.iBuckets[3], tx.iBuckets[4], tx.iBuckets[5]
		);
	return EFalse;
	}

void CSockManData::TXferStats::AccumulateXfer(TUint aValue)
	{
	++iCount;
	if(aValue > 0)
		{
		iTotal += aValue;
		TInt bucketCap = 128;
		TInt bucket = 0;
		while(bucket < KNumBuckets)
			{
			if(bucketCap > aValue)
				{
				++iBuckets[bucket];
				break;
				}
			bucketCap <<= 2;
			++bucket;
			}
		}
	}


CSockManData::~CSockManData()
	{
	CommsFW::CPerfMetricStore::RemoveClient(this);
	}

#endif

const ::CESockIniData* CSockManData::IniData()
	{
	return iEskData;
	}

#ifdef ESOCK_HOME_THREAD_CHECK_ENABLED

void CSockManData::AssertOwnThread() const
	{
	TThreadId home = iWorkerThread->OwnThread();
	TThreadId curr = RThread().Id();
	if(home != curr)
		{
		Fault(EWrongThread);
		}
	}

#endif

#ifdef _DEBUG

void CSockManData::LogActiveProtocols()
	{
	TSglQueIter<CProtocolRef> i(*iProtocols);
	CProtocolRef* ref = NULL;
	while((ref = i++) != NULL)
		{
		CProtocolBase* prot = ref->Protocol();
		if(prot)
			{
			CProtocolFamilyBase* fam = prot->ProtocolFamily();
			LOG(ESockLog::Printf(KESockSessDetailTag, _L("* Prot %S refCnt = %d, family refCnt = %d"), &ref->Info().iName, prot->RefCount(), fam->RefCount() ));
			}
		}
	}

#endif

EXPORT_C CCommsFactoryBase* CSockManData::InstallFactoryL( ESock::EFactoryType aFactoryType, const TDesC8& aName, TUid aFactoryUid )
	{
	CSockManData *globals=SockManGlobals::Get();
	CCommsFactoryBase* pBase = NULL;
	switch (aFactoryType)
		{
		case EProtocolFamilyFactory:
			pBase = globals->iProtocolFamilyFactories->FindOrCreateL( aName, aFactoryUid );
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		};
	return pBase;
	}
