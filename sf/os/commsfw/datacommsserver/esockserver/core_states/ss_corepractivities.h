// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core PR Activities
//
//
/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SS_COREPRACTIVITIES_H
#define SYMBIAN_SS_COREPRACTIVITIES_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_nodemessages_internal.h>
#include <comms-infras/ss_nodemessages_legacy.h>
#include <comms-infras/ss_internal_activities.h>

#include <elements/mm_states.h>
#include <comms-infras/ss_parameterfamilybundle.h>
#include <elements/nm_messages_internal.h>

namespace CoreErrorActivity
{ //If found in node activity map is ran by the MM whenewer an error is received
  //and not processed by its target activity
	DECLARE_EXPORT_NODEACTIVITY(CoreError)
}

namespace PRControlClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL)
	DECLARE_EXPORT_NODEACTIVITY(PRControlClientJoin)
}

namespace PRDataClientJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL)
	DECLARE_EXPORT_NODEACTIVITY(PRDataClientJoin)
}

namespace PRClientLeaveActivity
{ //This activity does not need the activity object
	DECLARE_EXPORT_NODEACTIVITY(PRClientLeave)
}

namespace PRClientLeftActivity
{ //This activity does not need the activity object
	DECLARE_EXPORT_NODEACTIVITY(PRClientLeft)
}

namespace PRDataClientIdleActivity
{ //TAwaitingDataClientIdle will only accept if no more clients joined to the node
	static const TInt KDataClientNotStarted = 1;		// idle data client is not started

	DECLARE_EXPORT_NODEACTIVITY(PRDataClientIdle)
}

namespace PRDataClientActiveActivity
{ //TAwaitingDataClientIdle will only accept if no more clients joined to the node
	static const TInt KDataClientNotStarted = 1;		// idle data client is not started

	DECLARE_EXPORT_NODEACTIVITY(PRDataClientActive)
}

namespace PRSetParamsRequest
{
DECLARE_EXPORT_NODEACTIVITY(PRSetParams)
}

namespace PRGetParamsRequest
{
DECLARE_EXPORT_NODEACTIVITY(PRGetParams)
}

namespace PRDataClientStopActivity
{
typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrProviderStopped, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrProviderStopped )

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrProviderStoppedBlockedByStart,
	CoreNetStates::TActivityStartMutex,
	TNoTagOrProviderStopped
	)
}

namespace CoreActivities
{

//-=========================================================
//
//Error Recovery Activity
//
//-=========================================================
class CErrorActivity : public MeshMachine::CNodeParallelActivityBase
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CErrorActivity();

protected:
	CErrorActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);
    virtual TBool IsIdle() const;
	virtual TBool Next(MeshMachine::TNodeContextBase& aContext);
	virtual void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);

protected:

    //save original error values
    Messages::TNodeSignal::TMessageId iMessageId;
	
	// We cannot simply store a pointer to the activity here, as the activity may
	// go away, and then any access to it would cause a KERN-EXEC: 3. So store the
	// id of the activity, and then when we want a pointer to the activity, call
	// FindActivityById on the node.
	TInt iErroredActivityId;

protected:
    typedef CoreStates::TContext TContext;

public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
		IMPORT_C virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TAwaitingError )

	DECLARE_SMELEMENT_HEADER( TCFAwaitingError, CoreActivities::CErrorActivity::TAwaitingError, NetStateMachine::MState, TContext )
		IMPORT_C virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TCFAwaitingError )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSendErrorRecoveryReq, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendErrorRecoveryReq )

	EXPORT_DECLARE_SMELEMENT_HEADER( TDoErrorRecovery, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TDoErrorRecovery )

public:
    friend class TSendErrorRecoveryReq;
    friend class TDoErrorRecovery;
	};


//-=========================================================
//
//Destroy Activity - will delete the node when destructed
//
//-=========================================================
class CDestroyActivity : public MeshMachine::CNodeRetryActivity, protected MeshMachine::APreallocatedOriginators<1>
/*
Overlooks destruction of a mesh machine based node. Destruction is triggered either
by the (only) control provider or by the last client leaving (in the lack of the
control provider - mcprs). This logic guarantees there only is one CDestroyActivity
(against a node) at a time. CDestroyActivity is also an infallible activity hence
its execution (including all the allocations) must not fail. This is achieved by:
(1) CDestroyActivity constructs itself over the space preallocated
    by MeshMachine::AMMNodeBase::ConstructL(), see CDestroyActivity::New()
(2) CDestroyActivity preallocating the space for the only originator
    (see logic description above), see CDestroyActivity::iOriginator.
*/
	{
public:
    IMPORT_C static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
    CDestroyActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    virtual void Destroy();

//States, StateForks & StateTransitions
protected:
	typedef CoreStates::TContext TContext;

public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoTagBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrNoTagBackwards )

    EXPORT_DECLARE_SMELEMENT_HEADER( TMakeClientsLeaveOrProcessClientLeave, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        void MakeClientsLeaveL();
        void ProcessClientLeaveL();
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TMakeClientsLeaveOrProcessClientLeave )

    DECLARE_AGGREGATED_TRANSITION2(
		TProcessClientLeave,
		MeshMachine::TRemoveClient,
		CoreNetStates::TSendLeaveCompleteIfRequest
		)

	typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStop, ESock::ECFActivityStopDataClient, ESock::ECFActivityDataClientGoneDown, ESock::ECFActivityGoneDown, ESock::ECFActivityDestroyOrphans> TAllStopActivityMutex;
	typedef MeshMachine::TActivityIdMutex<ESock::ECFActivityClientLeave> TClientLeaveMutex;
    typedef MeshMachine::TClientMutex<Messages::TDefaultClientMatchPolicy, ESock::TCFClientType::EData, ESock::TCFClientType::ELeaving> TLeavingDataClientMutex;
	typedef MeshMachine::TAggregatedMutex_OR<TAllStopActivityMutex, TLeavingDataClientMutex> TAllStopActivityOrLeavingDataClientMutex;
	typedef MeshMachine::TAggregatedMutex_OR<TAllStopActivityOrLeavingDataClientMutex, TClientLeaveMutex> TAllStopActivityOrLeavingDataClientOrClientLeaveMutex;


    DECLARE_SERIALIZABLE_STATE(
    	TNoTagBlockedByActivitiesOrLeavingDataClient,
    	TAllStopActivityOrLeavingDataClientOrClientLeaveMutex,
    	MeshMachine::TNoTag
    	)
private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

//-=========================================================
//
// DestroyOrphans Activity - will delete the node when destructed
// if TMarkNodeForDestruction tuple has been called.
//
//-=========================================================

NONSHARABLE_CLASS(CDestroyOrphansActivity) : public MeshMachine::CNodeActivityBase, protected MeshMachine::APreallocatedOriginators<1>
    {
public:
    typedef PRStates::TContext TContext;

    static CNodeActivityBase* New( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
    void SetDestroyFlag();
    TBool DestroyFlag() const;

    DECLARE_SMELEMENT_HEADER( TMarkNodeForDestruction, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TMarkNodeForDestruction )

    DECLARE_SMELEMENT_HEADER( TNoTagOrNoTagBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrNoTagBackwards )

    DECLARE_SMELEMENT_HEADER( TNoTagOrNoClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrNoClients )

    DECLARE_SMELEMENT_HEADER( TControlProviderNoTagOrNoClients, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TControlProviderNoTagOrNoClients )

    ~CDestroyOrphansActivity();

protected:
    explicit CDestroyOrphansActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    virtual void Destroy();

private:
    TBool iDestroyFlag;         // whether to destroy node on destruction of activity
    };

//-=========================================================
//
//Looping Activity
//
//-=========================================================
class ACountLoopActivity
/*
*/
	{
public:
    static const TInt KInterfaceId = 0x102864D9;

public:
	void SetCount(TInt aCount)
		{
		iCount = aCount;
		}

	TInt DecCount()
		{
		return --iCount;
		}

protected:
	ACountLoopActivity()
	:	iCount(0)
		{
		}

	IMPORT_C virtual ~ACountLoopActivity();

protected:
    TInt iCount;

protected:
    typedef CoreStates::TContext TContext;

public:
    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoTagBackwards, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrNoTagBackwards )
	};

class CCountLoopActivityBase : public MeshMachine::CNodeActivityBase, CoreActivities::ACountLoopActivity,
                               public ITFHIERARCHY_1(CCountLoopActivityBase, ACountLoopActivity)
/*
*/
	{
public:
	typedef ITFHIERARCHY_1(CCountLoopActivityBase, ACountLoopActivity) TIfStaticFetcherNearestInHierarchy;

public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	IMPORT_C virtual ~CCountLoopActivityBase();
    void ReturnInterfacePtrL(CoreActivities::ACountLoopActivity*& aInterface)
        {
        aInterface = this;
        }

protected:
	IMPORT_C CCountLoopActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

//-=========================================================
//
//Binding Activity
//
//-=========================================================
class ABindingActivity
/*
ABindingActivity is a helper class for activities requesting binders (sending TCFServiceProvider::TCommsBinderRequest) and, 
following consumption of the binder, needing to respond to the recipient of TCFServiceProvider::TCommsBinderRequest with 
TCFServiceProvider::TBindToComplete as implied by the TCFServiceProvider::TCommsBinderRequest protocol. Such activities, 
after they receive TCFServiceProvider::TCommsBinderResponse need to remember the TNodeCtxId of the sender in order to 
correctly respond with TCFServiceProvider::TBindToComplete. They can remember the sender of 
TCFServiceProvider::TCommsBinderResponse by calling ABindingActivity::StoreOriginator and subsequently respond by using 
ABindingActivity::TSendBindToComplete.
*/
	{
public:
    static const TInt KInterfaceId = 0x102864DD;
    IMPORT_C void StoreOriginator(const Messages::TRuntimeCtxId& aNodeCtxId);

    //Will diagnostically panic if replying when !IsBinding()
    IMPORT_C void ReplyToOriginator(TInt aError = KErrNone);
    inline TBool IsBinding()
    	{
    	return !iOriginator.IsNull();
    	}

protected:
	ABindingActivity(const Messages::TNodeId& aOurNode)
	:	iOurNode(aOurNode)
		{
		}

	void FinalReplyToOriginator(TInt aError);
	IMPORT_C virtual ~ABindingActivity();

protected:
    Messages::TNodeCtxId iOriginator;
    Messages::TNodeId iOurNode;

protected:
    typedef CoreStates::TContext TContext;

public:
    EXPORT_DECLARE_SMELEMENT_HEADER( TSendBindToComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendBindToComplete )
    
	};

} //namespace CoreActivities












namespace PRActivities
{
	//Activity Map provided by CorePr to be used by SCprs.
	//(it is further extended in CoreSCpr).
	DECLARE_EXPORT_ACTIVITY_MAP(coreActivitiesSCpr)

	//Activity Map provided by CorePr to be used by Cprs.
	//(it is further extended in CoreCpr).
	DECLARE_EXPORT_ACTIVITY_MAP(coreActivitiesCpr)

	//Activity Map provided by CorePr to be used by MCprs.
	//(it is further extended in CoreMCpr).
	DECLARE_EXPORT_ACTIVITY_MAP(coreActivitiesMCpr)

	//Activity Map provided by CorePr to be used by TMs.
	//(it is further extended in CoreTM).
	DECLARE_EXPORT_ACTIVITY_MAP(coreActivitiesTM)



//-=========================================================
//
//CBindToActivity
//
//-=========================================================
class CBindToActivity : public MeshMachine::CNodeParallelActivityBase, CoreActivities::ABindingActivity,
                               public ITFHIERARCHY_1(CBindToActivity, CoreActivities::ABindingActivity)
/*
A readymade BindToRequest activity class implementing ABindingActivity (see ABindingActivity).
*/
	{
public:
	typedef ITFHIERARCHY_1(CBindToActivity, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;
private:
	enum
		{
		EAutocommit = 0x01,
		EDontLeaveServiceProvider = 0x02,
		};

public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    IMPORT_C virtual ~CBindToActivity();
    void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
        {
        aInterface = this;
        }

    Messages::RNodeInterface* NextDataClient();
    Messages::RNodeInterface* CurrentDataClient()
		{
		return iCurrentDataClient;
		}

    void ResetCurrentDataClient()
       	{
      	iCurrentDataClient = NULL;
       	}

    TBool IsAutocommit()
    	{
    	return (iFlags & EAutocommit);
    	}

    TBool DataClientsAutocommit();

    void SetAutocommit(TBool aValue)
    	{
    	iFlags = aValue ? (iFlags | EAutocommit) : (iFlags & ~EAutocommit);
    	}

    void SetDontLeaveServiceProvider()
    	{
    	iFlags = iFlags | EDontLeaveServiceProvider;
    	}

    TBool ShouldLeaveServiceProvider()
    	{
    	return !(iFlags & EDontLeaveServiceProvider);
    	}

    void AddClientAsSuccessfulL(Messages::RNodeInterface* aDataClient);
    void RemoveClientFromSuccessful(Messages::RNodeInterface* aDataClient);

protected:
	IMPORT_C CBindToActivity(const MeshMachine::TNodeActivity& aActivitySig,  MeshMachine::AMMNodeBase& aNode, TInt aNextActivityCount);

protected:
	// We cannot store iNewServiceProvider as an RNodeInterface* in case an activity (like GoneDown) executes simultaneously and removes
	// the client we hold a reference to.  Hence use the NodeId and lookup the corresponding RNodeInterface* as and when required.
    Messages::TNodeId iNewServiceProvider;
private:
	Messages::RNodeInterface* iCurrentDataClient;
	RPointerArray<Messages::RNodeInterface> iSuccessfulDataClients;
	TInt iFlags;

protected:
    typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

public:
    EXPORT_DECLARE_SMELEMENT_HEADER( TSendControlClientJoinRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendControlClientJoinRequest )

    EXPORT_DECLARE_SMELEMENT_HEADER( TCommit, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TCommit )

    EXPORT_DECLARE_SMELEMENT_HEADER( TCancel, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TCancel )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrDataClientReady, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrDataClientReady )

    EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindToCompleteOrError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    	virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingBindToCompleteOrError )

    EXPORT_DECLARE_SMELEMENT_HEADER( TCreateDataClient, PRStates::TCreateDataClient, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TCreateDataClient )

    EXPORT_DECLARE_SMELEMENT_HEADER( TSendBindTo, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendBindTo )

    EXPORT_DECLARE_SMELEMENT_HEADER( TSendBindToComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendBindToComplete )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBindToComplete, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrBindToComplete )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrCommit, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrCommit )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerReady, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerReady )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerReadyOrBindToComplete, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerReadyOrBindToComplete )

    EXPORT_DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )
	};


//-=========================================================
//
//Rejoin DataClient Activity
//
//-=========================================================
class CRejoinDataClientActivity : public MeshMachine::CNodeRetryParallelActivity
	{
/*
Used when migrating nodes from one control provider to another. The activity is
designed to run on the current owner of a group of migrated dataclients.
The activity can cache that group, overlook the migration and control error handling.

The activity comes with a set of states and transitions that use the features
provided by the activity class.
*/
public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CRejoinDataClientActivity();
protected:
	CRejoinDataClientActivity(const MeshMachine::TNodeActivity& aActivitySig, TUint aNextActivityCount, MeshMachine::AMMNodeBase& aNode )
	    :MeshMachine::CNodeRetryParallelActivity(aActivitySig, aNode, aNextActivityCount)
		{
		}

public:
    struct TMigrationPairs
        {
    /*
    Migration item - the dataclient and its new owner.
    */
        TMigrationPairs(Messages::RNodeInterface& aDataClient,
                        Messages::TNodeId& aNewOwner)
        :iDataClient(aDataClient),
         iNewOwner(aNewOwner)
         {}

        Messages::RNodeInterface& iDataClient;
        Messages::TNodeId        iNewOwner;
        };

	RArray<TMigrationPairs> iDataClients;

    typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

public:
    EXPORT_DECLARE_SMELEMENT_HEADER( TRejoinDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRejoinDataClient )

    EXPORT_DECLARE_SMELEMENT_HEADER( TApplyRejoin, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TApplyRejoin )

    EXPORT_DECLARE_SMELEMENT_HEADER( TRejoinLoopTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TRejoinLoopTag )

    EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingJoinComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    	virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingJoinComplete )


public:
    class TCFDataClientJoiningRequest : public Messages::TSelfDispatcherAndErrorHandler
    /**
    Is a self-dispatcher for the peerless dataclient rejoin sent by RejoinDataClient.
    The dispatcher will assume the node (aItf) it's working with is the new owner
    of the given dataclient. It will add the dataclient to the new owner, mark it
    as RClientIdenifiedInterface::EJoining, so that the new owner will know the
    subjects of the subsequent TApply and confirm back to the sender.

    @internalComponent
    */
        {
    public:
        TCFDataClientJoiningRequest(const Messages::TNodeId& aDataClient, const Messages::TClientType& aDataClientType)
    	:	iDataClient(aDataClient),
    		iDataClientType(aDataClientType)
            {
            }

	protected:
     	TCFDataClientJoiningRequest()
     		{
     		}

        virtual void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);

	protected:
        Messages::TNodeId iDataClient;
        Messages::TClientType iDataClientType;

    public:
		DECLARE_MVIP_CTR(TCFDataClientJoiningRequest)
		DATA_VTABLE
        };
	};

//-=========================================================
//
//CommsBinder Activity
//
//-=========================================================
class CCommsBinderActivity : public MeshMachine::CNodeRetryParallelActivity,
                             public ITFHIERARCHY_LINK_1(CCommsBinderActivity, MeshMachine::CNodeRetryParallelActivity, CCommsBinderActivity)
/*
The class (or its derivative) must be used for any activity featuring (using):

- TSendDefaultBinderResponse
- TSendNewClientBinderResponse
- TMarkClientActive

CCommsBinderActivity will remember the client that's been sent with a TCommsBinderRequest
and that should be cleared after receiving TBindToComplete.

This class implements a parallel form of CommsBinder activity that is generally used with
SCPRs and CPRs.  See also the non-parallel form (CCommsBinderCombiningActivity) that
is used by MCPRs.
*/
	{
public:
	typedef ITFHIERARCHY_LINK_1(CCommsBinderActivity, MeshMachine::CNodeRetryParallelActivity, CCommsBinderActivity) TIfStaticFetcherNearestInHierarchy;

    IMPORT_C static MeshMachine::CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );

    void ReturnInterfacePtrL(CCommsBinderActivity*& aInterface)
        {
        aInterface = this;
        }

public:
	struct TDataClientMutex
		{
		IMPORT_C static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
		};

	struct TDefaultDataClientMutex
		{
		IMPORT_C static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
		};

public:
    static const TInt KInterfaceId = 0x102864DE;

    IMPORT_C virtual void Cancel(MeshMachine::TNodeContextBase& aContext);    

	IMPORT_C static Messages::RNodeInterface* IsDataClientPresent(MeshMachine::TNodeContextBase& aContext, TUint aClientFlags = 0);

    IMPORT_C void StoreBinder(Messages::RNodeInterface* aDataClient);
   	IMPORT_C Messages::RNodeInterface* Binder() const;

   	IMPORT_C void StoreBinderRequestParameters(const ESock::RCFParameterFamilyBundleC& aBinderRequestParameters);

	void SendBinderResponseToOriginator();
    void SendCustomFlowProvision();
    void BindToComplete();


protected:
	IMPORT_C CCommsBinderActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount);
	IMPORT_C virtual ~CCommsBinderActivity();

public:
	typedef PRStates::TContext TContext;

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrUseExisting, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrUseExisting )

	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrUseExistingBlockedByBinderRequest,
		CCommsBinderActivity::TDataClientMutex,
		CCommsBinderActivity::TNoTagOrUseExisting
		)

	EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindToComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
		IMPORT_C virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TAwaitingBindToComplete )

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrWaitForIncomingOrUseExisting, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrWaitForIncomingOrUseExisting )

	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrWaitForIncomingOrUseExistingBlockedByBinderRequest,
		CCommsBinderActivity::TDefaultDataClientMutex,
		CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExisting
		)

    EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrWaitForIncomingOrUseExistingDefault, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
    	IMPORT_C virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagOrWaitForIncomingOrUseExistingDefault )

	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrWaitForIncomingOrUseExistingDefaultBlockedByBinderRequest,
		CCommsBinderActivity::TDefaultDataClientMutex,
		CCommsBinderActivity::TNoTagOrWaitForIncomingOrUseExistingDefault
		)

    // DoL() exported because it is used in aggregate ProcessDataClientCreation below, which is referenced externally
    EXPORT_DECLARE_SMELEMENT_HEADER( TStorePendingBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TStorePendingBinder )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSendCustomFlowProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendCustomFlowProvision )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSendBinderResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TSendBinderResponse )

    DECLARE_AGGREGATED_TRANSITION3(
       TProcessDataClientCreation,
       PRStates::TProcessDataClientCreation,
       CCommsBinderActivity::TStorePendingBinder,
       CCommsBinderActivity::TSendCustomFlowProvision
       )

private:
    Messages::RNodeInterface* iPendingBinder;	// Node that we are creating
    ESock::RCFParameterFamilyBundleC iBinderRequestParameters;
	};


//-=========================================================
//
//CNoBearer
//
//-=========================================================
class CNoBearer : public MeshMachine::CNodeRetryParallelActivity, public CoreActivities::ABindingActivity,
		public ITFHIERARCHY_LINK_1(CNoBearer, MeshMachine::CNodeRetryParallelActivity, CoreActivities::ABindingActivity)
/*
A readymade NoBearer activity class implementing ABindingActivity (see ABindingActivity).
*/
	{
public:
	struct TServiceProviderMutex
		{
		IMPORT_C static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext);
		};

public:
	typedef ITFHIERARCHY_LINK_1(CNoBearer, MeshMachine::CNodeRetryParallelActivity, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

//public:
    IMPORT_C Messages::TNodePeerId& GetOriginator();

public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CNoBearer();
    IMPORT_C void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface);

protected:
	CNoBearer(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);

protected:
    typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrDataClientsToStart, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrDataClientsToStart )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresent )

	EXPORT_DECLARE_SMELEMENT_HEADER( TStartOriginatingDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TStartOriginatingDataClient )

    EXPORT_DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )

	EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingNoBearer, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    	IMPORT_C virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingNoBearer )

    typedef MeshMachine::TRetryTransition<TRequestCommsBinder, ESock::TCFServiceProvider::TCommsBinderRequest> TRequestCommsBinderRetry;

	DECLARE_SERIALIZABLE_STATE(
		TNoTagOrBearerPresentBlockedByNoBearer,
		CNoBearer::TServiceProviderMutex,
		CNoBearer::TNoTagOrBearerPresent
		)

	EXPORT_DECLARE_SMELEMENT_HEADER( TStoreRequestParameters, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	IMPORT_C virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TStoreRequestParameters )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresentForAutostart, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresentForAutostart )

private:
	// Params from the original NoBearer request
    ESock::RCFParameterFamilyBundleC iNoBearerParameters;

	};


//-=========================================================
//
//Start Activity
//
//-=========================================================
class CStartActivity : public MeshMachine::CNodeRetryActivity, public CoreActivities::ABindingActivity,
                       public ITFHIERARCHY_LINK_2(CStartActivity, MeshMachine::CNodeRetryActivity, CoreActivities::ABindingActivity, MeshMachine::AContextStore)
    {
public:
	typedef ITFHIERARCHY_LINK_2(CStartActivity, MeshMachine::CNodeRetryActivity, CoreActivities::ABindingActivity, MeshMachine::AContextStore) TIfStaticFetcherNearestInHierarchy;

public:
	static const TInt KStartServiceProvider = 1;
	static const TInt KFlowCreated = 2;

public:
    IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
    IMPORT_C virtual ~CStartActivity();

	void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
	    {
	    aInterface = this;
	    }

	void ReturnInterfacePtrL(MeshMachine::AContextStore*& aInterface)
	    {
	    aInterface = this;
	    }

protected:
	CStartActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingBindToCompleteOrCancel, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    	virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingBindToCompleteOrCancel )
    };

//-=========================================================
//
//Gone Down Activity
//
//-=========================================================

class CGoneDownActivity : public MeshMachine::CNodeRetryActivity, protected MeshMachine::APreallocatedOriginators<2>
    {
public:
    IMPORT_C static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CGoneDownActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual TBool IsIdle() const;
	virtual void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);
	virtual ~CGoneDownActivity();
    virtual void Destroy();

private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

public:
    TUint iGoneDownApId;

protected:
    typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;

public:
    EXPORT_DECLARE_SMELEMENT_HEADER( TIgnoreOrPropagate, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
		virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TIgnoreOrPropagate )

    EXPORT_DECLARE_SMELEMENT_HEADER( TSendErrorRecoveryReq, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendErrorRecoveryReq )
	};

//-=========================================================
//
//Stop Activity
//
//-=========================================================
/*
class CStopActivity : public MeshMachine::CNodeRetryActivity
	{
	static const TUint KPreallocatedStopActivitySize = sizeof(CStopActivity) + sizeof(MeshMachine::APreallocatedOriginators<10>);
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
protected:
	CStopActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CStopActivity();
    virtual void Destroy();
private:
*/
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead
	*/

/*
static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	};
*/

} //namespace PRActivities



#endif //SYMBIAN_SS_COREPRACTIVITIES_H

