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

#ifndef SYMBIAN_NM_NODE_H
#define SYMBIAN_NM_NODE_H

#include <elements/nm_address_internal.h>
#include <elements/nm_log.h>
#include <elements/interfacetable.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessNodH, "ElemNodeMessNodH");
#endif


namespace Messages
{

//-=========================================================
//
//MNodeId
//
//-=========================================================
class MNodeId
	{
public:
	virtual const TNodeId& NodeId() const = 0;
	};

class TSignatureBase;
class ANode : public MNodeId
	{
public:
    //if Received returns != KErrNone than the dispatcher will send a TError to the
    //originator of the message
	virtual void ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage) = 0;

	IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);
	IMPORT_C TAny* FetchNodeInterfaceL(TInt aInterfaceId);
	};

//-=========================================================
//
//ANodeId
//
//-=========================================================
class ANodeId
	{
public:
	const TNodeId& Id() const
		{
		return iNodeId;
		}

protected:
	explicit ANodeId(ANode& aNode) :
		iNodeId(aNode)
		{
		}

	explicit ANodeId(TAny* aNode) :
		iNodeId(aNode)
		{
		}

	void UnregisterSelf()
		{
		iNodeId.UnregisterSelf();
		}

private:
    TNodeOid iNodeId;
	};


//-=========================================================
//
//ASimpleNodeIdBase - aggregation base class for simple nodes able to receive messages
//
//-=========================================================
class ASimpleNodeIdBase : protected ANode, public ANodeId
	{
protected:
    explicit ASimpleNodeIdBase()
    :	ANodeId(*(static_cast<ANode*>(this)))
        {
        }

    IMPORT_C virtual const TNodeId& NodeId() const;
	};

//-=========================================================
//
//ANodeBase
//
//-=========================================================
class ANodeBase : protected NetInterfaces::TInterfaceControl,
                  public ITFHIERARCHY_1(ANodeBase, ANodeBase)
	{
public:
	typedef ITFHIERARCHY_1(ANodeBase, ANodeBase) TIfStaticFetcherNearestInHierarchy;

public:
	static const TInt KInterfaceId = 10005; //Normally a UID - 10005 for compatibility reasons
	IMPORT_C void ReturnInterfacePtrL(ANodeBase*& aInterface);

public:
    IMPORT_C virtual RNodeInterface* NewClientInterfaceL(const TClientType& aClientType, TAny* aClientInfo = NULL);
    IMPORT_C virtual RNodeInterface* AddClientL(const TNodeId& aClientId, const TClientType& aClientType, TAny* aClientInfo = NULL);
    IMPORT_C void RemoveClient(RNodeInterface& aClient);
    IMPORT_C void RemoveClient(TInt aIndex);
    inline const TNodeId& Id() const
    	{
    	return iSelfInterface.RecipientId();
    	}

    template<class TTYPEMATCHPOLICY>
    inline TClientIter<TTYPEMATCHPOLICY> GetClientIter(const TClientType& aIncClientType, const TClientType& aExcClientType = TClientType::NullType()) const
    	{
    	return TClientIter<TTYPEMATCHPOLICY>(iClients, aIncClientType, aExcClientType);
    	}

	template<class TTYPEMATCHPOLICY>
    inline RNodeInterface* GetFirstClient(const TClientType& aIncClientType, const TClientType& aExcClientType = TClientType::NullType()) const
        {
        return GetClientIter<TTYPEMATCHPOLICY>(aIncClientType, aExcClientType)[0];
        }

	template<class TTYPEMATCHPOLICY>
    inline TInt PostToClients(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage, const TClientType& aIncClientType, const TClientType& aExcClientType = TClientType::NullType(), TUint32 aFlagsToSet = 0, TUint32 aFlagsToClear = 0) const
        {
        TClientIter<TTYPEMATCHPOLICY> iter = GetClientIter<TTYPEMATCHPOLICY>(aIncClientType, aExcClientType);
        return PostToClients(iter, aPostFrom, aMessage, aFlagsToSet, aFlagsToClear);
        }

	template<class TTYPEMATCHPOLICY>
    inline TUint CountClients(const TClientType& aIncClientType, const TClientType& aExcClientType = TClientType::NullType()) const
        {
        TClientIter<TTYPEMATCHPOLICY> iter = GetClientIter<TTYPEMATCHPOLICY>(aIncClientType, aExcClientType);
        return CountClients(iter);
        }

    IMPORT_C RNodeInterface* FindClient(const TRuntimeCtxId& aId) const;
    IMPORT_C RNodeInterface* FindClientL(const TRuntimeCtxId& aId) const; //Must not return NULL
    RNodeInterface& SelfInterface()
    	{
    	__ASSERT_DEBUG(iSelfInterface.IsOpen(), User::Panic(KSpecAssert_ElemNodeMessNodH, 1)); //TODO - add panic code
    	return iSelfInterface;
    	}

protected:
	IMPORT_C explicit ANodeBase(const TNodeId& aNodeId);
	IMPORT_C ~ANodeBase();
	IMPORT_C TInt PostToClients(TClientIterBase& aClientIterator, const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage, TUint32 aFlagsToSet, TUint32 aFlagsToClear) const;
	IMPORT_C TUint CountClients(TClientIterBase& aClientIterator) const;
	IMPORT_C RNodeInterface* DoFindClient(const TRuntimeCtxId& aId, TInt& aFoundAtIndex) const;

protected:
    RPointerArray<RNodeInterface> iClients;
//TODO[PROD] - could it be combined with the Id() somehow to optimise for size?
   	RNodeInterface iSelfInterface; //For posting messages to ourselves + necessary for requests origiated from the same node at the MM level
	};

//-=========================================================
//
//ANodeIdBase - aggregation base class for nodes able to receive messages
//
//-=========================================================
class ANodeIdBase : protected ANode, public ANodeId, public ANodeBase
	{
protected:
    explicit ANodeIdBase()
    :   ANodeId(static_cast<ANode&>(*this)),
    	ANodeBase(ANodeId::Id())
        {
        }

    IMPORT_C virtual const TNodeId& NodeId() const;
	};

//-=========================================================
//
//mnode_cast
//
//-=========================================================
template<class NODE>
NODE* mnode_cast(Messages::ANode* aNode)
    {
   	return static_cast<NODE*>(aNode);
    }

template <class NODE>
NODE& mnode_cast(Messages::ANode& aNode)
    {
    return static_cast<NODE&>(aNode);
    }


} //namespace Messages


#endif //SYMBIAN_NM_NODE_H

