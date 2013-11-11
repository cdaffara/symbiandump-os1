// ioutils.h
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

#ifndef __IOUTILS_H__
#define __IOUTILS_H__

#include <e32std.h>
#include <e32keys.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <f32file.h>
#include <fshell/iocli.h>
#include <fshell/iocons_writer.h>
#include <fshell/line_editor_observer.h>

class CColorConsoleBase;

namespace IoUtils
	{

const TUint KEnvironmentProcessSlot		= 15;
const TUint KDllCommandLineProcessSlot	= 14;
const TUint KDllNameToLoadProcessSlot	= 13;
const TUint KPipsCommandNameProcessSlot = 14;
const TUint KPipsCommandLineProcessSlot = 13;

const TUint KValueTypeBool         		= 0;
const TUint KValueTypeInt          		= 1;
const TUint KValueTypeUint         		= 2;
const TUint KValueTypeString       		= 3;
const TUint KValueTypeFileName     		= 4;
const TUint KValueTypeEnum				= 5;
const TUint KValueTypeReal				= 6;
const TUint KValueTypeUint64       		= 7;
const TUint KValueTypeInt64        		= 8;
const TUint KValueTypeFlagLast	 		= 0x20000000;	///< Tag this argument as the last one that this command handles. Can only be used with string arguments. Causes the remainder of the command line to be written to the string.
const TUint KValueTypeFlagOptional 		= 0x40000000;	///< Tag this argument being optional. The command should handle the fact that the corresponding variable may not have been initialised (e.g. by calling RCommandArgumentList::IsPresent).

enum TCmdBasePanic
	{
	ENoEnvironment = 0,
	EInvalidCommandOptionType = 1,
	EInvalidCommandArgumentType = 2,
	EIncorrectCommandOptionType1 = 3,
	EIncorrectCommandOptionType2 = 4,
	EIncorrectCommandOptionType3 = 5,
	EIncorrectCommandOptionType4 = 6,
	EIncorrectCommandOptionType5 = 7,
	EIncorrectCommandOptionType6 = 8,
	EIncorrectCommandOptionType7 = 9,
	EIncorrectCommandOptionType8 = 10,
	EIncorrectCommandOptionType9 = 11,
	EIncorrectCommandOptionType10 = 12,
	EIncorrectCommandOptionType11 = 13,
	ERFsNotConnected = 14,
	EAutoAborterAlreadyActive = 15,
	EInvalidArgumentIndex = 16,
	EDuplicateShortOptionName = 17,
	EUnused9 = 18,
	ENoStaticCommand = 19,
	EAlreadyCompleted = 20,
	EInvalidUseOfEnum = 21,
	EUnused10 = 22,
	ECifSubCommandParentNotFound = 23,
	EEnumValueContainsSpace = 24,
	ENonOptionalArgumentFollowsOptionalArgument = 25,
	EArgumentFollowsMultipleArgument = 26,
	EActiveObjectRunLNotOverridden = 27,
	EActiveObjectDoCancelNotOverridden = 28,
	ENotYetComplete = 29,
	EUnused1 = 30,
	EUnused2 = 31,
	EUnused3 = 32,
	EUnused4 = 33,
	EUnused5 = 34,
	EUnused6 = 35,
	EUnused7 = 36,
	EUnused8 = 37,
	EEnvVarNotFound1 = 38,
	EEnvVarNotFound2 = 39,
	EOptionTaggedAsLast = 40,
	ELastArgNotStringType = 41,
	ENoFactory = 42,
	EUnexpectedAsyncCommand = 43,
	EUnused11 = 44,
	ELastArgMultiple = 45,
	EEnumValueOutOfRange = 46,
	EEnumDescriptionMissing = 47,
	EValueTypeAlreadySet = 48,
	EValueDescriptionAlreadySet = 49,
	EEnumValueListAlreadySet = 50,
	EEnumDescriptionListAlreadySet = 51,
	EIncompleteArgumentOrOptionInitialization = 52,
	EENotifyKeypressesSpecifiedWithoutExtensionBeingSet = 53,
	};

class CCommandBase;
class CReaderChangeNotifier;
class CCommandCompleter;
class MLineReader;
class CKeypressWatcher;

class TOverflowTruncate : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};

class TOverflowTruncate8 : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8&) {}
	};

class TOverflowLeave : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) { User::Leave(KErrOverflow); }
	};

class TOverflowLeave8 : public TDes8Overflow
	{
public:
	virtual void Overflow(TDes8&) { User::Leave(KErrOverflow); }
	};

class TFileName2 : public TFileName
	{
public:
	enum TType
		{
		EFile,
		EDirectory
		};
public:
	IMPORT_C TFileName2();
	IMPORT_C TFileName2(TType aType);
	IMPORT_C TFileName2(const TDesC& aName);
	IMPORT_C TFileName2(const TDesC& aName, TType aType);
	IMPORT_C TBool IsAbsolute() const;
	IMPORT_C TBool IsDriveLetter() const;
	IMPORT_C TBool IsDriveRoot() const;
	IMPORT_C TBool IsWild() const;
	IMPORT_C TBool IsFile() const;
	IMPORT_C TBool IsDir() const;
	IMPORT_C TBool HasDriveLetter() const;
	IMPORT_C TBool HasLeadingSlash() const;
	IMPORT_C TBool HasTrailingSlash() const;
	IMPORT_C TPtrC Drive() const;
	IMPORT_C TPtrC Path() const;
	IMPORT_C TPtrC DriveAndPath() const;
	IMPORT_C TPtrC Name() const;
	IMPORT_C TPtrC Ext() const;
	IMPORT_C TPtrC NameAndExt() const;
	IMPORT_C void SetL(const TFileName2& aName);
	IMPORT_C void SetTypeL(TType aType);
	IMPORT_C void SetTypeL(RFs& aFs);
	IMPORT_C void SetDriveL(const TDesC& aDrive);
	IMPORT_C void SetPathL(const TDesC& aPath);
	IMPORT_C void SetNameL(const TDesC& aName);
	IMPORT_C void SetExtL(const TDesC& aExt);
	IMPORT_C void SetNameAndExtL(const TDesC& aNameAndExt);
	IMPORT_C void PrependL(const TFileName2& aName);
	IMPORT_C void AppendComponentL(const TFileName2& aName);
	IMPORT_C void AppendComponentL(const TDesC& aPathComponent, TType aPathComponentType);
	IMPORT_C void AppendComponentL(const TEntry& aEntry);
	IMPORT_C void SplitL(RArray<TFileName2>& aOutput);
	IMPORT_C void UnsplitL(const RArray<TFileName2>& aInput);
	IMPORT_C void MakeAbsoluteL(const TFileName2& aAbsoluteTo);
	IMPORT_C void MakeAbsoluteL(RFs& aFs); // Make absolute relative to aFs's session path
	IMPORT_C TBool Exists(RFs& aFs) const;
	IMPORT_C TBool IsReadOnlyL(RFs& aFs) const;
	IMPORT_C TBool IsHiddenL(RFs& aFs) const;
	IMPORT_C TBool IsSystemL(RFs& aFs) const;
	IMPORT_C TBool IsDirL(RFs& aFs) const;
	IMPORT_C TBool IsArchiveL(RFs& aFs) const;
	IMPORT_C void SetIsDirectoryL(); // Ensure the descriptor ends in a slash
	IMPORT_C TInt FindFile(RFs& aFs);
	IMPORT_C void FindFileL(RFs& aFs);
	IMPORT_C void Normalize(RFs& aFs);
private:
	void CheckSpaceL(TInt aLengthToAdd) const;
private:
	TType iType;
	};


enum TAlignment
	{
	EUnaligned,
	EColumnAlignedLeft,
	EColumnAlignedRight
	};

enum TTabMode
	{
	EWrapLastColumn,
	ETruncateLongestColumn,
	EIgnoreAvailableWidth,
	};

NONSHARABLE_CLASS(CTextBuffer) : public CActive
	{
public:
	IMPORT_C static CTextBuffer* NewL(TInt aExpandSize);
	IMPORT_C static CTextBuffer* NewLC(TInt aExpandSize);
	IMPORT_C ~CTextBuffer();
	IMPORT_C virtual void Zero();
	IMPORT_C virtual void Reset();
	IMPORT_C virtual void ResetText();
	IMPORT_C void SetAttributesL(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor = ConsoleAttributes::EUnchanged, ConsoleAttributes::TColor aBackgroundColor = ConsoleAttributes::EUnchanged);
	IMPORT_C void SetAttributesL(const ConsoleAttributes::TAttributes& aAttributes);
	IMPORT_C void GetCurrentAttributes(TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const;
	IMPORT_C void GetAttributes(TInt aPos, TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const;
	IMPORT_C void AppendL(const TChar& aChar);
	IMPORT_C void AppendL(const TDesC16& aText);
	IMPORT_C void AppendL(const TDesC8& aText);
	IMPORT_C void AppendL(const CTextBuffer& aText);
	IMPORT_C void AppendL(const CTextBuffer& aText, TInt aPosition);
	IMPORT_C void AppendL(const CTextBuffer& aText, TInt aPosition, TInt aLength);
	IMPORT_C void AppendFormatL(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void AppendFormatL(TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C void AppendFormatListL(const TDesC16& aFmt, VA_LIST& aList);
	IMPORT_C void AppendFormatListL(const TDesC8& aFmt, VA_LIST& aList);
	IMPORT_C void AppendHumanReadableSizeL(TInt aSize);
	IMPORT_C void AppendHumanReadableSizeL(TInt64 aSize);
	IMPORT_C void AppendHumanReadableSizeL(TInt aSize, TAlignment aAlignment);
	IMPORT_C void AppendHumanReadableSizeL(TInt64 aSize, TAlignment aAlignment);
	IMPORT_C void AppendSpacesL(TInt aCount);
	IMPORT_C void Delete(TInt aPos, TInt aLength);
	IMPORT_C TInt Length() const;
	IMPORT_C const TDesC& Descriptor() const;
	IMPORT_C const TDesC& Descriptor(TInt aPos, TInt aLength) const;
	IMPORT_C TPtrC8 Collapse();
	IMPORT_C TInt Write(RIoWriteHandle& aWriteHandle) const;
	IMPORT_C void Write(RIoWriteHandle& aWriteHandle, TRequestStatus& aStatus) const;
	IMPORT_C TInt Write(RIoWriteHandle& aWriteHandle, TInt aPosition, TInt aLength) const;
	IMPORT_C void Write(RIoWriteHandle& aWriteHandle, TInt aPosition, TInt aLength, TRequestStatus& aStatus) const;
protected:
	CTextBuffer();
	void ConstructL(TInt aExpandSize);
private:
	void NextBlock(TInt& aBlockIndex, const TDesC*& aText, TUint& aAttributes, ConsoleAttributes::TColor& aForegroundColor, ConsoleAttributes::TColor& aBackgroundColor) const;
	void AsyncWriteNextBlock() const;
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
private:
	class TAttributes
		{
	public:
		TAttributes(TInt aPosition, TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor);
		TBool Matches(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor) const;
	public:
		TInt iPosition;
		ConsoleAttributes::TAttributes iAttributes;
		};
private:
	CBufBase* iBuf;
	mutable TPtrC iPtr;
	HBufC* iScratchBuf;
	HBufC8* iScratchBuf8;
	RArray<TAttributes> iAttributes;
	mutable RIoConsoleWriteHandle iConsoleWriteHandle; // Not owned.
	mutable TInt iAsyncBlockIndex;
	mutable TInt iAsyncWritePos;
	mutable TInt iAsyncWriteStartPos;
	mutable TInt iAsyncWriteLength;
	mutable TPtrC iAsyncWritePtr;
	mutable TRequestStatus* iWriteStatus;
	};


NONSHARABLE_CLASS(CTextFormatter) : public CTextBuffer
	{
public:
	IMPORT_C static CTextFormatter* NewL(TInt aAvailableWidth);
	IMPORT_C static CTextFormatter* NewLC(TInt aAvailableWidth);
	IMPORT_C static CTextFormatter* NewL(RIoConsoleWriteHandle& aConsoleWriteHandle);
	IMPORT_C static CTextFormatter* NewLC(RIoConsoleWriteHandle& aConsoleWriteHandle);
	IMPORT_C ~CTextFormatter();
	IMPORT_C virtual void Zero();
	IMPORT_C virtual void Reset();
	IMPORT_C virtual void ResetText();
	IMPORT_C void WrapL(TInt aIndent, const TDesC& aText);
	IMPORT_C void WrapL(TInt aStartPosition, TInt aIndent, const TDesC& aText);
	IMPORT_C void TabulateL(TInt aIndent, TInt aGap, const TDesC& aText);
	IMPORT_C void TabulateL(TInt aIndent, TInt aGap, const TDesC& aText, TTabMode aMode);
	IMPORT_C void ColumnizeL(TInt aIndent, TInt aGap, const TDesC& aText);
	IMPORT_C void ColumnizeL(TInt aIndent, TInt aGap, const TArray<TPtrC>& aItems);
	IMPORT_C void AppendPodL(const TDesC& aPod);
	IMPORT_C TInt Write();
private:
	CTextFormatter(TInt aAvailableWidth);
	void ConstructL();
	void DoAppendPodL(const TDesC& aPod);
	void DecodeInteriorPodSequencesL(const TDesC& aPod, CTextBuffer& aBuffer) const;
	TBool NextColumn(TLex& aLex, TPtrC& aPtr) const;
	TBool NextLine(TLex& aLex, TPtrC& aPtr) const;
	TInt ActualLength(const TDesC& aPod) const;
private:
	TInt iAvailableWidth;
	RIoConsoleWriteHandle* iWriteHandle;
	TBool iAttributesSupported;
	};


NONSHARABLE_CLASS(CEnvironment) : public CBase
	{
public:
	IMPORT_C static CEnvironment* NewL();
	IMPORT_C static CEnvironment* NewL(const CEnvironment& aEnv);
	IMPORT_C ~CEnvironment();
	IMPORT_C void SetL(const TDesC& aKey, TInt aValue);
	IMPORT_C void SetL(const TDesC& aKey, const TDesC& aValue);
	IMPORT_C TInt GetAsInt(const TDesC& aKey) const;
	IMPORT_C TInt GetAsIntL(const TDesC& aKey) const;
	IMPORT_C const TDesC& GetAsDes(const TDesC& aKey) const;
	IMPORT_C const TDesC& GetAsDesL(const TDesC& aKey) const;
	IMPORT_C TInt Remove(const TDesC& aKey);
	IMPORT_C void RemoveL(const TDesC& aKey);
	IMPORT_C void RemoveAll(); // Doesn't remove anything in parent env
	IMPORT_C void InternalizeL(const TDesC8& aDes);
	IMPORT_C HBufC8* ExternalizeLC() const;
	IMPORT_C TBool IsDefined(const TDesC& aKey) const;
	IMPORT_C TBool IsInt(const TDesC& aKey) const;
	IMPORT_C TBool IsDes(const TDesC& aKey) const;
	IMPORT_C TInt Count() const;
	IMPORT_C void GetKeysL(RPointerArray<HBufC>& aResult) const;
	IMPORT_C void SetPwdL(const TDesC& aPwd);
	IMPORT_C const TDesC& Pwd() const;
	IMPORT_C TChar EscapeChar() const;
	IMPORT_C void SetLocalL(const TDesC& aKey);
	IMPORT_C CEnvironment* CreateSharedEnvironmentL();
private:
	CEnvironment();
	CEnvironment(CEnvironment* aParentEnv);
	void ConstructL();
	void WaitLC() const;
	static void Signal(TAny* aSelf);
	void Lock() const;
	void Unlock() const;
	void CopyL(const CEnvironment& aEnv);
	HBufC* Get(const TDesC& aKey) const;
	HBufC* GetL(const TDesC& aKey) const;
private:
	TUint32 iVarsImpl[14];
	mutable RMutex iLock;
	CEnvironment* iParentEnv;
	};


class MCommandBaseObserver
	{
public:
	virtual void HandleCommandComplete(CCommandBase& aCommand, TInt aError) = 0;
	};

class TEnum
	{
public:
	IMPORT_C TEnum();
	IMPORT_C TEnum(const TDesC& aEnumValueList);
	IMPORT_C TEnum(const TDesC& aEnumValueList, const TDesC& aEnumDescriptionList);
	IMPORT_C TInt Parse(const TDesC& aString, TInt& aValue);
	IMPORT_C TInt ParseL(const TDesC& aString);
	IMPORT_C const TDesC& ValueList() const;
	IMPORT_C const TDesC& DescriptionList() const;
	IMPORT_C void AppendValuesL(CTextBuffer& aBuf) const;
	IMPORT_C const TPtrC GetString(TInt aIndex) const;
	IMPORT_C void SetValueList(const TDesC& aValueList);
	IMPORT_C void SetDescriptionList(const TDesC& aDescriptionList);
	IMPORT_C void operator=(const TEnum& aEnum);
private:
	void ValidateValues() const;
	TInt GetMatches(const TDesC& aString, RArray<TInt>& aMatches, TInt& aExactMatch);
private:
	TPtrC iEnumValueList;
	TPtrC iEnumDescriptionList;
	};

class TValue
	{
public:
	IMPORT_C const TDesC& Name() const;
	IMPORT_C TUint Type() const;
	IMPORT_C TBool AcceptsMultiple() const;
	IMPORT_C const TDesC& EnvVar() const;
	IMPORT_C void* ValuePtr() const;
	IMPORT_C const TDesC& EnumValueList() const;

	TValue(TUint aValueType, void* aValuePointer, const TDesC& aName);
	TValue(TUint aValueType, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar = KNullDesC);
	TValue(TUint aValueType, const TDesC& aEnumValueList, const TDesC& aEnumDescriptionList, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar = KNullDesC);
	TUint ValueType() const;
	void SetValue();
	void SetValueL(TInt aInt);
	void SetValueL(TInt64 aInt);
	void SetValueL(TUint aUint);
	void SetValueL(TUint64 aUint);
	void SetValueL(TReal64 aReal);
	void SetValueL(HBufC* aString);
	void SetValueL(const TFileName2& aFileName, RFs& aFs);
	void SetEnumValues(const TEnum& aEnum);
	TBool IsSet() const;
	const TDesC& EnumDescriptionList() const;
	const TDesC& Description() const;
	TPtrC AsString() const;
	void AppendEnumValuesL(CTextBuffer& aBuf) const;
	void Combine(const TValue& aValue);
protected:
	TUint iValueType;
	void* iValue;
	TBool iIsSet;
	TPtrC iEnvVar;
	TEnum iEnum;
	const TPtrC iName;
	TPtrC iDescription;
	};


class TCommandOption : public TValue
	{
public:
	TCommandOption(TUint aValueType, void* aValuePointer, const TDesC& aLongName);
	TCommandOption(TUint aValueType, void* aValuePointer, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aEnvVar = KNullDesC);
	TCommandOption(void* aValuePointer, TUint aValueType, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aEnvVar = KNullDesC);
	TChar ShortName() const;
	void Combine(const TCommandOption& aOption);
private:
	TChar iShortName;
	};


class RCommandOptionList
	{
public:
	// Use these APIs to only partially declare options in C++ code, and to have the remaining details (e.g. documentation) read from a Command Info File (CIF).
	IMPORT_C void AppendBoolL(TBool& aBool, const TDesC& aLongName);
	IMPORT_C void AppendBoolL(RArray<TBool>& aBools, const TDesC& aLongName);
	IMPORT_C void AppendIntL(TInt& aInt, const TDesC& aLongName);
	IMPORT_C void AppendIntL(RArray<TInt>& aInts, const TDesC& aLongName);
	IMPORT_C void AppendIntL(TInt64& aInt, const TDesC& aLongName);
	IMPORT_C void AppendIntL(RArray<TInt64>& aInts, const TDesC& aLongName);
	IMPORT_C void AppendUintL(TUint& aUint, const TDesC& aLongName);
	IMPORT_C void AppendUintL(RArray<TUint>& aUints, const TDesC& aLongName);
	IMPORT_C void AppendUintL(TUint64& aUint, const TDesC& aLongName);
	IMPORT_C void AppendUintL(RArray<TUint64>& aUints, const TDesC& aLongName);
	IMPORT_C void AppendRealL(TReal64& aReal, const TDesC& aLongName);
	IMPORT_C void AppendRealL(RArray<TReal64>& aReals, const TDesC& aLongName);
	IMPORT_C void AppendStringL(HBufC*& aBuf, const TDesC& aLongName);
	IMPORT_C void AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aLongName);
	IMPORT_C void AppendFileNameL(TFileName2& aFileName, const TDesC& aLongName);
	IMPORT_C void AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aLongName);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aLongName);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aLongName, const TDesC& aCommaSeparatedValueList);
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aLongName); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template withe a different memory layout to the general version.
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aLongName, const TDesC& aCommaSeparatedValueList); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template withe a different memory layout to the general version.

	// Use these APIs to fully declare options in C++ code.
	IMPORT_C void AppendBoolL(TBool& aBool, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendBoolL(RArray<TBool>& aBools, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(TInt& aInt, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(RArray<TInt>& aInts, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(TInt64& aInt, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(RArray<TInt64>& aInts, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(TUint& aUint, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(RArray<TUint>& aUints, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(TUint64& aUint, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(RArray<TUint64>& aUints, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendRealL(TReal64& aReal, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendRealL(RArray<TReal64>& aReals, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendStringL(HBufC*& aBuf, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendStringL(RPointerArray<HBufC>& aBufs, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendFileNameL(TFileName2& aFileName, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendFileNameL(RArray<TFileName2>& aFileNames, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(TInt& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(TInt& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template withe a different memory layout to the general version.
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, TChar aShortName, const TDesC& aLongName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template withe a different memory layout to the general version.

	IMPORT_C TBool IsPresent(void* aValuePointer) const;
	IMPORT_C TPtrC AsString(void* aValuePointer) const;
	void Close();
	IMPORT_C TInt Count() const;
	IMPORT_C TCommandOption& operator[](TInt aIndex);
	IMPORT_C const TCommandOption& operator[](TInt aIndex) const;
	TInt FindShort(TChar aShortName) const;
	TInt Find(const TDesC& aLongName) const;
private:
	void Validate(const TCommandOption& aOption);
private:
	RArray<TCommandOption> iOptions;
	};


class TCommandArgument : public TValue
	{
public:
	TCommandArgument(TUint aValueType, void* aValuePointer, const TDesC& aName);
	TCommandArgument(TUint aValueType, void* aValuePointer, const TDesC& aName, const TDesC& aDescription, const TDesC& aEnvVar = KNullDesC);
	TCommandArgument(void* aValuePointer, TUint aValueType, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, const TDesC& aEnvVar = KNullDesC);
	TBool IsOptional() const;
	TBool IsLast() const;
	};


class RCommandArgumentList
	{
public:
	// Use these APIs to only partially declare arguments in C++ code, and to have the remaining details (e.g. documentation) read from a Command Info File (CIF).
	IMPORT_C void AppendIntL(TInt& aInt, const TDesC& aName);
	IMPORT_C void AppendIntL(RArray<TInt>& aInts, const TDesC& aName);
	IMPORT_C void AppendIntL(TInt64& aInt, const TDesC& aName);
	IMPORT_C void AppendIntL(RArray<TInt64>& aInts, const TDesC& aName);
	IMPORT_C void AppendUintL(TUint& aUint, const TDesC& aName);
	IMPORT_C void AppendUintL(RArray<TUint>& aUints, const TDesC& aName);
	IMPORT_C void AppendUintL(TUint64& aUint, const TDesC& aName);
	IMPORT_C void AppendUintL(RArray<TUint64>& aUints, const TDesC& aName);
	IMPORT_C void AppendRealL(TReal64& aReal, const TDesC& aName);
	IMPORT_C void AppendRealL(RArray<TReal64>& aReals, const TDesC& aName);
	IMPORT_C void AppendStringL(HBufC*& aBuf, const TDesC& aName);
	IMPORT_C void AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aName);
	IMPORT_C void AppendFileNameL(TFileName2& aFileName, const TDesC& aName);
	IMPORT_C void AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aName);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aName);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aCommaSeparatedValueList);
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template with a different memory layout to the general version.
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aCommaSeparatedValueList); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template with a different memory layout to the general version.

	// Use these APIs to fully declare arguments in C++ code.
	IMPORT_C void AppendIntL(TInt& aInt, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(RArray<TInt>& aInts, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(TInt64& aInt, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendIntL(RArray<TInt64>& aInts, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(TUint& aUint, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(RArray<TUint>& aUints, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(TUint64& aUint, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendUintL(RArray<TUint64>& aUints, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendRealL(TReal64& aReal, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendRealL(RArray<TReal64>& aReals, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendStringL(HBufC*& aBuf, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendStringL(RPointerArray<HBufC>& aBufs, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendFileNameL(TFileName2& aFileName, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendFileNameL(RArray<TFileName2>& aFileNames, const TDesC& aName, const TDesC& aDescription, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(TInt& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC);
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template with a different memory layout to the general version.
	IMPORT_C void AppendEnumL(RArray<TInt>& aEnum, const TDesC& aName, const TDesC& aDescription, const TDesC& aCommaSeparatedValueList, const TDesC& aCommaSeparatedDescriptionList, TUint aFlags = 0, const TDesC& aEnvVar = KNullDesC); // Be sure to pass an actual RArray<TInt>& here, not an RArray<TSomeEnum>&. Internally, CCommandBase casts whatever you pass in to an RArray<TInt> and this is a specialized template with a different memory layout to the general version.

	IMPORT_C TBool IsPresent(TInt aIndex) const;
	IMPORT_C TBool IsPresent(void* aValuePointer) const;
	IMPORT_C TPtrC AsString(void* aValuePointer) const;
	void Close();
	IMPORT_C TInt Count() const;
	TCommandArgument& operator[](TInt aIndex);
	IMPORT_C const TCommandArgument& operator[](TInt aIndex) const;
	IMPORT_C TBool AllSet() const;
	TInt Find(const TDesC& aName) const;
private:
	void Validate(const TCommandArgument& aArgument);
private:
	RArray<TCommandArgument> iArguments;
	};


class CCommandInfoFile : public CBase
	{
public:
	IMPORT_C static CCommandInfoFile* NewL(RFs& aFs, const TDesC& aFileName);
	IMPORT_C static CCommandInfoFile* NewL(RFs& aFs, const CEnvironment& aEnvironment, const TDesC& aCommandName);
	IMPORT_C ~CCommandInfoFile();
	IMPORT_C const TDesC& CifFileName() const;
	IMPORT_C const TDesC& Name() const;
	IMPORT_C const TDesC& ShortDescription() const;
	IMPORT_C const TDesC& LongDescription() const;
	IMPORT_C const TDesC& SeeAlso() const;
	IMPORT_C const TDesC& Copyright() const;
	IMPORT_C const TDesC& SmokeTest() const;
	IMPORT_C TInt GetSmokeTestStartingLineNumber() const;
	IMPORT_C const RCommandArgumentList& Arguments();
	IMPORT_C const RCommandOptionList& Options() const;
	IMPORT_C void AssignL(RCommandArgumentList& aArguments, RCommandOptionList& aOptions) const;
	IMPORT_C TInt NumSubCommands() const;
	IMPORT_C const CCommandInfoFile& SubCommand(TInt aIndex) const;
	IMPORT_C const CCommandInfoFile* SubCommand(const TDesC& aSubCommandName) const;
private:
	CCommandInfoFile(const TDesC& aFileName);
	CCommandInfoFile(CCommandInfoFile& aParent);
	void ConstructL(RFs& aFs);
	void ReadFileL(RFs& aFs, const TDesC& aFileName);
	void ReadDetailsL(TLex& aLex, RFs& aFs, const TDesC& aFileName);
	void ReadArgumentL(TLex& aLex, const TDesC& aFileName);
	void ReadOptionL(TLex& aLex, const TDesC& aFileName);
	void ProcessNewChild();
	void ProcessInclude(CCommandInfoFile& aChild);
	void AddSubCommandL(TLex& aNameLex, TLex& aDataLex, RFs& aFs, const TDesC& aFileName);
	RBuf& NewBuffer();
private:
	TFileName iFileName;
	TPtrC iName;
	TPtrC iShortDescription;
	TPtrC iLongDescription;
	TPtrC iSeeAlso;
	TPtrC iCopyright;
	TPtrC iSmokeTest;
	TInt iSmokeTestLineNumber;
	RCommandArgumentList iArguments;
	RCommandOptionList iOptions;
	RArray<RBuf> iBufs;
	RPointerArray<CCommandInfoFile> iChildren;
	CCommandInfoFile* iParent;
	CCommandInfoFile* iCurrentChild;
	TBool iProcessInclude;
	};


class Stringify
	{
public:
	enum TReadMode
		{
		EBreakOnComma      = 0x00000001		// The default behaviour is to break on white space. If this flag is set both commas and white space (that are unquoted / escaped) will cause reading of the string to terminate.
		};
public:
	IMPORT_C static const TDesC* Error(TInt aError);
	IMPORT_C static HBufC* ReadLC(TLex& aLex, const TChar& aEscapeChar);
	IMPORT_C static HBufC* ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase& aErrorContext);
	IMPORT_C static HBufC* ReadLC(TLex& aLex, const TChar& aEscapeChar, TUint aReadMode);
	IMPORT_C static HBufC* ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase& aErrorContext, TUint aReadMode);
	IMPORT_C static HBufC* WidenLC(const TDesC8& aDes);
	IMPORT_C static HBufC8* NarrowLC(const TDesC& aDes);
	IMPORT_C static HBufC* EscapeLC(const TDesC& aDes, const TChar& aEscapeChar);
	IMPORT_C static HBufC8* EscapeLC(const TDesC8& aDes, const TChar& aEscapeChar);
private:
	static HBufC* ReadLC(TLex& aLex, const TChar& aEscapeChar, CCommandBase* aErrorContext, TUint aReadMode);
	};

enum TCommandExtensionVersion
	{
	ECommandExtensionV1 = 1,
	ECommandExtensionV2 = 2,
	};

class MCommandExtensionsV1
	{
public:
	IMPORT_C virtual TCommandExtensionVersion ExtensionVersion() const; // Don't override this yourself!
	
	// Override this if you have custom error codes you want CCommandBase to understand
	IMPORT_C virtual const TDesC* StringifyError(TInt aError) const;
	};

class MCommandExtensionsV2 : public MCommandExtensionsV1
	{
public:
	IMPORT_C virtual TCommandExtensionVersion ExtensionVersion() const; // Don't override this yourself!

	IMPORT_C virtual void KeyPressed(TUint aKeyCode, TUint aModifiers);

	// Override this and set the ECaptureCtrlC flag to get a callback for custom cleanup when ctrl-c is pressed
	IMPORT_C virtual void CtrlCPressed();
	};

class CCommandBase : public CActive
	{
public:
	enum TFlags
		{
		EManualComplete			= 0x00000001,
		ESharableIoSession		= 0x00000002,
		EReportAllErrors		= 0x00000004,
		ENotifyStdinChanges		= 0x00000008,
		ENotifyKeypresses		= 0x00000010,
		ECompleteOnRunL			= 0x00000020, // Implies EManualComplete
		ECaptureCtrlC			= 0x00000040, // Implies ENotifyKeypresses
		};
public:
	IMPORT_C ~CCommandBase();
	IMPORT_C void RunCommandL();
	IMPORT_C void RunCommandL(const TDesC* aCommandLine, CEnvironment* aEnv);
	void RunCommand(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const TDesC* aCommandLine, CEnvironment* aEnv, MCommandBaseObserver* aObserver);
	IMPORT_C TUint Flags() const;
	IMPORT_C void SetFlags(TUint aFlags);
	IMPORT_C const RFs& Fs() const;
	IMPORT_C const RFs& FsL() const;
	IMPORT_C RFs& Fs();
	IMPORT_C RFs& FsL();
	IMPORT_C RIoSession& IoSession();
	IMPORT_C RIoConsoleReadHandle& Stdin();
	IMPORT_C RIoConsoleWriteHandle& Stdout();
	IMPORT_C RIoConsoleWriteHandle& Stderr();
	IMPORT_C void Complete();
	IMPORT_C void Complete(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void Complete(TInt aError);
	IMPORT_C void Complete(TInt aError, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void CompleteAsync(TInt aError);
	IMPORT_C void CompleteAsync(TInt aError, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C CEnvironment& Env();
	IMPORT_C const CEnvironment& Env() const;
	IMPORT_C void ReadL(TDes& aData);
	IMPORT_C TUint ReadKey();
	IMPORT_C void Write(const TDesC& aData);
	IMPORT_C void Printf(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void Printf(TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C void PrintError(TInt aError, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void PrintWarning(TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C void LeaveIfFileExists(const TDesC& aFileName);
	IMPORT_C void LeaveIfFileNotFound(const TDesC& aFileName);
	IMPORT_C CConsoleBase* GetConsoleBaseL();
	IMPORT_C CColorConsoleBase* GetColorConsoleBaseL();
	IMPORT_C const TDesC* ConsoleImplementation();
	IMPORT_C void SetErrorReported(TBool aReported);
	IMPORT_C void StartSupressingErrors();
	IMPORT_C void StopSupressingErrors();
	IMPORT_C TBool IsComplete() const;
	IMPORT_C TInt CompletionReason() const;
	IMPORT_C virtual void HandleLeave(TInt aError);
	IMPORT_C virtual const CTextBuffer* GetHelpTextL();
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C static CCommandBase& Static();
	IMPORT_C static TBool HaveStatic();
	IMPORT_C virtual void StdinChange(TUint aChange);
	IMPORT_C void DisplayHelp();
	IMPORT_C void PageL(const CTextBuffer& aText);
	IMPORT_C void PageL(RIoReadHandle& aInput);
	IMPORT_C TBool UsingCif() const;

public:
	virtual const TDesC& Name() const = 0;
	IMPORT_C virtual const TDesC& Description() const;
protected:
	IMPORT_C CCommandBase();
	IMPORT_C CCommandBase(TUint aFlags);
	IMPORT_C void SetExtension(MCommandExtensionsV1* aExtension);
	IMPORT_C virtual void BaseConstructL();
	IMPORT_C void CreateEnvironmentL(CEnvironment* aEnv);

protected:
	virtual void DoRunL() = 0;
	IMPORT_C virtual void OptionsL(RCommandOptionList& aOptions);
	IMPORT_C virtual void ArgumentsL(RCommandArgumentList& aArguments);
	IMPORT_C virtual TInt ParseCommandLine(const TDesC& aCommandLine);
	IMPORT_C virtual void ParseCommandLineL(const TDesC& aCommandLine);
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
private:
	enum TPrivateFlags // Must not exceed 0xffff0000 (KPrivateFlagsMask).
		{
		EOwnsEnv					= 0x80000000,
		EOwnsHandles				= 0x40000000,
		ETlsSet						= 0x20000000,
		ECompleted					= 0x10000000,
		EErrorReported				= 0x08000000,
		EReadCif                    = 0x04000000,
		ECifReadFailed              = 0x02000000,
		EOwnsCif                    = 0x01000000
		};
	enum TReadStringOptions
		{
		EDisallowLeadingHyphen		= 0x00000001,
		EIncludeWhiteSpace			= 0x00000002,
		EBreakOnComma               = 0x00000004	// The default behaviour is to break on white space. If this flag is set both commas and white space (that are unquoted / escaped) will cause reading of the string to terminate.
		};
protected:
	IMPORT_C void SetCif(const CCommandInfoFile& aCif);
	IMPORT_C const CCommandInfoFile* Cif() const;
private:
	void RunCommandL(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const TDesC* aCommandLine, CEnvironment* aEnv, MCommandBaseObserver* aObserver);
	void RunL(const TDesC& aCommandLine);
	void SetValueL(TLex& aLex, TValue& aValue);
	void SetValueFromEnvL(TValue& aValue);
	void ClearValueL(TValue& aValue);
	const TDesC* ReadCommandLineLC();
	void DisplayHelpAndComplete();
	void DisplayHelpAndComplete(TInt aError, const TDesC& aText);
	void CreateHandlesL();
	void UpdateHandlesL();
	void DoCreateHandlesL(RIoConsole& aConsole, RIoConsoleReadHandle& aStdin, RIoConsoleWriteHandle& aStdout, RIoConsoleWriteHandle& aStderr);
	HBufC* ReadStringLC(TLex& aLex, TUint aOptions) const;
	void FormatErrorText(TDes& aDes, TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList) const;
	void FormatWarningText(TDes& aDes, TRefByValue<const TDesC> aFmt, VA_LIST& aList) const;
	void CheckNewLine();
	void FormatEnumL(const TValue& aValue, CTextBuffer& aBuffer) const;
	void PageL(MLineReader& aLineReader);
	void DoParseCommandLineL(const TDesC& aCommandLine);
	TBool CifReadRequired() const;
	void ReadCifL();
	void DoReadCifL();
private:
	friend class CCommandCompleter;
	void CompleteCallback(TInt aError);
protected: // From CActive.
	IMPORT_C virtual void RunL();
	IMPORT_C virtual void DoCancel();
	IMPORT_C virtual TInt RunError(TInt aError);
protected:
	RCommandOptionList iOptions;
	RCommandArgumentList iArguments;
private:
	TUint iFlags;
	CEnvironment* iEnv;
	MCommandBaseObserver* iObserver;
	mutable RFs iFs;
	RIoSession iIoSession;
	RIoConsoleReadHandle iStdin;
	RIoConsoleWriteHandle iStdout;
	RIoConsoleWriteHandle iStderr;
	TBool iDisplayHelp;
	TInt iSupressingErrors;
	HBufC* iConsoleImplementation;
	HBufC* iConsoleTitle;
	RArray<TUint> iConsoleSize;
	TBool* iDeleted;
	HBufC* iPersistentConsoleName;
	CCommandCompleter* iCompleter;
	CReaderChangeNotifier* iReadChangeNotifier;
	TInt iCompletionReason;
	CCommandInfoFile* iCif;
	MCommandExtensionsV1* iExtension;
	CKeypressWatcher* iKeypressWatcher;
	void* iSpare1;
	void* iSpare2;
	void* iSpare3;
	};


/**
 * @brief A helper class for creating and running child processes that share 
 * I/O handles (and hence console and / or redirected file handles) with the parent.
 *
 * Note, CreateL just creates the process; it doesn't start its main thread. This
 * can either be done by calling RChildProcess::Process().Resume() or calling
 * RChildProcess::Run. Either way it is important that the newly created process
 * is given a chance to open its I/O handles before RChildProcess::Close is called.
 * Otherwise the server side objects will be destroyed before it has had a chance
 * to claim a reference count on them. The easiest way to achieve this is by using
 * RChildProcess::Run, and waiting until the associated TRequestStatus is completed
 * before calling RChildProcess::Close. The TRequestStatus will be completed when
 * the child process exits.
 */
class RChildProcess
	{
public:
	IMPORT_C void Close();
	IMPORT_C void CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr);
	IMPORT_C void CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const CEnvironment& aEnv);
	IMPORT_C void CreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoConsole& aConsole, const CEnvironment* aEnv);
	IMPORT_C void Run(TRequestStatus& aStatus);
	IMPORT_C void Detach();
	IMPORT_C RProcess& Process();
	IMPORT_C const RProcess& Process() const;
private:
	void NameHandles(const TDesC& aExecutableName, RIoSession& aIoSession);
	void DoCreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, const CEnvironment* aEnv);
	void ProcessCreateL(const TDesC& aExecutableName, const TDesC& aCommandLine, const CEnvironment* aEnv);
private:
	RProcess iProcess;
	RIoReadHandle iStdin;
	RIoWriteHandle iStdout;
	RIoWriteHandle iStderr;
	};

IMPORT_C CConsoleBase* NewConsole(); // Only for use by iocons
IMPORT_C void ReadL(TDes& aData);
IMPORT_C void Write(const TDesC& aData);
IMPORT_C void Printf(TRefByValue<const TDesC> aFmt, ...);
IMPORT_C void Printf(TRefByValue<const TDesC8> aFmt, ...);
IMPORT_C void PrintError(TInt aError, TRefByValue<const TDesC> aFmt, ...);
IMPORT_C void PrintWarning(TRefByValue<const TDesC> aFmt, ...);


class CServerWatcher;
class CServerReader;
class CServerWriter;

/**
 * @brief The base class for command server client commands.
 *
 * Normally, CCommandBase derived objects are run in the context of a separate
 * process (or possibly thread). This process (or thread) only exists until
 * the command completes. This makes it difficult of such commands to maintain
 * state between repeated invokations.
 *
 * Command servers are a means of addressing this limitation. They are implemented
 * as a CCommandBase (actually a CServerBase) sub-class that runs in its own
 * process. However, the stdin and stdout handles of this command are not attached
 * directly to a console (or set of pipes) as is normally the case. Instead they are
 * attached to a persistent console (RIoPersistentConsole). This type of console
 * allows a real console (or set of pipes) to be transiently connected and then
 * disconnected without the server command being aware that this has happend. This
 * allows a client command to temporarily connect to the server command's stdio,
 * issue a command, and receive a response. This can happen numerous times to a
 * single server command instance, which allows the server command to maintain
 * state.
 *
 * Generally, server commands implement a set of internal commands that are
 * themselves CCommandBase (actually CServerCommandBase or CBranchCommandBase)
 * sub-classes. These are created as required inside the command server process
 * in response to client command requests.
 *
 * There is generally a single sub-class of this class (that is built into an
 * executable) to interact with a given server command. Its job is to:
 *
 * a) Create the server command process if its not already running.
 * b) Create a pair of pipes that are connected to the server command's
 *    persistent console.
 * c) Issue a command to the server (normally read from the command-line of
 *    this command).
 * d) Retrieve a response from the server.
 * e) Possibly parse the server response.
 * f) Print a result to this command's stdout (which, note, is completely
 *    independent of the server command's stdout).
 * g) Exit with an integer value indicating the success or failure of the
 *    command.
 *
 * This base class takes care of (a) - (d). The concrete sub-class is responsible
 * for (e) - (g).
 */
class CClientBase : public CCommandBase
	{
protected:
	IMPORT_C CClientBase(TUint aFlags, const TDesC& aServerExeName, const TDesC& aPersistentConsoleName, const TDesC& aServerPrompt);
	IMPORT_C ~CClientBase();
private:
	virtual void HandleServerResponseL(RPointerArray<HBufC> aLines) = 0;
private: // From CCommandBase.
	IMPORT_C virtual void DoRunL();
	IMPORT_C virtual void ArgumentsL(RCommandArgumentList& aArguments);
	IMPORT_C virtual void HandleLeave(TInt aError);
private:
	friend class CServerWatcher;
	friend class CServerReader;
	friend class CServerWriter;
	void HandleServerReadComplete(TInt aError, TDes& aLine);
	void HandleServerWriteComplete(TInt aError);
	void HandleServerDeath(TExitType aExitType, TInt aExitReason, const TDesC& aExitCategory);
	void SendCommand();
protected:
	TBool iVerbose;
private:
	const TPtrC iServerExeName;
	const TPtrC iPersistentConsoleName;
	const TPtrC iServerPrompt;
	HBufC* iCommand;
	RChildProcess iServerProcess;
	RIoPersistentConsole iPcons;
	RIoPipe iServerWritePipe;
	RIoWriteHandle iServerWriteHandle;
	RIoPipe iServerReadPipe;
	RIoReadHandle iServerReadHandle;
	CServerWatcher* iServerWatcher;
	CServerReader* iServerReader;
	CServerWriter* iServerWriter;
	RPointerArray<HBufC> iLines;
	TBool iWaitingForServerPrompt;
	};


/**
 * @brief A class used to identify instances of individual commands running inside
 * a command server (which is a CServerBase sub-class).
 *
 * Note, it is up to a suitable sub-class (of usually CBranchCommandBase) to assign
 * these identifiers via CServerCommandBase::SetId.
 */
class TServerCommandId
	{
public:
	IMPORT_C TServerCommandId();
	IMPORT_C TServerCommandId(TUint aValue);
	IMPORT_C TUint Value() const;
	IMPORT_C void Set(const TServerCommandId& aId);
	IMPORT_C TBool operator==(const TServerCommandId& aId) const;
private:
	TUint iId;
	};


/**
 * @brief An interface used by CServerCommandBase to report information to the server's
 * user.
 *
 * Note, this class is not intended for external derivation. Rather, sub-classes of
 * CServerBase must provide an implementation of its simplified pure virtual interface
 * (Report, ReportWarning and ReportError).
 */
class MServerCommandReporter
	{
public:
	virtual void Report(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...) = 0;
	virtual void ReportList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList) = 0;
	virtual void Report(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, ...) = 0;
	virtual void ReportList(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, VA_LIST& aList) = 0;
	virtual void ReportWarning(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...) = 0;
	virtual void ReportWarningList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList) = 0;
	virtual void ReportError(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, ...) = 0;
	virtual void ReportErrorList(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList) = 0;
	};


class CServerCommandFactory;

/**
 * @brief The base class for command servers.
 *
 * Each command server is expected to create one (and only one) sub-class of this class. It is
 * effectively the root object of the server and is responsible for parsing input received on stdin
 * and creates corresponding command objects accordingly. Note, all textual responses from the
 * server's commands are routed through the sub-class of this class via its pure virtual interface.
 * Note also, only one call to ReportError will be received for a given line read from stdin.
 */
class CServerBase : public CCommandBase, public MServerCommandReporter, public MLineEditorObserver
	{
public:
	IMPORT_C ~CServerBase();
	IMPORT_C void Exit(TInt aError);
protected:
	IMPORT_C CServerBase(TUint aFlags, const TDesC& aPrompt, const TDesC& aCommandHistoryFileName);
	IMPORT_C CServerCommandFactory& Factory();
	IMPORT_C void CheckNewConsoleLine();
protected:
	virtual void InitializeL() = 0;
	virtual void Report(const TServerCommandId& aId, const TDesC& aDes) = 0;
	virtual void Report(const TServerCommandId& aId, const TDesC8& aDes) = 0;
	virtual void ReportWarning(const TServerCommandId& aId, const TDesC& aDes) = 0;
	virtual void ReportError(const TServerCommandId& aId, TInt aError, const TDesC& aDes) = 0;
protected: // From CCommandBase.
	IMPORT_C virtual void BaseConstructL();
	IMPORT_C virtual void DoRunL();
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
private: // From MServerCommandReporter.
	IMPORT_C virtual void Report(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C virtual void ReportList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void Report(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, ...);
	IMPORT_C virtual void ReportList(const TServerCommandId& aId, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C virtual void ReportWarning(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C virtual void ReportWarningList(const TServerCommandId& aId, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void ReportError(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C virtual void ReportErrorList(const TServerCommandId& aId, TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
private: // From MLineEditorObserver.
	IMPORT_C virtual void LeoHandleLine(const TDesC& aLine);
private:
	void ParseLineL(const TDesC& aLine);
	const TDesC& NextLineL();
private:
	const TDesC& iPrompt;
	const TDesC& iCommandHistoryFileName;
	TBuf<512> iLine;
	CLineEditor* iLineEditor;
	CServerCommandFactory* iFactory;
	TIoConsWriterAdaptor iWriterAdaptor;
	TBool iLineRead;
	TBool iExit;
	TInt iExitReason;
	TBool iErrorReported;
	};


class CServerCommandConstructor;

/**
 * @brief The base class for commands that run within a command server.
 *
 * A class should be derived from this class for each "leaf" command of
 * the server. A leaf command is one that doesn't support any sub-commands
 * (see CBranchCommand for details of commands that do support sub-commands).
 * Each sub-class must be registered with a CServerCommandFactory object
 * via the template method AddLeafCommandL.
 */
class CServerCommandBase : public CCommandBase
	{
public:
	IMPORT_C virtual ~CServerCommandBase();
	IMPORT_C const TServerCommandId& Id() const;
	IMPORT_C void SetId(const TServerCommandId& aId);
	IMPORT_C void SetFactory(CServerCommandFactory& aFactory);
protected:
	IMPORT_C CServerCommandBase();
	IMPORT_C CServerCommandBase(TUint aFlags);
	IMPORT_C CServerCommandFactory& Factory();
private: // From CCommandBase.
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintList(TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintErrorList(TInt aError, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	IMPORT_C virtual void PrintWarningList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
private:
	friend class CServerBase;
	friend class CServerCommandConstructor;
	void SetReporter(MServerCommandReporter* aReporter);
private:
	TServerCommandId iId;
	MServerCommandReporter* iReporter;
	CServerCommandFactory* iFactory;
	};


typedef CServerCommandBase* (*TServerCommandConstructor)();

/**
 * @brief A helper class for CServerCommandFactory used to store a means of
 * constructing leaf commands.
 */
class CServerCommandConstructor : public CBase
	{
public:
	IMPORT_C static CServerCommandConstructor* NewLC(const TDesC& aName, TServerCommandConstructor aConstructor, CServerCommandFactory& aFactory, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
	IMPORT_C ~CServerCommandConstructor();
	IMPORT_C const TPtrC& Name() const;
	IMPORT_C CServerCommandBase* CreateImplementationLC() const;
	IMPORT_C virtual TBool IsFactory() const;
protected:
	CServerCommandConstructor(const TDesC& aName, TServerCommandConstructor aConstructor, CServerCommandFactory& aFactory, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
protected:
	TPtrC iName;
	TServerCommandConstructor iConstructorLC;
	CServerCommandFactory& iFactory;
	MServerCommandReporter* iReporter;
	const CCommandInfoFile* iCif;
	};


/**
 * @brief A class that can be used to manage a hierarchy of commands.
 */
class CServerCommandFactory : public CServerCommandConstructor, public MLineCompleter
	{
public:
	IMPORT_C static CServerCommandFactory* NewL(MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
	IMPORT_C static CServerCommandFactory* NewLC(const TDesC& aName, TServerCommandConstructor aConstructor, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
	IMPORT_C ~CServerCommandFactory();
	IMPORT_C const CServerCommandConstructor& GetSubCommandL(const TDesC& aName) const;
	IMPORT_C virtual TBool IsFactory() const;
	template <class SubCmnd> inline CServerCommandConstructor& AddLeafCommandL();
	template <class SubCmnd> inline CServerCommandFactory& AddFactoryCommandL();
	IMPORT_C void ListCommandsL(RArray<TPtrC>& aList) const;
private: // From MLineCompleter.
	virtual void LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar);
private:
	enum TFactoryType
		{
		ERoot,
		EBranch
		};
private:
	CServerCommandFactory(MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
	CServerCommandFactory(const TDesC& aName, TServerCommandConstructor aConstructor, MServerCommandReporter* aReporter, const CCommandInfoFile* aCif);
	void ConstructL(TFactoryType aType);
	void CompleteLineL(TConsoleLine& aLine, TInt aPos, const RArray<TPtrC> aPossibilities) const;
	const CServerCommandConstructor* GetSubCommand(const TDesC& aName) const;
	IMPORT_C void AddSubCommandL(const CServerCommandConstructor* aCommand);
private:
	RHashMap<const TPtrC, const CServerCommandConstructor*> iSubCommands;
	};


/**
 * @brief The base class for commands that support multiple sub-commands.
 */
class CBranchCommandBase : public CServerCommandBase
	{
public:
	IMPORT_C virtual ~CBranchCommandBase();
protected:
	IMPORT_C CBranchCommandBase();
	IMPORT_C virtual const TDesC& TypeDescription();
	IMPORT_C virtual void ConfigureSubCommand(CServerCommandBase& aNewlyConstructedCommand);
	IMPORT_C virtual void HandleSubCommandComplete(CServerCommandBase& aCommand);
	IMPORT_C virtual void HandleBackgroundSubCommand(CServerCommandBase& aCommand);
private:
	void BuildEnumL();
private: // From CCommandBase.
	IMPORT_C virtual void DoRunL();
	IMPORT_C virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	IoUtils::CTextBuffer* iSubCommandEnum;
	HBufC* iSubCommandArgs;
	TInt iType;
	};

#include <fshell/ioutils.inl>

	} // Closing namespace brace.


#define EXE_BOILER_PLATE(__CLASS_NAME) \
void MainL()\
	{\
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;\
	CleanupStack::PushL(scheduler);\
	CActiveScheduler::Install(scheduler);\
	IoUtils::CCommandBase* command = __CLASS_NAME::NewLC();\
	command->RunCommandL();\
	CleanupStack::PopAndDestroy(2, scheduler);\
	}\
\
GLDEF_C TInt E32Main()\
	{\
	__UHEAP_MARK;\
	TInt err = KErrNoMemory;\
	CTrapCleanup* cleanup = CTrapCleanup::New();\
	if (cleanup)\
		{\
		TRAP(err, MainL());\
		delete cleanup;\
		}\
	__UHEAP_MARKEND;\
	return err;\
	}\

// Note, this used to be a member function of CCommandBase (called "LeaveIfError"). It was changed to a macro to work around
// a problem with GCCE (version "arm-none-symbianelf-gcc.exe (GCC) 3.4.3 (release) (CodeSourcery ARM Q1C 2005)") whereby 
// USER-EXEC 3 panics would occur if a leave from a function with variadic parameters took place.
#define LeaveIfErr(_error, _errorFmtString...) \
	do \
		{ \
		TInt __err = (_error); \
		if (__err < 0) \
			{ \
			this->PrintError(__err, _errorFmtString); \
			User::Leave(__err); \
			} \
		} \
		while (0)

#define StaticLeaveIfErr(_error, _errorFmtString...) \
	do \
		{ \
		TInt __err = (_error); \
		if (__err < 0) \
			{ \
			if (IoUtils::CCommandBase::HaveStatic()) IoUtils::PrintError(__err, _errorFmtString); \
			User::Leave(__err); \
			} \
		} \
		while (0)

#define CommandLeaveIfErr(_command, _error, _errorFmtString...) \
	do \
		{ \
		TInt __err = (_error); \
		if (__err < 0) \
			{ \
			(_command).PrintError(__err, _errorFmtString); \
			User::Leave(__err); \
			} \
		} \
		while (0)

#define TRAPL(leavingFunction, errorFmtString...) { TRAPD(_err, leavingFunction); LeaveIfErr(_err, errorFmtString); }
#define STRAPL(leavingFunction, errorFmtString...) { TRAPD(_err, leavingFunction); StaticLeaveIfErr(_err, errorFmtString); }
#define CTRAPL(command, leavingFunction, errorFmtString...) { TRAPD(_err, leavingFunction); CommandLeaveIfErr(command, _err, errorFmtString); }
#define TRAP_QUIETLY(err, leavingFunction) \
	if (IoUtils::CCommandBase::HaveStatic()) \
		{ \
		IoUtils::CCommandBase::Static().StartSupressingErrors(); \
		TRAP(err, leavingFunction); \
		IoUtils::CCommandBase::Static().StopSupressingErrors(); \
		} \
	else \
		{ \
		TRAP(err, leavingFunction); \
		}
		
#define TRAPD_QUIETLY(_err, leavingFunction) TInt _err = KErrNone; TRAP_QUIETLY(_err, leavingFunction);
#define TRAP_IGNORE_QUIETLY(leavingFunction) { TRAPD_QUIETLY(_err, leavingFunction); (void)_err; }

#endif // __IOUTILS_H__
