// memsampler.cpp
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

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <e32cons.h>
#include FSHELL_D32BTRACE_HEADER
#include "memsamplerdd.h"

_LIT(KLdd, "memsamplerdd");	
const TInt KMaxCategories = 256;
const TInt KBtraceBufferSize = 64 * 1024;

using namespace IoUtils;


class TChunkInfo
	{
public:
	TChunkInfo(TUint32 aAddress);
	TChunkInfo(const TDesC& aName, TUint32 aAddress, TInt aMaxSize);
	static TBool Match(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByUpdateHistoryAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByUpdateHistoryDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByNameAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByNameDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByMaxSizeAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByMaxSizeDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByCurrentSizeAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByCurrentSizeDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByHighWaterMarkAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
	static TInt SortByHighWaterMarkDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB);
public:
	TInt iUpdateCount;
	TFullName iName;
	TUint32 iAddress;
	TInt iMaxSize;
	TInt iCurrentSize;
	TInt iHighWaterMark;
	};

TChunkInfo::TChunkInfo(TUint32 aAddress)
	: iUpdateCount(0), iAddress(aAddress), iMaxSize(0), iCurrentSize(0), iHighWaterMark(0)
	{
	}

TChunkInfo::TChunkInfo(const TDesC& aName, TUint32 aAddress, TInt aMaxSize)
	: iUpdateCount(0), iName(aName), iAddress(aAddress), iMaxSize(aMaxSize), iCurrentSize(0), iHighWaterMark(0)
	{
	}

TBool TChunkInfo::Match(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iAddress == aChunkInfoB.iAddress);
	}

TInt TChunkInfo::SortByUpdateHistoryAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iUpdateCount > aChunkInfoB.iUpdateCount);
	}

TInt TChunkInfo::SortByUpdateHistoryDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iUpdateCount < aChunkInfoB.iUpdateCount);
	}

TInt TChunkInfo::SortByNameAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iName.CompareC(aChunkInfoB.iName));
	}

TInt TChunkInfo::SortByNameDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoB.iName.CompareC(aChunkInfoA.iName));
	}

TInt TChunkInfo::SortByMaxSizeAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iMaxSize > aChunkInfoB.iMaxSize);
	}

TInt TChunkInfo::SortByMaxSizeDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iMaxSize < aChunkInfoB.iMaxSize);
	}

TInt TChunkInfo::SortByCurrentSizeAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iCurrentSize > aChunkInfoB.iCurrentSize);
	}

TInt TChunkInfo::SortByCurrentSizeDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iCurrentSize < aChunkInfoB.iCurrentSize);
	}

TInt TChunkInfo::SortByHighWaterMarkAscending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iHighWaterMark > aChunkInfoB.iHighWaterMark);
	}

TInt TChunkInfo::SortByHighWaterMarkDescending(const TChunkInfo& aChunkInfoA, const TChunkInfo& aChunkInfoB)
	{
	return (aChunkInfoA.iHighWaterMark < aChunkInfoB.iHighWaterMark);
	}


class CMemoryView : public CBase
	{
public:
	enum TSortType
		{
		ESortUnspecified,
		ESortByChunkName,
		ESortByCurrentSize,
		ESortByHighWaterMark,
		ESortByMaxSize,
		ESortByUpdateHistory
		};
	enum TSortOrder
		{
		EAscending,
		EDescending
		};
public:
	static CMemoryView* NewL(RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, TBool aHuman);
	~CMemoryView();
	void ChangeSortType(TSortType aSortType);
	void ChangeSortOrder(TSortOrder aSortOrder);
	void ScrollUp();
	void ScrollDown();
	void ScrollLeft();
	void ScrollRight();
	void HandleNewChunk(const TDesC& aName, TUint32 aAddress, TInt aMaxSize);
	void HandleChangedChunk(TUint32 aAddress, TInt aSize, TInt aHighWaterMark);
	void HandleDeletedChunk(TUint32 aAddress);
	void UpdateL();
private:
	CMemoryView(RIoWriteHandle& aStderr, TBool aHuman);
	void ConstructL(RIoWriteHandle& aStdout);
	void Sort();
	void PrintWarning(TRefByValue<const TDesC> aFmt, ...);
private:
	RIoWriteHandle& iStderr;
	TBool iHuman;
	RIoConsoleWriteHandle iStdout;
	TInt iNumConsoleLines;
	TInt iNumLinesInLastUpdate;
	RArray<TChunkInfo> iChunkList;
	CTextBuffer* iBuffer;
	CTextFormatter* iFormatter;
	TSortOrder iSortOrder;
	TLinearOrder<TChunkInfo> iSortByUpdateHistoryAscending;
	TLinearOrder<TChunkInfo> iSortByUpdateHistoryDescending;
	TLinearOrder<TChunkInfo> iSortByChunkNameAscending;
	TLinearOrder<TChunkInfo> iSortByChunkNameDescending;
	TLinearOrder<TChunkInfo> iSortByCurrentSizeAscending;
	TLinearOrder<TChunkInfo> iSortByCurrentSizeDescending;
	TLinearOrder<TChunkInfo> iSortByHighWaterMarkAscending;
	TLinearOrder<TChunkInfo> iSortByHighWaterMarkDescending;
	TLinearOrder<TChunkInfo> iSortByMaxSizeAscending;
	TLinearOrder<TChunkInfo> iSortByMaxSizeDescending;
	TLinearOrder<TChunkInfo>* iCurrentSortType;
	TInt iVerticalOffset;
	TInt iHorizontalOffset;
	};

CMemoryView* CMemoryView::NewL(RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, TBool aHuman)
	{
	CMemoryView* self = new(ELeave) CMemoryView(aStderr, aHuman);
	CleanupStack::PushL(self);
	self->ConstructL(aStdout);
	CleanupStack::Pop(self);
	return self;
	}

CMemoryView::~CMemoryView()
	{
	delete iBuffer;
	delete iFormatter;
	iChunkList.Close();
	}

void CMemoryView::ChangeSortType(TSortType aSortType)
	{
	switch (aSortType)
		{
		default:
		case ESortByChunkName:
			{
			if (iCurrentSortType == &iSortByChunkNameAscending)
				{
				iCurrentSortType = &iSortByChunkNameDescending;
				}
			else if (iCurrentSortType == &iSortByChunkNameDescending)
				{
				iCurrentSortType = &iSortByChunkNameAscending;
				}
			else
				{
				iCurrentSortType = &iSortByChunkNameAscending;
				}
			break;
			}
		case ESortByCurrentSize:
			{
			if (iCurrentSortType == &iSortByCurrentSizeAscending)
				{
				iCurrentSortType = &iSortByCurrentSizeDescending;
				}
			else if (iCurrentSortType == &iSortByCurrentSizeDescending)
				{
				iCurrentSortType = &iSortByCurrentSizeAscending;
				}
			else
				{
				iCurrentSortType = &iSortByCurrentSizeDescending;
				}
			break;
			}
		case ESortByHighWaterMark:
			{
			if (iCurrentSortType == &iSortByHighWaterMarkAscending)
				{
				iCurrentSortType = &iSortByHighWaterMarkDescending;
				}
			else if (iCurrentSortType == &iSortByHighWaterMarkDescending)
				{
				iCurrentSortType = &iSortByHighWaterMarkAscending;
				}
			else
				{
				iCurrentSortType = &iSortByHighWaterMarkDescending;
				}
			break;
			}
		case ESortByMaxSize:
			{
			if (iCurrentSortType == &iSortByMaxSizeAscending)
				{
				iCurrentSortType = &iSortByMaxSizeDescending;
				}
			else if (iCurrentSortType == &iSortByMaxSizeDescending)
				{
				iCurrentSortType = &iSortByMaxSizeAscending;
				}
			else
				{
				iCurrentSortType = &iSortByMaxSizeDescending;
				}
			break;
			}
		case ESortByUpdateHistory:
			{
			if (iCurrentSortType == &iSortByUpdateHistoryAscending)
				{
				iCurrentSortType = &iSortByUpdateHistoryDescending;
				}
			else if (iCurrentSortType == &iSortByUpdateHistoryDescending)
				{
				iCurrentSortType = &iSortByUpdateHistoryAscending;
				}
			else
				{
				iCurrentSortType = &iSortByUpdateHistoryAscending;
				}
			break;
			}
		}
	Sort();
	}

void CMemoryView::ChangeSortOrder(TSortOrder aSortOrder)
	{
	iSortOrder = aSortOrder;
	}

void CMemoryView::ScrollUp()
	{
	if (iChunkList.Count() > iNumConsoleLines)
		{
		if (iVerticalOffset > 0)
			{
			// Not yet reached the top.
			--iVerticalOffset;
			}
		}
	}

void CMemoryView::ScrollDown()
	{
	const TInt numChunks = iChunkList.Count();
	if (numChunks > iNumConsoleLines)
		{
		if ((numChunks - iVerticalOffset) > iNumConsoleLines)
			{
			// Not yet reached the bottom.
			++iVerticalOffset;
			}
		}
	}

void CMemoryView::ScrollLeft()
	{
	if (iHorizontalOffset > 0)
		{
		--iHorizontalOffset;
		}
	}

void CMemoryView::ScrollRight()
	{
	// Allow the horizontal offset to increase unboundedly here - it'll be limited to something sensible in UpdateL.
	++iHorizontalOffset;
	}

void CMemoryView::HandleNewChunk(const TDesC& aName, TUint32 aAddress, TInt aMaxSize)
	{
	// Age all the existing chunk's update counts by one.
	const TInt numChunks = iChunkList.Count();
	for (TInt i = 0; i < numChunks; ++i)
		{
		++iChunkList[i].iUpdateCount;
		}

	// Insert the new chunk (claiming update count zero).
	TChunkInfo newChunkInfo(aName, aAddress, aMaxSize);
	TInt err = iChunkList.InsertInOrderAllowRepeats(newChunkInfo, *iCurrentSortType);
	if (err)
		{
		PrintWarning(_L("Couldn't handle new chunk: %d"), err);
		}
	}

void CMemoryView::HandleChangedChunk(TUint32 aAddress, TInt aSize, TInt aHighWaterMark)
	{
	TChunkInfo chunkInfo(aAddress);
	TInt pos = iChunkList.Find(chunkInfo, TChunkInfo::Match);
	if (pos < 0)
		{
		PrintWarning(_L("Couldn't handle updated to chunk 0x%08x: %d"), aAddress, pos);
		}
	else
		{
		TChunkInfo& c = iChunkList[pos];
		// Age all the chunks that have been updated more recently than this one.
		const TInt numChunks = iChunkList.Count();
		for (TInt i = 0; i < numChunks; ++i)
			{
			TChunkInfo& d = iChunkList[i];
			if (d.iUpdateCount < c.iUpdateCount)
				{
				++d.iUpdateCount;
				}
			}

		c.iUpdateCount = 0;
		c.iCurrentSize = aSize;
		c.iHighWaterMark = aHighWaterMark;
		Sort();
		}
	}

void CMemoryView::HandleDeletedChunk(TUint32 aAddress)
	{
	TChunkInfo chunkInfo(aAddress);
	TInt pos = iChunkList.Find(chunkInfo, TChunkInfo::Match);
	if (pos < 0)
		{
		PrintWarning(_L("Couldn't handle deletion of chunk 0x%08x: %d"), aAddress, pos);
		}
	else
		{
		TChunkInfo& c = iChunkList[pos];

		// Rejuvenate chunks that haven't been updated since this one last was.
		const TInt numChunks = iChunkList.Count();
		for (TInt i = 0; i < numChunks; ++i)
			{
			TChunkInfo& d = iChunkList[i];
			if (d.iUpdateCount > c.iUpdateCount)
				{
				--d.iUpdateCount;
				}
			}
		
		iChunkList.Remove(pos);
		if (iChunkList.Count() <= iNumConsoleLines)
			{
			iVerticalOffset = 0;
			}
		}
	}

CMemoryView::CMemoryView(RIoWriteHandle& aStderr, TBool aHuman)
	: iStderr(aStderr),
	iHuman(aHuman),
	iSortByUpdateHistoryAscending(TChunkInfo::SortByUpdateHistoryAscending),
	iSortByUpdateHistoryDescending(TChunkInfo::SortByUpdateHistoryDescending),
	iSortByChunkNameAscending(TChunkInfo::SortByNameAscending),
	iSortByChunkNameDescending(TChunkInfo::SortByNameDescending),
	iSortByCurrentSizeAscending(TChunkInfo::SortByCurrentSizeAscending),
	iSortByCurrentSizeDescending(TChunkInfo::SortByCurrentSizeDescending),
	iSortByHighWaterMarkAscending(TChunkInfo::SortByHighWaterMarkAscending),
	iSortByHighWaterMarkDescending(TChunkInfo::SortByHighWaterMarkDescending),
	iSortByMaxSizeAscending(TChunkInfo::SortByMaxSizeAscending),
	iSortByMaxSizeDescending(TChunkInfo::SortByMaxSizeDescending),
	iCurrentSortType(&iSortByUpdateHistoryAscending)
	{
	}

void CMemoryView::ConstructL(RIoWriteHandle& aStdout)
	{
	if (aStdout.AttachedToConsole())
		{
		iStdout = aStdout;
		TSize size;
		User::LeaveIfError(iStdout.GetScreenSize(size));
		iNumConsoleLines = size.iHeight;
		iBuffer = CTextBuffer::NewL(0x100);
		iFormatter = CTextFormatter::NewL(size.iWidth);
		iStdout.SetCursorHeight(0);
		iStdout.ClearScreen();
		}
	else
		{
		PrintWarning(_L("memsampler can't run unless it is attached directly to a console, aborting..."));
		User::Leave(KErrArgument);
		}

	}

void CMemoryView::Sort()
	{
	iChunkList.Sort(*iCurrentSortType);
	}

void CMemoryView::UpdateL()
	{
	iBuffer->Zero();
	iFormatter->Zero();
	const TInt numChunks = iChunkList.Count();
	iBuffer->AppendL(_L("Chunk name\t   Current\t       Max\t      Peak\r\n"));
	TInt numLines = 1;
	for (TInt i = iVerticalOffset; i < numChunks; ++i)
		{
		++numLines;
		const TChunkInfo& chunkInfo = iChunkList[i];
		if (iHuman)
			{
			if (iHorizontalOffset >= chunkInfo.iName.Length())
				{
				// The horizontal offset is larger than this chunk name - reduce it to stop the chunk name column disappearing altogether.
				// Note, ideally it would be nice to limit horizontal scrolling when the right most part of all the chunk names is visible.
				// However, that would involve calculating all the column widths twice which seems a bit clumsy.
				iHorizontalOffset = chunkInfo.iName.Length() - 1;
				}
			TPtrC name(chunkInfo.iName.Mid(iHorizontalOffset));
			iBuffer->AppendFormatL(_L("%S\t"), &name);
			iBuffer->AppendHumanReadableSizeL(chunkInfo.iCurrentSize, EColumnAlignedRight);
			iBuffer->AppendL(_L("\t"));
			iBuffer->AppendHumanReadableSizeL(chunkInfo.iMaxSize, EColumnAlignedRight);
			iBuffer->AppendL(_L("\t"));
			iBuffer->AppendHumanReadableSizeL(chunkInfo.iHighWaterMark, EColumnAlignedRight);
			iBuffer->AppendL(_L("\r\n"));
			}
		else
			{
			iBuffer->AppendFormatL(_L("%S\t%d\t%d\t%d\r\n"), &chunkInfo.iName, chunkInfo.iCurrentSize, chunkInfo.iMaxSize, chunkInfo.iHighWaterMark);
			}
		if (numLines >= (iNumConsoleLines - 1))
			{
			break;
			}
		}
	iFormatter->TabulateL(0, 1, iBuffer->Descriptor(), ETruncateLongestColumn);
	User::LeaveIfError(iStdout.SetCursorPosAbs(TPoint(0, 0)));
	iStdout.Write(iFormatter->Descriptor());
	TInt numOldLines = iNumLinesInLastUpdate - numLines;
	while (numOldLines > 0)
		{
		iStdout.ClearToEndOfLine();
		iStdout.SetCursorPosRel(TPoint(0, 1));
		--numOldLines;
		}
	iNumLinesInLastUpdate = numLines;
	}

void CMemoryView::PrintWarning(TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf(_L("Warning: "));
	buf.AppendFormatList(aFmt, list, &overflow);
	buf.AppendFormat(_L("\r\n"), &overflow);
	iStderr.Write(buf);
	}


class CStdinReader : public CActive
	{
public:
	static CStdinReader* NewL(RIoReadHandle& aStdin, CCommandBase& aCommand, CMemoryView* aMemoryView);
	~CStdinReader();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
private:
	CStdinReader(RIoReadHandle& aStdin, CCommandBase& aCommand, CMemoryView* aMemoryView);
	void QueueRead();
private:
	RIoConsoleReadHandle iStdin;
	CCommandBase& iCommand;
	CMemoryView* iMemoryView;
	};

CStdinReader* CStdinReader::NewL(RIoReadHandle& aStdin, CCommandBase& aCommand, CMemoryView* aMemoryView)
	{
	CStdinReader* self = new(ELeave) CStdinReader(aStdin, aCommand, aMemoryView);
	self->QueueRead();
	return self;
	}

CStdinReader::~CStdinReader()
	{
	Cancel();
	}

void CStdinReader::RunL()
	{
	if (iStatus.Int())
		{
		iCommand.Complete(iStatus.Int());
		}
	else
		{
		TBool noUpdate(EFalse);
		CMemoryView::TSortType newSortType = CMemoryView::ESortUnspecified;
		switch (iStdin.KeyCode())
			{
			case 'n':
			case 'N':
				{
				newSortType = CMemoryView::ESortByChunkName;
				break;
				}
			case 'c':
			case 'C':
				{
				newSortType = CMemoryView::ESortByCurrentSize;
				break;
				}
			case 'p':
			case 'P':
				{
				newSortType = CMemoryView::ESortByHighWaterMark;
				break;
				}
			case 'm':
			case 'M':
				{
				newSortType = CMemoryView::ESortByMaxSize;
				break;
				}
			case 'u':
			case 'U':
				{
				newSortType = CMemoryView::ESortByUpdateHistory;
				break;
				}
			case 'q':
			case 'Q':
				{
				iCommand.Complete(KErrNone);
				break;
				}
			case EKeyUpArrow:
				{
				if (iMemoryView)
					{
					iMemoryView->ScrollUp();
					}
				break;
				}
			case EKeyDownArrow:
				{
				if (iMemoryView)
					{
					iMemoryView->ScrollDown();
					}
				break;
				}
			case EKeyLeftArrow:
				{
				if (iMemoryView)
					{
					iMemoryView->ScrollLeft();
					}
				break;
				}
			case EKeyRightArrow:
				{
				if (iMemoryView)
					{
					iMemoryView->ScrollRight();
					}
				break;
				}
			default:
				{
				noUpdate = ETrue;
				}
			}

		if ((newSortType != CMemoryView::ESortUnspecified) && iMemoryView)
			{
			iMemoryView->ChangeSortType(newSortType);
			}

		if (iMemoryView && !noUpdate)
			{
			iMemoryView->UpdateL();
			}

		QueueRead();
		}
	}

void CStdinReader::DoCancel()
	{
	iStdin.WaitForKeyCancel();
	}

TInt CStdinReader::RunError(TInt aError)
	{
	iCommand.Complete(aError);
	return KErrNone;
	}

CStdinReader::CStdinReader(RIoReadHandle& aStdin, CCommandBase& aCommand, CMemoryView* aMemoryView)
	: CActive(CActive::EPriorityStandard), iCommand(aCommand), iMemoryView(aMemoryView)
	{
	iStdin = aStdin;
	CActiveScheduler::Add(this);
	}

void CStdinReader::QueueRead()
	{
	iStdin.WaitForKey(iStatus);
	SetActive();
	}



class TBtraceHeader
	{
public:
	TUint8 iSize;
	TUint8 iFlags;
	TUint8 iCategory;
	TUint8 iSubCategory;
	};

class CBtraceReader : public CActive
	{
public:
	enum TMode
		{
		EConfigBtrace = 0x0001,
		EDebug        = 0x0002
		};
public:
	static CBtraceReader* NewL(TUint aMode, const TDesC& aFileName, CCommandBase& aCommand, CMemoryView* aMemoryView);
	~CBtraceReader();
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
private:
	CBtraceReader(const TDesC& aFileName, CCommandBase& aCommand, CMemoryView* aMemoryView);
	void ConstructL(TUint aMode);
	void QueueRead();
	void DecodeFrame(const TBtraceHeader& aHeader, const TDesC8& aFrame, TUint32 aTickCount);
	void Printf(TRefByValue<const TDesC> aFmt, ...);
private:
	const TDesC& iFileName;
	CCommandBase& iCommand;
	CMemoryView* iMemoryView;
	RBTrace iBtrace;
	CConsoleBase* iDebugConsole;
	RFile iFile;
	};

CBtraceReader* CBtraceReader::NewL(TUint aMode, const TDesC& aFileName, CCommandBase& aCommand, CMemoryView* aMemoryView)
	{
	CBtraceReader* self = new(ELeave) CBtraceReader(aFileName, aCommand, aMemoryView);
	CleanupStack::PushL(self);
	self->ConstructL(aMode);
	CleanupStack::Pop(self);
	return self;
	}

CBtraceReader::~CBtraceReader()
	{
	Cancel();
	iBtrace.Close();
	iFile.Close();
	delete iDebugConsole;
	}

void CBtraceReader::RunL()
	{
	QueueRead();
	TUint8* data;
	TInt size;
	while ((size = iBtrace.GetData(data)) != 0)
		{
		if (iMemoryView || iDebugConsole)
			{
			// Only decode btrace frames if we're attached to a CMemoryView object or have a debug console.
			TUint8* c = data;
			TUint8* end = c + size;
			do
				{
				TBtraceHeader* header = (TBtraceHeader*)c;
				TUint8* d = c + sizeof(TBtraceHeader);
				TUint32 tickCount = 0;
				if (header->iFlags & BTrace::EMissingRecord)
					{
					User::Leave(KErrOverflow);
					}
				if (header->iFlags & BTrace::EHeader2Present)
					{
					d += 4;
					}
				if (header->iFlags & BTrace::ETimestampPresent)
					{
					tickCount = *((TUint32*)d);
					d += 4;
					}
				if (header->iFlags & BTrace::ETimestamp2Present)
					{
					d += 4;
					}
				if (header->iFlags & BTrace::EContextIdPresent)
					{
					d += 4;
					}
				if (header->iFlags & BTrace::EPcPresent)
					{
					d += 4;
					}
				if (header->iFlags & BTrace::EExtraPresent)
					{
					d += 4;
					}
				TPtrC8 ptr(d, (c + header->iSize) - d);
				DecodeFrame(*header, ptr, tickCount);
				c += (header->iSize + 3) & ~3;
				}
				while (c < end);
			}
		if (iFileName.Length())
			{
			User::LeaveIfError(iFile.Write(TPtrC8(data, size)));
			}
		iBtrace.DataUsed();
		}
	if (iMemoryView)
		{
		iMemoryView->UpdateL();
		}
	}

void CBtraceReader::DoCancel()
	{
	iBtrace.CancelRequestData();
	}

TInt CBtraceReader::RunError(TInt aError)
	{
	if (aError == KErrOverflow)
		{
		Printf(_L("Warning: BTrace buffer overflowed, aborting..."), aError);
		iCommand.Complete(aError);
		}
	else if (aError)
		{
		Printf(_L("Warning: Could not update view (%d), aborting..."), aError);
		iCommand.Complete(aError);
		}
	return KErrNone;
	}

CBtraceReader::CBtraceReader(const TDesC& aFileName, CCommandBase& aCommand, CMemoryView* aMemoryView)
	: CActive(CActive::EPriorityStandard), iFileName(aFileName), iCommand(aCommand), iMemoryView(aMemoryView)
	{
	CActiveScheduler::Add(this);
	}

void CBtraceReader::ConstructL(TUint aMode)
	{
	if (aMode & EDebug)
		{
		iDebugConsole = Console::NewL(_L("debug"), TSize(KConsFullScreen,KConsFullScreen));
		}
	User::LeaveIfError(iBtrace.Open());
	if (aMode & EConfigBtrace)
		{
		User::LeaveIfError(iBtrace.ResizeBuffer(KBtraceBufferSize));
		// Turn everything off.
		for (TInt i = 0; i < KMaxCategories; ++i)
			{
			iBtrace.SetFilter(i, 0);
			}
		if (aMode & EDebug)
			{
			iBtrace.SetFilter(BTrace::EKernPrintf, 1);
			}
		iBtrace.SetFilter(RMemSampler::EBtraceCategory, 1);
		User::LeaveIfError(iBtrace.SetFilter2((const TUint32*)NULL, 0));
		iBtrace.SetMode(RBTrace::EEnable | RBTrace::EFreeRunning);
		}
	if (iFileName.Length())
		{
		User::LeaveIfError(iFile.Replace(iCommand.FsL(), iFileName, EFileWrite));
		}
	QueueRead();
	}

void CBtraceReader::QueueRead()
	{
	iBtrace.RequestData(iStatus, 0);
	SetActive();
	}

void CBtraceReader::DecodeFrame(const TBtraceHeader& aHeader, const TDesC8& aFrame, TUint32)
	{
	if (aHeader.iCategory == BTrace::EKernPrintf)
		{
		TUint32 threadId = *(TUint32*)aFrame.Ptr();
		TBuf<256> text;
		text.Copy(aFrame.Mid(4));
		if (iDebugConsole)
			{
			iDebugConsole->Printf(_L("Kern::Printf (0x%08x) \'%S\'\r\n"), &threadId, &text);
			}
		}
	else if (aHeader.iCategory == RMemSampler::EBtraceCategory)
		{
		switch (aHeader.iSubCategory)
			{
			case RMemSampler::ENewChunk:
				{
				TUint32 address = *(TUint32*)aFrame.Ptr();
				TInt maxSize = *((TUint32*)aFrame.Ptr() + 1);
				TFullName fullName;
				fullName.Copy(aFrame.Mid(8));
				if (iDebugConsole)
					{
					iDebugConsole->Printf(_L("New chunk - %S\r\n\taddress: 0x%08x max size: %d\r\n"), &fullName, address, maxSize);
					}
				if (iMemoryView)
					{
					iMemoryView->HandleNewChunk(fullName, address, maxSize);
					}
				break;
				}
			case RMemSampler::EChangedChunk:
				{
				TUint32 address = *(TUint32*)aFrame.Ptr();
				TUint32 size = *((TUint32*)aFrame.Ptr() + 1);
				TUint32 highWaterMark = *((TUint32*)aFrame.Ptr() + 2);
				if (iDebugConsole)
					{
					iDebugConsole->Printf(_L("Changed chunk - address: 0x%08x size: %d hwm: %d\r\n"), address, size, highWaterMark);
					}
				if (iMemoryView)
					{
					iMemoryView->HandleChangedChunk(address, size, highWaterMark);
					}
				break;
				}
			case RMemSampler::EDeletedChunk:
				{
				TUint32 address = *(TUint32*)aFrame.Ptr();
				if (iDebugConsole)
					{
					iDebugConsole->Printf(_L("Deleted chunk - address: 0x%08x\r\n"), address);
					}
				if (iMemoryView)
					{
					iMemoryView->HandleDeletedChunk(address);
					}
				break;
				}
			}
		}
	}

void CBtraceReader::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, list, &overflow);
	iCommand.Stdout().Write(buf);
	}


class CCmdMemsampler : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMemsampler();
private:
	CCmdMemsampler();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RMemSampler iMemSampler;
	CMemoryView* iMemoryView;
	CStdinReader* iStdinReader;
	CBtraceReader* iBtraceReader;
	TUint iRate;
	TBool iNoBtraceConfig;
	TBool iDebug;
	TBool iHuman;
	TFileName2 iFileName;
	TBool iNoLiveView;
	};


CCommandBase* CCmdMemsampler::NewLC()
	{
	CCmdMemsampler* self = new(ELeave) CCmdMemsampler();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMemsampler::~CCmdMemsampler()
	{
	iMemSampler.Close();
	User::FreeLogicalDevice(KLdd);
	delete iStdinReader;
	delete iBtraceReader;
	delete iMemoryView;
	}

CCmdMemsampler::CCmdMemsampler() : CCommandBase(EManualComplete), iRate(1000)
	{
	}

const TDesC& CCmdMemsampler::Name() const
	{
	_LIT(KName, "memsampler");	
	return KName;
	}

void CCmdMemsampler::DoRunL()
	{
	TUint mode = 0;
	if (!iNoBtraceConfig)
		{
		mode |= CBtraceReader::EConfigBtrace;
		}
	if (iDebug)
		{
		mode |= CBtraceReader::EDebug;
		}

	if (!iNoLiveView)
		{
		iMemoryView = CMemoryView::NewL(Stdout(), Stderr(), iHuman);
		}
	iStdinReader = CStdinReader::NewL(Stdin(), *this, iMemoryView);
	iBtraceReader = CBtraceReader::NewL(mode, iFileName, *this, iMemoryView);
	User::LeaveIfError(User::LoadLogicalDevice(KLdd));
	User::LeaveIfError(iMemSampler.Open());
	iMemSampler.Start(iRate);
	}

void CCmdMemsampler::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptSampleRate, "rate");
	aOptions.AppendUintL(iRate, KOptSampleRate);

	_LIT(KOptNoBtraceConfig, "no-btrace-config");
	aOptions.AppendBoolL(iNoBtraceConfig, KOptNoBtraceConfig);

	_LIT(KOptDebug, "debug");
	aOptions.AppendBoolL(iDebug, KOptDebug);

	_LIT(KOptHuman, "human");
	aOptions.AppendBoolL(iHuman, KOptHuman);

	_LIT(KOptFile, "file");
	aOptions.AppendFileNameL(iFileName, KOptFile);

	_LIT(KOptNoLiveView, "no-live-view");
	aOptions.AppendBoolL(iNoLiveView, KOptNoLiveView);
	}


EXE_BOILER_PLATE(CCmdMemsampler)

