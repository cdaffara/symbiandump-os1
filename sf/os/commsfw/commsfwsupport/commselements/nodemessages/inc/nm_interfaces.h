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
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NM_INTERFACES_H
#define SYMBIAN_NM_INTERFACES_H

//#include <elements/nm_signals.h>
#include <elements/nm_address.h>
#include <elements/nm_log.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessIntH, "ElemNodeMessIntH");
#endif

// Forward declarations to be used with RNodeInterface to allow for friend assignment
namespace ESock
	{
	class CMMCommsProviderBase;
	}

namespace CprClientLeaveActivity
	{
	class CClientLeaveActivity;
	}

namespace Messages
{

class MTransportSender;
class ANode;
class TSignalBase;


class TClientType
/*
TClientType defines a technology agnostic collection of enums for types and
flags describing various traits of peers. It is also a placeholder for these
values. When implementing a relation with a peer, TClientType (or its technology
specific override) can be used to collect informations about the peers.
@see RNodeInterface
*/
	{
public:
    enum TType
        {
        //-=============================================
        //
        // 1. Types 32bits (a client can only have one type)
        //
        //-=============================================
        EAll            = 0xFFFFFFFF,
        EUnknown        = 0x00000000,
        ERegistrar      = 0x00000001, //Useful general purpose role.
                                      //Used to represent simple non-standard or one-off or transient
                                      //roles which now do not need to be specially defined for these
                                      //simple and/or rare relationships.
        EWorker         = 0x00000002, //Useful general purpose role.

		/**
		@internalTechnology
		Reserved types that can change at any time and must NOT be used under any circumstances.
		Please take care when defining derived client types (do not reference ELastReservedType).
		*/
        ELastReservedType_InternalTechnology_CanBeReusedAtAnyTime_DoNotUseInCode = 0x00000080,
		};

	enum TFlags
		{
        //-=============================================
        //
        // 2. Flags 32bits (a client can have many flags)
        //
        //-=============================================
        ELeaving        = 0x00000001, //Client has been sent the final goodbye message (e.g TClientLeavingRequest)
                             //and we are awaiting the channel clear confirmation (e.g. TLeaveComplete).
                             //No further communication with this client is safe as it assumes we discard
                             //any references to it. For example it could destruct itself etc.
		EAdministrative = 0x00000002, //These clients perform some administrative tasks on our node.

		/**
		@internalTechnology
		Reserved flags that can change at any time and must NOT be used under any circumstances.
		Please take care when defining derived client flags (do not reference ELastReservedFlag).
		*/
        ELastReservedFlag_InternalTechnology_CanBeReusedAtAnyTime_DoNotUseInCode = 0x00000080
        };

public:
	IMPORT_C static const TClientType& NullType();

	TClientType()
	:	iClientType(EUnknown),
	    iClientFlags(0)
		{
		}

	TClientType(TUint aClientType, TUint aClientFlags = 0)
	:	iClientType(aClientType),
		iClientFlags(aClientFlags)
		{
		}

	TClientType(const TClientType& aClientType)
	:	iClientType(aClientType.iClientType),
		iClientFlags(aClientType.iClientFlags)
		{
		}

	TBool IsNull() const
	    {
	    return iClientType == 0 && iClientFlags == 0;
	    }

	TUint32 Type() const
	    {
	    return iClientType;
	    }

	TUint32 Flags() const
	    {
	    return iClientFlags;
	    }

	TUint32 SetFlags(TUint32 aFlags)
	    {
	    return iClientFlags |= aFlags;
	    }

	TUint32 ClearFlags(TUint32 aFlags)
	    {
	    return iClientFlags &= ~aFlags;
	    }

protected:
    TUint32 iClientType;
    TUint32 iClientFlags;
	};

//None of the specified
struct TNoMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
        return (aVal & aMatch) == 0;
		}
	};

//Any of the specified, can be more, can be less
struct TAnyMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
        return (aVal & aMatch) != 0;
		}
	};

//Any of the specified, both can be null, can be more, can be less
struct TExactOrAnyMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
		return (aVal == aMatch) || (aVal & aMatch) != 0;
		}
	};

//All of the specified, can't be more, can be less
struct TSubSetMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
        return (aVal & aMatch) == aVal;
		}
	};

//Any of the specified, can be more, can't be less
struct TSuperSetMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
        return (aVal & aMatch) == aMatch;
		}
	};

//All of the specified, can't be less, can't be more
struct TExactMatchPolicy
	{
	inline static TBool Match(TUint32 aVal, TUint32 aMatch)
		{
		return aVal == aMatch;
		}
	};

template <typename TTYPEMATCHPOLICY = TSubSetMatchPolicy, typename TFLAGSMATCHPOLICY = TSuperSetMatchPolicy>
struct TMatchPolicy
	{
	inline static TBool Match(const TClientType& aType, const TClientType& aMatch)
		{
		return TTYPEMATCHPOLICY::Match(aType.Type(),aMatch.Type())
			&& TFLAGSMATCHPOLICY::Match(aType.Flags(),aMatch.Flags());
		}
	};

template <typename TINCMATCHPOLICY = TMatchPolicy<TSubSetMatchPolicy,TSuperSetMatchPolicy>, typename TEXCMATCHPOLICY = TMatchPolicy<TExactMatchPolicy,TExactMatchPolicy> >
struct TTypeMatchPolicy
	{
	inline static TBool Match(const TClientType& aType, const TClientType& aInclude, const TClientType& aExclude)
		{
		TBool inc = TINCMATCHPOLICY::Match(aType, aInclude);
		TBool exc = aExclude.IsNull() || !TEXCMATCHPOLICY::Match(aType, aExclude);
		return inc && exc;
		}
	};

//We could be using partially specialised typedefs here,
//if they only were supported by C++ at the time of writing this code...
//                         INCType   |     Type                  Flag              EXCType    |      Type               Flag
//                       ----------------------------------------------           -------------------------------------------------
typedef TTypeMatchPolicy<TMatchPolicy<TSubSetMatchPolicy,  TSuperSetMatchPolicy>, TMatchPolicy<TSuperSetMatchPolicy,TAnyMatchPolicy> > TDefaultClientMatchPolicy;
typedef TTypeMatchPolicy<TMatchPolicy<TSuperSetMatchPolicy,TSuperSetMatchPolicy>, TMatchPolicy<TSuperSetMatchPolicy,TAnyMatchPolicy> > TFlagsOnlyClientMatchPolicy;
typedef TTypeMatchPolicy<TMatchPolicy<TSubSetMatchPolicy,  TNoMatchPolicy>,       TMatchPolicy<TSuperSetMatchPolicy,TAnyMatchPolicy> > TExcludeTypeAndFlagClientMatchPolicy;
typedef TTypeMatchPolicy<TMatchPolicy<TSubSetMatchPolicy, TExactOrAnyMatchPolicy>, TMatchPolicy<TSuperSetMatchPolicy,TAnyMatchPolicy> > TExactOrAnyIncludeFlagsClientMatchPolicy;


class RClientInterface
/*
Represents an opaque recipient of messages with no assumptions about its
type (i.e.: it's not necesserilly a node and the only thing known about it, is its
TRuntimeCtxId). Relations with such recipients should be implemented by declaring
RClientInterface as members.
@see RNodeInterface
*/
	{
public:
	IMPORT_C RClientInterface();
	IMPORT_C virtual ~RClientInterface(); //Derived types may be used via base ptrs
	IMPORT_C void Open(const TRuntimeCtxId& aPostTo, MTransportSender* aSender = NULL);
	IMPORT_C void Close(); // stomp members
	IMPORT_C TBool operator==(const TRuntimeCtxId& aRHS) const;
	IMPORT_C TBool operator==(const RClientInterface& aRHS) const;

	IMPORT_C TBool IsOpen() const;

	inline const TRuntimeCtxId& RecipientId() const
		{
		return *reinterpret_cast<const TRuntimeCtxId*>(iRecipientAddress);
		}

	IMPORT_C static void OpenPostMessageClose(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage);
	IMPORT_C void PostMessage(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const;

protected:
	void PostMessage(const TRuntimeCtxId& aPostFrom, const TRuntimeCtxId& aPostTo, const TSignalBase& aMessage) const;
	inline TRuntimeCtxId& RecipientRef()
		{
		return *reinterpret_cast<TRuntimeCtxId*>(iRecipientAddress);
		}

	inline const TRuntimeCtxId& RecipientRef() const
		{
		return *reinterpret_cast<const TRuntimeCtxId*>(iRecipientAddress);
		}

private:
	MTransportSender* iTransportSender;
	TUint8 iRecipientAddress[__Align8(TRuntimeCtxId::KMaxInlineAddressSize)];
	};


class RNodeInterface : public RClientInterface
/*
RNodeInterface represents a relation with a node (ANode). It does so by:
- storing the node's address (TNodeId)
- collecting informations about the node (::Type & ::Flags).
Classes (typically nodes) should implement relations with another nodes
by declaring RNodeInterface as members.
@see TClientType
*/
	{
	friend class TClientIterBase;

	// Friend classes needed to allow access to memory preallocation functionality
	// This friend funtionality is to be removed when the preallocation functionality is revisited
	// These friends are not to be used for any further functionality
	friend class ESock::CMMCommsProviderBase;
	friend class CprClientLeaveActivity::CClientLeaveActivity;

public:
	IMPORT_C RNodeInterface();
	IMPORT_C virtual ~RNodeInterface();

	IMPORT_C void Open(TNodeId aPostTo, const TClientType& aClientType = TClientType::NullType(), MTransportSender* aSender = NULL);
	IMPORT_C void Close();
	IMPORT_C TBool operator==(const RNodeInterface& aRHS) const;

	inline TBool operator==(const TRuntimeCtxId& aRHS) const
		{
		return RClientInterface::operator==(aRHS);
		}

	IMPORT_C void PostMessage(const TRuntimeCtxId& aPostFrom, const TNodeId::TRemainder& aPostTo, const TSignalBase& aMessage) const;
	inline void PostMessage(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const
		{
		#ifdef SYMBIAN_TRACE_ENABLE
			if(Flags() & TClientType::ELeaving)
				{
				NM_LOG_START_BLOCK(KNodeMessagesSubTag, _L8("ERROR: Post while node is leaving"));
				NM_LOG_ADDRESS_EXT(KNodeMessagesSubTag, aPostFrom, _L8("From:"));
				NM_LOG_ADDRESS_EXT(KNodeMessagesSubTag, RecipientId(), _L8("To:"));
				NM_LOG_MESSAGE_EXT(KNodeMessagesSubTag, aMessage, _L8("Msg:"));
				NM_LOG_END_BLOCK(KNodeMessagesSubTag, KNullDesC8);
				}
		#endif
		__ASSERT_DEBUG(!(Flags() & TClientType::ELeaving), User::Panic(KSpecAssert_ElemNodeMessIntH, 1)); //OOOOOPS! What are you doing??
		RClientInterface::PostMessage(aPostFrom, aMessage);
		}

	inline const TNodeId& RecipientId() const
		{
		return address_cast<TNodeId>(RClientInterface::RecipientId());
		}

	TClientType& ClientType()
		{
		return iClientType;
		}

	TUint32 Type() const
	    {
	    return iClientType.Type();
	    }

	TUint32 Flags() const
	    {
	    return iClientType.Flags();
	    }

	TUint32 SetFlags(TUint32 aFlags)
	    {
#if defined(_DEBUG) && defined(NM_LOG_FLAG_SETTINGS)
	    // Note that this generates quite alot of logging, hence is not turned on by default.
	    // Note also that (a minority of) flag setting may also take place in TNodePeerId.  
	    const TUint32 oldFlags = Flags();
	    const TUint32 newFlags = iClientType.SetFlags(aFlags);
	    if (oldFlags != newFlags)
	    	{
	    	NM_LOG((KNodeMessagesSubTag, _L8("RNI::SetFlags(%x) ANode=%08x, %x->%x"), aFlags, &RecipientId().Node(), oldFlags, newFlags));
	    	}
	    return newFlags;
#else
	    return iClientType.SetFlags(aFlags);
#endif
	    }

	TUint32 ClearFlags(TUint32 aFlags)
	    {
#if defined(_DEBUG) && defined(NM_LOG_FLAG_SETTINGS)
	    const TUint32 oldFlags = Flags();
	    const TUint32 newFlags = iClientType.ClearFlags(aFlags);
	    if (oldFlags != newFlags)
	    	{
	    	NM_LOG((KNodeMessagesSubTag, _L8("RNI::ClearFlags(%x) ANode=%08x, %x->%x"), aFlags, &RecipientId().Node(), oldFlags, newFlags));
	    	}
	    return newFlags;
#else
	    return iClientType.ClearFlags(aFlags);
#endif
	    }

// Handling of preallocating memory to be used by the Leave activity
private:
	IMPORT_C void PreAllocL(TUint aAllocSize);

	/**
	Claim some preallocated memory from the RNodeInterface. This preallocated memory is used for activities that
	absolutely cannot fail in any scenario, such as for Leaving the node. For this reason, when the
	RNodeInterface is created, some memory is allocated so that a leaving activity can't even fail in an out
	of memory situation.

	This method is static as the memory must be claimed before the activity object is constructed.

	@param aNode The node that owns the preallocated space. This must be the node that the activity runs on.
	@param aSize Size of buffer to allocate.

	@return A pointer to the allocated block of memory
	*/
	IMPORT_C TAny* ClaimPreallocatedSpace(TUint aSize);

protected:
	TClientType iClientType;
private:
	struct TPreAllocStore
		{
		TPreAllocStore()
			{
			iPreAllocSize=0;
			iPreAllocatedActivityChunk=NULL;
			}
		~TPreAllocStore()
			{
			if(iPreAllocatedActivityChunk != NULL)
				{
				User::Free(iPreAllocatedActivityChunk);
				}
			}
		TUint iPreAllocSize;
		TAny* iPreAllocatedActivityChunk;
		};

	TPreAllocStore* iPreAlloc;
	};

class RRequestOriginator
/*
RRequestOriginator is a helper class that ackowledges the very common scenario
where an activity running at a remote node sends a request and that request
needs to be replied to. RRequestOriginator is associated with a node (holds
a reference to RNodeInterface) as well as stores the TNodeId::TReminder of
the address in order to identify the activity running at that node.
*/
    {
public:
	IMPORT_C TInt Open(RNodeInterface& aNode, const TRuntimeCtxId& aRequestOriginator);
	IMPORT_C void Open(RRequestOriginator& aOriginalRequest);
	IMPORT_C void Close();

	IMPORT_C TBool operator==(const RRequestOriginator& aRHS) const;
	IMPORT_C TBool operator==(const TRuntimeCtxId& aRHS) const;

	IMPORT_C TBool IsOpen() const;

	Messages::RNodeInterface& Node()
		{
		__ASSERT_DEBUG(IsOpen(), User::Panic(Messages::KMessagesPanic,Messages::EClientNotValidPanic));
		return *iNode;
		}
	const Messages::RNodeInterface& Node() const
		{
		__ASSERT_DEBUG(IsOpen(), User::Panic(Messages::KMessagesPanic,Messages::EClientNotValidPanic));
		return *iNode;
		}

	Messages::TNodeId::TRemainder& Remainder()
		{
		Node(); //Make sure 'this' is openned
		return iRemainder;
		}
	const Messages::TNodeId::TRemainder& Remainder() const
		{
		Node(); //Make sure 'this' is openned
		return iRemainder;
		}


	IMPORT_C void PostMessage(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const;
	IMPORT_C void ReplyTo(const TRuntimeCtxId& aReplyFrom, const TSignalBase& aMessage);

private:
	Messages::TNodeId::TRemainder iRemainder;
	Messages::RNodeInterface* 	  iNode;
    };

//
// TClientIterBase
class TClientIterBase
	{
public:
	typedef RPointerArray<RNodeInterface> TClientArray;

public:
	inline RNodeInterface* operator++(TInt /*aInd*/) //-postfix
	    {
	    RNodeInterface* cli = Find(iIndex, +1, 1);
	    iIndex++;
	    return cli;
	    }

	inline RNodeInterface* operator++()              //-prefix
	    {
	    ++iIndex;
	    return Find(iIndex, +1, 1);
	    }

	inline RNodeInterface* operator--(TInt /*aInd*/) //-postfix
	    {
	    RNodeInterface* cli = Find(iIndex, -1, 1);
	    iIndex--;
	    return cli;
	    }

	IMPORT_C RNodeInterface* operator[](TInt aInd);

	inline RNodeInterface* operator*()
		{
		return Find(iIndex, +1, 0);
		}

    void SetToLast()
        {
        iIndex = iClients.Count() - 1;
        }

	void Reset()
		{
		iIndex = 0;
		}

protected:
	virtual TBool TypeMatch(RNodeInterface& aClient) const = 0;
	explicit TClientIterBase(const RPointerArray<RNodeInterface>& aClients)
    :	iClients(aClients),
    	iIndex(0)
    	{
		}

    IMPORT_C RNodeInterface* Find(TInt& aInd, TInt aDir, TInt aCount);
    const RPointerArray<RNodeInterface>& iClients;
    TInt iIndex;
	};

//
// TClientIter
template<class TTYPEMATCHPOLICY = TDefaultClientMatchPolicy>
class TClientIter : public TClientIterBase
	{
public:
	explicit TClientIter(const RPointerArray<RNodeInterface>& aClients, const TClientType& aInclude, const TClientType& aExclude = TClientType::NullType())
	:	TClientIterBase(aClients), iInclude(aInclude), iExclude(aExclude) {};

private:
	virtual TBool TypeMatch(RNodeInterface& aClient) const
		{
		return TTYPEMATCHPOLICY::Match(aClient.ClientType(), iInclude, iExclude);
		}

	TClientType iInclude;
	TClientType iExclude;
	};


} //namespace Messages

#endif
//SYMBIAN_NM_INTERFACES_H


