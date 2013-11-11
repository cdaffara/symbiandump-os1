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

#include <ss_std.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include "ss_msgs.h"
#include <es_ini.h>
#include <comms-infras/ss_log.h>
#include <ecom/ecom.h>
#include <ss_protprov.h>
#include <comms-infras/idquerynetmsg.h> //just to get Uids
#include <comms-infras/ss_tiermanagerutils.h>
#include <metadatabase.h> //CMDBSession
#include <elements/cftransportmsg.h>
#include <elements/nm_address_internal.h>
#include <elements/sd_msgs.h>
#include "ss_connectionserver.h"
#include <comms-infras/ss_common.h>
#include <comms-infras/ss_tiermanager.h>
#include <cfforwardmsg.h>
#include "ss_tierthreadmap.h"

// Required for the VERIFY_RESULT's in CFBindMessageReceived and DoCompleteUnbinding.
#include "../../../commsfwsupport/commselements/serverden/inc/sd_rootserverchannelhandler.h"

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_thrd, "ESockSSocks_thrd");
#endif

using namespace CommsFW;
using namespace ESock;
using namespace Messages;
using namespace Den;

//
// CWorkerThread class definitions
//


CWorkerThread* CWorkerThread::NewL(TCFModuleInfo* aModuleInfo)
	{
	CleanupStack::PushL(TCleanupItem(CCommonWorkerThread::DeleteHBufC8, aModuleInfo->iIniData));
	CWorkerThread* self = new (ELeave) CWorkerThread;
	CleanupStack::PushL(self);
	self->ConstructL(aModuleInfo);
	CleanupStack::Pop(self);
	CleanupStack::PopAndDestroy();	// aModuleInfo->iIniData
	return self;
	}

CWorkerThread::CWorkerThread()
:	CCommonWorkerThread(),
	iEskMaskList(KEskMaskListGranularity)
	{
	}

CWorkerThread::~CWorkerThread()
	{
	iTransportUsers.Shutdown(EFalse);
	iEskMaskList.Close();
	}

CPlayer* CWorkerThread::Player() const
	{
	return static_cast<CPlayer*>(CCommonWorkerThread::Player());
	}

CPitBoss& CWorkerThread::PitBoss() const
	{
	return static_cast<CPitBoss&>(CCommonWorkerThread::PitBoss());
	}

/**
The worker thread secondary construction will create the relevant Player/Dealer
instances needed as well as the channel handler to the Root Server. If and only if it
is the main thread it will also create the PitBoss.
@note If it has a Dealer and is not the main thread it is a WorkerDealer.
*/
TInt SocketServerShutDownCb(TAny*)
	{
	SocketServer::ShutDown();
	return KErrNone;
	}

void CWorkerThread::ConstructL(TCFModuleInfo* aModuleInfo)
	{
	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ConstructL Determining roles")));
	CCommonWorkerThread::ConstructL(*aModuleInfo, *Den::CShutdownWatchDog::NewL(this, TCallBack(SocketServerShutDownCb, NULL)));

#ifdef ESOCK_HOME_THREAD_CHECK_ENABLED
	iOwnThread = RThread().Id();
#endif

	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ConstructL Init SockManGlobals")));
	iSockManGlobals = CSockManData::NewL(*iGlobals, this);

	SocketServer::InitL(this);

	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ConstructL Init ProtocolManager")));
	ProtocolManager::InitL();

	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ConstructL Done Init ProtocolManager")));

	if(iPlayer)
		{
		SockManGlobals()->InstallFactoryContainersL();
		}

	LOG(ESockLog::Printf(_L("CWorkerThread::ConstructL Done")));
	}

/** Will read the .ESK masks from the inidata, to determine which .ESK files to load. */
void CWorkerThread::DoDeterminePlayerRoleL(HBufC8* aIniData, Den::TPlayerRole& aPlayerRole)
	{
	_LIT8(KEskMaskLabel, "EskMask");
	_LIT8(KEskMaskLabelNumFormat, "EskMask%d");
	_LIT8(KPlayerRoleLabel, "PlayerRole");

	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::DetermineRoleL building .ESK mask list.")));
	TPtrC8 currentEskMaskPtr;
	TBuf8<KEskMaskLabelLength> currentEskMask;
	currentEskMask.Format(KEskMaskLabel);
	TEskMask currentMaskEntry;
	TInt i=0;
	do
		{
		if(GetVarFromIniData(*aIniData, KNullDesC8, currentEskMask, currentEskMaskPtr))
			{
			LOG(ESockLog::Printf(KESockBootingTag, _L8("Found EskMask under heading [IniData]: %S"), &currentEskMaskPtr));
			// Sanity check length
			if(currentEskMaskPtr.Length()>KMaxEskMaskLength)
				{
				LOG(ESockLog::Printf(KESockBootingTag, _L8("EskMask%d under heading [IniData] contains value of more than %d characters"), i, KMaxEskMaskLength));
#ifdef _DEBUG
				Panic(ECorruptIniData);
#endif
				// leave if ini data corrup rather than panic unless debug
				User::Leave(KErrCorrupt);
				}
			// Add entry to list of EskMasks
			currentMaskEntry.Copy(currentEskMaskPtr);
			iEskMaskList.AppendL(currentMaskEntry);
			}
		else
			{
			/* If first iteration and nothing found in "EskMask" it is a critical error.
			For EskMask0..max it is merely cause for breaking the loop */
			if(i==0)
				{
				LOG(ESockLog::Printf(KESockBootingTag, _L("EskMask under heading [IniData] not found")));
				// leave if ini data corruprather than panic unless debug
#ifdef _DEBUG
				Panic(ECorruptIniData);
#endif
				User::Leave(KErrCorrupt);
				}
			else
				{
				/* Break out of loop, if we didnt find EskMask<n> no reason to waste
				time searching for EskMask<n+1> */
				LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::DetermineRoleL No more ESK masks found.")));
				break;
				}
			}
		// Getting ready for next one
		++i;
		currentEskMask.Format(KEskMaskLabelNumFormat, i);
		}
	while(i<KEskMaskLabelMax);

	TInt playerRole = (iWorkerId == TCFWorkerThreadPublicInfo::EIpPlayerThread)?		// defaults for legacy configurations
		(TCFPlayerRole::EDataPlane | TCFPlayerRole::ESubConnPlane | TCFPlayerRole::EConnPlane | TCFPlayerRole::EMetaConnPlane | TCFPlayerRole::ETierMgrPlane | TCFPlayerRole::ETierResolver | TCFPlayerRole::EPrePlanarArchitecture):
		(TCFPlayerRole::EDataPlane | TCFPlayerRole::ESubConnPlane | TCFPlayerRole::EConnPlane | TCFPlayerRole::EMetaConnPlane | TCFPlayerRole::ETierMgrPlane | TCFPlayerRole::EPrePlanarArchitecture);
	if(GetVarFromIniData(*aIniData, KNullDesC8, KPlayerRoleLabel, playerRole))
		{
		if((playerRole != 0) &&
			((playerRole & ~(TCFPlayerRole::EDataPlane | TCFPlayerRole::ESubConnPlane | TCFPlayerRole::EConnPlane | TCFPlayerRole::EMetaConnPlane | TCFPlayerRole::ETierMgrPlane | TCFPlayerRole::ETierResolver)) == 0))	// no unknown flags
			{
			}
		else
			{
			LOG(ESockLog::Printf(KESockBootingTag, _L("PlayerType=%04x - unsupported type (must be OR of TPlayerRole flags or unspecified for legacy"), playerRole));
#ifdef _DEBUG
			Panic(ECorruptIniData);
#endif
			// leave if ini data corrupt rather than panic if not debug
			User::Leave(KErrCorrupt);
			}
		}
	aPlayerRole = TCFPlayerRole(playerRole, aPlayerRole.Kindex());
	}

CCommonPitBoss* CWorkerThread::DoCreatePitBossL(CCommonWorkerThread* aWorkerThread)
	{
	return CPitBoss::NewL(static_cast<CWorkerThread*>(aWorkerThread));
	}

CCommonPlayer* CWorkerThread::DoCreatePlayerL(CCommonWorkerThread* aWorkerThread,TPlayerRole aPlayerRole)
	{
	CPlayer* player = CPlayer::NewL(static_cast<CWorkerThread*>(aWorkerThread),aPlayerRole);
//	CleanupStack::PushL(player);
//	CleanupStack::Pop(player);
	return player;
	}

CCommonDealer* CWorkerThread::DoCreateDealerL(CCommonWorkerThread* aWorkerThread, Den::TPlayerRole aPlayerRole)
	{
	CCommonDealer* dealer = NULL;
	if (aPlayerRole.HasRole(TCFPlayerRole::ETierResolver))
		{
		CConnectionServer* cs = CConnectionServer::NewL(static_cast<CWorkerThread*>(aWorkerThread));
		CleanupStack::PushL(cs);
		dealer = CCommonDealer::NewL(cs);
		CleanupStack::Pop(cs);
		cs->SetDealer(dealer);
		}
	else
		{
		CSocketServer* ss = CSocketServer::NewL(static_cast<CWorkerThread*>(aWorkerThread));
		CleanupStack::PushL(ss);

		if (IsMainThread())
			{
			dealer = CCommonDealer::NewL(ss);
			}
		else
			{
			dealer = CCommonWorkerDealer::NewL(ss);
			}
		CleanupStack::Pop(ss);
		ss->SetDealer(dealer);
		}
	return dealer;
	}

/**
Takes the total list of .ESK files and generates a new local list containing only the file names
matching the .ESK list for this worker. Then creates a new ini data instance with the local list to be
used by the protocol manager for initialisation.
@see CESockIniData
*/
void CWorkerThread::ProcessMatchingEskFilesL(const COwnEntryList* aEskFiles)
	{
	LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ProcessMatchingEskFilesL: %d files"), aEskFiles->Count()));
	TInt i;
	for(i=0;i<iEskMaskList.Count();i++)
		{
		LOG(ESockLog::Printf(KESockBootingTag, _L("CWorkerThread::ProcessMatchingEskFilesL: mask %S"), &iEskMaskList[i]));
		}

	// The Gordian Knot of global data remains only half slashed... Also future work may eliminate the sharing of iEskData
	// through this, rather than PitBoss()?
	CESockIniData** iniData = &SockManGlobals::Get()->iEskData;
	delete *iniData;
	*iniData = NULL;
	// Build a private, filtered copy of the total set of ESK files
	CommsFW::COwnEntryList* ownEskFiles = new(ELeave) COwnEntryList(6);
	CleanupStack::PushL(ownEskFiles);

	TParse parsed;
	const TInt cnt = aEskFiles->Count();
	for(i = 0; i < cnt; ++i)
		{
		const COwnEntryList::TOwnEntry& entry = aEskFiles->operator[](i);
		parsed.Set(entry.iName, NULL, NULL);
		TPtrC nameAndExt = parsed.NameAndExt();
		for(TInt j=0; j<iEskMaskList.Count(); ++j)
			{
			if(nameAndExt.MatchF(iEskMaskList[j]) >= 0)
				{
				LOG(ESockLog::Printf(KESockBootingTag, _L("- matched %S"), &entry.iName));
				ownEskFiles->AddL(entry);
				break; // Stop inner loop
				}
			}
		}
	iEskMaskList.Close();
	LOG(ESockLog::Printf(KESockBootingTag, _L8("CWorkerThread::ProcessMatchingEskFilesL: %d files matched"), ownEskFiles->Count()));
	if(ownEskFiles->Count() == 0 && (!iPlayer || static_cast<CPlayer*>(iPlayer)->HasDataPlane()))
		{
		// Threads with data planes must have one or more matching ESK files (otherwise
		// they have no function and surely result from configuration errors)
		User::Leave(KErrCorrupt);
		}
	// The ini data processing lives in the global protocol manager, for legacy reasons
	*iniData = CESockIniData::NewL(ownEskFiles);

	CleanupStack::Pop(ownEskFiles);
	delete ownEskFiles;
	if (iPlayer)
		{
		if(static_cast<CPlayer*>(iPlayer)->HasDataPlane())
			{
			ProtocolManager::ProcessIniDataL();
			}
		}
	}

void CWorkerThread::DoProcessWorkerIntroductionL(const TWorkerIntroductionMsg& aMsg)
	{
	TBuf8<TWorkerIntroductionMsg::KMaxIntroductionInfoSize> eskFilesList;
	aMsg.IntroductionInfo(eskFilesList);
	CommsFW::COwnEntryList* const* eskFiles = reinterpret_cast<CommsFW::COwnEntryList* const*>(eskFilesList.Ptr());
	TRAPD(ret, ProcessMatchingEskFilesL(*eskFiles));

	if (ret != KErrNone)
		{
		LOG(ESockLog::Printf(KESockBootingTag, _L("ERROR %d processing Esk files"), ret));
	#ifdef _DEBUG
		RDebug::Printf("CWorkerThread::DoProcessWorkerIntroductionL ERROR %d processing Esk files", ret);
		Panic(EMisconfigured);
	#endif
		// Leave if problem with EskFiles instead of panic unless debug
		User::Leave(KErrCorrupt);
		}
	}

void CWorkerThread::DoFillIntroMessage(TWorkerId /*aPeerId*/, TWorkerIntroductionMsg& aIntroMsg)
	{
	if(IsMainThread())
		{
		const CommsFW::COwnEntryList* eskList = PitBoss().GetCompleteList();
		TPtrC8 ptr((TUint8*)&eskList, sizeof(TAny*)); //Yes, pointer to pointer
		aIntroMsg.SetIntroductionInfo(ptr);
		}
	else
		{
		TPckg<TBool> defaultOptDealer(DefaultOptimalDealer());
		aIntroMsg.SetIntroductionInfo(defaultOptDealer);
	}
	}

/**
Deals with binding requests from the Root Server. Note that although the Root Server allows
multiple identical bindings ESock does not allow this and will panic if the binding already exists.
Bindings are expressed in ESock by worker transport objects stored in an array. Since all sub-module
names are unique across all ESock instances (they are the individual owning worker ID converted to text)
they can be used here. I.e. the remote end sub-module is converted back to int and used to insert the
CWorkerTransport into an array in the position corresponding to the remote end sub-module name/number.
As for responding to the bind request there are two cases:
    -# This worker is a "peer Player: Immediately introduce ourselves, including cookies
	-# This worker is the Main thread: Wait to receive its introduction message, which carries the ESK file list.
@see CWorkerTransport
*/
void CWorkerThread::CFBindMessageReceived(const TCFBindMsg& aMsg)
	{
    __CFLOG_SMADDR2(( KESockComponentTag, KESockServerTag, _L8("W%d: CFBindMessageReceived %S <=> %S"),
		WorkerId(),
		&aMsg.SubModule1()->Printable(__FullModName1),
		&aMsg.SubModule2()->Printable(__FullModName2) ));
	TWorkerId bindId;
	TInt err = DecodePeerId(aMsg.SubModule1(), aMsg.SubModule2(), bindId);
	__ASSERT_DEBUG(bindId <=  TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ESockSSocks_thrd, 1));
	if(err == KErrNone)
		{
		if(iTransport->PeerReachable(bindId))
			{
			LOG(ESockLog::Printf(_L8("%d Already exists. Error in configuration of cmi files"),bindId));
			Panic(EDuplicateWorkerId); //Must panic b/c the new module will be left in a half bound state and will tend to get out of hand
			}
		else
			{
			err = iTransport->EstablishTransportToPeer(bindId, aMsg.ChannelInput(), aMsg.ChannelOutput());
			if(err == KErrNone)
				{
				iTransport->SetLastRequestIdConcerningPeer(bindId, aMsg.Identifier());

				// If we're binding to the main thread then wait for its intro, with the all-important ESK file list
				if(bindId != TWorkerThreadPublicInfo::EMainThread)
					{
					SendIntroMessage(aMsg.SubModule2()->Module(), bindId);

					// If we have a player then follow up with a bucket of our own cookies (always a good way to make friends
					// with the new neighbour...). Only "planar architecture" configurations share their cookies; this allows
					// bindings between legacy threads for purposes of Dealer -> Player forwarding only.
					// @TODO - either we stop forwarding from optimal Dealers to Players or we have to add of indicating which
					// bindings are for sharing planes and which are for simply forwarding requests. Probably we should do the
					// former - if someone asks for "optimal IP data" then why should they be able to send SMS from that session?
					Messages::TNodeId dataFC;
					Messages::TNodeId subConnFC;
					Messages::TNodeId connFC;
					Messages::TNodeId metaConnFC;
					Messages::TNodeId tierManagerFC;
					if(Player())
						{
						const TPlayerRole& role = Player()->PlayerRole();
						if(!role.HasRole(TCFPlayerRole::EPrePlanarArchitecture))
							{
							// It's safe to simply copy the cookies we know of here - we cannot yet have learnt of any others so they must be ours
							CSockManData* globs = Player()->SockManGlobals();
							if(role.HasRole(TCFPlayerRole::EDataPlane))
								{
								dataFC = globs->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EDataPlane));
								}
							if(role.HasRole(TCFPlayerRole::ESubConnPlane))
								{
								subConnFC = globs->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ESubConnPlane));
								}
							if(role.HasRole(TCFPlayerRole::EConnPlane))
								{
								connFC = globs->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EConnPlane));
								}
							if(role.HasRole(TCFPlayerRole::EMetaConnPlane))
								{
								metaConnFC = globs->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::EMetaConnPlane));
								}
							if(role.HasRole(TCFPlayerRole::ETierMgrPlane))
								{
								tierManagerFC = globs->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane));
								}
							}
						}

                    NM_LOG_START_BLOCK(KESockComponentTag, _L8("PeerIntro"));
                    NM_LOG((KESockComponentTag, _L8("CFBindMessageReceived(): Sending Cookies to peer #%d "), bindId));
                    NM_LOG_ADDRESS_EXT(KESockComponentTag, dataFC, _L8("dataFC: "));
                    NM_LOG_ADDRESS_EXT(KESockComponentTag, subConnFC, _L8("scFC: "));
                    NM_LOG_ADDRESS_EXT(KESockComponentTag, connFC, _L8("cnFC: "));
                    NM_LOG_ADDRESS_EXT(KESockComponentTag, metaConnFC, _L8("mcFC: "));
                    NM_LOG_ADDRESS_EXT(KESockComponentTag, tierManagerFC, _L8("tmFC: "));
				    NM_LOG_END_BLOCK(KESockComponentTag, _L8("PeerIntro"));
					TWorkerCookiesMsg cookieMsg(dataFC, subConnFC, connFC, metaConnFC, tierManagerFC);
					PostMessage(bindId, cookieMsg);

					if(!IsMainThread())
						{
						if(Player()->HasConnPlane())
							{
							SendObjectBrokerIntroductionMessage(bindId);
							}
						}
					}
				}
			}
		}
	/* Main dealer only completes when it has received the introduction response messages from peers,
	dumb players respond immediately. */
	if(err!=KErrNone || !IsMainThread())
		{
		TCFBindCompleteMsg respMsg(aMsg.Identifier(), err);
		VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
		}
	}


void CWorkerThread::SendObjectBrokerIntroductionMessage(const CommsFW::TWorkerId& aPeerId)
	{
	// Share our central object broker node with a worker
	__ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ESockSSocks_thrd, 2));
	__ASSERT_DEBUG(Player()->HasConnPlane(), User::Panic(KSpecAssert_ESockSSocks_thrd, 3));
	TBrokerIntroductionMsg brokerIntroMsg(SockManGlobals()->iCommsFactoryContainerBroker);
	PostMessage(aPeerId, brokerIntroMsg);
	}

void CWorkerThread::DoCompleteUnbinding(CommsFW::TWorkerId aWorker)
	{
	TCFPlayerRole ourRole(TCFPlayerRole::EAllPlanes | TCFPlayerRole::ETierResolver);
	TPlaneUnboundMsg unboundMsg(ourRole);
	PostMessage(aWorker, unboundMsg);

	CSockManData* globs = SockManGlobals();
	globs->iFCMap.DropFCOfPeer(TCFPlayerRole(TCFPlayerRole::EAllPlanes), aWorker);

	CommsFW::TCFUnbindCompleteMsg respMsg(iTransport->LastRequestIdConcerningPeer(aWorker), KErrNone);
	iTransport->DropTransportToPeer(aWorker);
	VERIFY_RESULT(iChannelHandler->Send(respMsg), KErrNone);
	}

/**
It is possible for client applications outside of the Comms Process to send messages to
an ESock Worker thread. This is done through the Root Server SendMessage method.
The message is received by the destination ESock Worker in the CFMessageForward member,
which will deal with it accordingly.
Supported message type is TCFMessageType::EFactoryMsg destined for one or more
CProtocolFamilyFactoryBase instances from globals->iProtocolFamilyFactories. ESock will
traverse through this list and deliver (by calling SendMessage()) the message content to
every instance present there.
The message content for such a message sent to an ESock Worker is required to be based on
the CNetMessage class.
@note Completion of the client request the responsibility of the recipient.
@see RRootServ::SendMessage
*/
void CWorkerThread::CFMessageForward(const CommsFW::TCFForwardMsg& aMessage)
	{
	const RMessage2& msg = aMessage.Msg();
	if ( !SockManGlobals::Get()->iProtocolFamilyFactories )
		{
		msg.Complete( KErrNotReady );
		return;
		}
	TInt len = msg.GetDesLength(2);
	SMetaDataECom* pInstance = NULL;

	TInt nErr=KErrArgument;

	TPtr8 pDataPtr(0,0);
	RBuf8 buf;

	if( len >= 0 )
		{
		nErr = buf.Create( len );
		if ( nErr == KErrNone )
			{
			pDataPtr.Set( const_cast<TUint8*>(buf.Ptr()), 0, buf.MaxLength() );
			nErr = msg.Read( 2, pDataPtr );
			}
		if ( nErr == KErrNone )
			{
			TPtrC8 ptr(pDataPtr);
			TRAP
				(nErr,
				pInstance = SMetaDataECom::LoadL( ptr )
				)
			}
		}

	if ( nErr == KErrNone )
		{
		if ( !pInstance->IsTypeOf( STypeId::CreateSTypeId( NetMessages::KInterfaceUid, NetMessages::EMessageAny ) ) )
			{
			nErr = KErrArgument;
			}
		else
			{
			NetMessages::CMessage* pNetMessage = static_cast<NetMessages::CMessage*>(pInstance);
			switch ( msg.Int1() )
				{
			case /*TCFMessageType::*/EFactoryMsg:
				{
				TInt nIndex = 0;
				nErr = KErrNotSupported;
				ESock::CProtocolFamilyFactoryBase* pFactory = SockManGlobals::Get()->iProtocolFamilyFactories->Get( nIndex++ );
				while ( pFactory != NULL )
					{
					if ( (nErr = pFactory->SendMessage( *pNetMessage )) != KErrNotSupported )
						{
						break;
						}
					pFactory = SockManGlobals::Get()->iProtocolFamilyFactories->Get( nIndex++ );
					}
				pDataPtr.Zero();
				if(nErr == KErrNone)
					{
					nErr = pNetMessage->Store( pDataPtr );
					}
				if ( nErr == KErrNone )
					{
					nErr = msg.Write( 2, pDataPtr );
					}
				break;
				}
			default:
				nErr = KErrArgument;
				break;
				}
			}
		}
	buf.Close();
	delete pInstance;
	msg.Complete( nErr );
	}

void CWorkerThread::MaybeIncorporateFCL(const TPlayerRole& aPlane, const Messages::TNodeId& aPeerId)
	{
	CSockManData* globals = SockManGlobals();

	if(!aPeerId.IsNull())
		{
		TNodeId ourId(globals->GetPlaneFC(aPlane));
		if(!ourId.IsNull() && Player() && !Player()->HasDataPlane())
			{
			// We're meeting a peer with a role that we either have or have already acquired. This is acceptable for a data plane being bound to multiple control
			// planes since modern session flows will be associated a conn or subconn, but in other cases is an error because of the need for explicit resolution
			// it presents (eg would become "the correction SubConn plane for Conn X" rather than simply "the SubConn plane bound to current Conn Plane"
			LOG(ESockLog::Printf(KESockBootingTag, _L8("WARNING: Worker thread %d being bound also has FC for plane 0x%x"), aPeerId.Thread(), aPlane.Role()));

#ifdef _DEBUG
            // If this stings you see DEF109193, and speak to ThomasG, BarryA, or PeterSau
            Panic(EAmbigiousRoles);
#endif
			// Leave if problem with config instead of panic unless debug
            User::Leave(KErrCorrupt);	// no benefit to specific error; unlikely to be visible in UREL (no logging, debugging)
            return;
			}

		// The addition may fail if we're a multiply-bound data thread, as described above. This is harmless
		TInt err = globals->iFCMap.AddPlaneFC(aPlane, aPeerId);

		// Add the factory to our central factory container object broker (if we are hosting it)
		if(Player()->HasConnPlane() && (err == KErrNone))
			{
			// Must have installed our broker already
			__ASSERT_DEBUG(globals->iCommsFactoryContainerBrokerSingleton, User::Panic(KSpecAssert_ESockSSocks_thrd, 4));

			// Can only have one factory container from each unique player role (plane)
			__ASSERT_DEBUG(0 == globals->iCommsFactoryContainerBrokerSingleton->CountClients<TDefaultClientMatchPolicy>(aPlane), User::Panic(KSpecAssert_ESockSSocks_thrd, 5));

			// Add the factory container to our broker's list
			globals->iCommsFactoryContainerBrokerSingleton->AddClientL(aPeerId, aPlane);
			}
		}
	}


/**
Deal with incoming messages from other workers. If the message is not a known type it might
be a generic transport message and an attempt to use a transporthandler ECOM plugin is made.
*/
TBool CWorkerThread::DoDispatchL(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId)
	{
	switch(aMessage.Code())
		{
		case TWorkerMsg::EWorkerCookies:
			{
			LOG(ESockLog::Printf(_L("PeerWorkerMessageReceived(%d). EWorkerCookies"), aSenderId));
			const TWorkerCookiesMsg& msg = reinterpret_cast<const TWorkerCookiesMsg&>(aMessage);

			if(aSenderId != TWorkerThreadPublicInfo::EMainThread && !IsMainThread())
				{
				// Not the PitBoss meeting us - if we're not the PitBoss then it should be a peer Player with some cookies to share
				// For any given plane the rules are:
				//	(1) if our Player has that role itself then we simply ignore their cookie (if any)
				//	(2) if our Player lacks the role but we've already bound to a peer which does then its a config error
				// The rules are driven by the need to avoid ambiguity in the coupling of planes, a subconn doesn't need (or have)
				// the ability to specify which SubConnflowFC will create a flow for it. So it's either its own thread (if it implements
				// the role) or the one & only peer bound to it which implements this. We don't support ideas like "the first peer" because
				// that depends upon the startup ordering of the RootServer and although that is well-defined and user controllable it seems
				// likely to be seen as unhelpfully complex and a source of mysterious config errors (eg BT data thread being asked to
				// create a UDP flow)
				TUint32 localRole = Player()? Player()->PlayerRole().Role() : TCFPlayerRole::EUnknown;
				if ((localRole & (TCFPlayerRole::EAllPlanes | TCFPlayerRole::ETierResolver)) != TCFPlayerRole::ETierResolver)
					{
					// This is presuming that the tier resolver is running in its own thread
					// and should not learn the roles from peers
					MaybeIncorporateFCL(TCFPlayerRole(TCFPlayerRole::EDataPlane), msg.DataPlaneFC());
					MaybeIncorporateFCL(TCFPlayerRole(TCFPlayerRole::ESubConnPlane), msg.SubConnPlaneFC());
					MaybeIncorporateFCL(TCFPlayerRole(TCFPlayerRole::EConnPlane), msg.ConnPlaneFC());
					MaybeIncorporateFCL(TCFPlayerRole(TCFPlayerRole::EMetaConnPlane), msg.MetaConnPlaneFC());
					}

	            // For the benefit of the connection server the tier resolver is allowed
	            // to learn about the tier managers.
				MaybeIncorporateFCL(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane), msg.TierManagerFC());
				}
			break;
			}

		case TWorkerMsg::EBrokerIntroduction:
			{
			__ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ESockSSocks_thrd, 6));

			// The tier resolver is virtually friends with everyone and so not a candidate for learning
			// about a technology specific singleton like the object broker
			if(!Player()->HasTierResolver() && !Player()->HasDataPlane())
				{
				CSockManData* globals = SockManGlobals();
				const TBrokerIntroductionMsg& msg = reinterpret_cast<const TBrokerIntroductionMsg&>(aMessage);

				// If we don't already have our own broker then accept the one being introduced
				if(globals->iCommsFactoryContainerBroker == TNodeId::NullId())
					{
					globals->iCommsFactoryContainerBroker = msg.BrokerId();
					}
				}
			break;
			}

		case TWorkerMsg::ELoadTierMappings:
			{
			if(IsMainThread())
				{
				static_cast<CPitBoss&>(PitBoss()).RequestLoadTierMapping();
				}
			else
				{
				__ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ESockSSocks_thrd, 7));
				__ASSERT_DEBUG(static_cast<CPlayer*>(Player())->HasTierResolver(), User::Panic(KSpecAssert_ESockSSocks_thrd, 8));

				RTierThreadMap* map = NULL;
                TRAPD(err,
				    CommsDat::CMDBSession* dbs = CommsDat::CMDBSession::NewLC(KCDVersion1_1);
                    map = TierManagerUtils::BuildTierThreadMappingL(*dbs);
                    CleanupStack::PopAndDestroy(dbs);
                    );
                (void) err;
                LOG(ESockLog::Printf(_L8("CWorkerThread:\tDoDispatch, ELoadTierMappings err=%d, %d mappings"), err, map? map->Count(): 0));

				TWorkerTierMappingsLoaded respMsg(map);
				PostMessage(aSenderId, respMsg);
				}
			break;
			}

		case TWorkerMsg::ETierMappingsLoaded:
			{
			__ASSERT_DEBUG(IsMainThread(), User::Panic(KSpecAssert_ESockSSocks_thrd, 9));
			static_cast<CPitBoss&>(PitBoss()).OnTierMappingLoadedL(&reinterpret_cast<const TWorkerTierMappingsLoaded&>(aMessage), aSenderId);
			break;
			}
		case TWorkerMsg::EPlaneUnbound:
			{
			const TPlaneUnboundMsg& msg(reinterpret_cast<const TPlaneUnboundMsg&>(aMessage));
			LOG(ESockLog::Printf(_L("PeerWorkerMessageReceived(%d). EPlaneUnbound (planes=0x%04x)"), aSenderId, msg.PlayerRole().Role()));
			CSockManData* globs = SockManGlobals();
			globs->iFCMap.DropFCOfPeer(msg.PlayerRole(), aSenderId);
	//		MaybeCompleteUnbinding(aSenderId);
			MaybeTriggerThreadShutdownCallback();
			break;
			}
		LOG( //This "case" is only for logging, it is properly forwarded in the CCommonWorkerThread
		case TPlayerMsg::EForwardRequest:
			{
			TPlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TPlayerForwardRequestMsg&>(aMessage));
			TBuf8<64> messBuf;
			ESockLog::IPCMessName((TSockMess) fwdReqMsg.SafeMessage().Function(), messBuf);
			ESockLog::Printf(KESockSessDetailTag, _L8("PeerWorkerMessageReceived(%d). EForwardRequest(%S)"), aSenderId, &messBuf);
			break;
			}
		);
		default:
			{
			// If it isn't an internal message then it should be a TWorkerTransportMsg for a plugin
			const TWorkerTransportMsg& msg = reinterpret_cast<const TWorkerTransportMsg&>(aMessage);
			const RThread& peerThread = PitBoss().RThreadRef(aSenderId);
			TRequestWrapper request(msg.Status(), EFalse);
			request.SetPeerThread(peerThread);
			TransportMessageReceived(msg, request, aSenderId);
			}
		}

	return ETrue; //We always process the message (TransportMessageReceived() may panic if not recognised)
	}

/**
This override ensures we do not check an ECNCreate message to see if the subsession is orphaned - the subsession
pointer is reused to hold a tier ID rather than a pointer.
@see CPlayer::ProcessMessageL
*/
void CWorkerThread::OnDispatchLeave(const CommsFW::TCFMessage& aMessage, CommsFW::TWorkerId aSenderId, TInt aError)
	{
	__ASSERT_DEBUG(aError!=KErrNone, User::Panic(KSpecAssert_ESockSSocks_thrd, 10));
	switch(aMessage.Code())
		{
        case TPlayerMsg::EForwardRequest:
            {
            if(aError != KErrBadDescriptor)
                {
                TPlayerForwardRequestMsg fwdReqMsg(reinterpret_cast<const TPlayerForwardRequestMsg&>(aMessage));
                // Should we also test Message().IsNull(). Should be no aliasing of it now?
                LOG(ESockLog::Printf(_L8("CWorkerThread:\tOnDispatchLeave, Complete message(%08x) with %d."), fwdReqMsg.SafeMessage().Handle(), aError));
                __ASSERT_DEBUG(Player(), User::Panic(KSpecAssert_ESockSSocks_thrd, 11));
                if (Player()->ShouldCompleteCurrentRequest())
                    {
                    fwdReqMsg.SafeMessage().Complete(aError);
                    }
                }
            }
            break;

        default:
            CCommonWorkerThread::OnDispatchLeave(aMessage, aSenderId, aError);
		}
	}

/**
Attempt to locate and create a new transportuser (using ECOM).
@leave KErrNotFound
@leave KErrNoMemory
*/
void CWorkerThread::CreateTransportUserL(TTransportUser& aTemplateUser)
	{
	aTemplateUser.iUserStorage = new(ELeave) TTransportUserStorage;
	TTransportReceiverFactoryArgs factoryArgs(aTemplateUser.iUserStorage, WorkerId());
	aTemplateUser.iUser = reinterpret_cast<CWorkerTransportReceiver*>(REComSession::CreateImplementationL(TUid::Uid(aTemplateUser.iInterfaceId), aTemplateUser.iDtorId, &factoryArgs));
	TInt ret = iTransportUsers.InsertInOrder(aTemplateUser, TLinearOrder<TTransportUser>(TTransportUser::Compare));
	if(ret != KErrNone)
		{
		REComSession::DestroyedImplementation(aTemplateUser.iDtorId);
		delete aTemplateUser.iUser;
		User::Leave(ret);
		}
	}

/**
Given a transport message try to look up its code in the list of loaded ECOM plugins. If this doesnt
work ask ECOM whether it has any plugins supporting this interface ID.
*/
TInt CWorkerThread::TransportMessageReceived(const TWorkerTransportMsg& aMessage, TRequestWrapper& aRequestWrapper, TWorkerId aWorkerId)
	{
	// A message other than the private ESOCKSVR ones was received. Its id should be that of a plugin which will process it
	TTransportUser user;
	user.iInterfaceId = aMessage.Code();
	TInt ret = iTransportUsers.FindInOrder(user, TLinearOrder<TTransportUser>(TTransportUser::Compare));
	if(ret == KErrNotFound)
		{
		TRAP(ret, CreateTransportUserL(user));
		if(ret == KErrNone)
			{
			user.iUser->ProcessMessage(aMessage, aRequestWrapper, *user.iUserStorage, aWorkerId);
			}
		else
			{
			delete user.iUserStorage;
			if(ret != KErrNoMemory)
				{
				// Message was neither an internal one nor a loadable plugin. Asserting is helpful here
				LOG(ESockLog::Printf(_L("UNKNOWN: Transport message #%d (#%08x)"), aMessage.Code(), aMessage.Code()));
				__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSocks_thrd, 12));
				}
			aRequestWrapper.RequestComplete(ret);
			}
		}
	else
		{
		const TTransportUser& refUser = iTransportUsers[ret];
		refUser.iUser->ProcessMessage(aMessage, aRequestWrapper, *refUser.iUserStorage, aWorkerId);
		}
	return ret;
	}


void CWorkerThread::DoPostMortemCleanup()
	{
	// First cleanup any transport users - these are more important than clients since they
	// may have peer threads blocked on them
	iTransportUsers.Shutdown(ETrue);
	}

void CWorkerThread::DoSetShuttingDown()
	{
	SockManGlobals::Get()->iShutdownGracefully = ETrue;
	}

/**
Given address family and socket type will locate the sender object for the worker thread
hosting the protocol. If it is the same worker as the current one, returns the self sender,
which use direct calls instead of channels for processing messages, otherwise return the
appropriate CWorkerTransport.
Although an exported function it is only allowed to call it from the context of an ESock worker thread.
It is mainly for use by plugins.
@see CWorkerTransport
@see TWorkerTransportSelfSender
*/
EXPORT_C TInt CWorkerThread::CookieForProtocol(TUint aAddrFamily, TUint aSockType, TUint aProtocol, Messages::TRuntimeCtxId& aCookie)
	{
	CWorkerThread* selfWorker = SockManGlobals::Get()->SelfWorker();
	__ASSERT_DEBUG(selfWorker, User::Panic(KSpecAssert_ESockSSocks_thrd, 13));	// calling this function from other than a ESOCK Worker thread cannot work
	if(selfWorker)
		{
		TWorkerId workerId;
		if(selfWorker->PitBoss().GetWorkerForProtocol(aAddrFamily, aSockType, aProtocol, workerId))
			{
			if(selfWorker->Transport()->PeerReachable(workerId))
				{
				Messages::TRuntimeCtxIdOp(aCookie).Set(0, workerId);
				return KErrNone;
				}
			}
		}
	return KErrBadName;
	}

/**
Given protocol name will locate the sender object for the worker thread
hosting the protocol. If it is the same worker as the current one, returns the self sender,
which use direct calls instead of channels for processing messages, otherwise return the
appropriate CWorkerTransport.
Although an exported function it is only allowed to call it from the context of an ESock worker thread.
It is mainly for use by plugins.
@see CWorkerTransport
@see TWorkerTransportSelfSender
*/
EXPORT_C TInt CWorkerThread::CookieForProtocolByName(const TProtocolName& aName, Messages::TRuntimeCtxId& aCookie)
	{
	CWorkerThread* selfWorker = SockManGlobals::Get()->SelfWorker();
	__ASSERT_DEBUG(selfWorker, User::Panic(KSpecAssert_ESockSSocks_thrd, 14));	// calling this function from other than a ESOCK Worker thread cannot work
	if(selfWorker)
		{
		TWorkerId workerId;
		if(selfWorker->PitBoss().GetWorkerForProtocolByName(aName, workerId))
			{
			if(selfWorker->Transport()->PeerReachable(workerId))
				{
				Messages::TRuntimeCtxIdOp(aCookie).Set(0, workerId);
				return KErrNone;
				}
			}
		}
	return KErrNotFound;
	}

/**
Given Worker ID will locate the sender object for the worker thread
hosting the protocol. If it is the same worker as the current one, returns the self sender,
which use direct calls instead of channels for processing messages, otherwise return the
appropriate CWorkerTransport.
Although an exported function it is only allowed to call it from the context of an ESock worker thread.
It is mainly for use by plugins.
@see CWorkerTransport
@see TWorkerTransportSelfSender
*/
EXPORT_C TInt CWorkerThread::CookieForWorkerId(TWorkerId aWorkerId, Messages::TRuntimeCtxId& aCookie)
	{
	CWorkerThread* selfWorker = SockManGlobals::Get()->SelfWorker();
	__ASSERT_DEBUG(selfWorker, User::Panic(KSpecAssert_ESockSSocks_thrd, 15));	// calling this function from other than a ESOCK Worker thread cannot work
	if(selfWorker)
		{
		if(selfWorker->Transport()->PeerReachable(aWorkerId))
			{
			Messages::TRuntimeCtxIdOp(aCookie).Set(0, aWorkerId);
			return KErrNone;
			}
		}
	return KErrNotSupported;
	}

EXPORT_C TBool CWorkerThread::ResolveWorkerNameToId(const TDesC8& aWorkerName, TWorkerId& aWorkerId)
	{
	CSockManData* globs = SockManGlobals::Get();
	__ASSERT_DEBUG(globs, User::Panic(KSpecAssert_ESockSSocks_thrd, 16));
	return globs->SelfWorker()->PitBoss().ResolveWorkerNameToId(aWorkerName, aWorkerId);
	}

/**
Use SockManGlobals in thread local storage (TLS) to retrieve WorkerID of the current worker thread.
*/
EXPORT_C TInt CWorkerThread::CurrentWorkerId(TWorkerId& aWorkerId)
	{
	CWorkerThread* selfWorker = SockManGlobals::Get()->SelfWorker();
	__ASSERT_DEBUG(selfWorker, User::Panic(KSpecAssert_ESockSSocks_thrd, 17));	// calling this function from other than a ESOCK Worker thread cannot work
	if(selfWorker)
		{
		aWorkerId = selfWorker->WorkerId();
		return KErrNone;
		}
	return KErrNotSupported;
	}

//
// CWorkerThread::RTransportUsers
//

/** Notify all active transport user ECOM plugins of shutdown. */
void CWorkerThread::RTransportUsers::Shutdown(TBool aAlreadyDead)
	{
	for(TInt idx = Count() - 1; idx >= 0; --idx)
		{
		TTransportUser& user = operator[](idx);
		user.iUser->Shutdown(aAlreadyDead, *user.iUserStorage);
		if(!aAlreadyDead)
			{
			REComSession::DestroyedImplementation(user.iDtorId);
			delete user.iUser;
			delete user.iUserStorage;
			}
		}
	Close();
	}

/** Notify all active transport user ECOM plugins that a thread have died. */
void CWorkerThread::RTransportUsers::NotifyPeerDeath(TWorkerId aPeer)
	{
	for(TInt idx = Count() - 1; idx >= 0; --idx)
		{
		TTransportUser& user = operator[](idx);
		user.iUser->OnPeerDeath(aPeer, *user.iUserStorage);
		}
	}

//
// TRequestWrapper
//

EXPORT_C TRequestWrapper::TRequestWrapper()
: iStatus(NULL),
  iPeerThread(NULL),
  iSyncLocalCall(EFalse)
	{
	}

EXPORT_C TRequestWrapper::TRequestWrapper(TRequestStatus& aStatus, TBool aSyncLocalCall)
: iStatus(&aStatus),
  iPeerThread(NULL),
  iSyncLocalCall(aSyncLocalCall)
	{
	}

/**
Complete the request in a way relevant to the context:
    -# If within the same thread, just set the status member as value is later used further up in the call stack.
	-# If within other thread, use "real" completion function to signal status/thread semaphore.
*/
EXPORT_C void TRequestWrapper::RequestComplete(TInt aReason)
	{
	if(iSyncLocalCall)
		{
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("TRequestWrapper::RequestComplete() %08x = %d (local, synchronous)"), iStatus, aReason));
		// Don't need to complete request since it's a synchronous call down within
		// the requesting thread, which isn't waiting for the return
		__ASSERT_DEBUG(iStatus, User::Panic(KSpecAssert_ESockSSocks_thrd, 18));	// no valid case for completing twice
		*iStatus = aReason;
		iStatus = NULL;
		}
	else
		{
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("TRequestWrapper::RequestComplete() %08x = %d (requester tid = %x)"), iStatus, aReason, static_cast<TUint>(iPeerThread->Id())));
		iPeerThread->RequestComplete(iStatus, aReason);
		}
	}

//
// TTransportUser
//
TInt TTransportUser::Compare(const TTransportUser& aLHS, const TTransportUser& aRHS)
	{
	if(aLHS.iInterfaceId < aRHS.iInterfaceId)
		{
		return -1;
		}
	else if(aLHS.iInterfaceId > aRHS.iInterfaceId)
		{
		return 1;
		}
	return 0;
	}

/**
The socket server thread. This is where control will resume when the RootServer starts an
ESock instance.
This function creates the worker thread object and starts the active scheduler.
*/
EXPORT_C TInt CWorkerThread::ThreadEntrypoint(TAny* aArg)
	{
#if defined (__EPOC32__)
	// Ensure that our thread has a handle on this library.
	RLibrary sockLib;
	__ASSERT_ALWAYS(sockLib.Load(ESOCK_SERVER_LIB_NAME)==KErrNone, Fault(ESvrStartServer));
#endif

	CTrapCleanup* tc=CTrapCleanup::New();
	if (!tc)
		{
		return KErrNoMemory;
		}

	CSocketScheduler* ss = CSocketScheduler::New();

	TCFModuleInfo* moduleInfo = static_cast<TCFModuleInfo*>(aArg);

	/* We instantiate our CWorkerThread here, but it is deleted through the
	destruction of SockManGlobals by the CShutdownWatchdog. */
	TRAPD(res, CWorkerThread::NewL(moduleInfo));
	if(res != KErrNone)
		{
		return res;
		}

	RThread::Rendezvous(KErrNone);
	CActiveScheduler::Start();

	/**
	   This is deleted here because the globals can theoretically be used
	   at any time up until when the active scheduler stops */
	CSockManData* globals=SockManGlobals::Get();
	delete globals;
	globals = NULL;

	REComSession::FinalClose();

	TInt returncode=KErrNone;

	delete tc;
	delete ss;
	return returncode;
	}


