// filebuffer.h
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
#ifndef FILEBUFFER_H_
#define FILEBUFFER_H_

#include <f32file.h>
#include <Babitflags.h>
#include "bufferbase.h"

class CFileBlock;
class CCnvCharacterSetConverter;

_LIT8(KBigEndianBom,    "\xFE\xFF");
_LIT8(KLittleEndianBom, "\xFF\xFE");
_LIT8(KUtf8Bom,         "\xEF\xBB\xBF");

#ifdef __BIG_ENDIAN__
static const TDesC8& KNativeBom(KBigEndianBom);
#else
static const TDesC8& KNativeBom(KLittleEndianBom);
#endif

/*
 An efficient backing store for document data, that allows files larger than available memory to be shown.
 Data is paged in and out of the store depending on what part of the document is being viewed. The
 granularity of paging is the CFileBlock object, which can represent up to 64KB of on-disk data. Blocks that
 have been modified are kept in memory until the file is saved, whereas unmodified blocks can be flushed
 whenever it is required. We take care to ensure that UTF-8 and CRLF sequences are never split across blocks,
 and that blocks are split if they become too large to page out (ie >64KB)

 CFileBlock keeps some metadata about the block it represents in memory at all times - the extent and number of
 lines in the block. This is so we can support efficient per-character and per-line seeking through the file
 without having to page the whole thing into memory.

 In principle CFileBuffer supports file sizes up to 2GB, but in practice the CFileBlock metadata will probably
 fill the default 1MB heap after around 50-100MB.
*/

class CFileBuffer : public CFedBufferBase
	{
public:

	static CFileBuffer* NewL(RFs& aFs, CCnvCharacterSetConverter* aCharconv, const TDesC& aName, TBool aAllowNonexistantName);
	~CFileBuffer();

protected:
	CFileBuffer(RFs& aFs, CCnvCharacterSetConverter* aCharconv);
	void ConstructL(const TDesC& aName, TBool aAllowNonexistantName);
	void ConstructL();
	void CommonConstructL();

public:
	const TDesC& Title() const;
	TBool Modified() const;
	TDelimiterType DelimiterType() const;
	TEncodingType Encoding() const;
	TBool Editable() const;
	TBool IsOpen() const;

	TInt GetData(MSharedCacheClient& aClient, TInt aDocumentPosition); // "Get me the block containing aDocumentPosition".
	TInt SeekFromOffset(MSharedCacheClient& aClient, TInt aOffset, TInt aNumLinesFromOffset, TInt aLineLength); 
	TBool DocumentPositionIsEof(TInt aDocumentPosition) const; // The block containing aDocumentPosition MUST have already have been loaded (otherwise we could not guarantee const-ness)

	void InsertTextL(TInt aDocumentPosition, const TDesC& aText);
	void DeleteTextL(TRange aRange);
	void SaveL(const TDesC& aName, TBool aReplace);
	TInt Find(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards);

protected:
	static TInt CalculateCacheBufSize(TInt aFileSize);
	void TidyCache(); // Calls UnloadBlockIfPossible on all blocks
	void UnloadBlockIfPossible(CFileBlock* aBlock);
	CFileBlock* FindBlockForDocumentPosition(TInt aPosition, TInt* aOffset, TInt* aLineCountToDocPos=NULL); // If aOffset isn't null, it is set to the offset from the block start to the specified document position. If aLineCountToDocPos is nonnull the line count to the offset is calculated (may load the block if necessary)
	TInt LoadBlock(CFileBlock* aBlock);
	static void DeleteTempFile(TAny* aSelf);
	static void SetFinalNameToTemporary(TAny* aSelf);
	TInt DoFindL(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards);

protected:
	TDelimiterType iDelimType;
	TEncodingType iEncoding;
	RFs& iFs;
	RFile iFile;
	TInt iCacheBufSize;
	TFileName iFinalName;
	TFileName* iTempName; // Used during saving, if we're writing to a temporary.

	CFileBlock* iFirstBlock;
	CFileBlock* iCurrentBlock; // For a given (currently, unimportant) value of current
	RBuf8 iNarrowBuf;
	TBool iUnsaved;
	CCnvCharacterSetConverter* iCharconv; // This wouldn't be needed if the exported version of CnvUtfConverter::ConvertToUnicodeFromUtf8 ACTUALLY WORKED
	};

class CFileBlock : public CBase
	{
public:
	CFileBlock(TInt aFilePosition, TInt aBlockSize);
	~CFileBlock();
	void InsertAfterBlock(CFileBlock* aBlock);
	CFileBlock* Next() const;
	CFileBlock* Prev() const;
	TInt BlockDidLoad(TEncodingType& aFileEncoding, TDelimiterType& aDelimType, const TDesC8& aData, CCnvCharacterSetConverter* aCharconvForUtf8Conversion);
	TInt ConvertToEightBit(TEncodingType aEncoding, TDelimiterType aDelimeter, RBuf8& aResultBuf);
	void FileHasBeenSaved(); // Only call on the first block of the file - it will update other blocks as needed
	
	/* Return the buffer position after the aTarget'th newline, or KErrNotFound.
	 * On return, aTarget is set to the number of newlines actually read.
	 * Pass in KMaxTInt as aTarget to count how many newlines in the buffer. (target passes back the resulting number read, the function returns KErrNotFound in this case because your 'target' of KMaxTInt was not reached)
	 * Pass in negative target to count backwards from end of block
	 * Pass in KMaxTInt as aSoftWrapLineLength if you don't want lines to soft wrap
	 * If non-null, aDelimType is set to the first type of newline encountered in the block
	 */
	static TInt CountNewLines(const TDesC& aDes, TInt& aTarget, TInt aSoftWrapLineLength, TDelimiterType* aDelimType=NULL);
	void Unload();

	TBool IsLoaded() const; // Returns true if the block is in memory, ie if iData.Size() != 0.
	TBool HasBeenLoaded() const; // Indicates that cached data (such as num newlines etc) is valid
	TBool IsDirty() const; // Returns true if the block has been modified
	const TDesC16& Text() const; // block must be loaded

	void CalculateBlockPositions(TInt& aCharacterPos, TInt& aLineCount) const; // Requires iterating block list
	TInt FilePosition() const; // Returns the on-disk location (in bytes) of this block (constant-time operation)
	TInt BlockSize() const; // block size in bytes
	TInt CharacterCount() const; // block must have been loaded, as knowing block size is not sufficient to calculate number of chars
	TInt NewlineCount() const; // block must have been loaded
	TInt CountNewLinesUpToOffset(TInt aOffset) const;

	void InsertTextL(TInt aBlockOffset, const TDesC16& aText);
	void DeleteText(TInt aPos, TInt aLen);

private:
	TInt TransferDataFromPreviousBlock(CFileBlock* aPrev, const TDesC8& aData);
	TInt ReplaceAll(const TDesC& aFrom, const TDesC& aTo);
	TInt SplitBufferAt(TInt aCharacterPosition); // Splits one buffer in to two

private:
	enum TFlags
		{
		EHasBeenLoaded = 0,
		EDirty = 1,
		EHaveCalculatedLineEndings = 2, // We can't use iNumLineEndings=-1 to indicate it's not valid because it's unsigned, and it's unsigned because we need every bit
		};
	TInt iFilePosition; // in bytes
	TUint16 iBlockSize; // in bytes
	TUint16 iNumCharacters; // Cached first time block is loaded. Will never be more than iBlockSize because one byte can never represent more than one character (in any encoding we support)
	// We cache the number of line endings so that "Go to line" operations are fast, the correct block for a particular line can be calculated just by iterating the CFileBlocks
	mutable TUint16 iNumLineEndings; // Number of line endings *in this block*. Mutable so NewlineCount can update it 
	mutable TBitFlags16 iFlags; // Only the EHaveCalculatedLineEndings flag needs to be mutable but no avoiding it
	
	HBufC16* iData;
	CFileBlock* iPrev;
	CFileBlock* iNext;
	};

#endif /*FILEBUFFER_H_*/
