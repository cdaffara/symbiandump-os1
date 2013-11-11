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

#ifndef SYMBIAN_SS_SUBCONNSTATES_H
#define SYMBIAN_SS_SUBCONNSTATES_H

#include <comms-infras/ss_esockstates.h>
#include <comms-infras/ss_esockactivities.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/sockmes.h>
#include "ss_subconn.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESocks_sbcnsts, "ESocks_sbcnsts.");
#endif

//-=========================================================
//
//Activities
//
//-=========================================================
namespace SubConnActivities
{
class CSubConnBindingActivityBase : public SubSessActivities::CESockClientActivityBase, public CoreActivities::ABindingActivity,
                public ITFHIERARCHY_LINK_1(CSubConnBindingActivityBase, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity)
	{
public:
	typedef ITFHIERARCHY_LINK_1(CSubConnBindingActivityBase, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CSubConnBindingActivityBase() {}

    void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
        {
        aInterface = this;
        }

    void CompleteMessage();

protected:
	CSubConnBindingActivityBase( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	:	SubSessActivities::CESockClientActivityBase(aActivitySig, aNode),
		CoreActivities::ABindingActivity(aNode.Id()),
	    TIfStaticFetcherNearestInHierarchy(this)
		{
		}
	};


//-=========================================================
//CNoBearer
//-=========================================================
/**
@internalComponent
*/
class CNoBearer : public CSubConnBindingActivityBase
	{
public:
	typedef ITFHIERARCHY_LINK_1(CNoBearer, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CNoBearer();

    inline ESock::RCFParameterFamilyBundleC& GetFlowRequestParameters()
    	{return iFlowRequestBundle;}

    void SetFlowRequestParameters(const ESock::RCFParameterFamilyBundleC& aParams);
		
    void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
        {aInterface = this;}

DECLARE_SMELEMENT_HEADER( TStoreFlowParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreFlowParams )

protected:
	CNoBearer( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	:	CSubConnBindingActivityBase(aActivitySig, aNode)
		{
		}
private:
    ESock::RCFParameterFamilyBundleC iFlowRequestBundle;
	};


//-=========================================================
//CBuildStack
//-=========================================================
class CBuildStack : public MeshMachine::CNodeActivityBase, public CoreActivities::ABindingActivity,
	public ITFHIERARCHY_1(CBuildStack, CoreActivities::ABindingActivity)
	{
public:
	typedef ITFHIERARCHY_1(CBuildStack, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

    void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
        {
        aInterface = this;
        }

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CBuildStack();

protected:
	CBuildStack(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

public:
typedef MeshMachine::TNodeContext<ESock::CSubConnection, SubSessStates::TContext> TContext;
DECLARE_SMELEMENT_HEADER( TSendBuildStackResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBuildStackResponse )

DECLARE_SMELEMENT_HEADER( TAwaitingBuildStack, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBuildStack )

DECLARE_SMELEMENT_HEADER( TRequestServiceProviderFromCPR, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestServiceProviderFromCPR )

private:
	void RequestServiceProviderFromCPRL(TContext& aContext);
};


//-=========================================================
//CCreate
//-=========================================================
class CCreate : public CSubConnBindingActivityBase
	{
public:
	typedef ITFHIERARCHY_LINK_1(CCreate, SubSessActivities::CESockClientActivityBase, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CCreate();

protected:
	CCreate(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CSubConnBindingActivityBase(aActivitySig, aNode),
	    iSubConnectionNode(aNode.Id())
		{
		}
private:
	//We are taking ownership of the sub session for the duration of this activity
	//If something goes wrong, we need to destroy this subsession.
    Messages::TNodeId iSubConnectionNode;
	};

//-=========================================================
//CRejoin
//-=========================================================
class ESock::CSubConnection;
class CRejoin : public SubSessActivities::CESockClientActivityBase
/*

*/
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CRejoin();
	const Messages::TNodeId& OldOwner()
    	{
    	__ASSERT_DEBUG(!iOldOwner.IsNull(), User::Panic(KSpecAssert_ESocks_sbcnsts, 1));
    	return iOldOwner;
    	}

	const Messages::TNodeId& NewOwner()
    	{
    	__ASSERT_DEBUG(!iNewOwner.IsNull(), User::Panic(KSpecAssert_ESocks_sbcnsts, 2));
    	return iNewOwner;
    	}

  	const Messages::TNodeId& TheOtherOwner()
    	{
    	__ASSERT_DEBUG(!iOldOwner.IsNull() && !iNewOwner.IsNull(), User::Panic(KSpecAssert_ESocks_sbcnsts, 3));
    	return iMessage.Function() == ESCAddSocket ? iOldOwner : iNewOwner;
    	}

	const Messages::TNodeId& Flow()
    	{
    	__ASSERT_DEBUG(!iFlow.IsNull(), User::Panic(KSpecAssert_ESocks_sbcnsts, 4));
    	return iFlow;
    	}

	void InitialiseL(ESock::CSubConnection& aSC);

    void SetOldOwnerIntf(Messages::RNodeInterface * aIntf)
        {
        iOldOwnerIntf = aIntf;
        }
	
	Messages::RNodeInterface* OldOwnerIntf()
	    {
        __ASSERT_DEBUG(iOldOwnerIntf != NULL, User::Panic(KSpecAssert_ESocks_sbcnsts, 1));
        return iOldOwnerIntf;
	    }

protected:
	CRejoin( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	:	SubSessActivities::CESockClientActivityBase(aActivitySig, aNode)
		{
		}


private:
    Messages::TNodeId iOldOwner;
    Messages::TNodeId iNewOwner;
    Messages::TNodeId iFlow;
	
	Messages::RNodeInterface * iOldOwnerIntf;
	};

//-=========================================================
//CSetParameters
//-=========================================================
class CSetParameters : public SubSessActivities::CESockClientActivityBase
/*

*/
	{
public:
	typedef MeshMachine::TNodeContext<ESock::CSubConnection, SubSessStates::TContext> TContext;
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CSetParameters();

    ESock::RCFParameterFamilyBundleC& GetParameterBundleL();
    void CompleteMessage();

protected:
	CSetParameters( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	:	SubSessActivities::CESockClientActivityBase(aActivitySig, aNode)
		{
		}
private:
	void StoreParametersInSubConn(TContext& aContext);

    ESock::RCFParameterFamilyBundleC iParameterBundle;

public:
DECLARE_SMELEMENT_HEADER( TCompleteClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
 	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCompleteClient )

DECLARE_SMELEMENT_HEADER( TStoreNewParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreNewParams )

	};

//-=========================================================
//CEventNotification
//-=========================================================
class CEventNotification : public SubSessActivities::CESockClientActivityBase
/*

*/
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	virtual ~CEventNotification();

    void RequestEventNotificationSetupL();
    void RequestAllEventNotificationsL();
    TBool FillInEvent(const CSubConNotificationEvent& aEvent);

protected:
	CEventNotification( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	:	SubSessActivities::CESockClientActivityBase(aActivitySig, aNode)
		{
		}
private:
	RSubConnection::TEventFilter iEventUidFilterList[KMaxUidFilterListLen];
	TUint		                 iEventUidFilterListLength;
	};

} //namespace SubConnActivities


//-=========================================================
//
//States
//
//-=========================================================
namespace SubConnStates
{
typedef MeshMachine::TNodeContext<ESock::CSubConnection, SubSessStates::TContext> TContext;

//-=========================================================
//Create
//-=========================================================
DECLARE_SMELEMENT_HEADER( TNoTagOrWaitForIncoming, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
 	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrWaitForIncoming )

DECLARE_SMELEMENT_HEADER( TJoinCPR, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinCPR )

DECLARE_SMELEMENT_HEADER( TRequestServiceProvider, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestServiceProvider )

DECLARE_SMELEMENT_HEADER( TLeaveCPR, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TLeaveCPR )

DECLARE_SMELEMENT_HEADER( TNoTagOrAttachToDefaultOrWaitForIncoming, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
 	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrAttachToDefaultOrWaitForIncoming )

//-=========================================================
//Rejoin
//-=========================================================
DECLARE_SMELEMENT_HEADER( TJoinTheOtherOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinTheOtherOwner )

DECLARE_SMELEMENT_HEADER( TLeaveTheOtherOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TLeaveTheOtherOwner )

DECLARE_SMELEMENT_HEADER( TSendRejoinDataClientRequestToOldOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendRejoinDataClientRequestToOldOwner )

DECLARE_SMELEMENT_HEADER( TSendApplyToOldOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendApplyToOldOwner )

DECLARE_SMELEMENT_HEADER( TSendApplyToNewOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendApplyToNewOwner )

DECLARE_SMELEMENT_HEADER( TSendCancelToOldOwner, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendCancelToOldOwner )

//-=========================================================
//Get/Set Params
//-=========================================================
DECLARE_SMELEMENT_HEADER( TSendParamsToServiceProvider, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendParamsToServiceProvider )

DECLARE_SMELEMENT_HEADER( TStoreParams, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreParams )

DECLARE_AGGREGATED_TRANSITION2(
    TStoreParamsAndSendApply,
	SubConnStates::TStoreParams,
	CoreNetStates::TSendApplyRequest
	)

DECLARE_SMELEMENT_HEADER( TSendParamRequest, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendParamRequest )

DECLARE_SMELEMENT_HEADER( TNoTagOrParamsPresent, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrParamsPresent )

DECLARE_SMELEMENT_HEADER( TWriteSubConnParamsLength, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TWriteSubConnParamsLength )

DECLARE_SMELEMENT_HEADER( TWriteSubConnParams, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TWriteSubConnParams )

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrParamsPresentBlockedBySetParams,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityParamRequest>,
	SubConnStates::TNoTagOrParamsPresent
	)

//-=========================================================
//Events
//-=========================================================
DECLARE_SMELEMENT_HEADER( TAwaitingEventNotificationSubscription, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingEventNotificationSubscription )

DECLARE_SMELEMENT_HEADER( TNoTagOrAllNotifications, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrAllNotifications )

DECLARE_SMELEMENT_HEADER( TSetupFilteredEventNotification, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetupFilteredEventNotification )

DECLARE_SMELEMENT_HEADER( TSetupAllEventNotification, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetupAllEventNotification )

DECLARE_SMELEMENT_HEADER( TFillInEvent, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TFillInEvent )

DECLARE_SMELEMENT_HEADER( TEnqueueEvent, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
	protected:
    void CreateOrUpdateBundleL(CSubConGenEventParamsGranted* aEvent);
DECLARE_SMELEMENT_FOOTER( TEnqueueEvent )

DECLARE_SMELEMENT_HEADER( TNoTagOrActiveWhenEventEnqued, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrActiveWhenEventEnqued )



//-=========================================================
//Start/Stop SubConnection
//-=========================================================
DECLARE_SMELEMENT_HEADER( TStartSubConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartSubConnection )

DECLARE_SMELEMENT_HEADER( TStopSubConnection, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStopSubConnection )

//-=========================================================
//Close
//-=========================================================
DECLARE_SMELEMENT_HEADER( TDetachControlProvider, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDetachControlProvider )

DECLARE_AGGREGATED_TRANSITION2(
	TProcessClose,
	SubSessStates::TProcessClose,
	TDetachControlProvider
	)

//-=========================================================
// Build stack
//-=========================================================

DECLARE_SMELEMENT_HEADER( TAwaitingBuildStackResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBuildStackResponse )

DECLARE_SMELEMENT_HEADER( TSendBuildStack, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBuildStack )

} //namespace SubConnStates


#endif
// SYMBIAN_SS_SUBCONNSTATES_H

