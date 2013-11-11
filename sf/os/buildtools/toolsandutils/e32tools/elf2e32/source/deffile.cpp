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
// Implementation of the Class DefFile for the elf2e32 tool
// @internalComponent
// @released
// 
//

//
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "pl_symbol.h"
#include "deffile.h"
#include "errorhandler.h"

#ifdef __LINUX__
    #include "h_utl.h"
    #define STRUPR strupr
#else 
    #define STRUPR _strupr
#endif

using std::cerr;
using std::cout;
using std::endl;

#define SUCCESS 1
#define FAILURE 0

/**
Destructor to release all the allocated memory
@internalComponent
@released
*/
DefFile::~DefFile()
{
	if(iSymbolList && iSymbolList->size())
	{
		SymbolList::iterator aItr = iSymbolList->begin();
		SymbolList::iterator last = iSymbolList->end();
		Symbol *temp;

		while(aItr != last)
		{
			temp = *aItr;
			aItr++;
			delete temp;
		}
		iSymbolList->clear();
	}
	delete iSymbolList;
}
/**
Function to Get File Size.
@param fptrDef - File pointer to DEF file
@internalComponent
@released
*/
int DefFile::GetFileSize(FILE *fptrDef)
{
	int fileSize,status;

	status=fseek(fptrDef, 0, SEEK_END);
	if(status!=0)
	{
		throw FileError(FILEREADERROR,iFileName);
	}
	fileSize=ftell(fptrDef);
	rewind(fptrDef);

	return fileSize;

}

/**
Function to Open File and read it in memory.
@param defFile - DEF File name
@internalComponent
@released
*/
char* DefFile::OpenDefFile(char * defFile)
{
	int fileSize;
	char *defFileEntries;
	FILE *fptrDef;

	iFileName=defFile;
	if((fptrDef=fopen(defFile,"rb"))==NULL)
	{
		throw FileError(FILEOPENERROR,defFile);
	}

	fileSize=GetFileSize(fptrDef);

	if((defFileEntries= new char[fileSize+2]) ==NULL)
	{
		throw MemoryAllocationError(MEMORYALLOCATIONERROR,defFile);
	}

	//Getting whole file in memory
	if(!fread(defFileEntries, fileSize, 1, fptrDef))
	{
		throw FileError(FILEREADERROR,defFile);
	}

	//Adding ENTER at end
	*(defFileEntries+fileSize)='\n';
	//Adding '\0' at end
	*(defFileEntries+fileSize+1)='\0';

	fclose(fptrDef);

	return defFileEntries;

}

/**
Function to Parse Def File which has been read in buffer.
@param defFileEntries - pointer to def file entries which has been read in buffer
@internalComponent
@released
*/
void DefFile::ParseDefFile(char *defFileEntries)
{
	iSymbolList = new SymbolList;

	int ordinalNo = 0;
	int symbolType=SymbolTypeCode;
	int PreviousOrdinal=0;
	char MultiLineStatement[1024]="";
	bool NAMEorLIBRARYallowed=true;
	int LineNum = 0;
	bool isComment;

	char *lineToken;
	int aLineLength = 0, width = 0;
	unsigned i;
	char *ptrEntry,*ptrEntryType;
	char entryType[256];
	bool entryFlag;


	lineToken=strtok(defFileEntries,"\n");
	while(lineToken != NULL)
	{
		symbolType=SymbolTypeCode;
		isComment=false;
		entryType[0]='\0';
		aLineLength = strlen(lineToken);
		LineNum++;

		if (lineToken == NULL || lineToken[0]==13)
		{
			lineToken=strtok(NULL,"\n");
			continue;
		}

		// comment lines
		if (lineToken[0] == ';')
		{
			lineToken=strtok(NULL,"\n");
			continue;
		}

		ptrEntry=lineToken;

		if((!strstr(lineToken, "NONAME") && ((ptrEntryType=strstr(lineToken, "NAME")) != NULL)) ||
			((ptrEntryType=strstr(lineToken, "EXPORTS")) != NULL) ||
			((ptrEntryType=strstr(lineToken, "IMPORTS")) != NULL) ||
			((ptrEntryType=strstr(lineToken, "SECTIONS")) != NULL) ||
			((ptrEntryType=strstr(lineToken, "LIBRARY")) != NULL) ||
			((ptrEntryType=strstr(lineToken, "DESCRIPTION")) != NULL)||
			((ptrEntryType=strstr(lineToken, "STACKSIZE")) != NULL)||
			((ptrEntryType=strstr(lineToken, "VERSION")) != NULL)
			)
		{
			entryFlag=true;

			for(i=0; ptrEntry!=ptrEntryType; i++,ptrEntry++)
			{
				switch(lineToken[i])
				{
				case ' ':
				case '\t':
					continue;
				default:
					entryFlag=false;
					break;
				}
				if(entryFlag==false)
					break;
			}

			if(entryFlag==false && !strcmp(MultiLineStatement,""))
			{
				throw DEFFileError(UNRECOGNIZEDTOKEN,iFileName,LineNum,lineToken);
			}

			if(entryFlag==true)
			{
				switch(ptrEntryType[0])
				{
				case 'E':
				case 'I':
				case 'L':
				case 'V':
					width=7;
					break;
				case 'S':
					if(ptrEntryType[1]=='E')
						width=8;
					else
						width=9;
					break;
				case 'N':
					width=4;
					break;
				case 'D':
					width=11;
					break;
				}
			}

			if(entryFlag==true)
			{
  				for(i=i+width; i<strlen(lineToken); i++)
				{
					switch(lineToken[i])
					{
					case ' ':
					case '\t':
						continue;
					case '\r':
					case '\0':
						break;
					default:
						entryFlag=false;
						break;
					}

					if(entryFlag==false)
						break;
				}
			}

			if(entryFlag==false && !strcmp(MultiLineStatement,""))
			{
				throw DEFFileError(UNRECOGNIZEDTOKEN,iFileName,LineNum,lineToken+i);
			}

			if(entryFlag==true)
			{
				strncpy(entryType, ptrEntryType, width);
				entryType[width]='\0';

				switch(ptrEntryType[0])
				{
				case 'E':		// check for multi-line sections starting
					strcpy(MultiLineStatement, STRUPR(entryType)); // Uppercase token
					NAMEorLIBRARYallowed = false;
					lineToken = strtok(NULL, "\n" ); // Get the next line
					continue;
				case 'N':
					break;
				case 'L':
					break;
				case 'D':
					break;
				case 'S':
				case 'V':
					if(entryType[1]!='E')
					{
						// set MULTI-LINE statement to OFF
						strcpy(MultiLineStatement, STRUPR(entryType)); // Uppercase token
						// check single-line statements are specified correctly
						// check NAME or LIBRARY statements aren't supplied incorrectly
						if (!strcmp(entryType, "NAME") ||
							!strcmp(entryType, "LIBRARY")
							)
						{
							if (NAMEorLIBRARYallowed == false)
							{
								throw DEFFileError(NAMELIBRARYNOTCORRECT,iFileName,LineNum,lineToken);
							}
							lineToken=strtok(NULL,"\n");
							continue;
						}
						NAMEorLIBRARYallowed = false;
						lineToken=strtok(NULL,"\n");
						continue;
					}
					continue;
				case 'I':
					strcpy(MultiLineStatement, STRUPR(entryType)); // Uppercase token
					lineToken = strtok(NULL, "\n" ); // Get the next line
					continue;
				}
			}

		}
		else
		{
			if (!strcmp(MultiLineStatement,""))
		    {
				throw DEFFileError(EXPORTSEXPECTED,iFileName,LineNum,lineToken);
			}
		}	
			
		// Get Export entries
		if (!strcmp(MultiLineStatement,"EXPORTS"))
		{
			Symbol aSymbol(NULL, SymbolTypeCode);
			if( Tokenize(lineToken, LineNum, aSymbol) )
			{
				Symbol *newSymbolEntry = new Symbol(aSymbol);
				iSymbolList->push_back(newSymbolEntry);

				ordinalNo = aSymbol.OrdNum();
				//Check for ordinal sequence
				if (ordinalNo != PreviousOrdinal+1)
		   		{
					throw DEFFileError(ORDINALSEQUENCEERROR,iFileName,LineNum,(char*)aSymbol.SymbolName());
				}

				PreviousOrdinal = ordinalNo;

			}
			lineToken=strtok(NULL,"\n");
			continue;
		}
		else if(strcmp(MultiLineStatement,"")!=0)//For entry other than exports
			lineToken = strtok(NULL, "\n" ); // Get the next line

	}//End of while
}

/**
This Function calls LineToken's Tokenize function.
@param aTokens   - Input string at the current line number
@param aLineNum  - Current line number
@param aSymbol   - Symbol to be populated while parsing the line.
Return value     - It returns true if a valid def file entry is found.
@internalComponent
@released
*/
bool DefFile::Tokenize(char* aTokens, int aLineNum, Symbol& aSymbol)
{
	/*
	 * Pattern to match is:
	 * START\s*(\S+)\s+@\s*(d+)\s*(NONAME)?\s*(R3UNUSED)?\s*(ABSENT)?\s*(;\s*(.*))END
	 */
	LineToken aLine(iFileName, aLineNum, aTokens, &aSymbol);
	return aLine.Tokenize();
}


char * DefFilePatterns[] =
	{
		"NONAME",//index 0
		"DATA",
		"R3UNUSED",
		"ABSENT"
	};

#define DEF_NONAME		0
#define DEF_DATA		1
#define DEF_R3UNUSED	2
#define DEF_ABSENT		3

/**
This constructor creates an instance of LineToken class.
@param aFileName - Def File Name.
@param aLineNum  - Current line number
@param aLn       - Input string at the current line number
@param aSym      - Symbol to be populated while parsing the line.
@internalComponent
@released
*/
LineToken::LineToken(char* aFileName, int aLineNum, char *aLn, Symbol* aSym) : \
		iLine(aLn) , iSymbol(aSym) , iOffset(0), iState(EInitState),iFileName(aFileName), iLineNum(aLineNum) 
{
}

/**
This function tokenizes the line and populates a symbol entry 
if there is one.
Return Value - True, if the current line has a valid def entry.
@internalComponent
@released
*/
bool LineToken::Tokenize()
{
	while (1) 
	{
		switch( iState )
		{
		case EFinalState:
			return true;
		case EInitState:
			if( *(iLine + iOffset) == '\0' || 
				*(iLine + iOffset) == '\r' || 
				*(iLine + iOffset) == '\n')
			{
				/*
				 * Skip empty lines.
				 */
				return false;
			}
			else
			{
				NextToken();
			}
			break;
		default:
			NextToken();
			break;
		}
	}
	return false;
}

/**
This function parses a line of the def file based on the current 
state it is in.
@internalComponent
@released
*/
void LineToken::NextToken()
{
	int aCurrentPos = 0;
	char *aSymbolName;

	switch( iState )
	{
	case EInitState:
		if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
		{
			IncrOffset(aCurrentPos);
		}

		if(IsWord(iLine + iOffset, aCurrentPos))
		{
			SetState(ESymbolName);
		}
	break;

	case ESymbolName:
	{
		// Get the length of the symbol
		IsWord(iLine + iOffset, aCurrentPos);

		char *cmt = strchr(iLine + iOffset, ';');
		char *aAlias = strchr(iLine + iOffset, '=');

		if( aAlias && (!cmt || (aAlias < cmt)) )
		{
			int aAliasPos = aAlias - (iLine+ iOffset);

			//Check if alias name is also supplied, they should be separated 
			// by whitespace, i.e., SymbolName=AliasName is valid while, 
			// SymbolName =AliasName is invalid.
			if( aAliasPos > aCurrentPos)
			{
				char *aToken = (iLine + iOffset + aCurrentPos);
				throw DEFFileError(UNRECOGNIZEDTOKEN, iFileName, iLineNum, aToken);
			}

			aSymbolName = new char[aAliasPos+1];
			strncpy(aSymbolName, iLine + iOffset, aAliasPos);
			aSymbolName[aAliasPos] = '\0';
			char *aExportName = new char[aCurrentPos - aAliasPos + 1];
			strncpy(aExportName, aAlias +1, (aCurrentPos - aAliasPos));
			aExportName[(aCurrentPos - aAliasPos)] = '\0';
			iSymbol->ExportName(aExportName);
		}
		else
		{
			aSymbolName = new char[aCurrentPos+1];
			strncpy(aSymbolName, iLine+ iOffset, aCurrentPos);
			aSymbolName[aCurrentPos] = '\0';
		}
		iSymbol->SymbolName(aSymbolName);
		
		IncrOffset(aCurrentPos);

		if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
		{
			IncrOffset(aCurrentPos);
		}
		
		if(*(iLine+iOffset) == '@')
		{
			SetState(EAtSymbol);
			IncrOffset(1);
		}
		else
		{
			/*
			 * The first non-whitespace entry in a line is assumed to be the 
			 * symbol name and a symbol name might also have a '@' char. Hence
			 * there MUST be a whitespace between symbol name and '@'.
			 */
			throw DEFFileError(ATRATEMISSING,iFileName,iLineNum,(iLine+iOffset));
		}
	}
	break;

	case EAtSymbol:
		if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
		{
			IncrOffset(aCurrentPos);
		}
		
		SetState(EOrdinal);
		break;
	case EOrdinal:
	{
		if(!IsDigit(iLine+iOffset, aCurrentPos ) )
		{
			throw DEFFileError(ORDINALNOTANUMBER, iFileName, iLineNum, (iLine+iOffset));
		}
		char aTmp[32];
		strncpy(aTmp, iLine+iOffset, aCurrentPos);
		aTmp[aCurrentPos] = '\0';
		int aOrdinal = atoi(aTmp);
		iSymbol->SetOrdinal(aOrdinal);

		IncrOffset(aCurrentPos);

		if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
		{
			IncrOffset(aCurrentPos);
		}
		SetState(EOptionals);
	}
	break;

	case EOptionals:
		{
		int aPrevPatternIndex, aPatternIdx = 0;
		aPrevPatternIndex = -1;
		while (*(iLine + iOffset) != '\n' || *(iLine + iOffset) != '\r')
		{
			if(IsPattern(iLine+iOffset, aCurrentPos, aPatternIdx) )
			{
				switch(aPatternIdx)
				{
				case DEF_NONAME:
					break;
				case DEF_DATA:
					iSymbol->CodeDataType(SymbolTypeData);

					IncrOffset(aCurrentPos);
					if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
					{
						IncrOffset(aCurrentPos);
					}
					if(IsDigit(iLine+iOffset, aCurrentPos ) )
					{
						char aSymSz[16];
						strncpy(aSymSz, (iLine + iOffset), aCurrentPos);
						aSymSz[aCurrentPos] = '\0';
						iSymbol->SetSymbolSize(atoi(aSymSz));
					}
					break;
				case DEF_R3UNUSED:
					iSymbol->R3Unused(true);
					break;
				case DEF_ABSENT:
					iSymbol->SetAbsent(true);
					break;
				default:
					break;
				}

				/*
				 * NONAME , DATA, R3UNUSED and ABSENT, all the 3 are optional. But, if more than
				 * one of them appear, they MUST appear in that order. 
				 * Else, it is not accepted.
				 */
				if( aPrevPatternIndex >= aPatternIdx)
				{
					throw DEFFileError(UNRECOGNIZEDTOKEN, iFileName, iLineNum,(iLine + iOffset));
				}
				aPrevPatternIndex = aPatternIdx;

				IncrOffset(aCurrentPos);
				
				if(IsWhiteSpace((iLine + iOffset), aCurrentPos))
				{
					IncrOffset(aCurrentPos);
				}
			}
			else
			{
				if( *(iLine + iOffset) == ';' )
				{
					SetState(EComment);
					IncrOffset(1);
					return;
				}
				else if( *(iLine + iOffset) == '\0' || 
						 *(iLine + iOffset) == '\r' || 
						 *(iLine + iOffset) == '\n')
				{
					SetState(EFinalState);
					return;
				}
				else
				{
					throw DEFFileError(UNRECOGNIZEDTOKEN, iFileName, iLineNum,(iLine + iOffset));
				}
			}
		}
		}
	break;

	case EComment:
	{
		if(IsWhiteSpace(iLine + iOffset, aCurrentPos))
		{
			IncrOffset(aCurrentPos);
		}
		
	
		int aLen = strlen(iLine + iOffset);
		if( *(iLine + iOffset + aLen - 1 ) == '\n' ||  *(iLine + iOffset + aLen - 1 ) == '\r')
			aLen -=1;

		char * aComment = new char[ aLen + 1];
		strncpy( aComment, iLine + iOffset, aLen);
		aComment[aLen] = '\0';

		IncrOffset(aLen);

		iSymbol->Comment(aComment);
		SetState(EFinalState);
	}
	break;

	case EFinalState:
		return;
	default:
		break;	
	}
}

/**
This function returns true if the string starts with one
of the fixed patterns.
It also updates the length  and index of this pattern.
@param aChar - Line Token
@param aTill - Length of the pattern
@param aTill - Index of the pattern
Return Value - True, if the string starts with one of the patterns.
@internalComponent
@released
*/
bool LineToken::IsPattern(char* aStr, int& aTill, int& aIndex)
{
	int pos = 0;
	int aLength;
	int size = sizeof(DefFilePatterns)/sizeof(char*);
	while(size > pos)
	{
		aLength = strlen(DefFilePatterns[pos]);
		if(!strncmp(aStr, DefFilePatterns[pos], aLength))
		{
			aTill = aLength;
			aIndex = pos;
			return true;
		}
		pos++;
	}
	return false;
}

/**
This function returns true if the string starts with digits.
It also updates the length of this digit string.
@param aChar - Line Token
@param aTill - Length of the digit string
Return Value - True, if the string starts with digit(s)
@internalComponent
@released
*/
bool LineToken::IsDigit(char *aChar, int &aTill)
{
	int pos = 0;
	if( aChar[pos] - '0' >= 0 &&  aChar[pos] - '0' <= 9)
	{
		pos++;
		while(aChar[pos] - '0' >= 0 &&  aChar[pos] - '0' <= 9)
		{
			pos++;
		}
		aTill = pos;
		return true;
	}
	else
	{
		return false;
	}
}

/**
This function returns true if the string starts with white space.
It also updates the length of this white string!
@param aStr - Line Token
@param aTill - Length of the white string
Return Value - True, if the string starts with whitespace
@internalComponent
@released
*/
bool LineToken::IsWhiteSpace(char *aStr, int &aTill)
{
	int pos = 0;
	switch( aStr[pos] )
	{
	case ' ':
	case '\t':
		break;
	default:
		return false;
	}

	pos++;
	while( aStr[pos])
	{
		switch(aStr[pos])
		{
			case ' ':
			case '\t':
				pos++;
			break;
			default:
				aTill = pos;
				return true;
		}

	}
	aTill = pos;
	return true;
}

/**
This function returns true if the string starts with non-whitespace.
It also updates the length of this word.
@param aStr  - Line Token
@param aTill - Length of the word
Return Value - True, if the string starts with non-whitespace chars.
It also updates the length of the word.
@internalComponent
@released
*/
bool LineToken::IsWord(char *aStr, int &aTill)
{
	int pos = 0;
	switch( aStr[pos] )
	{
	case '\0':
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return false;
	default:
		break;
	}

	pos++;
	while( aStr[pos])
	{
		switch(aStr[pos])
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				aTill = pos;
				return true;
			default:
				pos++;
				break;
		}

	}
	aTill = pos;
	return true;
}

/**
This function increments the current offset.
@param aOff  - increment by this value
@internalComponent
@released
*/
void LineToken::IncrOffset(int aOff)
{ 
	iOffset += aOff;
}

/**
This function sets the state of the tokenizer that is parsing 
the line.
@param aState  - next state
@internalComponent
@released
*/
void LineToken::SetState(DefStates aState)
{
	iState = aState;
}

/**
Function to Read def file and get the internal representation in structure.
@param defFile - DEF File name
@internalComponent
@released
*/
SymbolList* DefFile::ReadDefFile(char *defFile)
{
	char *defFileEntries;

	defFileEntries=OpenDefFile(defFile);
	ParseDefFile(defFileEntries);

	delete [] defFileEntries;//Free the memory which was required to read def file

	return iSymbolList;

}

/**
Function to get the internal representation of Def File.
@param defFile - DEF File name
@internalComponent
@released
*/
SymbolList* DefFile::GetSymbolEntryList(char *defFile)
{
	if(iSymbolList)
	{
		return iSymbolList;
	}
	else
	{
		iSymbolList=ReadDefFile(defFile);
		return iSymbolList;
	}

}

/**
Function to write DEF file from symbol entry List.
@param fileName - Def file name
@param newSymbolList - pointer to SymbolList which we get as an input for writing in DEF File
@internalComponent
@released
*/
void DefFile::WriteDefFile(char *fileName, SymbolList * newSymbolList)
{

	char ordinal[6];
	int newDefEntry=0;
	FILE *fptr;

	if((fptr=fopen(fileName,"wb"))==NULL)
	{
		throw FileError(FILEOPENERROR,fileName);
	}
	else
	{
		SymbolList::iterator aItr = newSymbolList->begin();
		SymbolList::iterator last = newSymbolList->end();
		Symbol *aSym;

		fputs("EXPORTS",fptr);
		fputs("\r\n",fptr);
		while( aItr != last)
		{
			aSym = *aItr;
			//Do not write now if its a new entry
			if(aSym->GetSymbolStatus()==New)
			{
				newDefEntry=1;
				aItr++;
				continue;
			}

			//Make it comment if its missing def entry
			if(aSym->GetSymbolStatus()==Missing)
				fputs("; MISSING:",fptr);

			fputs("\t",fptr);
			if((aSym->ExportName()) && strcmp(aSym->SymbolName(),aSym->ExportName())!=0)
			{
				fputs(aSym->ExportName(),fptr);
				fputs("=",fptr);
			}
			fputs(aSym->SymbolName(),fptr);
			fputs(" @ ",fptr);
			sprintf(ordinal,"%d",aSym->OrdNum());
			fputs(ordinal,fptr);
			fputs(" NONAME",fptr);
			if(aSym->CodeDataType()==SymbolTypeData) {
				fputs(" DATA",fptr);
				fputs(" ",fptr);
				char aSymSize[16];
				sprintf(aSymSize, "%d", aSym->SymbolSize());
				fputs(aSymSize,fptr);
			}
			if(aSym->R3unused())
				fputs(" R3UNUSED",fptr);
			if(aSym->Absent())
				fputs(" ABSENT",fptr);
				
			if(aSym->Comment()!=NULL)
			{
				fputs(" ; ",fptr);
				fputs(aSym->Comment(),fptr);
			}
			fputs("\r\n",fptr);
			aItr++;
		}

		//This is for writing new def entry in DEF File
		if(newDefEntry)
		{
			fputs("; NEW:",fptr);
			fputs("\r\n",fptr);
			aItr = newSymbolList->begin();
			last = newSymbolList->end();

			while( aItr != last)
			{
				aSym = *aItr;
				if(aSym->GetSymbolStatus()!=New)
				{
					aItr++;
					continue;
				}
				fputs("\t",fptr);
				if((aSym->ExportName()) && strcmp(aSym->SymbolName(),aSym->ExportName())!=0)
				{
					fputs(aSym->ExportName(),fptr);
					fputs("=",fptr);
				}
				fputs(aSym->SymbolName(),fptr);
				fputs(" @ ",fptr);
				sprintf(ordinal,"%d",aSym->OrdNum());
				fputs(ordinal,fptr);
				fputs(" NONAME",fptr);

				if(aSym->CodeDataType()==SymbolTypeData) {
					fputs(" DATA",fptr);
					fputs(" ",fptr);
					char aSymSize[16];
					sprintf(aSymSize, "%d", aSym->SymbolSize());
					fputs(aSymSize,fptr);
				}

				if(aSym->R3unused())
					fputs(" R3UNUSED",fptr);
				if(aSym->Absent())
					fputs(" ABSENT",fptr);

				if(aSym->Comment()!=NULL)
				{
					if(aSym->CodeDataType()!=SymbolTypeCode &&
						aSym->CodeDataType()!=SymbolTypeData)
					{
						fputs(" ; ",fptr);
						fputs(aSym->Comment(),fptr);
					}
					else
					{
						fputs(" ",fptr);
						fputs(aSym->Comment(),fptr);
					}
				}
				fputs("\r\n",fptr);
				aItr++;
			}
		}
		fputs("\r\n",fptr);
		fclose(fptr);
	}
}
