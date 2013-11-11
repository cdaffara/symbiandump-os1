// readwrite.h
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

#ifndef __READWRITE_H__
#define __READWRITE_H__

#include <e32base.h>
#include "server.h"
#include "iocli.h"
#include "object.h"


class CIoConsole;
class CThreadOwner;

class MIoReadWriter
	{
public:
	virtual RIoReadWriteHandle::TMode IorwMode() const = 0;
	};


class MIoReader : public MIoReadWriter
	{
public:
	virtual TBool IorReadPending() const = 0;
	virtual TBool IorReadKeyPending() const = 0;
	virtual TDes& IorReadBuf() = 0;
	virtual void IorDataBuffered(TInt aLength) = 0;
	virtual TBool IorDataIsBuffered() const = 0;
	virtual TBool IorIsKeyCaptured(TUint aKeyCode, TUint aModifiers) = 0;
	virtual void IorReadComplete(TInt aError) = 0;
	virtual void IorReadKeyComplete(TInt aError, TUint aKeyCode, TUint aModifiers) = 0;
	virtual TName IorName() = 0;
	virtual void IorReaderChange(TUint aChange) = 0;
	virtual void IorSetConsoleModeComplete(TInt aError) = 0;
	};


class MIoWriter : public MIoReadWriter
	{
public:
	virtual TInt IowWriteLength() const = 0;
	virtual TInt IowWrite(TDes& aBuf) = 0;
	virtual void IowComplete(TInt aError) = 0;
	virtual TName IowName() = 0;
	virtual void IowCursorPos(TInt aError, TPoint aPos) = 0;
	virtual void IowSetCursorPosAbsComplete(TInt aError) = 0;
	virtual void IowSetCursorPosRelComplete(TInt aError) = 0;
	virtual void IowSetCursorHeightComplete(TInt aError) = 0;
	virtual void IowSetTitleComplete(TInt aError) = 0;
	virtual void IowClearScreenComplete(TInt aError) = 0;
	virtual void IowClearToEndOfLineComplete(TInt aError) = 0;
	virtual void IowScreenSize(TInt aError, TSize aSize) = 0;
	virtual void IowSetAttributesComplete(TInt aError) = 0;
	virtual HBufC* IowTitleLC() = 0;
	virtual TBool IowIsStdErr() const { return EFalse; }
	};


class MIoEndPoint
	{
public:
	TBool IoepIsConsole() const;
	virtual TName IoepName() const = 0;
	virtual TBool IoepIsType(RIoHandle::TType aType) const = 0;
	};

class MIoReadEndPoint : public MIoEndPoint
	{
public:
	virtual void IorepAttachL(MIoReader& aReader, RIoEndPoint::TReadMode aMode) = 0;
	virtual void IorepDetach(MIoReader& aReader) = 0;
	virtual void IorepReadL(MIoReader& aReader) = 0;
	virtual void IorepSetForegroundReaderL(MIoReader& aReader) = 0;
	virtual TBool IorepIsForegroundL(const MIoReader& aReader) const = 0;
	virtual void IorepReadKeyL(MIoReader& aReader);
	virtual void IorepSetConsoleModeL(RIoReadWriteHandle::TMode aMode, MIoReader& aReader);
	static TBool AttachedToConsole(const MIoReadEndPoint* aEndPoint);
	};


class MIoWriteEndPoint : public MIoEndPoint
	{
public:
	virtual void IowepAttachL(MIoWriter& aReader) = 0;
	virtual void IowepDetach(MIoWriter& aReader) = 0;
	virtual void IowepWriteL(MIoWriter& aWriter) = 0;
	virtual void IowepWriteCancel(MIoWriter& aWriter) = 0;
	virtual void IowepCursorPosL(MIoWriter& aWriter) const;
	virtual void IowepSetCursorPosAbsL(const TPoint& aPoint, MIoWriter& aWriter);
	virtual void IowepSetCursorPosRelL(const TPoint& aPoint, MIoWriter& aWriter);
	virtual void IowepSetCursorHeightL(TInt aPercentage, MIoWriter& aWriter);
	virtual void IowepSetTitleL(MIoWriter& aWriter);
	virtual void IowepClearScreenL(MIoWriter& aWriter);
	virtual void IowepClearToEndOfLineL(MIoWriter& aWriter);
	virtual void IowepScreenSizeL(MIoWriter& aWriter) const;
	virtual void IowepSetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriter& aWriter);
	static TBool AttachedToConsole(const MIoWriteEndPoint* aEndPoint);
protected:
	virtual TPoint IowepCursorPos() const;
	virtual void IowepSetCursorPosAbs(const TPoint& aPoint);
	virtual void IowepSetCursorPosRel(const TPoint& aPoint);
	virtual void IowepSetCursorHeight(TInt aHeight);
	virtual void IowepSetTitle(const TDesC& aTitle);
	virtual void IowepClearScreen();
	virtual void IowepClearToEndOfLine();
	virtual TSize IowepScreenSize() const;
	virtual void IowepSetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor);
	};


class CIoReadWriteObject : public CIoObject
	{
public:
	TUint Id() const;
	void SetOwnerL(TThreadId aOwningThread);
	TInt Open(TThreadId aOwningThread);
	void DoDuplicateL(const CIoReadWriteObject& aDuplicate);
	TBool IsOwner(TThreadId aOwningThread) const;
	TBool OpenedByOwner() const;
	void SetConsole(CIoConsole& aConsole);
	CIoConsole* Console();
	const CIoConsole* Console() const;
	MIoEndPoint* EndPoint();
	virtual void SetModeL(const RMsg& aMessage);
protected:
	CIoReadWriteObject(TUint aId);
	void Dump(const TDesC& aData) const;
protected:
	MIoEndPoint* iEndPoint;
	RIoReadWriteHandle::TMode iMode;
private:
	const TUint iId;
	TThreadId iOwningThreadId;
	TBool iOpenedByOwner;
	CIoConsole* iConsole;
	};


class CIoReadObject : public CIoReadWriteObject, public MIoReader
	{
public:
	static CIoReadObject* NewLC(TInt aId);
	virtual ~CIoReadObject();
	void DuplicateL(const CIoReadObject& aDuplicate);
	void AttachL(MIoReadEndPoint& aEndPoint, RIoEndPoint::TReadMode aMode);
	void SetReadMode(RIoReadHandle::TReadMode aMode);
	void SetToForegroundL();
	TBool IsForegroundL() const;
	void ReadL(const RMsg& aMessage);
	void ReadCancel(const CIoSession& aSession);
	void SetLineSeparatorL(const RMsg& aMessage);
	void ReadKeyL(const RMsg& aMessage);
	void ReadKeyCancel(const CIoSession& aSession);
	void CaptureKeyL(const RMsg& aMessage);
	void CancelCaptureKey(const RMsg& aMessage);
	void CaptureAllKeys(const RMsg& aMessage);
	void CancelCaptureAllKeys(const RMsg& aMessage);
	void NotifyChange(const RMsg& aMessage);
	void CancelNotifyChange(const CIoSession& aSession);
public: // From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
	virtual void SessionClosed(const CIoSession& aSession);
public: // From CIoReadWriteObject.
	virtual void SetModeL(const RMsg& aMessage);
public: // From MIoReadWriter.
	virtual RIoReadWriteHandle::TMode IorwMode() const;
private: // From MIoReader.
	virtual TBool IorReadPending() const;
	virtual TBool IorReadKeyPending() const;
	virtual TDes& IorReadBuf();
	virtual void IorDataBuffered(TInt aLength);
	virtual TBool IorDataIsBuffered() const;
	virtual TBool IorIsKeyCaptured(TUint aKeyCode, TUint aModifier);
	virtual void IorReadComplete(TInt aError);
	virtual void IorReadKeyComplete(TInt aError, TUint aKeyCode, TUint aModifiers);
	virtual TName IorName();
	virtual void IorReaderChange(TUint aChange);
	virtual void IorSetConsoleModeComplete(TInt aError);
private:
	CIoReadObject(TInt aId);
	void ProcessRead();
	void AllocateBufferL(TInt aLength);
	void TryToCompleteRead();
	void CompleteRead(TInt aError, TInt aLength);
	void CompleteReadKey(TInt aError, RIoConsoleReadHandle::TConsoleKey& aConsoleKey);
	MIoReadEndPoint* ReadEndPoint() const;
	void CancelSetMode(const CIoSession& aSession);
private:
	class TCapturedKey
		{
	public:
		TCapturedKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers);
	public:
		TUint iKeyCode;
		TUint iModifierMask;
		TUint iModifiers;
		};
private:
	RIoReadHandle::TReadMode iReadMode;
	TInt iMaxReadLength;
	RMsg iReadMessage;
	RMsg iReadKeyMessage;
	HBufC* iBuf;
	TPtr iPtr;
	HBufC* iLineSeparator; // NULL means use default "\n"
	RArray<TCapturedKey> iCapturedKeys;
	RArray<RIoConsoleReadHandle::TConsoleKey> iKeyBuffer;
	TBool iCaptureAllKeys;
	TInt iCompleteErr; // non-zero if we have completed reading
	RMsg iChangeNotifyMessage;
	RMsg iSetModeMessage;
	};


class CIoWriteObject : public CIoReadWriteObject, public MIoWriter
	{
public:
	static CIoWriteObject* NewLC(TInt aId);
	~CIoWriteObject();
	void DuplicateL(const CIoWriteObject& aDuplicate);
	void AttachL(MIoWriteEndPoint& aEndPoint);
	void WriteL(const RMsg& aMessage);
	void WriteCancel(const CIoSession& aSession);
	void CursorPosL(const RMsg& aMessage);
	void SetCursorPosAbsL(const RMsg& aMessage);
	void SetCursorPosRelL(const RMsg& aMessage);
	void SetCursorHeightL(const RMsg& aMessage);
	void SetTitleL(const RMsg& aMessage);
	void ClearScreen(const RMsg& aMessage);
	void ClearToEndOfLine(const RMsg& aMessage);
	void ScreenSizeL(const RMsg& aMessage);
	void SetAttributesL(const RMsg& aMessage);
	void SetIsStdErr(TBool aFlag);
public: // From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
	virtual void SessionClosed(const CIoSession& aSession);
public: // From MIoReadWriter.
	virtual RIoReadWriteHandle::TMode IorwMode() const;
private: // From MIoWriter.
	virtual TInt IowWriteLength() const;
	virtual TInt IowWrite(TDes& aBuf);
	virtual void IowComplete(TInt aError);
	virtual TName IowName();
	virtual RIoWriteHandle::TMode IowWriteMode() const;
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
	virtual TBool IowIsStdErr() const;
private:
	CIoWriteObject(TInt aId);
	MIoWriteEndPoint* WriteEndPoint() const;
protected:
	TInt iOffset;
	TInt iWriteLength;
	RMsg iWriteMessage;
	RMsg iGetCursorPosMsg;
	RMsg iSetCursorPosAbsMsg;
	RMsg iSetCursorPosRelMsg;
	RMsg iSetCursorHeightMsg;
	RMsg iSetTitleMsg;
	RMsg iClearScreenMsg;
	RMsg iClearToEndOfLineMsg;
	RMsg iGetScreenSizeMsg;
	RMsg iSetAttributesMsg;
	TBool iTriedConsoleScreenSize;
	TBool iIsStdErr;
	};

#endif // __READWRITE_H__
