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
// e32tools/e32image/tr_main.cpp
// Translate X->E32Image top level
// 
//

#ifndef __LINUX__
  #include <io.h>
#endif
#include <string.h>

#ifdef __VC32__
 #ifdef __MSVCDOTNET__
  #include <strstream>
  #include <iomanip>
 #else //!__MSVCDOTNET__
  #include <strstrea.h>
  #include <iomanip.h>
 #endif //__MSVCDOTNET__
#else // !__VC32__*/
#ifdef __TOOLS2__
  #include <sstream>
  #include <iomanip>
  #else
 #include <strstream.h>
 #include <iomanip.h>
#endif // __VC32__
#endif

#include <e32std.h>
#include <e32std_private.h>

#ifdef __SUPPORT_PE_FILES__
#include "pe_file.h"
#endif
#ifdef __SUPPORT_ELF_FILES__
#include "elftran.h"
#endif

#include "h_utl.h"
#include "h_ver.h"
#include <stdio.h>

extern int gAlignConstSection;
extern TUint gConstSectionAddressMask;

int gVerbose=0;
char *gFile1=NULL;
char *gFile2=NULL;
unsigned int gStack=0;
unsigned int gHeapMin=0;
unsigned int gHeapMax=0;
TUid gUid1=KNullUid;
TUid gUid2=KNullUid;
TUid gUid3=KNullUid;
unsigned int gSecureId=0;
unsigned int gVendorId=0;
unsigned int gVersionWord=0x00010000u;
int gCallEntryPoints=TRUE;
int gFixedAddress=FALSE;
int gPriority=EPriorityForeground;
SCapabilitySet gCapability={0};
int gAllowDllData=FALSE;
// fix warning for Linux warning: 0 instead of NULL
TUint gDataBase=0;
int gCompress=TRUE;
unsigned int gFPU=0;

int gCodePaged=FALSE;
int gCodeUnpaged=FALSE;
int gCodeDefaultPaged=FALSE;

int gDataPaged=FALSE;
int gDataUnpaged=FALSE;
int gDataDefaultPaged=FALSE;

int gDebuggable=FALSE;
int gSmpSafe=FALSE;

int gSetStack=FALSE;
int gSetHeap=FALSE;
int gSetUid1=FALSE;
int gSetUid2=FALSE;
int gSetUid3=FALSE;
int gSetCallEntryPoints=FALSE;
int gSetFixedAddress=FALSE;
int gSetPriority=FALSE;
int gSetCapability=FALSE;
int gSetCompress=FALSE;
int gSetVersion=FALSE;
int gSetSecureId=FALSE;
int gSetVendorId=FALSE;
int gSetFPU=FALSE;

int gSetCodePaged=FALSE;
int gSetDataPaged=FALSE;

int gSetSymLkup=FALSE;
int gSetDebuggable=FALSE;
int gSetSmpSafe=FALSE;

enum CompressionMethods
{
	ENoCompression = 0,
	EDeflate = 1,
	EBytePair = 2,
};

int gCompressionMethod = EDeflate;
int gSuppressComprMethod = FALSE;

#ifdef __SUPPORT_PE_FILES__
char* gX86imp=NULL;
int gX86num_imp_dlls=0;
int gX86imp_size=0;
int gX86num_imports=0;
#endif

TBool gLittleEndian=ETrue;

class E32ImageFileRef
	{
public:
	E32ImageFileRef() {iPtr = E32ImageFile::New();}
	~E32ImageFileRef() {delete iPtr;}
	class E32ImageFile& Ref() {return *iPtr;}
private:
	E32ImageFileRef(const E32ImageFileRef&);
	E32ImageFileRef& operator=(const E32ImageFileRef&);
private:
	E32ImageFile* iPtr;
	};

int setPagedFlags(E32ImageFile& f)
	{
	unsigned check1 = gCodePaged + gCodeUnpaged + gCodeDefaultPaged;
	unsigned check2 = gDataPaged + gDataUnpaged + gDataDefaultPaged;

	if (check1 > 1 || check2 > 1)
		{
		Print(EError, "Conflicting paging options.\n");
		return KErrArgument;
		}

	if (gCodePaged)
		{
		f.iHdr->iFlags |= KImageCodePaged;
		f.iHdr->iFlags &= ~KImageCodeUnpaged;
		}
	else if (gCodeUnpaged)
		{
		f.iHdr->iFlags |= KImageCodeUnpaged;
		f.iHdr->iFlags &= ~KImageCodePaged;
		}
	else if (gCodeDefaultPaged)
		{
		f.iHdr->iFlags &= ~KImageCodePaged;
		f.iHdr->iFlags &= ~KImageCodeUnpaged;
		}

	if (gDataPaged)
		{
		f.iHdr->iFlags |=  KImageDataPaged;
		f.iHdr->iFlags &= ~KImageDataUnpaged;
		}
	else if (gDataUnpaged)
		{
		f.iHdr->iFlags |=  KImageDataUnpaged;
		f.iHdr->iFlags &= ~KImageDataPaged;
		}
	else if (gDataDefaultPaged)
		{
		f.iHdr->iFlags &= ~KImageDataPaged;
		f.iHdr->iFlags &= ~KImageDataUnpaged;
		}

	return KErrNone;
	}

void setDebuggableFlags(E32ImageFile& f)
	{
	if (gDebuggable)
		{
		f.iHdr->iFlags |= KImageDebuggable;
		}
	else
		{
		f.iHdr->iFlags &= ~KImageDebuggable;
		}
	}

void setSmpSafeFlags(E32ImageFile& f)
	{
	if (gSmpSafe)
		{
		f.iHdr->iFlags |= KImageSMPSafe;
		}
	else
		{
		f.iHdr->iFlags &= ~KImageSMPSafe;
		}
	}

int dotran(const char* ifilename, const char* ofilename)
	{
	E32ImageFileRef fRef;
	E32ImageFile& f = fRef.Ref();
	int r=f.Translate(ifilename, gDataBase, gAllowDllData, gSetSymLkup);
	if (r!=KErrNone)
		return r;
	if (gSetStack)
		f.SetStackSize(gStack);
	if (gSetHeap)
		{
		f.SetHeapSizeMin(gHeapMin);
		f.SetHeapSizeMax(gHeapMax);
		}
	if (!gSetUid1)
		gUid1=TUid::Uid(f.iHdr->iUid1);
	if (!gSetUid2)
		gUid2=TUid::Uid(f.iHdr->iUid2);
	if (!gSetUid3)
		gUid3=TUid::Uid(f.iHdr->iUid3);
	if (!gSetSecureId)
		gSecureId = f.iHdr->iUid3;
	if (!gSetVendorId)
		gVendorId = 0;
	f.SetUids(gUid1, gUid2, gUid3);
	f.SetSecureId(gSecureId);
	f.SetVendorId(gVendorId);
	if (gSetCallEntryPoints)
		f.SetCallEntryPoints(gCallEntryPoints);
	if (gSetCapability)
		f.SetCapability(gCapability);
	if (gSetPriority)
		{
		if (f.iHdr->iFlags&KImageDll)
			Print(EWarning,"Cannot set priority of a DLL.\n");
		else
			f.SetPriority((TProcessPriority)gPriority);
		}
	if (gSetFixedAddress)
		{
		if (f.iHdr->iFlags&KImageDll)
			Print(EWarning,"Cannot set fixed address for DLL.\n");
		else
			f.SetFixedAddress(gFixedAddress);
		}
	if (gSetVersion)
		f.iHdr->iModuleVersion = gVersionWord;

	if(gCompress)
	{
		switch(gCompressionMethod)
		{
			case ENoCompression:
				f.iHdr->iCompressionType = KFormatNotCompressed;
				break;
				
			case EDeflate:
				f.iHdr->iCompressionType = KUidCompressionDeflate;
			
				break;
			
			case EBytePair:
				f.iHdr->iCompressionType = KUidCompressionBytePair;
			
				break;
			
			default:
				Print(EError, "Unknown compression method:%d", gCompressionMethod);
				return 1;
			
		} // End of switch()
		
	}
	else
	{
		f.iHdr->iCompressionType = KFormatNotCompressed;		
	}
		

	if (gSetFPU)
		f.SetFPU(gFPU);

	r = setPagedFlags(f);
	if (r != KErrNone)
	{
		return r;
	}

	setDebuggableFlags(f);

	setSmpSafeFlags(f);

	f.CreateExportBitMap();
	f.AddExportDescription();
	f.UpdateHeaderCrc();
	r = f.Validate();
	if (r!=KErrNone)
		return r;

	ofstream ofile(ofilename, ios::binary);
	if (!ofile)
		{
		Print(EError,"Cannot open %s for output.\n",ofilename);
		return 1;
		}
	ofile << f;
	ofile.close();
	if (gVerbose)
		f.Dump((TText*)ofilename,gVerbose);
	return KErrNone;
	}


int dodump(const char* ifilename)
	{
	E32ImageFile f;
	TInt r = f.Open(ifilename);
	if (r>0)
		return 1;
	else if (r==KErrCorrupt || r==KErrNotSupported)
		{
		Print(EError,"%s is not a valid E32Image file.\n",ifilename);
		return 1;
		}
	else if (r!=0)
		{
		Print(EError,"Error %d reading %s.\n",r,ifilename);
		return 1;
		}
	f.Dump((TText*)ifilename, gVerbose ? gVerbose : E32ImageFile::EDumpDefaults);
	return KErrNone;
	}

int doalter(const char* ifilename)
	{
	E32ImageFile f;
	TInt r = f.Open(ifilename);
	if (r>0)
		return 1;
	else if (r==KErrCorrupt || r==KErrNotSupported)
		{
		Print(EError,"%s is not a valid E32Image file.\n",ifilename);
		return 1;
		}
	else if (r!=0)
		{
		Print(EError,"Error %d reading %s.\n",r,ifilename);
		return 1;
		}

	TUint hdrfmt = f.iHdr->HeaderFormat();
	if (hdrfmt != KImageHdrFmt_V)
		{
		Print(EError,"Can't modify old format binaries\n");
		return 1;
		}

	if (gDataBase)
		{
		Print(EWarning, "Ignoring -datalinkaddress Switch");
		}
	if (gSetStack)
		f.SetStackSize(gStack);
	if (gSetHeap)
		{
		f.SetHeapSizeMin(gHeapMin);
		f.SetHeapSizeMax(gHeapMax);
		}
	if (!gSetUid1)
		gUid1=TUid::Uid(f.iHdr->iUid1);
	if (!gSetUid2)
		gUid2=TUid::Uid(f.iHdr->iUid2);
	if (!gSetUid3)
		gUid3=TUid::Uid(f.iHdr->iUid3);
	f.SetUids(gUid1, gUid2, gUid3);
	if (gSetSecureId)
		f.SetSecureId(gSecureId);
	if (gSetVendorId)
		f.SetVendorId(gVendorId);
	if (gSetCallEntryPoints)
		f.SetCallEntryPoints(gCallEntryPoints);
	if (gSetCapability)
		f.SetCapability(gCapability);
	if (gSetPriority)
		{
		if (f.iHdr->iFlags&KImageDll)
			Print(EWarning,"Cannot set priority of a DLL.\n");
		else
			f.SetPriority((TProcessPriority)gPriority);
		}
	if (gSetFixedAddress)
		{
		if (f.iHdr->iFlags&KImageDll)
			Print(EWarning,"Cannot set fixed address for DLL.\n");
		else
			f.SetFixedAddress(gFixedAddress);
		}
	if (gSetVersion)
		f.iHdr->iModuleVersion = gVersionWord;

	if(gCompress)
	{
		switch(gCompressionMethod)
		{
			case ENoCompression:
				f.iHdr->iCompressionType = KFormatNotCompressed;
				break;
				
			case EDeflate:
				f.iHdr->iCompressionType = KUidCompressionDeflate;
			
				break;
			
			case EBytePair:
				f.iHdr->iCompressionType = KUidCompressionBytePair;
			
				break;
			
			default:
				Print(EError, "Unknown compression method:%d", gCompressionMethod);
				return 1;
			
		} // End of switch()
	}
	else
		f.iHdr->iCompressionType = KFormatNotCompressed;

	if (gSetFPU)
		f.SetFPU(gFPU);

	r = setPagedFlags(f);
	if (r != KErrNone)
	{
		return r;
	}
	
	setDebuggableFlags(f);

	setSmpSafeFlags(f);

	f.UpdateHeaderCrc();
	r = f.Validate();
	if (r!=KErrNone)
		return r;

	ofstream ofile(ifilename, ios::binary);
	if (!ofile)
		{
		Print(EError,"Cannot open %s for output.\n",ifilename);
		return 1;
		}
	ofile << f;
	ofile.close();
	if (gVerbose)
		f.Dump((TText *)ifilename,gVerbose);
	return KErrNone;
	}

int helpme(char *aStr)
	{
	Print(EAlways,"Syntax: %s [options] inputfile outputfile\n",aStr);
	Print(EAlways,"        %s [options] e32imagefile\n",aStr);
	Print(EAlways,"option: [-v] [[-no]call[entrypoint]] [-priority <priority>]\n");
	Print(EAlways,"        [-stack <size>] [-heap <min> <max>] [-uid<n> <uid>]\n");
	Print(EAlways,"        [-allowdlldata] [-datalinkaddress <base>] [-fixed] [-moving]\n");
	Print(EAlways,"        [-align-const-section] [-const-section-address-mask <mask>]\n");
	Print(EAlways,"        [-[no]compress] [-compressionmethod none|deflate|bytepair]\n");
	Print(EAlways,"        [-capability \"<list>\"] [-version M.m] [-vid <id>]\n");
	Print(EAlways,"        [-fpu <softvfp|vfpv2>]\n");
	Print(EAlways,"        [-codepaging <paged|unpaged|default>]\n");
	Print(EAlways,"        [-datapaging <paged|unpaged|default>]\n");
	Print(EAlways,"        [-debuggable]\n");
	Print(EAlways,"        [-smpsafe]\n");
	Print(EAlways,"        [-sym_name_lkup]\n");
	Print(EAlways,"        [-dump [h][s][c][d][e][i]]\n");
	Print(EAlways,"flags for dump: h Header\n");
	Print(EAlways,"                s Security info\n");
	Print(EAlways,"                c Code section\n");
	Print(EAlways,"                d Data section\n");
	Print(EAlways,"                e Export info\n");
	Print(EAlways,"                i Import table\n");
	return KErrArgument;
	}

int isNumber(char *aStr)
	{
	return (aStr[0]>='0') && (aStr[0]<='9');
	}

int getUIntArg(unsigned int &aVal, int argc, char *argv[], int i)
	{
	if (i>=argc)
		return KErrArgument;
	if (!isNumber(argv[i]))
		return KErrArgument;
#ifdef __LINUX__
	int n;
	sscanf(argv[i], "%i", &n);
	aVal = n;
#else
#ifdef __TOOLS2__
istringstream s(argv[i]/*, strlen(argv[i])*/);
#else
istrstream s(argv[i], strlen(argv[i]));
#endif

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	s >> setbase(0);
#endif //__MSVCDOTNET__

	s >> aVal;
#endif // __LINUX__
	return KErrNone;
	}

int getCapabilitiesArg(SCapabilitySet& aVal, int argc, char *argv[], int i)
	{
	memset(&aVal,0,sizeof(aVal));
	if (i>=argc)
		return KErrArgument;
	if (isNumber(argv[i]))
		return getUIntArg(*(TUint*)&aVal[0], argc, argv, i);
	return ParseCapabilitiesArg(aVal,argv[i]);
	}

int getPriorityArg(int &aVal, int argc, char *argv[], int i)
	{

	if (i>=argc)
		return KErrArgument;
	if (isNumber(argv[i]))
		{
#ifdef __LINUX__
		int n;
		sscanf(argv[i], "%i", &n);
		aVal = n;
#else
#ifdef __TOOLS2__
istringstream s(argv[i]);
#else
istrstream s(argv[i], strlen(argv[i]));
#endif

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
		s >> setbase(0);
#endif //__MSVCDOTNET__

		s>>aVal;
#endif // __LINUX__
		}
	else
		{
		if (stricmp(argv[i], "low")==0)
			aVal=EPriorityLow;
		else if (strnicmp(argv[i], "background",4)==0)
			aVal=EPriorityBackground;
		else if (strnicmp(argv[i], "foreground",4)==0)
			aVal=EPriorityForeground;
		else if (stricmp(argv[i], "high")==0)
			aVal=EPriorityHigh;
		else if (strnicmp(argv[i], "windowserver",3)==0)
			aVal=EPriorityWindowServer;
		else if (strnicmp(argv[i], "fileserver",4)==0)
			aVal=EPriorityFileServer;
		else if (strnicmp(argv[i], "realtime",4)==0)
			aVal=EPriorityRealTimeServer;
		else if (strnicmp(argv[i], "supervisor",3)==0)
			aVal=EPrioritySupervisor;
		else
			{
			Print(EError, "Unrecognised priority\n");
			return KErrArgument;
			}
		}
	if (aVal<EPriorityLow || aVal>EPrioritySupervisor)
		{
		Print(EError, "Priority out of range\n");
		return KErrArgument;
		}
	return KErrNone;
	}

int getVersionArg(unsigned int& aVal, const char* aArg)
	{
	const char* s = aArg;
	unsigned int major = 0;
	unsigned int minor = 0;
	for (; major<=6553 && *s>='0' && *s<='9'; ++s)
		major = major*10 + (*s - '0');
	if (*s == '.')
		{
		for (++s; minor<=6553 && *s>='0' && *s<='9'; ++s)
			minor = minor*10 + (*s - '0');
		if (*s==0 && major<32768 && minor<32768)
			{
			aVal = (major << 16) | minor;
			return KErrNone;
			}
		}
	Print(EError, "Bad argument to -version\n");
	return KErrArgument;
	}

int getFPUArg(unsigned int &aVal, int argc, char *argv[], int i)
	{
	if (i>=argc)
		return KErrArgument;
	else if (strnicmp(argv[i], "softvfp", 7)==0)
		aVal = 0;
	else if (strnicmp(argv[i], "vfpv2", 5)==0)
		aVal = 1;
	else
		{
		Print(EError, "Bad argument to -fpu\n");
		return KErrArgument;
		}

	return KErrNone;
	}

int getCompressionMethod(int &aVal, int argc, char *argv[], int i)
{
	if( i >= argc || argv[i] == NULL)
	{
		Print(EError, "Missing argument to -compressionmethod\n");
		return KErrArgument;
	}
	else if (strnicmp(argv[i], "none", 2) == 0)
	{
		aVal = ENoCompression;
	}
	else if (strnicmp(argv[i], "deflate", 7) == 0)
	{
		aVal = EDeflate;
	}
	else if (strnicmp(argv[i], "bytepair", 8) == 0)
	{
		aVal = EBytePair;
	}
	else
	{
		Print(EError, "Bad argument '%s' to -compressionmethod\n", argv[i]);
		return KErrArgument;
	}

	return KErrNone;
	
}


int processCL(int argc, char *argv[])
	{

	int r=KErrNone;
	int i=1;
	while (i<argc)
		{
		if (stricmp("-v", argv[i])==0)
			gVerbose |= E32ImageFile::EDumpDefaults;
		else if (stricmp("-dump", argv[i])==0)
			{
			i++;
			if (i>=argc)
				return KErrArgument;
			char* s=argv[i];
			while(char c = *(s++))
				{
				if(c<'a')
					c += 'a'-'A';
				switch(c)
					{
				case 'h': gVerbose |= E32ImageFile::EDumpHeader; break;
				case 's': gVerbose |= E32ImageFile::EDumpSecurityInfo; break;
				case 'c': gVerbose |= E32ImageFile::EDumpCode; break;
				case 'd': gVerbose |= E32ImageFile::EDumpData; break;
				case 'e': gVerbose |= E32ImageFile::EDumpExports; break;
				case 'i': gVerbose |= E32ImageFile::EDumpImports; break;
				default: return KErrArgument;
					}
				}
			}
		else if (stricmp("-stack", argv[i])==0)
			{
			i++;
			gSetStack=TRUE;
			r=getUIntArg(gStack, argc, argv, i);
			}
		else if (stricmp("-uid1", argv[i])==0)
			{
			i++;
			gSetUid1=TRUE;
			unsigned int id;
			r=getUIntArg(id, argc, argv, i);
			gUid1=TUid::Uid(id);
			}
		else if (stricmp("-uid2", argv[i])==0)
			{
			i++;
			gSetUid2=TRUE;
			unsigned int id;
			r=getUIntArg(id, argc, argv, i);
			gUid2=TUid::Uid(id);
			}
		else if (stricmp("-uid3", argv[i])==0)
			{
			i++;
			gSetUid3=TRUE;
			unsigned int id;
			r=getUIntArg(id, argc, argv, i);
			gUid3=TUid::Uid(id);
			}
		else if (stricmp("-version", argv[i])==0)
			{
			i++;
			r=getVersionArg(gVersionWord, argv[i]);
			gSetVersion=TRUE;
			}
		else if (stricmp("-sid", argv[i])==0)
			{
			i++;
			r=getUIntArg(gSecureId, argc, argv, i);
			gSetSecureId=TRUE;
			}
		else if (stricmp("-vid", argv[i])==0)
			{
			i++;
			r=getUIntArg(gVendorId, argc, argv, i);
			gSetVendorId=TRUE;
			}
		else if (strnicmp("-nocall", argv[i], 7)==0)
			{
			gSetCallEntryPoints=TRUE;
			gCallEntryPoints=FALSE;
			}
		else if (strnicmp("-call", argv[i], 5)==0)
			{
			gSetCallEntryPoints=TRUE;
			gCallEntryPoints=TRUE;
			}
		else if (strnicmp("-fixed", argv[i], 3)==0)
			{
			gSetFixedAddress=TRUE;
			gFixedAddress=TRUE;
			}
		else if (strnicmp("-moving", argv[i], 3)==0)
			{
			gSetFixedAddress=TRUE;
			gFixedAddress=FALSE;
			}
		else if (strnicmp("-priority", argv[i], 4)==0)
			{
			i++;
			gSetPriority=TRUE;
			r=getPriorityArg(gPriority,argc,argv,i);
			}
		else if (strnicmp("-capability", argv[i], 10)==0)
			{
			i++;
			gSetCapability=TRUE;
			r=getCapabilitiesArg(gCapability, argc, argv, i);
			}
		else if (strnicmp("-heap", argv[i], 4)==0)
			{
			i++;
			gSetHeap=TRUE;
			r=getUIntArg(gHeapMin, argc, argv, i);
			if (r==KErrNone)
				r=getUIntArg(gHeapMax, argc, argv, ++i);
			}
		else if (strnicmp("-allow", argv[i], 6)==0) // Note, toolchain passes 'allow' for 'allowdlldata'
			{
			gAllowDllData=TRUE;
			}
		else if( strnicmp("-compressionmethod", argv[i], 18) == 0)
		{
			if(!gSuppressComprMethod)
			{
				gCompress = TRUE;
				gSetCompress = TRUE;
				r = getCompressionMethod(gCompressionMethod, argc, argv, ++i);  
			}
			else
			{
				++i; // Skip the compression method because compessionmethod suppressed.
			}
		}
		else if (strnicmp("-compress", argv[i], 9)==0)
			{
			gCompress=TRUE;
			gSetCompress=TRUE;
			gSuppressComprMethod=FALSE;
			}
		else if (strnicmp("-nocompress", argv[i], 11)==0)
			{
			gCompress=FALSE;
			gSetCompress=TRUE;
			gSuppressComprMethod = TRUE;
			gCompressionMethod = ENoCompression;
			}
		else if (strnicmp("-datalinkaddress", argv[i], 16)==0)
			{
			i++;
			r=getUIntArg(gDataBase, argc, argv, i);
			}
		else if (strnicmp("-align-const-section", argv[i], 20)==0)
			{
			gAlignConstSection=TRUE;
			}
		else if (strnicmp("-const-section-address-mask", argv[i], 27)==0)
			{
			i++;
			r=getUIntArg(gConstSectionAddressMask, argc, argv, i);
			}
		else if (strnicmp("-fpu", argv[i], 4)==0)
			{
			i++;
			r=getFPUArg(gFPU, argc, argv, i);
			gSetFPU=TRUE;
			}
		else if (strnicmp("-paged", argv[i], 6) == 0)
			{
			gCodePaged=TRUE;
			gSetCodePaged=TRUE;
			}
		else if (strnicmp("-unpaged", argv[i], 8) == 0)
			{
			gCodeUnpaged=TRUE;
			gSetCodePaged=TRUE;
			}
		else if (strnicmp("-defaultpaged", argv[i], 13) == 0)
			{
			gCodeDefaultPaged=TRUE;
			gSetCodePaged=TRUE;
			}
		else if (strnicmp("-codepaging", argv[i], 11)==0)
			{
			i++;

			if (i>=argc)
				{
				r = KErrArgument;
				}
			else if ( strnicmp(argv[i], "paged", 5) == 0 )
				{
				gCodePaged=TRUE;
				}
			else if ( strnicmp(argv[i], "unpaged", 7) == 0 )
				{
				gCodeUnpaged=TRUE;
				}
			else if ( strnicmp(argv[i], "default", 7) == 0 )
				{
				gCodeDefaultPaged=TRUE;
				}
			else
				{
				Print(EError, "Bad argument to -codepaging\n");
				r = KErrArgument;
				}

			gSetCodePaged=TRUE;
			}
		else if (strnicmp("-datapaging", argv[i], 11)==0)
			{
			i++;

			if (i>=argc)
				{
				r = KErrArgument;
				}
			else if ( strnicmp(argv[i], "paged", 5) == 0 )
				{
				gDataPaged=TRUE;
				}
			else if ( strnicmp(argv[i], "unpaged", 7) == 0 )
				{
				gDataUnpaged=TRUE;
				}
			else if ( strnicmp(argv[i], "default", 7) == 0 )
				{
				gDataDefaultPaged=TRUE;
				}
			else
				{
				Print(EError, "Bad argument to -datapaging\n");
				r = KErrArgument;
				}

			gSetDataPaged=TRUE;
			}
		else if (strnicmp("-sym_name_lkup", argv[i], 14) == 0)
			{
			gSetSymLkup=TRUE;
			}
		else if (strnicmp("-debuggable", argv[i], 11) == 0)
			{
			gDebuggable=TRUE;
			gSetDebuggable=TRUE;
			}
		else if (strnicmp("-smpsafe", argv[i], 8) == 0)
			{
			gSmpSafe=TRUE;
			gSetSmpSafe=TRUE;
			}
#ifdef __SUPPORT_PE_FILES__
		else if (strnicmp("-x86imp=", argv[i], 8)==0)
			{
			const char* x86impfile=argv[i]+8;
//			printf("%s\n",x86impfile);
			FILE* f=fopen(x86impfile,"rb");
			if (!f)
				r=KErrArgument;
			else
				{
				fseek(f,0,SEEK_END);
				long size=ftell(f);
				fseek(f,0,SEEK_SET);
				if (size>4)
					{
					gX86imp=new char[size];
					fread(gX86imp,1,size,f);
					}
				fclose(f);
				r=KErrNone;
				if (gX86imp)
					{
					gX86imp_size=ALIGN4(size);
	//				printf("size %d\n",size);
					int i;
					int* p=(int*)gX86imp;
					gX86num_imp_dlls=*p++;
					for (i=0; i<gX86num_imp_dlls; ++i)
						{
						++p;
						int n=*p++;
						gX86num_imports+=n;
						p+=n;
						}
	//				fprintf(stderr,"#imports=%d\n",gX86num_imports);
					}
				}
			}
#endif
		else if (gFile1==NULL)
			{
			gFile1 = NormaliseFileName(argv[i]);
			}
		else if (gFile2==NULL)
			{
			gFile2 = NormaliseFileName(argv[i]);
			}
		else
			r=KErrArgument;
		if (r!=KErrNone)
			return r;
		i++;
		}
	return KErrNone;
	}

int main(int argc, char *argv[])
	{
#ifdef __SUPPORT_PE_FILES__
	Print(EAlways,"\nPETRAN - PE file preprocessor");
#endif
#ifdef __SUPPORT_ELF_FILES__
	Print(EAlways,"\nELFTRAN - ELF file preprocessor");
#endif
  	Print(EAlways," V%02d.%02d (Build %03d)\n",MajorVersion,MinorVersion,Build);
	int r=processCL(argc, argv);
	if (r!=KErrNone)
		return helpme(argv[0]);
	if (gFile2)
		return dotran(gFile1, gFile2);
	if ((gSetStack || gSetUid1 || gSetUid2 || gSetUid3 || gSetCallEntryPoints || gSetPriority
		|| gSetCapability || gSetCompress || gSetHeap || gSetVersion || gSetSecureId
		|| gSetVendorId || gSetFixedAddress || gSetFPU || gSetCodePaged || gSetDataPaged || gSetDebuggable || gSetSmpSafe) && gFile1)
		return doalter(gFile1);
	if (gFile1)
		return dodump(gFile1);
	helpme(argv[0]);
	return KErrArgument;
	}
