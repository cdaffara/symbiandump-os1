// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef MMEXAMPLE1NODES_H
#define MMEXAMPLE1NODES_H

#include <e32cons.h>
#include <elements/nm_node.h>
#include <elements/mm_node.h>

class CClock : public Messages::ASimpleNodeIdBase,
			   public MeshMachine::AMMNodeBase
	{
	// has a parallel activity for timers
public:
	CClock();

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);
	
	const Messages::TNodeId& Id() const
 		{
 		return NodeId();
 		}

	};

class CTrafficLight : public Messages::ASimpleNodeIdBase,
					  public MeshMachine::AMMNodeBase
	{
public:
	enum TColour {
		ERed = 1,
		EAmber = 2,
		EGreen = 3
	};
	
	CTrafficLight(CConsoleBase* aConsole, const TDesC& aPositionText);

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);

	void SetColour(TColour aColour);
	
	const Messages::TNodeId& Id() const
 		{
 		return NodeId();
 		}

private:
	TColour iColour;
	CConsoleBase* iConsole; // for reporting
	const TDesC& iPositionText;
	};

class CController : public Messages::ASimpleNodeIdBase,
					public MeshMachine::AMMNodeBase
	{
public:
	CController();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);

	const Messages::TNodeId& Id() const
 		{
 		return NodeId();
 		}

	};

#endif
