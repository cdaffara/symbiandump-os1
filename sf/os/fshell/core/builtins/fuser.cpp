// fuser.cpp
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

#include "fuser.h"
#include <f32file.h>


CCommandBase* CCmdFileUser::NewLC()
	{
	CCmdFileUser* self = new(ELeave) CCmdFileUser();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFileUser::~CCmdFileUser()
	{
	iFiles.Close();
	}

CCmdFileUser::CCmdFileUser()
	{
	}

const TDesC& CCmdFileUser::Name() const
	{
	_LIT(KName, "fuser");
	return KName;
	}

void CCmdFileUser::DoRunL()
	{
	const TInt numFiles = iFiles.Count();
	if (numFiles == 0)
		{
		PrintAllUsersL();
		}
	else
		{
		for (TInt i = 0; i < numFiles; ++i)
			{
			PrintUsersL(iFiles[i]);
			}
		}
	}

void CCmdFileUser::PrintAllUsersL()
	{
	TOpenFileScan openFileScanner(FsL());
	CFileList* fileList = NULL;
	FOREVER
		{
		openFileScanner.NextL(fileList);
		if (fileList == NULL)
			{
			break;
			}
		CleanupStack::PushL(fileList);
		const TInt numOpenFiles = fileList->Count();
		for (TInt i = 0; i < numOpenFiles; ++i)
			{
			const TEntry& entry = (*fileList)[i];
			Printf(_L("%S: \r\n"), &entry.iName);
			TThreadId threadId(openFileScanner.ThreadId());
			RThread thread;
			TInt err = thread.Open(threadId);
			if (err)
				{
				Printf(_L("\tError %d opening thread id %u\r\n"), err, (TUint)threadId);
				}
			else
				{
				TFullName name(thread.FullName());
				thread.Close();
				Printf(_L("\t%S (%u)\r\n"), &name, (TUint)threadId);
				}
			}
		CleanupStack::PopAndDestroy(fileList);
		}
	}

void CCmdFileUser::PrintUsersL(const TDesC& aFileName)
	{
	Printf(_L("%S: \r\n"), &aFileName);
	TOpenFileScan openFileScanner(FsL());
	CFileList* fileList = NULL;
	TBool userFound(EFalse);
	FOREVER
		{
		openFileScanner.NextL(fileList);
		if (fileList == NULL)
			{
			break;
			}
		CleanupStack::PushL(fileList);
		const TInt numOpenFiles = fileList->Count();
		for (TInt i = 0; i < numOpenFiles; ++i)
			{
			const TEntry& entry = (*fileList)[i];
			if (entry.iName.MatchF(aFileName.Mid(2)) != KErrNotFound) // Note entry.iName doesn't have a drive letter, hence the Mid(2).
				{
				userFound = ETrue;
				TThreadId threadId(openFileScanner.ThreadId());
				RThread thread;
				TInt err = thread.Open(threadId);
				if (err)
					{
					Printf(_L("\tError %d opening thread id %u\r\n"), err, (TUint)threadId);
					}
				else
					{
					TFullName name(thread.FullName());
					thread.Close();
					Printf(_L("\t%S (%u)\r\n"), &name, (TUint)threadId);
					}
				break;
				}
			}
		CleanupStack::PopAndDestroy(fileList);
		}

	if (!userFound)
		{
		Write(_L("\tNo users"));
		}
	}

void CCmdFileUser::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFiles, "file_name");
	aArguments.AppendFileNameL(iFiles, KArgFiles);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdFileUser)
#endif
