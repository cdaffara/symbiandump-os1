// t_ioread.cpp
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

#include <fshell/ioutils.h>

using namespace IoUtils;

// enum corresponding to RIoReadHandle::TReadMode
_LIT(KReadModeEnum, "full,line,oneormore");

static const TInt KDefaultReadSize = 0x100;

class CIoReadTest : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CIoReadTest();
private:
	CIoReadTest();
	void ReadFileL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TFileName2 iFile;
	TInt iReadSize;
	RIoReadHandle::TReadMode iReadMode;
	RBuf iFileContents;
	
	RIoFile iIoFile;
	RIoReadHandle iFileReader;
	RBuf iReadBuf;
	CTextBuffer* iFileBuffer;
	};


CCommandBase* CIoReadTest::NewLC()
	{
	CIoReadTest* self = new(ELeave) CIoReadTest();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CIoReadTest::~CIoReadTest()
	{
	iFileContents.Close();
	iIoFile.Close();
	iFileReader.Close();
	iReadBuf.Close();
	delete iFileBuffer;
	}

CIoReadTest::CIoReadTest()
	: iReadSize(KDefaultReadSize)
	{
	}

const TDesC& CIoReadTest::Name() const
	{
	_LIT(KName, "t_ioread");	
	return KName;
	}

const TDesC& CIoReadTest::Description() const
	{
	_LIT(KDescription, "Test harness for iosrv file reading functionality");
	return KDescription;
	}
	
void CIoReadTest::ReadFileL()
	{
	RFile file;
	LeaveIfErr(file.Open(Fs(), iFile, EFileRead | EFileShareExclusive), _L("Cannot open file '%S'"), &iFile);
	CleanupClosePushL(file);
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	
	RBuf8 contents;
	contents.CreateL(fileSize);
	CleanupClosePushL(contents);
	User::LeaveIfError(file.Read(contents));
	
	// convert data to 16-bit which is how it'll come out of the io server
	iFileContents.CreateL(fileSize);
	iFileContents.Copy(contents); // copy & expand
	CleanupStack::PopAndDestroy(2, &file); // contents, file
	}

void CIoReadTest::DoRunL()
	{
	// read the file into memory
	ReadFileL();
	
	LeaveIfErr(iIoFile.Create(IoSession(), iFile, RIoFile::ERead), _L("Cannot create file end point for '%S'"), &iFile);
	
	iFileReader.CreateL(IoSession());
	iIoFile.AttachL(iFileReader, RIoEndPoint::EForeground);
	
	iFileReader.SetReadModeL(iReadMode);
	
	iReadBuf.CreateL(iReadSize);
	
	
	iFileBuffer = CTextBuffer::NewL(0x20);
	TInt err;
	do
		{
		iReadBuf.Zero();
		err = iFileReader.Read(iReadBuf);
		iFileBuffer->AppendL(iReadBuf);
		} while (err == KErrNone);
		
	if (err!=KErrEof) LeaveIfErr(err, _L("Unexpected error from RIoReadHandle::Read"));
	
	TInt completion = 0;
	if (iFileBuffer->Length() != iFileContents.Length())
		{
		PrintWarning(_L("Amount of data read from file (%d) differs from file size (%d)."), iFileBuffer->Length(), iFileContents.Length());
		completion = 1;
		}
	if (iFileBuffer->Descriptor().Compare(iFileContents) != 0)
		{
		PrintWarning(_L("Data read from file differs from file contents."));
		completion = 1;
		}
	Complete(completion);
	}

void CIoReadTest::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KFile, "file");
	_LIT(KFileDescription, "File to test with");
	aArguments.AppendFileNameL(iFile, KFile, KFileDescription);
	}

void CIoReadTest::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KReadSize, "size");
	_LIT(KReadSizeDescription, "The size of each read to issue");
	aOptions.AppendIntL(iReadSize, 's', KReadSize, KReadSizeDescription);
	
	_LIT(KReadMode, "mode");
	_LIT(KReadModeDescription, "The type of read to issue");
	aOptions.AppendEnumL((TInt&)iReadMode, 'm', KReadMode, KReadModeDescription, KReadModeEnum);
	}


EXE_BOILER_PLATE(CIoReadTest)

