// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_MM_NODE_H
#define SYMBIAN_MM_NODE_H

#include <elements/nm_node.h>
#include <elements/sm_core.h>
#include <elements/nm_signatures.h>

namespace MeshMachine
{

static const TInt KUseDefaultPreallocatedSize = -1;

class CNodeActivityBase;
struct TNodeActivity;
struct TNodeActivityMap;
class TNodeContextBase;

//-=========================================================
//
//AMMNodeBase
//
//-=========================================================
class AMMNodeBase : public Messages::ANodeBase,
                    public ITFHIERARCHY_LINK_1(AMMNodeBase, Messages::ANodeBase, AMMNodeBase)
/**
@internalTechnology
*/
	{
	friend class CNodeActivityBase;
	friend class AContextStore; //to access HandleMessageReturnValue from Retry
	friend class AActivitySemaphore; //to access HandleMessageReturnValue from Retry

public:
	typedef ITFHIERARCHY_LINK_1(AMMNodeBase, Messages::ANodeBase, AMMNodeBase) TIfStaticFetcherNearestInHierarchy;

    static const TInt KInterfaceId = 0x102864E5;

	/**	@internalAll */
    IMPORT_C void ReturnInterfacePtrL(AMMNodeBase*& aInterface);

protected:
	IMPORT_C explicit AMMNodeBase(const TNodeActivityMap& aActivityMap, const Messages::TNodeId& aNodeId);
	IMPORT_C ~AMMNodeBase();
    IMPORT_C void ConstructL(TInt aSize = KUseDefaultPreallocatedSize); //When KUseDefaultPreallocatedSize, KDefaultMaxPreallocatedActivitySize will be used

public:
	/**	@internalAll */
	IMPORT_C void Received(const Messages::TNodeSignal::TMessageId aNoPeerMsgIds[], TNodeContextBase& aContext);

	/**
	This function may delete the object pointed to by aCurrentActivity. So don't use it after this function is called.
	@internalAll
	*/
	IMPORT_C void PostReceived(TNodeContextBase& aContext);

	/**	@publishedPartner @released */
	IMPORT_C void RemoveClient(const Messages::TRuntimeCtxId& aClientId, TNodeContextBase& aContext);

public:
	/**	@internalAll */
	IMPORT_C void RemoveClient(const Messages::TRuntimeCtxId& aClientId);

	/**	@publishedPartner @released */
    IMPORT_C TUint CountActivities(TUint8 aActivitySigId) const; //Counts only active activities (not idle)
	/**	@publishedPartner @released */
    IMPORT_C TUint CountAllActivities(TUint8 aActivitySigId) const;  //Counts all activities with the specified sig id
	/**	@publishedPartner @released */
    IMPORT_C TUint CountAllActivities() const; //Count all activities
	/**	@publishedPartner @released */
	IMPORT_C CNodeActivityBase* FindActivityById(TUint aActivityId) const;

    /**	@internalAll */
    IMPORT_C const RPointerArray<CNodeActivityBase>& Activities() const;
	/**	@internalAll */
    IMPORT_C CNodeActivityBase* FindAddressedActivity(const TNodeContextBase& aContext) const;
	/**	@internalAll */
	IMPORT_C CNodeActivityBase* FindActivityOriginatedBy(const Messages::TRuntimeCtxId& aPeerId);
	/**	@internalAll */
    IMPORT_C void AbortActivitiesOriginatedBy(TNodeContextBase& aContext, const Messages::TNodeId& aNodeId = Messages::TNodeId::NullId(), TBool aIsNodeBeingDestroyed = EFalse);

protected:
	/**	@internalAll */
	IMPORT_C virtual void HandlePassThroughMessage(TNodeContextBase& aContext);
	/**	@internalAll */
	IMPORT_C virtual void HandleMessageReturnValue(TNodeContextBase& aContext);
	/**	@internalAll */	
	void SignalActivities();

private:
	/**	@internalAll */
    void StartNewActivitiesL(TNodeContextBase& aContext, CNodeActivityBase* aSendersActivity);
	/**	@internalAll */
    void StartActivityL(TNodeContextBase& aContext, const TNodeActivity& aActivitySig, const NetStateMachine::TStateTriple& aFirst);
	/**	@internalAll */
    TBool Accept(TNodeContextBase& aContext, CNodeActivityBase*& aFoundActivity);
	/**	@internalAll */
    TAny* BorrowPreallocatedSpace(TUint aSize);
	/**	@internalAll */
    void ReturnPreallocatedSpace(TAny* aSpace);

protected:
	/**	@internalAll */
	const TNodeActivityMap& iActivityMap;

private:
	/**	@internalAll */
	RPointerArray<CNodeActivityBase> iActivities;

	//-===============================================================================
    //PREALLOCATION:
    //This facility is designed to assist in securing sensitive behaviour that
    //shouldn't fail and/but needs to allocate memory (usually cleanup tasks).
    //
    //The memory is allocated using User::AllocZL(aSize), the same way the CBase new
    //operator does it. CBase doesn't override delete operator and hence makes the same
    //assumption we make herein.
    //-===============================================================================
	/**	@internalAll */
	void PreallocateSpaceL(TUint aSize); //Utility function
	TUint MaxPreallocatedActivityCount() const;
	TAny* GetPreallocatedCell(TUint aIndex) const;
	/**	@internalAll */
	TAny* iPreallocatedSpace;
	};

//-=========================================================
//
//AMMNodeIdBase
//
//-=========================================================

class AMMNodeIdBase : protected Messages::ANode, public Messages::ANodeId, public AMMNodeBase
/**
@publishedPartner
@released

Aggregation base class for mesh-machine based nodes.
*/
	{
protected:
    explicit AMMNodeIdBase(const TNodeActivityMap& aActivityMap)
    :	Messages::ANodeId(static_cast<Messages::ANode&>(*this)),
    	AMMNodeBase(aActivityMap, Messages::ANodeId::Id())
        {
        }

    IMPORT_C virtual const Messages::TNodeId& NodeId() const;
public:
	//this needs to be here provisionally - subclasses complain about imbiguous access to ::Id()
	const Messages::TNodeId& Id() const
		{
		return NodeId();	
		}    
	};

} //namespace MeshMachine

#endif //SYMBIAN_NM_NODE_H

