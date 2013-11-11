/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file mm_nodepeer.h
 @internalAll
*/

#ifndef SYMBIAN_MM_NODEPEER_H
#define SYMBIAN_MM_NODEPEER_H


namespace Messages
{
class TNodePeerId
    {
	friend class TNodeCtxId;

public:
	TNodePeerId(const TRuntimeCtxId& aPeerId, RNodeInterface* aNodePeer)
		{
		__ASSERT_DEBUG(aNodePeer == NULL || (*aNodePeer) == aPeerId, User::Panic(KMessagesPanic, EAddressNotValidPanic));
		iClientPeer.Open(aPeerId);
		iNodePeer = aNodePeer;
		}

	TBool operator==(const TRuntimeCtxId& aRHS) const
		{
		return (iClientPeer == aRHS);
		}

	TBool operator==(const TNodePeerId& aRHS) const
		{
		return (iClientPeer == aRHS.iClientPeer);
		}

	void PostMessage(const TRuntimeCtxId& aPostFrom, const TSignalBase& aMessage) const
		{
		iClientPeer.PostMessage(aPostFrom, aMessage);
		}

	inline const TRuntimeCtxId& RecipientId() const
		{
		return iClientPeer.RecipientId();
		}

	TUint32 Type() const
		{
		return Peer().Type();
		}

    TUint32 Flags() const
        {
        // Non-client peers lack flags (a state early in relationship)
        if(PeerPtr())
            {
            return PeerPtr()->Flags();
            }
        return 0;
        }

    TUint32 SetFlags(TUint32 aFlags)
        {
        // We only require peer info to be present if an actual state change is occurring (as opposed to default handling)
        if(aFlags)
            {
            return Peer().SetFlags(aFlags);
            }
        return Flags();
        }

    TUint32 ClearFlags(TUint32 aFlags)
        {
        // We only require peer info to be present if an actual state change is occurring (as opposed to default handling)
        if(aFlags)
            {
            return Peer().ClearFlags(aFlags);
            }
        return Flags();
        }

	const RNodeInterface& Peer() const
		{
		__ASSERT_DEBUG(PeerPtr(), User::Panic(KMessagesPanic, EClientNotValidPanic));
		return *PeerPtr();
		}

	RNodeInterface& Peer()
		{
		__ASSERT_DEBUG(PeerPtr(), User::Panic(KMessagesPanic, EClientNotValidPanic));
		return *PeerPtr();
		}

	const RNodeInterface* PeerPtr() const
		{
		return iNodePeer;
		}

	RNodeInterface* PeerPtr()
		{
		return iNodePeer;
		}

private:
    RClientInterface   iClientPeer;
	RNodeInterface*    iNodePeer;
    };

///////////////////////////////////////////////////////////////////////////////
//XNodePeerId
class XNodePeerId : public TNodePeerId
    {
public:
    //The originator could be just another node (not TNodeCtxId)
    IMPORT_C explicit XNodePeerId(const TRuntimeCtxId& aPeerId, RNodeInterface* aPeer, CBase* aInfo = NULL);
	IMPORT_C void Destroy();

	CBase* iInfo; //Used to store information related to the originator
    };

} // namespace Messages

#endif // SYMBIAN_MM_NODEPEER_H


