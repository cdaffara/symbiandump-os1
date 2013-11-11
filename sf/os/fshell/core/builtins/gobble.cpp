// gobble.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "gobble.h"
#include <fshell/ltkutils.h>


CCommandBase* CCmdGobble::NewLC()
	{
	CCmdGobble* self = new(ELeave) CCmdGobble();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdGobble::~CCmdGobble()
	{
	}

CCmdGobble::CCmdGobble()
	: iBlockSize(512)
	{
	}

const TDesC& CCmdGobble::Name() const
	{
	_LIT(KName, "gobble");
	return KName;
	}

void CCmdGobble::DoRunL()
	{
	RFs& fs = FsL();
	if (iAmount == 0)
		{
		TInt drive = EDriveC;
		if (iFileName.Length() == 0) LeaveIfErr(KErrBadName, _L("Bad file name"));
		RFs::CharToDrive(iFileName[0], drive);
		TVolumeInfo volInfo;
		LeaveIfErr(fs.Volume(volInfo, drive), _L("Couldn't get volume information for drive %c"), iFileName[0]);
		iAmount = volInfo.iFree;
		}

	if (!iNoWrite && iAmount < iBlockSize)
		{
		LeaveIfErr(KErrArgument, _L("The amount to consume must be greater than the block size (%d)"), iBlockSize);
		}
	if (iAmount & 0x80000000)
		{
		LeaveIfErr(KErrArgument, _L("The amount to consume is too large (maximum is %d)"), KMaxTInt);
		}
	if (iBlockSize & 0x80000000)
		{
		LeaveIfErr(KErrArgument, _L("The block size is too large (maximum is %d)"), KMaxTInt);
		}
	fs.MkDirAll(iFileName);
	RFile file;
	TInt err = file.Open(fs, iFileName, EFileWrite);
	if (err == KErrNotFound)
		{
		err = file.Create(fs, iFileName, EFileWrite);
		}
	LeaveIfErr(err, _L("Couldn't create file %S"), &iFileName);
	CleanupClosePushL(file);

	if (iNoWrite)
		{
		TInt size = 0;
		LeaveIfErr(file.Size(size), _L("Couldn't get file size"));
		size += iAmount;
		LeaveIfErr(file.SetSize(size), _L("Couldn't set filesize to %d"), size);
		}
	else
		{
		TInt pos = 0;
		User::LeaveIfError(file.Seek(ESeekEnd, pos));
		HBufC8* buf = HBufC8::NewLC(iBlockSize);
		TPtr8 ptr(buf->Des());
		ptr.Fill(TChar('x'), iBlockSize);
		
		TInt toWrite = static_cast<TInt>(iAmount);
		do
			{
			TInt writeSize;
			if (toWrite > static_cast<TInt>(iBlockSize))
				{
				writeSize = static_cast<TInt>(iBlockSize);
				}
			else
				{
				writeSize = toWrite;
				}
			ptr.SetLength(writeSize);
			err = file.Write(ptr);
			if (err == KErrNone)
				{
				if (iVerbose)
					{
					Printf(_L("\rWrote %d"), iAmount - toWrite);
					}
				toWrite -= writeSize;
				}
			}
		while ((err == KErrNone) && (toWrite > 0));
		if (iVerbose)
			{
			Printf(_L("\rWrote %d"), iAmount - toWrite);
			}
		CleanupStack::PopAndDestroy(buf);
		}
	CleanupStack::PopAndDestroy(&file);
	}

void CCmdGobble::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KCmdOptVerbose);

	_LIT(KCmdOptNoWrite, "no-write");
	aOptions.AppendBoolL(iNoWrite, KCmdOptNoWrite);
	}

void CCmdGobble::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);

	_LIT(KArgAmount, "amount");
	aArguments.AppendUintL(iAmount, KArgAmount);

	_LIT(KArgBlockSize, "block_size");
	aArguments.AppendUintL(iBlockSize, KArgBlockSize);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdGobble)
#endif

