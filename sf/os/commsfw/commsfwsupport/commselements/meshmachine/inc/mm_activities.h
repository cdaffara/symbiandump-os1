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
// generic framework node state machine states
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_MM_ACTIVITIES_H
#define SYMBIAN_MM_ACTIVITIES_H

#include <elements/mm_node.h>
#include <elements/mm_nodepeer.h>
#include <elements/mm_activities_internal.h>
#include <elements/nm_address_internal.h>
#include <elements/nm_signatures.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachActH, "ElemMeshMachActH");
#endif

//-=========================================================
//
//Panics
//
//-=========================================================
_LIT (KMMActivityPanic,"MMActivityPanic");
enum
	{
	EPanicCorruptedContext = 1,
	EPanicNoPreallocatedSpace = 2,
	EPanicOutOfActivities = 3,
	EPanicOutOfBounds
	};

//-=========================================================
//
// MACROS
//
//-=========================================================

/**
Declare and export a node activity.

@param name  The name of the activity, which will be used to put it into the node's activity map

@see DECLARE_NODEACTIVITY
*/
#define DECLARE_EXPORT_NODEACTIVITY( name ) \
	struct name \
		{ \
		IMPORT_C static const MeshMachine::TNodeActivity& Self(); \
		IMPORT_C static const NetStateMachine::TStateTriple& FirstTriple(); \
		static const MeshMachine::TNodeActivity iSelf; \
		static const NetStateMachine::TStateTriple iData[]; \
		};

/**
Declares a node activity. A node activity is a state machine. It contains multiple nodeactivity
entries, which respresent the states, transitions and forks of the state machine.

@param name  The name of the activity, which will be used to put it into the node's activity map

@see DEFINE_NODEACTIVITY
@see NODEACTIVITY_ENTRY
*/
#define DECLARE_NODEACTIVITY( name ) \
	struct name \
		{ \
		inline static const MeshMachine::TNodeActivity& Self() {return iSelf;} \
		inline static const NetStateMachine::TStateTriple& FirstTriple() {return iData[1];} \
		static const MeshMachine::TNodeActivity iSelf; \
		static const NetStateMachine::TStateTriple iData[]; \
		};

#ifdef SYMBIAN_TRACE_ENABLE
/**
Define and export a custom node activity.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_CUSTOM_NODEACTIVITY
*/
	#define DEFINE_EXPORT_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		EXPORT_C const MeshMachine::TNodeActivity& name :: Self() {return iSelf;} \
		EXPORT_C const NetStateMachine::TStateTriple& name :: FirstTriple() {return iData[1];} \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::ENodeCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, _S8(#name)}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

/**
Define a custom node activity. A custom node activity is a node activity which defines its own
node activity class. This is used, for example, if the activity wishes to share some custom context
information between the states and transition in the activity.

Node activity class must derive from CNodeActivityBase.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_NODEACTIVITY
@see CNodeActivityBase
*/
	#define DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::ENodeCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, _S8(#name)}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

#else

/**
Define and export a custom node activity.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_CUSTOM_NODEACTIVITY
*/
	#define DEFINE_EXPORT_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		EXPORT_C const MeshMachine::TNodeActivity& name :: Self() {return iSelf;} \
		EXPORT_C const NetStateMachine::TStateTriple& name :: FirstTriple() {return iData[1];} \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::ENodeCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, NULL}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

/**
Define a custom node activity. A custom node activity is a node activity which defines its own
node activity class. This is used, for example, if the activity wishes to share some custom context
information between the states and transition in the activity.

Node activity class must derive from CNodeActivityBase.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_NODEACTIVITY
@see CNodeActivityBase
*/
	#define DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::ENodeCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, NULL}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

#endif

/**
Define and declare a custom node activity

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_CUSTOM_NODEACTIVITY
*/
#define DECLARE_DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
	DECLARE_NODEACTIVITY( name ) \
	DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor )

/**
Define and export node activity

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity

@see DEFINE_NODEACTIVITY
*/
#define DEFINE_EXPORT_NODEACTIVITY( id, name, msgtype ) \
	DEFINE_EXPORT_CUSTOM_NODEACTIVITY( id, name, msgtype, MeshMachine::CNodeActivityBase::NewL )

/**
Define the start of a node activity. There must be a matching @c NODEACTIVITY_END to close the
node activity.

The @c id is a unique identifier for the activity. It allows activities to be overridden in an
activity map. When deriving from an activity map, if you specify a node activity in your map
whose id is the same as one in the parent map, the one in the parent map with be overriden.

The @c name of the activity is used to add it to the activity map.

The @c msgtype is the type of message which will initiate this activity. This can be left as the
null message, in which case it will be the first state which decides whether to start the activity
or not.

@code
namespace ExampleActivity {
DEFINE_NODEACTIVITY(EFoobarActivity, SimpleFoobarActivity, TFoobar)
    FIRST_NODEACTIVITY_ENTRY(TAwaitingFoobar, TDoOneThingOrAnother)
	NODEACTIVITY_ENTRY(KDoOneThing, TDoTheThing, TAwaitingResponseToThing, TNoTag)
	NODEACTIVITY_ENTRY(KAnother, TDoTheOtherThing, TAwaitingResponseToThing, TNoTag)
	LAST_NODEACTIVITY_ENTRY(KNoTag, TDoFinalThing)
NODEACTIVITY_END()
} // end namespace ExampleActivity
@endcode

A node activity must start with either a @c FIRST_NODEACTIVITY_ENTRY or a @c SINGLE_NODEACTIVITY_ENTRY. In
the latter case there should be no other entries in the node activity.

The activity object (@c CNodeActivityBase) is only created once the second @c NODEACTIVITY_ENTRY is entered.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity

@see DEFINE_ACTIVITY_MAP
*/
#define DEFINE_NODEACTIVITY( id, name, msgtype ) \
	DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, MeshMachine::CNodeActivityBase::NewL )

/**
Define and declare a node activity

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity

@see DEFINE_ACTIVITY_MAP
*/
#define DECLARE_DEFINE_NODEACTIVITY( id, name, msgtype ) \
	DECLARE_DEFINE_CUSTOM_NODEACTIVITY( id, name, msgtype, MeshMachine::CNodeActivityBase::NewL )

/**
Define an entry in a node activity.

@param transitionTag     Condition on which the entry is entered
@param stateTransition   Transition to be executed
@param state             State to enter after
@param stateFork         Fork to decide which entry to enter next

@see DEFINE_NODEACTIVITY
*/
#define NODEACTIVITY_ENTRY( transitionTag, stateTransition, state, stateFork ) \
	STATE_TRIPLE_ENTRY( transitionTag, stateTransition, state, stateFork )

/**
Define the first entry in a node activity

@param firstState       Initial state of the activity
@param firstStateFork   Fork to decide which entry to enter next

@see DEFINE_NODEACTIVITY
*/
#define FIRST_NODEACTIVITY_ENTRY( firstState, firstStateFork ) \
	FIRST_TRIPLE_ENTRY( firstState, firstStateFork )

/**
Define a through entry. Through entries execute a transition but do not
enter a state. Instead they move onto the next entry in the node activity
using the @c stateFork to decide which entry this is.

@param transitionTag   Condition on which the entry is entered
@param stateTransition Transition to be executed
@param stateFork       Fork to decide which entry to enter next
*/
#define THROUGH_NODEACTIVITY_ENTRY( transitionTag, stateTransition, stateFork ) \
	THROUGH_TRIPLE_ENTRY( transitionTag, stateTransition, stateFork )

/**
Define the final entry in a node activity.

@param transitionTag  Condition on which the entry is entered
@param lastTransition Final transition to be executed

@see DEFINE_NODEACTIVITY
*/
#define LAST_NODEACTIVITY_ENTRY( transitionTag, lastTransition ) \
	LAST_TRIPLE_ENTRY( transitionTag, lastTransition )

/**
Define a single node activity entry. This is the first and last entry in any
node activity in which it is used.

@param state             Initial state of the activity
@param stateTransition   Transition to execute once the state is fulfilled

@see DEFINE_NODEACTIVITY
*/
#define SINGLE_NODEACTIVITY_ENTRY( stateTransition, state ) \
	STATE_TRIPLE_ENTRY( MeshMachine::KNoTag, stateTransition, state, MeshMachine::TNoTag )

/**
Define a routing node activity entry. Routing entries neither execute any
transition, nor wait in any state. These are useful only for redirecting the
path of execution in the state machine.

@param transitionTag  Condition on which the entry is entered
@param stateFork      Fork to decide the next transition to enter
*/
#define ROUTING_NODEACTIVITY_ENTRY( transitionTag, stateFork ) \
	THROUGH_TRIPLE_ENTRY( transitionTag, MeshMachine::TDoNothing, stateFork )

/**
Marks the end of a node activity.

@see DEFINE_NODEACTIVITY
*/
#define NODEACTIVITY_END() \
	TRIPLES_TABLE_END()



/**
Declare and export an activity map

@param map Name of the activity map
@see DECLARE_ACTIVITY_MAP
*/
#define DECLARE_EXPORT_ACTIVITY_MAP( map ) \
	struct map \
		{ \
		IMPORT_C static const MeshMachine::TNodeActivityMap& Self(); \
		static const MeshMachine::TNodeActivityMap iSelf; \
		static const MeshMachine::TNodeActivityMap::TStaticNodeActivity iData[]; \
		};

/**
Define and export the activity map

@param map Name of the activity map
@see DEFINE_ACTIVITY_MAP
*/
#define DEFINE_EXPORT_ACTIVITY_MAP( map ) \
	EXPORT_C const MeshMachine::TNodeActivityMap& map :: Self() {return iSelf;} \
	const MeshMachine::TNodeActivityMap map :: iSelf = {map :: iData[0]}; \
	const MeshMachine::TNodeActivityMap::TStaticNodeActivity map :: iData[] = {

/**
Declare an activity map.

For example,
@code
DECLARE_ACTIVITY_MAP(MyNodeActivityMap)
@endcode

This can then be passed into the constructor of a mesh machine node as follows.

@code
CMyMeshNode::CMyMeshNode(CConnectionProviderFactoryBase& aFactory)
    : CCoreConnectionProvider(aFactory, MyNodeActivityMap::Self())
	{
	...
@endcode

@param map Name of the activity map
*/
#define DECLARE_ACTIVITY_MAP( map ) \
	struct map \
		{ \
		inline static const MeshMachine::TNodeActivityMap& Self() {return iSelf;} \
		static const MeshMachine::TNodeActivityMap iSelf; \
		static const MeshMachine::TNodeActivityMap::TStaticNodeActivity iData[]; \
		}; \

/**
Define an activity map. An activity map is a collection of node activities. The activity map can be
based on another activity map or this macro can be used to define a standalone activity map.

Once defined an activity map can be passed into the constructor of a node, to define
what activities that node is capable of executing.

@code
DECLARE_DEFINE_ACTIVITY_MAP(MyNodeActivityMap)
	ACTIVITY_MAP_ENTRY(MyNodeActivities, MyFirstActivity)
	ACTIVITY_MAP_ENTRY(MyNodeActivities, MySecondActivity)
ACTIVITY_MAP_END_BASE(BaseActivities, BaseActivityMap)
@endcode

@param map Name of the activity map

@see ACTIVITY_MAP_END_BASE
@see ACTIVITY_MAP_END
*/
#define DEFINE_ACTIVITY_MAP( map ) \
	const MeshMachine::TNodeActivityMap map :: iSelf = {map :: iData[0]}; \
	const MeshMachine::TNodeActivityMap::TStaticNodeActivity map :: iData[] = {

/**
Define and declare an activity map.

@param map Name of the activity map

@see DEFINE_ACTIVITY_MAP
*/
#define DECLARE_DEFINE_ACTIVITY_MAP( map ) \
	DECLARE_ACTIVITY_MAP(map) \
	DEFINE_ACTIVITY_MAP(map)

/**
Create an entry in an activity map.

@param name_space Namespace of the activity
@param name       Name of the activity

@see DEFINE_ACTIVITY_MAP
*/
#define ACTIVITY_MAP_ENTRY( name_space, name ) &name_space::name::Self,

/**
Mark the end of an activity map. This macro should be used for standalone activity
maps.

@see DEFINE_ACTIVITY_MAP
*/
#define ACTIVITY_MAP_END() \
	NULL, \
	NULL \
	};

/**
RESERVED MACROS NOT TO BE USED.
The following are new macro definitions allowing the context to be passed as a parameter rather than the node.
In time the ability to pass context rather than node will be placed in the original macros. For the meantime,
only critical commsFw activities which must have the context passed instead of the node should use these macros.
All other activities should continue to use original macros declared above.
*/
/**
Declare and export a node activity.

@param name  The name of the activity, which will be used to put it into the node's activity map

@see DECLARE_NODEACTIVITY
*/
#define DECLARE_RESERVED_EXPORT_NODEACTIVITY( name ) \
	struct name \
		{ \
		IMPORT_C static const MeshMachine::TNodeActivity& Self(); \
		IMPORT_C static const NetStateMachine::TStateTriple& FirstTriple(); \
		static const MeshMachine::TNodeActivity iSelf; \
		static const NetStateMachine::TStateTriple iData[]; \
		};

/**
Declares a node activity. A node activity is a state machine. It contains multiple nodeactivity
entries, which respresent the states, transitions and forks of the state machine.

@param name  The name of the activity, which will be used to put it into the node's activity map

@see DEFINE_NODEACTIVITY
@see NODEACTIVITY_ENTRY
*/
#define DECLARE_RESERVED_NODEACTIVITY( name ) \
	struct name \
		{ \
		inline static const MeshMachine::TNodeActivity& Self() {return iSelf;} \
		inline static const NetStateMachine::TStateTriple& FirstTriple() {return iData[1];} \
		static const MeshMachine::TNodeActivity iSelf; \
		static const NetStateMachine::TStateTriple iData[]; \
		};

#ifdef SYMBIAN_TRACE_ENABLE

/**
Define a custom node activity. A custom node activity is a node activity which defines its own
node activity class. This is used, for example, if the activity wishes to share some custom context
information between the states and transition in the activity.

Node activity class must derive from CNodeActivityBase.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_RESERVED_NODEACTIVITY
@see CNodeActivityBase
	
*/
	#define DEFINE_RESERVED_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::EContextCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, _S8(#name)}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

#else

/**
Define and export a custom node activity.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_RESERVED_CUSTOM_NODEACTIVITY
*/
	#define DEFINE_RESERVED_EXPORT_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		EXPORT_C const MeshMachine::TNodeActivity& name :: Self() {return iSelf;} \
		EXPORT_C const NetStateMachine::TStateTriple& name :: FirstTriple() {return iData[1];} \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::EContextCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, NULL}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

/**
Define a custom node activity. A custom node activity is a node activity which defines its own
node activity class. This is used, for example, if the activity wishes to share some custom context
information between the states and transition in the activity.

Node activity class must derive from CNodeActivityBase.

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_RESERVED_NODEACTIVITY
@see CNodeActivityBase
*/
	#define DEFINE_RESERVED_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
		const MeshMachine::TNodeActivity name :: iSelf = {id, MeshMachine::TNodeActivity::EContextCtor, msgtype::EId, msgtype::ERealm, name :: iData[1], (TAny*)&ctor, NULL}; \
		DEFINE_TRIPLES_TABLE( name :: iData )

#endif

/**
Define and declare a custom node activity

@param id        Identifier for the activity
@param name      The name of the activity
@param msgtype   Message which will kickoff this activity
@param ctor      Constructor for the custom activity class

@see DEFINE_RESERVED_CUSTOM_NODEACTIVITY
*/
#define DECLARE_DEFINE_RESERVED_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor ) \
	DECLARE_RESERVED_NODEACTIVITY( name ) \
	DEFINE_RESERVED_CUSTOM_NODEACTIVITY( id, name, msgtype, ctor )

/**
END OF RESERVED MACROS
*/

/**
Mark the end of an activity map. This macro should be used for an activity map based
on other activity maps.

@param name_space The namespace of the base map
@param base_map   The name of the base map

@see DEFINE_ACTIVITY_MAP
*/
#define ACTIVITY_MAP_END_BASE( name_space, base_map ) \
	NULL, \
	(MeshMachine::TNodeActivityMap::TStaticNodeActivity)&name_space::base_map::Self \
	};

namespace MeshMachine
{

/**
Base class for all node activity objects. Non custom node activities will use this by default.
**/
class CNodeActivityBase : public CBase,
                          public NetInterfaces::AApiExtBase,
                          protected NetStateMachine::ACore,
                          protected NetInterfaces::TInterfaceControl
	{
    friend class AMMNodeBase;
    friend class AContextStore; //to be able access NetStateMachine::ACore
    friend class AActivitySemaphore; //to be able access NetStateMachine::ACore

public:
	/**
	   Create a new activity. This should never be called by user code.

	   @param aActivitySig Context information about how the activity is to be started
	   @param aNode        The node to which this activity will belong.
	   @return A pointer to the new Activity base object. Ownership is transferred.
	*/
    IMPORT_C static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );

	/**
	Get the @c id of the activity.
	   @return The activity id of the activity
	*/
	IMPORT_C virtual TUint16 ActivityId() const;

	/**
	   Remove an originator from the originator list.

	   @param aIndex Index of originator to remove
	*/
	IMPORT_C void RemoveOriginator(TInt aIndex);

	/**
	Check if the activity is running.
	   @return ETrue if the activity is still running, otherwise EFalse.
	*/
	IMPORT_C virtual TBool IsIdle() const;

	/**
	   Put the activity into the idle state. This stops the activity running and notifies the mesh machine that
	   it's ok to delete this activity.
	*/
	IMPORT_C virtual void SetIdle();

	/**
	   Return the activity signature id of the activity. This may not necessarily be the same as the activity id
	   in the case of parallel activities, where the activity id will be [8 bit unique id|8 bit activity sig]

	   @return The activity signature id
	*/
    TUint ActivitySigId() const
   		{
  		return iActivitySig.iId;
   		}

	/**
	Get the current error state for the activity.
	   @return The activity's current error state. @c KErrNone indicates there is no error.
	*/
	TInt Error() const
		{
		return iError;
		}

	/**
	   Set the error state of the activity.

	   @param aError error code to set
	*/
	void SetError(TInt aError)
		{
		if (iError == KErrNone || aError == KErrNone)
    		{
    		iError = aError;
    		};
		}

	/**
	Get the id of the node that the last request from this activity was sent to.
	   @return Node id of the last node the activity has posted a request to
	*/
	IMPORT_C const Messages::TNodeId& PostedToNodeId() const;
	
    /**
    Get the id of the node that the last request from this activity was sent to.
       @return Node id of the last node the activity has posted a request to
    */
	IMPORT_C const Messages::RNodeInterface* PostedToPeer() const;

	/**
	Get the id of the message that started this activity.
	   @return Message id of message that kicked off this activity
	*/
	const Messages::TNodeSignal::TMessageId KickOffMessageId() const
		{
		return Messages::TNodeSignal::TMessageId(iActivitySig.iKickOffMessageId, iActivitySig.iKickOffMessageRealm);
		}

	/**
	Get the id of the node that started this activity.
	   @return Node peer id of the node whose message first kicked off this activity
	*/
	const Messages::TNodePeerId& FirstOriginator() const
		{
		__ASSERT_DEBUG(iOriginators.Count(), User::Panic(KSpecAssert_ElemMeshMachActH, 1));
		return iOriginators[0];
		}

	/**
	In essence the same as FirstOriginator(). This should be used in preference in situations where the activity should
	be run in parallel, and therefore only ever have one originator.
	   @return Node peer id of the node whose message first kicked off this activity
	*/
	const Messages::TNodePeerId& SoleOriginator() const
		{
		__ASSERT_DEBUG(iOriginators.Count() == 1, User::Panic(KSpecAssert_ElemMeshMachActH, 2));
		return iOriginators[0];
		}

	/**
	   Post a request to a node.

	   @param aRecepient Endpoint for the message
	   @param aMessage   Message to send
	   @param aRecipientIdCritical If true, the postedTo id is set to the id of the recipient. If false, the postedTo id is set to null.
	   @see PostedToId
	*/
	IMPORT_C void PostRequestTo(const Messages::RNodeInterface& aRecepient, const Messages::TSignalBase& aMessage, const TBool aRecipientIdCritical = ETrue);
	/**
	   Post a request to a node.

	   @param aRecepient Endpoint for the message
	   @param aMessage   Message to send
	   @param aRecipientIdCritical If true, the postedTo id is set to the id of the recipient. If false, the postedTo id is set to null.
	   @see PostedToId
	*/
	IMPORT_C void PostRequestTo(const Messages::TNodeId& aRecepient, const Messages::TSignalBase& aMessage, const TBool aRecipientIdCritical = ETrue);

	/**
	   Manually set the postedTo id

	   @param aNodeId Node id to set the postedTo id to
	*/
	IMPORT_C void SetPostedTo(const Messages::TNodeId& aNodeId);
	
    /**
       Manually set the postedTo id

       @param aNodeId Node id to set the postedTo id to
    */
	IMPORT_C void SetPostedTo(const Messages::RNodeInterface& aRecipient);

	/**
	   Clear the postedTo id
	*/
	IMPORT_C void ClearPostedTo();

#ifdef SYMBIAN_TRACE_ENABLE
	/**
	   @return the human readable name of the activity
	*/
    const TText8* ActivityName() const
        {
        return (iActivitySig.iName)? iActivitySig.iName : _S8("Undefined");
        }

	/**
	   @return the human readable description of the current node activity entry
	*/
    const TText8* CurrentTripleName() const
        {
        return ACore::CurrentTripleName();
        }
#endif

public:
	/**
	Cancels the activity. Only to be called by the meshmachine.
	@param aContext the context in which the activity is being executed
	@internalAll
	*/
	IMPORT_C virtual void Cancel(TNodeContextBase& aContext);

	/**
	Attempt to advance the activity one entry. Only to be called by the mesh machine
	@param aContext the context in which the activity is being executed
	@return Whether the activity advanced
	@internalAll
	*/
	IMPORT_C virtual TBool Next(TNodeContextBase& aContext);

	/**
	Find the originator matching the parameters
	@param aPeerToFind Matching criteria
	@return Index of the matching originator. KErrNotFound if not found.
	@internalAll
	*/
	IMPORT_C TInt FindOriginator(const Messages::RNodeInterface& aPeerToFind) const;

	/**
   	Find the originator matching the parameters
	@param aPeerToFind Matching criteria
	@return Index of the matching originator. KErrNotFound if not found.
	@internalAll
	*/
	IMPORT_C TInt FindOriginator(const Messages::TRuntimeCtxId& aPeerToFind) const;

	/**
   	Find the originator matching the parameters
	@param aOriginator xx
	@return Index of the matching originator. KErrNotFound if not found.

	@internalAll
	*/
	IMPORT_C TInt FindOriginator(const Messages::TNodePeerId& aOriginator) const;

	/**
	Post a message to an originator
	@param aOriginator originator to post the message to
	@param aMessage    message to post
	@return Whether the posting succeeded
	@internalAll
	*/
	IMPORT_C TBool PostToOriginator(const Messages::TNodePeerId& aOriginator, const Messages::TSignalBase& aMessage) const;

	/**
    Post a message to all originators
	@param aMessageSig the message to post
	@param aFlagsToSet Flags to set on the peer as the message is being sent
	@param aFlagsToClear Flags to cleared on the peer as the message is being sent
	@return number of originators the message was posted to
	@internalAll
	*/
	IMPORT_C TInt PostToOriginators(const Messages::TSignalBase& aMessageSig, TUint32 aFlagsToSet = 0, TUint32 aFlagsToClear = 0);

	/**
	Calls cancel on current state. Sends TError to all originators and sets activity idle
	@param aContext the context in which the activity is being executed
	@param aIsNodeBeingDestroyed  indicate to the activity as to whether the node is being destroyed
	@internalAll
	*/
	IMPORT_C void Abort(TNodeContextBase& aContext, TBool aIsNodeBeingDestroyed = EFalse);

	
protected:
	/**
	Constructor for CNodeActivityBase
	@param aActivitySig Signature with which to create the activity
	@param aNode        Node which will own the activity
	*/
	IMPORT_C explicit CNodeActivityBase(const TNodeActivity& aActivitySig, AMMNodeBase& aNode);
	/**
	Destructor. If the Activity is running in error mode, a message is sent to the originator.
	*/
	IMPORT_C virtual ~CNodeActivityBase();

	/**
	Destroy the activity.

	Used primarily by preallocated activities to return extracted space
	*/
    virtual void Destroy()
    	{
    	delete this;
    	};

	/**

	@param aInterfaceId id of requested interface
	@return Interface control for accessing the extension interfaces of the activity
	@see TInterfaceControl
	*/
    IMPORT_C virtual NetInterfaces::TInterfaceControl* DoFetchInterfaceControlL(TInt aInterfaceId);

	/**
	Signal the activity that an event has happened in the mesh machine. Used for waking up sleeping activities
	which use mutexes.

	@param # The context in which the activity is being executed
	@return ETrue if the activity was awoken
	@see AActivitySemaphore
	*/
	virtual TBool Signal(TNodeContextBase& /*aContext*/) { return EFalse; };

protected:
	/**
	Test whether a activity can start given the current context. If it can, the first transition will be executed.
	Note that this is a static method, and as such the activity hasn't been instanciated when this is called

	@param aContext the context in which the activity may be executed
	@param aActivitySig the activity which we are testing for the accept condition
	@param aTransitionTag the condition which must be matched by the first entry for the activity to be accepted. Set to KExecuteAlways to skip this check

	@return the first entry triple for the activity if it's accepted. NULL otherwise
	*/
	static const NetStateMachine::TStateTriple* Accept(TNodeContextBase& aContext, const TNodeActivity& aActivitySig, TInt aTransitionTag);

	/**
	Start a new activity. The first transition should have been run within the @c Accept() method before this is called.

	@param aContext  The context to start the activity in
	@param aOriginator  The peer that requires this activity to start
	@param aFirst       First entry triple in the activity, as returned by @c Accept()
	*/
	IMPORT_C virtual void StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);

protected:
	/**
	Append self to the node's list of running activities.

	Should only be accessed from @c Activity::NewL. Generally activities can choose to be inserted or appended
	(if they do not choose they are appended by default). When inserting, extreme caution must be taken, as
	this behavior is reserved for destroying activities.
	*/
	IMPORT_C void AppendActivityL();

	/**
	Append self to the node's list of running activities. This should only be called if you are certain that
	the activity list has space for the append.
	*/
	IMPORT_C void AppendPreallocatedActivity();

	/**
	Insert an activity at the start of the node's list of running activities.
	*/
	IMPORT_C void InsertPreallocatedDestroyActivity();

	/**
	Borrow some preallocated memory from the node. This preallocated memory is used for activities that
	absolutely cannot fail in any scenario, such as for Destroying the node. For this reason, when the
	node is created, some memory is allocated so that a destroying activity can't even fail in an out
	of memory situation.

	This method is static as the memory must be borrowed before the activity object is constructed.

	The memory is returned using @c ReturnPreallocatedSpace().

	@param aNode The node that owns the preallocated space. This must be the node that the activity runs on.
	@param aSize Size of buffer to allocate.

	@return A pointer to the allocated block of memory
	*/
	IMPORT_C static TAny* BorrowPreallocatedSpace(AMMNodeBase& aNode, TUint aSize);

	/**
	Return preallocated space to the node. The node does not have to be passed in as a parameter as it will
	already be a member of the activity.

	@param aSpace Memory buffer to return.
	*/
	IMPORT_C void ReturnPreallocatedSpace(TAny* aSpace);
	
	/**
	Test whether aContext carries a message that is to be expected by 'this' in its current state. 
	The method is effectivelly a filter that hides the messages flowing throught the node, but not
	intended for 'this'. The method will check:
	- if aContext carries a message from PostedToId (if set) then the message should be presented to 'this'.
	- if aContext carries a message from one of the originators then the message should be presented to 'this'.
	@return ETrue if the sender of the current message matches PostedToId if it is set.
	*/
	IMPORT_C TBool MatchSender(const TNodeContextBase& aContext) const;	

protected:
	/**
	The node the activity is running on
	*/
	AMMNodeBase& iNode;

	/**
	All the nodes that have started the same activity
	*/
	RArray<Messages::XNodePeerId> iOriginators;
/*	class TOriginatorStore
		{
	private:
		typedef Messages::XNodePeerId TOriginatorType;
	public:
		class Iterator
			{
		public:
			Iterator(const TOriginatorStore& aOriginatorStore);
			TOriginatorType* operator++();
			operator*();
			};
		virtual TInt Count() const {return iOriginators.Count();}
		virtual const TOriginatorType& operator[](TUint aIndex) const {return iOriginators[aIndex];};
		virtual TInt Append(const Messages::RNodeInterface& aInterface);
		virtual TInt Remove(const Messages::RNodeInterface& aInterface);
	private:
		RArray<TOriginatorType> iOriginators;
		} iOriginators;
*/
private: //Shouldn't be accessed directly
	TInt     iError;    //Risk of failure is a generic property of an activity.
	                    //Activities may choose to use external error handling activities,
	                    //or may choose to handle errors locally. In the latter case,
	                    //the activity may want to know it's running an error mode
	                    //(by setting this member).
	                    //iError must be cleared before the activity goes out of scope
	                    //to indicate the error has been handled.

	const TNodeActivity& iActivitySig;

	class RPostedToNodeOrPeer
	/*
	 * Class is used to store the recipient of the last request sent from the 'this' 
	 * (the activity). This recipient can be represented either by its TNodeId address 
	 * or, if it's a peer of the local node, by RNodeInterface. 
     * The implementation may seem awkward or overengineered. This is to protect binary
     * compatibility (it used to be TNodeId here and sizeof TNodeId was all there was at hand).
     * 
     * Rules of the game are that RPostedToNodeOrPeer:iBuf is a shared dwelling for either:
     * - address of an RNodeInterface (in case posted to is a peer)
     * - TNodeId (in case posted to is not a peer)
     * Inspecting the content of iBuf and the assumptions around it are a little fragile 
     * (based on TNodeId class layout a bit), so the code must be viligent. 
     * _Node and _Peer perform arbitral happy conversion, so there can be 3 reasons why _Node()->Ptr() is NULL:
     * (1) iBuf stores a 4 byte ptr to RNodeInterface and what Ptr normally returns is beyond these 4 bytes in a land hopefully zeroed by ::Close
     * (2) iBuf stores nothing. 
     * (3) iBuf stores a TNodeId pointing a NULL node (effectivelly TNodeId::NullId()). (3) is only theoretical as Open(TNodeId) prevens Ptr be NULL
     * Therefore it is safe to assume that if Ptr() is NULL, it is either a valid pointer to a peer or NULL.
     * Happily _Peer() will return that pointer or NULL respectivelly.* 
	 */
	    {
	private:
	    typedef Messages::RNodeInterface* TPeerType;
    public:
	    RPostedToNodeOrPeer();
	    void Open(const Messages::RNodeInterface& aPeer);
	    void Open(const Messages::TNodeId& aNode);
	    void Close();
	    
	    const Messages::RNodeInterface* Peer() const;
	    const Messages::TNodeId& NodeId() const;
	    
	private:
	    TPeerType* _Peer() 
	        {
            return reinterpret_cast<TPeerType*>(&iBuf[0]);
	        }
        
	    const TPeerType* _Peer() const 
            {
            return reinterpret_cast<const TPeerType*>(&iBuf[0]);
            }	    
	    
	    Messages::TNodeId* _Node()
	        {
	        return reinterpret_cast<Messages::TNodeId*>(&iBuf[0]);
	        }
	    
        const Messages::TNodeId* _Node() const
            {
            return reinterpret_cast<const Messages::TNodeId*>(&iBuf[0]);
            }
        TUint8 iBuf[__Align8(sizeof(Messages::TNodeId))];	    
	    } iPostedToId;
	};


/**
Activity type that generates a unique activity id every time it is instantiated.
The id is generated from the @c <TNodeActivityId::EActivityParallelRangeMin,TCFNodeActivityId::EActivityParallelRangeMax>
range. The id should never be found in a static activity map.
A new instance id is created rather than adding a new originator to the existing activity.
*/
class CNodeParallelActivityBase : public CNodeActivityBase

	{
public:
	/**
    Create a new activity. This should never be called by user code.

	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return A pointer to the new activity. Ownership is transfered.
	*/
    IMPORT_C static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );

	/**
	Get the activity id
	@return The activity id of the activity
	*/
   	IMPORT_C virtual TUint16 ActivityId() const;

protected:
	/**
	For use by custom activity NewLs to generate the unique part of the activity id.
	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return Generated unique component of activity id
	*/
	IMPORT_C static TUint GetNextActivityCountL( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode );
	/**
	For use by custom activity News to generate the unique part of the activity id.
	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return Generated unique component of activity id
	*/
	IMPORT_C static TUint GetNextActivityCount( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode );

	/**
	Constructor for CNodeParallelActivityBase
	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@param aNextActivityCount The unique part of the activity id
	*/
    IMPORT_C CNodeParallelActivityBase( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aNextActivityCount );

private:
	/**
	For use by custom activity NewLs to generate the unique part of the activity id.
	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return Generated unique component of activity id
	*/
	static TUint DoGetNextActivityCountL( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode );

protected:
	/**
	The activity id. The id format is [8 bit unique id|8 bit activity sig].
	*/
    TUint16 iActivityId;
	};

/**
   Base class for parallel activities which store their kickoff message.

   @see CNodeParallelActivityBase
*/
class CNodeParallelMessageStoreActivityBase : public CNodeParallelActivityBase
	{
public:
	IMPORT_C static CNodeActivityBase* NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
	IMPORT_C ~CNodeParallelMessageStoreActivityBase();

	/**
	   @return Kick off message for this activity
	*/
	IMPORT_C Messages::TSignalBase& Message();

protected:
	IMPORT_C virtual void StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);
	void SaveMessageL(Messages::TSignalBase& aMessage);
	
	IMPORT_C CNodeParallelMessageStoreActivityBase( const MeshMachine::TNodeActivity& aActivitySig,
		MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount );

private:
	Messages::TSignalBase* iMsg;
	TBuf8<__Align8(Messages::TSignalBase::KMaxInlineMessageSize + Messages::TSignalBase::KMaxUnstoredOverhead)> iMsgBuf;
	};

/**

If an activity has a transition or fork that is blocked by a mutex, the context must be stored so
that when the transition or fork is unblocked it would be able to continue with the context that
it had before it was blocked. If this aggregate wasn't used in the previous scenario, the context
of the event that triggered the unblocking would be used, and this may have the wrong message, or
event originator set.

Error recovery is another scenario this class is useful in.
*/
class AContextStore
    {
public:
    /**
    Interface id
    */
    static const TInt KInterfaceId = 0x102864DC;

protected:
    /**
    Constructor for AContextStore
    */
    explicit AContextStore()
        {
        }
    /**
    Destructor for AContextStore
    */
    ~AContextStore()
        {
    	iContextDesc.Close();
        }

public:
	/**
	Used for error recovery. If the context is stored, loads the stored context and reruns the last transition that was run.

	@param aActivity The activity to retry
	@param aContext  The context within which the retrying is taking place. NOTE: This is not the stored context

	@see StoreContext
	@see LoadContext
	*/
	IMPORT_C void Retry(CNodeActivityBase& aActivity, TNodeContextBase& aContext);

    /**
	Store context in buffer. Designed to be called from a transition to store the transitions context for error recovery or
	to wait on a mutex.

	@param aContext The context to store
	@return Error code. @c KErrNone if storage is successful.
	*/
	IMPORT_C TInt StoreContext(const TNodeContextBase& aContext);

	/**
	Check of the context is stored.
	@return @c ETrue if a context is stored. @c EFalse otherwise.
	*/
	IMPORT_C TBool IsStored() const;

	/**
	Get the id of the message which requested @c Retry()
	@return Message Id of message which kicked off the @c ::Retry

	@see AContextStore::Retry
	*/
 	IMPORT_C const Messages::TNodeSignal::TMessageId& RetryingForMessageId() const; //Needs to be moved to error-recoverable activity class

protected:
	/**
	Load a stored context. Once the stored context has been loaded the status of @c iContextDesc is undefined and load shouldn't
	be called again until something has been stored.

	The pointer returned uses memory allocated in the @c aCtxBuff that is passed in, and as such, does not need to be freed. However @c aCtxBuff and @c aMsgBuff
	must be cleaned up after use.

	@param aNode The node for the context
	@param aNodeActivity The activity for the context.
	@param aCtxBuff  A memory buffer in which the context will be created.
	@param aMsgBuff  A memory buffer in which the message will be created.
	@param aDummy    Will become the recipient node id in the context.

	@return Pointer to loaded context
	*/
	IMPORT_C TNodeContextBase* LoadContext(AMMNodeBase& aNode, CNodeActivityBase* aNodeActivity, TDes8& aCtxBuff, TDes8& aMsgBuff, const Messages::TNodeId& aDummy);

protected:
	/**
    Buffer to store the context.
	*/
	RBuf8 iContextDesc;
	/**
	Message id of the message which kicked off the retry attempt
	*/
	Messages::TNodeSignal::TMessageId iRetryingForMessageId;
	};

/**
Aggregate class for activities which wish to use mutexes.

After a serialised state or transition has been unblocked, signalled and invoked, AActivitySemaphore
clears all of the synchronisation related information (e.g stored context), assuming that the job has
been done. Sometimes however the serialised state, upon more careful inspection of the environment
(context), may decide that it still does not want to be executed and it wishes to sleep again.

In such case it would call Wait() and return @c EIgnore from its @c TransitionTag(). The same applies to a
serialised transition. It may want to call Wait() instead of, for example sending a request message.

After calling Wait() the serialised state will be woken up when the policy matches, which allows the serialised state
to finish its job.
 */
class AActivitySemaphore : public AContextStore
    {
public:
    /**
    Interface id
    */
    static const TInt KInterfaceId = 10015; //Normally a UID - 10015 for compatibility reasons

public:
	/**
	Park a state.

	@param aContext The context which the state is to run in. This will be stored

	@return @c KErrNone if successful. System wide error code otherwise
	*/
	IMPORT_C static TInt ParkState(const TNodeContextBase& aContext);

	/**
	Unpark a state.

	@param aContext The context which the state is to run in.

	@return @c KErrNone if successful. System wide error code otherwise
	*/
	IMPORT_C static TInt UnparkState(const TNodeContextBase& aContext);

	/**
	Park a transition

	@param aContext The context which the transition is to run in.
	*/
	IMPORT_C static void ParkTransitionL(const TNodeContextBase& aContext);

	/**
	Signals to the activity that an event has occurred. Used for waking up sleeping activities
	which use mutexes. The activity is expected to check if the mutex has now cleared.

	@param aContext Current context in the state machine.

	@return @c ETrue if the state or transition has been unparked
	*/
	IMPORT_C TBool Signal(TNodeContextBase& aContext);

	/**
	Tell the activity to wait. The activity will wait until Signal() is called.
	*/
	IMPORT_C void Wait();

	/**
	Check if the activity is waiting
	@return ETrue if the activity is currently waiting
	*/
	TBool IsWaiting() const
		{
		__ASSERT_DEBUG(!(iFlags&KIsWaiting) || IsStored(), User::Panic(KSpecAssert_ElemMeshMachActH, 3));

		return iFlags&KIsWaiting;
		}

protected:
    /**
    Constructor for AActivitySemaphore
    */
    explicit AActivitySemaphore()
    :   iFlags(0)
		{
        }
    /**
    Destructor for AActivitySemaphore
    */
    ~AActivitySemaphore()
        {
        }

protected:
	/**
	Set the @c isWaiting flag
	*/
	inline void SetIsWaiting() { iFlags|=KIsWaiting; };
	/**
	Clear the @c isWaiting flag
	*/
	inline void ClearIsWaiting() { iFlags&=~KIsWaiting; };

	/**
	Check if the WillWait flag is set. The WillWait flag indicates whether the activity will be in a waiting state at the end of the signal function.
	This is distinct from "is waiting" which indicates whether the activity was waiting on entry to the Signal()
	function.

	@return ETrue if the WillWait flag is set
	*/
	inline TBool WillWait() const { return (iFlags&KWillWait)==KWillWait; };
	/**
	Set the willWait flag
	*/
	inline void SetWillWait() { iFlags|=KWillWait; };
	/**
	Clear the willWait flag
	*/
	inline void ClearWillWait() { iFlags&=~KWillWait; };

private:
	//Is waiting for mutex? == not idle yet, even if last triple
 	//iIsWaiting prevents Idle condition following the last
	//transition (in case the transition has parked the activity
	//and wants to be reexecuted). When reexecuting, the activity
	//mustn't be idle (iIsWaiting has to be true), on the
	//other hand the transition may to park the activity again
	//We need to distinguish the entry value of iIsWaiting from
	//the exit value of iIsWaiting, we hance need this aditional
	//flag.

	inline TBool IsTransition() const { return (iFlags&KIsTransition)==KIsTransition; };
	inline void SetIsTransition() { iFlags|=KIsTransition; };
	inline void ClearIsTransition() { iFlags&=~KIsTransition; };

	enum { KIsWaiting = 0x1, KWillWait = 0x2, KIsTransition = 0x4 };

	TUint8 iFlags;
    };

/**
Activity class base for activities which wish to use synchronisation or retries.
*/
class CNodeRetryActivity : public CNodeActivityBase, public AActivitySemaphore,
                public ITFHIERARCHY_2(CNodeRetryActivity, AContextStore, AActivitySemaphore)
	{
public:
	/**
	Defines a helper list of interfaces.
 	*/
	typedef ITFHIERARCHY_2(CNodeRetryActivity, AContextStore, AActivitySemaphore) TIfStaticFetcherNearestInHierarchy;

public:
	/**
    Create a new activity. This should never be called by user code.

	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return A pointer to the new object. Ownership is transfered.
	*/
    IMPORT_C static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );

	/**
	Fetch the context store interface
	@param aInterface pointer into which a pointer to the interface will be placed.
	*/
    IMPORT_C void ReturnInterfacePtrL(AContextStore*& aInterface);

	/**
	Fetch the activity semaphore interface
	@param aInterface On return, contains a pointer to the interface.
	*/
    IMPORT_C void ReturnInterfacePtrL(AActivitySemaphore*& aInterface);

	/**
	Check if the activity has stopped running
	@return @c ETrue if the activity is still running, otherwise @c EFalse.
	*/
    IMPORT_C virtual TBool IsIdle() const;

	/**
	Put the activity into the idle state. This stops the activity running and notifies the mesh machine that
	it is ok to delete this activity.
	*/
    IMPORT_C virtual void SetIdle();

protected:
	/**
	Signal the activity that an event has occurred and that it is worthwhile checking
	to see if the mutex has now cleared.

	@param aContext Current context in the state machine.

	@return @c ETrue if state or transition has been unparked
	*/
	IMPORT_C virtual TBool Signal(TNodeContextBase& aContext);

    /**
    Constructor for CNodeRetryActivity
	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
    */
    IMPORT_C CNodeRetryActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );
	};

/**
Base class for activities that run in parallel and can be synchronised or retried.
*/
class CNodeRetryParallelActivity : public CNodeParallelActivityBase, public AActivitySemaphore,
                public ITFHIERARCHY_2(CNodeRetryParallelActivity, AContextStore, AActivitySemaphore)
	{
public:
	/**
	Defines a helper list of interfaces.
 	*/
 	typedef ITFHIERARCHY_2(CNodeRetryParallelActivity, AContextStore, AActivitySemaphore) TIfStaticFetcherNearestInHierarchy;

public:
	/**
    Create a new activity. This should never be called by user code.

	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@return A pointer to the new object. Ownership is transfered.
	*/
    IMPORT_C static CNodeActivityBase* NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode );

	/**
	Fetch the context store interface
	@param aInterface On return, contains a pointer to the store interface
	*/
    IMPORT_C void ReturnInterfacePtrL(AContextStore*& aInterface);

	/**
	Fetch the activity semaphore interface
	@param aInterface On return, contains a pointer to the semaphore interface
	*/
    IMPORT_C void ReturnInterfacePtrL(AActivitySemaphore*& aInterface);

	/**
	Check if the activity has stopped
	@return @c ETrue if the activity is still running, otherwise @c EFalse
	*/
    IMPORT_C virtual TBool IsIdle() const;

	/**
	Put the activity into the idle state. This stops the activity running and notifies the mesh machine that
	it is ok to delete this activity.
	*/
    IMPORT_C virtual void SetIdle();

protected:
	/**
	Signal to the activity that an event has occurred and that it is worthwhile checking
	to see if the mutex has now cleared.

	@param aContext Current context in the state machine.

	@return @c ETrue if state or transition has been unparked
	*/
	IMPORT_C virtual TBool Signal(TNodeContextBase& aContext);

	/**
    Constructor for CNodeRetryParallelActivity

	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	@param aActivitiesCount The unique part of the activity id
    */
    IMPORT_C CNodeRetryParallelActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount );
	};

/**
Aggregate originator array template for activities that use preallocated space.
This object has a fixed size array to hold the originators which will be allocated from the preallocated space
when the node activity object is created.
*/
template <TInt ORIGINATORSCOUNT>
class APreallocatedOriginators
	{
protected:
	/**
	Initialise the array and ensure it is empty as it should be on creation.
	Generally, iOriginators from the activity will be passed into this as a reference. A new
	RArray will then be created using the preallocated space.

	@param aOriginators Array of originators for which to preallocate space
	*/
	explicit APreallocatedOriginators(RArray<Messages::XNodePeerId>& aOriginators)
	:	iOriginatorsRef(aOriginators)
		{
		iOriginatorsRef = RArray<Messages::XNodePeerId>(sizeof(Messages::XNodePeerId), (Messages::XNodePeerId*)iOriginatorBuf, ORIGINATORSCOUNT);
		for (TInt i = iOriginatorsRef.Count() - 1; i >= 0; --i)
			{
			iOriginatorsRef.Remove(i);
			}
		}
    /**
    Destructor for APreallocatedOriginators.
    */
	~APreallocatedOriginators()
		{
		iOriginatorsRef = RArray<Messages::XNodePeerId>();
		}

private:
	//The originator's list
	RArray<Messages::XNodePeerId>& iOriginatorsRef;
    //Preallocated space in the originator's list
    TUint8 iOriginatorBuf[__Align8(sizeof(Messages::XNodePeerId) * ORIGINATORSCOUNT)];
	};

/**
Base for preallocated node activity class
*/
template <TInt ORIGINATORSCOUNT>
class CPreallocatedNodeActivityBase : public MeshMachine::CNodeActivityBase,
                                      protected MeshMachine::APreallocatedOriginators<ORIGINATORSCOUNT>
/**
@internalTechnology
*/
	{
public:
	/**
    Create a new activity. This should never be called by user code.

	@param aActivitySig Context information about how the activity is to be started
	@param aNode        The node to which this activity will belong.
	*/
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    	{
   		TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CPreallocatedNodeActivityBase));
		CPreallocatedNodeActivityBase* self = new (space) CPreallocatedNodeActivityBase(aActivitySig, aNode);
		self->AppendPreallocatedActivity();
		return self;
    	}

protected:


	CPreallocatedNodeActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeActivityBase(aActivitySig, aNode),
		APreallocatedOriginators<ORIGINATORSCOUNT>(iOriginators)
		{
		}

	/**
	Destroy the activity. Return the preallocated space and call the destructor directly. i.e. not though delete
	as delete would try and release the memory again
	*/
	virtual void Destroy()
		{
		ReturnPreallocatedSpace(this);
		this->~CPreallocatedNodeActivityBase(); //Run the destructor
		}
private:
	/**
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead
	*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

//-=========================================================
//
// CPreallocatedNodeActivityBase
//
//-=========================================================
// APreallocatedOriginators is primed with 2 as the vast majority of Activities will have a maximum of 2 originators.
// Corner cases should be handled seperately. For example CDestroyActivity has its own implementation as it will only ever have 1 originator
class CPreallocatedNodeRetryActivity : public MeshMachine::CNodeRetryActivity,
                                      protected MeshMachine::APreallocatedOriginators<2>
/**
@internalTechnology
*/
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    	{
   		TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CPreallocatedNodeRetryActivity));
		CPreallocatedNodeRetryActivity* self = new (space) CPreallocatedNodeRetryActivity(aActivitySig, aNode);
		self->AppendPreallocatedActivity();
		return self;
    	}

protected:
	CPreallocatedNodeRetryActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	CNodeRetryActivity(aActivitySig, aNode),
		APreallocatedOriginators<2>(iOriginators)
		{
		}

	virtual void Destroy()
		{
		ReturnPreallocatedSpace(this);
		this->~CPreallocatedNodeRetryActivity(); //Run the destructor
		}
private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

//By default we reserve the space for preallocated activities generously, to fit even a synchronised activity preallocating space for up to 3 originators.
//Any node, hosting specific activities that may need the preallocation mechanism can choose a more optimal amounts.
//For efficiency reasons it is strongly recommended that any node (or family of nodes) is revisited and an optimal
//amount of space is specified (could be specified in the base class for those nodes or in every type of node separatelly).
static const TUint KDefaultMaxPreallocatedActivityCount = 3;
static const TUint KDefaultMaxPreallocatedActivitySize = sizeof(CNodeRetryParallelActivity) + sizeof(APreallocatedOriginators<3>);
static const TUint KDefaultPreallocatedActivityBufferSize = KDefaultMaxPreallocatedActivityCount * KDefaultMaxPreallocatedActivitySize;

} //namespace MeshMachine

#endif //SYMBIAN_MM_ACTIVITIES_H

