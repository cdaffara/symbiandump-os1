// NotifierProxy.h
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
#include <e32base.h>
#include <fshell/memoryaccess.h>
#include <W32STD.H>

class CRedrawer;
class CLine;
class CPanicDialogWatcher;

NONSHARABLE_CLASS(CNotifierProxyServer) : public CServer2
	{
public:
	CNotifierProxyServer();
	void ConstructL();
	~CNotifierProxyServer();

	void InfoPrint(const TDesC& aText, TUint aClientId);
	void DoWservInitiatedRedraw(const TRect& aRect);
	RWsSession& WsSession();

protected:
	CSession2* NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const;

private:
	void SetupWindowL();
	void DisplayAndStartTimer();
	static TInt StaticTick(TAny* aSelf);
	void Tick();
	void RecalculateWindow();
	TRect BoxForLine(TInt aLineIndex);

private:
	RMemoryAccess iMemAccess;
	TBool iProxying;
	//RBuf iInfoPrintText;
	CPeriodic* iPeriodic;
	CFont* iFont;
	RPointerArray<CLine> iText;
	CPanicDialogWatcher* iUndertaker;
	//
	RWsSession iWs;
	RWindowGroup iWg;
	RWindow iWin;
	CWsScreenDevice* iScreenDevice;
	CWindowGc* iGc;
	CRedrawer* iRedrawer;
	};

class RRealNotifier : public RNotifier
	{
public:
	TInt Connect();
	};

NONSHARABLE_CLASS(CNotifierProxySession) : public CSession2
	{
public:
	void ConstructL();

protected:
	void ServiceL(const RMessage2 &aMessage);
	void Disconnect(const RMessage2 &aMessage);

private:
	CNotifierProxyServer& Server();
	const CNotifierProxyServer& Server() const;

private:
	RRealNotifier iRealNotifier;
	};

NONSHARABLE_CLASS(CAsyncWaiter) : public CActive
	{
public:
	CAsyncWaiter(RRealNotifier& aRealNotifier, const RMessage2& aMessage);
	~CAsyncWaiter();
	void ForwardMessageL();

protected:
	void RunL();
	void DoCancel();

private:
	RRealNotifier& iRealNotifier;
	const RMessage2 iMsg;
	RBuf8 iBuf;
	RBuf16 iWideBuf;
	RBuf8 iResponseBuf;
	TPtrC iTempPtrs[4];
	};

NONSHARABLE_CLASS(CRedrawer) : public CActive
	{
public:
	CRedrawer(CNotifierProxyServer& aServer);
	void Go();
	~CRedrawer();

private:
	void RunL();
	void DoCancel();

private:
	CNotifierProxyServer& iServer;
	};

NONSHARABLE_CLASS(CLine) : public CBase
	{
public:
	CLine(const TDesC& aText, TUint aClientId);

	TBuf<100> iText;
	TUint iClientId;
	//TTime iTimeOfPrint;
	};

NONSHARABLE_CLASS(CPanicDialogWatcher) : public CActive
	{
public:
	CPanicDialogWatcher(CNotifierProxyServer& aServer);
	void ConstructL();
	~CPanicDialogWatcher();
protected:
	void RunL();
	void DoCancel();

private:
	RUndertaker iUndertaker;
	CNotifierProxyServer& iServer;
	TInt iThreadHandle;
	TFullName iTempName;
	//TExitCategoryName iTempCategory;
	};
