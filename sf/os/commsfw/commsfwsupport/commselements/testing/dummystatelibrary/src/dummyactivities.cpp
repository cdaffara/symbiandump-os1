// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "dummyactivities.h"
#include "dummystates.h"
#include "dummymessages.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingdmyct, "ElemTestingdmyct");
#endif

using namespace Dummy;
using namespace DummyActivities;
using namespace DummyStates;
using namespace Messages;
using namespace MeshMachine;


//(1) CStopActivity constructs itself over the preallocated space
//(2) CStopActivity preallocates space for only one originator
//    (as in our demo only the last client triggers the activity)
class CStopActivity : public MeshMachine::CNodeRetryActivity, protected MeshMachine::APreallocatedOriginators<1>
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CStopActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeRetryActivity(aActivitySig, aNode),
		APreallocatedOriginators<1>(iOriginators)
		{
		}

	virtual void Destroy();
	};

//(1) CDestroyActivity constructs itself over the preallocated space
//(2) CDestroyActivity preallocates space for only one originator
//    (as in our demo only the host node itself can trigger it)
class CDestroyActivity : public MeshMachine::CNodeActivityBase, protected MeshMachine::APreallocatedOriginators<1>
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CDestroyActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeActivityBase(aActivitySig, aNode),
		APreallocatedOriginators<1>(iOriginators)
		{
		}

	virtual void Destroy();

//States, StateForks & StateTransitions specific to the CDestroyActivity
protected:
	typedef DummyStates::TContext TContext;

public:
    DECLARE_SMELEMENT_HEADER( TMarkForDestruction, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
    	virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TMarkForDestruction )
private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);    
	};


namespace CoreErrorActivity
{
	typedef DummyStates::TContext TContext;

	DECLARE_SMELEMENT_HEADER( TAwaitingError, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
		virtual TBool Accept();
	DECLARE_SMELEMENT_FOOTER( TAwaitingError )

	DEFINE_SMELEMENT(TAwaitingError, NetStateMachine::MState, TContext)
	TBool TAwaitingError::Accept()
		{
		if (!iContext.iMessage.IsMessage<TEBase::TError>())
			{
			return EFalse;
			}

	#ifdef _DEBUG
		//Diagnostic panic only. TError message should not be travelling around with KErrNone.
		//If you see this panic, please send a proper error code.
		TEBase::TError& errroMessage = message_cast<TEBase::TError>(iContext.iMessage);
		__ASSERT_DEBUG(errroMessage.iValue != KErrNone, User::Panic(KSpecAssert_ElemTestingdmyct, 1));
	#endif

	    return ETrue;
	    }

	DECLARE_SMELEMENT_HEADER( TProcessError, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TProcessError )

	DEFINE_SMELEMENT(TProcessError, NetStateMachine::MStateTransition, TContext)
	void TProcessError::DoL()
		{
		//Find matching activity, if any
	    CNodeActivityBase* a = iContext.Node().FindAddressedActivity(iContext);
	    
	    //We do not allow any unsolicited errors here
	    __ASSERT_DEBUG(a, User::Panic(KSpecAssert_ElemTestingdmyct, 2));

	    //We are started based on the fact the the last message iContext.Node()'s received is TError
	    TEBase::TError& errmsg = message_cast<TEBase::TError>(iContext.iMessage);
	    a->SetError(errmsg.iValue);
	  	a->SetIdle();
		}
}


//
//Activities
//For more information about single-tripple activities see MeshMachineFundamentals.doc (par. 5.2)
//For more information about parallel activities see MeshMachineFundamentals.doc (par. TBA)

namespace CoreErrorActivity
{
//This activity can not fail.
//This activity must complete within a single call stack or it would need to be a paralel one.
//This activity is not cancellable as its actions are irreversible.
DECLARE_DEFINE_NODEACTIVITY(EActivityError, CoreError, TEBase::TError)
	NODEACTIVITY_ENTRY(KNoTag, CoreErrorActivity::TProcessError, CoreErrorActivity::TAwaitingError, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace DummyNodeJoinActivity
{
//This activity does not require the activity object.
//This activity must complete within a single call stack or it would need to be a paralel one.
//This activity is cancellable.
//since every originator wants to be separately added to the client's list.
//When adding of the client is not successful, the first transition will post a self destruct
//message to invoke the destroy activity.
DECLARE_DEFINE_NODEACTIVITY(EActivityClientJoin, DummyNodeJoin, TBindProtocol::TClientJoiningRequest)
	//TAddClient posts TError to the Client (and TDestroy to the node) if an error occurs.
	NODEACTIVITY_ENTRY(KNoTag, DummyStates::TCommenceJoinAndAddClientAndPostJoinComplete, DummyStates::TAwaitingClientJoiningRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace DummyNodeLeaveActivity
{
//This activity can not fail.
//This activity must complete within a single call stack or it would need to be a paralel one.
//This activity is not cancellable as its actions are irreversible.
//This activity does not require the activity object and it does not allocate any memory.
//It "offloads" the object destruction to the destroy activity.
DECLARE_DEFINE_NODEACTIVITY(EActivityClientLeave, DummyNodeLeave, TEPeer::TLeaveRequest)
	NODEACTIVITY_ENTRY(KNoTag, DummyStates::TPostLeaveCompleteAndRemoveClientAndPostDestroyIfLast, MeshMachine::TAwaitingClientLeavingRequest, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

namespace DummyNodeStartActivity
{
//This activity can fail.
//This activity is cancellable by its originators.
//This activity is synchronised with the DummyNodeStop activity.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(EActivityStart, DummyNodeStart, TStartProtocol::TStart, CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(DummyStates::TAwaitingStart, DummyStates::TNoTagOrStartedOrNoServiceProviderBlockedByStop)
	//Started ServiceProvider already present
	LAST_NODEACTIVITY_ENTRY(KStarted, DummyStates::TPostStarted)
	//No ServiceProvider present
	NODEACTIVITY_ENTRY(KNoServiceProvider, DummyStates::TCreateServiceProvider, DummyStates::TAwaitingFactoryObjectCreated, MeshMachine::TTag<KNoServiceProvider>)
	NODEACTIVITY_ENTRY(KNoServiceProvider, DummyStates::TPostClientJoiningRequest, DummyStates::TAwaitingJoinComplete, MeshMachine::TNoTag)
	//Non-started ServiceProvider already present
	//TAddServiceProvider will post TLeaveRequest to the Service Provider should an error occur
	NODEACTIVITY_ENTRY(KNoTag, DummyStates::TAddServiceProviderAndPostStart, DummyStates::TAwaitingStarted, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, DummyStates::TPostStarted)
NODEACTIVITY_END()
}

namespace DummyNodeStopActivity
{
//This activity can not fail but needs the activity object (therefore uses preallocated activity slot).
//This activity is not cancellable as its actions are irreversible.
//This activity is synchronised with the DummyNodeStart activity.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(EActivityStop, DummyNodeStop, TStopProtocol::TStop, CStopActivity::New)
	FIRST_NODEACTIVITY_ENTRY(DummyStates::TAwaitingStop, DummyStates::TNoTagOrStartedOrNoServiceProviderBlockedByStart)
	//Started ServiceProvider present
	NODEACTIVITY_ENTRY(KStarted, DummyStates::TPostStop, DummyStates::TAwaitingStopped, MeshMachine::TNoTag)
	//ServiceProvider present
	NODEACTIVITY_ENTRY(KNoTag, DummyStates::TPostClientLeavingRequest, MeshMachine::TAwaitingLeaveComplete, MeshMachine::TTag<KNoServiceProvider>)
	//No ServiceProvider present
	LAST_NODEACTIVITY_ENTRY(KNoServiceProvider, DummyStates::TPostStopped)
NODEACTIVITY_END()
}

namespace DummyNodeDestroyActivity
{
//For our demonstration purpose this activity implements only an internal protocol
//(i.e. nodes do not send TDestroy between each other).

//This activity is started when:
//1) Join request from our only Client fails (plus there are no "outstanding" Clients
//   who have already taken a handle to this node from the Factory and are about to
//   send us TClientJoiningRequest),
//2) The last Client sends us TLeaveRequest (and there are no outstanding
//   Clients who have already taken a handle to this node from the Factory).

//This activity can not fail but needs the activity object as it will destruct the host node.
//Destroying activities destroy their hosts when going out of scope.
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(EActivityDestroy, DummyNodeDestroy, TEChild::TDestroy, CDestroyActivity::New)
	FIRST_NODEACTIVITY_ENTRY(DummyStates::TAwaitingDestroy, MeshMachine::TNoTag)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CDestroyActivity::TMarkForDestruction, DummyStates::TNoTagOrNoServiceProvider)
	//The next tripple will loop for as long as there are any Service Providers left
	NODEACTIVITY_ENTRY(KNoTag, DummyStates::TPostClientLeavingRequest, MeshMachine::TAwaitingLeaveComplete, DummyStates::TNoTagBackwardsOrNoServiceProvider)
	LAST_NODEACTIVITY_ENTRY(KNoServiceProvider, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace DummyActivities
{
DEFINE_EXPORT_ACTIVITY_MAP(dummyActivities)
	//The error activity must be at the bottom of the activitys' map because
	//it should see the TError message last, after all of the other activities
	//got their chances to handle it.
	ACTIVITY_MAP_ENTRY(CoreErrorActivity, CoreError)
	ACTIVITY_MAP_ENTRY(DummyNodeJoinActivity, DummyNodeJoin)
	ACTIVITY_MAP_ENTRY(DummyNodeLeaveActivity, DummyNodeLeave)
	ACTIVITY_MAP_ENTRY(DummyNodeStartActivity, DummyNodeStart)
	ACTIVITY_MAP_ENTRY(DummyNodeStopActivity, DummyNodeStop)
	ACTIVITY_MAP_ENTRY(DummyNodeDestroyActivity, DummyNodeDestroy)
ACTIVITY_MAP_END()
}

CNodeActivityBase* CStopActivity::New(const TNodeActivity& aActivitySig, AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CStopActivity));
	CStopActivity* self = new (space) CStopActivity(aActivitySig, aNode);
	self->AppendPreallocatedActivity(); //Non-destructing preallocated activity
	return self;
	}

void CStopActivity::Destroy()
	{
	ReturnPreallocatedSpace(this);
	this->~CStopActivity(); //Run the destructor
	}


CNodeActivityBase* CDestroyActivity::New(const TNodeActivity& aActivitySig, AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CDestroyActivity));
	CDestroyActivity* self = new (space) CDestroyActivity(aActivitySig, aNode);
	self->InsertPreallocatedDestroyActivity(); //Destructing preallocated activity
	return self;
	}

void CDestroyActivity::Destroy()
	{
	ReturnPreallocatedSpace(this);
	this->~CDestroyActivity(); //Run the destructor

	//Delete the provider.
	static_cast<CTestNodeBase&>(iNode).DeleteMeNow();
	}

DEFINE_SMELEMENT(CDestroyActivity::TMarkForDestruction, NetStateMachine::MStateTransition, CDestroyActivity::TContext)
void CDestroyActivity::TMarkForDestruction::DoL()
	{
	//Mark the node for deletion, so that it's not served by the factory from now on.
    iContext.Node().MarkMeForDeletion();
	}

