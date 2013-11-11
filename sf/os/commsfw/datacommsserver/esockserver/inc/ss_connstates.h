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

#ifndef SYMBIAN_SS_CONNSTATES_H
#define SYMBIAN_SS_CONNSTATES_H

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_esockstates.h>
#include <comms-infras/ss_esockactivities.h>
#include <comms-infras/ss_corepractivities.h>
#include "SS_conn.H"
#include <comms-infras/esock_params_internal.h>
#include "ss_internal_activities.h"

#include <comms-infras/ss_nodemessages_selector.h> // for TSelectionPrefs

namespace ConnStates
{
typedef MeshMachine::TNodeContext<ESock::CConnection, SubSessStates::TContext> TContext;


//
//Mutex checking the progress queue
class TEmptyProgressQueueMutex
	{
public:
    static TBool IsBlocked(TContext& aContext)
    	{
		return aContext.Node().iProgressQueue.IsEmpty();
    	}
	};

typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStart,ESock::ECFActivityConnectionAttach> TStartAttachMutex;

//
//Start


DECLARE_SMELEMENT_HEADER( TErrorIfAlreadyStartedAttached, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TErrorIfAlreadyStartedAttached )

DECLARE_SMELEMENT_HEADER( TClearProgressQueue, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TClearProgressQueue )

DECLARE_SMELEMENT_HEADER( TParseECNStart, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TParseECNStart )

DECLARE_SMELEMENT_HEADER( TSendStartingSelectionStateChange, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStartingSelectionStateChange )

DECLARE_SMELEMENT_HEADER( TRequestCSRCreation, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCSRCreation )

DECLARE_SMELEMENT_HEADER( TProcessCSRCreation, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessCSRCreation )


DECLARE_SERIALIZABLE_TRANSITION(
	TProcessStartBlockedByStop,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStop>,
	ConnStates::TParseECNStart
	)

DECLARE_SMELEMENT_HEADER( TSelectMetaPlane, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelectMetaPlane )

DECLARE_SMELEMENT_HEADER( TSendFinishedSelectionStateChange, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendFinishedSelectionStateChange )


DECLARE_SMELEMENT_HEADER( TJoinReceivedSCpr, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinReceivedSCpr )

DECLARE_SMELEMENT_HEADER( TJoinReceivedMcpr, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinReceivedMcpr )


DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )

DECLARE_SMELEMENT_HEADER( TRequestCommsBinderFromMcpr, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinderFromMcpr )

DECLARE_SMELEMENT_HEADER( TProcessBinderResponseForCpr, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessBinderResponseForCpr )

DECLARE_SMELEMENT_HEADER( TStartConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartConnection )

DECLARE_SMELEMENT_HEADER( TErrorOrCancel, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TErrorOrCancel )

//
//Attach
DECLARE_SMELEMENT_HEADER( TParseECNAttach, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TParseECNAttach )

DECLARE_AGGREGATED_TRANSITION4(
	TProcessAttach,
	ConnStates::TErrorIfAlreadyStartedAttached,
	ConnStates::TParseECNAttach,
	ConnStates::TClearProgressQueue,
	ConnStates::TRequestCSRCreation
	)

DECLARE_SERIALIZABLE_TRANSITION(
	TProcessAttachBlockedByStop,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStop>,
	ConnStates::TProcessAttach
	)

DECLARE_SMELEMENT_HEADER( TCompleteLegacyAttach, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCompleteLegacyAttach )

//
//WaitForIncoming
DECLARE_SMELEMENT_HEADER( TRequestIncomingConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestIncomingConnection )

DECLARE_SMELEMENT_HEADER( TProcessIncomingConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessIncomingConnection )

DECLARE_SERIALIZABLE_TRANSITION(
	TRequestIncomingConnectionBlockedByStartAttach,
	TStartAttachMutex,
	ConnStates::TRequestIncomingConnection
	)

//
//Stop
DECLARE_SMELEMENT_HEADER( TSendStopConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendStopConnection )

DECLARE_SMELEMENT_HEADER( TCancelStartOrAttachConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelStartOrAttachConnection )

DECLARE_SMELEMENT_HEADER( TConnectionSendStopSCPR, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TConnectionSendStopSCPR )

DECLARE_SMELEMENT_HEADER( TAwaitingGoneDown, CoreNetStates::TAwaitingGoneDown, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingGoneDown )

typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStart,ESock::ECFActivityConnectionAttach> TStartAttachMutex;

DECLARE_SERIALIZABLE_STATE(
     TNoTagOrNoBearerBlockedByStartOrAttach,
	 TStartAttachMutex,
     CoreNetStates::TNoTagOrNoBearer
     )

//
//Close

const TInt KCancelAllInterfaceWorker = 10000;

DECLARE_SMELEMENT_HEADER(TDetachDataClients, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TDetachDataClients)

DECLARE_SMELEMENT_HEADER( TStartOrAttachActive, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TStartOrAttachActive )

DECLARE_SMELEMENT_HEADER( TNoTagOrCancelAllInterfaceWorker, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrCancelAllInterfaceWorker )

DECLARE_SMELEMENT_HEADER( TCancelAllInterfaceNotificationWorker, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelAllInterfaceNotificationWorker )

DECLARE_SERIALIZABLE_STATE(
	TActiveOrNoTagBlockedByStopOrGoneDown,
	CoreNetStates::TActivityStopAndGoneDownMutex,
	TStartOrAttachActive
	)

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByStartOrAttach,
	TStartAttachMutex,
	MeshMachine::TNoTag
	)

DECLARE_AGGREGATED_TRANSITION2(
	TProcessClose,
	SubSessStates::TProcessClose,
	TDetachDataClients
	)

//
//Progress & Progress Request
DECLARE_SMELEMENT_HEADER( TAwaitingStateChange, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStateChange )

DECLARE_SMELEMENT_HEADER( TProcessStateChange, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext ) //From the framework
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessStateChange )

DECLARE_SMELEMENT_HEADER( TEnqueueStateChange, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext ) //From the framework
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TEnqueueStateChange )

DECLARE_SMELEMENT_HEADER( TProcessProgressRequest, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext ) //From the client
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessProgressRequest )

DECLARE_SERIALIZABLE_TRANSITION(
	ProcessProgressRequestBlockedByEmptyProgressQueue,
	TEmptyProgressQueueMutex,
	ConnStates::TProcessProgressRequest
	)


DECLARE_SMELEMENT_HEADER( TGenerateConnectionUpProgress,  MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext ) //From the framework
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGenerateConnectionUpProgress )

DECLARE_SMELEMENT_HEADER( TGenerateConnectionDownProgress,  MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext ) //From the framework
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGenerateConnectionDownProgress )
	
DECLARE_SMELEMENT_HEADER( TGenerateConnectionUninitialisedProgress,  MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext ) //From the framework
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TGenerateConnectionUninitialisedProgress )



//
// Legacy enumeration support
DECLARE_SMELEMENT_HEADER( TAwaitingEnumResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingEnumResponse )

DECLARE_SMELEMENT_HEADER( TProcessEnumResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessEnumResponse )

//
// Legacy RMessage2 handling

DECLARE_SMELEMENT_HEADER( TProcessLegacyRMessage2, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessLegacyRMessage2 )

DECLARE_SMELEMENT_HEADER( TCompleteRMessage2, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCompleteRMessage2 )

DECLARE_SMELEMENT_HEADER( THandleRMessage2Error, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( THandleRMessage2Error )

DECLARE_SMELEMENT_HEADER( TCancelAllLegacyRMessage2Activities, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelAllLegacyRMessage2Activities )

typedef MeshMachine::TActivityIdMutex<ESock::ECFActivityConnectionLegacyRMessage2Handler> TLegacyRMessage2ActivitiesMutex;

DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByLegacyRMessage2Activities,
	TLegacyRMessage2ActivitiesMutex,
	MeshMachine::TNoTag
	)

} // namespace ConnStates


namespace AllInterfaceNotificationActivity
{
class TAddClient;
class TFindTierManager;
class TAwaitingTierManager;
class TJoinTierManager;
class TStartLinkNotification;
class TAwaitingLinkNotification;
class TAwaitingLinkNotificationError;
class TEnqueueNotification;
class TCancelLinkNotification;
class TLeaveTierManager;
}

namespace EnumerateConnectionsActivity
{
class TAwaitingTierStatus;
class TQueryTierStatus;
}

namespace ConnActivities
{

//
//CStartAttachActivity
class CStartAttachActivity : public SubSessActivities::CESockClientActivityBase, public CoreActivities::ABindingActivity,
                             public ITFHIERARCHY_LINK_1(CStartAttachActivity, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity)
	{
	friend class ConnStates::TParseECNStart;
	
public:
	typedef ITFHIERARCHY_LINK_1(CStartAttachActivity, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

public:
	//Transition Tags
	static const TInt KExecuteLegacyAttach = 1;
	static const TInt KWaitAvailable = 2;
	static const TInt KCancelTag = 3;
	static const TInt KStartPrefsSetTag = 4;

	static const TInt KUpsErrorTag = 10011;


public:
	void SetSelectionScope(TUint aSelectionScope) { iConnPrefs.SetScope(aSelectionScope); }
	void SetSelectionFlags(TUint aSelectionScope) { iConnPrefs.SetFlags(aSelectionScope); }
	ESock::TSelectionPrefs& SelectionPrefs() { return iConnPrefs; }
	
	void SetCSRPreferences(TConnCSRPref* aCustomPrefs) { iCSRPreferences = aCustomPrefs; }
	TConnCSRPref* CSRPreferences() { return iCSRPreferences; }
	
	TConnPrefList* ConnPrefList() { return iConnPrefList; }
	void SetConnPrefList(TConnPrefList* aConnPrefList){ iConnPrefList = aConnPrefList; }
	void SetCSR(const Messages::TNodeId& aCSR) { iCSR = aCSR; }
	const Messages::TNodeId& CSR() const { return iCSR; }
    static MeshMachine::CNodeActivityBase* NewStartConnectionActivityL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    static MeshMachine::CNodeActivityBase* NewWaitForIncomingConnectionActivityL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	virtual ~CStartAttachActivity();
	
	virtual TBool Next(MeshMachine::TNodeContextBase& aContext);
	virtual void Cancel(MeshMachine::TNodeContextBase& aContext);

	void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
    	{
    	aInterface = this;
    	}
	TBool& AutoStartPresent()
		{
		return iIsAutoStartPresent;
		}

protected:
	CStartAttachActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CESockClientActivityBase(aActivitySig, aNode),
		CoreActivities::ABindingActivity(aNode.Id()),
		TIfStaticFetcherNearestInHierarchy(this),
		iConnPrefs()
		{
		}

private:
	ESock::TSelectionPrefs iConnPrefs;
	Messages::TNodeId iCSR;
	TConnPrefList* iConnPrefList;
	TConnCSRPref* iCSRPreferences;
	TBool iIsAutoStartPresent;
	TBool iStartReceived;  // Indicates that ECNStart has been received
	
protected:
    typedef MeshMachine::TNodeContext<ESock::CConnection, SubSessStates::TContext> TContext;

//States, StateForks & StateTransitions
public:
    DECLARE_SMELEMENT_HEADER( TNoTagOrStartPrefsSetTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrStartPrefsSetTag )

    typedef MeshMachine::TTag<ConnActivities::CStartAttachActivity::KStartPrefsSetTag> TStartPrefsSetTag;
    
    //Not the first state fork on the ECAB, so just normal state fork
	DECLARE_SMELEMENT_HEADER( TNoTagOrLegacyAttach, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrLegacyAttach )

	DECLARE_SMELEMENT_HEADER( TNoTagOrWaitAvailable, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrWaitAvailable )

	DECLARE_SMELEMENT_HEADER( TErrorTagOrWaitAvailableBackward, MeshMachine::TStateFork<ConnStates::TContext>, NetStateMachine::MStateFork, ConnStates::TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TErrorTagOrWaitAvailableBackward )
	
	DECLARE_SMELEMENT_HEADER( TAwaitingAvailableOrError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TAwaitingAvailableOrError )

	DECLARE_SMELEMENT_HEADER( TSubscribeForAvailability, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSubscribeForAvailability )

	DECLARE_SMELEMENT_HEADER( TSetIdleIfStopOutstanding, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSetIdleIfStopOutstanding )

	DECLARE_SMELEMENT_HEADER(TAwaitingSelectCompleteOrError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER(TAwaitingSelectCompleteOrError)


	DECLARE_AGGREGATED_TRANSITION2(
        TSendBindToCompleteAndCompleteLegacyAttach,
     	TSendBindToComplete,
     	ConnStates::TCompleteLegacyAttach
     	)	

#ifdef SYMBIAN_NETWORKING_UPS
    DECLARE_SMELEMENT_HEADER(TSendPolicyCheckRequestToServiceProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, ConnStates::TContext)
	virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TSendPolicyCheckRequestToServiceProvider)

	DECLARE_SMELEMENT_HEADER(TNoTagOrUpsErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, ConnStates::TContext)
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER(TNoTagOrUpsErrorTag)	
#endif

	};

/**
	A base activity for activities wishing to interact with a tier manager.
*/
class CTierManagerActivity : public SubSessActivities::CESockClientActivityBase
	{
public:
	typedef MeshMachine::TNodeContext<ESock::CConnection, SubSessStates::TContext, CTierManagerActivity> TContext;

public:
	static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	// States
	DECLARE_SMELEMENT_HEADER(TAwaitingTierManager, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER(TAwaitingTierManager)

	DECLARE_SMELEMENT_HEADER(TAwaitingJoinComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER(TAwaitingJoinComplete)

	// Transitions
	DECLARE_SMELEMENT_HEADER(TFindTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TFindTierManager)

	DECLARE_SMELEMENT_HEADER(TJoinTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TJoinTierManager)

	DECLARE_SMELEMENT_HEADER(TLeaveTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER(TLeaveTierManager)

protected:
	CTierManagerActivity (const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode) :
		CESockClientActivityBase(aActivitySig, aNode)
		{
		iTierUid = TUid::Null();
		}

protected:
	TUid iTierUid;
	Messages::TNodeId iTierManager;
	};

const TInt KIPProtoTierId = 0x10281DF0;
	
/**
	An activity supporting the legacy RConnection::AllInterfaceNotification method.
*/
NONSHARABLE_CLASS(CAllInterfaceNotificationActivity) : public MeshMachine::CNodeActivityBase
	{
friend class AllInterfaceNotificationActivity::TAddClient;
friend class AllInterfaceNotificationActivity::TFindTierManager;
friend class AllInterfaceNotificationActivity::TAwaitingTierManager;
friend class AllInterfaceNotificationActivity::TJoinTierManager;
friend class AllInterfaceNotificationActivity::TStartLinkNotification;
friend class AllInterfaceNotificationActivity::TAwaitingLinkNotification;
friend class AllInterfaceNotificationActivity::TAwaitingLinkNotificationError;
friend class AllInterfaceNotificationActivity::TEnqueueNotification;
friend class AllInterfaceNotificationActivity::TCancelLinkNotification;
friend class AllInterfaceNotificationActivity::TLeaveTierManager;

public:
	static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    void InterfaceStateChangeNotification(const TDesC8& aInfo);
    ESock::CCommsFactoryBase* IpProtoCprFactory() const;

private:
	CAllInterfaceNotificationActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode) :
		MeshMachine::CNodeActivityBase(aActivitySig, aNode)
		{
		iTierUid = TUid::Uid(KIPProtoTierId); // IPProto tier
		iAllInterfaceNotificationWorker = static_cast<const CAllInterfaceNotificationWorker*>(&aNode);
		}

	virtual ~CAllInterfaceNotificationActivity();
    const Factories::TAnyFn& GetVTable() { return iInterfaceVTableF[0]; };

private:
	TUid iTierUid;
	Messages::TNodeId iTierManager;
	const CAllInterfaceNotificationWorker* iAllInterfaceNotificationWorker;
	TBool iRegisteredForInterfaceStateChanges:1;       // Registered with IpProtoCpr factory

private:
	static const ESock::TAnyFn iInterfaceVTableF[];
	};

/**
	An activity supporting the legacy RConnection::EnumerateConnections method.
*/
class CEnumerateConnectionsActivity : public CTierManagerActivity
	{
friend class EnumerateConnectionsActivity::TAwaitingTierStatus;
friend class EnumerateConnectionsActivity::TQueryTierStatus;

public:
	static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

private:
	CEnumerateConnectionsActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode) :
		CTierManagerActivity(aActivitySig, aNode)
		{
			iTierUid = TUid::Uid(KIPProtoTierId); // Link tier
		}
	};

/**
   An activity for handling legacy RMessage2 requests
*/
class CConnLegacyRMessage2Activity : public MeshMachine::CNodeParallelMessageStoreActivityBase
	{
public:
	static CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
	~CConnLegacyRMessage2Activity();

	void PanicClient(const TDesC& aCatagory, TInt aCode);
	void Complete(TInt aCode);
	
	void SetCancelRequest(const Den::RSafeMessage& aMessage);

private:
	CConnLegacyRMessage2Activity( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount);
	
public:
	Den::RSafeMessage iSafeMessage;
	Den::RSafeMessage iCancelMessage;
	};

} //namespace ConnActivities


/**
	Container for activity entities common to CConnection cleanup.
*/
namespace ConnectionCleanupActivities
{
typedef MeshMachine::TNodeContext<ESock::CConnection, SubSessStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER(TNoTagOrNoTagBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TNoTagOrNoTagBackwards)
}

namespace AllInterfaceNotificationActivity
{
typedef MeshMachine::TNodeContext<CAllInterfaceNotificationWorker, CoreStates::TContext, ConnActivities::CAllInterfaceNotificationActivity> TContext;

// States
DECLARE_SMELEMENT_HEADER(TAwaitingTierManager, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingTierManager)

DECLARE_SMELEMENT_HEADER(TAwaitingJoinComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingJoinComplete)

DECLARE_SMELEMENT_HEADER(TAwaitingStart, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingStart)

DECLARE_SMELEMENT_HEADER(TAwaitingLinkNotification, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
	virtual void Cancel();
DECLARE_SMELEMENT_FOOTER(TAwaitingLinkNotification)

DECLARE_SMELEMENT_HEADER(TAwaitingLinkNotificationError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingLinkNotificationError)

// Transitions
DECLARE_SMELEMENT_HEADER(TAddClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TAddClient)

DECLARE_SMELEMENT_HEADER(TFindTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TFindTierManager)

DECLARE_SMELEMENT_HEADER(TJoinTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TJoinTierManager)

DECLARE_SMELEMENT_HEADER(TStartLinkNotification, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TStartLinkNotification)

DECLARE_SMELEMENT_HEADER(TEnqueueNotification, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TEnqueueNotification)

DECLARE_SMELEMENT_HEADER(TCancelLinkNotification, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TCancelLinkNotification)

DECLARE_SMELEMENT_HEADER(TLeaveTierManager, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TLeaveTierManager)

DECLARE_SMELEMENT_HEADER( TSendErrorToConnection, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendErrorToConnection )
}

namespace ConnectionGoingDownActivity
{
typedef MeshMachine::TNodeContext<ESock::CConnection, CoreStates::TContext, PRActivities::CGoneDownActivity> TContext;

DECLARE_SMELEMENT_HEADER(TStoreGoneDownError, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TStoreGoneDownError)
}

namespace EnumerateConnectionsActivity
{
typedef MeshMachine::TNodeContext<ESock::CConnection, ConnActivities::CTierManagerActivity::TContext, ConnActivities::CEnumerateConnectionsActivity> TContext;

// States
DECLARE_SMELEMENT_HEADER(TAwaitingTierStatus, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingTierStatus)

// Transitions
DECLARE_SMELEMENT_HEADER(TQueryTierStatus, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TQueryTierStatus)

DECLARE_SMELEMENT_HEADER(TCacheConnectionInfo, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TCacheConnectionInfo)

DECLARE_SMELEMENT_HEADER(TCompleteClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TCompleteClient)
}


namespace ConnSubConnEventsActivity
{
typedef MeshMachine::TNodeContext<ESock::CConnection, CoreStates::TContext> TContext;

// States
DECLARE_SMELEMENT_HEADER(TAwaitingSubConnEvent, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingSubConnEvent)

// Transitions
DECLARE_SMELEMENT_HEADER(TProcessSubConnEvent, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TProcessSubConnEvent)
}

#endif
// SYMBIAN_SS_CONNSTATES_H

