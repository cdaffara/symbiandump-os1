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

#include "e32image.h"

#include "h_ver.h"
#include "h_utl.h"
#include "pe_defs.h"

#ifdef __MSVCDOTNET__
 #include <fstream>
 #include <iomanip>
 #include <string>
 using namespace std;
#else //!__MSVCDOTNET__
 #include <fstream.h>
 #include <iomanip.h>
 #include <string.h>
#endif //__MSVCDOTNET__

const int KDiffIdentical=0;
const int KDiffDifferent=2;

class PeFile
	{
public:
	PeFile();
	~PeFile();
	int Open(char *aFileName);
	void Close();
	PIMAGE_DOS_HEADER DosHeader();
	PIMAGE_NT_HEADERS Headers();
	PIMAGE_SECTION_HEADER SectionHeaders();
	char *LoadSection(PIMAGE_SECTION_HEADER aSectionHeader);
	int NumberOfSections();
public:
	fstream iFile;
	PIMAGE_DOS_HEADER iDosHeader;
	PIMAGE_NT_HEADERS iHeaders;
	PIMAGE_SECTION_HEADER iSectionHeaders;
	};

PeFile::PeFile()
	: iDosHeader(NULL), iHeaders(NULL), iSectionHeaders(NULL)
	{}

PeFile::~PeFile()
	{
	
	delete iDosHeader;
	delete iHeaders;
	delete [] iSectionHeaders;
	}

int PeFile::Open(char *aFileName)
	{
	iFile.open(aFileName, ios::in | ios::binary);
	if (!iFile.is_open())
		return KErrNotFound;
	return KErrNone;
	}

void PeFile::Close()
	{

	iFile.close();
	}


PIMAGE_DOS_HEADER PeFile::DosHeader()
	{
	if (iDosHeader)
		return iDosHeader;
	iDosHeader=new IMAGE_DOS_HEADER;
	iFile.seekg(0);
	iFile.read((char *)iDosHeader, sizeof(IMAGE_DOS_HEADER));
	if (iDosHeader->e_magic!=IMAGE_DOS_SIGNATURE)
		return NULL;
	return iDosHeader;
	}

PIMAGE_NT_HEADERS PeFile::Headers()
	{
	if (iHeaders)
		return iHeaders;
	PIMAGE_DOS_HEADER d=DosHeader();
	if (d==NULL)
		return NULL;
	iHeaders=new IMAGE_NT_HEADERS;
	iFile.seekg(d->e_lfanew);
	if (iFile.eof())
		return NULL;
	iFile.read((char *)iHeaders, sizeof(IMAGE_NT_HEADERS));
	return iHeaders;
	}

PIMAGE_SECTION_HEADER PeFile::SectionHeaders()
	{
	if (iSectionHeaders)
		return iSectionHeaders;
	PIMAGE_NT_HEADERS h=Headers();
	if (h==NULL)
		return NULL;
	iSectionHeaders=new IMAGE_SECTION_HEADER [NumberOfSections()];
	iFile.seekg(DosHeader()->e_lfanew+sizeof(IMAGE_NT_HEADERS));
	int i=sizeof(IMAGE_SECTION_HEADER)*NumberOfSections();
	iFile.read((char *)iSectionHeaders, i);
	return iSectionHeaders;
	}

int PeFile::NumberOfSections()
	{
	if (Headers())
		return iHeaders->FileHeader.NumberOfSections;
	else
		return -1;
	}

char *PeFile::LoadSection(PIMAGE_SECTION_HEADER h)
	{
	char *section=new char [h->SizeOfRawData];
	memset(section, 0, h->SizeOfRawData);
	if (section==NULL)
		return NULL;
	iFile.seekg(h->PointerToRawData);
	iFile.read(section, h->SizeOfRawData);
	return section;
	}

int pecmp(char *aFileName1, char *aFileName2)
	{

	PeFile file1, file2;
	if (file1.Open(aFileName1)!=KErrNone)
		{
		cout << "Cannot open file '"<<aFileName1<<"' for input.\n";
		return KErrNotFound;
		}
	if (file2.Open(aFileName2)!=KErrNone)
		{
		cout << "Cannot open file '"<<aFileName2<<"' for input.\n";
		return KErrNotFound;
		}

	PIMAGE_DOS_HEADER dosheader1=file1.DosHeader();
	if (dosheader1==NULL)
		{
		cout << aFileName1 << " does not have a valid DOS header.\n";
		return KErrGeneral;
		}
	PIMAGE_DOS_HEADER dosheader2=file2.DosHeader();
	if (dosheader2==NULL)
		{
		cout << aFileName2 << " does not have a valid DOS header.\n";
		return KErrGeneral;
		}

	PIMAGE_NT_HEADERS headers1=file1.Headers();
	if (headers1==NULL)
		{
		cout << aFileName1 << " is too small to be a PE file.\n";
		return KErrGeneral;
		}
	PIMAGE_NT_HEADERS headers2=file2.Headers();
	if (headers2==NULL)
		{
		cout << aFileName2 << " is too small to be a PE file.\n";
		return KErrGeneral;
		}

	int sameTime=(headers1->FileHeader.TimeDateStamp==headers2->FileHeader.TimeDateStamp);
	headers1->FileHeader.TimeDateStamp=0;
	headers2->FileHeader.TimeDateStamp=0;

	int r=memcmp(headers1, headers2, sizeof(IMAGE_NT_HEADERS));
	if (r)
		{
		cout << "PE file headers are different.\n";
		return KDiffDifferent;
		}

	PIMAGE_SECTION_HEADER sectionheaders1=file1.SectionHeaders();
	PIMAGE_SECTION_HEADER sectionheaders2=file2.SectionHeaders();
	// file one and two have the same number of sections
	int numberofsections=file1.NumberOfSections();
	if (numberofsections==-1)
		{
		cout << "Not a valid PE file.\n";
		return KErrGeneral;
		}
	r=memcmp(sectionheaders1, sectionheaders2, sizeof(IMAGE_SECTION_HEADER)*file1.NumberOfSections());
	if (r)
		{
		cout << "The files are different:  PE section headers are different.\n";
		return KDiffDifferent;
		}

	TUint exportDirVa=headers1->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	TUint debugDirVa=headers1->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;

	int i;
	for (i=0; i<numberofsections; i++)
		{
		PIMAGE_SECTION_HEADER h=&sectionheaders1[i];
		char *section1=file1.LoadSection(h);
		char *section2=file2.LoadSection(h);
		if ((section1==NULL) || (section2==NULL))
			{
			cout << "Error:  Out of memory\n";
			return KErrNoMemory;
			}
		char name[9];
		for (int j=0; j<9; j++)
			name[j]=h->Name[j];
		name[8]=0;

		if (debugDirVa>=h->VirtualAddress && debugDirVa<h->VirtualAddress+h->SizeOfRawData)
			{
			// Debug data in this section
			PIMAGE_DEBUG_DIRECTORY dd1=(PIMAGE_DEBUG_DIRECTORY)(section1+debugDirVa-h->VirtualAddress);
			PIMAGE_DEBUG_DIRECTORY dd2=(PIMAGE_DEBUG_DIRECTORY)(section2+debugDirVa-h->VirtualAddress);
			TInt debugDirSize=headers1->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
			while (debugDirSize>0)
				{
				if (sameTime)
					sameTime=(dd1->TimeDateStamp==dd2->TimeDateStamp);
				dd1->TimeDateStamp=0;
				dd2->TimeDateStamp=0;
				// size & location in file of debug data is not significant
				// unless it's also mapped (AddressOfRawData != 0). If that's
				// true, then the data will be visible in one of the sections
				// anyway...
				dd1->SizeOfData=0;
				dd2->SizeOfData=0;
				dd1->PointerToRawData=0;
				dd2->PointerToRawData=0;
				dd1++;
				dd2++;
				debugDirSize-=sizeof(IMAGE_DEBUG_DIRECTORY);
				}
			}
		if (exportDirVa>=h->VirtualAddress && exportDirVa<h->VirtualAddress+h->SizeOfRawData)
			{
			// Export directory in this section
			PIMAGE_EXPORT_DIRECTORY ed1=(PIMAGE_EXPORT_DIRECTORY)(section1+exportDirVa-h->VirtualAddress);
			PIMAGE_EXPORT_DIRECTORY ed2=(PIMAGE_EXPORT_DIRECTORY)(section2+exportDirVa-h->VirtualAddress);
			if (sameTime)
				sameTime=(ed1->TimeDateStamp==ed2->TimeDateStamp);
			ed1->TimeDateStamp=0;
			ed2->TimeDateStamp=0;
			}
		if (strcmp(".rsrc",name)==0)
			{
			int *t1=(int *)(section1+4);
			int *t2=(int *)(section2+4);
			if (sameTime)
				sameTime=(*t1==*t2);
			*t1=0;
			*t2=0;
			}

		r=memcmp(section1, section2, h->SizeOfRawData);
		if (r)
			{
			cout << name << " sections are different.\n";
			return KDiffDifferent;
			}
		delete section1;
		delete section2;
		}

	if (sameTime)
		cout << "PE files are identical (time/data stamps also identical).\n";
	else
		cout << "PE files are identical except for time/date stamps.\n";
	file1.Close();
	file2.Close();
	return KDiffIdentical;
	}

int e32cmp(char *aFileName1, char *aFileName2)
	{
	char* f1 = NormaliseFileName(aFileName1);
	char* f2 = NormaliseFileName(aFileName2);
	
	E32ImageFile e32image1;
	E32ImageFile e32image2;
	TInt r = e32image1.Open(f1);
	if (r != KErrNone)
		{
		if (r<0)
			fprintf(stderr, "%s is not a valid E32Image file (error %d)\n", f1, r);
		else
			r = -1;
		return r;
		}
	r = e32image2.Open(f2);
	if (r != KErrNone)
		{
		if (r<0)
			fprintf(stderr, "%s is not a valid E32Image file (error %d)\n", f2, r);
		else
			r = -1;
		return r;
		}



	int sametime=(e32image1.iHdr->iTimeLo==e32image2.iHdr->iTimeLo)
		&&(e32image1.iHdr->iTimeHi==e32image2.iHdr->iTimeHi);
	e32image1.iHdr->iTimeLo=0;
	e32image1.iHdr->iTimeHi=0;
	e32image1.iOrigHdr->iTimeLo=0;
	e32image1.iOrigHdr->iTimeHi=0;
	e32image2.iHdr->iTimeLo=0;
	e32image2.iHdr->iTimeHi=0;
	e32image2.iOrigHdr->iTimeLo=0;
	e32image2.iOrigHdr->iTimeHi=0;
	e32image1.iHdr->iToolsVersion=e32image2.iHdr->iToolsVersion;
	e32image1.iOrigHdr->iToolsVersion=e32image2.iOrigHdr->iToolsVersion;
	e32image1.iHdr->iHeaderCrc=0;
	e32image2.iHdr->iHeaderCrc=0;

	int ohs1 = e32image1.iOrigHdr->TotalSize();
	int ohs2 = e32image2.iOrigHdr->TotalSize();
	int orighdrcmp = (ohs1==ohs2) ? memcmp(e32image1.iOrigHdr, e32image2.iOrigHdr, ohs1) : 1;

	int hs1 = e32image1.iHdr->TotalSize();
	int hs2 = e32image2.iHdr->TotalSize();
	int hdrcmp = (hs1==hs2) ? memcmp(e32image1.iHdr, e32image2.iHdr, hs1) : 1;

	int rs1 = e32image1.iSize - ohs1;
	int rs2 = e32image2.iSize - ohs2;
	int rcmp = (rs1==rs2) ? memcmp(e32image1.iData + ohs1, e32image2.iData + ohs2, rs1) : 1;

	if (orighdrcmp==0 && rcmp==0)
		{
		if (sametime)
			printf("E32 image files are identical\n");
		else
			printf("E32 image files are identical apart from timestamps and Header CRC \n");
		return KDiffIdentical;
		}
	if (hdrcmp==0 && rcmp==0)
		{
		printf("E32 image files are functionally equivalent but have different headers\n");
		return KDiffDifferent;
		}
	printf("E32 image files are different\n");
	return KDiffDifferent;
	}


int main(int argc, char *argv[])
	{
	cout << "\nPEDIFF - PE file compare V";
	cout << MajorVersion << '.' << setfill('0') << setw(2) << MinorVersion;
	cout << '(' << setw(3) << Build << ")\n";
	cout << Copyright;
	
	int r=KErrArgument;
	if ((argc==3) || (argc==4))
		{
		if (argc==3)
			r=pecmp(argv[1], argv[2]);
		else if (strcmp("-e32", argv[1])==0)
			r=e32cmp(argv[2], argv[3]);
		}
	if (r==KErrArgument)
		{
		cout << "Syntax: "<<argv[0]<<" pefile pefile\n";
		cout << "        "<<argv[0]<<" -e32 e32imagefile e32imagefile\n";
		}
	return r;
	}

