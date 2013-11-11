// btrace_reader.cpp
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

#include "btrace_parser.h"


class TBtraceHeader
	{
public:
	TUint8 iSize;
	TUint8 iFlags;
	TUint8 iCategory;
	TUint8 iSubCategory;
	};

NONSHARABLE_CLASS(CMultipartFragment) : public CBase
	{
public:
	static CMultipartFragment* NewLC(const TBtraceFrame& aFirstFrame, TUint32 aExtra);
	~CMultipartFragment();
	static TInt Compare(const CMultipartFragment& aFirst, const CMultipartFragment& aSecond);
	void AddDataL(const TDesC8& aData);

	TUint8 Category() const { return iFirstFrame.iCategory; }
	TUint8 SubCategory() const { return iFirstFrame.iSubCategory; }
	const TBtraceFrame& Frame() const { return iFirstFrame; }
	TInt TotalSize() const { return iTotalSize; }

	CMultipartFragment(TUint32 aExtra); // Only used for search key

private:
	CMultipartFragment(const TBtraceFrame& aFirstFrame, TUint32 aExtra);
	void ConstructL();

private:
	TInt iTotalSize;
	TBtraceFrame iFirstFrame;
	TUint32 iUniqueId;
	RBuf8 iData;
	};

NONSHARABLE_CLASS(CFlusher) : public CTimer
	{
public:
	static CFlusher* NewL(CBtraceReader& aReader);
	~CFlusher();
	void Start(TTimeIntervalMicroSeconds32 aPeriod);
	void Stop();
private:
	CFlusher(CBtraceReader& aReader);
	void Queue();
private: // From CTimer.
	virtual void RunL();
private:
	TTimeIntervalMicroSeconds32 iPeriod;
	CBtraceReader& iReader;
	};

CFlusher* CFlusher::NewL(CBtraceReader& aReader)
	{
	CFlusher* self = new(ELeave) CFlusher(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFlusher::~CFlusher()
	{
	}

void CFlusher::Start(TTimeIntervalMicroSeconds32 aPeriod)
	{
	ASSERT(!IsActive());
	iPeriod = aPeriod;
	Queue();
	}

void CFlusher::Stop()
	{
	Cancel();
	}

CFlusher::CFlusher(CBtraceReader& aReader)
	: CTimer(CActive::EPriorityStandard), iReader(aReader)
	{
	CActiveScheduler::Add(this);
	}

void CFlusher::Queue()
	{
	if (iPeriod.Int() > 0)
		{
		After(iPeriod);
		}
	}

void CFlusher::RunL()
	{
	Queue();
	iReader.FlushL();	
	}

const TInt KMaxCategories = 256;
const TInt KBlockSize = 512;
const TInt KArrayGranularity = 4;
const TUint KObserveAllCategories = 0xFFFFFFFFu;


TBtraceFrame::TBtraceFrame(const TBtraceHeader& aHeader)
	: iSize(aHeader.iSize), iFlags(aHeader.iFlags), iCategory(aHeader.iCategory), iSubCategory(aHeader.iSubCategory)
	{
	}


EXPORT_C CBtraceReader* CBtraceReader::NewL(TMode aMode, TInt aBufferSize, TInt aThreshold, MBtraceLog* aLog)
	{
	CBtraceReader* self = new(ELeave) CBtraceReader(aMode, aThreshold, aLog);
	CleanupStack::PushL(self);
	self->ConstructL(aBufferSize);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBtraceReader* CBtraceReader::NewL(const TDesC& aReplayFileName, MBtraceLog* aLog)
	{
	CBtraceReader* self = new(ELeave) CBtraceReader(EAllowBtraceToOverflow, 0, aLog);
	CleanupStack::PushL(self);
	self->ConstructL(aReplayFileName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBtraceReader::~CBtraceReader()
	{
	delete iFlusher;
	Cancel();
	DisableBtrace();
	iBtrace.Close();
#ifdef FSHELL_ATRACE_SUPPORT
	iAtraceConfig.Close();
#endif
	iObservers.Close();
	iReplayFile.Close();
	iRecordFile.Close();
	iFs.Close();
	delete iReplayData;
	iMultipartFragments.ResetAndDestroy();
	}

EXPORT_C void CBtraceReader::Reset()
	{
	iReplayFile.Close();
	iRecordFile.Close();
	delete iReplayData;
	ResetAndReprimeL();
	}

EXPORT_C void CBtraceReader::SynchronizeL()
	{
	Log(_L("Synchronizing clock time with btrace time...\r\n"));
	__ASSERT_ALWAYS(!iSynchronizing, Panic(EBtpPanicNestedSynchronizeCalls));
	iSynchronizing = ETrue;
	FlushL(); // FlushL now takes care of putting a sync frame in
	Log(_L("Synchronize complete\r\n"));
	}

EXPORT_C void CBtraceReader::Start(const TBtraceTickCount& aInitialTickCount)
	{
	iInitialTickCount = aInitialTickCount;
	QueueRead();
	}

EXPORT_C void CBtraceReader::Start(const TBtraceTickCount& aInitialTickCount, TTimeIntervalMicroSeconds32 aAutoFlushPeriod)
	{
	if (iFlusher)
		{
		iFlusher->Stop();
		}
	else
		{
		iFlusher = CFlusher::NewL(*this);
		}

	iFlusher->Start(aAutoFlushPeriod);
	Start(aInitialTickCount);
	}

EXPORT_C void CBtraceReader::DisableBtrace()
	{
	if (iBtrace.Handle())
		{
		// Turn everything off.
		for (TInt i = 0; i < KMaxCategories; ++i)
			{
			iBtrace.SetFilter(i, 0);
			}
		iBtrace.SetMode(0);
		}
	}

EXPORT_C void CBtraceReader::EnableCategoryL(TUint aCategory)
	{
#ifdef FSHELL_ATRACE_SUPPORT
	if (iAtraceConfig.Handle() && aCategory == BTrace::EProfiling)
		{
		User::LeaveIfError(iAtraceConfig.EnableProfiling(1));
		}
	else
#endif
	if (iBtrace.Handle())
		{
		User::LeaveIfError(iBtrace.SetFilter(aCategory, 1));
		}
	}

EXPORT_C void CBtraceReader::DisableCategoryL(TUint aCategory)
	{
	// Check no-one's using it
	TInt found = iObservers.SpecificFindInUnsignedKeyOrder(TObserver::ArrayKey(aCategory), EArrayFindMode_Any);
	if (found != KErrNotFound) User::Leave(KErrInUse);

	if (aCategory == BTrace::EMetaTrace) User::Leave(KErrInUse);

#ifdef FSHELL_ATRACE_SUPPORT
	if (iAtraceConfig.Handle() && aCategory == BTrace::EProfiling)
		{
		User::LeaveIfError(iAtraceConfig.EnableProfiling(EFalse));
		}
	else
#endif
		if (iBtrace.Handle())
		{
		User::LeaveIfError(iBtrace.SetFilter(aCategory, 0));
		}
	}
	
EXPORT_C void CBtraceReader::AddObserverL(MBtraceObserver& aObserver)
	{
	AddObserverL(aObserver, ENormal);
	}

EXPORT_C void CBtraceReader::AddObserverL(MBtraceObserver& aObserver, TObserverType aType)
	{
	__ASSERT_ALWAYS(!iIteratingObservers, Panic(EBtpPanicAddObserverCalledWhileIterating));
	TObserver observer(aObserver, aType, KObserveAllCategories);
	User::LeaveIfError(iObservers.InsertInUnsignedKeyOrderAllowRepeats(observer));
	}

EXPORT_C void CBtraceReader::AddObserverL(TUint aCategory, MBtraceObserver& aObserver)
	{
	AddObserverL(aCategory, aObserver, ENormal);
	}

EXPORT_C void CBtraceReader::AddObserverL(TUint aCategory, MBtraceObserver& aObserver, TObserverType aType)
	{
	__ASSERT_ALWAYS(!iIteratingObservers, Panic(EBtpPanicAddObserverCalledWhileIterating));
	EnableCategoryL(aCategory);
	TObserver observer(aObserver, aType, aCategory);
	User::LeaveIfError(iObservers.InsertInUnsignedKeyOrderAllowRepeats(observer));
	}

TBool CBtraceReader::ObserversEqual(const TObserver& aA, const TObserver& aB)
	{
	return (aA.iObserver == aB.iObserver && aA.iCategory == aB.iCategory);
	}

EXPORT_C void CBtraceReader::RemoveObserver(MBtraceObserver& aObserver)
	{
	__ASSERT_ALWAYS(!iIteratingObservers, Panic(EBtpPanicRemoveObserverCalledWhileIterating));
	TObserver observer(aObserver, ENormal, KObserveAllCategories);
	TInt pos = iObservers.Find(observer, TIdentityRelation<TObserver>(&ObserversEqual));
	if (pos >= 0)
		{
		iObservers.Remove(pos);
		}
	}

EXPORT_C void CBtraceReader::RemoveObserver(TUint aCategory, MBtraceObserver& aObserver)
	{
	__ASSERT_ALWAYS(!iIteratingObservers, Panic(EBtpPanicRemoveObserverCalledWhileIterating));
	// There could be multiple observers for each category, so find the last one...
	TObserver observer(aObserver, ENormal, aCategory);
	TInt pos = iObservers.SpecificFindInUnsignedKeyOrder(observer, EArrayFindMode_Last);
	if (pos >= 0)
		{
		// ...now do a linear search for this specific one (backwards because the observer can then be removed without messing up the loop index).
		TBool specificObserverFound(EFalse);
		TInt numCatObservers = 0;
		for (TInt i = (pos - 1); i >= 0; --i)
			{
			const TObserver& theObserver = iObservers[i];
			if (theObserver.iCategory == aCategory)
				{
				if (theObserver.iObserver == &aObserver)
					{
					iObservers.Remove(i);
					specificObserverFound = ETrue;
					}
				else
					{
					++numCatObservers;
					}
				}
			else
				{
				break;
				}
			}
		//__ASSERT_DEBUG(specificObserverFound, Panic(EBtpPanicObserverNotFound));
		if (specificObserverFound && (numCatObservers == 0) && (iBtrace.Handle()))
			{
			// This was the last observer of this category, so turn it off.
			DisableCategoryL(aCategory);
			}
		}
	}
	
EXPORT_C void CBtraceReader::AddFrameObseverL(MBtraceFrameObserver& aObserver)
	{
	if (iFrameObserver) Panic(EBtpPanicFrameObserverAlreadyExists);
	iFrameObserver = &aObserver;
	}
	
EXPORT_C void CBtraceReader::RemoveFrameObserver(MBtraceFrameObserver& aObserver)
	{
	if (iFrameObserver == &aObserver)
		{
		iFrameObserver = NULL;
		}
	}

EXPORT_C void CBtraceReader::GetReplayStartTickCountL(TBtraceTickCount& aTickCount)
	{
	__ASSERT_ALWAYS(iReplayData != NULL, Panic(EBtpPanicReaderNotReplaying));

	aTickCount.iNano = 0;
	aTickCount.iFast = 0;

	// Read the first btrace frame to find its tick count.
	TUint8* data;
	TInt size = GetData(data);
	__ASSERT_ALWAYS(size > 0, Panic(EBtpPanicNoReplayData));
	TUint8* c = data;
	TBtraceHeader* header = (TBtraceHeader*)c;
	TUint8* d = c + sizeof(TBtraceHeader);
	if ((header->iFlags & BTrace::EMissingRecord) && (c != data))
		{
		Log(_L("Warning: btrace buffer overflowed\r\n"));
		User::Leave(KErrOverflow);
		}
	if (header->iFlags & BTrace::EHeader2Present)
		{
		d += 4;
		}
	if (header->iFlags & BTrace::ETimestampPresent)
		{
		aTickCount.iFast = *((TUint32*)d);
		d += 4;
		}
	if (header->iFlags & BTrace::ETimestamp2Present)
		{
		aTickCount.iNano = *((TUint32*)d);
		d += 4;
		}

	// Rewind the file to ensure the first btrace frame is read again when the buffer
	// is properly parsed.
	TInt pos = 0;
	User::LeaveIfError(iReplayFile.Seek(ESeekStart, pos));
	}

EXPORT_C void CBtraceReader::SetBufferSizeL(TInt aBufferSize)
	{
	if (iBtrace.Handle() == 0)
		{
		User::Leave(KErrNotReady);
		}
	User::LeaveIfError(iBtrace.ResizeBuffer(aBufferSize));
	ResetAndReprimeL(); // This is needed to a) reprime, and b) switch btrace back on. Calling ResizeBuffer actually sets the mode to zero. Who knew?
	}

EXPORT_C void CBtraceReader::SetReplayFileNameL(const TDesC& aReplayFileName)
	{
	__ASSERT_ALWAYS(aReplayFileName.Length() || iBtrace.Handle(), Panic(EBtpPanicTriedToDisableReplayWhenBtraceNotRunning)); // Can't transition from replay to live btrace if btrace was never opened (ie if the NewL overload taking a replay file name was used)
	
	if (iReplayFile.SubSessionHandle() > 0)
		{
		iReplayFile.Close();
		}

	if (aReplayFileName.Length())
		{
		User::LeaveIfError(iReplayFile.Open(iFs, aReplayFileName, EFileRead));
		if (iReplayData == NULL)
			{
			iReplayData = HBufC8::NewL(KBlockSize);
			}
		}
	else
		{
		delete iReplayData;
		iReplayData = NULL;
		}
	}

EXPORT_C const TBtraceTickCount& CBtraceReader::CurrentTickCount() const
	{
	Log(_L("Tick count: %u (%u)\r\n"), iCurrentTickCount.iNano, iCurrentTickCount.iFast);
	return iCurrentTickCount;
	}

void CBtraceReader::RunL()
	{
	switch (iMode)
		{
		case EAllowBtraceToOverflow:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EBtpPanicUnexpectedReaderRun));
			}
		case EFlushOnBtraceThreshold:
			{
			Log(_L("Btrace buffer crossed threshold - flushing...\r\n"));
			QueueRead();
			FlushL();
			break;
			}
		case ELeaveOnBtraceThreshold:
			{
			Log(_L("Btrace buffer crossed threshold - leaving with KErrTooBig\r\n"));
			User::Leave(KErrTooBig);
			}
		default:
			{
			__ASSERT_ALWAYS(EFalse, Panic(EBtpPanicInvalidReaderMode));
			}
		}
	}

void CBtraceReader::DoCancel()
	{
	if (iBtrace.Handle())
		{
		iBtrace.CancelRequestData();
		}
	}

CBtraceReader::CBtraceReader(TMode aMode, TInt aThreshold, MBtraceLog* aLog)
	: CActive(CActive::EPriorityStandard), iMode(aMode), iLog(aLog), iThreshold(aThreshold), iObservers(KArrayGranularity, _FOFF(TObserver, iCategory))
	{
	CActiveScheduler::Add(this);
	}

void CBtraceReader::ConstructL(TInt aBufferSize)
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iBtrace.Open());
#ifdef FSHELL_ATRACE_SUPPORT
	User::LeaveIfError(iAtraceConfig.Connect());
#endif
	iBtrace.Empty();
	if (iBtrace.BufferSize() < aBufferSize)
		{
		// If the buffer is bigger, don't shrink it
		SetBufferSizeL(aBufferSize);
		}
	iBtrace.SetTimestamp2Enabled(ETrue);

	// Turn everything off.
	for (TInt i = 0; i < KMaxCategories; ++i)
		{
		iBtrace.SetFilter(i, 0);
		}
	// And turn metatrace on (we always keep this on)
	iBtrace.SetFilter(BTrace::EMetaTrace, 1);

	iBtrace.SetMode(RBTrace::EEnable);
	}

void CBtraceReader::ConstructL(const TDesC& aReplayFileName)
	{
	User::LeaveIfError(iFs.Connect());
	SetReplayFileNameL(aReplayFileName);
	}

void CBtraceReader::QueueRead()
	{
	if ((iMode > EAllowBtraceToOverflow) && (iThreshold > 0))
		{
		iBtrace.RequestData(iStatus, iThreshold);
		SetActive();
		}
	}

void CBtraceReader::DoReadL(TBool& aFinished)
	{
	TUint8* data;
	TInt size = GetData(data);
	if (size <= 0)
		{
		aFinished = ETrue;
		iBtraceBuffer.Set(NULL, 0);
		}
	else
		{
		aFinished = EFalse;
		iBtraceBuffer.Set(data, size);
		iNumBytesRead += size;
		TUint8* c = data;
		TUint8* end = c + size;
		do
			{
			TBtraceHeader* header = (TBtraceHeader*)c;
			TBtraceFrame frame(*header);
			TUint8* d = c + sizeof(TBtraceHeader);
			TUint32 extra = 0;
			TUint32 header2 = 0;
			if ((header->iFlags & BTrace::EMissingRecord)) // && (c != data))
				{
				Log(_L("Warning: btrace buffer overflowed\r\n"));
				//TODO: Why check if c != data? A missing record is a missing record isn't it? [TomS]
				//[JoeB] IIRC there was a bug in btrace that caused it to produce a frame with EMissingRecord set
				//       each time the btrace buffer was reset. Earlier releases didn't behave that way
				//       so we hacked our way around it. I'll try removing the hack and see what breaks... 
				User::Leave(KErrOverflow);
				}
			if (header->iFlags & BTrace::EHeader2Present)
				{
				header2 = *((TUint32*)d);
				d += 4;
				}
			if (header->iFlags & BTrace::ETimestampPresent)
				{
				frame.iTickCount.iFast = *((TUint32*)d);
				d += 4;
				}
			if (header->iFlags & BTrace::ETimestamp2Present)
				{
				frame.iTickCount.iNano = *((TUint32*)d);
				d += 4;
				}
			if (header->iFlags & BTrace::EContextIdPresent)
				{
				frame.iThreadContext = *((TUint32*)d);
				d += 4;
				}
			if (header->iFlags & BTrace::EPcPresent)
				{
				frame.iProgramCounter = *((TUint32*)d);
				d += 4;
				}
			if (header->iFlags & BTrace::EExtraPresent)
				{
				extra = *(TUint32*)d;
				d += 4;
				}
			frame.iData.Set(d, (c + header->iSize) - d);
			iCurrentTickCount = frame.iTickCount;

			TBool multipartHandled = EFalse;
			TUint multiFlags = (header2 & BTrace::EMultipartFlagMask);
			if (iMaxTrackedMultipartFragments > 0 && multiFlags != 0)
				{
				multipartHandled = ETrue;
				if (multiFlags == BTrace::EMultipartFirst)
					{
					NewMultipartFrameL(frame, extra);
					}
				else
					{
					TBool last = multiFlags == BTrace::EMultipartLast;
					AdditionalMultipartFragmentL(frame, extra, last);
					}
				}

			if ((frame.iCategory == KAmTraceCategory) && (frame.iSubCategory == EAmTraceSubCategoryReaderSync))
				{
				if (frame.iData.Length() == 0)
					{
					// Shouldn't happen - old-style sync frame. We'll consider this as always matching
					iSyncFrameIdToWaitFor = 0;
					}
				else if (*(TInt*)frame.iData.Ptr() == iSyncFrameIdToWaitFor)
					{
					iSyncFrameIdToWaitFor = 0;
					}
				iSynchronizing = EFalse;
				}
			if (!multipartHandled)
				{
				DispatchFrameL(frame);
				}
			c += (header->iSize + 3) & ~3;
			}
			while (c < end);

		if (iBtrace.Handle())
			{
			iBtrace.DataUsed();
			}
		}
	}

TInt CBtraceReader::GetData(TUint8*& aData)
	{
	TInt size = 0;
	if (iReplayData)
		{
		TPtr8 ptr(iReplayData->Des());
		ptr.Zero();
		TInt err = iReplayFile.Read(ptr, sizeof(TBtraceHeader));
		if ((err == KErrNone) && (ptr.Length() == sizeof(TBtraceHeader)))
			{
			TBtraceHeader* btraceHeader = (TBtraceHeader*)ptr.Ptr();
			const TInt length = ((btraceHeader->iSize + 3) & ~3) - sizeof(TBtraceHeader);
			TPtr8 ptr2(const_cast<TUint8*>(ptr.Ptr()) + sizeof(TBtraceHeader), 0, ptr.MaxLength() - sizeof(TBtraceHeader));
			ASSERT(ptr2.MaxLength() >= length);
			err = iReplayFile.Read(ptr2, length);
			if ((err == KErrNone) && (ptr2.Length() == length))
				{
				aData = const_cast<TUint8*>(ptr.Ptr());
				size = btraceHeader->iSize;
				}
			}
		}
	else
		{
		size = iBtrace.GetData(aData);
		}

	if (size > 0 && iRecordFile.SubSessionHandle())
		{
		TPtrC8 data(aData, size);
		iRecordFile.Write(data);
		// Don't care about any error
		}
	return size;
	}

void CBtraceReader::DispatchFrameL(const TBtraceFrame& aFrame)
	{
	if (iFrameObserver) iFrameObserver->HandleFrameDispatchStart();
	iIteratingObservers++;
	CleanupStack::PushL(TCleanupItem(&DecIteratingObservers, this));
	TBool preStartFrame = (aFrame.iTickCount < iInitialTickCount);
	++iNumFramesRead;
	TObserver arraySearchKey = TObserver::ArrayKey(aFrame.iCategory);
	const TInt numObservers = iObservers.Count();
	TInt pos = iObservers.SpecificFindInUnsignedKeyOrder(arraySearchKey, EArrayFindMode_First);
	for (TInt i = pos; i >= 0 && i < numObservers; ++i)
		{
		const TObserver& theObserver = iObservers[i];
		if (theObserver.iCategory == aFrame.iCategory)
			{
			if ((theObserver.iType == EIncludeSynchronizationFrames) || !preStartFrame || !iSynchronizing)
				{
				theObserver.iObserver->HandleBtraceFrameL(aFrame);
				}
			}
		else
			{
			break;
			}
		}

	// Now dispatch to observers who asked for everything
	arraySearchKey = TObserver::ArrayKey(KObserveAllCategories);
	pos = iObservers.SpecificFindInUnsignedKeyOrder(arraySearchKey, EArrayFindMode_First);
	for (TInt i = pos; i >= 0 && i < numObservers; ++i)
		{
		const TObserver& theObserver = iObservers[i];
		if (theObserver.iCategory == KObserveAllCategories && (theObserver.iType == EIncludeSynchronizationFrames || !preStartFrame))
			{
			theObserver.iObserver->HandleBtraceFrameL(aFrame);
			}
		}
	CleanupStack::PopAndDestroy(); // iIteratingObservers--
	if (iFrameObserver) iFrameObserver->HandleFrameDispatchComplete();
	}

CBtraceReader::TObserver::TObserver(MBtraceObserver& aObserver, TObserverType aType, TUint aCategory)
	: iObserver(&aObserver), iType(aType), iCategory(aCategory)
	{
	}

CBtraceReader::TObserver CBtraceReader::TObserver::ArrayKey(TUint aCategory)
	{
	return TObserver(aCategory);
	}

CBtraceReader::TObserver::TObserver(TUint aCategory)
	: iObserver(NULL), iType(ENormal), iCategory(aCategory)
	{
	}

EXPORT_C void CBtraceReader::RecordBtraceDataToFileL(const TDesC& aRecordFileName)
	{
	if (!iBtrace.Handle()) return; // This method is specced to do nothing if we're replaying
	SynchronizeL(); // Ensure the file won't contain any data from before it asked to start (in the case of starting) or ensure that the file contains everything in the buffer up till this time (in the case of stopping)
	iRecordFile.Close();
	if (aRecordFileName.Length())
		{
		// File must not already exist, we open in exclusive access
		User::LeaveIfError(iRecordFile.Create(iFs, aRecordFileName, EFileShareExclusive|EFileWrite));
		ResetAndReprimeL();
		}
	}

EXPORT_C void CBtraceReader::LogBtraceStatus()
	{
	Log(_L("BTrace categories currently enabled:"));
	for (TInt i = 0; i < KMaxCategories; ++i)
		{
		TBool isOn = iBtrace.Filter(i) == 1; // 0 means off, 1 is on, KErrNotSupported means off and not supported
		if (isOn)
			{
			Log(_L("%d"), i);
			}
		}
	}

void CBtraceReader::NotifyObserversOfBufferReset()
	{
	iIteratingObservers++;
	const TInt numObservers = iObservers.Count();
	for (TInt i = 0; i < numObservers; ++i)
		{
		const TObserver& theObserver = iObservers[i];
		theObserver.iObserver->BtraceBufferHasBeenReset();
		}
	iIteratingObservers--;
	}

void CBtraceReader::DecIteratingObservers(TAny* aSelf)
	{
	static_cast<CBtraceReader*>(aSelf)->iIteratingObservers--;
	}

void CBtraceReader::ResetAndReprimeL()
	{
	iBtrace.Empty();
	iBtrace.SetMode(RBTrace::EEnable);
	iBtrace.SetTimestamp2Enabled(ETrue);
	iMultipartFragments.ResetAndDestroy();
	// This seems to be the only way to force repriming
	for (TInt i = 0; i < KMaxCategories; ++i)
		{
		TBool isOn = iBtrace.Filter(i) == 1; // 0 means off, 1 is on, KErrNotSupported means off and not supported
		if (isOn)
			{
			TInt err = iBtrace.SetFilter(i, 0);
			err = iBtrace.SetFilter(i, 1);
			}
		}
	NotifyObserversOfBufferReset();
	}

EXPORT_C void CBtraceReader::FlushL()
	{
	Log(_L("Flushing btrace buffer...\r\n"));

	// Inject a sync frame so we know when the flush should be considered finished (in case there is a steady stream of data coming in)
	TBool categoryAlreadySet = iBtrace.Filter(KAmTraceCategory);
	if (!categoryAlreadySet)
		{
		User::LeaveIfError(iBtrace.SetFilter(KAmTraceCategory, 1));
		}
	iNextSyncFrameId++;
	iSyncFrameIdToWaitFor = iNextSyncFrameId;
	BTrace4(KAmTraceCategory, EAmTraceSubCategoryReaderSync, iSyncFrameIdToWaitFor);

	TInt flushErr = KErrNone;
	TBool fullyFlushed(EFalse);
	while (flushErr == KErrNone && !fullyFlushed && iSyncFrameIdToWaitFor != 0)
		{
		TRAP(flushErr, DoReadL(fullyFlushed)); // Trap this to make sure we do the cleanup in the rest of this function
		}

	if (iSyncFrameIdToWaitFor)
		{
		Log(_L("Warning: got to end of buffer without finding sync frame!\r\n"));
		iSyncFrameIdToWaitFor = 0;
		// Important to clean this up, otherwise a future call to Synchronize will panic
		iSynchronizing = EFalse;
		}

	if (!categoryAlreadySet)
		{
		iBtrace.SetFilter(KAmTraceCategory, 0);
		}
	if (flushErr)
		{
		Log(_L("Flush completed with error %d\r\n"), flushErr);
		User::Leave(flushErr);
		}
	Log(_L("Flush complete\r\n"));
	}

void CBtraceReader::Log(const TDesC& aDes) const
	{
	if (iLog && iLog->LogEnabled())
		{
		iLog->Log(aDes);
		}
	}

class TOverflowTruncate : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};

void CBtraceReader::Log(TRefByValue<const TDesC> aFmt, ...) const
	{
	if (iLog && iLog->LogEnabled())
		{
		TOverflowTruncate overflow;
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf<0x100> buf;
		buf.AppendFormatList(aFmt, list, &overflow);
		iLog->Log(buf);
		}
	}

EXPORT_C void CBtraceReader::SetMultipartReassemblyL(TInt aMaxTrackedFragments)
	{
	iMaxTrackedMultipartFragments = aMaxTrackedFragments;
	while (iMultipartFragments.Count() > iMaxTrackedMultipartFragments)
		{
		delete iMultipartFragments[0];
		iMultipartFragments.Remove(0);
		}
	iMultipartFragments.ReserveL(iMaxTrackedMultipartFragments);
	}

CMultipartFragment* CMultipartFragment::NewLC(const TBtraceFrame& aFirstFrame, TUint32 aExtra)
	{
	CMultipartFragment* res = new(ELeave) CMultipartFragment(aFirstFrame, aExtra);
	CleanupStack::PushL(res);
	res->ConstructL();
	return res;
	}

CMultipartFragment::CMultipartFragment(const TBtraceFrame& aFirstFrame, TUint32 aExtra)
	: iFirstFrame(aFirstFrame), iUniqueId(aExtra)
	{
	}

CMultipartFragment::CMultipartFragment(TUint32 aExtra)
	: iFirstFrame(TBtraceHeader()), iUniqueId(aExtra)
	{
	}

void CMultipartFragment::ConstructL()
	{
	// header2 and extra have been stripped off by this point.
	iTotalSize = *(TInt32*)iFirstFrame.iData.Ptr();
	iData.CreateL(iTotalSize+4);
	iData.Copy(iFirstFrame.iData.Mid(4)); // iData now has A and the first X bytes of D
	iFirstFrame.iData.Set(iData); // We keep iFirstFrame's iData set to the running total of the current data
	}

CMultipartFragment::~CMultipartFragment()
	{
	iData.Close();
	}

TInt CMultipartFragment::Compare(const CMultipartFragment& aFirst, const CMultipartFragment& aSecond)
	{
	return aSecond.iUniqueId - aFirst.iUniqueId;
	}

void CMultipartFragment::AddDataL(const TDesC8& aData)
	{
	// iData was allocated to the total size, so this should never fail
	iData.Append(aData);
	iFirstFrame.iData.Set(iData); // We keep iFirstFrame's iData set to the running total of the current data
	}

void CBtraceReader::NewMultipartFrameL(const TBtraceFrame& aFrame, TUint32 aExtra)
	{
	if (aFrame.iData.Size() < 8)
		{
		// Can't be a real multipart frame, has to have N and A (as per TMultiPart docs)
		Log(_L("Warning: Corrupt multipart btrace frame found for cat=%d subcat=%d\r\n"), (TInt)aFrame.iCategory, (TInt)aFrame.iSubCategory);
		}
	else
		{
		if (iMultipartFragments.Count() == iMaxTrackedMultipartFragments)
			{
			// We have to drop a fragment on the floor
			Log(_L("Warning: Maximum number of multipart fragments reached (%d), dropping first (cat=%d subcat=%d)\r\n"), iMaxTrackedMultipartFragments, iMultipartFragments[0]->Category(), iMultipartFragments[0]->SubCategory());
			delete iMultipartFragments[0];
			iMultipartFragments.Remove(0);
			}

		CMultipartFragment* frag = CMultipartFragment::NewLC(aFrame, aExtra);
		iMultipartFragments.InsertInOrder(frag, &CMultipartFragment::Compare);
		CleanupStack::Pop(frag);
		}
	}

void CBtraceReader::AdditionalMultipartFragmentL(const TBtraceFrame& aFrame, TUint32 aExtra, TBool aLast)
	{
	if (aFrame.iData.Size() < 8)
		{
		// Can't be a real multipart frame, has to have N and A (as per TMultiPart docs)
		Log(_L("Warning: Corrupt multipart btrace frame found for cat=%d subcat=%d\r\n"), (TInt)aFrame.iCategory, (TInt)aFrame.iSubCategory);
		}
	TInt offset = ((TInt32*)aFrame.iData.Ptr())[1];
	CMultipartFragment dummySearchKey(aExtra);
	TInt pos = iMultipartFragments.FindInOrder(&dummySearchKey, &CMultipartFragment::Compare);
	if (pos == KErrNotFound)
		{
		Log(_L("Warning: Couldn't find multipart fragment for frame with cat=%d subcat=%d offset=%d\r\n"), aFrame.iCategory, aFrame.iSubCategory, offset);
		return;
		}
	CMultipartFragment* frag = iMultipartFragments[pos];
	if (frag->Frame().iData.Size() - 4 != offset)
		{
		// Subtract 4 because iData also has the 'A' parameter in it, whereas offset is into the payload data
		Log(_L("Warning: Out of order multipart fragment - expected offset %d, was actually %d\r\n"), frag->Frame().iData.Size() - 4, offset);
		// If we've dropped a bit, we're lost
		delete frag;
		iMultipartFragments.Remove(pos);
		}
	else
		{
		frag->AddDataL(aFrame.iData.Mid(8));
		}

	if (aLast)
		{
		if (frag->TotalSize() != frag->Frame().iData.Size() - 4)
			{
			Log(_L("Warning: multipart fragment is supposed to be finished but is wrong size - expected %d actually %d\r\n"), frag->TotalSize(), frag->Frame().iData.Size() - 4);
			// Dispatch it anyway, it's the client's problem now (note we don't do this if we've knowingly dropped part of the frame)
			}
		iMultipartFragments.Remove(pos);
		CleanupStack::PushL(frag);
		DispatchFrameL(frag->Frame());
		CleanupStack::PopAndDestroy(frag);
		}
	}
