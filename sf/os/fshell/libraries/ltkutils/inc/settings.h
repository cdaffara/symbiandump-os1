// settings.h
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

#ifndef __SETTINGSEDITOR_H__
#define __SETTINGSEDITOR_H__

#include <e32base.h>
#include <e32hashtab.h>
#include <fshell/ioutils.h>

namespace LtkUtils
	{
	
	_LIT(KSettingPanic, "FSHELL_settings");
	enum TSettingPanic
		{
		EInvalidType,
		ENotDefined,
		};

/**
Class for encapsulating the context of an error within a text file that is being processed.

Note, the valid lifetime of this class is tied to the lifetime of the decriptor it refers to.
Often, this is a descriptor on the stack and so copies of this class must not be kept.
*/
class TErrorContext
	{
public:
	IMPORT_C TErrorContext();
	IMPORT_C TErrorContext(const TDesC& aFilename);
	IMPORT_C void NextLine();
	IMPORT_C TInt LineNumber();
	IMPORT_C const TDesC& StringLC();
private:
	const TPtrC iFilename;
	TInt iLineNumber;
	};
	
class MValueHandler
	{
public:
	virtual void HandleValueL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext, TBool aOverwrite) = 0;
	virtual void HandleCommentL(const TDesC& aComment, TErrorContext aErrorContext) = 0;
	};
	
class CIniReader;

enum TFileNotFoundAction
	{
	EFailIfFileNotFound,
	ESucceedIfFileNotFound,
	};
	
IMPORT_C void ReadIniFileL(const TDesC& aFilename, MValueHandler& aValueHandler, TErrorContext aErrorContext = TErrorContext(), TFileNotFoundAction aNotFoundAction = EFailIfFileNotFound);
IMPORT_C void ReadIniFileL(RIoReadHandle& aReadHandle, MValueHandler& aValueHandler);
IMPORT_C void WriteIniFileL(const TDesC& aFilename, CIniReader& aValues);

class CValue : public CBase
	{
public:
	IMPORT_C static CValue* NewL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C static CValue* NewLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C ~CValue();
	
	IMPORT_C const TDesC& Id() const;
	IMPORT_C const TDesC& Value() const;
	IMPORT_C virtual void SetL(const TDesC& aNewValue, TErrorContext aErrorContext);
	IMPORT_C TInt LineNumber() const;
	IMPORT_C virtual TInt AsInt(TInt& aInt) const;
	IMPORT_C virtual TInt AsBool(TBool& aBool) const;
	IMPORT_C TInt AsIntL() const;
	IMPORT_C TBool AsBoolL() const;
	
protected:
	CValue(TErrorContext aErrorContext);
	void ConstructL(const TDesC& aId, const TDesC& aValue);
private:
	RBuf iId;
	RBuf iValue;
protected:
	TInt iLineNumber;
	};

/**
Class for reading an fshell style INI file. This class is intended to be robust, and should always
read the file even if it has errors in it.

The class CIniFile is more strict and will validate the contents of the file as it is read.
*/
class CIniReader	: public CBase
					, public MValueHandler
	{
public:
	IMPORT_C static CIniReader* NewL(const TDesC& aFilename, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C ~CIniReader();
	
	IMPORT_C const TDesC* GetValue(const TDesC& aId) const;
	IMPORT_C void GetValuesL(RPointerArray<CValue>& aValues);
	IMPORT_C void GetValuesL(RPointerArray<const CValue>& aValues) const;
	IMPORT_C void GetIdsL(RArray<const TPtrC>& aIds) const;
	
	IMPORT_C void SetValueL(const TDesC& aId, const TDesC& aValue);
	IMPORT_C void RemoveValueL(const TDesC& aId);
	
	void AppendFirstLineL(IoUtils::CTextFormatter* aFormatter);
protected: // from MValueHandler
	virtual void HandleValueL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext, TBool aOverwrite);
	virtual void HandleCommentL(const TDesC& aComment, TErrorContext aErrorContext);
protected:
	virtual CValue* CreateValueLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext);
	virtual void HandleFirstLineCommentL(const TDesC&, TErrorContext) {};
	virtual TBool IncludeValue(const CValue*) const { return ETrue; }
	virtual void DoRemoveL(CValue* aValue);
	CIniReader();
protected:
	RPtrHashMap<TDesC, CValue> iValues;
	RBuf iFirstLineComment;
	};
	
enum TSettingType
	{
	ETypeEnum,
	ETypeFilename,
	ETypeString,
	ETypeInteger,
	ETypeBoolean,
	};
	
class CSetting : public CValue
	{
public:
	IMPORT_C static CSetting* NewL(TSettingType aType, const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault = KNullDesC, const TDesC& aEnumValues = KNullDesC, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C static CSetting* NewLC(TSettingType aType, const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault = KNullDesC, const TDesC& aEnumValues = KNullDesC, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C ~CSetting();
	
	IMPORT_C virtual void SetL(const TDesC& aNewValue, TErrorContext aErrorContext = TErrorContext());
	IMPORT_C void SetL(TInt aNewValue);
	IMPORT_C virtual TInt AsInt(TInt& aInt) const;
	IMPORT_C virtual TInt AsBool(TBool& aBool) const;
	IMPORT_C TInt AsInt() const;
	IMPORT_C TBool AsBool() const;
	
	IMPORT_C TBool IsSet() const;
	IMPORT_C void ClearL();
	
	IMPORT_C const TDesC& Name();
	IMPORT_C const TDesC& Description();
private:
	CSetting(TSettingType aType);
	void ConstructL(const TDesC& aId, const TDesC& aName, const TDesC& aDescription, const TDesC& aDefault, const TDesC& aEnumValues, TErrorContext aErrorContext = TErrorContext());
	TInt ParseL(const TDesC& aValue, TErrorContext aErrorContext);
private:
	const TSettingType iType;
	RBuf iName;
	RBuf iDescription;
	RBuf iDefault;
	RBuf iEnumValues;
	TInt iIntValue;
	TInt iDefaultInt;
	TBool iIsSet;
	IoUtils::TEnum iEnum;
	};
	
/**
Class for validating an fshell style ini file. This class will fail (and reports useful errors) if the
file is corrupt of invalid in any way. The file will be validated using an INI description file (IDF).
*/
class CIniFile : public CIniReader
	{
public:
	/**
	Read and validate an INI file.
	
	If an IDF file is given, it will be used to validate the INI file regardless of any #!iniedit
	line at the start of the file. Also, if the INI file given does not exist, this function will
	still succeed and can then be used to create the INI file by calling WriteIniFileL().
	
	If no IDF file is specified here, then an IDF file specified in the #!iniedit comment on the
	first line will be used. Note, if the INI file does not exist this function will fail in this
	case.
	*/
	IMPORT_C static CIniFile* NewL(const TDesC& aIniFile, const TDesC& aInfoFile = KNullDesC);
	IMPORT_C ~CIniFile();
	IMPORT_C CSetting* GetSetting(const TDesC& aId);
	
	IMPORT_C const TDesC& GetString(const TDesC& aId);
	IMPORT_C TInt GetInt(const TDesC& aId);
	IMPORT_C TBool GetBool(const TDesC& aId);
	
	IMPORT_C void SetL(const TDesC& aId, const TDesC& aString);
	IMPORT_C void SetL(const TDesC& aId, TInt aInt);
	
	IMPORT_C void WriteL();
	
protected:// from CIniReader
	virtual CValue* CreateValueLC(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext);
	virtual void HandleFirstLineCommentL(const TDesC& aComment, TErrorContext aErrorContext);
	virtual TBool IncludeValue(const CValue* aValue) const;
	virtual void DoRemoveL(CValue* aValue);
protected: // from MValueHandler
	virtual void HandleValueL(const TDesC& aId, const TDesC& aValue, TErrorContext aErrorContext, TBool aOverwrite);
private:
	CIniFile();
	void ConstructL(const TDesC& aIniFile, const TDesC& aInfoFile);
	void ReadInfoFileL(const TDesC& aFileName, TErrorContext aErrorContext);
private:
	CIniReader* iInfoFile;
	RBuf iFilename;
	};

	} // namespace LtkUtils

#endif //__SETTINGSEDITOR_H__
