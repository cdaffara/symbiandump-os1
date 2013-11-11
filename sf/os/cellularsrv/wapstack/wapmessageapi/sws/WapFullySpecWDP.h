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

#ifndef __CSWSWAPFULLYSPECWDPSERVICE_H__
#define __CSWSWAPFULLYSPECWDPSERVICE_H__

// Include definitions of ECOM interfaces for the WAP Messaging API
#include <wapmessage.h>
#include "WapMessageApiAgent.h"

class CSWSWapFullySpecWDPService : public CWapFullySpecDatagramService
/** 
The WAP Messaging API, SWS WAP Stack binding.  Implementation of the Fully-Specified WDP API.
@internalComponent
@released
@since v8.0
*/
{
public:
	static CSWSWapFullySpecWDPService* NewL();
	virtual ~CSWSWapFullySpecWDPService();

public:
	virtual TInt Connect(const TDesC8& aRemoteHost,
						 Wap::TPort aRemotePort,
						 Wap::TBearer aBearer,
						 TInetAddr aInetAddr);
	virtual TInt Connect(const TDesC8& aRemoteHost,
						 Wap::TPort aRemotePort,
						 Wap::TBearer aBearer);
	virtual TInt Send(const TDesC8& aBuffer);
	virtual TInt AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus);
	virtual TInt Recv(TDes8& aBuffer,
					  TBool& aTruncated,
					  TRequestStatus& aReqStatus,
					  TUint32 aTimeout);
	virtual void CancelRecv();
	virtual TInt GetLocalPort(Wap::TPort& aPort);
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost);
	virtual TInt GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize);

private:
	CSWSWapFullySpecWDPService();
	void ConstructL();

private:
	/**
	the agent instance which actually handles wdp PDU.
	*/
	CWdpMessageApiAgent* iAgent;
	/**
	the remote host name.
	*/
	TBuf8<KMaxSockAddrSize> iHostName;
	/**
	the remote port
	*/
	Wap::TPort iPort;
};

#endif // __CSWSWAPFULLYSPECWDPSERVICE_H__
