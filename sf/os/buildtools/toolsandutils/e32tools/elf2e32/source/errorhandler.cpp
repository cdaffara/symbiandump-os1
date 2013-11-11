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
// Error Handler Operations for elf2e32 tool
// @internalComponent
// @released
// 
//

#ifdef _MSC_VER
	#pragma warning(disable: 4514) // unreferenced inline function has been removed
	#pragma warning(disable: 4702) // unreachable code
	#pragma warning(disable: 4710) // function not inlined
#endif

#include "errorhandler.h"
#include "messagehandler.h"
#include <iostream>
#include <stdio.h>
using std::cerr;
using std::endl;

char *errMssgPrefix="elf2e32 : Error: E";
char *colonSpace=": ";

/**
ErrorHandler constructor for doing common thing required for derived class functions.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
ErrorHandler::ErrorHandler(int aMessageIndex)
{
	char mssgNo[MAXMSSGNOLENGTH];
	int mssgIndex;
	
	iMessageIndex=aMessageIndex;
	iMessage=errMssgPrefix;
	mssgIndex=BASEMSSGNO+iMessageIndex;
	sprintf(mssgNo,"%d",mssgIndex);
	iMessage+=mssgNo;
	iMessage+=colonSpace;
};

/**
ErrorHandler destructor.
@internalComponent
@released
*/
ErrorHandler::~ErrorHandler()
{
	MessageHandler::CleanUp();
}

/**
FileError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - File name
@internalComponent
@released
*/
FileError:: FileError(int aMessageIndex, char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{
}

/**
FileError destructor.
@internalComponent
@released
*/
FileError:: ~FileError()
{
}

/**
Function to report File Errors.
@internalComponent
@released
*/
void FileError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete[] tempMssg;
	}
}

/**
ELFFormatError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - ELF File name
@internalComponent
@released
*/
ELFFormatError::ELFFormatError(int aMessageIndex, char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{

}

/**
ELFFormatError destructor.
@internalComponent
@released
*/
ELFFormatError::~ELFFormatError()
{

}

/**
Function to report ELF Format Errors.
@internalComponent
@released
*/
void ELFFormatError::Report()
{
	char *tempMssg;
	char *errMessage;
	
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
DEFFileError constructor for initializing message index, argument name and line number and token.
@param aMessageIndex - Message Index
@param aName - DEF File name
@param aLineNo - DEF File line number
@param aToken - Token in export entry
@internalComponent
@released
*/
DEFFileError::DEFFileError(int aMessageIndex, char * aName, int aLineNo,char * aToken) : ErrorHandler(aMessageIndex), iName(aName), iLineNo(aLineNo)
{
	iToken=aToken;
	if(iToken[iToken.size()-1]=='\r')
		iToken[iToken.size()-1]='\0';
}

/**
DEFFileError destructor.
@internalComponent
@released
*/
DEFFileError::~DEFFileError()
{
}

/**
Function to report DEF File Errors.
@internalComponent
@released
*/
void DEFFileError::Report()
{
	char *tempMssg;
	char *errMessage;
	
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+5+strlen(iName.c_str())+strlen(iToken.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str(),iLineNo,iToken.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
ParameterParserError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@Param aName - Argument name
@internalComponent
@released
*/
ParameterParserError::ParameterParserError(int aMessageIndex, char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{
}

/**
ParameterParserError destructor.
@internalComponent
@released
*/
ParameterParserError::~ParameterParserError()
{
}

/**
Function to report Parameter Parser Error.
@internalComponent
@released
*/
void ParameterParserError::Report()
{
	char *tempMssg;
	char *errMessage;
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);

	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
InvalidArgumentError constructor for initializing message index, argument value and option.
@param aMessageIndex - Message Index
@param aValue - Argument value
@param aOption - Argument option
@internalComponent
@released
*/
InvalidArgumentError::InvalidArgumentError(int aMessageIndex, const char * aValue, char * aOption) : ErrorHandler(aMessageIndex), iValue(aValue), iOption(aOption)
{
}

/**
InvalidArgumentError destructor.
@internalComponent
@released
*/
InvalidArgumentError::~InvalidArgumentError()
{
}

/**
Function to report Invalid Argument Error.
@internalComponent
@released
*/
void InvalidArgumentError::Report()
{
	char *tempMssg;
	char *errMessage;
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);

	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iValue.c_str())+strlen(iOption.c_str())];
		sprintf(tempMssg,errMessage,iValue.c_str(),iOption.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
E32ImageCompressionError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
E32ImageCompressionError::E32ImageCompressionError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
E32ImageCompressionError destructor.
@internalComponent
@released
*/
E32ImageCompressionError::~E32ImageCompressionError()
{
}

/**
Function to report E32 Image Compression Error.
@internalComponent
@released
*/
void E32ImageCompressionError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
CapabilityError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
CapabilityError::CapabilityError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
CapabilityError destructor.
@internalComponent
@released
*/
CapabilityError::~CapabilityError()
{
}

/**
Function to report Capability Error.
@internalComponent
@released
*/
void CapabilityError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
UnrecognisedCapabilityError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - Capability
@internalComponent
@released
*/
UnrecognisedCapabilityError::UnrecognisedCapabilityError(int aMessageIndex, char * aName) : CapabilityError(aMessageIndex), iName(aName)
{
}

/**
UnrecognisedCapabilityError destructor.
@internalComponent
@released
*/
UnrecognisedCapabilityError::~UnrecognisedCapabilityError()
{
}

/**
Function to report Unrecognised Capability Error.
@internalComponent
@released
*/
void UnrecognisedCapabilityError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
ELFFileError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - ELF File name
@internalComponent
@released
*/
ELFFileError::ELFFileError(int aMessageIndex, const char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{
}

/**
ELFFileError destructor.
@internalComponent
@released
*/
ELFFileError::~ELFFileError()
{
}

/**
Function to report ELF File Error.
@internalComponent
@released
*/
void ELFFileError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
UndefinedSymbolError constructor for initializing message index, argument name and symbol name.
@param aMessageIndex - Message Index
@param aName - File Name
@param aSymbolName - Symbol Name
@internalComponent
@released
*/
UndefinedSymbolError::UndefinedSymbolError(int aMessageIndex, char * aName, char *aSymbolName) : ELFFileError(aMessageIndex,aName), iSymbolName(aSymbolName)
{
}

/**
UndefinedSymbolError destructor.
@internalComponent
@released
*/
UndefinedSymbolError::~UndefinedSymbolError()
{
}

/**
Function to report Undefined Symbol Error.
@internalComponent
@released
*/
void UndefinedSymbolError::Report()
{
	char *tempMssg;
	char *errMessage;
	
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iSymbolName.c_str())+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iSymbolName.c_str(),iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
SymbolMissingFromElfError constructor for initializing message index, symbol name list and argument name.
@param aMessageIndex - Message Index
@param aSymbolList - List of symbols
@param aName - File Name
@internalComponent
@released
*/
SymbolMissingFromElfError::SymbolMissingFromElfError(int aMessageIndex, list<String> &aSymbolList, const char * aName) : ELFFileError(aMessageIndex,aName)
{

	std::list<String>::iterator aItr = aSymbolList.begin();
	std::list<String>::iterator last = aSymbolList.end();

	while(aItr != last)
	{
		iSymbolNames+=*aItr;
		aItr++;
		if(aItr != last)
		{
			iSymbolNames+=",";
		}
	}

}

/**
SymbolMissingFromElfError destructor.
@internalComponent
@released
*/
SymbolMissingFromElfError::~SymbolMissingFromElfError()
{
}

/**
Function to report Symbol Missing From Elf Error.
@internalComponent
@released
*/
void SymbolMissingFromElfError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iSymbolNames.c_str())+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iSymbolNames.c_str(),iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
MemoryAllocationError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - File Name
@internalComponent
@released
*/
MemoryAllocationError::MemoryAllocationError(int aMessageIndex, char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{
}

/**
MemoryAllocationError destructor.
@internalComponent
@released
*/
MemoryAllocationError::~MemoryAllocationError()
{
}

/**
Function to report Memory Allocation Error.
@internalComponent
@released
*/
void MemoryAllocationError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
E32ImageError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
E32ImageError::E32ImageError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
E32ImageError destructor.
@internalComponent
@released
*/
E32ImageError::~E32ImageError()
{
}

/**
Function to report E32 Image Error.
@internalComponent
@released
*/
void E32ImageError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
InvalidInvocationError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
InvalidInvocationError::InvalidInvocationError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
InvalidInvocationError destructor.
@internalComponent
@released
*/
InvalidInvocationError::~InvalidInvocationError()
{
}

/**
Function to report Invalid Invocation Error.
@internalComponent
@released
*/
void InvalidInvocationError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
TargetTypeError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
TargetTypeError::TargetTypeError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
TargetTypeError destructor.
@internalComponent
@released
*/
TargetTypeError::~TargetTypeError()
{
}

/**
Function to report Target Type Error.
@internalComponent
@released
*/
void TargetTypeError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
UnsupportedTargetTypeError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - Target type name
@internalComponent
@released
*/
UnsupportedTargetTypeError::UnsupportedTargetTypeError(int aMessageIndex, char * aName) : TargetTypeError(aMessageIndex), iName(aName)
{
}

/**
UnsupportedTargetTypeError destructor.
@internalComponent
@released
*/
UnsupportedTargetTypeError::~UnsupportedTargetTypeError()
{
}

/**
Function to report Unsupported Target Type Error.
@internalComponent
@released
*/
void UnsupportedTargetTypeError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
MessageError constructor for initializing message index and index value.
@param aMessageIndex - Message Index
@param aIndexValue - Value of Index
@internalComponent
@released
*/
MessageError::MessageError(int aMessageIndex, int aIndexValue) : ErrorHandler(aMessageIndex), iIndexValue(aIndexValue)
{
}

/**
MessageError destructor.
@internalComponent
@released
*/
MessageError::~MessageError()
{
}

/**
Function to report Message Errors.
@internalComponent
@released
*/
void MessageError::Report()
{
	char *tempMssg;
	char *errMessage;
	
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+5];
		sprintf(tempMssg,errMessage,iIndexValue);
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
NoMessageFileError constructor for initializing message index.
@param aMessageIndex - Message Index
@internalComponent
@released
*/
NoMessageFileError::NoMessageFileError(int aMessageIndex) : ErrorHandler(aMessageIndex)
{
}

/**
NoMessageFileError destructor.
@internalComponent
@released
*/
NoMessageFileError::~NoMessageFileError()
{
}

/**
Function to report No Message File Error.
@internalComponent
@released
*/
void NoMessageFileError::Report()
{
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		iMessage+=errMessage;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
	}
}

/**
SysDefMismatchError constructor for initializing message index, symbol name list and argument name.
@param aMessageIndex - Message Index
@param aSymbolList - list of symbols
@param aName - sysdef
@internalComponent
@released
*/
SysDefMismatchError::SysDefMismatchError(int aMessageIndex, list<String> &aSymbolList, const char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{

	std::list<String>::iterator aItr = aSymbolList.begin();
	std::list<String>::iterator last = aSymbolList.end();

	while(aItr != last)
	{
		iSymbolNames+=*aItr;
		aItr++;
		if(aItr != last)
		{
			iSymbolNames+=",";
		}
	}

}

/**
SysDefMismatchError destructor.
@internalComponent
@released
*/
SysDefMismatchError::~SysDefMismatchError()
{
}

/**
Function to report SysDef Mismatch Error.
@internalComponent
@released
*/
void SysDefMismatchError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iSymbolNames.c_str())+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iSymbolNames.c_str(),iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
InvalidE32ImageError constructor for initializing message index and argument name.
@param aMessageIndex - Message Index
@param aName - E32Image File name
@internalComponent
@released
*/
InvalidE32ImageError::InvalidE32ImageError(int aMessageIndex, char * aName) : ErrorHandler(aMessageIndex), iName(aName)
{
}

/**
InvalidE32ImageError destructor.
@internalComponent
@released
*/
InvalidE32ImageError::~InvalidE32ImageError()
{
}

/**
Function to report Invalid E32 Image Error.
@internalComponent
@released
*/
void InvalidE32ImageError::Report()
{
	char *tempMssg;
	char *errMessage;

	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}

/**
ImportRelocationError constructor for initializing message index, argument name and symbol name.
@param aMessageIndex - Message Index
@param aName - File Name
@param aSymbolName - Symbol Name
@internalComponent
@released
*/
ImportRelocationError::ImportRelocationError(int aMessageIndex, char * aName, char *aSymbolName) : ELFFileError(aMessageIndex,aName), iSymbolName(aSymbolName)
{
}

/**
ImportRelocationError destructor.
@internalComponent
@released
*/
ImportRelocationError::~ImportRelocationError()
{
}

/**
Function to report Import Relocations references to Data Segment Error
@internalComponent
@released
*/
void ImportRelocationError::Report()
{
	char *tempMssg;
	char *errMessage;
	
	errMessage=MessageHandler::GetInstance()->GetMessageString(iMessageIndex);
	if(errMessage)
	{
		tempMssg = new char[strlen(errMessage)+strlen(iSymbolName.c_str())+strlen(iName.c_str())];
		sprintf(tempMssg,errMessage,iSymbolName.c_str(),iName.c_str());
		iMessage+=tempMssg;
		MessageHandler::GetInstance()->Output(iMessage.c_str());
		delete tempMssg;
	}
}


