// persistentconsole.h
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

#ifndef __PERSISTENTCONSOLE_H__
#define __PERSISTENTCONSOLE_H__

#include <e32base.h>
#include "console.h"

class TConsoleTitleSetter : public MIoWriter
	{
public:
	TConsoleTitleSetter(HBufC*& aTitle);
private: //from MIoReadWriter:
	virtual RIoReadWriteHandle::TMode IorwMode() const;
private: //from MIoWriter:
	virtual TInt IowWriteLength() const;
	virtual TInt IowWrite(TDes& aBuf);
	virtual void IowComplete(TInt aError);
	virtual TName IowName();
	virtual void IowCursorPos(TInt aError, TPoint aPos);
	virtual void IowSetCursorPosAbsComplete(TInt aError);
	virtual void IowSetCursorPosRelComplete(TInt aError);
	virtual void IowSetCursorHeightComplete(TInt aError);
	virtual void IowSetTitleComplete(TInt aError);
	virtual void IowClearScreenComplete(TInt aError);
	virtual void IowClearToEndOfLineComplete(TInt aError);
	virtual void IowScreenSize(TInt aError, TSize aSize);
	virtual void IowSetAttributesComplete(TInt aError);
	virtual HBufC* IowTitleLC();
private:
	HBufC*& iTitle;
	};


class CIoPersistentConsole
		: public CIoConsole
	{
	class TIoWriterProxy;
	class TIoReaderProxy;
public:
	static CIoPersistentConsole* NewLC(const TDesC& aName, const TDesC& aTitle, CIoServer& aServer, const RMsg& aMessage);
	virtual ~CIoPersistentConsole();
	
	TBool CanConnectReaderL(const MIoReadEndPoint& aReader) const;
	void AttachTransientReaderL(MIoReadEndPoint& aReader, CIoSession* aDetachOnClose);
	void DetachTransientReader();
	const MIoReadEndPoint* TransientReader() const;
	TBool CanConnectWriterL(const MIoWriteEndPoint& aWriter) const;
	void AttachTransientWriterL(MIoWriteEndPoint& aWriter, CIoSession* aDetachOnClose);
	void DetachTransientWriter();
	const MIoWriteEndPoint* TransientWriter() const;
	MIoWriteEndPoint* TransientWriter();
	
	void NotifyReadDetachL(const RMsg& aMessage);
	void CancelNotifyReadDetach(TRequestStatus* aClientStatus);
	void NotifyWriteDetachL(const RMsg& aMessage);
	void CancelNotifyWriteDetach(TRequestStatus* aClientStatus);
	
	TName TransientReaderName();
	TName TransientWriterName();
	
	TThreadId Creator();
	
	// From CObject:
	virtual TName Name() const;
	// From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
	virtual void SessionClosed(const CIoSession& aSession);
	virtual void ClosedBy(const CIoSession& aSession);
	// From MIoReadEndPoint.
	virtual void IorepReadL(MIoReader& aReader);
	virtual void IorepReadKeyL(MIoReader& aReader);
	virtual void IorepSetConsoleModeL(RIoReadWriteHandle::TMode aMode, MIoReader& aReader);
	// From MIoWriteEndPoint.
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
private:
	// from CIoEndPoint:
	virtual TInt HandleReaderAttached(MIoReader& aReader);
	virtual void HandleReaderDetached(MIoReader& aReader);
	virtual void ForegroundReaderChanged();
	virtual TInt HandleWriterAttached(MIoWriter& aWriter);
	virtual void HandleWriterDetached(MIoWriter& aWriter);
	virtual void HandleForegroundReaderChanged();
	
	void SendReadDetachNotifications();
	void SendWriteDetachNotifications();
	
	TInt ReaderProxyIndex(MIoReader* aReader) const;
	TInt WriterProxyIndex(MIoWriter* aWriter) const;
	TIoWriterProxy& GetWriterProxy(MIoWriter* aWriter) const;
	TIoReaderProxy& GetReaderProxy(MIoReader* aReader) const;
private:
	CIoPersistentConsole(CIoServer& aServer);
	void ConstructL(const TDesC& aName, const TDesC& aTitle, const RMsg& aMessage);

	class TIoWriterProxy : public MIoWriter
		{
	public:
		TIoWriterProxy(MIoWriter& aWriter, CIoPersistentConsole& aOwner);
		void TransientWriterAttached(MIoWriteEndPoint& aWriteEndPoint);
		void TransientWriterDetach(MIoWriteEndPoint& aWriteEndPoint);
		
		void WriteL(MIoWriteEndPoint* aEndPoint);
		void WriteCancel(MIoWriteEndPoint* aEndPoint);
		MIoWriter& ClientWriter() const;
		void GetCursorPosL(MIoWriteEndPoint* aEndPoint);
		void SetCursorPosAbsL(const TPoint& aPos, MIoWriteEndPoint* aEndPoint);
		void SetCursorPosRelL(const TPoint& aPos, MIoWriteEndPoint* aEndPoint);
		void SetCursorHeightL(TInt aPercentage, MIoWriteEndPoint* aEndPoint);
		void SetTitleL(MIoWriteEndPoint* aEndPoint);
		void ClearScreenL(MIoWriteEndPoint* aEndPoint);
		void ClearToEndOfLineL(MIoWriteEndPoint* aEndPoint);
		void GetScreenSizeL(MIoWriteEndPoint* aEndPoint);
		void SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriteEndPoint* aEndPoint);

		// from MIoReadWriter
		virtual RIoReadWriteHandle::TMode IorwMode() const;
		
		// from MIoWriter
		virtual TInt IowWriteLength() const;
		virtual TInt IowWrite(TDes& aBuf);
		virtual void IowComplete(TInt aError);
		virtual TName IowName();
		virtual void IowCursorPos(TInt aError, TPoint aPos);
		virtual void IowSetCursorPosAbsComplete(TInt aError);
		virtual void IowSetCursorPosRelComplete(TInt aError);
		virtual void IowSetCursorHeightComplete(TInt aError);
		virtual void IowSetTitleComplete(TInt aError);
		virtual void IowClearScreenComplete(TInt aError);
		virtual void IowClearToEndOfLineComplete(TInt aError);
		virtual void IowScreenSize(TInt aError, TSize aSize);
		virtual void IowSetAttributesComplete(TInt aError);
		virtual HBufC* IowTitleLC();
	private:
		enum TFlags
			{
			EWritePending			= 0x0001,
			EGetCursorPosPending	= 0x0002,
			ESetCursorPosAbsPending	= 0x0004,
			ESetCursorPosRelPending	= 0x0008,
			EClearScreenPending		= 0x0010,
			EClearToEndOfLinePending= 0x0020,
			EGetScreenSizePending	= 0x0040,
			EDetaching				= 0x0080,
			ESetCursorHeightPending = 0x0100,
			ESetTitlePending		= 0x0200,
			ESetAttributesPending	= 0x0400
			};
	private:
		TBool GetFlag(TFlags aFlag);
		void SetFlag(TFlags aFlag);
		void ClearFlag(TFlags aFlag);
	private:
		MIoWriter& iWriter;
		CIoPersistentConsole& iOwner;
		TUint iFlags;
		TPoint iSetCursPosAbsPoint;
		TPoint iSetCursPosRelPoint;
		TInt iSetCursorHeight;
		TUint iAttributes;
		ConsoleAttributes::TColor iForegroundColor;
		ConsoleAttributes::TColor iBackgroundColor;
		};
		
	class TIoReaderProxy : public MIoReader
		{
	public:
		TIoReaderProxy(MIoReader& aReader, CIoPersistentConsole& aOwner);
		void TransientReaderAttached(MIoReadEndPoint& aReader);
		
		void ReadL(MIoReadEndPoint* aEndPoint);
		void ReadKeyL(MIoReadEndPoint* aEndPoint);
		void SetConsoleModeL(MIoReadEndPoint* aEndPoint, RIoReadWriteHandle::TMode aMode);
		MIoReader& ClientReader() const;

		// from MIoReadWriter
		virtual RIoReadWriteHandle::TMode IorwMode() const;
		
		// from MIoReader
		virtual TBool IorReadPending() const;
		virtual TBool IorReadKeyPending() const;
		virtual TDes& IorReadBuf();
		virtual void IorDataBuffered(TInt aLength);
		virtual TBool IorDataIsBuffered() const;
		virtual TBool IorIsKeyCaptured(TUint aKeyCode, TUint aModifiers);
		virtual void IorReadComplete(TInt aError);
		virtual void IorReadKeyComplete(TInt aError, TUint aKeyCode, TUint aModifiers);
		virtual TName IorName();
		virtual void IorReaderChange(TUint aChange);
		virtual void IorSetConsoleModeComplete(TInt aError);
	private:
		MIoReader& iReader;
		CIoPersistentConsole& iOwner;
		TBool iDetaching;
		TBool iSetConsoleModePending;
		RIoReadWriteHandle::TMode iSetConsoleMode;
		};
private:
	CIoServer& iServer;
	RThread iCreator;
	TName iName; // name for the persistent console, used to identify it
	HBufC* iTitle; // title for the console, passed to CConsoleBase::SetTitle
	MIoReadEndPoint* iTransientReadEndPoint;
	CIoSession* iReadEndPointDetachOnClose;
	MIoWriteEndPoint* iTransientWriteEndPoint;
	CIoSession* iWriteEndPointDetachOnClose;
	
	RPointerArray<TIoWriterProxy> iWriterProxies;
	RPointerArray<TIoReaderProxy> iReaderProxies;
	
	RIoReadWriteHandle::TMode iPersistentConsoleMode;
	
	RArray<RMsg> iReadDetachNotifications;
	RArray<RMsg> iWriteDetachNotifications;
	
	TConsoleTitleSetter iTitleSetter;
	};
	
#endif //__PERSISTENTCONSOLE_H__
