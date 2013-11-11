// mapfile.cpp
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
#include "bsymtree.h"
#include <fshell/descriptorutils.h>

#include <fshell/iocli.h>
using namespace LtkUtils;
using namespace IoUtils;

const TInt KSymbolGranularity = 256; // I have no idea...

struct SSymbol
	{
	TUint iAddress;
	TUint iLength;
	HBufC8* iName;
	};

NONSHARABLE_CLASS(CMapFileImpl) : public CMapFile
	{
public:
	CMapFileImpl();
	~CMapFileImpl();
	void ConstructL(RFs& aFs, const TDesC& aFileName);
	void DoLookup(TUint32 aOffsetInCodeSeg, TDes& aResult);
	RNode* DoCreateCompletionTreeL();

private:
	RArray<SSymbol> iSymbols;
	};

EXPORT_C CMapFile* CMapFile::NewL(RFs& aFs, const TDesC& aFileName)
	{
	CMapFileImpl* result = new (ELeave) CMapFileImpl;
	CleanupStack::PushL(result);
	//CleanupStack::PushL((CBase*)1); //DEBUG
	result->ConstructL(aFs, aFileName);
	//CleanupStack::Pop(); //DEBUG
	CleanupStack::Pop(result);
	return result;
	}

CMapFile::CMapFile()
	{
	}

CMapFileImpl::CMapFileImpl()
	: iSymbols(KSymbolGranularity, _FOFF(SSymbol, iAddress))
	{
	}

CMapFileImpl::~CMapFileImpl()
	{
	const TInt count = iSymbols.Count();
	for (TInt i = 0; i < count; i++)
		{
		delete iSymbols[i].iName;
		}
	iSymbols.Close();
	}

EXPORT_C CMapFile::~CMapFile()
	{
	iReadBuf.Close();
	delete iFileName;
	}

#undef LeaveIfErr
#define LeaveIfErr(args...) StaticLeaveIfErr(args)

void CMapFileImpl::ConstructL(RFs& aFs, const TDesC& aFileName)
	{
	iFileName = aFileName.AllocL();
	iReadBuf.CreateL(4096);
	LeaveIfErr(iFile.Open(aFs, aFileName, EFileShareReadersOnly), _L("Couldn't open file %S"), &aFileName);

	TPtrC8 line;
	TBool rvct = EFalse;
	_LIT8(KRvct, "ARM Linker, RVCT");
	if (GetNextLine(line) && HasPrefix(line, KRvct)) rvct = ETrue;
	TBool foundOffset = EFalse;
	TInt linenum = 0; // for debugging

	if (rvct)
		{
		TInt local = ETrue;
		while (GetNextLine(line))
			{
			linenum++;
			if (line.Length() == 0) continue;
			_LIT8(KRVCTOffsetLine, "    Image$$ER_RO$$Base                       ");
			if (!foundOffset)
				{
				if (HasPrefix(line, KRVCTOffsetLine))
					{
					TLex8 lex(line.Mid(KRVCTOffsetLine().Length()));
					iTextOffset = HexLexL(lex);
					foundOffset = ETrue;

					// No go through any symbols we found before this, and correct their offsets
					for (TInt i = 0; i < iSymbols.Count(); i++)
						{
						iSymbols[i].iAddress -= iTextOffset;
						}
					continue;
					}
				}

			_LIT8(KGlobalSection, "    Global Symbols");
			if (HasPrefix(line, KGlobalSection)) local = EFalse; // We've reached the global section, don't have to ignore so much stuff

			SSymbol symbol;

			// This is rather awkward, because there's no proper delimiting between symbol name and the next stuff
			_LIT8(KSpaces, "   ");
			if (line.Length() < 55) continue;
			TInt foundSpaces = line.Mid(55).Find(KSpaces);
			if (foundSpaces == KErrNotFound) continue;
			TInt spp = 55 + foundSpaces - 10;
			symbol.iName = line.Left(spp).AllocLC();
			symbol.iName->Des().Trim();
				
			TLex8 lex(line);
			lex.Inc(spp);
			symbol.iAddress = HexLexL(lex) - iTextOffset;
			lex.SkipSpace();
			if (local)
				{
				// In local symbols section we have to worry about non-code stuff
				_LIT8(KArm,   "ARM Code  ");
				_LIT8(KThumb, "Thumb Code");
				TPtrC8 type = lex.Remainder().Left(KThumb().Length());
				if (type != KArm && type != KThumb)
					{
					CleanupStack::PopAndDestroy(symbol.iName);
					continue;
					}
				}

			lex.Inc(10); // Far enough to get over "ARM Code" or "Thumb Code"
			lex.SkipSpace();
			User::LeaveIfError(lex.Val(symbol.iLength));
			if (symbol.iLength == 0)
				{
				CleanupStack::PopAndDestroy(symbol.iName);
				continue; // todo thunks lie about their size...
				}

			User::LeaveIfError(iSymbols.Append(symbol));
			CleanupStack::Pop(symbol.iName);
			}
		}
	else
		{
		while (GetNextLine(line))
			{
			linenum++;
			_LIT8(KGccTextOffsetLine, "Address of section .text set to ");
			if (HasPrefix(line, KGccTextOffsetLine))
				{
				// GCCE style
				TLex8 lex(line.Mid(KGccTextOffsetLine().Length()));
				iTextOffset = HexLexL(lex);
				continue;
				}

			_LIT8(KFin, ".fini");
			if (line == KFin) break;
			if (line.Length() < 16) continue;
			TLex8 lex(line);
			lex.Inc(16);
			SSymbol symbol;
			symbol.iLength = 4; // unless otherwise updated by the next symbol addr
			TInt err = HexLex(lex, symbol.iAddress);
			if (err) continue;
			symbol.iAddress -= iTextOffset;
			lex.SkipSpace();
			if (lex.Offset() < 42) continue; // Code symbols have space up to column 42
			symbol.iName = lex.Remainder().AllocLC();

			User::LeaveIfError(iSymbols.Append(symbol));
			CleanupStack::Pop(symbol.iName);
			}
		// GCCE doesn't even sort the symbols in its map file. Unbelievable! Or possibly even inconceivable!
		iSymbols.SortUnsigned();
		for (TInt i = 1; i < iSymbols.Count(); i++)
			{
			SSymbol& prevSymbol = iSymbols[i - 1];
			prevSymbol.iLength = iSymbols[i].iAddress - prevSymbol.iAddress;
			}
		}
	iFile.Close();
	iReadBuf.Close();
	}

TBool CMapFile::GetNextLine(TPtrC8& aPtr)
	{
	_LIT8(KNewline, "\r\n");
	iReadBuf.Delete(0, aPtr.Length());
	if (HasPrefix(iReadBuf, KNewline)) iReadBuf.Delete(0, KNewline().Length());

	TInt newline = iReadBuf.Find(KNewline);
	if (newline != KErrNotFound)
		{
		aPtr.Set(iReadBuf.Left(newline));
		return ETrue;
		}
	// Otherwise need to try reading some more from file
	TPtr8 restOfDesc((TUint8*)iReadBuf.Ptr() + iReadBuf.Size(), 0, iReadBuf.MaxSize() - iReadBuf.Size());
	TInt err = iFile.Read(restOfDesc);
	if (err) restOfDesc.Zero();
	iReadBuf.SetLength(iReadBuf.Length() + restOfDesc.Length());
	
	// Now re-try looking for newline
	newline = iReadBuf.Find(KNewline);
	if (newline != KErrNotFound)
		{
		aPtr.Set(iReadBuf.Left(newline + KNewline().Length()));
		return ETrue;
		}

	// No more newlines, just return whatever's left in the buffer
	aPtr.Set(iReadBuf);
	return aPtr.Length() != 0; // And return whether it's worth having
	}

EXPORT_C void CMapFile::Lookup(TUint32 aOffsetInCodeSeg, TDes& aResult)
	{
	//TUint32 offset = aOffsetInCodeSeg & 0xFFFFFFFE; // Mask bottom bit as it just indicates thumb mode
	CMapFileImpl* impl = static_cast<CMapFileImpl*>(this); // We know we're actually a CMapFileImpl
	impl->DoLookup(aOffsetInCodeSeg, aResult);
	}

void CMapFileImpl::DoLookup(TUint32 aOffsetInCodeSeg, TDes& aResult)
	{
	TInt pos = 0;
	SSymbol dummy; dummy.iAddress = aOffsetInCodeSeg;
	TBool found = iSymbols.FindInUnsignedKeyOrder(dummy, pos) == KErrNone;
	if (!found && pos != 0) pos--;

	aResult.Zero();
	const SSymbol& symbol = iSymbols[pos];
	if (aOffsetInCodeSeg >= symbol.iAddress && aOffsetInCodeSeg < symbol.iAddress + symbol.iLength)
		{
		aResult.Copy(*symbol.iName);
		aResult.AppendFormat(_L(" + 0x%x"), aOffsetInCodeSeg - symbol.iAddress);
		}
	}

EXPORT_C void CMapFile::GetFileNameL(TDes& aFileName) const
	{
	aFileName.Copy(*iFileName);
	}

RNode* CMapFile::CreateCompletionTreeL()
	{
	CMapFileImpl* impl = static_cast<CMapFileImpl*>(this); // We know we're actually a CMapFileImpl
	return impl->DoCreateCompletionTreeL();
	}

RNode* CMapFileImpl::DoCreateCompletionTreeL()
	{
	RNode* result = RNode::NewL();
	CleanupDeletePushL(result);

	RLtkBuf tempBuf;
	tempBuf.CreateLC(256);
	for (TInt i = 0; i < iSymbols.Count(); i++)
		{
		tempBuf.Zero();
		tempBuf.AppendL(*iSymbols[i].iName);
		tempBuf.ReserveExtraL(1);
		result->InsertStringL(tempBuf.PtrZ(), iSymbols[i].iAddress);
		}
	CleanupStack::PopAndDestroy(&tempBuf);
	CleanupStack::Pop(result);
	return result;
	}
