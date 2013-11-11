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
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tools/elfdefs.h>

#define ADDR(rtype, p, o) (rtype *)(((char *)p) + o)

void croak(char * s)
{
  printf("GETDIRECTIVE ERROR: %s\n", s);
  exit(EXIT_FAILURE);
}

void croak2(char * s1, char * s2)
{
  printf("GETDIRECTIVE ERROR: %s %s\n", s1, s2);
  exit(EXIT_FAILURE);
}

int GetFileSize(FILE * f, char * file)
{
  int r = fseek(f, 0, SEEK_END);
  if (r) croak2("can't seek to end of file:", file);
  r = ftell(f);
  rewind(f);
  return r;
}

void EnsureElf(Elf32_Ehdr* ef)
	{
	// Do nothing for now
	(void)ef;
	}

Elf32_Ehdr* OpenElfFile (char* file)
	{
	Elf32_Ehdr* eh;
	FILE* f = fopen(file, "rb");
	if (!f)
		croak2("can't open file:", file);

	int fsize = GetFileSize(f, file);

	eh = (Elf32_Ehdr*)malloc(fsize);
	if (!eh)
		croak("Out of memory");

	if (fread(eh, fsize, 1, f) != 1)
		croak2("Can't read file", file);

	EnsureElf(eh);

	return eh;
	}

void PrintDirectiveSections (Elf32_Ehdr * eh)
{
  int shoff = eh->e_shoff;                      // offset of section header table
  if (shoff) {
    Elf32_Shdr * shdr = ADDR(Elf32_Shdr, eh, shoff);

    int shnum = eh->e_shnum;                    // number of section headers

	// e_shnum will be 0 if the number of sections is >= SHN_LORESERVE (0xff00)
	// If this is the case, sh_size contains the actual number of section headers.
	// If sh_size is 0, there really aren't any section headers.
	if (!shnum)
		shnum = shdr->sh_size;

    int shstrndx = eh->e_shstrndx;

	// If the section name string table index is >= SHN_LORESERVE (0xff00), shstrndx
	// contains SHN_XINDEX (0xffff).
	// If this is the case, sh_link contains the actual index of the section name string
	// table, otherwise sh_link is 0.

	if (shstrndx >= 65535)
		shstrndx = shdr->sh_link;

    int snameoffset = shdr[shstrndx].sh_offset; // offset in file of sections' names
    char * sname = ADDR(char, eh, snameoffset);
    for (int i = 0; i < shnum; i++) {
      if (i != shstrndx) {
	if (!strcmp(".directive", &sname[shdr[i].sh_name])) {
	  // we're in business. print the section to stdout
	  char * data = ADDR(char, eh, shdr[i].sh_offset);
	  int size = shdr[i].sh_size;
	  fwrite(data, size, 1, stdout);
	  printf("\n");
	}
      }
    }
  }
}
	
int main(int argc, char** argv)
	{
	(void)argc;
	char* file = argv[1];
	PrintDirectiveSections(OpenElfFile(file));

	return EXIT_SUCCESS;
	}
  

  

  
  
