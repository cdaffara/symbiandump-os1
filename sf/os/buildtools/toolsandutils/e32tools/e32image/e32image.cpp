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
// e32tools/e32image/e32image.cpp
// Basic operations on E32Image files which are used by ROMBUILD.
// These are independent of the original file format from which the
// E32Image file was derived.
// 
//

#include <time.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "h_utl.h"


#if defined (__MSVCDOTNET__) || defined(__TOOLS2__)
#include <fstream>
#else //!__MSVCDOTNET__
#include <fstream.h>
#endif //__MSVCDOTNET__

#include <assert.h>
#ifndef __LINUX__
  #include <io.h>
#endif
#include "h_ver.h"

// get E32ImageHeader class...
#define INCLUDE_E32IMAGEHEADER_IMPLEMENTATION
#define RETURN_FAILURE(_r) return (fprintf(stderr, "line %d\n", __LINE__),_r)
//#define E32IMAGEHEADER_TRACE(_t) printf _t
#include "e32image.h"

void DeflateCompress(char* bytes, TInt size, ostream& os);
void InflateUnCompress(unsigned char* source, int sourcesize, unsigned char* dest, int destsize);
void CompressPages(TUint8 * bytes, TInt size, ostream &os, CBytePair *aBPE);
int  DecompressPages(TUint8 * bytes, istream& is, CBytePair *aBPE);

// needed by E32ImageHeaderV::ValidateHeader...
void Mem::Crc32(TUint32& aCrc, const TAny* aPtr, TInt aLength)
	{
	HMem::Crc32(aCrc, aPtr, aLength);
	}

//
// E32 Image files
//
E32ImageFile::E32ImageFile(CBytePair *aBPE)
	: iData(NULL), iSize(0), iOrigHdr(NULL), iHdr(NULL), iFileName(NULL)
#ifndef __LINUX__
	  , iWideFileName(NULL)
#endif
, iError(0), iSource(EE32Image), iOrigHdrOffsetAdj(0), iExportBitMap(0), iBPE(aBPE)
	{
            if(aBPE == NULL) { 
                iOwnBPE = 1;
                iBPE = new CBytePair(EFalse);
            }
            else {
                iOwnBPE = 0;
                iBPE = aBPE;
            }
        }

E32ImageFile::~E32ImageFile()
	{
            if(iOwnBPE == 1) {
                delete iBPE;
            }

	free(iData);
	delete [] iFileName;
#ifndef __LINUX__
	delete [] iWideFileName;
#endif
	if (iHdr && iHdr != iOrigHdr)
		delete iHdr;
	free(iExportBitMap);
	}

// dummy implementation
TBool E32ImageFile::Translate(const char*, TUint, TBool, TBool)
	{
	return EFalse;
	}

Int64 timeToInt64(TInt aTime)
	{
	aTime-=(30*365*24*60*60+7*24*60*60);	// seconds since midnight Jan 1st, 2000
	Int64 daysTo2000AD=730497;
	Int64 t=daysTo2000AD*24*3600+aTime;	// seconds since 0000
	t=t+3600;								// BST (?)
	return t*1000000;						// milliseconds
	}

class TE32ImageUids : public TCheckedUid
	{
public:
	TE32ImageUids(TUint32 aUid1, TUint32 aUid2, TUint32 aUid3) : TCheckedUid(TUidType(TUid::Uid(aUid1), TUid::Uid(aUid2), TUid::Uid(aUid3))) {}
	TUint Check() { return TCheckedUid::Check(); }
	};

void E32ImageFile::SetDefaultHeader()
	{
	iHdr = (E32ImageHeaderV*)iOrigHdr;
	iHdr->iUid1 = 0;
	iHdr->iUid2 = 0;
	iHdr->iUid3 = 0;
	iHdr->iHeaderCrc = 0;
	iHdr->iSignature = 0x434f5045u;
	iHdr->iModuleVersion = 0x00010000u;
	iHdr->iCompressionType = 0;
	iHdr->iToolsVersion = TVersion(MajorVersion, MinorVersion, Build);
	Int64 time1(timeToInt64(time(0))); 
	iHdr->iTimeLo=(TUint32)time1;
	iHdr->iTimeHi=(TUint32)(time1>>32);
	iHdr->iFlags = KImageHdrFmt_V;
	iHdr->iCodeSize = 0;
	iHdr->iDataSize = 0;
	iHdr->iHeapSizeMin = 0;
	iHdr->iHeapSizeMax = 0;
	iHdr->iStackSize = 0;
	iHdr->iBssSize = 0;
	iHdr->iEntryPoint = 0;
	iHdr->iCodeBase = 0;
	iHdr->iDataBase = 0;
	iHdr->iDllRefTableCount = 0;
	iHdr->iExportDirOffset = 0;
	iHdr->iExportDirCount = 0;
	iHdr->iTextSize = 0;
	iHdr->iCodeOffset = 0;
	iHdr->iDataOffset = 0;
	iHdr->iImportOffset = 0;
	iHdr->iCodeRelocOffset = 0;
	iHdr->iDataRelocOffset = 0;
	iHdr->iProcessPriority = (TUint16)EPriorityForeground;
	iHdr->iUncompressedSize = 0;
	iHdr->iS.iSecureId = 0;
	iHdr->iS.iVendorId = 0;
	iHdr->iExceptionDescriptor = 0;
	iHdr->iSpare2 = 0;

	iHdr->iExportDescSize = 0;
	iHdr->iExportDescType = KImageHdr_ExpD_NoHoles;
	iHdr->iExportDesc[0] = 0;
	}

void E32ImageFile::SetCallEntryPoints(TInt aBool)
	{

	if (aBool)
		iHdr->iFlags&=~KImageNoCallEntryPoint;
	else
		iHdr->iFlags|=KImageNoCallEntryPoint;
	}

void E32ImageFile::SetFixedAddress(TInt aBool)
	{

	if (aBool)
		iHdr->iFlags|=KImageFixedAddressExe;
	else
		iHdr->iFlags&=~KImageFixedAddressExe;
	}

void E32ImageFile::SetPriority(TProcessPriority aPri)
	{

	iHdr->iProcessPriority = (TUint16)aPri;
	}

void E32ImageFile::SetCapability(SCapabilitySet& aCapabilities)
	{
	iHdr->iS.iCaps = aCapabilities;
	}

void E32ImageFile::SetFPU(unsigned int aFPU)
	{
	iHdr->iFlags &=~ KImageHWFloatMask;

	if (aFPU == 1)
		iHdr->iFlags |= KImageHWFloat_VFPv2;
	}

void E32ImageFile::Adjust(TInt aSize, TBool aAllowShrink)
//
// Adjust the size of allocated data and fix the member data
//
	{

	TInt asize = ALIGN4(aSize);
	if (asize == iSize)
		return;
	if (iSize == 0)
		{
		iSize = asize;
		iData = (char*)malloc(iSize);
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

TInt E32ImageFile::ReadHeader(ifstream& is)
	{
	Adjust(sizeof(E32ImageHeader), EFalse);
	is.read(iData, sizeof(E32ImageHeader));
	TInt hdrsz = iOrigHdr->TotalSize();
	if (hdrsz > 0x10000 || hdrsz <= 0)
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

void E32ImageFile::SetStackSize(TInt aSize)
	{
	iHdr->iStackSize=aSize;
	}

void E32ImageFile::SetHeapSizeMin(TInt aSize)
	{
	iHdr->iHeapSizeMin=aSize;
	}

void E32ImageFile::SetHeapSizeMax(TInt aSize)
	{
	iHdr->iHeapSizeMax=aSize;
	}

TUint E32ImageFile::TextOffset()
//
// Return the offset of the text section
//
	{
	return 0;
	}

TUint E32ImageFile::DataOffset()
//
// return the offset of the initialised data
//
	{
	return iHdr->iCodeSize;
	}

TUint E32ImageFile::BssOffset()
//
// return the offset from the start of code where the bss is linked
//
	{
	return DataOffset()+iHdr->iDataSize;
	}


TInt E32ImageFile::IsDll()
//
//
//
	{
	return iHdr->iFlags&KImageDll;
	}


void E32ImageFile::RelocateSection(char* aPtr, char* aRelocs, TUint aCodeDelta, TUint aDataDelta, char* aImagePtr, TLinAddr** aIATRefs, TBool keepIAT)
//
// Relocates the section data at aPtr
//	
	{

	TUint codeStart=iHdr->iCodeBase;
	TUint codeFinish=codeStart+iHdr->iCodeSize;
	TUint iatStart = aIATRefs ? codeStart+iHdr->iTextSize : 0;
	TUint iatFinish = aIATRefs ? iatStart+NumberOfImports()*sizeof(TUint) : 0;
	char* relocs=aRelocs;
	TUint page=0;
	TInt size=0;
	TInt i=((E32RelocSection *)relocs)->iNumberOfRelocs;
	relocs+=sizeof(E32RelocSection);
	while (i>0)
		{
		if (size>0)
			{
			TUint offset=*(TUint16 *)relocs;
			relocs+=2;
			if (offset!=0)
				{ // its a reloc
				TUint va=page+(offset&0x0fff);
				TUint relocType=offset&0xf000;
				TUint *dataptr=(TUint *)(aPtr+va);
				assert((char *)dataptr < aRelocs);
				TUint data=*dataptr;
				if (relocType == KTextRelocType) 
					*dataptr=data+aCodeDelta; // points to text/rdata section
				else if (relocType == KDataRelocType)
					*dataptr=data+aDataDelta;
				else 
					{
					if (relocType != KInferredRelocType)
						Print(EError,"Unrecognized relocation type %x\n",relocType);

					if (data>=iatStart && data<iatFinish)
						{

						TUint iatNum = (data-iatStart)/sizeof(TLinAddr);

						// If "keepIAT" is used then the importing instruction must import through the IAT entry,
						// but otherwise we change the IAT entry to point to the bit of code doing the importing
						// and do the real fix-up later on in TRomBuilderEntry::FixupImports.
						// NB: We always want to do this for X86 or data exports dont work.
						if (keepIAT || (iHdr->iCpuIdentifier & 0x1000) /*denotes X86*/) 
							*dataptr=data+aCodeDelta; 
						else 
							{
							if ((TUint)aIATRefs[iatNum]>65535)
								Print(EWarning, "Multiple relocations for IAT entry %d (0x%x, 0x%x)\n",
										iatNum, aIATRefs[iatNum], dataptr);
							else
								aIATRefs[iatNum] = (TLinAddr*)(aImagePtr+va);	// ROM image address of importing pointer					
							}
						}
					else if (data>=codeStart && data<codeFinish)
						*dataptr=data+aCodeDelta; // points to text/rdata section
					else
						*dataptr=data+aDataDelta; // points to data section
					}
				--i;
				}
			size-=2;
			}
		else
			{ // next page of relocs
			page=*(TUint *)relocs;
			relocs+=4;
			size=*(TUint *)relocs;
			relocs+=4;
			size-=8;
			}
		}
	}

void E32ImageFile::SetUids(TUid aUid1, TUid aUid2, TUid aUid3)
	{
	iHdr->iUid1=aUid1.iUid;
	iHdr->iUid2=aUid2.iUid;
	iHdr->iUid3=aUid3.iUid;
	}

void E32ImageFile::SetSecureId(TUint32 aId)
	{
	((E32ImageHeaderV*)iHdr)->iS.iSecureId = aId;
	}

void E32ImageFile::SetVendorId(TUint32 aId)
	{
	((E32ImageHeaderV*)iHdr)->iS.iVendorId = aId;
	}

void E32ImageFile::UpdateHeaderCrc()
	{
	TE32ImageUids u(iHdr->iUid1, iHdr->iUid2, iHdr->iUid3);
	iHdr->iUidChecksum = u.Check();
	TInt hdrsz = iHdr->TotalSize();
	TInt orighdrsz = iOrigHdr->TotalSize();
	iHdr->iUncompressedSize = iSize - orighdrsz;
	iHdr->iHeaderCrc = KImageCrcInitialiser;
	TUint32 crc = 0;
	HMem::Crc32(crc, iHdr, hdrsz);
	iHdr->iHeaderCrc = crc;
	}

TInt E32ImageFile::NumberOfImports()
//
// Return the number of imports made by this image
//
	{

	if (iHdr->iDllRefTableCount==0 || iHdr->iImportOffset==0)
		return 0;

	TUint impfmt = iHdr->ImportFormat();
	const E32ImportSection* isection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
	TInt d;
	TInt nImports = 0;
	const E32ImportBlock* b = (const E32ImportBlock*)(isection+1);
	for (d=0; d<iHdr->iDllRefTableCount; d++)
		{
		nImports += b->iNumberOfImports;
		b = b->NextBlock(impfmt);
		}

	if (impfmt==KImageImpFmt_PE || impfmt==KImageImpFmt_PE2)
		{
		TUint *imports=(TUint *)(iData + iOrigHdr->iCodeOffset + iHdr->iTextSize);
		TInt i=0;
		while (*imports++)
			i++;
		assert(i==nImports);
		}
	
	return nImports;
	}

// Work out which exports are missing from the export directory
void E32ImageFile::CreateExportBitMap()
	{
	TInt nexp = iOrigHdr->iExportDirCount;
	TInt memsz = (nexp + 7) >> 3;
	iExportBitMap = (TUint8*)malloc(memsz);
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

// Append an export description to the E32ImageHeader if necessary
void E32ImageFile::AddExportDescription()
	{
	if (iMissingExports == 0)
		return;	// nothing to do
	TInt nexp = iOrigHdr->iExportDirCount;
	TInt memsz = (nexp + 7) >> 3;	// size of complete bitmap
	TInt mbs = (memsz + 7) >> 3;	// size of meta-bitmap
	TInt nbytes = 0;
	TInt i;
	for (i=0; i<memsz; ++i)
		if (iExportBitMap[i] != 0xff)
			++nbytes;				// number of groups of 8
	TUint8 edt = KImageHdr_ExpD_FullBitmap;
	TInt extra_space = memsz - 1;
	if (mbs + nbytes < memsz)
		{
		edt = KImageHdr_ExpD_SparseBitmap8;
		extra_space = mbs + nbytes - 1;
		}
	extra_space = (extra_space + sizeof(TUint) - 1) &~ (sizeof(TUint) - 1);
	TInt hdrsz = sizeof(E32ImageHeaderV) + extra_space;
	iHdr = (E32ImageHeaderV*)malloc(hdrsz);
	memcpy(iHdr, iOrigHdr, sizeof(E32ImageHeaderV));
	iHdr->iExportDescType = edt;
	if (edt == KImageHdr_ExpD_FullBitmap)
		{
		iHdr->iExportDescSize = (TUint16)memsz;
		memcpy(iHdr->iExportDesc, iExportBitMap, memsz);
		}
	else
		{
		iHdr->iExportDescSize = (TUint16)(mbs + nbytes);
		memset(iHdr->iExportDesc, 0, extra_space + 1);
		TUint8* mptr = iHdr->iExportDesc;
		TUint8* gptr = mptr + mbs;
		for (i=0; i<memsz; ++i)
			{
			if (iExportBitMap[i] != 0xff)
				{
				mptr[i>>3] |= (1u << (i&7));
				*gptr++ = iExportBitMap[i];
				}
			}
		}
	iHdr->iCodeOffset += extra_space;
	if (iHdr->iDataOffset)
		iHdr->iDataOffset += extra_space;
	if (iHdr->iCodeRelocOffset)
		iHdr->iCodeRelocOffset += extra_space;
	if (iHdr->iDataRelocOffset)
		iHdr->iDataRelocOffset += extra_space;
	if (iHdr->iImportOffset)
		iHdr->iImportOffset += extra_space;
	if (iHdr->iExportDirOffset)
		iHdr->iExportDirOffset += extra_space;
	}

// Check the export description is consistent with the export directory
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


TInt E32ImageFile::Validate()
	{
	TInt orighdrsz = iOrigHdr->TotalSize();
	TInt r = iHdr->ValidateWholeImage(iData+orighdrsz,iSize-orighdrsz);
	if(r!=KErrNone)
		return r;
	return r;
	}


ostream& operator<<(ostream& os, const E32ImageFile& aImage)
//
// Output an E32ImageFile
//
	{
	E32ImageHeaderV* h = aImage.iHdr;
	TUint hdrfmt = h->HeaderFormat();
	if (hdrfmt != KImageHdrFmt_V)
		return os;	// don't generate old binary formats
	TInt hdrsz = h->TotalSize();
	TInt orighdrsz = aImage.iOrigHdr->TotalSize();

	os.write((const char*)aImage.iHdr, hdrsz);

	TUint compression = h->CompressionType();
	if (compression == KUidCompressionDeflate)
		{
		int srcsize = aImage.iSize - orighdrsz;
		DeflateCompress(aImage.iData + orighdrsz, srcsize, os);
		}
	else if (compression == KUidCompressionBytePair)
		{
		// Compress and write out code part
		int srcStart = orighdrsz;
		CompressPages( (TUint8*)aImage.iData + srcStart, aImage.iOrigHdr->iCodeSize, os, aImage.iBPE);
		
		
		// Compress and write out data part
		srcStart += aImage.iOrigHdr->iCodeSize;
		int srcLen = aImage.iSize - srcStart;

		CompressPages((TUint8*)aImage.iData + srcStart, srcLen, os, aImage.iBPE);		

		}
	else if (compression == KFormatNotCompressed)
		{
		int srcsize = aImage.iSize - orighdrsz;
		os.write(aImage.iData + orighdrsz, srcsize); // image not to be compressed
		}
	return os;
	}

ifstream& operator>>(ifstream& is, E32ImageFile& aImage)
//
// Input an E32ImageFile
//
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
			Print(EWarning, "Inconsistent sizes discovered during uncompression.\n");
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

		unsigned int uncompressedCodeSize = DecompressPages((TUint8 *) (aImage.iData + orighdrsz), is, aImage.iBPE);

		
		// Read and decompress data part of the image

		unsigned int uncompressedDataSize = DecompressPages((TUint8 *) (aImage.iData + orighdrsz + uncompressedCodeSize), is, aImage.iBPE);

		if (uncompressedCodeSize + uncompressedDataSize != uncompsize)
			Print(EWarning, "Inconsistent sizes discovered during uncompression.\n");

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
	aImage.CreateExportBitMap();
	return is;
	}

TInt E32ImageFile::IsE32ImageFile(char *aFileName)
	{

#ifdef __LINUX__
	E32ImageFile f;
	struct stat buf;
	if (stat(aFileName, &buf) < 0) 
		{
		return FALSE;
		}
	f.iFileSize = buf.st_size;
#else
	_finddata_t fileinfo;
	int ret=_findfirst((char *)aFileName,&fileinfo);
	if (ret==-1) 
		return FALSE;
	E32ImageFile f;
	f.iFileSize = fileinfo.size;
#endif
	ifstream ifile(aFileName, ios::in | ios::binary);
	if(!ifile.is_open())
		return FALSE;
	TInt r = f.ReadHeader(ifile);
	ifile.close();
	return (r == KErrNone);
	}

TInt E32ImageFile::IsValid()
	{
	return (iError == KErrNone);
	}

TInt E32ImageFile::Open(const char* aFileName)
//
// Open an E32 Image file
//
	{
#ifdef __LINUX__
	struct stat buf;
	if (stat(aFileName, &buf) < 0) 
		{
		Print(EError,"Cannot open %s for input.\n",aFileName);
		return 1;
		}
	iFileSize = buf.st_size;
#else
	_finddata_t fileinfo;
	int ret=_findfirst((char *)aFileName,&fileinfo);
	if (ret==-1) 
		{
		Print(EError,"Cannot open %s for input.\n",aFileName);
		return 1;
		}
	iFileSize = fileinfo.size;
#endif
	Adjust(iFileSize);
	ifstream ifile((char *)aFileName, ios::in | ios::binary);
	if(!ifile.is_open())
		{
		Print(EError,"Cannot open %s for input.\n",aFileName);
		return 1;
		}
	ifile >> *this;
	ifile.close();
	if (iError != KErrNone)
		return iError;
	iFileName=strdup((char *)aFileName);
	if (iFileName==NULL)
		return KErrNoMemory;
	return KErrNone;
	}

#ifndef __LINUX__
TInt E32ImageFile::Open(const wchar_t* aFileName)
//
// Open an E32 Image file
//
	{
	_wfinddata_t fileinfo;
	int ret=_wfindfirst(aFileName,&fileinfo);
	if (ret==-1) 
		{
		Print(EError,"Cannot open %ls for input.\n",aFileName);
		return 1;
		}
	iFileSize = fileinfo.size;
	Adjust(iFileSize);

	FILE* file = _wfopen(aFileName, L"rb");

	if(!file)
		{
		Print(EError,"Cannot open %ls for input.\n",aFileName);
		return 1;
		}
	
	#if defined(__TOOLS2__) || defined(__MSVCDOTNET__)
	char tmp[100];  				// Convert wide character name to char *, then open a file.
	wcstombs(tmp,aFileName,100);
	ifstream ifile(tmp, ios::in | ios::binary);
	#else
	ifstream ifile(fileno(file));
	#endif
	
	if(!ifile.is_open())
		{
		Print(EError,"Cannot open %ls for input,\n",aFileName);
		return 1;
		}
		
	ifile >> *this;
	ifile.close();
	fclose(file);
	if (iError != KErrNone)
		return iError;
	iWideFileName=wcsdup(aFileName);
	if (iWideFileName==NULL)
		return KErrNoMemory;
	return KErrNone;
	}
#endif

TUint E32ImageFile::VaOfOrdinal(TUint aOrdinal)
// return the offset of the exported symbol
	{
	TUint* exportdir = (TUint*)(iData + iOrigHdr->iExportDirOffset);
	return exportdir[aOrdinal-KOrdinalBase];
	}

// Determine the type of entry point in this module and set the flags
// in the E32Image header accordingly.
TInt E32ImageFile::DetermineEntryPointType()
	{
	TUint cpu = iHdr->CpuIdentifier();
	if (cpu != ECpuArmV4 && cpu != ECpuArmV5)
		return KErrNone;	// if not ARM, leave EPT as 0
	TUint epOffset = iHdr->iEntryPoint;
	if (epOffset & 3)
		return KErrNone;	// if entry point not 4 byte aligned, must be old style
	TUint fileOffset = epOffset + iHdr->iCodeOffset;
	if (fileOffset+4 > (TUint)iSize)
		return KErrCorrupt;	// entry point is past the end of the file??
	TInt ept = 0;			// old style if first instruction not recognised
	unsigned char* p = (unsigned char*)iData + fileOffset + 4;
	TUint32 x = *--p;
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
		return KErrNotSupported;
	iHdr->iFlags |= (ept<<KImageEptShift);
	return KErrNone;
	}

