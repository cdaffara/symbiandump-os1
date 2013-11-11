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

#ifndef __CSWSWAPBOUNDWDPSERVICE_H__
#define __CSWSWAPBOUNDWDPSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "WapMessageApiAgent.h"

class CSWSWapBoundWDPService : public CWapBoundDatagramService
/** 
The WAP Messaging API, SWS WAP Stack binding. Implementation of the bound WDP API.
@internalComponent
@released
@since v8.0
*/
{
public:
	static CSWSWapBoundWDPService* NewL();	
	virtual ~CSWSWapBoundWDPService();

public:
	virtual TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TInetAddr aInetAddr);
	virtual TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort);
	virtual TInt SendTo(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, const TDesC8& aBuffer,Wap::TBearer aBearer);
	virtual TInt AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus);
	virtual TInt RecvFrom(TDes8& aRemoteHost, Wap::TPort& aRemotePort, TDes8& aBuffer, TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout);
	virtual void CancelRecv();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);
	virtual TInt GetBearer(Wap::TBearer& aBearer);
	virtual TInt GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize);

private:
	CSWSWapBoundWDPService();
	void ConstructL();

private:
	/**
	the agent instance which actually handles wdp PDU.
	*/
	CWdpMessageApiAgent* iAgent;
};


#endif // __CSWSWAPBOUNDWDPSERVICE_H__
