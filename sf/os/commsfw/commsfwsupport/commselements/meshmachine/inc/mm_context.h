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

#ifndef SYMBIAN_MM_CONTEXT_H
#define SYMBIAN_MM_CONTEXT_H

#include <elements/nm_messages_base.h>

namespace MeshMachine
{
class CNodeActivityBase;
class AMMNodeBase;

//-=========================================================
//
//TNodeContextBase
//
//-=========================================================

class TNodeContextBase
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C explicit TNodeContextBase(AMMNodeBase& aNode, Messages::TSignatureBase& aMessage, const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, CNodeActivityBase* aNodeActivity = NULL);
	IMPORT_C ~TNodeContextBase();

	//Utility fns
	IMPORT_C void PostToSender(const Messages::TSignalBase& aMessage) const;
    IMPORT_C TUint16 ActivityId() const;
    IMPORT_C const Messages::TNodeId& NodeId() const; //Only the node id part
	IMPORT_C TBool IsPeer() const;
	IMPORT_C TBool IsSelf() const;

	AMMNodeBase& Node() {return iNode;}
	CNodeActivityBase* Activity() {return iNodeActivity;}

	CNodeActivityBase* iNodeActivity;
	Messages::TSignatureBase& iMessage;
	AMMNodeBase& iNode;
	TInt iReturn; //return values from transition in case transition doesn't process errors
	Messages::RNodeInterface* iPeer; //iPeer can point to iTemporaryPeer or to a valid peer itf
	const Messages::TRuntimeCtxId& iSender; //Reference to an opaque sender's address (from this node's point of view)
	const Messages::TNodeId& iRecipient;

	};

//-=========================================================
//
//TNodeContext
//
//-=========================================================

template <class TNODE, class TBaseContext = TNodeContextBase, class ACTIVITY = CNodeActivityBase>
class TNodeContext : public TBaseContext
/**
@publishedPartner
@released
*/
	{
public:
	TNodeContext(TNODE& aNode, Messages::TSignatureBase& aMessageSig, const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, CNodeActivityBase* aNodeActivity = NULL)
	:	TBaseContext(aNode, aMessageSig, aSender, aRecipient, aNodeActivity)
		{
		}

	TNODE& Node() {return static_cast<TNODE&>(TBaseContext::Node());};
	const TNODE& Node() const {return static_cast<const TNODE&>(TBaseContext::Node());};
	ACTIVITY* Activity() { return static_cast<ACTIVITY*>(TBaseContext::Activity()); }
	const ACTIVITY* Activity() const { return static_cast<const ACTIVITY*>(TBaseContext::Activity()); }
	};

} //namespace MeshMachine

#endif
	//SYMBIAN_MM_CONTEXT_H


