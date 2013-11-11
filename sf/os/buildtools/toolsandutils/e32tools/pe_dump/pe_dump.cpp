// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>
#include <e32def_private.h>
#include <e32err.h>

#include "h_ver.h"
#include "pe_defs.h"
#include <sys\stat.h>

#if defined(__MSVCDOTNET__) || defined (__TOOLS2__)
 #include <iostream>
 #include <iomanip>
 #include <fstream>
 #include <string>
 using namespace std;
#else //__MSVCDOTNET__
 #include <iostream.h>
 #include <iomanip.h>
 #include <fstream.h>
 #include <string.h>
#endif //__MSVCDOTNET__

#include <stdio.h>

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
	char *LoadExtraData(int aOffsetToExtraData, TUint aFileSize);
	
	int Errno();  // Should be invoked only if previous call of PeFile's 
	              // member function returns incorrectly
	int Errno(int aErrno);  // Assign error number for special purpose.
public:
	fstream iFile;
	PIMAGE_DOS_HEADER iDosHeader;
	PIMAGE_NT_HEADERS iHeaders;
	PIMAGE_SECTION_HEADER iSectionHeaders;
private:
	int errno;
	};

PeFile::PeFile()
	: iDosHeader(NULL), iHeaders(NULL), iSectionHeaders(NULL), errno(KErrNone)
	{}

PeFile::~PeFile()
	{
	delete iDosHeader;
	delete iHeaders;
	delete [] iSectionHeaders;
	}

int PeFile::Open(char *aFileName)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	iFile.open(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	iFile.open(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__
	if (!iFile.is_open()) {
		this->errno = KErrNotFound;
		return KErrNotFound;
	}
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
	if (!iDosHeader) {
		this->errno = KErrNoMemory;
		return NULL;
	}

	memset(iDosHeader, 0, sizeof(IMAGE_DOS_HEADER));

	iFile.seekg(0);
	iFile.read((char *)iDosHeader, sizeof(IMAGE_DOS_HEADER));
	if (iDosHeader->e_magic!=IMAGE_DOS_SIGNATURE) {
		this->errno = KErrCorrupt;
		return NULL;
	}
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
	if (!iHeaders) {
		this->errno = KErrNoMemory;
		return NULL;
	}

	memset(iHeaders, 0, sizeof(IMAGE_NT_HEADERS));

	iFile.seekg(d->e_lfanew);
	if (iFile.eof()) {
		this->errno = KErrCorrupt;  // File size is too small.
		return NULL;
	}

	iFile.read((char *)iHeaders, sizeof(IMAGE_NT_HEADERS));
	return iHeaders;
	}

PIMAGE_SECTION_HEADER PeFile::SectionHeaders()
	{
	if (iSectionHeaders)
		return iSectionHeaders;
	PIMAGE_NT_HEADERS h=Headers();
	if (h==NULL) {
		this->errno = KErrNoMemory;
		return NULL;
	}

	int numSec;
	if ((numSec = NumberOfSections()) < 0) {
		return NULL;
	}
	else {
		iSectionHeaders=new IMAGE_SECTION_HEADER [numSec];
		if (!iSectionHeaders) {
			this->errno = KErrNoMemory;
			return NULL;
		}
	}

	iFile.seekg(DosHeader()->e_lfanew+sizeof(IMAGE_NT_HEADERS));
	int i=sizeof(IMAGE_SECTION_HEADER)*numSec;
	iFile.read((char *)iSectionHeaders, i);
	if (iFile.gcount() != i) {  // The size of header is incorrect.
		printf("Error: Cannot load section headers in offset: 0x%x \n", 
			(int) (DosHeader()->e_lfanew + sizeof(IMAGE_NT_HEADERS)));
		this->errno = KErrCorrupt;
		return NULL;
	}

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
	if (section==NULL){
		this->errno = KErrNoMemory;
		return NULL;
	}

	try
	{
	memset(section, 0, h->SizeOfRawData);
	}
	catch(...)
	{
	}

	iFile.seekg(h->PointerToRawData);
	iFile.read(section, h->SizeOfRawData);
	
	// Guarantee we have loaded the section correctly.
	unsigned int actNum = iFile.gcount();
	if (actNum != h->SizeOfRawData) { // The size of section is incorrect.
		printf("Error: Cannot load section in offset: 0x%x \n", 
			(int)h->PointerToRawData);
		this->errno = KErrCorrupt;
		return NULL;
	}

	return section;
	}

char* PeFile::LoadExtraData(int aOffset, TUint aFileSize)
	{
	TUint sizeOfExtraData = 0;
	if(aFileSize >= (TUint)aOffset)
		sizeOfExtraData=aFileSize-aOffset;
	if(sizeOfExtraData>0){
		char* buffer=new char [sizeOfExtraData];
		if (!buffer) {
			this->errno = KErrNoMemory;
			return NULL;
		}

		memset(buffer, 0, sizeOfExtraData);
		iFile.seekg(aOffset);
		iFile.read(buffer, sizeOfExtraData); // Should be OK if the file size is correct.

		// Guarantee we have loaded the data correctly.
		unsigned int actNum = iFile.gcount();
		if (actNum != sizeOfExtraData){ // Shouldn't be here is the file size is correct.
			printf("Error: Cannot load extra section in offset: 0x%x \n", aOffset);
			this->errno = KErrCorrupt;
			return NULL;
		}

		return buffer;
	}
	else {
		this->errno = KErrCorrupt;
		return NULL;
	}
		
}

int PeFile::Errno() {
	return this->errno;
}

int PeFile::Errno(int aErrno) {
	return (this->errno = aErrno);
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
		printf("%06x:", i);
		while (i<aLength && ccount<8)
			{
			printf(" %08x", *p++);
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
		printf("%s\n", line+(ccount*9));
		}
	}

int pecmp(char *aFileName)
	{

	PeFile peFile;
	if (peFile.Open(aFileName)!=KErrNone)
		{
		cout << "Cannot open file '"<<aFileName<<"' for input.\n";
		return KErrNotFound;
		}

	PIMAGE_DOS_HEADER dosHeader=peFile.DosHeader();
	if (dosHeader==NULL)
	{
		switch(peFile.Errno()){
		case KErrNoMemory:
			cout << "Error:  Out of memory\n";
			return KErrNoMemory;
		default:
			cout << aFileName << " does not have a valid DOS header.\n";
		}
		return KErrGeneral;
	}

	PIMAGE_NT_HEADERS headers=peFile.Headers();
	if (headers==NULL)
	{
		switch(peFile.Errno()){
		case KErrNoMemory:
			cout << "Error:  Out of memory\n";
			return KErrNoMemory;
		default:
			cout << aFileName << " is too small to be a PE file.\n";
		}
		return KErrGeneral;
	}

	headers->FileHeader.TimeDateStamp=0;
	headers->OptionalHeader.CheckSum = 0; // Fix for TOOLS2 

	printf("NT Headers:\n");
	dump((TUint*)headers, sizeof(IMAGE_NT_HEADERS));

	int numberofsections=peFile.NumberOfSections();
	if (numberofsections==-1)
		{
		cout << "Not a valid PE file.\n";
		return KErrGeneral;
		}

	PIMAGE_SECTION_HEADER sectionheaders=peFile.SectionHeaders();

	printf("Section Headers:\n");
	dump((TUint*)sectionheaders, sizeof(IMAGE_SECTION_HEADER)*peFile.NumberOfSections());

	TUint exportDirVa=headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	TUint debugDirVa=headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
	int SizeOfHeaders=headers->OptionalHeader.SizeOfHeaders;
	int offsetToExtraData=0;
	int i;
	for (i=0; i<numberofsections; i++)
		{
		PIMAGE_SECTION_HEADER h=&sectionheaders[i];
		char *section=peFile.LoadSection(h);
		if (section==NULL)
		{
			switch(peFile.Errno()){
			case KErrNoMemory:
				cout << "Error:  Out of memory\n";
				return KErrNoMemory;
			default:
				cout << "Not a valid PE file.\n";
			}
			return KErrGeneral;
		}
		char name[9];
		for (int j=0; j<9; j++)
			name[j]=h->Name[j];
		name[8]=0;

		if (debugDirVa>=h->VirtualAddress && debugDirVa<h->VirtualAddress+h->SizeOfRawData)
			{
			// Debug data in this section
			PIMAGE_DEBUG_DIRECTORY dd=(PIMAGE_DEBUG_DIRECTORY)(section+debugDirVa-h->VirtualAddress);
			TInt debugDirSize=headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
			while (debugDirSize>0)
				{
				dd->TimeDateStamp=0;
				// size & location in file of debug data is not significant
				// unless it's also mapped (AddressOfRawData != 0). If that's
				// true, then the data will be visible in one of the sections
				// anyway...
				dd->SizeOfData=0;
				dd->PointerToRawData=0;
				dd++;
				debugDirSize-=sizeof(IMAGE_DEBUG_DIRECTORY);
				}
			}
		if (exportDirVa>=h->VirtualAddress && exportDirVa<h->VirtualAddress+h->SizeOfRawData)
			{
			// Export directory in this section
			PIMAGE_EXPORT_DIRECTORY ed=(PIMAGE_EXPORT_DIRECTORY)(section+exportDirVa-h->VirtualAddress);
			ed->TimeDateStamp=0;
			}
		if (strcmp(".rsrc",name)==0)
			{
			int *t=(int *)(section+4);
			*t=0;
			}

		offsetToExtraData=offsetToExtraData+h->SizeOfRawData;
		printf("Raw data:\n");
		dump((TUint*)section, h->SizeOfRawData);

		delete section;
		}
	struct stat buf;
	stat (aFileName,&buf);
	TUint fileSize=buf.st_size;
	offsetToExtraData=offsetToExtraData+SizeOfHeaders;
	TUint sizeOfExtraData = 0;
	if(buf.st_size >= offsetToExtraData)
		sizeOfExtraData=buf.st_size-offsetToExtraData;
	char* extraData=peFile.LoadExtraData(offsetToExtraData, fileSize);
	if(sizeOfExtraData>0){
		char* nsisSign;
		nsisSign=extraData+8;
		if(strncmp (nsisSign,"NullsoftInst",12) == 0){
			printf("\n\n Extra Data:\n");
			dump((TUint*)extraData, sizeOfExtraData);
		}
	}
	peFile.Close();
	return KErrNone;
	}

int main(int argc, char *argv[])
	{
	int r=KErrArgument;
	if (argc==2)
		{
		r=pecmp(argv[1]);
		}
	if (r==KErrArgument)
		{
		cout << "\nPE_DUMP - PE file dumper V";
		cout << MajorVersion << '.' << setfill('0') << setw(2) << MinorVersion;
		cout << '(' << setw(3) << Build << ")\n";
		cout << Copyright;
		cout << "Syntax: "<<argv[0]<<" pefile\n";
		}
	return r;
	}
