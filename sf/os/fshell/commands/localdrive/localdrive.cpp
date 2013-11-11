// localdrive.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <d32locd.h>

using namespace IoUtils;

class CCmdLocalDrive : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdLocalDrive();
private:
	CCmdLocalDrive();
	TInt OpenL(TInt aDrive, TBool aLeaveOnConnectErr);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCmd
		{
		EList,
		ERead,
		EWrite,
		};
	TInt iDriveNum;
	TCmd iCmd;
	TBool iSizesInBytes;
	TFileName2 iFile;

	RLocalDrive iDrive;
	TLocalDriveCapsV2 iCaps;
	};

EXE_BOILER_PLATE(CCmdLocalDrive)

CCommandBase* CCmdLocalDrive::NewLC()
	{
	CCmdLocalDrive* self = new(ELeave) CCmdLocalDrive();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdLocalDrive::~CCmdLocalDrive()
	{
	iDrive.Close();
	}

CCmdLocalDrive::CCmdLocalDrive()
	{
	}

const TDesC& CCmdLocalDrive::Name() const
	{
	_LIT(KName, "localdrive");	
	return KName;
	}

void CCmdLocalDrive::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCmd, _L("operation"));
	aArguments.AppendIntL(iDriveNum, _L("drive"));
	}

void CCmdLocalDrive::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iSizesInBytes, _L("bytes"));
	aOptions.AppendFileNameL(iFile, _L("file"));
	}

// More GCC issues (see qresources3\src\utils.cpp). Happily the compiler is happier with SLitC and DESC than it is with TLitC and _LIT
#include <fshell/descriptorutils.h>
#define CASE_LIT(x) case x: { static const LtkUtils::SLitC KName = DESC(#x); return &KName; }

const TDesC* MediaTypeDesc(TMediaType aType)
	{
	enum TEnumsNotIn91
		{
		EMediaRotatingMedia = EMediaNANDFlash + 1,
		};
	switch (aType)
		{
		CASE_LIT(EMediaNotPresent)
		CASE_LIT(EMediaUnknown)
		CASE_LIT(EMediaFloppy)
		CASE_LIT(EMediaHardDisk)
		CASE_LIT(EMediaCdRom)
		CASE_LIT(EMediaRam)
		CASE_LIT(EMediaFlash)
		CASE_LIT(EMediaRom)
		CASE_LIT(EMediaRemote)
		CASE_LIT(EMediaNANDFlash)
		CASE_LIT(EMediaRotatingMedia)
		default:
			{
			_LIT(KUnknown, "?");
			return &KUnknown;
			}
		}
	}

void CCmdLocalDrive::DoRunL()
	{
	if (iCmd == EList)
		{
		for (TInt i = 0; i < KMaxLocalDrives; i++)
			{
			if (iArguments.IsPresent(&iDriveNum) && i != iDriveNum) continue;
			TInt err = OpenL(i, EFalse);
			if (err == KErrNone)
				{
				CTextBuffer* buf = CTextBuffer::NewLC(256);
				buf->AppendFormatL(_L("Found drive %d partitionType=0x%x mediatype=%S(%d) size="), i, iCaps.iPartitionType, MediaTypeDesc(iCaps.iType), iCaps.iType);
				if (iSizesInBytes)
					{
					buf->AppendFormatL(_L("%Ld"), iCaps.iSize);
					}
				else
					{
					buf->AppendHumanReadableSizeL(iCaps.iSize, EUnaligned);
					}
				if (iCaps.iType == EMediaFlash)
					{
					// NOR flash
					buf->AppendFormatL(_L(" eraseblocksize=%d"), iCaps.iEraseBlockSize);
					}

				buf->AppendL(_L("\r\n"));
				buf->Write(Stdout());
				CleanupStack::PopAndDestroy(buf);
				}
			iDrive.Close();
			}
		}
	else if (iCmd == ERead)
		{
		if (!iArguments.IsPresent(&iDriveNum)) LeaveIfErr(KErrArgument, _L("Must supply <drive> argument for the read command"));
		OpenL(iDriveNum, ETrue);
		RFile file;
		CleanupClosePushL(file);
		const TInt KBufSize = 32*1024;
		RBuf8 buf;
		CleanupClosePushL(buf);
		if (iFile.Length())
			{
			LeaveIfErr(file.Create(FsL(), iFile, EFileWrite), _L("Couldn't create file %S"), &iFile);
			buf.CreateL(KBufSize);
			}
		else
			{
			LeaveIfErr(Stdout().SetMode(RIoReadWriteHandle::EBinary), _L("Unable to set stdout to binary mode"));
			buf.CreateL(KBufSize*2);
			}

		TInt64 bytesRead = 0;
		while (bytesRead < iCaps.iSize)
			{
			buf.Zero();
			LeaveIfErr(iDrive.Read(bytesRead, KBufSize, buf), _L("Failed to read %Ld+%d bytes from partition"), bytesRead, KBufSize);
			bytesRead += buf.Length();
			if (file.SubSessionHandle())
				{
				LeaveIfErr(file.Write(buf), _L("Couldn't write %d bytes to file"), buf.Length());
				}
			else
				{
				Stdout().Write(buf.Expand());
				}
			}
		CleanupStack::PopAndDestroy(2, &file); // buf, file
		}
	else if (iCmd == EWrite)
		{
		if (!iArguments.IsPresent(&iDriveNum)) LeaveIfErr(KErrArgument, _L("Must supply <drive> argument for the write command"));
		if (!iOptions.IsPresent(&iFile)) LeaveIfErr(KErrArgument, _L("Must supply file name argument for the write command, using --file"));

		OpenL(iDriveNum, ETrue);
		RFile file;
		CleanupClosePushL(file);
		const TInt KBufSize = 32*1024;
		RBuf8 buf;
		CleanupClosePushL(buf);
		LeaveIfErr(file.Open(FsL(), iFile, EFileRead), _L("Couldn't open file %S"), &iFile);
		TInt fileSize;
		LeaveIfErr(file.Size(fileSize), _L("Couldn't read file size"));
		if (fileSize != iCaps.iSize) LeaveIfErr(KErrNotReady, _L("File size (%d) does not match partition size (%d)"), fileSize, (TInt)iCaps.iSize);

		buf.CreateL(KBufSize);

		TInt64 bytesRead = 0;
		while (bytesRead < fileSize)
			{
			buf.Zero();
			LeaveIfErr(file.Read(buf), _L("Failed to read file"));
			LeaveIfErr(iDrive.Write(bytesRead, buf), _L("Couldn't write %d bytes to flash at partition offset %Ld"), buf.Length(), bytesRead);
			bytesRead += buf.Length();
			}
		CleanupStack::PopAndDestroy(2, &file); // buf, file
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("Unrecognised operation %d"), iCmd);
		}
	}

TInt CCmdLocalDrive::OpenL(TInt aDrive, TBool aLeaveOnConnectErr)
	{
	if (aDrive >= KMaxLocalDrives) LeaveIfErr(KErrArgument, _L("drive number %d is outside of KMaxLocalDrives (%d)"), aDrive, KMaxLocalDrives);

	TBool changed; // What is this for?
	TInt err = iDrive.Connect(aDrive, changed);
	if (err == KErrNone)
		{
		TPckg<TLocalDriveCapsV2> capsBuf(iCaps);
		err = iDrive.Caps(capsBuf);
		if (err)
			{
			iDrive.Close();
			if (aLeaveOnConnectErr) LeaveIfErr(err, _L("Opened drive %d but couldn't read caps"), aDrive);
			}
		}
	else if (aLeaveOnConnectErr)
		{
		LeaveIfErr(err, _L("Couldn't open drive %d"), aDrive);
		}
	return err;
	}
