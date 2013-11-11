// cat.cpp
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
#include "cat.h"

const TInt KDefaultBlockSize = 512;


CCommandBase* CCmdCat::NewLC()
	{
	CCmdCat* self = new(ELeave) CCmdCat();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdCat::~CCmdCat()
	{
	iFiles.Close();
	iBuf16.Close();
	delete iCharacterConverter;
	delete iFileReader;
	}

CCmdCat::CCmdCat()
	: CCommandBase(EManualComplete), iBlockSize(KDefaultBlockSize)
	{
	}

void CCmdCat::ConstructL()
	{
	BaseConstructL();
	}

void CCmdCat::ReadNextFile()
	{
	if (iFiles.Count() > 0)
		{
		iFileReader->Read(iFiles[0], *this);
		}
	else
		{
		Complete();
		}
	}

void CCmdCat::RemoveCurrentFileName()
	{
	iFiles.Remove(0);
	}

void CCmdCat::WriteChunkToConsoleL(const TDesC8& aData, TReadType aType)
	{
	if (iEncoding == EBinary)
		{
		iBuf16.Copy(aData);
		Write(iBuf16);
		if (aType == MFileReaderObserver::ELast)
			{
			RemoveCurrentFileName();
			ReadNextFile();
			}
		}
	else if (iEncoding == ELtkUtf8)
		{
		iBuf16.Zero();
		iBuf16.AppendUtf8L(aData);
		Write(iBuf16);
		if (aType == MFileReaderObserver::ELast)
			{
			TInt firstBad;
			iBuf16.FinalizeUtf8(firstBad);
			if (firstBad != KErrNotFound) PrintWarning(_L("Found bad UTF-8 sequence in %S at byte offset %d"), &iFiles[0], firstBad);

			RemoveCurrentFileName();
			ReadNextFile();
			}
		}
	else
		{
		switch (aType)
			{
			case MFileReaderObserver::EFirst:
				{
				Write(iCharacterConverter->ConvertChunkL(aData, CCharacterConverter::EFirst));
				break;
				}
			case MFileReaderObserver::EMiddle:
				{
				Write(iCharacterConverter->ConvertChunkL(aData, CCharacterConverter::EMiddle));
				break;
				}
			case MFileReaderObserver::ELast:
				{
				Write(iCharacterConverter->ConvertChunkL(aData, CCharacterConverter::ELast));
				RemoveCurrentFileName();
				ReadNextFile();
				break;
				}
			default:
				{
				ASSERT(FALSE);
				}
			}
		}
	}

const TDesC& CCmdCat::Name() const
	{
	_LIT(KName, "cat");
	return KName;
	}

void CCmdCat::DoRunL()
	{
	if (iBinary)
		{
		if (iOptions.IsPresent(&iEncoding))
			{
			PrintWarning(_L("--encoding overrides legacy --binary option"));
			}
		else
			{
			iEncoding = EBinary;
			}
		}

	if (iEncoding == EBinary || iEncoding == ELtkUtf8 || iFiles.Count() == 0)
		{
		iBuf16.CreateL(iBlockSize);
		}
	else
		{
		TUint charset;
		switch (iEncoding)
			{
			case EUtf8:
				charset = KCharacterSetIdentifierUtf8;
				break;
			case ELatin1:
				charset = KCharacterSetIdentifierIso88591;
				break;
			case EAuto:
			default:
				charset = 0;
				break;

			}
		TRAPL(iCharacterConverter = CCharacterConverter::NewL(iBlockSize, FsL(), charset), _L("Couldn't load charconv - try using --binary mode"));
		}
	iFileReader = CFileReader::NewL(iBlockSize, FsL(), iForce);

	if (iFiles.Count() > 0)
		{
		ReadNextFile();
		}
	else
		{
		TInt err = KErrNone;
		while (err == KErrNone)
			{
			err = Stdin().Read(iBuf16);
			if (err == KErrNone)
				{
				Write(iBuf16);
				}
			}
		if (err == KErrEof)
			{
			err = KErrNone;
			}
		Complete(err);
		}
	}

void CCmdCat::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptForce, "force");
	_LIT(KOptBinary, "binary");
	_LIT(KOptEncoding, "encoding");
	_LIT(KOptBlockSize, "block-size");

	aOptions.AppendBoolL(iForce, KOptForce);
	aOptions.AppendEnumL((TInt&)iEncoding, KOptEncoding);
	aOptions.AppendIntL(iBlockSize, KOptBlockSize);
	aOptions.AppendBoolL(iBinary, KOptBinary);
	}

void CCmdCat::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFiles, "file_name");
	aArguments.AppendFileNameL(iFiles, KArgFiles);
	}

CCommandBase& CCmdCat::Command()
	{
	return *this;
	}

void CCmdCat::HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue)
	{
	aContinue = ETrue;
	TRAPD(err, WriteChunkToConsoleL(aData, aType));
	if (err)
		{
		aContinue = EFalse;
		PrintWarning(_L("Problem writing chunk of %S to console: %d"), &iFiles[0], err);
		RemoveCurrentFileName();
		ReadNextFile();
		}
	}

void CCmdCat::HandleFileReadError(TInt aError)
	{
	PrintWarning(_L("Problem reading %S: %d"), &iFiles[0], aError);
	RemoveCurrentFileName();
	ReadNextFile();
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdCat)
#endif

