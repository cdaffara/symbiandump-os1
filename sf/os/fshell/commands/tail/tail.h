// tail.h
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

#include <fshell/ioutils.h>
#include "file_reader.h"
#include "character_converter.h"

using namespace IoUtils;

class CFileWatcher;
class CTrailingLineFinder;


class MTrailingLineFinderObserver
	{
public:
	virtual void HandleTrailingLines(TInt aNumLinesFound, TInt aEndFilePos) = 0;
	virtual void HandleTrailingLineError(TInt aError) = 0;
	};


class MFileChangeObserver
	{
public:
	virtual void HandleFileChange(const TDesC& aFileName) = 0;
	};


class CCmdTail : public CCommandBase, public MTrailingLineFinderObserver, public MFileChangeObserver, public MFileReaderObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdTail();
private:
	CCmdTail();
	void ConstructL();
	void WriteChunkToConsoleL(const TDesC8& aData, TReadType aType);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private: // From MTrailingLineFinderObserver.
	virtual void HandleTrailingLines(TInt aNumLinesFound, TInt aEndFilePos);
	virtual void HandleTrailingLineError(TInt aError);
private: // From MFileChangeObserver.
	virtual void HandleFileChange(const TDesC& aFileName);
private: // From MFileReaderObserver.
	virtual void HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue);
	virtual void HandleFileReadError(TInt aError);
private:
	TFileName2 iFileName;
	CCharacterConverter* iCharacterConverter;
	CTrailingLineFinder* iTrailingLineFinder;
	CFileWatcher* iFileWatcher;
	CFileReader* iFileReader;
	TBool iFollow;
	TInt iNumLines;
	TInt iFilePos;
	};
