// bsym.h
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
#ifndef FSHELL_BSYM_H
#define FSHELL_BSYM_H

#include <e32base.h>
#include <f32file.h>
#include <fshell/ioutils.h>
#include <fshell/stringhash.h>
class CDesC16Array;

namespace LtkUtils
	{
	class CMapFile;
	class CBsymFile;
	class RNode;

	NONSHARABLE_CLASS(CSymbolics) : public CBase
		{
	public:
		IMPORT_C CSymbolics(RFs& aFs);
		IMPORT_C ~CSymbolics();
		IMPORT_C void AddBsymFileL(const TDesC& aFileName);
		IMPORT_C void AddMapFileL(const TDesC& aFileName);
		IMPORT_C void AddMapFileL(CMapFile* aMapFile); // Takes ownership
		IMPORT_C void SetFallbackMapFileDirL(const TDesC& aDir);

		IMPORT_C TPtrC LookupL(TUint32 aRomAddress);
		IMPORT_C TPtrC LookupL(const TDesC& aCodeseg, TUint32 aOffset); // Need to use memaccess or similar to get codeseg name

		IMPORT_C void CompleteL(const TDesC& aCodeseg, TDes& aSymbolName, CDesC16Array& aSuggestions);
		IMPORT_C TUint32 CodesegOffsetFromSymbolNameL(const TDesC& aCodeseg, const TDesC& aSymbolName);

	private:
		CMapFile* FindOrLoadMapFileL(const TDesC& aCodeseg);
		RNode* TreeForCodesegL(const TDesC& aCodeseg);

	private:
		RFs& iFs;
		RStringHash<CMapFile*> iCodeSegHash;
		RPointerArray<CBsymFile> iBsyms;
		TFileName iFallbackMapFileDir;
		TBuf<256> iTempString;
		RNode* iTabCompleteTree;
		HBufC* iTabCompleteCodeseg;
		};

	NONSHARABLE_CLASS(CBsymFile) : public CBase
		{
	public:
		IMPORT_C static CBsymFile* NewL(RFs& aFs, const TDesC& aFileName);
		IMPORT_C ~CBsymFile();
		IMPORT_C TPtrC LookupL(TUint32 aRomAddress);
		IMPORT_C TPtrC LookupL(const TDesC& aCodesegName, TInt aOffset);

		RNode* CreateCompletionTreeL(const TDesC& aCodesegName);

	protected:
		CBsymFile();

	protected:
		RFile iFile;
		TUint iFileSize;
		TUint32 iVersion;
		TUint32 iCodesegOffset;
		TUint32 iSymbolsOffset;
		TUint32 iTokensOffset;
		};

	NONSHARABLE_CLASS(CMapFile) : public CBase
		{
	public:
		IMPORT_C static CMapFile* NewL(RFs& aFs, const TDesC& aFileName);
		IMPORT_C ~CMapFile();
		IMPORT_C void Lookup(TUint32 aOffsetInCodeSeg, TDes& aResult);
		IMPORT_C void GetFileNameL(TDes& aFileName) const;
		RNode* CreateCompletionTreeL();

	protected:
		CMapFile();
		void ConstructL(RFs& aFs, const TDesC& aFileName);
		TBool GetNextLine(TPtrC8& aPtr);

	protected:
		RFile iFile;
		RBuf8 iReadBuf;
		TInt iTextOffset;
		HBufC* iFileName;
		};
	}
#endif
