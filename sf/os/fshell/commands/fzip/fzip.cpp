// fzip.cpp
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

#include <zipfile.h>
#include <ezgzip.h>
#include "fzip.h"

_LIT(KGzExtension, ".gz");
_LIT(KZipExtension, ".zip");

CCommandBase* CCmdZip::NewLC()
	{
	CCmdZip* self = new (ELeave) CCmdZip();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdZip::~CCmdZip()
	{
	iFileToZip.Close();
	}
	
CCmdZip::CCmdZip() : CCommandBase(CCommandBase::EManualComplete)
	{
	}

const TDesC& CCmdZip::Name() const
	{	
	_LIT(KName, "fzip");
	return KName;
	}

void CCmdZip::DoRunL()
	{
	FsL();
	if (iUnzip)
		{
		if (!iOptions.IsPresent(&iUnzipPath))
			{
			iUnzipPath = Env().Pwd();
			}

		if (iVerbose)
			{
			// command-line sanity checks
			if (iFileToZip.Count() > 0)
				{
				PrintWarning(_L("--file option is not relevant when unzipping."));
				}
			if (iRecurse)
				{
				PrintWarning(_L("--recurse option is not relevant when unzipping."));
				}
			}
		ExpandArchiveL();
		}
	else
		{
		if (iVerbose)
			{
			// command-line sanity checks
			if (iUnzipPath.Length() > 0)
				{
				PrintWarning(_L("--directory option is not relevant when zipping."));	
				}
			}
		if (iFileToZip.Count() == 0)
			{
			LeaveIfErr(KErrArgument, _L("Specify some files to zip up using --file option."));
			}
		TRAPL(CreateArchiveL(), _L("Couldn't create %S"), &iArchive);
		}
	if (iVerbose)
		{
		Printf(_L("Done\r\n"));	
		}
	Complete(KErrNone);
	}
	
void CCmdZip::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg1, "zipfile");
	aArguments.AppendFileNameL(iArchive, KArg1);
	}

void CCmdZip::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptUnzip, "unzip");
	aOptions.AppendBoolL(iUnzip, KOptUnzip);

	_LIT(KOptDirectory, "directory");
	aOptions.AppendFileNameL(iUnzipPath, KOptDirectory);

	_LIT(KOptRecurse, "recurse");
	aOptions.AppendBoolL(iRecurse, KOptRecurse);

	_LIT(KOptSource, "file");
	aOptions.AppendFileNameL(iFileToZip, KOptSource);

	_LIT(KOptCompressionType, "compression-type");
	aOptions.AppendEnumL((TInt&)iCompressionType, KOptCompressionType);

	_LIT(KOptOverwrite, "overwrite");
	aOptions.AppendBoolL(iOverwrite, KOptOverwrite);
	}


//
// COMPRESSION FUNCTIONS
//

//
// CCmdZip::CreateArchiveL
// determine which zip format to use and go ahead & create the archive
//
void CCmdZip::CreateArchiveL()
	{
	if (iArchive.Length() == 0)
		{
		iArchive = iFileToZip[0];
		iArchive.Append(iCompressionType == EGZip ? KGzExtension() : KZipExtension());
		}

	if (iArchive.Exists(FsL()))
		{
		if (iOverwrite)
			{
			FsL().Delete(iArchive);
			}
		else
			{
			LeaveIfErr(KErrAlreadyExists, _L("File %S already exists on disk. Use --overwrite or specify a different file"), &iArchive);
			}
		}

	if (iCompressionType == EGZip)
		{
		CreateGzArchiveL();
		}
	else
		{
		CreateZipArchiveL();
		}
	}

//
// CCmdZip::CreateGzArchiveL
// create an archive, zipping up all the specified files
//
void CCmdZip::CreateGzArchiveL()
	{
	if (iRecurse)
		{
		LeaveIfErr(KErrArgument, _L("GNU Zip format does not support recursion"));
		}

	if (iFileToZip.Count() > 1)
		{
		LeaveIfErr(KErrArgument, _L("GNU Zip format can only handle a single file"));
		}

	if (iVerbose)
		{
		Printf(_L("Creating '%S'\r\n"), &iArchive);
		}

	// open the input file
	RFile input;
	User::LeaveIfError(input.Open(FsL(), iFileToZip[0], EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);

	CEZFileToGZip* zip = CEZFileToGZip::NewLC(FsL(), iArchive, input);
	while (zip->DeflateL())
		{
		// do nothing
		}
	if (iVerbose)
		{
		Printf(_L("Deflating '%S'\r\n"), &iFileToZip[0]);
		}

	CleanupStack::PopAndDestroy(2); // zip, input
	}

//
// CCmdZip::CreateZipArchiveL
// zip format archive creation
//
void CCmdZip::CreateZipArchiveL()
	{
	CZipItUp* zipArchive = CZipItUp::NewLC(Fs(), iArchive);
	for (TInt ii = 0 ; ii < iFileToZip.Count() ; ii++)
		{
		TFileName2& fileName = iFileToZip[ii];
		fileName.SetTypeL(Fs());
		AddFileL(*zipArchive, fileName);
		}
	zipArchive->CreateZipL();
	if (iVerbose)
		{
		Printf(_L("Created '%S'\r\n"), &iArchive);
		}
	CleanupStack::PopAndDestroy(); // zipArchive 
	}

//
// CCmdZip::AddFileL
// examines a file for wildcards, directories/recursion etc
// if it finds an actual file, it'll add it to the zip archive
// recursive function
//
void CCmdZip::AddFileL(CZipItUp& aZipArchive, const TFileName2& aFile)
	{
	if (aFile.IsDir())
		{
		CDir* dir;
		LeaveIfErr(Fs().GetDir(aFile, KEntryAttMatchMask, EDirsLast, dir), _L("Unable to read directory '%S'"), &aFile);
		CleanupStack::PushL(dir);
		for (TInt ii = 0 ; ii < dir->Count() ; ii++)
			{
			const TEntry& entry = (*dir)[ii];
			TFileName2* newFile = new(ELeave) TFileName2(entry.iName);
			CleanupStack::PushL(newFile);
			newFile->MakeAbsoluteL(aFile.DriveAndPath());
			newFile->SetTypeL(Fs());
			if (newFile->IsDir())
				{
				if (iRecurse)
					{
					AddFileL(aZipArchive, *newFile);
					}
				}
			else
				{
				AddFileL(aZipArchive, *newFile);
				}
			CleanupStack::PopAndDestroy(newFile);
			}
		CleanupStack::PopAndDestroy(dir);
		}
	else
		{
		if (iVerbose)
			{
			Printf(_L("Adding '%S'\r\n"), &aFile);
			}
		aZipArchive.AddFileL(aFile);
		}
	}


//
// DECOMPRESSION FUNCTIONS
//

//
// CCmdZip::ExpandArchiveL
// determine which zip format to use and go ahead & expand the archive
//
void CCmdZip::ExpandArchiveL()
	{
	if (iCompressionType == EGZip)
		{
		ExpandGzArchiveL();
		}
	else
		{
		ExpandZipArchiveL();
		}
	}

//
// CCmdZip::ExpandGzArchiveL
// unzip an existing gzip compressed file
//
void CCmdZip::ExpandGzArchiveL()
	{
	// open the destination file, determine where it goes
	RFile newFile;
	TFileName2 dest(iUnzipPath);
	if (iVerbose)
		{
		Printf(_L("Opening\t\t\'%S\'\r\n"), &iArchive);
		}
	if (iArchive.Ext().CompareF(KGzExtension) != 0)
		{
		LeaveIfErr(KErrArgument, _L("Compressed file must have '.gz' extension."));
		}
	dest.AppendComponentL(iArchive.Name());
	TInt err = Fs().MkDirAll(dest);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		LeaveIfErr(err, _L("Couldn't create path '%S'"), &dest);
		}
	if (iOverwrite)
		{
		err = newFile.Replace(FsL(), dest, EFileStream | EFileRead | EFileWrite | EFileShareAny);
		}
	else
		{
		err = newFile.Create(FsL(), dest, EFileStream | EFileRead | EFileWrite | EFileShareAny);
		}
	LeaveIfErr(err, _L("Couldn't create file %S"), &dest);
	CleanupClosePushL(newFile);

	// inflate the compressed file
	CEZGZipToFile* zip = CEZGZipToFile::NewLC(Fs(), iArchive, newFile);
	while (zip->InflateL())
		{
		// do nothing
		}
	if (iVerbose)
		{
		Printf(_L("Inflating '%S'\r\n"), &dest);
		}
	CleanupStack::PopAndDestroy(2); // zip, newFile
	}

//
// CCmdZip::ExpandZipArchiveL
// Unzip an existing archive iterating through each member file contained within the archive
//
void CCmdZip::ExpandZipArchiveL()
	{
	if (iVerbose)
		{
		Printf(_L("Opening\t\t\'%S\'\r\n"), &iArchive);
		}
	CZipFile* zip = NULL;
	TRAPL(zip = CZipFile::NewL(Fs(), iArchive), _L("Couldn't create CZipFile for %S"), &iArchive);
	CleanupStack::PushL(zip);
	CZipFileMemberIterator* zipIterator = zip->GetMembersL();
	CleanupStack::PushL(zipIterator);
	CZipFileMember* zipMember = zipIterator->NextL();
	while (zipMember)
		{
		CleanupStack::PushL(zipMember);
		ExtractZipFileL(*zip, *zipMember);
		CleanupStack::PopAndDestroy(zipMember);
		zipMember = zipIterator->NextL();
		}
	CleanupStack::PopAndDestroy(2); // zipIterator, zip
	}
	
//
// CCmdZip::ExtractZipFileL
// extracts a single file from within the zip archive
//
void CCmdZip::ExtractZipFileL(CZipFile& aZip, const CZipFileMember& aMember)
	{
	// prep. the destination file. 
	// note if iUnzipPath is not specified, it'll stuff the extracted file in the current directory from which fzip.exe runs
	RFile newFile;
	TFileName2 dest(iUnzipPath);
	dest.AppendComponentL(*aMember.Name());
	TInt err = Fs().MkDirAll(dest);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		LeaveIfErr(err, _L("Couldn't create directory for file %S"), &dest);
		}
	if (aMember.Name()->Right(1) == _L("\\")) return; // It's a directory entry, nothing more to be done

	// prep. the stream
	RZipFileMemberReaderStream* readStream;
	aZip.GetInputStreamL(&aMember, readStream);
	CleanupStack::PushL(readStream);

	if (iOverwrite)
		{
		err = newFile.Replace(Fs(), dest, EFileShareExclusive);
		}
	else
		{
		err = newFile.Create(Fs(), dest, EFileShareExclusive);
		}
	LeaveIfErr(err, _L("Couldn't create file %S"), &dest);
	CleanupClosePushL(newFile);
	if (iVerbose)
		{
		Printf(_L("Inflating '%S'\r\n\tcrc: 0x%x\r\n\tcompressed size: %d\r\n\tuncompressed size: %d\r\n"), &dest, aMember.CRC32(), aMember.CompressedSize(), aMember.UncompressedSize());
		}
		
	// stream from the zip archive member into the destination file
	TInt bytesRead = 0;
	TInt length = KDefaultZipBufferLength; // 32Kb
	if (aMember.UncompressedSize() < KDefaultZipBufferLength)
		{
		length = aMember.UncompressedSize();
		}
	HBufC8* data = HBufC8::NewLC(length);
	TPtr8 ptr = data->Des();
	do
		{
		LeaveIfErr(readStream->Read(ptr, length), _L("Error reading from zip stream"));
		LeaveIfErr(newFile.Write(ptr), _L("Error writing to file %S"), &dest);
		bytesRead += length;
		if ((aMember.UncompressedSize() - bytesRead) < KDefaultZipBufferLength)
			{
			length = aMember.UncompressedSize() - bytesRead;
			}
		} while (length > 0);
		
	// cleanup
	CleanupStack::PopAndDestroy(3); // data, newfile, readstream
	}

	
EXE_BOILER_PLATE(CCmdZip)
