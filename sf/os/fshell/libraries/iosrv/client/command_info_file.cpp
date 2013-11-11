// command_info_file.cpp
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

#include <f32file.h>
#include "ioutils.h"
#include "command_base.h"

_LIT(KCifExt, ".cif");
_LIT(KCifPathVar, "CIF_PATH");
_LIT(KCifPath, "\\resource\\cif\\fshell");
_LIT(KEnumSeparator, ",");
_LIT(KEnumQuote, "\"");
_LIT(KNewLine, "\r\n");
_LIT(KPartialCommandStart, "==");
_LIT(KCommandStart, "\r\n==");
_LIT(KCmndName, "name");
_LIT(KCmndShortDescription, "short-description");
_LIT(KCmndLongDescription, "long-description");
_LIT(KCmndSeeAlso, "see-also");
_LIT(KCmndCopyright, "copyright");
_LIT(KCmndSmokeTest, "smoke-test");
_LIT(KCmndArgument, "argument");
_LIT(KCmndOption, "option");
_LIT(KCmndInclude, "include");
_LIT(KCmndSubCommand, "sub-command");
_LIT(KCmndEnumValue, "enum-value");
_LIT(KCmndFlagOptional, "optional");
_LIT(KCmndFlagMultiple, "multiple");
_LIT(KCmndFlagLast, "last");
_LIT(KTypeBool, "bool");
_LIT(KTypeInt, "int");
_LIT(KTypeUint, "uint");
_LIT(KTypeString, "string");
_LIT(KTypeFileName, "filename");
_LIT(KTypeEnum, "enum");
_LIT(KTypeReal, "real");
_LIT(KTypeUint64, "uint64");
_LIT(KTypeInt64, "int64");

using namespace IoUtils;

TInt SkipTo(TLex& aLex, const TDesC& aDes)
	{
	TInt ret = aLex.Remainder().Find(aDes);
	if (ret >= 0)
		{
		aLex.Inc(ret);
		return KErrNone;
		}
	return KErrNotFound;
	}

TInt SkipToEnd(TLex& aLex, const TDesC& aDes)
	{
	TInt ret = aLex.Remainder().Find(aDes);
	if (ret >= 0)
		{
		aLex.Inc(ret + aDes.Length());
		return KErrNone;
		}
	return KErrNotFound;
	}

TInt SkipToNextCommand(TLex& aLex)
	{
	if ((aLex.Offset() == 0) && (aLex.Remainder().Find(KPartialCommandStart) == 0))
		{
		return KErrNone;
		}

	return SkipTo(aLex, KCommandStart);
	}

TPtrC NextCommand(TLex& aLex)
	{
	TPtrC command;
	if (SkipToNextCommand(aLex) == KErrNone)
		{
		SkipToEnd(aLex, KPartialCommandStart);
		aLex.Mark();
		aLex.SkipCharacters();
		command.Set(aLex.MarkedToken());
		}
	else
		{
		aLex.Inc(aLex.Remainder().Length());
		}
	return command;
	}

TPtrC LineRemainder(TLex& aLex)
	{
	aLex.Mark();
	if (SkipTo(aLex, KNewLine) == KErrNone)
		{
		return aLex.MarkedToken();
		}
	else
		{
		return aLex.Remainder();
		}
	}

TPtrC NextWord(TLex& aLex)
	{
	aLex.SkipSpaceAndMark();
	aLex.SkipCharacters();
	TPtrC word(aLex.MarkedToken());
	return word;
	}

TPtrC TextToNextCommand(TLex& aLex)
	{
	aLex.Mark();

	if (SkipToNextCommand(aLex) != KErrNone)
		{
		aLex.Inc(aLex.Remainder().Length());
		}

	TLex lex(aLex.MarkedToken());
	
	// Strip leading white space.
	while (!lex.Eos())
		{
		if (lex.Peek().IsSpace())
			{
			lex.Get();
			}
		else
			{
			break;
			}
		}
	lex.Mark();
	TInt startOffset = lex.Offset();

	// Remove trailing white space.
	lex.Inc(lex.Remainder().Length());
	while (lex.Offset() > startOffset)
		{
		lex.UnGet();
		if (!lex.Peek().IsSpace())
			{
			lex.Get();
			break;
			}
		}

	return lex.MarkedToken();
	}

void AppendL(RBuf& aBuf, const TDesC& aDes, TBool aQuote)
	{
	const TInt requiredSpace = aDes.Length() + ((aBuf.Length() == 0) ? 0 : KEnumSeparator().Length()) + (aQuote ? 2 : 0);
	if ((aBuf.Length() + requiredSpace) > aBuf.MaxLength())
		{
		aBuf.ReAllocL(Max(aBuf.Length() * 2, aBuf.Length() + requiredSpace));
		}
	if (aBuf.Length() > 0)
		{
		aBuf.Append(KEnumSeparator);
		}
	if (aQuote)
		{
		aBuf.Append(KEnumQuote);
		}
	aBuf.Append(aDes);
	if (aQuote)
		{
		aBuf.Append(KEnumQuote);
		}
	}

void GetEnumValuesL(TLex& aLex, RBuf& aValues, RBuf& aDescriptions)
	{
	aValues.CreateL(0x100);
	aDescriptions.CreateL(0x100);

	while (!aLex.Eos())
		{
		TLexMark mark;
		aLex.Mark(mark);
		TPtrC command(NextCommand(aLex));
		if (command == KCmndEnumValue)
			{
			TPtrC value(NextWord(aLex));
			TPtrC description(TextToNextCommand(aLex));
			if (value.Length() == 0)
				{
				User::Leave(KErrArgument);
				}
			AppendL(aValues, value, EFalse);
			if (description.Length() > 0)
				{
				AppendL(aDescriptions, description, ETrue);
				}
			}
		else
			{
			aLex.UnGetToMark(mark);
			break;
			}
		}
	}


EXPORT_C CCommandInfoFile* CCommandInfoFile::NewL(RFs& aFs, const TDesC& aFileName)
	{
	CCommandInfoFile* self = new(ELeave) CCommandInfoFile(aFileName);
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCommandInfoFile* CCommandInfoFile::NewL(RFs& aFs, const CEnvironment& aEnvironment, const TDesC& aCommandName)
	{
	TFileName2 fileName;
	TRAPD(err, fileName.Copy(aEnvironment.GetAsDesL(KCifPathVar)));
	if (err) fileName.Copy(KCifPath);

	// Search the drive the command is running from first (rather than what the session path happens to be, which was the previous behaviour)
	if (!fileName.HasDriveLetter()) fileName.Insert(0, _L("?:"));
	fileName[0] = RProcess().FileName()[0];

	fileName.AppendComponentL(aCommandName);
	fileName.SetExtL(KCifExt);
	StaticLeaveIfErr(fileName.FindFile(aFs), _L("Couldn't find \"%S\""), &fileName);

	CCommandInfoFile* cif = CCommandInfoFile::NewL(aFs, fileName);
	if (cif->Name() != aCommandName)
		{
		CleanupStack::PushL(cif);
		StaticLeaveIfErr(KErrArgument, _L("Command named \"%S\" in implementation but \"%S\" in \"%S\""), &aCommandName, &cif->Name(), &fileName);
		}
	return cif;
	}

void CCommandInfoFile::ConstructL(RFs& aFs)
	{
	ReadFileL(aFs, iFileName);
	}

void CCommandInfoFile::ReadFileL(RFs& aFs, const TDesC& aFileName)
	{
	// Read the file into a buffer.
	RFile file;
	StaticLeaveIfErr(file.Open(aFs, aFileName, EFileRead | EFileShareReadersOnly), _L("Couldn't open '%S' for reading"), &aFileName);
	CleanupClosePushL(file);
	TInt fileSize;
	User::LeaveIfError(file.Size(fileSize));
	RBuf8 buf8;
	buf8.CreateL(fileSize);
	buf8.CleanupClosePushL();
	User::LeaveIfError(file.Read(buf8));
	RBuf& buf = NewBuffer();
	buf.CreateL(fileSize);
	buf.Copy(buf8);
	CleanupStack::PopAndDestroy(2, &file);
	TLex lex(buf);
	while (!lex.Eos())
		{
		if (iProcessInclude || !iCurrentChild)
			{
			ReadDetailsL(lex, aFs, aFileName);
			}
		else
			{
			iCurrentChild->ReadDetailsL(lex, aFs, aFileName);
			}
		}
	}

void CCommandInfoFile::ReadDetailsL(TLex& aLex, RFs& aFs, const TDesC& aFileName)
	{
	SkipToNextCommand(aLex); // Ignore everything before the first '==' command.

	while (!aLex.Eos())
		{
		TLexMark mark;
		aLex.Mark(mark);
		TPtrC command(NextCommand(aLex));
		if (command == KNullDesC)
			{
			// Do nothing - we're at the end of the string.
			}
		else if (command == KCmndName)
			{
			aLex.SkipSpaceAndMark();
			aLex.SkipCharacters();
			iName.Set(aLex.MarkedToken());
			}
		else if (command == KCmndShortDescription)
			{
			iShortDescription.Set(TextToNextCommand(aLex));
			}
		else if (command == KCmndLongDescription)
			{
			iLongDescription.Set(TextToNextCommand(aLex));
			}
		else if (command == KCmndSeeAlso)
			{
			iSeeAlso.Set(TextToNextCommand(aLex));
			}
		else if (command == KCmndCopyright)
			{
			iCopyright.Set(TextToNextCommand(aLex));
			}
		else if (command == KCmndSmokeTest)
			{
			// Hmm no easy way to get the line number we're currently on
			iSmokeTestLineNumber = 1;
			TLex lex(aLex);
			lex.Inc(-aLex.Offset()); // Only way to put a TLex back to the beginning!
			TPtrC preceding = lex.Remainder().Left(aLex.Offset());
			const TUint16* ptr = preceding.Ptr();
			const TUint16* end = ptr + preceding.Length();
			while (ptr != end)
				{
				if (*ptr++ == '\n') iSmokeTestLineNumber++;
				}
			// At this point iSmokeTestLineNumber points to the "==smoketest" line - add 2 to skip this line and the blank line below it
			iSmokeTestLineNumber += 2;
			iSmokeTest.Set(TextToNextCommand(aLex));
			}
		else if (command == KCmndArgument)
			{
			ReadArgumentL(aLex, aFileName);
			}
		else if (command == KCmndOption)
			{
			ReadOptionL(aLex, aFileName);
			}
		else if (command == KCmndInclude)
			{
			if (iParent == NULL)
				{
				iProcessInclude = EFalse;
				TLex lineLex(LineRemainder(aLex));
				TPtrC fileName(NextWord(lineLex));
				TFileName2* fullFileName = new(ELeave) TFileName2(aFileName);
				CleanupStack::PushL(fullFileName);
				fullFileName->SetNameAndExtL(fileName);
				ReadFileL(aFs, *fullFileName);
				CleanupStack::PopAndDestroy(fullFileName);
				break;
				}
			else
				{
				// We're a sub-command. Let control return to the root to handle the include.
				aLex.UnGetToMark(mark);
				iParent->ProcessInclude(*this);
				break;
				}
			}
		else if (command == KCmndSubCommand)
			{
			if (iParent == NULL)
				{
				TLex lineLex(LineRemainder(aLex));
				AddSubCommandL(lineLex, aLex, aFs, aFileName);
				}
			else
				{
				// We're a sub-command. Let control return to the root to handle the next sub-command.
				aLex.UnGetToMark(mark);
				iParent->ProcessNewChild();
				break;
				}
			}
		else
			{
			StaticLeaveIfErr(KErrArgument, _L("Unknown command \"%S\" in \"%S\""), &command, &aFileName);
			}
		}
	}

void CCommandInfoFile::ReadArgumentL(TLex& aLex, const TDesC& aFileName)
	{
	TLex lineLex(LineRemainder(aLex));
	TUint flags = 0;
	TBool multiple(EFalse);
	TPtrC argType(NextWord(lineLex));
	TPtrC argName(NextWord(lineLex));
	TPtrC envVar;
	while (!lineLex.Eos())
		{
		TPtrC word(NextWord(lineLex));
		if (word == KCmndFlagOptional)
			{
			flags |= KValueTypeFlagOptional;
			}
		else if (word == KCmndFlagMultiple)
			{
			multiple = ETrue;
			}
		else if (word == KCmndFlagLast)
			{
			flags |= KValueTypeFlagLast;
			}
		else
			{
			envVar.Set(word);
			}
		}

	if (argType.Length() == 0)
		{
		StaticLeaveIfErr(KErrArgument, _L("Argument missing type"));
		}
	else if (argName.Length() == 0)
		{
		StaticLeaveIfErr(KErrArgument, _L("Argument missing name"));
		}
	else
		{
		TPtrC argDescription = TextToNextCommand(aLex);
		if (argType == KTypeEnum)
			{
			RBuf& enumValues = NewBuffer();
			RBuf& enumDescriptions = NewBuffer();
			GetEnumValuesL(aLex, enumValues, enumDescriptions);
			if (multiple)
				{
				RArray<TInt> dummyArray;
				if (enumDescriptions.Length() > 0)
					{
					iArguments.AppendEnumL(dummyArray, argName, argDescription, enumValues, enumDescriptions, flags, envVar);
					}
				else
					{
					iArguments.AppendEnumL(dummyArray, argName, argDescription, enumValues, flags, envVar);
					}
				}
			else
				{
				TInt dummyInt;
				if (enumDescriptions.Length() > 0)
					{
					iArguments.AppendEnumL(dummyInt, argName, argDescription, enumValues, enumDescriptions, flags, envVar);
					}
				else
					{
					iArguments.AppendEnumL(dummyInt, argName, argDescription, enumValues, flags, envVar);
					}
				}
			}
		else if (argType == KTypeInt)
			{
			if (multiple)
				{
				RArray<TInt> dummyArray;
				iArguments.AppendIntL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TInt dummyInt;
				iArguments.AppendIntL(dummyInt, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeUint)
			{
			if (multiple)
				{
				RArray<TUint> dummyArray;
				iArguments.AppendUintL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TUint dummyUint;
				iArguments.AppendUintL(dummyUint, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeString)
			{
			if (multiple)
				{
				RPointerArray<HBufC> dummyArray;
				iArguments.AppendStringL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				HBufC* dummyHBufC;
				iArguments.AppendStringL(dummyHBufC, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeFileName)
			{
			if (multiple)
				{
				RArray<TFileName2> dummyArray;
				iArguments.AppendFileNameL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TFileName2 dummyFileName;
				iArguments.AppendFileNameL(dummyFileName, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeReal)
			{
			if (multiple)
				{
				RArray<TReal64> dummyArray;
				iArguments.AppendRealL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TReal64 dummyReal;
				iArguments.AppendRealL(dummyReal, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeUint64)
			{
			if (multiple)
				{
				RArray<TUint64> dummyArray;
				iArguments.AppendUintL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TUint64 dummyUint64;
				iArguments.AppendUintL(dummyUint64, argName, argDescription, flags, envVar);
				}
			}
		else if (argType == KTypeInt64)
			{
			if (multiple)
				{
				RArray<TInt64> dummyArray;
				iArguments.AppendIntL(dummyArray, argName, argDescription, flags, envVar);
				}
			else
				{
				TInt64 dummyInt64;
				iArguments.AppendIntL(dummyInt64, argName, argDescription, flags, envVar);
				}
			}
		else
			{
			StaticLeaveIfErr(KErrArgument, _L("Unknown argument type \"%S\" in \"%S\""), &argType, &aFileName);
			}
		}
	}

void CCommandInfoFile::ReadOptionL(TLex& aLex, const TDesC& aFileName)
	{
	TLex lineLex(LineRemainder(aLex));
	TBool multiple(EFalse);
	TPtrC optType(NextWord(lineLex));
	TPtrC optShortName(NextWord(lineLex));
	TPtrC optLongName(NextWord(lineLex));
	TPtrC envVar;
	while (!lineLex.Eos())
		{
		TPtrC word(NextWord(lineLex));
		if (word == KCmndFlagMultiple)
			{
			multiple = ETrue;
			}
		else
			{
			envVar.Set(word);
			}
		}

	if (optType.Length() == 0)
		{
		StaticLeaveIfErr(KErrArgument, _L("Option missing type"));
		}
	else if (optShortName.Length() == 0)
		{
		StaticLeaveIfErr(KErrArgument, _L("Option missing short name"));
		}
	else if (optShortName.Length() != 1)
		{
		StaticLeaveIfErr(KErrArgument, _L("Option short name not a single character"));
		}
	else if (optLongName.Length() == 0)
		{
		StaticLeaveIfErr(KErrArgument, _L("Option missing long name"));
		}
	else
		{
		TPtrC argDescription = TextToNextCommand(aLex);
		if (optType == KTypeEnum)
			{
			RBuf& enumValues = NewBuffer();
			RBuf& enumDescriptions = NewBuffer();
			GetEnumValuesL(aLex, enumValues, enumDescriptions);
			if (multiple)
				{
				RArray<TInt> dummyArray;
				if (enumDescriptions.Length() > 0)
					{
					iOptions.AppendEnumL(dummyArray, optShortName[0], optLongName, argDescription, enumValues, enumDescriptions, 0, envVar);
					}
				else
					{
					iOptions.AppendEnumL(dummyArray, optShortName[0], optLongName, argDescription, enumValues, 0, envVar);
					}
				}
			else
				{
				TInt dummyInt;
				if (enumDescriptions.Length() > 0)
					{
					iOptions.AppendEnumL(dummyInt, optShortName[0], optLongName, argDescription, enumValues, enumDescriptions, 0, envVar);
					}
				else
					{
					iOptions.AppendEnumL(dummyInt, optShortName[0], optLongName, argDescription, enumValues, 0, envVar);
					}
				}
			}
		else if (optType == KTypeBool)
			{
			if (multiple)
				{
				RArray<TBool> dummyArray;
				iOptions.AppendBoolL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TBool dummyBool;
				iOptions.AppendBoolL(dummyBool, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeInt)
			{
			if (multiple)
				{
				RArray<TInt> dummyArray;
				iOptions.AppendIntL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TInt dummyInt;
				iOptions.AppendIntL(dummyInt, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeUint)
			{
			if (multiple)
				{
				RArray<TUint> dummyArray;
				iOptions.AppendUintL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TUint dummyUint;
				iOptions.AppendUintL(dummyUint, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeString)
			{
			if (multiple)
				{
				RPointerArray<HBufC> dummyArray;
				iOptions.AppendStringL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				HBufC* dummyHBufC;
				iOptions.AppendStringL(dummyHBufC, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeFileName)
			{
			if (multiple)
				{
				RArray<TFileName2> dummyArray;
				iOptions.AppendFileNameL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TFileName2 dummyFileName;
				iOptions.AppendFileNameL(dummyFileName, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeReal)
			{
			if (multiple)
				{
				RArray<TReal64> dummyArray;
				iOptions.AppendRealL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TReal64 dummyReal;
				iOptions.AppendRealL(dummyReal, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeUint64)
			{
			if (multiple)
				{
				RArray<TUint64> dummyArray;
				iOptions.AppendUintL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TUint64 dummyUint64;
				iOptions.AppendUintL(dummyUint64, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else if (optType == KTypeInt64)
			{
			if (multiple)
				{
				RArray<TInt64> dummyArray;
				iOptions.AppendIntL(dummyArray, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			else
				{
				TInt64 dummyInt64;
				iOptions.AppendIntL(dummyInt64, optShortName[0], optLongName, argDescription, 0, envVar);
				}
			}
		else
			{
			StaticLeaveIfErr(KErrArgument, _L("Unknown option type \"%S\" in \"%S\""), &optType, &aFileName);
			}
		}
	}

void CCommandInfoFile::ProcessNewChild()
	{
	if (iParent)
		{
		iParent->ProcessNewChild();
		}
	else
		{
		ASSERT(!iProcessInclude);
		iCurrentChild = NULL;
		}
	}

void CCommandInfoFile::ProcessInclude(CCommandInfoFile& aChild)
	{
	if (iParent)
		{
		iParent->ProcessInclude(aChild);
		}
	else
		{
		iProcessInclude = ETrue;
		iCurrentChild = &aChild;
		}
	}

void CCommandInfoFile::AddSubCommandL(TLex& aNameLex, TLex& aDataLex, RFs& aFs, const TDesC& aFileName)
	{
	TPtrC subCommandName(NextWord(aNameLex));
	TBool found(EFalse);
	for (TInt i = (iChildren.Count() - 1); i >= 0; --i)
		{
		if (iChildren[i]->Name() == subCommandName)
			{
			iChildren[i]->AddSubCommandL(aNameLex, aDataLex, aFs, aFileName);
			found = ETrue;
			break;
			}
		}
	__ASSERT_ALWAYS(found || (aNameLex.Remainder().Length() == 0), IoUtils::Panic(ECifSubCommandParentNotFound));
	if (!found)
		{
		CCommandInfoFile* subCommand = new(ELeave) CCommandInfoFile(*this);
		CleanupStack::PushL(subCommand);
		subCommand->iName.Set(subCommandName);
		subCommand->ReadDetailsL(aDataLex, aFs, aFileName);
		iChildren.AppendL(subCommand);
		CleanupStack::Pop(subCommand);
		}
	}

RBuf& CCommandInfoFile::NewBuffer()
	{
	RBuf buf;
	iBufs.AppendL(buf);
	return iBufs[iBufs.Count() - 1];
	}

EXPORT_C CCommandInfoFile::~CCommandInfoFile()
	{
	iArguments.Close();
	iOptions.Close();

	const TInt numBufs = iBufs.Count();
	for (TInt i = 0; i < numBufs; ++i)
		{
		iBufs[i].Close();
		}
	iBufs.Close();
	iChildren.ResetAndDestroy();
	}

EXPORT_C const TDesC& CCommandInfoFile::Name() const
	{
	return iName;
	}

EXPORT_C const TDesC& CCommandInfoFile::ShortDescription() const
	{
	return iShortDescription;
	}

EXPORT_C const TDesC& CCommandInfoFile::LongDescription() const
	{
	return iLongDescription;
	}

EXPORT_C const TDesC& CCommandInfoFile::SeeAlso() const
	{
	return iSeeAlso;
	}

EXPORT_C const TDesC& CCommandInfoFile::Copyright() const
	{
	return iCopyright;
	}

EXPORT_C const TDesC& CCommandInfoFile::SmokeTest() const
	{
	return iSmokeTest;
	}

EXPORT_C TInt CCommandInfoFile::GetSmokeTestStartingLineNumber() const
	{
	return iSmokeTestLineNumber;
	}

EXPORT_C const RCommandArgumentList& CCommandInfoFile::Arguments()
	{
	return iArguments;
	}

EXPORT_C const RCommandOptionList& CCommandInfoFile::Options() const
	{
	return iOptions;
	}

template <class T> void CombineL(const T& aSourceList, T& aDestinationList, const TDesC& aListType, const TDesC& aCifName)
	{
	const TInt sourceCount = aSourceList.Count();
	const TInt destinationCount = aDestinationList.Count();

	if (sourceCount < destinationCount) // Note, allow the source to have more than the destination because some things are compile time configurable but CIF files don't support this.
		{
		StaticLeaveIfErr(KErrArgument, _L("%S count mismatch between \"%S\" (%d) and implementation (%d)"), &aListType, &aCifName, sourceCount, destinationCount);
		}

	for (TInt i = 0; i < destinationCount; ++i)
		{
		const TDesC& destinationName = aDestinationList[i].Name();
		TInt sourcePos = aSourceList.Find(destinationName);
		if (sourcePos < 0)
			{
			StaticLeaveIfErr(KErrArgument, _L("Couldn't find %S \"%S\" in \"%S\""), &aListType, &destinationName, &aCifName);
			}

		TUint sourceType = aSourceList[sourcePos].ValueType();
		TUint sourceTypeForComparison = sourceType & ~(KValueTypeFlagOptional|KValueTypeFlagLast); // destType can't know about these flags, so clear them from sourceType so we don't have problems when destType doesn't set them
		TUint destType = aDestinationList[i].ValueType();
		if (sourceTypeForComparison != destType)
			{
			StaticLeaveIfErr(KErrArgument, _L("Type mismatch for %S '%S' between \"%S\" (0x%x) and implementation (0x%x)"), &aListType, &destinationName, &aCifName, sourceType, destType);
			}
		aDestinationList[i].Combine(aSourceList[sourcePos]);
		}
	}

EXPORT_C void CCommandInfoFile::AssignL(RCommandArgumentList& aArguments, RCommandOptionList& aOptions) const
	{
	CombineL(iArguments, aArguments, _L("Argument"), iFileName);
	CombineL(iOptions, aOptions, _L("Option"), iFileName);
	}

EXPORT_C TInt CCommandInfoFile::NumSubCommands() const
	{
	return iChildren.Count();
	}

EXPORT_C const CCommandInfoFile& CCommandInfoFile::SubCommand(TInt aIndex) const
	{
	return *(iChildren[aIndex]);
	}

EXPORT_C const CCommandInfoFile* CCommandInfoFile::SubCommand(const TDesC& aSubCommandName) const
	{
	for (TInt i = (NumSubCommands() - 1); i >= 0; --i)
		{
		const CCommandInfoFile& thisSubCommand = SubCommand(i);
		if (thisSubCommand.Name() == aSubCommandName)
			{
			return &thisSubCommand;
			}
		}
	return NULL;
	}

CCommandInfoFile::CCommandInfoFile(const TDesC& aFileName)
	: iFileName(aFileName)
	{
	}

CCommandInfoFile::CCommandInfoFile(CCommandInfoFile& aParent)
	: iFileName(aParent.iFileName), iParent(&aParent)
	{
	}

EXPORT_C const TDesC& CCommandInfoFile::CifFileName() const
	{
	return iFileName;
	}
