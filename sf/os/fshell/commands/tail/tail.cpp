// tail.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "character_converter.h"
#include "file_reader.h"
#include "tail.h"

const TInt KBlockSize = 512;
const TText KUnicodeParagraphSeparator = 0x2029;
const TText KUnicodeLineSeparator = 0x2028;
const TText KCarriageReturn = 0x000d;
const TText KLineFeed = 0x000a;


//
// CTrailingLineFinder.
//

class CTrailingLineFinder : public CBase, public MFileReaderObserver
	{
public:
	static CTrailingLineFinder* NewL(TInt aBlockSize, CCharacterConverter& aCharacterConverter);
	~CTrailingLineFinder();
	void Find(CFileReader& aFileReader, const TDesC& aFileName, TInt aNumLines, MTrailingLineFinderObserver& aObserver);
	void Cancel();
	const TDesC& operator[](TInt aIndex) const;
private:
	CTrailingLineFinder(CCharacterConverter& aCharacterConverter);
	void ConstructL(TInt aBlockSize);
	void HandleFileDataL(const TDesC8& aData, TReadType aType, TBool& aContinue);
	void FindNextLine(const TDesC& aData, TInt aOffset, TPtrC& aLine, TBool& aComplete);
	void AddLineL(const TDesC& aLine, TBool aComplete);
private:	// From MFileReaderObserver.
	virtual void HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue);
	virtual void HandleFileReadError(TInt aError);
private:
	CFileReader* iFileReader;
	CCharacterConverter& iCharacterConverter;
	MTrailingLineFinderObserver* iObserver;
	HBufC8* iBuf;
	TPtr8 iPtr;
	RPointerArray<HBufC> iTrailingLines;
	TInt iCurrentLineIndex;
	TInt iNumLinesToFind;
	TInt iNumCompleteLinesFound;
	TInt iFilePos;
	TBool iPossiblySplitCrLf;
	TBool iLastLineNotComplete;
	};

CTrailingLineFinder* CTrailingLineFinder::NewL(TInt aBlockSize, CCharacterConverter& aCharacterConverter)
	{
	CTrailingLineFinder* self = new(ELeave) CTrailingLineFinder(aCharacterConverter);
	CleanupStack::PushL(self);
	self->ConstructL(aBlockSize);
	CleanupStack::Pop(self);
	return self;
	}

CTrailingLineFinder::~CTrailingLineFinder()
	{
	delete iBuf;
	iTrailingLines.ResetAndDestroy();
	}

void CTrailingLineFinder::Find(CFileReader& aFileReader, const TDesC& aFileName, TInt aNumLines, MTrailingLineFinderObserver& aObserver)
	{
	ASSERT(!iFileReader);
	iTrailingLines.ResetAndDestroy();
	iFileReader = &aFileReader;
	iObserver = &aObserver;
	iNumLinesToFind = aNumLines;
	iNumCompleteLinesFound = 0;
	iCurrentLineIndex = -1;
	iPossiblySplitCrLf = EFalse;
	iFileReader->Read(aFileName, *this);
	}

void CTrailingLineFinder::Cancel()
	{
	if (iFileReader)
		{
		iFileReader->Cancel();
		}
	}

const TDesC& CTrailingLineFinder::operator[](TInt aIndex) const
	{
	return *iTrailingLines[aIndex];
	}

void CTrailingLineFinder::HandleFileDataL(const TDesC8& aData, TReadType aType, TBool& aContinue)
	{
	iFilePos += aData.Length();
	aContinue = ETrue;
	const TDesC* convertedData = NULL;

	switch (aType)
		{
		case MFileReaderObserver::EFirst:
			{
			convertedData = &iCharacterConverter.ConvertChunkL(aData, CCharacterConverter::EFirst);
			break;
			}
		case MFileReaderObserver::EMiddle:
			{
			convertedData = &iCharacterConverter.ConvertChunkL(aData, CCharacterConverter::EMiddle);
			break;
			}
		case MFileReaderObserver::ELast:
			{
			convertedData = &iCharacterConverter.ConvertChunkL(aData, CCharacterConverter::ELast);
			break;
			}
		default:
			{
			ASSERT(FALSE);
			}
		}

	if (convertedData->Length() > 0)
		{
		TInt offset = 0;
		FOREVER
			{
			TPtrC linePtr;
			TBool completeLine;
			FindNextLine(*convertedData, offset, linePtr, completeLine);
			AddLineL(linePtr, completeLine);
			if (!completeLine)
				{
				break;
				}
			offset += linePtr.Length();
			}
		}

	if (aType == MFileReaderObserver::ELast)
		{
		iFileReader = NULL;
		iObserver->HandleTrailingLines(iCurrentLineIndex + 1, iFilePos);
		}
	}

void CTrailingLineFinder::AddLineL(const TDesC& aLine, TBool aComplete)
	{
	if (iLastLineNotComplete)
		{
		HBufC*& buf = iTrailingLines[iCurrentLineIndex];
		buf = buf->ReAllocL(buf->Length() + aLine.Length());
		buf->Des() += aLine;
		}
	else
		{
		HBufC* line = aLine.AllocLC();
		if (iNumCompleteLinesFound <= iNumLinesToFind)
			{
			++iCurrentLineIndex;
			User::LeaveIfError(iTrailingLines.Append(line));
			}
		else
			{
			delete iTrailingLines[0];
			iTrailingLines.Remove(0);
			iTrailingLines.Append(line); // Will always succeed due to the above removal.
			}
		CleanupStack::Pop(line);
		}

	iLastLineNotComplete = !aComplete;
	if (aComplete)
		{
		++iNumCompleteLinesFound;
		}
	}

CTrailingLineFinder::CTrailingLineFinder(CCharacterConverter& aCharacterConverter)
	: iCharacterConverter(aCharacterConverter), iPtr(NULL, 0)
	{
	}

void CTrailingLineFinder::ConstructL(TInt aBlockSize)
	{
	iBuf = HBufC8::NewL(aBlockSize);
	iPtr.Set(iBuf->Des());
	}

void CTrailingLineFinder::FindNextLine(const TDesC& aData, TInt aOffset, TPtrC& aLine, TBool& aComplete)
	{
	const TText* first = aData.Ptr() + aOffset;
	const TText* p = first;
	const TText* last = first + aData.Length() - aOffset - 1;
	aComplete = EFalse;
	TBool foundLineBreak = EFalse;
	while ((p <= last) && !foundLineBreak)
		{
		if (iPossiblySplitCrLf)
			{
			iPossiblySplitCrLf = EFalse;
			if (*p == KLineFeed)
				{
				aComplete = ETrue;
				foundLineBreak = ETrue;
				}
			}
		else if ((*p == KUnicodeParagraphSeparator) || (*p == KUnicodeLineSeparator) || (*p == KLineFeed))
			{
			aComplete = ETrue;
			foundLineBreak = ETrue;
			}
		else if (*p == KCarriageReturn)
			{
			if (p == last)
				{
				iPossiblySplitCrLf = ETrue;
				}
			else if (*(p + 1) == KLineFeed)
				{
				aComplete = ETrue;
				foundLineBreak = ETrue;
				++p;
				}
			else
				{
				aComplete = ETrue;
				foundLineBreak = ETrue;
				}
			}
		++p;
		}

	aLine.Set(first, p - first);
	}

void CTrailingLineFinder::HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue)
	{
	TRAPD(err, HandleFileDataL(aData, aType, aContinue));
	if (err)
		{
		iObserver->HandleTrailingLineError(err);
		}
	}

void CTrailingLineFinder::HandleFileReadError(TInt aError)
	{
	iObserver->HandleTrailingLineError(aError);
	}


//
// CFileWatcher.
//

class CFileWatcher : public CActive
	{
public:
	static CFileWatcher* NewL(RFs& aFs);
	~CFileWatcher();
	void Start(const TDesC& aFileName, MFileChangeObserver& aObserver);
private:
	CFileWatcher(RFs& aFs);
	void Queue();
private:	// From CActive.
	virtual void DoCancel();
	virtual void RunL();
private:
	RFs& iFs;
	TFileName iFileName;
	MFileChangeObserver* iObserver;
	};


CFileWatcher* CFileWatcher::NewL(RFs& aFs)
	{
	return new(ELeave) CFileWatcher(aFs);
	}

CFileWatcher::~CFileWatcher()
	{
	Cancel();
	}

void CFileWatcher::Start(const TDesC& aFileName, MFileChangeObserver& aObserver)
	{
	ASSERT(!IsActive());
	iFileName = aFileName;
	iObserver = &aObserver;
	Queue();
	}

CFileWatcher::CFileWatcher(RFs& aFs)
	: CActive(CActive::EPriorityStandard), iFs(aFs)
	{
	CActiveScheduler::Add(this);
	}

void CFileWatcher::Queue()
	{
	ASSERT(!IsActive());
	iFs.NotifyChange(ENotifyWrite, iStatus, iFileName);
	SetActive();
	}

void CFileWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);
	}

void CFileWatcher::RunL()
	{
	Queue();
	iObserver->HandleFileChange(iFileName);
	}


//
// CCmdTail.
//

CCommandBase* CCmdTail::NewLC()
	{
	CCmdTail* self = new(ELeave) CCmdTail();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdTail::~CCmdTail()
	{
	delete iCharacterConverter;
	delete iTrailingLineFinder;
	delete iFileWatcher;
	delete iFileReader;
	}

CCmdTail::CCmdTail()
	: CCommandBase(EManualComplete), iNumLines(10)
	{
	}

void CCmdTail::ConstructL()
	{
	BaseConstructL();
	}

void CCmdTail::WriteChunkToConsoleL(const TDesC8& aData, TReadType aType)
	{
	switch (aType)
		{
		case MFileReaderObserver::EFirst:
			{
			Write(iCharacterConverter->ConvertChunkL(aData, CCharacterConverter::EFirst));
			break;
			}
		case MFileReaderObserver::EMiddle: // Treat "middle" and "last" to same as with tailing it's not possible to know if "last" is really last.
		case MFileReaderObserver::ELast:
			{
			Write(iCharacterConverter->ConvertChunkL(aData, CCharacterConverter::EMiddle));
			break;
			}
		default:
			{
			ASSERT(FALSE);
			}
		}
	}

const TDesC& CCmdTail::Name() const
	{
	_LIT(KName, "tail");
	return KName;
	}

void CCmdTail::DoRunL()
	{
	iCharacterConverter = CCharacterConverter::NewL(KBlockSize, FsL());
	iTrailingLineFinder = CTrailingLineFinder::NewL(KBlockSize, *iCharacterConverter);
	iFileWatcher = CFileWatcher::NewL(Fs());
	iFileReader = CFileReader::NewL(KBlockSize, Fs(), ETrue);
	iTrailingLineFinder->Find(*iFileReader, iFileName, iNumLines, *this);
	}

void CCmdTail::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptFollow, "follow");
	_LIT(KOptNumLines, "lines");
	aOptions.AppendBoolL(iFollow, KOptFollow);
	aOptions.AppendIntL(iNumLines, KOptNumLines);
	}

void CCmdTail::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);
	}

void CCmdTail::HandleTrailingLines(TInt aNumLinesFound, TInt aEndFilePos)
	{
	if (iFollow)
		{
		iFilePos = aEndFilePos;
		iFileWatcher->Start(iFileName, *this);
		}
	for (TInt i = 0; i < aNumLinesFound; ++i)
		{
		Write((*iTrailingLineFinder)[i]);
		}
	if (!iFollow)
		{
		Complete();
		}
	}

void CCmdTail::HandleTrailingLineError(TInt aError)
	{
	PrintError(aError, _L("Problem finding trailing %d lines in %S: %d"), iNumLines, &iFileName, aError);
	Complete(aError);
	}

void CCmdTail::HandleFileChange(const TDesC& /*aFileName*/)
	{
	if (!iFileReader->IsActive())
		{
		iFileReader->Read(iFileName, iFilePos, *this);
		}
	}

void CCmdTail::HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue)
	{
	aContinue = ETrue;
	iFilePos += aData.Length();
	TRAPD(err, WriteChunkToConsoleL(aData, aType));
	if (err)
		{
		aContinue = EFalse;
		PrintError(err, _L("Problem writing chunk of %S to console: %d"), &iFileName, err);
		}
	}

void CCmdTail::HandleFileReadError(TInt aError)
	{
	PrintWarning(_L("Problem reading %S: %d"), &iFileName, aError);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdTail)
#endif

