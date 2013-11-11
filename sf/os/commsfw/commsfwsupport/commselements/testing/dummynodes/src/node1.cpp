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

#include <dummystates.h>
#include <dummyactivities.h>
#include "Node1Factory.h"
#include "Node1.h"

using namespace Messages;
using namespace Dummy;

#ifdef SYMBIAN_TRACE_ENABLE
	_LIT8(KNode1Tag, "Node1");
#endif

//-=========================================================
//
// CNode1 methods
//
//-=========================================================
CNode1* CNode1::NewL(CNode1Factory& aFactory)
    {
    CNode1* provider = new (ELeave) CNode1(aFactory);
    CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop(provider);
	return provider;
    }

CNode1::CNode1(CNode1Factory& aFactory)
:	CTestNodeBase(aFactory, DummyActivities::dummyActivities::Self())
    {
    NM_LOG_NODE_CREATE(KNode1Tag, CNode1);
    }

CNode1::~CNode1()
    {
    NM_LOG_NODE_DESTROY(KNode1Tag, CNode1);
    }

void CNode1::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
	CTestNodeBase::ReceivedL(aSender, aRecipient, aMessage);
	}

TUid CNode1::ServiceProviderUid() const
	{
	//This will normally would be read from a DB..
	//In this case we hardcoded it to CNode2 UID
	return TUid::Uid(0x10285A81);
	}

