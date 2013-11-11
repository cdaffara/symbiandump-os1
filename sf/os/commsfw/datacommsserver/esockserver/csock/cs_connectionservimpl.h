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

/**
 @file
 @internalComponent
 @released since 399
*/

#ifndef CS_CONNECTION_SERV_IMPL_H
#define CS_CONNECTION_SERV_IMPL_H

#include <e32std.h>
#include <es_sock.h>
#include <comms-infras/es_connectionserv.h>
#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>

namespace ConnectionServ
{

const TUint KInitialBufferSize = 1024; // bytes

class CConnectionServImpl;

class CAccessPointStatusRequest : public CActive
/**
 Handles the 2 RPC calls required by the Access Point status call.
*/
	{
protected:
	// constructor
	CAccessPointStatusRequest(CConnectionServImpl& aImpl);

public:
	~CAccessPointStatusRequest();

	static CAccessPointStatusRequest* NewL(CConnectionServImpl& aImpl);
	void ConstructL();
	void Start(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	void CompleteClientRequest(TInt aError);


private:
	CConnectionServImpl& iConnectionServImpl;

	TInt iSentRequest;

	RBuf8 iQueryBuffer;
	TInt iResultBufferSize;
	RBuf8 iResultBuffer;

	// pointer to bundle to populate (created by caller)
	CConnectionServParameterBundle* iResult;

	TRequestStatus* iClientRequest;
	};

class CAccessPointNotificationRequest : public CActive
/**
 Handles the 1 or 2 RPC calls required by each access point notification.
*/
	{
protected:
	// constructor
	CAccessPointNotificationRequest(CConnectionServImpl& aImpl);

public:
	static CAccessPointNotificationRequest* NewL(CConnectionServImpl& aImpl);
	void ConstructL();
	void Start(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CConnectionServImpl& iConnectionServImpl;
	
	TInt iSentRequest;

	RBuf8 iQueryBuffer;
	TInt iResultBufferSize;
	RBuf8 iResultBuffer;

	MAccessPointNotificationObserver* iObserver;
	};




class CConnectionServImpl : public CBase
/**
 Proxy class for all the interesting functions which need delegate active objects to work asynchronously
*/
	{
protected:
	// constructor
	CConnectionServImpl(RConnectionServ& aCS);

public:
	// destructor
	~CConnectionServImpl();

	static CConnectionServImpl* NewL(RConnectionServ& aCS);

	void AccessPointStatusQuery(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus);
	void CancelAccessPointStatusQuery();
	void AccessPointNotification(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver);
	void CancelAccessPointNotification();

	inline TInt Send(TInt aFunction,const TIpcArgs& aArgs) const;
	inline void SendReceive(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus) const;
	inline TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) const;
	inline TInt Send(TInt aFunction) const;
	inline void SendReceive(TInt aFunction,TRequestStatus& aStatus) const;
	inline TInt SendReceive(TInt aFunction) const;

	static TInt EnsureBufferIsBigEnough(RBuf8& aBuf, TUint aLen);

private:
	RConnectionServ& iConnectionServ;

	CAccessPointStatusRequest* iAccessPointStatusRequest;
	CAccessPointNotificationRequest* iAccessPointNotificationRequest;
	};

} // namespace ConnectionServ


#include "cs_connectionservimpl.inl"

#endif // ES_CONNECTION_SERV_IMPL_H

