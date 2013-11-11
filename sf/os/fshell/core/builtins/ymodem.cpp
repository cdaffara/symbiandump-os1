// ymodem.cpp
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

#include <fshell/ioutils.h>
#include "ymodem.h"

using namespace IoUtils;

CCommandBase* CCmdYmodem::NewLC()
	{
	CCmdYmodem* self = new(ELeave) CCmdYmodem();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	self->ConstructL();
	return self;
	}

CCmdYmodem::~CCmdYmodem()
	{
	iFileNames.Close();
	}

CCmdYmodem::CCmdYmodem()
	{
	}

const TDesC& CCmdYmodem::Name() const
	{
	_LIT(KName, "ymodem");	
	return KName;
	}

class TFailedFile
	{
public:
	TFailedFile();
public:
	TInt iError;
	TFileName2 iFileName;
	};

TFailedFile::TFailedFile()
	: iError(KErrNone)
	{
	}

void CCmdYmodem::DoRunL()
	{
	PrepareConsoleToTransferL();

	if (iMode == EReceive)
		{
		TPtrC receiveDir;
		if (iFileNames.Count() > 1)
			{
			PrintError(KErrArgument, _L("Too many arguments - only the receive directory may be specified in receive mode."));
			User::Leave(KErrArgument);
			}
		else if (iFileNames.Count() == 1)
			{
			TFileName2 fileName(iFileNames[0]);
			receiveDir.Set(iFileNames[0]);

			if ((fileName.Exists(FsL()) && !fileName.IsDirL(FsL())) || (!fileName.Exists(FsL()) && !iOverwrite))
				{
				LeaveIfErr(KErrArgument, _L("\"%S\" is not a directory"), &fileName);
				}
			}
		else
			{
			receiveDir.Set(Env().Pwd());
			}

		Progress(_L("YMODEM receive to \"%S\"\r\n"), &receiveDir);
		RArray<TFileName2> receivedFiles;
		CleanupClosePushL(receivedFiles);
		TFailedFile failedFile;
		TBool finished(EFalse);
		while (!finished)
			{
			TSyncResult syncResult = SendSyncL();
			if (syncResult == EEot)
				{
				finished = ETrue;
				}
			else
				{
				iPacketNumber = 0;
				TBool isFinalBlock(EFalse);
				TPtrC block(ReceiveBlockL(ETrue, isFinalBlock));
				TPtrC fileName(block.Ptr()); // filename is the first null terminated string in the block.
				if (fileName.Length() > 0)
					{
					TFileName2 fileName2(fileName);
					fileName2.MakeAbsoluteL(receiveDir);
					TLex lex(block.Mid(fileName.Length() + 1));
					TInt size;
					User::LeaveIfError(lex.Val(size));
					TInt err = KErrNone;
					if (!iOverwrite && fileName2.Exists(FsL()))
						{
						err = KErrAlreadyExists;
						}
					RFile file;
					if (err == KErrNone)
						{
						if (size == 0)
							{
							if (iOverwrite)
								{
								err = file.Replace(FsL(), fileName2, EFileWrite);
								}
							else
								{
								err = file.Create(FsL(), fileName2, EFileWrite);
								}
							}
						else
							{
							Progress(_L("Starting receive of \"%S\" (%d bytes)\r\n"), &fileName, size);
							SendSyncL();
							TRAP(err, ReceiveToFileL(fileName2));
							if (err == KErrNone)
								{
								err = file.Open(FsL(), fileName2, EFileWrite);
								if (err == KErrNone)
									{
									err = file.SetSize(size);
									}
								}
							else
								{
								Fs().Delete(fileName2);
								}
							}
						}
					else
						{
						TRAP_IGNORE(ReceiveToNullL());
						}
					file.Close();
					if (err)
						{
						failedFile.iError = err;
						failedFile.iFileName = fileName2;
						finished = ETrue;
						}
					else
						{
						receivedFiles.Append(fileName2); // Ignore error.
						}
					}
				else
					{
					finished = ETrue;
					}
				}
			}

		CleanupClonsoleAfterTransferL();

		const TInt numFilesReceived = receivedFiles.Count();
		if (numFilesReceived == 0)
			{
			if (failedFile.iError == KErrNone)
				{
				Printf(_L("No files to receive.\r\n"));
				}
			}
		else if (numFilesReceived == 1)
			{
			const TFileName2& fileName = receivedFiles[0];
			Printf(_L("Successfully received \"%S\".\r\n"), &fileName);
			}
		else
			{
			Printf(_L("Successfully received:\r\n"));
			for (TInt i = 0; i < numFilesReceived; ++i)
				{
				const TFileName2& fileName = receivedFiles[i];
				Printf(_L("\t\"%S\"\r\n"), &fileName);
				}
			}

		if (failedFile.iError)
			{
			PrintError(failedFile.iError, _L("Failed to receive \"%S\""), &failedFile.iFileName);
			}
		CleanupStack::PopAndDestroy(&receivedFiles);
		}
	else if (iMode == ESend)
		{
		const TInt numFiles = iFileNames.Count();
		if (numFiles == 0)
			{
			PrintError(KErrArgument, _L("No files to send specified"));
			User::Leave(KErrArgument);
			}
		for (TInt i = 0; i < numFiles; ++i)
			{
			const TFileName2& fileName = iFileNames[i];
			if (fileName.Exists(FsL()))
				{
				if (fileName.IsDirL(FsL()))
					{
					LeaveIfErr(KErrArgument, _L("\"%S\" is not a file"), &fileName);
					}
				}
			else
				{
				LeaveIfErr(KErrArgument, _L("\"%S\" does not exist"), &fileName);
				}
			}
		WaitForSyncL();
		HBufC* buf = HBufC::NewLC(iBlockSize);
		TPtr bufPtr(buf->Des());
		for (TInt i = 0; i < numFiles; ++i)
			{
			const TFileName2& fileName = iFileNames[i];
			LeaveIfFileNotFound(fileName);
			TEntry entry;
			User::LeaveIfError(FsL().Entry(fileName, entry));
			bufPtr.SetLength(bufPtr.MaxLength());
			bufPtr.FillZ();
			bufPtr.Copy(fileName.NameAndExt());
			bufPtr.SetLength(buf->Length() + 1); // To leave a terminating null after the file name.
			bufPtr.AppendNum(entry.iSize);
			if (bufPtr.Length() < iBlockSize)
				{
				// There's not enough data to fill this block, so pad with NULL.
				bufPtr.AppendFill(0, iBlockSize - bufPtr.Length());
				}
			iPacketNumber = 0;
			SendBlockL(*buf);
			WaitForSyncL();
			SendFileL(fileName);
			}
		iPacketNumber = 0;
		bufPtr.SetLength(bufPtr.MaxLength());
		bufPtr.FillZ();
		SendBlockL(*buf);
		CleanupStack::PopAndDestroy(buf);
		CleanupClonsoleAfterTransferL();

		if (numFiles == 1)
			{
			Printf(_L("Successfully sent \"%S\".\r\n"), &iFileNames[0]);
			}
		else
			{
			Printf(_L("Successfully sent:\r\n"));
			for (TInt i = 0; i < numFiles; ++i)
				{
				const TFileName2& fileName = iFileNames[i];
				Printf(_L("\t\"%S\"\r\n"), &fileName);
				}
			}
		}
	User::LeaveIfError(Stdin().CancelCaptureAllKeys());
	}

void CCmdYmodem::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg1, "mode");
	aArguments.AppendEnumL((TInt&)iMode, KArg1);
	_LIT(KArg2, "file_name");
	aArguments.AppendFileNameL(iFileNames, KArg2);
	}

void CCmdYmodem::OptionsL(RCommandOptionList& aOptions)
	{
	CCmdXmodem::OptionsL(aOptions);
	}

