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

#include "SS_conn.H"
#include "ss_connstates.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include "ss_subconn.h"
#include <comms-infras/ss_mcprnodemessages.h>
#include <elements/sm_core.h>
#include <commdbconnpref.h> //TCommDbConnPref
#include <ss_glob.h> //TCommDbConnPref
#include <connpref.h> //TConnPrefList
#include <etelpckt.h>
#include <comms-infras/metadata.h>
#include <comms-infras/esock_params.h> //TConnAPPref
#include <comms-infras/esock_params_internal.h> //TConnCSRPref

#include <comms-infras/es_connectionservparameterbundle.h> // CConnectionServParameterBundle
#include <comms-infras/es_accesspointstatus.h> // TAccessPointStatusFilter
#include <comms-infras/cs_connservparams.h> // XAccessPointGenericParameterSet
#include <comms-infras/cs_connservparams_internal.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#include <nifvar_internal.h>
#endif

#include <elements/nm_messages_child.h>
#include <elements/nm_messages_peer.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_legacy.h>
#include <comms-infras/ss_nodemessages_tiermanager.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include <comms-infras/ss_nodemessages_selector.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_tiermanagerfactory.h>

#define SYMBIAN_NETWORKING_UPS

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_upsaccesspointconfigext.h>
#include <comms-infras/upsmessages.h>
#endif
#include <comms-infras/ss_roles.h>
#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockscnsts, "ESockSSockscnsts");
#endif

using namespace ESock;
using namespace SubSessActivities;
using namespace NetStateMachine;
using namespace ConnStates;
using namespace ConnActivities;
using namespace ConnectionServ;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;

//#ifdef LOG
//	#define KESockActivityTag KESockCoreProviderTag
//	_LIT8(KESockActivitySubTag, "esockactivity");
//#endif

_LIT (KCConnectionPanic,"CConnectionPanic");

void ConnPanic(TInt aCode)
	{
	User::Panic(KCConnectionPanic, aCode);
	}

//
//Start

DEFINE_SMELEMENT(ConnStates::TErrorIfAlreadyStartedAttached, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TErrorIfAlreadyStartedAttached::DoL()
	{
	__ASSERT_DEBUG((subsessmessage_cast<ECNStart>(&iContext.iMessage))
		|| (subsessmessage_cast<ECNAttach>(&iContext.iMessage)),
			ConnPanic(KPanicIncorrectMessage));
	RNodeInterface* sp = iContext.Node().ServiceProvider();
	User::LeaveIfError(sp? KErrAlreadyExists : KErrNone);
	}

DEFINE_SMELEMENT(ConnStates::TParseECNStart, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TParseECNStart::DoL()
	{
	__ASSERT_DEBUG((subsessmessage_cast<ECNStart>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& ac = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

    if (ac.Function() == ECNSetStartPrefs)
    	{
        //Extract preferences if ECNSetStartPrefs
    	TConnPref prefs;
		ac.ReadL(0, prefs);

   		if(prefs.ExtensionId() != TConnPref::EConnPrefEComList)
  			{
  			ac.SelectionPrefs().SetPrefs(prefs);
  			}
  		else
  			{
  			RBuf8 buffer;
  			TInt length = ac.GetDesLengthL(0);
  			buffer.CreateL(length);
  			CleanupClosePushL(buffer);
  			ac.ReadL(0, buffer);

  			TConnPrefList* prefsList = TConnPrefList::NewL();
  			CleanupStack::PushL(prefsList);
  			TPtrC8 ptr(buffer);
  			TInt ret = prefsList->Load(ptr);
  			User::LeaveIfError(ret);

  			//This loop is used to check that there is no more than 1 instance
  			//of TConnAPPref in the list passed to the CSR. This is not allowed
  			//(as it may have adverse effects in the construction of the stack)
  			//KErrArugment is returned if there is more than 1 instance present
  			TInt instances = 0;
  			for(TInt i=0; i<prefsList->Count(); i++)
  				{
  				TConnAPPref* pref = smetadata_cast<TConnAPPref>((*prefsList)[i]);
  				if(pref)
  				    {
  					instances++;
  				    }
  				}
  			if(instances > 1)
  			    {
  				User::Leave(KErrArgument);
  			    }

  			TConnCSRPref* csr = TConnCSRPref::NewL();
  			CleanupStack::PushL(csr);
  			ac.SetCSRPreferences(csr);
  			csr->SetScope(TSelectionPrefs::EExplicitConnection);
  			prefsList->AppendL(csr);

  			ac.SelectionPrefs().SetPrefs(*prefsList);
    		ac.SetConnPrefList(prefsList);

    		CleanupStack::Pop(csr);
    		CleanupStack::Pop(prefsList);
    		CleanupStack::PopAndDestroy(&buffer);
    		}
		
		// Complete the ECNSetStartPrefs, activity will be held until the 2nd IPC of the Start with Prefs
   		// call is received. Null iSubSession so that the 2nd IPC can be aquired by the activity.
   		ac.iMessage.Complete(KErrNone);
   		ac.iSubSession = NULL;
		}
    else
        {
		ac.iStartReceived = ETrue;
        ac.SetSelectionScope(TSelectionPrefs::EExplicitConnection);
        }
	}

DEFINE_SMELEMENT(ConnStates::TSendStartingSelectionStateChange, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TSendStartingSelectionStateChange::DoL()
	{
	//Send the relevant progress (don't use ReceivedL or you will warp), also use Null Activity id.
	RNodeInterface::OpenPostMessageClose(iContext.NodeId(), iContext.NodeId(), TCFMessage::TStateChange(TStateChange(KStartingSelection,KErrNone)).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TClearProgressQueue, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TClearProgressQueue::DoL()
	{
	iContext.Node().iProgressQueue.Reset();		// clear progress notification cache
	iContext.Node().ResetLastProgressError();	// clear last progress in error
	// Initialise iLastProgress with an invalid progress rather than 0 (which is actually KConnectionUninitialised)
	// otherwise we can end up unintentionally filtering out a KConnectionUninitialised progress.
	const TInt KInvalidProgress = -1;
	iContext.Node().iLastProgress = TStateChange(KInvalidProgress, 0);
	iContext.Node().iLastProgressError = TStateChange();
	}

DEFINE_SMELEMENT(ConnStates::TRequestCSRCreation, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TRequestCSRCreation::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	__ASSERT_DEBUG(static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity).CSR().IsNull(), ConnPanic(KPanicExpectedNoCSR));
	iContext.iNodeActivity->PostRequestTo(SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)), TCFInternalEsock::TCreateCSR(iContext.Node().iTierId).CRef(), EFalse);
	}

DEFINE_SMELEMENT(ConnStates::TProcessCSRCreation, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessCSRCreation::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& ac = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);
	__ASSERT_DEBUG(ac.CSR().IsNull(), User::Panic(KSpecAssert_ESockSSockscnsts, 1));
	ac.SetCSR(message_cast<TCFInternalEsock::TCSRCreated>(iContext.iMessage).iNodeId);

	
	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), ac.CSR(),
		TCFMessage::TProvisionConnectionInfo(iContext.Node().ConnectionInfo()).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TSelectMetaPlane, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TSelectMetaPlane::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& ac = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

    //Diagnostic only. If attach then it must be ESelectFromExisting
    __ASSERT_DEBUG(!iContext.Node().IsMonitor()
    	|| ac.SelectionPrefs().Scope() & TSelectionPrefs::ESelectFromExisting, User::Panic(KSpecAssert_ESockSSockscnsts, 2));

	if(ac.SelectionPrefs().Prefs().ExtensionId() != TConnPref::EConnPrefEComList)
		{
		ac.SelectionPrefs().SetSubSessionUniqueId(iContext.Node().UniqueId());
		iContext.iNodeActivity->PostRequestTo(
			ac.CSR(),
			TCFSelector::TSimpleSelect(ac.SelectionPrefs()).CRef()
			);
		}
	else
		{
		ac.CSRPreferences()->SetSubSessionUniqueId(iContext.Node().UniqueId());
		RConnPrefList handle;
		handle.Open(ac.ConnPrefList());

		iContext.iNodeActivity->PostRequestTo(
			ac.CSR(),
			TCFSelector::TSelect(handle).CRef()
			);
		}
	}

DEFINE_SMELEMENT(ConnStates::TSendFinishedSelectionStateChange, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TSendFinishedSelectionStateChange::DoL()
	{
	//Send the relevant progress (don't use ReceivedL or you will warp), also use Null Activity id.
	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.NodeId(),
		TCFMessage::TStateChange(TStateChange(KFinishedSelection,KErrNone)).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TJoinReceivedSCpr, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TJoinReceivedSCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	__ASSERT_DEBUG(iContext.iPeer == iContext.Node().ServiceProvider(), ConnPanic(KPanicExpectedNoServiceProvider));

	TCFServiceProvider::TCommsBinderResponse& br = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);
    RNodeInterface* scpr = iContext.Node().AddClientL(br.iNodeId, TClientType(TCFClientType::EServProvider, TCFClientType::EDefault));

    //If this is attach, we need to see if we are a monitor or not
    TCFClientType clientType(TCFClientType::ECtrl);

    TUint selPrefs = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity).SelectionPrefs().Flags();
    if (selPrefs & TSelectionPrefs::EMonitor)
    	{
		clientType.SetFlags(TCFClientType::EMonitor);
    	}

	// If it is an attach set the flag cause it is used by NetUPS to check if a JoinRequest comes from an RConnection::Attach
	if (selPrefs & TSelectionPrefs::EAttach)
    	{
		clientType.SetFlags(TCFClientType::EAttach);
    	}

    iContext.iNodeActivity->PostRequestTo(*scpr,
    	TCFServiceProvider::TJoinRequest(iContext.NodeId(), clientType).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TJoinReceivedMcpr, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TJoinReceivedMcpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	__ASSERT_DEBUG(iContext.Node().ServiceProvider()==NULL, ConnPanic(KPanicExpectedNoServiceProvider));

	TCFSelector::TSelectComplete& sc = message_cast<TCFSelector::TSelectComplete>(iContext.iMessage);
	RNodeInterface* mcpr = iContext.Node().AddClientL(sc.iNodeId,TClientType(TCFClientType::EServProvider, TCFClientType::EAvailabilityProvider));


	iContext.iNodeActivity->PostRequestTo(*mcpr,
		TCFServiceProvider::TJoinRequest(iContext.NodeId(), TCFClientType(TCFClientType::ECtrl)).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TRequestCommsBinder, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TRequestCommsBinder::DoL()
	{
    __ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
    RNodeInterface* sp = iContext.Node().ServiceProvider();
    __ASSERT_DEBUG(sp, ConnPanic(KPanicNoServiceProvider));
    iContext.iNodeActivity->PostRequestTo(*sp,
    	TCFServiceProvider::TCommsBinderRequest(TSubConnOpen::EAttachToDefault).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TRequestCommsBinderFromMcpr, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TRequestCommsBinderFromMcpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EAvailabilityProvider));
	RNodeInterface* mcpr = iter[0];
	__ASSERT_DEBUG(mcpr, ConnPanic(KPanicNoAvailabilityProvider));
	iContext.iNodeActivity->PostRequestTo(*mcpr,
		TCFServiceProvider::TCommsBinderRequest().CRef());
	}

DEFINE_SMELEMENT(ConnStates::TProcessBinderResponseForCpr, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessBinderResponseForCpr::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	__ASSERT_DEBUG(iContext.Node().ServiceProvider()==NULL, ConnPanic(KPanicExpectedNoServiceProvider));

	TCFServiceProvider::TCommsBinderResponse& br = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);

	RNodeInterface* sp = iContext.Node().AddClientL(br.iNodeId, TClientType(TCFClientType::EServProvider, TCFClientType::EActive));
    __ASSERT_DEBUG(iContext.Node().ServiceProvider()==sp, ConnPanic(KPanicNoServiceProvider)); //[RZ] revise this, possibly overdefensive

	//If this is attach, we need to see if we are a monitor or not
	TCFClientType clientType;

	TUint selPrefs = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity).SelectionPrefs().Flags();
	if (selPrefs & TSelectionPrefs::EMonitor)
		{
		clientType.SetFlags(TCFClientType::EMonitor);
		iContext.Node().iIsMonitor = ETrue;
		}

	if (selPrefs & TSelectionPrefs::EAttach)
		{
		clientType.SetFlags(TCFClientType::EAttach);
		}
	
	iContext.iNodeActivity->PostRequestTo(*sp,
		TCFServiceProvider::TJoinRequest(iContext.NodeId(), clientType).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TStartConnection, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TStartConnection::DoL()
	{
	RNodeInterface* cpr = iContext.Node().ServiceProvider();
	__ASSERT_DEBUG(cpr, ConnPanic(KPanicNoServiceProvider));

	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
    iContext.iNodeActivity->PostRequestTo(*cpr, TCFServiceProvider::TStart().CRef());
	}

DEFINE_SMELEMENT(ConnStates::TErrorOrCancel, NetStateMachine::MStateFork, ConnStates::TContext)
/**
If the activity has been cancelled, return KCancelTag else return KErrorTag
 */
TInt ConnStates::TErrorOrCancel::TransitionTag()
	{
	if (iContext.Activity()->Error() == KErrCancel)
		{
		return ConnActivities::CStartAttachActivity::KCancelTag;
		}
	else
		{
		__ASSERT_DEBUG(iContext.Activity()->Error() != KErrNone, User::Panic(KSpecAssert_ESockSSockscnsts, 3));		// meant to be called in error conditions only
		return KErrorTag;
		}
	}

//
//Attach
DEFINE_SMELEMENT(ConnStates::TParseECNAttach, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TParseECNAttach::DoL()
	{
	__ASSERT_DEBUG((subsessmessage_cast<ECNAttach>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& ac = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

	//Read preferences
	const TUint KMaxConnectionInfoLength = 32;
	TBuf8<KMaxConnectionInfoLength> info;
	ac.ReadL(1,info);
	if (info.Length() < KConnInfoPart)
		{
		User::Leave(KErrArgument);
		}

	TUint monitorMode = (RConnection::TConnAttachType)ac.Int0() == RConnection::EAttachTypeMonitor?
		TSelectionPrefs::EMonitor : 0;
	// The attach flag should always be set since it is used by NetUPS to check if a JoinRequest comes from an RConnection::Attach
	ac.SetSelectionFlags(monitorMode | TSelectionPrefs::EAttach);

	TConnArgBase& connInfo = *((TConnArgBase*)&info[0]);
	if (connInfo.Version() < KConnArgVersion3)
		{
        TConnectionInfo& connI = static_cast<TConnectionInfo&>(connInfo);
        TCommDbConnPref pref;
		pref.SetIapId(connI.iIapId);
    	ac.SelectionPrefs().SetPrefs(pref);
    	ac.SetSelectionScope(TSelectionPrefs::ESelectFromExisting | TSelectionPrefs::EExplicitConnection);
		}
	else
		{
		__ASSERT_DEBUG(connInfo.Version() == KConnArgVersion3, User::Panic(KSpecAssert_ESockSSockscnsts, 4)); //Currently no support for > KConnArgVersion3;
		TConnProviderInfo& provinfo = static_cast<TConnProviderInfo&>(connInfo);
		__ASSERT_DEBUG(iContext.Node().iTierId.iUid == provinfo.iInfo[0], User::Panic(KSpecAssert_ESockSSockscnsts, 5));
    	ac.SelectionPrefs().SetPrefs(TConnProviderInfoPref(provinfo));
    	ac.SetSelectionScope(TSelectionPrefs::ESelectFromExisting);
		}
	}

DEFINE_SMELEMENT(ConnStates::TCompleteLegacyAttach, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TCompleteLegacyAttach::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	iContext.Node().iLegacyConnection.CompleteAttachL(static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity).SelectionPrefs());
	}


//
//WaitForIncoming
DEFINE_SMELEMENT(ConnStates::TRequestIncomingConnection, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TRequestIncomingConnection::DoL()
	{
	__ASSERT_DEBUG((subsessmessage_cast<ECNWaitForIncoming>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
	RNodeInterface* sp = iContext.Node().ServiceProvider();
	User::LeaveIfError(sp ? KErrNone : KErrNotReady);

    iContext.iNodeActivity->PostRequestTo(*sp,
    	TCFServiceProvider::TCommsBinderRequest(TSubConnOpen::EWaitForIncoming).CRef());
	}

DEFINE_SMELEMENT(ConnStates::TProcessIncomingConnection, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessIncomingConnection::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
    CSubConnection* waitingSubConn = iContext.Node().Session()->CSubConnectionFromHandle(static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity).Int0());
	User::LeaveIfError(waitingSubConn != NULL ? KErrNone : KErrCancel);
    RNodeInterface* waitingSubConnPeer = iContext.Node().FindClient(waitingSubConn->Id()); //To my surprise SC is a peer of this (so must use peer handle when talking to it)
    __ASSERT_DEBUG(waitingSubConnPeer, ConnPanic(KPanicNoDataClient));
    	
    TCFServiceProvider::TCommsBinderResponse& binderResp = message_cast<TCFServiceProvider::TCommsBinderResponse>(iContext.iMessage);
    iContext.iNodeActivity->PostRequestTo(*waitingSubConnPeer,
        TCFDataClient::TBindTo(binderResp.iNodeId).CRef());
	}


//
//Stop
DEFINE_SMELEMENT(ConnStates::TSendStopConnection, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TSendStopConnection::DoL()
	{
	__ASSERT_DEBUG((subsessmessage_cast<ECNStop>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	CESockClientActivityBase& ac = static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);

	//Convert the received enum (the stop type) into an error code that will be passed to the interface clients
	TInt stopCode = KErrCancel;
	switch (ac.Int0())
		{
		case RConnection::EStopNormal:
			stopCode = KErrCancel;
			break;
		case RConnection::EStopAuthoritative:
			stopCode = KErrConnectionTerminated;
			break;
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnsts, 6));
		}

	RNodeInterface* sp = iContext.Node().ServiceProvider();
	if(sp)
		{ // We can only post the 'stop' if we've got a service provider and its not already leaving (e.g., gone down).
		if (!(sp->Flags() & TClientType::ELeaving))
		    {
			sp->PostMessage(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), TCFServiceProvider::TStop(stopCode).CRef());
		    }
		else
			{
			MESH_LOG((KESockConnectionTag, _L8("ConnStates::TSendStopConnection:\tDoL - IGNORING POST!")));
			}
		}
	else
		{ // Otherwise just set the activity idle and bail out.
		iContext.Activity()->SetIdle();
		}
	}

DEFINE_SMELEMENT(ConnStates::TConnectionSendStopSCPR, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TConnectionSendStopSCPR::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));

	TInt stopCode = 0;

	switch (static_cast<CESockClientActivityBase*>(iContext.iNodeActivity)->Int1())
		{
	case RConnection::EStopNormal:
		stopCode = KErrCancel;
		break;

	case RConnection::EStopAuthoritative:
		stopCode = KErrConnectionTerminated;
		break;

	default:
		__ASSERT_DEBUG( false , User::Panic(KSpecAssert_ESockSSockscnsts, 7)); // CConnection should have verified the parameters before stopping the subconnection
		}

	// Send the stop message to the subconnection.
	RNodeInterface* scpr = static_cast<CConnection&>(iContext.Node()).DefaultSubConnectionServiceProvider();
	__ASSERT_DEBUG( scpr , User::Panic(KSpecAssert_ESockSSockscnsts, 8)); // subconnection must exist or the stop operation should not have been initiated
	scpr->PostMessage(
		TNodeCtxId(iContext.ActivityId(), iContext.NodeId()),
		TCFServiceProvider::TStop(stopCode).CRef()
		);
	}

DEFINE_SMELEMENT(ConnStates::TCancelStartOrAttachConnection, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TCancelStartOrAttachConnection::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));

	if (iContext.Node().FindActivityById(ECFActivityStart))
		{
		RClientInterface::OpenPostMessageClose(
			TNodeCtxId(ECNStart, iContext.NodeId()),
			TNodeCtxId(ECNStart, iContext.NodeId()),
			TEBase::TCancel().CRef());
		}
	else if (iContext.Node().FindActivityById(ECFActivityConnectionAttach))
		{
		RClientInterface::OpenPostMessageClose(
			TNodeCtxId(ECNAttach, iContext.NodeId()),
			TNodeCtxId(ECNAttach, iContext.NodeId()),
			TEBase::TCancel().CRef());
		}
	}

//
//Close
DEFINE_SMELEMENT(ConnStates::TDetachDataClients, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TDetachDataClients::DoL()
	{
	while (ETrue)
    	{
    	TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
    	RNodeInterface* client = iter[0];
    	if (client)
        	{
#if defined(__GCCXML__)
        	CSubConnection& subconn = *reinterpret_cast<CSubConnection*>(&client->RecipientId().Node());
#else
        	CSubConnection& subconn = *mcfnode_cast<CSubConnection>(&client->RecipientId().Node());
#endif
        	iContext.Node().RemoveClient(subconn.Id());
        	subconn.RemoveClient(iContext.NodeId());
        	}
    	else
        	{
        	break;
        	}
    	}
	}

DEFINE_SMELEMENT(ConnStates::TStartOrAttachActive, NetStateMachine::MStateFork, ConnStates::TContext)
TInt ConnStates::TStartOrAttachActive::TransitionTag()
	{
	if (iContext.Node().FindActivityById(ECFActivityStart) || iContext.Node().FindActivityById(ECFActivityConnectionAttach))
		{
		return MeshMachine::KActiveTag;
		}
	return KNoTag;
	}

DEFINE_SMELEMENT(ConnStates::TAwaitingGoneDown, NetStateMachine::MState, ConnStates::TContext)
TBool ConnStates::TAwaitingGoneDown::Accept()
	{
	if (!iContext.iMessage.IsMessage<TCFControlClient::TGoneDown>())
    	{
    	return EFalse;
    	}

    //TODO - fix this defect:
    //1) TGoneDown should not be sent when stopping or closing
    //2) FindActivityById(ECFActivityStop) is not safe
    //3) why the condition iContext.Node().ServiceProvider()? Who can be sending TGoneDown if not the service provider?
    bool stoppingOrClosing = iContext.Node().FindActivityById(ECFActivityStop) || iContext.Node().FindActivityById(ECFActivityDestroy);
    if (!stoppingOrClosing && iContext.Node().ServiceProvider() &&
        *iContext.Node().ServiceProvider() == iContext.iSender)
        {
        return ETrue;
        }
    iContext.iMessage.ClearMessageId();
    return EFalse;
	}



//----------------------------------------------------
//Handling Progress/TStateChange
DEFINE_SMELEMENT(ConnStates::TAwaitingStateChange, NetStateMachine::MState, ConnStates::TContext)
TBool ConnStates::TAwaitingStateChange::Accept()
    {
    TCFMessage::TStateChange* progressMsg = message_cast<TCFMessage::TStateChange>(&iContext.iMessage);
    if (progressMsg)
        {
        TStateChange& progress = progressMsg->iStateChange;
        
		// Check whether KDataTransferUnblocked is received and if yes, then traslate it to KConnectionUp (== KLinkLayerOpen)
		// the log only sees the translated version, this goes into the queue, so it alright I suppose.
		if (progress.iStage == KDataTransferUnblocked )
			{
			progress.iStage = KConnectionUp;    // KLinkLayerOpen
			}
		if (progress.iError == KErrForceDisconnected)
			{
			progress.iError = KErrDisconnected;
			}
        
		// CConnection trusts that locally generated progresses are to be trusted,
		// but some of the legacy progresses coming from the stack need some filtering.
		// Here's the filtering.
		if (iContext.iSender != iContext.Node().Id())
            {
            if (progress.iStage == KConnectionUninitialised)
                {
                //KConnectionUninitialised has been deprecated in the stack and it will be ignored
                //when reported by it. It is still valid towards the application. Here it is being reported by the stack.
                //The original KConnectionUninitialised denoted CAgentReference destruction and as such speculated too
                //much about the sturcture of the stack (notably: there isn't necesserily an agent anymore.
                //As defined KConnectionUninitialised would need to be reported after TCFControlClient::TGoneDown or TCFServiceProvider::TStopped,
                //except the stack decomposes after TCFControlClient::TGoneDown or TCFServiceProvider::TStopped and the progress path along with
                //it. It is therefore impossible for the stack to generate KConnectionUninitialised after
                //TCFControlClient::TGoneDown or TCFServiceProvider::TStopped. CConnection takes over and will generate KConnectionUninitialised
                //when detatching from the service providers (see ConnStates::TGenerateConnectionUninitialisedProgress)
                LOG( ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tProgressNotification(TInt aStage %d, TInt aError %d) - deprecated progress, ignoring"),
                        &(iContext.Node()), progress.iStage, progress.iError) );
                progressMsg->ClearMessageId();
                return EFalse;
                }   
            }
        LOG( ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tProgressNotification(TInt aStage %d, TInt aError %d)"),
            &(iContext.Node()), progress.iStage, progress.iError) );
        
        CConnection& cc = iContext.Node();
        if (cc.iLastProgress == progress)
            {
            progressMsg->ClearMessageId();            
            return EFalse; //ignore this duplicate
            }
        cc.iLastProgress = progress;

        if (progress.iError != KErrNone)
            {
            cc.iLastProgressError = progress; //Save last progress in error for use by LastProgressErrorL()
            }
        return ETrue;
        }
    return EFalse;
    }

//Progress & Progress Request
DEFINE_SMELEMENT(ConnStates::TEnqueueStateChange, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TEnqueueStateChange::DoL()
    {
    TStateChange& progress = message_cast<TCFMessage::TStateChange>(iContext.iMessage).iStateChange;
    //Diagnostic assertion.
    
    //If ECFActivityConnectionStateChangeRequest is running, it has not been
    //presented with the TStateChange message (channel activity id != 0?) which is a serious mistake.
    __ASSERT_DEBUG(iContext.Node().CountActivities(ECFActivityConnectionStateChangeRequest)==0, User::Panic(KSpecAssert_ESockSSockscnsts, 9));

    CConnection& cc = iContext.Node();
#ifdef ESOCK_LOGGING_ACTIVE
    // Check to see if the progress queue is full causing older progress to be discarded.
    // This has the potential to cause problems if a critical progress item is lost.
    // Normally the queue is large enough such that this doesn't happen but this log entry
    // serves as a warning if it ever does.
    if( cc.iProgressQueue.IsFull() )
        {
        LOG( ESockLog::Printf(KESockConnectionTag, _L("CConnection %08x:\tThe maximum progress queue size of %d has been exceeded - discarding old progress to make space for new item"), this, KMaxProgressQueueLength); )
        }
#endif
    
    //Add the progress to queue
    cc.iProgressQueue.Enque(progress);
    }


DEFINE_SMELEMENT(ConnStates::TProcessProgressRequest, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessProgressRequest::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
    CESockClientActivityBase& ac = static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);
    TUint selectedProgressStage = ac.Int1();
    TStateChange progress;
    TBool found = EFalse;

    __ASSERT_DEBUG((subsessmessage_cast<ECNProgressNotification>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
    
    //Process the queue looking for the progress of interest
    found = iContext.Node().iProgressQueue.Deque(progress);
    if (found && selectedProgressStage != KConnProgressDefault)
        {
        // For a selected progress request, dequeue entries until we find one which
        // matches the criteria.  If we dequeue all entries, fall through without
        // completing the message.  It it not considered useful to retain un-matching
        // entries on the queue if a selected progress request is pending.
        while (found)
            {
            if (progress.iStage == selectedProgressStage || progress.iError != KErrNone)
                {
                break;
                }
            found = iContext.Node().iProgressQueue.Deque(progress);
            }
        }

    if (found)
        {
        //We have found a progress of interest, finish
        TPckg<TStateChange> progressPkg(progress);
        ac.WriteL(0,progressPkg);
        ac.SetIdle(); //We are done
        }
    }

DEFINE_SMELEMENT(ConnStates::TProcessStateChange, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessStateChange::DoL()
    {
    __ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
    CESockClientActivityBase& ac = static_cast<CESockClientActivityBase&>(*iContext.iNodeActivity);
    TUint selectedProgressStage = ac.Int1();

    //Are we here as a result of receiving TStateChange (rather than ECNProgressNotification)?
    TCFMessage::TStateChange& progress = message_cast<TCFMessage::TStateChange>(iContext.iMessage);

    if (selectedProgressStage == KConnProgressDefault || 
        selectedProgressStage == progress.iStateChange.iStage || 
        KErrNone != progress.iStateChange.iError)
        {
        //We have found a progress of interest, finish
        TPckg<TStateChange> progressPkg(progress.iStateChange);
        ac.WriteL(0,progressPkg);
        ac.SetIdle(); //We are done
        }
    }


DEFINE_SMELEMENT(ConnStates::TGenerateConnectionUpProgress, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TGenerateConnectionUpProgress::DoL()
	{
	TCFMessage::TStateChange msg(TStateChange(KConnectionUp, KErrNone));
	RNodeInterface::OpenPostMessageClose(iContext.Node().Id(), iContext.Node().Id(), msg);
	}

DEFINE_SMELEMENT(ConnStates::TGenerateConnectionDownProgress, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TGenerateConnectionDownProgress::DoL()
	{
	// Do not self-post a message if there is a pending ConnectionClose activity.  When the ConnectionGoingDown
	// activity (calling this method) terminates, the ConnectionClose activity unparks (being synchronised
	// against ConnectionGoingDown), the CConnection is destroyed, and the subsequent self-dispatch
	// will panic.
    __ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	TInt error = iContext.iNodeActivity->Error();

	if (error == KErrNone && iContext.iMessage.IsMessage<TCFServiceProvider::TStopped>())
		{
		error = message_cast<TCFServiceProvider::TStopped>(iContext.iMessage).iValue;
		}

    __ASSERT_DEBUG(error != KErrNone, ConnPanic(KPanicIncorrectState));
	
	if (iContext.Node().CountActivities(ECFActivityDestroy) == 0)
		{
		TCFMessage::TStateChange msg(TStateChange(KConnectionDown, error));
		CConnection& cc = iContext.Node();
		RNodeInterface::OpenPostMessageClose(cc.Id(), cc.Id(), msg);
		}
	}

DEFINE_SMELEMENT(ConnStates::TGenerateConnectionUninitialisedProgress, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TGenerateConnectionUninitialisedProgress::DoL()
    {
    if (iContext.Node().CountActivities(ECFActivityDestroy) == 0)
        {//TGenerateConnectionUninitialisedProgress is called when the stack goes away 
         //below the CConnection's feet and it sometimes goes away because CConnection is
         //being ECNClosed and therefore sitting on an electric chair. It is risky, but
         //above all uselsess to send the progress when when app clearly doesn't want
         //to see it.
        TCFMessage::TStateChange msg(TStateChange(KConnectionUninitialised, KErrNone));
        RNodeInterface::OpenPostMessageClose(iContext.Node().Id(), iContext.Node().Id(), msg);
        }
    }

//
//Legacy enumeration
DEFINE_SMELEMENT(ConnStates::TAwaitingEnumResponse, NetStateMachine::MState, ConnStates::TContext)
TBool ConnStates::TAwaitingEnumResponse::Accept()
	{
	return iContext.iMessage.IsMessage<TCFInternalEsock::TLegacyConnectionEnumResponse>();
	}

DEFINE_SMELEMENT(ConnStates::TProcessEnumResponse, NetStateMachine::MStateTransition, ConnStates::TContext)
void ConnStates::TProcessEnumResponse::DoL()
	{
	TCFInternalEsock::TLegacyConnectionEnumResponse& msg = message_cast<TCFInternalEsock::TLegacyConnectionEnumResponse>(iContext.iMessage);
	iContext.Node().iLegacyConnection.CompleteDataPlaneEnumeration(address_cast<TNodeId>(iContext.iSender), msg.iValue1, reinterpret_cast<HBufC8*>(msg.iValue2), msg.iValue3);
	}


//
//CStartAttachActivity
MeshMachine::CNodeActivityBase* ConnActivities::CStartAttachActivity::NewWaitForIncomingConnectionActivityL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	//Leave from here will complete the client message
	User::LeaveIfError(aNode.CountActivities(ECFActivityStart)? KErrInUse : KErrNone);
	User::LeaveIfError(aNode.CountActivities(ECFActivityConnectionAttach)? KErrInUse : KErrNone);

	// When waiting for an incoming connection we will additionally check that the connection is already started
	User::LeaveIfError(aNode.CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)) > 0 ? KErrNone : KErrNotReady );

	return new (ELeave) ConnActivities::CStartAttachActivity(aActivitySig,aNode);
	}

MeshMachine::CNodeActivityBase* ConnActivities::CStartAttachActivity::NewStartConnectionActivityL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	//Leave from here will complete the client message
	User::LeaveIfError(aNode.CountActivities(ECFActivityStart)? KErrInUse : KErrNone);
	User::LeaveIfError(aNode.CountActivities(ECFActivityConnectionAttach)? KErrInUse : KErrNone);

	// When starting a connection we will additionally check that the connection is not already started
	User::LeaveIfError(aNode.CountClients<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider)) > 0 ? KErrInUse : KErrNone);

	return new (ELeave) ConnActivities::CStartAttachActivity(aActivitySig,aNode);
	}

ConnActivities::CStartAttachActivity::~CStartAttachActivity()
	{
	// Call ABindingActivity base class to send TBindToComplete (before TDestroy)
	FinalReplyToOriginator(KErrAbort);

	if (!iCSR.IsNull())
		{
#ifndef __GCCXML__
		RNodeInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), iCSR, TEChild::TDestroy().CRef());
#endif
		iCSR.SetNull(); // = TNodeId::NullCommsId();
		}

	if(iConnPrefList)
		{
		while(iConnPrefList->Count() != 0)
			{
			SMetaData* pref = (*iConnPrefList)[0];
			if(pref != NULL)
				delete pref;
			iConnPrefList->Remove(0);
			}

		delete iConnPrefList;
		iConnPrefList = NULL;
		}
	}


TBool ConnActivities::CStartAttachActivity::Next(MeshMachine::TNodeContextBase& aContext)
    {
    // If the connection start preferences were set the activity will have completed the initial IPC and will
    // now be waiting upon the async ECNStart IPC to arrive before proceeding. The TCFInternalEsock::TSubSess
    // message will use the IPC number in the activity id part of aContext. MatchSender() will expect the
    // activity Id and so will never give a match. Here we ensure that our second IPC is matched and accepted.
    
    TCFInternalEsock::TSubSess* msg = message_cast<TCFInternalEsock::TSubSess>(&aContext.iMessage);
	if (!msg || (msg->iMessage.Function() != ECNStart && msg->iMessage.Function() != ECNSetStartPrefs))
        {
        return CNodeActivityBase::Next(aContext);
        }
    else if (iStartReceived)
        {
        // ECNSetStartPrefs should only ever be seen as the IPC in the TSubSess kick off message
        // ECNStart should only be seen once after an ECNSetStartPrefs or as the kick off message
        PanicClient(ETwice);
        aContext.iReturn = KErrInUse;
        return ETrue;
        }
    
    MESH_LOG((KESockConnectionTag, _L8("CStartAttachActivity::Next:\tAccepted ECNStart IPC after ECNSetStartPrefs")));
    TBool nextRet = ACore::Next(&aContext);
    if(nextRet)
        {
        MESH_LOG_ACTIVITY_EXT(KESockConnectionTag, this, &aContext, (_L8("CStartAttachActivity::Next:\tNext->transition")));
        }
    return nextRet;
    }

void ConnActivities::CStartAttachActivity::Cancel(MeshMachine::TNodeContextBase& aContext)
    {
    if (!iCSR.IsNull() && iCSR == PostedToNodeId())
        {//iCSR is a bit akward as it isn't stored in the Node's client array. MeshMachine auto-cancelling refuses to auto-forward TCancels
         //to nodes it can't verify they are alive. CStartAttachActivity can warrant iCSR is alive.
        RClientInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), PostedToNodeId(), TEBase::TCancel().CRef());
        SetError(KErrCancel);
        }
    else
        {
        CESockClientActivityBase::Cancel(aContext);
        }
    }


DEFINE_SMELEMENT(CStartAttachActivity::TNoTagOrStartPrefsSetTag, NetStateMachine::MStateFork, ConnStates::TContext)
TInt CStartAttachActivity::TNoTagOrStartPrefsSetTag::TransitionTag()
    {
    CStartAttachActivity& ac = static_cast<CStartAttachActivity&>(*iContext.iNodeActivity);

    if (ac.Function() == ECNSetStartPrefs)
        {
        return CStartAttachActivity::KStartPrefsSetTag | NetStateMachine::EForward;
        }
    return MeshMachine::KNoTag | NetStateMachine::EForward;
    }

DEFINE_SMELEMENT(CStartAttachActivity::TNoTagOrLegacyAttach, NetStateMachine::MStateFork, ConnStates::TContext)
TInt CStartAttachActivity::TNoTagOrLegacyAttach::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	const TConnPref& cp = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity).SelectionPrefs().Prefs();
	if (cp.ExtensionId()==TConnPref::EConnPrefCommDb)
		{//this is legacy attach
		return CStartAttachActivity::KExecuteLegacyAttach;
		}
	return KNoTag;
	}

DEFINE_SMELEMENT(CStartAttachActivity::TNoTagOrWaitAvailable, NetStateMachine::MStateFork, ConnStates::TContext)
TInt CStartAttachActivity::TNoTagOrWaitAvailable::TransitionTag()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& activity = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

	TConnPrefList* prefsList = activity.ConnPrefList();
	if (prefsList)
		{
		TBool startAuto = EFalse;
		for (TInt i = 0; i < prefsList->Count(); i++)
				{
				TConnAutoStartPref* pref = smetadata_cast<TConnAutoStartPref>((*prefsList)[i]);
				if(pref)
					{
					startAuto = ETrue;
					break;
					}
				}
		if (startAuto)
			{
			TBool &autoStart = activity.AutoStartPresent();
			autoStart = ETrue;
			iContext.iNodeActivity->ClearPostedTo();
			return KWaitAvailable;
			}

		}
	return KNoTag;
	}

DEFINE_SMELEMENT(CStartAttachActivity::TErrorTagOrWaitAvailableBackward, NetStateMachine::MStateFork, ConnStates::TContext)
TInt CStartAttachActivity::TErrorTagOrWaitAvailableBackward::TransitionTag()
	{
	__ASSERT_DEBUG((message_cast<TEBase::TError>(&iContext.iMessage)), ConnPanic(KPanicIncorrectMessage));
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& activity = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

	if (!activity.AutoStartPresent())
		{
		return KErrorTag;
		}

	TEBase::TError& msg = message_cast<TEBase::TError>(iContext.iMessage);
	if (msg.iValue == KErrUmtsMaxNumOfContextExceededByNetwork ||
		msg.iValue == KErrUmtsMaxNumOfContextExceededByPhone)
		{
		return KWaitAvailable | NetStateMachine::EBackward;
		}
	else
		{
		return KErrorTag;
		}
	}

DEFINE_SMELEMENT(CStartAttachActivity::TAwaitingAvailableOrError, NetStateMachine::MState, TContext)
TInt CStartAttachActivity::TAwaitingAvailableOrError::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFAvailabilityControlClient::TAvailabilityNotification>())
		{
		TCFAvailabilityControlClient::TAvailabilityNotification& msg = message_cast<TCFAvailabilityControlClient::TAvailabilityNotification>(iContext.iMessage);
		if (msg.iAvailabilityStatus.Score() > TAvailabilityStatus::EMinAvailabilityScore)
			{
			RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.iSender,
					TEBase::TCancel().CRef());
			return ETrue;
			}
		return EFalse;
		}
	else if (iContext.iMessage.IsMessage<TEBase::TError>())
		{
		TEBase::TError* msg = message_cast<TEBase::TError>(&iContext.iMessage);
		iContext.iNodeActivity->SetError(msg->iValue);
		return ETrue;
		}
	return EFalse;
	}


DEFINE_SMELEMENT(CStartAttachActivity::TSubscribeForAvailability, NetStateMachine::MStateTransition, TContext)
void CStartAttachActivity::TSubscribeForAvailability::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& activity = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

	TConnPrefList* connPrefList = activity.ConnPrefList();
	if (connPrefList && connPrefList->Count()>0)
		{
		TBool startAuto = EFalse;
		for (TInt i = 0; i < connPrefList->Count(); i++)
				{
				TConnAutoStartPref* pref = smetadata_cast<TConnAutoStartPref>((*connPrefList)[i]);
				if(pref)
					{
					startAuto = ETrue;
					break;
					}
				}
		if (startAuto)
			{
			TAvailabilitySubscriptionOptions subscriptionOptions;
			TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EServProvider, TCFClientType::EAvailabilityProvider));
			RNodeInterface* mcpr = iter[0];
			__ASSERT_DEBUG(mcpr, ConnPanic(KPanicNoAvailabilityProvider));
			RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), mcpr->RecipientId(),
				TCFAvailabilityProvider::TAvailabilityNotificationRegistration(subscriptionOptions).CRef());
			}
		}
	}

DEFINE_SMELEMENT(CStartAttachActivity::TAwaitingSelectCompleteOrError, NetStateMachine::MState, TContext)
TInt CStartAttachActivity::TAwaitingSelectCompleteOrError::Accept()
	{
	if (iContext.iMessage.IsMessage<TCFSelector::TSelectComplete>())
		{
		return ETrue;
		}
	else if(iContext.iMessage.IsMessage<TEBase::TError>())
		{
		TEBase::TError& errorMsg = message_cast<TEBase::TError>(iContext.iMessage);
		iContext.Activity()->SetError(errorMsg.iValue);
		return ETrue;
		}

	return EFalse;
	}

DEFINE_SMELEMENT(CStartAttachActivity::TSetIdleIfStopOutstanding, NetStateMachine::MStateTransition, TContext)
void CStartAttachActivity::TSetIdleIfStopOutstanding::DoL()
	{
	__ASSERT_DEBUG(iContext.iNodeActivity, ConnPanic(KPanicNoActivity));
	ConnActivities::CStartAttachActivity& activity = static_cast<ConnActivities::CStartAttachActivity&>(*iContext.iNodeActivity);

	// If there is a stop activity outstanding then we set this start activity idle
	TInt numRunningStopActivities = iContext.Node().CountActivities(ESock::ECFActivityStop);
	if (numRunningStopActivities > 0)
		{
		activity.SetIdle();
		}
	}


#ifdef SYMBIAN_NETWORKING_UPS

DEFINE_SMELEMENT(CStartAttachActivity::TSendPolicyCheckRequestToServiceProvider, NetStateMachine::MStateTransition, ConnStates::TContext)
void CStartAttachActivity::TSendPolicyCheckRequestToServiceProvider::DoL()
	{
	MPlatsecApiExt* platsec = NULL;
	iContext.Node().ReturnInterfacePtrL(platsec);		// from CConnection
	// TODO: what to do on an error (see TODO below)
    __ASSERT_DEBUG(platsec, User::Panic(KSpecAssert_ESockSSockscnsts, 18));

	TProcessId processId;
	TThreadId threadId;
	TInt err = platsec->GetProcessAndThreadId(processId, threadId);
	if (err != KErrNone)
		{
		const TNodeId& self = iContext.Node().Id();
		iContext.Node().SelfInterface().PostMessage(self, TEBase::TError(err).CRef());
		iContext.Activity()->ClearPostedTo();
		}
	else
		{
		// KErrCorrupt is a default value for the platsec result.  The platsec result must be filled-in by the provider
		// and not ESock because only the provider knows the (technology specific) policy to check against.
		// Destination address is not valid in this context, so it is just a null descriptor.

		const TPolicyCheckRequestParams params(processId, threadId, KErrCorrupt, KNullDesC8(), TUpsDestinationAddrType(ETNone), iContext.NodeId());

		const UpsMessage::TPolicyCheckRequest checkMsg(params);

		RNodeInterface* serviceProvider = iContext.Node().ServiceProvider();

        __ASSERT_DEBUG(serviceProvider, User::Panic(KSpecAssert_ESockSSockscnsts, 19));
		iContext.Activity()->PostRequestTo(*serviceProvider, checkMsg);
		}
	}

DEFINE_SMELEMENT(CStartAttachActivity::TNoTagOrUpsErrorTag, NetStateMachine::MStateFork, ConnStates::TContext)
TInt CStartAttachActivity::TNoTagOrUpsErrorTag::TransitionTag()
	{
	UpsMessage::TPolicyCheckResponse* policyCheckResponseMsg = message_cast<UpsMessage::TPolicyCheckResponse>(&iContext.iMessage);
	if (policyCheckResponseMsg && policyCheckResponseMsg->iValue != KErrNone)
		{
		iContext.Activity()->SetError(policyCheckResponseMsg->iValue);
		return KUpsErrorTag;
		}
	else
		{
		TEBase::TError* errorMsg = message_cast<TEBase::TError>(&iContext.iMessage);
		// TODO: there is already code for this isn't there?  A template that checks for TError and
		// sets the activity in error?
		if (errorMsg)
			{
			iContext.Activity()->SetError(errorMsg->iValue);
			return KUpsErrorTag;
			}
		}
	return KNoTag;
	}
#endif

//
// CTierManagerActivity

MeshMachine::CNodeActivityBase* ConnActivities::CTierManagerActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new(ELeave) ConnActivities::CTierManagerActivity(aActivitySig, aNode);
	}

// States

DEFINE_SMELEMENT(CTierManagerActivity::TAwaitingTierManager, NetStateMachine::MState, TContext)
TInt CTierManagerActivity::TAwaitingTierManager::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>())
		{
		TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);
		iContext.Activity()->iTierManager = msg.iNodeId;
		iContext.Activity()->ClearPostedTo();

		return ETrue;
		}
	else if(iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		iContext.Activity()->SetError(KErrCancel);

		return ETrue;
		}
	else if(iContext.iMessage.IsMessage<TEBase::TError>())
		{
		// Register the error
		TEBase::TError& errorMsg = message_cast<TEBase::TError>(iContext.iMessage);
		iContext.Activity()->SetError(errorMsg.iValue);

		return ETrue;
		}

	return EFalse;
	}

DEFINE_SMELEMENT(CTierManagerActivity::TAwaitingJoinComplete, NetStateMachine::MState, TContext)
TInt CTierManagerActivity::TAwaitingJoinComplete::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFPeer::TJoinComplete>())
		{
		return ETrue;
		}
	else if(iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		iContext.Activity()->SetError(KErrCancel);

		return ETrue;
		}
	else if(iContext.iMessage.IsMessage<TEBase::TError>())
		{
		// Register the error
		TEBase::TError& errorMsg = message_cast<TEBase::TError>(iContext.iMessage);
		iContext.Activity()->SetError(errorMsg.iValue);

		return ETrue;
		}

	return EFalse;
	}

// Transitions

DEFINE_SMELEMENT(CTierManagerActivity::TFindTierManager, NetStateMachine::MStateTransition, TContext)
void CTierManagerActivity::TFindTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	// Find out if the tier UID has been set.
	TBool tierUidIsNull = (iContext.Activity()->iTierUid == TUid::Null());

	// Panic in UDEB, Leave in UREL. The tier UID should be set in the derived
	// class's constructor
	__ASSERT_DEBUG(!tierUidIsNull, User::Panic(KSpecAssert_ESockSSockscnsts, 10));
	User::LeaveIfError(tierUidIsNull ? KErrNotReady : KErrNone);
	TAlwaysFindFactoryQuery query;
	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)),
		 TCFFactory::TFindOrCreatePeer(TCFPlayerRole::ETierMgrPlane, iContext.Activity()->iTierUid, &query).CRef());
	}

DEFINE_SMELEMENT(CTierManagerActivity::TJoinTierManager, NetStateMachine::MStateTransition, TContext)
void CTierManagerActivity::TJoinTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.Activity()->iTierManager,
		TCFServiceProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::EAdministrative)).CRef());

	iContext.Node().AddClientL(iContext.Activity()->iTierManager, TClientType(TCFClientType::EServProvider));
	}

// This solely exists separately from the other identical-looking version
//    in this file
//    ("AllInterfaceNotificationActivity::TLeaveTierManager::DoL()") because
//    the activities referenced by its TContext have iTierManager
//    at different offsets. This is because they have different parent
//    classes and, as such, have different member variable.
//
// The terrible thing is, this is usable in the other place,
//    and builds without complaint... then explodes when it tries
//    to send messages to a garbage iTierManager.
//
//    TAKE CARE OF IT!!!
DEFINE_SMELEMENT(CTierManagerActivity::TLeaveTierManager, NetStateMachine::MStateTransition, TContext)
void CTierManagerActivity::TLeaveTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.Activity()->iTierManager,
		TEChild::TLeft().CRef());

	iContext.Node().RemoveClient(iContext.Activity()->iTierManager);
	}

//
// CAllInterfaceNotificationActivity

CNodeActivityBase* ConnActivities::CAllInterfaceNotificationActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new (ELeave) ConnActivities::CAllInterfaceNotificationActivity(aActivitySig, aNode);
	}

ConnActivities::CAllInterfaceNotificationActivity::~CAllInterfaceNotificationActivity()
	{
	//Remove the only client here
	RNodeInterface* aux = iNode.GetFirstClient<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EAux));
	if (aux)
		{
		iNode.RemoveClient(aux->RecipientId());
		}

    if (iRegisteredForInterfaceStateChanges)
        {
        CCommsFactoryBase* ipProtoCprFactory = IpProtoCprFactory();
        if (ipProtoCprFactory)
            {
            Factories::IFactoryNotify itf(this, GetVTable());
            ipProtoCprFactory->DeRegisterNotifier(itf);
            }
        }

	delete iAllInterfaceNotificationWorker; //delete this node (why via iAllInterfaceNotificationWorker??)
	}

/**
Called by the IpProtoCpr factory to propagate an interface state change from IpProtoCpr to registered
observers (suc as ourself).  If the interface is restarting, then we need to insert an EInterfaceDown,
EInterfaceUp "toggle" sequence into the AllInterfaceNotification queue.  This is because, ordinarily,
EInterfaceUp and EInterfaceDown are generated when the IpProtoCpr itself is created and destroyed,
but not if it is restarted before being destroyed. 
*/
void CAllInterfaceNotificationActivity::InterfaceStateChangeNotification(const TDesC8& aInfo)
    {
    if (aInfo.Length() == sizeof(TInterfaceNotification))
        {
        const TInterfaceNotification* const info = reinterpret_cast<const TInterfaceNotification*>(aInfo.Ptr());
        if (info && info->iState == EInterfaceRestarting)
            {
            LOG( ESockLog::Printf(KESockConnectionTag, _L("CAllInterfaceNotificationActivity %08x:\tInterfaceStateChangeNotification(): CConnection %08x, IapId %d, NetId %d"),
                this, &iAllInterfaceNotificationWorker->iConnection, info->iConnectionInfo.iIapId, info->iConnectionInfo.iNetId  ));

            AConnectionLegacy& legacy = iAllInterfaceNotificationWorker->iConnection.iLegacyConnection;
            TInterfaceNotification notification(*info);
            notification.iState = EInterfaceDown;
            legacy.iNotificationQueue.Enque(notification);
            notification.iState = EInterfaceUp;
            legacy.iNotificationQueue.Enque(notification);
            legacy.CompleteAllInterfaceNotificationL(KErrNone);
            }
        }
    }

CCommsFactoryBase* CAllInterfaceNotificationActivity::IpProtoCprFactory() const
    {
    CConnectionFactoryContainer& container = *static_cast<CPlayer&>(iAllInterfaceNotificationWorker->iConnection.Player()).SockManGlobals()->iConnectionFactories;
    return static_cast<CCommsFactoryBase*>(container.FindFactory(TUid::Uid(AConnectionLegacy::KIPProtoConnectionProviderFactoryUid)));
    }

// Registration table for receiving InterfaceStateChangeNotification() events.
const Factories::TAnyFn CAllInterfaceNotificationActivity::iInterfaceVTableF[] =
    {
    (Factories::TAnyFn)1, // number of methods. the following entries must be in this order!
    (Factories::TAnyFn)(Factories::TFactoryNotify<CAllInterfaceNotificationActivity>::Notification)
    };

// States

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAwaitingStart, NetStateMachine::MState, TContext)
TInt AllInterfaceNotificationActivity::TAwaitingStart::Accept()
	{
	return iContext.iMessage.IsMessage<TCFServiceProvider::TStart>();
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAwaitingTierManager, NetStateMachine::MState, TContext)
TInt AllInterfaceNotificationActivity::TAwaitingTierManager::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>())
		{
		TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>(iContext.iMessage);
		iContext.Activity()->iTierManager = msg.iNodeId;
		iContext.Activity()->ClearPostedTo();

		return ETrue;
		}
	return EFalse;
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAwaitingJoinComplete, NetStateMachine::MState, TContext)
TInt AllInterfaceNotificationActivity::TAwaitingJoinComplete::Accept()
	{
	return iContext.iMessage.IsMessage<TCFPeer::TJoinComplete>();
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAwaitingLinkNotification, NetStateMachine::MState, TContext)
TInt AllInterfaceNotificationActivity::TAwaitingLinkNotification::Accept()
	{
	return (iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierNotification>() ||
			iContext.iMessage.IsMessage<TEBase::TCancel>() ||
			iContext.iMessage.IsMessage<TEBase::TError>() );
	}

void AllInterfaceNotificationActivity::TAwaitingLinkNotification::Cancel()
	{
	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnsts, 11));
	}


// for shutdown
DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAwaitingLinkNotificationError, NetStateMachine::MState, TContext)
TInt AllInterfaceNotificationActivity::TAwaitingLinkNotificationError::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierNotification>())
		{
		// we're already shutting down. so have detached from CConnection.
		//  so we have to delete the enclosed object instead.
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AllInterfaceNotification received bundle when already shutting down so deleting it")));
		TCFTierStatusProvider::TTierNotification& msg = message_cast<TCFTierStatusProvider::TTierNotification>(iContext.iMessage);
		msg.iBundle->Close();
       	iContext.iMessage.ClearMessageId();
		}
	else if(iContext.iMessage.IsMessage<TEBase::TCancel>())
		{
		LOG( ESockLog::Printf(KESockConnectionTag, _L("AllInterfaceNotification received TCancel when already shutting down so discarding it")));
       	iContext.iMessage.ClearMessageId();
		}

	return iContext.iMessage.IsMessage<TEBase::TError>();
	}


// Transitions

DEFINE_SMELEMENT(ConnectionGoingDownActivity::TStoreGoneDownError, NetStateMachine::MStateTransition, TContext)
void ConnectionGoingDownActivity::TStoreGoneDownError::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));
    TCFControlClient::TGoneDown& goneDownMessage = message_cast<TCFControlClient::TGoneDown>(iContext.iMessage);
	LOG( ESockLog::Printf(KESockConnectionTag, _L("TStoreGoneDownError. error:%d"), goneDownMessage.iValue1));
	iContext.Activity()->SetError(goneDownMessage.iValue1);
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TAddClient, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TAddClient::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity() == NULL, ConnPanic(KPanicNoActivity));
	__ASSERT_DEBUG(iContext.iPeer == NULL, User::Panic(KSpecAssert_ESockSSockscnsts, 12));
	iContext.iPeer = iContext.Node().AddClientL(address_cast<TNodeId>(iContext.iSender), TClientType(TCFClientType::EAux));
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TFindTierManager, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TFindTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	// Find out if the tier UID has been set.
	TBool tierUidIsNull = (iContext.Activity()->iTierUid == TUid::Null());

	// Panic in UDEB, Leave in UREL. The tier UID should be set in the derived
	// class's constructor
	__ASSERT_DEBUG(!tierUidIsNull, User::Panic(KSpecAssert_ESockSSockscnsts, 13));
	User::LeaveIfError(tierUidIsNull ? KErrNotReady : KErrNone);

	TAlwaysFindFactoryQuery query;
	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), SockManGlobals::Get()->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane)),
		TCFFactory::TFindOrCreatePeer(TCFPlayerRole::ETierMgrPlane, iContext.Activity()->iTierUid, &query).CRef());
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TJoinTierManager, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TJoinTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));


	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.Activity()->iTierManager,
		TCFServiceProvider::TJoinRequest(iContext.NodeId(), TClientType(TCFClientType::EAdministrative)).CRef());

	iContext.Node().AddClientL(iContext.Activity()->iTierManager, TClientType(TCFClientType::EServProvider));
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TStartLinkNotification, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TStartLinkNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	CConnectionServParameterBundle* queryBundle = CConnectionServParameterBundle::NewL();
   	CleanupStack::PushL(queryBundle);
   	CRefCountOwnedParameterBundle* bundleOwner = new(ELeave)CRefCountOwnedParameterBundle(queryBundle);
   	CleanupStack::Pop();
   	bundleOwner->Open();
   	CleanupClosePushL(*bundleOwner);

	CParameterSetContainer* psc = CParameterSetContainer::NewL(*queryBundle);

	// Set up a status filter for connected access points only
	TAccessPointStatusFilter availFilter;
	availFilter.Configured(EAccessPointFlagIgnore);
	availFilter.Restricted(EAccessPointFlagIgnore);
	availFilter.Available(EAccessPointFlagIgnore);
	availFilter.Started(EAccessPointFlagIgnore);
	availFilter.Active(EAccessPointFlagMatchAny);
	queryBundle->AddMatchConditionL(availFilter);
	queryBundle->AddParameterSetToReturnL(XAccessPointGenericParameterSet::Type());
	queryBundle->AddParameterSetToReturnL(XIpProtoAccessPointParameterSet::Type());

   	const CAllInterfaceNotificationWorker* worker = iContext.Activity()->iAllInterfaceNotificationWorker;
   	const RMessage2* rm2 = &(iContext.Activity()->iAllInterfaceNotificationWorker->iConnection.Message());

   	RClientInterface::OpenPostMessageClose(worker->Id(), iContext.Activity()->iTierManager,
   		TCFTierStatusProvider::TTierNotificationRegistration(bundleOwner, rm2).CRef());
   	CleanupStack::Pop(); // bundleOwner
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TEnqueueNotification, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TEnqueueNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	TCFTierStatusProvider::TTierNotification& msg = message_cast<TCFTierStatusProvider::TTierNotification>(iContext.iMessage);

	__ASSERT_DEBUG(msg.iBundle, User::Panic(KSpecAssert_ESockSSockscnsts, 14));
	
    CAllInterfaceNotificationActivity& act = *iContext.Activity();
    
    // Register with the IpProtoCpr factory for interface state changes if we haven't already done so.
    // We can only do this if the IpProtoCpr is active, and the assumption here is that it must be active
    // if an interface event has been generated.
    if (!act.iRegisteredForInterfaceStateChanges)
        {
        CCommsFactoryBase* ipProtoCprFactory = act.IpProtoCprFactory();
        if (ipProtoCprFactory)
            {
            Factories::IFactoryNotify itf(&act, act.GetVTable());
            ipProtoCprFactory->RegisterNotifierL(itf);
            act.iRegisteredForInterfaceStateChanges = ETrue;
            }
        }

	if(msg.iBundle->PtrL()->CountParamSetContainers() > 0)
		{
		TInt i = 0;
		TBool found = EFalse;
		while(const CConnectionServParameterSetContainer* container = static_cast<const CConnectionServParameterSetContainer*>(msg.iBundle->Ptr()->GetParamSetContainer(i++)))
			{
			const XAccessPointGenericParameterSet* status = XAccessPointGenericParameterSet::FindInParamSetContainer(*container);
			
			TConnInterfaceState interfaceState;
			if (status->AccessPointStatus().Active() == ConnectionServ::EAccessPointFlagTrue)
				{
				interfaceState = EInterfaceUp;
				}
			else
				{			
				interfaceState = EInterfaceDown;
				}

			const XIpProtoAccessPointParameterSet* ipApParams = XIpProtoAccessPointParameterSet::FindInParamSetContainer(*container);
//			ASSERT(ipApParams);
			if (ipApParams) // commsdat might be broken so XIpProtoAccessPointParameterSet not available
				{
				TConnectionInfo connectionInfo(ipApParams->IapId(), ipApParams->NetworkId());

				TInterfaceNotification notification = { connectionInfo,  interfaceState };

				iContext.Node().iConnection.iLegacyConnection.iNotificationQueue.Enque(notification);

				found = ETrue;
				}
			}
		if (found)
			iContext.Node().iConnection.iLegacyConnection.CompleteAllInterfaceNotificationL(KErrNone);

		}

	msg.iBundle->Close();
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TCancelLinkNotification, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TCancelLinkNotification::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

   	TNodeId goo = iContext.Activity()->iTierManager;

   	const CAllInterfaceNotificationWorker* worker = iContext.Activity()->iAllInterfaceNotificationWorker;

	RNodeInterface::OpenPostMessageClose(worker->Id(), iContext.Activity()->iTierManager,
		TEBase::TCancel().CRef());
	}

// This solely exists separately from the other identical-looking version
//    in this file ("CTierManagerActivity::TLeaveTierManager::DoL()") because
//    the activities referenced by its TContext have iTierManager
//    at different offsets. This is because they have different parent
//    classes and, as such, have different member variable.
//
// The terrible thing is, this is usable in the other place,
//    and builds without complaint... then explodes when it tries
//    to send messages to a garbage iTierManager.
//
//    TAKE CARE OF IT!!!
DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TLeaveTierManager, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TLeaveTierManager::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.Activity()->iTierManager,
		TEChild::TLeft().CRef());

	iContext.Node().RemoveClient(iContext.Activity()->iTierManager);
	}

DEFINE_SMELEMENT(AllInterfaceNotificationActivity::TSendErrorToConnection, NetStateMachine::MStateTransition, TContext)
void AllInterfaceNotificationActivity::TSendErrorToConnection::DoL()
	{
	iContext.Node().iConnection.iLegacyConnection.CompleteAllInterfaceNotificationL(iContext.Activity()->Error());

	TEBase::TError msg(iContext.Activity()->KickOffMessageId(), iContext.Activity()->Error());
	iContext.Activity()->PostToOriginators(msg);

	iContext.Activity()->SetError(KErrNone);
	}

DEFINE_SMELEMENT(TNoTagOrCancelAllInterfaceWorker, NetStateMachine::MStateFork, TContext)
TInt TNoTagOrCancelAllInterfaceWorker::TransitionTag()
	{
	// If this CConnection has an AllInterfaceNotificationWorker, return KCancelAllInterfaceWorker else return KNoTag.
	AConnectionLegacy& cl = iContext.Node().iLegacyConnection;
	return cl.iAllInterfaceNotificationWorker.IsNull() ? KNoTag : KCancelAllInterfaceWorker;
	}

DEFINE_SMELEMENT(TCancelAllInterfaceNotificationWorker, NetStateMachine::MStateTransition, TContext)
void TCancelAllInterfaceNotificationWorker::DoL()
	{
	CConnection& cc = iContext.Node();
	AConnectionLegacy& cl = cc.iLegacyConnection;
	// If this CConnection has an AllInterfaceNotificationWorker, send a TCancel to it to begin
	// the shutdown handshake.
	if(!cl.iAllInterfaceNotificationWorker.IsNull())
		{
		TNodeCtxId dest(ECFActivityConnectionAllInterfaceNotification, cl.iAllInterfaceNotificationWorker);
		RNodeInterface::OpenPostMessageClose(cc.Id(), dest, TEBase::TCancel().CRef());
		cl.iAllInterfaceNotificationWorker.SetNull();
		}
	}

//
// CEnumerateConnectionsActivity

MeshMachine::CNodeActivityBase* ConnActivities::CEnumerateConnectionsActivity::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new(ELeave) ConnActivities::CEnumerateConnectionsActivity(aActivitySig, aNode);
	}

// States

DEFINE_SMELEMENT(EnumerateConnectionsActivity::TAwaitingTierStatus, NetStateMachine::MState, TContext)
TInt EnumerateConnectionsActivity::TAwaitingTierStatus::Accept()
	{
	TBool isTierStatus = iContext.iMessage.IsMessage<TCFTierStatusProvider::TTierStatus>();
	TBool isError = iContext.iMessage.IsMessage<TEBase::TError>();

	if(isTierStatus || isError)
		{
		// Leave the tier manager
		RNodeInterface::OpenPostMessageClose(TNodeCtxId(iContext.ActivityId(), iContext.NodeId()), iContext.Activity()->iTierManager,
			TEChild::TLeft().CRef());

		iContext.Node().RemoveClient(iContext.Activity()->iTierManager);
		}

	return isTierStatus;
	}

// Transitions

DEFINE_SMELEMENT(EnumerateConnectionsActivity::TQueryTierStatus, NetStateMachine::MStateTransition, TContext)
void EnumerateConnectionsActivity::TQueryTierStatus::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	// Clear the connection info cache
	iContext.Node().iLegacyConnection.iConnectionInfoPtrArray.ResetAndDestroy();

	CConnectionServParameterBundle* queryBundle = CConnectionServParameterBundle::NewL();
   	CleanupStack::PushL(queryBundle);
   	CRefCountOwnedParameterBundle* bundleOwner = new (ELeave)CRefCountOwnedParameterBundle(queryBundle);
   	CleanupStack::Pop(queryBundle);
   	bundleOwner->Open();
   	CleanupClosePushL(*bundleOwner);

	// Set up a status filter for connected access points only
	TAccessPointStatusFilter availFilter;
	availFilter.Configured(EAccessPointFlagIgnore);
	availFilter.Restricted(EAccessPointFlagIgnore);
	availFilter.Available(EAccessPointFlagIgnore);
	availFilter.Started(EAccessPointFlagIgnore);
	availFilter.Active(EAccessPointFlagMatchTrue);

	queryBundle->AddMatchConditionL(availFilter);
	queryBundle->AddParameterSetToReturnL(XIpProtoAccessPointParameterSet::Type());

   	RClientInterface::OpenPostMessageClose(iContext.NodeId(), iContext.Activity()->iTierManager,
   		TCFTierStatusProvider::TTierStatusQuery(bundleOwner, &iContext.Activity()->iMessage).CRef());
   	CleanupStack::Pop(); // bundleOwner
	}

DEFINE_SMELEMENT(EnumerateConnectionsActivity::TCompleteClient, NetStateMachine::MStateTransition, TContext)
void EnumerateConnectionsActivity::TCompleteClient::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	TPckg<TUint> pckg(iContext.Node().iLegacyConnection.iConnectionInfoPtrArray.Count());
	iContext.Activity()->WriteL(0, pckg);
	}

DEFINE_SMELEMENT(EnumerateConnectionsActivity::TCacheConnectionInfo, NetStateMachine::MStateTransition, TContext)
void EnumerateConnectionsActivity::TCacheConnectionInfo::DoL()
	{
	__ASSERT_DEBUG(iContext.Activity(), ConnPanic(KPanicNoActivity));

	TCFTierStatusProvider::TTierStatus& msg = message_cast<TCFTierStatusProvider::TTierStatus>(iContext.iMessage);

	const CConnectionServParameterSetContainer* container;
	const XIpProtoAccessPointParameterSet* ipApParams;

	for (TInt i=0; i<msg.iBundle->PtrL()->CountParamSetContainers(); i++)
		{
		container = static_cast<const CConnectionServParameterSetContainer*>(msg.iBundle->Ptr()->GetParamSetContainer(i));

		ipApParams = XIpProtoAccessPointParameterSet::FindInParamSetContainer(*container);
		__ASSERT_DEBUG(ipApParams, User::Panic(KSpecAssert_ESockSSockscnsts, 16));

		TSourcedConnectionInfo* connInfo = new(ELeave) TSourcedConnectionInfo(ipApParams->IapId(), ipApParams->NetworkId(), ipApParams->ConnectionType(), ipApParams->ControlClientId());
		iContext.Node().iLegacyConnection.iConnectionInfoPtrArray.AppendL(connInfo);
		}

	msg.iBundle->Close();
	}

DEFINE_SMELEMENT(ConnectionCleanupActivities::TNoTagOrNoTagBackwards, NetStateMachine::MStateFork, TContext)
TInt ConnectionCleanupActivities::TNoTagOrNoTagBackwards::TransitionTag()
    {
	if ( iContext.iMessage.IsMessage<TEPeer::TLeaveComplete>() )
		{
		if (iContext.Node().GetFirstClient<TDefaultClientMatchPolicy>(TCFClientType(TCFClientType::EServProvider)) == NULL)
			{ // This was the last service provider
			return NetStateMachine::EForward | MeshMachine::KNoTag;
			}
		// There are more service providers to expect 'leave complete' from so
		// loop round and wait for the next one.
		return NetStateMachine::EBackward | MeshMachine::KNoTag;
		}
    __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnsts, 17));
    return KNoTag;
    }

DEFINE_SMELEMENT(ConnSubConnEventsActivity::TAwaitingSubConnEvent, NetStateMachine::MState, TContext)
TInt ConnSubConnEventsActivity::TAwaitingSubConnEvent::Accept()
	{
	if(iContext.iMessage.IsMessage<TCFInternalEsock::TSubConnectionOpenedEvent>() ||
	   iContext.iMessage.IsMessage<TCFInternalEsock::TSubConnectionClosedEvent>())
		{
		return ETrue;
		}
	return EFalse;
	}

DEFINE_SMELEMENT(ConnSubConnEventsActivity::TProcessSubConnEvent, NetStateMachine::MStateTransition, TContext)
void ConnSubConnEventsActivity::TProcessSubConnEvent::DoL()
	{
	CConnection& conn = static_cast<CConnection&>(iContext.Node());

	if(iContext.iMessage.IsMessage<TCFInternalEsock::TSubConnectionOpenedEvent>())
		{
		const TCFInternalEsock::TSubConnectionOpenedEvent& msg = message_cast<TCFInternalEsock::TSubConnectionOpenedEvent>(iContext.iMessage);
		conn.iLegacyConnection.SubConnectionEvent(msg.iSubConnectionOpenedEvent);
		}
	else
		{
		const TCFInternalEsock::TSubConnectionClosedEvent& msg = message_cast<TCFInternalEsock::TSubConnectionClosedEvent>(iContext.iMessage);
		conn.iLegacyConnection.SubConnectionEvent(msg.iSubConnectionClosedEvent);
		}
	}

CNodeActivityBase* CConnLegacyRMessage2Activity::NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	return new (ELeave) CConnLegacyRMessage2Activity( aActivitySig, aNode, c);
	}
	
CConnLegacyRMessage2Activity::CConnLegacyRMessage2Activity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aNextActivityCount)
	: MeshMachine::CNodeParallelMessageStoreActivityBase(aActivitySig, aNode, aNextActivityCount)
	{
	}

CConnLegacyRMessage2Activity::~CConnLegacyRMessage2Activity()
	{
	if (Error() != KErrNone)
		{
		Complete(Error());
		}
	if (!iCancelMessage.IsNull())
		{
		iCancelMessage.Complete(KErrNone);
		}
	SetError(KErrNone);
	}

void CConnLegacyRMessage2Activity::PanicClient(const TDesC& aCatagory, TInt aCode)
	{
	if (!iSafeMessage.IsNull())
		{
		iSafeMessage.PanicClient(aCatagory, aCode);
		}
	}

void CConnLegacyRMessage2Activity::Complete(TInt aCode)
	{
	if (!iSafeMessage.IsNull())
		{
		iSafeMessage.Complete(aCode);
		}
	}

void CConnLegacyRMessage2Activity::SetCancelRequest(const Den::RSafeMessage& aMessage)
	{
	iCancelMessage.Duplicate(aMessage);
	}


DEFINE_SMELEMENT(TProcessLegacyRMessage2, NetStateMachine::MStateTransition, TContext)
void TProcessLegacyRMessage2::DoL()
	{
	ASSERT(iContext.iMessage.IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
	// static_cast as it will be a derivative and message_cast won't work
	CConnLegacyRMessage2Activity* act = static_cast<CConnLegacyRMessage2Activity*>(iContext.Activity());
	TCFSigLegacyRMessage2Ext& msg = static_cast<TCFSigLegacyRMessage2Ext&>(iContext.iMessage);
	act->iSafeMessage.Duplicate(msg.iMessage);
	
	if (msg.CanProcess(iContext))
		{
		msg.ProcessL(iContext);
		}
	else
		{
		msg.ForwardL(iContext);
		}
	}

DEFINE_SMELEMENT(TCompleteRMessage2, NetStateMachine::MStateTransition, TContext)
void TCompleteRMessage2::DoL()
	{
	TCFLegacyMessage::TLegacyRMessage2Processed& msg = message_cast<TCFLegacyMessage::TLegacyRMessage2Processed>(iContext.iMessage);
	CConnLegacyRMessage2Activity* act = static_cast<CConnLegacyRMessage2Activity*>(iContext.Activity());

    if (msg.iResponse.iType == TLegacyRMessage2Response::EPanic)
		{
		act->PanicClient(msg.iResponse.iCatagory, msg.iResponse.iCode);
		}
    else
		{
		ASSERT(act->Message().IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
		TInt error = static_cast<TCFSigLegacyRMessage2Ext&>(act->Message()).CheckError(iContext, msg.iResponse.iCode);
		act->Complete(error);
		}
	}

DEFINE_SMELEMENT(THandleRMessage2Error, NetStateMachine::MStateTransition, ConnStates::TContext)
void THandleRMessage2Error::DoL()
    {
	CConnLegacyRMessage2Activity* act = static_cast<CConnLegacyRMessage2Activity*>(iContext.Activity());
	ASSERT(act->Message().IsTypeOf(Meta::STypeId::CreateSTypeId(TCFSigLegacyRMessage2Ext::EUid, TCFSigLegacyRMessage2Ext::ETypeId)));
	TInt error = static_cast<TCFSigLegacyRMessage2Ext&>(act->Message()).CheckError(iContext, act->Error());
	act->SetError(error);
	act->Complete(error);
    }

DEFINE_SMELEMENT(TCancelAllLegacyRMessage2Activities, NetStateMachine::MStateTransition, TContext)
void TCancelAllLegacyRMessage2Activities::DoL()
	{
	const RPointerArray<CNodeActivityBase>& activities = iContext.Node().Activities();

	for (TInt i = 0; i < activities.Count(); i++)
		{
		if (activities[i]->ActivitySigId() == ESock::ECFActivityConnectionLegacyRMessage2Handler)
			{
			activities[i]->Cancel(iContext);
			}
		}
	}


