// bufferbase.h
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

#ifndef BUFFERBASE_H_
#define BUFFERBASE_H_

#include <e32base.h>

#include "common.h"

enum TEncodingType
	{
	EEncodingUnknown,
	EEncodingNarrow,
	EEncodingUtf8,
	EEncodingUtf16LE,
	EEncodingUtf16BE,
#ifdef __BIG_ENDIAN__ // I know Symbian doesn't actually define the endian macros (afaics) but if they do, we'll do the right thing(TM)
	EEncodingUtf16Native = EEncodingUtf16BE,
	EEncodingUtf16Switched = EEncodingUtf16LE,
#else
	// Assume little-endian unless specifically told otherwise
	EEncodingUtf16Native = EEncodingUtf16LE,
	EEncodingUtf16Switched = EEncodingUtf16BE,
#endif
	};

enum TDelimiterType
	{
	EDelimNotYetKnown = 0,
	EDelimLF, // Unix
	EDelimCRLF, // DOS
	EDelimCR, // old school mac
	EDelimUnicode, // KUnicodeLineBreak - really doubt we'll see many of these!
	};

class CFileBlock;

//Parent class for all memory buffers which can be used by appropriate views to read/write data and show it on the screen
class CFedBufferBase : public CActive, public MSharedCacheProvider
{
protected:
	CFedBufferBase();

public:
	TInt RefCount() const;
	void PushL(); // Pushes a cleanup stack item that does a DecRef()
	void IncRef();
	TInt DecRef();
	~CFedBufferBase();

public: // From MSharedCacheProvider
	TInt RegisterClient(MSharedCacheClient& aClient, TPtrC& aDes, TRange& aRange, TInt& aRangeStartLineNumber);
	void UnregisterClient(MSharedCacheClient& aClient);

public: // CActive (noops in this class, override if you actually use the CActive-ness
	void DoCancel();
	void RunL();

public:
	virtual const TDesC& Title() const = 0;
	virtual TBool Modified() const;
	virtual TBool Editable() const;
	virtual TDelimiterType DelimiterType() const = 0;
	virtual TEncodingType Encoding() const = 0;
	virtual TBool IsOpen() const;

	virtual TInt GetData(MSharedCacheClient& aClient, TInt aDocumentPosition) = 0; // "Get me the block containing aDocumentPosition".
	virtual TInt SeekFromOffset(MSharedCacheClient& aClient, TInt aOffset, TInt aNumLinesFromOffset, TInt aLineLength) = 0; 
	virtual TBool DocumentPositionIsEof(TInt aDocumentPosition) const = 0; // The block containing aDocumentPosition MUST have already have been loaded (otherwise we could not guarantee const-ness)
	virtual TInt Find(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards) = 0;

	virtual void InsertTextL(TInt aDocumentPosition, const TDesC& aText);
	virtual void DeleteTextL(TRange aRange);
	virtual void SaveL(const TDesC& aName, TBool aReplace);

protected:
	TInt FindClientRequest(MSharedCacheClient& aClient);

	class TClientRequest
		{
	public:
		TClientRequest(MSharedCacheClient* aClient, TPtrC& aDes, TRange& aRange, TInt& aRangeStartLineNumber)
			: iClient(aClient), iDes(aDes), iRange(aRange), iLastRequestedBlock(NULL), iRangeStartLineNumber(aRangeStartLineNumber)
			{}
	public:
		MSharedCacheClient* iClient;
		TPtrC& iDes;
		TRange& iRange; // iRange is the document range for whatever's in iDes (used for something else during seeking)
		CFileBlock* iLastRequestedBlock;
		TInt& iRangeStartLineNumber;
		};

private:
	TInt iRefCount;
protected:
	RArray<TClientRequest> iClientRequests;

};

class CConstDataBuffer : public CFedBufferBase
	{
public:
	CConstDataBuffer(const TDesC& aTitle, const TDesC& aText);
	CConstDataBuffer(const TDesC& aTitle, HBufC* aTextPtr);
	~CConstDataBuffer();

	const TDesC& Title() const;
	TDelimiterType DelimiterType() const;
	TEncodingType Encoding() const;

	TInt GetData(MSharedCacheClient& aClient, TInt aDocumentPosition);
	TInt SeekFromOffset(MSharedCacheClient& aClient, TInt aOffset, TInt aNumLinesFromOffset, TInt aLineLength); 
	TBool DocumentPositionIsEof(TInt aDocumentPosition) const;
	TInt Find(TInt aStartingPosition, const TDesC& aSearchString, TBool aBackwards);

private:
	const TDesC& iTitle;
	TPtrC iText;
	HBufC* iOwnedTextPtr;
	};

#endif /*BUFFERBASE_H_*/
