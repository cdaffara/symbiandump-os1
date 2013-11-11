// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CSWSWAPBOUNDCLPUSHSERVICE_H__
#define __CSWSWAPBOUNDCLPUSHSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "WapMessageApiAgent.h"



class CSWSWapBoundCLPushService : public CWapBoundCLPushService
/** 
The WAP Messaging API, SWS WAP Stack binding.  Implementation of the bound CLPush API.
@internalComponent
@released
@since v8.0
*/ 
{
public:
	static CSWSWapBoundCLPushService* NewL();
	virtual ~CSWSWapBoundCLPushService();

public:
	virtual TInt Connect(Wap::TBearer aBearer,
					  Wap::TPort aPort,
					  TBool aSecure,
					  TInetAddr aInetAddr);
	virtual TInt Connect(Wap::TBearer aBearer,
					  Wap::TPort aPort,
					  TBool aSecure);
	virtual TInt AwaitPush(TDes8& aPushHeaders,
						  TDes8& aPushBody,
						  TPckgBuf<TUint8>& aPushIdPckg,
						  TRequestStatus& aReqStatus);
	virtual void CancelAwaitPush();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);
	virtual TInt GetBearer(Wap::TBearer& aBearer);
	virtual TInt GetServerAddress(HBufC8*& aRemoteHost);

private:
	CSWSWapBoundCLPushService();
	void ConstructL();

private:
	/**
	the agent instance which actually handles wsp PDU.
	*/
	CWspMessageApiAgent* iAgent;
	/**
	the wsp status in received wsp PDU. Not used in push.
	*/
	TWSPStatus iWspStatus;
};

#endif // __CSWSWAPBOUNDCLPUSHSERVICE_H__
