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
 @internalTechnology
 @released
*/

#if !defined(__SS_FLOWREQUESTSTATES_H__)
#define __SS_FLOWREQUESTSTATES_H__

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include "ss_nodemessages_dataclient.h"

#include "ss_flowrequest.h"

#include <elements/nm_messages_child.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESocksflwrqststs, "ESocksflwrqststs");
#endif

namespace ESock
{
	class CFlowRequest;
}

namespace FlowRequestStates
{

typedef MeshMachine::TNodeContext<ESock::CFlowRequest, CoreStates::TContext> TContext;
typedef MeshMachine::TNodeContext<ESock::CImplicitFlowRequest, FlowRequestStates::TContext> TImplicitContext;


//
//Implicit and common

DECLARE_SMELEMENT_HEADER( TAwaitingImplicitFlowRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingImplicitFlowRequest )

DECLARE_SMELEMENT_HEADER( TAwaitingConnFlowRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingConnFlowRequest )

DECLARE_SMELEMENT_HEADER( TAwaitingSubConnFlowRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSubConnFlowRequest )

DECLARE_SMELEMENT_HEADER( TRequestCSRCreation, MeshMachine::TStateTransition<TImplicitContext>, NetStateMachine::MStateTransition, TImplicitContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCSRCreation )

DECLARE_SMELEMENT_HEADER( TSelectMetaPlane, MeshMachine::TStateTransition<TImplicitContext>, NetStateMachine::MStateTransition, TImplicitContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSelectMetaPlane )

DECLARE_SMELEMENT_HEADER( TJoinReceivedSCpr, MeshMachine::TStateTransition<TImplicitContext>, NetStateMachine::MStateTransition, TImplicitContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinReceivedSCpr )

DECLARE_SMELEMENT_HEADER( TRequestCommsBinderFromSCpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinderFromSCpr )

DECLARE_SMELEMENT_HEADER( TRemoveRequestor, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRemoveRequestor )

DECLARE_SMELEMENT_HEADER( TSendNoBearer, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendNoBearer )

DECLARE_SMELEMENT_HEADER( TJoinSubConnection, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinSubConnection )

DECLARE_SMELEMENT_HEADER( TForwardBindToMsgToOriginator, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardBindToMsgToOriginator )

DECLARE_SMELEMENT_HEADER( TLeaveSubConnection, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TLeaveSubConnection )

DECLARE_SMELEMENT_HEADER( TSendClientLeavingAndRemoveControlProvider, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendClientLeavingAndRemoveControlProvider )


//
//Cpr specific
DECLARE_SMELEMENT_HEADER( TJoinCpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinCpr )

//
//SCpr specific
DECLARE_SMELEMENT_HEADER( TJoinSCpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TJoinSCpr )

} //namespace FlowRequestStates


namespace FlowRequestActivities
{

class CFlowRequestActivity : public MeshMachine::CNodeActivityBase, public CoreActivities::ABindingActivity,
                             public ITFHIERARCHY_1(CFlowRequestActivity, CoreActivities::ABindingActivity)
	{
public:
	typedef ITFHIERARCHY_1(CFlowRequestActivity, CoreActivities::ABindingActivity) TIfStaticFetcherNearestInHierarchy;

public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    	{
		return new (ELeave) CFlowRequestActivity(aActivitySig,aNode);
    	}

	void ReturnInterfacePtrL(CoreActivities::ABindingActivity*& aInterface)
		{
		aInterface = this;
		}

protected:
	CFlowRequestActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:MeshMachine::CNodeActivityBase(aActivitySig, aNode),
	 ABindingActivity(aNode.Id()),
	 TIfStaticFetcherNearestInHierarchy(this)
		{
		}

	virtual ~CFlowRequestActivity()
		{
		if( !iBindToSender.IsNull() )
		    {
		    TInt err = Error() ? Error() : KErrAbort;
		    Messages::RNodeInterface::OpenPostMessageClose(Messages::TNodeCtxId(ActivityId(), iNode.Id()),
		            iBindToSender, Messages::TEBase::TError(ESock::TCFDataClient::TBindToComplete::Id(), err).CRef());
		    }
		
		static_cast<ESock::CFlowRequest&>(iNode).iFlowParameters.Close();
		__ASSERT_DEBUG(iNode.CountAllActivities()==0, User::Panic(KSpecAssert_ESocksflwrqststs, 1));
#ifndef __GCCXML__
		Messages::RNodeInterface::OpenPostMessageClose(Messages::TNodeCtxId(ActivityId(), iNode.Id()),
			iNode.Id(), Messages::TEChild::TDestroy().CRef());
#endif
		}
	
public:
    /* A node that has sent TCFDataClient::TBindTo. The response TCFDataClient::TBindToComplete
     * must be sent to this node. 
     */ 
    Messages::TNodeCtxId iBindToSender;

public:
	typedef MeshMachine::TNodeContext<ESock::CFlowRequest, CoreStates::TContext> TContext;
	
DECLARE_SMELEMENT_HEADER( TAwaitingBindTo, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBindTo )
	
DECLARE_SMELEMENT_HEADER( TStoreFlowParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreFlowParams )

DECLARE_SMELEMENT_HEADER( TSendBindToComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBindToComplete )
	};

class CFlowRequestDestroyActivity : public MeshMachine::CNodeActivityBase, protected MeshMachine::APreallocatedOriginators<1>
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    	{
		TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CFlowRequestDestroyActivity));
		CFlowRequestDestroyActivity* self = new (space) CFlowRequestDestroyActivity(aActivitySig, aNode);
		self->InsertPreallocatedDestroyActivity(); //Destructing preallocated activity
		return self;
	   	}

protected:
	CFlowRequestDestroyActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:MeshMachine::CNodeActivityBase(aActivitySig, aNode),
	MeshMachine::APreallocatedOriginators<1>(iOriginators)
		{
		}

	void Destroy()
		{
		ReturnPreallocatedSpace(this);
		this->~CFlowRequestDestroyActivity(); //Run the destructor

		delete &static_cast<ESock::CFlowRequest&>(iNode);
		}

private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};


} //namespace FlowRequestActivities


#endif
// __SS_FLOWREQUESTSTATES_H__


