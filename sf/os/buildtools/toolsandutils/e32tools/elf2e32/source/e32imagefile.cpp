// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of e32 image creation and dump for elf2e32 tool
// @internalComponent
// @released
// 
//

#include "pl_elfexecutable.h"

// get E32ImageHeader class...
#define INCLUDE_E32IMAGEHEADER_IMPLEMENTATION
#define RETURN_FAILURE(_r) return (fprintf(stderr, "line %d\n", __LINE__),_r)
//#define E32IMAGEHEADER_TRACE(_t) printf _t
#include "e32imagefile.h"

#include "pl_elfimportrelocation.h"
#include "pl_elflocalrelocation.h"
#include "pl_elfimports.h"
#include "elffilesupplied.h"
#include "pl_dllsymbol.h"
#include "h_ver.h"
#include "checksum.h"
#include "errorhandler.h"

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#ifndef __LINUX__ 
    #include <io.h>
#else
    #include <time.h>
#endif 
#include <time.h>
#include <stdio.h>

using namespace std;

template <class T> inline T Align(T v, size_t s)
{
	unsigned int inc = s-1;
	unsigned int mask = ~inc;
	unsigned int val = (unsigned int)v;
	unsigned int res = (val+inc) & mask;
	return (T)res;
}

// Need a default constructor for TVersion, but don't want all the other stuff in h_utl.cpp
/**
Default constructor for TVersion class.
@internalComponent
@released
*/
TVersion::TVersion(){}

/**
Constructor for TVersion class.
@internalComponent
@released
*/
TVersion::TVersion(TInt aMajor, TInt aMinor, TInt aBuild): 
	iMajor((TInt8)aMajor), iMinor((TInt8)aMinor), iBuild((TInt16)aBuild)
{
}


/**
Constructor for E32ImageChunkDesc class.
@internalComponent
@released
*/
E32ImageChunkDesc::E32ImageChunkDesc(char * aData, size_t aSize, size_t aOffset, char * aDoc):
	iData(aData), iSize(aSize), iOffset(aOffset), iDoc(aDoc)
{
}

/**
Destructor for E32ImageChunkDesc class.
@internalComponent
@released
*/
E32ImageChunkDesc::~E32ImageChunkDesc()
{
}

/**
This function writes its data in the buffer.
@param aPlace - a location in the buffer
@internalComponent
@released
*/
void E32ImageChunkDesc::Write(char * aPlace)
{
	memcpy(aPlace+iOffset, iData, iSize);
}

/**
Constructor for E32ImageChunks class.
@internalComponent
@released
*/
E32ImageChunks::E32ImageChunks(): 
	iOffset(0)
{
}

/**
Destructor for E32ImageChunks class.
@internalComponent
@released
*/
E32ImageChunks::~E32ImageChunks()
{	
 	if(iChunks.size())
 	{
 		ChunkList::iterator aItr = iChunks.begin();
 		ChunkList::iterator last = iChunks.end();
 		E32ImageChunkDesc *temp;
 
 		while( aItr != last)
 		{
 			temp = *aItr;
 			aItr++;
 			delete temp;
 		}
 	}
}

/**
This function creates and adds a chunk into a list.
@param aData - input buffer
@param asize - size of the input buffer
@param aOffset - byte offset of this chunk from the 1 byte in e32 image file.
@param aDoc - name of the chunk
@internalComponent
@released
*/
void E32ImageChunks::AddChunk(char * aData, size_t aSize, size_t aOffset, char * aDoc)
{
	E32ImageChunkDesc * aChunk = new E32ImageChunkDesc(aData, aSize, aOffset, aDoc);
	iChunks.push_back(aChunk);
	iOffset += Align(aSize, sizeof(TUint32));
}

/**
This function returns the list of chunks.
@internalComponent
@released
*/
E32ImageChunks::ChunkList & E32ImageChunks::GetChunks()
{
	return iChunks;
}

/**
This function returns the current offset pointing to the last chunk that
was added into the list of chunks.
@internalComponent
@released
*/
size_t E32ImageChunks::GetOffset()
{
	return iOffset;
}

/**
This function sets the current offset pointing to the last chunk that 
was added into the list of chunks.
@internalComponent
@released
*/
void E32ImageChunks::SetOffset(size_t aOffset)
{
	iOffset = aOffset;
}

/**
Constructor for E32ImageFile class.
@internalComponent
@released
*/
E32ImageFile::E32ImageFile(const char * aFileName, ElfExecutable * aExecutable, ElfFileSupplied *aUseCase) :
	iFileName(aFileName), 
	iE32Image(0),
	iExportBitMap(0),
	iElfExecutable(aExecutable), 
	iData(0),
	iUseCase(aUseCase),
	iHdr(0),
	iHdrSize(sizeof(E32ImageHeaderV)),
	iImportSection(0), 
	iImportSectionSize(0),
	iCodeRelocs(0), 
	iCodeRelocsSize(0),
	iDataRelocs(0), 
	iDataRelocsSize(0),
	iExportOffset(0),
	iLayoutDone(false), 
	iMissingExports(0),
	iSymNameOffset(0)
{
}

/**
This function generates the E32 image.
@internalComponent
@released
*/
void E32ImageFile::GenerateE32Image()
{
	if( iUseCase->GetNamedSymLookup() ){
		ProcessSymbolInfo();
	}
	ProcessImports();
	ProcessRelocations();
	ConstructImage();
}

/**
This function processes the import map by looking into the dso files
from which the symbols are imported. It also fetches the ordinal numbers
for the corresponding symbols.
@internalComponent
@released
*/
void E32ImageFile::ProcessImports()
{
	string aStrTab;
	vector<int> aStrTabOffsets;
	int aNumDlls = 0;
	int aNumImports = 0;
	bool aNamedLookup = iUseCase->GetNamedSymLookup();
	TUint aImportTabEntryPos = 0;

	ElfImports::ImportMap aImportMap = iElfExecutable->GetImports();
	ElfImports::ImportMap::iterator p;

	// First set up the string table and record offsets into string table of each 
	// LinkAs name.
	for (p = aImportMap.begin(); p != aImportMap.end(); p++)
	{
		ElfImports::RelocationList & aImports = (*p).second;
		char* aLinkAs = aImports[0]->iVerRecord->iLinkAs;

		aStrTabOffsets.push_back(aStrTab.size()); // 
		string s = aLinkAs;
		aStrTab.insert(aStrTab.end(),s.begin(),s.end());
		aStrTab.insert(aStrTab.end(),0);
		aNumDlls++;
		aNumImports += aImports.size();
	}
	
	iNumDlls = aNumDlls;
	iNumImports = aNumImports;

	// Now we can figure out the size of everything
	size_t aImportSectionSize = sizeof(E32ImportSection) + 
								(sizeof(E32ImportBlock) * aNumDlls) +
								(sizeof(unsigned int) * aNumImports);
 
	vector<Elf32_Word> aImportSection;

	// This is the 'E32ImportSection' header - fill with 0 for the moment
	aImportSection.push_back(0);

	if( aNamedLookup ) {
		// These are the 0th ordinals imported into the import table, one
		// entry for each DLL.
		aImportSectionSize += (sizeof(unsigned int) * aNumDlls);
	}
	// Now fill in the E32ImportBlocks
	int idx = 0;
	char * aDsoName;
	for (p = aImportMap.begin(); p != aImportMap.end(); p++, idx++)
	{
		ElfImports::RelocationList & aImports = (*p).second;
		aDsoName = aImports[0]->iVerRecord->iSOName;

		//const char * aDSO = FindDSO((*p).first);
		const char * aDSO = FindDSO(aDsoName);

		aImportSection.push_back(aStrTabOffsets[idx] + aImportSectionSize); 
		int nImports = aImports.size();

		// Take the additional 0th ordinal import into account
		if( aNamedLookup )
			nImports++;
		aImportSection.push_back(nImports);

		size_t aSize;
		Elf32_Ehdr * aElfFile = 0;
		ReadInputELFFile(aDSO, aSize, aElfFile);

		ElfExecutable aElfExecutable(NULL);//(aElfFile, aSize);
		aElfExecutable.ProcessElfFile(aElfFile);

		ElfImports::RelocationList::iterator q;
		for (q = aImports.begin(); q != aImports.end(); q++)
		{
			ElfImportRelocation * aReloc = *q;
			char * aSymName = iElfExecutable->GetSymbolName(aReloc->iSymNdx);
			unsigned int aOrdinal = aElfExecutable.GetSymbolOrdinal(aSymName);
			
			//check the reloc refers to Code Segment
			try
			{
				if (iElfExecutable->SegmentType(aReloc->iAddr) != ESegmentRO)
				{
					throw ImportRelocationError(ILLEGALEXPORTFROMDATASEGMENT, aSymName, iElfExecutable->iParameterListInterface->ElfInput());
				}
			}
			/**This catch block introduced here is to avoid deleting partially constructed object(s).
			Otherwise global catch block will delete the partially constructed object(s) and the tool will crash.
			*/
			catch(ErrorHandler& aError)
			{
				aError.Report();
				exit(EXIT_FAILURE);
			}
				
			Elf32_Word aRelocOffset = iElfExecutable->GetRelocationOffset(aReloc);
			aImportSection.push_back(aRelocOffset);

			Elf32_Word * aRelocPlace = iElfExecutable->GetRelocationPlace(aReloc);
			if (aOrdinal > 0xFFFF)
			{
			}
			if (aReloc->iAddend > 0xFFFF)
			{
			}
			* aRelocPlace = (aReloc->iAddend<<16) | aOrdinal;
		}

		if( aNamedLookup ) {
			aImportTabEntryPos = aImportSection.size();
			// Keep track of the location of the entry
			iImportTabLocations.push_back(aImportTabEntryPos);
			// Put the entry as 0 now, which shall be updated 
			aImportSection.push_back(0);
		}

		delete [] ((char*)aElfFile);
	}

	assert(aImportSectionSize == aImportSection.size() * sizeof(Elf32_Word));

	size_t aTotalSize = Align(aImportSectionSize + aStrTab.size(), sizeof(Elf32_Word));

	// Fill in the section header now we have the correct value.
	aImportSection[0] = aTotalSize;

	// Now construct the unified section
	iImportSectionSize = aTotalSize;
	iImportSection = (uint32 *)new char[aTotalSize];
	memset(iImportSection, 0, aTotalSize);
	memcpy(iImportSection, aImportSection.begin(), aImportSectionSize);
	char * strTab = ((char *)iImportSection) + aImportSectionSize;
	memcpy(strTab, aStrTab.data(), aStrTab.size());

}


/**
This function checks if a DSO file exists.
@param aPath - DSO file name.
@internalComponent
@released
*/
static bool ProbePath(string & aPath)
{
	ifstream aInput;
	const char * p = aPath.c_str();
	aInput.open(p);
	if (aInput.is_open())
	{
		aInput.close();
		return true;
	}
	else
	{
		return false;
	}
}

/**
This function allocates space for a DSO file name.
@param aPath - DSO file name
@internalComponent
@released
*/
const char * AllocatePath(string & aPath)
{
	const char * p = aPath.c_str();
	size_t len = aPath.size();
	char * result = new char[len+1];
	strcpy(result, p);
	return (const char *)result;
}

/**
This function searches for a DSO in the libpath specified.
@param aName - DSO file name
@internalComponent
@released
*/
const char * E32ImageFile::FindDSO(const char * aName)
{
	string aDSOName(aName);
	string aDSOPath(aName);

	const char *aNewDsoName;

	if (ProbePath(aDSOName))
	{
		aNewDsoName = AllocatePath(aDSOName);
		cleanupStack.push_back((char*)aNewDsoName);
		return aNewDsoName;
	}

	ParameterListInterface::LibSearchPaths & paths = iUseCase->GetLibSearchPaths();
	ParameterListInterface::LibSearchPaths::iterator p = paths.begin();
	for (; p != paths.end(); p++)
	{
		string path(*p);
		char dir = iUseCase->GetDirectorySeparator();
		aDSOPath.erase();
		aDSOPath.insert(aDSOPath.end(), path.begin(), path.end());
		aDSOPath.insert(aDSOPath.end(), dir);
		aDSOPath.insert(aDSOPath.end(), aDSOName.begin(), aDSOName.end());
		if (ProbePath(aDSOPath))
		{
			aNewDsoName = AllocatePath(aDSOPath);
			cleanupStack.push_back((char*)aNewDsoName);
			return aNewDsoName;
		}
	}
	throw ELFFileError(DSONOTFOUNDERROR,(char*)aName);
}

void E32ImageFile::ReadInputELFFile(const char * aName, size_t & aFileSize, Elf32_Ehdr * & aELFFile )
{
	ifstream aInput;
	aInput.open(aName, ifstream::binary|ifstream::in);
	if (aInput.is_open()) 
	{
		aInput.seekg(0,ios::end);
		aFileSize = (unsigned int) aInput.tellg();
		aInput.seekg(0,ios::beg);
		aELFFile = (Elf32_Ehdr *)new char [aFileSize];
		aInput.read((char *)aELFFile, aFileSize);
		aInput.close();
	} 
	else
	{
		throw FileError(FILEOPENERROR,(char*)aName);
	}
}

/**
This function processes relocations.
@internalComponent
@released
*/
void E32ImageFile::ProcessRelocations()
{
	ProcessCodeRelocations();
	ProcessDataRelocations();
}

/**
This function processes Code relocations.
@internalComponent
@released
*/
void E32ImageFile::ProcessCodeRelocations()
{
	CreateRelocations(iElfExecutable->GetCodeRelocations(), iCodeRelocs, iCodeRelocsSize);
}

/**
This function processes Data relocations.
@internalComponent
@released
*/
void E32ImageFile::ProcessDataRelocations()
{
	CreateRelocations(iElfExecutable->GetDataRelocations(), iDataRelocs, iDataRelocsSize);
}

/**
This function creates Code and Data relocations from the corresponding
ELF form to E32 form.
@internalComponent
@released
*/
void E32ImageFile::CreateRelocations(ElfRelocations::RelocationList & aRelocList, char * & aRelocs, size_t & aRelocsSize)
{
	size_t rsize = RelocationsSize(aRelocList);
	if (rsize)
	{
		aRelocsSize = Align(rsize + sizeof(E32RelocSection), sizeof(uint32));

		uint32 aBase = (*aRelocList.begin())->iSegment->p_vaddr;
		//add for cleanup to be done later..
		cleanupStack.push_back(aRelocs);
		aRelocs =	new char [aRelocsSize];
		memset(aRelocs, 0, aRelocsSize);
		E32RelocSection * aRelocSection = (E32RelocSection * )aRelocs;

		uint16 * data = (uint16 *)(aRelocSection + 1);
		E32RelocPageDesc * startofblock = (E32RelocPageDesc *)data;

		int page = -1;
		int pagesize = sizeof(E32RelocPageDesc);
		ElfRelocations::RelocationList::iterator r;
		for (r = aRelocList.begin(); r != aRelocList.end(); r++)
		{
			ElfLocalRelocation * aReloc = *r;
			int p = aReloc->iAddr & 0xfffff000;
			if (page != p)
			{
				if (pagesize%4 != 0)
				{
					*data++ = 0;
					pagesize += sizeof(uint16);
				}
				if (page == -1) page = p;
				startofblock->aOffset = page - aBase;
				startofblock->aSize = pagesize;
				pagesize = sizeof(E32RelocPageDesc);
				page = p;
				startofblock = (E32RelocPageDesc *)data;
				data = (uint16 *)(startofblock + 1);
			}
			uint16 relocType = aReloc->Fixup();
			*data++ = (uint16)((aReloc->iAddr & 0xfff) | relocType);
			pagesize += sizeof(uint16);
		}
		if (pagesize%4 != 0)
		{
			*data++ = 0;
			pagesize += sizeof(uint16);
		}
		startofblock->aOffset = page - aBase;
		startofblock->aSize = pagesize;
		((E32RelocSection *)aRelocs)->iNumberOfRelocs = aRelocList.size();
		((E32RelocSection *)aRelocs)->iSize = rsize;

	}
}

/**
This function calculates the relocation taking into consideration
the page boundaries if they are crossed. The relocations are
sorted.
@param aRelocList - relocations found in the Elf file.
@internalComponent
@released
*/
size_t E32ImageFile::RelocationsSize(ElfRelocations::RelocationList & aRelocList)
{
	size_t bytecount = 0;
	int page = -1;
	ElfRelocations::RelocationList::iterator r;
	for (r = aRelocList.begin(); r != aRelocList.end(); r++)
	{
		ElfLocalRelocation * aReloc = *r;
		int p = aReloc->iAddr & 0xfffff000;
		if (page != p)
			{
			if (bytecount%4 != 0)
				bytecount += sizeof(uint16);
			bytecount += sizeof(E32RelocPageDesc); // page, block size
			page = p;
			}
		bytecount += sizeof(uint16);
		}
	if (bytecount%4 != 0)
		bytecount += sizeof(uint16);
	return bytecount;
}

/**
This function returns the E32 interpretation for an Elf relocation type.
@param aReloc - relocation entry.
@internalComponent
@released
*/
E32ImageFile::uint16 E32ImageFile::GetE32RelocType(ElfRelocation * aReloc)
{
	ESegmentType aSegType = aReloc->iSegmentType; // iElfExecutable->SegmentType(aReloc->iSymbol->st_value);
	switch (aSegType)
	{
	case ESegmentRO:
		return KTextRelocType;
	case ESegmentRW:
		return KDataRelocType;
	default:
		break;
	};

	// maybe this should be an error
	return KInferredRelocType;
}

/**
This function constructs the E32 image.
@internalComponent
@released
*/
void E32ImageFile::ConstructImage()
{
	InitE32ImageHeader();
	ComputeE32ImageLayout();
	FinalizeE32Image();
	AllocateE32Image();
}

/**
This function calculates the timestamp.
@param aTime
@internalComponent
@released
*/
Int64 timeToInt64(TInt aTime)
{
	aTime-=(30*365*24*60*60+7*24*60*60);	// seconds since midnight Jan 1st, 2000
	Int64 daysTo2000AD=730497;
	Int64 t=daysTo2000AD*24*3600+aTime;	// seconds since 0000
	t=t+3600;								// BST (?)
	return t*1000000;						// milliseconds
}

/**
This function returns the E32 image header size.
@internalComponent
@released
*/
size_t E32ImageFile::GetE32ImageHeaderSize()
{
	return sizeof(E32ImageHeaderV);
}

/**
This function returns the extended E32 image header size.
@internalComponent
@released
*/
size_t E32ImageFile::GetExtendedE32ImageHeaderSize()
{
	return iHdrSize;
}

/**
This function sets the extended E32 image header size.
@internalComponent
@released
*/
void E32ImageFile::SetExtendedE32ImageHeaderSize(size_t aSize)
{
	iHdrSize = aSize;
}

/**
This function initialises the E32 image header fields.
@internalComponent
@released
*/
void E32ImageFile::InitE32ImageHeader()
{
	iHdr = iUseCase->AllocateE32ImageHeader();
	
	iHdr->iUid1 = 0;
	iHdr->iUid2 = 0;
	iHdr->iUid3 = 0;
	iHdr->iHeaderCrc = 0;
	iHdr->iSignature = 0x434f5045u;
	iHdr->iModuleVersion = 0x00010000u;
	iHdr->iCompressionType = 0;
	iHdr->iToolsVersion = TVersion(MajorVersion, MinorVersion, Build);
	Int64 ltime(timeToInt64(time(0)));
	iHdr->iTimeLo=(uint32)ltime;
	iHdr->iTimeHi=(uint32)(ltime>>32);
	iHdr->iFlags = KImageHdrFmt_V;
	// Confusingly, CodeSize means everything except writable data
	iHdr->iCodeSize = 0;
	iHdr->iDataSize = iElfExecutable->GetRWSize();
	iHdr->iHeapSizeMin = 0;
	iHdr->iHeapSizeMax = 0;
	iHdr->iStackSize = 0;
	iHdr->iBssSize = iElfExecutable->GetBssSize();
	iHdr->iEntryPoint = 0;
	iHdr->iCodeBase = iElfExecutable->GetROBase();
	iHdr->iDataBase = iElfExecutable->GetRWBase();
	iHdr->iDllRefTableCount = iNumDlls;
	iHdr->iExportDirOffset = 0;
	iHdr->iExportDirCount = iUseCase->GetNumExports();
	iHdr->iTextSize = iElfExecutable->GetROSize();
	iHdr->iCodeOffset = 0;
	iHdr->iDataOffset = 0;
	iHdr->iImportOffset = 0;
	iHdr->iCodeRelocOffset = 0;
	iHdr->iDataRelocOffset = 0;
	iHdr->iProcessPriority = (uint16)EPriorityForeground;
	iHdr->iUncompressedSize = 0;
	iHdr->iS.iSecureId = 0;
	iHdr->iS.iVendorId = 0;
	iHdr->iExceptionDescriptor = 0;
	iHdr->iSpare2 = 0;

	iHdr->iExportDescSize = iUseCase->GetExportDescSize();
	iHdr->iExportDescType = iUseCase->GetExportDescType();
	if (iHdr->iExportDescSize == 0) iHdr->iExportDesc[0] = 0;

}

/**
This function creates the E32 image layout.
@internalComponent
@released
*/
void E32ImageFile::ComputeE32ImageLayout()
{
	// E32Image header
	iChunks.AddChunk((char *)iHdr, Align(GetExtendedE32ImageHeaderSize(), sizeof(uint32)), 0, "Image Header");

	uint32 endOfHeader = iChunks.GetOffset();
	
		// Code section
	iHdr->iCodeOffset = iChunks.GetOffset();
	iChunks.AddChunk(iElfExecutable->GetRawROSegment(), iElfExecutable->GetROSize(), iHdr->iCodeOffset, "Code Section"); 
	
	// Exports Next - then we can set up CodeSize
	// Call out to the use case so it can decide how we do this
	// record exporttable offset for default case

	bool aSymLkupEnabled = iUseCase->GetNamedSymLookup();
	// The export table is required either when:
	//	a. there are exported symbols
	//	b. symbol lookup is enabled - because this table also indicates the dependencies
	bool aExportTableNeeded = (iHdr->iExportDirCount || aSymLkupEnabled) ? 1 : 0;

	iExportOffset = iChunks.GetOffset() + 4;
	iHdr->iExportDirOffset = aExportTableNeeded ? iUseCase->GetExportOffset() : 0;
	if ( aExportTableNeeded && iUseCase->AllocateExportTableP())
		iChunks.AddChunk(iUseCase->GetExportTable(), iUseCase->GetExportTableSize(), iChunks.GetOffset(), "Export Table");

	// Symbol info next
	if( aSymLkupEnabled ){
		E32EpocExpSymInfoHdr* aSymHdrInfo = (E32EpocExpSymInfoHdr*)CreateSymbolInfo(iChunks.GetOffset());
		if( aSymHdrInfo )
			iChunks.AddChunk( (char*)aSymHdrInfo, aSymHdrInfo->iSize, iChunks.GetOffset(), "Symbol Info" );
	}

	// CodeSize is current offset - endof header offset
	iHdr->iTextSize = iHdr->iCodeSize = iChunks.GetOffset() - endOfHeader;

	// Data section
	if (iElfExecutable->GetRWSize())
	{
		iHdr->iDataOffset = iChunks.GetOffset();
		iChunks.AddChunk(iElfExecutable->GetRawRWSegment(), iElfExecutable->GetRWSize(), iHdr->iDataOffset, "Data Section");
	}
	
	// Import Section
	if (iImportSectionSize)
	{
		iHdr->iImportOffset = iChunks.GetOffset();
		iChunks.AddChunk((char *)iImportSection, iImportSectionSize, iHdr->iImportOffset, "Import Section");
	}

	// Code relocs
	if (iCodeRelocsSize)
	{
		iHdr->iCodeRelocOffset = iChunks.GetOffset();
		iChunks.AddChunk((char *)iCodeRelocs, iCodeRelocsSize, iHdr->iCodeRelocOffset, "Code Relocs");
	}

	// Data relocs
	if (iDataRelocsSize)
	{
		iHdr->iDataRelocOffset = iChunks.GetOffset();
		iChunks.AddChunk((char *)iDataRelocs, iDataRelocsSize, iHdr->iDataRelocOffset, "Data Relocs");
	}

	iLayoutDone = true;
}

/**
This function returns the byte offset in the E32 image from where the
export table starts.
@internalComponent
@released
*/
size_t E32ImageFile::GetExportOffset()
{
	return iExportOffset;
}

/**
This function returns E32 image size.
@internalComponent
@released
*/
size_t E32ImageFile::GetE32ImageSize()
{
	assert(iLayoutDone);
	return iChunks.GetOffset();

}

/**
This function creates the export bitmap also replacing the absent symbols 
with the entry point functions.
@internalComponent
@released
*/
void E32ImageFile::CreateExportBitMap()
{
	int nexp = iUseCase->GetNumExports();
	size_t memsz = (nexp + 7) >> 3;
	iExportBitMap = new uint8[memsz];
	memset(iExportBitMap, 0xff, memsz);
	// skip header
	uint32 * exports = ((uint32 *)iUseCase->GetExportTable()) + 1;
	uint32 absentVal = EntryPointOffset() + iElfExecutable->GetROBase();
	iMissingExports = 0;
	for (int i=0; i<nexp; ++i)
	{
		if (exports[i] == absentVal)
		{
			iExportBitMap[i>>3] &= ~(1u << (i & 7));
			++iMissingExports;
		}
	}
}

/**
This function creates export desription for the absent symbols.
@internalComponent
@released
*/
void E32ImageFile::AddExportDescription()
{
	CreateExportBitMap();
	if (iMissingExports == 0)
		return;	// nothing to do

	int nexp = iUseCase->GetNumExports();
	size_t memsz = (nexp + 7) >> 3;	// size of complete bitmap
	size_t mbs = (memsz + 7) >> 3;	// size of meta-bitmap
	size_t nbytes = 0;
	unsigned int i;
	for (i=0; i<memsz; ++i)
		if (iExportBitMap[i] != 0xff)
			++nbytes;				// number of groups of 8
	uint8 edt = KImageHdr_ExpD_FullBitmap;
	uint32 extra_space = memsz - 1;
	if (mbs + nbytes < memsz)
	{
		edt = KImageHdr_ExpD_SparseBitmap8;
		extra_space = mbs + nbytes - 1;
	}
	extra_space = (extra_space + sizeof(uint32) - 1) &~ (sizeof(uint32) - 1);

	iHdr->iExportDescType = edt;
	if (edt == KImageHdr_ExpD_FullBitmap)
	{
		iHdr->iExportDescSize = (uint16)memsz;
		iHdr->iExportDesc[0] = iExportBitMap[0];
		uint8 * aDesc = new uint8[extra_space];
		memset(aDesc, 0, extra_space);
		memcpy(aDesc, &iExportBitMap[1], memsz-1);
		iChunks.AddChunk((char *)aDesc,extra_space, iChunks.GetOffset(), "Export Description");
	}
	else
	{
		iHdr->iExportDescSize = (uint16)(mbs + nbytes);
		uint8 * aBuf = new uint8[extra_space + 1];
		memset(aBuf , 0, extra_space + 1);
		TUint8* mptr = aBuf;
		TUint8* gptr = mptr + mbs;
		for (i=0; i<memsz; ++i)
		{
			if (iExportBitMap[i] != 0xff)
			{
				mptr[i>>3] |= (1u << (i&7));
				*gptr++ = iExportBitMap[i];
			}
		}
		iHdr->iExportDesc[0] = aBuf[0];
		uint8 * aDesc = new uint8[extra_space];
		memcpy(aDesc, aBuf+1, extra_space);
		delete[] aBuf;
		iChunks.AddChunk((char *)aDesc,extra_space, iChunks.GetOffset(), "Export Description");
	}
}

/**
This function sets the fields of the E32 image.
@internalComponent
@released
*/
void E32ImageFile::FinalizeE32Image()
{
	// Arrange a header for this E32 Image
	iHdr->iCpuIdentifier = GetCpuIdentifier();
	// Import format is ELF-derived
	iHdr->iFlags |= KImageImpFmt_ELF;
	// ABI is ARM EABI
	iHdr->iFlags |= KImageABI_EABI;
	iHdr->iFlags |= KImageEpt_Eka2;

	bool isDllp = iUseCase->ImageIsDll();
	if (isDllp)
	{
		iHdr->iFlags |= KImageDll;
		if (iHdr->iDataSize && !iUseCase->AllowDllData())
			throw ELFFileError(DLLHASINITIALISEDDATAERROR, (char*)iUseCase->InputElfFileName());

		if (iHdr->iBssSize  && !iUseCase->AllowDllData())
			throw ELFFileError(DLLHASUNINITIALISEDDATAERROR, (char*)iUseCase->InputElfFileName());

	}

	iHdr->iHeapSizeMin = iUseCase->HeapCommittedSize();
	iHdr->iHeapSizeMax = iUseCase->HeapReservedSize();
	iHdr->iStackSize = iUseCase->StackCommittedSize();


	iHdr->iEntryPoint = EntryPointOffset();

	EEntryPointStatus r = ValidateEntryPoint();
	if (r == EEntryPointCorrupt)
		throw ELFFileError(ENTRYPOINTCORRUPTERROR, (char*)iUseCase->InputElfFileName());
	else if (r == EEntryPointNotSupported)
		throw ELFFileError(ENTRYPOINTNOTSUPPORTEDERROR, (char*)iUseCase->InputElfFileName());

	SetUpExceptions();
	SetUids();
	SetSecureId();
	SetVendorId();
	SetCallEntryPoints();
	SetCapability();
	SetPriority(isDllp);
	SetFixedAddress(isDllp);
	SetVersion();
	SetCompressionType();
	SetFPU();

	SetPaged();

	SetSymbolLookup();
	SetDebuggable();
	SetSmpSafe();
	UpdateHeaderCrc();
}

/**
This function returns the CPU identifier for the E32 image header.
@internalComponent
@released
*/
E32ImageFile::uint16 E32ImageFile::GetCpuIdentifier()
{
	return (uint16)ECpuArmV5;
}

/**
This function returns the entry point of the E32 image .
@internalComponent
@released
*/
E32ImageFile::uint32 E32ImageFile::EntryPointOffset()
{
	return iElfExecutable->EntryPointOffset();
}

/**
This function validates the entry point of the E32 image .
@internalComponent
@released
*/
E32ImageFile::EEntryPointStatus E32ImageFile::ValidateEntryPoint()
{
	uint32 epOffset = iHdr->iEntryPoint;
	if (epOffset & 3)
		return EEntryPointOK;	// if entry point not 4 byte aligned, must be old style
	uint32 fileOffset = epOffset + iElfExecutable->iCodeSegmentHdr->p_offset;
	if (fileOffset+4 > iChunks.GetOffset())
		return EEntryPointCorrupt;	// entry point is past the end of the file??
	int ept = 0;			// old style if first instruction not recognised
	uint8 * p = ELF_ENTRY_PTR(uint8, iElfExecutable->iElfHeader, fileOffset + 4);
	uint32 x = *--p;
	x<<=8;
	x|=*--p;
	x<<=8;
	x|=*--p;
	x<<=8;
	x|=*--p;
	if ((x & 0xffffff00) == 0xe31f0000)
	{
		// starts with tst pc, #n - new entry point
		ept = (x & 0xff) + 1;
	}
	if (ept>7)
		return EEntryPointNotSupported;
	iHdr->iFlags |= (ept<<KImageEptShift);
	return EEntryPointOK;
}

/**
This function sets the exciption descriptor in the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetUpExceptions()
{
	char * aExDescName = "Symbian$$CPP$$Exception$$Descriptor";
	Elf32_Sym * aSym = iElfExecutable->LookupStaticSymbol(aExDescName);
	if (aSym)
	{
		uint32 aSymVaddr = aSym->st_value;
		uint32 aROBase = iElfExecutable->GetROBase();
		uint32 aROSize = iElfExecutable->GetROSize();
		//check its in RO segment
		if (aSymVaddr < aROBase || aSymVaddr >= (aROBase + aROSize))
		{
			throw ELFFileError(EXCEPTIONDESCRIPTOROUTSIDEROERROR,(char*)iUseCase->InputElfFileName());
		}
		// Set bottom bit so 0 in header slot means an old binary.
		// The decriptor is always aligned on a 4 byte boundary.
		iHdr->iExceptionDescriptor = (aSymVaddr - aROBase) | 0x00000001;
	}	
}

/**
This function sets the UIDs of the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetUids()
{
	iHdr->iUid1=iUseCase->GetUid1();
	iHdr->iUid2=iUseCase->GetUid2();
	iHdr->iUid3=iUseCase->GetUid3();
}

/**
This function sets the secure ID of the E32 image as passed in the command line.
@internalComponent
@released
*/
void E32ImageFile::SetSecureId()
{
	if (iUseCase->GetSecureIdOption())
		iHdr->iS.iSecureId = iUseCase->GetSecureId();
	else
		iHdr->iS.iSecureId = iUseCase->GetUid3();
}

/**
This function sets the vendor Id of the E32 image as passed in command line.
@internalComponent
@released
*/
void E32ImageFile::SetVendorId()
{
	iHdr->iS.iVendorId = iUseCase->GetVendorId();
}

/**
This function sets the call entry point of the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetCallEntryPoints()
{
	if (iUseCase->GetCallEntryPoints())
		iHdr->iFlags|=KImageNoCallEntryPoint;
	else
		iHdr->iFlags&=~KImageNoCallEntryPoint;
}

/**
This function sets the capcbility of the E32 image as specified in the command line.
@internalComponent
@released
*/
void E32ImageFile::SetCapability()
{
	iHdr->iS.iCaps = iUseCase->GetCapability();
}

/**
This function sets the priority of the E32 exe.
@internalComponent
@released
*/
void E32ImageFile::SetPriority(bool isDllp)
{
	if (iUseCase->GetPriority())
	{
		if (isDllp)
		{
			cerr << "Warning: Cannot set priority of a DLL." << endl;
		}
		else
			iHdr->iProcessPriority = (unsigned short)iUseCase->GetPriority();
	}
}

/**
This function sets the fixed address flag of the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetFixedAddress(bool isDllp)
{
	if (iUseCase->GetFixedAddress())
	{
		if (isDllp)
		{
			cerr << "Warning: Cannot set fixed address for DLL." << endl;
		}
		else
			iHdr->iFlags|=KImageFixedAddressExe;
	}
	else
		iHdr->iFlags&=~KImageFixedAddressExe;
}

/**
This function sets the version of the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetVersion()
{
	iHdr->iModuleVersion = iUseCase->GetVersion();
}

/**
This function sets the compression type of the E32 image .
@internalComponent
@released
*/
void E32ImageFile::SetCompressionType()
{
	if(iUseCase->GetCompress())
		iHdr->iCompressionType = iUseCase->GetCompressionMethod();
	else
		iHdr->iCompressionType = KFormatNotCompressed;

}

/**
This function sets the FPU type that the E32 image targets .
@internalComponent
@released
*/
void E32ImageFile::SetFPU()
{
	iHdr->iFlags &=~ KImageHWFloatMask;

	if (iUseCase->GetFPU() == 1)
		iHdr->iFlags |= KImageHWFloat_VFPv2;
}

/**
This function sets the paging attribute in the E32 image.
@internalComponent
@released
*/
void E32ImageFile::SetPaged()
{
	// Code paging.

	if ( iUseCase->IsCodePaged() )
	{
		iHdr->iFlags |= KImageCodePaged;
		iHdr->iFlags &= ~KImageCodeUnpaged;
	}
	else if ( iUseCase->IsCodeUnpaged() )
	{
		iHdr->iFlags |= KImageCodeUnpaged;
		iHdr->iFlags &= ~KImageCodePaged;
	}
	else if ( iUseCase->IsCodeDefaultPaged() )
	{
		iHdr->iFlags &= ~KImageCodePaged;
		iHdr->iFlags &= ~KImageCodeUnpaged;
	}

	// Data paging.

	if ( iUseCase->IsDataPaged() )
	{
		iHdr->iFlags |=  KImageDataPaged;
		iHdr->iFlags &= ~KImageDataUnpaged;
	}
	else if ( iUseCase->IsDataUnpaged() )
	{
		iHdr->iFlags |=  KImageDataUnpaged;
		iHdr->iFlags &= ~KImageDataPaged;
	}
	else if ( iUseCase->IsDataDefaultPaged() )
	{
		iHdr->iFlags &= ~KImageDataPaged;
		iHdr->iFlags &= ~KImageDataUnpaged;
	}
}

/**
This function sets the Debuggable attribute in the E32 image.
@internalComponent
@released
*/
void E32ImageFile::SetDebuggable()
{
	if (iUseCase->IsDebuggable() == true)
	{
		iHdr->iFlags |= KImageDebuggable;
	}
	else
	{
		iHdr->iFlags &= ~KImageDebuggable;
	}
}


void E32ImageFile::SetSmpSafe()
{
	if ( iUseCase->IsSmpSafe() )
	{
		iHdr->iFlags |= KImageSMPSafe;
	}
	else
	{
		iHdr->iFlags &= ~KImageSMPSafe;
	}
}

/**
This function sets the named symol-lookup attribute in the E32 image.
@internalComponent
@released
*/
void E32ImageFile::SetSymbolLookup()
{
	if(iUseCase->GetNamedSymLookup())
	{
		iHdr->iFlags |= KImageNmdExpData;
	}
	else
	{
		iHdr->iFlags &= ~KImageNmdExpData;
	}
}

/**
Class for Uids.
@internalComponent
@released
*/
class TE32ImageUids
{
public:
	TE32ImageUids(TUint32 aUid1, TUint32 aUid2, TUint32 aUid3);
	void Set(const TUidType& aUidType);
	TUint Check() { return ((checkSum(((TUint8*)this)+1)<<16)|checkSum(this));}
private:
	TUidType iType;
	TUint iCheck;

};

/**
Constructor for TE32ImageUids.
@internalComponent
@released
*/
TE32ImageUids::TE32ImageUids(TUint32 aUid1, TUint32 aUid2, TUint32 aUid3)
{
	Set(TUidType(TUid::Uid(aUid1), TUid::Uid(aUid2), TUid::Uid(aUid3)));
}

/**
This function sets the Uid.
@internalComponent
@released
*/
void TE32ImageUids::Set(const TUidType& aUidType)
{
    iType=aUidType;
    iCheck=Check();
}

/**
Default constructor for TUidType class.
@internalComponent
@released
*/
TUidType::TUidType()
{
	memset(this, 0, sizeof(TUidType));
}

/**
Constructor for TUidType class.
@internalComponent
@released
*/
TUidType::TUidType(TUid aUid1,TUid aUid2,TUid aUid3)
{
	iUid[0]=aUid1;
	iUid[1]=aUid2;
	iUid[2]=aUid3;
}

// needed by E32ImageHeaderV::ValidateHeader...
TCheckedUid::TCheckedUid(const TUidType& aUidType)
	{
    iType = aUidType;
    iCheck = ((TE32ImageUids*)this)->Check();
	}

// needed by E32ImageHeaderV::ValidateHeader...
void Mem::Crc32(TUint32& aCrc, const TAny* aPtr, TInt aLength)
	{
	::Crc32(aCrc, aPtr, aLength);
	}

/**
This function updates the CRC of the E32 Image.
@internalComponent
@released
*/
void E32ImageFile::UpdateHeaderCrc()
{
	TE32ImageUids u(iHdr->iUid1, iHdr->iUid2, iHdr->iUid3);
	iHdr->iUidChecksum = u.Check();
	TInt hdrsz = GetExtendedE32ImageHeaderSize();
	iHdr->iUncompressedSize = iChunks.GetOffset() - Align(GetExtendedE32ImageHeaderSize(), sizeof(uint32));
	iHdr->iHeaderCrc = KImageCrcInitialiser;
	uint32 crc = 0;
	Crc32(crc, iHdr, hdrsz);
	iHdr->iHeaderCrc = crc;
}

/**
This function creates a buffer and writes all the data into the buffer.
@internalComponent
@released
*/
void E32ImageFile::AllocateE32Image()
{
	size_t aImageSize = iChunks.GetOffset();
	iE32Image = new char[aImageSize];
	memset(iE32Image, 0, aImageSize);

	E32ImageChunks::ChunkList aChunkList = iChunks.GetChunks();
	E32ImageChunks::ChunkList::iterator p;
	for(p = aChunkList.begin(); p != aChunkList.end(); p++)
	{
		(*p)->Write(iE32Image);
	}

	E32ImageHeaderV* header = (E32ImageHeaderV*)iE32Image;
	TInt headerSize = header->TotalSize();
	if(KErrNone!=header->ValidateWholeImage(iE32Image+headerSize,GetE32ImageSize()-headerSize))
		throw InvalidE32ImageError(VALIDATIONERROR, (char*)iUseCase->OutputE32FileName());
}

/**
This function deflates the compressed data.
@param bytes
@param size
@param os
@internalComponent
@released
*/
void DeflateCompress(char* bytes, size_t size, ofstream & os);

/**
This function Paged Pack the compressed data.
@param bytes
@param size
@param os
@internalComponent
@released
*/
void CompressPages(TUint8 * bytes, TInt size, ofstream& os);


/**
This function writes into the final E32 image file.
@param aName - E32 image file name
@internalComponent
@released
*/
bool E32ImageFile::WriteImage(const char * aName)
{
	ofstream *os = new ofstream();
	os->open(aName, ofstream::binary|ofstream::out);

	if (os->is_open()) 
	{
		uint32 compression = iHdr->CompressionType();
		if (compression == KUidCompressionDeflate)
		{
			size_t aHeaderSize = GetExtendedE32ImageHeaderSize();
			size_t aBodySize = GetE32ImageSize() - aHeaderSize;
			os->write(iE32Image, aHeaderSize);
			DeflateCompress(iE32Image + aHeaderSize, aBodySize, *os);
		}
		else if (compression == KUidCompressionBytePair)
		{
			size_t aHeaderSize = GetExtendedE32ImageHeaderSize();
			os->write(iE32Image, aHeaderSize);
			
			// Compress and write out code part
			int srcStart = GetExtendedE32ImageHeaderSize();
			CompressPages( (TUint8*)iE32Image + srcStart, iHdr->iCodeSize, *os);
			
			
			// Compress and write out data part
			srcStart += iHdr->iCodeSize;
			int srcLen = GetE32ImageSize() - srcStart;
			
			CompressPages((TUint8*)iE32Image + srcStart, srcLen, *os);		

		}
		else if (compression == 0)
		{
			os->write(iE32Image, GetE32ImageSize()); // image not compressed
		}

	} 
	else
	{
		throw FileError(FILEOPENERROR,(char*)aName);
	}
	os->close();
	if(os!=NULL)
	{
	delete os;
	os = NULL;
	}
	return true;
}


/**
Constructor for E32ImageFile class.
@internalComponent
@released
*/
E32ImageFile::E32ImageFile(): iFileName(NULL), iE32Image(NULL),iExportBitMap(0),cleanupStack(0),  iData(NULL),iHdr(NULL),iImportSection(0), iSize(0), iOrigHdr(NULL),  iError(0), iSource(EE32Image), iOrigHdrOffsetAdj(0)  
{
	
};

/**
Destructor for E32ImageFile class.
@internalComponent
@released
*/
E32ImageFile::~E32ImageFile()
{
	delete[] iData;
	if (iHdr && iHdr != iOrigHdr)
		delete iHdr;

	delete [] iExportBitMap;
	delete [] iE32Image;
	delete [] iImportSection;

 	std::vector<char*>::iterator aPos;
 	char *aPtr;
 	aPos = cleanupStack.begin();
 	while( aPos != cleanupStack.end() )
 	{
 		aPtr = *aPos;
 		delete [] aPtr;
 		aPos++;
 	}
	
}

/**
Adjust the size of allocated data and fix the member data
@internalComponent
@released
*/
void E32ImageFile::Adjust(TInt aSize, TBool aAllowShrink)
{
	TInt asize = ((aSize+0x3)&0xfffffffc);

	if (asize == iSize)
		return;

	if (iSize == 0)
	{
		iSize = asize;
		iData = new char[iSize];
		memset(iData, 0, iSize);
	}
	else if (aAllowShrink || asize > iSize)
	{
		TInt oldsize = iSize;
		iSize = asize;
		iData = (char*)realloc(iData, iSize);
		
		if (iSize > oldsize)
			memset(iData+oldsize, 0, iSize-oldsize);
	}
	
	if (!iData)
		iSize = 0;
	
	if (iHdr && iHdr == iOrigHdr)
		iHdr = (E32ImageHeaderV*)iData;
	
	iOrigHdr = (E32ImageHeader*)iData;
}

/**
Read the E32 image.
@param is - input stream
@internalComponent
@released
*/
TInt E32ImageFile::ReadHeader(ifstream& is)
{
	Adjust(sizeof(E32ImageHeader), EFalse);
	is.read(iData, sizeof(E32ImageHeader));
	TInt hdrsz = iOrigHdr->TotalSize();
	
	if (hdrsz > 0x10000)
		return KErrCorrupt;	// sanity check
	
	if (hdrsz > (TInt)sizeof(E32ImageHeader))
	{
		Adjust(hdrsz, EFalse);
		is.read(iData+sizeof(E32ImageHeader), hdrsz-sizeof(E32ImageHeader));
	}

	TUint32 uncompressedSize;
	TInt r = iOrigHdr->ValidateHeader(iFileSize,uncompressedSize);
	
	if (r != KErrNone)
	{
		fprintf(stderr, "Integrity check failed %d\n", r);
		return r;
	}
	
	iHdr = (E32ImageHeaderV*)iOrigHdr;
	return KErrNone;
}

/**
Return the offset of the text section
@internalComponent
@released
*/
TUint E32ImageFile::TextOffset()
{
	return 0;
}

/**
Return the offset of the data section
@internalComponent
@released
*/
TUint E32ImageFile::DataOffset()
{
	return iHdr->iCodeSize;
}

/**
Return the offset of the bss section
@internalComponent
@released
*/
TUint E32ImageFile::BssOffset()
{
	return DataOffset()+iHdr->iDataSize;
}


/**
This function creates the bitmap after reading the E32 image file
@internalComponent
@released
*/
void E32ImageFile::E32ImageExportBitMap()
{
	TInt nexp = iOrigHdr->iExportDirCount;
	TInt memsz = (nexp + 7) >> 3;
	iExportBitMap = new TUint8[memsz];
	memset(iExportBitMap, 0xff, memsz);
	TUint* exports = (TUint*)(iData + iOrigHdr->iExportDirOffset);
	TUint absoluteEntryPoint = iOrigHdr->iEntryPoint + iOrigHdr->iCodeBase;
	TUint impfmt = iOrigHdr->ImportFormat();
	TUint hdrfmt = iOrigHdr->HeaderFormat();
	TUint absentVal = (impfmt == KImageImpFmt_ELF) ? absoluteEntryPoint : iOrigHdr->iEntryPoint;
	TInt i;
	iMissingExports = 0;
	
	for (i=0; i<nexp; ++i)
	{
		if (exports[i] == absentVal)
		{
			iExportBitMap[i>>3] &= ~(1u << (i & 7));
			++iMissingExports;
		}
	}
	
	if (hdrfmt < KImageHdrFmt_V && iMissingExports)
	{
		fprintf(stderr, "Bad exports\n");
		exit(999);
	}
}

/**
This function creates the export description after reading the E32 image file
@internalComponent
@released
*/
TInt E32ImageFile::CheckExportDescription()
{
	TUint hdrfmt = iOrigHdr->HeaderFormat();
	if (hdrfmt < KImageHdrFmt_V && iMissingExports)
		return KErrCorrupt;
	
	if (iHdr->iExportDescType == KImageHdr_ExpD_NoHoles)
	{
		return iMissingExports ? KErrCorrupt : KErrNone;
	}
	
	TInt nexp = iOrigHdr->iExportDirCount;
	TInt memsz = (nexp + 7) >> 3;	// size of complete bitmap
	TInt mbs = (memsz + 7) >> 3;	// size of meta-bitmap
	TInt eds = iHdr->iExportDescSize;
	
	if (iHdr->iExportDescType == KImageHdr_ExpD_FullBitmap)
	{
		if (eds != memsz)
			return KErrCorrupt;
		if (memcmp(iHdr->iExportDesc, iExportBitMap, eds) == 0)
			return KErrNone;
		return KErrCorrupt;
	}
	
	if (iHdr->iExportDescType != KImageHdr_ExpD_SparseBitmap8)
		return KErrNotSupported;
	
	TInt nbytes = 0;
	TInt i;
	for (i=0; i<memsz; ++i)
		if (iExportBitMap[i] != 0xff)
			++nbytes;				// number of groups of 8
	
	TInt exp_extra = mbs + nbytes;
	if (eds != exp_extra)
		return KErrCorrupt;
	
	const TUint8* mptr = iHdr->iExportDesc;
	const TUint8* gptr = mptr + mbs;
	for (i=0; i<memsz; ++i)
	{
		TUint mbit = mptr[i>>3] & (1u << (i&7));
		if (iExportBitMap[i] != 0xff)
		{
			if (!mbit || *gptr++ != iExportBitMap[i])
				return KErrCorrupt;
		}
		else if (mbit)
			return KErrCorrupt;
	}
	
	return KErrNone;
}


int  DecompressPages(TUint8 * bytes, ifstream& is);


/**
This function creates the E32 image reading from the file
@param is
@param aImage
@internalComponent
@released
*/
ifstream& operator>>(ifstream& is, E32ImageFile& aImage)
{
	aImage.iError = aImage.ReadHeader(is);
	if (aImage.iError != KErrNone)
		return is;
	
	E32ImageHeader* oh = aImage.iOrigHdr;
	TInt orighdrsz = oh->TotalSize();
	int remainder = aImage.iSize - orighdrsz;
	TUint compression = oh->CompressionType();
	if (compression == 0)
	{
		is.read(aImage.iData + orighdrsz, remainder);
	}
	else if (compression == KUidCompressionDeflate)
	{ //Uncompress
		aImage.iError = KErrNoMemory;
		unsigned int uncompsize = ((E32ImageHeaderComp*)aImage.iOrigHdr)->iUncompressedSize;
		aImage.Adjust(uncompsize + orighdrsz);
		
		if (aImage.iData==NULL)
			return is;
		
		oh = aImage.iOrigHdr;
		
		unsigned char* compressedData = new unsigned char[remainder];
		if (compressedData==NULL)
			return is;
		
		is.read(reinterpret_cast<char *>(compressedData), remainder);
		unsigned int destsize = uncompsize;
		InflateUnCompress( compressedData, remainder, (unsigned char*)(aImage.iData + orighdrsz), destsize);
		
		if (destsize != uncompsize)
			MessageHandler::GetInstance()->ReportMessage(WARNING, HUFFMANINCONSISTENTSIZEERROR);
		
		delete [] compressedData;
		
		if ((TUint)orighdrsz > oh->iCodeOffset)
		{
			// need to adjust code offsets in original
			aImage.iOrigHdrOffsetAdj = (TUint)orighdrsz - oh->iCodeOffset;
			aImage.OffsetAdjust(oh->iCodeOffset);
			aImage.OffsetAdjust(oh->iDataOffset);
			aImage.OffsetAdjust(oh->iCodeRelocOffset);
			aImage.OffsetAdjust(oh->iDataRelocOffset);
			aImage.OffsetAdjust(oh->iImportOffset);
			aImage.OffsetAdjust(oh->iExportDirOffset);
		}
		aImage.iError = KErrNone;
	}
	else if(compression == KUidCompressionBytePair)
	{ // Uncompress
		aImage.iError = KErrNoMemory;
		unsigned int uncompsize = ((E32ImageHeaderComp*)aImage.iOrigHdr)->iUncompressedSize;
		aImage.Adjust(uncompsize + orighdrsz);
		if (aImage.iData==NULL)
			return is;
		oh = aImage.iOrigHdr;

		// Read and decompress code part of the image

		unsigned int uncompressedCodeSize = DecompressPages((TUint8 *) (aImage.iData + orighdrsz), is);

		
		// Read and decompress data part of the image

		unsigned int uncompressedDataSize = DecompressPages((TUint8 *) (aImage.iData + orighdrsz + uncompressedCodeSize), is);

		if (uncompressedCodeSize + uncompressedDataSize != uncompsize)
			MessageHandler::GetInstance()->ReportMessage(WARNING, BYTEPAIRINCONSISTENTSIZEERROR);

		if ((TUint)orighdrsz > oh->iCodeOffset)
			{
			// need to adjust code offsets in original
			aImage.iOrigHdrOffsetAdj = (TUint)orighdrsz - oh->iCodeOffset;
			aImage.OffsetAdjust(oh->iCodeOffset);
			aImage.OffsetAdjust(oh->iDataOffset);
			aImage.OffsetAdjust(oh->iCodeRelocOffset);
			aImage.OffsetAdjust(oh->iDataRelocOffset);
			aImage.OffsetAdjust(oh->iImportOffset);
			aImage.OffsetAdjust(oh->iExportDirOffset);
			}
		aImage.iError = KErrNone;
	}
	aImage.E32ImageExportBitMap();

	return is;
}
#ifdef __LINUX__ 
#include <sys/stat.h>
/**
Simple function uses stdlib fstat to obtain the size of the file. 
@param aFileName - e32 image file name
@internalComponent
@released
*/ 
int GetFileSize(const char* aFileName) {
    // Open the file the old-fashioned way :-)
    struct stat fileInfo; 
    if(stat(aFileName,&fileInfo)!=0) {
        throw FileError(FILEOPENERROR,(char *)aFileName);
    }
    off_t fileSize = fileInfo.st_size;
    return fileSize; 
}
#else 
int GetFileSize(const char* aFileName) {
    _finddata_t fileinfo;
	int ret=_findfirst((char *)aFileName,&fileinfo);
	if (ret==-1) 
	{
		throw FileError(FILEOPENERROR,(char *)aFileName);
	}
    return fileinfo.size; 
}
#endif


/**
This function opens the  e32 image file.
@param aFileName - e32 image file name
@internalComponent
@released
*/
TInt E32ImageFile::Open(const char* aFileName)
{
	iFileSize = GetFileSize(aFileName);
	
	Adjust(iFileSize);
	ifstream ifile((char *)aFileName, ios::in | ios::binary);
	if(!ifile.is_open())
	{
		throw FileError(FILEOPENERROR,(char *)aFileName);
	}
	ifile >> *this;
	ifile.close();

	if (iError != KErrNone)
		return iError;
	
	return KErrNone;
}

void E32ImageFile::ProcessSymbolInfo() {
	
	Elf32_Addr aPlace = iUseCase->GetExportTableAddress() - 4;// This location points to 0th ord.
	// Create a relocation entry for the 0th ordinal.
	ElfLocalRelocation *aRel = new ElfLocalRelocation(iElfExecutable, aPlace, 0, 0, R_ARM_ABS32, \
		NULL, ESegmentRO, NULL, false);
	aRel->Add();

	aPlace += iUseCase->GetExportTableSize();// aPlace now points to the symInfo
	uint32 *aZerothOrd = (uint32*)iUseCase->GetExportTable();
	*aZerothOrd = aPlace;
	aPlace += sizeof(E32EpocExpSymInfoHdr);// aPlace now points to the symbol address
											// which is just after the syminfo header.
	if(!iElfExecutable->iExports)
		return;

	// Donot disturb the internal list sorting.
	ElfExports::ExportList aList = iElfExecutable->iExports->GetExports(false);

	ElfExports::ExportList::iterator aIter = aList.begin();
	DllSymbol *aSym;
	TUint aAlign, aNameLen;

	
	char aPad[] = {'\0', '\0', '\0', '\0'};


	while ( aIter != aList.end() ) {
		aSym = *aIter;
		iSymAddrTab.push_back(aSym->iElfSym->st_value);
		// The symbol names always start at a 4-byte aligned offset.
		iSymNameOffset = iSymbolNames.size() >> 2;
		iSymNameOffTab.push_back(iSymNameOffset);

		iSymbolNames += aSym->SymbolName();
		iSymbolNames += '\0';
		aNameLen = iSymbolNames.size();
		aAlign = Align(aNameLen, sizeof(int));
		aAlign = aAlign - aNameLen;
		if(aAlign % 4){
			iSymbolNames.append(aPad, aAlign);
		}
		//Create a relocation entry...
		aRel = new ElfLocalRelocation(iElfExecutable, aPlace, 0, 0, R_ARM_ABS32, NULL,\
			ESegmentRO, aSym->iElfSym, false);
		aPlace += sizeof(uint32);
		aRel->Add();
		aIter++;
	}
}

char* E32ImageFile::CreateSymbolInfo(size_t aBaseOffset) {
	E32EpocExpSymInfoHdr aSymInf;
	uint32 aSizeofNames, aSize;

	SetSymInfo(aSymInf);
	if( aSymInf.iFlags & 1) {
		aSizeofNames = sizeof(uint32);
	}
	else {
		aSizeofNames = sizeof(uint16);
	}

	aSize = aSymInf.iSize;

	char* aInfo = new char[aSize];
	memset(aInfo, 0, aSize);
	memcpy(aInfo, (void*)&aSymInf, sizeof(aSymInf));

	TUint aPos = aSymInf.iSymbolTblOffset;
	memcpy(aInfo+aPos, iSymAddrTab.begin(), iSymAddrTab.size()*sizeof(uint32));

	aPos += iSymAddrTab.size()*aSizeofNames;
	aPos += iSymNameOffTab.size()*aSizeofNames;
	aPos = Align(aPos, sizeof(uint32));

	std::vector<uint32>::iterator Iter = iSymNameOffTab.begin();
	TInt aOffLen = 2;
	if(aSymInf.iFlags & 1)
		aOffLen=4;
	while(Iter != iSymNameOffTab.end()){
		memcpy( ((void*)(aInfo+aPos)), ((void*)Iter), aOffLen);
		aPos += aOffLen;
		Iter++;
	}

	aPos = aSymInf.iStringTableOffset;
	memcpy(aInfo+aPos, iSymbolNames.begin(), iSymbolNames.size());

	// At the end, the dependencies are listed. They remain zeroes and shall be fixed up
	// while relocating.

	// Update the import table to have offsets to ordinal zero entries
	uint32 *aLocation, aOffset;
	uint32 *aImportTab = iImportSection;

	std::vector<int>::iterator aIter = iImportTabLocations.begin();
	aOffset = aBaseOffset - iHdr->iCodeOffset;// This gives the offset of syminfo table base
										// wrt the code section start
	aOffset += aSymInf.iDepDllZeroOrdTableOffset; // This points to the ordinal zero offset table now
	while( aIter != iImportTabLocations.end()) {
		aLocation = (aImportTab + *aIter);
		*aLocation = aOffset;
		aOffset += sizeof(uint32);
		aIter++;
	}

	return aInfo;
}

void E32ImageFile::SetSymInfo(E32EpocExpSymInfoHdr& aSymInfo)
{
	uint32 aSize = sizeof(E32EpocExpSymInfoHdr);
	memset(&aSymInfo, 0, aSize);

	uint16 aNSymbols = (uint16) iSymAddrTab.size();
	aSymInfo.iSymCount = aNSymbols;
	aSymInfo.iSymbolTblOffset = aSize;
	aSize += aNSymbols * sizeof(uint32); // Symbol addresses
	TUint aNameTabSz = iSymbolNames.size();
	TInt aSizeofNames;

	if( iSymNameOffset < 0xffff) {
		aSizeofNames = sizeof(uint16);
		aSymInfo.iFlags &= ~1;//reset the 0th bit
	}
	else {
		aSizeofNames = sizeof(uint32);
		aSymInfo.iFlags |= 1;//set the 0th bit
	}
	aSize += Align((aNSymbols * aSizeofNames), sizeof(uint32)); // Symbol name offsets
	aSymInfo.iStringTableOffset = aSize;
	aSize += aNameTabSz; // Symbol names in string tab
	aSymInfo.iStringTableSz = aNameTabSz;
	aSymInfo.iDepDllZeroOrdTableOffset = aSize;
	aSymInfo.iDllCount = iNumDlls ;
	aSize += iNumDlls * sizeof(uint32); // Dependency list - ordinal zero placeholder
	aSymInfo.iSize = aSize;
}

