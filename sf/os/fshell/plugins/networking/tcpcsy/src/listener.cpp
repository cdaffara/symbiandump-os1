// listener.cpp
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

#include "Listener.h"

CListener* CListener::NewL(RSocketServ& aSocketServ, RSocket& aSocket, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr, MListenerObserver& aObserver)
	{
	CListener* self = new(ELeave) CListener(aSocket, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSocketServ, aProtocolFamily, aProtocol, aSockAddr);
	CleanupStack::Pop(self);
	return self;
	}

CListener::~CListener()
	{
	Cancel();
	iListeningSocket.Close();
	}

CListener::CListener(RSocket& aSocket, MListenerObserver& aObserver)
	: CActive(CActive::EPriorityStandard), iSocket(aSocket), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CListener::ConstructL(RSocketServ& aSocketServ, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr)
	{
	User::LeaveIfError(iSocket.Open(aSocketServ));
	User::LeaveIfError(iListeningSocket.Open(aSocketServ, aProtocolFamily, KSockStream, aProtocol));
	User::LeaveIfError(iListeningSocket.Bind(const_cast<TSockAddr&>(aSockAddr)));
	User::LeaveIfError(iListeningSocket.Listen(1));
	iListeningSocket.Accept(iSocket, iStatus);
	SetActive();
	}

void CListener::RunL()
	{
	iObserver.ListenComplete(iStatus.Int());
	}

void CListener::DoCancel()
	{
	iSocket.CancelAccept();
	}

