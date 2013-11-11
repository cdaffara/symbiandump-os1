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
// Implementation of the Class UseCaseBase for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include <iostream>

#include "usecasebase.h"
#include "e32exporttable.h"

#include "errorhandler.h"

UseCaseBase::UseCaseBase(ParameterListInterface *aParameterListInterface)
{
	iParameterListInterface = aParameterListInterface;
}


UseCaseBase::~UseCaseBase()
{
}

/**
This function calls the DefInput() from ParameterManager through the interface.

@internalComponent
@released

@return the name of the input DEF file if provided as input through --definput or 0.

*/
char * UseCaseBase::DefInput()
{
	return (iParameterListInterface->DefInput());
}

/**
This function calls the DSOOutput() from ParameterManager through the interface.

@internalComponent
@released

@return the name of the outut DSO file if provided as input through --dso or 0.

*/
char * UseCaseBase::DSOOutput()
{
	return (iParameterListInterface->DSOOutput());
}

/**
This function calls the LinkAsDLLName() from ParameterManager through the interface.

@internalComponent
@released

@return the name of the DLL to be linked with if provided as input through --linkas or 0.

*/
char * UseCaseBase::LinkAsDLLName()
{
	return (iParameterListInterface->LinkAsDLLName());
}

/**
This function calls the FileName() from ParameterManager through the interface.

@internalComponent
@released

@param aFileName
The filename alongwith the absolute path.

@return the filename (without the absolute path) for valid input else 0.

*/
char * UseCaseBase::FileName(char * aFileName)
{
	return (iParameterListInterface->FileName(aFileName));
}

/**
This function calls the DefOutput() from ParameterManager through the interface.

@internalComponent
@released

@return the name of the output DEF file if provided as input through --defoutput or 0.

*/
char * UseCaseBase::DefOutput()
{
	return iParameterListInterface->DefOutput();
}

/**
This function calls the ElfInput() from ParameterManager through the interface.

@internalComponent
@released

@return the name of the Input ELF file if provided as input through --elfinput or 0.

*/
char * UseCaseBase::InputElfFileName()
{
	return iParameterListInterface->ElfInput();
}

/**
This function gets DSO file name from the parameter manager

@internalComponent
@released

@return DSO file name

*/
const char * UseCaseBase::OutputDSOFileName()
{
	return iParameterListInterface->DSOOutput();
}

/**
This function gets the E32 image file name freom the parameter manager

@internalComponent
@released

@return E32 image file name

*/
const char * UseCaseBase::OutputE32FileName()
{
	return iParameterListInterface->E32ImageOutput();
}

/**
This function returns if data in a DLL is allowed.

@internalComponent
@released

@return if data in a DLL is allowed.

*/
bool UseCaseBase::AllowDllData()
{
	return iParameterListInterface->DllDataP();
}

/**
This function returns committed heap size

@internalComponent
@released

@return committed heap size

*/
size_t UseCaseBase::HeapCommittedSize()
{
	return iParameterListInterface->HeapCommittedSize();
}

/**
This function returns reserved heap size

@internalComponent
@released

@return reserved heap size

*/
size_t UseCaseBase::HeapReservedSize()
{
	return iParameterListInterface->HeapReservedSize();
}

/**
This function returns committed stack size

@internalComponent
@released

@return committed stack size

*/
size_t UseCaseBase::StackCommittedSize()
{
	return iParameterListInterface->StackCommittedSize();
}

/**
This function returns if the def file is unfrozen

@internalComponent
@released

@return if the def file is unfrozen

*/
bool UseCaseBase::Unfrozen()
{
	return iParameterListInterface->Unfrozen();
}

/**
This function returns Uid1

@internalComponent
@released

@return Uid1

*/
unsigned int UseCaseBase::GetUid1()
{
	return iParameterListInterface->Uid1();
}

/**
This function returns Uid2

@internalComponent
@released

@return Uid2

*/
unsigned int UseCaseBase::GetUid2()
{
	return iParameterListInterface->Uid2();
}

/**
This function returns Uid3

@internalComponent
@released

@return Uid3

*/
unsigned int UseCaseBase::GetUid3()
{
	return iParameterListInterface->Uid3();
}

/**
This function returns secure Id

@internalComponent
@released

@return secure Id

*/
unsigned int UseCaseBase::GetSecureId()
{
	return iParameterListInterface->SecureId();
}

/**
This function returns true if the --sid option is enabled.

@internalComponent
@released

@return true if --sid option is passed in.

*/
bool UseCaseBase::GetSecureIdOption()
{
	return iParameterListInterface->SecureIdOption();
}

/**
This function returns vendor Id

@internalComponent
@released

@return Vendor Id

*/
unsigned int UseCaseBase::GetVendorId()
{
	return iParameterListInterface->VendorId();
}

/**
This function returns version

@internalComponent
@released

@return version

*/
unsigned int UseCaseBase::GetVersion()
{
	return iParameterListInterface->Version();
}

/**
This function returns call entry point

@internalComponent
@released

@return call entry point

*/
bool UseCaseBase::GetCallEntryPoints()
{
	return iParameterListInterface->CallEntryPoint();
}

/**
This function returns capability

@internalComponent
@released

@return capability

*/
SCapabilitySet UseCaseBase::GetCapability()
{
	return iParameterListInterface->Capability();
}

/**
This function returns priority

@internalComponent
@released

@return priority

*/
TProcessPriority UseCaseBase::GetPriority()
{
	return iParameterListInterface->Priority();
}

/**
This function returns if fixed address is enabled.

@internalComponent
@released

@return if fixed address is enabled.

*/
bool UseCaseBase::GetFixedAddress()
{
	return iParameterListInterface->FixedAddress();
}

/**
This function returns if compression is enabled.

@internalComponent
@released

@return if compression is enabled.

*/
bool UseCaseBase::GetCompress()
{
		return iParameterListInterface->Compress();
}

/**
This function returns compression method

@internalComponent
@released

@return UID of compression method

*/
unsigned int UseCaseBase::GetCompressionMethod()
{
	return iParameterListInterface->CompressionMethod();
}

/**
This function returns the FPU type.

@internalComponent
@released

@return FPU type.

*/
unsigned int UseCaseBase::GetFPU()
{
	return iParameterListInterface->FPU();
}

/**
This function returns the library search path.

@internalComponent
@released

@return libpath

*/
ParameterListInterface::LibSearchPaths &  UseCaseBase::GetLibSearchPaths()
{
	return iParameterListInterface->LibPath();
}

/**
This function returns the logfile name

@internalComponent
@released

@return log file name

*/
char * UseCaseBase::LogFile()
{
	return (iParameterListInterface->LogFile());
}

/**
This function returns if the logging option is enabled

@internalComponent
@released

@return logging option enabled.

*/
bool UseCaseBase::LogFileOption()
{
	return (iParameterListInterface->LogFileOption());
}

/**
This function returns if export library is enabled.

@internalComponent
@released

@return if the export library is enabled.

*/
bool UseCaseBase::GetIgnoreNonCallable()
{
	return iParameterListInterface->IgnoreNonCallable();
}

/**
This function returns the directory separator

@internalComponent
@released

@return directory separator

*/
char UseCaseBase::GetDirectorySeparator()
{
	return iParameterListInterface->DirectorySeparator();
}


bool UseCaseBase::IsCodePaged()
{
	return (iParameterListInterface->IsCodePaged());
}

bool UseCaseBase::IsCodeUnpaged()
{
	return (iParameterListInterface->IsCodeUnpaged());
}

bool UseCaseBase::IsCodeDefaultPaged()
{
	return (iParameterListInterface->IsCodeDefaultPaged());
}

bool UseCaseBase::IsDataPaged()
{
	return (iParameterListInterface->IsDataPaged());
}

bool UseCaseBase::IsDataUnpaged()
{
	return (iParameterListInterface->IsDataUnpaged());
}

bool UseCaseBase::IsDataDefaultPaged()
{
	return (iParameterListInterface->IsDataDefaultPaged());
}

/**
This function returns whether the debuggable option is enabled or not.

@internalComponent
@released

@return whether debuggable option is enabled.

*/
bool UseCaseBase::IsDebuggable()
{
	return (iParameterListInterface->IsDebuggable());
}


bool UseCaseBase::IsSmpSafe()
{
	return iParameterListInterface->IsSmpSafe();
}

/**
This function returns if the unwanted exports are to be ignored

@internalComponent
@released

@return if the unwanted exports are to be ignored

*/
bool UseCaseBase::ExcludeUnwantedExports()
{
	return iParameterListInterface->ExcludeUnwantedExports();
}

/**
This function returns if the target is a custom dll

@internalComponent
@released

@return if target is a custom dll

*/
bool UseCaseBase::IsCustomDllTarget()
{
	return iParameterListInterface->IsCustomDllTarget();
}

/**
This function returns whether named lookup of symbols is enabled or not.

@internalComponent
@released

@return default named symbol lookup enabled.

*/
bool UseCaseBase::GetNamedSymLookup()
{
	return (iParameterListInterface->SymNamedLookup());
}

