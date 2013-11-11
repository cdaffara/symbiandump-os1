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
// Implementation of the Header file for the ParameterListInterface of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef PARAMETERLISTINTERFACE_H
#define PARAMETERLISTINTERFACE_H

#ifdef _MSC_VER 
#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information
#pragma warning(disable: 4702) // unreachable code
#endif

#include <string>
#include <vector>
#include "e32imagedefs.h"

enum ETargetType
{
	ETargetTypeNotSet = - 2,
	EInvalidTargetType = - 1,	
	/** Target type is Library */
	ELib,
	/** Target type is DLL */
	EDll,
	EExe,
	EPolyDll,
	EExexp,
	EStdExe
};

typedef unsigned int UINT;

using std::vector;
/**
This class is the abstract base class from which ParameterManager is derived.

@internalComponent
@released
*/
class ParameterListInterface
{

public:
	typedef vector<char *> LibSearchPaths;

	struct SysDefs
	{
		unsigned int iSysDefOrdinalNum;
		char * iSysDefSymbolName;
	};

	typedef struct SysDefs Sys;

	typedef std::string string;

	virtual ~ParameterListInterface();
/**
This function extracts the DEF file name that is passed as input through the --definput option.

@internalComponent
@released

@return the name of the input DEF file if provided as input through --definput or 0.
*/
	virtual char * DefInput()=0;
/**
This function extracts the Elf file name that is passed as input through the --dsoin option.

@internalComponent
@released

@return the name of the input Elf file if provided as input through --dsoin or 0.
*/
	virtual char * ElfInput()=0;
/**
This function extracts the E32 image name that is passed as input through the --e32dump option.

@internalComponent
@released

@return the name of the input E32 image if provided as input through --e32dump or 0.
*/
	virtual char * E32Input()=0;
	virtual bool E32ImageInOption()=0;
	virtual bool FileDumpOption()=0;
	virtual int DumpOptions()=0;

/**
This function extracts the output DEF file name that is passed as input through the --defoutput option.

@internalComponent
@released

@return the name of the output DEF file if provided as input through --defoutput or 0.
*/
	virtual char * DefOutput()=0;
/**
This function extracts the DSO file name that is passed as input through the --dso option.

@internalComponent
@released

@return the name of the output DSO file if provided as input through --dso or 0.
*/
	virtual char * DSOOutput()=0;
/**
This function extracts the E32 image output that is passed as input through the --output option.

@internalComponent
@released

@return the name of the output E32 image output if provided as input through --output or 0.
*/
	virtual char * E32ImageOutput()=0;
/**
This function extracts the target type that is passed as input through the --targettype option.

@internalComponent
@released

@return the name of the input target type if provided as input through --targettype or 0.
*/
	virtual ETargetType TargetTypeName()=0;
/**
This function extracts the name of the DLL (that the DSO is to be linked with) 
that is passed as input through the --linkas option.

@internalComponent
@released

@return the name of the DLL name to be linked with if provided as input through --linkas or 0.
*/
	virtual char * LinkAsDLLName()=0;
/**
This function extracts the path (where the intermediate libraries should be put)
that is passed as input through the --libpath option.

@internalComponent
@released

@return the path if provided as input through --libpath or 0.
*/
	virtual LibSearchPaths& LibPath()=0;
/**
This function extracts the filename from the absolute path that is given as input.

@internalComponent
@released

@param aFileName
The filename alongwith the absolute path.

@return the filename (without the absolute path) for valid input else the filename itself.

*/
	virtual char * FileName(char * aArg)=0;
/**
This function finds out the directory separator '\' in the path

@internalComponent
@released

@return the directory spearator '\'
*/
	virtual char DirectorySeparator()=0;
/**
This function finds out if the --definput option is passed to the program.

@internalComponent
@released

@return True if --definput option is passed in or False.
*/
	virtual bool DefFileInOption()=0;
/**
This function finds out if the --elfinput option is passed to the program.

@internalComponent
@released

@return True if --elfinput option is passed in or False.
*/
	virtual bool ElfFileInOption()=0;
/**
This function finds out if the --dso option is passed to the program.

@internalComponent
@released

@return True if --dso option is passed in or False.
*/
	virtual bool DSOFileOutOption()=0;
/**
This function finds out if the --linkas option is passed to the program.

@internalComponent
@released

@return True if --linkas option is passed in or False.
*/
	virtual bool LinkAsOption()=0;
/**
This function parses the command line options and sets the appropriate values based on the
input options.

@internalComponent
@released

*/
	virtual void ParameterAnalyser()=0;

	virtual bool LogFileOption()=0;
	virtual char* LogFile()=0;

	virtual bool E32OutOption()=0;
	virtual bool DefFileOutOption()=0;

	virtual bool Uid1Option()=0;
	virtual bool SecureIdOption()=0;
	virtual bool VendorIdOption()=0;

	virtual bool MessageFileOption()=0;
	virtual char* MessageFile()=0;

	virtual bool DumpMessageFileOption()=0;
	virtual char* DumpMessageFile()=0;

	virtual Sys SysDefSymbols(int count)=0;


	virtual UINT Uid1() = 0;
	virtual UINT Uid2() = 0;
	virtual UINT Uid3() = 0;
	virtual UINT SecureId() = 0;
	virtual UINT VendorId() = 0;
	virtual bool FixedAddress() = 0;
	virtual bool Compress() = 0;
	virtual UINT CompressionMethod() = 0;
	virtual size_t HeapCommittedSize() = 0;
	virtual size_t HeapReservedSize() = 0;
	virtual size_t StackCommittedSize() = 0;
	virtual bool Unfrozen() = 0;
	virtual bool IgnoreNonCallable() = 0;
	virtual SCapabilitySet Capability() = 0;
	virtual int SysDefCount() = 0;
	virtual char * FileDumpSubOptions() = 0;
	virtual TProcessPriority Priority() = 0;
	virtual bool DllDataP() = 0;
	virtual unsigned int Version() = 0;
	virtual bool CallEntryPoint() = 0; //{ return true;}
	virtual UINT FPU() = 0;

	virtual bool IsCodePaged() = 0;
	virtual bool IsCodeUnpaged() = 0;
	virtual bool IsCodeDefaultPaged() = 0;

	virtual bool IsDataPaged() = 0;
	virtual bool IsDataUnpaged() = 0;
	virtual bool IsDataDefaultPaged() = 0;
	
	virtual bool ExcludeUnwantedExports() = 0;
	virtual bool IsCustomDllTarget() = 0;
	virtual bool SymNamedLookup() = 0;
	virtual bool IsDebuggable() = 0;
	virtual bool IsSmpSafe() = 0;
};


#endif // PARAMETERLISTINTERFACE_H

