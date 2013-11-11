// command_base.cpp
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

#include "iocons.h"
#include "ioutils.h"
#include "command_base.h"
#include <fshell/ltkutils.h>
#include <fshell/line_editor.h> // for definition of CTRL()
using namespace IoUtils;

#define RETURN_IF_ERROR(x) {TInt _err = (x); if (_err<0) return _err;}

//
// Constants.
//

_LIT(KHelpName, "help");
_LIT(KHelpDescription, "Display help.");
_LIT(KOptConsole, "console");
_LIT(KOptConsoleTitle, "console-title");
_LIT(KOptConsoleSize, "console-size");
_LIT(KOptPersistentConsole, "persistent-console");
_LIT(KDllExt, ".dll");
_LIT(KNewLine, "\r\n");

const TUint KValueTypeUninitialized		= 0x0000ffff;
const TUint KValueTypeFlagMultiple 		= 0x00010000;
const TUint KValueTypeFlagMask     		= 0x0000ffff;
const TUint KPublicFlagsMask			= 0x0000ffff;
const TUint KPrivateFlagsMask			= 0xffff0000;


//
// Misc.
//

void IoUtils::Panic(TCmdBasePanic aReason)
	{
	_LIT(KCategory, "iocli");
	User::Panic(KCategory, aReason);
	}

// If any of these asserts fail, it constitutes a BC break
__ASSERT_COMPILE(sizeof(RChildProcess) == 7*4);
__ASSERT_COMPILE(sizeof(RCommandOptionList) == 8*4);
__ASSERT_COMPILE(sizeof(RCommandArgumentList) == 8*4);
__ASSERT_COMPILE(sizeof(RIoConsoleReadHandle) == 6*4);
__ASSERT_COMPILE(sizeof(RIoReadHandle) == 2*4);
__ASSERT_COMPILE(sizeof(RIoConsoleWriteHandle) == 2*4);

// The member variables of CCommandBase actually sum to 59 words, and from subclasses' point of view it's 59 words,
// but EABI rounds the sizeof up to an 8-byte boundary. We can't change CCommandBase to be 60 words so that it's
// handled the same on all platforms, because that in itself would be a BC break.
#ifdef __EABI__
__ASSERT_COMPILE(sizeof(CCommandBase) == 60*4);
#else
__ASSERT_COMPILE(sizeof(CCommandBase) == 59*4);
#endif

//
// TEnumLex.
//
	
class TEnumLex
	{
public:
	TEnumLex(const TDesC& aCommaSeparatedList);
	TBool More();
	TPtrC Next();
private:
	TPtrC iRemainder;
	};

TEnumLex::TEnumLex(const TDesC& aCommaSeparatedList)
	: iRemainder(aCommaSeparatedList)
	{
	}
		

TBool TEnumLex::More()
	{
	return iRemainder.Length() > 0;
	}
	
_LIT(KComma, ",");

TPtrC TEnumLex::Next()
	{
	TInt commaPos = iRemainder.Find(KComma);
	if (commaPos<0) commaPos = iRemainder.Length();
	TPtrC value(iRemainder.Mid(0, commaPos));
	iRemainder.Set(iRemainder.Mid(commaPos));
	if (iRemainder.Length()) iRemainder.Set(iRemainder.Mid(1));
	return value;
	}


//
// TEnum.
//

EXPORT_C TEnum::TEnum()
	{
	}

EXPORT_C TEnum::TEnum(const TDesC& aEnumValueList)
	: iEnumValueList(aEnumValueList)
	{
	ValidateValues();
	}

EXPORT_C TEnum::TEnum(const TDesC& aEnumValueList, const TDesC& aEnumDescriptionList)
	: iEnumValueList(aEnumValueList), iEnumDescriptionList(aEnumDescriptionList)
	{
	ValidateValues();
	}
	
TInt TEnum::GetMatches(const TDesC& aString, RArray<TInt>& aMatches, TInt& aExactMatch)
	{
	TEnumLex lex(iEnumValueList);
	TInt i = 0;
	aExactMatch = KErrNotFound;
	while ((lex.More()) && (aExactMatch == KErrNotFound))
		{
		TPtrC value(lex.Next());
		if (value.Compare(aString)==0)
			{
			aExactMatch = i;
			}
		if (value.Find(aString)==0)
			{
			TInt err = aMatches.Append(i);
			if (err) return err;
			}
		++i;
		}
	return KErrNone;
	}
	
EXPORT_C TInt TEnum::Parse(const TDesC& aString, TInt& aValue)
	{
	RArray<TInt> matches;
	TInt exactMatch;
	TInt err = GetMatches(aString, matches, exactMatch);
	if (err==KErrNone)
		{
		if (exactMatch != KErrNotFound)
			{
			aValue = exactMatch;
			}
		else if (matches.Count()==1)
			{
			aValue = matches[0];
			}
		else
			{
			err = KErrArgument;
			}
		}
	matches.Close();
	return err;	
	}

EXPORT_C TInt TEnum::ParseL(const TDesC& aString)
	{
	RArray<TInt> matches;
	TInt exactMatch;
	CleanupClosePushL(matches);
	User::LeaveIfError(GetMatches(aString, matches, exactMatch));
	
	TInt value = 0;
	if ((matches.Count()==1) || (exactMatch != KErrNotFound))
		{
		value = exactMatch != KErrNotFound ? exactMatch : matches[0];
		}
	else if (matches.Count()==0)
		{
		if (CCommandBase::HaveStatic())
			{
			CTextBuffer* buf = CTextBuffer::NewLC(0x20);
			buf->AppendL(_L("String '"));
			buf->AppendL(aString);
			buf->AppendL(_L("' does not match any of "));
			AppendValuesL(*buf);
			PrintError(KErrArgument, buf->Descriptor());
			}
		User::Leave(KErrArgument);
		}
	else
		{
		if (CCommandBase::HaveStatic())
			{
			CTextBuffer* buf = CTextBuffer::NewLC(0x20);
			buf->AppendL(_L("String '"));
			buf->AppendL(aString);
			buf->AppendL(_L("' does not match a unique value (matches "));
			TInt enumCount = -1;
			TEnumLex lex(iEnumValueList);
			// here we rely on the fact the matches will be sorted, due to the way the array was populated above.
			for (TInt i=0; i<matches.Count(); ++i)
				{
				TPtrC enumValue;
				while (matches[i]>enumCount)
					{
					++enumCount;
					enumValue.Set(lex.Next());
					}
				buf->AppendL(enumValue);
				if (i<matches.Count()-1) buf->AppendL(_L(", "));
				}
			buf->AppendL(')');
			PrintError(KErrArgument, buf->Descriptor());
			}
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy(); //matches
	return value;
	}
	
EXPORT_C const TDesC& TEnum::ValueList() const
	{
	return iEnumValueList;
	}

EXPORT_C const TDesC& TEnum::DescriptionList() const
	{
	return iEnumDescriptionList;
	}
	
void TEnum::ValidateValues() const
	{
	for (TInt i=0; i<iEnumValueList.Length(); ++i)
		{
		__ASSERT_ALWAYS(!TChar(iEnumValueList[i]).IsSpace(), IoUtils::Panic(EEnumValueContainsSpace));
		}
	}

EXPORT_C void TEnum::AppendValuesL(CTextBuffer& aBuf) const
	{
	TEnumLex values(iEnumValueList);
	while (values.More())
		{
		aBuf.AppendL(values.Next());
		if (values.More())
			{
			aBuf.AppendL(_L(", "));
			}
		}
	}
	
EXPORT_C const TPtrC TEnum::GetString(TInt aIndex) const
	{
	TEnumLex lex(iEnumValueList);
	while (aIndex)
		{
		lex.Next();
		--aIndex;
		__ASSERT_ALWAYS(lex.More(), Panic(EEnumValueOutOfRange));
		}
	return lex.Next();
	}

EXPORT_C void TEnum::SetValueList(const TDesC& aValueList)
	{
	__ASSERT_ALWAYS(iEnumValueList.Length() == 0, IoUtils::Panic(EEnumValueListAlreadySet));
	iEnumValueList.Set(aValueList);
	}

EXPORT_C void TEnum::SetDescriptionList(const TDesC& aDescriptionList)
	{
	__ASSERT_ALWAYS(iEnumDescriptionList.Length() == 0, IoUtils::Panic(EEnumDescriptionListAlreadySet));
	iEnumDescriptionList.Set(aDescriptionList);
	}

EXPORT_C void TEnum::operator=(const TEnum& aEnum)
	{
	iEnumValueList.Set(aEnum.iEnumValueList);
	iEnumDescriptionList.Set(aEnum.iEnumDescriptionList);
	}


//
// TValue.
//

TValue::TValue(TUint aValueType, void* aValuePointer, const TDesC& aName)
	: iValueType(aValueType), iValue(aValuePointer), iIsSet(EFalse), iEnum(KNullDesC, KNullDesC), iName(aName)
	{
	}

TValue::TValue(TUint aValueType, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar)
	: iValueType(aValueType), iValue(aValuePointer), iIsSet(EFalse), iEnvVar(aEnvVar), iEnum(KNullDesC, KNullDesC), iName(aName), iDescription(aDescription)
	{
	__ASSERT_ALWAYS(aValueType != KValueTypeEnum, IoUtils::Panic(EInvalidUseOfEnum));
	}
	
TValue::TValue(TUint aValueType, const TDesC& aEnumValueList, const TDesC& aEnumDescriptionList, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar)
	: iValueType(aValueType), iValue(aValuePointer), iIsSet(EFalse), iEnvVar(aEnvVar), iEnum(aEnumValueList, aEnumDescriptionList), iName(aName), iDescription(aDescription)
	{
	}

TUint TValue::ValueType() const
	{
	return iValueType;
	}

EXPORT_C TUint TValue::Type() const
	{
	return iValueType & KPublicFlagsMask;
	}

void TValue::SetValue()
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeBool, IoUtils::Panic(EIncorrectCommandOptionType1));
	if (iValueType & KValueTypeFlagMultiple)
		{
		(*(TInt*)iValue)++;
		}
	else
		{
		*(TBool*)iValue = ETrue;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(TInt aInt)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeInt, IoUtils::Panic(EIncorrectCommandOptionType2));
	if (iValueType & KValueTypeFlagMultiple)
		{
		User::LeaveIfError(((RArray<TInt>*)iValue)->Append(aInt));
		}
	else
		{
		*(TInt*)iValue = aInt;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(TInt64 aInt)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeInt64, IoUtils::Panic(EIncorrectCommandOptionType11));
	if (iValueType & KValueTypeFlagMultiple)
		{
		User::LeaveIfError(((RArray<TInt64>*)iValue)->Append(aInt));
		}
	else
		{
		*(TInt64*)iValue = aInt;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(TUint aUint)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeUint, IoUtils::Panic(EIncorrectCommandOptionType3));
	if (iValueType & KValueTypeFlagMultiple)
		{
		User::LeaveIfError(((RArray<TUint>*)iValue)->Append(aUint));
		}
	else
		{
		*(TUint*)iValue = aUint;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(TUint64 aUint)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeUint64, IoUtils::Panic(EIncorrectCommandOptionType10));
	if (iValueType & KValueTypeFlagMultiple)
		{
		User::LeaveIfError(((RArray<TUint64>*)iValue)->Append(aUint));
		}
	else
		{
		*(TUint64*)iValue = aUint;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(TReal64 aReal)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeReal, IoUtils::Panic(EIncorrectCommandOptionType4));
	if (iValueType & KValueTypeFlagMultiple)
		{
		User::LeaveIfError(((RArray<TReal64>*)iValue)->Append(aReal));
		}
	else
		{
		*(TReal64*)iValue = aReal;
		}
	iIsSet = ETrue;
	}

void TValue::SetValueL(HBufC* aString)
	{
	__ASSERT_ALWAYS( ((iValueType & KValueTypeFlagMask) == KValueTypeString) || (iValueType & KValueTypeFlagMask) == KValueTypeEnum, IoUtils::Panic(EIncorrectCommandOptionType5));
	if ((iValueType & KValueTypeFlagMask) == KValueTypeEnum)
		{
		TInt value = iEnum.ParseL(*aString);
		if (iValueType & KValueTypeFlagMultiple)
			{
			User::LeaveIfError(((RArray<TInt>*)iValue)->Append(value));
			}
		else
			{
			*(TInt*)iValue = value;
			}
		delete aString; // Because the command class knows nothing about how enums are converted from HBufCs in this case, it cannot be responsible for deleting the HBufC like it would be for a normal string argument. So we delete it here.
		iIsSet = ETrue;
		}
	else // string
		{
		if (iValueType & KValueTypeFlagMultiple)
			{
			User::LeaveIfError(((RPointerArray<HBufC>*)iValue)->Append(aString));
			}
		else
			{
			*(HBufC**)iValue = aString;
			}
		iIsSet = ETrue;
		}
	}

void TValue::SetValueL(const TFileName2& aFileName, RFs& aFs)
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeFileName, IoUtils::Panic(EIncorrectCommandOptionType6));
		
	if (AcceptsMultiple())
		{
		if (aFileName.IsWild())
			{
			CDir* matches;
			User::LeaveIfError(aFs.GetDir(aFileName, KEntryAttMaskSupported, ESortByName, matches));
			CleanupStack::PushL(matches);
			for (TInt i = 0; i < matches->Count(); ++i)
				{
				TFileName2 expandedFileName((*matches)[i].iName);
				expandedFileName.MakeAbsoluteL(aFileName.DriveAndPath());
				((RArray<TFileName2>*)iValue)->AppendL(expandedFileName);
				}
			CleanupStack::PopAndDestroy(matches);
			}
		else
			{
			((RArray<TFileName2>*)iValue)->AppendL(aFileName);
			}
		}
	else
		{
		((TFileName2*)iValue)->SetL(aFileName);
		}

	iIsSet = ETrue;
	}

void TValue::SetEnumValues(const TEnum& aEnum)
	{
	iEnum = aEnum;
	}

TBool TValue::IsSet() const
	{
	return iIsSet;
	}

EXPORT_C TBool TValue::AcceptsMultiple() const
	{
	return (iValueType & KValueTypeFlagMultiple);
	}
	
EXPORT_C const TDesC& TValue::EnumValueList() const
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeEnum, IoUtils::Panic(EIncorrectCommandOptionType7));
	return iEnum.ValueList();
	}

const TDesC& TValue::EnumDescriptionList() const
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeEnum, IoUtils::Panic(EIncorrectCommandOptionType8));
	return iEnum.DescriptionList();
	}

EXPORT_C const TDesC& TValue::EnvVar() const
	{
	return iEnvVar;
	}

EXPORT_C const TDesC& TValue::Name() const
	{
	return iName;
	}

const TDesC& TValue::Description() const
	{
	return iDescription;
	}

EXPORT_C void* TValue::ValuePtr() const
	{
	return iValue;
	}
	
TPtrC TValue::AsString() const
	{
	if (!IsSet()) return KNullDesC();
	if (AcceptsMultiple()) return KNullDesC();
	switch (iValueType)
		{
	case KValueTypeString:
		return *(HBufC*)iValue;
	case KValueTypeFileName:
		return *(TFileName2*)iValue;
	case KValueTypeEnum:
		return iEnum.GetString(*(TInt*)iValue);
	default:
		return KNullDesC();
		}
	}
	
void TValue::AppendEnumValuesL(CTextBuffer& aBuf) const
	{
	__ASSERT_ALWAYS((iValueType & KValueTypeFlagMask) == KValueTypeEnum, IoUtils::Panic(EIncorrectCommandOptionType9));
	return iEnum.AppendValuesL(aBuf);
	}

void TValue::Combine(const TValue& aValue)
	{
	__ASSERT_ALWAYS(iDescription.Ptr() == NULL, IoUtils::Panic(EValueDescriptionAlreadySet));

	iValueType = aValue.iValueType;
	iDescription.Set(aValue.iDescription);
	if ((iValueType & KValueTypeFlagMask) == KValueTypeEnum)
		{
		if (aValue.iEnum.ValueList().Length() > 0)
			{
			iEnum.SetValueList(aValue.iEnum.ValueList());
			}
		if (aValue.iEnum.DescriptionList().Length() > 0)
			{
			iEnum.SetDescriptionList(aValue.iEnum.DescriptionList());
			}
		}
	iEnvVar.Set(aValue.EnvVar());
	}


//
// TCommandOption.
//

TCommandOption::TCommandOption(TUint aValueType, void* aValuePointer, const TDesC& aLongName)
	: TValue(aValueType, aValuePointer, aLongName)
	{
	}

TCommandOption::TCommandOption(TUint aValueType, void* aValuePointer, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aEnvVar)
	: TValue(aValueType, aValuePointer, aLongName, aDescription, aEnvVar), iShortName(aShortName)
	{
	}
	
TCommandOption::TCommandOption(void* aValuePointer, TUint aValueType, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aEnvVar)
	: TValue(aValueType, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aValuePointer, aLongName, aDescription, aEnvVar), iShortName(aShortName)
	{
	}

TChar TCommandOption::ShortName() const
	{
	return iShortName;
	}

void TCommandOption::Combine(const TCommandOption& aOption)
	{
	TValue::Combine(aOption);
	iShortName = aOption.iShortName;
	}


//
// RCommandOptionList.
//

void RCommandOptionList::Close()
	{
	iOptions.Close();
	}

EXPORT_C void RCommandOptionList::AppendBoolL(TBool& aBool, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeBool, &aBool, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendBoolL(RArray<TBool>& aBools, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeBool | KValueTypeFlagMultiple, &aBools, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendIntL(TInt& aInt, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeInt, &aInt, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendIntL(RArray<TInt>& aInts, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeInt | KValueTypeFlagMultiple, &aInts, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendIntL(TInt64& aInt, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeInt64, &aInt, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendIntL(RArray<TInt64>& aInts, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeInt64 | KValueTypeFlagMultiple, &aInts, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendUintL(TUint& aUint, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeUint, &aUint, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendUintL(RArray<TUint>& aUints, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeUint | KValueTypeFlagMultiple, &aUints, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendUintL(TUint64& aUint, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeUint64, &aUint, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendUintL(RArray<TUint64>& aUints, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeUint64 | KValueTypeFlagMultiple, &aUints, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendRealL(TReal64& aReal, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeReal, &aReal, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendRealL(RArray<TReal64>& aReals, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeReal | KValueTypeFlagMultiple, &aReals, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendStringL(HBufC*& aBuf, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeString, &aBuf, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeString | KValueTypeFlagMultiple, &aBufs, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendFileNameL(TFileName2& aFileName, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeFileName, &aFileName, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeFileName | KValueTypeFlagMultiple, &aFileNames, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendEnumL(TInt& aEnum, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeEnum, &aEnum, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendEnumL(TInt& aEnum, const TDesC& aLongName, const TDesC& aCommaSeparatedValueList)
	{
	TCommandOption opt(KValueTypeEnum, &aEnum, aLongName);
	opt.SetEnumValues(TEnum(aCommaSeparatedValueList));
	iOptions.AppendL(opt);
	}

EXPORT_C void RCommandOptionList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aLongName)
	{
	iOptions.AppendL(TCommandOption(KValueTypeEnum | KValueTypeFlagMultiple, &aEnum, aLongName));
	}

EXPORT_C void RCommandOptionList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aLongName, const TDesC& aCommaSeparatedValueList)
	{
	TCommandOption opt(KValueTypeEnum | KValueTypeFlagMultiple, &aEnum, aLongName);
	opt.SetEnumValues(TEnum(aCommaSeparatedValueList));
	iOptions.AppendL(opt);
	}

EXPORT_C void RCommandOptionList::AppendBoolL(TBool& aBool, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeBool | aFlags, &aBool, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendBoolL(RArray<TBool>& aBools, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeBool | KValueTypeFlagMultiple | aFlags, &aBools, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendIntL(TInt& aInt, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeInt | aFlags, &aInt, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendIntL(RArray<TInt>& aInts, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeInt | KValueTypeFlagMultiple | aFlags, &aInts, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendIntL(TInt64& aInt, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeInt64 | aFlags, &aInt, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendIntL(RArray<TInt64>& aInts, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeInt64 | KValueTypeFlagMultiple | aFlags, &aInts, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendUintL(TUint& aUint, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeUint | aFlags, &aUint, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendUintL(RArray<TUint>& aUints, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeUint | KValueTypeFlagMultiple | aFlags, &aUints, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendUintL(TUint64& aUint, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeUint64 | aFlags, &aUint, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendUintL(RArray<TUint64>& aUints, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeUint64 | KValueTypeFlagMultiple | aFlags, &aUints, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendRealL(TReal64& aReal, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeReal | aFlags, &aReal, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendRealL(RArray<TReal64>& aReals, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeReal | KValueTypeFlagMultiple | aFlags, &aReals, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendStringL(HBufC*& aBuf, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeString | aFlags, &aBuf, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendStringL(RPointerArray<HBufC>& aBufs, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeString | KValueTypeFlagMultiple | aFlags, &aBufs, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendFileNameL(TFileName2& aFileName, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeFileName | aFlags, &aFileName, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendFileNameL(RArray<TFileName2>& aFileNames, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(KValueTypeFileName | KValueTypeFlagMultiple | aFlags, &aFileNames, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendEnumL(TInt& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(&aEnum, KValueTypeEnum | aFlags, aCommaSeparatedValueList, KNullDesC, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendEnumL(TInt& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(&aEnum, KValueTypeEnum | aFlags, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendEnumL(RArray<TInt>& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(&aEnum, KValueTypeEnum | KValueTypeFlagMultiple | aFlags, aCommaSeparatedValueList, KNullDesC, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

EXPORT_C void RCommandOptionList::AppendEnumL(RArray<TInt>& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandOption option(&aEnum, KValueTypeEnum | KValueTypeFlagMultiple | aFlags, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aShortName, aLongName, aDescription, aEnvVar);
	Validate(option);
	iOptions.AppendL(option);
	}

void RCommandOptionList::Validate(const TCommandOption& aOption)
	{
	__ASSERT_ALWAYS(!(aOption.ValueType() & KValueTypeFlagLast), Panic(EOptionTaggedAsLast));
	__ASSERT_ALWAYS((aOption.ShortName() == 0) || (FindShort(aOption.ShortName()) == KErrNotFound), Panic(EDuplicateShortOptionName));
	TInt pos = Find(aOption.Name());
	if (pos >= 0)
		{
		iOptions.Remove(pos); // Allow a newly added option of the same long name as an existing one to replace it. Primarily of use in .cif files via the "==include" directive to support command class hierarchies.
		}
	}

EXPORT_C TInt RCommandOptionList::Count() const
	{
	return iOptions.Count();
	}

EXPORT_C TCommandOption& RCommandOptionList::operator[](TInt aIndex)
	{
	return iOptions[aIndex];
	}

EXPORT_C const TCommandOption& RCommandOptionList::operator[](TInt aIndex) const
	{
	return iOptions[aIndex];
	}

TInt RCommandOptionList::FindShort(TChar aShortName) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iOptions[i].ShortName() == aShortName)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

TInt RCommandOptionList::Find(const TDesC& aLongName) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iOptions[i].Name() == aLongName)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

EXPORT_C TBool RCommandOptionList::IsPresent(void* aValuePointer) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iOptions[i].ValuePtr() == aValuePointer)
			{
			return iOptions[i].IsSet();
			}
		}
	return EFalse;
	}
	
EXPORT_C TPtrC RCommandOptionList::AsString(void* aValuePointer) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iOptions[i].ValuePtr() == aValuePointer)
			{
			return iOptions[i].AsString();
			}
		}
	return KNullDesC();
	}


//
// TCommandArgument.
//

TCommandArgument::TCommandArgument(TUint aValueType, void* aValuePointer, const TDesC& aName)
	: TValue(aValueType, aValuePointer, aName)
	{
	}

TCommandArgument::TCommandArgument(TUint aValueType, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar)
	: TValue(aValueType, aValuePointer, aName, aDescription, aEnvVar)
	{
	}

TCommandArgument::TCommandArgument(void* aValuePointer, TUint aValueType, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, const TDesC& aEnvVar)
	: TValue(aValueType, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aValuePointer, aName, aDescription, aEnvVar)
	{
	}

TBool TCommandArgument::IsOptional() const
	{
	return ValueType() & KValueTypeFlagOptional;
	}

TBool TCommandArgument::IsLast() const
	{
	return ValueType() & KValueTypeFlagLast;
	}


//
// RCommandArgumentList.
//

void RCommandArgumentList::Close()
	{
	iArguments.Close();
	}

EXPORT_C void RCommandArgumentList::AppendIntL(TInt& aInt, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeInt, &aInt, aName));
	}

EXPORT_C void RCommandArgumentList::AppendIntL(RArray<TInt>& aInts, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeInt | KValueTypeFlagMultiple, &aInts, aName));
	}

EXPORT_C void RCommandArgumentList::AppendIntL(TInt64& aInt, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeInt64, &aInt, aName));
	}

EXPORT_C void RCommandArgumentList::AppendIntL(RArray<TInt64>& aInts, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeInt64 | KValueTypeFlagMultiple, &aInts, aName));
	}

EXPORT_C void RCommandArgumentList::AppendUintL(TUint& aUint, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeUint, &aUint, aName));
	}

EXPORT_C void RCommandArgumentList::AppendUintL(RArray<TUint>& aUints, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeUint | KValueTypeFlagMultiple, &aUints, aName));
	}

EXPORT_C void RCommandArgumentList::AppendUintL(TUint64& aUint, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeUint64, &aUint, aName));
	}

EXPORT_C void RCommandArgumentList::AppendUintL(RArray<TUint64>& aUints, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeUint64 | KValueTypeFlagMultiple, &aUints, aName));
	}

EXPORT_C void RCommandArgumentList::AppendRealL(TReal64& aReal, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeReal, &aReal, aName));
	}

EXPORT_C void RCommandArgumentList::AppendRealL(RArray<TReal64>& aReals, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeReal | KValueTypeFlagMultiple, &aReals, aName));
	}

EXPORT_C void RCommandArgumentList::AppendStringL(HBufC*& aBuf, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeString, &aBuf, aName));
	}

EXPORT_C void RCommandArgumentList::AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeString | KValueTypeFlagMultiple, &aBufs, aName));
	}

EXPORT_C void RCommandArgumentList::AppendFileNameL(TFileName2& aFileName, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeFileName, &aFileName, aName));
	}

EXPORT_C void RCommandArgumentList::AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeFileName | KValueTypeFlagMultiple, &aFileNames, aName));
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(TInt& aEnum, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeEnum, &aEnum, aName));
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aCommaSeparatedValueList)
	{
	TCommandArgument arg(KValueTypeEnum, &aEnum, aName);
	arg.SetEnumValues(TEnum(aCommaSeparatedValueList));
	iArguments.AppendL(arg);
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName)
	{
	iArguments.AppendL(TCommandArgument(KValueTypeEnum | KValueTypeFlagMultiple, &aEnum, aName));
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aCommaSeparatedValueList)
	{
	TCommandArgument arg(KValueTypeEnum | KValueTypeFlagMultiple, &aEnum, aName);
	arg.SetEnumValues(TEnum(aCommaSeparatedValueList));
	iArguments.AppendL(arg);
	}

EXPORT_C void RCommandArgumentList::AppendIntL(TInt& aInt, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeInt | aFlags, &aInt, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendIntL(RArray<TInt>& aInts, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeInt | KValueTypeFlagMultiple | aFlags, &aInts, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendIntL(TInt64& aInt, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeInt64 | aFlags, &aInt, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendIntL(RArray<TInt64>& aInts, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeInt64 | KValueTypeFlagMultiple | aFlags, &aInts, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendUintL(TUint& aUint, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeUint | aFlags, &aUint, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendUintL(RArray<TUint>& aUints, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeUint | KValueTypeFlagMultiple | aFlags, &aUints, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendUintL(TUint64& aUint, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeUint64 | aFlags, &aUint, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendUintL(RArray<TUint64>& aUints, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeUint64 | KValueTypeFlagMultiple | aFlags, &aUints, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendRealL(TReal64& aReal, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeReal | aFlags, &aReal, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendRealL(RArray<TReal64>& aReals, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeReal | KValueTypeFlagMultiple | aFlags, &aReals, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendStringL(HBufC*& aBuf, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeString | aFlags, &aBuf, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeString | KValueTypeFlagMultiple | aFlags, &aBufs, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendFileNameL(TFileName2& aFileName, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeFileName | aFlags, &aFileName, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aName, const TDesC& aDescription, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(KValueTypeFileName | KValueTypeFlagMultiple | aFlags, &aFileNames, aName, aDescription, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(&aEnum, KValueTypeEnum | aFlags, aName, aDescription, aCommaSeparatedValueList, KNullDesC, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(&aEnum, KValueTypeEnum | aFlags, aName, aDescription, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(&aEnum, KValueTypeEnum | KValueTypeFlagMultiple | aFlags, aName, aDescription, aCommaSeparatedValueList, KNullDesC, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C void RCommandArgumentList::AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags, const TDesC& aEnvVar)
	{
	TCommandArgument argument(&aEnum, KValueTypeEnum | KValueTypeFlagMultiple | aFlags, aName, aDescription, aCommaSeparatedValueList, aCommaSeparatedDescriptionList, aEnvVar);
	Validate(argument);
	iArguments.AppendL(argument);
	}

EXPORT_C TBool RCommandArgumentList::IsPresent(TInt aIndex) const
	{
	__ASSERT_ALWAYS((aIndex >= 0) && (aIndex < Count()), Panic(EInvalidArgumentIndex));	
	return iArguments[aIndex].IsSet();
	}

EXPORT_C TBool RCommandArgumentList::IsPresent(void* aValuePointer) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iArguments[i].ValuePtr() == aValuePointer)
			{
			return iArguments[i].IsSet();
			}
		}
	return EFalse;
	}

void RCommandArgumentList::Validate(const TCommandArgument& aArgument)
	{
	TUint valueType = aArgument.ValueType();
	__ASSERT_ALWAYS(!(valueType & KValueTypeFlagLast) || (valueType & KValueTypeString), Panic(ELastArgNotStringType));
	__ASSERT_ALWAYS(!(valueType & KValueTypeFlagLast) || !(valueType & KValueTypeFlagMultiple), Panic(ELastArgMultiple));
	if (iArguments.Count() > 0)
		{
		__ASSERT_ALWAYS(!iArguments[iArguments.Count() - 1].AcceptsMultiple(), Panic(EArgumentFollowsMultipleArgument));
		__ASSERT_ALWAYS(valueType & KValueTypeFlagOptional || !iArguments[iArguments.Count() - 1].IsOptional(), Panic(ENonOptionalArgumentFollowsOptionalArgument));
		}
	TInt pos = Find(aArgument.Name());
	if (pos >= 0)
		{
		iArguments.Remove(pos); // Allow a newly added argument of the same long name as an existing one to replace it. Primarily of use in .cif files via the "==include" directive to support command class hierarchies.
		}
	}

EXPORT_C TInt RCommandArgumentList::Count() const
	{
	return iArguments.Count();
	}

TCommandArgument& RCommandArgumentList::operator[](TInt aIndex)
	{
	return iArguments[aIndex];
	}

EXPORT_C const TCommandArgument& RCommandArgumentList::operator[](TInt aIndex) const
	{
	return iArguments[aIndex];
	}

EXPORT_C TBool RCommandArgumentList::AllSet() const
	{
	TBool allSet(ETrue);
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		const TCommandArgument& thisArgument = iArguments[i];
		if (!thisArgument.IsOptional() && !thisArgument.IsSet() && !(thisArgument.ValueType() == (KValueTypeFileName | KValueTypeFlagMultiple))) // Note, allow multiple file names because "dir\*" might not produces any matches, but we don't want to report that as a syntax error. Instead expect commands to cope with zero file names.
			{
			allSet = EFalse;
			break;
			}
		}
	return allSet;
	}

TInt RCommandArgumentList::Find(const TDesC& aName) const
	{
	const TInt count = Count();
	for (TInt i = 0; i < count; ++i)
		{
		if (iArguments[i].Name() == aName)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

EXPORT_C TPtrC RCommandArgumentList::AsString(void* aValuePointer) const
	{
	for (TInt i=0; i<Count(); ++i)
		{
		if (iArguments[i].ValuePtr() == aValuePointer)
			{
			return iArguments[i].AsString();
			}
		}
	return KNullDesC();
	}


//
// CCommandBase.
//

EXPORT_C void CCommandBase::RunCommandL()
	{
	RunCommandL(NULL, NULL);
	}

NONSHARABLE_CLASS(TExitController) : public MCommandBaseObserver
	{
public:
	TExitController();
	TBool IsComplete() const;
	TInt CompletionError() const;
	void WaitForCompletion();
private:
	virtual void HandleCommandComplete(CCommandBase& aCommand, TInt aError);
private:
	TBool iSchedulerStarted;
	TBool iComplete;
	TInt iCompletionError;
	};

TExitController::TExitController()
	: iSchedulerStarted(EFalse), iComplete(EFalse), iCompletionError(KErrNone)
	{
	}

TBool TExitController::IsComplete() const
	{
	return iComplete;
	}

TInt TExitController::CompletionError() const
	{
	return iCompletionError;
	}

void TExitController::WaitForCompletion()
	{
	iSchedulerStarted = ETrue;
	CActiveScheduler::Start();
	}

void TExitController::HandleCommandComplete(CCommandBase&, TInt aError)
	{
	iComplete = ETrue;
	iCompletionError = aError;
	if (iSchedulerStarted)
		{
		CActiveScheduler::Stop();
		iSchedulerStarted = EFalse;
		}
	}

void CCommandBase::CreateHandlesL()
	{
	// This function is called *before* the command line is parsed, so things like iConsoleImplementation and iConsoleSize are not available.
	// UpdateHandlesL is called *after* the command line is parsed to change the handles if necessary.

	TFullName name;
	LtkUtils::GetFriendlyThreadName(RThread(), name);
	RIoConsole console;
	CleanupClosePushL(console);

	TInt err = iStdin.Open(iIoSession);
	if (err == KErrNone)
		{
		User::LeaveIfError(iStdout.Open(iIoSession));
		User::LeaveIfError(iStderr.Open(iIoSession));
		}
	else
		{
		console.CreateL(iIoSession, name, TSize(KConsFullScreen, KConsFullScreen), RIoConsole::ELazyCreate);
		DoCreateHandlesL(console, iStdin, iStdout, iStderr);
		}

	CleanupStack::PopAndDestroy(&console); // Console (if used) will be held open by the read and write handles.

	if (iFlags & ENotifyStdinChanges)
		{
		iReadChangeNotifier = new(ELeave)CReaderChangeNotifier(*this, iStdin);
		iReadChangeNotifier->Notify();
		}	
	}

void CCommandBase::UpdateHandlesL()
	{
	TFullName name;
	LtkUtils::GetFriendlyThreadName(RThread(), name);
	RIoConsole console;
	CleanupClosePushL(console);

	if (iPersistentConsoleName)
		{
		((RIoPersistentConsole&)console).CreateL(iIoSession, *iPersistentConsoleName, (iConsoleTitle == NULL) ? name : *iConsoleTitle);
		}
	else
		{
		TSize size(KConsFullScreen, KConsFullScreen);
		if (iConsoleSize.Count() > 0)
			{
			size.iWidth = iConsoleSize[0];
			}
		if (iConsoleSize.Count() > 1)
			{
			size.iHeight = iConsoleSize[1];
			}
		RIoConsole underlyingConsole;
		underlyingConsole.OpenL(iIoSession, iStdout);
		CleanupClosePushL(underlyingConsole);
		console.CreateL(iIoSession, *iConsoleImplementation, underlyingConsole, (iConsoleTitle == NULL) ? name : *iConsoleTitle, size);
		CleanupStack::PopAndDestroy(&underlyingConsole);
		RIoConsoleReadHandle stdin;
		CleanupClosePushL(stdin);
		RIoConsoleWriteHandle stdout;
		CleanupClosePushL(stdout);
		RIoConsoleWriteHandle stderr;
		CleanupClosePushL(stderr);
		DoCreateHandlesL(console, stdin, stdout, stderr);
		if (iFlags & ENotifyStdinChanges)
			{
			CReaderChangeNotifier* readChangeNotifier = new(ELeave)CReaderChangeNotifier(*this, iStdin);
			delete iReadChangeNotifier;
			iReadChangeNotifier = readChangeNotifier;
			}	
		iStdin.Close();
		iStdin = stdin;
		iStdout.Close();
		iStdout = stdout;
		iStderr.Close();
		iStderr = stderr;
		CleanupStack::Pop(3, &stdin);
		if (iFlags & ENotifyStdinChanges)
			{
			iReadChangeNotifier->Notify();
			}
		}

	CleanupStack::PopAndDestroy(&console); // Console (if used) will be held open by the read and write handles.
	}

void CCommandBase::DoCreateHandlesL(RIoConsole& aConsole, RIoConsoleReadHandle& aStdin, RIoConsoleWriteHandle& aStdout, RIoConsoleWriteHandle& aStderr)
	{
	TFullName name;
	LtkUtils::GetFriendlyThreadName(RThread(), name);

	TName consImpl;
	TInt err = aConsole.Implementation(consImpl);
	if (err != KErrOverflow)
		{
		User::LeaveIfError(err);
		}
	if (consImpl.Right(KDllExt().Length()).CompareF(KDllExt)==0)
		{
		consImpl.SetLength(consImpl.Length() - KDllExt().Length());
		}
	if (name.Length() + consImpl.Length() + 1 > name.MaxLength())
		{
		name.SetLength(name.MaxLength() - (name.Length() + consImpl.Length() + 1));
		}
	name.Insert(0, _L("_"));
	name.Insert(0, consImpl);

	iIoSession.SetObjectNameL(aConsole.SubSessionHandle(), name);
	
	aStdin.CreateL(iIoSession);
	_LIT(KStdin, "_stdin");
	name.Append(KStdin);
	iIoSession.SetObjectNameL(aStdin.SubSessionHandle(), name);

	aStdout.CreateL(iIoSession);
	name.Delete(name.Length() - KStdin().Length(), KStdin().Length());
	_LIT(KStdout, "_stdout");
	name.Append(KStdout);
	iIoSession.SetObjectNameL(aStdout.SubSessionHandle(), name);

	aStderr.CreateL(iIoSession);
	User::LeaveIfError(aStderr.SetIsStdErr(ETrue));
	name.Delete(name.Length() - KStdout().Length(), KStdout().Length());
	_LIT(KStderr, "_stderr");
	name.Append(KStderr);
	iIoSession.SetObjectNameL(aStderr.SubSessionHandle(), name);

	aConsole.AttachL(aStdin, RIoEndPoint::EForeground);
	aConsole.AttachL(aStdout);
	aConsole.AttachL(aStderr);

	aStdin.SetUnderlyingConsoleL(aConsole);
	aStdout.SetUnderlyingConsoleL(aConsole);
	aStderr.SetUnderlyingConsoleL(aConsole);
	}

EXPORT_C void CCommandBase::RunCommandL(const TDesC* aCommandLine, CEnvironment* aEnv)
	{
	iFlags |= EOwnsHandles;

	User::LeaveIfError(iIoSession.Connect());
	if (iFlags & ESharableIoSession)
		{
		User::LeaveIfError(iIoSession.ShareAuto());
		}

	TBool commandLineOnStack(EFalse);
	const TDesC* commandLine = aCommandLine;
	if (aCommandLine == NULL)
		{
		commandLine = ReadCommandLineLC();
		commandLineOnStack = ETrue;
		}

	CreateEnvironmentL(aEnv);
	CreateHandlesL();

	TExitController exitController;
	RunCommand(iIoSession, iStdin, iStdout, iStderr, commandLine, iEnv, &exitController);
	if (!exitController.IsComplete())
		{
		exitController.WaitForCompletion();
		}

	TInt completionError = exitController.CompletionError();
	if (completionError != KErrNone)
		{
		User::Leave(completionError);
		}

	if (commandLineOnStack)
		{
		CleanupStack::PopAndDestroy((TAny*)commandLine);
		}
	}

void CCommandBase::RunCommand(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const TDesC* aCommandLine, CEnvironment* aEnv, MCommandBaseObserver* aObserver)
	{
	TRAPD(err, RunCommandL(aIoSession, aStdin, aStdout, aStderr, aCommandLine, aEnv, aObserver));
	if (err)
		{
		Complete(err);
		}
	}

EXPORT_C TUint CCommandBase::Flags() const
	{
	return (iFlags & KPublicFlagsMask);
	}

EXPORT_C void CCommandBase::SetFlags(TUint aFlags)
	{
	TUint privateFlags = (iFlags & KPrivateFlagsMask);
	iFlags = (aFlags & KPublicFlagsMask);
	iFlags |= privateFlags;
	// Make sure implied flags are set
	if (iFlags & ECompleteOnRunL)
		{
		iFlags |= EManualComplete;
		}
	if (iFlags & ECaptureCtrlC)
		{
		iFlags |= ENotifyKeypresses;
		}
	}

EXPORT_C void CCommandBase::SetCif(const CCommandInfoFile& aCif)
	{
	ASSERT((iCif == NULL) && !(iFlags & EOwnsCif));
	iCif = &const_cast<CCommandInfoFile&>(aCif);
	}

EXPORT_C const CCommandInfoFile* CCommandBase::Cif() const
	{
	return iCif;
	}

void CCommandBase::RunCommandL(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const TDesC* aCommandLine, CEnvironment* aEnv, MCommandBaseObserver* aObserver)
	{
	iIoSession = aIoSession;
	iStdin = aStdin;
	iStdout = aStdout;
	iStderr = aStderr;
	iObserver = aObserver;

	CreateEnvironmentL(aEnv);

	if (aCommandLine)
		{
		RunL(*aCommandLine);
		}
	else
		{
		const TDesC* commandLine = ReadCommandLineLC();
		RunL(*commandLine);
		CleanupStack::PopAndDestroy((TAny*)commandLine);
		}
	}

void CCommandBase::RunL(const TDesC& aCommandLine)
	{
	TInt err = ParseCommandLine(aCommandLine);
	if (iFlags & ECifReadFailed)
		{
		// No point in trying to display help.
		Complete(err);
		}
	else if (err || iDisplayHelp)
		{
		if (err && !iDisplayHelp)
			{
			DisplayHelpAndComplete(err, _L("Couldn't parse command line"));
			}
		else
			{
			DisplayHelpAndComplete();
			}
		}
	else
		{
		if (iConsoleImplementation || iPersistentConsoleName)
			{
			UpdateHandlesL();
			}

		if (iFlags & ENotifyKeypresses)
			{
			__ASSERT_ALWAYS(iExtension && iExtension->ExtensionVersion() >= ECommandExtensionV2, IoUtils::Panic(EENotifyKeypressesSpecifiedWithoutExtensionBeingSet));
			iKeypressWatcher = new(ELeave) CKeypressWatcher(*static_cast<MCommandExtensionsV2*>(iExtension), Stdin());
			if (iFlags & ECaptureCtrlC)
				{
				User::LeaveIfError(Stdin().CaptureKey(CTRL('c'), 0, 0));
				}
			iKeypressWatcher->Notify();
			}

		TBool deleted(EFalse);
		iDeleted = &deleted;	
		// Note, commands that manually complete may call CCommandBase::Complete from their DoRunL, which in the case 
		// of "local" fshell commands (i.e. ones that run in the context of fshell's thread) will result in the object
		// being deleted. This flag detects this to avoid code later in this function blowing up.
		TRAP(err, DoRunL());
		if (err)
			{
			HandleLeave(err);
			}
		else if (!deleted && !(iFlags & EManualComplete) && !IsActive() && !(iFlags & ECompleted))
			{
			Complete();
			}
		if (!deleted)
			{
			iDeleted = NULL;
			}
		}
	}

EXPORT_C CCommandBase::CCommandBase()
	: CActive(CActive::EPriorityStandard)
	{
	// These are protected so could be accessed directly, hence can't move
	// The asserts have to be scoped inside a CCommandBase function because the member vars are protected. They don't generate any code.
	__ASSERT_COMPILE(_FOFF(CCommandBase, iOptions) == 7*4);
	__ASSERT_COMPILE(_FOFF(CCommandBase, iArguments) == 15*4);

	CActiveScheduler::Add(this);
	if (Dll::Tls() == NULL)
		{
		Dll::SetTls(this);
		iFlags |= ETlsSet;
		}
	}

EXPORT_C CCommandBase::CCommandBase(TUint aFlags)
	: CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	if (Dll::Tls() == NULL)
		{
		Dll::SetTls(this);
		iFlags |= ETlsSet;
		}

	// Don't initialise iFlags directly - SetFlags checks that private flags aren't being set
	SetFlags(aFlags);
	}

EXPORT_C CCommandBase::~CCommandBase()
	{
	delete iReadChangeNotifier;
	delete iCompleter;
	delete iKeypressWatcher;
	iFs.Close();
	if (iFlags & EOwnsHandles)
		{
		iStderr.Close();
		iStdout.Close();
		iStdin.Close();
		iIoSession.Close();
		}

	if (iFlags & EOwnsEnv)
		{
		delete iEnv;
		}
	delete iConsoleImplementation;
	delete iConsoleTitle;
	delete iPersistentConsoleName;
	iConsoleSize.Close();
	iOptions.Close();
	iArguments.Close();
	if (iFlags & EOwnsCif)
		{
		delete iCif;
		}
	if (iDeleted)
		{
		*iDeleted = ETrue;
		}
	if (iFlags & ETlsSet)
		{
		Dll::SetTls(NULL);
		}
	}

EXPORT_C void CCommandBase::BaseConstructL()
	{
	iCompleter = new(ELeave)CCommandCompleter(*this);
	}

EXPORT_C CEnvironment& CCommandBase::Env()
	{
	__ASSERT_ALWAYS(iEnv, Panic(ENoEnvironment));
	return *iEnv;
	}

EXPORT_C const CEnvironment& CCommandBase::Env() const
	{
	__ASSERT_ALWAYS(iEnv, Panic(ENoEnvironment));
	return *iEnv;
	}

void CCommandBase::CompleteCallback(TInt aError)
	{
	Complete(aError);
	}

EXPORT_C void CCommandBase::Complete()
	{
	Complete(KErrNone);
	}

EXPORT_C void CCommandBase::Complete(TInt aError)
	{
	__ASSERT_ALWAYS(!(iFlags & ECompleted), Panic(EAlreadyCompleted));
	iFlags |= ECompleted;
	iCompletionReason = aError;
	if (aError < 0)
		{
		RProcess::Rendezvous(aError);
		const TDesC& name = Name();
		PrintError(aError, _L("Unable to run \"%S\""), &name);
		}
	if (iObserver)
		{
		iObserver->HandleCommandComplete(*this, aError);
		}
	}

EXPORT_C void CCommandBase::Complete(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> buf;
	TOverflowTruncate overflow;
	buf.AppendFormatList(aFmt, list, &overflow);
	VA_END(list);
	iStdout.Write(buf);
	Complete();
	}

EXPORT_C void CCommandBase::Complete(TInt aError, TRefByValue<const TDesC> aFmt, ...)
	{
	if (iFlags & EErrorReported)
		{
		Complete(aError);
		}
	else
		{
		RProcess::Rendezvous(aError); // This isn't right for thread commands - not sure how to distinguish. Probably won't cause a problem though. (We also rendezvous in Complete(TInt), but to avoid deadlocks in server commands we need to rendezvous before writing anything (in the error case)
		VA_LIST list;
		VA_START(list, aFmt);
		PrintErrorList(aError, aFmt, list);
		Complete(aError);
		VA_END(list);
		}
	}

EXPORT_C void CCommandBase::CompleteAsync(TInt aError)
	{
	iCompleter->Complete(aError);
	}

EXPORT_C void CCommandBase::CompleteAsync(TInt aError, TRefByValue<const TDesC> aFmt, ...)
	{
	if (iFlags & EErrorReported)
		{
		iCompleter->Complete(aError);
		}
	else
		{
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf<0x100> buf;
		FormatErrorText(buf, aError, aFmt, list);
		VA_END(list);
		iCompleter->Complete(aError, buf, iStderr);
		}
	}

EXPORT_C void CCommandBase::ReadL(TDes& aData)
	{
	User::LeaveIfError(iStdin.Read(aData));
	}

EXPORT_C void CCommandBase::Write(const TDesC& aData)
	{
	iStdout.Write(aData); // Ignore error.
	}

EXPORT_C TUint CCommandBase::ReadKey()
	{
	// Make sure keypresswatcher doesn't get in the way (some commands do some synchronous ReadKeys followed by use of async ENotifyKeypresses callbacks)
	if (iKeypressWatcher) iKeypressWatcher->Cancel();
	TUint result = iStdin.ReadKey();
	if (iKeypressWatcher) iKeypressWatcher->Notify();
	return result;
	}

EXPORT_C void CCommandBase::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	PrintList(aFmt, list);
	VA_END(list);
	}

EXPORT_C void CCommandBase::Printf(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	PrintList(aFmt, list);
	VA_END(list);
	}

EXPORT_C void CCommandBase::PrintError(TInt aError, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	PrintErrorList(aError, aFmt, list);
	VA_END(list);
	}

EXPORT_C void CCommandBase::PrintWarning(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	PrintWarningList(aFmt, list);
	VA_END(list);
	}

EXPORT_C void CCommandBase::PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate overflow;
	TBuf<0x100> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	iStdout.Write(buf);
	}

EXPORT_C void CCommandBase::PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	TOverflowTruncate8 overflow;
	TBuf8<0x200> buf;
	buf.AppendFormatList(aFmt, aList, &overflow);
	if (buf.Length() > 0x100) buf.SetLength(0x100); // Truncate to half the buffer size so that the call to Expand doesn't panic
	TPtrC wideBuf = buf.Expand();
	iStdout.Write(wideBuf);
	}

EXPORT_C void CCommandBase::PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (!Stderr().SubSessionHandle())
		{
		// Only can happen if we error during say command line parsing, which occurs before the handles are set up.
		return;
		}

	if (iSupressingErrors > 0 || ((iFlags & EErrorReported) && !(iFlags & EReportAllErrors)))
		{
		return;
		}

	CheckNewLine();
	TBuf<0x100> buf;
	FormatErrorText(buf, aError, aFmt, aList);
	iStderr.Write(buf);
	iFlags |= EErrorReported;
	}

EXPORT_C void CCommandBase::PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList)
	{
	if (!Stderr().SubSessionHandle())
		{
		// Only can happen if we error during say command line parsing, which occurs before the handles are set up.
		return;
		}
	CheckNewLine();
	TBuf<0x100> buf;
	FormatWarningText(buf, aFmt, aList);
	iStderr.Write(buf);
	}

EXPORT_C void CCommandBase::LeaveIfFileExists(const TDesC& aFileName)
	{
	TFileName2 fileName(aFileName);
	if (fileName.Exists(FsL()))
		{
		PrintError(KErrAlreadyExists, _L("\"%S\" already exists"), &aFileName);
		User::Leave(KErrAlreadyExists);
		}
	}

EXPORT_C void CCommandBase::LeaveIfFileNotFound(const TDesC& aFileName)
	{
	TEntry e;
	TInt err = FsL().Entry(aFileName, e);
	if (err == KErrNotFound)
		{
		PrintError(err, _L("\"%S\" not found"), &aFileName);
		}
	else if (err)
		{
		PrintError(err, _L("Couldn't read \"%S\""), &aFileName);
		}
	User::LeaveIfError(err);
	}

EXPORT_C RFs& CCommandBase::Fs()
	{
	__ASSERT_ALWAYS(iFs.Handle(), Panic(ERFsNotConnected));
	return iFs;
	}

EXPORT_C const RFs& CCommandBase::Fs() const
	{
	__ASSERT_ALWAYS(iFs.Handle(), Panic(ERFsNotConnected));
	return iFs;
	}

EXPORT_C RFs& CCommandBase::FsL()
	{
	if (iFs.Handle() == 0)
		{
		User::LeaveIfError(iFs.Connect());
		const TDesC& pwd = Env().Pwd();
		TInt err = iFs.SetSessionPath(pwd);
		if (err)
			{
			// Could fail if, for eg, we have no capabilities and the fshell we were launched from had \sys\bin as its cwd. In such a case shouldn't be considered an error, only a warning.
			PrintWarning(_L("Couldn't set session path for %S to '%S'. (err = %d)\r\n"), &Name(), &pwd, err);
			}
		}
	return iFs;
	}

EXPORT_C const RFs& CCommandBase::FsL() const
	{
	return const_cast<CCommandBase*>(this)->FsL();
	}

EXPORT_C RIoSession& CCommandBase::IoSession()
	{
	return iIoSession;
	}

EXPORT_C RIoConsoleReadHandle& CCommandBase::Stdin()
	{
	return iStdin;
	}

EXPORT_C RIoConsoleWriteHandle& CCommandBase::Stdout()
	{
	return iStdout;
	}

EXPORT_C RIoConsoleWriteHandle& CCommandBase::Stderr()
	{
	return iStderr;
	}

EXPORT_C void CCommandBase::OptionsL(RCommandOptionList&)
	{
	}

EXPORT_C void CCommandBase::ArgumentsL(RCommandArgumentList&)
	{
	}

EXPORT_C TInt CCommandBase::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CActive::Extension_(aExtensionId, a0, a1);
	}

EXPORT_C void CCommandBase::HandleLeave(TInt aError)
	{
	const TDesC& name = Name();
	Complete(aError, _L("Command \"%S\" failed"), &name);
	}

void HandleEscapeL(TLex& aLex, CTextBuffer& aBuf)
	{
	_LIT(KBell, "\a");
	_LIT(KBackspace, "\b");
	_LIT(KFormFeed, "\f");
	_LIT(KHorizontalTab, "\t");
	_LIT(KVerticalTab, "\v");
	_LIT(KLineFeed, "\n");
	_LIT(KCarriageReturn, "\r");
	if (!aLex.Eos())
		{
		TChar c2 = aLex.Get();
		switch (c2)
			{
			case 'a':
				aBuf.AppendL(KBell);
				break;
			case 'b':
				aBuf.AppendL(KBackspace);
				break;
			case 'f':
				aBuf.AppendL(KFormFeed);
				break;
			case 't':
				aBuf.AppendL(KHorizontalTab);
				break;
			case 'v':
				aBuf.AppendL(KVerticalTab);
				break;
			case 'n':
				aBuf.AppendL(KLineFeed);
				break;
			case 'r':
				aBuf.AppendL(KCarriageReturn);
				break;
			case 'x':
			case 'X':
				if (aLex.Remainder().Length() >= 2)
					{
					TLex lex(aLex.Remainder().Mid(0, 2)); // Note, can't use aLex directly because there could be hex characters following the 2 that relate to this escape sequence.
					TUint16 val;
					User::LeaveIfError(lex.Val(val, EHex));
					aBuf.AppendL(TPtrC(&val, 1));
					aLex.Inc(2);
					}
				break;
			case 'u':
				if (aLex.Remainder().Length() >= 4)
					{
					TLex lex(aLex.Remainder().Mid(0, 4)); // Note, can't use aLex directly because there could be hex characters following the 4 that relate to this escape sequence.
					TUint16 val;
					User::LeaveIfError(lex.Val(val, EHex));
					aBuf.AppendL(TChar(val));
					aLex.Inc(4);
					}
				break;
			case 'U':
				if (aLex.Remainder().Length() >= 8)
					{
					TLex lex(aLex.Remainder().Mid(0, 8)); // Note, can't use aLex directly because there could be hex characters following the 8 that relate to this escape sequence.
					TUint32 val;
					User::LeaveIfError(lex.Val(val, EHex));
					aBuf.AppendL(TChar(val));
					aLex.Inc(8);
					}
				break;
			default:
				aBuf.AppendL(TPtrC((TUint16*)&c2, 1));
				break;
			}
		}
	}

EXPORT_C HBufC* Stringify::ReadLC(TLex& aLex, const TChar& aEscapeChar)
	{
	return ReadLC(aLex, aEscapeChar, NULL, 0);
	}

EXPORT_C HBufC* Stringify::ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase& aErrorContext)
	{
	return ReadLC(aLex, aEscapeChar, &aErrorContext, 0);
	}

EXPORT_C HBufC* Stringify::ReadLC(TLex& aLex, const TChar& aEscapeChar, TUint aReadMode)
	{
	return ReadLC(aLex, aEscapeChar, NULL, aReadMode);
	}

EXPORT_C HBufC* Stringify::ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase& aErrorContext, TUint aReadMode)
	{
	return ReadLC(aLex, aEscapeChar, &aErrorContext, aReadMode);
	}
	
#define ReadLCLeaveIfErr(_command, _error, _errorFmtString...) do {if (_command) {CommandLeaveIfErr(*_command, _error, _errorFmtString);} else User::LeaveIfError(_error); } while(0)
	
HBufC* Stringify::ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase* aErrorContext, TUint aReadMode)
	{
	CTextBuffer* buf = CTextBuffer::NewLC(20);
	while (!aLex.Eos())
		{
		TChar c = aLex.Get();
		if (c == aEscapeChar)
			{
			HandleEscapeL(aLex, *buf);
			}
		else if (c == '\'')
			{
			TBool foundClose = EFalse;
			while (!aLex.Eos())
				{
				c = aLex.Get();
				if ((c == aEscapeChar) && !aLex.Eos() && (aLex.Peek() == '\''))
					{
					// Allow quoted single quote characters. Note, the is a departure from Bash behaviour, but is in line with Perl and is generally helpful.
					buf->AppendL('\'');
					aLex.Get();
					}
				else if (c == '\'')
					{
					foundClose = ETrue;
					break;
					}
				else
					{
					buf->AppendL(c);
					}
				}
			if (!foundClose)
				{
				ReadLCLeaveIfErr(aErrorContext, KErrArgument, _L("Unmatched single quote"));
				}
			}
		else if (c == '"')
			{
			TBool foundClose = EFalse;
			while (!aLex.Eos())
				{
				c = aLex.Get();
				if (c == aEscapeChar)
					{
					HandleEscapeL(aLex, *buf);
					}
				else if (c == '"')
					{
					foundClose = ETrue;
					break;
					}
				else
					{
					buf->AppendL(c);
					}
				}
			if (!foundClose)
				{
				ReadLCLeaveIfErr(aErrorContext, KErrArgument, _L("Unmatched double quote"));
				}
			}
		else if (c.IsSpace() || ((c == ',') && (aReadMode & EBreakOnComma)))
			{
			aLex.UnGet();
			break;
			}
		else
			{
			buf->AppendL(c);
			}
		}

	HBufC* hbuf = buf->Descriptor().AllocL();
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(hbuf);
	return hbuf;
	}

EXPORT_C HBufC* Stringify::WidenLC(const TDesC8& aDes)
	{
	HBufC* buf = HBufC::NewLC(aDes.Length());
	buf->Des().Copy(aDes);
	return buf;
	}

EXPORT_C HBufC8* Stringify::NarrowLC(const TDesC& aDes)
	{
	HBufC8* buf = HBufC8::NewLC(aDes.Length());
	buf->Des().Copy(aDes);
	return buf;
	}

EXPORT_C HBufC* Stringify::EscapeLC(const TDesC& aDes, const TChar& aEscapeChar)
	{
	const TInt origLength = aDes.Length();
	TInt newLength = origLength;
	for (TInt i = 0; i < origLength; ++i)
		{
		if (!TChar(aDes[i]).IsPrint())
			{
			// Hmmm, is this going to go horribly wrong with surrogate pairs?
			newLength += 5; // 5 because <unprintable-char> -> '^unnnn'
			}
		}
	HBufC* buf = HBufC::NewLC(newLength);
	TPtr bufPtr(buf->Des());
	for (TInt i = 0; i < origLength; ++i)
		{
		if (TChar(aDes[i]).IsPrint())
			{
			bufPtr.Append(aDes[i]);
			}
		else
			{
			bufPtr.Append(aEscapeChar);
			bufPtr.Append('u');
			bufPtr.AppendNumFixedWidthUC(aDes[i], EHex, 4);
			}
		}
	return buf;
	}

EXPORT_C HBufC8* Stringify::EscapeLC(const TDesC8& aDes, const TChar& aEscapeChar)
	{
	const TInt origLength = aDes.Length();
	TInt newLength = origLength;
	for (TInt i = 0; i < origLength; ++i)
		{
		if (!TChar(aDes[i]).IsPrint())
			{
			newLength += 3; // 3 because <unprintable-char> -> '^xnn'
			}
		}
	HBufC8* buf = HBufC8::NewLC(newLength);
	TPtr8 bufPtr(buf->Des());
	for (TInt i = 0; i < origLength; ++i)
		{
		if (TChar(aDes[i]).IsPrint())
			{
			bufPtr.Append(aDes[i]);
			}
		else
			{
			bufPtr.Append(aEscapeChar);
			bufPtr.Append('x');
			bufPtr.AppendNumFixedWidthUC(aDes[i], EHex, 2);
			}
		}
	return buf;
	}

TRadix Radix(TLex& aLex)
	{
	_LIT(KBinPrefix, "0b");
	_LIT(KOctPrefix, "0.");
	_LIT(KHexPrefix, "0x");
	
	TRadix radix(EDecimal);

	if (aLex.Remainder().Length() > 2)
		{
		TLexMark mark;
		aLex.Mark(mark);
		aLex.Inc(2);
		TPtrC prefix(aLex.MarkedToken(mark));
		if (prefix == KBinPrefix)
			{
			radix = EBinary;
			}
		else if (prefix == KOctPrefix)
			{
			radix = EOctal;
			}
		else if (prefix == KHexPrefix)
			{
			radix = EHex;
			}
		
		if (radix == EDecimal)
			{
			aLex.UnGetToMark(mark);
			}
		}

	return radix;
	}

void CCommandBase::SetValueL(TLex& aLex, TValue& aValue)
	{
	TBool more((aValue.ValueType() & KValueTypeFlagMultiple) && !(aValue.ValueType() & KValueTypeFlagLast));
	if (aValue.IsSet() && !(aValue.ValueType() & KValueTypeFlagMultiple)) User::Leave(KErrArgument); // Can't set the same parameter repeatedly unless it has the multiple attribute
	do
		{
		switch (aValue.ValueType() & KValueTypeFlagMask)
			{
			case KValueTypeBool:
				{
				aValue.SetValue();
				break;
				}
			case KValueTypeInt:
				{
				aLex.SkipSpace();
				TInt val;
				if (aLex.Val(val) < 0)
					{
					User::Leave(KErrArgument);
					}
				aValue.SetValueL(val);
				break;
				}
			case KValueTypeInt64:
				{
				aLex.SkipSpace();
				TInt64 val;
				if (aLex.Val(val) < 0)
					{
					User::Leave(KErrArgument);
					}
				aValue.SetValueL(val);
				break;
				}
			case KValueTypeUint:
				{
				aLex.SkipSpace();
				TUint val;
				if (aLex.Val(val, Radix(aLex)) < 0)
					{
					User::Leave(KErrArgument);
					}
				aValue.SetValueL(val);
				break;
				}
			case KValueTypeUint64:
				{
				aLex.SkipSpace();
				TInt64 val; // There's no implementation of TLex16::Val(TUint64&, TRadix), so we'll have to use the TInt64 version instead.
				if (aLex.Val(val, Radix(aLex)) < 0)
					{
					User::Leave(KErrArgument);
					}
				aValue.SetValueL((TUint64)val);
				break;
				}
			case KValueTypeReal:
				{
				aLex.SkipSpace();
				TReal64 val;
				if (aLex.Val(val) < 0)
					{
					User::Leave(KErrArgument);
					}
				aValue.SetValueL(val);
				break;
				}
			case KValueTypeString:
				{
				if (aValue.ValueType() & KValueTypeFlagLast)
					{
					TLexMark mark;
					aLex.Mark(mark);
					HBufC* string = ReadStringLC(aLex, 0);
					if ((aLex.Remainder().Length() == 0) || (aLex.Remainder() == KNewLine))
						{
						// ReadStringLC consumed the whole of the remainder of the command line. This means the
						// user must have either provided a just single word, or explicitly quoted / escaped the whitespace
						// between multiple words.
						// Either way, we've got a single argument to satisfy 'last' so we'll use what ReadStringLC
						// returned.
						aValue.SetValueL(string);
						CleanupStack::Pop(string);
						}
					else
						{
						// ReadStringLC didn't consume the remainder of the command line. This means the user must have
						// provided multiple words, but didn't quote or escape the whitespace. In this case take the
						// string they provided unaltered (which is equivalent to adding quotes around the whole string
						// and passing it back through ReadStringLC).
						CleanupStack::PopAndDestroy(string);
						aLex.UnGetToMark(mark);
						aValue.SetValueL(aLex.Remainder().AllocLC());
						CleanupStack::Pop();
						aLex.Inc(aLex.Remainder().Length());
						}
					}
				else
					{
					HBufC* string = ReadStringLC(aLex, (aValue.ValueType() & KValueTypeFlagLast) ? 0 : EDisallowLeadingHyphen);
					aValue.SetValueL(string);
					CleanupStack::Pop(string);
					}
				break;
				}		
			case KValueTypeFileName:
				{
				HBufC* string = ReadStringLC(aLex, EDisallowLeadingHyphen);
				TFileName2 fileName(*string);
				CleanupStack::PopAndDestroy(string);
				if (fileName.Length() == 0)
					{
					User::Leave(KErrArgument);
					}
				fileName.MakeAbsoluteL(iEnv->Pwd());
				aValue.SetValueL(fileName, FsL());
				break;
				}
			case KValueTypeEnum:
				{
				HBufC* string = ReadStringLC(aLex, EDisallowLeadingHyphen);
				aValue.SetValueL(string);
				CleanupStack::Pop(string); // SetValueL takes ownership
				break;
				}
			default:
				{
				__ASSERT_ALWAYS(EFalse, Panic(EInvalidCommandOptionType));
				break;
				}
			}

		if (more)
			{
			TLexMark mark;
			aLex.Mark(mark);
			aLex.SkipSpace();
			if (aLex.Peek() == ',')
				{
				aLex.Get();
				aLex.SkipSpace();
				// Loop again to process the next value in the set.
				}
			else
				{
				aLex.UnGetToMark(mark);
				more = EFalse;
				}
			}
		}
		while (more);
	}

void CCommandBase::SetValueFromEnvL(TValue& aValue)
	{
	if (!aValue.IsSet() && (aValue.EnvVar().Length() > 0) && iEnv->IsDefined(aValue.EnvVar()))
		{
		TUint type = aValue.ValueType() & KValueTypeFlagMask;
		if ((type == KValueTypeString) || (type == KValueTypeEnum))
			{
			HBufC* value = iEnv->GetAsDesL(aValue.EnvVar()).AllocLC();
			aValue.SetValueL(value);
			CleanupStack::Pop(value);
			}
		else if (type == KValueTypeFileName)
			{
			TFileName2 fileName(iEnv->GetAsDesL(aValue.EnvVar()));
			fileName.MakeAbsoluteL(iEnv->Pwd());
			aValue.SetValueL(fileName, FsL());
			}
		else
			{
			TLex lex(iEnv->GetAsDesL(aValue.EnvVar()));
			SetValueL(lex, aValue);
			}
		}
	}



template <class T> TBool ValueTypesConsistent(const T& aList)
	{
	// class T is expected to be either RCommandArgumentList or RCommandOptionList.
	// Returns true if either all the list values have an uninitialized value type, or all have an initialized value type.
	// Returns false if there is a mixture of unitialized and uninitialized value types.

	TBool unitialized(EFalse);
	const TInt count = aList.Count();
	if (count > 0)
		{
		for (TInt i = 0; i < count; ++i)
			{
			if (i == 0)
				{
				unitialized = (aList[i].ValueType() == KValueTypeUninitialized);
				}
			else if (!unitialized != !(aList[i].ValueType() == KValueTypeUninitialized))
				{
				return EFalse;
				}
			}
		}
	return ETrue;
	}

template <class T> TBool ValueTypesInitialized(const T& aList)
	{
	return !(aList[0].ValueType() == KValueTypeUninitialized);
	}


TBool ValueTypesInitializedL(const RCommandArgumentList& aArguments, const RCommandOptionList& aOptions)
	{
	if (!ValueTypesConsistent(aArguments))
		{
		StaticLeaveIfErr(KErrArgument, _L("Arguments inconsistently or incompletely initialized."));
		}
	if (!ValueTypesConsistent(aOptions))
		{
		StaticLeaveIfErr(KErrArgument, _L("Options inconsistently or incompletely initialized."));
		}
	if (aArguments.Count() && aOptions.Count())
		{
		TBool argumentsInitialized(ValueTypesInitialized(aArguments));
		TBool optionsInitialized(ValueTypesInitialized(aOptions));
		if (!argumentsInitialized != !optionsInitialized)
			{
			StaticLeaveIfErr(KErrArgument, _L("Arguments and options inconsistently or incompletely initialized."));
			}
		}
	if (aArguments.Count())
		{
		return ValueTypesInitialized(aArguments);
		}
	else if (aOptions.Count())
		{
		return ValueTypesInitialized(aOptions);
		}
	else
		{
		return ETrue;
		}
	}

EXPORT_C TInt CCommandBase::ParseCommandLine(const TDesC& aCommandLine)
	{
	TRAPD(err, DoParseCommandLineL(aCommandLine));
	return err;
	}

void CCommandBase::DoParseCommandLineL(const TDesC& aCommandLine)
	{
	// Read the CIF if required.
	if (CifReadRequired())
		{
		ReadCifL();
		}

	// Add command specific arguments and options.
	ArgumentsL(iArguments);
	OptionsL(iOptions);

	if (iCif)
		{
		TRAPD(err, iCif->AssignL(iArguments, iOptions));
		if (err)
			{
			iFlags |= ECifReadFailed;
			User::Leave(err);
			}
		__ASSERT_ALWAYS(ValueTypesInitializedL(iArguments, iOptions), Panic(EIncompleteArgumentOrOptionInitialization));
		}

	// Add global options.
	iOptions.AppendBoolL(iDisplayHelp, TChar('h'), KHelpName, KHelpDescription);
	iOptions.AppendStringL(iConsoleImplementation, TChar(0), KOptConsole, KNullDesC);
	iOptions.AppendStringL(iConsoleTitle, TChar(0), KOptConsoleTitle, KNullDesC);
	iOptions.AppendUintL(iConsoleSize, TChar(0), KOptConsoleSize, KNullDesC);
	iOptions.AppendStringL(iPersistentConsoleName, TChar(0), KOptPersistentConsole, KNullDesC);

	// Parse the command line.
	ParseCommandLineL(aCommandLine);

	// Set optional arguments that haven't been set explicitly from environment variables if applicable.
	const TInt numArgs = iArguments.Count();
	for (TInt i = 0; i < numArgs; ++i)
		{
		SetValueFromEnvL(iArguments[i]);
		}

	// Set options that haven't been set explicitly from environment variables if applicable.
	const TInt numOpts = iOptions.Count();
	for (TInt i = 0; i < numOpts; ++i)
		{
		SetValueFromEnvL(iOptions[i]);
		}
		
	if (!iArguments.AllSet())
		{
		User::Leave(KErrArgument);
		}
	}

TBool CCommandBase::CifReadRequired() const
	{
	if (iCif)
		{
		return EFalse;
		}

	// Call the virtual Description function to see if the sub-class has implemented it.
	// If not, it is assumed that it expects a CIF file to be read.
	Description();
	return (iFlags & EReadCif);
	}

void CCommandBase::ReadCifL()
	{
	TRAPD(err, DoReadCifL());
	if (err)
		{
		iFlags |= ECifReadFailed;
		}
	}

void CCommandBase::DoReadCifL()
	{
	iCif = CCommandInfoFile::NewL(FsL(), Env(), Name());
	iFlags |= EOwnsCif;
	}

EXPORT_C TBool CCommandBase::UsingCif() const
	{
	if (iCif)
		{
		return ETrue;
		}
	return (CifReadRequired());
	}

EXPORT_C void CCommandBase::ParseCommandLineL(const TDesC& aCommandLine)
	{
	TInt argumentIndex = 0;
	TLex lex(aCommandLine);
	lex.SkipSpace();
	TBool lastArgSeen(EFalse);
	while (!lex.Eos())
		{
		TBool isOption = EFalse;
		if (!lastArgSeen && (lex.Peek() == TChar('-')))
			{	// Start of an option.
			isOption = ETrue;
			lex.Get();
			TChar peek = lex.Peek();
			if (peek == TChar('-'))
				{	// Start of a long named option.
				lex.Get();
				lex.Mark();
				lex.SkipCharacters();
				TInt optionIndex = iOptions.Find(lex.MarkedToken());
				if (optionIndex < 0)
					{
					User::Leave(KErrArgument);
					}
				SetValueL(lex, iOptions[optionIndex]);
				}
			else if (peek.IsDigit())
				{
				// It could be a negative integer argument, providing there aren't any numeric short options.
				// If there are, they take precedence, so don't use a digit as a short option unless you don't care about negative arguments
				TInt optionIndex = iOptions.FindShort(peek);
				if (optionIndex < 0)
					{
					isOption = EFalse; // get to the argument clause below
					lex.UnGet(); // So the minus sign is picked up
					}
				}
			
			if (isOption)
				{	// Start of a short option (or block of short options).
				while (!lex.Eos() && !lex.Peek().IsSpace() && (lex.Peek() != TChar('-')))
					{
					TInt optionIndex = iOptions.FindShort(lex.Get());
					if (optionIndex < 0)
						{
						User::Leave(KErrArgument);
						}
					SetValueL(lex, iOptions[optionIndex]);
					}
				}
			}

		if (!isOption)
			{	// Start of an argument.
			if (argumentIndex < iArguments.Count())
				{
				TCommandArgument& argument = iArguments[argumentIndex];
				SetValueL(lex, argument);
				if (!(argument.ValueType() & KValueTypeFlagMultiple))
					{
					++argumentIndex;
					}
				lastArgSeen = (argument.ValueType() & KValueTypeFlagLast);
				// Look ahead to see if the next argument is the last, and if so set lastArgSeen early. This allows the next argument to be an option, but be treated as part of the 'last' string not an option for this command.
				if (!lastArgSeen && (argumentIndex < iArguments.Count()))
					{
					lastArgSeen = (iArguments[argumentIndex].ValueType() & KValueTypeFlagLast);
					}
				}
			else
				{
				User::Leave(KErrArgument);
				}
			}

		lex.SkipSpace();
		}
	}

const TDesC* CCommandBase::ReadCommandLineLC()
	{
#ifdef EKA2
	HBufC* commandLine = HBufC::NewLC(User::CommandLineLength());
	TPtr commandLinePtr(commandLine->Des());
	User::CommandLine(commandLinePtr);
#else
	RProcess process;
	HBufC* commandLine = HBufC::NewLC(process.CommandLineLength());
	TPtr commandLinePtr(commandLine->Des());
	process.CommandLine(commandLinePtr);
#endif
	return commandLine;
	}

void AppendHeadingL(const TDesC& aHeading, CTextBuffer& aBuffer)
	{
	_LIT(KFormat, "\r\n=head1 %S\r\n\r\n");
	aBuffer.AppendFormatL(KFormat, &aHeading);
	}

void AppendSubCommandL(TInt aIndent, CTextBuffer& aBuffer, const CCommandInfoFile& aCif)
	{
	aBuffer.AppendSpacesL(aIndent);
	aBuffer.AppendL(aCif.Name());
	aBuffer.AppendL(KNewLine);

	TInt numSubCommands = aCif.NumSubCommands();
	for (TInt i = 0; i < numSubCommands; ++i)
		{
		AppendSubCommandL(aIndent + 2, aBuffer, aCif.SubCommand(i));
		}
	}

EXPORT_C const CTextBuffer* CCommandBase::GetHelpTextL()
	{
	CTextBuffer* buffer = CTextBuffer::NewLC(0x100);

	AppendHeadingL(_L("SYNTAX"), *buffer);
	buffer->AppendFormatL(_L("    %S"), &Name());

	const TInt numOptions = iOptions.Count();
	if (numOptions > 0)
		{
		buffer->AppendL(_L(" [options]"));
		}

	const TInt numArguments = iArguments.Count();
	if (numArguments > 0)
		{
		for (TInt i = 0; i < numArguments; ++i)
			{
			const TCommandArgument& thisArgument = iArguments[i];
			TBool isOptional = thisArgument.IsOptional();
			if (isOptional)
				{
				buffer->AppendL(_L(" [<"));
				}
			else
				{
				buffer->AppendL(_L(" <"));
				}
			buffer->AppendL(thisArgument.Name());
			if (isOptional)
				{
				buffer->AppendL(_L(">]"));
				}
			else
				{
				buffer->AppendL(_L(">"));
				}

			if (thisArgument.AcceptsMultiple())
				{
				buffer->AppendL(_L(" ..."));
				}
			}
		}
	buffer->AppendL(_L("\r\n"));

	if (numOptions > 0)
		{
		AppendHeadingL(_L("OPTIONS"), *buffer);
		buffer->AppendL(_L("=over 5\r\n\r\n"));
		for (TInt i = 0; i < numOptions; ++i)
			{
			const TCommandOption& thisOption = iOptions[i];
			TChar shortName(thisOption.ShortName());
			if (shortName)
				{
				buffer->AppendFormatL(_L("=item -%c (--%S)"), (TUint)shortName, &thisOption.Name());
				switch (thisOption.ValueType() & KValueTypeFlagMask)
					{
					case KValueTypeBool:
						{
						// Do nothing.
						break;
						}
					case KValueTypeInt:
						{
						buffer->AppendL(_L(" <int>"));
						break;
						}
					case KValueTypeInt64:
						{
						buffer->AppendL(_L(" <int64>"));
						break;
						}
					case KValueTypeUint:
						{
						buffer->AppendL(_L(" <uint>"));
						break;
						}
					case KValueTypeUint64:
						{
						buffer->AppendL(_L(" <uint64>"));
						break;
						}
					case KValueTypeString:
						{
						buffer->AppendL(_L(" <string>"));
						break;
						}
					case KValueTypeFileName:
						{
						buffer->AppendL(_L(" <filename>"));
						break;
						}
					case KValueTypeEnum:
						{
						buffer->AppendL(_L(" <enum>"));
						break;
						}
					case KValueTypeReal:
						{
						buffer->AppendL(_L(" <real>"));
						break;
						}
					default:
						{
						__ASSERT_ALWAYS(EFalse, Panic(EInvalidCommandOptionType));
						break;
						}
					}
				buffer->AppendFormatL(_L("\r\n\r\n%S"), &thisOption.Description());
				if ((thisOption.ValueType() & KValueTypeFlagMask) == KValueTypeEnum)
					{
					if (thisOption.EnumDescriptionList().Length() > 0)
						{
						FormatEnumL(thisOption, *buffer);
						}
					else
						{
						buffer->AppendL(_L(" ["));
						thisOption.AppendEnumValuesL(*buffer);
						buffer->AppendL(']');
						}
					}
				if (thisOption.ValueType() & KValueTypeFlagMultiple)
					{
					buffer->AppendL(_L(" Can be specified more than once."));
					}
				if (thisOption.EnvVar().Length() > 0)
					{
					const TDesC& envVar = thisOption.EnvVar();
					buffer->AppendFormatL(_L(" Can also be specified by defining the environment variable \"%S\"."), &envVar);
					}
				buffer->AppendL(_L("\r\n\r\n"));
				}
			}
		buffer->AppendL(_L("=back\r\n"));
		}

	if (numArguments > 0)
		{
		AppendHeadingL(_L("ARGUMENTS"), *buffer);
		buffer->AppendL(_L("=over 5\r\n\r\n"));
		for (TInt i = 0; i < numArguments; ++i)
			{
			buffer->AppendL(_L("=item "));
			const TCommandArgument& thisArgument = iArguments[i];
			TBool isOptional = thisArgument.IsOptional();
			if (isOptional)
				{
				buffer->AppendL(_L("[<"));
				}
			else
				{
				buffer->AppendL(_L("<"));
				}
			buffer->AppendL(thisArgument.Name());
			if (isOptional)
				{
				buffer->AppendL(_L(">]\r\n\r\n"));
				}
			else
				{
				buffer->AppendL(_L(">\r\n\r\n"));
				}

			buffer->AppendL(thisArgument.Description());

			if (thisArgument.ValueType() & KValueTypeFlagMultiple)
				{
				buffer->AppendL(_L(" Can be specified more than once."));
				}

			if (thisArgument.EnvVar().Length() > 0)
				{
				const TDesC& envVar = thisArgument.EnvVar();
				buffer->AppendFormatL(_L(" Can also be specified by defining the environment variable \"%S\"."), &envVar);
				}

			if (thisArgument.IsLast())
				{
				buffer->AppendFormatL(_L(" Any further arguments or options will be coalesced into this one."));
				}

			TBool noClosingSquareBracket(EFalse);

			switch (thisArgument.ValueType() & KValueTypeFlagMask)
				{
				case KValueTypeInt:
					{
					buffer->AppendL(_L(" [int"));
					break;
					}
				case KValueTypeInt64:
					{
					buffer->AppendL(_L(" [int64"));
					break;
					}
				case KValueTypeUint:
					{
					buffer->AppendL(_L(" [uint"));
					break;
					}
				case KValueTypeUint64:
					{
					buffer->AppendL(_L(" [uint64"));
					break;
					}
				case KValueTypeString:
					{
					buffer->AppendL(_L(" [string"));
					break;
					}
				case KValueTypeFileName:
					{
					buffer->AppendL(_L(" [filename"));
					break;
					}
				case KValueTypeEnum:
					{
					if (thisArgument.EnumDescriptionList().Length() > 0)
						{
						FormatEnumL(thisArgument, *buffer);
						noClosingSquareBracket = ETrue;
						}
					else
						{
						buffer->AppendL(_L(" ["));
						thisArgument.AppendEnumValuesL(*buffer);
						}
					break;
					}
				case KValueTypeReal:
					{
					buffer->AppendL(_L(" [real"));
					break;
					}
				default:
					{
					__ASSERT_ALWAYS(EFalse, Panic(EInvalidCommandArgumentType));
					break;
					}
				}

			if (!noClosingSquareBracket)
				{
				if (thisArgument.ValueType() & KValueTypeFlagMultiple)
					{
					buffer->AppendL(_L("(s)]\r\n\r\n"));
					}
				else
					{
					buffer->AppendL(_L("]\r\n\r\n"));
					}
				}
			}
		buffer->AppendL(_L("=back\r\n"));
		}

	AppendHeadingL(_L("DESCRIPTION"), *buffer);
	buffer->AppendL(Description());
	buffer->AppendL(_L("\r\n\r\n"));
	if (iCif && iCif->LongDescription().Length())
		{
		buffer->AppendL(iCif->LongDescription());
		buffer->AppendL(_L("\r\n\r\n"));
		}

	if (iCif && iCif->SeeAlso().Length())
		{
		AppendHeadingL(_L("SEE ALSO"), *buffer);
		buffer->AppendL(iCif->SeeAlso());
		buffer->AppendL(_L("\r\n\r\n"));
		}

	if (iCif && (iCif->NumSubCommands() > 0))
		{
		AppendHeadingL(_L("SUB-COMMANDS"), *buffer);
		TInt numSubCommands = iCif->NumSubCommands();
		for (TInt i = 0; i < numSubCommands; ++i)
			{
			AppendSubCommandL(2, *buffer, iCif->SubCommand(i));
			}
		}

	if (iCif && iCif->Copyright().Length())
		{
		AppendHeadingL(_L("COPYRIGHT"), *buffer);
		buffer->AppendL(iCif->Copyright());
		buffer->AppendL(_L("\r\n\r\n"));
		}
	
	CTextFormatter* textFormatter = CTextFormatter::NewLC(Stdout());
	textFormatter->AppendPodL(buffer->Descriptor());
	CleanupStack::Pop(textFormatter);
	CleanupStack::PopAndDestroy(buffer);
	return textFormatter;
	}

void CCommandBase::DisplayHelpAndComplete()
	{
	const CTextBuffer* helpText = NULL;
	TRAPD(err, helpText = GetHelpTextL());
	if (err)
		{
		Complete(err, _L("Error displaying help"));
		}
	else
		{
		CleanupStack::PushL(const_cast<CTextBuffer*>(helpText));
		CheckNewLine();
		PageL(*helpText);
		Complete();
		CleanupStack::PopAndDestroy(const_cast<CTextBuffer*>(helpText));
		}
	}

void CCommandBase::DisplayHelpAndComplete(TInt aError, const TDesC& aText)
	{
	CheckNewLine();
	PrintError(aError, aText);
	const CTextBuffer* helpText = NULL;
	TRAPD(err, helpText = GetHelpTextL());
	if (err == KErrNone)
		{
		CleanupStack::PushL(const_cast<CTextBuffer*>(helpText));
		PageL(*helpText);
		CleanupStack::PopAndDestroy(const_cast<CTextBuffer*>(helpText));
		}
	Complete(aError);
	}

void CCommandBase::FormatErrorText(TDes& aDes, TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList) const
	{
	TOverflowTruncate overflow;
	aDes = _L("Error: ");
	aDes.AppendFormatList(aFmt, aList, &overflow);
	const TDesC* errorDesc = NULL;
	// See if we have an extension that knows about this error
	if (iExtension) errorDesc = iExtension->StringifyError(aError);
	// If the extension didn't know, fall back to Stringify::Error() to handle the system ones
	if (errorDesc == NULL) errorDesc = Stringify::Error(aError);
	aDes.AppendFormat(_L(" : %S (%d)\r\n"), &overflow, errorDesc, aError);
	}

void CCommandBase::FormatWarningText(TDes& aDes, TRefByValue<const TDesC> aFmt, VA_LIST& aList) const
	{
	TOverflowTruncate overflow;
	aDes = _L("Warning: ");
	aDes.AppendFormatList(aFmt, aList, &overflow);
	if ((aDes.Length() < KNewLine().Length()) || ((aDes.Right(KNewLine().Length()) != KNewLine)))
		{
		aDes.AppendFormat(KNewLine, &overflow);
		}
	}

void CCommandBase::CheckNewLine()
	{
	if (iStderr.AttachedToConsole())
		{
		TPoint cursorPos(0, 0);
		iStderr.GetCursorPos(cursorPos);
		if (cursorPos.iX != 0)
			{
			iStderr.Write(_L("\r\n"));
			}
		}
	}

void CCommandBase::FormatEnumL(const TValue& aValue, CTextBuffer& aBuffer) const
	{
	aBuffer.AppendL(_L(" Supported values are:\r\n\r\n=over 5\r\n\r\n"));
	TLex descLex(aValue.EnumDescriptionList());
	TLex valLex(aValue.EnumValueList());
	while (!valLex.Eos())
		{
		HBufC* value = ReadStringLC(valLex, EBreakOnComma);
		__ASSERT_ALWAYS(!descLex.Eos(), IoUtils::Panic(EEnumDescriptionMissing));
		HBufC* description = ReadStringLC(descLex, EBreakOnComma);
		aBuffer.AppendFormatL(_L("=item %S\r\n\r\n%S\r\n\r\n"), value, description);
		CleanupStack::PopAndDestroy(2, value);
		}
	aBuffer.AppendL(_L("=back\r\n\r\n"));
	}

EXPORT_C void CCommandBase::CreateEnvironmentL(CEnvironment* aEnv)
	{
	if (aEnv)
		{
		iEnv = aEnv;
		}
	else
		{
		iEnv = CEnvironment::NewL();
		iFlags |= EOwnsEnv;
#ifdef EKA2
		const TInt length = User::ParameterLength(KEnvironmentProcessSlot);
		if (length > 0)
			{
			HBufC8* buf = HBufC8::NewLC(length);
			TPtr8 bufPtr(buf->Des());
			User::LeaveIfError(User::GetDesParameter(KEnvironmentProcessSlot, bufPtr));
			iEnv->InternalizeL(bufPtr);
			CleanupStack::PopAndDestroy(buf);
			}
#endif
		}
	}

EXPORT_C void CCommandBase::RunL()
	{
	if (iFlags & ECompleteOnRunL)
		{
		Complete(iStatus.Int());
		}
	else
		{
		// if a derived class ever makes this Active Object active, it must also override this unless ECompleteOnRunL is specified.
		Panic(EActiveObjectRunLNotOverridden);
		}
	}

EXPORT_C void CCommandBase::DoCancel()
	{
	// if a derived class ever makes this Active Object active, it must also override this.
	Panic(EActiveObjectRunLNotOverridden);
	}

EXPORT_C TInt CCommandBase::RunError(TInt aError)
	{
	HandleLeave(aError);
	return KErrNone;
	}

EXPORT_C CConsoleBase* CCommandBase::GetConsoleBaseL()
	{
	CIoConsole* console = new(ELeave) CIoConsole(Stdin(), Stdout());
	return console;
	}

EXPORT_C CColorConsoleBase* CCommandBase::GetColorConsoleBaseL()
	{
	// CIoConsole now derives from CColorConsoleBase so implementation is the same as GetConsoleBaseL
	CIoConsole* console = new(ELeave) CIoConsole(Stdin(), Stdout());
	return console;
	}

EXPORT_C CCommandBase& CCommandBase::Static()
	{
	CCommandBase* self = static_cast<CCommandBase*>(Dll::Tls());
	__ASSERT_ALWAYS(self, Panic(ENoStaticCommand));
	return *self;
	}
	
EXPORT_C TBool CCommandBase::HaveStatic()
	{
	CCommandBase* self = static_cast<CCommandBase*>(Dll::Tls());
	return (self != NULL);
	}
	
EXPORT_C void CCommandBase::StdinChange(TUint /*aChange*/)
	{
	}

EXPORT_C void CCommandBase::DisplayHelp()
	{
	const CTextBuffer* helpText = NULL;
	TRAPD(err, helpText = GetHelpTextL());
	if (err)
		{
		PrintError(err, _L("Error displaying help"));
		}
	else
		{
		CleanupStack::PushL(const_cast<CTextBuffer*>(helpText));
		CheckNewLine();
		PageL(*helpText);
		CleanupStack::PopAndDestroy(const_cast<CTextBuffer*>(helpText));
		}
	}

namespace IoUtils
	{
	class MLineReader
		{
	public:
		virtual TInt GetNextLine(CTextBuffer& aText) = 0;
		virtual void InitializeL() {}
		};

	NONSHARABLE_CLASS(TTextBufferLineReader) : public MLineReader
		{
	public:
		TTextBufferLineReader(const CTextBuffer& aBuffer);
	public: // From MLineReader.
		virtual TInt GetNextLine(CTextBuffer& aText);
	private:
		const CTextBuffer& iBuffer;
		TInt iPos;
		};

	NONSHARABLE_CLASS(TReadHandleLineReader) : public MLineReader
		{
	public:
		TReadHandleLineReader(RIoReadHandle& aHandle);
	public: // From MLineReader.
		virtual void InitializeL();
		virtual TInt GetNextLine(CTextBuffer& aText);
	private:
		RIoReadHandle& iHandle;
		TBuf<512> iLineBuf; // Note, the code in CCommandBase::PageL(MLineReader&) doesn't properly handle the situation where a given line of input is longer than this buffer.
		};
	}

TTextBufferLineReader::TTextBufferLineReader(const CTextBuffer& aBuffer)
	: iBuffer(aBuffer), iPos(0)
	{
	}

TInt TTextBufferLineReader::GetNextLine(CTextBuffer& aText)
	{
	aText.Zero();
	TInt newLinePos = iBuffer.Descriptor().Mid(iPos).Find(KNewLine);
	if (newLinePos < 0)
		{
		if (iPos < iBuffer.Length())
			{
			aText.AppendL(iBuffer, iPos);
			return KErrNone;
			}
		return KErrNotFound;
		}
	else
		{
		const TInt lineLength = newLinePos + KNewLine().Length();
		aText.AppendL(iBuffer, iPos, lineLength);
		iPos += lineLength;
		return KErrNone;
		}
	}

TReadHandleLineReader::TReadHandleLineReader(RIoReadHandle& aHandle)
	: iHandle(aHandle)
	{
	}

void TReadHandleLineReader::InitializeL()
	{
	iHandle.SetReadModeL(RIoReadHandle::ELine);
	}

TInt TReadHandleLineReader::GetNextLine(CTextBuffer& aText)
	{
	aText.Zero();
	TInt err = iHandle.Read(iLineBuf);
	if (err == KErrNone)
		{
		aText.AppendL(iLineBuf);
		}
	else if (err == KErrEof)
		{
		err = KErrNotFound;
		}
	return err;
	}


EXPORT_C void CCommandBase::PageL(const CTextBuffer& aText)
	{
	TTextBufferLineReader lineReader(aText);
	lineReader.InitializeL();
	PageL(lineReader);
	}

EXPORT_C void CCommandBase::PageL(RIoReadHandle& aInput)
	{
	TReadHandleLineReader lineReader(aInput);
	lineReader.InitializeL();
	PageL(lineReader);
	}

void CCommandBase::PageL(MLineReader& aLineReader)
	{
	_LIT(KPrompt, "--More--");
	_LIT(KRemovePrompt, "\x08\x08\x08\x08\x08\x08\x08\x08        \x08\x08\x08\x08\x08\x08\x08\x08");

	if (iStdout.AttachedToConsole())
		{
		RIoConsole console;
		console.OpenL(IoSession(), iStdout);
		CleanupClosePushL(console);
		RIoConsoleReadHandle consReader;
		consReader.CreateL(IoSession());
		CleanupClosePushL(consReader);
		console.AttachL(consReader, RIoEndPoint::EForeground);
		consReader.CaptureAllKeysL();
		TSize consoleSize = iStdout.GetScreenSizeL();
		CTextBuffer* lineBuf = CTextBuffer::NewLC(0x100);
		TInt linesPrinted = 0;
		TBool finished(EFalse);
		while (!finished)
			{
			if (linesPrinted < (consoleSize.iHeight - 1))
				{
				if (lineBuf->Length() == 0)
					{
					TInt err = aLineReader.GetNextLine(*lineBuf);
					if (err == KErrNotFound)
						{
						finished = ETrue;
						}
					else if (err)
						{
						User::Leave(err);
						}
					}

				if (!finished && lineBuf->Length())
					{
					const TInt lineLength = lineBuf->Length();
					const TInt numConsoleLines = ((lineLength / consoleSize.iWidth) + ((lineLength % consoleSize.iWidth) ? 1 : 0));
					const TInt numFreeLines = consoleSize.iHeight - linesPrinted - 1;
					TInt lengthToWrite;
					if (numConsoleLines < numFreeLines)
						{
						lengthToWrite = lineLength;
						}
					else
						{
						lengthToWrite = Min(lineLength, numFreeLines * consoleSize.iWidth);
						}

					lineBuf->Write(iStdout, 0, lengthToWrite);
					linesPrinted += ((lengthToWrite / consoleSize.iWidth) + ((lengthToWrite % consoleSize.iWidth) ? 1 : 0));
					lineBuf->Delete(0, lengthToWrite);

					if (lineBuf->Length() == 0)
						{
						TInt err = aLineReader.GetNextLine(*lineBuf);
						if (err == KErrNotFound)
							{
							finished = ETrue;
							}
						else if (err)
							{
							User::Leave(err);
							}
						}
					}
				}
			else
				{
				iStdout.Write(KPrompt);
				switch ((TInt)consReader.ReadKey())
					{
					case 'q':
					case 'Q':
					case EKeyNull:
						{
						finished = ETrue;
						break;
						}
					case EKeyEnter:
						{
						--linesPrinted;
						break;
						}
					default:
						{
						linesPrinted = 0;
						break;
						}
					}
				iStdout.Write(KRemovePrompt);
				}
			}
		CleanupStack::PopAndDestroy(3, &console);
		}
	else
		{
		// Not attached to a console, so just copy stdin to stdout.
		CTextBuffer* lineBuf = CTextBuffer::NewLC(0x100);
		while (aLineReader.GetNextLine(*lineBuf) == KErrNone)
			{
			iStdout.Write(lineBuf->Descriptor());
			}
		CleanupStack::PopAndDestroy(lineBuf);
		}
	}

EXPORT_C const TDesC& CCommandBase::Description() const
	{
	if (iCif == NULL)
		{
		const_cast<CCommandBase*>(this)->iFlags |= EReadCif;
		return KNullDesC; // Dummy return value.
		}
	return iCif->ShortDescription();
	}
	
EXPORT_C const TDesC* CCommandBase::ConsoleImplementation()
	{
	return iConsoleImplementation;
	}

EXPORT_C void CCommandBase::SetErrorReported(TBool aReported)
	{
	if (aReported)
		{
		iFlags |= EErrorReported;
		}
	else
		{
		iFlags &= ~EErrorReported;
		}
	}

EXPORT_C void CCommandBase::StartSupressingErrors()
	{
	iSupressingErrors++;
	}

EXPORT_C void CCommandBase::StopSupressingErrors()
	{
	iSupressingErrors--;
	}

EXPORT_C TBool CCommandBase::IsComplete() const
	{
	return (iFlags & ECompleted);
	}

EXPORT_C TInt CCommandBase::CompletionReason() const
	{
	__ASSERT_ALWAYS(IsComplete(), Panic(ENotYetComplete));
	return iCompletionReason;
	}

HBufC* CCommandBase::ReadStringLC(TLex& aLex, TUint aOptions) const
	{
	while (!aLex.Eos() && (aLex.Peek().IsSpace() || ((aOptions & EBreakOnComma) && (aLex.Peek() == ','))))
		{
		aLex.Inc();
		}

	if (aLex.Eos())
		{
		// There aren't any characters at all. This can be considered a syntax error because if an empty string was intended
		// it should have been written as "" or ''.
		CommandLeaveIfErr(*const_cast<CCommandBase*>(this), KErrArgument, _L("Missing argument"));
		}

	TChar c = aLex.Peek();
	TBool quoted = ((c == '\'') || (c == '"') || (c == iEnv->EscapeChar()));
	TUint readMode = 0;
	if (aOptions & EBreakOnComma)
		{
		readMode |= Stringify::EBreakOnComma;
		}
	HBufC* string = Stringify::ReadLC(aLex, iEnv->EscapeChar(), *const_cast<CCommandBase*>(this), readMode);
	if ((aOptions & EDisallowLeadingHyphen) && (string->Length() > 0) && ((*string)[0] == '-') && !quoted)
		{
		CommandLeaveIfErr(*const_cast<CCommandBase*>(this), KErrArgument, _L("Argument \"%S\" appears to be an option - probable missing argument"), string);
		}
	return string;
	}

EXPORT_C void RChildProcess::Close()
	{
	iStderr.Close();
	iStdout.Close();
	iStdin.Close();
	iProcess.Close();
	}

void SetIoObjectName(RIoSession& aIoSession, TInt aObjHandle, TRefByValue<const TDesC> aFmt, ...)
	{
	TOverflowTruncate overflow;
	VA_LIST list;
	VA_START(list, aFmt);
	TFullName name;
	name.AppendFormatList(aFmt, list, &overflow);
	VA_END(list);
	aIoSession.SetObjectName(aObjHandle, name);
	}

EXPORT_C void RChildProcess::CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr)
	{
	DoCreateL(aExecutableName, aCommandLine, aIoSession, aStdin, aStdout, aStderr, NULL);
	}

EXPORT_C void RChildProcess::CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const CEnvironment& aEnv)
	{
	DoCreateL(aExecutableName, aCommandLine, aIoSession, aStdin, aStdout, aStderr, &aEnv);
	}
	
EXPORT_C void RChildProcess::CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoConsole& aConsole, const CEnvironment* aEnv)
	{
	CleanupClosePushL(*this);
	iStdin.CreateL(aIoSession);
	iStdout.CreateL(aIoSession);
	iStderr.CreateL(aIoSession);
	
	NameHandles(aExecutableName, aIoSession);
	
	aConsole.AttachL(iStdin, RIoEndPoint::EForeground);
	iStdin.SetUnderlyingConsoleL(aConsole);
	aConsole.AttachL(iStdout);
	iStdout.SetUnderlyingConsoleL(aConsole);
	aConsole.AttachL(iStderr);
	iStderr.SetUnderlyingConsoleL(aConsole);
	
	ProcessCreateL(aExecutableName, aCommandLine, aEnv);
	
	CleanupStack::Pop();
	}

void RChildProcess::NameHandles(const TDesC& aExecutableName, RIoSession& aIoSession)
	{
	// Give the handles a name to aid debugging (makes the iosvr logs easier to understand).
	SetIoObjectName(aIoSession, iStdin.SubSessionHandle(), _L("%S_stdin"), &aExecutableName);
	SetIoObjectName(aIoSession, iStdout.SubSessionHandle(), _L("%S_stdout"), &aExecutableName);
	SetIoObjectName(aIoSession, iStderr.SubSessionHandle(), _L("%S_stderr"), &aExecutableName);
	}

void RChildProcess::DoCreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const CEnvironment* aEnv)
	{
	CleanupClosePushL(*this);

	// Create a set of I/O handles for the new process.
	User::LeaveIfError(iStdin.Create(aIoSession));
	User::LeaveIfError(iStdout.Create(aIoSession));
	User::LeaveIfError(iStderr.Create(aIoSession));
	
	NameHandles(aExecutableName, aIoSession);

	// Make the newly created handles duplicates of the ones that were passed in.
	// This makes them attached to the same I/O end-points (consoles, files, etc)
	// as the passed in handles.
	User::LeaveIfError(iStdin.Duplicate(aStdin));
	// iStdin.SetToForeground() moved to Run() so the child doesn't steal foreground until it actually runs
	User::LeaveIfError(iStdout.Duplicate(aStdout));
	User::LeaveIfError(iStderr.Duplicate(aStderr));

	// Create the process.
	ProcessCreateL(aExecutableName, aCommandLine, aEnv);
	
	CleanupStack::Pop(this);	
	}
	
void RChildProcess::ProcessCreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, const CEnvironment* aEnv)
	{
	User::LeaveIfError(iProcess.Create(aExecutableName, aCommandLine));

#ifdef EKA2
	if (aEnv)
		{
		// Attach a copy of aEnv as a parameter.
		HBufC8* envBuf = aEnv->ExternalizeLC();
		User::LeaveIfError(iProcess.SetParameter(IoUtils::KEnvironmentProcessSlot, *envBuf));
		CleanupStack::PopAndDestroy(envBuf);
		}
#endif

	// Make the main thread of the new process the owner of the I/O handles.
	// This makes it possible for the new process to subsequently open them and
	// avoid having to create a new set (which would prevent end-point sharing).
	TFullName fullName(iProcess.Name());
	_LIT(KThreadName,"::Main");
	fullName.Append(KThreadName);
	RThread thread;
	User::LeaveIfError(thread.Open(fullName));
	TThreadId threadId(thread.Id());
	thread.Close();
	User::LeaveIfError(iStdin.SetOwner(threadId));
	User::LeaveIfError(iStdout.SetOwner(threadId));
	User::LeaveIfError(iStderr.SetOwner(threadId));
	}

EXPORT_C void RChildProcess::Run(TRequestStatus& aStatus)
	{
	iStdin.SetToForeground();
	iProcess.Logon(aStatus);
	if (aStatus != KRequestPending)
		{
		iProcess.Kill(KErrAbort);
		}
	else
		{
		iProcess.Resume();
		}
	}
	
EXPORT_C void RChildProcess::Detach()
	{
	iProcess.Resume();
	}

EXPORT_C RProcess& RChildProcess::Process()
	{
	return iProcess;
	}

EXPORT_C const RProcess& RChildProcess::Process() const
	{
	return iProcess;
	}

// Really should define a standard CASE_LIT macro that every compiler can handle...
#include <fshell/descriptorutils.h>
#define CASE_RETURN_LIT(x) case x: { static const LtkUtils::SLitC KName = DESC(#x); return &KName; }
#define CASE_RETURN_LIT2(XXX, YYY) case XXX: { _LIT(_KLit, YYY); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }
	
EXPORT_C const TDesC* Stringify::Error(TInt aError)
	{
	enum TThingsNotAvailableEverywhere
		{
		KErrNoSecureTime = -49,
		KErrCorruptSurrogateFound = -50,
		};

	switch (aError)
		{
		CASE_RETURN_LIT(KErrNone);
		CASE_RETURN_LIT(KErrNotFound);
		CASE_RETURN_LIT(KErrGeneral);
		CASE_RETURN_LIT(KErrCancel);
		CASE_RETURN_LIT(KErrNoMemory);
		CASE_RETURN_LIT(KErrNotSupported);
		CASE_RETURN_LIT(KErrArgument);
		CASE_RETURN_LIT(KErrTotalLossOfPrecision);
		CASE_RETURN_LIT(KErrBadHandle);
		CASE_RETURN_LIT(KErrOverflow);
		CASE_RETURN_LIT(KErrUnderflow);
		CASE_RETURN_LIT(KErrAlreadyExists);
		CASE_RETURN_LIT(KErrPathNotFound);
		CASE_RETURN_LIT(KErrDied);
		CASE_RETURN_LIT(KErrInUse);
		CASE_RETURN_LIT(KErrServerTerminated);
		CASE_RETURN_LIT(KErrServerBusy);
		CASE_RETURN_LIT(KErrCompletion);
		CASE_RETURN_LIT(KErrNotReady);
		CASE_RETURN_LIT(KErrUnknown);
		CASE_RETURN_LIT(KErrCorrupt);
		CASE_RETURN_LIT(KErrAccessDenied);
		CASE_RETURN_LIT(KErrLocked);
		CASE_RETURN_LIT(KErrWrite);
		CASE_RETURN_LIT(KErrDisMounted);
		CASE_RETURN_LIT(KErrEof);
		CASE_RETURN_LIT(KErrDiskFull);
		CASE_RETURN_LIT(KErrBadDriver);
		CASE_RETURN_LIT(KErrBadName);
		CASE_RETURN_LIT(KErrCommsLineFail);
		CASE_RETURN_LIT(KErrCommsFrame);
		CASE_RETURN_LIT(KErrCommsOverrun);
		CASE_RETURN_LIT(KErrCommsParity);
		CASE_RETURN_LIT(KErrTimedOut);
		CASE_RETURN_LIT(KErrCouldNotConnect);
		CASE_RETURN_LIT(KErrCouldNotDisconnect);
		CASE_RETURN_LIT(KErrDisconnected);
		CASE_RETURN_LIT(KErrBadLibraryEntryPoint);
		CASE_RETURN_LIT(KErrBadDescriptor);
		CASE_RETURN_LIT(KErrAbort);
		CASE_RETURN_LIT(KErrTooBig);
		CASE_RETURN_LIT(KErrDivideByZero);
		CASE_RETURN_LIT(KErrBadPower);
		CASE_RETURN_LIT(KErrDirFull);
		CASE_RETURN_LIT(KErrHardwareNotAvailable);
#ifdef EKA2
		CASE_RETURN_LIT(KErrSessionClosed);
		CASE_RETURN_LIT(KErrPermissionDenied);
		CASE_RETURN_LIT(KErrExtensionNotSupported);
		CASE_RETURN_LIT(KErrCommsBreak);
		CASE_RETURN_LIT(KErrNoSecureTime);
		CASE_RETURN_LIT(KErrCorruptSurrogateFound);
#endif
		DEFAULT_RETURN_LIT("*** ERROR UNKNOWN ***");
		}
	}

EXPORT_C void IoUtils::ReadL(TDes& aData)
	{
	CCommandBase::Static().ReadL(aData);
	}

EXPORT_C void IoUtils::Write(const TDesC& aData)
	{
	CCommandBase::Static().Write(aData);
	}

EXPORT_C void IoUtils::Printf(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	CCommandBase::Static().PrintList(aFmt, list);
	VA_END(list);
	}

EXPORT_C void IoUtils::Printf(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	CCommandBase::Static().PrintList(aFmt, list);
	VA_END(list);
	}

EXPORT_C void IoUtils::PrintError(TInt aError, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	CCommandBase::Static().PrintErrorList(aError, aFmt, list);
	VA_END(list);
	}

EXPORT_C void IoUtils::PrintWarning(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	CCommandBase::Static().PrintWarningList(aFmt, list);
	VA_END(list);
	}


//
// CCommandCompleter.
//

CCommandCompleter::CCommandCompleter(IoUtils::CCommandBase& aCmd)
	: CActive(CActive::EPriorityStandard), iCmd(aCmd)
	{
	CActiveScheduler::Add(this);
	}

CCommandCompleter::~CCommandCompleter()
	{
	Cancel();
	delete iTextBuffer;
	}

void CCommandCompleter::Complete(TInt aError)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aError);
	SetActive();
	}

void CCommandCompleter::Complete(TInt aError, const TDesC& aText, RIoWriteHandle& aWhere)
	{
	TOverflowTruncate overflow;
	iBuf.AppendFormat(aText, &overflow);
	iError = aError;
	aWhere.Write(iBuf, iStatus);
	SetActive();
	}
	
void CCommandCompleter::Complete(TInt aError, const CTextBuffer* aTextBuffer, RIoWriteHandle& aWhere)
	{
	ASSERT(iTextBuffer == NULL);
	iTextBuffer = aTextBuffer;
	iError = aError;
	iTextBuffer->Write(aWhere, iStatus);
	SetActive();
	}

void CCommandCompleter::RunL()
	{
	iCmd.CompleteCallback(iError ? iError : iStatus.Int());
	}

void CCommandCompleter::DoCancel()
	{
	}
	

//
// CReaderChangeNotifier.
//

CReaderChangeNotifier::CReaderChangeNotifier(IoUtils::CCommandBase& aCmd, RIoReadHandle& aReadHandle)
	: CActive(CActive::EPriorityStandard)
	, iCmd(aCmd)
	, iReadHandle(aReadHandle)
	{
	CActiveScheduler::Add(this);
	}

CReaderChangeNotifier::~CReaderChangeNotifier()
	{
	Cancel();
	}

void CReaderChangeNotifier::Notify()
	{
	iReadHandle.NotifyChange(iChangeType, iStatus);
	SetActive();
	}

void CReaderChangeNotifier::RunL()
	{
	if (iStatus.Int()==KErrNone)
		{
		TUint change = iChangeType();
		Notify();
		iCmd.StdinChange(change);
		}
	}

void CReaderChangeNotifier::DoCancel()
	{
	iReadHandle.CancelNotifyChange();
	}

//
// CKeypressWatcher
// 

CKeypressWatcher::CKeypressWatcher(IoUtils::MCommandExtensionsV2& aCmd, RIoConsoleReadHandle& aReadHandle)
	: CActive(CActive::EPriorityStandard), iCmd(aCmd), iReadHandle(aReadHandle)
	{
	CActiveScheduler::Add(this);
	}

CKeypressWatcher::~CKeypressWatcher()
	{
	Cancel();
	}

void CKeypressWatcher::Notify()
	{
	iReadHandle.WaitForKey(iStatus);
	SetActive();
	}

void CKeypressWatcher::RunL()
	{
	TUint code = iReadHandle.KeyCode();
	TUint modifiers = iReadHandle.KeyModifiers();
	Notify();
	iCmd.KeyPressed(code, modifiers);
	if (code == CTRL('c'))
		{
		iCmd.CtrlCPressed();
		}
	}

void CKeypressWatcher::DoCancel()
	{
	iReadHandle.WaitForKeyCancel();
	}

//

EXPORT_C TCommandExtensionVersion MCommandExtensionsV1::ExtensionVersion() const
	{
	return ECommandExtensionV1;
	}

EXPORT_C const TDesC* MCommandExtensionsV1::StringifyError(TInt /*aError*/) const
	{
	return NULL; // By default we don't have anything additional
	}

EXPORT_C void CCommandBase::SetExtension(MCommandExtensionsV1* aExtension)
	{
	iExtension = aExtension;
	}

EXPORT_C TCommandExtensionVersion MCommandExtensionsV2::ExtensionVersion() const
	{
	return ECommandExtensionV2;
	}

EXPORT_C void MCommandExtensionsV2::KeyPressed(TUint /*aKeyCode*/, TUint /*aModifiers*/)
	{
	// Default is to do nothing
	}

EXPORT_C void MCommandExtensionsV2::CtrlCPressed()
	{
	// Default is to do nothing
	}
