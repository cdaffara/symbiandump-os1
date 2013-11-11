// console.cpp
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

#include <e32cons.h>
#include <e32uid.h>
#include "server.h"
#include "console.h"
#include "log.h"


#ifdef IOSRV_LOGGING
#define CONSOLE_NAME TName consoleName(Name())
#define READER_NAME(x) TName readerName((x).IorName())
#define WRITER_NAME(x) TName writerName((x).IowName())
#else
#define CONSOLE_NAME
#define READER_NAME(x)
#define WRITER_NAME(x)
#endif

const TInt KConsoleThreadMinHeapSize = 0x1000;
const TInt KConsoleThreadMaxHeapSize = 0x1000000;

CIoConsole* CIoConsole::NewLC(const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, const TIoConfig& aConfig, CIoConsole* aUnderlying, TUint aOptions)
	{
	CIoConsole* self = new(ELeave) CIoConsole(aConfig);
	CleanupClosePushL(*self);
	self->ConstructL(aImplementation, aTitle, aSize, aUnderlying, aOptions);
	return self;
	}

CIoConsole::~CIoConsole()
	{
	// if the sub-thread is servicing a request of ours now, we have to kill it. Otherwise, we might hang
	// when we try and cancel the request AO, if (for example) it's servicing a long-standing Create request.
	if (iRequestAo && (iRequestAo->IsActive()) && (iServerThread.Handle()!=KNullHandle))
		{
		iServerThread.Kill(KErrCancel);
		iThreadServer.Close(); // this won't be closed by the sub-thread, as we just killed it.
		}
	delete iThreadWatcher;
	delete iRequestAo;
	iRequestQueue.Close();
	delete iImplementation;
	delete iReader;
	iConsole.Close();
	delete iCreationTitle ;
	iServerThread.Close();
	}

const TDesC& CIoConsole::Implementation() const
	{
	return *iImplementation;
	}

TBool CIoConsole::IsType(RIoHandle::TType aType) const
	{
	return ((aType == RIoHandle::EEndPoint) || (aType == RIoHandle::EConsole));
	}

void CIoConsole::HandleReaderDetached(MIoReader& aReader)
	{
	HandleReadWriterDetached(aReader);
	}

void CIoConsole::HandleWriterDetached(MIoWriter& aWriter)
	{
	HandleReadWriterDetached(aWriter);
	}

template <class T> void CIoConsole::HandleReadWriterDetached(T& aReadWriter)
	{
	// Remove pending requests originated from this reader / writer.
	for (TInt i = (iRequestQueue.Count() - 1); i >= 0; --i)
		{
		TConsoleRequest* request = iRequestQueue[i];
		if (request->OriginatedFrom(aReadWriter))
			{
			delete request;
			iRequestQueue.Remove(i);
			}
		}

	// If this reader / writer originated the request that's currently being process, abort it.
	if (iRequestAo->CurrentRequest() && iRequestAo->CurrentRequest()->OriginatedFrom(aReadWriter))
		{
		iRequestAo->Abort();
		}
	}

void CIoConsole::IorepReadL(MIoReader&)
	{
	if (!iReader->IsActive())
		{
		iReader->QueueRead();
		}
	}

void CIoConsole::IorepReadKeyL(MIoReader& aReader)
	{
	IorepReadL(aReader);
	}

void CIoConsole::IorepSetConsoleModeL(RIoReadWriteHandle::TMode aMode, MIoReader& aReader)
	{
	NewRequest(new(ELeave)TConsoleSetModeRequest(aReader, *this, aMode));
	}

void CIoConsole::IowepWriteL(MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleWriteRequest(aWriter));
	}

void CIoConsole::IowepWriteCancel(MIoWriter&)
	{
	}

void CIoConsole::IowepCursorPosL(MIoWriter& aWriter) const
	{
	NewRequest(new(ELeave)TConsoleCursorPosRequest(aWriter));
	}

void CIoConsole::IowepSetCursorPosAbsL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleSetCursorPosAbsRequest(aWriter, aPoint));
	}

void CIoConsole::IowepSetCursorPosRelL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleSetCursorPosRelRequest(aWriter, aPoint));
	}

void CIoConsole::IowepSetCursorHeightL(TInt aPercentage, MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleSetCursorHeightRequest(aWriter, aPercentage));
	}

void CIoConsole::IowepSetTitleL(MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleSetTitleRequest(aWriter));
	}

void CIoConsole::IowepClearScreenL(MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleClearScreenRequest(aWriter));
	}

void CIoConsole::IowepClearToEndOfLineL(MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleClearToEndOfLineRequest(aWriter));
	}

void CIoConsole::IowepSetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriter& aWriter)
	{
	NewRequest(new(ELeave)TConsoleSetAttributesRequest(aWriter, aAttributes, aForegroundColor, aBackgroundColor));
	}

void CIoConsole::IowepScreenSizeL(MIoWriter& aWriter) const
	{
	NewRequest(new(ELeave)TConsoleScreenSizeRequest(aWriter, iConfig));
	}

CIoConsole::CIoConsole(const TIoConfig& aConfig)
	: iConfig(aConfig), iDetectedSize(-1, -1)
	{
	}

void CIoConsole::ConstructL(const TDesC& aImplementation, const TDesC& aTitle, const TSize& aSize, CIoConsole* aUnderlying, TUint aOptions)
	{
	LOG(CIoLog::Printf(_L("Console 0x%08x created"), this));
	iCreationTitle = aTitle.AllocL();
	iCreationSize = aSize;
	
	iRequestAo = new(ELeave)CConsoleRequest(*this);
	
	if (aImplementation.Length())
		{
		iImplementation = aImplementation.AllocL();
		}
	else
		{
		iImplementation = iConfig.ConsoleImplementation().AllocL();
		}
		
	User::LeaveIfError(iConsole.Connect(CIoConsoleProxyServerNewL, iImplementation, *iImplementation, KDefaultStackSize, KConsoleThreadMinHeapSize, KConsoleThreadMaxHeapSize, iThreadServer, iServerThread));
	iThreadWatcher = new(ELeave)CServerDeathWatcher(iThreadServer, iServerThread);
	if (aOptions & RIoConsole::ELazyCreate)
		{
		User::LeaveIfError(iConsole.SetLazyConstruct());
		}
	if (iConfig.ConsoleSizeDetect())
		{
		User::LeaveIfError(iConsole.SetConsoleSizeDetect());
		}
	if (aUnderlying)
		{
		User::LeaveIfError(aUnderlying->Open());
		CleanupClosePushL(*aUnderlying);
		NewRequest(new(ELeave)TConsoleSetUnderlyingRequest(*aUnderlying));
		CleanupStack::Pop();
		}
	NewRequest(new(ELeave)TConsoleCreateRequest(*this));
		
	iReader = CConsoleReader::NewL(*this);
	}
	
void CIoConsole::CreateComplete(TInt aError)
	{
	iCreateStatus = aError;
	}
	
void CIoConsole::NewRequest(TConsoleRequest* aRequest) const
	{
	ASSERT(aRequest);
	TInt err = iRequestQueue.Append(aRequest);
	if (err!=KErrNone)
		{
		aRequest->CompleteD(err);
		return;
		}

	CheckQueue();
	}
	
void CIoConsole::CheckQueue() const
	{
	if (iCreateStatus != KErrNone)
		{
		while (iRequestQueue.Count())
			{
			TConsoleRequest* req = iRequestQueue[0];
			iRequestQueue.Remove(0);
			req->CompleteD(iCreateStatus);
			}
		return;
		}
	if ((!iRequestAo->IsActive()) && (iRequestQueue.Count()))
		{
		iRequestAo->Service(iRequestQueue[0]);
		iRequestQueue.Remove(0);
		}
	}
	
void CIoConsole::ConsoleDied()
	{
	iCreateStatus = KErrGeneral;

	iReader = CConsoleReader::NewL(*this);
	}

void CIoConsole::ReadComplete(TInt aError)
	{
	MIoReader* foregroundReader = AttachedReader(0);
	if (foregroundReader)
		{
		foregroundReader->IorReadKeyComplete(aError, 0, 0);
		}
	}

void CIoConsole::ReadComplete(TUint aKeyCode, TUint aModifiers)
	{
	TInt index = 0;
	MIoReader* foregroundReader = AttachedReader(index++);
	MIoReader* reader = foregroundReader;
	TBool keyHandled(EFalse);
	while (reader)
		{
		if (reader->IorIsKeyCaptured(aKeyCode, aModifiers))
			{
			if (reader->IorReadPending())
				{
				TPtrC keyCodePtr((TUint16*)&aKeyCode, 1);
				reader->IorReadBuf().Append(keyCodePtr);
				reader->IorDataBuffered(1);
				}
			else
				{
				reader->IorReadKeyComplete(KErrNone, aKeyCode, aModifiers);
				}
			keyHandled = ETrue;
			break;
			}
		reader = AttachedReader(index++);;
		}

	// Key not captured, so send to foreground (i.e. the first) reader.
	if (!keyHandled && foregroundReader)
		{
		foregroundReader->IorReadKeyComplete(KErrNone, aKeyCode, aModifiers);
		}

	QueueReaderIfRequired();
	}

void CIoConsole::QueueReaderIfRequired()
	{
	TBool pendingReader(EFalse);
	TInt index = 0;
	MIoReader* reader = AttachedReader(index++);
	while (reader)
		{
		if (reader->IorReadPending() || reader->IorReadKeyPending())
			{
			pendingReader = ETrue;
			break;
			}
		reader = AttachedReader(index++);
		}

	if (pendingReader && !iReader->IsActive())
		{
		iReader->QueueRead();
		}
	else if (!pendingReader && iReader->IsActive())
		{
		iReader->Cancel();
		}
	}

CIoConsole::CConsoleReader* CIoConsole::CConsoleReader::NewL(CIoConsole& aConsole)
	{
	return new(ELeave) CConsoleReader(aConsole);
	}

CIoConsole::CConsoleReader::~CConsoleReader()
	{
	Cancel();
	}

void CIoConsole::CConsoleReader::QueueRead()
	{
	iConsole.iConsole.Read(iKeyCodePckg, iKeyModifiersPckg, iStatus);
	SetActive();
	}

CIoConsole::CConsoleReader::CConsoleReader(CIoConsole& aConsole)
	: CActive(CActive::EPriorityStandard), iConsole(aConsole)
	, iKeyCodePckg(iKeyCode), iKeyModifiersPckg(iKeyModifiers)
	{
	CActiveScheduler::Add(this);
	}

void CIoConsole::CConsoleReader::RunL()
	{
	TInt err = iStatus.Int();
	if (err==KErrServerTerminated)
		{
		iConsole.ConsoleDied();
		err = KErrGeneral;
		}
	if (err)
		{
		iConsole.ReadComplete(err);
		}
	else
		{
		iConsole.ReadComplete(iKeyCode, iKeyModifiers);
		}
	}

void CIoConsole::CConsoleReader::DoCancel()
	{
	iConsole.iConsole.ReadCancel();
	}

//______________________________________________________________________________
//						TConsoleRequest
void CIoConsole::TConsoleRequest::PrepareL()
	{
	}

TBool CIoConsole::TConsoleRequest::OriginatedFrom(MIoReader&) const
	{ 
	return EFalse; 
	}

TBool CIoConsole::TConsoleRequest::OriginatedFrom(MIoWriter&) const
	{
	return EFalse;
	}

//______________________________________________________________________________
//						TConsoleWriterRequest
CIoConsole::TConsoleWriterRequest::TConsoleWriterRequest(MIoWriter& aWriter)
	: iWriter(aWriter)
	{
	}

TBool CIoConsole::TConsoleWriterRequest::OriginatedFrom(MIoWriter& aWriter) const
	{
	return (&iWriter == &aWriter);
	}

//______________________________________________________________________________
//						TConsoleReaderRequest
CIoConsole::TConsoleReaderRequest::TConsoleReaderRequest(MIoReader& aReader)
	: iReader(aReader)
	{
	}

TBool CIoConsole::TConsoleReaderRequest::OriginatedFrom(MIoReader& aReader) const
	{
	return (&iReader == &aReader);
	}

//______________________________________________________________________________
//						TConsoleCreateRequest
CIoConsole::TConsoleCreateRequest::TConsoleCreateRequest(CIoConsole& aOwner)
	: iOwner(aOwner)
	{
	}

void CIoConsole::TConsoleCreateRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.Create(*iOwner.iCreationTitle, iOwner.iCreationSize, aStatus);
	}

void CIoConsole::TConsoleCreateRequest::CompleteD(TInt aError)
	{
	iOwner.CreateComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleWriteRequest
CIoConsole::TConsoleWriteRequest::TConsoleWriteRequest(MIoWriter& aWriter)
	: TConsoleWriterRequest(aWriter), iBuf(NULL)
	{
	}

void CIoConsole::TConsoleWriteRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	if (iWriter.IowIsStdErr())
		{
		aProxy.WriteStdErr(*iBuf, aStatus);
		}
	else
		{
		aProxy.Write(*iBuf, aStatus);
		}
	}

void CIoConsole::TConsoleWriteRequest::PrepareL()
	{
	const TInt length = iWriter.IowWriteLength();
	if (iBuf == NULL)
		{
		iBuf = HBufC::NewL(length);
		}
	else if (iBuf->Des().MaxLength() < length)
		{
		iBuf = iBuf->ReAllocL(length);
		}

	TPtr bufPtr(iBuf->Des());
	bufPtr.Zero();
	iWriter.IowWrite(bufPtr);

	if (iWriter.IorwMode() == RIoReadWriteHandle::EText)
		{
		// Fix line endings (change LF to CRLF).
		RArray<TInt> indicies(5);
		CleanupClosePushL(indicies);
		_LIT(KCarriageReturn, "\r");
		for (TInt i = 0; i < length; ++i)
			{
			if ((*iBuf)[i] == '\n')
				{
				if ((i == 0) || ((*iBuf)[i - 1] != '\r'))
					{
					User::LeaveIfError(indicies.Append(i));
					}
				}
			}
		const TInt count = indicies.Count();
		if (count > 0)
			{
			if (bufPtr.MaxLength() < (length + count))
				{
				iBuf = iBuf->ReAllocL(length + count);
				bufPtr.Set(iBuf->Des());
				}
			for (TInt i = (count - 1); i >= 0; --i)
				{
				bufPtr.Insert(indicies[i], KCarriageReturn);
				}
			}
		CleanupStack::PopAndDestroy(&indicies);
		}
	
	}

void CIoConsole::TConsoleWriteRequest::CompleteD(TInt aError)
	{
	delete iBuf;
	iWriter.IowComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleCursorPosRequest
CIoConsole::TConsoleCursorPosRequest::TConsoleCursorPosRequest(MIoWriter& aWriter)
	: TConsoleWriterRequest(aWriter), iPosPckg(iPos)
	{
	}

void CIoConsole::TConsoleCursorPosRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.CursorPos(iPosPckg, aStatus);
	}

void CIoConsole::TConsoleCursorPosRequest::CompleteD(TInt aError)
	{
	iWriter.IowCursorPos(aError, iPos);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetCursorPosAbsRequest
CIoConsole::TConsoleSetCursorPosAbsRequest::TConsoleSetCursorPosAbsRequest(MIoWriter& aWriter, const TPoint& aPoint)
	: TConsoleWriterRequest(aWriter), iPoint(aPoint)
	{
	}

void CIoConsole::TConsoleSetCursorPosAbsRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetCursorPosAbs(iPoint, aStatus);	
	}

void CIoConsole::TConsoleSetCursorPosAbsRequest::CompleteD(TInt aError)
	{
	iWriter.IowSetCursorPosAbsComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetCursorPosRelRequest
CIoConsole::TConsoleSetCursorPosRelRequest::TConsoleSetCursorPosRelRequest(MIoWriter& aWriter, const TPoint& aPoint)
	: TConsoleWriterRequest(aWriter), iPoint(aPoint)
	{
	}

void CIoConsole::TConsoleSetCursorPosRelRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetCursorPosRel(iPoint, aStatus);
	}

void CIoConsole::TConsoleSetCursorPosRelRequest::CompleteD(TInt aError)
	{
	iWriter.IowSetCursorPosRelComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetCursorHeightRequest
CIoConsole::TConsoleSetCursorHeightRequest::TConsoleSetCursorHeightRequest(MIoWriter& aWriter, TInt aHeight)
	: TConsoleWriterRequest(aWriter), iHeight(aHeight)
	{
	}	

void CIoConsole::TConsoleSetCursorHeightRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetCursorHeight(iHeight, aStatus);
	}

void CIoConsole::TConsoleSetCursorHeightRequest::CompleteD(TInt aError)
	{
	iWriter.IowSetCursorHeightComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetTitleRequest
CIoConsole::TConsoleSetTitleRequest::TConsoleSetTitleRequest(MIoWriter& aWriter)
	: TConsoleWriterRequest(aWriter)
	{
	}

void CIoConsole::TConsoleSetTitleRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetTitle(*iTitle, aStatus);
	}

void CIoConsole::TConsoleSetTitleRequest::PrepareL()
	{
	iTitle = iWriter.IowTitleLC();
	CleanupStack::Pop(iTitle);
	}

void CIoConsole::TConsoleSetTitleRequest::CompleteD(TInt aError)
	{
	delete iTitle;
	iWriter.IowSetTitleComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleClearScreenRequest
CIoConsole::TConsoleClearScreenRequest::TConsoleClearScreenRequest(MIoWriter& aWriter)
	: TConsoleWriterRequest(aWriter)
	{
	}

void CIoConsole::TConsoleClearScreenRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.ClearScreen(aStatus);
	}

void CIoConsole::TConsoleClearScreenRequest::CompleteD(TInt aError)
	{
	iWriter.IowClearScreenComplete(aError);
	delete this;
	}

		
	

//______________________________________________________________________________
//						TConsoleClearToEndOfLineRequest
CIoConsole::TConsoleClearToEndOfLineRequest::TConsoleClearToEndOfLineRequest(MIoWriter& aWriter)
	: TConsoleWriterRequest(aWriter)
	{
	}

void CIoConsole::TConsoleClearToEndOfLineRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.ClearToEndOfLine(aStatus);
	}

void CIoConsole::TConsoleClearToEndOfLineRequest::CompleteD(TInt aError)
	{
	iWriter.IowClearToEndOfLineComplete(aError);
	delete this;
	}

		
	

//______________________________________________________________________________
//						TConsoleScreenSizeRequest
CIoConsole::TConsoleScreenSizeRequest::TConsoleScreenSizeRequest(MIoWriter& aWriter, const TIoConfig& aConfig)
	: TConsoleWriterRequest(aWriter), iConfig(aConfig), iSizeBuf(iSize)
	{
	}

void CIoConsole::TConsoleScreenSizeRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.GetScreenSize(iSizeBuf, aStatus);
	}

void CIoConsole::TConsoleScreenSizeRequest::CompleteD(TInt aError)
	{
	if (aError==KErrNone)
		{
		iSize.iWidth += iConfig.ConsoleSizeAdjustment().iWidth;
		iSize.iHeight += iConfig.ConsoleSizeAdjustment().iHeight;
		}
	iWriter.IowScreenSize(aError, iSize);
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetAttributesRequest
CIoConsole::TConsoleSetAttributesRequest::TConsoleSetAttributesRequest(MIoWriter& aWriter, TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	: TConsoleWriterRequest(aWriter), iAttributes(aAttributes), iForegroundColor(aForegroundColor), iBackgroundColor(aBackgroundColor)
	{
	}

void CIoConsole::TConsoleSetAttributesRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetAttributes(iAttributes, iForegroundColor, iBackgroundColor, aStatus);
	}

void CIoConsole::TConsoleSetAttributesRequest::CompleteD(TInt aError)
	{
	iWriter.IowSetAttributesComplete(aError);
	delete this;
	}

		
//______________________________________________________________________________
//						TConsoleSetUnderlyingRequest
CIoConsole::TConsoleSetUnderlyingRequest::TConsoleSetUnderlyingRequest(CIoConsole& aUnderlyingConsole)
	: iConsole(aUnderlyingConsole)
	{
	}

void CIoConsole::TConsoleSetUnderlyingRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	aProxy.SetUnderlyingConsole(iSession, aStatus);
	}

void CIoConsole::TConsoleSetUnderlyingRequest::PrepareL()
	{
	if (iConsole.iThreadServer.Handle())
		{
		User::LeaveIfError(iSession.Connect(iConsole.iThreadServer));
		}
	else
		{
		User::Leave(KErrBadHandle);
		}	
	}

void CIoConsole::TConsoleSetUnderlyingRequest::CompleteD(TInt)
	{
	iSession.Close();
	iConsole.Close();
	delete this;
	}

//______________________________________________________________________________
//						TConsoleSetModeRequest
CIoConsole::TConsoleSetModeRequest::TConsoleSetModeRequest(MIoReader& aReader,CIoConsole& aConsole, RIoReadWriteHandle::TMode aMode)
	: TConsoleReaderRequest(aReader), iConsole(aConsole), iMode(aMode)
	{
	}

void CIoConsole::TConsoleSetModeRequest::Request(RIoConsoleProxy aProxy, TRequestStatus& aStatus)
	{
	iConsole.iReader->Cancel();
	aProxy.SetConsoleMode(iMode, aStatus);
	}

void CIoConsole::TConsoleSetModeRequest::CompleteD(TInt aError)
	{
	iConsole.QueueReaderIfRequired();
	iReader.IorSetConsoleModeComplete(aError);
	delete this;
	}

//______________________________________________________________________________
//						CConsoleRequest
CIoConsole::CConsoleRequest::CConsoleRequest(CIoConsole& aConsole)
	: CActive(EPriorityStandard), iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}
		
void CIoConsole::CConsoleRequest::Service(TConsoleRequest* aRequest)
	{
	ASSERT(!IsActive());
	ASSERT(!iCurrentRequest);
	TRAPD(err, aRequest->PrepareL());
	if (err!=KErrNone)
		{
		Complete(aRequest, err);
		return;
		}
	aRequest->Request(iConsole.iConsole, iStatus);
	SetActive();
	iCurrentRequest = aRequest;
	}
	
void CIoConsole::CConsoleRequest::Complete(TConsoleRequest* aRequest, TInt aError)
	{
	if (aError == KErrServerTerminated)
		{
		// console has panicked.
		iConsole.ConsoleDied();
		// don't want to send KErrServerTerminated up to our (iosrv) clients, as it will
		// make them think the iosrv has died.
		aError = KErrGeneral;
		}
	if (aRequest)
		{
		aRequest->CompleteD(aError);
		}
	iConsole.CheckQueue();
	}


CIoConsole::CConsoleRequest::~CConsoleRequest()
	{
	Cancel();
	}
	
void CIoConsole::CConsoleRequest::RunL()
	{
	TConsoleRequest* req = iCurrentRequest;
	iCurrentRequest = NULL;
	Complete(req, iStatus.Int());
	}

void CIoConsole::CConsoleRequest::DoCancel()
	{
	// request are handled synchronously on the server side, no cancelling is possible.
	}

const CIoConsole::TConsoleRequest* CIoConsole::CConsoleRequest::CurrentRequest() const
	{
	return iCurrentRequest;
	}

void CIoConsole::CConsoleRequest::Abort()
	{
	// We can't cancel a pending request (because they are handled synchronously on the server side),
	// so instead we delete and NULL the associated request object. This active object will then
	// continue to wait for the server to complete the request (as normal), but will take no further
	// action. This is used when the originating MIoReader or MIoWriter object has been detached from
	// the console and no longer exists.
	ASSERT(IsActive());
	ASSERT(iCurrentRequest);
	delete iCurrentRequest;
	iCurrentRequest = NULL;
	}


//______________________________________________________________________________
//						CServerDeathWatcher
CIoConsole::CServerDeathWatcher::CServerDeathWatcher(RServer2& aServer, RThread& aThread)
	: CActive(CActive::EPriorityStandard), iServer(aServer), iThread(aThread)
	{
	CActiveScheduler::Add(this);
	aThread.Logon(iStatus);
	SetActive();
	}
	
CIoConsole::CServerDeathWatcher::~CServerDeathWatcher()
	{
	Cancel();
	}

void CIoConsole::CServerDeathWatcher::RunL()
	{
	}

void CIoConsole::CServerDeathWatcher::DoCancel()
	{
	iThread.LogonCancel(iStatus);
	}

//______________________________________________________________________________
//						RIoConsoleProxy
TInt RIoConsoleProxy::SetConsoleSizeDetect()
	{
	return SendReceive(ESetConsoleSizeDetect);
	}
	
TInt RIoConsoleProxy::SetLazyConstruct()
	{
	return SendReceive(ESetLazyConstruct);
	}
	
void RIoConsoleProxy::SetConsoleMode(RIoReadWriteHandle::TMode aMode, TRequestStatus& aStatus)
	{
	SendReceive(ESetConsoleMode, TIpcArgs(aMode), aStatus);
	}
	
void RIoConsoleProxy::SetUnderlyingConsole(const RIoConsoleProxy& aUnderlyingSession, TRequestStatus& aStatus)
	{
	SendReceive(ESetUnderlyingConsole, TIpcArgs(aUnderlyingSession), aStatus);
	}
	
TInt RIoConsoleProxy::OpenExisting()
	{
	return SendReceive(EOpenExistingConsole);
	}

void RIoConsoleProxy::WriteStdErr(const TDesC& aDescriptor, TRequestStatus& aStatus)
	{
	SendReceive(EWriteStdErr, TIpcArgs(&aDescriptor), aStatus);
	}

//______________________________________________________________________________
//						CIoConsoleProxyServer
CConsoleProxyServer* CIoConsoleProxyServerNewL(TAny* aParams)
	{
	const TDesC* dllName = (const TDesC*)aParams;
	RLibrary lib;

	User::LeaveIfError(lib.Load(*dllName));
	CleanupClosePushL(lib);
	if ((lib.Type()[1] == KSharedLibraryUid) && (lib.Type()[2] == KConsoleDllUid))
		{
		TConsoleCreateFunction entry = (TConsoleCreateFunction)lib.Lookup(1);
		if (!entry) User::Leave(KErrNotSupported);
		CleanupStack::Pop(&lib);
		return CIoConsoleProxyServer::NewL(entry, lib);
		}
	else
		{
		User::Leave(KErrNotSupported);
		return NULL; // ASSERT(Happy(compiler))
		}
	}
	
CIoConsoleProxyServer* CIoConsoleProxyServer::NewL(TConsoleCreateFunction aConsoleCreate, RLibrary& aConsoleLibrary)
	{
	CIoConsoleProxyServer* self = new CIoConsoleProxyServer(aConsoleCreate, aConsoleLibrary);
	if (!self)
		{
		aConsoleLibrary.Close();
		User::Leave(KErrNoMemory);
		}
	CleanupStack::PushL(self);
	self->ConstructL(KNullDesC);
	CleanupStack::Pop(self);
	return self;
	}
	
CIoConsoleProxyServer::CIoConsoleProxyServer(TConsoleCreateFunction aConsoleCreate, const RLibrary& aConsoleLibrary)
	: CConsoleProxyServer(aConsoleCreate, CActive::EPriorityStandard)
	, iConsoleLibrary(aConsoleLibrary)
	{
	}

CSession2* CIoConsoleProxyServer::NewSessionL(const TVersion&,const RMessage2&) const
	{
	return new(ELeave)CIoConsoleProxySession(iConsoleCreate);
	}
	
CIoConsoleProxyServer::~CIoConsoleProxyServer()
	{
	iConsoleLibrary.Close();
	}
	
MProxiedConsole* CIoConsoleProxyServer::TheConsole() const
	{
	return iTheConsole;
	}
	
void CIoConsoleProxyServer::SetTheConsole(MProxiedConsole* aConsole)
	{
	ASSERT(!iTheConsole);
	iTheConsole = aConsole;
	}

//______________________________________________________________________________
TSize DetectConsoleSize(CConsoleBase* aConsole)
	{
	TSize detectedSize;
	aConsole->SetCursorHeight(0);
	aConsole->ScreenSize(); // This used to be assigned to a variable, which was never used, but I'm not sure if calling ScreenSize() has side-effects so I'm leaving the call in
	aConsole->SetCursorPosAbs(TPoint(0, 0));
	_LIT(KSpace, " ");
	for (TInt x = 0; ; ++x)
		{
		aConsole->Write(KSpace);
		if (aConsole->CursorPos().iX == 0)
			{
			detectedSize.iWidth = x + 1;
			break;
			}
		}
	aConsole->SetCursorPosAbs(TPoint(0, 0));
	TInt prevYPos = 0;
	_LIT(KNewLine, "\r\n");
	for (TInt y = 0; ; ++y)
		{
		aConsole->Write(KNewLine);
		if (aConsole->CursorPos().iY == prevYPos)
			{
			detectedSize.iHeight = y;
			break;
			}
		else
			{
			prevYPos = y;
			}
		}
	aConsole->ClearScreen();
	aConsole->SetCursorHeight(20);
	return detectedSize;
	}

//______________________________________________________________________________
//						CIoConsoleProxySession
CIoConsoleProxySession::CIoConsoleProxySession(TConsoleCreateFunction aConsoleCreate)
	: CConsoleProxySession(aConsoleCreate), iFlags(ESupportsStdErr)
	{
	// Assume ESupportsStdErr until proven otherwise
	}

CIoConsoleProxySession::~CIoConsoleProxySession()
	{
	delete iUnderlyingConsole;
	}

void CIoConsoleProxySession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case RIoConsoleProxy::ESetConsoleSizeDetect:
		if (iConsole) User::Leave(KErrNotReady); // too late!
		SetFlag(EAutoDetectSize, ETrue);
		aMessage.Complete(KErrNone);
		return;
	case RIoConsoleProxy::ESetLazyConstruct:
		if (iConsole) User::Leave(KErrNotReady); // too late!
		SetFlag(ELazy, ETrue);
		aMessage.Complete(KErrNone);
		return;
	case RIoConsoleProxy::ESetConsoleMode:
		SetModeL(aMessage);
		return;
	case RIoConsoleProxy::ESetUnderlyingConsole:
		SetUnderlyingConsoleL(aMessage);
		return;
	case RIoConsoleProxy::EOpenExistingConsole:
		OpenExistingL(aMessage);
		return;	
	case RConsoleProxy::EGetScreenSize:
		if (GetFlag(EAutoDetectSize) && !GetFlag(ELazy))
			{
			DetectSizeL(aMessage);
			return;
			}
		break;
	case RIoConsoleProxy::EWriteStdErr:
		{
		RBuf buf;
		CleanupClosePushL(buf);
		buf.CreateL(aMessage.GetDesLengthL(0));
		aMessage.ReadL(0, buf);
		if (iFlags & ESupportsStdErr)
			{
			TInt err = ConsoleStdErr::Write(iConsole->Console(), buf);
			if (err != KErrNone)
				{
				// Clearly it doesn't support it, clear the flag so we fall back to normal write and don't bother trying again
				iFlags &= ~ESupportsStdErr;
				}
			}

		if (!(iFlags & ESupportsStdErr))
			{
			iConsole->Console()->Write(buf);
			}
		CleanupStack::PopAndDestroy(&buf);
		aMessage.Complete(KErrNone);
		return;
		}
	default:
		break;
		}

	CConsoleProxySession::ServiceL(aMessage);
	}
	
MProxiedConsole* CIoConsoleProxySession::InstantiateConsoleL()
	{
	if (Server()->TheConsole()!=NULL)
		{
		// make sure that only 1 console is ever created in this server
		User::Leave(KErrAlreadyExists);
		}
	MProxiedConsole* cons;
	if (GetFlag(ELazy))
		{
		CLazyConsole* lazy = new(ELeave)CLazyConsole(iConsoleCreate, GetFlag(EAutoDetectSize));
		CleanupStack::PushL(lazy);
		cons = MProxiedConsole::DefaultL(lazy);
		CleanupStack::Pop();
		}
	else
		{
		cons = CConsoleProxySession::InstantiateConsoleL();
		}
	
	Server()->SetTheConsole(cons);
	
	if (iUnderlyingConsole)
		{
		TInt err = UnderlyingConsole::Set(cons->Console(), iUnderlyingConsole);
		// if this succeeds, ownership of the underlying console has been taken
		// if it didn't, we should delete it as it's not needed.
		if (err!=KErrNone)
			{
			delete iUnderlyingConsole;
			}
		iUnderlyingConsole = NULL;
		}
	
	return cons;	
	}
	
void CIoConsoleProxySession::ConsoleCreatedL(MProxiedConsole* aConsole)
	{
	if (GetFlag(EAutoDetectSize) && !(GetFlag(ELazy)))
		{
		iDetectedSize = DetectConsoleSize(aConsole->Console());
		}
	}

void CIoConsoleProxySession::DetectSizeL(const RMessage2& aMessage)
	{
	if (!iConsole) User::Leave(KErrNotReady);
	
	aMessage.WriteL(0, TPckg<TSize>(iDetectedSize));
	aMessage.Complete(KErrNone);	
	}

void CIoConsoleProxySession::SetModeL(const RMessage2& aMessage)
	{
	if (!iConsole) User::Leave(KErrNotReady);
	RIoReadWriteHandle::TMode mode = (RIoReadWriteHandle::TMode)aMessage.Int0();
	TInt err = ConsoleMode::Set(iConsole->Console(), (mode == RIoReadWriteHandle::EBinary) ? ConsoleMode::EBinary : ConsoleMode::EText);
	aMessage.Complete(err);
	}

void CIoConsoleProxySession::SetUnderlyingConsoleL(const RMessage2& aMessage)
	{
	if (iUnderlyingConsole) User::Leave(KErrAlreadyExists);
	
	RIoConsoleProxy underlyingSession;
	RThread client;
	aMessage.ClientL(client, EOwnerThread);
	CleanupClosePushL(client);

	underlyingSession.SetHandle(aMessage.Int0());
	User::LeaveIfError(underlyingSession.Duplicate(client, EOwnerThread));
	
	CleanupClosePushL(underlyingSession);
	User::LeaveIfError(underlyingSession.OpenExisting());
	
	CConsoleProxy* underlying = CWriteOnlyConsoleProxy::NewL(underlyingSession);
	
	CleanupStack::PopAndDestroy(2, &client); // we can close underlyingSession as it's been duplicated by CConsoleProxy::NewL
	
	if (iConsole && iConsole->Console())
		{
		CleanupStack::PushL(underlying);
		User::LeaveIfError(UnderlyingConsole::Set(iConsole->Console(), underlying));
		// ownership of underlying now taken.
		CleanupStack::Pop();
		}
	else
		{
		// save it for when the console is instantiated
		iUnderlyingConsole = underlying;
		}
	
	aMessage.Complete(KErrNone);
	}

void CIoConsoleProxySession::OpenExistingL(const RMessage2& aMessage)
	{
	if (Server()->TheConsole()==NULL) User::Leave(KErrNotReady); // no console to connect to
	
	iConsole = Server()->TheConsole();
	iConsole->Open();
	aMessage.Complete(KErrNone);
	}

TBool CIoConsoleProxySession::GetFlag(TFlag aFlag)
	{
	return iFlags & aFlag ? (TBool)ETrue : EFalse;
	}
	
void CIoConsoleProxySession::SetFlag(TFlag aFlag, TBool aSet)
	{
	if (aSet)
		{
		iFlags |= aFlag;
		}
	else
		{
		iFlags &= (~(TUint)aFlag);
		}
	}


//______________________________________________________________________________
//						CWriteOnlyConsoleProxy
CConsoleProxy* CWriteOnlyConsoleProxy::NewL(const RConsoleProxy& aProxySession)
	{
	CWriteOnlyConsoleProxy* self = new(ELeave)CWriteOnlyConsoleProxy();
	CleanupStack::PushL(self);
	self->ConstructL(aProxySession);
	CleanupStack::Pop(self);
	return self;
	}
	
CWriteOnlyConsoleProxy::CWriteOnlyConsoleProxy()
	{
	}

void CWriteOnlyConsoleProxy::Read(TRequestStatus&)
	{
	User::Panic(KIoServerName, EPanicCannotReadFromUnderlyingConsole);
	}

void CWriteOnlyConsoleProxy::ReadCancel()
	{
	}

TKeyCode CWriteOnlyConsoleProxy::KeyCode() const
	{
	return EKeyNull;
	}


TUint CWriteOnlyConsoleProxy::KeyModifiers() const
	{
	return 0;
	}

//______________________________________________________________________________
//						CLazyConsole
CLazyConsole::CLazyConsole(TConsoleCreateFunction aConsoleCreate, TBool aAutoDetectSize)
	: iConsoleCreate(aConsoleCreate), iSizeAutoDetect(aAutoDetectSize)
	{
	}

CLazyConsole::~CLazyConsole()
	{
	iTitle.Close();
	delete iConsole;
	}

TInt CLazyConsole::Create(const TDesC &aTitle,TSize aSize)
	{
	iSize = aSize;
	return iTitle.Create(aTitle);
	}
	
TInt CLazyConsole::CheckCreated() const
	{
	if (iCreateError) return iCreateError;
	if (iConsole) return KErrNone;
	
	TRAP(iCreateError, iConsole = iConsoleCreate());
	if ((iCreateError==KErrNone) && (!iConsole))
		{
		iCreateError = KErrNoMemory;
		}
	if (iCreateError == KErrNone)
		{
		TName procName = RProcess().Name(); // econseik sets the process name to the console title...
		iCreateError = iConsole->Create(iTitle, iSize);
		User::RenameProcess(procName.Left(procName.Locate('['))); // ...so restore it just in case
		}
	if ((iCreateError == KErrNone) && iSizeAutoDetect)
		{
		iDetectedSize = DetectConsoleSize(iConsole);
		}
	if (iCreateError != KErrNone)
		{
		delete iConsole;
		iConsole = NULL;
		}
	return iCreateError;
	}

void CLazyConsole::Read(TRequestStatus &aStatus)
	{
	TInt err = CheckCreated();
	if (err)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, err);
		return;
		}
	iConsole->Read(aStatus);
	}

void CLazyConsole::ReadCancel()
	{
	if (iConsole)
		{
		iConsole->ReadCancel();
		}
	}

void CLazyConsole::Write(const TDesC &aDes)
	{
	if (CheckCreated() == KErrNone)
		{
		iConsole->Write(aDes);
		}
	}

TPoint CLazyConsole::CursorPos() const
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->CursorPos();
		}
	return TPoint(0,0);
	}

void CLazyConsole::SetCursorPosAbs(const TPoint &aPoint)
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->SetCursorPosAbs(aPoint);
		}
	}

void CLazyConsole::SetCursorPosRel(const TPoint &aPoint)
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->SetCursorPosRel(aPoint);
		}
	}

void CLazyConsole::SetCursorHeight(TInt aPercentage)
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->SetCursorHeight(aPercentage);
		}
	}

void CLazyConsole::SetTitle(const TDesC &aTitle)
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->SetTitle(aTitle);
		}
	}

void CLazyConsole::ClearScreen()
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->ClearScreen();
		}
	}

void CLazyConsole::ClearToEndOfLine()
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->ClearToEndOfLine();
		}
	}

TSize CLazyConsole::ScreenSize() const
	{
	if (CheckCreated() == KErrNone)
		{
		if (iSizeAutoDetect)
			{
			return iDetectedSize;
			}
		else
			{
			return iConsole->ScreenSize();
			}
		}
	else
		{
		return TSize(0,0);
		}
	}

TKeyCode CLazyConsole::KeyCode() const
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->KeyCode();
		}
	return EKeyNull;
	}

TUint CLazyConsole::KeyModifiers() const
	{
	if (CheckCreated() == KErrNone)
		{
		return iConsole->KeyModifiers();
		}
	return 0;
	}

TInt CLazyConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == LazyConsole::KLazyConsoleExtension)
		{
		TBool* constructed = (TBool*)a1;
		*constructed = (iConsole != NULL);
		return KErrNone;
		}
	else 
		{
		TInt err = CheckCreated();
		if (err == KErrNone)
			{
			return ((CLazyConsole*)iConsole)->Extension_(aExtensionId, a0, a1);
			}
		return err;
		}
	}
