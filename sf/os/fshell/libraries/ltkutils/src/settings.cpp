// settings.cpp
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
#include <fshell/settings.h>

using namespace LtkUtils;
	
void Panic(TSettingPanic aReason)
	{
	User::Panic(KSettingPanic, aReason);
	}

void RemoveSpaces(TDes& aDes)
	{
	for (TInt i=aDes.Length()-1; i>=0; --i)
		{
		if (TChar(aDes[i]).IsSpace()) aDes.Delete(i, 1);
		}
	}
	
/*
List of valid values for a boolean. Values interpreted as false, true, false, true, etc.
i.e. even poitions are false, odd true.
*/
_LIT(KBoolValues, "0,1,off,on,no,yes,false,true");
	

void HandleLineL(const TDesC& aLine, MValueHandler& aValueHandler, TErrorContext aErrorContext)
	{
	if (aLine.Length() == 0) return;
	if (aLine[0] == '#') // comment
		{
		aValueHandler.HandleCommentL(aLine.Mid(1), aErrorContext);
		}
	else
		{
		TLex lex(aLine);
		lex.SkipSpaceAndMark();
		lex.SkipCharacters();
		TPtrC id(lex.MarkedToken());
		lex.SkipSpace();
		TPtrC value(lex.Remainder());
		if (id.Length())
			{
			aValueHandler.HandleValueL(id, value, aErrorContext, EFalse);
			}
		}
	}
	

	
_LIT(KNewLine, "\n");

EXPORT_C void LtkUtils::ReadIniFileL(const TDesC& aFilename, MValueHandler& aValueHandler, TErrorContext aErrorContext, TFileNotFoundAction aNotFoundAction)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	IoUtils::TFileName2 fn(aFilename);
	TInt err = fn.FindFile(fs);
	if ((err == KErrNotFound) && (aNotFoundAction == ESucceedIfFileNotFound))
		{
		CleanupStack::PopAndDestroy(&fs);
		return;
		}
	StaticLeaveIfErr(err, _L("%SCannot find file '%S'"), &aErrorContext, &fn);
	
	RFile file;
	StaticLeaveIfErr(file.Open(fs, fn, EFileRead | EFileShareReadersOnly), _L("%SCannot open file '%S'"), &aErrorContext, &fn);
	CleanupClosePushL(file);
	
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x40);
	
	TErrorContext context(fn);
	TBuf8<0x40> readBuf;
	do
		{
		TInt lineEnd;
		do 
			{
			const TDesC& des(buf->Descriptor());
			lineEnd = des.Find(KNewLine);
			if (lineEnd != KErrNotFound)
				{
				TPtrC line(des.Left(lineEnd));
				if (line.Length() > 0)
					{
					if (line[line.Length()-1] == '\r') line.Set(line.Left(line.Length()-1));
					HandleLineL(line, aValueHandler, context);					
					}
				buf->Delete(0, lineEnd+1);
				context.NextLine();
				}
			} while (lineEnd != KErrNotFound);
		
		
		User::LeaveIfError(file.Read(readBuf));
		buf->AppendL(readBuf);
		}
	while (readBuf.Length() > 0);
	
	
	CleanupStack::PopAndDestroy(3, &fs); // buf, file, fs
	}
	
EXPORT_C void LtkUtils::ReadIniFileL(RIoReadHandle& aReadHandle, MValueHandler& aValueHandler)
	{
	RBuf lineBuf;
	lineBuf.CreateL(0x100);
	CleanupClosePushL(lineBuf);
	aReadHandle.SetReadModeL(RIoReadHandle::ELine);
	TName name;
	aReadHandle.ObjectNameL(name);
	
	TInt err;
	TErrorContext context(name);
	while ((err = aReadHandle.Read(lineBuf))==KErrNone)
		{
		HandleLineL(lineBuf, aValueHandler, context);
		context.NextLine();
		}
	if (err!=KErrEof) User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(&lineBuf);
	}
	
TInt ValueLineNumberOrder(const CValue& aRodger, const CValue& aMinnie)
	{
	return aRodger.LineNumber() - aMinnie.LineNumber();
	}
	
EXPORT_C void LtkUtils::WriteIniFileL(const TDesC& aFilename, CIniReader& aValues)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	StaticLeaveIfErr(file.Replace(fs, aFilename, EFileWrite | EFileShareExclusive), _L("Cannot open '%S' for writing"), &aFilename);
	CleanupClosePushL(file);
	

	IoUtils::CTextFormatter* formatter = IoUtils::CTextFormatter::NewLC(KMaxTInt);
	aValues.AppendFirstLineL(formatter);

	RPointerArray<CValue> values;
	CleanupClosePushL(values);
	aValues.GetValuesL(values);
	values.Sort(TLinearOrder<CValue>(ValueLineNumberOrder));
	
	IoUtils::CTextBuffer* buf = IoUtils::CTextBuffer::NewLC(0x20);	
	for (TInt i=0; i<values.Count(); ++i)
		{
		buf->AppendFormatL(_L("%S\t%S\r\n"), &values[i]->Id(), &values[i]->Value());
		}

	formatter->TabulateL(0, 4, buf->Descriptor(), IoUtils::EIgnoreAvailableWidth);
	
	User::LeaveIfError(file.Write(formatter->Collapse()));
	
	CleanupStack::PopAndDestroy(5, &fs); // fs, file, values, buf, formatter
	}
	
//______________________________________________________________________________
//						TErrorContext
EXPORT_C TErrorContext::TErrorContext()
	: iFilename(KNullDesC), iLineNumber(0)
	{
	}

EXPORT_C TErrorContext::TErrorContext(const TDesC& aFilename)
	: iFilename(aFilename), iLineNumber(1)
	{
	}

EXPORT_C void TErrorContext::NextLine()
	{
	iLineNumber++;
	}

EXPORT_C TInt TErrorContext::LineNumber()
	{
	return iLineNumber;
	}

EXPORT_C const TDesC& TErrorContext::StringLC()
	{
	_LIT(KErrorContextFmt, "%S:%d: ");
	if (iFilename.Length() && iLineNumber>0)
		{
		HBufC* string = HBufC::NewLC(iFilename.Length()+0x10);
		string->Des().AppendFormat(KErrorContextFmt, &iFilename, iLineNumber);
		return *string;
		}
	else
		{
		CleanupStack::PushL((void*)NULL);
		return KNullDesC;
		}
	
	}

	
//______________________________________________________________________________
//						CValue
EXPORT_C CValue* CValue::NewL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext)
	{
	CValue* self = CValue::NewLC(aId, aValue, aErrorContext);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CValue* CValue::NewLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext)
	{
	CValue* self = new(ELeave)CValue(aErrorContext);
	CleanupStack::PushL(self);
	self->ConstructL(aId, aValue);
	return self;
	}

EXPORT_C CValue::~CValue()
	{
	iId.Close();
	iValue.Close();
	}

EXPORT_C const TDesC& CValue::Id() const
	{
	return iId;
	}

EXPORT_C const TDesC& CValue::Value() const
	{
	return iValue;
	}

EXPORT_C void CValue::SetL(const TDesC& aNewValue, TErrorContext aErrorContext)
	{
	if (iValue.MaxLength() < aNewValue.Length())
		{
		RBuf newValue;
		newValue.CreateL(aNewValue.Length());
		newValue.Swap(iValue);
		newValue.Close();
		}
	if (aErrorContext.LineNumber() > 0) iLineNumber = aErrorContext.LineNumber();
	iValue.Copy(aNewValue);
	}
	
EXPORT_C TInt CValue::LineNumber() const
	{
	return iLineNumber;
	}
	
EXPORT_C TInt CValue::AsInt(TInt& aInt) const
	{
	TLex lex(iValue);
	TInt err = lex.Val(aInt);
	if (err == KErrNone)
		{
		if (lex.Remainder().Length()) err = KErrArgument;
		}
	return err;
	}

EXPORT_C TInt CValue::AsBool(TBool& aBool) const
	{
	IoUtils::TEnum truth(KBoolValues);
	TInt i;
	TInt err = truth.Parse(iValue, i);
	if (err==KErrNone)
		{
		aBool = i & 0x01;
		}
	return err;
	}
	
EXPORT_C TInt CValue::AsIntL() const
	{
	TInt i;
	StaticLeaveIfErr(AsInt(i), _L("'%S' value '%S' is not a valid integer"), &iId ,&iValue);
	return i;
	}
	
EXPORT_C TBool CValue::AsBoolL() const
	{
	TBool b;
	StaticLeaveIfErr(AsBool(b), _L("'%S' value '%S' is not a valid boolean"), &iId, &iValue);
	return b;
	}


CValue::CValue(TErrorContext aErrorContext)
	: iLineNumber(aErrorContext.LineNumber())
	{
	}

void CValue::ConstructL(const TDesC& aId, const TDesC& aValue)
	{
	iId.CreateL(aId);
	iValue.CreateL(aValue);
	}
	
//______________________________________________________________________________
//						CIniReader
EXPORT_C CIniReader* CIniReader::NewL(const TDesC& aFilename, TErrorContext aErrorContext)
	{
	CIniReader* self = new(ELeave)CIniReader();
	CleanupStack::PushL(self);
	ReadIniFileL(aFilename, *self, aErrorContext, EFailIfFileNotFound);
	CleanupStack::Pop(self);
	return self;
	}
	
CIniReader::CIniReader()
	{
	}

EXPORT_C CIniReader::~CIniReader()
	{
	TPtrHashMapIter<TDesC, CValue> iter(iValues);
	while (iter.NextValue())
		{
		const CValue* val = iter.CurrentValue();
		iter.RemoveCurrent();
		delete val;
		}
	iValues.Close();
	iFirstLineComment.Close();
	}

EXPORT_C const TDesC* CIniReader::GetValue(const TDesC& aId) const
	{
	const CValue* value = iValues.Find(aId);
	if (value)
		{
		return &value->Value();
		}
	else
		{
		return NULL;
		}
	}


EXPORT_C void CIniReader::GetValuesL(RPointerArray<CValue>& aValues)
	{
	TPtrHashMapIter<TDesC, CValue> iter(iValues);
	while (iter.NextValue())
		{
		if (IncludeValue(iter.CurrentValue())) aValues.AppendL(iter.CurrentValue());
		}
	}
	
EXPORT_C void CIniReader::GetValuesL(RPointerArray<const CValue>& aValues) const
	{
	TPtrHashMapIter<TDesC, CValue> iter(iValues);
	while (iter.NextValue())
		{
		if (IncludeValue(iter.CurrentValue())) aValues.AppendL(iter.CurrentValue());
		}
	}

	
EXPORT_C void CIniReader::GetIdsL(RArray<const TPtrC>& aIds) const
	{
	TPtrHashMapIter<TDesC, CValue> iter(iValues);
	while (iter.NextValue())
		{
		aIds.AppendL(*iter.CurrentKey());
		}
	}
	
EXPORT_C void CIniReader::SetValueL(const TDesC& aId, const TDesC& aValue)
	{
	CValue* value = iValues.Find(aId);
	if (value)
		{
		value->SetL(aValue, TErrorContext());
		}
	else
		{
		HandleValueL(aId, aValue, TErrorContext(), ETrue);
		}
	}
	
EXPORT_C void CIniReader::RemoveValueL(const TDesC& aId)
	{
	CValue* value = iValues.Find(aId);
	if (value)
		{
		DoRemoveL(value);
		}
	}
	
void CIniReader::DoRemoveL(CValue* aValue)
	{
	iValues.Remove(&aValue->Id());
	delete aValue;
	}
	
void CIniReader::AppendFirstLineL(IoUtils::CTextFormatter* aFormatter)
	{
	if (iFirstLineComment.Length())
		{
		aFormatter->AppendFormatL(_L("#%S\r\n"), &iFirstLineComment);
		}
	}
	
void CIniReader::HandleValueL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext, TBool aOverwrite)
	{
	CValue* value = iValues.Find(aId);
	if (!value)
		{
		value = CreateValueLC(aId, aValue, aErrorContext);
		iValues.InsertL(&value->Id(), value);
		CleanupStack::Pop(value);
		}
	else
		{
		if (aOverwrite)
			{
			value->SetL(aValue, aErrorContext);
			}
		// else ignore the duplicate value
		}
	}
	
void CIniReader::HandleCommentL(const TDesC& aComment, TErrorContext aErrorContext)
	{
	if (aErrorContext.LineNumber() == 1)
		{
		iFirstLineComment.Close();
		iFirstLineComment.CreateL(aComment);
		HandleFirstLineCommentL(iFirstLineComment, aErrorContext);
		}
	}
	
CValue* CIniReader::CreateValueLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext)
	{
	return CValue::NewLC(aId, aValue, aErrorContext);
	}

//______________________________________________________________________________
//						CSetting
EXPORT_C CSetting* CSetting::NewL(TSettingType aType, const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault, const TDesC& aEnumValues, TErrorContext aErrorContext)
	{
	CSetting* self = CSetting::NewLC(aType, aId, aName, aDescription, aDefault, aEnumValues, aErrorContext);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CSetting* CSetting::NewLC(TSettingType aType, const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault, const TDesC& aEnumValues, TErrorContext aErrorContext)
	{
	CSetting* self = new(ELeave)CSetting(aType);
	CleanupStack::PushL(self);
	self->ConstructL(aId, aName, aDescription, aDefault, aEnumValues, aErrorContext);
	return self;
	}
	
EXPORT_C CSetting::~CSetting()
	{
	iName.Close();
	iDescription.Close();
	iEnumValues.Close();
	iDefault.Close();
	}

EXPORT_C void CSetting::SetL(const TDesC& aNewValue, TErrorContext aErrorContext)
	{
	TInt value = ParseL(aNewValue, aErrorContext);
	switch (iType)
		{
	case ETypeEnum:
			{
			CValue::SetL(iEnum.GetString(value), aErrorContext);
			iIntValue = value;
			}
		break;
	case ETypeFilename:
	case ETypeString:
		CValue::SetL(aNewValue, aErrorContext);
		break;
	case ETypeInteger:
			{
			CValue::SetL(aNewValue, aErrorContext);
			iIntValue = value;
			}
		break;
	case ETypeBoolean:
			{
			IoUtils::TEnum truth(KBoolValues);
			TInt value = truth.ParseL(aNewValue); // even false, odd true
			CValue::SetL(truth.GetString(value), aErrorContext);
			iIntValue = (value & 0x01) ? (TBool)ETrue : EFalse;
			}
		break;
		}
	iIsSet = ETrue;
	}
	
TInt CSetting::ParseL(const TDesC& aValue, TErrorContext aErrorContext)
	{
	switch (iType)
		{
	case ETypeEnum:
			return iEnum.ParseL(aValue);
	case ETypeFilename:
	case ETypeString:
		return 0;
	case ETypeInteger:
			{
			TLex lex(aValue);
			TInt value;
			lex.SkipSpace();
			StaticLeaveIfErr(lex.Val(value), _L("%S'%S' value '%S' is not a valid integer"), &aErrorContext.StringLC(), &Id(), &aValue);
			lex.SkipSpace();
			if (lex.Remainder().Length()) StaticLeaveIfErr(KErrArgument, _L("%S'%S' value '%S' is not a valid integer"), &aErrorContext.StringLC(), &Id(), &aValue);
			return value;
			}
	case ETypeBoolean:
			{
			TInt value = iEnum.ParseL(aValue); // even false, odd true
			return (value & 0x01) ? (TBool)ETrue : EFalse;
			}
		}
	return 0;
	}
	
_LIT(KIntFormat, "%d");
	
EXPORT_C void CSetting::SetL(TInt aNewValue)
	{
	switch (iType)
		{
	case ETypeEnum:
			{
			IoUtils::TEnum _enum(iEnumValues);
			CValue::SetL(_enum.GetString(aNewValue), TErrorContext());
			iIntValue = aNewValue;
			}
		break;
	case ETypeFilename:
	case ETypeString:
	case ETypeInteger:
			{
			TBuf<0x10> buf;
			buf.AppendFormat(KIntFormat, aNewValue);
			CValue::SetL(buf, TErrorContext());
			if (iType == ETypeInteger)
				{
				iIntValue = aNewValue;
				}
			}
		break;
	case ETypeBoolean:
			{
			CValue::SetL(iEnum.GetString(aNewValue ? 1 : 0), TErrorContext());
			iIntValue = aNewValue;
			}
		break;
		}
	iIsSet = ETrue;
	}
	
EXPORT_C TInt CSetting::AsInt(TInt& aInt) const
	{
	if (iType == ETypeEnum || iType == ETypeInteger)
		{
		aInt = AsInt();
		return KErrNone;
		}
	else return CValue::AsInt(aInt);
	}
	
EXPORT_C TInt CSetting::AsBool(TBool& aBool) const
	{
	if (iType == ETypeBoolean)
		{
		aBool = AsBool();
		return KErrNone;
		}
	else return CValue::AsBool(aBool);
	}

EXPORT_C TInt CSetting::AsInt() const
	{
	__ASSERT_ALWAYS(iType == ETypeEnum || iType == ETypeInteger, Panic(EInvalidType));
	return iIntValue;
	}
	
EXPORT_C TBool CSetting::AsBool() const
	{
	__ASSERT_ALWAYS(iType == ETypeBoolean, Panic(EInvalidType));
	return iIntValue;
	}
	
EXPORT_C TBool CSetting::IsSet() const
	{
	return iIsSet;
	}
	
EXPORT_C void CSetting::ClearL()
	{
	if (IsSet())
		{
		SetL(iDefault);
		iIsSet = EFalse;
		}
	}
	
EXPORT_C const TDesC& CSetting::Name()
	{
	return iName;
	}
	
EXPORT_C const TDesC& CSetting::Description()
	{
	return iDescription;
	}

CSetting::CSetting(TSettingType aType)
	: CValue(TErrorContext()), iType(aType), iEnum(KNullDesC)
	{
	}

void CSetting::ConstructL(const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault, const TDesC& aEnumValues, TErrorContext aErrorContext)
	{
	CValue::ConstructL(aId, KNullDesC);
	iName.CreateL(aName);
	iDescription.CreateL(aDescription);
	
	if (iType == ETypeEnum)
		{
		iEnumValues.CreateL(aEnumValues);
		RemoveSpaces(iEnumValues);
		new(&iEnum)IoUtils::TEnum(iEnumValues); // in-place construct
		}
	else if (iType == ETypeBoolean)
		{
		new(&iEnum)IoUtils::TEnum(KBoolValues);
		}

	iDefaultInt = ParseL(aDefault, aErrorContext);
	iDefault.CreateL(aDefault);
	
	iIntValue = iDefaultInt;
	CValue::SetL(iDefault, TErrorContext());
	}

	
//______________________________________________________________________________
//						CIniFile
EXPORT_C CIniFile* CIniFile::NewL(const TDesC& aIniFile, const TDesC& aInfoFile)
	{
	CIniFile* self = new(ELeave)CIniFile();
	CleanupStack::PushL(self);
	self->ConstructL(aIniFile, aInfoFile);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CIniFile::~CIniFile()
	{
	delete iInfoFile;
	iFilename.Close();
	}

EXPORT_C CSetting* CIniFile::GetSetting(const TDesC& aId)
	{
	return (CSetting*)iValues.Find(aId);
	}
	
EXPORT_C const TDesC& CIniFile::GetString(const TDesC& aId)
	{
	CSetting* setting = GetSetting(aId);
	__ASSERT_ALWAYS(setting, Panic(ENotDefined));
	return setting->Value();
	}
	
EXPORT_C TInt CIniFile::GetInt(const TDesC& aId)
	{
	CSetting* setting = GetSetting(aId);
	__ASSERT_ALWAYS(setting, Panic(ENotDefined));
	return setting->AsInt();
	}
	
EXPORT_C TBool CIniFile::GetBool(const TDesC& aId)
	{
	CSetting* setting = GetSetting(aId);
	__ASSERT_ALWAYS(setting, Panic(ENotDefined));
	return setting->AsBool();
	}
	
EXPORT_C void CIniFile::SetL(const TDesC& aId, const TDesC& aString)
	{
	CSetting* setting = GetSetting(aId);
	__ASSERT_ALWAYS(setting, Panic(ENotDefined));
	setting->SetL(aString, TErrorContext());
	}
	
EXPORT_C void CIniFile::SetL(const TDesC& aId, TInt aInt)
	{
	CSetting* setting = GetSetting(aId);
	__ASSERT_ALWAYS(setting, Panic(ENotDefined));
	setting->SetL(aInt);
	}

CValue* CIniFile::CreateValueLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext)
	{
	ASSERT(0);
	CSetting* setting = CSetting::NewLC(ETypeString, aId, aId, KNullDesC, KNullDesC, KNullDesC, aErrorContext);
	setting->SetL(aValue, aErrorContext);
	return setting;
	}

void CIniFile::HandleValueL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext, TBool aOverwrite)
	{
	if (!iInfoFile) StaticLeaveIfErr(KErrCorrupt, _L("%SNo description file has been specified"), &aErrorContext.StringLC());
	CSetting* setting = GetSetting(aId);
	if (setting)
		{
		if (setting->IsSet() && (!aOverwrite))
			{
			StaticLeaveIfErr(KErrAlreadyExists, _L("%Sid '%S' already defined on line %d"), &aErrorContext.StringLC(), &aId, setting->LineNumber());
			}
		setting->SetL(aValue, aErrorContext);
		}
	else
		{
		StaticLeaveIfErr(KErrCorrupt, _L("%Sid '%S' not recognised"), &aErrorContext.StringLC(), &aId);
		}
	}
	
_LIT(KFirstLineCommentMatch, "!iniedit ");
_LIT(KOptionInfoFile, "-i");

void CIniFile::HandleFirstLineCommentL(const TDesC& aComment, TErrorContext aErrorContext)
	{
	if (!iInfoFile)
		{
		if (aComment.Left(KFirstLineCommentMatch().Length()).CompareF(KFirstLineCommentMatch)==0)
			{
			//TODO see if we can do something better here, maybe reusing CCommandBase's option parsing?
					// remove the matched string, without the wildcard
			TLex lex(aComment.Mid(KFirstLineCommentMatch().Length()));
			lex.SkipSpace();
			while (!lex.Eos())
				{
				TPtrC next(lex.NextToken());
				lex.SkipSpace();
				if (next.Compare(KOptionInfoFile)==0 && !lex.Eos())
					{
					TPtrC infoFile(lex.NextToken());
					lex.SkipSpace();
					
					ReadInfoFileL(infoFile, aErrorContext);					
					}
				}
			}
		}
	}
	
TBool CIniFile::IncludeValue(const CValue* aValue) const
	{
	return ((const CSetting*)aValue)->IsSet();
	}
	
void CIniFile::DoRemoveL(CValue* aValue)
	{
	static_cast<CSetting*>(aValue)->ClearL();
	}

CIniFile::CIniFile()
	{
	}
	
_LIT(KSpace, " ");

void CIniFile::ConstructL(const TDesC& aIniFile, const TDesC& aInfoFile)
	{
	if (aInfoFile.Length()!=0)
		{
		ReadInfoFileL(aInfoFile, TErrorContext());
		
		}
	ReadIniFileL(aIniFile, *this, TErrorContext(), aIniFile.Length() ? ESucceedIfFileNotFound : EFailIfFileNotFound);
	iFilename.CreateL(aIniFile);
	
	if ((iFirstLineComment.Length()==0) && (aInfoFile.Length()!=0))
		{
		// construct the first line comment if it doesn't exist in the file, and we have an IDF file
		iFirstLineComment.Close();
		iFirstLineComment.CreateL(aInfoFile.Length() + KFirstLineCommentMatch().Length() + KOptionInfoFile().Length() + 1);
		iFirstLineComment.Append(KFirstLineCommentMatch);
		iFirstLineComment.Append(KOptionInfoFile);
		iFirstLineComment.Append(KSpace);
		iFirstLineComment.Append(aInfoFile);
		}
	}

_LIT(KDot, ".");
_LIT(KNameFmt, "%S.name");
_LIT(KDescriptionFmt, "%S.description");
_LIT(KEnumValuesFmt, "%S.values");
_LIT(KDefaultFmt, "%S.default");
_LIT(KTypeEnum, "enum,filename,string,integer,boolean");

void CIniFile::ReadInfoFileL(const TDesC& aFilename, TErrorContext aErrorContext)
	{
	if (iInfoFile) return;
	iInfoFile = CIniReader::NewL(aFilename, aErrorContext);
	IoUtils::TEnum typeEnum(KTypeEnum);
	IoUtils::CTextBuffer* idBuf = IoUtils::CTextBuffer::NewLC(8);
	
	RArray<const TPtrC> ids;
	CleanupClosePushL(ids);
	iInfoFile->GetIdsL(ids);
	
	for (TInt i=0; i<ids.Count(); ++i)
		{
		if (ids[i].Find(KDot) == KErrNotFound)
			{
			const TDesC& id = ids[i];
			
			TSettingType type = (TSettingType)typeEnum.ParseL(*iInfoFile->GetValue(id));
			
			idBuf->Reset();
			idBuf->AppendFormatL(KNameFmt, &id);
			const TDesC* name = iInfoFile->GetValue(idBuf->Descriptor());
			
			idBuf->Reset();
			idBuf->AppendFormatL(KDescriptionFmt, &id);
			const TDesC* description = iInfoFile->GetValue(idBuf->Descriptor());
			
			idBuf->Reset();
			idBuf->AppendFormatL(KDefaultFmt, &id);
			const TDesC* defaultValue = iInfoFile->GetValue(idBuf->Descriptor());
			
			const TDesC* enumValues = NULL;
			if (type == ETypeEnum)
				{
				idBuf->Reset();
				idBuf->AppendFormatL(KEnumValuesFmt, &id);
				enumValues = iInfoFile->GetValue(idBuf->Descriptor());
				if (!enumValues) StaticLeaveIfErr(KErrCorrupt, _L("%S: enum '%S' doesn't have any values specified (%S.values not defined)"), &aFilename, &id, &id);
				}
				
			CSetting* setting = CSetting::NewLC(type, id, name ? *name : id, description ? *description : KNullDesC, defaultValue ? *defaultValue : KNullDesC, enumValues ? *enumValues : KNullDesC, aErrorContext);
			iValues.InsertL(&setting->Id(), setting);
			CleanupStack::Pop();			
			}
		}
	
	CleanupStack::PopAndDestroy(2, idBuf);// idBuf, ids
	}
	
EXPORT_C void CIniFile::WriteL()
	{
	IoUtils::TFileName2 iniFile(iFilename);
	
	RFs fs; User::LeaveIfError(fs.Connect());
	TInt err = iniFile.FindFile(fs);
	
	if (err==KErrNotFound)
		{
		if (iniFile.Length()==0) User::Leave(KErrBadName);
		if (iniFile[0] != '\\') iniFile.Insert(0, _L("\\"));
		iniFile.Insert(0, _L("C:"));
		
		err = fs.MkDirAll(iniFile);
		if (err== KErrAlreadyExists) err=KErrNone;		
		}

	fs.Close();
	User::LeaveIfError(err);
	
	WriteIniFileL(iniFile, *this);
	}

	
