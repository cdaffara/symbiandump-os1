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
// Implementation of the Class Elf2E32 for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "elf2e32.h"
#include "errorhandler.h"
#include "librarytarget.h"
#include "dll_fb_target.h"
#include "dll_rebuild_target.h"
#include "exetarget.h"
#include "polydll_fb_target.h"
#include "exexp_fb_target.h"
#include "exexp_rebuild_target.h"
#include "polydll_rebuild_target.h"
#include "stdexe_target.h"
#include "filedump.h"

#include <iostream>

using std::cout;
using std::endl;

ParameterListInterface * Elf2E32::iInstance=0;

/**
Constructor for Elf2E32

@internalComponent 
@released

@param aArgc
 The number of command line arguments passed into the program
@param aArgv
 The listing of all the arguments
*/
Elf2E32::Elf2E32(int aArgc, char **aArgv)
{
	iParameterListInterface = GetInstance(aArgc, aArgv);
}


Elf2E32::~Elf2E32()
{
	delete iInstance;
}

/**
This function creates a single instance of the ParameterManager which is derived from 
ParameterListInterface which is the abstract base class.

@internalComponent
@released

@param aArgc
 The number of command line arguments passed into the program
@param aArgv
 The listing of all the arguments

@return A pointer to the newly created ParameterListInterface object which is the 
 abstract interface
*/
ParameterListInterface * Elf2E32::GetInstance(int aArgc, char ** aArgv)
{
	if (iInstance == 0)
		iInstance = new ParameterManager(aArgc, aArgv);

	return iInstance;
}

/**
This function is to select the appropriate use case based on the input values.
1. If the input is only DEF file, then the usecase is the Create Library Target.
   For Library Creation, alongwith the DEF file input, the DSO file option and 
   the link as option SHOULD be passed. Otherwise, appropriate error message will
   be generated.
@internalComponent
@released

@return A pointer to the newly created UseCaseBase object

*/
UseCaseBase * Elf2E32::SelectUseCase()
{
	bool definputoption = iParameterListInterface->DefFileInOption();
	bool elfinputoption = iParameterListInterface->ElfFileInOption();
	char * deffilein = iParameterListInterface->DefInput();
	char * elfin = iParameterListInterface->ElfInput();
	bool filedumpoption = iParameterListInterface->FileDumpOption();
	int dumpOptions = iParameterListInterface->DumpOptions();
	char *filedumpsuboptions = iParameterListInterface->FileDumpSubOptions();

	bool e32inputoption = iParameterListInterface->E32ImageInOption();
	char * e32in = iParameterListInterface->E32Input();

	bool dumpMessageFileOption = iParameterListInterface->DumpMessageFileOption();

	if (definputoption && !deffilein)
		throw ParameterParserError(NOARGUMENTERROR, "--definput");

	if (elfinputoption && !elfin)
		throw ParameterParserError(NOARGUMENTERROR, "--elfinput");

	if(filedumpoption && !dumpOptions)
	{
		//throw for wrong options
		throw InvalidArgumentError(INVALIDARGUMENTERROR,filedumpsuboptions,"--dump");
	}

	if(e32inputoption && !e32in)
	{
        throw ParameterParserError(NOARGUMENTERROR, "--e32input");
	}
	
	iTargetType = iParameterListInterface->TargetTypeName();

//	if (iTargetType == ETargetTypeNotSet) // Will get this warning in build
//		cout << "Warning: Target Type is not specified explicitly" << endl;

	if (iTargetType == EInvalidTargetType || iTargetType == ETargetTypeNotSet)
	{
		if (elfin)
		{
			if (deffilein)
				return iUseCase = new ExportTypeRebuildTarget(iParameterListInterface);
			else
				return iUseCase = new ElfFileSupplied(iParameterListInterface);
		}
		else if (filedumpoption || e32in)
		{
			return iUseCase = new FileDump(iParameterListInterface);
		}
		else if (deffilein)
		{
			iTargetType = ELib;
		}
		else if (dumpMessageFileOption)
			return NULL;
		else
			throw InvalidInvocationError(INVALIDINVOCATIONERROR); //REVISIT
	}

	switch (iTargetType)
	{
	case EDll:
		if (!deffilein && elfin)
			iUseCase = new DLLFBTarget(iParameterListInterface);
		else if (deffilein && elfin)
			iUseCase = new DLLRebuildTarget(iParameterListInterface);
		else if (!elfin)
			throw ParameterParserError(NOREQUIREDOPTIONERROR,"--elfinput");

		ValidateDSOGeneration(iParameterListInterface, iTargetType);
		ValidateE32ImageGeneration(iParameterListInterface, iTargetType);
		return iUseCase;
	case ELib:
		if (deffilein)
		{
			ValidateDSOGeneration(iParameterListInterface, iTargetType);
			return iUseCase = new LibraryTarget(iParameterListInterface);
		}
		else
		{
			throw ParameterParserError(NOREQUIREDOPTIONERROR,"--definput");
		}
		break;
	case EExe:
		if (elfin)
		{
			iUseCase = new ExeTarget(iParameterListInterface);
			ValidateE32ImageGeneration(iParameterListInterface, iTargetType);
		}
		else
		{
			throw ParameterParserError(NOREQUIREDOPTIONERROR,"--elfinput");
		}
		return iUseCase;
	case EPolyDll:
		if (!deffilein && elfin)
			iUseCase = new POLYDLLFBTarget(iParameterListInterface);
		else if (deffilein && elfin)
			iUseCase = new POLYDLLRebuildTarget(iParameterListInterface);
		else if (!elfin)
			throw ParameterParserError(NOREQUIREDOPTIONERROR,"--elfinput");

		ValidateE32ImageGeneration(iParameterListInterface, iTargetType);
		return iUseCase;
	case EExexp:
	       	if (!deffilein && elfin)
                iUseCase = new ExexpFBTarget(iParameterListInterface);
			else if (deffilein && elfin)
				iUseCase = new ExExpRebuildTarget(iParameterListInterface);
			else if (!elfin)
				throw ParameterParserError(NOREQUIREDOPTIONERROR,"--elfinput");

		ValidateDSOGeneration(iParameterListInterface,iTargetType);
		ValidateE32ImageGeneration(iParameterListInterface, iTargetType);
		return iUseCase;
	case EStdExe:
		iUseCase = new StdExeTarget(iParameterListInterface);
		return iUseCase;
	default:
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
	}

	return (iUseCase=0x0);
}

void Elf2E32::ValidateDSOGeneration(ParameterListInterface *aParameterListInterface, ETargetType aTargetType)
{
	bool dsofileoutoption = aParameterListInterface->DSOFileOutOption();
	bool linkasoption = aParameterListInterface->LinkAsOption();
	char * dsofileout = aParameterListInterface->DSOOutput();
	char * linkas = aParameterListInterface->LinkAsDLLName();

	if (aTargetType != ELib)
	{
		bool deffileoutoption = aParameterListInterface->DefFileOutOption();

		if (!deffileoutoption)
			throw ParameterParserError(NOREQUIREDOPTIONERROR,"--defoutput");

		char * deffileout = aParameterListInterface->DefOutput();
		//Incase if the DEF file name is not passed as an input
		if (!deffileout)
			throw ParameterParserError(NOARGUMENTERROR,"--defoutput");
	}

	if (dsofileoutoption && !dsofileout)
		throw ParameterParserError(NOARGUMENTERROR, "--dso");
	else if (linkasoption && !linkas)
		throw ParameterParserError(NOFILENAMEERROR,"--linkas");
	else if (!dsofileoutoption && !linkasoption)
		throw ParameterParserError(NOREQUIREDOPTIONERROR,"--dso, --linkas");
	else if (!dsofileoutoption)
		throw ParameterParserError(NOREQUIREDOPTIONERROR,"--dso");
	else if (!linkasoption)
		throw ParameterParserError(NOREQUIREDOPTIONERROR,"--linkas");

}

void Elf2E32::ValidateE32ImageGeneration(ParameterListInterface *aParameterListInterface, ETargetType aTargetType)
{
	bool e32fileoutoption = aParameterListInterface->E32OutOption();
	char * e32fileout = aParameterListInterface->E32ImageOutput();

	if (!e32fileoutoption)
		throw ParameterParserError(NOREQUIREDOPTIONERROR,"--output");
	else if (!e32fileout)
		throw ParameterParserError(NOARGUMENTERROR,"--output");

	UINT uid1option = aParameterListInterface->Uid1Option();
	UINT uid1val = aParameterListInterface->Uid1();

	if (!uid1option) // REVISIT
		throw ParameterParserError(NOREQUIREDOPTIONERROR,"--uid1");
	else if (uid1option && !uid1val)
		throw ParameterParserError(NOARGUMENTERROR,"--uid1");
	else if (aTargetType == EDll && uid1val != 0x10000079)
		cout << "UID1 should be set to 0x10000079 for DLL Generation" << endl;
	else if (aTargetType == EExe && uid1val != 0x1000007A)
		cout << "UID1 should be set to 0x1000007A for EXE Generation" << endl;

}

/**
This function:
 1. Calls the ParameterAnalyser() which parses the command line options and extracts the inputs. 
 2. Calls the SelectUseCase() to select the appropriate use case based on the input values.
 3. Calls the Execute() of the selected use case.
@internalComponent
@released

@return EXIT_SUCCESS if the generation of the target is successful, else EXIT_FAILURE

*/
int Elf2E32::Execute()
{
	int result = EXIT_SUCCESS;
	UseCaseBase * usecase=0;
	bool dumpMessageFileOption;
	char * dumpMessageFile;

	try
	{
		iParameterListInterface->ParameterAnalyser();

		dumpMessageFileOption = iParameterListInterface->DumpMessageFileOption();
		dumpMessageFile = iParameterListInterface->DumpMessageFile();

	 	if(dumpMessageFileOption)
		{
			if (dumpMessageFile)
			{
				//create message file
				MessageHandler::GetInstance()->CreateMessageFile(dumpMessageFile);
				//return result;
			}
			else
			//dumpmessage file name is not provided as input
			throw ParameterParserError(NOARGUMENTERROR, "--dumpmessagefile");
		}
	
		usecase = SelectUseCase();
		if (usecase)
		{
			result = usecase->Execute();
		}
		else if (dumpMessageFileOption)
		{
			return result;
		}
		else
		{
			result = EXIT_FAILURE;
		}
	}
	catch(ErrorHandler& error) 
	{ 
		result = EXIT_FAILURE;
		error.Report();
	} 
	catch(...) // If there are any other unhandled exception,they are handled here.
	{
		result = EXIT_FAILURE;
		MessageHandler::GetInstance()->ReportMessage(ERROR, POSTLINKERERROR);
	} 
	delete usecase;
	return result;
}



