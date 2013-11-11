// bsym.cpp
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
#include <fshell/descriptorutils.h>
#include <fshell/bsym.h>

#include <fshell/iocli.h>
using LtkUtils::CBsymFile;
using namespace IoUtils;
#include "bsymtree.h"
using LtkUtils::RNode;

#ifdef ASSERT
#undef ASSERT
#endif

_LIT(KBsymPanic, "BSym");
#define ASSERT(x) __ASSERT_ALWAYS(x, User::Panic(KBsymPanic, __LINE__))
#define DEBUG_XY(x, y) RDebug::Printf(#x " = %d " #y " = %d", (int)(x), (int)(y))
#define ASSERT_GE(x, y) if (!((x) >= (y))) { DEBUG_XY(x, y); ASSERT(x >= y); }
#define ASSERT_GT(x, y) if (!((x) >  (y))) { DEBUG_XY(x, y); ASSERT(x >  y); }
#define ASSERT_LE(x, y) if (!((x) <= (y))) { DEBUG_XY(x, y); ASSERT(x <= y); }
#define ASSERT_LT(x, y) if (!((x) <  (y))) { DEBUG_XY(x, y); ASSERT(x <  y); }
#define ASSERT_EQ(x, y) if (!((x) == (y))) { DEBUG_XY(x, y); ASSERT(x == y); }
#define ASSERT_NE(x, y) if (!((x) != (y))) { DEBUG_XY(x, y); ASSERT(x != y); }

#define LOG(args...)
//#include <fshell/clogger.h>
//#define LOG(args...) RClogger::Slog(args)

enum TBsymVersion
	{
	EVersion1_0 = 0x00010000,
	EVersion2_0 = 0x00020000,
	EVersion2_1 = 0x00020001,
	};
static const TUint32 KMajorVersion = 0xFFFF0000;
static const TUint32 KMinorVersion = 0x0000FFFF;
static const TUint32 KMagic = ('B'<<24)|('S'<<16)|('Y'<<8)|'M';

inline TUint32 fromBigEndian(TUint32 aVal)
	{
	TUint8* ptr = (TUint8*)&aVal;
	return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
	}

inline TUint32 toBigEndian(TUint32 aVal)
	{
	return fromBigEndian(aVal); // Same thing really
	}

class TSymbol
	{
public:
	TUint32 Address() const { return fromBigEndian(iData[0]); }
	TUint32 Length() const { return fromBigEndian(iData[1]) & 0xFFFF; }
	TUint32 NameOffset() const { return fromBigEndian(iData[2]); }
	TUint32 NamePrefixIndex() const { return fromBigEndian(iData[1]) >> 16; }
	
	TSymbol(TUint32 aAddress) { iData[0] = toBigEndian(aAddress); }

protected:
	TUint32 iData[3];
	};

class TCodeSeg : public TSymbol
	{
public:
	TUint32 SymbolStart() const { return fromBigEndian(iSymbolsStartIndex); }
	TUint32 SymbolCount() const { return fromBigEndian(iData[1]); } // Code segs use the TSymbol 'length' field to store the number of symbols, *not* the length. To get the length of a codeseg, call CBsymFile::CodeSegLen(const TCodeSeg&)
	TUint32 PrefixTableOffset() const { return fromBigEndian(iPrefixTableOffset); }
	
	TCodeSeg(TUint32 aAddress) : TSymbol(aAddress) {}
private:
	TUint32 iSymbolsStartIndex;
	TUint32 iPrefixTableOffset;
	};

class TCodeAndSym
	{
public:
	TCodeAndSym(const TCodeSeg* aCodeSeg, const TSymbol* aSymbol);
	TCodeSeg iCodeSeg;
	TSymbol iSymbol;
	};

NONSHARABLE_CLASS(CBsymFileImpl) : public CBsymFile
	{
public:
	CBsymFileImpl();
	~CBsymFileImpl();
	void ConstructL(RFs& aFs, const TDesC& aFileName);
	TPtrC DoLookupL(TUint32 aRomAddress);
	TCodeAndSym DoLookup(TUint32 aAddress) const; // Only useful for ROM symbols files
	void LoadSymbolNameL(const TSymbol* aSymbol, const TCodeSeg* aParentCodeSeg);
	RNode* DoCreateCompletionTreeL(const TDesC& aCodesegName);
	TPtrC DoLookupL(const TDesC& aCodesegName, TInt aOffset);

private:
	TUint32 UintL(TInt aOffset) const;
	TUint32 UnalignedUintL(TInt aOffset) const;
	TCodeSeg CodeSegL(TInt aIndex) const;
	TSymbol SymbolL(TInt aIndex) const;

	TUint32 CodesegSectionLengthL() const;
	TUint32 SymbolsSectionLengthL() const;
	TInt SymbolCountL() const;
	TInt CodeSegCountL() const;
	TInt TokenCountL() const;

	void CreateCodesegHashIfNeededL();
	TCodeAndSym DoCodesegLookup(const TCodeSeg& aCodeSeg, TUint32 aAddress) const;
	HBufC* GetTokenL(TInt aToken) const;
	HBufC* GetStringL(TUint32 aLocation, TBool aExpandTokens=ETrue) const;
	TUint32 CodeSegLengthL(const TCodeSeg* aCodeSeg) const;

	friend class TCodeSegKey;
	friend class TSymbolKey;

private:
	LtkUtils::RLtkBuf iTempString;
	LtkUtils::RStringHash<TCodeSeg> iCodeSegHash;
	TInt iFileCacheOffset;
	LtkUtils::RLtkBuf8 iFileCache;
	};

NONSHARABLE_CLASS(TCodeSegKey) : public TKey
	{
public:
	TCodeSegKey(const CBsymFileImpl* aFile, TUint32 aAddress);
	TInt Compare(TInt aLeft, TInt aRight) const;

private:
	const CBsymFileImpl* iFile;
	TUint32 iAddress;
	};

NONSHARABLE_CLASS(TSymbolKey) : public TKey
	{
public:
	TSymbolKey(const CBsymFileImpl* aFile, const TCodeSeg& aCodeSeg, TUint32 aAddress);
	TInt Compare(TInt aLeft, TInt aRight) const;

private:
	const CBsymFileImpl* iFile;
	const TCodeSeg& iCodeSeg;
	TUint32 iAddress;
	};

EXPORT_C CBsymFile* CBsymFile::NewL(RFs& aFs, const TDesC& aFileName)
	{
	CBsymFileImpl* result = new (ELeave) CBsymFileImpl;
	CleanupStack::PushL(result);
	result->ConstructL(aFs, aFileName);
	CleanupStack::Pop(result);
	return result;
	}

EXPORT_C CBsymFile::~CBsymFile()
	{
	iFile.Close();
	}

EXPORT_C TPtrC CBsymFile::LookupL(TUint32 aAddress)
	{
	CBsymFileImpl* impl = static_cast<CBsymFileImpl*>(this); // We know we're actually a CBsymFileImpl
	return impl->DoLookupL(aAddress);
	}

TPtrC CBsymFileImpl::DoLookupL(TUint32 aAddress)
	{
	iTempString.Zero();
	TCodeAndSym res = DoLookup(aAddress);
	if (res.iSymbol.Address())
		{
		LoadSymbolNameL(&res.iSymbol, &res.iCodeSeg);
		iTempString.AppendFormatL(_L(" + 0x%x"), aAddress - res.iSymbol.Address());
		}
	return TPtrC(iTempString);
	}

CBsymFile::CBsymFile()
	{
	}

CBsymFileImpl::CBsymFileImpl()
	{
	}

CBsymFileImpl::~CBsymFileImpl()
	{
	iTempString.Close();
	iCodeSegHash.Close();
	iFileCache.Close();
	}

TUint32 CBsymFileImpl::UintL(TInt aOffset) const
	{
	ASSERT((aOffset & 3) == 0);
	return UnalignedUintL(aOffset);
	}

TUint32 CBsymFileImpl::UnalignedUintL(TInt aOffset) const
	{	
	ASSERT_LE((TUint)aOffset+4, iFileSize);
	TBuf8<4> buf;
	TInt err = iFile.Read(aOffset, buf);
	User::LeaveIfError(err);
	return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]);
	}

#undef LeaveIfErr
#define LeaveIfErr(args...) StaticLeaveIfErr(args)

void CBsymFileImpl::ConstructL(RFs& aFs, const TDesC& aFileName)
	{
	iTempString.CreateL(256);
	LeaveIfErr(iFile.Open(aFs, aFileName, EFileShareReadersOnly), _L("Couldn't open file %S"), &aFileName);
	LeaveIfErr(iFile.Size(*reinterpret_cast<TInt*>(&iFileSize)), _L("Couldn't read file size"));
	if (iFileSize < 5*4)
		{
		LeaveIfErr(KErrCorrupt, _L("%S is too small to be a bsym file"), &aFileName);
		}
	// Validate the data
	if (UintL(0) != KMagic)
		{
		LeaveIfErr(KErrCorrupt, _L("%S is not a BSYM file."), &aFileName);
		}
	iVersion = UintL(4);
	iCodesegOffset = UintL(8);
	iSymbolsOffset = UintL(0x0C);
	
	TUint32 majorVersion = (iVersion&KMajorVersion);
	if (majorVersion != EVersion1_0 && majorVersion != EVersion2_0)
		{
		LeaveIfErr(KErrNotSupported, _L("Unsupported bysm version v%d.%d in %S"), iVersion>>16, iVersion & KMinorVersion, &aFileName);
		}
	if (iVersion >= EVersion2_0)
		{
		iTokensOffset = UintL(0x10);
		if (iTokensOffset + 4 > iFileSize || TokenCountL() > 128 || iTokensOffset + 4 + TokenCountL()*4 > iFileSize)
			{
			LeaveIfErr(KErrCorrupt, _L("Bad token offset %x or count"), iTokensOffset);
			}
		}
	if (iVersion >= EVersion2_1)
		{
		TUint32 renamesOffset = UintL(0x14);
		if (renamesOffset + 4 > iFileSize || renamesOffset + UintL(renamesOffset)*8 > iFileSize)
			{
			// UintL(renamesOffset)*8 is size of renames section
			LeaveIfErr(KErrCorrupt, _L("Bad rename offset %x or count"), renamesOffset);
			}
		}
	if (iCodesegOffset+4 > iFileSize || iCodesegOffset + CodesegSectionLengthL() > iFileSize)
		{
		LeaveIfErr(KErrCorrupt, _L("Bad codeseg offset %x or count"), iCodesegOffset);
		}
	if (iSymbolsOffset+4 > iFileSize || iSymbolsOffset + SymbolsSectionLengthL() > iFileSize)
		{
		LeaveIfErr(KErrCorrupt, _L("Bad symbols offset %x or count"), iSymbolsOffset);
		}

	// Don't construct the codeseg hash now, it takes too long and we only need it if we need to do completions
	}

TCodeSeg CBsymFileImpl::CodeSegL(TInt aIndex) const
	{
	TCodeSeg result(0);
	const TInt fileIdx = iCodesegOffset + 4 + aIndex * sizeof(TCodeSeg);
	if (fileIdx >= iFileCacheOffset && fileIdx + sizeof(TCodeSeg) <= iFileCacheOffset + iFileCache.Length())
		{
		Mem::Copy(&result, iFileCache.Ptr() + fileIdx - iFileCacheOffset, sizeof(TCodeSeg));
		}
	else
		{
		TPckg<TCodeSeg> pkg(result);
		User::LeaveIfError(iFile.Read(fileIdx, pkg));
		}
	return result;
	}
	
TSymbol CBsymFileImpl::SymbolL(TInt aIndex) const
	{
	TSymbol result(0);
	const TInt fileIdx = iSymbolsOffset + 4 + aIndex * sizeof(TSymbol);
	TPckg<TSymbol> pkg(result);
	User::LeaveIfError(iFile.Read(fileIdx, pkg));
	return result;
	}
	
TUint32 CBsymFileImpl::CodesegSectionLengthL() const
	{
	return 4 + CodeSegCountL() * sizeof(TCodeSeg);
	}

TUint32 CBsymFileImpl::SymbolsSectionLengthL() const
	{
	return 4 + SymbolCountL() * sizeof(TSymbol);
	}

TInt CBsymFileImpl::SymbolCountL() const
	{
	return UintL(iSymbolsOffset);
	}

TInt CBsymFileImpl::CodeSegCountL() const
	{
	return UintL(iCodesegOffset);
	}

TUint32 CBsymFileImpl::CodeSegLengthL(const TCodeSeg* aCodeSeg) const
	{
	if (aCodeSeg->SymbolCount())
		{
		TSymbol lastSymbol = SymbolL(aCodeSeg->SymbolStart() + aCodeSeg->SymbolCount()-1);
		return lastSymbol.Address() + lastSymbol.Length() - aCodeSeg->Address();
		}
	else
		{
		return 0;
		}
	}

TCodeSegKey::TCodeSegKey(const CBsymFileImpl* aFile, TUint32 aAddress)
	: TKey(), iFile(aFile), iAddress(aAddress)
	{}

TInt TCodeSegKey::Compare(TInt aLeft, TInt aRight) const
	{
	TUint32 left = aLeft == KIndexPtr ? iAddress : iFile->CodeSegL(aLeft).Address();
	TUint32 right = aRight == KIndexPtr ? iAddress : iFile->CodeSegL(aRight).Address();
	return (TInt)left - (TInt)right;
	}


TSymbolKey::TSymbolKey(const CBsymFileImpl* aFile, const TCodeSeg& aCodeSeg, TUint32 aAddress)
	: TKey(), iFile(aFile), iCodeSeg(aCodeSeg), iAddress(aAddress)
	{}

TInt TSymbolKey::Compare(TInt aLeft, TInt aRight) const
	{
	TUint32 left = aLeft == KIndexPtr ? iAddress : iFile->SymbolL(iCodeSeg.SymbolStart() + aLeft).Address();
	TUint32 right = aRight == KIndexPtr ? iAddress : iFile->SymbolL(iCodeSeg.SymbolStart() + aRight).Address();
	return (TInt)left - (TInt)right;
	}

TCodeAndSym::TCodeAndSym(const TCodeSeg* aCodeSeg, const TSymbol* aSymbol)
	: iCodeSeg(0), iSymbol(0)
	{
	if (aCodeSeg) iCodeSeg = *aCodeSeg;
	if (aSymbol) iSymbol = *aSymbol;
	}

TCodeAndSym CBsymFileImpl::DoLookup(TUint32 aAddress) const
	{
	// Find codeseg (need to know this if the symbol has a name prefix)
	TInt pos = 0;
	TCodeSegKey codesegKey(this, aAddress);
	TBool found = !User::BinarySearch(CodeSegCountL(), codesegKey, pos);
	if (!found && pos != 0) pos--;
	TCodeSeg codeseg = CodeSegL(pos);

	if (aAddress >= codeseg.Address() && aAddress < codeseg.Address() + CodeSegLengthL(&codeseg))
		{
		return DoCodesegLookup(codeseg, aAddress);
		}
	return TCodeAndSym(NULL, NULL);
	}

EXPORT_C TPtrC CBsymFile::LookupL(const TDesC& aCodesegName, TInt aOffset)
	{
	return static_cast<CBsymFileImpl*>(this)->DoLookupL(aCodesegName, aOffset);
	}

TPtrC CBsymFileImpl::DoLookupL(const TDesC& aCodesegName, TInt aOffset)
	{
	CreateCodesegHashIfNeededL();
	const TCodeSeg* codeseg = iCodeSegHash.Find(aCodesegName);
	if (!codeseg) return TPtrC();
	TUint32 addr = codeseg->Address() + aOffset;
	TCodeAndSym res = DoCodesegLookup(*codeseg, addr);
	iTempString.Zero();
	if (res.iSymbol.Address())
		{
		LoadSymbolNameL(&res.iSymbol, &res.iCodeSeg);
		iTempString.AppendFormatL(_L(" + 0x%x"), addr - res.iSymbol.Address());
		}
	return TPtrC(iTempString);
	}

TCodeAndSym CBsymFileImpl::DoCodesegLookup(const TCodeSeg& aCodeSeg, TUint32 aAddress) const
	{
	TInt pos = 0;
	TSymbolKey symbolKey(this, aCodeSeg, aAddress);
	TBool found = !User::BinarySearch(aCodeSeg.SymbolCount(), symbolKey, pos);
	if (!found && pos != 0) pos--;
	TSymbol symbol = SymbolL(aCodeSeg.SymbolStart() + pos);

	if (aAddress >= symbol.Address() && aAddress < symbol.Address() + symbol.Length())
		return TCodeAndSym(&aCodeSeg, &symbol);
	// TODO: Handle cases where the address falls outside of any symbol but still within the code segment, ie when symbols in the symbol file are not contiguous. This does happen, maybe because of padding/alignment.
	//qFatal("DoCodesegLookup with aAddress not within any symbol in aCodeSeg");
	return TCodeAndSym(NULL, NULL);
	}

HBufC* CBsymFileImpl::GetStringL(TUint32 aLocation, TBool aExpandTokens) const
	{
	TPckgBuf<TUint8> len8;
	User::LeaveIfError(iFile.Read(aLocation, len8));

	TUint16 nameLen = (TUint16)len8();
	aLocation++;
	if (nameLen == 255)
		{
		TPckgBuf<TUint16> len16;
		User::LeaveIfError(iFile.Read(aLocation, len16));
		nameLen = len16() >> 8 | len16() << 8;
		aLocation += 2;
		}
	RBuf8 temp;
	CleanupClosePushL(temp);
	temp.CreateL(nameLen);
	User::LeaveIfError(iFile.Read(aLocation, temp));
	LtkUtils::RLtkBuf res;
	res.CreateL(nameLen);
	res.Copy(temp);
	CleanupStack::PopAndDestroy(&temp);
	CleanupClosePushL(res);
	if (aExpandTokens && iVersion >= EVersion2_0)
		{
		for (TInt i = res.Length() - 1; i >= 0; i--)
			{
			TUint16 val = res[i];
			if (val >= 128)
				{
				HBufC* token = GetTokenL(val-128);
				CleanupStack::PushL(token);
				res.ReplaceL(i, 1, *token);
				CleanupStack::PopAndDestroy(token);
				}
			}

		}
	CleanupStack::Pop(&res);
	return res.ToHBuf();
	}

void CBsymFileImpl::LoadSymbolNameL(const TSymbol* aSymbol, const TCodeSeg* aParentCodeSeg)
	{
	TUint32 prefixIdx = aSymbol->NamePrefixIndex();
	if (aParentCodeSeg && prefixIdx)
		{
		// Then the symbol name has a prefix that needs looking up separately in the codeseg 
		TUint32 prefixLocation = aParentCodeSeg->PrefixTableOffset() + ((prefixIdx-1) * sizeof(TUint32));
		// This location has the offset of the relevant string
		HBufC* prefix = GetStringL(UnalignedUintL(prefixLocation));
		CleanupStack::PushL(prefix);
		iTempString.AppendL(*prefix);
		CleanupStack::PopAndDestroy(prefix);
		iTempString.AppendL(_L("::"));
		}

	TUint32 nameOffset = aSymbol->NameOffset();
	HBufC* result = GetStringL(nameOffset);
	CleanupStack::PushL(result);
	iTempString.AppendL(*result);
	CleanupStack::PopAndDestroy(result);
	}

TInt CBsymFileImpl::TokenCountL() const
	{
	if (iTokensOffset != 0)
		{
		return UintL(iTokensOffset);
		}
	else
		{
		return 0;
		}
	}

HBufC* CBsymFileImpl::GetTokenL(TInt aToken) const
	{
	if (aToken >= TokenCountL())
		{
		LeaveIfErr(KErrCorrupt, _L("Request for token %d >= tokenCount %d"), aToken, TokenCountL());
		}
	TUint32 tokenOffset = UintL(iTokensOffset + 4 + 4*aToken);
	return GetStringL(tokenOffset, EFalse);
	}

RNode* CBsymFile::CreateCompletionTreeL(const TDesC& aCodesegName)
	{
	return static_cast<CBsymFileImpl*>(this)->DoCreateCompletionTreeL(aCodesegName);
	}

RNode* CBsymFileImpl::DoCreateCompletionTreeL(const TDesC& aCodesegName)
	{
	CreateCodesegHashIfNeededL();
	TCodeSeg* codeseg = iCodeSegHash.Find(aCodesegName);
	if (!codeseg)
		{
		LOG(_L("Couldn't find %S in bsym's codeseg hash"), &aCodesegName);
		return NULL;
		}
	RNode* result = RNode::NewL();
	CleanupDeletePushL(result);

	const TInt symbolcount = codeseg->SymbolCount();
	const TUint32 symbolstart = codeseg->SymbolStart();
	LOG("Constructing RNode with %d symbols", symbolcount);
	for (TInt i = 0; i < symbolcount; i++)
		{
		TSymbol symbol = SymbolL(symbolstart + i);
		iTempString.Zero();
		LoadSymbolNameL(&symbol, codeseg);
		iTempString.ReserveExtraL(1);
		LOG(_L("Adding symbol %S @ 0x%08x"), &iTempString, symbol.Address() - codeseg->Address());
		result->InsertStringL(iTempString.PtrZ(), symbol.Address() - codeseg->Address());
		}
	CleanupStack::Pop(result);
	return result;
	}

void CBsymFileImpl::CreateCodesegHashIfNeededL()
	{
	if (iCodeSegHash.Count() > 0) return; // Already populated

	const TInt count = CodeSegCountL();
	iCodeSegHash.ReserveL(count);
	// Cut down the number of IPC calls by preloading the whole codeseg area in one (since we know we'll need it all)
	iFileCache.CreateL(4 + count * sizeof(TCodeSeg));
	iFileCacheOffset = iCodesegOffset;
	User::LeaveIfError(iFile.Read(iFileCacheOffset, iFileCache));
	for (TInt i = 0; i < count; i++)
		{
		TCodeSeg codeseg = CodeSegL(i);
		iTempString.Zero();
		LoadSymbolNameL(&codeseg, NULL);
		iTempString.Delete(0, iTempString.Length() - TParsePtrC(iTempString).NameAndExt().Length());

		LOG(_L("Adding %S to codeseg hash"), &iTempString);
		iCodeSegHash.InsertL(iTempString, codeseg);
		if (iVersion < EVersion2_1)
			{
			// Fall back to some hacky guesses about possible renamed binaries
			_LIT(KEuser, "euser_");
			if (iTempString.Length() && iTempString[0] == '_')
				{
				TInt lastUnderscore = iTempString.LocateReverse('_');
				if (lastUnderscore > 0)
					{
					iTempString.Delete(0, lastUnderscore + 1);
					if (iTempString.Length()) iCodeSegHash.InsertL(iTempString, codeseg);
					}
				}
			else if (iTempString.Left(KEuser().Length()) == KEuser)
				{
				// EUser appears to be a special case that some platforms do differently, as euser_variant.dll
				iCodeSegHash.InsertL(_L("euser.dll"), codeseg);
				}
			}
		}

	if (iVersion >= EVersion2_1)
		{
		TUint32 renamesOffset = UintL(0x14);
		const TInt renamesCount = UintL(renamesOffset);
		for (TInt i = 0; i < renamesCount; i++)
			{
			TUint32 offset = renamesOffset + i*8;
			TInt codesegIdx = UintL(offset);
			TUint32 stringOffset = UintL(offset+4);
			TCodeSeg codeseg = CodeSegL(codesegIdx);
			GetStringL(stringOffset);
			iCodeSegHash.InsertL(iTempString, codeseg);
			}
		}

	iFileCache.Close();
	}
