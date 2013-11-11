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
//

/**
 @file
*/

#include "mm_node.h"
#include "mm_nodepeer.h"
#include "mm_activities.h"
#include <elements/mm_context_internal.h>
#include <elements/mm_log.h>

#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachNodC, "ElemMeshMachNodC");
#endif

using namespace Messages;
using namespace MeshMachine;
using namespace NetStateMachine;

//-=========================================================
//
//Panics
//
//-=========================================================
_LIT (KMMNodePanic,"MMNodePanic");
enum
	{
	EPanicPreallocatedSpaceAlreadyTaken = 1,
	EPanicPreallocatedSpaceReturnedOther = 2,
	EPanicInvalidClientId = 3,
	EPanicOriginatorIsNotAPeer = 4,
	EPanicNotSynchronisedActivity = 5,
	};

//-=========================================================
//
//AMMNodeBase
//
//-=========================================================
EXPORT_C AMMNodeBase::AMMNodeBase(const TNodeActivityMap& aActivityMap, const TNodeId& aNodeId)
:	ANodeBase(aNodeId),
	TIfStaticFetcherNearestInHierarchy(this),
	iActivityMap(aActivityMap),
	iPreallocatedSpace(NULL)
	{
	};

EXPORT_C void AMMNodeBase::ReturnInterfacePtrL(AMMNodeBase*& aInterface)
    {
    aInterface = this;
    }

EXPORT_C TUint AMMNodeBase::CountActivities(TUint8 aActivitySigId) const
    {
	TInt i = iActivities.Count();
	TUint count = 0;
	while (i>0)
		{
		CNodeActivityBase* a = iActivities[--i];
		if (aActivitySigId == a->ActivitySigId() && !a->IsIdle())
			{
			count++;
			}
		}
	return count;
    }

EXPORT_C TUint AMMNodeBase::CountAllActivities(TUint8 aActivitySigId) const
    {
	TInt i = iActivities.Count();
	TUint count = 0;
	while (i>0)
		{
		CNodeActivityBase* a = iActivities[--i];
		if (aActivitySigId == a->ActivitySigId())
			{
			count++;
			}
		}
	return count;
    }

EXPORT_C TUint AMMNodeBase::CountAllActivities() const
	{
	return iActivities.Count();
	}

EXPORT_C const RPointerArray<CNodeActivityBase>& AMMNodeBase::Activities() const
	{
	return iActivities;
	}

EXPORT_C void AMMNodeBase::ConstructL(TInt aSize)
    {
    //Preallocate space for preallocated activities
    if (aSize==KUseDefaultPreallocatedSize)
    	{
    	//Nodes will usually use the default parameter (-1) and rely on KDefaultPreallocatedActivityBufferSize.
    	//We do not use KDefaultPreallocatedActivityBufferSize as the default argument to avoid publishing of
    	//this constant or KDefaultMaxPreallocatedActivityCount/KDefaultMaxPreallocatedActivitySize (either can be
    	//freely changed at any time).
    	aSize = KDefaultPreallocatedActivityBufferSize;
    	}

    if (aSize>0)
    	{
    	PreallocateSpaceL(aSize);
    	}
	}

EXPORT_C AMMNodeBase::~AMMNodeBase()
	{
	//We may still have some idle activities in the iActivities array.
	//This may be considered normal since there is no strict requirement
	//for a node to destroy all of them before going into destruction phase.
	//What is crutial however, is that there are no running activities left.


	for (TInt i = iActivities.Count() - 1; i >= 0; i--)
		{
		CNodeActivityBase* activity = iActivities[i];
		if (activity->IsIdle())
			{
			iActivities.Remove(i);
			activity->Destroy();
			}
		else
			{
			MESH_LOG((KMeshMachineSubTag, _L8("ERROR: ~AMMNodeBase(%08x) - Running activity found! (%08x) %s (%d)"),
			    this, activity, activity->ActivityName(), activity->ActivityId()));
			}
		}

#ifdef _DEBUG
	for (TInt i = iActivities.Count() -1; i >= 0; i--)
		{
		CNodeActivityBase* activity = iActivities[i];
		if (!activity->IsIdle())
			{
			User::Invariant(); //Running activities still present! Please fix your node's cleanup!
			}
		}
#endif

    for (TInt i = iActivities.Count() -1; i >= 0; i--)
		{
		CNodeActivityBase* activity = iActivities[i];
		iActivities.Remove(i);
		activity->Destroy();
		}
    iActivities.Close();

    User::Free(iPreallocatedSpace);
    iPreallocatedSpace = NULL;

	}

EXPORT_C void AMMNodeBase::RemoveClient(const TRuntimeCtxId& aClientId)
	{
	TNodeNullContext ctx(*this);
    RemoveClient(aClientId, ctx);
	}

EXPORT_C void AMMNodeBase::RemoveClient(const TRuntimeCtxId& aClientId, TNodeContextBase& aContext)
	{
	TInt foundAt = 0;
	RNodeInterface* client = DoFindClient(aClientId, foundAt);
	__ASSERT_DEBUG(client, User::Panic(KMMNodePanic, EPanicInvalidClientId));

	//here we need to cancel all activities originating aClientId (except the current one)
	__ASSERT_DEBUG(!client->RecipientId().IsNull(), User::Panic(KSpecAssert_ElemMeshMachNodC, 1));
	AbortActivitiesOriginatedBy(aContext, client->RecipientId());

    if (aContext.iSender == aClientId)
        {
        aContext.iPeer = NULL;
        }	
	ANodeBase::RemoveClient(foundAt);
	}

EXPORT_C CNodeActivityBase* AMMNodeBase::FindActivityById(TUint aActivityId) const
	{
	CNodeActivityBase* a = NULL;
	for (TInt i = iActivities.Count() - 1; i>=0 && a==NULL; i--)
		{
		CNodeActivityBase* comp = iActivities[i];
		if (!comp->IsIdle()	&& comp->ActivityId() == aActivityId)
			{
			a = comp;
			}
		}
	return a;
	}

EXPORT_C CNodeActivityBase* AMMNodeBase::FindAddressedActivity(const TNodeContextBase& aContext) const
	{
	const TNodeCtxId* recipient = address_cast<const TNodeCtxId>(&aContext.iRecipient);
	if (recipient==NULL)
		{
		return NULL;
		}

	CNodeActivityBase* a = NULL;
	for (TInt i = iActivities.Count() - 1; i>=0 && a==NULL; i--)
		{
		CNodeActivityBase* act = iActivities[i];
		const TNodeId& postedTo = act->PostedToNodeId();
		if (!act->IsIdle()
			&& (postedTo.IsNull() || aContext.iSender == postedTo)
				&& (recipient->NodeCtx() == act->ActivityId()))
			{
			a = act;
			}
		}
	return a;
	}

EXPORT_C void AMMNodeBase::Received(const TNodeSignal::TMessageId aNoPeerMsgIds[], TNodeContextBase& aContext)
    {
	aContext.iReturn = KErrNone; //Clear the return value.
    CNodeActivityBase* sendersActivity = NULL;
    if (!Accept(aContext, sendersActivity) && aContext.iReturn == KErrNone )
        {//start new ones? but only if we know who sent a message or if it's a noPeer message
        //or if it's us sending message to ourselves

	    //is it a message that must have a peer
	    TBool noPeer = EFalse;
	    for (const TNodeSignal::TMessageId* p = &aNoPeerMsgIds[0] ; !p->IsNull() ; ++p)
	    	{
	    	noPeer = (*p == aContext.iMessage.MessageId());
	    	if (noPeer)
	    		{
	    		break;
	    		}
	    	}
        if (noPeer || aContext.IsPeer() || aContext.IsSelf())
            {
			//At this stage we are about to add the peer to the activiy originators.
			//For all "peer requests" we must ensure the peer is in the client's list.
			//If this assertion fires, your environment is critically corrupted.
			//You must repair it by reconsidering your protocols.
			__ASSERT_ALWAYS(noPeer || aContext.iPeer!=NULL, User::Panic(KMMNodePanic, EPanicOriginatorIsNotAPeer));
           	TRAP(aContext.iReturn, StartNewActivitiesL(aContext, sendersActivity));
            }
        else
            {
            //unknown sender => ignore the message
			NM_LOG_ADDRESS_EXT(KMeshMachineSubTag, aContext.iSender, _L8("AMMNodeBase:\tReceived() - Unknown sender. Message must have a peer => ignoring message"));
            }
        }
    else
        {
        __ASSERT_DEBUG(aContext.iMessage.MessageId().IsNull() || aContext.iReturn != KErrNone, User::Panic(KSpecAssert_ElemMeshMachNodC, 2)); //message must be processed
        }
	}

EXPORT_C void AMMNodeBase::HandlePassThroughMessage(TNodeContextBase& aContext)
    {
	if (aContext.iMessage.IsMessage<TEBase::TCancel>())
	    {
	    ASSERT(aContext.iNodeActivity == NULL); //by definition, since nobody accepted this;
	    TInt originatorIndex = KErrNotFound;
		//Please note:
		//The same originator must not attempt to start more than one activities on the same node
		//(so there is only ever one activity on the node with the originator == that this TCancel is addressed to)!
		//In general one originator (identified by a unique channel id) must never originate more
		//than one activities at all.
	    for (TInt i = 0; i < iActivities.Count(); i++)
	    	{
	    	if ((originatorIndex = iActivities[i]->FindOriginator(aContext.iSender)) != KErrNotFound)
	    		{
	    		aContext.iNodeActivity = iActivities[i];
	    		break;
	    		}
	    	}
	    
	    if (aContext.iNodeActivity)
    	    {
    	    ASSERT(originatorIndex != KErrNotFound);
     	    if(aContext.iNodeActivity->iOriginators.Count() == 1) // only if this is the final originator
				{
				aContext.iNodeActivity->Cancel(aContext);
				}
			else
				{
				// just tell this originator that we've stopped with respect to it.. otherwise keep running
				TEBase::TError err(TEBase::TCancel::Id(), KErrCancel);
				aContext.PostToSender(err);
				aContext.iNodeActivity->RemoveOriginator(originatorIndex);
				}
    		}
	    }    
	else if (aContext.iReturn==KErrNone && !aContext.iMessage.MessageId().IsNull())
	    { //this is the pass-through message, forward to .....
        //Find the activity being addressed, if any
		aContext.iNodeActivity = FindAddressedActivity(aContext);
		if (aContext.iNodeActivity)
			{ //Peer message, forward to originators
			//aContext.iSender = Id();
			NM_LOG_MESSAGE_EXT(KMeshMachineSubTag, aContext.iMessage, _L8("AMMNodeBase::HandlePassThroughMessage - message not served by the node but matching activity found=>SEND TO ACTIVITY'S ORIGINATORS"));
			aContext.iNodeActivity->PostToOriginators(aContext.iMessage);
			if (aContext.iMessage.IsMessage<TEBase::TError>())
			    {
			    //An activity received an error=>remove it from active array
			    //This scenario will usually happen when there is no error activity on the node,
			    //or when the error activity decides not to handle this particular error.
			    aContext.iNodeActivity->SetIdle();
			    }
			aContext.iMessage.ClearMessageId();
			}
		else
			{ //A stray message
			//Please note that we should not implement forwarding for non-response messages here.
			//Any non-response message that needs forwarding should use TMessageDispatcher or other
			//supported mechanisms.
			NM_LOG_MESSAGE_EXT(KMeshMachineSubTag, aContext.iMessage, _L8("ERROR: AMMNodeBase::HandlePassThroughMessage - A stray message received!"));
			}
        }
    }


EXPORT_C void AMMNodeBase::HandleMessageReturnValue(TNodeContextBase& aContext)
    {
	if (aContext.iReturn != KErrNone)
		{
		//keep the original activity in case no aCurrentActivity
		if(aContext.iNodeActivity && aContext.iNodeActivity->iOriginators.Count() > 0)
			{
			aContext.iNodeActivity->SetError(aContext.iReturn);
			// Mark the activity for destruction
			// When the activity is destroyed the Originators will be errored
			aContext.iNodeActivity->SetIdle();
			}

		// Check to see if the node was unable to handle an error (e.g., an error recovery activity failed to start
		// because there was insufficient memory).  If so, we manually error the activity and set it to idle causing
		// its originators to be errored.  This is the most we can do if an error activity cannot be run.
		else if(aContext.iMessage.IsMessage<TEBase::TError>())
			{
			CNodeActivityBase* activity = FindAddressedActivity(aContext);
			if(activity)
				{
				activity->SetError(static_cast<TEBase::TError&>(aContext.iMessage).iValue);
				// Mark the activity for destruction
				// When the activity is destroyed the Originators will be errored
				activity->SetIdle();
				}
			}

		else
			{ //No activity == single triple activity or an error from the first transition, so use the sender's message id.
			TEBase::TError err(aContext.iMessage.MessageId(), aContext.iReturn);
			aContext.PostToSender(err);
			}
		aContext.iReturn = KErrNone;
		aContext.iMessage.ClearMessageId();
		}
    }


//N.B. This function may delete the object pointed to by aCurrentActivity.
//So don't use it after this function is called.
//Also, it may delete the node itself, so don't use it after this function is called.
EXPORT_C void AMMNodeBase::PostReceived(TNodeContextBase& aContext)
	{
    HandleMessageReturnValue(aContext);
	HandlePassThroughMessage(aContext);
    SignalActivities();
    }

void AMMNodeBase::SignalActivities()
	{
	TBool awoke = ETrue;
	TInt c = iActivities.Count();
	TNodeNullContext context(*this);

	//Clean up..
	while (awoke)
		{
		awoke = EFalse;
		for (TInt i = iActivities.Count() - 1 ; i>=0 ; --i)
			{
			//Signal to waiting activities if:
			//1) an event was received (& the state has potentialy changed as a result)
			//- remember that an activity does not have to get idle as a result of processing an event which changes the state!!!!
			//2) another waiting activity awoke == reacted to the signal (& the state has potentialy changed as a result)
			//If any signalled activity reacted, the state could change and all other activities need to be signalled again.
			context.iNodeActivity = iActivities[i];
			awoke |= context.iNodeActivity->Signal(context);
			}
		}

	for (TInt i = iActivities.Count() - 1 ; i>=0 ; --i)
		{
		context.iNodeActivity = iActivities[i];
		if(context.iNodeActivity->IsIdle())
			{
			iActivities.Remove(i);
			context.iNodeActivity->Destroy();
			context.iNodeActivity = NULL;
			}
		}
	}


EXPORT_C void AMMNodeBase::AbortActivitiesOriginatedBy(TNodeContextBase& aContext, const TNodeId& aCommsId, TBool aIsNodeBeingDestroyed)
    {
    CNodeActivityBase* caller = aContext.iNodeActivity;
    TBool abortAll = aCommsId.IsNull();

    for (TInt i = iActivities.Count() - 1; i>=0; i--)
        {
        aContext.iNodeActivity = iActivities[i];
        
        if (!abortAll && aContext.iNodeActivity->PostedToNodeId() == aCommsId)
            {//clear postedto if a leaver has been set as a postedto at any of the running activities.
             //No other messages will ever come from the leaver and it is not gonna be safe to forward TCancels
             //to the leaver, so at least postedto must be cleared to avoid the crash. It could be speculated that
             //if the postedto is still set, then either the postedto node failed to respond or the local activity failed to
             //clear postedto when it had responded. Worth putting a speculative ASSERT here to catch misdeeds. 
        
             //clearing postedto shouldn't be done in here (AbortActivitiesOriginatedBy), but I (RZ) have expressed my disrespect to the
             //this method before and the suggestion that it should go (replaced by a CancelActivitiesOriginatedBy).
             //So instead of introducing another method that  loops through activities i decided to piggyback the function in an 
             //existing method. Note that the final logic should be based on RNodeInterfaces and not TNodeIds.
            aContext.iNodeActivity->ClearPostedTo();
            }
		
		// We dont want to abort already idle activities or they may error.		
		if(aContext.iNodeActivity->IsIdle())
			{
			continue;
			}
		
        if (caller != aContext.iNodeActivity)
            {
        	if (abortAll)
        	    {
        	    //Abort the whole activity (Cancel it & error all originators)
        	    aContext.iNodeActivity->Abort(aContext,aIsNodeBeingDestroyed);
        	    }
        	else
      	        {
      	        //Abort for one originator only (Cancel the activity if last originator & error just this one originator)
      	        TInt idx = aContext.iNodeActivity->FindOriginator(aCommsId);
      	        if (KErrNotFound!=idx)
      	        	{
                    TBool canSend = ETrue;
					if(aContext.iNodeActivity->iOriginators.Count() == 1) // only if this is the final originator
						{
						aContext.iNodeActivity->SetError(KErrAbort);
           	        	aContext.iNodeActivity->Cancel(aContext);
           	        	//This is a workaround for CCommsBinderRequest. The proper fix is to abolish the concept of aborting activities.
                        //Aborting activities is a bad idea as an aborted activity isn't given a chance to perform graceful cleanup.
           	        	//Today activities get aborted because their orinators urgently leave. I.e.: they are trully leaving now! Last orders!
           	        	//It is then incorrect to leave the activity d'tor to finish the wrap up - because the node will be gone by then.
           	        	//So whether and when to send an error must be decided here, by this generic code that has no clue on the subtleties
           	        	//of individual activities. If there is no abort - there is urgent leavers. They send TLeaveRequest and they politely
           	        	//wait for the completion and all this code is unnecessary.
           	        	canSend = (aContext.iNodeActivity->Error() != KErrNone);
						}
                    
                    //In the "quiet mode", when the hosting node is being destroyed, we can not afford sending
                    //an error to the node as it would hit void.
                    TNodePeerId& originator = aContext.iNodeActivity->iOriginators[idx];
                    canSend &= !((aIsNodeBeingDestroyed && originator == aContext.NodeId())
                        || aContext.iMessage.IsMessage<TEChild::TLeft>()); 
                    if (canSend)
                        {
                        aContext.iNodeActivity->PostToOriginator(originator, TEBase::TError(aContext.iMessage.MessageId(), KErrAbort).CRef());
                        }
                    aContext.iNodeActivity->RemoveOriginator(idx);
      	        	}
      	        }
            }
        }
    aContext.iReturn = KErrNone;
    aContext.iNodeActivity = caller;
    }

//This fn finds the (first) activity that has an originator matching that passed in
EXPORT_C CNodeActivityBase* AMMNodeBase::FindActivityOriginatedBy(const TRuntimeCtxId& aPeerId)
    {
    for (TInt i = 0; i < iActivities.Count(); i++)
		{
		CNodeActivityBase* a = iActivities[i];
		if (!a->IsIdle()
			&& a->FindOriginator(aPeerId) != KErrNotFound)
		    {
			return a;
		    }
		}
	return NULL;
    }

TBool AMMNodeBase::Accept(TNodeContextBase& aContext, CNodeActivityBase*& aFoundActivity)
    {
//    NM_LOG_MESSAGE_EXT(KMeshMachineSubTag, aContext.iMessage, _L8("AMMNodeBase:\tAccept"));

    //process the existing ones
    for (TInt i = 0; i < iActivities.Count(); i++)
		{
		CNodeActivityBase* a = aContext.iNodeActivity = iActivities[i];
		if(a->Next(aContext))
		    {
	    	if (KErrNone == aContext.iReturn)
				{
        		aContext.iMessage.ClearMessageId(); // message processed and value no longer needed
	        	}

			// If state went idle, aFoundActivity will be deleted later by PostReceived,
			//  this gives us a chance to report error to originators if necessary.
			aFoundActivity = a;
			return ETrue;
		    }

		if (a->FindOriginator(aContext.iSender) != KErrNotFound)
		    {
		    aFoundActivity = a;
		    }
	    }
	return EFalse;
    }

void AMMNodeBase::StartNewActivitiesL(TNodeContextBase& aContext, CNodeActivityBase* aSendersActivity)
    {
    //new activity can only be started if the senders channel hasn't started any yet
    //but we need to check so that we can return an error
    TNodeActivityIter activityIter(iActivityMap);

	const TNodeActivity* activity;
    while (NULL!=(activity = activityIter++) && aContext.iReturn == KErrNone)
        {
		const TStateTriple* first;
		aContext.iNodeActivity = NULL;
		TInt i = iActivities.Count();
		while (i>0 && !aContext.iNodeActivity)
			{
			if (activity->iId == iActivities[--i]->ActivityId())
				{
				aContext.iNodeActivity = iActivities[i];
				}
			}

        if ( (first = CNodeActivityBase::Accept( aContext,
                *activity, NetStateMachine::KExecuteAlways)) != NULL )
            { //we do need to have a state here to go to otherwise we assume it is an activity
            //with one triple only
            if ((first+1)->iTCtor != NULL)
                {
                //is it any of the existing ones?
    	        if (aContext.iNodeActivity)
    	            {
                    __ASSERT_DEBUG(aSendersActivity == NULL || aSendersActivity == aContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachNodC, 3));
                    if (aSendersActivity == NULL)
                        {
                        XNodePeerId originator(aContext.iSender, aContext.iPeer);
        	            aContext.iNodeActivity->StartL(aContext, originator, *first);
                        }
    	            }
    	        else
    	            {

#ifdef SYMBIAN_TRACE_ENABLE
                    if (aSendersActivity)
                        {
						//The sender is already running an activity on this node.
						//There is no definitive reason why to disallow starting of a next one,
						//it is however a sign that something _may_ be going horribly wrong.

						//Usually, this is the case when the same originator sends several requests without
						//caring of the order in which they are going to be completed and the possibility of
						//cancelling them (there exist no way of targetting TCancel to more than one activity).

						//Also, an error activity may be starting and producing this warning (but please note that
						//error activity has no originators).

						//Please also see note in CNodeActivityBase::Next()!
                        MESH_LOG_CONTEXT_EXT(KMeshMachineSubTag, aContext, _L8("WARNING: AMMNodeBase:\tStartNewActivitiesL - sender is starting an extra activity - TCancel will not work!"));
						TInt i = iActivities.Count();
                        MESH_LOG((KMeshMachineSubTag, _L8("\tThe sender has already started %d activities:"), i));
						while (i>0)
							{
							if (iActivities[--i]->FindOriginator(aContext.iSender) >= 0)
								{
		                        MESH_LOG((KMeshMachineSubTag, _L8("\t(%08x) %s (%d)"), aSendersActivity, aSendersActivity->ActivityName(), aSendersActivity->ActivityId()));
								}
							}
                        }
#endif
    	            //create a new one then
    	            TRAPD(err, StartActivityL(aContext, *activity, *first));
    	            if(err!=KErrNone)
    	            	{
	                    MESH_LOG((KMeshMachineSubTag,
	                        _L8("ERROR: AMMNodeBase %08x:\tStartNewActivitiesL - Activity failed to start due to error %d!"),
	                        this, err));

	                    User::Leave(err);
    	            	}
    	            }
                }
        	aContext.iMessage.ClearMessageId(); //message processed
    	    break;//only one activity can start since we have one activity per
    	    //channelId. (ChannelId == CommsId + activityId)
            }
        }
    }

void AMMNodeBase::StartActivityL(TNodeContextBase& aContext, const TNodeActivity& aActivitySig, const NetStateMachine::TStateTriple& aFirst)
    {
	CNodeActivityBase* nodeActivity;
	// Activity is based on one of 2 declarations. One of which has an extra member. In the case of the instance
	// with a second member the activities Ctor will point to this second member. Since the first member is a TNodeActivity
	// We can compare the activities Ctor pointer to the address of the second member to assess which type of declarations
	// this is.
	
	if (aActivitySig.iFlags & TNodeActivity::EContextCtor)
		{ // TNodeActivity's iCtor is a pointer to Activity Ctor
		nodeActivity = ((TNodeActivity::TStaticActivityContextCtor)aActivitySig.iCtor)(aActivitySig,aContext);
		}
	else
		{ // TNodeActivity's iCtor is a pointer to activity constructor
		nodeActivity = ((TNodeActivity::TStaticActivityCtor)aActivitySig.iCtor)(aActivitySig,*this);
		}

    if (iActivities.Find(nodeActivity)==KErrNotFound)
    	{
    	//The activity did not add itself to the list in any special way, append it here
	    CleanupStack::PushL(nodeActivity);
   		nodeActivity->AppendActivityL();
	    CleanupStack::Pop(nodeActivity);
		}

	//assign only after the activity is successfully appended
	aContext.iNodeActivity = nodeActivity;

    //if StartL leaves the "a" will be removed from the array and deleted in ::PostReceived
    //since it will be idle
    XNodePeerId originator(aContext.iSender, aContext.iPeer);
    nodeActivity->StartL(aContext, originator, aFirst);
    }

void AMMNodeBase::PreallocateSpaceL(TUint aSize)
	{
	__ASSERT_DEBUG(aSize>0, User::Panic(KSpecAssert_ElemMeshMachNodC, 4));
	// Reserve extra space for the maximum number of preallocated activities supported by this node
	// to ensure they can be added to the activities list while the system is out of memory.  We also
	// add enough space for extra data strucures to store the free list - the caller cannot reserve this
	// space without knowledge of the preallocation implementation so we do this ourselves.
	TUint maxPreallocatedActivities = aSize / KDefaultMaxPreallocatedActivitySize;
	iActivities.ReserveL(iActivities.Count() + maxPreallocatedActivities);
	__ASSERT_DEBUG(iPreallocatedSpace==NULL, User::Panic(KSpecAssert_ElemMeshMachNodC, 5));
    iPreallocatedSpace = User::AllocZL(aSize + sizeof(TUint) + maxPreallocatedActivities * sizeof(TAny*));
    *reinterpret_cast<TUint*>(iPreallocatedSpace) = maxPreallocatedActivities;
	}

TUint AMMNodeBase::MaxPreallocatedActivityCount() const
	{
	// Maximum number of activities that can be preallocated is stored at the beginning of the buffer.
	if(iPreallocatedSpace)
		{
		return *reinterpret_cast<TUint*>(iPreallocatedSpace);
		}
	return 0;
	}

TAny* AMMNodeBase::GetPreallocatedCell(TUint aIndex) const
	{
	// Calculate the offset of the start of the preallocated space after the free list.
	TUint8* bufferStart = reinterpret_cast<TUint8*>(iPreallocatedSpace) + sizeof(TUint*) + MaxPreallocatedActivityCount() * sizeof(TAny*);

	return bufferStart + aIndex * KDefaultMaxPreallocatedActivitySize;
	}

TAny* AMMNodeBase::BorrowPreallocatedSpace(TUint aSize)
	{
	MESH_LOG((KMeshMachineSubTag, _L8("AMMNodeBase %08x:\tBorrowPreallocatedSpace (%d)"),this,aSize));

	__ASSERT_ALWAYS(iPreallocatedSpace, User::Panic(KMMNodePanic, EPanicPreallocatedSpaceAlreadyTaken));
	__ASSERT_ALWAYS(aSize <= KDefaultMaxPreallocatedActivitySize, User::Panic(KMMNodePanic, EPanicPreallocatedSpaceAlreadyTaken));

	#ifdef SYMBIAN_TRACE_ENABLE
		if(!iPreallocatedSpace)
			{
			MESH_LOG((KMeshMachineSubTag, _L8("ERROR AMMNodeBase %08x:\tBorrowPreallocatedSpace - Preallocated space has not been allocated!"), this));
			}
		if(aSize>KDefaultMaxPreallocatedActivitySize)
			{
			MESH_LOG((KMeshMachineSubTag, _L8("ERROR AMMNodeBase %08x:\tBorrowPreallocatedSpace - Size exceeds maximum limit for a single allocation (%d was requested but only %d is available)!"), this, aSize, KDefaultMaxPreallocatedActivitySize));
			}
	#endif

	TAny* ptr = NULL;
	TUint maxPreallocatedActivities = MaxPreallocatedActivityCount();
	TAny** freeList = reinterpret_cast<TAny**>(reinterpret_cast<TUint8*>(iPreallocatedSpace) + sizeof(TUint));
	TUint index = 0;

	// Search the free list to see if any space is available.
	for(; index < maxPreallocatedActivities; index ++)
		{
		if(!freeList[index])
			{
			// This cell is now allocated to the calling activity.
			ptr = freeList[index] = GetPreallocatedCell(index);

			// Zero the cell so that any object allocated will have the expected initial zero fill.
			memset(ptr, 0, KDefaultMaxPreallocatedActivitySize);

			break;
			}
		}

	// Check to make sure a free cell was found.
	bool freeCellFound = index < maxPreallocatedActivities;
	if(!freeCellFound)
		{
		MESH_LOG((KMeshMachineSubTag, _L8("ERROR AMMNodeBase %08x:\tBorrowPreallocatedSpace - All %d preallocation cells have been allocated!"), this, maxPreallocatedActivities));
		__ASSERT_DEBUG(freeCellFound, User::Panic(KMMNodePanic, EPanicPreallocatedSpaceAlreadyTaken));
		// Attempt to allocate some space from the heap now in a last attempt to continue processing.
		TRAPD(err,ptr =  User::AllocL(aSize));
		__ASSERT_ALWAYS(err, User::Panic(KMessagesPanic, EPreAllocationFailedPanic));
		}

	return ptr;
	}

void AMMNodeBase::ReturnPreallocatedSpace(TAny* aSpace)
	{
	MESH_LOG((KMeshMachineSubTag, _L8("AMMNodeBase %08x:\tReturnPreallocatedSpace"), this));

	TUint maxPreallocatedActivities = MaxPreallocatedActivityCount();
	TAny** freeList = reinterpret_cast<TAny**>(reinterpret_cast<TUint8*>(iPreallocatedSpace) + sizeof(TUint));
	TUint index = 0;

	// Search the free list to return the cell.
	for(; index < maxPreallocatedActivities; index ++)
		{
		if(freeList[index] == aSpace)
			{
			// Mark this cell as free.
			freeList[index] = NULL;

			break;
			}
		}

	bool allocatedCellFound = index < maxPreallocatedActivities;
	if(!allocatedCellFound)
		{
		MESH_LOG((KMeshMachineSubTag, _L8("ERROR AMMNodeBase %08x:\tReturnPreallocatedSpace - the returned pointer 0x%08X is invalid!"), this, aSpace));
		__ASSERT_DEBUG(allocatedCellFound, User::Panic(KMMNodePanic, EPanicPreallocatedSpaceReturnedOther));
		// May be returning a cell which was created in error recovery section of the Borrow in which case if must be "free"d.
		User::Free(aSpace);
		}
	}


//-=========================================================
//
//AMMNodeIdBase
//
//-=========================================================
EXPORT_C const TNodeId& AMMNodeIdBase::NodeId() const
    {
    return Messages::ANodeId::Id();
    }

//-=========================================================
//
//XNodePeerId
//
//-=========================================================
EXPORT_C XNodePeerId::XNodePeerId(const TRuntimeCtxId& aPeerId, RNodeInterface* aPeer, CBase* aInfo)
:	TNodePeerId(aPeerId, aPeer),
	iInfo(aInfo)
	{
	}

EXPORT_C void XNodePeerId::Destroy()
	{
	delete iInfo;
	iInfo = NULL;
	}


