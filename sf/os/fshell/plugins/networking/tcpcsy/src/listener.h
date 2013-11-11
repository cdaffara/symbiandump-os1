// listener.h
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

#ifndef __LISTENER_H__
#define __LISTENER_H__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>


class MListenerObserver
	{
public:
	virtual void ListenComplete(TInt aError) = 0;
	};


class CListener : public CActive
	{
public:
	static CListener* NewL(RSocketServ& aSocketServ, RSocket& aSocket, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr, MListenerObserver& aObserver);
	~CListener();
private:
	CListener(RSocket& aSocket, MListenerObserver& aObserver);
	void ConstructL(RSocketServ& aSocketServ, TUint aProtocolFamily, TUint aProtocol, const TSockAddr& aSockAddr);
private: // From CActive.
	void RunL();
	void DoCancel();
private:
	RSocket& iSocket;
	RSocket iListeningSocket;
	MListenerObserver& iObserver;
	};

#endif // __LISTENER_H__

