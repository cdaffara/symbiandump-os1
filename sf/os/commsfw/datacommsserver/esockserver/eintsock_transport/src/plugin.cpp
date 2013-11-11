// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "eintsock_transport.h"
#include "ss_eintsock_msgs.h"
#include "ss_eintsockimpl.h"

using namespace Den;
using namespace Elements;
using namespace CommsFW;

// Define the interface UIDs

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KEIntSockTransportPluginImplementationUid, CEIntSockTransport::NewL),
	};

/**
ECOM Implementation Factories
*/

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

/**
Plugin Implementation
*/

CEIntSockTransport* CEIntSockTransport::NewL(TTransportReceiverFactoryArgs* /*aArgs*/)
	{
	CEIntSockTransport* self = new(ELeave) CEIntSockTransport;
	return self;
	}

void CEIntSockTransport::ProcessMessage(const TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, TWorkerId aWorkerId)
	{
	CInternalSockSubSession::ProcessMessage(aMsg, aRequest, aStorage, aWorkerId);
	}

void CEIntSockTransport::OnPeerDeath(TWorkerId aPeer, TTransportUserStorage& aStorage)
	{
	CInternalSockSubSession::OnPeerDeath(aPeer, aStorage);
	}

void CEIntSockTransport::Shutdown(TBool aAlreadyDead, TTransportUserStorage& aStorage)
	{
	CInternalSockSubSession::Shutdown(aAlreadyDead, aStorage);
	}

