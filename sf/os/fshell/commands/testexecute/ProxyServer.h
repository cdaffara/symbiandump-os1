// ProxyServer.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <fshell/memoryaccess.h>
#include <e32base.h>

class CProxySession;

class MMessageHandler
	{
public:
	virtual TBool HandleMessageL(CProxySession* aSession, const RMessage2& aMessage)=0;
	};

NONSHARABLE_CLASS(CProxyServer) : public CServer2
	{
public:
	static CProxyServer* NewInSeparateThreadL(const TDesC& aServerToReplace, MMessageHandler* aHandler=NULL);
	void Destroy(); // This should be used instead of delete - the server object has to be destroyed from the thread in which it was started
	MMessageHandler* Handler() const;

protected:
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
	~CProxyServer();
	CProxyServer(const TDesC& aServerToReplace, MMessageHandler* aHandler);
	void ConstructL();
	void ThreadConstructL();
	static TInt StartServerThreadFunction(TAny* aSelf);
	void ServerThreadRunL();
	static TInt Shutdown(TAny* aSelf);

private:
	RMemoryAccess iMemAccess;
	TBool iProxying;
	TName iServerName;
	TName iRealServerName;
	RThread iServerThread;
	MMessageHandler* iHandler;
	CAsyncCallBack* iShutdownCallback;
	};

class RUnderlyingSession : public RSessionBase
	{
public:
	friend class CProxySession;
	friend class CAsyncWaiter;
	};

NONSHARABLE_CLASS(CProxySession) : public CSession2
	{
public:
	void ConstructL(const TDesC& aServerName, const TVersion& aVersion);
	void ForwardUnhandledMessageL(const RMessage2& aMessage);
	void ForwardMessageArgsL(const RMessage2& aMessage, const TIpcArgs& aArgs);

protected:
	void ServiceL(const RMessage2 &aMessage);
	void Disconnect(const RMessage2 &aMessage);

private:
	CProxyServer& Server();
	const CProxyServer& Server() const;

private:
	RUnderlyingSession iSession;
	};


#endif
