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

#ifndef __CSWSWAPBOUNDCLWSPSERVICE_H__
#define __CSWSWAPBOUNDCLWSPSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "ActiveSocket.h"
#include "WapMessageApiAgent.h"

class CSWSWapBoundCLWSPService : public CWapBoundCLWSPService
/** 
The WAP Messaging API, SWS WAP Stack binding. Implementation of the bound Connectionless WSP API.
@internalComponent
@released
@since v8.0
*/
{
public:
	static CSWSWapBoundCLWSPService* NewL();
	virtual ~CSWSWapBoundCLWSPService();

public:

	virtual TInt Connect(Wap::TBearer aBearer,Wap::TPort aPort, TBool aSecure);
	virtual TInt MethodInvoke(Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, const TUint8 aTransactionId);
	virtual TInt MethodResult(TDes8& aReqHeaders, TDes8& aReqBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout);
	virtual void CancelReq();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);
	virtual TInt GetBearer(Wap::TBearer& aBearer);
	virtual TInt GetServerAddress(HBufC8*& aServerAddress);
private:
	CSWSWapBoundCLWSPService();
	void ConstructL();


private:
	/**
	the agent instance which actually handles wsp PDU.
	*/
	CWspMessageApiAgent* iAgent;

};

#endif // __CSWSWAPBOUNDCLWSPSERVICE_H__
