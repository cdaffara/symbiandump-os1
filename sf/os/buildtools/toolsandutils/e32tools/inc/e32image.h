// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __E32IMAGE_H__
#define __E32IMAGE_H__

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <fstream>
using namespace std;
#else //!__MSVCDOTNET__
#include <fstream.h>
#endif //__MSVCDOTNET__

#include <e32std.h>
#include <e32ldr.h>
#include <f32image.h>
#include "byte_pair.h"

enum TFileSource
	{
	EE32Image=0,
	EPeFile=1,
	EElfFile=2,
	};

class E32ImageFile
	{
private:
        int iOwnBPE;
public:
	static E32ImageFile* New();
	E32ImageFile(CBytePair *aBPE = NULL);
	virtual ~E32ImageFile();
	virtual TBool Translate(const char* aFileName, TUint aDataBase, TBool aAllowDllData, TBool aSymLkupEnabled = FALSE);
	TInt ReadHeader(ifstream& is);
	TInt Open(const char* aFileName);
#ifndef __LINUX__
	TInt Open(const wchar_t* aFileName);
#endif
	void Adjust(TInt aSize, TBool aAllowShrink=ETrue);
	TUint VaOfOrdinal(TUint aOrdinal);
	void RelocateSection(char* aPtr, char *aRelocs, TUint aCodeDelta, TUint aDataDelta, char* aImagePtr, TLinAddr** aIATRefs, TBool keepIAT=EFalse);
	//
	TUint TextOffset();
	TUint DataOffset();
	TUint BssOffset();
	TUint32 Capability();
	TUint32 Format();
	TInt NumberOfImports();

	TInt IsValid();
	TInt IsDll();
	enum TDumpFlags
		{
		EDumpHeader = 1<<0,
		EDumpSecurityInfo = 1<<1,
		EDumpCode = 1<<2,
		EDumpData = 1<<3,
		EDumpExports = 1<<4,
		EDumpImports = 1<<5,
		EDumpDefaults = EDumpHeader|EDumpCode|EDumpData|EDumpExports|EDumpImports
		};
	void Dump(TText *aFileName,TInt aDumpFlags);
	void DumpHeader(TInt aDumpFlags);
	void DumpData(TInt aDumpFlags);
	void SetStackSize(TInt aSize);
	void SetHeapSizeMin(TInt aSize);
	void SetHeapSizeMax(TInt aSize);
	void SetUids(TUid aUid1, TUid aUid2, TUid aUid3);
	void SetSecureId(TUint32 aId);
	void SetVendorId(TUint32 aId);
	void SetCallEntryPoints(TInt aBool);
	void SetFixedAddress(TInt aBool);
	void SetPriority(TProcessPriority aPri);
	void SetCapability(SCapabilitySet& aCapabilities);
	void SetFPU(unsigned int aFPU);
	static TInt IsE32ImageFile(char *aFileName);
	TInt DetermineEntryPointType();
	void UpdateHeaderCrc();
	void SetDefaultHeader();
	void CreateExportBitMap();
	void AddExportDescription();
	TInt CheckExportDescription();
	TInt Validate();
public:
	inline TUint OrigCodeOffset() const {return OffsetUnadjust(iOrigHdr->iCodeOffset);}
	inline TUint OrigDataOffset() const {return OffsetUnadjust(iOrigHdr->iDataOffset);}
	inline TUint OrigCodeRelocOffset() const {return OffsetUnadjust(iOrigHdr->iCodeRelocOffset);}
	inline TUint OrigDataRelocOffset() const {return OffsetUnadjust(iOrigHdr->iDataRelocOffset);}
	inline TUint OrigImportOffset() const {return OffsetUnadjust(iOrigHdr->iImportOffset);}
	inline TUint OrigExportDirOffset() const {return OffsetUnadjust(iOrigHdr->iExportDirOffset);}
	inline TUint OffsetUnadjust(TUint a) const {return a ? a-iOrigHdrOffsetAdj : 0;}
	inline void OffsetAdjust(TUint& a) { if (a) a+=iOrigHdrOffsetAdj; }
public:
	char* iData;
	TInt iSize;
	CBytePair *iBPE;
	E32ImageHeader* iOrigHdr;
	E32ImageHeaderV* iHdr;
	char* iFileName;
#ifndef __LINUX__
	wchar_t* iWideFileName;
#endif
	TInt iError;
	TFileSource iSource;
	TUint iOrigHdrOffsetAdj;
	TInt iFileSize;
	TUint8* iExportBitMap;
	TInt iMissingExports;
	};

#ifdef __VC32__
ostream &operator<<(ostream &os, const E32ImageFile &aImage);
#else
ostream &operator<<(ostream &os, const E32ImageFile &aImage);
#endif
ifstream &operator>>(ifstream &is, E32ImageFile &aImage);

#endif
