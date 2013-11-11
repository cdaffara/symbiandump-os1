// file_utils.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32base.h>
#include <f32file.h>
#include "ioutils.h"

using namespace IoUtils;


//
// Constants.
//

_LIT(KSlash, "\\");
_LIT(KDoubleSlash, "\\\\");
_LIT(KDot, ".");
_LIT(KDoubleDot, "..");

__ASSERT_COMPILE(sizeof(TFileName2) == sizeof(TFileName) + 4);

//
// TFileName2.
//

EXPORT_C TFileName2::TFileName2()
	{
	new(this) TFileName2(EFile);
	}

EXPORT_C TFileName2::TFileName2(TType aType)
	{
	new(this) TFileName2(KNullDesC, aType);
	}

EXPORT_C TFileName2::TFileName2(const TDesC& aName)
	: TBuf<256>(aName), iType(EFile)
	{
	new(this) TFileName2(aName, EFile);
	}

EXPORT_C TFileName2::TFileName2(const TDesC& aName, TType aType)
	: TBuf<256>(aName), iType(aType)
	{
	// Remove any double slashes that may be present.
	TInt pos;
	while ((pos = Find(KDoubleSlash)) >= 0)
		{
		Delete(pos, 1);
		}
	}

EXPORT_C TBool TFileName2::IsAbsolute() const
	{
	return ((Length() >= 3) && TChar((*this)[0]).IsAlpha() && ((*this)[1] == ':') && ((*this)[2] == '\\'));
	}

EXPORT_C TBool TFileName2::IsDriveLetter() const
	{
	return (Length() == 2) && TChar((*this)[0]).IsAlpha() && ((*this)[1] == ':');
	}

EXPORT_C TBool TFileName2::IsDriveRoot() const
	{
	return ((Length() == 3) && TChar((*this)[0]).IsAlpha() && ((*this)[1] == ':') && ((*this)[2] == '\\'));
	}

EXPORT_C TBool TFileName2::IsWild() const
	{
	return TParsePtrC(*this).IsWild();
	}

EXPORT_C TBool TFileName2::IsFile() const
	{
	TBool ret = (iType == EFile);
	ASSERT((ret && !HasTrailingSlash()) || (!ret && HasTrailingSlash()));
	return ret;
	}

EXPORT_C TBool TFileName2::IsDir() const
	{
	TBool ret = (iType == EDirectory);
	ASSERT((ret && HasTrailingSlash()) || (!ret && !HasTrailingSlash()));
	return ret;
	}

EXPORT_C TBool TFileName2::HasDriveLetter() const
	{
	return ((Length() >= 2) && TChar((*this)[0]).IsAlpha() && ((*this)[1] == ':'));
	}

EXPORT_C TBool TFileName2::HasLeadingSlash() const
	{
	return ((Length() >= 1) && ((*this)[0] == '\\'));
	}

EXPORT_C TBool TFileName2::HasTrailingSlash() const
	{
	return ((Length() >= 1) && ((*this)[Length() - 1] == '\\'));
	}

EXPORT_C TPtrC TFileName2::Drive() const
	{
	return TParsePtrC(*this).Drive();
	}

EXPORT_C TPtrC TFileName2::Path() const
	{
	return TParsePtrC(*this).Path();
	}

EXPORT_C TPtrC TFileName2::DriveAndPath() const
	{
	return TParsePtrC(*this).DriveAndPath();
	}

EXPORT_C TPtrC TFileName2::Name() const
	{
	return TParsePtrC(*this).Name();
	}

EXPORT_C TPtrC TFileName2::Ext() const
	{
	return TParsePtrC(*this).Ext();
	}

EXPORT_C TPtrC TFileName2::NameAndExt() const
	{
	return TParsePtrC(*this).NameAndExt();
	}

EXPORT_C void TFileName2::SetL(const TFileName2& aName)
	{
	Zero();
	AppendComponentL(aName);
	}

EXPORT_C void TFileName2::SetTypeL(TType aType)
	{
	switch (aType)
		{
		case EFile:
			{
			if (HasTrailingSlash() && ((HasDriveLetter() && (Length() > 3)) || (!HasDriveLetter() && (Length() > 1))))
				{
				Delete(Length() - 1, 1);
				}
			iType = EFile;
			break;
			}
		case EDirectory:
			{
			if (!HasTrailingSlash())
				{
				AppendComponentL(KNullDesC());
				}
			iType = EDirectory;
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		}
	}

EXPORT_C void TFileName2::SetTypeL(RFs& aFs)
	{
	if (IsDirL(aFs))
		{
		SetTypeL(EDirectory);
		}
	else
		{
		SetTypeL(EFile);
		}
	}

EXPORT_C void TFileName2::SetDriveL(const TDesC& aDrive)
	{
	if (Drive().Length() > 0)
		{
		(*this)[0] = aDrive[0];
		}
	else
		{
		PrependL(aDrive);
		}
	}

EXPORT_C void TFileName2::SetPathL(const TDesC& aPath)
	{
	TFileName2 temp;
	temp.SetDriveL(Drive());
	temp.AppendComponentL(aPath);
	temp.SetNameAndExtL(NameAndExt());
	Copy(temp);
	}

EXPORT_C void TFileName2::SetNameL(const TDesC& aName)
	{
	TPtrC namePtrC(Name());
	if (namePtrC.Length() > 0)
		{
		CheckSpaceL(aName.Length() - namePtrC.Length());
		TInt namePos = namePtrC.Ptr() - Ptr();
		Delete(namePos, namePtrC.Length());
		Insert(namePos, aName);
		}
	else
		{
		AppendComponentL(aName);
		}
	}

EXPORT_C void TFileName2::SetExtL(const TDesC& aExt)
	{
	TPtrC extPtrC(Ext());
	if (extPtrC.Length() > 0)
		{
		CheckSpaceL(aExt.Length() + ((aExt[0] == '.') ? 0 : 1) - extPtrC.Length());
		Delete(extPtrC.Ptr() - Ptr(), extPtrC.Length());
		}
	else
		{
		CheckSpaceL(aExt.Length() + ((aExt[0] == '.') ? 0 : 1));
		}

	if (aExt[0] != '.')
		{
		Append('.');
		}
	Append(aExt);
	}

EXPORT_C void TFileName2::SetNameAndExtL(const TDesC& aNameAndExt)
	{
	TPtrC nameAndExtPtrC(NameAndExt());
	if (nameAndExtPtrC.Length() > 0)
		{
		CheckSpaceL(aNameAndExt.Length() - nameAndExtPtrC.Length());
		TInt nameAndExtPos = nameAndExtPtrC.Ptr() - Ptr();
		Delete(nameAndExtPos, nameAndExtPtrC.Length());
		Insert(nameAndExtPos, aNameAndExt);
		}
	else
		{
		AppendComponentL(aNameAndExt);
		}
	}

EXPORT_C void TFileName2::PrependL(const TFileName2& aName)
	{
	if (Length() == 0)
		{
		CheckSpaceL(aName.Length() + (((iType == EDirectory) && !HasTrailingSlash()) ? 1 : 0));
		Append(aName);
		}
	else if (aName.HasTrailingSlash() && HasLeadingSlash())
		{
		CheckSpaceL(aName.Length() - 1 + (((iType == EDirectory) && !HasTrailingSlash()) ? 1 : 0));
		Insert(0, aName.Mid(1));
		}
	else if (!aName.HasTrailingSlash() && !HasLeadingSlash())
		{
		CheckSpaceL(aName.Length() + 1 + (((iType == EDirectory) && !HasTrailingSlash()) ? 1 : 0));
		Insert(0, KSlash);
		Insert(0, aName);
		}
	else
		{
		CheckSpaceL(aName.Length() + (((iType == EDirectory) && !HasTrailingSlash()) ? 1 : 0));
		Insert(0, aName);
		}

	if ((iType == EDirectory) && (!HasTrailingSlash()))
		{
		Append(KSlash);
		}
	}

EXPORT_C void TFileName2::AppendComponentL(const TFileName2& aName)
	{
	AppendComponentL(aName, iType);
	}

EXPORT_C void TFileName2::AppendComponentL(const TDesC& aPathComponent, TFileName2::TType aType)
	{
	// This function doesn't use iType, therefore you can call it and get the same results regardless of whether the type has been set - you will only get a trailing slash if (a) aPathComponent ends in a slash, or (b) aType is EDirectory.
	TPtrC pathWithoutLeadingSlash(aPathComponent);
	if (aPathComponent.Length() && aPathComponent[0] == '\\') pathWithoutLeadingSlash.Set(aPathComponent.Mid(1));
	TInt pathLen = pathWithoutLeadingSlash.Length();
	const TDesC* middle = &KNullDesC;
	if (Length() && !HasTrailingSlash()) middle = &KSlash;
	const TDesC* end = &KNullDesC;
	if (aType == EDirectory && pathLen && pathWithoutLeadingSlash[pathLen-1] != '\\') end = &KSlash;
	CheckSpaceL(pathLen + middle->Length() + end->Length());

	Append(*middle);
	Append(pathWithoutLeadingSlash);
	Append(*end);
	}

EXPORT_C void TFileName2::AppendComponentL(const TEntry& aEntry)
	{
	AppendComponentL(aEntry.iName, aEntry.IsDir() ? EDirectory : EFile);
	}

EXPORT_C void TFileName2::SetIsDirectoryL()
	{
	// This is the more complicated way to do it, the other would be: if (!HasTrailingSlash()) AppendL(KSlash);
	// Or indeed, AppendComponentL(KNullDesC, EDirectory) since the type is irrelevant if the component is empty
	AppendComponentL(KNullDesC, EFile);
	}

void TFileName2::CheckSpaceL(TInt aLengthToAdd) const
	{
	if ((MaxLength() - Length()) < aLengthToAdd)
		{
		User::Leave(KErrTooBig);
		}
	}

EXPORT_C void TFileName2::SplitL(RArray<TFileName2>& aOutput)
	{
	TLex lex(*this);
	while (!lex.Eos())
		{
		lex.Mark();
		while (!lex.Eos() && (lex.Peek() != '\\'))
			{
			lex.Get();
			}
		User::LeaveIfError(aOutput.Append(TFileName2(lex.MarkedToken())));
		if (!lex.Eos())
			{
			// Skip over the slash
			lex.Get();
			if (lex.Eos())
				{
				// ended with slash - insert empty path component
				User::LeaveIfError(aOutput.Append(TFileName2()));
				}
			}
		}
	}

EXPORT_C void TFileName2::MakeAbsoluteL(const TFileName2& aAbsoluteTo)
	{
	ASSERT(aAbsoluteTo.IsAbsolute());

	if (IsAbsolute())
		{
		return;
		}

	if (HasLeadingSlash())
		{
		// Just prepend drive letter.
		PrependL(aAbsoluteTo.Left(2));
		return;
		}

	PrependL(aAbsoluteTo);
	RArray<TFileName2> splitName;
	CleanupClosePushL(splitName);
	SplitL(splitName);
	TInt i = 0;
	while (i < splitName.Count())
		{
		const TFileName2& piece = splitName[i];
		if (piece == KDot)
			{
			splitName.Remove(i);
			}
		else if (piece == KDoubleDot)
			{
			if ((i == 0) || ((i == 1) && splitName[0].IsDriveLetter()))
				{
				User::Leave(KErrBadName);
				}
			splitName.Remove(i);
			splitName.Remove(i - 1);
			--i;
			}
		else if ((piece == KNullDesC) && (i != 0) && (i != splitName.Count()-1))
			{
			// Allow empty piece in first or last slot to support leading/trailing slashes.
			splitName.Remove(i);
			}
		else
			{
			++i;
			}
		}

	UnsplitL(splitName);
	CleanupStack::PopAndDestroy(&splitName);
	}

EXPORT_C void TFileName2::MakeAbsoluteL(RFs& aFs)
	{
	TFileName2 sessionPath;
	User::LeaveIfError(aFs.SessionPath(sessionPath));
	MakeAbsoluteL(sessionPath);
	}

EXPORT_C void TFileName2::UnsplitL(const RArray<TFileName2>& aInput)
	{
	Zero();
	const TInt count = aInput.Count();
	for (TInt i = 0; i < count; ++i)
		{
		AppendComponentL(aInput[i]);
		}
	if ((count == 1) && (aInput[0].Match(_L("?:")) == 0))
		{
		Append(KSlash);
		}
	}

EXPORT_C TBool TFileName2::Exists(RFs& aFs) const
	{
	TInt err;
	if (IsDriveRoot())
		{
		TInt driveNumber;
		err = RFs::CharToDrive(Drive()[0], driveNumber);
		if (err == KErrNone)
			{
			TDriveInfo driveInfo;
			err = aFs.Drive(driveInfo, driveNumber);
			}
		}
	else
		{
		TUint att;
		err = aFs.Att(*this, att);
		}
	return (err == KErrNone);
	}

EXPORT_C TBool TFileName2::IsReadOnlyL(RFs& aFs) const
	{
	TEntry entry;
	User::LeaveIfError(aFs.Entry(*this, entry));
	return entry.IsReadOnly();
	}

EXPORT_C TBool TFileName2::IsHiddenL(RFs& aFs) const
	{
	TEntry entry;
	User::LeaveIfError(aFs.Entry(*this, entry));
	return entry.IsHidden();
	}

EXPORT_C TBool TFileName2::IsSystemL(RFs& aFs) const
	{
	TEntry entry;
	User::LeaveIfError(aFs.Entry(*this, entry));
	return entry.IsSystem();
	}

EXPORT_C TBool TFileName2::IsDirL(RFs& aFs) const
	{
	if (IsDriveRoot())
		{
		if (!Exists(aFs))
			{
			User::Leave(KErrNotFound);
			}
		return ETrue;
		}
	else
		{
		TEntry entry;
		User::LeaveIfError(aFs.Entry(*this, entry));
		return entry.IsDir();
		}
	}

EXPORT_C TBool TFileName2::IsArchiveL(RFs& aFs) const
	{
	TEntry entry;
	User::LeaveIfError(aFs.Entry(*this, entry));
	return entry.IsArchive();
	}

EXPORT_C TInt TFileName2::FindFile(RFs& aFs)
	{
	TFindFile ff(aFs);
	TInt err = ff.FindByDir(*this, KNullDesC);
	if (err==KErrNone)
		{
		*this = ff.File();
		}
	return err;
	}
	
EXPORT_C void TFileName2::FindFileL(RFs& aFs)
	{
	User::LeaveIfError(FindFile(aFs));
	}

EXPORT_C void TFileName2::Normalize(RFs& aFs)
	{
	// Change this so that the case of each component matches what the file system reckons. ie C:\SYS\Bin would be changed to c:\sys\bin
	ASSERT(IsAbsolute());
	TInt err = KErrNone;
	TEntry entry;
	TParse parse;
	parse.Set(*this, NULL, NULL);

	while (!err)
		{
		err = aFs.Entry(parse.FullName(), entry);
		if (!err) err = parse.PopDir();
		if (!err)
			{
			Replace(parse.FullName().Length(), entry.iName.Length(), entry.iName);
			}
		}
	}
