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
 @internalTechnology
*/

#ifndef EINTSOCK_TRANSPORT_H
#define EINTSOCK_TRANSPORT_H

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/ss_threadtransport.h>

class CEIntSockTransport : public CWorkerTransportReceiver
	{
public:
	static CEIntSockTransport* NewL(TTransportReceiverFactoryArgs* aArgs);

	virtual void ProcessMessage(const Den::TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, CommsFW::TWorkerId aWorkerId);
	virtual void OnPeerDeath(CommsFW::TWorkerId aPeer, TTransportUserStorage& aStorage);
	virtual void Shutdown(TBool aAlreadyDead, TTransportUserStorage& aStorage);
	};


#endif	// EINTSOCK_TRANSPORT_H

