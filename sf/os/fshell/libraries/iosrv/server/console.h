// console.h
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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <e32base.h>
#include <fshell/consoleproxy.h>
#include "endpoint.h"
#include "readwrite.h"

class CLazyConsole;

class RIoConsoleProxy : public RConsoleProxy
	{
public:
	enum TOpCode
		{
		ESetConsoleSizeDetect = RConsoleProxy::EOpCodeCustomBase,
		ESetLazyConstruct,
		ESetConsoleMode,
		ESetUnderlyingConsole,
		EOpenExistingConsole,
		EWriteStdErr,
		};
public:
	TInt SetConsoleSizeDetect();
	TInt SetLazyConstruct();
	void SetConsoleMode(RIoReadWriteHandle::TMode aMode, TRequestStatus& aStatus);
	void SetUnderlyingConsole(const RIoConsoleProxy& aUnderlyingSession, TRequestStatus& aStatus);
	TInt OpenExisting();
	void WriteStdErr(const TDesC& aDescriptor, TRequestStatus& aStatus);
	};
	
class CIoConsole : public CIoEndPoint
	{
public:
	static CIoConsole* NewLC(const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, const TIoConfig& aConfig, CIoConsole* aUnderlying, TUint aOptions);
	~CIoConsole();
	const TDesC& Implementation() const;
public: // From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
public: // From CIoEndPoint.
	virtual void HandleReaderDetached(MIoReader& aReader);
	virtual void HandleWriterDetached(MIoWriter& aWriter);
public: // From MIoReadEndPoint.
	virtual void IorepReadL(MIoReader& aReader);
	virtual void IorepReadKeyL(MIoReader& aReader);
	virtual void IorepSetConsoleModeL(RIoReadWriteHandle::TMode aMode, MIoReader& aReader);
public: // From MIoWriteEndPoint.
	virtual void IowepWriteL(MIoWriter& aWriter);
	virtual void IowepWriteCancel(MIoWriter& aWriter);
	virtual void IowepCursorPosL(MIoWriter& aWriter) const;
	virtual void IowepSetCursorPosAbsL(const TPoint& aPoint, MIoWriter& aWriter);
	virtual void IowepSetCursorPosRelL(const TPoint& aPoint, MIoWriter& aWriter);
	virtual void IowepSetCursorHeightL(TInt aPercentage, MIoWriter& aWriter);
	virtual void IowepSetTitleL(MIoWriter& aWriter);
	virtual void IowepClearScreenL(MIoWriter& aWriter);
	virtual void IowepClearToEndOfLineL(MIoWriter& aWriter);
	virtual void IowepScreenSizeL(MIoWriter& aWriter) const;
	virtual void IowepSetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriter& aWriter);
protected:
	CIoConsole(const TIoConfig& aConfig);
private:
	class TConsoleRequest;
private:
	void ConstructL(const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, CIoConsole* aUnderlying, TUint aOptions);
	void ReadComplete(TInt aError);
	void ReadComplete(TUint aKeyCode, TUint aModifiers);
	void QueueReaderIfRequired();
	void CreateComplete(TInt aError);
	void NewRequest(TConsoleRequest* aRequest) const;
	void CheckQueue() const;
	void ConsoleDied();
	template <class T> void HandleReadWriterDetached(T& aReadWriter);
private:
	class CConsoleReader : public CActive
		{
	public:
		static CConsoleReader* NewL(CIoConsole& aConsole);
		~CConsoleReader();
		void QueueRead();
	private:
		CConsoleReader(CIoConsole& aConsole);
	private: // From CActive.
		virtual void RunL();
		virtual void DoCancel();
	private:
		CIoConsole& iConsole;
		TKeyCode iKeyCode;
		TUint iKeyModifiers;
		TPckg<TKeyCode> iKeyCodePckg;
		TPckg<TUint> iKeyModifiersPckg;
		};
		
	class TConsoleRequest
		{
	public:
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus) = 0;
		virtual void PrepareL();
		virtual void CompleteD(TInt aError) = 0;
		virtual TBool OriginatedFrom(MIoReader& aReader) const;
		virtual TBool OriginatedFrom(MIoWriter& aWriter) const;
		};
	
	class TConsoleWriterRequest : public TConsoleRequest
		{
	protected:
		TConsoleWriterRequest(MIoWriter& aWriter);
	private: // From TConsoleRequest.
		virtual TBool OriginatedFrom(MIoWriter& aWriter) const;
	protected:
		MIoWriter& iWriter;
		};
	
	class TConsoleReaderRequest : public TConsoleRequest
		{
	protected:
		TConsoleReaderRequest(MIoReader& aReader);
	private: // From TConsoleRequest.
		virtual TBool OriginatedFrom(MIoReader& aReader) const;
	protected:
		MIoReader& iReader;
		};
	
	class TConsoleCreateRequest : public TConsoleRequest
		{
	public:
		TConsoleCreateRequest(CIoConsole& aOwner);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		CIoConsole& iOwner;
		};
	
	class TConsoleWriteRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleWriteRequest(MIoWriter& aWriter);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void PrepareL();
		virtual void CompleteD(TInt aError);
	private:
		HBufC* iBuf;
		};
	
	class TConsoleCursorPosRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleCursorPosRequest(MIoWriter& aWriter);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		TPoint iPos;
		TPckg<TPoint> iPosPckg;
		};
		
	class TConsoleSetCursorPosAbsRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleSetCursorPosAbsRequest(MIoWriter& aWriter, const TPoint& aPoint);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		TPoint iPoint;
		};
		
	class TConsoleSetCursorPosRelRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleSetCursorPosRelRequest(MIoWriter& aWriter, const TPoint& aPoint);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		TPoint iPoint;
		};
	
	class TConsoleSetCursorHeightRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleSetCursorHeightRequest(MIoWriter& aWriter, TInt aHeight);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		TInt iHeight;
		};
		
	class TConsoleSetTitleRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleSetTitleRequest(MIoWriter& aWriter);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void PrepareL();
		virtual void CompleteD(TInt aError);
	private:
		HBufC* iTitle;
		};
		
	class TConsoleClearScreenRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleClearScreenRequest(MIoWriter& aWriter);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
		};
	
	class TConsoleClearToEndOfLineRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleClearToEndOfLineRequest(MIoWriter& aWriter);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
		};
		
	class TConsoleScreenSizeRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleScreenSizeRequest(MIoWriter& aWriter, const TIoConfig& aConfig);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		const TIoConfig& iConfig;
		TSize iSize;
		TPckg<TSize> iSizeBuf;
		};
		
	class TConsoleSetAttributesRequest : public TConsoleWriterRequest
		{
	public:
		TConsoleSetAttributesRequest(MIoWriter& aWriter, TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		TUint iAttributes;
		ConsoleAttributes::TColor iForegroundColor;
		ConsoleAttributes::TColor iBackgroundColor;
		};
		
	class TConsoleSetUnderlyingRequest : public TConsoleRequest
		{
	public:
		TConsoleSetUnderlyingRequest(CIoConsole& aUnderlyingConsole);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void PrepareL();
		virtual void CompleteD(TInt aError);
	private:
		CIoConsole& iConsole;
		RIoConsoleProxy iSession;
		};
	friend class TConsoleSetUnderlyingRequest;
	
	class TConsoleSetModeRequest : public TConsoleReaderRequest
		{
	public:
		TConsoleSetModeRequest(MIoReader& aReader, CIoConsole& aConsole, RIoReadWriteHandle::TMode aMode);
		virtual void Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus);
		virtual void CompleteD(TInt aError);
	private:
		CIoConsole& iConsole;
		RIoReadWriteHandle::TMode iMode;
		};
	friend class TConsoleSetModeRequest;
		
	class CConsoleRequest : public CActive
		{
	public:
		CConsoleRequest(CIoConsole& aConsole);
		
		void Service(TConsoleRequest* aRequest);
		~CConsoleRequest();
		const TConsoleRequest* CurrentRequest() const;
		void Abort();
	private:
		void Complete(TConsoleRequest* aRequest, TInt aError);
	private: // from CActive
		virtual void RunL();
		virtual void DoCancel();
	private:
		CIoConsole& iConsole;
		TConsoleRequest* iCurrentRequest;
		};
		
	class CServerDeathWatcher : public CActive
		{
	public:
		CServerDeathWatcher(RServer2& aServer, RThread& aThread);
		~CServerDeathWatcher();
	private:
		virtual void RunL();
		virtual void DoCancel();
	private:
		RServer2& iServer;
		RThread& iThread;
		};
	friend class TConsoleCreateRequest;
	friend class CConsoleRequest;
	friend class CConsoleReader;
	friend class TConsoleDetectSizeRequest;
protected:
	const TIoConfig& iConfig;
	RIoConsoleProxy iConsole;
	CConsoleReader* iReader;
	mutable TSize iDetectedSize;
	mutable TBool iSizeDetected;
	HBufC* iImplementation;
	TSize iCreationSize;
	HBufC* iCreationTitle;
	TInt iCreateStatus;
	mutable RArray<TConsoleRequest*> iRequestQueue;
	CConsoleRequest* iRequestAo;
	RServer2 iThreadServer;
	RThread iServerThread;
	CServerDeathWatcher* iThreadWatcher;
	};
	
class CIoConsoleProxyServer : public CConsoleProxyServer
	{
public:
	static CIoConsoleProxyServer* NewL(TConsoleCreateFunction aConsoleCreate, RLibrary& aConsoleLibrary);
	virtual CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	~CIoConsoleProxyServer();
	MProxiedConsole* TheConsole() const;
	void SetTheConsole(MProxiedConsole* aConsole);
private:
	CIoConsoleProxyServer(TConsoleCreateFunction aConsoleCreate, const RLibrary& aConsoleLibrary);
private:
	RLibrary iConsoleLibrary;
	MProxiedConsole* iTheConsole;
	};

CConsoleProxyServer* CIoConsoleProxyServerNewL(TAny* aParams);

class CIoConsoleProxySession : public CConsoleProxySession
	{
public:
	CIoConsoleProxySession(TConsoleCreateFunction aConsoleCreate);
	~CIoConsoleProxySession();
public: // from CConsoleProxySession:
	virtual void ServiceL(const RMessage2& aMessage);
	virtual MProxiedConsole* InstantiateConsoleL();
	virtual void ConsoleCreatedL(MProxiedConsole* aConsole);
private:
	enum TFlag
		{
		ELazy				= 0x01,
		EAutoDetectSize		= 0x02,
		ESupportsStdErr		= 0x04,
		};
private:
	CIoConsoleProxyServer* Server() { return (CIoConsoleProxyServer*)CConsoleProxySession::Server();}
	void DetectSizeL(const RMessage2& aMessage);
	void SetModeL(const RMessage2& aMessage);
	void SetUnderlyingConsoleL(const RMessage2& aMessage);
	void OpenExistingL(const RMessage2& aMessage);
	
	TBool GetFlag(TFlag aFlag);
	void SetFlag(TFlag aFlag, TBool aSet);
private:
	CConsoleProxy* iUnderlyingConsole;
	TUint iFlags;
	TSize iDetectedSize;
	};
	
class CWriteOnlyConsoleProxy : public CConsoleProxy
	{
public:
	static CConsoleProxy* NewL(const RConsoleProxy& aProxySession);
public: // from CConsoleProxy:
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel();
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
private:
	CWriteOnlyConsoleProxy();
	};
	
class CLazyConsole : public CConsoleBase
	{
public:
	CLazyConsole(TConsoleCreateFunction aConsoleCreate, TBool aAutoDetectSize);
	~CLazyConsole();
public: // from CConsoleBase
	virtual TInt Create(const TDesC &aTitle,TSize aSize);
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC &aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint &aPoint);
	virtual void SetCursorPosRel(const TPoint &aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC &aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
private:
	TInt CheckCreated() const;
private:
	TConsoleCreateFunction iConsoleCreate;
	TBool iSizeAutoDetect;
	RBuf iTitle;
	TSize iSize;
	mutable TSize iDetectedSize;
	mutable TInt iCreateError;
	mutable CConsoleBase* iConsole;
	};

#endif //__CONSOLE_H__
