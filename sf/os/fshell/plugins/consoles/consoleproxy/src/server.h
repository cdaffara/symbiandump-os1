// server.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __SERVER_H__
#define __SERVER_H__

static const TInt KServerShutdownTimer = 500000;

TInt ServerThreadFunction(TAny* aArgs);

class TServerParams
	{
public:
	TServerNewL iServerNewL;
	TAny* iServerParams;
	RServer2 iServer;
	};
	
NONSHARABLE_CLASS(CConsoleWrapper)	: public CActive
									, public MProxiedConsole
	{
public:
	static CConsoleWrapper* NewL(TConsoleCreateFunction aConsoleCreate);
	static CConsoleWrapper* NewL(CConsoleBase* aConsole);
	~CConsoleWrapper();
private:
	// from MProxiedConsole:
	virtual void Open();
	virtual void Close();
	virtual CConsoleBase* Console();
	virtual void Read(CConsoleProxySession& aSession);
	virtual void ReadCancel();
	// from CActive:
	virtual void RunL();
	virtual void DoCancel();
private:
	void ConstructL(TConsoleCreateFunction aConsoleCreate);
	CConsoleWrapper();
private:
	TInt iRefCount;
	CConsoleBase* iConsole;
	CConsoleProxySession* iReader;
	};
	
NONSHARABLE_CLASS(CShutdownTimer) : public CTimer
	{
public:
	static CShutdownTimer* NewL(CConsoleProxyServer& aServer);
	void Start();
private:
	CShutdownTimer(CConsoleProxyServer& aServer);
	virtual void RunL();
private:
	CConsoleProxyServer& iServer;
	};



#endif //__SERVER_H__
