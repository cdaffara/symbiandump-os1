// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sys/stat.h>

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <iostream>
#include <iomanip>
using namespace std;
#else //(!__MSVCDOTNET__ && !__TOOLS2__) 
#include <iostream.h>
#include <iomanip.h>
#endif //__MSVCDOTNET__

#define ADDR(rtype, p, o) (rtype *)(((char *)p) + o)

bool ignoreSomeSections;

void hexdump(unsigned char* data, int aSize, int offset)
	//  print hex dump of relevant sections
	{
	int i=0;
	int p=0;
	while (i<aSize)	
		{
		int count=0;
		if(p==0){printf("\t%06x   ",offset);} // offset into section
		while (i<aSize && count<4)
			{ 
			printf("%02X", *data);		// print 4 lots of %08x for the data expresed as 32-bit word 
			data++;
			i++;
			count++;
			offset++;
			}

		printf("  ");
		p++;
		if (p==4)
			{
			data=data-16;
			for (int i=0;i<16;i++)			//print 16 bytes of memory interpreted 
				{							//as ASCII characters with all non-printing 
				if (*data>32 && *data <127)	//characters converted to dots
					{
					printf("%1c",*data);
					}
				else
					{
					printf(".");
					}
					data++;
				}
			p=0; 
			printf(" \n "); 
			}
		}
		printf(" \n\n "); 	   	
	}

void print_directive(unsigned char* data, int size)
	// print formatted text of directive section
	{
	printf ("\t");

	for (int i=0; i<size; i++)
		{
		if ((char)data[i]>31 && (char)data[i]<127)
			{
			printf ("%c", (char)data[i]);
			}

		if ((char)data[i]=='\n')
			{
			printf ("\n\t");
			}
		}

	printf ("\n");
	}

void print_reloc(Elf32_Ehdr* eh, Elf32_Sym* symT, unsigned char* strtab)
	// print relocation section
	{
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, eh->e_shoff);
	for (int j=0;j< eh->e_shnum;j++)
		{
		char* sname = ADDR(char, eh, shdr[eh->e_shstrndx].sh_offset);
		if ( (shdr[j].sh_type==9) && 
		     ( (!ignoreSomeSections) || 
		       (strncmp(".rel.debug_", &sname[shdr[j].sh_name], 11))
		     )
		   )
			{
			unsigned char* data = ADDR(unsigned char, eh, shdr[j].sh_offset);
			int noOfReloc=shdr[j].sh_size / shdr[j].sh_entsize;
			printf("\n\n\n\t\t\t%s\n", &sname[shdr[j].sh_name]);
			Elf32_Rel* rl=(Elf32_Rel*)data;				// pointer to relocation section	
			for (int i=0;i<noOfReloc;i++)
				{
				unsigned char* symbolName = strtab;		// pointer to firest element of string											// table which holds symbol names
				Elf32_Sym*  sym = symT;					// pointer to symbol table
				int symTIndx= ELF32_R_SYM(rl->r_info);		// symbol Tableindex
				sym=sym+symTIndx;							
				symbolName=symbolName+sym->st_name;		// index into string table section 
															// with symbol names
				printf("\t0x%08x \t", rl->r_offset);		// prints offset into relocation section
				printf("%d", symTIndx);					// symbol table index
				printf("\t%s\n",symbolName);				// symbol name
				rl++;			
				}
			}
		}
	}	

void print_GlSymbols(Elf32_Ehdr* eh, Elf32_Sym* symT, unsigned char* strtab)
	// print global symbols from Symbol Table
	{	
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, eh->e_shoff);
	char* sname = ADDR(char, eh, shdr[eh->e_shstrndx].sh_offset);
	for (int i=0;i< eh->e_shnum;i++)
		{
		if (!strcmp(".symtab", &sname[shdr[i].sh_name]))
			{
		  	int noOfSym=shdr[i].sh_size / shdr[i].sh_entsize; 	// number of symbols
		  	const char *symName =(const char *)strtab;
		  	int count = 1;										
		  	printf("Global symbols:\n");
		  	printf("=================\n\n");
		  	for (int l=0;l< noOfSym ;l++)
				{
				symT=symT+1;
				if( ELF32_ST_BIND(symT->st_info) == 1)			// searching for global symbols
			 		{
			  		symName = symName + symT->st_name;			// index into string table section 
			 		printf("%d	",count);
			  		printf(symName);
			  		printf("\n");
			  		symName = symName - symT->st_name;			// back to pointing to first byte of string table
			  		count++;
					}
			
				}
			}
		}
	}
	
void print_elf_header(Elf32_Ehdr* eh)
	{
	// print elf header
	if (eh->e_version==1)
		printf("\tHeader version: EV_CURRENT (Current version)\n");
	else
		printf("\tInvalid version: EV_NONE (Invalid version)\n");

	if (eh->e_type==0)
		printf("\tFile Type: ET_NONE (No file type) (0)\n");
	else if (eh->e_type==1)
			printf("\tFile Type: ET_REL (Relocatable object) (1)\n");
	else if (eh->e_type==2)
			printf("\tFile Type: ET_EXEC (Executable file) (2)\n"); 
	else if (eh->e_type==3)
			printf("\tFile Type: ET_DYN (Shared object file) (3)\n"); 
	else if (eh->e_type==4)
			printf("\tFile Type: ET_CORE (Core File) (4)\n"); 
	else if (eh->e_type==65280)
			printf("\tFile Type: ET_LOPROC (Precessor Specific) (ff00)\n");
	else	
			printf("\tFile Type: ET_HIPROC (Precessor Specific) (ffff)\n");
	if (eh->e_machine==40)
		printf("\tMachine: EM_ARM (ARM)\n");
	else
		printf("\tERROR:\tUnexpected machine\n");

	printf("\tEntry offset (in SHF_ENTRYSECT section):0x%08x \n",eh->e_entry);
	printf("\tProgram header entries : %d\n",eh->e_phnum); 
	printf("\tSection header entries : %d\n",eh->e_shnum); 
  
	printf("\tProgram header offset  : %d",eh->e_phoff); 
	printf("  bytes (0x%08X",eh->e_phoff);
	printf(")\n");
	printf("\tSection header offset  : %d",eh->e_shoff); 
	printf("  bytes (0x%08X",eh->e_shoff);
	printf(")\n");

	printf("\tProgram header entry size  : %d",eh->e_phentsize); 
	printf("  bytes (0x%02X",eh->e_phentsize);
	printf(")\n");
	printf("\tSection header entry size  : %d",eh->e_shentsize); 
	printf("  bytes (0x%02X",eh->e_shentsize);
	printf(")\n");
	printf("\tSection header string table index: %d \n", eh->e_shstrndx);
	printf("\tHeader size: %d", eh->e_ehsize);
	printf("  bytes (0x%02X",eh->e_ehsize);
	printf(")\n");
	}

void print_sect_header(char* sname, Elf32_Shdr* shdr, int count)
	// print section header names
	{
	static const char* KtypeName[]={"0","SHT_PROGBITS (1)","SHT_SYMTAB (2)","SHT_STRTAB (3)",
								  "SHT_RELA (4)","5",	"SHT_DINAMIC (6)","7","8","SHT_REL (9)",
								  "10","SHT_DINSYM (11)"};
						
	printf("\n\n\tName\t\t:%1s\n ",&sname[shdr[count].sh_name]);
	printf("\tType\t\t: %s\n",  KtypeName[shdr[count].sh_type]);
	printf("\tAddr\t\t: 0x%08X\n",shdr[count].sh_addr);
	printf("\tSize\t\t: %1d", shdr[count].sh_size);
	printf("  bytes (0x%X",shdr[count].sh_size);
	printf(")\n");
	printf("\tEntry Size\t: %1d\n",shdr[count].sh_entsize);
	printf("\tAligment\t: %1d\n\n\n",shdr[count].sh_addralign);		 	
	}

unsigned char* findSymbolStringT(Elf32_Ehdr* eh)
	//calculate and return pointer to the first byte of string table(the one with symbol names)
	{
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, eh->e_shoff);
	char* sname = ADDR(char, eh, shdr[eh->e_shstrndx].sh_offset);
	for (int i=0;i < eh->e_shnum; i++)
		{
		if (!strcmp(".strtab", &sname[shdr[i].sh_name]))
			{
			unsigned char* data = ADDR(unsigned char, eh, shdr[i].sh_offset); 
			return data;	//pointer to the first byte of string table section
			}
		}
	return NULL;	//if not found  
	}

Elf32_Sym* findSymbolT(Elf32_Ehdr* eh)
	//calculate and return pointer to the first element of symbol table	
	{
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, eh->e_shoff);
	for (int i=0;i < eh->e_shnum;i++)
		{
		if (shdr[i].sh_type==2)
			{
			unsigned char* data = ADDR(unsigned char, eh, shdr[i].sh_offset);
			Elf32_Sym* sym=(Elf32_Sym*)data;
			return sym;		//pointer to the first element of symbol table.
			}
		}
	return NULL; // if not found
	}

void print_Summary(Elf32_Ehdr* eh)
	{
	//print section names
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, eh->e_shoff);
	char* sname = ADDR(char, eh, shdr[eh->e_shstrndx].sh_offset);
	printf("\nSummary: \n");
	printf("==========\n");
	for (int i=0;i< eh->e_shnum;i++)
		{
		printf(&sname[shdr[i].sh_name]);
		printf("\n");
		}
	}

bool printAll;

int do_elf_file(char* buffer, char* name)
	{
	Elf32_Ehdr* eh=(Elf32_Ehdr *)buffer;	//elf header
	if (eh->e_ident[EI_MAG0] !=0x7f || eh->e_ident[EI_MAG1] != 0x45 || eh->e_ident[EI_MAG2] !=0x4c || eh->e_ident[EI_MAG3] != 0x46)
		{
		// EI_MAG0 to EI_MAG3 - A files' first 4 bytes hold a 'magic number', identifying the file as an ELF object file. 
		cout << "Error: " << name << " is not a valid ELF file";
		return 1;
		}
	if (eh->e_ident[EI_DATA] == 2)							
		{
		// ELF Header size should be 52 bytes or converted into Big-Endian system 13312
		if (eh->e_ehsize != 13312)
			{
			printf("\tERROR:\tELF Header contains invalid file type\n");
			exit(1);
			}
		// e_ident[EI_DATA] specifies the data encoding of the processor-specific data in the object file.
		printf("\tERROR:\tData encoding ELFDATA2MSB (Big-Endian) not supported\n");
		exit(1);
		}
	if (eh->e_ehsize != 52)
		{
		// ELF Header size should be 52 bytes
        printf("\tERROR:\tELF Header contains invalid file type\n");
        exit(1);
        }
	int shoff = eh->e_shoff;							    // offset of section header table
	Elf32_Shdr* shdr = ADDR(Elf32_Shdr, eh, shoff);			// calculating pointer to Secton Header Table
															// Elf32_Shdr * shdr = (Elf32_Shdr *)(buffer+shoff); 
	int shnum = eh->e_shnum;							    // number of section headers
	int shstrndx = eh->e_shstrndx;
	int snameoffset = shdr[shstrndx].sh_offset;				// offset in file of sections' names
	char* sname = ADDR(char, eh, snameoffset);				// pointer to String Table which holds section names
															// char * sname = (char *)(buffer+snameoffset);
	print_elf_header(eh);									// print Elf Header
	
	Elf32_Sym* symT= findSymbolT(eh);	// pointer to Symbol table
	if (symT==NULL)
		{
		printf("\nSymbol table not found\n");
		}
	unsigned char* strtab=findSymbolStringT(eh);	// pointer to String table which holds symbol names
	if (strtab==NULL)
		{	
		printf("\nString (the one which holds symbol names) table not found\n");
		}
		print_reloc(eh,symT, strtab);	// print relocation info showing symbol names and
					        // and the name of section in which the relocaton occurs.														
	for(int i = 0; i < shnum; i++)
		{    
		unsigned char* data = ADDR(unsigned char, eh, shdr[i].sh_offset);	//pointer to the first byte in the section
															//unsigned char * data = (unsigned char * )(buffer+shdr[i].sh_offset);
		int size = shdr[i].sh_size;	// section size in bytes
	
		//print directive section
		if (!strcmp(".directive", &sname[shdr[i].sh_name])) 
			{ 
			print_sect_header(sname, shdr, i);
			print_directive(data,size);
			}

		if (!strcmp(".symtab", &sname[shdr[i].sh_name])) 
			{
			 print_sect_header(sname, shdr, i);	
			 // print global symbols
			 print_GlSymbols(eh,symT, strtab);								
			}

		//print relevant section header names
 		//print hex dump of relevant sections
		if (shdr[i].sh_type==1 || shdr[i].sh_type==4 || shdr[i].sh_type==6 ||
		    shdr[i].sh_type==9 || shdr[i].sh_type==11)
			{
			if (strcmp(".comment", &sname[shdr[i].sh_name])&&
				strcmp(".line", &sname[shdr[i].sh_name])   &&
				strcmp(".hash", &sname[shdr[i].sh_name])   &&
				strcmp(".note", &sname[shdr[i].sh_name])   &&
				strcmp(".directive", &sname[shdr[i].sh_name]) &&
				strncmp(".debug",&sname[shdr[i].sh_name] ,6))
				{
				if ( ! ( (ignoreSomeSections) &&
					 (strncmp(".rel.debug_", &sname[shdr[i].sh_name], 11)==0)
				       )
				   )
					{
					print_sect_header(sname, shdr, i);			
			 	    hexdump(data,size,i);
					}
				}
			}
		if (printAll)		// displays extra information
			{ 	
			if(i!=0)
			 	{
		 	 	print_sect_header(sname, shdr, i);					
		 		hexdump(data,size,i);				
		 		}
		 	}		
		}
	print_Summary(eh);	// print section names
	return 0;
}

int read_ar_element_header(char* ptr)
	{
	int length = strtol(ptr+48,0,10);

	if (strncmp(ptr+58, "\x60\x0A", 2) != 0)
		{
		return -1;
		}
	return length;
	}
	
int main(int argc, char* argv[])
	{
	char* arg;
	int numberOfOptions=2;
	printAll=0;
	ignoreSomeSections=0;
	if (argc<2)
		{
		cout << "File not specified";
		exit(1);
		}
	else if (argc>numberOfOptions+2)
		{
		cout << "Too many arguments";
		exit(1);
		}
	else
		{
		for (int i=1;i<=argc-2;i++)
			{
			if ( strcmp("-i", argv[i]) ==0 )
				{
				ignoreSomeSections=1;
				}
			else if ( strcmp("-a", argv[i]) ==0 )
				{
				printAll=1;
				}
			}
		arg=argv[argc-1];
		}

	struct stat results;
	stat(arg, &results); 
	FILE *elffile;
	if((elffile  = fopen(arg, "rb" )) == NULL)
   		{
   		cout << "Error opening file " << arg;
		exit (1); 
   		}
	char* buffer=new  char [results.st_size];//allocating enough memory 
	fread( buffer, sizeof( char ), results.st_size, elffile);
	fclose(elffile);						
	
	if (strncmp(buffer, "!<arch>\x0A", 8) != 0)
		{
		// plain ELF file
		if (do_elf_file(buffer, arg) != 0)
			{
			return 1;
			}
		return 0;
		}

	// library file
	char* nextfile = buffer;
	int remainder = results.st_size;

#define ADVANCE(n)	nextfile+=(n); remainder-=(n);

	ADVANCE(8);
		
	while (remainder > 0)
		{
		int element_length = read_ar_element_header(nextfile);
		ADVANCE(60);
		
		if (element_length < 0 || element_length > remainder)
			{
			cout << "Error: archive file corrupt";
			return 1;
			}
		
		if (strncmp(nextfile, "\x7F\x45\x4C\x46",4) == 0)
			{
			if (do_elf_file(nextfile, "archive_element") != 0)
				{
				return 1;
				}
			}
		element_length += element_length&1;	// round up to a multiple of 2
		ADVANCE(element_length);
		}
		
	return 0;
	}
