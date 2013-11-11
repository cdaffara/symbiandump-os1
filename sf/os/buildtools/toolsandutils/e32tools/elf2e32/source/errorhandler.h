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
// Error Handler Classes for elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef _ERROR_HANDLER_
#define _ERROR_HANDLER_

#include "messagehandler.h"
#include <list>
#include <string>

using std::list;
typedef std::string String;

/**
Base class from which all other error handler classes are derived from.
@internalComponent
@released
*/
class ErrorHandler
{
	public:
		ErrorHandler(int aMessageIndex);
		virtual ~ErrorHandler();
		virtual void Report() =0;

		String iMessage;
		int iMessageIndex;
};

/**
Base class for File Errors.
@internalComponent
@released
*/
class FileError : public ErrorHandler
{
	public:
		FileError(int aMessageIndex, char * aName);
		virtual ~FileError();
		void Report();

		String iName;
};

/**
Base class for ELFFormat Errors.
@internalComponent
@released
*/
class ELFFormatError : public ErrorHandler
{
	public:
		ELFFormatError(int aMessageIndex, char * aName);
		virtual ~ELFFormatError();
		void Report();

		String iName;
};

/**
Base class for DEF File Errors.
@internalComponent
@released
*/
class DEFFileError : public ErrorHandler
{
	public:
		DEFFileError(int aMessageIndex, char * aName, int aLineNo,char * aToken);
		virtual ~DEFFileError();
		void Report();

		String iName;
		int iLineNo;
		String iToken;
};

/**
Base class for Parameter Parser Errors.
@internalComponent
@released
*/
class ParameterParserError : public ErrorHandler
{
	public:
		ParameterParserError(int aMessageIndex, char * aName);
		virtual ~ParameterParserError();
		void Report();

		String iName;
};

/**
Class for Invalid Argument Errors.
@internalComponent
@released
*/
class InvalidArgumentError : public ErrorHandler
{
	public:
		InvalidArgumentError(int aMessageIndex, const char * aValue, char * aOption);
		virtual ~InvalidArgumentError();
		void Report();

		String iValue;
		String iOption;
};

/**
Base class for E32Image Compression Errors.
@internalComponent
@released
*/
class E32ImageCompressionError : public ErrorHandler
{
	public:
		E32ImageCompressionError(int aMessageIndex);
		virtual ~E32ImageCompressionError();
		void Report();
};

/**
Base class for Capability Errors.
@internalComponent
@released
*/
class CapabilityError : public ErrorHandler
{
	public:
		CapabilityError(int aMessageIndex);
		virtual ~CapabilityError();
		void Report();
};

/**
Class for handling Unrecognised Capability Errors.
@internalComponent
@released
*/
class UnrecognisedCapabilityError : public CapabilityError
{
	public:
		UnrecognisedCapabilityError(int aMessageIndex, char * aName);
		~UnrecognisedCapabilityError();
		void Report();

		String iName;
};

/**
Base class for ELF File Errors.
@internalComponent
@released
*/
class ELFFileError : public ErrorHandler
{
	public:
		ELFFileError(int aMessageIndex, const char * aName);
		virtual ~ELFFileError();
		void Report();

		String iName;
};

/**
Class for handling Undefined Symbol Errors.
@internalComponent
@released
*/
class UndefinedSymbolError : public ELFFileError
{
public:
	UndefinedSymbolError(int aMessageIndex, char * aName, char *aSymbolName);
	~UndefinedSymbolError();
	void Report();

	String iSymbolName;
};

/**
Class for handling Import relocation to Data segment
@internalComponent
@released
*/
class ImportRelocationError : public ELFFileError
{
public:
	ImportRelocationError(int aMessageIndex, char * aName, char *aSymbolName);
	~ImportRelocationError();
	void Report();

	String iSymbolName;
};

/**
Class for handling Symbol Missing From Elf Errors.
@internalComponent
@released
*/
class SymbolMissingFromElfError : public ELFFileError
{
	public:
		SymbolMissingFromElfError(int aMessageIndex, list<String> &aSymbolList, const char * aName);
		virtual ~SymbolMissingFromElfError();
		void Report();

		String iSymbolNames;
};

/**
Class for handling Memory Allocation Errors.
@internalComponent
@released
*/
class MemoryAllocationError : public ErrorHandler
{
	public:
		MemoryAllocationError(int aMessageIndex, char * aName);
		virtual ~MemoryAllocationError();
		void Report();

		String iName;
};

/**
Class for handling E32 Image Errors.
@internalComponent
@released
*/
class E32ImageError : public ErrorHandler
{
public:
	E32ImageError(int aMessageIndex);
	~E32ImageError();
	void Report();
};

/**
Class for handling Invalid Invocation Errors.
@internalComponent
@released
*/
class InvalidInvocationError : public ErrorHandler
{
public:
	InvalidInvocationError(int aMessageIndex);
	~InvalidInvocationError();
	void Report();
};

/**
Base class for handling Target Type Errors.
@internalComponent
@released
*/
class TargetTypeError : public ErrorHandler
{
public:
	TargetTypeError(int aMessageIndex);
	~TargetTypeError();
	void Report();
};

/**
Class for handling Unsupported Target Type Errors.
@internalComponent
@released
*/
class UnsupportedTargetTypeError : public TargetTypeError
{
public:
	UnsupportedTargetTypeError(int aMessageIndex, char * aName);
	~UnsupportedTargetTypeError();
	void Report();

	String iName;
};

/**
Class for handling Message Errors.
@internalComponent
@released
*/
class MessageError : public ErrorHandler
{
public:
	MessageError(int aMessageIndex, int aIndexValue);
	~MessageError();
	void Report();

	int iIndexValue;
};

/**
Class for handling No Message File Errors.
@internalComponent
@released
*/
class NoMessageFileError : public ErrorHandler
{
public:
	NoMessageFileError(int aMessageIndex);
	~NoMessageFileError();
	void Report();
};

/**
Class for handling Symbol that are passed through --sysdef 
not matching with the ones in the DEF file.
@internalComponent
@released
*/
class SysDefMismatchError : public ErrorHandler
{
	public:
		SysDefMismatchError(int aMessageIndex, list<String> &aSymbolList, const char * aName);
		virtual ~SysDefMismatchError();
		void Report();

		String iName;
		String iSymbolNames;
};

/**
Class for handling Invalid E32 Image Error
@internalComponent
@released
*/
class InvalidE32ImageError : public ErrorHandler
{
	public:
		InvalidE32ImageError(int aMessageIndex, char * aName);
		virtual ~InvalidE32ImageError();
		void Report();

		String iName;
};

#endif

