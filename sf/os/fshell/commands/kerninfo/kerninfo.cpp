// kerninfo.cpp
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
#include <fshell/memoryaccesscmd.h>
#include <fshell/qr3dll.h>
#include <fshell/ltkutils.h>
#include <fshell/descriptorutils.h>
#ifdef FSHELL_SPCRE_SUPPORT
#include <fshell/pcre/cregex.h>
#endif

using namespace IoUtils;
class CRegEx;

class CCmdKerninfo : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdKerninfo();
private:
	CCmdKerninfo();
	TInt InfoL(TInt aIndex, TBool aUseId, TBool aVerbose);
	void PrintInfoForL(TInt aIndex, TBool aUseId=EFalse);
	static TBool SupportsIds(TKernelObjectType aType);
	static void ChompBlankLines(TDes& aDes);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TKernelObjectType iType; 
	TInt iObjectIndex;
	TBool iVerbose;
	TBool iVerboseMatch;
	HBufC* iMatch;
	HBufC* iRegexMatch;

	CKernelObjectList* iModel;
	RBuf iTitle, iInfo;
	LtkUtils::RLtkBuf8 iNarrowBuf;
	CRegEx* iRegex;
	};


CCommandBase* CCmdKerninfo::NewLC()
	{
	CCmdKerninfo* self = new(ELeave) CCmdKerninfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdKerninfo::~CCmdKerninfo()
	{
	delete iModel;
	iTitle.Close();
	iInfo.Close();
	iNarrowBuf.Close();
	delete iMatch;
	delete iRegexMatch;
#ifdef FSHELL_SPCRE_SUPPORT
	delete iRegex;
#endif
	}

CCmdKerninfo::CCmdKerninfo()
	{
	}

const TDesC& CCmdKerninfo::Name() const
	{
	_LIT(KName, "kerninfo");	
	return KName;
	}

void CCmdKerninfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iType, _L("object-type"));
	aArguments.AppendIntL(iObjectIndex, _L("object-index"));
	}

void CCmdKerninfo::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendStringL(iMatch, _L("match"));
#ifdef FSHELL_SPCRE_SUPPORT
	aOptions.AppendStringL(iRegexMatch, _L("regex-match"));
#endif
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendBoolL(iVerboseMatch, _L("verbosematch"));
	}

EXE_BOILER_PLATE(CCmdKerninfo)

void CCmdKerninfo::DoRunL()
	{
	LoadMemoryAccessL();

	iModel = CKernelObjectList::NewL(&iMemAccess);
	
	iModel->SetCurrentType(iType);
	TRAPL(iModel->RefreshDataL(), _L("Couldn't get model data"));

	iTitle.CreateL(256);
	iInfo.CreateL(256);

#ifdef FSHELL_SPCRE_SUPPORT
	if (iRegexMatch)
		{
		if (iMatch) LeaveIfErr(KErrArgument, _L("Can't specify both --match and --regex-match, use one or the other"));
		iRegex = CRegEx::NewL(*iRegexMatch, TRegExOptions(EPcreExtended|EPcreNewlineAny));
		}
#endif
	if (iVerboseMatch) iVerbose = ETrue; // verbosematch implies verbose

	const TInt n = iModel->Count();
	TBool listAll = !iArguments.IsPresent(&iObjectIndex);
	if (listAll)
		{
		for (TInt i = 0; i < n; i++)
			{
			PrintInfoForL(i);
			}
		}
	else
		{
		TBool usingId = SupportsIds(iType);
		if (!usingId && (iObjectIndex < 0 || iObjectIndex >= n))
			{
			LeaveIfErr(KErrArgument, _L("Bad argument %d. Number of objects in this category: %d"), iObjectIndex, n);
			}
		PrintInfoForL(iObjectIndex, usingId);
		}
	}

TInt CCmdKerninfo::InfoL(TInt aIndex, TBool aUseId, TBool aVerbose)
	{
	TInt err = KErrNone;
	TInt itemid = 0;
	if (aUseId)
		{
		TRAP(err, iModel->GetInfoByIdL(aIndex, aVerbose, iTitle, iInfo));
		}
	else
		{
		TRAP(err, itemid = iModel->GetInfoByIndexL(aIndex, aVerbose, iTitle, iInfo));
		}
	LeaveIfErr(err, _L("Couldn't get info for item %d"), aIndex);
	return itemid;
	}

void CCmdKerninfo::PrintInfoForL(TInt aIndex, TBool aUseId /* =EFalse */)
	{
	TInt itemid = 0;

	if (iMatch || iRegexMatch)
		{
		// Filter out anything not matching iMatch/iRegexMatch
		// Unless --verbosematch is specified, match on the non-verbose (but display the verbose if specified). This is because calculating the verbose can be really slow so don't do it until we know we match.
		itemid = InfoL(aIndex, aUseId, (TBool)iVerboseMatch);
		const TDesC& toMatchAgainst = iVerboseMatch ? iInfo : iTitle;

		TBool match = EFalse;
		if (iMatch)
			{
			match = (toMatchAgainst.MatchF(*iMatch) != KErrNotFound);
			}
#ifdef FSHELL_SPCRE_SUPPORT
		else if (iRegexMatch)
			{
			iNarrowBuf.Zero();
			iNarrowBuf.AppendL(toMatchAgainst);
			match = iRegex->PartialMatchL(iNarrowBuf);
			}
#endif
		if (!match) return;
		}
	
	if ((!iMatch && !iRegexMatch) || (iVerbose && !iVerboseMatch))
		{
		// Need to get info for first time (if !iMatch) or re-get the verbose info (in the case of we just did a match on the non-verbose info)
		itemid = InfoL(aIndex, aUseId, iVerbose);
		}

	TInt idxToDisplay = (SupportsIds(iType) && !aUseId) ? itemid : aIndex;
	if (iVerbose)
		{
		// In verbose mode the title is always just "Process info" (etc) so no point displaying it
		Printf(_L("%d:\n"), idxToDisplay);
		ChompBlankLines(iInfo); // When doing a verbose listing (specially multiple ones) it's easier to remove the blank lines that the model adds for readability in the QResources dialog
		Write(iInfo);
		Write(_L("\n\n"));
		}
	else
		{
		Printf(_L("%d: %S\t%S\n"), idxToDisplay, &iTitle, &iInfo);
		}
	}

TBool CCmdKerninfo::SupportsIds(TKernelObjectType aType)
	{
	// These types can (and should) be looked up directly by id and not by index into the list.
	return (aType == EListThread || aType == EListProcess || aType == EListHal);
	}

void CCmdKerninfo::ChompBlankLines(TDes& aDes)
	{
	_LIT(KLfLf, "\n\n");
	TInt found;
	while ((found = aDes.Find(KLfLf)) != KErrNotFound)
		{
		aDes.Delete(found, 1); // Nuke one of the newlines
		}
	}

