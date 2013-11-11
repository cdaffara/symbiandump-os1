// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#define DEFAULT_VERSION 2

#define ELFADDR(rtype, p, o) (rtype *)(((char *)p) + o)

FILE *OUTFILE;

void croak(char * s)
{
	printf("GENINF ERROR: %s\n", s);
	exit(EXIT_FAILURE);
}

void croak2(char * s1, char * s2)
{
	printf("GENINF ERROR: %s %s\n", s1, s2);
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

void EnsureElf(Elf32_Ehdr* eh)
{
	if (!(eh->e_ident[EI_MAG0] == ELFMAG0 &&
	      eh->e_ident[EI_MAG1] == ELFMAG1 &&
	      eh->e_ident[EI_MAG2] == ELFMAG2 &&
	      eh->e_ident[EI_MAG3] == ELFMAG3)) 
		croak("Invalid ELF magic.\n");
	if (eh->e_ident[EI_CLASS] != ELFCLASS32) 
		croak("File is not a 32 bit ELF object file.\n");
	if (eh->e_ident[EI_DATA] != ELFDATA2LSB) 
		croak("File data encoding is not Little Endian.\n");
  	if (eh->e_machine != EM_ARM) 
		croak("File does not target ARM/THUMB processors.\n");

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

typedef struct NameList 
{
	NameList * nl_next;
	char * nl_name;
	unsigned int nl_size;
} NameList;


static NameList * ExportedData = 0;
static NameList * ExportedCode = 0;
//Workaround  for compiler defect(To avoid export of static symbols)
static NameList * LocalSymbols = 0;


NameList* IsPresentInList(char * name, NameList * list)
{
	NameList * e = list;
	while (e) {
		if (!strcmp(name, e->nl_name))
			return e;
		else
			e = e->nl_next;
	}
	return 0;
}

NameList* IsExportedData(char * name) 
{
	return IsPresentInList(name,ExportedData);
}

NameList* IsExportedCode(char * name) 
{
	return IsPresentInList(name,ExportedCode);
}

//Workaround  for compiler defect(To avoid export of local symbols)
NameList* IsLocalSymbol(char * name) 
{
 	return IsPresentInList(name,LocalSymbols);
}

void AddToList(char * name, NameList *& list, unsigned int aSymSz)
{
	NameList * ed = new NameList;
	ed->nl_name = name;
	ed->nl_size = aSymSz;
	ed->nl_next = list;
	list = ed;
}

void AddExportedData(char * name, unsigned int aSymSz)
{
	AddToList(name, ExportedData, aSymSz);
}

void AddExportedCode(char * name, unsigned int aSymSz)
{
	AddToList(name, ExportedCode, aSymSz);
}

//Workaround  for compiler defect(To avoid export of local symbols)
void AddLocalSymbols(char * name)
{
	AddToList(name, LocalSymbols, 0);
}

void InitSymbolsLists(Elf32_Ehdr * eh,Elf32_Shdr * shdr,int shnum)
{
	Elf32_Sym * symtab = 0;
	int nSyms = 0;
	char * strtab = 0;

	for (int i = 0; (i < shnum); i++) {
		if (shdr[i].sh_type == SHT_SYMTAB) {
			symtab = ELFADDR(Elf32_Sym, eh, shdr[i].sh_offset);
			nSyms = shdr[i].sh_size / shdr[i].sh_entsize;
			strtab = ELFADDR(char, eh, shdr[shdr[i].sh_link].sh_offset);
			break;
		}
	}
	for (i = 0; i < nSyms; i++) {
		if (symtab[i].st_info == ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT)) {
			char * name = ELFADDR(char, strtab, symtab[i].st_name);
			AddExportedData(name, symtab[i].st_size);
		} else if (symtab[i].st_info == ELF32_ST_INFO(STB_GLOBAL, STT_FUNC)) {
			char * name = ELFADDR(char, strtab, symtab[i].st_name);
			AddExportedCode(name, symtab[i].st_size);
		}
		else if (symtab[i].st_info == ELF32_ST_INFO(STB_LOCAL, STT_FUNC) || 
 			symtab[i].st_info == ELF32_ST_INFO(STB_LOCAL, STT_OBJECT)) {
		// Workaround for compiler defect to avoid export of local symbols
 			char * name = ELFADDR(char, strtab, symtab[i].st_name);
 			AddLocalSymbols(name);
 		}
	}
}

#define DIRECTIVE_TAG "#<SYMEDIT>#"
#define DIRECTIVE_TAG_SIZE (sizeof(DIRECTIVE_TAG) - 1)
#define EXPORT_DIRECTIVE "EXPORT "
#define EXPORT_DIRECTIVE_SIZE (sizeof(EXPORT_DIRECTIVE) - 1)

void PrintExportsFromSection(char * section, int size)
{
	if (strncmp(DIRECTIVE_TAG, section, DIRECTIVE_TAG_SIZE) != 0) 
		croak("Unrecognized .directive tag");
	section += (DIRECTIVE_TAG_SIZE + 1);
	size -= (DIRECTIVE_TAG_SIZE + 1);
	// The separator for the following entries is 0x0A (i.e. \n). 
	// We're only interested in lines starting with EXPORT
	char eolchar = '\n';
	NameList* aDataSymbol;
	while (size > 0) {
		
		char * eolp = (char *)memchr(section, eolchar, size);
		int linelength = (eolp - section) + 1;
		if (!strncmp(EXPORT_DIRECTIVE, section, EXPORT_DIRECTIVE_SIZE)) {
			char * symbol = section + EXPORT_DIRECTIVE_SIZE;
			int symbolsize = linelength - EXPORT_DIRECTIVE_SIZE - 1;
			// null-terminate the string - doesn't matter that we side effect the
			// section since we won't see it again and saves making a copy.
			symbol[symbolsize] = 0;
			if (IsExportedCode(symbol)) {
				fprintf(OUTFILE, " %s\n", symbol);

			} else if ( (aDataSymbol = IsExportedData(symbol)) != 0) {
				fprintf(OUTFILE, " %s DATA %d\n", symbol, aDataSymbol->nl_size);
			}
			else {
				//All those symbols included from static libs that are
				//treated as exported because of the dll_runtime compiler
				//option.Such symbols donot figure out in the symbol table.
				//Hence are handled separately here.
				
				//Workaround  for compiler defect - To avoid export of local symbols
				if(!IsLocalSymbol(symbol)) 
 				{																	
					fprintf(OUTFILE, " %s\n", symbol);								
				}

			}
		}
		size -= linelength;
		section += linelength;
	}
}

void PrintABIv1ExportSymbols (Elf32_Ehdr * eh)
{
  int shoff = eh->e_shoff;                      // offset of section header table
  if (shoff) {
    Elf32_Shdr * shdr = ELFADDR(Elf32_Shdr, eh, shoff);

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

	// Initialize list of global data symbols
	InitSymbolsLists(eh,shdr,shnum);

    int snameoffset = shdr[shstrndx].sh_offset; // offset in file of sections' names
    char * sname = ELFADDR(char, eh, snameoffset);
    for (int i = 0; i < shnum; i++) {
      if (i != shstrndx) {
		if (!strcmp(".directive", &sname[shdr[i].sh_name])) {
			// we're in business. print the section to stdout
			char * data = ELFADDR(char, eh, shdr[i].sh_offset);
			int size = shdr[i].sh_size;
			PrintExportsFromSection(data, size);
		}
      }
    }
  }
}

void PrintABIv2ExportSymbols(Elf32_Ehdr *eh)
{
	int shoff = eh->e_shoff;                      // offset of section header table
	if (shoff) {
	Elf32_Shdr * shdr = ELFADDR(Elf32_Shdr, eh, shoff);

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

	//Get the symbol table
	Elf32_Sym * symtab = 0;
	int nSyms = 0;
	char * strtab = 0;

	for (int i = 0; (i < shnum); i++) {
		if (shdr[i].sh_type == SHT_DYNSYM) {
			symtab = ELFADDR(Elf32_Sym, eh, shdr[i].sh_offset);
			nSyms = shdr[i].sh_size / shdr[i].sh_entsize;
			strtab = ELFADDR(char, eh, shdr[shdr[i].sh_link].sh_offset);
			break;
		}
	}
	for (i = 0; i < nSyms; i++) {
		if (symtab[i].st_info == ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT)) {
			char * name = ELFADDR(char, strtab, symtab[i].st_name);
			char * CodeSection = ELFADDR(char, eh, shdr[symtab[i].st_shndx].sh_offset);
			Elf32_Word *aLocation = ELFADDR(Elf32_Word, CodeSection, symtab[i].st_value);
			int ordinal = *aLocation;
			fprintf(OUTFILE, "%s %d DATA %d %d\n",name, ordinal, symtab[i].st_size);
		} else if (symtab[i].st_info == ELF32_ST_INFO(STB_GLOBAL, STT_FUNC)) {
			char * name = ELFADDR(char, strtab, symtab[i].st_name);
			char * CodeSection = ELFADDR(char, eh, shdr[symtab[i].st_shndx].sh_offset);
			Elf32_Word *aLocation = ELFADDR(Elf32_Word, CodeSection, symtab[i].st_value);
			int ordinal = *aLocation;
			fprintf(OUTFILE, "%s %d \n",name, ordinal);
		}
	}
	}
}

void PrintSONAME(Elf32_Ehdr *eh)
{
	int shoff = eh->e_shoff;                      // offset of section header table
	if (shoff) {
		Elf32_Shdr * shdr = ELFADDR(Elf32_Shdr, eh, shoff);

		int shnum = eh->e_shnum;                    // number of section headers

		// e_shnum will be 0 if the number of sections is >= SHN_LORESERVE (0xff00)
		// If this is the case, sh_size contains the actual number of section headers.
		// If sh_size is 0, there really aren't any section headers.
		if (!shnum)
			shnum = shdr->sh_size;

		int shstrndx = eh->e_shstrndx;
		char *aSHdrStrTab = ELFADDR(char, eh, shdr[shstrndx].sh_offset);
		int i;
		Elf32_Verdef *aVerDef = 0;
		char *aStringTab = 0;
		for(i = 0; i < shnum; i++) {
			if(strcmp(aSHdrStrTab + shdr[i].sh_name, ".version_d") == 0) {
				aVerDef = ELFADDR(Elf32_Verdef,eh , shdr[i].sh_offset );
				aStringTab = ELFADDR(char, eh, shdr[shdr[i].sh_link].sh_offset);
				break;
			}
		}
		if(!aVerDef)
			return;
		char *aLinkAsName;
		while(aVerDef) {
			if(aVerDef->vd_ndx == DEFAULT_VERSION){
				Elf32_Verdaux *aVerAux = ELFADDR(Elf32_Verdaux, aVerDef, aVerDef->vd_aux);
				aLinkAsName = ELFADDR(char, aStringTab, aVerAux->vda_name);
				fprintf(OUTFILE, "%s \n", aLinkAsName);
				break;
			}
			aVerDef = ELFADDR(Elf32_Verdef, aVerDef, aVerDef->vd_next);
		}
	}
}

int main(int argc, char** argv)
{
	if( argc < 2 )
	{
		return EXIT_FAILURE;
	}

	char* file = argv[argc - 1];//The last arg is the file name
	int idx = 0;
	bool outFileOpt = false;
	bool proxyDso = false;
	bool soname = false;
	while(idx < argc )
	{
		if((stricmp(argv[idx], "-o") == 0) && ((idx + 1) < argc) )
		{
			idx++;
			char *outfilename = argv[idx];
			OUTFILE = fopen(outfilename, "wb");
			outFileOpt = true;
		}
		else if(stricmp(argv[idx], "-d") == 0)
		{
			proxyDso = true;
		}
		else if(stricmp(argv[idx], "-s") == 0)
		{
			soname = true;
		}
		idx++;
	}

	if(!outFileOpt)
	{
		OUTFILE = stdout;
	}

	Elf32_Ehdr * eh = OpenElfFile(file);

	if( soname ) {
		PrintSONAME(eh);
	}
	else if( !proxyDso ) {
		PrintABIv1ExportSymbols(eh);
	}
	else {
		PrintABIv2ExportSymbols(eh);
	}
	return EXIT_SUCCESS;
}
  

  

  
  
