// filebuffer.cpp
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

#include <e32base.h>
#include <e32math.h>
#include <utf.h>
#include <charconv.h>

#include "filebuffer.h"

const TInt KMinBlockSize = 2048; // Not least because an 80x24 console is about 2k
const TInt KMaxBlockSize = 28*1024; // 28 is good compromise, at least up to about 50MB files
const TInt KTargetNumBlocks = 100; // Grow block size linearly to try and keep the number of blocks at around 100

/////DEBUG
//const TInt KMaxBlockSize = 64*1024-1;
//const TInt KTargetNumBlocks = 2;
//////END DEBUG

__ASSERT_COMPILE(KMaxBlockSize <= (TInt)KMaxTUint16);

const TInt KUnicodeLineBreak = 0x2028;
//const TInt KUnicodeParagraphBreak = 0x2029; // We treat this the same as a unicode line break, oh well.
const TUint KFilePermissions = EFileShareReadersOnly | EFileRead;
void ByteSwitch(TUint16* data, TInt aLen);

CFileBuffer* CFileBuffer::NewL(RFs& aFs, CCnvCharacterSetConverter* aCharconv, const TDesC& aName, TBool aAllowNonexistantName)
	{
	CFileBuffer* fb = new (ELeave) CFileBuffer(aFs, aCharconv);
	fb->PushL();
	if (aName.Length())
		{
		fb->ConstructL(aName, aAllowNonexistantName);
		}
	else
		{
		fb->ConstructL();
		}
	CleanupStack::Pop(fb);
	return fb;
	}

CFileBuffer::~CFileBuffer()
	{
	iFile.Close();

	CFileBlock* block = iFirstBlock;
	while (block)
		{
		CFileBlock* next = block->Next();
		delete block;
		block = next;
		}
	iNarrowBuf.Close();
	}

CFileBuffer::CFileBuffer(RFs& aFs, CCnvCharacterSetConverter* aCharconv)
	: iDelimType(EDelimNotYetKnown), iEncoding(EEncodingUnknown), iFs(aFs), iCharconv(aCharconv)
	{
	CActiveScheduler::Add(this);
	}

void CFileBuffer::ConstructL(const TDesC& aName, TBool aAllowNonexistantName)
	{
	TInt err = iFile.Open(iFs, aName, KFilePermissions);
	if ((err == KErrNotFound || err == KErrPathNotFound) && aAllowNonexistantName)
		{
		// Then treat it the same as an new untitled file (except that it has a title)
		ConstructL();
		iFinalName = aName;
		return;
		}

	User::LeaveIfError(err); // view will take care of displaying appropriate message
	CommonConstructL();
	User::LeaveIfError(iFile.FullName(iFinalName));
	TInt size = 0;
	User::LeaveIfError(iFile.Size(size));
	iCacheBufSize = CalculateCacheBufSize(size);
	User::LeaveIfError(iCacheBufSize);
	iNarrowBuf.CreateL(iCacheBufSize);
	// And set up our initial file blocks, all of which start out with size iCacheBufSize (with the exception of the last one)
	CFileBlock* endBlock = NULL;
	TInt blockStart = 0;
	while (blockStart < size)
		{
		TInt blockSize = Min(iCacheBufSize, size - blockStart);
		CFileBlock* block = new(ELeave) CFileBlock(blockStart, blockSize);
		if (endBlock)
			{
			block->InsertAfterBlock(endBlock);
			}
		else
			{
			iFirstBlock = block;
			}
		endBlock = block;
		blockStart += blockSize;
		}
	if (!iFirstBlock)
		{
		// File must be empty, in which case we need to do the same as in ConstructL() and create a placeholder empty block
		iFirstBlock = new(ELeave) CFileBlock(0, 0);
		// Do I need to call this?
		User::LeaveIfError(iFirstBlock->BlockDidLoad(iEncoding, iDelimType, KNullDesC8, iCharconv));
		}
	}

void CFileBuffer::ConstructL() // Overload for no file - ie use new, untitled file
	{
	CommonConstructL();
	iCacheBufSize = KMinBlockSize;
	iNarrowBuf.CreateL(iCacheBufSize);
	iFirstBlock = new(ELeave) CFileBlock(0, 0);
	User::LeaveIfError(iFirstBlock->BlockDidLoad(iEncoding, iDelimType, KNullDesC8, iCharconv));
	}

void CFileBuffer::CommonConstructL()
	{
	// Used to be something here, not any more
	}

TInt CFileBuffer::CalculateCacheBufSize(TInt aFileSize)
	{
	TInt blockSize = aFileSize / KTargetNumBlocks;
	blockSize = blockSize & 0xFFFFFF0; // Round down to nearest 16-byte boundary
	if (blockSize < KMinBlockSize)
		{
		return KMinBlockSize;
		}
	else if (blockSize > KMaxBlockSize)
		{
		return KMaxBlockSize;
		}
	else
		{
		return blockSize;
		}
	}

/*
 This function is responsible for unloading unnecessary CFileBlocks [and calling MSharedCacheClient::InvalidateBuffer
 on any client accessing data from buffers being deleted]. The function decides who is using which blocks based on
 TClientRequest::iRequestedBlock to see what the most-recently requested block was.
 */
//For now the function is kept as simple as possible. For each client it just keeps three read-only buffers: the current buffer,
//the previous buffer, and the next buffer. It also keeps all writable buffers (containing data written by the user) because
//they have to be written to the disk before deleting.
void CFileBuffer::TidyCache()
	{
	for (CFileBlock* block = iFirstBlock; block != NULL; block = block->Next())
		{
		UnloadBlockIfPossible(block);
		}
	}

void CFileBuffer::UnloadBlockIfPossible(CFileBlock* aBlock)
	{
	if (aBlock->IsDirty() || !aBlock->IsLoaded() || aBlock == iCurrentBlock) return;
	// Check if any client is using it
	TBool inUse = EFalse;
	for (TInt i = 0; i < iClientRequests.Count(); i++)
		{
		CFileBlock* clientBlock = iClientRequests[i].iLastRequestedBlock;
		if (!clientBlock) continue;
		else if (clientBlock == aBlock || clientBlock->Prev() == aBlock || clientBlock->Next() == aBlock)
			{
			inUse = ETrue;
			break;
			}
		}
	if (!inUse)
		{
		aBlock->Unload();
		}
	}

CFileBlock* CFileBuffer::FindBlockForDocumentPosition(TInt aDocPosition, TInt* aOffset, TInt* aLineCountToDocPos)
	{
	ASSERT(aDocPosition >= 0);

	TInt blockStart = 0;
	TInt lineCount = 0;
	CFileBlock* block = iFirstBlock;
	while (block)
		{
		if (!block->HasBeenLoaded())
			{
			// Need the block to have been loaded to know the char count (and the line count)
			TInt err = LoadBlock(block);
			if (err) return NULL;
			}

		TInt charCount = block->CharacterCount();

		if (aDocPosition < blockStart + charCount || block->Next() == NULL && aDocPosition == blockStart + charCount)
			{
			// Found it (note that the last block is returned if the document position DocumentSize() is requested)
			TInt offset = aDocPosition - blockStart;
			if (aOffset) *aOffset = offset;
			if (aLineCountToDocPos)
				{
				TInt lineCountToOffset = 0;
				if (offset != 0)
					{
					// We have to load the block if the caller requested line count info and offset is non-zero
					TInt err = LoadBlock(block);
					if (err) return NULL;
					lineCountToOffset = block->CountNewLinesUpToOffset(offset);
					}
				*aLineCountToDocPos = lineCount + lineCountToOffset;
				}
			return block;
			}
		else
			{
			UnloadBlockIfPossible(block);
			}
		blockStart += charCount;
		lineCount += block->NewlineCount();
		block = block->Next();
		}
	//ASSERT(EFalse); // If we get here then we don't have a block for this offset - maybe offset was >= file length?
	return NULL;
	}

TInt CFileBuffer::LoadBlock(CFileBlock* aBlock)
	{
	TInt err = KErrNone;
	if (aBlock->IsLoaded())
		{
		return KErrNone;
		}

	if (!iFile.SubSessionHandle()) return KErrNotReady;

	// Then load it from disk
	iNarrowBuf.Zero();
	TInt blockSize = aBlock->BlockSize();
	if (blockSize > iNarrowBuf.MaxSize())
		{
		err = iNarrowBuf.ReAlloc(blockSize);
		}

	if (!err)
		{
		err = iFile.Read(aBlock->FilePosition(), iNarrowBuf, blockSize);
		if (err)
			{
			aBlock->Unload();
			}
		}

	if (err == KErrNone)
		{
		TEncodingType blockEncoding = iEncoding;
		TDelimiterType blockDelim = iDelimType;
		err = aBlock->BlockDidLoad(blockEncoding, blockDelim, iNarrowBuf, iCharconv);
		if (err)
			{
			// Don't do anything with encodings
			return err;
			}

		if (iEncoding == EEncodingUtf8 && blockEncoding == EEncodingNarrow)
			{
			// Conceivably previous blocks could have been narrow that was also UTF-8 conformant but this block makes it clear the file definitely isn't UTF-8 - therefore allow the change of encoding
			iEncoding = EEncodingNarrow;
			}
		else if (iEncoding == EEncodingUnknown)
			{
			iEncoding = blockEncoding;
			}
		else if (blockEncoding != iEncoding)
			{
			__DEBUGGER();
			err = KErrCorrupt;
			}

		if (iDelimType == EDelimNotYetKnown && blockDelim)
			{
			iDelimType = blockDelim;
			}
		// Inconsistant line ending info is ignored here, as most of it has already been thrown away by CountNewLines
		}

	return err;
	}

TInt CFileBuffer::GetData(MSharedCacheClient& aClient, TInt aDocumentPosition)
	{
	TInt idx = FindClientRequest(aClient);
	if (idx < 0)
		{
		return idx;
		}

	TClientRequest& client = iClientRequests[idx];
	ASSERT(aDocumentPosition >= 0);

	TInt offset;
	TInt lineCount;
	CFileBlock* block = FindBlockForDocumentPosition(aDocumentPosition, &offset, &lineCount);
	client.iRange = TRange(aDocumentPosition, 0); // Length will be filled in once block is loaded (as could change due to TransferDataFromPreviousBlock)
	client.iRangeStartLineNumber = lineCount;

	TInt err = LoadBlock(block);
	if (err) return err;

	const TDesC& text = block->Text();
	client.iDes.Set(text.Mid(offset));
	client.iRange.iLength = text.Length() - offset;
	client.iLastRequestedBlock = block;
	TidyCache();
	return KErrNone;
	}

TInt CFileBuffer::SeekFromOffset(MSharedCacheClient& aClient, TInt aDocumentPosition, TInt aNumLinesFromOffset, TInt aLineLength)
	{
	TInt idx = FindClientRequest(aClient);
	if (idx < 0)
		{
		return idx;
		}

	TClientRequest& client = iClientRequests[idx];
	//ASSERT(client.iClientStatus == NULL); // Client can't make more than 1 request at once
	ASSERT(aDocumentPosition >= 0);
	//ASSERT(aDocumentPosition < DocumentSize());
	ASSERT(aNumLinesFromOffset != 0);

	TInt offset;
	CFileBlock* block = FindBlockForDocumentPosition(aDocumentPosition, &offset);

	TInt& lineDelta = aNumLinesFromOffset;
	TInt& maxLineLength = aLineLength;

	TInt err = KErrNone;
	while (ETrue)
		{
		ASSERT(block);
		err = LoadBlock(block);
		if (err) return err;

		TInt lines = lineDelta;
		TInt bufPos;
		if (lines > 0)
			{
			bufPos = CFileBlock::CountNewLines(block->Text().Mid(offset), lines, maxLineLength);
			if (bufPos == KErrNotFound)
				{
				CFileBlock* nextBlock = block->Next();
				if (!nextBlock)
					{
					// If no more blocks, change the request to be "find the first newline searching backwards from the end of the last block" (phrasing it like this means it'll work even if there aren't any newlines in this block at all)
					lineDelta = -1;
					offset = block->CharacterCount();
					continue;
					}
				offset = 0;
				lineDelta -= lines;
				UnloadBlockIfPossible(block);
				block = nextBlock;
				continue;
				}
			else
				{
				bufPos += offset; // Since the bufPos returned was relative to the Mid we did
				}
			}
		else
			{
			lineDelta--; // Client will want the line *before* the first new line we encounter (the first new line we encounter will be for the line they're actually on)
			lines = lineDelta;
			bufPos = CFileBlock::CountNewLines(block->Text().Left(offset), lines, maxLineLength);
			if (bufPos == KErrNotFound)
				{
				CFileBlock* prevBlock = block->Prev();
				if (!prevBlock)
					{
					// If no more blocks, then the start of the first block is closest
					offset = 0;
					break;
					}
				UnloadBlockIfPossible(block);
				block = prevBlock;
				offset = block->CharacterCount();
				lineDelta += lines; // since lineDelta is negative
				continue;
				}
			}
		ASSERT(bufPos != KErrNotFound); // Must be, to have got here
		offset = bufPos;
		break;
		}

	client.iDes.Set(block->Text().Mid(offset));
	TInt docPos;
	TInt lineCount;
	block->CalculateBlockPositions(docPos, lineCount);
	client.iRange = TRange(docPos + offset, block->CharacterCount() - offset);
	client.iLastRequestedBlock = block;
	ASSERT(client.iRange.iLength == client.iDes.Length());
	client.iRangeStartLineNumber = lineCount + block->CountNewLinesUpToOffset(offset);
	TidyCache();
	return KErrNone;
	}

TBool CFileBuffer::DocumentPositionIsEof(TInt aDocumentPosition) const
	{
	// Like FindBlockForDocumentPosition but doesn't try and load blocks
	TInt blockStart = 0;
	for (CFileBlock* block = iFirstBlock; block != NULL; block = block->Next())
		{
		if (!block->HasBeenLoaded())
			{
			return EFalse;
			}
		TInt charCount = block->CharacterCount();
		TInt endOfBlock = blockStart + charCount;
		if (aDocumentPosition < endOfBlock) return EFalse; // We have more characters than this doc pos so it can't be the end
		else if (charCount == 0 && block->Next() != NULL)
			{
			continue; // we're an empty block followed by another block, so defer the decision to the next block
			}
		else if (aDocumentPosition == endOfBlock)
			{
			// Then this position is the eof if there aren't any more blocks
			return (block->Next() == NULL);
			}
		blockStart += charCount;
		}
	ASSERT(EFalse);
	return EFalse;
	}

void CFileBuffer::InsertTextL(TInt aDocumentPosition, const TDesC& aText)
	{
	TInt offset;
	CFileBlock* block = FindBlockForDocumentPosition(aDocumentPosition, &offset);

	TInt err = LoadBlock(block);
	User::LeaveIfError(err);

	block->InsertTextL(offset, aText);
	iUnsaved = ETrue;
	}

void CFileBuffer::DeleteTextL(TRange aRange)
	{
	while (aRange.iLength > 0)
		{
		TInt offset;
		CFileBlock* block = FindBlockForDocumentPosition(aRange.iLocation, &offset);
		if (!block) return; // We will tolerate it if the range is beyond the document position
		TInt err = LoadBlock(block);
		User::LeaveIfError(err);

		TInt blockCharCount = block->CharacterCount();
		TInt blockLen = Min(blockCharCount - offset, aRange.iLength);
		if (blockLen) // Again, tolerating if the range is completely outside the document length
			{
			block->DeleteText(offset, blockLen);
			// The range location stays the same, since everything will have shuffled up
			aRange.iLength -= blockLen;
			iUnsaved = ETrue;
			}
		else
			{
			break;
			}
		}	
	}

void CFileBuffer::DeleteTempFile(TAny* aSelf)
	{
	CFileBuffer* self = static_cast<CFileBuffer*>(aSelf);
	if (self->iTempName->Length())
		{
		self->iFs.Delete(*self->iTempName);
		}
	delete self->iTempName;
	self->iTempName = NULL;
	}

void CFileBuffer::SetFinalNameToTemporary(TAny* aSelf)
	{
	CFileBuffer* self = static_cast<CFileBuffer*>(aSelf);
	self->iFinalName = *self->iTempName;
	}

void CFileBuffer::SaveL(const TDesC& aName, TBool aReplace)
	{
	// Try opening aName as given - if it suceeds use it, if it fails with alreadyExists use a temp file and rename at the end
	RBuf origRenamed;
	CleanupClosePushL(origRenamed);
	iTempName = new(ELeave) TFileName;
	origRenamed.CreateL(KMaxFileName);

	RFile outFile;
	CleanupStack::PushL(TCleanupItem(&DeleteTempFile, this));
	CleanupClosePushL(outFile);
	TBool needToRename = EFalse;

	TInt err = iFs.MkDirAll(aName); // Avoid path not found errors in the case where the file has not yet been saved
	if (err == KErrAlreadyExists) err = KErrNone;
	User::LeaveIfError(err);

	const TUint fileMode = EFileWrite | EFileShareExclusive | EFileStream;
	err = outFile.Create(iFs, aName, fileMode);
	if ((err == KErrAlreadyExists || err == KErrInUse) && aReplace)
		{
		needToRename = ETrue;
		TParsePtrC parse(aName);
		TPtrC path = parse.DriveAndPath();
		err = outFile.Temp(iFs, path, *iTempName, fileMode);
		}
	User::LeaveIfError(err);

	if (iDelimType == EDelimNotYetKnown) iDelimType = EDelimCRLF; // It's conceivable that we still might not have a newline type - default to Symbian default, CRLF.
	if (iEncoding == EEncodingUnknown) iEncoding = EEncodingUtf8;

	CFileBlock* block = iFirstBlock;
	while (block)
		{
		err = LoadBlock(block);
		User::LeaveIfError(err);
		err = block->ConvertToEightBit(iEncoding, iDelimType, iNarrowBuf);
		User::LeaveIfError(err); // Conversion could fail, or oom trying to realloc iNarrowBuf
		err = outFile.Write(iNarrowBuf);
		User::LeaveIfError(err); // File writing failed
		UnloadBlockIfPossible(block);
		block = block->Next();
		}
	CleanupStack::PopAndDestroy(&outFile); // closes outFile
	TBool fileWasOpen = IsOpen();
	if (fileWasOpen)
		{
		// If the file wasn't open, there's no need for us to preserve the temporary (because the whole file must be by definition in memory)
		CleanupStack::Pop(); // Remove DeleteTempFile - the temp file is important once we start tinkering with the original
		CleanupStack::PushL(iTempName); // But the descriptor isn't
		iUnsaved = EFalse; // We've succeeded in saving it *somewhere*
		}
	// From here on in, we bail on any failure that leaves the original file moved and/or closed. We do not try to recover from any of these, instead just bail and let the user tidy up the temp files

	if (needToRename)
		{
		CleanupStack::PushL(TCleanupItem(&SetFinalNameToTemporary, this)); // If we bail out and leave, this indicates where we managed to save the file to
		iFile.Close(); // Is necessary if we're trying to overwrite the original
		TBool done = EFalse;
		TInt nonce = 1;

		// First, rename the original out of the way
		while (!done)
			{
			_LIT(KFmt, "%S.orig%d");
			origRenamed.Format(KFmt, &aName, nonce);
			nonce++;
			err = iFs.Rename(aName, origRenamed);
			if (err != KErrAlreadyExists) done = ETrue;
			}
		User::LeaveIfError(err);

		// If we reach here, rename has succeeded - now rename temp file to orig
		err = iFs.Rename(*iTempName, aName);
		User::LeaveIfError(err);

		// Now just need to bin the original file
		iFs.Delete(origRenamed); // Don't care if this succeeds or not

		// Finally, reopen the file
		err = iFile.Open(iFs, aName, KFilePermissions);
		User::LeaveIfError(err);
		// If this all succeeded, we're sorted
		CleanupStack::Pop(); // SetFinalNameToTemporary
		}
	else
		{
		// If we didn't use a temporary, we just have to reopen the file
		iFile.Close(); // Don't imagine it could be open here...
		err = iFile.Open(iFs, aName, KFilePermissions);
		User::LeaveIfError(err);
		}

	CleanupStack::PopAndDestroy(2, &origRenamed); // iTempName OR DeleteTempFile, origNamed
	iTempName = NULL;
	iUnsaved = EFalse;
	iFile.FullName(iFinalName); // We don't just do "iFinalName = aName", in case the caller managed to sneak a relative path past the file system
	// Once everything has completed ok, go through and set every block as not dirty. This also updates block file positions
	iFirstBlock->FileHasBeenSaved();
	}

TBool CFileBuffer::Modified() const
	{
	return iUnsaved;
	}

TDelimiterType CFileBuffer::DelimiterType() const
	{
	return iDelimType;
	}

TEncodingType CFileBuffer::Encoding() const
	{
	return iEncoding;
	}

const TDesC& CFileBuffer::Title() const
	{
	return iFinalName;
	}

TBool CFileBuffer::Editable() const
	{
	return ETrue;
	}

TBool CFileBuffer::IsOpen() const
	{
	return iFile.SubSessionHandle() != 0;
	}

TInt CFileBuffer::Find(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards)
	{
	TInt res = 0;
	TRAPD(err, res = DoFindL(aStartingPosition, aSearchString, aBackwards));
	if (err) res = err;
	return res;
	}

#define EnsureCapacity(buf, len) if (buf.MaxLength() < len) { TInt err = buf.ReAlloc(Max(len, buf.Length()*2)); if (err) return err; }
#define EnsureCapacityL(buf, len) if (buf.MaxLength() < len) { buf.ReAllocL(Max(len, buf.Length()*2)); }
#define EnsureHBufCapacity(hbuf, len) if (hbuf->Des().MaxLength() < len) { HBufC* newBuf = hbuf->ReAllocL(Max(len, hbuf->Length()*2)); if (newBuf) { hbuf = newBuf; } else { return KErrNoMemory; } }
#define EnsureHBufCapacityL(hbuf, len) if (hbuf->Des().MaxLength() < len) { hbuf = hbuf->ReAllocL(Max(len, hbuf->Length()*2)); }

TInt CFileBuffer::DoFindL(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards)
	{
	ASSERT(!aBackwards); // TODO not supported yet
	// We use a separate buffer because searching directly through the block's text wouldn't handle the case
	// where the search string is split over 2 blocks. We use a rolling buffer containing the current buffer and
	// the last one, and seach in that, so 
	RBuf searchBuf;
	CleanupClosePushL(searchBuf);
	const TInt searchLen = aSearchString.Length();
	searchBuf.CreateL(iCacheBufSize + searchLen); // A match could only span buffers by a max of searchLen
	
	TInt offset;
	CFileBlock* block = FindBlockForDocumentPosition(aStartingPosition, &offset);
	TInt bufStartPos = aStartingPosition;
	while (block)
		{
		// Block must be loaded to search through its data
		TInt err = LoadBlock(block);
		User::LeaveIfError(err);
		// Get block data into the search buf, leaving up to searchLen's worth of the previous block, so we can handle the search term spanning 2 buffers
		TPtrC blockData = block->Text().Mid(offset);
		TInt newBlockLen = blockData.Length();

		EnsureCapacityL(searchBuf, newBlockLen + searchLen);
		TPtrC toKeep = searchBuf.Right(searchLen);
		TInt amountToDeleteFromBuf = searchBuf.Length() - toKeep.Length();
		searchBuf.Delete(0, amountToDeleteFromBuf);
		bufStartPos += amountToDeleteFromBuf;
		searchBuf.Append(blockData);

		TInt found = searchBuf.Find(aSearchString);
		if (found != KErrNotFound)
			{
			TInt result = bufStartPos + found;
			CleanupStack::PopAndDestroy(&searchBuf);
			return result;
			}
		
		UnloadBlockIfPossible(block);
		block = block->Next();
		offset = 0; // Non-zero offset only relevant for the first block searced, the one returned by FindBlockForDocumentPosition
		}
	User::Leave(KErrNotFound);
	return 0;
	}

//////

CFileBlock::CFileBlock(TInt aFilePosition, TInt aBlockSize)
	: iFilePosition(aFilePosition), iBlockSize(aBlockSize)
	{
	if (aBlockSize == 0)
		{
		ASSERT(aFilePosition == 0); // A zero blocksize is only valid if we're the first (and only) block of an empty, untitled, unsaved file
		iFlags.Set(EHasBeenLoaded); // Ie, it doesn't need loading
		}
	}

TInt CFileBlock::BlockSize() const
	{
	ASSERT(!iFlags.IsSet(EDirty)); // We can't know the blocksize for a dirty block (it's not known until the block is saved, ie converted back to 8-bit data)
	return iBlockSize;
	}

TBool CFileBlock::IsLoaded() const
	{
	return iData != NULL;
	}

TBool CFileBlock::HasBeenLoaded() const
	{
	return iFlags.IsSet(EHasBeenLoaded);
	}

void CFileBlock::CalculateBlockPositions(TInt& aCharacterPos, TInt& aLineCount) const
	{
	// Have to figure out our document position from the previous blocks. We don't cache this information because that would make inserting text into the file much harder
	aCharacterPos = 0;
	aLineCount = 0;
	
	CFileBlock* prev = iPrev;
	while (prev)
		{
		aCharacterPos += prev->CharacterCount();
		aLineCount += prev->NewlineCount();
		prev = prev->iPrev;
		}
	}

TInt CFileBlock::FilePosition() const
	{
	return iFilePosition;
	}

CFileBlock::~CFileBlock()
	{
	delete iData;
	}

TBool CFileBlock::IsDirty() const
	{
	return iFlags.IsSet(EDirty);
	}

void CFileBlock::Unload()
	{
	ASSERT(!IsDirty());
	ASSERT(iData->Length() <= (TInt)KMaxTUint16); // Otherwise we can't safely update our character count
	iNumCharacters = iData->Length();
	delete iData;
	iData = NULL;
	}

void CFileBlock::InsertAfterBlock(CFileBlock* aBlock)
	{
	ASSERT(iPrev == NULL);
	ASSERT(iNext == NULL);
	CFileBlock* next = aBlock->iNext;
	aBlock->iNext = this;
	iPrev = aBlock;
	iNext = next;
	if (next) next->iPrev = this;
	}

const TDesC16& CFileBlock::Text() const
	{
	ASSERT(IsLoaded()); // Otherwise shouldn't be calling this function
	return *iData;
	}

CFileBlock* CFileBlock::Next() const
	{
	return iNext;
	}

CFileBlock* CFileBlock::Prev() const
	{
	return iPrev;
	}

TInt CFileBlock::BlockDidLoad(TEncodingType& aEncoding, TDelimiterType& aDelimType, const TDesC8& aData, CCnvCharacterSetConverter* aCharconvForUtf8Conversion)
	{
	ASSERT(!IsLoaded());
	iData = HBufC::New(HasBeenLoaded() ? iNumCharacters : iBlockSize);
	if (!iData) return KErrNoMemory;
	
	// For the duration of this function, iData is now defined to be this->iData->Des(). This saves me having to rewrite the rest of the function that is expecting iData to be an RBuf
	TPtr iData = this->iData->Des();

	iFlags.Set(EHasBeenLoaded);

	TEncodingType& blockEncoding = aEncoding;

	TBool bom = EFalse;
	if (FilePosition() == 0)
		{
		// First block - check for BOM
		TPtrC8 bomData = aData.Left(2);
		if (bomData == KLittleEndianBom)
			{
			blockEncoding = EEncodingUtf16LE;
			bom = ETrue;
			}
		else if (bomData == KBigEndianBom)
			{
			blockEncoding = EEncodingUtf16BE;
			bom = ETrue;
			}
		else if (aData.Left(3) == KUtf8Bom)
			{
			blockEncoding = EEncodingUtf8;
			bom = ETrue;
			}
		}
	
	switch (blockEncoding)
		{
		case EEncodingUtf16Native:
			{
			TPtrC16 wdata = TPtrC16((TUint16*)aData.Ptr(), aData.Size()/2);
			if (bom) wdata.Set(wdata.Mid(1));
			iData.Copy(wdata);
			break;
			}
		case EEncodingUtf16Switched:
			{
			// There's a way to do this using charconv, but it's more effort to look it up than to byte-switch it myself
			TPtrC16 wdata = TPtrC16((TUint16*)aData.Ptr(), aData.Size()/2);
			if (bom) wdata.Set(wdata.Mid(1));
			iData.Copy(wdata);
			ByteSwitch((TUint16*)iData.Ptr(), iData.Length());
			break;
			}
		case EEncodingNarrow:
			// No attempt at conversion in this case
			iData.Copy(aData);
			break;
		case EEncodingUtf8:
		case EEncodingUnknown:
			{
			TPtrC8 data(aData);
			if (bom) data.Set(data.Mid(3));
			//TInt bytesLeft = CnvUtfConverter::ConvertToUnicodeFromUtf8(iData, data); <--- DOESN'T WORK!
			TInt state = CCnvCharacterSetConverter::KStateDefault;
			TInt numUnconverted = 0, firstUnconverted = -1;
			TInt bytesLeft = -1;
			if (aCharconvForUtf8Conversion)
				{
				// Can be null if charconv is busted
				bytesLeft = aCharconvForUtf8Conversion->ConvertToUnicode(iData, data, state, numUnconverted, firstUnconverted);
				ASSERT(bytesLeft <= 0); // Otherwise we didn't make our buffer big enough
				// bytesLeft less than zero means an invalid sequence at the start of the buffer. Since our block size will never be so small there's no risk it's the start of a truncated sequence
				}

			if (bytesLeft < 0)
				{
				// It's just not UTF-8
				iData.Copy(aData);
				blockEncoding = EEncodingNarrow;
				}
			else if (firstUnconverted >= 0)
				{
				// Got some bad data
				if (iNext && data.Length() - firstUnconverted < 4)
					{
					// Possibly a UTF-8 sequence spread over a block boundary
					blockEncoding = EEncodingUtf8;
					TInt err = iNext->TransferDataFromPreviousBlock(this, data.Mid(firstUnconverted));
					if (!err)
						{
						iBlockSize = firstUnconverted;
						}
					iData.SetLength(iData.Locate(0xFFFD)); // No better way of figuring out where charconv barfed than to scan for the first instance of the substitution character
					}
				else
					{
					// It's just not UTF-8
					iData.Copy(aData);
					blockEncoding = EEncodingNarrow;
					}
				}
			else
				{
				// it is conformant UTF-8, go with that for now. A future block could downgrade to narrow, if so no harm done (unless we had characters that appeared valid UTF-8 even though they weren't)
				blockEncoding = EEncodingUtf8;
				}
			break;
			}
		default:
			ASSERT(EFalse);
		}

	iNumCharacters = iData.Length();
	if (!iFlags.IsSet(EHaveCalculatedLineEndings))
		{
		// Need to calculate num line endings
		TInt numLineEndings = KMaxTInt;
		CountNewLines(iData, numLineEndings, KMaxTInt, &aDelimType);
		ASSERT(numLineEndings <= (TInt)KMaxTUint16); // Otherwise I've got some overflow-related badness
		iNumLineEndings = numLineEndings;
		iFlags.Set(EHaveCalculatedLineEndings);
		}

	if (iData.Length() && iData[iData.Length()-1] == '\r')
		{
		// uh oh, possible new line split over a block. To simplify matters we'll ensure this never happens, by moving the newline to the next block
		TInt charLen = (blockEncoding == EEncodingUtf16LE || blockEncoding == EEncodingUtf16BE) ? 2 : 1;
		if (iNext)
			{
			TInt err = iNext->TransferDataFromPreviousBlock(this, aData.Right(charLen));
			if (err)
				{
				// The hell with it, we tried... (line counts will not be precise but otherwise shouldn't be the end of the world
				}
			else
				{
				iBlockSize -= charLen;
				iNumCharacters--;
				iNumLineEndings--;
				iData.SetLength(iNumCharacters);
				}
			}
		}

	return KErrNone;
	}

TInt CFileBlock::CountNewLines(const TDesC& aDes, TInt& aTarget, TInt aSoftWrapLineLength, TDelimiterType* aDelimType)
	{
	ASSERT(aTarget != 0);
	ASSERT(aSoftWrapLineLength > 0);
	TBool countUp = aTarget > 0;
	if (!countUp) aTarget = -aTarget;
	TInt numFound = 0;
	TInt lineLen = 0;
	TBool foundDelim = EFalse;
	if (aDelimType) *aDelimType = EDelimNotYetKnown;

	for (TInt i = 0; i < aDes.Length(); i++)
		{
		TInt idx = (countUp ? i : aDes.Length()-1 - i);
		TUint16 ch = aDes[idx];
		TDelimiterType delim = EDelimNotYetKnown;

		if (ch == '\r')
			{
			numFound++;
			lineLen = 0;
			if (countUp && i+1 < aDes.Length() && aDes[i+1] == '\n')
				{
				delim = EDelimCRLF;
				i++; // Skip over the LF of a CRLF pair
				idx++; // Idx needs to point to the END of the line ending
				}
			else
				{
				delim = EDelimCR;
				}
			}
		else if (ch == '\n')
			{
			numFound++;
			lineLen = 0;
			if (!countUp && idx > 0 && aDes[idx-1] == '\r')
				{
				delim = EDelimCRLF;
				i++; // Skip over the CR of a CRLF pair
				// Don't modify idx, it is already pointing at the end character of the line ending
				}
			else
				{
				delim = EDelimLF;
				}
			}
		else if (ch == KUnicodeLineBreak /*|| ch == KUnicodeParagraphBreak*/)
			{
			numFound++;
			lineLen = 0;
			delim = EDelimUnicode;
			}
		else
			{
			lineLen++;
			}

		if (lineLen == aSoftWrapLineLength)
			{
			//TODO fix this so that soft wraps are put in the correct place when iterating backwards
			numFound++;
			lineLen = 0;
			}

		if (!foundDelim && delim && aDelimType)
			{
			foundDelim = ETrue;
			*aDelimType = delim;
			}
		if (numFound == aTarget)
			{
			return idx+1; // We return the index of the first character after the newline
			}
		}
	aTarget = numFound;
	return KErrNotFound;
	}

TInt CFileBlock::TransferDataFromPreviousBlock(CFileBlock* aPrev, const TDesC8& aData)
	{
	ASSERT(aPrev == iPrev);
	ASSERT(!IsLoaded()); // We should only ever be doing a transfer the first time through the file. It's too complicated to update everything (char count, etc) if the block is already loaded

	iBlockSize += aData.Length();
	iFilePosition -= aData.Length();
	return KErrNone;
	}

TInt CFileBlock::NewlineCount() const
	{
	ASSERT(HasBeenLoaded());
	if (!iFlags.IsSet(EHaveCalculatedLineEndings))
		{
		// We don't know the line count
		ASSERT(IsLoaded());
		TInt numLineEndings = KMaxTInt;
		CountNewLines(*iData, numLineEndings, KMaxTInt);
		ASSERT(numLineEndings <= (TInt)KMaxTUint16); // Otherwise I've got some overflow-related badness
		iNumLineEndings = numLineEndings;
		iFlags.Set(EHaveCalculatedLineEndings);
		}

	return iNumLineEndings;
	}

TInt CFileBlock::CountNewLinesUpToOffset(TInt aOffset) const
	{
	ASSERT(IsLoaded());
	// Don't try and be smart (yet)
	TInt res = KMaxTInt;
	CountNewLines(iData->Left(aOffset), res, KMaxTInt);
	return res;
	}

void CFileBlock::InsertTextL(TInt aBlockOffset, const TDesC16& aText)
	{
	ASSERT(IsLoaded());
	ASSERT(aBlockOffset <= iData->Length());
	EnsureHBufCapacityL(iData, iData->Length() + aText.Length());
	TPtr data = iData->Des();

	// Ugh check we're not splitting a CRLF
	if (aBlockOffset > 0 && aBlockOffset < data.Length() && data[aBlockOffset-1] == '\r' && data[aBlockOffset] == '\n')
		{
		aBlockOffset++;
		}

	data.Insert(aBlockOffset, aText);
	iFlags.Clear(EHaveCalculatedLineEndings); // Don't recalculate until asked for
	// iBlockSize is not updated until we save the file (as we don't know how much space it will take up until then)
	// iNumCharacters is not updated because it's not needed while the block is loaded (plus, in case our data has exceeded KMaxTUint16)
	iFlags.Set(EDirty);
	}

void CFileBlock::DeleteText(TInt aPos, TInt aLen)
	{
	ASSERT(IsLoaded());
	TPtr data = iData->Des();
	ASSERT(aPos >= 0);
	ASSERT(aLen > 0);
	ASSERT(aPos < data.Length());
	ASSERT(aPos + aLen <= data.Length());

	// Ugh check neither end of the range splits a CRLF
	if (data[aPos] == '\n' && aPos > 0 && data[aPos-1] == '\r') { aPos--; aLen++; }
	if (data[aPos+aLen-1] == '\r' && aPos + aLen < data.Length() && data[aPos+aLen] == '\n') aLen++; // Be greedy in both directions? Is this right?

	data.Delete(aPos, aLen);
	iFlags.Clear(EHaveCalculatedLineEndings); // Don't recalculate until asked for
	iFlags.Set(EDirty);
	}

TInt CFileBlock::CharacterCount() const
	{
	ASSERT(HasBeenLoaded());
	if (IsLoaded()) return Text().Length();
	else return iNumCharacters;
	}

TInt CFileBlock::ReplaceAll(const TDesC& aFrom, const TDesC& aTo)
	{
	// Replace all occurences of aFrom with aTo, in iData
	TInt pos = 0;
	TInt lenDelta = -aFrom.Length() + aTo.Length();
	while (ETrue)
		{
		TPtrC des(iData->Mid(pos));
		TInt found = des.Find(aFrom);
		if (found == KErrNotFound) break;
		TInt idx = pos + found;
		EnsureHBufCapacity(iData, iData->Length() + lenDelta);
		iData->Des().Replace(idx, aFrom.Length(), aTo);
		pos = idx + aTo.Length();
		}
	return KErrNone;
	}

TInt CFileBlock::ConvertToEightBit(TEncodingType aEncoding, TDelimiterType aDelimeter, RBuf8& aResultBuf)
	{
	ASSERT(IsLoaded());
	aResultBuf.Zero();
	// First, make sure the line endings are consistant
	
	_LIT(KCr, "\r");
	_LIT(KCrLf, "\r\n");
	_LIT(KLf, "\n");
	_LIT(KUniNl, "\x2028");
	//_LIT(KUniPara, "\x2029");
	// This could benefit from some optimisation, it is at least easy to understand
	TInt err = KErrNone;
	switch(aDelimeter)
		{
		case EDelimCR:
			ReplaceAll(KCrLf, KCr);
			ReplaceAll(KLf, KCr);
			ReplaceAll(KUniNl, KCr);
			break;
		case EDelimLF:
			ReplaceAll(KCrLf, KLf);
			ReplaceAll(KCr, KLf);
			ReplaceAll(KUniNl, KLf);
			break;
		case EDelimUnicode:
			ReplaceAll(KCrLf, KUniNl);
			ReplaceAll(KCr, KUniNl);
			ReplaceAll(KLf, KUniNl);
			break;
		case EDelimCRLF:
			// Easiest to replace everything (including CRLFs) with a single-character sequence, then replace that to CRLF at the end. This avoids having to figure out if a CR is part of a CRLF, etc
			ReplaceAll(KCrLf, KUniNl);
			ReplaceAll(KCr, KUniNl);
			ReplaceAll(KLf, KUniNl);
			err = ReplaceAll(KUniNl, KCrLf); // This one could actually need an alloc as the string could be bigger
			if (err) return err;
			break;
		default:
			__DEBUGGER();
			break;
		}

	switch (aEncoding)
		{
		case EEncodingUtf16Native:
		case EEncodingUtf16Switched:
			{
			TBool needBom = (FilePosition() == 0);
			TPtrC8 narrowData = TPtrC8((TUint8*)iData->Ptr(), iData->Size());
			TInt requiredLen = narrowData.Length();
			if (needBom) requiredLen += 2;
			EnsureCapacity(aResultBuf, requiredLen);
			if (needBom) aResultBuf.Append(KNativeBom);
			aResultBuf.Append(narrowData);
			if (aEncoding == EEncodingUtf16Switched)
				{
				ByteSwitch((TUint16*)aResultBuf.Ptr(), requiredLen);
				}
			break;
			}
		case EEncodingNarrow:
			// No attempt at conversion in this case
			EnsureCapacity(aResultBuf, iData->Length());
			aResultBuf.Copy(*iData);
			break;
		case EEncodingUtf8:
			{
			EnsureCapacity(aResultBuf, iData->Length() * 4); // Worst case for UTF-8 is 4 bytes per character
			TInt result = CnvUtfConverter::ConvertFromUnicodeToUtf8(aResultBuf, *iData);
			if (result > 0) return KErrCorrupt;
			if (result < 0) return result;
			break;
			}
		default:
			ASSERT(EFalse);
		}

	if (iData->Length() > (TInt)KMaxTUint16 || aResultBuf.Length() > (TInt)KMaxTUint16)
		{
		// Oh dear we've blown our (self-imposed) limit on how big iNumCharacters and iBlockSize can get
		// Split the block and try again
		TInt err = SplitBufferAt(iData->Length()/2);
		if (err) return err;
		else return ConvertToEightBit(aEncoding, aDelimeter, aResultBuf);
		}
	iBlockSize = aResultBuf.Length(); // Now we know how big the block will be when it gets written back to disk
	return KErrNone;
	}

void CFileBlock::FileHasBeenSaved()
	{
	ASSERT(iPrev == NULL); // Should only call on the first block
	TInt filePos = 0;
	CFileBlock* block = this;
	while (block)
		{
		block->iFilePosition = filePos;
		block->iFlags.Clear(EDirty);
		filePos = filePos + block->BlockSize(); // File positions need updating in light of how block sizes may have changed (the blocksize is set to the correct value at the end of ConvertToEightBit)
		block = block->Next();
		}
	}

TInt CFileBlock::SplitBufferAt(TInt aCharacterPosition)
	{
	ASSERT(IsLoaded());
	ASSERT(iData);

	if (aCharacterPosition > iData->Length()) aCharacterPosition = iData->Length();
	// Ugh check we're not splitting a CRLF
	if (aCharacterPosition > 0 && (*iData)[aCharacterPosition-1] == '\r' && (*iData)[aCharacterPosition] == '\n')
		{
		aCharacterPosition++;
		}

	CFileBlock* newBlock = new CFileBlock(-1, 1); // Neither file position nor blocksize is important for a block that is dirty, and this block will be created dirty
	if (!newBlock) return KErrNoMemory;
	newBlock->iFlags.Set(EDirty);
	newBlock->iFlags.Set(EHasBeenLoaded);

	TPtrC dataToMove = iData->Mid(aCharacterPosition);
	newBlock->iData = HBufC::New(dataToMove.Length());
	if (!newBlock->iData)
		{
		delete newBlock;
		return KErrNoMemory;
		}
	newBlock->iData->Des().Copy(dataToMove);
	// That's it bar the shouting
	iData->Des().SetLength(aCharacterPosition);
	iFlags.Set(EDirty); // We have to do this because we've invalidated iBlockSize (we can't just update iBlockSize because we don't know how many bytes aCharacterPosition corresponds to).
	newBlock->iPrev = this;
	newBlock->iNext = iNext;
	iNext->iPrev = newBlock;
	iNext = newBlock;
	return KErrNone;
	}

void ByteSwitch(TUint16* data, TInt aLen)
	{
	const TUint16* endp = data + aLen;
	while (data < endp)
		{
		TUint16 ch = *data;
		*data = (ch>>8) | ((ch & 0xFF) << 8);
		data++;
		}
	}
