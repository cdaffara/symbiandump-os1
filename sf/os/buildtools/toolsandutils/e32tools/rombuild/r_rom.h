// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __R_ROM_H__
#define __R_ROM_H__

#include "e32image.h"
#include <e32rom.h>

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <fstream>
#else //!__MSVCDOTNET__
#include <fstream.h>
#endif //__MSVCDOTNET__

#include "h_utl.h"
#include "r_mromimage.h"
#include <malloc.h>


//
const TUint KRomWrapperSize = 0x100;
const TUint KRomNameSize = 16;
//
const TUint KOverrideStack = 0x01;
const TUint KOverrideHeapMin = 0x02;
const TUint KOverrideHeapMax = 0x04;
const TUint KOverrideAddress = 0x08;
const TUint KOverrideUid1 = 0x10;
const TUint KOverrideUid2 = 0x20;
const TUint KOverrideUid3 = 0x40;
const TUint KOverrideCallEntryPoint = 0x80;
const TUint KOverrideNoCallEntryPoint = 0x100;
const TUint KOverridePriority = 0x200;
const TUint KOverrideStackReserve = 0x400;
const TUint KOverrideKeepIAT = 0x800;
const TUint KOverrideCapability = 0x1000;
const TUint KOverrideCodeUnpaged = 0x2000;
const TUint KOverrideCodePaged = 0x4000;
const TUint KOverrideDataUnpaged = 0x8000;
const TUint KOverrideDataPaged = 0x10000;
const TUint KOverrideHCRData = 0x20000 ;

enum TKernelModel {ESingleKernel, EMultipleKernels};
enum TMemModel {E_MM_Moving, E_MM_Direct, E_MM_Multiple, E_MM_Flexible};

const TUint KVariantIndependent = 0x01000000;

extern TInt CompareCapabilities(const TUint32* aSubCaps, const TUint32* aSuperCaps, const char* aSubName, const char* aSuperName);

class THardwareVariant
	{
public:
	enum THardwareVariantOrder
		{
		EUnordered=0,
		ELess=1,
		EEqual=2,
		EGreater=4,
		};
public:
	THardwareVariant() {iVariant=KVariantIndependent;}
	THardwareVariant(TUint aVariant) {iVariant=aVariant;}
	operator TUint() const {return iVariant;}
	TBool MutuallyExclusive(THardwareVariant aHardwareVariant) const;
	TInt Compare(THardwareVariant aHardwareVariant) const;
	TBool operator<(THardwareVariant a) const { return ((Compare(a)&ELess)!=0); }
	TBool operator==(THardwareVariant a) const { return ((Compare(a)&EEqual)!=0); }
	TBool operator>(THardwareVariant a) const { return ((Compare(a)&EGreater)!=0); }
	TBool operator<=(THardwareVariant a) const { return ((Compare(a)&(ELess|EEqual))!=0); }
	TBool operator>=(THardwareVariant a) const { return ((Compare(a)&(EGreater|EEqual))!=0); }
	TBool IsIndependent() const {return Layer()<=3;}
	TBool IsCpu() const {return Parent()==3; }
	TBool IsVariant() const;
	TUint32 ReturnVariant() const {return iVariant;} 
	TUint Layer() const {return iVariant>>24;}
	TUint Parent() const {return (iVariant>>16)&0xff;}
	TUint VMask() const {return iVariant&0xffff;}
private:
	TUint32 iVariant;
	};

class TRomLoad
	{
public:
	TText name[KRomNameSize];
	TText versionStr[4];
	TText buildNumStr[4];
	TUint romSize;
	TUint wrapSize;
	};
//

// representation of a relocation record for areas.
class TReloc
	{
public:
	TInt iLength;
	TUint iSrc;
	TUint iDest;
	};

// thing for tracking the various addresses at which to find something

class TAddressRange
	{
public:
	TAddressRange();

	void Append(TAddressRange& aRange);
	void Extend(TInt aOffset);
	void Move(TInt aOffset);

public:
	TAny*	iImagePtr;			// data in the ROM image buffer
	TUint32	iImageAddr;			// address in the ROM image 
	TUint32 iRunAddr;			// runtime address, usually the same as the iImageAddr
	TUint   iSize;				// size of this section
	};


class TImageSection : public TAddressRange
	{
public:
	TImageSection() : TAddressRange(), iFilePtr(0) {}

	TAny* iFilePtr;		// original data in the E32Image file

	void Load() const;	// copy the file data into the image, if needed
	};

//
const TUint KFillSize = KRomWrapperSize-sizeof(TRomLoad);
class CObeyFile;
class TRomLoaderHeader
	{
public:
	void SetUp(CObeyFile *aObey);
private:
	TRomLoad r;
	TUint8 filler[KFillSize];
	};
//
class TRomBuilderEntry;
class TRomNode;
class TRomFile;
class TDllFindInfo
	{
public:
	TDllFindInfo(const char* aImportName, const TRomBuilderEntry* aEntry);
	~TDllFindInfo();
public:
	const char* iBareName;		// filename+extension to find, not including path UID or version
	TUint32 iUid3;				// requested UID3 - 0 if don't care
	TUint32 iModuleVersion;		// requested version
	TUint32 iHwVariant;			// requested hardware variant
	};

class TAutoFree
	{
public:
	TAutoFree(void* aPtr) {iPtr=aPtr;}
	~TAutoFree() {free(iPtr);}
private:
	void* iPtr;
	};

#define AUTO_FREE(p)	TAutoFree _auto_free_##p(p)

class TModuleName
	{
public:
	TModuleName(const TRomBuilderEntry* aEntry);
	TModuleName(const TDllFindInfo& aInfo);
	TModuleName(const TRomNode& aNode);
	TModuleName(const TRomFile& aFile, const TRomNode* aRootDir);
	~TModuleName();
	operator const char* () {return iName;}
private:
	char* iName;
	};

// class representing a file (or directory) in the ROM
class TRomFile
	{
public:
	TRomFile();
	~TRomFile();
	TInt AddressFromOrdinal(TLinAddr& aEDataAddr,TLinAddr& aExport, TUint aOrdinal);
	void Finalise(TInt aSize);

	void Close() { if (--iRefCount<=0) delete this; };
	void Open() {iRefCount++; };

	TInt MarkDeps();
	TInt FindRouteTo(TRomFile* aDest, TRomFile** aStack, TInt aIndex);
	TRomEntry* RomEntry() const {return iRomEntry;}
	void SetRomEntry(TRomEntry* aEntry);

	TAddressRange iAddresses;

	inline TRomImageHeader* RomImgHdr() const
		{return (TRomImageHeader*)iAddresses.iImagePtr;}

	TAddressRange iExportDir;
	TRomBuilderEntry* iRbEntry;	// only until Layout stage
	TBool iDir;
	TBool iFinal;
	THardwareVariant iHwvd;
	TRomEntry* iRomEntry;		// in ROM image buffer
	friend class FileEntry;

	TInt iDataBssOffsetInExe;	// for allocating DLL data in process data section

	// dependence graph
	TInt iNumDeps;
	TRomFile** iDeps;
	TInt iNumPDeps;
	TRomFile** iPDeps;
	TInt iMark;
	TRomFile* iAttachProcess;
	TInt iTotalDataBss;

	// Data used when computing SMP properties.
	struct TSmpInfo
		{
		TSmpInfo() : isInit(0), isActive(0), isSafe(0) {}
		bool isInit;
		bool isActive;
		bool isSafe;
		} iSmpInfo;

	bool ComputeSmpSafe(const TRomBuilderEntry*);
public:
	TUint32 Uid3() const;
	TUint32 ModuleVersion() const;
	THardwareVariant HardwareVariant() const;
	TUint32 ABI() const;
	TInt ExportDirCount() const;
	TUint32 RomImageFlags() const;
	TLinAddr DataBssLinearBase() const;
	const SSecurityInfo& SecurityInfo() const;
private:
	TInt iRefCount;
	};
//
class TRomBuilderEntry;
class RomFileStructure;

// class representing a directory entry in the ROM file system
class TRomNode
	{
public:
	TRomNode(const TText* aName, TRomBuilderEntry* aEntry=0);
	TRomNode(const TText* aName, TRomNode* aNode);
private:
	TRomNode(const TRomNode& aNode);
public:
	~TRomNode();
	void Destroy();

	TInt SetAtt(TText *anAttWord);
	TInt NameCpy(char* aDest); // returns the length of aDest (in UTF-16 characters for Unicode, not bytes)
	TInt SetBareName();

	TRomNode* NewSubDir(const TText* aName);
	void AddFile(TRomNode *aChild);
	TInt Rename(TRomNode *aOldParent, TRomNode* aNewParent, TText* aNewName);
	TRomNode* FindInDirectory(const TText* aName);
	TRomNode* FindInDirectory(const TText* aName, THardwareVariant aVariant , TBool aPatchDataFlag=FALSE);

	TRomEntry* RomEntry() const {return iRomFile->RomEntry();}
	void Finalise(TInt aSize);

	void CountDirectory(TInt& aFileCount, TInt& aDirCount);
	TInt ProcessDirectory(RomFileStructure* aRFS);

	static void AddExecutableFile(TRomNode*& aLast, TRomNode* aNode);
	TRomNode* FindImageFileByName(const TDllFindInfo& aInfo, TBool aPrintDiag, TBool& aFallBack);
	TInt CheckForVersionConflicts(const TRomBuilderEntry* a);

	TRomNode* CopyDirectory(TRomNode*& aLastExecutable, TRomNode* aParent);
	TInt Alias(TRomNode* aNode, TRomNode*& aLastExecutable);
public:
	char* BareName() const;
	TUint32 Uid3() const;
	TUint32 ModuleVersion() const;
	THardwareVariant HardwareVariant() const;
	TUint32 ABI() const;
	TInt FullNameLength(TBool aIgnoreHiddenAttrib = EFalse) const;
	TInt GetFullName(char* aBuf, TBool aIgnoreHiddenAttrib = EFalse) const;

	void Add(TRomNode* aChild);
	void Remove(TRomNode* aChild);

	static TInt Count;			// seed for unique identifiers

	TRomNode* iSibling;
	TRomNode* iChild;
	TRomNode* iParent;
	TRomNode* iNextExecutable;

protected:
	TInt iIdentifier;
	friend class FileEntry;

public:
	TRomFile* iRomFile;
	TText* iName;
	char* iBareName;		// name including extension but without UID or version
	TUint8 iAtt;
	TBool iHidden;
	};

class E32Rom;
class Area;
class TRomBuilderEntry : public E32ImageFile
	{
public:
	TRomBuilderEntry(const char *aFileName, TText *aName);
	~TRomBuilderEntry();
	TInt SetCodeAlignment(TText *aStr);
	TInt SetDataAlignment(TText *aStr);
	TInt SetRelocationAddress(TText *aStr);
	TInt SetStackReserve(TText *aStr);
	TInt SetStackSize(TText *aStr);
	TInt SetHeapSizeMin(TText *aStr);
	TInt SetHeapSizeMax(TText *aStr);
	TInt SetCapability(TText *aStr);
	TInt SetUid1(TText *aStr);
	TInt SetUid2(TText *aStr);
	TInt SetUid3(TText *aStr);
	TInt SetCallEntryPoint(TBool aState);
	TInt SetPriority(TText *aStr);
	TInt SetAttachProcess(TText *aStr);
	TInt OpenImageFile();
	TInt SizeInRom();
	void SizeInSections(TInt& aSize1, TInt& aSize2);
	void LoadToRom();
	TInt FixupImports(E32Rom& aRom);
	TInt ResolveDllRefTable(E32Rom& aRom);
	TInt BuildDependenceGraph(E32Rom& aRom);
	TInt FindAttachProcess(E32Rom& aRom);
	const char* GetDefaultAttachProcess();
	void SetRomNode(TRomNode* aNode);
	void FixupRomEntries(TInt aSize);
	TRomEntry* RomEntry() const {return iRomNode->RomEntry(); };
	void DisplaySize(TPrintType aWhere);
public:
	inline TBool Primary() const
		{return iRomImageFlags & KRomImageFlagPrimary;}
	inline TBool Variant() const
		{return iRomImageFlags & KRomImageFlagVariant;}
	inline TBool Extension() const
		{return iRomImageFlags & KRomImageFlagExtension;}
	inline TBool Device() const
		{return iRomImageFlags & KRomImageFlagDevice;}
	inline TBool Secondary() const
		{return iRomImageFlags & KRomImageFlagSecondary;}
	inline TBool HCRDataFile() const 
		{ return iOverrideFlags & KOverrideHCRData ;}
private:
	TRomBuilderEntry();
	TRomBuilderEntry(const TRomBuilderEntry&);
	const TRomBuilderEntry& operator=(const TRomBuilderEntry &);
	void Relocate();
public:
	TText *iName;
	TBool iResource;
	TBool iNonXIP;
	TBool iPreferred;
	TUint iCompression;
	TBool iPatched;
	Area* iArea;
	TInt  iRomSectionNumber;
	TUint iOverrideFlags;
	TUint32 iRelocationAddress;
	TInt iAlignment;
	TInt iCodeAlignment;
	TInt iDataAlignment;
	TInt iStackSize;
	TInt iHeapSizeMin;
	TInt iHeapSizeMax;
	TProcessPriority iPriority;
	TUint iUid1;
	TUint iUid2;
	TUint iUid3;
	TLinAddr iDataBssLinearBase;
	SSecurityInfo iS;
	char* iBareName;			// EPOC name+extension but without path or version
	TUint32 iVersionInName;
	TUint32 iVersionPresentInName;

	THardwareVariant iHardwareVariant;
	TUint iDataBssOffset;
	TInt iStackReserve;

	TRomImageHeader* iRomImageHeader;

	TAddressRange iHeaderRange;
	TImageSection iCodeSection;
	TImageSection iImportAddressTableSection;
	TImageSection iExportDirSection;
	TImageSection iDataSection;
	TAddressRange iDllRefTableRange;

	TLinAddr**  iIATRefs;	// image addresses of pointers to IAT

	TRomBuilderEntry* iNext;
	TRomBuilderEntry* iNextInArea;

	TUint32 iRomImageFlags;
	TText* iProcessName;

	// Used by the collapse routines
	TInt iImportCount;
	TUint32 iImportBlockStartAddress;
	TUint32 iImportBlockEndAddress;
	TRomNode *iRomNode;
	};
//
class ImpTRomHeader	: public TRomHeader
	{
public:
	void Display();
	void SetUp(CObeyFile *aObey);
	void CheckSum(TUint32 aTargetValue);
	};
//
class COrderedFileList
	{
public:
	static COrderedFileList* New(TInt aMaxFiles);
	~COrderedFileList();
	void Add(TRomBuilderEntry* anEntry);
	TInt Count() {return iCount;}
	TRomBuilderEntry* operator[](TInt anIndex) {return iOrderedFiles[anIndex];}
private:
	COrderedFileList(TInt aMaxFiles);
private:
	TInt iCount;
	TInt iMaxFiles;
	TRomBuilderEntry **iOrderedFiles;
	};
//
class CObeyFile;
class Area;
class Memmap;
class E32Rom : public MRomImage
	{
public:
	E32Rom(CObeyFile *aObey);
	~E32Rom();
	TInt Create();
	TInt CreateExtension(MRomImage* aKernelRom);
	TInt Align(TInt aVal);
	void DisplaySizes(TPrintType aWhere);
	TInt WriteImages(TInt aHeaderType);
	TInt Compare(char* anImage, TInt aHeaderType);
	//
	//
	char *RomToActualAddress(TUint aPtr);
	TUint ActualToRomAddress(TAny *aPtr);
	void Display(TRomImageHeader *aHdr);
	TRomNode* FindImageFileByName(const TDllFindInfo& aInfo, TBool aPrintDiag, TBool& aFallBack);
	TInt CheckForVersionConflicts(const TRomBuilderEntry* a);
	TRomNode* CopyDirectory(TRomNode*& aLastExecutable);

	TRomNode* RootDirectory();
	TText* RomFileName();
	TUint32 RomBase();
	TUint32 RomSize();
	TVersion Version();
	TInt64 Time();
	TUint32 CheckSum();
	TUint32 DataRunAddress();
	TUint32 RomAlign();

private:
	void CalculateDataAddresses();
	void CalculateDataAddress(TRomBuilderEntry *aFile);
	char *LayoutRom(char *anAddr);
	char *ReserveRomExceptionSearchTable(char *anAddr, TRomExceptionSearchTable*& exceptionSearchTable);
	void ConstructRomExceptionSearchTable(TRomExceptionSearchTable* exceptionSearchTable);
	void LayoutFile(TRomBuilderEntry*, TAddressRange& aMain, TAddressRange* aSecond, CBytePair* aBPE);
	void NextRom(TAddressRange* aFirst, TAddressRange* aSecond);
	TInt TranslateFiles();
	void EnumerateVariants();
	TUint32 AlignToChunk(TUint32 anAddr);
	TUint32 AlignToPage(TUint32 anAddr);
	TUint32 AllocVirtual(TUint32 aAddr,TUint aSize);
	TInt LoadDataToRom(TRomBuilderEntry *aFile, TAddressRange& aAddress, CBytePair* aBPE);
	void LoadFileToRom(TRomBuilderEntry *aFile);

	void Write(ofstream &of, TInt aHeaderType);		// main ROM image
	void WriteOdd(ofstream &of);					// odd halfwords
	void WriteEven(ofstream &of);					// even halfwords
	void WriteSRecord(ofstream &of);				// SREC or S19 format

	TInt LoadContents(char*& anAddr, TRomHeader* aHeader);
	TInt BuildDependenceGraph();
	TInt ResolveDllRefTables();
	TInt ResolveImports();
	TInt CreateExportDirectory();
	char *WriteDirectory(char *aAddr, TRomHeader* aHeader);
	TInt WriteHeadersToRom(char *anAddr);
	TInt RequiredSize();
	void SetCompressionInfo(TUint aCompressionType, TUint aCompressedSize, TUint aUncompressedSize);
	void FinaliseSectionHeader();
	void FinaliseExtensionHeader(MRomImage* aKernelRom);
	void DisplayExtensionHeader();
	void SetImageAddr(TAddressRange& aSet, TAny* aPtr, TUint32 aRunOffset=0);
	void SetImageAddr(TAddressRange& aSet, TUint aAddr, TUint32 aRunOffset=0);
	TInt CollapseImportThunks();
	TInt CollapseImportThunks(TRomBuilderEntry* aFile);
	TInt CollapseBranches();
	TInt CollapseBranches(TRomBuilderEntry* aFile);
	TUint32 FindFinalJumpDestination(TUint32 ja);

	char* AllocateRelocationTable(char* aAddr, TReloc*& aRelocTable);
	void FillInRelocationTable(TReloc* aRelocTable);

	void LinkKernelExtensions(TRomBuilderEntry* aExtArray[], TInt aExtCount);
	void FillInCrcs();
	void ProcessDllData();

	void CreatePageIndex(char*& aAddr);
	TInt CompressPages();
	TInt CompressPage(SRomPageInfo& aPageInfo, TInt aOutOffset, CBytePair * aBPE);

	// dependence graph
	void UnmarkGraph(TInt aMark=0xffffffff);
	void FindMarked(TInt aMarkMask, TInt aMark, TRomFile**& aList);
	TInt ProcessDependencies();
	void ListRouteTo(TRomFile* aStart, TRomFile* aDest, TInt aNDeps);

	void SetSmpFlags();
	
	// Check if the unpaged memory size overflows. For page enabled ROM only.
	TInt CheckUnpagedMemSize();
	
	// Initialize the page info prior to compression
	TInt SetupPages();

	Memmap *iImageMap;
public:
	char* iData;
	TInt iSize;
	TRomLoaderHeader *iLoaderHeader;
	TRomHeader *iHeader;
	TExtensionRomHeader *iExtensionRomHeader;		// NULL if not extension ROM
	//
	CObeyFile *iObey;

	char *iSectionPtr;
	
	//
	TInt iSizeUsed;
	TInt iUncompressedSize;
	TInt iOverhead;
	TInt iDirectorySize;
	TInt iImportsFixedUp;
	TInt iBranchesFixedUp;
	TInt iVtableEntriesFixedUp;
	//
	TUint32 iNextDataChunkBase;
	TUint32 iTotalSvDataSize;
	TUint32 iNextDllDataAddr;
	char* iPrevPrimaryAddress;
	char* iPrevVariantAddress;
	COrderedFileList** iVariantFileLists;
	TRomBuilderEntry** iPeFiles;
	};


#endif
