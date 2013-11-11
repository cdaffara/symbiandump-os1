// readwrite.cpp
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

#include "server.h"
#include "pipe.h"
#include "console.h"
#include "readwrite.h"
#include "session.h"
#include "log.h"
#include "persistentconsole.h"

_LIT(KNewLine, "\r\n");
_LIT(KLf, "\n");

#ifdef IOSRV_LOGGING
#define OBJ_NAME(x) TName objName((x).Name())
#else
#define OBJ_NAME(x)
#endif

#define __ASSERT_RETURN(x, y) {if (!(x)) {{y;};return;}}


static void CleanupNullMessage(TAny* aMsgPtr)
	{
	*((RMsg*)aMsgPtr) = RMsg();
	}
	
static void CleanupNullMessagePushL(RMsg& aMsg)
	{
	CleanupStack::PushL(TCleanupItem(CleanupNullMessage, &aMsg));
	}
	

//
// MIoEndPoint.
//

TBool MIoEndPoint::IoepIsConsole() const
	{
	return IoepIsType(RIoHandle::EConsole);
	}


//
// MIoReadEndPoint.
//

void MIoReadEndPoint::IorepReadKeyL(MIoReader&)
	{
	User::Leave(KErrNotSupported);
	}

void MIoReadEndPoint::IorepSetConsoleModeL(RIoReadWriteHandle::TMode, MIoReader& aReader)
	{
	aReader.IorSetConsoleModeComplete(KErrNotSupported);
	}

TBool MIoReadEndPoint::AttachedToConsole(const MIoReadEndPoint* aEp)
	{
	if (!aEp) return EFalse;
	if (aEp->IoepIsType(RIoHandle::EPersistentConsole))
		{
		CIoPersistentConsole* pcons = (CIoPersistentConsole*)aEp;
		return AttachedToConsole(pcons->TransientReader());
		}
	else
		{
		return aEp->IoepIsConsole();
		}
	}


//
// MIoWriteEndPoint.
//

void MIoWriteEndPoint::IowepCursorPosL(MIoWriter& aWriter) const
	{
	TPoint pos = IowepCursorPos();
	aWriter.IowCursorPos(KErrNone, pos);
	}
	
void MIoWriteEndPoint::IowepSetCursorPosAbsL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	IowepSetCursorPosAbs(aPoint);
	aWriter.IowSetCursorPosAbsComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepSetCursorPosRelL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	IowepSetCursorPosRel(aPoint);
	aWriter.IowSetCursorPosRelComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepSetCursorHeightL(TInt aPercentage, MIoWriter& aWriter)
	{
	IowepSetCursorHeight(aPercentage);
	aWriter.IowSetCursorHeightComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepSetTitleL(MIoWriter& aWriter)
	{
	HBufC* title = aWriter.IowTitleLC();
	IowepSetTitle(*title);
	CleanupStack::PopAndDestroy(title);
	aWriter.IowSetTitleComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepClearScreenL(MIoWriter& aWriter)
	{
	IowepClearScreen();
	aWriter.IowClearScreenComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepClearToEndOfLineL(MIoWriter& aWriter)
	{
	IowepClearToEndOfLine();
	aWriter.IowClearToEndOfLineComplete(KErrNone);
	}
	
void MIoWriteEndPoint::IowepScreenSizeL(MIoWriter& aWriter) const
	{
	TSize size = IowepScreenSize();
	aWriter.IowScreenSize(KErrNone, size);
	}

void MIoWriteEndPoint::IowepSetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriter& aWriter)
	{
	IowepSetAttributes(aAttributes, aForegroundColor, aBackgroundColor);
	aWriter.IowSetAttributesComplete(KErrNone);
	}

TPoint MIoWriteEndPoint::IowepCursorPos() const
	{
	return TPoint(0, 0);
	}

void MIoWriteEndPoint::IowepSetCursorPosAbs(const TPoint&)
	{
	}

void MIoWriteEndPoint::IowepSetCursorPosRel(const TPoint&)
	{
	}

void MIoWriteEndPoint::IowepSetCursorHeight(TInt)
	{
	}

void MIoWriteEndPoint::IowepSetTitle(const TDesC&)
	{
	}

void MIoWriteEndPoint::IowepClearScreen()
	{
	}

void MIoWriteEndPoint::IowepClearToEndOfLine()
	{
	}

TSize MIoWriteEndPoint::IowepScreenSize() const
	{
	return TSize(0, 0);
	}

void MIoWriteEndPoint::IowepSetAttributes(TUint, ConsoleAttributes::TColor, ConsoleAttributes::TColor)
	{
	}

TBool MIoWriteEndPoint::AttachedToConsole(const MIoWriteEndPoint* aEp)
	{
	if (!aEp) return EFalse;
	if (aEp->IoepIsType(RIoHandle::EPersistentConsole))
		{
		CIoPersistentConsole* pcons = (CIoPersistentConsole*)aEp;
		return AttachedToConsole(pcons->TransientWriter());
		}
	else
		{
		return aEp->IoepIsConsole();
		}

	}

//
// CIoReadWriteObject.
//

TUint CIoReadWriteObject::Id() const
	{
	return iId;
	}

CIoReadWriteObject::CIoReadWriteObject(TUint aId)
	: iId(aId), iOwningThreadId(0)
	{
	}

void CIoReadWriteObject::SetOwnerL(TThreadId aOwningThread)
	{
	iOwningThreadId = aOwningThread;
	}

TInt CIoReadWriteObject::Open(TThreadId aOwningThread)
	{
	TInt err = CObject::Open();
	if ((err==KErrNone) && (IsOwner(aOwningThread)))
		{
		iOpenedByOwner = ETrue;
		}
	return err;
	}
	
void CIoReadWriteObject::SetModeL(const RMsg& aMessage)
	{
	TInt mode(aMessage.Int0());
	if ((mode >= RIoWriteHandle::EText) && (mode <= RIoWriteHandle::EBinary))
		{
		iMode = static_cast<RIoWriteHandle::TMode>(mode);
		Complete(aMessage, KErrNone);
		}
	else
		{
		Complete(aMessage, KErrNotSupported);
		}
	}

void CIoReadWriteObject::DoDuplicateL(const CIoReadWriteObject& aDuplicate)
	{
	iConsole = aDuplicate.iConsole;
	}

TBool CIoReadWriteObject::IsOwner(TThreadId aOwningThread) const
	{
	return iOwningThreadId == aOwningThread;
	}

TBool CIoReadWriteObject::OpenedByOwner() const
	{
	return iOpenedByOwner;
	}

void CIoReadWriteObject::SetConsole(CIoConsole& aConsole)
	{
	iConsole = &aConsole;
	}

CIoConsole* CIoReadWriteObject::Console()
	{
	return iConsole;
	}

MIoEndPoint* CIoReadWriteObject::EndPoint()
	{
	return iEndPoint;
	}

const CIoConsole* CIoReadWriteObject::Console() const
	{
	return iConsole;
	}

#ifndef IOSRV_LOGGING
void CIoReadWriteObject::Dump(const TDesC&) const
	{
	}
#else
void CIoReadWriteObject::Dump(const TDesC& aData) const
	{
	TBuf<80> out;
	TBuf<16> ascii;
	TInt dataIndex = 0;
	TInt pos = 0;
	do
		{
		out.Zero();
		ascii.Zero();
		out.AppendNumFixedWidthUC(pos, EHex, 8);
		out.Append(_L(": "));
		for (TInt i = 0; i < 16; ++i)
			{
			if (dataIndex < aData.Length())
				{
				TUint8 byte = (TUint8)aData[dataIndex++];
				out.AppendNumFixedWidthUC(byte, EHex, 2);
				out.Append(_L(" "));
				if ((byte < 0x20) || (byte >= 0x7f) || byte == '%')
					{
					byte = '.';
					}
				ascii.Append(TChar(byte));
				++pos;
				}
			else
				{
				out.Append(_L("   "));
				}
			}
		out.Append(ascii);
		CIoLog::Printf(out);
		}
		while (dataIndex < aData.Length());
	}
#endif


//
// CIoReadObject.
//

CIoReadObject* CIoReadObject::NewLC(TInt aId)
	{
	CIoReadObject* self = new(ELeave) CIoReadObject(aId);
	LOG(CIoLog::Printf(_L("Read object 0x%08x created"), self));
	CleanupClosePushL(*self);
	return self;
	}

CIoReadObject::~CIoReadObject()
	{
	OBJ_NAME(*this);
	LOG(CIoLog::Printf(_L("Read object \"%S\" (0x%08x) destroying"), &objName, this));
	if (iEndPoint)
		{
		ReadEndPoint()->IorepDetach(*this);
		}

	CompleteIfPending(iReadMessage, KErrSessionClosed);
	CompleteIfPending(iReadKeyMessage, KErrSessionClosed);
	CompleteIfPending(iChangeNotifyMessage, KErrSessionClosed);

	delete iBuf;
	delete iLineSeparator;
	iCapturedKeys.Close();
	iKeyBuffer.Close();
	}

void CIoReadObject::DuplicateL(const CIoReadObject& aDuplicate)
	{
	DoDuplicateL(aDuplicate);
	if (aDuplicate.iEndPoint)
		{
		AttachL(*aDuplicate.ReadEndPoint(), RIoEndPoint::EBackground);
		}
	}

void CIoReadObject::AttachL(MIoReadEndPoint& aEndPoint, RIoEndPoint::TReadMode aMode)
	{
	if (iEndPoint)
		{
		ReadEndPoint()->IorepDetach(*this);
		}

	iEndPoint = &aEndPoint;
	TRAPD(err, aEndPoint.IorepAttachL(*this, aMode));
	if (err)
		{
		iEndPoint = NULL;
		User::Leave(err);
		}
	}

void CIoReadObject::SetReadMode(RIoReadHandle::TReadMode aMode)
	{
	iReadMode = aMode;
	}

void CIoReadObject::SetToForegroundL()
	{
	if (iEndPoint)
		{
		ReadEndPoint()->IorepSetForegroundReaderL(*this);
		}
	}

TBool CIoReadObject::IsForegroundL() const
	{
	if (iEndPoint)
		{
		return ReadEndPoint()->IorepIsForegroundL(*this);
		}
	return EFalse;
	}

void CIoReadObject::ReadL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicReadWhenNotAttached));
	__ASSERT_RETURN(!MessagePending(iReadMessage) && !MessagePending(iReadKeyMessage), PanicClient(aMessage, EPanicReadAlreadyPending));

	const TInt maxReadLength = MaxDesLengthL(aMessage, 0);
	AllocateBufferL(maxReadLength);
	iReadMessage = aMessage;
	iMaxReadLength = maxReadLength;

	if (iCompleteErr)
		{
		if ((iCompleteErr == KErrEof) && (iBuf->Length() > 0))
			{
			// the end point has reached the end of the stream
			// but we haven't finished processing the data in our buffer yet
			TryToCompleteRead();
			}
		else
			{
			iReadMessage.Complete(iCompleteErr);
			}
		}
	else
		{
		ProcessRead();
		}
	}

void CIoReadObject::ReadCancel(const CIoSession& aSession)
	{
	if (MessagePending(iReadMessage) && (iReadMessage.Session() == static_cast<const CSession2*>(&aSession)))
		{
		Complete(iReadMessage, KErrCancel);
		iBuf->Des().Zero();
		}
	}

void CIoReadObject::ProcessRead()
	{
	ASSERT((MessagePending(iReadMessage) || MessagePending(iReadKeyMessage)) && !(MessagePending(iReadMessage) && MessagePending(iReadKeyMessage)));

	if (MessagePending(iReadMessage))
		{
		if (iCompleteErr)
			{
			iReadMessage.Complete(iCompleteErr);
			}
		else
			{
			TryToCompleteRead();

			if (IorReadPending())
				{
				TRAPD(err, ReadEndPoint()->IorepReadL(*this));
				if (err)
					{
					Complete(iReadMessage, err);
					}
				else if (IorReadPending())
					{
					TryToCompleteRead();
					}
				}
			}
		}
	else
		{
		if (iCompleteErr)
			{
			iReadKeyMessage.Complete(iCompleteErr);
			}
		else
			{
			if (iKeyBuffer.Count() > 0)
				{
				CompleteReadKey(KErrNone, iKeyBuffer[0]);
				iKeyBuffer.Remove(0);
				}
			else
				{
				TRAPD(err, ReadEndPoint()->IorepReadKeyL(*this));
				if (err)
					{
					Complete(iReadKeyMessage, err);
					}
				}
			}
		}
	}

void CIoReadObject::SetLineSeparatorL(const RMsg& aMessage)
	{
	HBufC* separator = HBufC::NewLC(DesLengthL(aMessage, 0));
	TPtr ptr(separator->Des());
	MessageReadL(aMessage, 0, ptr);
	delete iLineSeparator;
	iLineSeparator = separator;
	CleanupStack::Pop(separator);
	Complete(aMessage, KErrNone);
	}

void CIoReadObject::ReadKeyL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicReadKeyWhenNotAttached));
	__ASSERT_RETURN(!MessagePending(iReadMessage) && !MessagePending(iReadKeyMessage), PanicClient(aMessage, EPanicReadAlreadyPending));

	iReadKeyMessage = aMessage;

	if (iCompleteErr)
		{
		iReadKeyMessage.Complete(iCompleteErr);
		}
	else
		{
		ProcessRead();
		}
	}

void CIoReadObject::ReadKeyCancel(const CIoSession& aSession)
	{
	if (MessagePending(iReadKeyMessage) && (iReadKeyMessage.Session() == &aSession))
		{
		Complete(iReadKeyMessage, KErrCancel);
		}
	}

void CIoReadObject::CaptureKeyL(const RMsg& aMessage)
	{
	User::LeaveIfError(iCapturedKeys.Append(TCapturedKey(aMessage.Int0(), aMessage.Int1(), aMessage.Int2())));
	Complete(aMessage, KErrNone);
	}

void CIoReadObject::CancelCaptureKey(const RMsg& aMessage)
	{
	TCapturedKey capturedKey(aMessage.Int0(), aMessage.Int1(), aMessage.Int2());
	const TInt numCapturedKeys = iCapturedKeys.Count();
	for (TInt i = 0; i < numCapturedKeys; ++i)
		{
		const TCapturedKey& thisCapturedKey = iCapturedKeys[i];
		if ((capturedKey.iKeyCode == thisCapturedKey.iKeyCode) && (capturedKey.iModifiers == thisCapturedKey.iModifiers) && (capturedKey.iModifierMask == thisCapturedKey.iModifierMask))
			{
			iCapturedKeys.Remove(i);
			Complete(aMessage, KErrNone);
			return;
			}
		}

	Complete(aMessage, KErrNotFound);
	}

void CIoReadObject::CaptureAllKeys(const RMsg& aMessage)
	{
	iCaptureAllKeys = ETrue;
	Complete(aMessage, KErrNone);
	}

void CIoReadObject::CancelCaptureAllKeys(const RMsg& aMessage)
	{
	iCaptureAllKeys = EFalse;
	Complete(aMessage, KErrNone);
	}
	
void CIoReadObject::NotifyChange(const RMsg& aMessage)
	{
	if (MessagePending(iChangeNotifyMessage))
		{
		Complete(aMessage, KErrAlreadyExists);
		}
	else
		{
		iChangeNotifyMessage = aMessage;
		}
	}
	
void CIoReadObject::CancelNotifyChange(const CIoSession& aSession)
	{
	if (MessagePending(iChangeNotifyMessage) && (iChangeNotifyMessage.Session() == static_cast<const CSession2*>(&aSession)))
		{
		Complete(iChangeNotifyMessage, KErrCancel);
		}
	}

void CIoReadObject::CancelSetMode(const CIoSession& aSession)
	{
	if (MessagePending(iSetModeMessage) && (iSetModeMessage.Session() == static_cast<const CSession2*>(&aSession)))
		{
		Complete(iSetModeMessage, KErrCancel);
		}
	}

TBool CIoReadObject::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EReadWriteObject) || (aType == RIoHandle::EReadObject));
	}

void CIoReadObject::SessionClosed(const CIoSession& aSession)
	{
	ReadCancel(aSession);
	ReadKeyCancel(aSession);
	CancelNotifyChange(aSession);
	CancelSetMode(aSession);
	}

void CIoReadObject::SetModeL(const RMsg& aMessage)
	{
	TInt mode(aMessage.Int0());
	if ((mode >= RIoWriteHandle::EText) && (mode <= RIoWriteHandle::EBinary))
		{
		if (ReadEndPoint() && ReadEndPoint()->IorepIsForegroundL(*this))
			{
			__ASSERT_RETURN(!MessagePending(iSetModeMessage), PanicClient(aMessage, EPanicSetModeAlreadyPending));
			ReadEndPoint()->IorepSetConsoleModeL((RIoReadWriteHandle::TMode)mode, *this);
			iSetModeMessage = aMessage;
			}
		else
			{
			Complete(aMessage, KErrNone);
			}

		iMode = static_cast<RIoWriteHandle::TMode>(mode);
		}
	else
		{
		Complete(aMessage, KErrNotSupported);
		}
	}

RIoReadWriteHandle::TMode CIoReadObject::IorwMode() const
	{
	return iMode;
	}

TBool CIoReadObject::IorReadPending() const
	{
	return (MessagePending(iReadMessage));
	}

TBool CIoReadObject::IorReadKeyPending() const
	{
	return (MessagePending(iReadKeyMessage));
	}

TDes& CIoReadObject::IorReadBuf()
	{
	ASSERT((iMaxReadLength - iBuf->Length()) > 0);
	iPtr.Set(const_cast<TText*>(iBuf->Des().Ptr()) + iBuf->Length(), 0, iMaxReadLength - iBuf->Length());
	return iPtr;
	}

void CIoReadObject::IorDataBuffered(TInt aLength)
	{
	iBuf->Des().SetLength(iBuf->Length() + aLength);
	TryToCompleteRead();
	}

TBool CIoReadObject::IorDataIsBuffered() const
	{
	return (iBuf->Length() > 0);
	}

TBool CIoReadObject::IorIsKeyCaptured(TUint aKeyCode, TUint aModifiers)
	{
	if (iCaptureAllKeys)
		{
		return ETrue;
		}
	const TInt numCapturedKeys = iCapturedKeys.Count();
	for (TInt i = 0; i < numCapturedKeys; ++i)
		{
		const TCapturedKey& thisCapturedKey = iCapturedKeys[i];
		if ((aKeyCode == thisCapturedKey.iKeyCode) && ((aModifiers & thisCapturedKey.iModifierMask) == thisCapturedKey.iModifiers))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CIoReadObject::IorReadComplete(TInt aError)
	{
	if ((aError == KErrNone) || (aError==KErrEof))
		{
		CompleteRead(aError, iBuf->Length());
		}
	else
		{
		CompleteRead(aError, 0);
		}
	}

void CIoReadObject::IorReadKeyComplete(TInt aError, TUint aKeyCode, TUint aModifiers)
	{
	RIoConsoleReadHandle::TConsoleKey consoleKey;
	consoleKey.iKeyCode = aKeyCode;
	consoleKey.iModifiers = aModifiers;
	if (IorReadKeyPending())
		{
		CompleteReadKey(aError, consoleKey);
		}
	else
		{
		if (aError)
			{
			LOG(CIoLog::Printf(_L("Error in IorReadKeyComplete %d"), aError));
			if (IorReadPending()) CompleteRead(aError, 0);
			// What state are we in if neither IorReadKeyPending or IorReadPending are true? Should we be setting iCompleteErr?
			}
		else
			{
			iKeyBuffer.Append(consoleKey);
			if (IorReadPending())
				{
				TryToCompleteRead();
				}
			}
		}
	}

TName CIoReadObject::IorName()
	{
	return Name();
	}
	
void CIoReadObject::IorReaderChange(TUint aChange)
	{
	if ((aChange == RIoReadHandle::EGainedForeground) && iEndPoint && ReadEndPoint()->IoepIsType(RIoHandle::EConsole))
		{
		ReadEndPoint()->IorepSetConsoleModeL(iMode, *this);
		}

	if (MessagePending(iChangeNotifyMessage))
		{
		TRAPD(err, MessageWriteL(iChangeNotifyMessage, 0, TPckg<TUint>(aChange)));
		Complete(iChangeNotifyMessage, err);
		}
	}

void CIoReadObject::IorSetConsoleModeComplete(TInt aError)
	{
	if (MessagePending(iSetModeMessage))
		{
		// The set mode request was explicitly requested, so complete it.
		Complete(iSetModeMessage, aError);
		}
	else
		{
		// The set mode request was made because the read object has come to the foreground. If there was an error,
		// report it via the next read request. However, only report errors when setting to binary mode. This is because
		// every console must implicitly support text mode, but they may not support the extension interface and report
		// KErrExtensionNotSupported even for text mode.

		if (aError && (iMode == RIoReadWriteHandle::EBinary))
			{
			iCompleteErr = aError;
			}
		}
	}

CIoReadObject::CIoReadObject(TInt aId)
	: CIoReadWriteObject(aId), iPtr(NULL, 0, 0)
	{
	}

void CIoReadObject::AllocateBufferL(TInt aLength)
	{
	if (iBuf == NULL)
		{
		iBuf = HBufC::NewL(aLength);
		}
	else if (iBuf->Des().MaxLength() < aLength)
		{
		iBuf = iBuf->ReAllocL(aLength);
		}
	}

void CIoReadObject::TryToCompleteRead()
	{
	if (iKeyBuffer.Count() > 0)
		{
		// There are buffered key events so copy them into the read buffer.
		TDes& readBuf = IorReadBuf();
		while (iKeyBuffer.Count())
			{
			const RIoConsoleReadHandle::TConsoleKey& consoleKey = iKeyBuffer[0];
			if ((IorwMode() == RIoReadWriteHandle::EText) && (consoleKey.iKeyCode == EKeyEnter))
				{
				if ((readBuf.Length() + KNewLine().Length()) <= readBuf.MaxLength())
					{
					// If there's enough room, expand EKeyEnter into "\r\n" (otherwise RIoReadHandle::ELine reads won't complete).
					readBuf.Append(KNewLine);
					}
				else if (readBuf.Length() < readBuf.MaxLength())
					{
					// Otherwise, just put in the raw code code in the read buffer like normal - the client might be reading into a TBuf<1> (fshell does this for example), in which case there will never be enough room for "\r\n".
					TPtrC keyCodePtr((TUint16*)&consoleKey.iKeyCode, 1);
					readBuf.Append(keyCodePtr);
					}
				else
					{
					break;
					}
				}
			else
				{
				if (readBuf.Length() < readBuf.MaxLength())
					{
					TPtrC keyCodePtr((TUint16*)&consoleKey.iKeyCode, 1);
					readBuf.Append(keyCodePtr);
					}
				else
					{
					break;
					}
				}
			iKeyBuffer.Remove(0);
			}
		iBuf->Des().SetLength(iBuf->Length() + readBuf.Length());
		}

	switch (iReadMode)
		{
		case RIoReadHandle::EFull:
			{
			if (iBuf->Length() == iMaxReadLength)
				{
				CompleteRead(KErrNone, iMaxReadLength);
				}
			break;
			}
		case RIoReadHandle::ELine:
			{
			const TDesC* lineSeparator = iLineSeparator;
			if (lineSeparator == NULL) lineSeparator = &KLf;

			TInt pos = iBuf->Find(*lineSeparator);
			if (pos >= 0)
				{
				CompleteRead(KErrNone, pos + lineSeparator->Length());
				}
			else if (iBuf->Length() == iMaxReadLength)
				{
				CompleteRead(KErrNone, iMaxReadLength);
				}
			else if ((iBuf->Length() > 0) && (iCompleteErr == KErrEof))
				{
				// the read end point has reached the end of the stream
				// there is still some data remaining, but no new line
				// to just send the last bit
				CompleteRead(KErrNone, iBuf->Length());
				}
			break;
			}
		case RIoReadHandle::EOneOrMore:
			{
			if (iBuf->Length() > 0)
				{
				CompleteRead(KErrNone, iBuf->Length());
				}
			break;
			}
		}
	}

void CIoReadObject::CompleteRead(TInt aError, TInt aLength)
	{
	if (IorReadPending())
		{
		OBJ_NAME(*this);

		if ((aError == KErrNone)||(aError == KErrEof))
			{
			LOG(CIoLog::Printf(_L("Read object \'%S\' writing %d characters"), &objName, aLength));
			TPtrC ptr(iBuf->Des().Ptr(), aLength);
			LOG(Dump(ptr));
			TRAPD(err, MessageWriteL(iReadMessage, 0, ptr));
			if (err == KErrNone)
				{
				iBuf->Des().Delete(0, aLength);
				}
			else
				{
				aError = err;
				}
			}
		
		LOG(CIoLog::Printf(_L("Read object \'%S\' completing read message with %d"), &objName, aError));
		Complete(iReadMessage, aError);
		}
	else if (aError)
		{
		iCompleteErr = aError;
		}
	}

void CIoReadObject::CompleteReadKey(TInt aError, RIoConsoleReadHandle::TConsoleKey& aConsoleKey)
	{
	if (aError == KErrNone)
		{
		TPckgC<RIoConsoleReadHandle::TConsoleKey> consoleKeyPckg(aConsoleKey);
		TRAP(aError, MessageWriteL(iReadKeyMessage, 0, consoleKeyPckg));
		}
	Complete(iReadKeyMessage, aError);
	}

MIoReadEndPoint* CIoReadObject::ReadEndPoint() const
	{
	return static_cast<MIoReadEndPoint*>(iEndPoint);
	}

CIoReadObject::TCapturedKey::TCapturedKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers)
	: iKeyCode(aKeyCode), iModifierMask(aModifierMask), iModifiers(aModifiers)
	{
	}


//
// CIoWriteObject.
//

CIoWriteObject* CIoWriteObject::NewLC(TInt aId)
	{
	CIoWriteObject* self = new(ELeave) CIoWriteObject(aId);
	LOG(CIoLog::Printf(_L("Write object 0x%08x created"), self));
	CleanupClosePushL(*self);
	return self;
	}

CIoWriteObject::~CIoWriteObject()
	{
	OBJ_NAME(*this);
	LOG(CIoLog::Printf(_L("Write object \"%S\" (0x%08x) destroying"), &objName, this));
	if (iEndPoint)
		{
		WriteEndPoint()->IowepDetach(*this);
		}

	CompleteIfPending(iWriteMessage, KErrSessionClosed);
	CompleteIfPending(iGetCursorPosMsg, KErrSessionClosed);
	CompleteIfPending(iSetCursorPosAbsMsg, KErrSessionClosed);
	CompleteIfPending(iSetCursorPosRelMsg, KErrSessionClosed);
	CompleteIfPending(iSetCursorHeightMsg, KErrSessionClosed);
	CompleteIfPending(iSetTitleMsg, KErrSessionClosed);
	CompleteIfPending(iClearScreenMsg, KErrSessionClosed);
	CompleteIfPending(iClearToEndOfLineMsg, KErrSessionClosed);
	CompleteIfPending(iGetScreenSizeMsg, KErrSessionClosed);
	CompleteIfPending(iSetAttributesMsg, KErrSessionClosed);
	}

void CIoWriteObject::DuplicateL(const CIoWriteObject& aDuplicate)
	{
	DoDuplicateL(aDuplicate);
	if (aDuplicate.iEndPoint)
		{
		AttachL(*aDuplicate.WriteEndPoint());
		}
	iIsStdErr = aDuplicate.iIsStdErr;
	}

void CIoWriteObject::AttachL(MIoWriteEndPoint& aEndPoint)
	{
	if (iEndPoint)
		{
		WriteEndPoint()->IowepDetach(*this);
		}

	iEndPoint = &aEndPoint;
	TRAPD(err, aEndPoint.IowepAttachL(*this));
	if (err)
		{
		iEndPoint = NULL;
		User::Leave(err);
		}
	}

void CIoWriteObject::WriteL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicWriteWhenNotAttached));
	__ASSERT_RETURN(!MessagePending(iWriteMessage), PanicClient(aMessage, EPanicWriteAlreadyPending));
	iWriteMessage = aMessage;
	iOffset = 0;
	iWriteLength = DesLengthL(iWriteMessage, 0);
	WriteEndPoint()->IowepWriteL(*this);
	}

void CIoWriteObject::WriteCancel(const CIoSession& aSession)
	{
	if (MessagePending(iWriteMessage) && (iWriteMessage.Session() == &aSession))
		{
		if (iEndPoint)
			{
			WriteEndPoint()->IowepWriteCancel(*this);
			}
		Complete(iWriteMessage, KErrCancel);
		}
	}

TInt CIoWriteObject::IowWriteLength() const
	{
	ASSERT(MessagePending(iWriteMessage));
	return (iWriteLength - iOffset);
	}

TInt CIoWriteObject::IowWrite(TDes& aBuf)
	{
	ASSERT(MessagePending(iWriteMessage));
	ASSERT(aBuf.Length() <= (iWriteLength - iOffset));
	OBJ_NAME(*this);
	TRAPD(err, MessageReadL(iWriteMessage, 0, aBuf, iOffset));
	if (err == KErrNone)
		{
		LOG(CIoLog::Printf(_L("Write object \'%S\' read %d characters"), &objName, aBuf.Length()));
		LOG(Dump(aBuf));
		iOffset += aBuf.Length();
		}
	else
		{
		LOG(CIoLog::Printf(_L("Write object \'%S\' failed to read: %d"), &objName, err));
		}
	return err;
	}

RIoReadWriteHandle::TMode CIoWriteObject::IowWriteMode() const
	{
	return iMode;
	}
	
void CIoWriteObject::IowCursorPos(TInt aError, TPoint aPos)
	{
	TPckgC<TPoint> posPckg(aPos);
	Complete(iGetCursorPosMsg, aError==KErrNone ? MessageWrite(iGetCursorPosMsg, 0, posPckg) : aError);
	}

void CIoWriteObject::IowSetCursorPosAbsComplete(TInt aError)
	{
	Complete(iSetCursorPosAbsMsg, aError);
	}

void CIoWriteObject::IowSetCursorPosRelComplete(TInt aError)
	{
	Complete(iSetCursorPosRelMsg, aError);
	}
	
void CIoWriteObject::IowSetCursorHeightComplete(TInt aError)
	{
	Complete(iSetCursorHeightMsg, aError);
	}
	
void CIoWriteObject::IowSetTitleComplete(TInt aError)
	{
	Complete(iSetTitleMsg, aError);
	}
	
void CIoWriteObject::IowClearScreenComplete(TInt aError)
	{
	Complete(iClearScreenMsg, aError);
	}

void CIoWriteObject::IowClearToEndOfLineComplete(TInt aError)
	{
	Complete(iClearToEndOfLineMsg, aError);
	}

void CIoWriteObject::IowScreenSize(TInt aError, TSize aSize)
	{
	if ((aSize == TSize(0, 0)) && Console() && !iTriedConsoleScreenSize)
		{
		iTriedConsoleScreenSize = ETrue;
		TRAPD(err, Console()->IowepScreenSizeL(*this));
		if (err!=KErrNone)
			{
			Complete(iGetScreenSizeMsg, err);
			}
		return;
		}
	TPckgC<TSize> sizePckg(aSize);
	Complete(iGetScreenSizeMsg, aError == KErrNone ? MessageWrite(iGetScreenSizeMsg, 0, sizePckg) : aError);
	}

void CIoWriteObject::IowSetAttributesComplete(TInt aError)
	{
	Complete(iSetAttributesMsg, aError);
	}

HBufC* CIoWriteObject::IowTitleLC()
	{
	HBufC* titleBuf = HBufC::NewLC(DesLengthL(iSetTitleMsg, 0));
	TPtr titlePtr(titleBuf->Des());
	MessageReadL(iSetTitleMsg, 0, titlePtr);
	return titleBuf;
	}

void CIoWriteObject::CursorPosL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicCursorPosWhenNotAttached));
	__ASSERT_RETURN(iGetCursorPosMsg.IsNull(), PanicClient(aMessage, EPanicCursorPosAlreadyPending));
	iGetCursorPosMsg = aMessage;
	CleanupNullMessagePushL(iGetCursorPosMsg);
	WriteEndPoint()->IowepCursorPosL(*this);
	CleanupStack::Pop(&iGetCursorPosMsg);
	}

void CIoWriteObject::SetCursorPosAbsL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicSetCursorPosAbsWhenNotAttached));
	__ASSERT_RETURN(iSetCursorPosAbsMsg.IsNull(), PanicClient(aMessage, EPanicSetCursorPosAlreadyPending));
	TPoint pos;
	TPckg<TPoint> posPckg(pos);
	MessageReadL(aMessage, 0, posPckg);
	iSetCursorPosAbsMsg = aMessage;
	CleanupNullMessagePushL(iSetCursorPosAbsMsg);
	WriteEndPoint()->IowepSetCursorPosAbsL(pos, *this);
	CleanupStack::Pop(&iSetCursorPosAbsMsg);
	}

void CIoWriteObject::SetCursorPosRelL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicSetCursorPosRelWhenNotAttached));
	__ASSERT_RETURN(iSetCursorPosRelMsg.IsNull(), PanicClient(aMessage, EPanicSetCursorPosAlreadyPending));
	TPoint pos;
	TPckg<TPoint> posPckg(pos);
	MessageReadL(aMessage, 0, posPckg);
	iSetCursorPosRelMsg = aMessage;
	CleanupNullMessagePushL(iSetCursorPosRelMsg);
	WriteEndPoint()->IowepSetCursorPosRelL(pos, *this);
	CleanupStack::Pop(&iSetCursorPosRelMsg);
	}

void CIoWriteObject::SetCursorHeightL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicSetCursorHeightWhenNotAttached));
	__ASSERT_RETURN(iSetCursorHeightMsg.IsNull(), PanicClient(aMessage, EPanicSetCursorHeightAlreadyPending));
	iSetCursorHeightMsg = aMessage;
	CleanupNullMessagePushL(iSetCursorHeightMsg);
	WriteEndPoint()->IowepSetCursorHeightL(aMessage.Int0(), *this);
	CleanupStack::Pop(&iSetCursorHeightMsg);
	}

void CIoWriteObject::SetTitleL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicSetTitleWhenNotAttached));
	__ASSERT_RETURN(iSetTitleMsg.IsNull(), PanicClient(aMessage, EPanicSetTitleAlreadyPending));
	iSetTitleMsg = aMessage;
	// if we leave here, the message will be completed by the framework. But we do want to null the message
	// as it will no longer be valid.
	CleanupNullMessagePushL(iSetTitleMsg);
	WriteEndPoint()->IowepSetTitleL(*this);
	CleanupStack::Pop(&iSetTitleMsg);
	}

void CIoWriteObject::ClearScreen(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicClearScreenWhenNotAttached));
	__ASSERT_RETURN(iClearScreenMsg.IsNull(), PanicClient(aMessage, EPanicClearScreenAlreadyPending));
	iClearScreenMsg = aMessage;
	CleanupNullMessagePushL(iClearScreenMsg);
	WriteEndPoint()->IowepClearScreenL(*this);
	CleanupStack::Pop(&iClearScreenMsg);
	}

void CIoWriteObject::ClearToEndOfLine(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicClearToEndOfLineWhenNotAttached));
	__ASSERT_RETURN(iClearToEndOfLineMsg.IsNull(), PanicClient(aMessage, EPanicClearToEndOfLineAlreadyPending));
	iClearToEndOfLineMsg = aMessage;
	CleanupNullMessagePushL(iClearToEndOfLineMsg);
	WriteEndPoint()->IowepClearToEndOfLineL(*this);
	CleanupStack::Pop(&iClearToEndOfLineMsg);
	}

void CIoWriteObject::ScreenSizeL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicScreenSizeWhenNotAttached));
	__ASSERT_RETURN(iGetScreenSizeMsg.IsNull(), PanicClient(aMessage, EPanicScreenSizeAlreadyPending));
	iGetScreenSizeMsg = aMessage;
	CleanupNullMessagePushL(iGetScreenSizeMsg);
	iTriedConsoleScreenSize = EFalse;
	WriteEndPoint()->IowepScreenSizeL(*this);
	CleanupStack::Pop(&iGetScreenSizeMsg);
	}

void CIoWriteObject::SetAttributesL(const RMsg& aMessage)
	{
	__ASSERT_RETURN(iEndPoint, PanicClient(aMessage, EPanicSetAttributesWhenNotAttached));
	__ASSERT_RETURN(iSetAttributesMsg.IsNull(), PanicClient(aMessage, EPanicSetAttributesAlreadyPending));
	iSetAttributesMsg = aMessage;
	CleanupNullMessagePushL(iSetAttributesMsg);
	WriteEndPoint()->IowepSetAttributesL((TUint)aMessage.Int0(), (ConsoleAttributes::TColor)aMessage.Int1(), (ConsoleAttributes::TColor)aMessage.Int2(), *this);
	CleanupStack::Pop(&iSetAttributesMsg);
	}

TBool CIoWriteObject::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EReadWriteObject) || (aType == RIoHandle::EWriteObject));
	}

void CIoWriteObject::SessionClosed(const CIoSession& aSession)
	{
	WriteCancel(aSession);
	}

RIoReadWriteHandle::TMode CIoWriteObject::IorwMode() const
	{
	return iMode;
	}

void CIoWriteObject::IowComplete(TInt aError)
	{
	ASSERT(MessagePending(iWriteMessage));
	Complete(iWriteMessage, aError);
	iOffset = 0;
	iWriteLength = 0;
	}

TName CIoWriteObject::IowName()
	{
	return Name();
	}

CIoWriteObject::CIoWriteObject(TInt aId)
	: CIoReadWriteObject(aId)
	{
	}

MIoWriteEndPoint* CIoWriteObject::WriteEndPoint() const
	{
	return static_cast<MIoWriteEndPoint*>(iEndPoint);
	}

void CIoWriteObject::SetIsStdErr(TBool aFlag)
	{
	iIsStdErr = aFlag;
	}

TBool CIoWriteObject::IowIsStdErr() const
	{
	return iIsStdErr;
	}
