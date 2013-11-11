// symbolics.cpp
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
#include <fshell/bsym.h>
#include <fshell/ltkutils.h>
#include <fshell/stringhash.h>
#include <fshell/iocli.h>
#include "bsymtree.h"

using namespace LtkUtils;
using namespace IoUtils;

EXPORT_C CSymbolics::CSymbolics(RFs& aFs)
	: iFs(aFs)
	{
	}

EXPORT_C CSymbolics::~CSymbolics()
	{
	iBsyms.ResetAndDestroy();
	TStringHashIter<CMapFile*> iter(iCodeSegHash);
	while (iter.NextValue() != NULL)
		{
		CMapFile** file = iter.CurrentValue();
		delete *file;
		}
	iCodeSegHash.Close();
	delete iTabCompleteTree;
	delete iTabCompleteCodeseg;
	}

EXPORT_C void CSymbolics::AddBsymFileL(const TDesC& aFileName)
	{
	CBsymFile* file = CBsymFile::NewL(iFs, aFileName);
	TInt err = iBsyms.Append(file);
	if (err)
		{
		delete file;
		User::Leave(err);
		}
	}

EXPORT_C void CSymbolics::AddMapFileL(const TDesC& aFileName)
	{
	CMapFile* file = CMapFile::NewL(iFs, aFileName);
	CleanupStack::PushL(file);
	AddMapFileL(file);
	CleanupStack::Pop(file);
	}

EXPORT_C void CSymbolics::AddMapFileL(CMapFile* aMapFile)
	{
	aMapFile->GetFileNameL(iTempString);
	TParsePtrC parse(iTempString);
	TPtrC name = parse.Name(); // Don't use NameAndExt, that will include the .map!
	TPtr namePtr((TUint16*)name.Ptr(), name.Length(), name.Length()); // Nasty but TParsePtr won't give us a non-const Name().
	namePtr.LowerCase();
	User::LeaveIfError(iCodeSegHash.Insert(namePtr, aMapFile));
	}

EXPORT_C void CSymbolics::SetFallbackMapFileDirL(const TDesC& aDir)
	{
	iFallbackMapFileDir = aDir;
	}

EXPORT_C TPtrC CSymbolics::LookupL(TUint32 aRomAddress)
	{
	for (TInt i = 0; i < iBsyms.Count(); i++)
		{
		TPtrC res = iBsyms[i]->LookupL(aRomAddress);
		if (res.Length()) return res;
		}
	return TPtrC();
	}

EXPORT_C TPtrC CSymbolics::LookupL(const TDesC& aCodeseg, TUint32 aOffset)
	{
	CMapFile* mapFile = FindOrLoadMapFileL(aCodeseg);

	iTempString.Zero();
	if (mapFile)
		{
		mapFile->Lookup(aOffset, iTempString);
		if (iTempString.Length())
			{
			iTempString.AppendFormat(_L(" (%S)"), &aCodeseg);
			}
		}
	else
		{
		// Try BSYMs in case it's in ROFS
		for (TInt i = 0; i < iBsyms.Count(); i++)
			{
			TPtrC res = iBsyms[i]->LookupL(aCodeseg, aOffset);
			}
		}
	return TPtrC(iTempString);
	}

EXPORT_C void CSymbolics::CompleteL(const TDesC& aCodeseg, TDes& aSymbolName, CDesC16Array& aSuggestions)
	{
	RNode* tree = TreeForCodesegL(aCodeseg);
	if (tree)
		{
		tree->CompleteL(aSymbolName, aSuggestions);
		}
	}

RNode* CSymbolics::TreeForCodesegL(const TDesC& aCodeseg)
	{
	iTempString.Copy(aCodeseg);
	iTempString.LowerCase();
	if (iTabCompleteCodeseg && *iTabCompleteCodeseg != iTempString)
		{
		// If iTabCompleteCodeseg is set it means that iTabCompleteTree (if it's non-null) matches.
		delete iTabCompleteCodeseg;
		iTabCompleteCodeseg = NULL;
		delete iTabCompleteTree;
		iTabCompleteTree = NULL;
		}
	if (!iTabCompleteCodeseg) iTabCompleteCodeseg = iTempString.AllocL();

	if (iTabCompleteTree == NULL)
		{
		for (TInt i = 0; i < iBsyms.Count(); i++)
			{
			RNode* tree = iBsyms[i]->CreateCompletionTreeL(*iTabCompleteCodeseg);
			if (tree)
				{
				iTabCompleteTree = tree;
				break;
				}
			}
		}
	// Try the map files now
	if (iTabCompleteTree == NULL)
		{
		CMapFile* mapFile = FindOrLoadMapFileL(aCodeseg);
		if (mapFile)
			{
			iTabCompleteTree = mapFile->CreateCompletionTreeL();
			}
		}
	return iTabCompleteTree;
	}

EXPORT_C TUint32 CSymbolics::CodesegOffsetFromSymbolNameL(const TDesC& aCodeseg, const TDesC& aSymbolName)
	{
	RNode* tree = TreeForCodesegL(aCodeseg);
	if (!tree) User::Leave(KErrNotFound);
	return tree->ValueForStringL(aSymbolName);
	}

CMapFile* CSymbolics::FindOrLoadMapFileL(const TDesC& aCodeseg)
	{
	iTempString.Copy(aCodeseg);
	iTempString.LowerCase();

	// If it's already loaded, return it
	CMapFile* mapFile = iCodeSegHash.FindPtr(iTempString);
	if (mapFile) return mapFile;

	if (iFallbackMapFileDir.Length() == 0) return NULL;
	// Try loading the mapfile
	TFileName2 fn = iFallbackMapFileDir;
	fn.AppendComponentL(iTempString);
	fn.Append(_L(".map"));
	TEntry e;
	if (iFs.Entry(fn, e) == KErrNone)
		{
		TRAPD(err, AddMapFileL(fn));
		StaticLeaveIfErr(err, _L("Could not load map file %S"), &fn);
		// AddMapFileL overwrites iTempString, so restore it (nasty but I object to wasting buffers on the stack...)
		iTempString.Copy(aCodeseg);
		iTempString.LowerCase();
		return iCodeSegHash.FindPtr(iTempString);
		}
	return NULL;
	}
