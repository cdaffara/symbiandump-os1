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

/**
 @file
*/

#include "mm_activities.h"
#include <elements/mm_context.h>
#include <elements/mm_states.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>
#include <elements/nm_messages_errorrecovery.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachActC, "ElemMeshMachActC");
#endif

using namespace MeshMachine;
using namespace Messages;
using namespace Elements;
using namespace NetStateMachine;

//-=========================================================
//
//TNodeActivityIter
//
//-=========================================================
const TNodeActivity* TNodeActivityIter::FetchActivity()
	{
    const TNodeActivity* a = NULL;
    if (*iCurrentEntry)
    	{
    	a = &(*iCurrentEntry)();
    	++iCurrentEntry;
    	}
    else
    	{
    	if (NULL!=*(iCurrentEntry+1))
 		   	{
    		const TNodeActivityMap::TStaticNodeActivityMap& nextMapFn = (const TNodeActivityMap::TStaticNodeActivityMap&)*(iCurrentEntry+1);
    		iCurrentEntry = &nextMapFn().iFirstActivity;
    		a = FetchActivity();
    		}
    	}
    return a;
	}

//-=========================================================
//
//CNodeActivityBase
//
//-=========================================================
EXPORT_C CNodeActivityBase* CNodeActivityBase::NewL(const TNodeActivity& aActivitySig, AMMNodeBase& aNode)
    {
    return new(ELeave)CNodeActivityBase(aActivitySig, aNode);
    }

EXPORT_C CNodeActivityBase::CNodeActivityBase(const TNodeActivity& aActivitySig, AMMNodeBase& aNode)
:	iNode(aNode),
	iActivitySig(aActivitySig)
	{
	}

EXPORT_C CNodeActivityBase::~CNodeActivityBase()
	{
	if(iError != KErrNone)
		{
		PostToOriginators(TEBase::TError(KickOffMessageId(), iError).CRef());
		}

	//If an activity constructed custom originator's interfaces, now it needs to clean them up
	for (TInt i = iOriginators.Count() - 1; i >= 0; --i)
		{
		RemoveOriginator(i);
		}

	iOriginators.Close();
	}

EXPORT_C NetInterfaces::TInterfaceControl* CNodeActivityBase::DoFetchInterfaceControlL(TInt /*aInterfaceId*/)
/** Allows to fetch an arbitrary interface instance from the client

@internalTechnology
*/
	{
	return this; //as a base we assume are the control for any interface any derivative may implement
	}

//static
const TStateTriple* CNodeActivityBase::Accept(TNodeContextBase& aContext, const TNodeActivity& aActivitySig, TInt aTransitionTag)
    {
    const TStateTriple& first = aActivitySig.iFirstTriple;

    //in debug only(!) check if we are in the first triple as expected
    __ASSERT_DEBUG((&first-1)->iSCtor==NULL && (&first-1)->iTCtor==NULL, User::Panic(KSpecAssert_ElemMeshMachActC, 1));

    TBool accept = (aActivitySig.iKickOffMessageId == KNullMessageId
		|| (aActivitySig.iKickOffMessageId == aContext.iMessage.MessageId().MessageId() &&
			aActivitySig.iKickOffMessageRealm == aContext.iMessage.MessageId().Realm())
		)
    	&& NetStateMachine::ACore::Accept(first, &aContext, aTransitionTag);

    //if the activity accepted the message and the first triple has a transition, we execute it here
    //NOTE that the first transition is called BEFORE the originator (sender of the message)
    //is added to the activity list
    if (accept && first.iTCtor)
    	{
    	TBuf8<KMaxStateClassByteSize> mem;
   		ACore::Do(first,&aContext,mem);
   		MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase:\tAccept->first transition [ANode=0x%08x] [Activity=%s] [Triple=%s]"),
   				&aContext.NodeId().Node(), aActivitySig.iName ? aActivitySig.iName : _S8("Undefined"), first.iName? first.iName : _S8("Undefined")));
    	}

	if (accept && aContext.iReturn == KErrNone)
		{
		// MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase:\tAccept->accepted")));
		return &first;
		}
    return NULL;
    }

EXPORT_C void CNodeActivityBase::StartL(TNodeContextBase& aContext, const XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	__ASSERT_DEBUG(FindOriginator(aOriginator) == KErrNotFound, User::Panic(KSpecAssert_ElemMeshMachActC, 2));
  	iOriginators.AppendL(aOriginator);

	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CNodeActivityBase %08x:\tStartL->starting activity"), this));
	if (IsIdle())
		{
		NetStateMachine::ACore::Start(&aContext, aFirst);
		MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CNodeActivityBase %08x:\tStartL->activity started"), this));
		}
	}

EXPORT_C TBool CNodeActivityBase::MatchSender(const TNodeContextBase& aContext) const
    {
    //The role of this method is to filter out all messages that 'this' should
    //not be bothered with. 'this' should be interested in messages coming from
    //what's set as iPostedToId or from one of the orginators. 
    //if the message's recipient specifies the activity id, then that
    //activity must much that of 'this'.
    TBool sender = PostedToNodeId().IsNull() || 
    			   aContext.iSender == PostedToNodeId() || 
    			   FindOriginator(aContext.iSender) != KErrNotFound;
    const TNodeCtxId* recipient = address_cast<const TNodeCtxId>(&aContext.iRecipient);
    TBool activity = (recipient == NULL || ActivityId() == recipient->NodeCtx());

#ifdef SYMBIAN_TRACE_ENABLE
	//If didn't match, trace why.
	if (!(sender && activity))
		{
		NM_LOG_START_BLOCK(KMeshMachineSubTag, _L8("CNodeActivityBase::MatchSender"));
		if(!sender)
			{
			MESH_LOG((KMeshMachineSubTag(), _L8("CNodeActivityBase %08x:\tiPostedToId mismatch:"), this));
			NM_LOG_ADDRESS(KMeshMachineSubTag(), PostedToNodeId());
			NM_LOG_ADDRESS(KMeshMachineSubTag(), aContext.iSender);
			MESH_LOG((KMeshMachineSubTag(), _L8("CNodeActivityBase %08x:\toriginators' mismatch:"), this));
			for (TInt i = iOriginators.Count() - 1; i>=0; i--)
		        {
		        NM_LOG_ADDRESS(KMeshMachineSubTag(), iOriginators[i].RecipientId());
		        }
			}
		if(!activity)
			{
			MESH_LOG((KMeshMachineSubTag(), _L8("CNodeActivityBase %08x:\tactivity mismatch [ours 0x%x, recipient 0x%x]"), this, ActivityId(), recipient->NodeCtx()));
			}
		NM_LOG_END_BLOCK(KMeshMachineSubTag(), KNullDesC8());
		}
#endif

    return sender && activity;
    }

//static
EXPORT_C TAny* CNodeActivityBase::BorrowPreallocatedSpace(AMMNodeBase& aNode, TUint aSize)
	{
	return aNode.BorrowPreallocatedSpace(aSize);
	}

EXPORT_C void CNodeActivityBase::ReturnPreallocatedSpace(TAny* aSpace)
	{
	return iNode.ReturnPreallocatedSpace(aSpace);
	}

EXPORT_C void CNodeActivityBase::AppendActivityL()
	{
	//Based on the fact that there can ever be only one preallocated activity active at a time
	//it is sufficient to always maintain one reserved space in the activity array.
	//NOTE: ReserveL() will not cause table expansion when the already allocated space is
	//big enough to hold the new count = the array will not grow indefinietly.
	iNode.iActivities.ReserveL(iNode.iActivities.Count() + iNode.MaxPreallocatedActivityCount() + 1); //Reserve for additional + potential preallocated)

	//Can not fail now
	__ASSERT_ALWAYS(iNode.iActivities.Append(this) == KErrNone, User::Panic(KMMActivityPanic, EPanicNoPreallocatedSpace));
	}

EXPORT_C void CNodeActivityBase::AppendPreallocatedActivity()
	{
	//Can not fail now
	__ASSERT_ALWAYS(iNode.iActivities.Append(this) == KErrNone, User::Panic(KMMActivityPanic, EPanicNoPreallocatedSpace));
	}

EXPORT_C void CNodeActivityBase::InsertPreallocatedDestroyActivity()
	{
	//Can not fail now
	__ASSERT_ALWAYS(iNode.iActivities.Insert(this, 0) == KErrNone, User::Panic(KMMActivityPanic, EPanicNoPreallocatedSpace));
	}

EXPORT_C TInt CNodeActivityBase::FindOriginator(const RNodeInterface& aPeerToFind) const
    {
	for (TInt i = iOriginators.Count() - 1; i>=0; i--)
        {
        //When you see a panic here this is probably because your originator
        //stopped being a peer and this node failed to remove it from the activity originator's list.
        if (iOriginators[i] == aPeerToFind.RecipientId())
            {
	        return i;
            }
        }
    return KErrNotFound;
    }

EXPORT_C TInt CNodeActivityBase::FindOriginator(const TRuntimeCtxId& aPeerToFind) const
    {
	for (TInt i = iOriginators.Count() - 1; i>=0; i--)
        {
        //When you see a panic here this is probably because your originator
        //stopped being a peer and this node failed to remove it from the activity originator's list.
        if (iOriginators[i] == aPeerToFind)
            {
	        return i;
            }
        }
    return KErrNotFound;
	}

EXPORT_C TInt CNodeActivityBase::FindOriginator(const TNodePeerId& aOriginator) const
	{
	for (TInt i = iOriginators.Count() - 1; i>=0; i--)
        {
        //When you see a panic here this is probably because your originator
        //stopped being a peer and this node failed to remove it from the activity originator's list.
        if (iOriginators[i] == aOriginator)
            {
	        return i;
            }
        }
    return KErrNotFound;
	}

EXPORT_C TUint16 CNodeActivityBase::ActivityId() const
	{
	return ActivitySigId();
	}

//You should use this function to remove originators to prevent memory leaking!
EXPORT_C void CNodeActivityBase::RemoveOriginator(TInt aIndex)
	{
	__ASSERT_DEBUG(aIndex>=0 && aIndex<iOriginators.Count(), User::Panic(KSpecAssert_ElemMeshMachActC, 3));
	iOriginators[aIndex].Destroy();
	iOriginators.Remove(aIndex);
	}

EXPORT_C void CNodeActivityBase::SetIdle()
	{
	MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase %08x:\tSetIdle"), this));
	NetStateMachine::ACore::SetIdle();
	}

EXPORT_C TBool CNodeActivityBase::Next(TNodeContextBase& aContext)
	{
	TBool nextRet = EFalse;
	//Check if this is the same id we have recently sent to
	TBool senderMatch = MatchSender(aContext);
	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext,
	    (senderMatch ? _L8("CNodeActivityBase %08x:\tNext->match") : _L8("CNodeActivityBase %08x:\tNext->NO match"), this));

	if (senderMatch)
		{
		nextRet = ACore::Next(&aContext);
	    if(nextRet)
			{
	    	MESH_LOG_ACTIVITY_EXT(KMeshMachineSubTag, this, &aContext, (_L8("CNodeActivityBase %08x:\tNext->transition"), this));
			}
		}
    return nextRet;
	}

EXPORT_C void CNodeActivityBase::Cancel(TNodeContextBase& aContext)
	{//we expect KErrCancel be set as a result of the state cancelation
    MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase %08x:\tCancel(), PostedToNodeId %08x"), this, PostedToNodeId().Ptr()));

	if ((PostedToPeer() && !(PostedToPeer()->Flags() & TClientType::ELeaving)) ||
	     PostedToNodeId() == aContext.Node().Id())
		{//only safe to forward TCancels to non-leavers or self. There is an underlying assumption that a node won't dissapear in presence of activities (see AMMNodeBase::~AMMNodeBase)
		RClientInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), PostedToNodeId(), TEBase::TCancel().CRef());
		}
    else
        {
    	NetStateMachine::ACore::Cancel(&aContext);
        }
	SetError(KErrCancel);
	}

EXPORT_C TBool CNodeActivityBase::PostToOriginator(const TNodePeerId& aOriginator, const TSignalBase& aMessage) const
	{
	if (!(aOriginator.Flags() & TClientType::ELeaving))
	    {
	    aOriginator.PostMessage(TNodeCtxId(ActivityId(), iNode.Id()), aMessage);
	    }
	else
		{
		MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase:\tPostToOriginator - IGNORING POST!")));
		return EFalse;
		}
	return ETrue;
	}

EXPORT_C TInt CNodeActivityBase::PostToOriginators(const TSignalBase& aMessage, TUint32 aFlagsToSet, TUint32 aFlagsToClear)
	{
	TInt msgSendCount=0;
	for (TInt n = iOriginators.Count() - 1;n>=0; n--)
		{
		if (PostToOriginator(iOriginators[n], aMessage))
			{
			++msgSendCount;
	    	iOriginators[n].SetFlags(aFlagsToSet);
	    	iOriginators[n].ClearFlags(aFlagsToClear);
			};
		}
	return msgSendCount;
	}

EXPORT_C void CNodeActivityBase::SetPostedTo(const TNodeId& aNodeId)
    {
    //You are being punished for storing a postedto TNodeId that you also know as your peer.
    //the Messages::RNodeInterface& overload of CNodeActivityBase::SetPostedTo
    //It is dangerous to use this overload for peers when the relation with these peers
    //is being terminated. the PostedTo facility is also used to forward TCancel in
    //the automatic calencallation handling. No message can be posted to a leaving peer
    //but only peers (Messages::RNodeInterfaces) can be recognised as leaving. 
    //
    //ASSERT temporarily commened out as it is a behavioural break. A Polonium BR
    //has been drafted and will be raised when the RNodeInterface overloads end up in the
    //codeline. http://polonium.ext.nokia.com/changerequests/cr/601/
    //__ASSERT_DEBUG(iNode.FindClient(aNodeId) == NULL, User::Panic(KSpecAssert_ElemMeshMachActC, 14));     
    
    if (aNodeId == Messages::TNodeId::NullId())
        {
        ClearPostedTo();
        return;
        }
    iPostedToId.Open(aNodeId);
    }

EXPORT_C void CNodeActivityBase::SetPostedTo(const Messages::RNodeInterface& aRecepient)
    {
    iPostedToId.Open(aRecepient);
    }

EXPORT_C void CNodeActivityBase::ClearPostedTo()
    {
    iPostedToId.Close();
    }

EXPORT_C const Messages::TNodeId& CNodeActivityBase::PostedToNodeId() const
    {
    return iPostedToId.NodeId();
    }

EXPORT_C const Messages::RNodeInterface* CNodeActivityBase::PostedToPeer() const
    {
    return iPostedToId.Peer();
    }

EXPORT_C void CNodeActivityBase::PostRequestTo(const RNodeInterface& aRecipient, const TSignalBase& aMessage, const TBool aRecipientIdCritical)
	{
	aRecipient.PostMessage(TNodeCtxId(ActivityId(), iNode.Id()), aMessage);

	// Provide the option for the identity of the receipient to be unimportant when the response arrives
	ClearPostedTo();
    if (aRecipientIdCritical)
        {
        SetPostedTo(aRecipient);
        }
	}

//Avoid using this function, always prefer PostRequestTo(const RNodeInterface& aRecipient, const TNodeSignal& aMessage)
EXPORT_C void CNodeActivityBase::PostRequestTo(const TNodeId& aRecipient, const TSignalBase& aMessage, const TBool aRecipientIdCritical)
	{
	RClientInterface::OpenPostMessageClose(TNodeCtxId(ActivityId(), iNode.Id()), aRecipient, aMessage);

	// Provide the option for the identity of the receipient to be unimportant when the response arrives
	ClearPostedTo();
	if (aRecipientIdCritical)
	    {
        SetPostedTo(aRecipient);
	    }
	}

EXPORT_C TBool CNodeActivityBase::IsIdle() const
	{
	return NetStateMachine::ACore::IsIdle();
	}

EXPORT_C void CNodeActivityBase::Abort(TNodeContextBase& aContext, TBool aIsNodeBeingDestroyed)
    {
    MESH_LOG((KMeshMachineSubTag, _L8("CNodeActivityBase %08x:\tAbort"), this));

	//the error message will be send by message post processing see AMMNodeBase::Received
   	aContext.iReturn = KErrAbort; //before so as the error could be overwritten
    NetStateMachine::ACore::Cancel(&aContext);
    if (aContext.iReturn != KErrNone)
        {
        TEBase::TError error(aContext.iMessage.MessageId(), aContext.iReturn);
       	for (TInt n = iOriginators.Count() - 1;n>=0; n--)
			{
			TNodePeerId& originator = iOriginators[n];
			//In the "quiet mode", when the hosting node is being destroyed, we can not afford sending
			//an error to the node as it would hit void.
   	        TBool canSend = !((aIsNodeBeingDestroyed && originator == aContext.NodeId())
   	        	|| aContext.iMessage.IsMessage<TEChild::TLeft>());
			if (canSend)
				{
				PostToOriginator(originator, error);
				}
			RemoveOriginator(n); //Do not allow ~CNodeActivityBase to post as client's might be already gone
			}
        }
    }




//-=========================================================
//
//CNodeActivityBase::RPostedToNodeOrPeer
//
//-=========================================================
CNodeActivityBase::RPostedToNodeOrPeer::RPostedToNodeOrPeer() 
    {
    Close();
    }

void CNodeActivityBase::RPostedToNodeOrPeer::Open(const Messages::RNodeInterface& aPeer)
    {
    Close();
    *_Peer() = const_cast<Messages::RNodeInterface*>(&aPeer);
    }

void CNodeActivityBase::RPostedToNodeOrPeer::Open(const Messages::TNodeId& aNode)
    {
    __ASSERT_DEBUG(aNode.Ptr(), User::Panic(KSpecAssert_ElemMeshMachActC, 15));    
    //see Messages::TNodeId::operator= (snapping size is essential in case aNode is more than just plain TNodeId).
    *_Node() = Messages::TNodeId(); 
    
    //normal assigment
    *_Node() = aNode;
    }
void CNodeActivityBase::RPostedToNodeOrPeer::Close()
    {
    Mem::FillZ(iBuf, sizeof(iBuf));
    }

const Messages::RNodeInterface* CNodeActivityBase::RPostedToNodeOrPeer::Peer() const
    {
    return _Node()->Ptr() ? NULL : *_Peer();
    }
const Messages::TNodeId& CNodeActivityBase::RPostedToNodeOrPeer::NodeId() const
    {
    return (_Node()->Ptr() ? *_Node() : (*_Peer() ? (*_Peer())->RecipientId() : Messages::TNodeId::NullId()));
    }


//-=========================================================
//
//CNodeRetryActivity
//
//-=========================================================
EXPORT_C CNodeRetryActivity::CNodeRetryActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
:	CNodeActivityBase(aActivitySig, aNode),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	}

EXPORT_C CNodeActivityBase* CNodeRetryActivity::NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
    {
    return new(ELeave)CNodeRetryActivity(aActivitySig, aNode);
    }

EXPORT_C TBool CNodeRetryActivity::IsIdle() const
	{
	return !IsWaiting() && CNodeActivityBase::IsIdle();
	}

EXPORT_C void CNodeRetryActivity::SetIdle()
	{
	iContextDesc.Close();
	ClearIsWaiting();
	ClearWillWait(); //Set this too in case this fn was invoked from within the serialised state ot transition
	CNodeActivityBase::SetIdle();
	}

EXPORT_C TBool CNodeRetryActivity::Signal(TNodeContextBase& aContext)
	{
	return AActivitySemaphore::Signal(aContext);
	}

EXPORT_C void CNodeRetryActivity::ReturnInterfacePtrL(AContextStore*& aInterface)
    {
    aInterface = this;
    }

EXPORT_C void CNodeRetryActivity::ReturnInterfacePtrL(AActivitySemaphore*& aInterface)
    {
    aInterface = this;
    }

// BitMap utility used only by CNodeParallelActivityBase::GetNextActivityCount
template<TInt SIZE>
class TBitmap {
public:
    static const TInt iSize = sizeof(TUint32) * 8;
    static const TUint32 iSizeMask = iSize - 1;
    static const TUint32 iFull = ~0;
    static const TInt iCount = (SIZE + iSizeMask) / iSize;
    TBitmap();
    void SetBitL(TUint aBitNum);
    TInt GetFreeBit() const;

private:
    TUint32 iBits[iCount];
};

template<TInt SIZE>
TBitmap<SIZE>::TBitmap()
{
    for (TInt i = 0 ; i < iCount ; ++i)
        {
        iBits[i] = 0;
        }
}

template<TInt SIZE>
void TBitmap<SIZE>::SetBitL(TUint aBitNum)
    {
    const TInt index = aBitNum / iSize;
	User::LeaveIfError(index < iCount ? KErrNone : KErrOverflow);

    iBits[index] |= 1 << (aBitNum & iSizeMask);
    }

template<TInt SIZE>
TInt TBitmap<SIZE>::GetFreeBit() const
    {
    for (TInt i = 0 ; i < iCount ; ++i)
        {
        const TUint32 bits = iBits[i];
        if (bits != iFull)
            {
			// Bitmap represents list of activity IDs. Activity ID 1 is a reserved value.
			// In order to mirror this fact the first bit of the bitmap is also always reserved
            TUint32 mask = 2;
            for (TInt bitIndex = 1 ; bitIndex < iSize ; ++bitIndex)
                {
                if ((bits & mask) == 0)
                    {
                    return (i * iSize) + bitIndex;
                    }
                mask <<= 1;
                }
            }
        }
    return KErrNotFound;
    }


//-=========================================================
//
//CNodeParallelActivityBase
//
//-=========================================================

EXPORT_C TUint CNodeParallelActivityBase::GetNextActivityCountL( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode )
    {
    return DoGetNextActivityCountL(aActivitySig, aNode);
    }

EXPORT_C TUint CNodeParallelActivityBase::GetNextActivityCount( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode )
    {
    // Version for infallible activities
	TUint nextActivity(0);
    TRAPD(err,nextActivity = DoGetNextActivityCountL(aActivitySig, aNode));
	__ASSERT_ALWAYS(err == KErrNone,User::Panic(KMMActivityPanic,EPanicOutOfActivities));
	return nextActivity;
    }

// For custom activities to implement New
TUint CNodeParallelActivityBase::DoGetNextActivityCountL( const TNodeActivity& aActivitySig, const AMMNodeBase& aNode )
	{
	TInt c = 1;
	
	const RPointerArray<CNodeActivityBase>& activities = aNode.Activities();
	
	TBitmap<256> activityids;
	// collect the currently used ids
	for (TInt i = 0; i < activities.Count(); i++)
		{
		TInt16 id = activities[i]->ActivityId();
		if ((id&0xff) == aActivitySig.iId)
			{
			TInt8 uniqueid = id >> 8;
			activityids.SetBitL(uniqueid);
			}
		}
	c = activityids.GetFreeBit();
	
	User::LeaveIfError( c>=0 ? KErrNone : KErrOverflow);
    return c;
	}

EXPORT_C CNodeActivityBase* CNodeParallelActivityBase::NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CNodeParallelActivityBase(aActivitySig, aNode, c);
    }

EXPORT_C CNodeParallelActivityBase::CNodeParallelActivityBase(const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aNextActivityCount)
:	CNodeActivityBase(aActivitySig, aNode),
    iActivityId((aNextActivityCount<<8)|aActivitySig.iId)
    {
    }

EXPORT_C TUint16 CNodeParallelActivityBase::ActivityId() const
    {
    return iActivityId;
    }


//-=========================================================
//
//CNodeParallelMessageStoreActivityBase
//
//-=========================================================
EXPORT_C CNodeActivityBase* CNodeParallelMessageStoreActivityBase::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
	{
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
	return new (ELeave) CNodeParallelMessageStoreActivityBase( aActivitySig, aNode, c);
	}

EXPORT_C CNodeParallelMessageStoreActivityBase::~CNodeParallelMessageStoreActivityBase()
	{
	}

EXPORT_C Messages::TSignalBase& CNodeParallelMessageStoreActivityBase::Message()
	{
	ASSERT(iMsg);
	return *iMsg;
	}

EXPORT_C void CNodeParallelMessageStoreActivityBase::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst)
	{
	SaveMessageL(aContext.iMessage);
	CNodeParallelActivityBase::StartL(aContext, aOriginator, aFirst);
	}

void CNodeParallelMessageStoreActivityBase::SaveMessageL(Messages::TSignalBase& aMessage)
	{
	Meta::CMetaDataVirtualCtorInPlace* vctr = TlsGlobals::Get().VirtualCtor();
	TPtrC8 ptr(iMsgBuf);
	iMsg = static_cast<TSignalBase*>(vctr->New(aMessage.GetTypeId(), iMsgBuf));
	if (iMsg == NULL)
		{
		User::Leave(KErrNotFound);
		}
	
	iMsg->Copy(aMessage);
	}

EXPORT_C CNodeParallelMessageStoreActivityBase::CNodeParallelMessageStoreActivityBase( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount )
	: CNodeParallelActivityBase(aActivitySig, aNode, aNextActivityCount), iMsg(NULL)
	{
	}

//-=========================================================
//
//CNodeRetryParallelActivity
//
//-=========================================================
EXPORT_C CNodeRetryParallelActivity::CNodeRetryParallelActivity( const TNodeActivity& aActivitySig, AMMNodeBase& aNode, TUint aActivitiesCount )
:	CNodeParallelActivityBase(aActivitySig, aNode, aActivitiesCount),
	TIfStaticFetcherNearestInHierarchy(this)
	{
	}

EXPORT_C CNodeActivityBase* CNodeRetryParallelActivity::NewL( const TNodeActivity& aActivitySig, AMMNodeBase& aNode )
    {
	TUint c = GetNextActivityCountL(aActivitySig,aNode);
    return new(ELeave)CNodeRetryParallelActivity(aActivitySig, aNode, c);
    }

EXPORT_C TBool CNodeRetryParallelActivity::IsIdle() const
	{
	return !IsWaiting() && CNodeActivityBase::IsIdle();
	}

EXPORT_C void CNodeRetryParallelActivity::SetIdle()
	{
	iContextDesc.Close();
	ClearIsWaiting();
	ClearWillWait(); //Set this too in case this fn was invoked from within the serialised state ot transition
	CNodeActivityBase::SetIdle();
	}

EXPORT_C TBool CNodeRetryParallelActivity::Signal(TNodeContextBase& aContext)
	{
	return AActivitySemaphore::Signal(aContext);
	}

EXPORT_C void CNodeRetryParallelActivity::ReturnInterfacePtrL(AContextStore*& aInterface)
    {
    aInterface = this;
    }

EXPORT_C void CNodeRetryParallelActivity::ReturnInterfacePtrL(AActivitySemaphore*& aInterface)
    {
    aInterface = this;
    }

//-=========================================================
//
//AContextStore
//
//-=========================================================
EXPORT_C TInt AContextStore::StoreContext(const TNodeContextBase& aContext)
    {//doesn't actually store TNodeContextBase only the message since it assumes
    //that the transition context is the
    //template <class TNODE, class TDerivedContext = TNodeContextBase>
    //struct TNodeContext : public TDerivedContext
    //where TDerivedContext = TNodeContextBase is always TNodeContextBase
    //another type of context is not needed because all custom needs should be sorted by custom
    //activity class
    TInt error = KErrNone;
    TInt len = TRuntimeCtxId::KMaxInlineAddressSize + aContext.iMessage.Length();
    if (iContextDesc.MaxLength() < len)
        {
        iContextDesc.Close();
        error = iContextDesc.Create(len);
        }
    else
    	{
    	iContextDesc.Zero();
    	}
    if (KErrNone==error)
    	{
    	error = aContext.iSender.Store(iContextDesc); //would catch if iContextDesc has been allocated properly
    	}
    if (KErrNone==error)
    	{
    	error = aContext.iMessage.Store(iContextDesc); //would catch if iContextDesc has been allocated properly
    	}
    return error;
    }

EXPORT_C TNodeContextBase* AContextStore::LoadContext(AMMNodeBase& aNode, CNodeActivityBase* aNodeActivity, TDes8& aCtxBuff, TDes8& aMsgBuff, const TNodeId& aDummy)
    {
    if (iContextDesc.Length() < sizeof(TNodeId))
        {
        return NULL;
        }

	const TRuntimeCtxId* sender = reinterpret_cast<const TRuntimeCtxId*>(iContextDesc.Ptr());
	TPtrC8 contextStore(iContextDesc.Ptr()+sender->Size(),iContextDesc.Length()-sender->Size());
	TSignatureBase* msg = static_cast<TSignatureBase*>(TlsGlobals::Get().VirtualCtor()->New(contextStore, aMsgBuff));
	if (!msg)
        {
		return NULL;
        }

    //This assumes (as CNodeActivityBase::StoreContext) that the transition context is the
    //template <class TNODE, class TDerivedContext = TNodeContextBase>
    //struct TNodeContext : public TDerivedContext
    //where TDerivedContext = TNodeContextBase is always TNodeContextBase.
    //another type of context is not needed because all custom needs should be sorted by custom
    //activity class
    __ASSERT_DEBUG(aNodeActivity, User::Panic(KSpecAssert_ElemMeshMachActC, 4));
	return ::new ((TUint8*)aCtxBuff.Ptr()) TNodeContextBase(aNode, *msg, *sender, aDummy, aNodeActivity);
    }

EXPORT_C TBool AContextStore::IsStored() const
	{
	return iContextDesc.Length() >= sizeof(TNodeId);
	}

// Needs to be moved to error-recoverable activity class
EXPORT_C const TNodeSignal::TMessageId& AContextStore::RetryingForMessageId() const
	{
	return iRetryingForMessageId;
	}

//Called only ever from ReceivedL
EXPORT_C void AContextStore::Retry(CNodeActivityBase& aActivity, TNodeContextBase& aContext)
    {
	TBuf8<__Align8(sizeof(TNodeContextBase))> ctxBuf;
	TBuf8<__Align8(TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead)> msgBuf;
	TNodeContextBase* storedContext = LoadContext(aContext.iNode, &aActivity,ctxBuf, msgBuf, aContext.NodeId());

	//If we are here the context has been stored. There is no reason why it could not be loaded now.
	//The only reason would be that the message factory was unregistered since the context was stored
	//which is a critical condition.
	__ASSERT_ALWAYS(storedContext, User::Panic(KMMActivityPanic,EPanicCorruptedContext));

	TEErrorRecovery::TErrorRecoveryResponse& msg = message_cast<TEErrorRecovery::TErrorRecoveryResponse>(aContext.iMessage);
    iRetryingForMessageId = msg.iErrResponse.iMessageId;
	aActivity.ACore::DoCurrent(storedContext);
    aContext.Node().HandleMessageReturnValue(*storedContext);
    }

//-=========================================================
//
//AActivitySemaphore
//
//-=========================================================
EXPORT_C void AActivitySemaphore::ParkTransitionL(const TNodeContextBase& aContext)
	{
   	__ASSERT_DEBUG(aContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachActC, 5));
   	AActivitySemaphore* as = static_cast<AActivitySemaphore*>(aContext.iNodeActivity->FetchExtInterfaceL(AActivitySemaphore::KInterfaceId));
    if (!as->IsWaiting())
    	{
    	User::LeaveIfError(as->StoreContext(aContext));
    	
        //The context has been stored along with the message. Must consume the message, otherwise
        //it will fall through and be seen by other activities or passthrough handling.    	
        aContext.iMessage.ClearMessageId();
        
		as->SetIsWaiting();
		as->SetIsTransition();
	    MESH_LOG_CONTEXT_EXT(KMeshMachineSubTag, aContext, _L8("AActivitySemaphore:\tParkTransitionL->parked"));
		}
	as->SetWillWait();
    __ASSERT_DEBUG(as->IsWaiting(), User::Panic(KSpecAssert_ElemMeshMachActC, 6)); //Must be stored and waiting now
	}

EXPORT_C TInt AActivitySemaphore::ParkState(const TNodeContextBase& aContext)
	{
   	__ASSERT_DEBUG(aContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachActC, 7));
   	AActivitySemaphore* as = static_cast<AActivitySemaphore*>(aContext.iNodeActivity->FetchExtInterface(AActivitySemaphore::KInterfaceId));
    __ASSERT_DEBUG(as, User::Panic(KSpecAssert_ElemMeshMachActC, 8)); //State/transition mismatched with the activity object (activity doesn't support serialisation - wrong derivation?)
    if (!as->IsWaiting())
    	{
    	TInt error = as->StoreContext(aContext);
    	if (error!=KErrNone)
    		{
    		return error;
    		};
    	
    	//The context has been stored along with the message. Must consume the message, otherwise
    	//it will fall through and be seen by other activities or passthrough handling.
    	aContext.iMessage.ClearMessageId();
    	
		as->SetIsWaiting();
    	as->ClearIsTransition();
    	MESH_LOG_CONTEXT_EXT(KMeshMachineSubTag, aContext, _L8("AActivitySemaphore:\tParkState->parked"));
    	}
    as->SetWillWait();
    __ASSERT_DEBUG(as->IsWaiting(), User::Panic(KSpecAssert_ElemMeshMachActC, 9)); //Must be stored and waiting now
    return KErrNone;
	}

EXPORT_C TInt AActivitySemaphore::UnparkState(const TNodeContextBase& aContext)
	{
   	__ASSERT_DEBUG(aContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachActC, 10));
   	AActivitySemaphore* as = static_cast<AActivitySemaphore*>(aContext.iNodeActivity->FetchExtInterface(AActivitySemaphore::KInterfaceId));
    __ASSERT_DEBUG(as, User::Panic(KSpecAssert_ElemMeshMachActC, 11)); //State/transition mismatched with the activity object (activity doesn't support serialisation - wrong derivation?)
    if (as->IsWaiting())
    	{
		as->ClearIsWaiting();
    	MESH_LOG_CONTEXT_EXT(KMeshMachineSubTag, aContext, _L8("AActivitySemaphore:\tUnparkState->unparked"));
    	}
    as->ClearWillWait();
    __ASSERT_DEBUG(!as->IsWaiting(), User::Panic(KSpecAssert_ElemMeshMachActC, 12)); //Must be stored and waiting now
    return KErrNone;
	}

EXPORT_C void AActivitySemaphore::Wait()
	{
	MESH_LOG((KMeshMachineSubTag, _L8("AActivitySemaphore:\tWait->activity waiting")));
	SetWillWait();
	}

EXPORT_C TBool AActivitySemaphore::Signal(TNodeContextBase& aContext)
    {
	if (!(iFlags&KIsWaiting))
    	{
    	return EFalse;
    	}

    //aContext.iNodeActivity is us
   	TBuf8<__Align8(sizeof(TNodeContextBase))> ctxBuf;
   	TBuf8<__Align8(TSignalBase::KMaxInlineMessageSize + TSignalBase::KMaxUnstoredOverhead)> msgBuf;
   	TNodeCtxId dummy(aContext.ActivityId(), aContext.NodeId());
	TNodeContextBase* storedContext = LoadContext(aContext.iNode,aContext.iNodeActivity,ctxBuf,msgBuf,dummy);
	if (storedContext)
		{
        ClearWillWait();
		if (IsTransition())
			{
			//Transition stored
			aContext.iNodeActivity->ACore::DoCurrent(storedContext);
			}
		else
			{
			//This is the state stored
#ifdef _DEBUG
			TBool hasRun = aContext.iNodeActivity->ACore::Next(storedContext);
			__ASSERT_DEBUG(hasRun!=(iFlags & KWillWait), User::Panic(KSpecAssert_ElemMeshMachActC, 13));
#else
			aContext.iNodeActivity->ACore::Next(storedContext);
#endif
			}

		if (!(iFlags & KWillWait))
    		{ //We have been run
    		ClearIsWaiting();
            iContextDesc.Zero();
            aContext.Node().HandleMessageReturnValue(*storedContext);
            return ETrue;
    		}
    	}

    return EFalse;
    }


