// persistentconsole.cpp
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

#include "persistentconsole.h"
#include <e32hashtab.h>


static void CleanupNullPointer(TAny* aPtrPtr)
	{
	*((TAny**)aPtrPtr) = NULL;
	}
	
static void CleanupNullPointerPushL(TAny** aPtr)
	{
	CleanupStack::PushL(TCleanupItem(CleanupNullPointer, aPtr));
	}
	
_LIT(KProxySuffix, "_proxy");

//______________________________________________________________________________
//						CIoPersistentConsole
CIoPersistentConsole* CIoPersistentConsole::NewLC(const TDesC& aName, const TDesC& aTitle, CIoServer& aServer, const RMsg& aMessage)
	{
	CIoPersistentConsole* self = new(ELeave)CIoPersistentConsole(aServer);
	CleanupClosePushL(*self);
	self->ConstructL(aName, aTitle, aMessage);
	return self;
	}
	
TBool CIoPersistentConsole::CanConnectReaderL(const MIoReadEndPoint& aReader) const
	{
	// check that the candidate end point will not result in a circular connection of
	// persistent consoles if it's connected to this one.
	
	RHashSet<TInt> chained; // addresses of CIoPersistentConsole's in any chain
	CleanupClosePushL(chained);
	chained.InsertL((TInt)this);
	const MIoReadEndPoint* nextReader = &aReader;
	while (nextReader && nextReader->IoepIsType(RIoHandle::EPersistentConsole))
		{
		const CIoPersistentConsole* next = (const CIoPersistentConsole*)nextReader;
		if (chained.Find((TInt)next))
			{
			CleanupStack::PopAndDestroy();
			return EFalse;
			}
		chained.InsertL((TInt)next);
		nextReader = next->TransientReader();
		}
	CleanupStack::PopAndDestroy();
	return ETrue;	
	}
	
void CIoPersistentConsole::AttachTransientReaderL(MIoReadEndPoint& aReader, CIoSession* aDetachOnClose)
	{
	if (!CanConnectReaderL(aReader)) User::Leave(KErrCouldNotConnect);	
		
	if (iTransientReadEndPoint)
		{
		User::Leave(KErrInUse);
		}
		
	
	iTransientReadEndPoint = &aReader;
	CleanupNullPointerPushL((TAny**)&iTransientReadEndPoint);
	
	TInt fgProxyPos = ReaderProxyIndex(AttachedReader());
	if (fgProxyPos != KErrNotFound)
		{
		iTransientReadEndPoint->IorepAttachL(*iReaderProxies[fgProxyPos], RIoEndPoint::EForeground);
		}
	
	TInt i = 0;
	TRAPD(err, 
		for (i=0; i<iReaderProxies.Count(); ++i)
			{
			if (i != fgProxyPos)
				{
				iTransientReadEndPoint->IorepAttachL(*iReaderProxies[i], RIoEndPoint::EBackground);
				}
			}
		);
		
	if (err)
		{
		// deatch the readers the we successfully attached before failure
		for (TInt j=0; j<i; ++j)
			{
			iTransientReadEndPoint->IorepDetach(*iReaderProxies[j]);
			}
		// also make sure that the foreground reader is deatched
		if (fgProxyPos >= i)
			{
			iTransientReadEndPoint->IorepDetach(*iReaderProxies[fgProxyPos]);
			}
		User::Leave(err);
		}
	CleanupStack::Pop();
	
	iReadEndPointDetachOnClose = aDetachOnClose;
		
	// now foreward any outstanding read requests
	for (TInt i=0; i<iReaderProxies.Count(); ++i)
		{
		iReaderProxies[i]->TransientReaderAttached(*iTransientReadEndPoint);
		}
		
	MIoReader* fg = AttachedReader();
	if (fg)
		{
		fg->IorReaderChange(RIoReadHandle::EGainedForeground | RIoReadHandle::EConsoleSizeChanged);
		}
	}
	
void CIoPersistentConsole::DetachTransientReader()
	{
	if (!iTransientReadEndPoint)
		{
		return;
		}
	MIoReadEndPoint* detaching = iTransientReadEndPoint;
	iTransientReadEndPoint = NULL;

	TInt fgReaderPos = ReaderProxyIndex(AttachedReader());
	for (TInt i=0; i<iReaderProxies.Count(); ++i)
		{
		if (i!=fgReaderPos)
			{
			detaching->IorepDetach(*iReaderProxies[i]);
			}
		}
	if (fgReaderPos!=KErrNotFound)
		{
		detaching->IorepDetach(*iReaderProxies[fgReaderPos]);
		}
	iReadEndPointDetachOnClose = NULL;

	SendReadDetachNotifications();
	}
	
const MIoReadEndPoint* CIoPersistentConsole::TransientReader() const
	{
	return iTransientReadEndPoint;
	}	

TBool CIoPersistentConsole::CanConnectWriterL(const MIoWriteEndPoint& aWriter) const
	{
	// check that the candidate end point will not result in a circular connection of
	// persistent consoles if it's connected to this one.
	
	RHashSet<TInt> chained; // addresses of CIoPersistentConsole's in any chain
	CleanupClosePushL(chained);
	chained.InsertL((TInt)this);
	const MIoWriteEndPoint* nextWriter = &aWriter;
	while (nextWriter && nextWriter->IoepIsType(RIoHandle::EPersistentConsole))
		{
		const CIoPersistentConsole* next = (const CIoPersistentConsole*)nextWriter;
		if (chained.Find((TInt)next))
			{
			CleanupStack::PopAndDestroy();
			return EFalse;
			}
		chained.InsertL((TInt)next);
		nextWriter = next->TransientWriter();
		}
	CleanupStack::PopAndDestroy();
	return ETrue;	
	}
	
void CIoPersistentConsole::AttachTransientWriterL(MIoWriteEndPoint& aWriter, CIoSession* aDetachOnClose)
	{
	if (!CanConnectWriterL(aWriter)) User::Leave(KErrCouldNotConnect);	
	
	if (iTransientWriteEndPoint)
		{
		User::Leave(KErrInUse);
		}
	iTransientWriteEndPoint = &aWriter;
	CleanupNullPointerPushL((TAny**)&iTransientWriteEndPoint);
	
	TInt i = 0;
	TRAPD(err, 
		for (i=0; i<iWriterProxies.Count(); ++i)
			{
			iTransientWriteEndPoint->IowepAttachL(*iWriterProxies[i]);
			}
		);
	if (err)
		{
		// deatch the writers the we successfully attached before failure
		for (TInt j=0; j<i; ++j)
			{
			iWriterProxies[j]->TransientWriterDetach(*iTransientWriteEndPoint);
			}
		User::Leave(err);
		}
	CleanupStack::Pop();
	
	iWriteEndPointDetachOnClose = aDetachOnClose;
	
	TRAP_IGNORE(iTransientWriteEndPoint->IowepSetTitleL(iTitleSetter));
	
	for (TInt i=0; i<iWriterProxies.Count(); ++i)
		{
		iWriterProxies[i]->TransientWriterAttached(*iTransientWriteEndPoint);
		}		
	}
	
void CIoPersistentConsole::DetachTransientWriter()
	{
	if (!iTransientWriteEndPoint)
		{
		return;
		}
	MIoWriteEndPoint* detaching = iTransientWriteEndPoint;
	iTransientWriteEndPoint = NULL;
	for (TInt i=0; i<iWriterProxies.Count(); ++i)
		{
		iWriterProxies[i]->TransientWriterDetach(*detaching);
		}
	iWriteEndPointDetachOnClose = NULL;
	
	SendWriteDetachNotifications();
	}
	
const MIoWriteEndPoint* CIoPersistentConsole::TransientWriter() const
	{
	return iTransientWriteEndPoint;
	}
	
MIoWriteEndPoint* CIoPersistentConsole::TransientWriter()
	{
	return iTransientWriteEndPoint;
	}
	
void CIoPersistentConsole::NotifyReadDetachL(const RMsg& aMessage)
	{
	if ((iReaderProxies.Count()==0) || !iTransientReadEndPoint)
		{
		Complete(aMessage, KErrNone);
		}
	else
		{
		iReadDetachNotifications.AppendL(aMessage);
		}
	}
	
void CIoPersistentConsole::CancelNotifyReadDetach(TRequestStatus* aClientStatus)
	{
	for (TInt i=0; i<iReadDetachNotifications.Count(); ++i)
		{
		if (iReadDetachNotifications[i].ClientStatus() == aClientStatus)
			{
			Complete(iReadDetachNotifications[i], KErrCancel);
			iReadDetachNotifications.Remove(i);
			return;
			}
		}
	}
	
void CIoPersistentConsole::NotifyWriteDetachL(const RMsg& aMessage)
	{
	if ((iWriterProxies.Count()==0) || !iTransientWriteEndPoint)
		{
		Complete(aMessage, KErrNone);
		}
	else
		{
		iWriteDetachNotifications.AppendL(aMessage);
		}
	}
	
void CIoPersistentConsole::CancelNotifyWriteDetach(TRequestStatus* aClientStatus)
	{
	for (TInt i=0; i<iWriteDetachNotifications.Count(); ++i)
		{
		if (iWriteDetachNotifications[i].ClientStatus() == aClientStatus)
			{
			Complete(iWriteDetachNotifications[i], KErrCancel);
			iWriteDetachNotifications.Remove(i);
			return;
			}
		}
	}
	
TName CIoPersistentConsole::TransientReaderName()
	{
	if (iTransientReadEndPoint)
		{
		return iTransientReadEndPoint->IoepName();
		}
	else
		{
		return KNullDesC();
		}
	}
	
TName CIoPersistentConsole::TransientWriterName()
	{
	if (iTransientWriteEndPoint)
		{
		return iTransientWriteEndPoint->IoepName();
		}
	else
		{
		return KNullDesC();
		}
	}
	
TThreadId CIoPersistentConsole::Creator()
	{
	return iCreator.Id();
	}
	
TName CIoPersistentConsole::Name() const
	{
	return iName;
	}

TBool CIoPersistentConsole::IsType(RIoHandle::TType aType) const
	{
	if (aType == RIoHandle::EPersistentConsole)
		{
		return ETrue;
		}
	else
		{
		return CIoConsole::IsType(aType);
		}
	}
	
void CIoPersistentConsole::SessionClosed(const CIoSession& aSession)
	{
	if (&aSession == iReadEndPointDetachOnClose)
		{
		DetachTransientReader();
		}
	if (&aSession == iWriteEndPointDetachOnClose)
		{
		DetachTransientWriter();
		}
	}
	
void CIoPersistentConsole::ClosedBy(const CIoSession& aSession)
	{
	SessionClosed(aSession);
	}
	
void CIoPersistentConsole::SendReadDetachNotifications()
	{
	for (TInt i=0; i<iReadDetachNotifications.Count(); ++i)
		{
		Complete(iReadDetachNotifications[i], KErrNone);
		}
	iReadDetachNotifications.Reset();
	}
	
void CIoPersistentConsole::SendWriteDetachNotifications()
	{
	for (TInt i=0; i<iWriteDetachNotifications.Count(); ++i)
		{
		Complete(iWriteDetachNotifications[i], KErrNone);
		}
	iWriteDetachNotifications.Reset();
	}

//______________________________________________________________________________
//		MIoReadEndPoint implementation - to handle reads from persistent side
TInt CIoPersistentConsole::HandleReaderAttached(MIoReader& aReader)
	{
	TIoReaderProxy* proxy = new TIoReaderProxy(aReader, *this);
	TInt err = proxy ? KErrNone : KErrNoMemory;
	if (err==KErrNone)
		{
		err = iReaderProxies.Append(proxy);
		}
	
	if ((err == KErrNone) && iTransientReadEndPoint)
		{
		TRAP(err, iTransientReadEndPoint->IorepAttachL(*proxy, RIoEndPoint::EBackground));
		}

	if (err != KErrNone)
		{
		TInt pos = ReaderProxyIndex(proxy);
		if (pos!=KErrNotFound)
			{
			iReaderProxies.Remove(pos);
			}
		delete proxy;
		}		
	return err;
	}
	
void CIoPersistentConsole::HandleReaderDetached(MIoReader& aReader)
	{
	TInt pos = ReaderProxyIndex(&aReader);
	if (pos!=KErrNotFound)
		{
		if (iTransientReadEndPoint)
			{
			iTransientReadEndPoint->IorepDetach(*iReaderProxies[pos]);
			}
		delete iReaderProxies[pos];
		iReaderProxies.Remove(pos);
		
		if (iReaderProxies.Count()==0)
			{
			SendReadDetachNotifications();
			}
		}
	}
	
void CIoPersistentConsole::HandleForegroundReaderChanged()
	{
	if (iTransientReadEndPoint)
		{
		CIoEndPoint::HandleForegroundReaderChanged();
		}
	else
		{
		ForegroundReaderChanged();
		}
	}
	
void CIoPersistentConsole::ForegroundReaderChanged()
	{
	MIoReader* fgReader = AttachedReader();
	if (iTransientReadEndPoint && fgReader)
		{
		TInt fgPos = ReaderProxyIndex(fgReader);
		ASSERT(fgPos != KErrNotFound);
		TRAPD(err, iTransientReadEndPoint->IorepSetForegroundReaderL(*iReaderProxies[fgPos]));
		__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
		// this would only fail if this endpoints list of readers differs from the transient
		// endpoints list; the two should always be kept in sync.
		}
	}
	
TInt CIoPersistentConsole::ReaderProxyIndex(MIoReader* aReader) const
	{
	for (TInt i=0; i<iReaderProxies.Count(); ++i)
		{
		if (&iReaderProxies[i]->ClientReader() == aReader)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

void CIoPersistentConsole::IorepReadL(MIoReader& aReader)
	{
	TInt pos = ReaderProxyIndex(&aReader);
	ASSERT(pos!=KErrNotFound);
	iReaderProxies[pos]->ReadL(iTransientReadEndPoint);
	}

void CIoPersistentConsole::IorepReadKeyL(MIoReader& aReader)
	{
	TInt pos = ReaderProxyIndex(&aReader);
	ASSERT(pos!=KErrNotFound);
	iReaderProxies[pos]->ReadKeyL(iTransientReadEndPoint);
	}

void CIoPersistentConsole::IorepSetConsoleModeL(RIoReadWriteHandle::TMode aMode, MIoReader& aReader)
	{
	iPersistentConsoleMode = aMode;
	GetReaderProxy(&aReader).SetConsoleModeL(iTransientReadEndPoint, aMode);
	}

//______________________________________________________________________________
//		MIoWriteEndPoint implementation - to handles writes from persistent side
TInt CIoPersistentConsole::HandleWriterAttached(MIoWriter& aWriter)
	{
	// call from CIoEndPoint
	TIoWriterProxy* proxy = new TIoWriterProxy(aWriter, *this);
	TInt err = proxy ? KErrNone : KErrNoMemory;
	if (err==KErrNone)
		{
		err = iWriterProxies.Append(proxy);
		}
	
	if ((err == KErrNone) && iTransientWriteEndPoint)
		{
		TRAP(err, iTransientWriteEndPoint->IowepAttachL(*proxy));
		}

	if (err != KErrNone)
		{
		TInt pos = WriterProxyIndex(proxy);
		if (pos!=KErrNotFound)
			{
			iWriterProxies.Remove(pos);
			}
		delete proxy;
		}		
	return err;
	}
	
void CIoPersistentConsole::HandleWriterDetached(MIoWriter& aWriter)
	{
	TInt pos = WriterProxyIndex(&aWriter);
	if (pos!=KErrNotFound)
		{
		if (iTransientWriteEndPoint)
			{
			iWriterProxies[pos]->TransientWriterDetach(*iTransientWriteEndPoint);
			}
		delete iWriterProxies[pos];
		iWriterProxies.Remove(pos);
		
		if (iWriterProxies.Count()==0)
			{
			SendWriteDetachNotifications();
			}
		}
		
	}
	
TInt CIoPersistentConsole::WriterProxyIndex(MIoWriter* aWriter) const
	{
	for (TInt i=0; i<iWriterProxies.Count(); ++i)
		{
		if (&iWriterProxies[i]->ClientWriter() == aWriter)
			{
			return i;
			}
		}
	return KErrNotFound;
	}
	
CIoPersistentConsole::TIoWriterProxy& CIoPersistentConsole::GetWriterProxy(MIoWriter* aWriter) const
	{
	TInt pos = WriterProxyIndex(aWriter);
	ASSERT(pos!=KErrNotFound);
	return *iWriterProxies[pos];
	}
	
CIoPersistentConsole::TIoReaderProxy& CIoPersistentConsole::GetReaderProxy(MIoReader* aWriter) const
	{
	TInt pos = ReaderProxyIndex(aWriter);
	ASSERT(pos!=KErrNotFound);
	return *iReaderProxies[pos];
	}

void CIoPersistentConsole::IowepWriteL(MIoWriter& aWriter)
	{
	TInt pos = WriterProxyIndex(&aWriter);
	ASSERT(pos!=KErrNotFound);
	iWriterProxies[pos]->WriteL(iTransientWriteEndPoint);

	// move the proxy to the end of the array so that we service the writes 
	// in the correct order when we get a write end point, or when the end
	// point goes away and comes back
	TIoWriterProxy* proxy(iWriterProxies[pos]);
	iWriterProxies.Remove(pos);
	iWriterProxies.Append(proxy);
	}
	
void CIoPersistentConsole::IowepWriteCancel(MIoWriter& aWriter)
	{
	TInt pos = WriterProxyIndex(&aWriter);
	ASSERT(pos!=KErrNotFound);
	iWriterProxies[pos]->WriteCancel(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepCursorPosL(MIoWriter& aWriter) const
	{
	GetWriterProxy(&aWriter).GetCursorPosL(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepSetCursorPosAbsL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).SetCursorPosAbsL(aPoint, iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepSetCursorPosRelL(const TPoint& aPoint, MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).SetCursorPosRelL(aPoint, iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepSetCursorHeightL(TInt aPercentage, MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).SetCursorHeightL(aPercentage, iTransientWriteEndPoint);
	// TODO cache cursor height
	}

void CIoPersistentConsole::IowepSetTitleL(MIoWriter& aWriter)
	{
	delete iTitle;
	iTitle = NULL;
	TRAP_IGNORE(iTitle = aWriter.IowTitleLC(); CleanupStack::Pop(iTitle)); // not much we can do if this fails
	GetWriterProxy(&aWriter).SetTitleL(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepClearScreenL(MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).ClearScreenL(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepClearToEndOfLineL(MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).ClearToEndOfLineL(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepScreenSizeL(MIoWriter& aWriter) const
	{
	GetWriterProxy(&aWriter).GetScreenSizeL(iTransientWriteEndPoint);
	}

void CIoPersistentConsole::IowepSetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriter& aWriter)
	{
	GetWriterProxy(&aWriter).SetAttributesL(aAttributes, aForegroundColor, aBackgroundColor, iTransientWriteEndPoint);
	}
	
CIoPersistentConsole::CIoPersistentConsole(CIoServer& aServer)
	: CIoConsole(aServer.Config()), iServer(aServer), iTitleSetter(iTitle)
	{
	}
	
CIoPersistentConsole::~CIoPersistentConsole()
	{
	iServer.PersistentConsoleRemove(iName, *this);
	SendReadDetachNotifications();
	iReadDetachNotifications.Close();
	SendWriteDetachNotifications();
	iWriteDetachNotifications.Close();
	
	delete iTitle;
	iWriterProxies.ResetAndDestroy();
	iReaderProxies.ResetAndDestroy();
	
	iCreator.Close();
	}

_LIT(KPersistentConsoleImplementation, "<persistent console>");

void CIoPersistentConsole::ConstructL(const TDesC& aName, const TDesC& aTitle, const RMsg& aMessage)
	{
	LOG(CIoLog::Printf(_L("Persistent console %S @ 0x%08x created"), &aName, this));
	iName = aName.Left(KMaxName);
	iServer.PersistentConsoleAddL(iName, *this);
	iImplementation = KPersistentConsoleImplementation().AllocL();
	iTitle = aTitle.AllocL();
	aMessage.ClientL(iCreator);
	}

//______________________________________________________________________________
//							TIoWriterProxy
// MIoWriter proxy to service write requests from the transient side
CIoPersistentConsole::TIoWriterProxy::TIoWriterProxy(MIoWriter& aWriter, CIoPersistentConsole& aOwner)
	: iWriter(aWriter)
	, iOwner(aOwner)
	, iFlags(0)
	{
	}
	
TBool CIoPersistentConsole::TIoWriterProxy::GetFlag(TFlags aFlag)
	{
	return iFlags & aFlag;
	}
	
void CIoPersistentConsole::TIoWriterProxy::SetFlag(TFlags aFlag)
	{
	iFlags |= aFlag;
	}
	
void CIoPersistentConsole::TIoWriterProxy::ClearFlag(TFlags aFlag)
	{
	iFlags &= (~aFlag);
	}
	
#define TRAP_OR(x, y) {TRAPD(err, x); if (err!=KErrNone) {y;} }
	
	
void CIoPersistentConsole::TIoWriterProxy::TransientWriterAttached(MIoWriteEndPoint& aEndPoint)
	{
	if (GetFlag(EWritePending))
		{
		TRAP_OR(aEndPoint.IowepWriteL(*this), IowComplete(err));
		}
	if (GetFlag(EGetCursorPosPending))
		{
		TRAP_OR(aEndPoint.IowepCursorPosL(*this), IowCursorPos(err, TPoint(0,0)));
		}
	if (GetFlag(ESetCursorPosAbsPending))
		{
		TRAP_OR(aEndPoint.IowepSetCursorPosAbsL(iSetCursPosAbsPoint, *this), IowSetCursorPosAbsComplete(err));
		}
	if (GetFlag(ESetCursorPosRelPending))
		{
		TRAP_OR(aEndPoint.IowepSetCursorPosRelL(iSetCursPosRelPoint, *this), IowSetCursorPosRelComplete(err));
		}
	if (GetFlag(ESetCursorHeightPending))
		{
		TRAP_OR(aEndPoint.IowepSetCursorHeightL(iSetCursorHeight, *this), IowSetCursorHeightComplete(err));
		}
	if (GetFlag(ESetTitlePending))
		{
		TRAP_OR(aEndPoint.IowepSetTitleL(*this), IowSetTitleComplete(err));
		}
	if (GetFlag(EClearScreenPending))
		{
		TRAP_OR(aEndPoint.IowepClearScreenL(*this), IowClearScreenComplete(err));
		}
	if (GetFlag(EClearToEndOfLinePending))
		{
		TRAP_OR(aEndPoint.IowepClearToEndOfLineL(*this), IowClearToEndOfLineComplete(err));
		}
	if (GetFlag(EGetScreenSizePending))
		{
		TRAP_OR(aEndPoint.IowepScreenSizeL(*this), IowScreenSize(err, TSize(0,0)));
		}
	if (GetFlag(ESetAttributesPending))
		{
		TRAP_OR(aEndPoint.IowepSetAttributesL(iAttributes, iForegroundColor, iBackgroundColor, *this), IowSetAttributesComplete(err));
		}
	}
	
void CIoPersistentConsole::TIoWriterProxy::TransientWriterDetach(MIoWriteEndPoint& aEndPoint)
	{
	if (GetFlag(EDetaching)) return;
	if (GetFlag(EWritePending))
		{
		aEndPoint.IowepWriteCancel(*this);
		// leave the write pending so it is serviced when we get a new transient end point
		}
	aEndPoint.IowepDetach(*this);
	}
	
void CIoPersistentConsole::TIoWriterProxy::WriteL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(EWritePending);
	if (aEndPoint)
		{
		TRAPD(err, aEndPoint->IowepWriteL(*this));
		if (err)
			{
			ClearFlag(EWritePending);
			User::Leave(err);
			}
		}
	}
	
void CIoPersistentConsole::TIoWriterProxy::WriteCancel(MIoWriteEndPoint* aEndPoint)
	{
	if (aEndPoint)
		{
		aEndPoint->IowepWriteCancel(*this);
		}
	ClearFlag(EWritePending);
	}

	
MIoWriter& CIoPersistentConsole::TIoWriterProxy::ClientWriter() const
	{
	return iWriter;
	}
	
void CIoPersistentConsole::TIoWriterProxy::GetCursorPosL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(EGetCursorPosPending);
	if (aEndPoint)
		{
		aEndPoint->IowepCursorPosL(*this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::SetCursorPosAbsL(const TPoint& aPos, MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(ESetCursorPosAbsPending);
	iSetCursPosAbsPoint = aPos;
	if (aEndPoint)
		{
		aEndPoint->IowepSetCursorPosAbsL(iSetCursPosAbsPoint, *this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::SetCursorPosRelL(const TPoint& aPos, MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(ESetCursorPosRelPending);
	iSetCursPosRelPoint = aPos;
	if (aEndPoint)
		{
		aEndPoint->IowepSetCursorPosRelL(iSetCursPosRelPoint, *this);
		}
	}
	
void CIoPersistentConsole::TIoWriterProxy::SetCursorHeightL(TInt aPercentage, MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(ESetCursorHeightPending);
	iSetCursorHeight = aPercentage;
	if (aEndPoint)
		{
		aEndPoint->IowepSetCursorHeightL(iSetCursorHeight, *this);
		}
	}
	
void CIoPersistentConsole::TIoWriterProxy::SetTitleL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(ESetTitlePending);
	if (aEndPoint)
		{
		aEndPoint->IowepSetTitleL(*this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::ClearScreenL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(EClearScreenPending);
	if (aEndPoint)
		{
		aEndPoint->IowepClearScreenL(*this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::ClearToEndOfLineL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(EClearToEndOfLinePending);
	if (aEndPoint)
		{
		aEndPoint->IowepClearToEndOfLineL(*this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::GetScreenSizeL(MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(EGetScreenSizePending);
	if (aEndPoint)
		{
		aEndPoint->IowepScreenSizeL(*this);
		}
	}

void CIoPersistentConsole::TIoWriterProxy::SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor, MIoWriteEndPoint* aEndPoint)
	{
	SetFlag(ESetAttributesPending);
	iAttributes = aAttributes;
	iForegroundColor = aForegroundColor;
	iBackgroundColor = aBackgroundColor;
	if (aEndPoint)
		{
		aEndPoint->IowepSetAttributesL(iAttributes, iForegroundColor, iBackgroundColor, *this);
		}
	}

TInt CIoPersistentConsole::TIoWriterProxy::IowWriteLength() const
	{
	return iWriter.IowWriteLength();
	}

TInt CIoPersistentConsole::TIoWriterProxy::IowWrite(TDes& aBuf)
	{
	return iWriter.IowWrite(aBuf);
	}

void CIoPersistentConsole::TIoWriterProxy::IowComplete(TInt aError)
	{
	if (aError == KErrNone)
		{
		iWriter.IowComplete(KErrNone);
		ClearFlag(EWritePending);
		}
	else
		{
		SetFlag(EDetaching);
		iOwner.DetachTransientWriter();
		ClearFlag(EDetaching);
		}
	}

TName CIoPersistentConsole::TIoWriterProxy::IowName()
	{
	TName name(iWriter.IowName());
	if (name.Length() + KProxySuffix().Length() > name.MaxLength())
		{
		name.SetLength(name.MaxLength() - KProxySuffix().Length());
		}
	name.Append(KProxySuffix);
	return name;
	}

RIoReadWriteHandle::TMode CIoPersistentConsole::TIoWriterProxy::IorwMode() const
	{
	return iWriter.IorwMode();
	}
	
void CIoPersistentConsole::TIoWriterProxy::IowCursorPos(TInt aError, TPoint aPos)
	{
	ClearFlag(EGetCursorPosPending);
	iWriter.IowCursorPos(aError, aPos);
	}

void CIoPersistentConsole::TIoWriterProxy::IowSetCursorPosAbsComplete(TInt aError)
	{
	ClearFlag(ESetCursorPosAbsPending);
	iWriter.IowSetCursorPosAbsComplete(aError);
	}

void CIoPersistentConsole::TIoWriterProxy::IowSetCursorPosRelComplete(TInt aError)
	{
	ClearFlag(ESetCursorPosRelPending);
	iWriter.IowSetCursorPosRelComplete(aError);
	}
	
void CIoPersistentConsole::TIoWriterProxy::IowSetCursorHeightComplete(TInt aError)
	{
	ClearFlag(ESetCursorHeightPending);
	iWriter.IowSetCursorHeightComplete(aError);
	}
	
void CIoPersistentConsole::TIoWriterProxy::IowSetTitleComplete(TInt aError)
	{
	ClearFlag(ESetTitlePending);
	iWriter.IowSetTitleComplete(aError);
	}

void CIoPersistentConsole::TIoWriterProxy::IowClearScreenComplete(TInt aError)
	{
	ClearFlag(EClearScreenPending);
	iWriter.IowClearScreenComplete(aError);
	}

void CIoPersistentConsole::TIoWriterProxy::IowClearToEndOfLineComplete(TInt aError)
	{
	ClearFlag(EClearToEndOfLinePending);
	iWriter.IowClearToEndOfLineComplete(aError);
	}

void CIoPersistentConsole::TIoWriterProxy::IowScreenSize(TInt aError, TSize aSize)
	{
	ClearFlag(EGetScreenSizePending);
	iWriter.IowScreenSize(aError, aSize);
	}

void CIoPersistentConsole::TIoWriterProxy::IowSetAttributesComplete(TInt aError)
	{
	ClearFlag(ESetAttributesPending);
	iWriter.IowSetAttributesComplete(aError);
	}
	
HBufC* CIoPersistentConsole::TIoWriterProxy::IowTitleLC()
	{
	return iWriter.IowTitleLC();
	}

//______________________________________________________________________________
//							TIoReaderProxy
//	MIoReader proxy - to service read requests from transient side
CIoPersistentConsole::TIoReaderProxy::TIoReaderProxy(MIoReader& aReader, CIoPersistentConsole& aOwner)
	: iReader(aReader), iOwner(aOwner), iDetaching(EFalse), iSetConsoleModePending(EFalse)
	{
	}
	
void CIoPersistentConsole::TIoReaderProxy::TransientReaderAttached(MIoReadEndPoint& aReader)
	{
	if (iReader.IorReadPending())
		{
		TRAPD(err, aReader.IorepReadL(*this));
		if (err)
			{
			IorReadComplete(err);
			}
		}
	if (iReader.IorReadKeyPending())
		{
		TRAPD(err, aReader.IorepReadKeyL(*this));
		if (err)
			{
			IorReadKeyComplete(err, EKeyNull, 0);
			}
		}
	if (iSetConsoleModePending)
		{
		TRAP_OR(aReader.IorepSetConsoleModeL(iSetConsoleMode, *this), IorSetConsoleModeComplete(err));
		}
	}
	
void CIoPersistentConsole::TIoReaderProxy::ReadL(MIoReadEndPoint* aEndPoint)
	{
	if (aEndPoint)
		{
		aEndPoint->IorepReadL(*this);
		}
	}
	
void CIoPersistentConsole::TIoReaderProxy::ReadKeyL(MIoReadEndPoint* aEndPoint)
	{
	if (aEndPoint)
		{
		aEndPoint->IorepReadKeyL(*this);
		}
	}
	
void CIoPersistentConsole::TIoReaderProxy::SetConsoleModeL(MIoReadEndPoint* aEndPoint, RIoReadWriteHandle::TMode aMode)
	{
	iSetConsoleModePending = ETrue;
	iSetConsoleMode = aMode;
	if (aEndPoint)
		{
		CleanupNullPointerPushL((TAny**)&iSetConsoleModePending);
		aEndPoint->IorepSetConsoleModeL(iSetConsoleMode, *this);
		CleanupStack::Pop();
		}
	}
	
MIoReader& CIoPersistentConsole::TIoReaderProxy::ClientReader() const
	{
	return iReader;
	}

RIoReadWriteHandle::TMode CIoPersistentConsole::TIoReaderProxy::IorwMode() const
	{
	return iReader.IorwMode();
	}

TBool CIoPersistentConsole::TIoReaderProxy::IorReadPending() const
	{
	return iReader.IorReadPending();
	}

TBool CIoPersistentConsole::TIoReaderProxy::IorReadKeyPending() const
	{
	return iReader.IorReadKeyPending();
	}


TDes& CIoPersistentConsole::TIoReaderProxy::IorReadBuf()
	{
	return iReader.IorReadBuf();
	}

void CIoPersistentConsole::TIoReaderProxy::IorDataBuffered(TInt aLength)
	{
	iReader.IorDataBuffered(aLength);
	}

TBool CIoPersistentConsole::TIoReaderProxy::IorDataIsBuffered() const
	{
	return iReader.IorDataIsBuffered();
	}


TBool CIoPersistentConsole::TIoReaderProxy::IorIsKeyCaptured(TUint aKeyCode, TUint aModifiers)
	{
	return iReader.IorIsKeyCaptured(aKeyCode, aModifiers);
	}

void CIoPersistentConsole::TIoReaderProxy::IorReadComplete(TInt /*aError*/)
	{
	// indicates that the read end point will be providing no more data
	// so detatch it
	iDetaching = ETrue;
	iOwner.DetachTransientReader();
	iDetaching = EFalse;
	}

void CIoPersistentConsole::TIoReaderProxy::IorReadKeyComplete(TInt aError, TUint aKeyCode, TUint aModifiers)
	{
	if (aError == KErrEof)
		{
		// indicates that the read end point will be providing no more data
		// so detatch it
		iOwner.DetachTransientReader();
		}
	else
		{
		iReader.IorReadKeyComplete(aError, aKeyCode, aModifiers);
		}
	}

TName CIoPersistentConsole::TIoReaderProxy::IorName()
	{
	TName name(iReader.IorName());
	if (name.Length() + KProxySuffix().Length() > name.MaxLength())
		{
		name.SetLength(name.MaxLength() - KProxySuffix().Length());
		}
	name.Append(KProxySuffix);
	return name;
	}

void CIoPersistentConsole::TIoReaderProxy::IorReaderChange(TUint aChange)
	{
	iReader.IorReaderChange(aChange);
	}

void CIoPersistentConsole::TIoReaderProxy::IorSetConsoleModeComplete(TInt aError)
	{
	iSetConsoleModePending = EFalse;
	iReader.IorSetConsoleModeComplete(aError);
	}

//______________________________________________________________________________
//						TConsoleTitleSetter
TConsoleTitleSetter::TConsoleTitleSetter(HBufC*& aTitle)
	: iTitle(aTitle)
	{
	}

TInt TConsoleTitleSetter::IowWriteLength() const
	{
	ASSERT(0);
	return 0;
	}

TInt TConsoleTitleSetter::IowWrite(TDes&)
	{
	ASSERT(0);
	return 0;
	}

void TConsoleTitleSetter::IowComplete(TInt)
	{
	ASSERT(0);
	}

TName TConsoleTitleSetter::IowName()
	{
	ASSERT(0);
	return TName();
	}

RIoReadWriteHandle::TMode TConsoleTitleSetter::IorwMode() const
	{
	ASSERT(0);
	return RIoReadWriteHandle::EText;
	}

void TConsoleTitleSetter::IowCursorPos(TInt, TPoint)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowSetCursorPosAbsComplete(TInt)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowSetCursorPosRelComplete(TInt)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowSetCursorHeightComplete(TInt)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowSetTitleComplete(TInt)
	{
	}

void TConsoleTitleSetter::IowClearScreenComplete(TInt)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowClearToEndOfLineComplete(TInt)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowScreenSize(TInt, TSize)
	{
	ASSERT(0);
	}

void TConsoleTitleSetter::IowSetAttributesComplete(TInt)
	{
	ASSERT(0);
	}

HBufC* TConsoleTitleSetter::IowTitleLC()
	{
	return iTitle->AllocLC();
	}

	
