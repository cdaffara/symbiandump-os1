// cat.h
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
#include <fshell/descriptorutils.h>

using namespace IoUtils;

class CCmdCat : public CCommandBase, public MFileReaderObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdCat();
private:
	CCmdCat();
	void ConstructL();
	void ReadNextFile();
	void RemoveCurrentFileName();
	void WriteChunkToConsoleL(const TDesC8& aData, TReadType aType);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private: // From MFileReaderObserver.
	virtual CCommandBase& Command();
	virtual void HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue);
	virtual void HandleFileReadError(TInt aError);
private:
	RArray<TFileName2> iFiles;
	CCharacterConverter* iCharacterConverter;
	CFileReader* iFileReader;
	TBool iForce;
	TBool iBinary;
	LtkUtils::RLtkBuf16 iBuf16;
	enum TEncoding
		{
		EAuto,
		EBinary,
		EUtf8,
		ELatin1,
		ELtkUtf8,
		};
	TEncoding iEncoding;
	TInt iBlockSize;
	};
