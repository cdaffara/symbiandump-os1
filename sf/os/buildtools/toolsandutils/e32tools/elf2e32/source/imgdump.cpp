// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the e32 image dump for the elf2e32 tool
// @internalComponent
// @released
// 
//

#define __REFERENCE_CAPABILITY_NAMES__

#include "e32imagefile.h"
#include "pl_common.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __LINUX__ 
    #define VSNPRINTF vsnprintf
#else 
    #define VSNPRINTF _vsnprintf
#endif 

using std::cout;

const TInt KMaxStringLength=0x400;

/**
Variadic Function to print string.

@internalComponent
@released

@param aFmt
Formatted string.
*/
void PrintString(const char *aFmt,...)
{
	TText imageText[KMaxStringLength];
	va_list list;
	va_start(list,aFmt);
	VSNPRINTF((char *)imageText,KMaxStringLength,aFmt,list);
	va_end(list);
	cout << imageText;

	cout.flush();
}

/**
Function to set priority.

@internalComponent
@released

@param aPri
Priority Type passed in
@param aStr
The priority value corresponding to the appropriate priority type
*/
void PriorityToStr(TProcessPriority aPri, char *aStr)
{
	if (aPri==EPrioritySupervisor)
		strcpy(aStr,"Supervisor");

	else if (aPri>EPriorityRealTimeServer)
		sprintf(aStr, "RealTime+%d", aPri-EPriorityRealTimeServer);
	else if (aPri==EPriorityRealTimeServer)
		strcpy(aStr,"RealTime");

	else if (aPri>EPriorityFileServer)
		sprintf(aStr, "FileServer+%d", aPri-EPriorityFileServer);
	else if (aPri==EPriorityFileServer)
		strcpy(aStr,"FileServer");

	else if (aPri>EPriorityWindowServer)
		sprintf(aStr, "WindowServer+%d", aPri-EPriorityWindowServer);
	else if (aPri==EPriorityWindowServer)
		strcpy(aStr,"WindowServer");

	else if (aPri>EPriorityHigh)
		sprintf(aStr, "High+%d", aPri-EPriorityHigh);
	else if (aPri==EPriorityHigh)
		strcpy(aStr,"High");

	else if (aPri>EPriorityForeground)
		sprintf(aStr, "Foreground+%d", aPri-EPriorityForeground);
	else if (aPri==EPriorityForeground)
		strcpy(aStr,"Foreground");

	else if (aPri>EPriorityBackground)
		sprintf(aStr, "Background+%d", aPri-EPriorityBackground);
	else if (aPri==EPriorityBackground)
		strcpy(aStr,"Background");

	else if (aPri>EPriorityLow)
		sprintf(aStr, "Low+%d", aPri-EPriorityLow);
	else if (aPri==EPriorityLow)
		strcpy(aStr,"Low");

	else
		sprintf(aStr, "Illegal (%d)", aPri);
}

/**
Function to dump e32 image.

@internalComponent
@released

@param aFileName
Name of the E32image to be dumped.
@param aDumpFlags
sub options passed to the 'dump' option
*/
void E32ImageFile::Dump(TText *aFileName,TInt aDumpFlags)
{
	PrintString("E32ImageFile '%s'\n", aFileName);
	DumpHeader(aDumpFlags);
	DumpData(aDumpFlags);
}

/**
Function to dump e32 image header.

@internalComponent
@released

@param aDumpFlags
The flags set based on the sub options provided to the program.
*/
void E32ImageFile::DumpHeader(TInt aDumpFlags)
{
	TUint flags = iOrigHdr->iFlags;
	TUint abi = E32ImageHeader::ABIFromFlags(flags);
	TUint hdrfmt = E32ImageHeader::HdrFmtFromFlags(flags);
	TUint impfmt = E32ImageHeader::ImpFmtFromFlags(flags);
	TUint ept = E32ImageHeader::EptFromFlags(flags);
	TBool isARM = EFalse;

	if(aDumpFlags&EDumpHeader)
	{
		PrintString("V%d.%02d(%03d)", iOrigHdr->iToolsVersion.iMajor,iOrigHdr->iToolsVersion.iMinor,iOrigHdr->iToolsVersion.iBuild);
		PrintString("\tTime Stamp: %08x,%08x\n", iOrigHdr->iTimeHi, iOrigHdr->iTimeLo);
		char sig[5];
		memcpy(sig, (const char*)&iOrigHdr->iSignature, 4);
		sig[4]=0;
		PrintString(sig);
		if (iOrigHdr->iFlags&KImageDll)
			PrintString(" Dll for ");
		else
			PrintString(" Exe for ");
		switch (iOrigHdr->CpuIdentifier())
		{
		case ECpuX86:
			PrintString("X86 CPU\n");
			break;
		case ECpuArmV4:
			isARM = ETrue;
			PrintString("ARMV4 CPU\n");
			break;
		case ECpuArmV5:
			isARM = ETrue;
			PrintString("ARMV5 CPU\n");
			break;
		case ECpuMCore:
			PrintString("M*Core CPU\n");
			break;
		case ECpuUnknown:
			PrintString("Unknown CPU\n");
			break;
		default:
			PrintString("something or other\n");
			break;
		}
		
		PrintString("Flags:\t%08x\n", flags);

		if (!(flags & KImageDll))
		{
			char str[80];
			PriorityToStr(iOrigHdr->ProcessPriority(), str);
			PrintString("Priority %s\n", str);
			if (flags & KImageFixedAddressExe)
				PrintString("Fixed process\n");
		}
		
		if (flags & KImageNoCallEntryPoint)
			PrintString("Entry points are not called\n");
		
		PrintString("Image header is format %d\n", hdrfmt>>24);
		TUint compression = iOrigHdr->CompressionType();
		
		switch (compression)
		{
		case KFormatNotCompressed:
			PrintString("Image is not compressed\n");
			break;
		case KUidCompressionDeflate:
			PrintString("Image is compressed using the DEFLATE algorithm\n");
			break;
		case KUidCompressionBytePair:
			PrintString("Image is compressed using the BYTEPAIR algorithm\n");
			break;
		default:
			PrintString("Image compression type UNKNOWN (%08x)\n", compression);
		}
		
		if (compression)
		{
			PrintString("Uncompressed size %08x\n", iOrigHdr->UncompressedFileSize());
		}

		TUint FPU = flags & KImageHWFloatMask;

		if (FPU == KImageHWFloat_None)
			PrintString("Image FPU support : Soft VFP\n");
		else if (FPU == KImageHWFloat_VFPv2) 
			PrintString("Image FPU support : VFPv2\n");
		else
			PrintString("Image FPU support : Unknown\n");

		// Code paging.

		if (flags & KImageCodeUnpaged)
		{
			PrintString("Code Paging : Unpaged\n");
		}
		else if (flags & KImageCodePaged)
		{
			PrintString("Code Paging : Paged\n");
		}
		else
		{
			PrintString("Code Paging : Default\n");
		}

		// Data paging.

		if (flags & KImageDataUnpaged)
		{
			PrintString("Data Paging : Unpaged\n");
		}
		else if (flags & KImageDataPaged)
		{
			PrintString("Data Paging : Paged\n");
		}
		else
		{
			PrintString("Data Paging : Default\n");
		}

		if (iOrigHdr->iFlags & KImageDebuggable)
		{
			PrintString("Debuggable : True\n");
		}
		else
		{
			PrintString("Debuggable : False\n");
		}
		if (iOrigHdr->iFlags & KImageSMPSafe)
		{
			PrintString("SMP Safe : True\n");
		}
		else
		{
			PrintString("SMP Safe : False\n");
		}
	}

	if (hdrfmt >= KImageHdrFmt_V && (aDumpFlags&(EDumpHeader|EDumpSecurityInfo)))
	{
		//
		// Important. Don't change output format of following security info
		// because this is relied on by used by "Symbian Signed".
		//
		E32ImageHeaderV* v = iHdr;
		PrintString("Secure ID: %08x\n", v->iS.iSecureId);
		PrintString("Vendor ID: %08x\n", v->iS.iVendorId);
		PrintString("Capabilities: %08x %08x\n", v->iS.iCaps[1], v->iS.iCaps[0]);
		if(aDumpFlags&EDumpSecurityInfo)
		{
			TInt i;
			for(i=0; i<ECapability_Limit; i++)
				if(v->iS.iCaps[i>>5]&(1<<(i&31)))
					PrintString("              %s\n", CapabilityNames[i]);
			PrintString("\n");
		}
	}

	if(aDumpFlags&EDumpHeader)
	{
		if (hdrfmt >= KImageHdrFmt_V)
		{
			E32ImageHeaderV* v = iHdr;
			TUint32 xd = v->iExceptionDescriptor;
			if ((xd & 1) && (xd != 0xffffffffu))
			{
				xd &= ~1;
				PrintString("Exception Descriptor Offset:  %08x\n", v->iExceptionDescriptor);
				TExceptionDescriptor * aED = (TExceptionDescriptor * )(iData + v->iCodeOffset + xd);
				PrintString("Exception Index Table Base: %08x\n", aED->iExIdxBase);
				PrintString("Exception Index Table Limit: %08x\n", aED->iExIdxLimit);
				PrintString("RO Segment Base: %08x\n", aED->iROSegmentBase);
				PrintString("RO Segment Limit: %08x\n", aED->iROSegmentLimit);
			}
			else
				PrintString("No Exception Descriptor\n");

			PrintString("Export Description: Size=%03x, Type=%02x\n", v->iExportDescSize, v->iExportDescType);
			
			if (v->iExportDescType != KImageHdr_ExpD_NoHoles)
			{
				TInt nb = v->iExportDescSize;
				TInt i;
				TInt j = 0;
				for (i=0; i<nb; ++i)
				{
					if (++j == 8)
					{
						j = 0;
						PrintString("\n");
					}
					PrintString(" %02x", v->iExportDesc[i]);
				}
				PrintString("\n");
			}
			
			TInt r = CheckExportDescription();
			
			if (r == KErrNone)
				PrintString("Export description consistent\n");
			else if (r == KErrNotSupported)
				PrintString("Export description type not recognised\n");
			else
				PrintString("!! Export description inconsistent !!\n");
		}

		TUint32 mv = iOrigHdr->ModuleVersion();
		PrintString("Module Version: %d.%d\n", mv>>16, mv&0xffff);
		
		if (impfmt == KImageImpFmt_PE)
		{
			PrintString("Imports are PE-style\n");
		}
		else if (impfmt == KImageImpFmt_ELF)
		{
			PrintString("Imports are ELF-style\n");
		}
		else if (impfmt == KImageImpFmt_PE2)
		{
			PrintString("Imports are PE-style without redundant ordinal lists\n");
		}
		
		if (isARM)
		{
			if (abi == KImageABI_GCC98r2)
			{
				PrintString("GCC98r2 ABI\n");
			}
			else if (abi == KImageABI_EABI)
			{
				PrintString("ARM EABI\n");
			}
			if (ept == KImageEpt_Eka1)
			{
				PrintString("Built against EKA1\n");
			}
			else if (ept == KImageEpt_Eka2)
			{
				PrintString("Built against EKA2\n");
			}
		}

		PrintString("Uids:\t\t%08x %08x %08x (%08x)\n", iOrigHdr->iUid1, iOrigHdr->iUid2, iOrigHdr->iUid3, iOrigHdr->iUidChecksum);
		
		if (hdrfmt >= KImageHdrFmt_V)
			PrintString("Header CRC:\t%08x\n", iHdr->iHeaderCrc);
		
		PrintString("File Size:\t%08x\n", iSize);
		PrintString("Code Size:\t%08x\n", iOrigHdr->iCodeSize);
		PrintString("Data Size:\t%08x\n", iOrigHdr->iDataSize);
		PrintString("Compression:\t%08x\n", iOrigHdr->iCompressionType);
		PrintString("Min Heap Size:\t%08x\n", iOrigHdr->iHeapSizeMin);
		PrintString("Max Heap Size:\t%08x\n", iOrigHdr->iHeapSizeMax);
		PrintString("Stack Size:\t%08x\n", iOrigHdr->iStackSize);
		PrintString("Code link addr:\t%08x\n", iOrigHdr->iCodeBase);
		PrintString("Data link addr:\t%08x\n", iOrigHdr->iDataBase);
		PrintString("Code reloc offset:\t%08x\n", OrigCodeRelocOffset());
		PrintString("Data reloc offset:\t%08x\n", OrigDataRelocOffset());
		PrintString("Dll ref table count: %d\n", iOrigHdr->iDllRefTableCount);

		if (iOrigHdr->iCodeSize || iOrigHdr->iDataSize || iOrigHdr->iBssSize || iOrigHdr->iImportOffset)
			PrintString("        Offset  Size  Relocs #Relocs\n");

		PrintString("Code    %06x %06x", OrigCodeOffset(), iOrigHdr->iCodeSize);
		
		if (iOrigHdr->iCodeRelocOffset)
		{
			E32RelocSection *r=(E32RelocSection *)(iData + iOrigHdr->iCodeRelocOffset);
			PrintString(" %06x %06x", OrigCodeRelocOffset(), r->iNumberOfRelocs);
		}
		else
			PrintString("              ");
		
		PrintString("        +%06x (entry pnt)", iOrigHdr->iEntryPoint);
		PrintString("\n");

		PrintString("Data    %06x %06x", OrigDataOffset(), iOrigHdr->iDataSize);
		
		if (iOrigHdr->iDataRelocOffset)
		{
			E32RelocSection *r=(E32RelocSection *)(iData + iOrigHdr->iDataRelocOffset);
			PrintString(" %06x %06x", OrigDataRelocOffset(), r->iNumberOfRelocs);
		}
		PrintString("\n");

		PrintString("Bss            %06x\n", iOrigHdr->iBssSize);

		if (iOrigHdr->iExportDirOffset)
			PrintString("Export  %06x %06x                      (%d entries)\n", OrigExportDirOffset(), iOrigHdr->iExportDirCount*4, iOrigHdr->iExportDirCount);
		
		if (iOrigHdr->iImportOffset)
			PrintString("Import  %06x\n", OrigImportOffset());
	}
}

/**
Function to dump e32 image.

@internalComponent
@released

@param aData
Data to be dumped
@param aLength
Length of the file
*/
void dump(TUint *aData, TInt aLength)
{
	TUint *p=aData;
	TInt i=0;
	char line[256];
	char *cp=(char*)aData;
	TInt j=0;
	memset(line,' ',sizeof(line));
	while (i<aLength)
	{
		TInt ccount=0;
		char* linep=&line[8*9+2];
		PrintString("%06x:", i);
		while (i<aLength && ccount<8)
		{
			PrintString(" %08x", *p++);
			i+=4;
			ccount++;
			for (j=0; j<4; j++)
			{
				char c=*cp++;
				if (c<32 || c>127)
				{
					c = '.';
				}
				*linep++ = c;
			}
		}
		*linep='\0';
		PrintString("%s", line+(ccount*9));
		PrintString("\n");
	}
}

/**
Function to dump relocations.

@internalComponent
@released

@param aRelocs
Character pointer to relocations.
*/
void dumprelocs(char *aRelocs)
{

	TInt num=((E32RelocSection *)aRelocs)->iNumberOfRelocs;
	PrintString("%d relocs\n", num);
	aRelocs+=sizeof(E32RelocSection);
	TInt printed=0;
	while (num>0)
	{
		TInt page=*(TUint *)aRelocs;
		TInt size=*(TUint *)(aRelocs+4);
		TInt pagesize=size;
		size-=8;
		TUint16 *p=(TUint16 *)(aRelocs+8);
		while (size>0)
		{
			TUint a=*p++;
			TUint relocType = (a & 0x3000) >> 12;
			if ((relocType == 1) || (relocType == 3)) //only relocation type1 and type3
			{
				PrintString("%08x(%d) ", page + (a&0x0fff), relocType);
				printed++;
				if (printed>3)
				{
					PrintString("\n");
					printed=0;
				}
			}
			size-=2;
			num--;
		}
		aRelocs+=pagesize;
	}
	PrintString("\n");
}

/**
Function to dump e32 image data.

@internalComponent
@released

@param aDumpFlags
The flags set based on the sub options provided to the program.
*/
void E32ImageFile::DumpData(TInt aDumpFlags)
{
	if(aDumpFlags&EDumpCode)
	{
		PrintString("\nCode (text size=%08x)\n", iOrigHdr->iTextSize);
		dump((TUint *)(iData + iOrigHdr->iCodeOffset), iOrigHdr->iCodeSize);
	
		if (iOrigHdr->iCodeRelocOffset)
			dumprelocs(iData + iOrigHdr->iCodeRelocOffset);
	}

	if((aDumpFlags&EDumpData) && iOrigHdr->iDataOffset)
	{
		PrintString("\nData\n");
		dump((TUint *)(iData + iOrigHdr->iDataOffset), iOrigHdr->iDataSize);
	
		if (iOrigHdr->iDataRelocOffset)
			dumprelocs(iData + iOrigHdr->iDataRelocOffset);
	}

	if(aDumpFlags&EDumpExports)
	{
		PrintString("\nNumber of exports = %d\n", iOrigHdr->iExportDirCount);
		TInt i;
		TUint* exports = (TUint*)(iData + iOrigHdr->iExportDirOffset);
		TUint absoluteEntryPoint = iOrigHdr->iEntryPoint + iOrigHdr->iCodeBase;
		TUint impfmt = iOrigHdr->ImportFormat();
		TUint absentVal = (impfmt == KImageImpFmt_ELF) ? absoluteEntryPoint : iOrigHdr->iEntryPoint;
		for (i=0; i<iOrigHdr->iExportDirCount; ++i)
		{
			TUint exp = exports[i];
			if (exp == absentVal)
				PrintString("\tOrdinal %5d:\tABSENT\n", i+1);
			else
				PrintString("\tOrdinal %5d:\t%08x\n", i+1, exp);
		}
	}

	// Important. Don't change output format of following inport info
	// because this is relied on by tools used by "Symbian Signed".
	if((aDumpFlags&EDumpImports) && iOrigHdr->iImportOffset)
	{
		const E32ImportSection* isection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
		TUint* iat = (TUint*)((TUint8*)iData + iOrigHdr->iCodeOffset + iOrigHdr->iTextSize);
		PrintString("\nIdata\tSize=%08x\n", isection->iSize);
		PrintString("Offset of import address table (relative to code section): %08x\n", iOrigHdr->iTextSize);
		TInt d;
		const E32ImportBlock* b = (const E32ImportBlock*)(isection + 1);
		for (d=0; d<iOrigHdr->iDllRefTableCount; d++)
		{
			char* dllname = iData + iOrigHdr->iImportOffset + b->iOffsetOfDllName;
			TInt n = b->iNumberOfImports;
			PrintString("%d imports from %s\n", b->iNumberOfImports, dllname);
			const TUint* p = b->Imports();
			TUint impfmt = iOrigHdr->ImportFormat();
			if (impfmt == KImageImpFmt_ELF)
			{
				while (n--)
				{
					TUint impd_offset = *p++;
					TUint impd = *(TUint*)(iData + iOrigHdr->iCodeOffset + impd_offset);
					TUint ordinal = impd & 0xffff;
					TUint offset = impd >> 16;
				
					if (offset)
						PrintString("%10d offset by %d\n", ordinal, offset);
					else
						PrintString("%10d\n", ordinal);
				}
			}
			else
			{
				while (n--)
					PrintString("\t%d\n", *iat++);
			}
			b = b->NextBlock(impfmt);
		}
	}
	if((aDumpFlags & EDumpSymbols) && (iOrigHdr->iFlags & KImageNmdExpData))
	{
		TUint* aExpTbl = (TUint*)(iData + iOrigHdr->iExportDirOffset);
		TUint* aZeroethOrd = aExpTbl - 1;

		E32EpocExpSymInfoHdr *aSymInfoHdr = (E32EpocExpSymInfoHdr*)(iData + \
					iOrigHdr->iCodeOffset + \
					*aZeroethOrd - iOrigHdr->iCodeBase ) ;
		DumpSymbolInfo(aSymInfoHdr);
	}
}

void E32ImageFile::DumpSymbolInfo(E32EpocExpSymInfoHdr *aSymInfoHdr)
{
	if(!aSymInfoHdr)
		return;
	char	*aSymTblBase = (char*)aSymInfoHdr;
	TUint	*aSymAddrTbl;
	char	*aSymNameTbl;

	char *aStrTable = aSymTblBase + aSymInfoHdr->iStringTableOffset;
	aSymAddrTbl = (TUint*)(aSymTblBase + aSymInfoHdr->iSymbolTblOffset);
	aSymNameTbl = (char*)(aSymAddrTbl + aSymInfoHdr->iSymCount);
	
	int aIdx;
	char *aSymName;

	PrintString("\n\n\t\tSymbol Info\n");
	if(aSymInfoHdr->iSymCount)
	{
		PrintString("0x%x Symbols exported\n",aSymInfoHdr->iSymCount);
		PrintString("  Addr\t\tName\n");
		PrintString("----------------------------------------\n");
		TUint aNameOffset = 0;
		for(aIdx=0;aIdx<aSymInfoHdr->iSymCount ; aIdx++)
		{
			if(aSymInfoHdr->iFlags & 1)
			{
				TUint32* aOff = ((TUint32*)aSymNameTbl+aIdx);
				aNameOffset = (*aOff) << 2;
				aSymName = aStrTable + aNameOffset;
			}
			else
			{
				TUint16* aOff = ((TUint16*)aSymNameTbl+aIdx);
				aNameOffset = (*aOff) << 2;
				aSymName = aStrTable + aNameOffset;
			}
			PrintString("0x%08x \t%s\t\n",aSymAddrTbl[aIdx], aSymName);
		}
	}
	else
	{
		PrintString("No Symbol exported\n");
	}
	PrintString("\n\n");

	if(aSymInfoHdr->iDllCount)
	{
		// The import table orders the dependencies alphabetically...
		// We need to list out in the link order...
		PrintString("%d Static dependencies found\n", aSymInfoHdr->iDllCount);
		TUint* aDepTbl = (TUint*)((char*)aSymInfoHdr + aSymInfoHdr->iDepDllZeroOrdTableOffset);
		TUint* aDepOffset =  (TUint*)((char*)aDepTbl - iData);

		const E32ImportSection* isection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
		
		TInt d;

		/* The import table has offsets to the location (in code section) where the
		 * import is required. For dependencies pointed by 0th ordinal, this offset
		 * must be same as the offset of the dependency table entry (relative to 
		 * the code section).
		 */
		bool aZerothFound;
		for(int aDep = 0; aDep < aSymInfoHdr->iDllCount; aDep++)
		{
			const E32ImportBlock* b = (const E32ImportBlock*)(isection + 1);
			aZerothFound = false;
			for (d=0; d<iOrigHdr->iDllRefTableCount; d++)
			{
				char* dllname = iData + iOrigHdr->iImportOffset + b->iOffsetOfDllName;
				TInt n = b->iNumberOfImports;

				const TUint* p = b->Imports()+ (n - 1);//start from the end of the import table
				TUint impfmt = iOrigHdr->ImportFormat();
				if (impfmt == KImageImpFmt_ELF)
				{
					while (n--)
					{
						TUint impd_offset = *p--;
						TUint impd = *(TUint*)(iData + iOrigHdr->iCodeOffset + impd_offset);
						TUint ordinal = impd & 0xffff;
					
						if (ordinal == 0 )
						{
							if( impd_offset == ((TUint)aDepOffset - iOrigHdr->iCodeOffset))
							{
								/* The offset in import table is same as the offset of this 
								 * dependency entry
								 */
								PrintString("\t%s\n", dllname);
								aZerothFound = true;
							}
							break;
						}
					}
				}
				if(aZerothFound)
					break;

				b = b->NextBlock(impfmt);
			}
			if(!aZerothFound)
			{
				PrintString("!!Invalid dependency listed at %d\n",aDep );
			}

			aDepOffset++;
		}
	}
}
