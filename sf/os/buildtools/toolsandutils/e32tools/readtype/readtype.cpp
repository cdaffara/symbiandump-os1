// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reads a Unicode character type data file (such as UnicodeData-3.0.0.txt or a file containing locale-specific overrides)
// and writes C++ definitions of tables containing the information.
// Usage: readtype <input-file> <output-file> { <locale-name> }.
// <input-file>: either the standard Unicode character data file (e.g., UnicodeData-3.0.0.txt) or a file containing
// overriding information for a certain locale, in the same format as the standard file, but with ranges for which
// there is no data given in the form:
// 0041;;;;;;;;;;;;;;
// 006A;<No Data First>;;;;;;;;;;;;;
// FFFF;<No Data Last>;;;;;;;;;;;;;
// (in this example, these entries show that there is no overriding data for the character 0041 and range
// 006A..FFFF inclusive).
// Both single entries with no data and ranges with no data must have nothing in the third field (category).
// <output-file>: the C++ source file to be output: this file becomes \e32\unicode\unitable.cpp, or an overriding
// file in \e32\lsrc; there are none of these yet.
// <locale-name>: a an optional name to be inserted into identifiers in the output file: omit this for the standard
// data set; use names like 'Turkish', 'Japanese', etc., for locales.
// 
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _UNICODE
#define _UNICODE
#endif

#include <unicode.h>

// don't use unicode.h::TUnicodeDataRange, since its for 16-bit, and deprecated
struct TUnicodeDataRange32	// Only used inside this cpp.
	{
	TUint32 iRangeStart;	// Unicode value of the start of the range of characters
	TInt16 iIndex;			// index into an array of character information structures (-1 means data no available)
	};

const int PlaneCount = 17;
TUnicodePlane ThePlanesInReadType[PlaneCount];

// Tables to convert names used in the data file to categories defined in TChar.
struct CatInfo
	{
	const char* iName;
	TChar::TCategory iCat;
	};

static const CatInfo TheCatInfo[] =
	{
	{ "Lu", TChar::ELuCategory },
	{ "Ll", TChar::ELlCategory },
	{ "Lt", TChar::ELtCategory },
	{ "Lo", TChar::ELoCategory },
	{ "Lm", TChar::ELmCategory },
	{ "Mn", TChar::EMnCategory },
	{ "Mc", TChar::EMcCategory },
	{ "Me", TChar::EMeCategory },
	{ "Nd", TChar::ENdCategory },
	{ "Nl", TChar::ENlCategory },
	{ "No", TChar::ENoCategory },
	{ "Pc", TChar::EPcCategory },
	{ "Pd", TChar::EPdCategory },
	{ "Ps", TChar::EPsCategory },
	{ "Pe", TChar::EPeCategory },
	{ "Pi", TChar::EPiCategory },
	{ "Pf", TChar::EPfCategory },
	{ "Po", TChar::EPoCategory },
	{ "Sm", TChar::ESmCategory },
	{ "Sc", TChar::EScCategory },
	{ "Sk", TChar::ESkCategory },
	{ "So", TChar::ESoCategory },
	{ "Zs", TChar::EZsCategory },
	{ "Zl", TChar::EZlCategory },
	{ "Zp", TChar::EZpCategory },
	{ "Cc", TChar::ECcCategory },
	{ "Cf", TChar::ECfCategory },
	{ "Cs", TChar::ECsCategory },
	{ "Co", TChar::ECoCategory },
	{ "Cn", TChar::ECnCategory }
	};
const int TheCategories = sizeof(TheCatInfo) / sizeof(TheCatInfo[0]);

struct BdCatInfo
	{
	const char* iName;
	TChar::TBdCategory iBdCat;
	};

static const BdCatInfo TheBdCatInfo[] =
	{
	{ "L", TChar::ELeftToRight },
	{ "LRE", TChar::ELeftToRightEmbedding },
	{ "LRO", TChar::ELeftToRightOverride },
	{ "R", TChar::ERightToLeft },
	{ "AL", TChar::ERightToLeftArabic }, 
	{ "RLE", TChar::ERightToLeftEmbedding },
	{ "RLO", TChar::ERightToLeftOverride },
	{ "PDF", TChar::EPopDirectionalFormat },
	{ "EN", TChar::EEuropeanNumber }, 
	{ "ES", TChar::EEuropeanNumberSeparator }, 
	{ "ET", TChar::EEuropeanNumberTerminator },  
	{ "AN", TChar::EArabicNumber }, 
	{ "CS", TChar::ECommonNumberSeparator }, 
	{ "NSM", TChar::ENonSpacingMark },
	{ "BN", TChar::EBoundaryNeutral }, 
	{ "B", TChar::EParagraphSeparator },
	{ "S", TChar::ESegmentSeparator },
	{ "WS", TChar::EWhitespace }, 
	{ "ON", TChar::EOtherNeutral }, 
	};
const int TheBdCategories = sizeof(TheBdCatInfo) / sizeof(TheBdCatInfo[0]);

// Class derived from TUnicodeData to provide constructor etc.
class Data: public TUnicodeData
	{
	public:
	Data();
	TBool operator==(const Data& c) const;
	TBool operator!=(const Data& c) const { return !(*this == c); }
	void Write();
	};

// The character information table.
const int MaxDatas = 1000;
Data TheData[MaxDatas];
int Datas = 0;

// The range table, containing indices to the character information table.
const int MaxRanges = 4000;
TUnicodeDataRange32 TheRange[MaxRanges];
int Ranges = 0;

// The exhaustive index table, containing indices from every 16-bit value to the character information table.
int TheIndex[0x110000];

// The special tables for characters in the range 0..255.
TUint16 LowerCaseTable[256];
TUint16 FoldTable[256];

// The special table for characters in the range 0xFF00..0xFFFF
TUint16 CjkWidthFoldTable[256];

/*
The composition table. The compositions are stored as a word made up from the composition tag (high byte) and
the number of components (low byte), the Unicode value of the composed character, then the Unicode values of
the components.

Two tables are created containing the indices of compositions. One of these is sorted by
composed character, one by decomposition. This enables quick conversions to be made in both directions.
*/
const int MaxCompositionWords = 14000;
TUint32 CompositionBuffer[MaxCompositionWords];
int CompositionWords = 0;
const int MaxCompositions = 8000;
TInt16 Compose[MaxCompositions];		// composition buffer indices, sorted by composed character
TInt16 Decompose[MaxCompositions];		// composition buffer indices, sorted by decomposition
int Compositions = 0;
int trie_data[0x110000];					// used to build the trie

FILE *input_file;
FILE *output_file;
const char *input_filename;
const char *output_filename;

// Convert a hex string to an integer.
static int hex(const char *s)
	{
	int x = 0;
	while (*s)
		{
		int n = *s;
		if (n >= '0' && n <= '9')
			n -= '0';
		else if (n >= 'A' && n <= 'F')
			n -= 'A' - 10;
		else if (n >= 'a' && n <= 'f')
			n -= 'a' - 10;
		else
			break;
		x = x * 16 + n;

		s++;
		}
	return x;
	}

static TChar::TCategory Category(const char* aName,bool aWarn)
	{
	for (int i = 0; i < TheCategories; i++)
		if (!strcmp(aName,TheCatInfo[i].iName))
			return TheCatInfo[i].iCat;
	if (aWarn)
		fprintf(stderr,"unknown category %s\n",aName);
	return (TChar::TCategory)(-1);
	}

static TChar::TBdCategory BdCategory(const char* aName,bool aWarn)
	{
	for (int i = 0; i < TheBdCategories; i++)
		if (!strcmp(aName,TheBdCatInfo[i].iName))
			return TheBdCatInfo[i].iBdCat;
	if (aWarn)
		fprintf(stderr,"unknown bidirectional category %s\n",aName);
	return (TChar::TBdCategory)(-1);
	}

// Write an aggregate initialiser for a Data object to the output file.
void Data::Write()
	{
	fprintf(output_file,"{ %d, %d, %d, %d, %d, %d }",
			(int)iCategory,
			(int)iBdCategory,
			(int)iCombiningClass,
			(int)iDigitOffset,
			(int)iCaseOffset,
			(int)iFlags);
	}

/*
Add a new entry to the range table. If the category is the illegal value -1 store -1 as the
index; this feature is used when creating character data for specific locales, which mostly
consists of ranges for which the data is held in the main table, and is marked in this way
as unspecified in the locale table.
*/
void add_range(Data& info,TInt code)
	{
	// Get an index to the character info; add a new entry if necessary.
	int index = -1;
	if (info.iCategory != TChar::TCategory(0xFF))
		{
		for (int i = 0; i < Datas && index == -1; i++)
			if (TheData[i] == info)
				index = i;
		if (index == -1)
			{
			if (Datas >= MaxDatas)
				{
				fprintf(stderr,"too many Datas: > %d\n",MaxDatas);
				exit(1);
				}
			TheData[index = Datas++] = info;
			}
		}

	// Add the entry to the range table.
	if (Ranges >= MaxRanges)
		{
		fprintf(stderr,"too many Ranges: > %d, when processing U+%x\n", MaxRanges, code);
		exit(1);
		}
	TheRange[Ranges].iRangeStart = code;
	TheRange[Ranges].iIndex = (TInt16)index;
	Ranges++;
	}

// Write a table of "entries" integers each of "entry_size" bytes.
int write_table(const void *table,const char *name,
				int entries,int input_entry_size,int output_entry_size,
				int entry_signed,int entries_per_row,int write_array_size)
	{
	const char *type = entry_signed ? "TInt" : "TUint";
	const int bits = output_entry_size * 8;

	/*
	There is a choice here whether or not the number of entries in the array is written:
	either <name>[<size>] or <name>[] is written. The latter method is used where the header
	says <name>[] so that compilers like GCC don't moan about type mismatches.
	*/
	if (entries == 0)
		{
		// In case that given plane has no character.
		fprintf(output_file,"const %s%d * const %s = NULL;\n",type,bits,name);
		return 0;
		}
	if (write_array_size)
		fprintf(output_file,"const %s%d %s[%d] = \n\t{",type,bits,name,entries);
	else
		fprintf(output_file,"const %s%d %s[] = \n\t{ // %d entries",type,bits,name,entries);

	const unsigned char *p = (const unsigned char *)table;
	for (int i = 0; i < entries; i++, p += input_entry_size)
		{
		if (i % entries_per_row == 0)
			fprintf(output_file,"\n\t");
		if (output_entry_size == 1)
			fprintf(output_file,"0x%02x",(int)(*p));
		else if (output_entry_size == 2)
			fprintf(output_file,"0x%04x",(int)(*((TUint16 *)p)));
		else if (output_entry_size == 4)
			fprintf(output_file,"0x%08x",(int)(*((TUint32 *)p)));
		else
			{
			fprintf(stderr,"illegal output entry size: %d\n",output_entry_size);
			exit(1);
			}
		if (i < entries - 1)
			fputc(',',output_file);
		// comment for easy read
		//if ((i+1) % entries_per_row == 0)
		//	fprintf(output_file, "\t// U+%X-U+%X (%d-%d)", i+1-entries_per_row, i, i+1-entries_per_row, i);
		}
	fprintf(output_file,"\n\t};\n");

	return entries * output_entry_size;
	}

/*
Create and write a trie representing the data in 'aTheIndex'
The trie is of two levels, the first level indexed by the high 'aBlockBits' bits of the
character code, the second by the low bits. There is one wrinkle; if the index value, which is 16 bits,
has its top bit set, it is not an index but the actual data value for all entries in that block.

Thus the way to get the value for a code is:

int index = trie_index[code >> aBlockBits];
if (index & 0x8000)
	value = index & ~0x8000;
else
	value = aTrieData[code & (1 << (16 - aBlockBits))];

The data size in bytes is returned.
The argument 'aWrite' determines whether the data is written or not.
The arguments 'aTrie1Name' and 'aTrie2Name' are used as variable names in generated unitable.cpp.
*/
int write_trie(int aOutputEntrySize,int aBlockBits,bool aWrite, int *aTheIndex, int *aTrieData, char *aTrie1Name, char *aTrie2Name)
	{
	int n = 0; // number of entries used in trie_data

	int block_size = 1 << aBlockBits;
	int blocks = 1 << (16 - aBlockBits);

	int* trie_index = new int[blocks];
	int* block = new int[block_size];

	for (int block_index = 0; block_index < blocks; block_index++)
		{
		// Write the data for the current block.
		int block_start = block_index * block_size;
		bool all_the_same = true;
		for (int code = 0; code < block_size; code++)
			{
			block[code] = aTheIndex[block_start + code];
			if (block[code] != block[0])
				all_the_same = false;
			}

		// Try to find a match for it.
		int insert_at;
		if (all_the_same)
			trie_index[block_index] = block[0] | 0x8000;
		else
			{
			for (insert_at = 0; insert_at < n; insert_at++)
				{
				int entries = n - insert_at;
				if (entries > block_size)
					entries = block_size;
				int bytes = entries * sizeof(int);
				if (memcmp(block,aTrieData + insert_at,bytes) == 0)
					break;
				}

			memcpy(aTrieData + insert_at,block,block_size * sizeof(int));
			if (insert_at + block_size > n)
				n = insert_at + block_size;
			trie_index[block_index] = insert_at;
			}
		}

	if (aWrite)
		{
		write_table(trie_index,aTrie1Name,blocks,4,2,false,16,true);
		write_table(aTrieData,aTrie2Name,n,4,aOutputEntrySize,false,32,true);
		}

	delete [] trie_index;
	delete [] block;

	return blocks * 2 + n * aOutputEntrySize;
	}

// Write the best possible 2-level trie for all planes, trying block sizes of 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 and 8192
// @return Data size in bytes.
int write_trie()
	{
	int byteCount = 0;
	for (int plane=0; plane<PlaneCount; plane++)
		{
		int best_data_size = 1 << 30;
		int best_bits = 0;

		int outputEntrySize = 2;
		char trie1Name[255];
		char trie2Name[255];
		sprintf(trie1Name, "ThePlane%02dTrieIndex1", plane);
		sprintf(trie2Name, "ThePlane%02dTrieIndex2", plane);
		int *theIndex = TheIndex + plane * 0x10000;
		int *trieData = trie_data + plane * 0x10000;

		for (int cur_bits = 3; cur_bits < 14; cur_bits++)
			{
			int cur_data_size = write_trie(outputEntrySize, cur_bits, false, theIndex, trieData, trie1Name, trie2Name);
			if (cur_data_size < best_data_size)
				{
				best_bits = cur_bits;
				best_data_size = cur_data_size;
				}
			}

		byteCount += write_trie(outputEntrySize, best_bits, true, theIndex, trieData, trie1Name, trie2Name);
		ThePlanesInReadType[plane].iCodesPerBlock = (TUint8) best_bits;
		ThePlanesInReadType[plane].iMaskForCodePoint = (TUint16) ((1 << (best_bits)) - 1);
		ThePlanesInReadType[plane].iMaskForBlock = (TUint16) (~(ThePlanesInReadType[plane].iMaskForCodePoint));
		}
	return byteCount;
	}

/*
Compare entries in the decompose table for the purpose of sorting them. The entries are indices
into the starting words of compositions stored in the composition buffer.
*/
int compare_decompositions(const void *p,const void *q)
	{
	// Get the indexes.
	TInt16 index1 = *((const TInt16 *)p);
	TInt16 index2 = *((const TInt16 *)q);

	// Compare the two composition strings.
	return TUnicode::Compare((TUint16 *)&CompositionBuffer[index1 + 2], CompositionBuffer[index1 + 1]*2,
							 (TUint16 *)&CompositionBuffer[index2 + 2], CompositionBuffer[index2 + 1]*2);
	}
 
// Write the output file.
void write_output()
	{
	int data_bytes = 0;

	// Write the comment at the top of the file
	fprintf(output_file, "// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).\n");
	fprintf(output_file, "// All rights reserved.\n");
	fprintf(output_file, "// This component and the accompanying materials are made available\n");
	fprintf(output_file, "// under the terms of the License \"Eclipse Public License v1.0\"\n");
	fprintf(output_file, "// which accompanies this distribution, and is available\n");
	fprintf(output_file, "// at the URL \"http://www.eclipse.org/legal/epl-v10.html\".\n");
	fprintf(output_file, "//\n");
	fprintf(output_file, "// Initial Contributors:\n");
	fprintf(output_file, "// Nokia Corporation - initial contribution.\n");
	fprintf(output_file, "//\n");
	fprintf(output_file, "// Contributors:\n");
	fprintf(output_file, "//\n");
	fprintf(output_file, "// Description:\n");

	fprintf(output_file,
			"// Unicode character information tables.\n"
			"// Written by the READTYPE program.\n"
			"// Please read the 'Unicode Character Data and Line Break data Update History.doc' file for detailed history of updates to this file.\n"
			"// This file was generated by the READTYPE tool using UCD 5.0.\n"
			"// The contents of this file were generated automatically. Please do not edit this manually.\n"
			"//\n"
			"//\n"
			"\n");

	// Write the directive to include the header file.
	fprintf(output_file,"#include <unicode.h>\n\n");

	// Export two variables for unicode.cpp.
	fprintf(output_file, "\n");
	fprintf(output_file, "// Declarations for tables held in unitable.cpp and used by unicode.cpp.\n");
	fprintf(output_file, "extern const TStandardUnicodeDataSet TheStandardUnicodeDataSet[];\n");
	fprintf(output_file, "extern const TUnicodePlane ThePlanes[17];\n\n\n");

	// Write the trie data.
	data_bytes += write_trie();

	// Write the character information table.
	fprintf(output_file,"static const TUnicodeData TheUnicodeData[] =\n\t{ // %d entries\n", Datas);
	int i;
	for (i = 0; i < Datas; i++)
		{
		fputc('\t',output_file);
		TheData[i].Write();
		if (i < Datas - 1)
			fputc(',',output_file);
		fprintf(output_file, "\t// 0x%X (%d)", i, i);
		fputc('\n',output_file);
		}
	fprintf(output_file,"\t};\n\n");
	data_bytes += Datas * sizeof(Data);

	// write plane properties
	fprintf(output_file, "const TUnicodePlane ThePlanes[%d] =\n\t{\n", PlaneCount);
	int plane;
	for (plane=0; plane<=16; plane++)
		{
		fprintf(output_file, "\t{%d, 0x%04X, 0x%04X }",
			ThePlanesInReadType[plane].iCodesPerBlock, ThePlanesInReadType[plane].iMaskForBlock, ThePlanesInReadType[plane].iMaskForCodePoint);
		if (plane < 16)
			fprintf(output_file, ",\n");
		}
	fprintf(output_file, "\n\t};\n\n");
	data_bytes += 5*PlaneCount;

	// Write a data structure referring to the trie data.
	fprintf(output_file,"const TStandardUnicodeDataSet TheStandardUnicodeDataSet[] =\n\t{ // %d entries\n", PlaneCount);
	for (plane=0; plane<=16; plane++)
		{
		fprintf(output_file,"\t{ ThePlane%02dTrieIndex1, ThePlane%02dTrieIndex2, TheUnicodeData }", plane, plane);
		if (plane < 16)
			fprintf(output_file, ",\n");
		}
	fprintf(output_file, "\n\t};\n\n");
	data_bytes += 12*PlaneCount;

	// Convert the fold table to lower case.
	for (i = 0; i < 256; i++)
		FoldTable[i] = LowerCaseTable[FoldTable[i]];

	// Make 00A0 (non-break space) fold to space.
	FoldTable[0xA0] = 0x20;

	// Make unassigned characters in the CJK width fold table fold to themselves.
	for (i = 0; i < 256; i++)
		if (CjkWidthFoldTable[i] == 0)
			CjkWidthFoldTable[i] = (TUint16)(0xFF00 + i);

	// Write the special tables
	data_bytes += write_table(FoldTable,"TUnicode::FoldTable",256,2,2,false,16,true);
	data_bytes += write_table(CjkWidthFoldTable,"TUnicode::CjkWidthFoldTable",256,2,2,false,16,true);

	// Write the number of data bytes at the end of the file.
	fprintf(output_file,"\n// The tables and structures contain %d bytes of data.\n",data_bytes);
	}

int main(int argc,char **argv)
	{
	if (argc < 2)
		{
		fputs("usage: readtype <input-file> <output-file>",stderr);
		exit(1);
		}

	input_filename = argv[1];
	output_filename = argv[2];

	// Locale support in previous version is deprecated.

	input_file = fopen(input_filename,"r");
	if (!input_file)
		{
		fprintf(stderr,"cannot open input file %s\n",input_filename);
		exit(1);
		}
	output_file = fopen(output_filename,"w");
	if (!output_file)
		{
		fprintf(stderr,"cannot open output file %s\n",output_filename);
		exit(1);
		}

	Data range_info;		// attributes of the current range
	Data unassigned_info;	// attributes used for unassigned characters; the default constructor
							// sets the category to Cn, bidirectional category to L, everything else to 0.
	TBool first = true;
	
	char line[1024];
	const int Fields = 15;
	char *field[Fields];
	TInt prev_code = 0;
	while (fgets(line,sizeof(line),input_file))
		{
		// Strip trailing newline if any.
		int length = strlen(line);
		if (length && line[length - 1] == '\n')
			line[length - 1] = 0;

		// Parse into fields.
		int n = 1;
		field[0] = line;
		for (char *p = line; *p; p++)
			if (*p == ';' && n < Fields)
				{
				*p = 0;
				field[n++] = p + 1;
				}

		// Ignore the line if there is only one field.
		if (n == 1)
			continue;

		// Extract fields of interest.

		// Field 0: Unicode value in hexadecimal.
		int code = hex(field[0]);

		// Field 2: Category.
		Data cur_info;
		cur_info.iCategory = (TUint8)Category(field[2], true);

		// Field 3: Combining class.
		cur_info.iCombiningClass = (TUint8)atoi(field[3]);
		
		// Field 4: Bidirectional category.
		cur_info.iBdCategory = (TUint8)BdCategory(field[4], true);

		// Prepare to determine the folded version (converted to lower case, stripped of accents).
		int folded_code = code;

		// Field 5: Character decomposition.
		if (field[5][0])
			{
			int components = 0;
			const int MaxComponents = 18;		// FDFA; ARABIC LIGATURE SALLALLAHOU ALAYHE WASALLAM has 18 components!
			TUint32 component[MaxComponents];

			// Extract the tag if any.
			char *p = field[5];
			const char *tag = NULL;
			if (field[5][0] == '<')
				{
				tag = ++p;
				while (*p && *p != '>')
					p++;
				if (!*p)
					{
					fprintf(stderr,"syntax error: missing > on the line for code %x\n",code);
					exit(1);
					}
				*p++ = 0;
				}

			// Read the components.
			while (*p)
				{
				while (*p == ' ')
					p++;
				if (components >= MaxComponents)
					{
					fprintf(stderr,"decomposition of %x has too many components: increase MaxComponents\n",code);
					exit(1);
					}
				component[components++] = hex(p);
				while (*p && *p != ' ')
					p++;
				}

			// Store the composition if it has a null tag and is therefore canonical.
			if (tag == NULL)
				{
				// Put its index into the tables.
				if (Compositions >= MaxCompositions)
					{
					fprintf(stderr,"too many compositions (at code %x): increase MaxCompositions\n",code);
					exit(1);
					}
				if (CompositionWords >= 65535)
					{
					fprintf(stderr, "too many compositions (at code %x): need 32 bit!?\n", code);
					exit(1);
					}
				Compose[Compositions] = Decompose[Compositions] = (TInt16)CompositionWords;
				Compositions++;

				// Put it into the composition buffer.
				if (CompositionWords + 2 + components >= MaxCompositionWords)
					{
					fprintf(stderr,"too many compositions (at code %x): increase MaxCompositionWords\n",code);
					exit(1);
					}
				CompositionBuffer[CompositionWords++] = code;
				CompositionBuffer[CompositionWords++] = components;
				for (int i = 0; i < components; i++)
					CompositionBuffer[CompositionWords++] = component[i];
				}
			
			// Store the code used in the ordinary and CJK fold tables.
			if (components > 0)
				{
				if (code < 256)
					{
					if (tag == NULL)
						folded_code = component[0];
					}
				else if (code >= 0xFF00 && code <= 0xFFEE)	// tag will always be <wide> or <narrow>
					folded_code = component[0];
				}
			}

		// Field 8. Numeric value.
		if (field[8][0])
			{
			if (field[8][1] == '/' || field[8][2] == '/')		// fractions
				cur_info.iFlags |= TUnicodeData::EFraction;
			else
				{
				int value = atoi(field[8]);
				if (value >= 0 && value <= 255)
					{
					cur_info.iDigitOffset = (TUint8)((value - (code & 255)) & 255);
					cur_info.iFlags |= TUnicodeData::ESmallNumeric;
					}
				else if (value == 500)
					cur_info.iFlags |= TUnicodeData::EFiveHundred;
				else if (value == 1000)
					cur_info.iFlags |= TUnicodeData::EOneThousand;
				else if (value == 5000)
					cur_info.iFlags |= TUnicodeData::EFiveThousand;
				else if (value == 10000)
					cur_info.iFlags |= TUnicodeData::ETenThousand;
				else if (value == 100000)
					cur_info.iFlags |= TUnicodeData::EHundredThousand;
				else
					fprintf(stderr,"Warning: U+%X has a large numeric property with unrepresentable value %d. Ignored.\n",code,value);
				}
			}

		// Field 9: Mirrored property.
		if (field[9][0] == 'Y')
			cur_info.iFlags |= TUnicodeData::EMirrored;

		// Fields 12, 13, 14: Case variants.
		int uc = code, lc = code, tc = code;
		if (field[12][0])
			{
			uc = hex(field[12]);
			int uc_offset = uc - code;
			if (abs(uc_offset) > 32767)
				{
				fprintf(stderr, "Warning: offset to upper case is too large: code %X, upper case %X, offset %X. Ignored!\n", code, uc, uc_offset);
				}
			else
				{
				cur_info.iFlags |= TUnicodeData::EHasUpperCase;
				cur_info.iCaseOffset = (TInt16)(-uc_offset);
				if (code<0x10000 && uc>0x10000 || code>0x10000 && uc<0x10000)
					fprintf(stderr, "Info: %X and its upper case %X locate at different planes.\n");
				}
			}
		if (field[13][0])
			{
			lc = hex(field[13]);
			int lc_offset = lc - code;
			if (abs(lc_offset) > 32767)
				{
				fprintf(stderr, "Warning: offset to lower case is too large: code %X, lower case %X, offset %X. Ignored!\n", code, lc, lc_offset);
				}
			else
				{
				cur_info.iFlags |= TUnicodeData::EHasLowerCase;
				cur_info.iCaseOffset = (TInt16)lc_offset;
				if (code<0x10000 && lc>0x10000 || code>0x10000 && lc<0x10000)
					fprintf(stderr, "Info: %X and its lower case %X locate at different planes.\n");
				}
			}
		if (field[14][0])
			tc = hex(field[14]);
		if (tc != lc && tc != uc)
			cur_info.iFlags |= TUnicodeData::EHasTitleCase;

		// If this code is < 256 fill in the entries in the special tables.
		if (code < 256)
			{
			LowerCaseTable[code] = (TUint16)lc;
			FoldTable[code] = (TUint16)folded_code;
			}

		// If the code is >= 0xFF00 fill in the entry in the CJK width folding table.
		else if (code >= 0xFF00 && code <= 0xFFFF)
			CjkWidthFoldTable[code & 0xFF] = (TUint16)folded_code;

		/*
		If there was a gap between this code and the previous one, write an 'unassigned' range,
		unless this character is actually the end of a range not fully listed (like the CJK ideographs
		from 4E00 to 9FA5 inclusive), in which case the character name will end in ' Last>'.
		*/
		if (code - prev_code > 1)
			{
			TBool last_in_range = false;
			int name_length = strlen(field[1]);
			if (name_length >= 6 && !strcmp(field[1] + name_length - 6," Last>"))
				last_in_range = TRUE;
			if (!last_in_range)
				{
				add_range(unassigned_info,prev_code + 1);
				range_info = unassigned_info;
				}
			}

		// Write the range.
		if (first || cur_info != range_info)
			{
			add_range(cur_info,code);
			range_info = cur_info;
			}

		first = false;
		prev_code = code;
		}

	/*
	If there was a gap at the end of the encoding (there is at present; FFFE and FFFF are not Unicode characters)
	write an 'unassigned' range.
	*/
	if (prev_code < 0xFFFF)
		add_range(unassigned_info,prev_code + 1);

	// Write an array of indices from Unicode character values to character data sets.
	for (int i = 0; i < Ranges; i++)
		{
		TUint32 end = i < Ranges - 1 ? TheRange[i + 1].iRangeStart : 0x110000;
		for (TUint32 j = TheRange[i].iRangeStart; j < end; j++)
			TheIndex[j] = TheRange[i].iIndex;
		}

	// Write the output file.
	write_output();
	printf("\nDone.\n");

	return 0;
	}

Data::Data()
	{
	iCategory = TChar::ECnCategory;
	iBdCategory = TChar::ELeftToRight;
	iCombiningClass = 0;
	iDigitOffset = 0;
	iCaseOffset = 0;
	iFlags = 0;
	}

TBool Data::operator==(const Data& c) const
	{
	return iCategory == c.iCategory &&
		   iBdCategory == c.iBdCategory &&
		   iCombiningClass == c.iCombiningClass &&
		   iDigitOffset == c.iDigitOffset &&
		   iCaseOffset == c.iCaseOffset &&
		   iFlags == c.iFlags;
	}

/*
This function is copied from unicode.cpp: having it here saves me having to link in unicode.cpp and
unitable.cpp, which is probably the file we're trying to write!
*/
TInt TUnicode::Compare(const TUint16 *aString1,TInt aLength1,const TUint16 *aString2,TInt aLength2)
	{
	for (TInt i = 0; i < aLength1 || i < aLength2; i++, aString1++, aString2++)
		{
		TInt x = i < aLength1 ? *aString1 : -1;
		TInt y = i < aLength2 ? *aString2 : -1;
		if (x != y)
			return x - y;
		}
	return 0;
	}
