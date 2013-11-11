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

#ifndef SS_THREADTRANSPORT_H
#define SS_THREADTRANSPORT_H

#include <comms-infras/ss_common.h>
#include <elements/cftransport.h>

namespace Den
{
	class TWorkerTransportMsg;
}

NONSHARABLE_CLASS(TRequestWrapper)
	{
public:
	IMPORT_C TRequestWrapper();
	IMPORT_C TRequestWrapper(TRequestStatus& aStatus, TBool aSyncLocalCall);
	inline void SetPeerThread(const RThread& aPeerThread);
	inline const RThread& PeerThread () const;
	inline operator TRequestStatus*();
	IMPORT_C void RequestComplete(TInt aReason);
private:
	TRequestStatus* iStatus;
	const RThread* iPeerThread;
	TBool iSyncLocalCall;
	};

NONSHARABLE_CLASS(TTransportUserStorage)
	{
public:
	inline TTransportUserStorage();
	inline TAny* Ptr() const;
	inline TAny*& RefPtr();
private:
	TAny* iStorage;
	};

class CWorkerTransportReceiver;
NONSHARABLE_CLASS(TTransportUser)
	{
public:
	inline TTransportUser();
	static TInt Compare(const TTransportUser& aLHS, const TTransportUser& aRHS);
public:
	TInt iInterfaceId;
	TUid iDtorId;
	CWorkerTransportReceiver* iUser;
	TTransportUserStorage* iUserStorage;
	};


class TTransportReceiverFactoryArgs
	{
public:
	inline TTransportReceiverFactoryArgs(TTransportUserStorage* aStorage, CommsFW::TWorkerId aWorkerId);
public:
	TTransportUserStorage* iStorage;
	CommsFW::TWorkerId iWorkerId;
	};


// Receiving side of transport
class CWorkerTransportReceiver : public CBase
	{
public:
	virtual void ProcessMessage(const Den::TWorkerTransportMsg& aMsg, TRequestWrapper& aRequest, TTransportUserStorage& aStorage, CommsFW::TWorkerId aWorker) = 0;
	virtual void OnPeerDeath(CommsFW::TWorkerId aPeer, TTransportUserStorage& aStorage) = 0;
	virtual void Shutdown(TBool aAlreadyDead, TTransportUserStorage& aStorage) = 0;
	};

#include "comms-infras/ss_threadtransport.inl"

#endif

