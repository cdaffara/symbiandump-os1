// connector.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>


class MConnectorObserver
	{
public:
	virtual void ConnectionComplete(TInt aError) = 0;
	};


class CConnector : public CActive
	{
public:
	static CConnector* NewL(RSocketServ& aSocketServ, RSocket& aSocket, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr, MConnectorObserver& aObserver);
	~CConnector();
private:
	CConnector(RSocket& aSocket, MConnectorObserver& aObserver);
	void ConstructL(RSocketServ& aSocketServ, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr);
private: // From CActive.
	void RunL();
	void DoCancel();
private:
	RSocket& iSocket;
	MConnectorObserver& iObserver;
	};

#endif // __CONNECTOR_H__

