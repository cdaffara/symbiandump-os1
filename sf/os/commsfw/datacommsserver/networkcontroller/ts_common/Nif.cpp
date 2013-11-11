// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nifman.h"
#include "CStubbedAgent.h"
#include "NetConLog.h"

CNifAgentBase* Nif::CreateAgentL(const TDesC& aName, const TBool aNewInstance /* = EFalse */)
//
//  Create a new agent
//
//  Be careful when using this function!!!  The new agent is pushed onto the cleanup stack and it
//  is up to the test case to call either Pop() or PopAndDestroy().
//
//  See CNetConTest0501::CleanupAgent() and CNetConSelectionRequestTestBase::AddAgentToNetworkL()
//  for examples of how to do this.
//
	{

	ASSERT(aNewInstance);
	(void)aNewInstance;
	
	_LIT(KAgentName, "StubbedAgent");

	if(aName != KAgentName)
		User::Leave(KErrNotFound);

	CNifAgentBase* agent = new(ELeave) CStubbedAgent();

	// add the agent to the cleanup stack
	CleanupStack::PushL(agent);

	LOG( NetConLog::Printf(_L("Nif::CreateAgentL() returning a new CStubbedAgent [0x%08x]"), agent); )

	return agent;
	}

