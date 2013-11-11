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

#ifndef __CSWSWAPFULLYSPECCLWSPSERVICE_H__
#define __CSWSWAPFULLYSPECCLWSPSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "WapMessageApiAgent.h"

class CSWSWapFullySpecCLWSPService : public CWapFullySpecCLWSPService
/** 
The WAP Messaging API, SWS WAP Stack binding. Implementation of Fully-Specified Connectionless WSP API
@internalComponent
@released
@since v8.0
*/
{
public:
	static CSWSWapFullySpecCLWSPService* NewL();
	virtual ~CSWSWapFullySpecCLWSPService();
public:
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure);
	virtual TInt MethodInvoke(TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId);
	virtual TInt MethodResult(TDes8& aReqHeaders, TDes8& aReqBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout);
	virtual void CancelReq();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);
protected:
	CSWSWapFullySpecCLWSPService();
	void ConstructL();
private:
	/**
	the agent instance which actually handles wsp PDU.
	*/
	CWspMessageApiAgent* iAgent;
};

#endif //__CSWSWAPFULLYSPECCLWSPSERVICE_H__ 
