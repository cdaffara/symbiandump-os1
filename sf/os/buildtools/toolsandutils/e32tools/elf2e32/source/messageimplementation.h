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
// Message Implementation Class for elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef _MESSAGE_IMPLEMENTATION_
#define _MESSAGE_IMPLEMENTATION_

#ifdef _MSC_VER 
	#pragma warning(disable: 4514) // unreferenced inline function has been removed
	#pragma warning(disable: 4702) // unreachable code
	#pragma warning(disable: 4710) // function not inlined
	#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information
#endif

#include<map>
#include<string>

typedef std::map<int,char*> Map;
typedef std::string String;

enum { MAXMSSGNOLENGTH=5, BASEMSSGNO=1000 };

enum { ERROR = 0,
	   WARNING,
	   INFORMATION
};

enum {	FILEOPENERROR=1,
		FILEREADERROR,
		FILEWRITEERROR,
		ELFMAGICERROR,
		ELFCLASSERROR,
		ELFABIVERSIONERROR,
		ELFLEERROR,
		ELFARMERROR,
		ELFEXECUTABLEERROR,
		ELFSHSTRINDEXERROR,
		NAMELIBRARYNOTCORRECT,
		ORDINALSEQUENCEERROR,
		ARGUMENTNAMEERROR,
		OPTIONNAMEERROR,
		NOARGUMENTERROR,
		OPTIONPREFIXERROR,
		NOREQUIREDOPTIONERROR,
		NOFILENAMEERROR,
		INVALIDARGUMENTERROR,
		HUFFMANBUFFEROVERFLOWERROR,
		HUFFMANTOOMANYCODESERROR,
		HUFFMANINVALIDCODINGERROR,
		CAPABILITYALLINVERSIONERROR,
		CAPABILITYNONEINVERSIONERROR,
		UNRECOGNISEDCAPABILITYERROR,
		NOSTATICSYMBOLSERROR,
		DLLHASINITIALISEDDATAERROR,
		DLLHASUNINITIALISEDDATAERROR,
		ENTRYPOINTCORRUPTERROR,
		ENTRYPOINTNOTSUPPORTEDERROR,
		EXCEPTIONDESCRIPTOROUTSIDEROERROR,
		NOEXCEPTIONDESCRIPTORERROR,
		NEEDSECTIONVIEWERROR,
		DSONOTFOUNDERROR,
		UNDEFINEDSYMBOLERROR,
		SYMBOLMISSINGFROMELFERROR,
		MEMORYALLOCATIONERROR,
		E32IMAGEERROR,
		INVALIDINVOCATIONERROR,
		TARGETTYPENOTSPECIFIEDERROR,
		UNSUPPORTEDTARGETTYPEERROR,
		INDEXNOMESSAGEERROR,
		INDEXNOTREQUIREDERROR,
		INDEXNOTFOUNDERROR,
		NOMESSAGEFILEERROR,
		ENTRYPOINTNOTSETERROR,
		UNDEFINEDENTRYPOINTERROR,
		ORDINALNOTANUMBER,
		UNRECOGNIZEDTOKEN,
		NONAMEMISSING,
		EXPORTSEXPECTED,
		ATRATEMISSING,
		SYSDEFSMISMATCHERROR,
		SYSDEFERROR,
		INVALIDE32IMAGEERROR,
		HUFFMANBUFFERUNDERFLOWERROR,
		HUFFMANINCONSISTENTSIZEERROR,
		MULTIPLESYSDEFERROR,
		SYSDEFNOSYMBOLERROR,
		VALUEIGNOREDWARNING,
		ELFFILEERROR,
		SYMBOLCOUNTMISMATCHERROR,
		POSTLINKERERROR,
		BYTEPAIRINCONSISTENTSIZEERROR,
		ILLEGALEXPORTFROMDATASEGMENT,
		VALIDATIONERROR
};


/**
Abstract base Class for Message Implementation.
@internalComponent
@released
*/
class Message
{
    public:
		virtual ~Message(){};
		// get error string from message file
		virtual char * GetMessageString(int errorIndex)=0;
		// display message to output device
		virtual void Output(const char *aName) =0;
		// start logging to a file
		virtual void StartLogging(char *fileName)=0;
	//	virtual void ReportWarning(int warnIndex,...)=0;
		virtual void ReportMessage(int aMsgType, int aMsgIndex,...)=0;
		virtual void CreateMessageFile(char *fileName)=0;
		virtual void InitializeMessages(char *fileName)=0;
};

/**
Class for Message Implementation.
@internalComponent
@released
*/
class MessageImplementation : public Message
{
    public:
		MessageImplementation();
		~MessageImplementation();

		//override base class methods
		char* GetMessageString(int errorIndex);
		void Output(const char *aName);
		void StartLogging(char *fileName);
	//	void ReportWarning(int warnIndex,...);
		void ReportMessage(int aMsgType, int aMsgIndex,...);
		void CreateMessageFile(char *fileName);
		void InitializeMessages(char *fileName);
    private:

		bool iLogging;
		char* iLogFileName;
		FILE *iLogPtr;
		Map iMessage;
};

/**
Structure for Messages.
@internalComponent
@released
*/
struct EnglishMessage
{
	int index;
	char message[1024];
};

#endif

