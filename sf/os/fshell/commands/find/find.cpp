// find.cpp
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

class CCmdFind : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFind();
private:
	CCmdFind();
	void FoundFile(const TDesC& aDir, const TDesC& aName, TBool aIsDir);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TFileName2 iPath;
	HBufC* iName;
	TBool iPrint;
	TBool iAllDrives;
	TFileName2 iTempName;
	RPointerArray<HBufC> iSearchDirs;
	};


CCommandBase* CCmdFind::NewLC()
	{
	CCmdFind* self = new(ELeave) CCmdFind();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFind::~CCmdFind()
	{
	delete iName;
	iSearchDirs.ResetAndDestroy();
	}

CCmdFind::CCmdFind()
	{
	}

const TDesC& CCmdFind::Name() const
	{
	_LIT(KName, "find");
	return KName;
	}

void CCmdFind::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgPath, "path");
	aArguments.AppendFileNameL(iPath, KArgPath);
	}

void CCmdFind::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptName, "name");
	aOptions.AppendStringL(iName, KOptName);
	//aOptions.AppendBoolL(iPrint, TChar('p'), _L("print"), _L("Print the paths of files that match the given conditions, one per line. This is the default if no other options are specified."));
	}


EXE_BOILER_PLATE(CCmdFind)


void CCmdFind::DoRunL()
	{
	RFs& fs = FsL();
	iPath.SetIsDirectoryL();
	if (!iName)
		{
		LeaveIfErr(KErrArgument, _L("You must specify a name to match against"));
		}

	iSearchDirs.AppendL(iPath.AllocLC());
	CleanupStack::Pop();

	while (iSearchDirs.Count())
		{
		const TDesC& path = *iSearchDirs[0];

		TInt err;
		CDir* matchingFiles = NULL;
		iTempName.Copy(path);
		iTempName.AppendComponentL(*iName, TFileName2::EFile);
		// Look for files in this directory first
		err = fs.GetDir(iTempName, KEntryAttNormal|KEntryAttDir, ESortByName, matchingFiles);
		if (!err)
			{
			for (TInt i = 0; i < matchingFiles->Count(); i++)
				{
				const TEntry& entry = (*matchingFiles)[i];
				FoundFile(path, entry.iName, entry.IsDir());
				}
			}
		delete matchingFiles;

		// Then add all this dir's subdirectories to the list of ones to be scanned
		CDir* dirsToRecurse = NULL;
		err = fs.GetDir(path, KEntryAttDir|KEntryAttMatchExclusive, ESortNone, dirsToRecurse);
		if (!err)
			{
			CleanupStack::PushL(dirsToRecurse);
			for (TInt i = 0; i < dirsToRecurse->Count(); i++)
				{
				const TEntry& entry = (*dirsToRecurse)[i];
				iTempName.Copy(path);
				iTempName.AppendComponentL(entry);
				iSearchDirs.AppendL(iTempName.AllocLC());
				CleanupStack::Pop();
				}
			CleanupStack::PopAndDestroy(dirsToRecurse);
			}
		delete iSearchDirs[0];
		iSearchDirs.Remove(0);
		}
	}

void CCmdFind::FoundFile(const TDesC& aDir, const TDesC& aName, TBool aIsDir)
	{
	// For now, always print
	_LIT(KBack, "\\");
	Printf(_L("%S%S%S\n"), &aDir, &aName, aIsDir ? &KBack : &KNullDesC);
	}
