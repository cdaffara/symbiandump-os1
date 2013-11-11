/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include "testconfigfileparser.h"
#include <f32file.h>



#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason/* aReason*/)	
//
// DLL entry point
//
	{
	return (KErrNone);
	}
#endif


EXPORT_C CTestConfig* CTestConfig::NewLC(RFs& aFs, const TDesC& aComponent, const TDesC& aScript)
	{
	CTestConfig* self = NewLC(aFs, aComponent);
	self->ReadScriptL(aScript);
	return self;
	}

EXPORT_C CTestConfig* CTestConfig::NewLC(RFs& aFs, const TDesC& aComponent)
	{
	CTestConfig* self = new (ELeave) CTestConfig(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aComponent);
	return self;
	}

void CTestConfig::ConstructL(const TDesC& aComponent)
	{
	iComponent = aComponent.AllocL();
	}

CTestConfig::CTestConfig(RFs& aFs)
: iFs(aFs)
	{
	}

EXPORT_C CTestConfig::~CTestConfig()
	{
	iSections.ResetAndDestroy();
	iSections.Close();
	delete iComponent;
	}

EXPORT_C const CTestConfigSection* CTestConfig::Section(const TDesC8& aSectionName) const
	{
	const CTestConfigSection* section = NULL;
	const TInt count = iSections.Count();
		
	for (TInt i = 0; i < count; i++) //order important
		{
		if (iSections[i]->SectionName().CompareF(aSectionName) == 0)
			{
			section = iSections[i];
			break;
			}
		}

	return section;
	}

EXPORT_C CTestConfigSection* CTestConfig::Section(const TDesC8& aSectionName)
	{
	CTestConfigSection* section = NULL;
	const TInt count = iSections.Count();
		
	for (TInt i = 0; i < count; i++) //order important
		{
		if (iSections[i]->SectionName().CompareF(aSectionName) == 0)
			{
			section = iSections[i];
			break;
			}
		}

	return section;
	}

EXPORT_C const TDesC8& CTestConfig::ItemValue(const TDesC8& aSection, const TDesC8& aItem, const TDesC8& aDefault) const
	{
	const CTestConfigSection* section = Section(aSection);

	if (section != NULL)
		return section->ItemValue(aItem, aDefault);

	return aDefault;
	}

EXPORT_C TInt CTestConfig::ItemValue(const TDesC8& aSection, const TDesC8& aItem, const TInt aDefault) const
	{
	TInt output = aDefault;
	const CTestConfigSection* section = Section(aSection);

	if (section != NULL)
		{
		output = section->ItemValue(aItem, aDefault);
		}

	return output;
	}




HBufC8* CTestConfig::ReadFileL(const TDesC& aFile) const
	{
	//Returns a HBufC8 with the contents of aFile
	RFile file;
	const TInt err = file.Open(iFs, aFile, EFileShareAny | EFileRead);

	if (err != KErrNone)
		{
		TParse fileOut;
		User::LeaveIfError(ResolveFile(iFs, *iComponent, aFile, fileOut));
		User::LeaveIfError(file.Open(iFs, fileOut.FullName(), EFileShareAny | EFileRead));
		}

	CleanupClosePushL(file);

	TInt size = 0;
	User::LeaveIfError(file.Size(size));

	HBufC8* contents = HBufC8::NewLC(size + 4);
	TPtr8 contentsPtr(contents->Des());
	User::LeaveIfError(file.Read(0, contentsPtr));

	CleanupStack::Pop(contents);
	CleanupStack::PopAndDestroy(&file);
	return contents;
	}

EXPORT_C TInt CTestConfig::ResolveFile(RFs& aFs, const TDesC& aComponent, const TDesC& aFileName, TParse& aParseOut)
	{
	TFileName* savedPath = new TFileName;
	TFileName* fileName = new TFileName;
	if ((savedPath == NULL) || (fileName == NULL))
		{
		delete savedPath;
		delete fileName;
		return KErrNoMemory;
		}

	fileName->Append(KScriptPathSep);
//	fileName->Append(KSmsTestFileInputBase);
//	fileName->Append(KScriptPathSep);
	fileName->Append(aComponent);
	fileName->Append(KScriptPathSep);
	fileName->Append(aFileName);
	
	// file finder will look in the session drive first, then Y->A,Z
	// so set session drive to Y (save old and restore it afterwards)
	aFs.SessionPath(*savedPath);
	_LIT(KTopDrive,"Y:\\");
	aFs.SetSessionPath(KTopDrive);
    TFindFile file_finder(aFs);
    TInt err = file_finder.FindByDir(*fileName,KNullDesC);
	if(err==KErrNone)
		aParseOut.Set(file_finder.File(),NULL,NULL);
	aFs.SetSessionPath(*savedPath);
	delete savedPath;
	delete fileName;
	return(err);
	}

EXPORT_C void CTestConfig::ReadScriptL(const TDesC& aScript)
	{
	iSections.ResetAndDestroy();

	CTestConfigSection* section = NULL;
	CTestConfigItem* currentItem = NULL;
	TInt currentItemStart = 0;
	CTestConfigSection* sectionDefaults = NULL;

	HBufC8* scriptContents = ReadFileL(aScript);
	CleanupStack::PushL(scriptContents);

	TLex8 input(*scriptContents);

	while (!input.Eos())
		{
		input.SkipSpaceAndMark();
		input.SkipCharacters();

		if ( input.TokenLength() == 0)    // if valid potential token
			{
			//end of the script file found
			break;
			}

		const TPtrC8 token(input.MarkedToken());

		if (token.CompareF(_L8("endscript")) == 0)
			{
			//end of the script file found
			break;
			}
		else if (IsNewSection(*scriptContents, input))
			{
			ParseAndSetItemValueL(*scriptContents, input, currentItemStart, currentItem);

			TInt mid = 1;
			TInt len = token.Length() - 2;

			const TPtrC8 sectionName(token.Mid(mid, len));

			if (sectionDefaults != NULL)
				section = CTestConfigSection::NewLC(sectionName, *sectionDefaults);
			else
				section = CTestConfigSection::NewLC(sectionName);

			if (sectionDefaults == NULL && IsDefaultSection(section->SectionName()))
				sectionDefaults = section;

			User::LeaveIfError(iSections.Append(section));
			CleanupStack::Pop(section);
			}
		else if (section != NULL)
			{
			TInt valueOffset;
			TPtrC8 newItem;

			if (IsNewComment(*scriptContents, input))
				{
				ParseAndSetItemValueL(*scriptContents, input, currentItemStart, currentItem);
				__ASSERT_DEBUG(currentItem == NULL, User::Invariant());
				SkipToNextLine(input);
				}
			else if (IsNewItem(*scriptContents, input, newItem, valueOffset))
				{
				ParseAndSetItemValueL(*scriptContents, input, currentItemStart, currentItem);
				currentItemStart = input.MarkedOffset() + valueOffset;
				currentItem = &section->AddItemL(newItem, KNullDesC8);
				}
			}
		}

	ParseAndSetItemValueL(*scriptContents, input, currentItemStart, currentItem);
	CleanupStack::PopAndDestroy(scriptContents);
	}

TBool CTestConfig::IsNewSection(const TDesC8& aSource, const TLex8& aInput) const
/**
 * Checks whether the current marked token in aInput starts with a '[' and ends with a ']',
 * and checks that this token is at the start of a line.
 *
 * @returns Whether this is a new section in the script file
 */
	{
	const TPtrC8 token(aInput.MarkedToken());
	const TInt offset(aInput.MarkedOffset());

	TBool ret = token.Length() > 2 && token.Find(KScriptSectionStart) == 0;
	ret = ret && token.Find(KScriptSectionEnd) != KErrNotFound;

	if (ret && offset > 0)
		{
		const TPtrC8 lastChar(aSource.Mid(offset-1, 1));
		ret = ret && (lastChar == KScriptLF || lastChar == KScriptCR);
		}

	return ret;
	}

void CTestConfig::SkipToNextLine(TLex8& aInput) const
	{
	const TChar cr('\n');

	while (!aInput.Eos() && aInput.Peek() != cr)
		{
		aInput.Inc();
		}
	}

TBool CTestConfig::IsNewItem(const TDesC8& aSource, const TLex8& aLex, TPtrC8& aItem, TInt& aStartOfVal) const
	{
	TBool ret(EFalse);

	if (IsAtStartOfNewLine(aSource, aLex, ETrue))
		{
		const TPtrC8 itemEnd(KScriptItemEnd);
		const TInt itemEndLen(itemEnd.Length());

		TPtrC8 token(aLex.MarkedToken());

		//First check to see if this token contains '='
		const TInt find = token.Find(itemEnd);
		if (find > 0)
			{
			aStartOfVal = find + itemEndLen;
			aItem.Set(token.Left(find));
			ret = ETrue;
			}
		else
			{
			aItem.Set(token);
			aStartOfVal = token.Length();

			const TPtrC8 remain(aLex.Remainder());
			TLex8 lex(remain);
			//Check that the next token starts with and '='
			lex.SkipSpaceAndMark();
			lex.SkipCharacters();
			token.Set(lex.MarkedToken());

			if (token.Find(itemEnd) == 0)
				{
				aStartOfVal += lex.MarkedOffset() + itemEndLen;
				ret = ETrue;
				}
			}
		}

	return ret;
	}

TBool CTestConfig::IsNewComment(const TDesC8& aSource, const TLex8& aLex) const
	{
	TBool ret(EFalse);

	const TPtrC8 token(aLex.MarkedToken());
	const TPtrC8 commentStart(KScriptCommentStart);
	const TInt commentStartLen(commentStart.Length());
	const TInt tokenLen(token.Length());

	if (commentStartLen <= tokenLen && token.Left(commentStartLen).Compare(commentStart) == 0)
		{
		ret = IsAtStartOfNewLine(aSource, aLex, ETrue);
		}

	return ret;
	}

TBool CTestConfig::IsAtStartOfNewLine(const TDesC8& aSource, const TLex8& aLex, TBool aIgnoreSpaces) const
	{
	TInt offset(aLex.MarkedOffset());
	__ASSERT_ALWAYS(offset != 0, User::Invariant());

	TChar ch = NULL;

	if (aIgnoreSpaces)
		{
		while (offset--)
			{
			ch = aSource[offset];
			if (ch == KScriptLFChar || ch == KScriptCRChar || !ch.IsSpace())
				break;
			}
		}
	else
		ch = aSource[offset-1];

	TBool ret(EFalse);

	if (offset <= 0)
		ret = ETrue;
	else
		ret = (ch == KScriptLFChar || ch == KScriptCRChar);

	return ret;
	}

TBool CTestConfig::IsDefaultSection(const TDesC8& aSectionName) const
	{
	TBool retVal = (aSectionName.CompareF(KScriptDefaults) == 0);
	retVal = retVal || (aSectionName.CompareF(KScriptDefault1) == 0);
	return retVal;
	}

TPtrC8 CTestConfig::ParseValue(const TDesC8& aText, const TLex8& aInput, TInt aCurrentItemStart) const
	{
	const TInt mid = aCurrentItemStart;
	const TInt len = aInput.MarkedOffset() - mid;
	TPtrC8 ret(KNullDesC8);

	if (len > 0)
		ret.Set(aText.Mid(mid, len));

	return ret;
	}

void CTestConfig::ParseAndSetItemValueL(const TDesC8& aText, const TLex8& aInput, TInt aCurrentItemStart, CTestConfigItem*& arCurrentItem)
	{
	if (arCurrentItem)
		{
		delete arCurrentItem->iValue;
		arCurrentItem->iValue = NULL;

		TPtrC8 val(ParseValue(aText, aInput, aCurrentItemStart));
		arCurrentItem->iValue = ReplaceLC(KScriptCRLF, KScriptLF, val);
		arCurrentItem->iValue->Des().Trim();
		CleanupStack::Pop(arCurrentItem->iValue);

		if (arCurrentItem->Item().CompareF(KScriptDefaults) == 0)
			{
			TName filename;
			filename.Copy(arCurrentItem->Value());
			CopyInDefaultsL(arCurrentItem->iParent, filename);
			}
		}

	arCurrentItem = NULL;
	}

void CTestConfig::CopyInDefaultsL(CTestConfigSection& aSection, const TDesC& aDefaultFile)
	{
	CTestConfig* file = CTestConfig::NewLC(iFs, aDefaultFile);

	TInt count = file->Sections().Count();

	if (count > 0)
		{
		const CTestConfigSection& def = (*file)[0];
		aSection.SetDefaultsL(def);
		}

	CleanupStack::PopAndDestroy(file);
	}

EXPORT_C TInt CTestConfig::CountElements(const TDesC8& aInput, TChar aDelimiter)
	{
	TInt pos = aInput.Length();
	TInt count = 1;

	while (pos--)
		{
		if (TChar(aInput[pos]) == aDelimiter)
			count++;
		}

	return count;
	}

EXPORT_C TInt CTestConfig::GetElement(const TDesC8& aInput, TChar aDelimiter, TInt aIndex, TInt& aOutput)
	{
	aOutput = 0;
	TPtrC8 string;
	TInt err = GetElement(aInput, aDelimiter, aIndex, string, ETrue);

	if (err == KErrNone)
		{
		TLex8 number(string);
		err = number.Val(aOutput);
		}

	return err;
	}

EXPORT_C TInt CTestConfig::GetElement(const TDesC8& aInput, TChar aDelimiter, TInt aIndex, TPtrC8& aOutput, TBool aTrimOutput)
	{
	TLex8 input(aInput);
	TInt err = KErrNone;
	TPtrC8 ptr;

	for (TInt i = 0; i <= aIndex && err == KErrNone; i++)
		{
		err = GetNextElement(input, aDelimiter, ptr);
		}

	if (err == KErrNone)
		{
		if (aTrimOutput)
			aOutput.Set(Trim(ptr));
		else
			aOutput.Set(ptr);
		}
	else 
		{
		const TInt count = CountElements(aInput, aDelimiter);
		const TInt len = aInput.Length();

		if (len != 0 && count - 1 == aIndex && TChar(aInput[len-1]) == aDelimiter)
			{
			aOutput.Set(KNullDesC8);
			err = KErrNone;
			}
		}

	return err;
	}

EXPORT_C TPtrC8 CTestConfig::Trim(const TDesC8& aInput)
	{
	const TPtrC8 ptr(TrimLeft(aInput));
	return TrimRight(ptr);
	}

EXPORT_C TPtrC8 CTestConfig::TrimRight(const TDesC8& aInput)
	{
	const TText8* first = aInput.Ptr();					// pointer to first char
	const TText8* last = first + aInput.Length() - 1;		// pointer to last char
	while (last >= first && TChar(*last).IsSpace()) last--;	// trim the right
	return TPtrC8(first, last + 1 - first);				// return the result
	}

EXPORT_C TPtrC8 CTestConfig::TrimLeft(const TDesC8& aInput)
	{
	const TText8* first = aInput.Ptr();					// pointer to first char
	const TText8* last = first + aInput.Length() - 1;		// pointer to last char
	while (first < last && TChar(*first).IsSpace()) first++;	// trim the left
	return TPtrC8(first, last + 1 - first);				// return the result
	}


TInt CTestConfig::GetNextElement(TLex8& aInput, TChar aDelimiter, TPtrC8& aOutput)
	{
	if (aInput.Eos())
		return KErrNotFound;

	//Get to the start of the descriptor
	while (!aInput.Eos() && aInput.Peek() != aDelimiter)
		aInput.Inc();

	aOutput.Set(aInput.MarkedToken());
	if (!aInput.Eos())
		aInput.SkipAndMark(1);

	return KErrNone;
	}

/*EXPORT_C void CTestConfig::ReplaceL(const TDesC8& aOld, const TDesC8& aNew, HBufC*& rString)
	{
	HBufC* repl = ReplaceLC(aOld, aNew, *rString);
	CleanupStack::Pop(repl);
	rString = repl;
	}*/

EXPORT_C HBufC8* CTestConfig::ReplaceLC(const TDesC8& aOld, const TDesC8& aNew, const TDesC8& aOldString)
	{
	HBufC8* rString = aOldString.AllocLC();
	TInt oldLen = aOld.Length();
	TInt newLen = aNew.Length();

	if (!oldLen)
		return rString;

	for (TInt pos = 0; pos < rString->Length(); pos += newLen)
		{
		TPtrC8 ptrC = rString->Mid(pos);
		TInt find = ptrC.Find(aOld);

		if (find == KErrNotFound)
			return rString;
	
		pos += find;

		if (newLen > oldLen)
			{
			rString = rString->ReAllocL(rString->Length() + newLen - oldLen);
			CleanupStack::Pop();
			CleanupStack::PushL(rString);
			}

		TPtr8 ptr(rString->Des());
		ptr.Replace(pos, oldLen, aNew);
		}

	return rString;
	}


EXPORT_C void CTestConfig::WriteFileL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Replace(iFs, aFileName, EFileShareAny | EFileWrite));
	CleanupClosePushL(file);

	const TInt count = iSections.Count();
	
	for (TInt i=0; i < count; i++)
		iSections[i]->WriteL(file);

	User::LeaveIfError(file.Flush());
	CleanupStack::PopAndDestroy();  //file
	}

EXPORT_C TBool CTestConfig::operator==(const CTestConfig& aFile) const
	{
	TInt count = iSections.Count();
	if (count != aFile.Sections().Count())
		return EFalse;

	TBool retVal = ETrue;

	while (count-- && retVal)
		{
		retVal = retVal && (*iSections[count] == aFile[count]);
		}

	return retVal;
	}
