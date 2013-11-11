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
// e32tools/e32image/imgdump.cpp
// 
//

#define __REFERENCE_CAPABILITY_NAMES__

#include "e32image.h"
#include "h_utl.h"
#include <string.h>

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

void nl()
	{
	Print(EAlways, "\n");
	}

void E32ImageFile::Dump(TText *aFileName,TInt aDumpFlags)
	{
	if (IsValid())
		{
		Print(EAlways, "E32ImageFile '%s'\n", aFileName);
		DumpHeader(aDumpFlags);
		DumpData(aDumpFlags);
		}
	else
		Print(EAlways, "This is not an E32 image file (error %d).\n", iError);
	}

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
		Print(EAlways, "V%d.%02d(%03d)", iOrigHdr->iToolsVersion.iMajor,iOrigHdr->iToolsVersion.iMinor,iOrigHdr->iToolsVersion.iBuild);
		Print(EAlways, "\tTime Stamp: %08x,%08x\n", iOrigHdr->iTimeHi, iOrigHdr->iTimeLo);
		char sig[5];
		memcpy(sig, (const char*)&iOrigHdr->iSignature, 4);
		sig[4]=0;
		Print(EAlways, sig);
		if (iOrigHdr->iFlags&KImageDll)
			Print(EAlways, " Dll for ");
		else
			Print(EAlways, " Exe for ");
		switch (iOrigHdr->CpuIdentifier())
			{
		case ECpuX86:
			Print(EAlways, "X86 CPU\n");
			break;
		case ECpuArmV4:
			isARM = ETrue;
			Print(EAlways, "ARMV4 CPU\n");
			break;
		case ECpuArmV5:
			isARM = ETrue;
			Print(EAlways, "ARMV5 CPU\n");
			break;
		case ECpuMCore:
			Print(EAlways, "M*Core CPU\n");
			break;
		case ECpuUnknown:
			Print(EAlways, "Unknown CPU\n");
			break;
		default:
			Print(EAlways, "something or other\n");
			break;
			}
		
		Print(EAlways, "Flags:\t%08x\n", flags);
	
		if (!(flags & KImageDll))
			{
			char str[80];
			PriorityToStr(iOrigHdr->ProcessPriority(), str);
			Print(EAlways, "Priority %s\n", str);
			if (flags & KImageFixedAddressExe)
				Print(EAlways, "Fixed process\n");
			}
		if (flags & KImageNoCallEntryPoint)
			Print(EAlways, "Entry points are not called\n");
		Print(EAlways, "Image header is format %d\n", hdrfmt>>24);
		TUint compression = iOrigHdr->CompressionType();
		switch (compression)
			{
			case KFormatNotCompressed:
				Print(EAlways, "Image is not compressed\n");
				break;
			case KUidCompressionDeflate:
				Print(EAlways, "Image is compressed using the DEFLATE algorithm\n");
				break;
			case KUidCompressionBytePair:
				Print(EAlways, "Image is compressed using the BYTEPAIR algorithm\n");
				break;
			default:
				Print(EAlways, "Image compression type UNKNOWN (%08x)\n", compression);
			}
		if (compression)
			{
			Print(EAlways, "Uncompressed size %08x\n", iOrigHdr->UncompressedFileSize());
			}
		
		TUint FPU = flags & KImageHWFloatMask;

		if (FPU == KImageHWFloat_None)
			Print(EAlways, "Image FPU support : Soft VFP\n");
		else if (FPU == KImageHWFloat_VFPv2) 
			Print(EAlways, "Image FPU support : VFPv2\n");
		else
			Print(EAlways, "Image FPU support : Unknown\n");

		if (flags & KImageCodeUnpaged)
			{
			Print(EAlways, "Code Paging : Unpaged\n");
			}
		else if (flags & KImageCodePaged)
			{
			Print(EAlways, "Code Paging : Paged\n");
			}
		else
			{
			Print(EAlways, "Code Paging : Default\n");
			}

		if (flags & KImageDataUnpaged)
			{
			Print(EAlways, "Data Paging : Unpaged\n");
			}
		else if (flags & KImageDataPaged)
			{
			Print(EAlways, "Data Paging : Paged\n");
			}
		else
			{
			Print(EAlways, "Data Paging : Default\n");
			}

		if(flags & KImageDebuggable)
			{
 			Print(EAlways, "Debuggable : True\n");
 			}
 		else
 			{
 			Print(EAlways, "Debuggable : False\n");
 			}

		if(flags & KImageSMPSafe)
			{
 			Print(EAlways, "SMP Safe : True\n");
 			}
 		else
 			{
 			Print(EAlways, "SMP Safe : False\n");
 			}
		}

	if (hdrfmt >= KImageHdrFmt_V && (aDumpFlags&(EDumpHeader|EDumpSecurityInfo)))
		{
		//
		// Important. Don't change output format of following security info
		// because this is relied on by used by "Symbian Signed".
		//
		E32ImageHeaderV* v = iHdr;
		Print(EAlways, "Secure ID: %08x\n", v->iS.iSecureId);
		Print(EAlways, "Vendor ID: %08x\n", v->iS.iVendorId);
		Print(EAlways, "Capabilities: %08x %08x\n", v->iS.iCaps[1], v->iS.iCaps[0]);
		if(aDumpFlags&EDumpSecurityInfo)
			{
			TInt i;
			for(i=0; i<ECapability_Limit; i++)
				if(v->iS.iCaps[i>>5]&(1<<(i&31)))
					Print(EAlways, "              %s\n", CapabilityNames[i]);
			Print(EAlways, "\n");
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
				Print(EAlways, "Exception Descriptor Offset:  %08x\n", v->iExceptionDescriptor);
				TExceptionDescriptor * aED = (TExceptionDescriptor * )(iData + v->iCodeOffset + xd);
				Print(EAlways, "Exception Index Table Base: %08x\n", aED->iExIdxBase);
				Print(EAlways, "Exception Index Table Limit: %08x\n", aED->iExIdxLimit);
				Print(EAlways, "RO Segment Base: %08x\n", aED->iROSegmentBase);
				Print(EAlways, "RO Segment Limit: %08x\n", aED->iROSegmentLimit);
				}
			else
				Print(EAlways, "No Exception Descriptor\n");

			Print(EAlways, "Export Description: Size=%03x, Type=%02x\n", v->iExportDescSize, v->iExportDescType);
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
						Print(EAlways,"\n");
						}
					Print(EAlways," %02x", v->iExportDesc[i]);
					}
				Print(EAlways,"\n");
				}
			TInt r = CheckExportDescription();
			if (r == KErrNone)
				Print(EAlways,"Export description consistent\n");
			else if (r == KErrNotSupported)
				Print(EAlways,"Export description type not recognised\n");
			else
				Print(EAlways,"!! Export description inconsistent !!\n");
			}

		TUint32 mv = iOrigHdr->ModuleVersion();
		Print(EAlways, "Module Version: %d.%d\n", mv>>16, mv&0xffff);
		if (impfmt == KImageImpFmt_PE)
			{
			Print(EAlways, "Imports are PE-style\n");
			}
		else if (impfmt == KImageImpFmt_ELF)
			{
			Print(EAlways, "Imports are ELF-style\n");
			}
		else if (impfmt == KImageImpFmt_PE2)
			{
			Print(EAlways, "Imports are PE-style without redundant ordinal lists\n");
			}
		if (isARM)
			{
			if (abi == KImageABI_GCC98r2)
				{
				Print(EAlways, "GCC98r2 ABI\n");
				}
			else if (abi == KImageABI_EABI)
				{
				Print(EAlways, "ARM EABI\n");
				}
			if (ept == KImageEpt_Eka1)
				{
				Print(EAlways, "Built against EKA1\n");
				}
			else if (ept == KImageEpt_Eka2)
				{
				Print(EAlways, "Built against EKA2\n");
				}
			}

		Print(EAlways, "Uids:\t\t%08x %08x %08x (%08x)\n", iOrigHdr->iUid1, iOrigHdr->iUid2, iOrigHdr->iUid3, iOrigHdr->iUidChecksum);
		if (hdrfmt >= KImageHdrFmt_V)
			Print(EAlways, "Header CRC:\t%08x\n", iHdr->iHeaderCrc);
		Print(EAlways, "File Size:\t%08x\n", iSize);
		Print(EAlways, "Code Size:\t%08x\n", iOrigHdr->iCodeSize);
		Print(EAlways, "Data Size:\t%08x\n", iOrigHdr->iDataSize);
		Print(EAlways, "Compression:\t%08x\n", iOrigHdr->iCompressionType);
		Print(EAlways, "Min Heap Size:\t%08x\n", iOrigHdr->iHeapSizeMin);
		Print(EAlways, "Max Heap Size:\t%08x\n", iOrigHdr->iHeapSizeMax);
		Print(EAlways, "Stack Size:\t%08x\n", iOrigHdr->iStackSize);
		Print(EAlways, "Code link addr:\t%08x\n", iOrigHdr->iCodeBase);
		Print(EAlways, "Data link addr:\t%08x\n", iOrigHdr->iDataBase);
		Print(EAlways, "Code reloc offset:\t%08x\n", OrigCodeRelocOffset());
		Print(EAlways, "Data reloc offset:\t%08x\n", OrigDataRelocOffset());
		Print(EAlways, "Dll ref table count: %d\n", iOrigHdr->iDllRefTableCount);

		if (iOrigHdr->iCodeSize || iOrigHdr->iDataSize || iOrigHdr->iBssSize || iOrigHdr->iImportOffset)
			Print(EAlways, "        Offset  Size  Relocs #Relocs\n");

		Print(EAlways, "Code    %06x %06x", OrigCodeOffset(), iOrigHdr->iCodeSize);
		if (iOrigHdr->iCodeRelocOffset)
			{
			E32RelocSection *r=(E32RelocSection *)(iData + iOrigHdr->iCodeRelocOffset);
			Print(EAlways, " %06x %06x", OrigCodeRelocOffset(), r->iNumberOfRelocs);
			}
		else
			Print(EAlways, "              ");
		Print(EAlways, "        +%06x (entry pnt)", iOrigHdr->iEntryPoint);
		nl();

		Print(EAlways, "Data    %06x %06x", OrigDataOffset(), iOrigHdr->iDataSize);
		if (iOrigHdr->iDataRelocOffset)
			{
			E32RelocSection *r=(E32RelocSection *)(iData + iOrigHdr->iDataRelocOffset);
			Print(EAlways, " %06x %06x", OrigDataRelocOffset(), r->iNumberOfRelocs);
			}
		nl();

		Print(EAlways, "Bss            %06x\n", iOrigHdr->iBssSize);

		if (iOrigHdr->iExportDirOffset)
			Print(EAlways, "Export  %06x %06x                      (%d entries)\n", OrigExportDirOffset(), iOrigHdr->iExportDirCount*4, iOrigHdr->iExportDirCount);
		if (iOrigHdr->iImportOffset)
			Print(EAlways, "Import  %06x\n", OrigImportOffset());
		}
	}

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
		Print(EAlways, "%06x:", i);
		while (i<aLength && ccount<8)
			{
			Print(EAlways," %08x", *p++);
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
		Print(EAlways, "%s", line+(ccount*9));
		nl();
		}
	}

void dumprelocs(char *aRelocs)
	{

	TInt num=((E32RelocSection *)aRelocs)->iNumberOfRelocs;
	Print(EAlways, "%d relocs\n", num);
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
			TUint relocType = a >> 12;
			if (relocType==3 || relocType==1)
				{
				Print(EAlways, "%08x(%1x) ", page+(a&0x0fff), relocType);
				printed++;
				if (printed>3)
					{
					nl();
					printed=0;
					}
				}
			size-=2;
			num--;
			}
		aRelocs+=pagesize;
		}
	nl();
	}


void E32ImageFile::DumpData(TInt aDumpFlags)
	{
	if(aDumpFlags&EDumpCode)
		{
		Print(EAlways, "\nCode (text size=%08x)\n", iOrigHdr->iTextSize);
		dump((TUint *)(iData + iOrigHdr->iCodeOffset), iOrigHdr->iCodeSize);
		if (iOrigHdr->iCodeRelocOffset)
			dumprelocs(iData + iOrigHdr->iCodeRelocOffset);
		}

	if((aDumpFlags&EDumpData) && iOrigHdr->iDataOffset)
		{
		Print(EAlways, "\nData\n");
		dump((TUint *)(iData + iOrigHdr->iDataOffset), iOrigHdr->iDataSize);
		if (iOrigHdr->iDataRelocOffset)
			dumprelocs(iData + iOrigHdr->iDataRelocOffset);
		}

	if(aDumpFlags&EDumpExports)
		{
		Print(EAlways, "\nNumber of exports = %d\n", iOrigHdr->iExportDirCount);
		TInt i;
		TUint* exports = (TUint*)(iData + iOrigHdr->iExportDirOffset);
		TUint absoluteEntryPoint = iOrigHdr->iEntryPoint + iOrigHdr->iCodeBase;
		TUint impfmt = iOrigHdr->ImportFormat();
		TUint absentVal = (impfmt == KImageImpFmt_ELF) ? absoluteEntryPoint : iOrigHdr->iEntryPoint;
		for (i=0; i<iOrigHdr->iExportDirCount; ++i)
			{
			TUint exp = exports[i];
			if (exp == absentVal)
				Print(EAlways, "\tOrdinal %5d:\tABSENT\n", i+1);
			else
				Print(EAlways, "\tOrdinal %5d:\t%08x\n", i+1, exp);
			}
		}

	//
	// Important. Don't change output format of following inport info
	// because this is relied on by tools used by "Symbian Signed".
	//
	if((aDumpFlags&EDumpImports) && iOrigHdr->iImportOffset)
		{
		const E32ImportSection* isection = (const E32ImportSection*)(iData + iOrigHdr->iImportOffset);
		TUint* iat = (TUint*)((TUint8*)iData + iOrigHdr->iCodeOffset + iOrigHdr->iTextSize);
		Print(EAlways, "\nIdata\tSize=%08x\n", isection->iSize);
		Print(EAlways, "Offset of import address table (relative to code section): %08x\n", iOrigHdr->iTextSize);
		TInt d;
		const E32ImportBlock* b = (const E32ImportBlock*)(isection + 1);
		for (d=0; d<iOrigHdr->iDllRefTableCount; d++)
			{
			char* dllname = iData + iOrigHdr->iImportOffset + b->iOffsetOfDllName;
			TInt n = b->iNumberOfImports;
			Print(EAlways, "%d imports from %s\n", b->iNumberOfImports, dllname);
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
						Print(EAlways, "%10d offset by %d\n", ordinal, offset);
					else
						Print(EAlways, "%10d\n", ordinal);
					}
				}
			else
				{
				while (n--)
					Print(EAlways, "\t%d\n", *iat++);
				}
			b = b->NextBlock(impfmt);
			}
		}
	}
