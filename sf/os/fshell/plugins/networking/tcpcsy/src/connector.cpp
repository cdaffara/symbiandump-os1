// connector.cpp
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

#include "connector.h"

CConnector* CConnector::NewL(RSocketServ& aSocketServ, RSocket& aSocket, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr, MConnectorObserver& aObserver)
	{
	CConnector* self = new(ELeave) CConnector(aSocket, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSocketServ, aProtocolFamily, aProtocol, aSockAddr);
	CleanupStack::Pop(self);
	return self;
	}

CConnector::~CConnector()
	{
	Cancel();
	}

CConnector::CConnector(RSocket& aSocket, MConnectorObserver& aObserver)
	: CActive(CActive::EPriorityStandard), iSocket(aSocket), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CConnector::ConstructL(RSocketServ& aSocketServ, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr)
	{
	User::LeaveIfError(iSocket.Open(aSocketServ, aProtocolFamily, KSockStream, aProtocol));
	iSocket.Connect(const_cast<TSockAddr&>(aSockAddr), iStatus);
	SetActive();
	}

void CConnector::RunL()
	{
	iObserver.ConnectionComplete(iStatus.Int());
	}

void CConnector::DoCancel()
	{
	iSocket.CancelConnect();
	}

