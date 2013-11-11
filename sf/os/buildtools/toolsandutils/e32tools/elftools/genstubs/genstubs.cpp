// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#define MAXPATHNAMELENGTH 2000
#define MAXSYMBOLLENGTH 2000

#define writef(f, d, n, s) if ( 1 != fwrite(d, n, 1, f)) { croak(s); }

void croak(char * s)
{
  printf("GENSTUB ERROR: %s\n", s);
  exit(EXIT_FAILURE);
}

long codeSectionData [] = 
{
  0xe51ff004,    // LDR      pc,0x4
  0x00000000     // the pointer to the imported code
};


#define COMMENTSTRING "\tAndy's magic stub generator vsn 0.2\n"
char commentSectionData[] = // "\tAndy's magic stub generator vsn 0.2\n"
{ 
  '\t','A','n','d','y','\'','s',' ','m','a','g','i','c',' ',
  's','t','u','b',' ','g','e','n','e','r','a','t','o','r',' ',
  'v','s','n',' ','0','.','2','\n'
};

char directiveSectionData[] = // "#<SYMEDIT>\nIMPORT "
{
  '#','<','S','Y','M','E','D','I','T','>','\n',
  'I','M','P','O','R','T',' '
};

char strtabSectionData [] =
{
  0,'$','a',0,                                                          // $a
  '$','d',0,                                                            // $d
  'S','t','u','b','C','o','d','e',0,                                    // StubCode
  '.','d','i','r','e','c','t','i','v','e',0,                            // .directive
  't','h','e','I','m','p','o','r','t','e','d','S','y','m','b','o','l',0 // theImportSymbol
};

Elf32_Sym symtabSectionData[] =
{
  // undefined symbol
  {
    0, // st_name
    0, // st_value
    0, // st_size
    0, // st_info
    0, // st_other
    0  // stshndx
  },
  // $a
  {
    1, // st_name
    0, // st_value
    0, // st_size
    ELF32_ST_INFO(STB_LOCAL,STT_FUNC), // st_info
    0, // st_other
    1  // stshndx
  },
  // $d
  {
    4, // st_name
    4, // st_value
    0, // st_size
    ELF32_ST_INFO(STB_LOCAL,STT_OBJECT), // st_info
    0, // st_other
    1  // stshndx
  },
  // StubCode
  {
    7, // st_name
    0, // st_value
    sizeof(codeSectionData), // st_size
    ELF32_ST_INFO(STB_LOCAL,STT_SECTION), // st_info
    0, // st_other
    1  // stshndx
  },
  // .directive
#define DIRECTIVESYMNDX 4
  {
    16,// st_name
    0, // st_value
    0, // st_size - to be filled on
    ELF32_ST_INFO(STB_LOCAL,STT_SECTION), // st_info
    0, // st_other
    3  // stshndx
  },
  // theImportedSymbol
  {
    27, // st_name
    4, // st_value
    0, // st_size
    ELF32_ST_INFO(STB_LOCAL,STT_FUNC), // st_info
    0, // st_other
    1  // stshndx
  },

  // the exported symbol
#define EXPORTEDSYMBOLSTRNDX 45
  {
    EXPORTEDSYMBOLSTRNDX, // st_name
    0, // st_value
    0, // st_size
    ELF32_ST_INFO(STB_GLOBAL,STT_FUNC), // st_info
    0, // st_other
    1  // stshndx
  },
  // the imported symbol
#define IMPORTEDSYMBOLSYMNDX 7
  {
    0, // st_name - needs to be filled in = EXPORTEDSYMBOLSTRNDX + strlen(exportedSymbol) + 1
    0, // st_value
    0, // st_size
    ELF32_ST_INFO(STB_GLOBAL,STT_NOTYPE), // st_info
    0, // st_other
    SHN_UNDEF  // stshndx
  }
};

Elf32_Rel relocSectionData [] =
{
  {
    0x00000004, // r_offset
    ELF32_R_INFO(IMPORTEDSYMBOLSYMNDX, R_ARM_ABS32)
  }
};

char shstrtabSectionData []= 
{
  0,'S','t','u','b','C','o','d','e',0,                 // StubCode
  '.','r','e','l','S','t','u','b','C','o','d','e',0,   // .relStubCode
  '.','d','i','r','e','c','t','i','v','e',0,           // .directive
  '.','s','y','m','t','a','b',0,                       // .symtab
  '.','s','t','r','t','a','b',0,                       // .strtab
  '.','c','o','m','m','e','n','t',0,                  // .comment
  '.','s','h','s','t','r','t','a','b',0                // .shstrtab
};

Elf32_Shdr SectionHeaderTable[] = 
{
  // dummy section
  {
    0,             // sh_name - to be filled
    SHT_NULL,      // sh_type
    0,             // sh_flags
    0,             // sh_addr
    0,             // sh_offset - file offset of this section - to be filled in
    0,             // sh_size - to be filled in
    0,             // sh_link
    0,             // sh_info
    0,             // sh_addralign
    0              // sh_entsize
  },

  // StubCode section
#define CODEINDEX 1
#define CODEOFFSET (sizeof(Elf32_Ehdr) + (8 * sizeof(Elf32_Shdr)))
#define CODESIZE (sizeof(codeSectionData))
  {
    1,             // sh_name 
    SHT_PROGBITS,  // sh_type
    SHF_ALLOC + SHF_EXECINSTR, // sh_flags
    0,             // sh_addr
    CODEOFFSET,    // sh_offset 
    CODESIZE,      // sh_size
    SHN_UNDEF,     // sh_link
    0,             // sh_info
    4,             // sh_addralign
    0              // sh_entsize
  },

  // relocation data
#define RELOCINDEX 2
#define RELOCOFFSET (CODEOFFSET + CODESIZE)
#define RELOCSIZE (sizeof(relocSectionData))
  {
    10,            // sh_name - to be filled
    SHT_REL,       // sh_type
    0,             // sh_flags
    0,             // sh_addr
    RELOCOFFSET,   // sh_offset - file offset of this section - to be filled in
    RELOCSIZE,     // sh_size - there's only one of them
    3,             // sh_link - index of the symbol table we use
    1,             // sh_info - refers to section 1
    0,             // sh_addralign
    sizeof(Elf32_Rel) // sh_entsize
  },

  // .symtab
#define SYMTABINDEX 3
#define SYMTABOFFSET (RELOCOFFSET + RELOCSIZE)
#define SYMTABSIZE sizeof(symtabSectionData)
{
    34,            // sh_name 
    SHT_SYMTAB,    // sh_type
    0,             // sh_flags
    0,             // sh_addr
    SYMTABOFFSET,  // sh_offset
    SYMTABSIZE, // sh_size
    7,             // sh_link - .strtab - the string table section
    6,             // sh_info - last local symbol
    0,             // sh_addralign
    sizeof(Elf32_Sym)// sh_entsize
  },

  // .comment
#define COMMENTINDEX 4
#define COMMENTOFFSET (SYMTABOFFSET + SYMTABSIZE)
#define COMMENTSIZE (sizeof(commentSectionData))
{
    50,            // sh_name 
    SHT_PROGBITS,  // sh_type
    0,             // sh_flags
    0,             // sh_addr
    COMMENTOFFSET, // sh_offset
    COMMENTSIZE,   // sh_size - to be filled in
    SHN_UNDEF,     // sh_link
    0,             // sh_info
    0,             // sh_addralign
    0              // sh_entsize
  },


  // .shstrtab
#define SHSTRTABINDEX 5
#define SHSTRTABOFFSET (COMMENTOFFSET + COMMENTSIZE)
#define SHSTRTABSIZE sizeof(shstrtabSectionData)
  {
    59,            // sh_name 
    SHT_STRTAB,    // sh_type
    0,             // sh_flags
    0,             // sh_addr
    SHSTRTABOFFSET,// sh_offset
    SHSTRTABSIZE,  // sh_size - to be filled in
    SHN_UNDEF,     // sh_link
    0,             // sh_info
    0,             // sh_addralign
    0              // sh_entsize
      },

  // .directive section
#define DIRECTIVEINDEX 6
#define DIRECTIVEOFFSET SHSTRTABOFFSET + SHSTRTABSIZE
  {
    23,            // sh_name 
    SHT_PROGBITS,  // sh_type
    0,             // sh_flags
    0,             // sh_addr
    DIRECTIVEOFFSET,// sh_offset
    0,             // sh_size - to be filled in
    SHN_UNDEF,     // sh_link 
    0,             // sh_info 
    1,             // sh_addralign
    0              // sh_entsize
  },

  // .strtab
#define STRTABINDEX 7
  {
    42,            // sh_name
    SHT_STRTAB,    // sh_type
    0,             // sh_flags
    0,             // sh_addr
    0,             // sh_offset - to be filled in
    0,             // sh_size - to be filled in
    SHN_UNDEF,     // sh_link
    0,             // sh_info
    0,             // sh_addralign
    0              // sh_entsize
  }
};

#define ELFOBJECTFILEVERSION 1
#define EF_NONE 0x02000000

Elf32_Ehdr headerTemplate = 
{
  {0x7f, 'E', 'L', 'F', 
   ELFCLASS32, ELFDATA2LSB, ELFOBJECTFILEVERSION, 0,
   0, 0, 0, 0,
   0, 0, 0, 0},       // e_ident
  ET_REL,             // e_type
  EM_ARM,             // e_machine
  EV_CURRENT,         // e_version
  0x00000000,         // e_entry
  0,                  // e_phoff
  sizeof(Elf32_Ehdr), // e_shoff
  EF_ARM_EABI_VERSION,// e_flags
  sizeof(Elf32_Ehdr), // e_ehsize
  sizeof(Elf32_Phdr), // e_phentsize
  0,                  // e_phnum
  sizeof(Elf32_Shdr), // e_shentsize
#define SHNUM (sizeof(SectionHeaderTable)/sizeof(Elf32_Shdr))
  SHNUM,              // e_shnum 
  SHSTRTABINDEX       // e_shstrndx
};

void WriteElfHeader(FILE * file);
void WriteSectionHeaderTable(FILE * file);
void WriteCodeSection(FILE * file);
void WriteRelocationSection(FILE * file);
void WriteCommentSection(FILE * file);
void WriteSymtabSection(FILE * file);
void WriteShStrtabSection(FILE * file);
void WriteDirectiveSection(FILE * file, char * import);
void WriteStrtabSection(FILE * file, char * eexport, char * import);

void WriteStubFileX(FILE * file, char * eexport, char * import)
{
  int directiveSize = sizeof(directiveSectionData) + strlen(import) + 1; // terminated by a newline
  int strtabSize = sizeof(strtabSectionData) + strlen(eexport) + strlen(import) + 2; // strings are null terminated

  SectionHeaderTable[DIRECTIVEINDEX].sh_size = directiveSize;
  SectionHeaderTable[STRTABINDEX].sh_offset = DIRECTIVEOFFSET+ directiveSize;
  SectionHeaderTable[STRTABINDEX].sh_size = strtabSize;

  symtabSectionData[IMPORTEDSYMBOLSYMNDX].st_name  = EXPORTEDSYMBOLSTRNDX + strlen(eexport) + 1;

  WriteElfHeader(file);
  WriteSectionHeaderTable(file);
  WriteCodeSection(file);
  WriteRelocationSection(file);
  WriteSymtabSection(file);
  WriteCommentSection(file);
  WriteShStrtabSection(file);
  WriteDirectiveSection(file, import);
  WriteStrtabSection(file, eexport, import);
}

void WriteElfHeader(FILE * file)
{
  writef(file, &headerTemplate, sizeof(headerTemplate), "ELF header");
}

void WriteSectionHeaderTable(FILE * file)
{
  writef(file, &SectionHeaderTable, sizeof(SectionHeaderTable), "Section header table");
}

void CheckFileOffset(FILE * f, int i, char *s)
{
	int o = SectionHeaderTable[i].sh_offset;
	if (ftell(f)!= o) { croak(s); }
}

#define WriteSection(f, i, d, e1, e2) { CheckFileOffset(f, i, e1); writef(f, d, sizeof(d), e2); }

void WriteCodeSection(FILE * file)
{
  WriteSection(file, CODEINDEX, codeSectionData,"StubCode section offset incorrect", "StubCode section");
}

void WriteRelocationSection(FILE * file)
{
  WriteSection(file, RELOCINDEX, relocSectionData,".relStubCode section offset incorrect", ".relStubCode section");
}

void WriteCommentSection(FILE * file)
{
  WriteSection(file, COMMENTINDEX, commentSectionData,".comment section offset incorrect", ".comment section");
}

void WriteSymtabSection(FILE * file)
{
  WriteSection(file, SYMTABINDEX, symtabSectionData,".symtab section offset incorrect", ".symtab section");
}

void WriteShStrtabSection(FILE * file)
{
  WriteSection(file, SHSTRTABINDEX, shstrtabSectionData,".shstrtab section offset incorrect", ".shstrtab section");
}

void WriteDirectiveSection(FILE * file, char * import)
{
  WriteSection(file, DIRECTIVEINDEX, directiveSectionData,
	       ".directive section offset incorrect", ".directive section");
  fprintf(file, "%s\n", import);
}

void WriteStrtabSection(FILE * file, char * eexport, char * import)
{
  char n = 0;
  WriteSection(file, STRTABINDEX, strtabSectionData,".strtab section offset incorrect", ".strtab section");
  fprintf(file, eexport);
  fwrite(&n, sizeof(n), 1, file);
  fprintf(file, import);
  fwrite(&n, sizeof(n), 1, file);
}



void WriteStubFile(char * file, char * eexport, char * import)
{
  FILE * f = fopen(file, "wb");
  if (!f) croak("can't open output file");
  
  WriteStubFileX(f, eexport, import);

  fclose(f);
}


int  main (void)
{
  
  char file[MAXPATHNAMELENGTH], eexport[MAXSYMBOLLENGTH], import[MAXSYMBOLLENGTH];
  
  while (!feof(stdin) && !ferror(stdin)) {
    fscanf(stdin, "%s %s %s\n", file, eexport, import);
    WriteStubFile(file, eexport, import);
  }
  return EXIT_SUCCESS;
}
