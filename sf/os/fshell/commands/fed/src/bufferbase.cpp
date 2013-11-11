// bufferbase.cpp
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
#include "bufferbase.h"
#include "filebuffer.h" // for CFileBlock::CountNewLines

CFedBufferBase::CFedBufferBase()
	: CActive(EPriorityStandard), iRefCount(1)
	{
	}

TInt CFedBufferBase::RefCount() const
	{
	return iRefCount;
	}

void CFedBufferBase::IncRef()
	{
	iRefCount++;
	}

TInt CFedBufferBase::DecRef()
	{
	iRefCount--;
	TInt refCount = iRefCount;
	if (refCount == 0) delete this; 
	return refCount;
	}

void StaticDecRef(TAny* aPtr)
	{
	static_cast<CFedBufferBase*>(aPtr)->DecRef();
	}

void CFedBufferBase::PushL()
	{
	CleanupStack::PushL(TCleanupItem(&StaticDecRef, this));
	}

void CFedBufferBase::DoCancel()
	{
	ASSERT(EFalse);
	}

void CFedBufferBase::RunL()
	{
	ASSERT(EFalse);
	}

TInt CFedBufferBase::FindClientRequest(MSharedCacheClient& aClient)
	{
	TPtrC dummy;
	TRange dummyRange;
	TInt dummyLine;
	return iClientRequests.FindInUnsignedKeyOrder(TClientRequest(&aClient, dummy, dummyRange, dummyLine));
	}

TInt CFedBufferBase::RegisterClient(MSharedCacheClient& aClient, TPtrC& aDes, TRange& aRange, TInt& aRangeStartLineNumber)
	{
	TInt err = iClientRequests.InsertInUnsignedKeyOrder(TClientRequest(&aClient, aDes, aRange, aRangeStartLineNumber));
	return err;
	}

void CFedBufferBase::UnregisterClient(MSharedCacheClient& aClient)
	{
	TInt res = FindClientRequest(aClient);
	if (res >= 0) iClientRequests.Remove(res);
	}

CFedBufferBase::~CFedBufferBase()
	{
	ASSERT(iRefCount == 0); // Otherwise someone has deleted us directly rather than going through the reference counting
	iClientRequests.Reset();
	}

void CFedBufferBase::InsertTextL(TInt /*aDocumentPosition*/, const TDesC& /*aText*/)
	{
	User::Leave(KErrNotSupported);
	}

void CFedBufferBase::DeleteTextL(TRange /*aRange*/)
	{
	User::Leave(KErrNotSupported);
	}

void CFedBufferBase::SaveL(const TDesC& /*aNewName*/, TBool /*aReplace*/)
	{
	User::Leave(KErrNotSupported);
	}

TBool CFedBufferBase::Modified() const
	{
	return EFalse;
	}

TBool CFedBufferBase::Editable() const
	{
	return EFalse;
	}

TBool CFedBufferBase::IsOpen() const
	{
	return EFalse;
	}

///////

CConstDataBuffer::CConstDataBuffer(const TDesC& aTitle, const TDesC& aText)
: iTitle(aTitle), iText(aText)
	{
	}

CConstDataBuffer::CConstDataBuffer(const TDesC& aTitle, HBufC* aTextPtr)
	: iTitle(aTitle), iText(*aTextPtr), iOwnedTextPtr(aTextPtr)
	{
	}

CConstDataBuffer::~CConstDataBuffer()
	{
	delete iOwnedTextPtr;
	}

const TDesC& CConstDataBuffer::Title() const
	{
	return iTitle;
	}

TDelimiterType CConstDataBuffer::DelimiterType() const
	{
	return EDelimLF;
	}

TEncodingType CConstDataBuffer::Encoding() const
	{
	return EEncodingUtf16Native;
	}

TInt CConstDataBuffer::GetData(MSharedCacheClient& aClient, TInt aDocumentPosition)
	{
	TInt idx = FindClientRequest(aClient);
	if (idx < 0)
		{
		return idx;
		}

	TClientRequest& client = iClientRequests[idx];
	ASSERT(aDocumentPosition >= 0);

	client.iRange = TRange(aDocumentPosition, iText.Length()-aDocumentPosition);
	client.iDes.Set(iText.Mid(aDocumentPosition));
	TInt lines = KMaxTInt;
	CFileBlock::CountNewLines(iText.Left(aDocumentPosition), lines, KMaxTInt);
	client.iRangeStartLineNumber = lines;
	return KErrNone;
	}

TInt CConstDataBuffer::SeekFromOffset(MSharedCacheClient& aClient, TInt aOffset, TInt aNumLinesFromOffset, TInt aLineLength)
	{
	TInt idx = FindClientRequest(aClient);
	if (idx < 0)
		{
		return idx;
		}
	TClientRequest& client = iClientRequests[idx];

	TInt lineDelta = aNumLinesFromOffset;
	TInt bufPos = -1;
	if (lineDelta > 0)
		{
		TPtrC ptr = iText.Mid(aOffset);
		bufPos = CFileBlock::CountNewLines(ptr, lineDelta, aLineLength);
		if (bufPos == KErrNotFound)
			{
			aOffset = 0;
			ptr.Set(iText);
			lineDelta = -1;
			bufPos = CFileBlock::CountNewLines(ptr, lineDelta, aLineLength);
			}
		}
	else
		{
		lineDelta--;
		TPtrC ptr = iText.Left(aOffset);
		aOffset = 0;
		bufPos = CFileBlock::CountNewLines(ptr, lineDelta, aLineLength);
		if (bufPos == KErrNotFound) bufPos = 0;
		}
	

	client.iDes.Set(iText.Mid(bufPos+aOffset));
	client.iRange = TRange(bufPos + aOffset, client.iDes.Length());
	TInt lines = KMaxTInt;
	CFileBlock::CountNewLines(iText.Left(bufPos + aOffset), lines, KMaxTInt);
	client.iRangeStartLineNumber = lines;
	return KErrNone;
	}

TBool CConstDataBuffer::DocumentPositionIsEof(TInt aDocumentPosition) const
	{
	return (aDocumentPosition == iText.Length());
	}

TInt CConstDataBuffer::Find(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards)
	{
	ASSERT(!aBackwards);
	TInt res = iText.Mid(aStartingPosition).Find(aSearchString);
	if (res >= 0) res += aStartingPosition;
	return res;
	}
