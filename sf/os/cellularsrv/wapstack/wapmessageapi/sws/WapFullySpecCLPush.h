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

#ifndef __CSWSWAPFULLYSPECCLPUSHSERVICE_H__
#define __CSWSWAPFULLYSPECCLPUSHSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "WapMessageApiAgent.h"

class CSWSWapFullySpecCLPushService : public CWapFullySpecCLPushService
/** 
The WAP Messaging API, SWS WAP Stack binding. Implementation of the fully-specified connectionless push API.
@internalComponent
@released
@since v8.0
*/
{
public:
	static CSWSWapFullySpecCLPushService* NewL();
	virtual ~CSWSWapFullySpecCLPushService();

public:
	virtual TInt Connect(const TDesC8& aRemoteHost,
					  Wap::TPort aRemotePort,
					  Wap::TBearer aBearer,
					  TBool aSecure,
					  TInetAddr aInetAddr);
	virtual TInt Connect(const TDesC8& aRemoteHost,
					  Wap::TPort aRemotePort,
					  Wap::TBearer aBearer,
					  TBool aSecure);
	virtual TInt AwaitPush(TDes8& aPushHeaders,
						  TDes8& aPushBody,
						  TPckgBuf<TUint8>& aPushIdPckg,
						  TRequestStatus& aReqStatus);
	virtual void CancelAwaitPush();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);

private:
	CSWSWapFullySpecCLPushService();
	void ConstructL();

private:
	/**
	the agent instance which actually handles wsp PDU.
	*/
	CWspMessageApiAgent* iAgent;
	/**
	the wsp status in received wsp PDU.
	*/
	TWSPStatus iWspStatus;
};


#endif // __CSWSWAPFULLYSPECCLPUSHSERVICE_H__
