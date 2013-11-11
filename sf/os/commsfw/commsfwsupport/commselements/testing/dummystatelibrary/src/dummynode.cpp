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

#include "dummymessages.h"
#include "dummynode.h"
#include "dummystates.h"
#include "dummyactivities.h"

using namespace Dummy;
using namespace Messages;
using namespace MeshMachine;


EXPORT_C CTestNodeBase::CTestNodeBase(Factories::CFactoryBase& aFactory,
                                      const MeshMachine::TNodeActivityMap& aActivityMap)
:	AFactoryObject(aFactory),
	AMMNodeIdBase(aActivityMap)
	{
	}

EXPORT_C CTestNodeBase::~CTestNodeBase()
	{
	}

EXPORT_C void CTestNodeBase::Received(TNodeContextBase& aContext)
    {
    TNodeSignal::TMessageId noPeerIds[] = {
        TBindProtocol::TClientJoiningRequest::Id(),
		TEBase::TError::Id(),
        TNodeSignal::TMessageId()
        };
    AMMNodeBase::Received(noPeerIds, aContext);
	AMMNodeBase::PostReceived(aContext);
	}

EXPORT_C void CTestNodeBase::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	TNodeContext<CTestNodeBase> ctx(*this, aMessage, aSender, aRecipient);
    CTestNodeBase::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}


