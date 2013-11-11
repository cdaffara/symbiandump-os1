// consoleproxy.h
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

#ifndef __CONSOLEPROXY_H__
#define __CONSOLEPROXY_H__

#include <e32base.h>
#include <e32cons.h>
#include <fshell/consoleextensions.h>

class CConsoleProxyServer;
class TServerParams;

typedef CConsoleBase* (*TConsoleCreateFunction)();
typedef CConsoleProxyServer* (*TServerNewL)(TAny*);

enum TConsoleProxyPanic
	{
	EConsoleAlreadyCreated = 0,
	ENoConsoleInstatiated = 1,
	};
	
void Panic(TConsoleProxyPanic aReason);

class RConsoleProxy : public RSessionBase
	{
public:
	enum TOpCode
		{
		EGetServer,
		ECreate,
		ERead,
		EReadKey,
		EReadCancel,
		EWrite,
		EGetCursorPos,
		ESetCursorPosAbs,
		ESetCursorPosRel,
		ESetCursorHeight,
		ESetTitle,
		EClearScreen,
		EClearToEndOfLine,
		EGetScreenSize,
		EGetKeyCode,
		EGetKeyModifiers,
		ESetAttributes,
		EIsConstructed,
		
		EOpCodeCustomBase = 1000,
		};
public:
	/**
	Create a new sub-thread to host a console and connect to it.

	@param aConsoleCreate Function pointer to create the console in the new thread
	@param aServer On return, the handle of the server running in the sub thread. Ownership is NOT returned
			to the caller.
	@param aServerThread On return, the handle to the thread that the server is running in. Ownership is
			returned to the caller.
	*/
	IMPORT_C TInt Connect(TConsoleCreateFunction aConsoleCreate, const TDesC& aThreadNameBase,
							TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize,
							RServer2& aServer, RThread& aServerThread);
	
	/**
	Create a new sub-thread to host a console and connect to it, using a custom CConsoleProxyServer derived
	server.
	
	@param aServerConstructor NewL of the server class
	@param aServerParams Parameters to pass into the server NewL.
	@param aServer On return, the handle of the server running in the sub thread. Ownership is NOT returned
			to the caller.
	@param aServerThread On return, the handle to the thread that the server is running in. Ownership is
			returned to the caller.
	*/
	IMPORT_C TInt Connect(TServerNewL aServerConstructor, TAny* aServerParams, const TDesC& aThreadNameBase,
							TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize,
							RServer2& aServer, RThread& aServerThread);
	
	/**
	Connects to a console hosted with the given server.
	*/
	IMPORT_C TInt Connect(RServer2 aServer);
	/**
	Connect to a console created within another process. 
	*/
	IMPORT_C TInt Connect(const TDesC& aServerName, const TDesC& aServerArgs = KNullDesC);
	
	IMPORT_C TInt Server(RServer2& aServer);
	
	IMPORT_C TInt Create(const TDesC& aConsoleTitle, TSize aSize);
	IMPORT_C void Read(TRequestStatus& aStatus);
	IMPORT_C void Read(TPckg<TKeyCode>& aKeyCode, TPckg<TUint>& aKeyModifiers, TRequestStatus& aStatus);
	IMPORT_C void ReadCancel();
	IMPORT_C TInt Write(const TDesC& aDescriptor);
	IMPORT_C TInt CursorPos(TPoint& aPos) const;
	IMPORT_C TInt SetCursorPosAbs(const TPoint& aPoint);
	IMPORT_C TInt SetCursorPosRel(const TPoint& aPoint);
	IMPORT_C TInt SetCursorHeight(TInt aPercentage);
	IMPORT_C TInt SetTitle(const TDesC& aTitle);
	IMPORT_C TInt ClearScreen();
	IMPORT_C TInt ClearToEndOfLine();
	IMPORT_C TInt GetScreenSize(TSize& aSize) const;
	IMPORT_C TInt GetKeyCode(TKeyCode& aCode) const;
	IMPORT_C TInt GetKeyModifiers(TUint& aModifiers) const;
	
	IMPORT_C void Create(const TDesC& aConsoleTitle, TSize aSize, TRequestStatus& aStatus);
	IMPORT_C void Write(const TDesC& aDescriptor, TRequestStatus& aStatus);
	IMPORT_C void CursorPos(TPckg<TPoint>& aPoint, TRequestStatus& aStatus) const;
	IMPORT_C void SetCursorPosAbs(const TPoint& aPoint, TRequestStatus& aStatus);
	IMPORT_C void SetCursorPosRel(const TPoint& aPoint, TRequestStatus& aStatus);
	IMPORT_C void SetCursorHeight(TInt aPercentage, TRequestStatus& aStatus);
	IMPORT_C void SetTitle(const TDesC& aTitle, TRequestStatus& aStatus);
	IMPORT_C void ClearScreen(TRequestStatus& aStatus);
	IMPORT_C void ClearToEndOfLine(TRequestStatus& aStatus);
	IMPORT_C void GetScreenSize(TPckg<TSize>& aSize, TRequestStatus& aStatus) const;
	IMPORT_C void SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, TRequestStatus& aStatus);
	IMPORT_C TInt IsConstructed(TBool& aLazy);
private:
	TInt DoConnect(TServerParams* aParams, const TDesC& aThreadNameBase,
			TInt aStackSize, TInt aHeapMinSize, TInt aHeapMaxSize,
			RServer2& aServer, RThread& aServerThread);
	};
	
class CConsoleProxy : public CConsoleBase
	{
public:
	/**
	Create a CConsoleBase wrapper around an RConsoleProxy.
	
	@param aProxySession A connected session. The handle will be duplicated by this class,
			so ownership is  not transferred.
	*/
	IMPORT_C static CConsoleProxy* NewL(const RConsoleProxy& aProxySession);
	IMPORT_C ~CConsoleProxy();
public: // from CConsoleBase
	IMPORT_C virtual TInt Create(const TDesC &aTitle,TSize aSize);
	IMPORT_C virtual void Read(TRequestStatus &aStatus);
	IMPORT_C virtual void ReadCancel();
	IMPORT_C virtual void Write(const TDesC &aDes);
	IMPORT_C virtual TPoint CursorPos() const;
	IMPORT_C virtual void SetCursorPosAbs(const TPoint &aPoint);
	IMPORT_C virtual void SetCursorPosRel(const TPoint &aPoint);
	IMPORT_C virtual void SetCursorHeight(TInt aPercentage);
	IMPORT_C virtual void SetTitle(const TDesC &aTitle);
	IMPORT_C virtual void ClearScreen();
	IMPORT_C virtual void ClearToEndOfLine();
	IMPORT_C virtual TSize ScreenSize() const;
	IMPORT_C virtual TKeyCode KeyCode() const;
	IMPORT_C virtual TUint KeyModifiers() const;
protected:
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	IMPORT_C CConsoleProxy();
	IMPORT_C void ConstructL(const RConsoleProxy& aProxySession);
protected:
	RConsoleProxy iConsole;
	TKeyCode iKeyCode;
	TUint iKeyModifiers;
	TPckg<TKeyCode> iKeyCodePckg;
	TPckg<TUint> iKeyModifiersPckg;
	};
	
class CConsoleProxySession;

class MProxiedConsole
	{
public:
	IMPORT_C static MProxiedConsole* DefaultL(CConsoleBase* aConsole);
public:
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual CConsoleBase* Console() = 0;
	virtual void Read(CConsoleProxySession& aSession) = 0;
	virtual void ReadCancel() = 0;
	};
	
class TConsoleProxyServerNewLParams
	{
public:
	TConsoleProxyServerNewLParams(const TDesC& aName, TInt aAoPriority, TConsoleCreateFunction aConsoleCreate = NULL)
		: iName(aName), iAoPriority(aAoPriority), iConsoleCreate(aConsoleCreate) {}
public:
	const TDesC& iName;
	TInt iAoPriority;
	TConsoleCreateFunction iConsoleCreate;
	};
	
class CShutdownTimer;

class CConsoleProxyServer : public CServer2
	{
public:
	static CConsoleProxyServer* NewL(TAny* aParams); // aParams is TConsoleProxyServerNewLParams*
	IMPORT_C static CConsoleProxyServer* NewL(const TDesC& aName, TInt aAoPriority, TConsoleCreateFunction aConsoleCreate = NULL);
	
	IMPORT_C virtual ~CConsoleProxyServer();

	void AddSession();
	void DropSession();
protected:
	IMPORT_C CConsoleProxyServer(TConsoleCreateFunction aConsoleCreate, TInt aAoPriority);
	IMPORT_C void ConstructL(const TDesC& aName);
	
	/**
	Called after a timeout when
	- no session connects to the server after startup
	- the last session disconnects
	and stops the active scheduler. Can be over-ridden to change the behaviour in these cases.
	*/
	IMPORT_C virtual void ShutdownTimerExpired();
	
	// from CServer2:
	IMPORT_C virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
protected:
	TConsoleCreateFunction iConsoleCreate;
private:
	TInt iSessionCount;
	CShutdownTimer* iShutdownTimer;
	friend class CShutdownTimer;
	};
	
class CConsoleProxySession	: public CSession2
	{
public:
	IMPORT_C CConsoleProxySession(TConsoleCreateFunction aConsoleCreate);
	IMPORT_C CConsoleProxySession(MProxiedConsole* aConsole);
	IMPORT_C virtual ~CConsoleProxySession();
	
	CConsoleProxyServer* Server() { return (CConsoleProxyServer*)CSession2::Server();}
		
	
	// from CSession2:
	IMPORT_C virtual void ServiceL(const RMessage2& aMessage);
	IMPORT_C virtual void CreateL();
	
	// callback from from MProxiedConsole:
	IMPORT_C void ReadComplete(TInt aStatus);
protected:
	/**
	Creates the console using the given title and size. The console must not already have
	been created.
	*/
	IMPORT_C void DoCreateL(const TDesC& aTitle, const TSize& aSize);
	/**
	Instantiate the CConsoleBase class, but don't call Create() on it; return it wrapped up
	inside a MProxiedConsole derived class.
	*/
	IMPORT_C virtual MProxiedConsole* InstantiateConsoleL();
	/**
	Called just after a console has been successfully Create()'d, to perform any extra
	configuration necessary. Default implementation does nothing.
	
	Note, iConsole will not have been set yet. If this leaves, it causes construction
	of the console to fail.
	*/
	IMPORT_C virtual void ConsoleCreatedL(MProxiedConsole* aConsole);
private:
	void CreateL(const RMessage2& aMessage);
protected:
	TConsoleCreateFunction iConsoleCreate;
	MProxiedConsole* iConsole;
	RMessagePtr2 iReadMessage;
	enum 
		{
		EReadBasic,
		EReadKeys,
		} iReadType;
	TKeyCode iKeyCode;
	TUint iKeyModifiers;
	};




#endif //__CONSOLEPROXY_H__
