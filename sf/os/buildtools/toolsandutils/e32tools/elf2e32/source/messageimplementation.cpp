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
// Message Implementation Operations for elf2e32 tool
// @internalComponent
// @released
// 
//


#include "messageimplementation.h"
#include "errorhandler.h"
#include<iostream>
#include<stdarg.h>
#include<string>
#include <stdio.h>

using std::endl;
using std::cout;
typedef std::string String;

char *errorMssgPrefix="elf2e32 : Error: E";
char *warnMssgPrefix="elf2e32 : Warning: W";
char *infoMssgPrefix="elf2e32 : Information: I";
char *colSpace=": ";

enum MessageArraySize{MAX=66};

//Messages stored required for the program
struct EnglishMessage MessageArray[MAX]=
{
	{FILEOPENERROR,"Could not open file : %s."},
	{FILEREADERROR,"Could not read file : %s."},
	{FILEWRITEERROR,"Could not write file : %s."},
	{ELFMAGICERROR,"Invalid ELF magic in file : %s."},
	{ELFCLASSERROR,"ELF file %s is not 32 bit."},
	{ELFABIVERSIONERROR,"ELF file %s is not BPABI conformant."},
	{ELFLEERROR,"ELF file %s is not Little Endian."},
	{ELFARMERROR,"ELF file %s does not target ARM."},
	{ELFEXECUTABLEERROR,"ELF file %s is neither executable (ET_EXEC) or shared (ET_DYN)."},
	{ELFSHSTRINDEXERROR,"Error in ELF Section Header String Index : %s."},
	{NAMELIBRARYNOTCORRECT,"Name or Library not supplied correctly : %s[Line No=%d][%s]"},
	{ORDINALSEQUENCEERROR,"Ordinal number is not in sequence : %s[Line No=%d][%s]."},
	{ARGUMENTNAMEERROR,"Argument %s is not correct."},
	{OPTIONNAMEERROR,"Option %s is Unrecognized."},
	{NOARGUMENTERROR,"Missing arguments for option : %s."},
	{OPTIONPREFIXERROR,"Option %s is neither preceedded by '-' nor '--'."},
	{NOREQUIREDOPTIONERROR,"Missing options : %s."},
	{NOFILENAMEERROR,"Missing argument for option : %s."},
	{INVALIDARGUMENTERROR,"Argument '%s' not permitted for option %s."},
	{HUFFMANBUFFEROVERFLOWERROR,"Huffman buffer overflow during E32Image compression."},
	{HUFFMANTOOMANYCODESERROR,"Too many Huffman codes during E32Image compression."},
	{HUFFMANINVALIDCODINGERROR,"Invalid Huffman coding during E32Image compression."},
	{CAPABILITYALLINVERSIONERROR,"-ALL not a valid capability."},
	{CAPABILITYNONEINVERSIONERROR,"+NONE not a valid capability."},
	{UNRECOGNISEDCAPABILITYERROR,"Unrecognized capability : %s."},
	{NOSTATICSYMBOLSERROR,"ELF File %s contains no static symbols."},
	{DLLHASINITIALISEDDATAERROR,"ELF File %s contains initialized writable data."},
	{DLLHASUNINITIALISEDDATAERROR,"ELF File %s contains uninitialized writable data."},
	{ENTRYPOINTCORRUPTERROR,"ELF File %s has corrupt entry point."},
	{ENTRYPOINTNOTSUPPORTEDERROR,"ELF File %s has unsupported entry point type."},
	{EXCEPTIONDESCRIPTOROUTSIDEROERROR,"ELF File %s has invalid exception descriptor."},
	{NOEXCEPTIONDESCRIPTORERROR,"ELF File %s has no exception descriptor."},
	{NEEDSECTIONVIEWERROR,"ELF File %s has no section headers."},
	{DSONOTFOUNDERROR,"DSO %s not found."},
	{UNDEFINEDSYMBOLERROR,"Undefined Symbol %s found in ELF File %s."},
	{SYMBOLMISSINGFROMELFERROR,"Symbol %s Missing from ELF File : %s."},
	{MEMORYALLOCATIONERROR,"Memory allocation failure : %s."},
	{E32IMAGEERROR,"Not able to write E32 Image file."},
	{INVALIDINVOCATIONERROR,"Invalid invocation of elf2e32."},
	{TARGETTYPENOTSPECIFIEDERROR,"Target Type Not Specified."},
	{UNSUPPORTEDTARGETTYPEERROR,"Unsupported Target Type '%s'."},
	{INDEXNOMESSAGEERROR,"There is no message for the message index[%d]."},
	{INDEXNOTREQUIREDERROR,"Message index[%d] not required in message file."},
	{INDEXNOTFOUNDERROR,"Message index [%d] not found in message file"},
	{NOMESSAGEFILEERROR,"There is no message file."},
	{ENTRYPOINTNOTSETERROR,"Entry point is not set for %s."},
	{UNDEFINEDENTRYPOINTERROR,"Entry point and Text segment base both 0, can't tell if entry point set for %s."},
	{ORDINALNOTANUMBER,"Ordinal not a Number : %s[Line No=%d][%s]."},
	{UNRECOGNIZEDTOKEN,"Unrecognized Token : %s[Line No=%d][%s]."},
	{NONAMEMISSING,"NONAME Missing : %s[Line No=%d][%s]."},
	{EXPORTSEXPECTED,"EXPORTS expected before first export entry : %s[Line No=%d][%s]."},
	{ATRATEMISSING,"@ Missing : %s[Line No=%d][%s]."},
	{SYSDEFSMISMATCHERROR,"Symbol %s passed through '--sysdef' option is not at the specified ordinal in the DEF file %s."},
	{SYSDEFERROR,"Ordinal number is not provided as input to the option: %s"},
	{INVALIDE32IMAGEERROR,"%s is not a valid E32Image file."},
	{HUFFMANBUFFERUNDERFLOWERROR,"Huffman buffer underflow on deflate."},
	{HUFFMANINCONSISTENTSIZEERROR,"Inconsistent sizes discovered during uncompression."},
	{MULTIPLESYSDEFERROR, "Multiple system definitions passed to %s should be separated by ';'"},
	{SYSDEFNOSYMBOLERROR, "Symbol Name is not provided as input to the option: %s"},
	{VALUEIGNOREDWARNING, "Value passed to '%s' is ignored"},
	{ELFFILEERROR,"Error while processing the ELF file %s."},
	{SYMBOLCOUNTMISMATCHERROR, "Symbol count provided by DT_ARM_SYMTABSZ is not same as that in the Hash Table in %s"},
	{POSTLINKERERROR,"Fatal Error in Postlinker"},
	{BYTEPAIRINCONSISTENTSIZEERROR, "Inconsistent sizes discovered during Byte pair uncompression." },
	{ILLEGALEXPORTFROMDATASEGMENT,"'%s' : '%s' Import relocation does not refer to code segment."},
	{VALIDATIONERROR,"Image failed validation"}
};

/**
Constructor to reset the logging option flag.

@internalComponent
@released
*/
MessageImplementation::MessageImplementation()
{
    iLogging = false;
}

/**
Destructor to close log file if logging is enabled and to clear the messaged.
@internalComponent
@released
*/
MessageImplementation::~MessageImplementation()
{
    if(iLogging)
    {
		fclose(iLogPtr);
    }
	iMessage.clear();
}

/**
Function to Get Message stored in map.

@internalComponent
@released

@param aMessageIndex
Index of the Message to be displayed
@return Message string to be displayed
*/
char * MessageImplementation::GetMessageString(int aMessageIndex)
{
	Map::iterator p;

	if(iMessage.empty())
	{
		if(aMessageIndex <= MAX)
		{
			return MessageArray[aMessageIndex-1].message;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		p=iMessage.find(aMessageIndex);
		if(p == iMessage.end())
		{
			if(aMessageIndex <= MAX)
			{
				return MessageArray[aMessageIndex-1].message;
			}
			else
			{
				return NULL;
			}
		}

		if(aMessageIndex <= MAX)
		{
			return p->second;
		}
		else
		{
			return NULL;
		}
	}
}

/**
Function to display output and log message in log file if logging is enable.

@internalComponent
@released

@param aString
Message to be displayed
*/
void MessageImplementation::Output(const char *aString)
{

    if (iLogging)
    {
		fputs(aString,iLogPtr);
		fputs("\n",iLogPtr);
    }
	cout << aString << endl;
}

/**
Function to Get Message stored in map and to display the Message

@internalComponent
@released

@param
The type of the message, whether it is Error or Warning or Information.
@param
The index of the information and the corresponding arguments.
*/
void MessageImplementation::ReportMessage(int aMessageType, int aMsgIndex,...)
{
	char *reportMessage, *ptr, *tmpMessage;
	char intStr[16];
	char mssgNo[MAXMSSGNOLENGTH];
	int mssgIndex,k;

	va_list ap;
	va_start(ap,aMsgIndex);
	
	reportMessage=GetMessageString(aMsgIndex);
	
	if(reportMessage)
	{
		String message;
		switch (aMessageType)
		{
			case ERROR:
				message = errorMssgPrefix;
				break;
			case WARNING:
				message = warnMssgPrefix;
				break;
			case INFORMATION:
				message = infoMssgPrefix;
				break;
		}
		mssgIndex = BASEMSSGNO + aMsgIndex;
		sprintf(mssgNo,"%d",mssgIndex);
		message += mssgNo;
		message += colSpace;

		ptr = strchr(reportMessage,'%');

		while( ptr != NULL && (ptr[0]) == '%' )
		{
			tmpMessage=new char[ptr - reportMessage + 1];
			strncpy(tmpMessage, reportMessage, ptr - reportMessage+1);
			tmpMessage[ptr - reportMessage]='\0';
			message += tmpMessage;
			delete tmpMessage;
			ptr++;
			switch(ptr[0])
			{
				case 'd':
					k = va_arg(ap, int);
					sprintf(intStr,"%d",k);
					message += intStr;
					ptr++;
					reportMessage = ptr;
					break;
				case 's':
					message += va_arg(ap, char *);
					ptr++;
					reportMessage = ptr;
					break;
				case '%':
					message += ptr[0];
					reportMessage = ptr;
				default:
					break;
			}
			ptr=strchr(reportMessage,'%');
		}
		message += reportMessage;
		Output(message.c_str());
	}
}

/**
Function to start logging.

@internalComponent
@released

@param aFileName
Name of the Log file
*/
void MessageImplementation::StartLogging(char *aFileName)
{
	char logFile[1024];
	FILE *fptr;

	strcpy(logFile,aFileName);

	// open file for log etc.
	if((fptr=fopen(logFile,"w"))==NULL)
	{
		ReportMessage(WARNING, FILEOPENERROR,aFileName);
	}
	else
	{
	    iLogging = true;
		iLogPtr=fptr;
	}
}

/**
Function to Create Messages file.

@internalComponent
@released
@param aFileName
Name of the Message file to be dumped
*/
void MessageImplementation::CreateMessageFile(char *aFileName)
{
	int i;
	FILE *fptr;

	// open file for writing messages.
	if((fptr=fopen(aFileName,"w"))==NULL)
	{
		ReportMessage(WARNING, FILEOPENERROR,aFileName);
	}
	else
	{
		for(i=0;i<MAX;i++)
		{
			fprintf(fptr,"%d,%s\n",i+1,MessageArray[i].message);
		}
			
		fclose(fptr);
	}

}

/**
Function to put Message string in map which is stored in message file.
If file is not available the put message in map from Message Array structure.

@internalComponent
@released

@param aFileName
Name of the Message file passed in
*/
void MessageImplementation::InitializeMessages(char *aFileName)
{
	char index[16];
	char *message, *errStr;
	int i, lineLength;
	int fileSize;
	char *messageEntries, *lineToken;

	FILE *fptr;

	if(aFileName && (fptr=fopen(aFileName,"rb"))!=NULL)
	{

		iMessage.clear();
		//Getting File size
		fseek(fptr, 0, SEEK_END);
		fileSize=ftell(fptr);
		rewind(fptr);

		messageEntries= new char[fileSize+2];

		//Getting whole file in memory
		fread(messageEntries, fileSize, 1, fptr);

		//Adding ENTER at end
		*(messageEntries+fileSize)='\n';
		//Adding '\0' at end
		*(messageEntries+fileSize+1)='\0';

		fclose(fptr);

		lineToken=strtok(messageEntries,"\n");
		while(lineToken != NULL)
		{
			if( lineToken[0] == '\n' || lineToken[0] == '\r' )
			{
				lineToken=strtok(NULL,"\n");
				continue;
			}

			lineLength=strlen(lineToken);

			if( lineToken[strlen(lineToken)-1] == '\r' )
			{
				lineToken[strlen(lineToken)-1]='\0';
			}

			message=strchr(lineToken,',');
			strncpy(index,lineToken,message-lineToken);
			index[message-lineToken]='\0';
			errStr = new char[strlen(message+1) + 1];
			strcpy(errStr, (message+1));
			iMessage.insert(std::pair<int,char*>(atoi(index),errStr));

			lineToken=strtok(lineToken+lineLength+1,"\n");
		}

		delete messageEntries;

	}
	else
	{
		for(i=0;i<MAX;i++)
		{
			errStr = new char[strlen(MessageArray[i].message) + 1];
			strcpy(errStr, MessageArray[i].message);
			iMessage.insert(std::pair<int,char*>(MessageArray[i].index,errStr));
		}
	}
}


