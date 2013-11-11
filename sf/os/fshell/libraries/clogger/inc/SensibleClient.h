// SensibleClient.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef SENSIBLECLIENT_H
#define SENSIBLECLIENT_H

#include <e32std.h>

class CServerCallbackDispatcher;
class TServerCallback;
class TCallbackParser;

/* This class is designed to be usable in a handle-body implementation. Ie you can hide the use of RSensibleSessionBody 
 * from your clients by declaring a TUint32 iPimpl[3] (or somesuch) in your client class, and constructing a subclass of
 * RSensibleSessionBody with appropriate definitions of DoCreateSession and DoSendReceive that call into the real session
 * object.
 *
 * You can also derive directly from RSensibleSessionBody if you're not fussed about hiding the implementation.
 */

NONSHARABLE_CLASS(RSensibleSessionBody)
	{
	friend class CServerCallbackDispatcher;
public:
	RSensibleSessionBody();
	
protected:
	void Close();
	TInt Connect(TInt aMessageSlots, TBool aStartCallbackDispatcher);
	TInt StartCallbackDispatcher(); // If not already started
	void StopCallbackDispatcher();
	virtual void DispatchCallbackL(TServerCallback& aCallback, TCallbackParser& aParser);
	virtual void ServerDiedL();

	// These have to be implemented by a derived class to call through to the session proper
	virtual TInt DoCreateSession(const TDesC &aServer, const TVersion &aVersion, TInt aAsyncMessageSlots)=0;
	virtual TInt DoSendReceive(TInt aFunction, const TIpcArgs &aArgs) const=0;
	virtual void DoSendReceive(TInt aFunction, const TIpcArgs &aArgs, TRequestStatus &aStatus) const=0;

protected:
	CServerCallbackDispatcher* iDispatcher;
	};

#endif
