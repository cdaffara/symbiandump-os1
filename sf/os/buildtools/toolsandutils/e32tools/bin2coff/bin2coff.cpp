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
// Add a simple coff header to a binary file
// optionally strip off the first 256 bytes
// This is primarily for generating an image
// file that can be downloaded via the ether
// to cogent using the SmartFirmWare boot rom
// 
//

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// little error functionet
void error(const char* p)
{
	fprintf(stderr, "%s\n", p);
	exit(1);
}

// little usage functionet
void usage(const char* p)
{
	fprintf(stderr, "%s\n", p);
	exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
	// check for usage
	if (argc < 3 || argc > 4)
		{
		usage("Usage: bin2coff infile outfile nostrip\nOption nostrip: don't strip off the first 256 bytes");
		}

	// make sure we are not operating on only one file
	if (!strcmp(argv[1], argv[2]))
		{
		usage("Usage: bin2coff must specify different input and output files");
		}

	// try to open the input file
	FILE *pInFile;
	if((pInFile  = fopen(argv[1], "rb" )) == NULL)
   		{
   		error("Cannot open input file");
   		}

	// open output file
	FILE *pOutFile;
	if((pOutFile = fopen(argv[2], "wb") ) == NULL)
		{
   		error("Cannot open output file");
		}

	// strip the 256 long header?
	// ok, so it's simple, so what
	bool stripheader = (argc == 3);

	// how big is the input file
	struct stat filelength;
	stat(argv[1], &filelength);
	unsigned long fsize = filelength.st_size;
	if (stripheader)
		{
		fsize -= 256;
		}

	// write out the coff header to the new file
	// somewhere to put a simple coff header
	unsigned char coffhead[0x58] = {0};  // zero all the elements

	// fill in the constant bits
	// this is supposed to be simple, remember
	coffhead[1] = 0x0a;
	coffhead[2] = 0x01;
	coffhead[0x10] = 0x1c;
	coffhead[0x12] = 0x0f;
	coffhead[0x13] = 0xa1;
	coffhead[0x14] = 0x0b;
	coffhead[0x15] = 0x01;
	coffhead[0x26] = 0x40; // entry point 0x00400000
	coffhead[0x2a] = 0x40;
	coffhead[0x30] = 0x2e;
	coffhead[0x31] = 0x74;
	coffhead[0x32] = 0x65;
	coffhead[0x33] = 0x78;
	coffhead[0x34] = 0x74;
	coffhead[0x3a] = 0x40;
	coffhead[0x3e] = 0x40;
	coffhead[0x44] = 0x58;
	coffhead[0x54] = 0x20;

	// now fill in the text segment size
	*(int *) (&coffhead[0x18]) = fsize;
	*(int *) (&coffhead[0x40]) = fsize;

	// write out the new file
	// first the header
	int numwritten=0;
	numwritten = fwrite(coffhead, sizeof(char), 0x58, pOutFile);
	if (numwritten < 0x58)
		{
   		error("The number of items written is less than 0x58");
		}

	// now the rest of the file
	if (stripheader)
		{
		fpos_t filePositionIndicator = 0x100;
		if (fsetpos(pInFile, &filePositionIndicator) !=0)
			{
			error("The file is not accessible ");
			}
		}
	
	static const int buffsize = 1024;
	unsigned char fbuff[buffsize];
	while (!feof(pInFile))
	        {
			int nread = fread(fbuff,sizeof(unsigned char), sizeof(fbuff), pInFile);
			int nwritten = fwrite(fbuff, sizeof(unsigned char), nread, pOutFile);
			if (nwritten < nread)
		        {
				error("The number of items written is less than number of items read");
				}
			}
   
	fclose(pOutFile);
	fclose(pInFile);

	return 0;
}
