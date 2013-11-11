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
// Implementation of the Class ParameterManager for the elf2e32 tool
// @internalComponent
// @released
// 
//

// This must go before ParameterManager.h
#define __INCLUDE_CAPABILITY_NAMES__
#include <e32capability.h>

#include "pl_common.h"
#include "parametermanager.h"
#include "errorhandler.h"
#include <iostream>
#include <ctype.h>

#include "h_utl.h"
#include "h_ver.h"
/**
Constructor for the ParameterManager.

@internalComponent
@released

@param aArgc
 The number of command line arguments passed into the program
@param aArgv
 The listing of all the arguments
*/
ParameterManager::ParameterManager(int aArgc, char** aArgv) :
	iArgc(aArgc),
	iArgv(aArgv),
	iImageLocation(0), 
	iImageName(0),
	iTargetTypeOption(false), 
	iDefFileInOption(false),
	iDefFileOutOption(false), 
	iFileDumpOption(false),		
	iDSOFileOutOption(false), 
	iOutFileOption(false),
	iElfFileInOption(false),
	iE32ImageInOption(false),			
	iLinkAsOption(false),
	iUid1Option(false), 	
	iSecureIDOption(false), 
	iVendorIDOption(false),
	iUID1(0), 
	iUID2(0), 
	iUID3(0),
	iSecureID(0),
	iVendorID(0),
	iCompress(true),
	iCompressionMethod(KUidCompressionDeflate),
	iFixedAddress(false),
	iHeapCommittedSize(0x1000),
	iHeapReservedSize(0x100000),	
	iStackCommittedSize(0x2000),
	iUnfrozen(false),
	iIgnoreNonCallable(false),
	iTargetTypeName(ETargetTypeNotSet),	
	iDefOutput(0),
	iDSOOutput(0),	
	iOutFileName(0),
	iDefInput(0),		
	iElfInput(0),
	iE32Input(0),
	iLinkDLLName(0),
	iDumpOptions(EDumpDefaults),
	iFileDumpSubOptions(0),	
	iSysDefOption(false),
	iLogFileName(0),
	iLogFileOption(false),
	iMessageFileName(0),
	iMessageFileOption(false),		
	iDumpMessageFileName(0),	
	iDumpMessageFileOption(false),
	iDllDataP(false),	
	iLibPathList (0),	
	iSysDefCount (0),	
	iPriorityOption(false),
	iPriorityVal((TProcessPriority)0),
	iVersion(0x000a0000u),
	iVersionOption(false),
	iCallEntryPoint(true),
	iFPU(0),
	iFPUOption(false),
	iArgumentCount(0),

	iCodePaged(false),
	iCodeUnpaged(false),
	iCodeDefaultPaged(false),

	iDataPaged(false),
	iDataUnpaged(false),
	iDataDefaultPaged(false),
	
	iExcludeUnwantedExports(false),
	iCustomDllTarget(false),
	iSymNamedLookup(false),
	iDebuggable(false),
	iSmpSafe(false)
{
	iArgumentCount = aArgc;
	ParamList temp(aArgv, aArgv+aArgc);
	iParamList = temp;
	iCapability.iCaps[0] = 0;	
	iCapability.iCaps[1] = 0;
}

/**
Destructor for the ParameterManager.

@internalComponent
@released
*/
ParameterManager::~ParameterManager()
{
	DELETE_PTR_ARRAY(iImageLocation);
}

/**
Function to check if the given value is a valid number (decimal value)

@internalComponent
@released

@param aArg
Value to be checked
@return True if the provided value is a decimal value.
*/
static bool IsAllDigits(const char * aArg)
{
	const char * p = aArg;
	while (*p)
	{
		if (!isdigit(*p++)) return false;
	}
	return true;	
}

/**
Function to check if the given value is a valid number (Hexadecimal value)

@internalComponent
@released

@param aArg
Value to be checked
@return True if the provided value is a hexadecimal value.
*/
static bool IsAllXDigits(const char * aArg)
{
	const char * p = aArg;
	while (*p)
	{
		if (!isxdigit(*p++)) return false;
	}
	return true;	
}

/**
Function to check if the given value is a valid number (Decimal or Hexadecimal value)

@internalComponent
@released

@param aVal
Holds the validated number.
@param aArg
Value to be checked
@return True if the provided value is a valid number.
*/
static bool GetUInt(UINT & aVal, const char * aArg)
{
	char * final = (char *)aArg;
	// try base 10 first
	if (IsAllDigits(aArg))
	{
		aVal = strtoul(aArg, &final, 10);
		if (aArg != final) return true;
		// now try as hex
	}
	if (aArg[0] == '0' && (aArg[1] == 'x' || aArg[1] == 'X'))
	{
		aArg += 2;
		if (IsAllXDigits(aArg))
		{
			final = (char *)aArg ;
			aVal = strtoul(aArg, &final, 16);
			if (aArg != final) return true;
		}
	}

	return false;
}

const char * ParameterManager::iParamPrefix = "--";
const char * ParameterManager::iParamShortPrefix = "-";
const char ParameterManager::iParamEquals = '=';

// Option Map
const ParameterManager::OptionDesc ParameterManager::iOptions[] = 
{
	{ 
		"definput", 
		(void *)ParameterManager::ParseDefInput,
		"Input DEF File",
	},
	{ 
		"defoutput", 
		(void *)ParameterManager::ParseDefOutput,
		"Output DEF File",
	},
	{ 
		"elfinput", 
		(void *)ParameterManager::ParseElfInput,
		"Input ELF File",
	},
	{ 
		"output", 
		(void *)ParameterManager::ParseOutput,
		"Output E32 Image",
	},
	{ 
		"dso", 
		(void *)ParameterManager::ParseDSOOutput,
		"Output import DSO File",
	},
	{ 
		"targettype", 
		(void *)ParameterManager::ParseTargetTypeName,
		"Target Type",
	},
	{ 
		"linkas", 
		(void *)ParameterManager::ParseLinkAs,
		"name",
	},
	{ 
		"uid1", 
		(void *)ParameterManager::ParseUID1,
		"UID 1",
	},
	{ 
		"uid2", 
		(void *)ParameterManager::ParseUID2,
		"UID 2",
	},
	{ 
		"uid3", 
		(void *)ParameterManager::ParseUID3,
		"UID 3",
	},
	{ 
		"sid", 
		(void *)ParameterManager::ParseSecureId,
		"Secure ID",
	},
	{ 
		"vid", 
		(void *)ParameterManager::ParseVendorId,
		"Vendor ID",
	},
	{ 
		"fixedaddress", 
		(void *)ParameterManager::ParseFixedAddress,
		"Has fixed address",
		},
	{ 
		"uncompressed", 
		(void *)ParameterManager::ParseUncompressed,
		"Don't compress output e32image",
		},
	{
		"compressionmethod",
		(void*)ParameterManager::ParseCompressionMethod,
		"Input compression method [none|inflate|bytepair]\n\t\tnone     no compress the image.\
		\n\t\tinflate  compress image with Inflate algorithm.\
		\n\t\tbytepair tcompress image with BytePair Pak algorithm."
	},
	{ 
		"heap", 
		(void *)ParameterManager::ParseHeap,
		"Heap committed and reserved size in bytes(.EXEs only)",
	},
	{ 
		"stack", 
		(void *)ParameterManager::ParseStackCommitted,
		"Stack size in bytes(.EXEs only)",
	},
	{ 
		"unfrozen", 
		(void *)ParameterManager::ParseUnfrozen,
		"Don't treat input dot-def file as frozen",
	},
	{ 
		"ignorenoncallable", 
		(void *)ParameterManager::ParseIgnoreNonCallable,
		"Suppress implicit exports",
	},
	{
		"capability",
		(void *)ParameterManager::ParseCapability,
		"capability option",
	},
	{ 
		"libpath", 
		(void *)ParameterManager::ParseLibPaths,
		"A semi-colon separated search path list to locate import DSOs",
	},
	{
		"sysdef",
		(void *)ParameterManager::ParseSysDefs,
		"A semi-colon separated Pre Defined Symbols to be exported and the ordinal number",
	},
	{
		"log",
		(void *)ParameterManager::ParseLogFile,
		"Output Log Message",
	},
	{
		"messagefile",
		(void *)ParameterManager::ParseMessageFile,
		"Input Message File",
	},
	{
		"dumpmessagefile",
		(void *)ParameterManager::ParseDumpMessageFile,
		"Output Message File",
	},
	{
		"dlldata", 
		(void *)ParameterManager::ParseAllowDllData,
		"Allow writable static data in DLL",
	},
	{
		"dump",
		(void *)ParameterManager::ParseFileDump,
		"Input dump options [hscdeit] --e32input <filename>\n\tFlags for dump:\n\t\th Header \
		 \n\t\ts Security info\n\t\tc Code section\n\t\td Data section \
		 \n\t\te Export info\n\t\ti Import table\n\t\tt Symbol Info",
	},
	{
		"e32input",
		(void *)ParameterManager::ParseE32ImageInput,
		"Input E32 Image File Name",
	},
	{
		"priority",
		(void *)ParameterManager::ParsePriority,
		"Input Priority",
	},
	{
		"version",
		(void *)ParameterManager::ParseVersion,
		"Module Version",
	},
	{
		"callentry",
		(void *)ParameterManager::ParseCallEntryPoint,
		"Call Entry Point",
	},
	{
		"fpu",
		(void *)ParameterManager::ParseFPU,
		"FPU type [softvfp|vfpv2]",
	},
	{
        "codepaging",
		(void *)ParameterManager::ParseCodePaging,
		"Code Paging Strategy [paged|unpaged|default]",
	},
	{
        "datapaging",
		(void *)ParameterManager::ParseDataPaging,
		"Data Paging Strategy [paged|unpaged|default]",
	},

	{
		"paged",
		(void *)ParameterManager::ParsePaged,
		"This option is deprecated. Use --codepaging=paged instead.",
	},
	{
		"unpaged",
		(void *)ParameterManager::ParseUnpaged,
		"This option is deprecated. Use --codepaging=unpaged instead.",
	},
	{
		"defaultpaged",
		(void *)ParameterManager::ParseDefaultPaged,
		"This option is deprecated. Use --codepaging=default instead.",
	},

	{
		"excludeunwantedexports",
		(void *)ParameterManager::ParseExcludeUnwantedExports,
		"Exclude Unwanted Exports",
	},
	{
		"customdlltarget",
		(void *)ParameterManager::ParseIsCustomDllTarget,
		"Custom Dll Target",
	},
	{
		"namedlookup",
		(void*)ParameterManager::ParseSymNamedLookup,
		"Enable named lookup of symbols",
	},
	{
		"debuggable",
		(void*)ParameterManager::ParseDebuggable,
		"Debuggable by run-mode debug subsystem",
	},
	{
		"smpsafe",
		(void*)ParameterManager::ParseSmpSafe,
		"SMP Safe",
	},
	{ 
		"help", 
		(void *)ParameterManager::ParamHelp,
		0,
	},
};

// Map for the short options (abbreviation)
const ParameterManager::OptionDesc ParameterManager::iShortOptions[] =
{
	{ 
		"h", 
		(void *)ParameterManager::ParamHelp,
		0,
	},
};

/**
This function finds the total number of options with normal prefix ('--')

@internalComponent
@released

@return the total number of options with normal prefix '--' in the option list.
*/
int ParameterManager::NumOptions()
{
	return (sizeof(iOptions) / sizeof(OptionDesc));
}

/**
This function finds the total number of options with short prefix ('-')

@internalComponent
@released

@return the total number of options with short prefix '-' in the option list.
*/
int ParameterManager::NumShortOptions()
{
	return (sizeof(iShortOptions) / sizeof(OptionDesc));
}

/**
This function initializes the option list, options preceeded by '--' and '-'

@internalComponent
@released
*/
void ParameterManager::InitParamParser()
{
	int lim = NumOptions();
	int shortoptionlim = NumShortOptions();
	int i;

 	for (i = 0; i < lim; i++)
	{
		string aOption = iOptions[i].iName;
		iOptionMap[aOption] = &iOptions[i];
	}

	for (i = 0; i < shortoptionlim; i++)
	{
		string aShortOption = iShortOptions[i].iName;
		iShortOptionMap[aShortOption] = &iShortOptions[i];
	}
}

/**
This function parses the command line options and identifies the input parameters.
If no options or if --log is the only option passed in, then usage information will 
be displayed.

@internalComponent
@released
*/
void ParameterManager::ParameterAnalyser()
{
	InitParamParser();
	const OptionDesc * aHelpDesc = iOptionMap["help"];
	int prefixLen = strlen(iParamPrefix);
	int prefixShortLen = strlen(iParamShortPrefix);
	int ArgCount = iArgumentCount-1;
	RecordImageLocation();
	ParamList::iterator p = iParamList.begin()+1;

	int OnlyLoggingOption = 0;
	
	// Have to have some arguments. Otherwise, assume this is a request for --help and display the usage information
	if (p == iParamList.end())
	{
		ParserFn parser = (ParserFn)aHelpDesc->iParser ;
		parser(this, "help", 0, aHelpDesc);
	}

	if ( (ArgCount ==1) && (!strncmp(*p,"--log",5)) )
		OnlyLoggingOption = 1;

	for (; p != iParamList.end(); p++)
	{
		int Prefix=0, ShortPrefix=0;
		ArgCount--;
		

		// Check if the option provided is correct and display help on getting incorrect options
		try 
		{
			if (!strncmp(*p, iParamPrefix, prefixLen))
				Prefix = 1;
			else if (!strncmp(*p, iParamShortPrefix, prefixShortLen))
				ShortPrefix = 1;
			else // Option is neither preceeded by '-' or by '--'
				throw ParameterParserError(OPTIONNAMEERROR,*p);
		}
		catch (ErrorHandler& error) 
		{ 
			error.Report();
			ParserFn parser = (ParserFn)aHelpDesc->iParser ;
			parser(this, *p, 0, 0);
		}
		
		char *option;
		const OptionDesc * aDesc;
		char * optval=NULL;

		// Get the option name excluding the prefix, '-' or '--'if (Prefix)
		if (Prefix)
			{
 			option = *p + prefixLen;
			}
 		else
			{
			option = *p + prefixShortLen;
			}

		char * pos = strchr(*p, iParamEquals);

		char *end = pos ? pos : *p + strlen(*p);

		string aName(option, end);
		string optionval;

		if (pos)
		{
			if (*end != 0 && (*(end+1) != NULL) )
			{
			// Check to ensure that optval points to the correct option value
				optval=end+1;
			}
			if ( ((p+1) != iParamList.end()) && (**(p+1) != '-') )
			{
				pos = 0;
			}
		}
		if ( !pos)
		{
			// The option is not immediately preceeded by '='
			// Space may be used to separate the input option and the paramters
			// '=' may be enclosed within space
			//if ((p+1) != iParamList.end())
			if (ArgCount != 0) //To find if we have consumed all the arguments
			{
				while ( ((p+1) != iParamList.end()) && (**(p+1) != '-') )
				{
					end = *(p+1);
					if (*end == '=') 
					{
						if ( (*(end+1) != ' ') && (*(end+1) != NULL) )
							// This is the case where '=' is preceeded by space.
							optionval += end+1;
					}
					else
					{
						optionval += end;
					}
					p++;
					ArgCount-- ;
				}
				if (optionval.length())
				{
					if (optval)
  						strcat(optval, optionval.c_str());
  					else
  					{
  						optval = new char[optionval.length()+1];
  						strcpy(optval, optionval.c_str());
  					}
				}				
			}
		}
	
		if (Prefix)
			aDesc = iOptionMap[aName];
		else
			aDesc = iShortOptionMap[aName];

		ParserFn parser = aDesc ?(ParserFn)aDesc->iParser : (ParserFn)aHelpDesc->iParser ;
		// NB aDesc might be NULL which tells help to exit with error.
		try
		{
			if (!aDesc)
				throw ParameterParserError(OPTIONNAMEERROR,*p);
		}
		catch (ErrorHandler& error) 
		{ 
			error.Report();
			ParserFn parser = (ParserFn)aHelpDesc->iParser ;
			parser(this, "help", 0, 0);
		}

		// If --log is the only option provided, then display the usage information
		if (OnlyLoggingOption)
		{
			parser(this, (char *)aName.c_str(), optval, aDesc);
			parser = (ParserFn)aHelpDesc->iParser ;
			parser(this, "help", 0, 0);
		}

		parser(this, const_cast<char*>(aName.c_str()), optval, aDesc);
	}
}

/**
This function extracts the path

@internalComponent
@released

@param aPathName
The file path name

@return the Path in case of valid path name, else 0.

*/
char * ParameterManager::Path(char * aPathName)
{
	string str(aPathName);
	size_t pos = str.find_last_of(DirectorySeparator());

	if (pos < str.size())
	{
		char * aPath = new char[pos+2];
		memcpy(aPath, aPathName, pos+1);
		aPath[pos+1] = 0;
		return aPath;
	}
	else
		return (char *)0;
}


/**
This function extracts the location where the image is to be dumped and the image name.

@internalComponent
@released
*/
void ParameterManager::RecordImageLocation()
{
	iImageLocation = Path(iArgv[0]);
	iImageName = FileName(iArgv[0]);
}

/**
This function finds out if the --definput option is passed to the program.

@internalComponent
@released

@return True if --definput option is passed in or False.
*/
bool ParameterManager::DefFileInOption()
{
	return iDefFileInOption;
}

/**
This function finds out if the --e32input option is passed to the program.

@internalComponent
@released

@return True if --e32input option is passed in or False.
*/
bool ParameterManager::E32ImageInOption()
{
	return iE32ImageInOption;
}

/**
This function finds out if the --elfinput option is passed to the program.

@internalComponent
@released

@return True if --elfinput option is passed in or False.
*/
bool ParameterManager::ElfFileInOption()
{
	return iElfFileInOption;
}

/**
This function finds out if the --dso option is passed to the program.

@internalComponent
@released

@return True if --dso option is passed in or False.
*/
bool ParameterManager::DSOFileOutOption()
{
	return iDSOFileOutOption;
}

/**
This function finds out if the --defoutput option is passed to the program.

@internalComponent
@released

@return True if --defoutput option is passed in or False.
*/
bool ParameterManager::DefFileOutOption()
{
	return iDefFileOutOption;
}

/**
This function finds out if the --output option is passed to the program.


@internalComponent
@released

@return true if --output option is passed else return False.
*/
bool ParameterManager::E32OutOption()
{
	return iOutFileOption;
}

/**
This function finds out if the --linkas option is passed to the program.

@internalComponent
@released

@return True if --linkas option is passed in or False.
*/
bool ParameterManager::LinkAsOption()
{
	return iLinkAsOption;
}


/**
This function finds out if the --UID1 option is passed to the program.

@internalComponent
@released

@return True if --UID1 option is passed in or False.
*/
bool ParameterManager::Uid1Option()
{
	return iUid1Option;
}

/**
This function finds out if the --sid option is passed to the program.

@internalComponent
@released

@return True if --sid option is passed in or False.
*/
bool ParameterManager::SecureIdOption()
{
	return iSecureIDOption;
}

/**
This function finds out if the --vid option is passed to the program.

@internalComponent
@released

@return True if --vid option is passed in or False.
*/
bool ParameterManager::VendorIdOption()
{
	return iVendorIDOption;
}

/**
This function finds out if the --log option is passed to the program.

@internalComponent
@released

@return True if --log option is passed in or False.
*/
bool ParameterManager::LogFileOption()
{
	return iLogFileOption;
}

/**
This function finds out if the --messagefile option is passed to the program.

@internalComponent
@released

@return True if --messagefile option is passed in or False.
*/
bool ParameterManager::MessageFileOption()
{
	return iMessageFileOption;
}

/**
This function finds out if the --dumpmessagefile option is passed to the program.

@internalComponent
@released

@return True if --dumpmessagefile option is passed in or False.
*/
bool ParameterManager::DumpMessageFileOption()
{
	return iDumpMessageFileOption;
}

/**
This function finds out if the --fixedaddress option is passed to the program.

@internalComponent
@released

@return True if --fixedaddress option is passed in or False.
*/
bool ParameterManager::FixedAddress()
{
	return iFixedAddress;
}

/**
This function finds out if the --uncompressed option is passed to the program.

@internalComponent
@released

@return True if --uncompressed option is passed in or False.
*/
bool ParameterManager::Compress()
{
	return iCompress;
}


/**
This function finds out if the --compressionmethod option is passed to the program.

@internalComponent
@released

@return UId if --compressionmethod option is passed in or UId of deflate compressor (compatibility).
*/
UINT ParameterManager::CompressionMethod()
{
	return iCompressionMethod;
}


/**
This function finds out if the --unfrozen option is passed to the program.

@internalComponent
@released

@return True if --unfrozen option is passed in or False.
*/
bool ParameterManager::Unfrozen()
{
	return iUnfrozen;
}

/**
This function finds out if the --ignorenoncallable option is passed to the program.

@internalComponent
@released

@return True if --ignorenoncallable option is passed in or False.
*/
bool ParameterManager::IgnoreNonCallable()
{
	return iIgnoreNonCallable;
}

/**
This function finds out if the --dlldata option is passed to the program.

@internalComponent
@released

@return True if --dlldata option is passed in or False.
*/
bool ParameterManager::DllDataP()
{
 	return iDllDataP;
}

/**
This function finds out if the --version option is passed to the program.

@internalComponent
@released

@return True if --version option is passed in or False.
*/
bool ParameterManager::VersionOption()
{
	return iVersionOption;
}

/**
This function finds out if the --callentry option is passed to the program.

@internalComponent
@released

@return True if --callentry option is passed in or False.
*/
bool ParameterManager::CallEntryPoint()
{
	return iCallEntryPoint;
}

/**
This function finds out if the --priority option is passed to the program.

@internalComponent
@released

@return True if --priority option is passed in or False.
*/
bool ParameterManager::PriorityOption()
{
	return iPriorityOption;
}

/**
This function finds out if the --sysdef option is passed to the program.

@internalComponent
@released

@return True if --sysdef option is passed in or False.
*/
bool ParameterManager::SysDefOption()
{
	return iSysDefOption;
}

/**
This function finds out if the --dump option is passed to the program.

@internalComponent
@released

@return True if --dump option is passed in or False
*/
bool ParameterManager::FileDumpOption()
{
	return iFileDumpOption;
}

/**
This function finds out if the --fpu option is passed to the program.

@internalComponent
@released

@return True if --fup option is passed in or False.
*/
bool ParameterManager::FPUOption()
{
	return iFPUOption;
}


/**
This function extracts the DEF file name that is passed as input through the --definput option.

@internalComponent
@released

@return the name of the input DEF file if provided as input through --definput or 0.
*/
char * ParameterManager::DefInput()
{
	return iDefInput;
}

/**
This function extracts the Elf file name that is passed as input through the --elfinput option.

@internalComponent
@released

@return the name of the input Elf file if provided as input through --elfinput or 0.
*/
char * ParameterManager::ElfInput()
{
	return iElfInput;
}

/**
This function extracts the E32 image name that is passed as input through the --e32input option.

@internalComponent
@released

@return the name of the input E32 image if provided as input through --e32input or 0.
*/
char * ParameterManager::E32Input()
{
	return iE32Input;
}

/**
This function extracts the output DEF file name that is passed as input through the --defoutput option.

@internalComponent
@released

@return the name of the output DEF file if provided as input through --defoutput or 0.
*/
char * ParameterManager::DefOutput()
{
	return iDefOutput;
}

/**
This function extracts the DSO file name that is passed as input through the --dso option.

@internalComponent
@released

@return the name of the output DSO file if provided as input through --dso or 0.
*/
char * ParameterManager::DSOOutput()
{
	return iDSOOutput;
}

/**
This function extracts the E32 image output that is passed as input through the --output option.

@internalComponent
@released

@return the name of the output E32 image output if provided as input through --output or 0.
*/
char * ParameterManager::E32ImageOutput()
{
	return iOutFileName;
}

/**
This function extracts the target type that is passed as input through the --targettype option.

@internalComponent
@released

@return the name of the input target type if provided as input through --targettype or 0.
*/
ETargetType ParameterManager::TargetTypeName()
{
	return iTargetTypeName;
}

/**
This function extracts the name of the DLL (that the DSO is to be linked with) 
that is passed as input through the --linkas option.

@internalComponent
@released

@return the name of the DLL name to be linked with if provided as input through --linkas or 0.
*/
char * ParameterManager::LinkAsDLLName()
{
	return iLinkDLLName;
}

/**
This function extracts the path (where the intermediate libraries should be put)
that is passed as input through the --libpath option.

@internalComponent
@released

@return the path if provided as input through --libpath or 0.
*/
ParameterListInterface::LibSearchPaths& ParameterManager::LibPath()
{
	return iLibPathList;
}

/**
This function extracts the total number of predefined symbols passed to --sysdef option.

@internalComponent
@released

@return the total number of predefined symbols passed through --elfinput or 0.
*/
int ParameterManager::SysDefCount()
{
	return iSysDefCount;
}

/**
This function extracts the E32 image dump options passed as input through the --dump option.

/@internalComponent
@released

@return the name of the dump options if provided as input through --dump or 0.
*/
char * ParameterManager::FileDumpSubOptions()
{
	return iFileDumpSubOptions;
}

/**
This function extracts the E32 image dump options in integer value passed as character input
through the --dump option.

@internalComponent
@released

@return the name of the dump options in integer.
*/
int ParameterManager::DumpOptions()
{
      return iDumpOptions;
}

/**
This function gets the first UID in a compound identifier (UID type) that is passed as 
input through the --uid1 option.UID1 differentiates the executables, DLLs and file stores.

@internalComponent
@released

@return the System level identifier if provided as input through --uid1 or 0.
*/
UINT ParameterManager::Uid1()
{
	return iUID1;
}

/**
This function gets the UID2 that is passed as input through the --uid2 option.
UID2 differentiates the static interface (shared library) and polymorphic interface 
(application or plug-in framework) DLLs.
	
@internalComponent
@released

@return the Interface identifier if provided as input through --uid2 or 0.
*/
UINT ParameterManager::Uid2()
{
	return iUID2;
}

/**
This function gets the UID3 that is passed as input through the --uid3 option. UID3 
is shared by all objects belonging to a given program, including library DLLs if any, 
framework DLLs, and all documents.
	
@internalComponent
@released

@return the Project identifier if provided as input through --uid3 or 0.
*/
UINT ParameterManager::Uid3()
{
	return iUID3;
}

/**
This function gets the Secure Id that is passed as input through the --sid option.

@internalComponent
@released

@return the Secure Id if provided as input through --sid or 0.
*/
UINT ParameterManager::SecureId()
{
	return iSecureID;
}

/**
This function gets the Vendor Id that is passed as input through the --vid option.

@internalComponent
@released

@return the Vendor Id if provided as input through --vid or 0.
*/
UINT ParameterManager::VendorId()
{
	return iVendorID;
}

/**
This function gets the capability value passed to '--capability' option.

@internalComponent
@released

@return the capability value passed to '--capability' option.
*/
SCapabilitySet ParameterManager::Capability()
{
	return iCapability;
}

/**
This function extracts the Log file name that is passed as input through the --log option.

@internalComponent
@released

@return the name of the Log file if provided as input through --log or 0.
*/
char * ParameterManager::LogFile()
{
	return iLogFileName;
}

/**
This function extracts the Message file name that is passed as input through the --messagefile option.

@internalComponent
@released

@return the name of the Message file if provided as input through --messagefile or 0.
*/
char * ParameterManager::MessageFile()
{
	return iMessageFileName;
}

/**
This function extracts the Message file name that is passed as input through the --dumpmessagefile option.

@internalComponent
@released

@return the name of the Message file to be dumped if provided as input through --dumpmessagefile or 0.
*/
char * ParameterManager::DumpMessageFile()
{
	return iDumpMessageFileName;
}

/**
This function extracts the list of predefined symbols that is passed as input through
the --sysdef option.

@internalComponent
@released

@return the list of predefined symbols that is passed as input through the --sysdef option.
*/
ParameterManager::Sys ParameterManager::SysDefSymbols(int sysdefcount)
{
	return iSysDefSymbols[sysdefcount];
}

/**
This function extracts the heap commited size passed as input to --heap option.

@internalComponent
@released

@return the heap commited size passed as input to --heap option.
*/
size_t ParameterManager::HeapCommittedSize()
{
 	return iHeapCommittedSize;
}

/**
This function extracts the heap reserved size passed as input to --heap option.

@internalComponent
@released

@return the heap reserved size passed as input to --heap option.
*/
size_t ParameterManager::HeapReservedSize()
{
 	return iHeapReservedSize;
}

/**
This function extracts the stack commited size passed as input to --stack option.

@internalComponent
@released

@return the stack commited size passed as input to --stack option.
*/
size_t ParameterManager::StackCommittedSize()
{
 	return iStackCommittedSize;
}

/**
This function extracts the priority value passed as input to --priority option.

@internalComponent
@released

@return the priority value passed as input to --priority option
*/
TProcessPriority ParameterManager::Priority()
{
	return iPriorityVal;
}

/**
This function extracts the version information passed as input to --version option.

@internalComponent
@released

@return the version information passed as input to --version option.
*/
UINT ParameterManager::Version()
{
	return iVersion;
}

/**
This function extracts the fpu information passed as input to the --fpu option.

@internalComponent
@released

@return the fpu information passed as input to the --fpu option.
*/
UINT ParameterManager::FPU()
{
	return iFPU;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsCodePaged()
{
	return iCodePaged;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsCodeUnpaged()
{
	return iCodeUnpaged;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsCodeDefaultPaged()
{
	return iCodeDefaultPaged;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsDataPaged()
{
	return iDataPaged;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsDataUnpaged()
{
	return iDataUnpaged;
}

/**
@internalComponent
@released
*/
bool ParameterManager::IsDataDefaultPaged()
{
	return iDataDefaultPaged;
}

/**
This function finds out if the --excludeunwantedexports option is passed to the program.

@internalComponent
@released

@return true if --excludeunwantedexports option is passed in or False.
*/
bool ParameterManager::ExcludeUnwantedExports()
{
	return iExcludeUnwantedExports;
}

/**
This function finds out if the --customdlltarget option is passed to the program.

@internalComponent
@released

@return true if --customdlltarget option is passed in or False.
*/
bool ParameterManager::IsCustomDllTarget()
{
	return iCustomDllTarget;
}

/**
This function extracts the SymNamedLookup information passed as input to the --namedlookup option.

@internalComponent
@released

@return the namedlookup information passed as input to the --namedlookup option.
*/
bool ParameterManager::SymNamedLookup()
{
	return iSymNamedLookup;
}

/**
This function determines if the -debuggable option is passed to the program.

@internalComponent
@released

@return true if --debuggable is passed in or False.
*/
bool ParameterManager::IsDebuggable()
{
	return iDebuggable;
}


bool ParameterManager::IsSmpSafe()
{
	return iSmpSafe;
}

/**
This function extracts the filename from the absolute path that is given as input.

@internalComponent
@released

@param aFileName
The filename alongwith the absolute path.

@return the filename (without the absolute path) for valid input else the filename itself.
*/
char * ParameterManager::FileName(char * aFileName)
{
	string str(aFileName);
	
	size_t pos = str.find_last_of(DirectorySeparator());	
#ifndef __LINUX__ 	
	// Support Unix slashes on Windows when stripping filenames from paths
	if (pos > str.size())
		{
		pos = str.find_last_of('/');
		}
#endif

	if (pos < str.size())
		return aFileName + pos + 1;
	else
		return aFileName;
}

/**
This function finds out the directory separator '\' in the path

@internalComponent
@released

@return the directory spearator '\'
*/
char ParameterManager::DirectorySeparator()
{
#ifdef __LINUX__ 
	return '/';
#else 
	return '\\';
#endif 
}

/**
This function set the input Def file name that is passed through --definput option.

void ParameterManager::ParseDefInput(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --definput
@param aValue
The DEF file name passed to --definput option
@param aDesc
Pointer to function ParameterManager::ParseDefInput returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDefInput)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetDefInput(aValue);
}

/**
This function set the input Elf file name that is passed through --elfinput option.

void ParameterManager::ParseElfInput(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --elfinput
@param aValue
The ELF file name passed to --elfinput option
@param aDesc
Pointer to function ParameterManager::ParseElfInput returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseElfInput)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetElfInput(aValue);
}

/**
This function set the output Def file name that is passed through --defoutput option.

void ParameterManager::ParseDefOutput(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --defoutput
@param aValue
The DEF file name passed to --defoutput option
@param aDesc
Pointer to function ParameterManager::ParseDefOutput returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDefOutput)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetDefOutput(aValue);
}

/**
This function set the input E32 file name that is passed through --e32input option.

void ParameterManager::ParseE32Input(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --e32input
@param aValue
The E32 file name passed to --e32input option
@param aDesc
Pointer to function ParameterManager::ParseE32Input returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseE32ImageInput)
{
      INITIALISE_PARAM_PARSER;
      aPM->SetE32Input(aValue);
}

/**
This function set the output E32 image name that is passed through --output option.

void ParameterManager::ParseOutput(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --output
@param aValue
The E32 file name passed to --output option
@param aDesc
Pointer to function ParameterManager::ParseOutput returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseOutput)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetE32Output(aValue);
}

/**
This function set the output Log file name that is passed through --log option.

void ParameterManager::ParseLogFile(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --log
@param aValue
The Log file name passed to --log option
@param aDesc
Pointer to function ParameterManager::ParseLogFile returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseLogFile)
{
	INITIALISE_PARAM_PARSER;
	if(aValue)
	{
		aPM->SetLogFile(aValue);
		MessageHandler::StartLogging(aValue);
	}
	else
	{
		throw ParameterParserError(NOARGUMENTERROR, "--log");
	}
}

/**
This function set the input Message file name that is passed through --messagefile option.

void ParameterManager::ParseMessageFile(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --messagefile
@param aValue
The Message file name passed to --messagefile option
@param aDesc
Pointer to function ParameterManager::ParseMessageFile returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseMessageFile)
{
	INITIALISE_PARAM_PARSER;
	if(aValue)
	{
		aPM->SetMessageFile(aValue);
		MessageHandler::InitializeMessages(aValue);
	}
	else
	{
		throw ParameterParserError(NOARGUMENTERROR, "--messagefile");
	}
}

/**
This function set the Message file name to be dumped that is passed through --dumpmessagefile option.

void ParameterManager::ParseDumpMessageFile(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --dumpmessagefile
@param aValue
The output Message file name passed to --dumpmessagefile option
@param aDesc
Pointer to function ParameterManager::ParseDumpMessageFile returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDumpMessageFile)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetDumpMessageFile(aValue);
}

/**
This function checks the arguments passed to the options. If an incorrect argument
is passed or if no arguments are passed then appropriate error message is displayed.

@internalComponent
@released

@param aValue
Input to the option
@param aOption
Option that is passed as input
@return the validated value
*/
UINT ValidateInputVal(char * aValue, char * aOption)
{
	UINT uid;
	if (!aValue)
		throw ParameterParserError(NOARGUMENTERROR, aOption);
	else if (!GetUInt(uid, aValue))
		throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, aOption);
	return uid;
}

/**
There are some boolean options which does not expect any inputs. This function checks
if there are any the arguments passed to the option. In case of getting an input argument,
the warning message "Value is ignored" is displayed.

@internalComponent
@released

@param aValue
Input to the option
@param aOption
Option that is passed as input
*/
void CheckInput(char * aValue, char * aOption)
{
	if (aValue)
		MessageHandler::GetInstance()->ReportMessage(WARNING, VALUEIGNOREDWARNING, aOption);
}

/**
This function set the version information that is passed through --version option.

void ParameterManager::ParseVersion(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --version
@param aValue
The version information passed to --version option
@param aDesc
Pointer to function ParameterManager::ParseVersion returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseVersion)
{
	INITIALISE_PARAM_PARSER;
	UINT version = 0;
	UINT majorVal = 0;
	UINT minorVal = 0;

	if(!aValue)
  	throw ParameterParserError(NOARGUMENTERROR, "--version");

	char * tokens = strdup(aValue);
	char * major, * minor;

	major = strtok(tokens, ".");
	majorVal = ValidateInputVal(major, "--version");
	minor = strtok(NULL, ".");
	if (minor && !GetUInt(minorVal, minor))
		throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, "--version");

	version = ((majorVal & 0xFFFF) << 16) | (minorVal & 0xFFFF);
	aPM->SetVersion(version);
}

/**
This function set the UID1 value that is passed through --uid1 option.

void ParameterManager::ParseUID1(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --uid1
@param aValue
The UID1 value passed to --uid1 option
@param aDesc
Pointer to function ParameterManager::ParseUID1 returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUID1)
{
	INITIALISE_PARAM_PARSER;
	UINT aUid = ValidateInputVal(aValue, "--uid1");
	aPM->SetUID1(aUid);
}

/**
This function set the UID2 value that is passed through --uid2 option.

void ParameterManager::ParseUID2(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --uid2
@param aValue
The UID2 value passed to --uid2 option
@param aDesc
Pointer to function ParameterManager::ParseUID2 returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUID2)
{
	INITIALISE_PARAM_PARSER;
	UINT aUid = ValidateInputVal(aValue, "--uid2");
	aPM->SetUID2(aUid);
}

/**
This function set the UID3 value that is passed through --uid3 option.

void ParameterManager::ParseUID3(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --uid3
@param aValue
The UID3 value passed to --uid3 option
@param aDesc
Pointer to function ParameterManager::ParseUID3 returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUID3)
{
	INITIALISE_PARAM_PARSER;
	UINT aUid = ValidateInputVal(aValue, "--uid3");
	aPM->SetUID3(aUid);
}

/**
This function set the Secure ID value that is passed through --sid option.

void ParameterManager::ParseSecureId(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --sid
@param aValue
The Secure ID value passed to --sid option
@param aDesc
Pointer to function ParameterManager::ParseSecureId returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseSecureId)
{
	INITIALISE_PARAM_PARSER;
	UINT aUid = ValidateInputVal(aValue, "--sid");
	aPM->SetSecureId(aUid);
}

/**
This function set the Vendor ID value that is passed through --vid option.

void ParameterManager::ParseVendorId(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --vid
@param aValue
The Vendor ID value passed to --vid option
@param aDesc
Pointer to function ParameterManager::ParseVendorId returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseVendorId)
{
	INITIALISE_PARAM_PARSER;
	UINT aUid = ValidateInputVal(aValue, "--vid");
	aPM->SetVendorId(aUid);
}

/**
This function set the FixedAddress flag if --fixedaddress option is passed to the program.

void ParameterManager::ParseFixedAddress(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --fixedaddress
@param aValue
The value passed to --fixedaddress option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseFixedAddress returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseFixedAddress)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--fixedaddress");
	aPM->SetFixedAddress(true);
}

/**
This function set the CallEntryPoint flag if --callentry option is passed to the program.

void ParameterManager::ParseCallEntryPoint(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --callentry
@param aValue
The value passed to --callentry option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseCallEntryPoint returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseCallEntryPoint)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetCallEntryPoint(true);
}

/**
This function set the Uncompressed flag if --uncompressed option is passed to the program.

void ParameterManager::ParseUncompressed(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --uncompressed
@param aValue
The value passed to --uncompressed option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseUncompressed returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUncompressed)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--uncompressed");
	aPM->SetCompress(false);
}


static const ParameterManager::CompressionMethodDesc MethodNames[] =
{
	{ "none", 0},
	{ "inflate", KUidCompressionDeflate},
	{ "bytepair", KUidCompressionBytePair},
	{ 0, 0}
};

/**
This function parses the compression method value passed in and finds the corresponding match
from the list of existing method values.

@internalComponent
@released

@param aMethod
Holds the method value mapped from list.
@param aText
The priority value passed to --compressionmethod option
@return True if the method value passed in is a valid one present in the list.
*/
static bool ParseCompressionMethodArg(UINT & aMethod, const char *aText)
{
	
	int i = 0;
	aMethod = 0;
	for (;; i++)
	{
		if (!MethodNames[i].iMethodName)
			return false;
		if (!stricmp(aText, MethodNames[i].iMethodName))
		{
			aMethod = MethodNames[i].iMethodUid;
			return true;
		}
	}
	return false;
}

/**
This function set the CompressionMethod variable if --compressionmethod option is passed to the program.

void ParameterManager::ParseUncompressed(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --compressionmethod
@param aValue
The value passed to --compressionmethod option, in this case none|inflate|bytepair
@param aDesc
Pointer to function ParameterManager::ParseCompressionMethod returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseCompressionMethod)
{
	INITIALISE_PARAM_PARSER;
	UINT method;
	if(!aValue)
		throw ParameterParserError(NOARGUMENTERROR, "--compressionmethod");
	
	if(ParseCompressionMethodArg(method, aValue) )
		aPM->SetCompressionMethod(method);
	else
		throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, "compression method");
}

/**
This function parses the compression method value passed in and finds the corresponding match
from the list of existing method values.

@internalComponent
@released



/**
This function set the AllowDllData flag if --dlldata option is passed to the program.

void ParameterManager::ParseAllowDllData(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --dlldata
@param aValue
The value passed to --dlldata option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseAllowDllData returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseAllowDllData)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--dlldata");
	aPM->SetDllDataP(true);
}

static const ParameterManager::PriorityValueDesc PriorityNames[] =
{
	{ "Low", EPriorityLow},
	{ "Background", EPriorityBackground},
	{ "Foreground", EPriorityForeground},
	{ "High", EPriorityHigh},
	{ "WindowServer", EPriorityWindowServer},
	{ "FileServer", EPriorityFileServer},
	{ "RealTimeServer", EPriorityRealTimeServer},
	{ "Supervisor", EPrioritySupervisor},
	{ 0, (TProcessPriority)0}
};

/**
This function parses the priority value passed in and finds the corresponding match
from the list of existing priority values.

@internalComponent
@released

@param aPriority
Holds the priority value mapped from list.
@param aText
The priority value passed to --priority option
@return True if the priority value passed in is a valid one present in the list.
*/
static bool ParsePriorityArg(unsigned int & aPriority, const char *aText)
{
	int i = 0;
	aPriority = 0;
	for (;; i++)
	{
		if (!PriorityNames[i].iName)
			return false;
		if (!stricmp(aText, PriorityNames[i].iName))
		{
			aPriority = PriorityNames[i].iPriority;
			return true;
		}
	}
	return false;
}


/**
This function set the priority value that is passed through --priority option.

void ParameterManager::ParsePriority(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --priority
@param aValue
The priority value passed to --priority option
@param aDesc
Pointer to function ParameterManager::ParsePriority returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParsePriority)
{
	INITIALISE_PARAM_PARSER;
	unsigned int priority;
	if (!aValue)
		throw ParameterParserError(NOARGUMENTERROR, "--priority");
	if (GetUInt(priority, aValue) || ParsePriorityArg(priority, aValue))
		aPM->SetPriority((TProcessPriority)priority);
	else
		throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, "priority");
}

/**
This function sets the predefined symbols, at the specified ordinal numbers, that are passed through 
--sysdef option.

void ParameterManager::ParseSysDefs(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --sysdef
@param aValue
The predefined symbols alongwith the ordinal numbers passed to --sysdef option
@param aDesc
Pointer to function ParameterManager::ParseSysDefs returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseSysDefs)
{
	INITIALISE_PARAM_PARSER;
	if (!aValue)
		throw ParameterParserError(NOARGUMENTERROR, "--sysdef");
	string aSysDefValues(aValue);
	string::iterator p = aSysDefValues.begin();

	int sysdeflength = aSysDefValues.size();
	
	int parsesysdef = 1; 

	int sysdefcount=0;

	while (parsesysdef && sysdeflength)
	{

		int q = 0;
		unsigned int ordinalnum = 0;
		char *symbol = 0;

		int nq = aSysDefValues.find_first_of(",", q,sizeof(*p));
		if (nq && (nq != string::npos))
		{
			int len = nq;
			symbol = new char[len+1];
			memcpy(symbol, p, len);
			symbol[len] = 0;
			q = nq+1;

			char val = *(p+q);
			if (!val || !isdigit(val))
				throw ParameterParserError(SYSDEFERROR, "--sysdef");
			ordinalnum = *(p+q) - '0';
			aPM->SetSysDefs(ordinalnum, symbol, sysdefcount);

			unsigned int separator = aSysDefValues.find(";", 0);

			if (separator && (separator != string::npos))
			{
				if (separator != (unsigned int)(q+1))
					throw ParameterParserError(MULTIPLESYSDEFERROR, "--sysdef");
				else 
				{
					sysdeflength -= separator + 1;
					aSysDefValues = aSysDefValues.substr(separator+1);
					sysdefcount++;
				}	
			}
			else
			{
				sysdeflength -= q+1;
				if (sysdeflength)
					throw ParameterParserError(MULTIPLESYSDEFERROR, "--sysdef");
				else
					parsesysdef = 0;
			}
		}
		else
		{
			if (!nq && sysdeflength)
				throw ParameterParserError(SYSDEFNOSYMBOLERROR, "--sysdef");
			else
				throw ParameterParserError(SYSDEFERROR, "--sysdef");
		}
	}
}

/**
This function parses the capability value passed to --capability option.

@internalComponent
@released

@param aName
The pointer marking the beginning value passed to the --capability option
@param aEnd
The pointer marking the end value passed to the --capability option
@param aCapabilities
List of Capability Values allowed
@param aInvert
Flag to denote if value can be inverted.
*/
void ParameterManager::ParseCapability1(const char * aName, const char * aEnd, SCapabilitySet& aCapabilities, bool aInvert)
{
	int n = aEnd - aName;
	int i = 0;
	if(n==3 && strnicmp("all",aName,n)==0)
	{
		if(aInvert)
			throw CapabilityError(CAPABILITYALLINVERSIONERROR);

		for(i=0; i<ECapability_Limit; i++)
		{
			if(CapabilityNames[i])
				aCapabilities[i>>5] |= (1<<(i&31));
		}
		return;
	}

	if(n==4 && strnicmp("none",aName,n)==0)
	{
		if(aInvert)
			throw CapabilityError(CAPABILITYNONEINVERSIONERROR);

		memset(&aCapabilities,0,sizeof(aCapabilities));
		return;
	}

	for(i=0; i<ECapability_Limit; i++)
	{
		const char* cap = CapabilityNames[i];
		if(!cap)
			continue;
		if((int)strlen(cap)!=n)
			continue;
		if(strnicmp(cap,aName,n)!=0)
			continue;
		break;
	}
	if(i>=ECapability_Limit)
	{
		string aBadCap(aName, aEnd);
		throw UnrecognisedCapabilityError(UNRECOGNISEDCAPABILITYERROR,"BAD CAP");
	}
	if(aInvert)
		aCapabilities[i>>5] &= ~(1<<(i&31));
	else
		aCapabilities[i>>5] |= (1<<(i&31));
}

/**
This function parses the capability value passed to --capability option.

@internalComponent
@released

@param aCapabilities
List of Capability Values allowed
@param aText
Value passed to --capability option.
*/
void ParameterManager::ParseCapabilitiesArg(SCapabilitySet& aCapabilities, const char *aText)
{
	string aCapList(aText);
	string::iterator b = aCapList.begin();
	string::iterator e = b;
	bool invert = false;

	while(e != aCapList.end())
	{
		invert = false;
		if (*b == '-')
		{
			invert = true;
			b++;
		}
		else if (*b == '+')
			b++;

		e = b;
		for (; e != aCapList.end(); e++)
		{
			if (*e == '-' || *e == '+') break;
		}
		if (e != b)
			ParseCapability1(b, e, aCapabilities, invert);

		b = e;
		
	}
}

/**
This function parses the capability value passed to --capability option.

void ParameterManager::ParseCapability(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --capability
@param aValue
The predefined symbols alongwith the ordinal numbers passed to --capability option
@param aDesc
Pointer to function ParameterManager::ParseCapability returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseCapability)
{
	INITIALISE_PARAM_PARSER;
	unsigned int cap;
	SCapabilitySet capSet = {0, 0};
	if (!aValue)
		throw ParameterParserError(NOARGUMENTERROR, "--capability");
	if (GetUInt(cap, aValue))
	{
		aPM->SetCapability(cap);
	}
	else
	{
		aPM->ParseCapabilitiesArg(capSet, aValue);
		aPM->SetCapability(capSet);
	}
}

/**
This function set the Heap Reserved and Committed Size value that is passed through --heap option.

void ParameterManager::ParseHeap(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --heap
@param aValue
The Heap Reserved and Committed Size value passed to --heap option separated by ','.
@param aDesc
Pointer to function ParameterManager::ParseHeap returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseHeap)
{
	INITIALISE_PARAM_PARSER;
	string aArg(aValue);
	UINT committed = aPM->HeapCommittedSize();
	UINT reserved = aPM->HeapReservedSize();
	int p = aArg.find_first_of(",");
	if (p  < 0)
	{
		if ( !GetUInt(committed, aValue)) 
			throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, "heap");
	}
	else
	{
		aArg[p] = 0;
		const char * committedval = aArg.c_str();
		const char * reservedval = committedval + p + 1;
		if (!(GetUInt(committed, committedval) && GetUInt(reserved, reservedval)))
			throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, "heap");
	}
	aPM->SetHeapCommittedSize(committed);
	aPM->SetHeapReservedSize(reserved);
}

/**
This function set the stack Committed Size that is passed through --stack option.

void ParameterManager::ParseStackCommitted(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --stack
@param aValue
The Stack Committed Size value passed to --stack option.
@param aDesc
Pointer to function ParameterManager::ParseStackCommitted returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseStackCommitted)
{
	INITIALISE_PARAM_PARSER;
	UINT aSize = ValidateInputVal(aValue, "--stack");
	aPM->SetStackCommittedSize(aSize);
}

/**
This function set the Unfrozen flag if --unfrozen option is passed to the program.

void ParameterManager::ParseUnfrozen(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --unfrozen
@param aValue
The value passed to --unfrozen option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseUnfrozen returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUnfrozen)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--unfrozen");
	aPM->SetUnfrozen(true);
}

/**
This function set the ignorenoncallable flag if --ignorenoncallable option is passed to the program.

void ParameterManager::ParseIgnoreNonCallable(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --ignorenoncallable
@param aValue
The value passed to --ignorenoncallable option, in this case NULL
@param aDesc
Pointer to function ParameterManager::Parseignorenoncallable returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseIgnoreNonCallable)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--ignorenoncallable");
	aPM->SetIgnoreNonCallable(true);
}

/**
This function sets the FPU type that is passed using the --fpu option.

void ParameterManager::ParseFPU(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --fpu
@param aValue
The fpu information passed to the --fpu option
@param aDesc
Pointer to function ParameterManager::ParseFPU returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseFPU)
{
	INITIALISE_PARAM_PARSER;

	if (strnicmp(aValue, "softvfp", 7)==0)
		aPM->SetFPU(0);
	else if (strnicmp(aValue, "vfpv2", 5)==0)
		aPM->SetFPU(1);
	else
		throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, aOption);
}

/**
This function set the Paged flag if --paged option is passed to the program.

void ParameterManager::ParsePaged(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --paged
@param aValue
The value passed to --paged option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParsePaged returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParsePaged)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--paged");

	if (aPM->IsCodeUnpaged() || aPM->IsCodeDefaultPaged())
	{
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
	}

	aPM->SetCodePaged(true);
}

/**
This function set the Unpaged flag if --unpaged option is passed to the program.

void ParameterManager::ParseUnpaged(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --unpaged
@param aValue
The value passed to --unpaged option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseUnpaged returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseUnpaged)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--unpaged");

	if (aPM->IsCodePaged() || aPM->IsCodeDefaultPaged())
	{
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
	}
	
	aPM->SetCodeUnpaged(true);
}

/**
This function set the Defaultpaged flag if --defaultpaged option is passed to the program.

void ParameterManager::ParseDefaultpaged(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --defaultpaged
@param aValue
The value passed to --defaultpaged option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseDefaultpaged returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDefaultPaged)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--defaultpaged");

	if (aPM->IsCodePaged() || aPM->IsCodeUnpaged())
	{
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
	}
	
	aPM->SetCodeDefaultPaged(true);
}


/**
@internalComponent
@released
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseCodePaging)
{
	INITIALISE_PARAM_PARSER;

	if (strnicmp(aValue, "paged", 5)==0)
    {
        aPM->SetCodePaged(true);
    }
	else if (strnicmp(aValue, "unpaged", 7)==0)
    {
        aPM->SetCodeUnpaged(true);
    }
	else if (strnicmp(aValue, "default", 7)==0)
    {
        aPM->SetCodeDefaultPaged(true);
    }
	else
    {
        throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, aOption);
    }

	// Check that we haven't been given conflicting options.

    unsigned check = 0;

    if ( aPM->IsCodePaged() )
    {
        check++;
    }
    if ( aPM->IsCodeUnpaged() )
    {
        check++;
    }
    if ( aPM->IsCodeDefaultPaged() )
    {
        check++;
    }

    if (check > 1)
    {
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
    }
}

/**
@internalComponent
@released
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDataPaging)
{
	INITIALISE_PARAM_PARSER;

	if (strnicmp(aValue, "paged", 5)==0)
    {
        aPM->SetDataPaged(true);
    }
	else if (strnicmp(aValue, "unpaged", 7)==0)
    {
        aPM->SetDataUnpaged(true);
    }
	else if (strnicmp(aValue, "default", 7)==0)
    {
        aPM->SetDataDefaultPaged(true);
    }
	else
    {
        throw InvalidArgumentError(INVALIDARGUMENTERROR, aValue, aOption);
    }

	// Check that we haven't been given conflicting options.

    unsigned check = 0;

    if ( aPM->IsDataPaged() )
    {
        check++;
    }
    if ( aPM->IsDataUnpaged() )
    {
        check++;
    }
    if ( aPM->IsDataDefaultPaged() )
    {
        check++;
    }

    if (check > 1)
    {
		throw InvalidInvocationError(INVALIDINVOCATIONERROR);
    }
}


/**
This function sets the iExcludeUnwantedExports flag if --excludeunwantedexports option is passed to the program.

void ParameterManager::ParseExcludeUnwantedExports(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --excludeunwantedexports
@param aValue
The value passed to --excludeunwantedexports, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseExcludeUnwantedExports returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseExcludeUnwantedExports)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--excludeunwantedexports");
	aPM->SetExcludeUnwantedExports(true);
}

/**
This function sets the customdlltarget flag if --customdlltarget option is passed to the program.

void ParameterManager::ParseIsCustomDllTarget(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --customdlltarget
@param aValue
The value passed to --customdlltarget option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseIsCustomDllTarget returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseIsCustomDllTarget)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--customdlltarget");
	aPM->SetCustomDllTarget(true);
}

DEFINE_PARAM_PARSER(ParameterManager::ParseSymNamedLookup)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--namedlookup");
	aPM->SetSymNamedLookup(true);
}

/**
This function set the iDebuggable flag if --debuggable option is passed to the program.

void ParameterManager::ParseDebuggable(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --debuggable
@param aValue
The value passed to --debuggable option, in this case NULL
@param aDesc
Pointer to function ParameterManager::ParseDebuggable returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDebuggable)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--debuggable");
	aPM->SetDebuggable(true); 
}


DEFINE_PARAM_PARSER(ParameterManager::ParseSmpSafe)
{
	INITIALISE_PARAM_PARSER;
	CheckInput(aValue, "--smpsafe");
	aPM->SetSmpSafe(true); 
}

static const ParameterManager::TargetTypeDesc DefaultTargetTypes[] =
{
	{ "DLL", EDll },
	{ "LIB", ELib },
	// allow full name
	{ "LIBRARY", ELib},
	{ "IMPLIB", ELib},
	{ "EXE", EExe},
	{ "ANI", EPolyDll},
	{ "APP", EPolyDll},
	{ "CTL", EPolyDll},
	{ "CTPKG", EPolyDll},
	{ "FSY", EPolyDll},
	{ "LDD", EPolyDll},
	{ "ECOMIIC", EPolyDll},
	{ "PLUGIN", EPolyDll},
	{ "PLUGIN3", EPolyDll},
	{ "KDLL", EPolyDll},
	{ "KEXT", EPolyDll},
	{ "MDA", EPolyDll},
	{ "MDl", EPolyDll},
	{ "RDL", EPolyDll},
	{ "NOTIFIER", EPolyDll},
	{ "NOTIFIER2", EPolyDll},
	{ "TEXTNOTIFIER2", EPolyDll},
	{ "PDD", EPolyDll},
	{ "PDL", EPolyDll},
	{ "VAR", EPolyDll},
	{ "VAR2", EPolyDll},
	{ "EXEXP", EExexp},
	{ "STDEXE", EStdExe},
	{ "STDDLL", EDll},
	{ 0, EInvalidTargetType }
};

/**
Function to check if the given target type is a valid one from the list.

@internalComponent
@released

@param aArg
Value to be checked, the one that is passed to '--targettype'.
@return True if the provided value is a valid targettype.
*/
static ETargetType IsDefaultTargetType(const char * aArg)
{
	for (int i = 0; DefaultTargetTypes[i].iName; i++)
	{
		if (aArg && !stricmp(aArg, DefaultTargetTypes[i].iName)) 
			return DefaultTargetTypes[i].iTargetType;
	}
	return EInvalidTargetType;
}

/**
Function to check if the given target type is a valid one from the list. If no value is
passed or if unsupported targettypes are passed, then the appropriate warnings are displayed.

@internalComponent
@released

@param aArg
Value to be checked, the one that is passed to '--targettype'.
@return the appropriate matching value from the list.
*/
ETargetType ParameterManager::ValidateTargetType(const char * aArg)
{
	ETargetType res = IsDefaultTargetType(aArg);
	if (res == EInvalidTargetType)
	{
		if (aArg)
			MessageHandler::GetInstance()->ReportMessage(WARNING, UNSUPPORTEDTARGETTYPEERROR,aArg);
		else
			MessageHandler::GetInstance()->ReportMessage(WARNING, TARGETTYPENOTSPECIFIEDERROR);
	}
	return res;
}

/**
This function set the target type that is passed through --targettype option.

void ParameterManager::ParseTargetTypeName(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --targettype
@param aValue
The target type passed to --targettype option
@param aDesc
Pointer to function ParameterManager::ParseTargetTypeName returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseTargetTypeName)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetTargetTypeName(aPM->ValidateTargetType(aValue));
}

/**
This function set the output DSO file name that is passed through --dso option.

void ParameterManager::ParseDSOOutput(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --dso
@param aValue
The DSO file name passed to --dso option
@param aDesc
Pointer to function ParameterManager::ParseDSOOutput returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseDSOOutput)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetDSOOutput(aValue);
}

/**
This function displays the usage information if --help option is passed in.
For invalid option, this function displays the usage information and throws the
appropriate error message.

void ParameterManager::ParamHelp(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --help or -h
@param aValue
The value passed to --help option, in this case, no value, hence 0.
@param aDesc
Pointer to function ParameterManager::ParamHelp returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParamHelp)
{
	INITIALISE_PARAM_PARSER;
	using std::cerr;
	using std::endl;

	cerr << "\nSymbian Post Linker, " << "Elf2E32"
         << " V" << MajorVersion << "." << MinorVersion << " (Build "<<Build <<")"
         << endl;

	cerr << "Usage:\t" << "elf2e32" << " [options] [filename]\n" << endl;
    
    cerr << "Options:\n" ;

	const OptionDesc * aHelpDesc = aPM->iOptionMap["help"];
	int lim = aPM->NumOptions();
	for (int i = 0; i < lim; i++)
	{
		if (aPM->iOptions[i].iName == aHelpDesc->iName)
		{
			cerr << '\t' << aPM->iParamPrefix << aPM->iOptions[i].iName 
				<< " : This command." << endl;
		}
		else
		{
			cerr << '\t' << aPM->iParamPrefix << aPM->iOptions[i].iName;
			if (aPM->iOptions[i].iDoc) cerr << aPM->iParamEquals << aPM->iOptions[i].iDoc;
			cerr << endl;
		}
	}
	
	if (!aDesc)
		exit(EXIT_FAILURE);
	else
		exit(EXIT_SUCCESS);
}

/**
This function set the LibPath that is passed through --libpath option.

void ParameterManager::ParseLibPath(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --libpath
@param aValue
The LibPath value passed to --libpath option.
@param aDesc
Pointer to function ParameterManager::ParseLibPath returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseLibPaths)
{
	INITIALISE_PARAM_PARSER;
	if (!aValue)
		throw ParameterParserError(NOARGUMENTERROR, "--libpath");
	string aPathList(aValue);
	const char* p = aPathList.c_str(); 
	int n =  strlen(p); 
	int q = 0;
	while (n)
	{
		int nq = aPathList.find_first_of(";", q, sizeof(*p));
		if (nq > 0)
		{
			// handle case where there are multiple paths which are ; separated
			int len = nq - q;
			if (p[len-1] == aPM->DirectorySeparator()) len--;
			char * path = new char[len+1];
			memcpy(path, p+q, len);
			path[len] = 0;
			aPM->iLibPathList.push_back(path);
			n -= nq - q + 1;
			q = nq+1;
		}
		else
		{
			p += q;
			int len = strlen(p);
			if (p[len-1] == aPM->DirectorySeparator()) len--;
			char * path = new char[len+1];
			memcpy(path, p, len);
			path[len] = 0;
			aPM->iLibPathList.push_back(path);
			break;
		}
	}
}

/**
This function sets the linkas dll name when --linkas option is passed in.

void ParameterManager::ParseLinkAs(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --linkas
@param aValue
The DLL name to be linked with passed through --linkas option
@param aDesc
Pointer to function ParameterManager::ParseLinkAs returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseLinkAs)
{
	INITIALISE_PARAM_PARSER;
	aPM->SetLinkDLLName(aValue);
}

/**
This function set the sub options that are passed through --dump option.

void ParameterManager::ParseFileDump(ParameterManager * aPM, char * aOption, char * aValue, void * aDesc)

@internalComponent
@released

@param aPM
Pointer to the ParameterManager
@param aOption
Option that is passed as input, in this case --dump
@param aValue
The FileDump suboptions passed to --dump option.
@param aDesc
Pointer to function ParameterManager::ParseFileDump returning void.
*/
DEFINE_PARAM_PARSER(ParameterManager::ParseFileDump)
{
      INITIALISE_PARAM_PARSER;
      aPM->SetFileDumpOptions(aValue);
}

/**
This function sets the target type that is passed as input through the --targettype option
and sets the flag if --targettype option is passed in.

@internalComponent
@released

@param aTargetTypeVal
Name of the input target type if provided as input through --targettype or 0.
*/
void ParameterManager::SetTargetTypeName(ETargetType aTargetTypeVal)
{
	iTargetTypeOption = true;
	iTargetTypeName = aTargetTypeVal;
}

/**
This function sets the DEF file name that is passed as input through the --definput option and
sets the flag if --definput option is passed in.

@internalComponent
@released

@param aDefInputVal
Name of the input DEF file if provided as input through --definput or 0.
*/
void ParameterManager::SetDefInput(char * aDefInputVal)
{
	iDefFileInOption = true;
	iDefInput = aDefInputVal;
}

/**
This function sets the DSO file name that is passed as input through the --dso option
and sets the flag if the --dso option is passed in.

@internalComponent
@released

@param aDSOOutputVal
Name of the output DSO file if provided as input through --dso or 0.
*/
void ParameterManager::SetDSOOutput(char * aDSOOutputVal)
{
	iDSOFileOutOption = true;
	iDSOOutput = aDSOOutputVal;
}

/**
This function sets the Elf file name that is passed as input through the --dsoin option.

@internalComponent
@released

@param aSetElfInput
Name of the input Elf file if provided as input through --dsoin or 0.
*/
void ParameterManager::SetElfInput(char * aElfInputVal)
{
	iElfFileInOption = true;
	iElfInput = aElfInputVal;
}

/**
This function sets the E32 name that is passed as input through the --e32dump option.

@internalComponent
@released

@param aSetE32Input
Name of the input E32 image if provided as input through --e32dump or 0.
*/
void ParameterManager::SetE32Input(char * aSetE32Input)
{
	iE32ImageInOption = true;
	iE32Input = aSetE32Input;
}

/**
This function sets the E32 dump options that is passed as input through the --dump option.

@internalComponent
@released

@param aSetFileDumpOptions
Dump sub options passed to --dump option
*/
void ParameterManager::SetFileDumpOptions(char * aSetFileDumpOptions)
{
	iFileDumpOption = true;
	iFileDumpSubOptions = aSetFileDumpOptions;

	if (aSetFileDumpOptions)
	{
		iDumpOptions=0;
		while (char c = *(aSetFileDumpOptions++))
		{
			if (c < 'a')
				c += 'a'-'A';
			switch(c)
			{
			case 'h': iDumpOptions |= EDumpHeader; break;
			case 's': iDumpOptions |= EDumpSecurityInfo; break;
			case 'c': iDumpOptions |= EDumpCode; break;
			case 'd': iDumpOptions |= EDumpData; break;
			case 'e': iDumpOptions |= EDumpExports; break;
			case 'i': iDumpOptions |= EDumpImports; break;
			case 'a': iDumpOptions |= EDumpAsm; break; //Dump the Assembler code
			case 't': iDumpOptions |= EDumpSymbols;break;
			default: 
				iDumpOptions=0;
				return;
			}
		}
	}
}

/**
This function extracts the E32 image output that is passed as input through the --output option.

@internalComponent
@released

@param aSetE32Output
Name of the output E32 image output if provided as input through --output or 0.
*/
void ParameterManager::SetE32Output(char * aSetE32Output)
{
	iOutFileOption = true;
	iOutFileName = aSetE32Output;
}

/**
This function sets the output DEF file name that is passed as input through the --defoutput option.

@internalComponent
@released

@param aSetDefOutput
Name of the output DEF file if provided as input through --defoutput or 0.
*/
void ParameterManager::SetDefOutput(char * aSetDefOutput)
{
	iDefFileOutOption = true;
	iDefOutput = aSetDefOutput;
}

/**
This function sets the name of the DLL (that the DSO is to be linked with) that is passed 
as input through the --linkas option and sets the flag if the --linkas option is passed in.

@internalComponent
@released

@param aSetLinkDLLName
The DLL name with which the DSO is to be linked with
*/
void ParameterManager::SetLinkDLLName(char * aSetLinkDLLName)
{
	iLinkAsOption = true;
	iLinkDLLName = aSetLinkDLLName;
}

/**
This function sets the priority value.

@internalComponent
@released

@param anewVal
priority value passed in to --priority option.
*/
void ParameterManager::SetPriority(TProcessPriority anewVal)
{
	iPriorityOption = true;
	iPriorityVal = anewVal;
}

/**
This function sets the capability value.

@internalComponent
@released

@param anewVal
Capability value passed in to --capability option.
*/
void ParameterManager::SetCapability(unsigned int anewVal)
{
	iCapability[0] = anewVal;
}

/**
This function sets the capability value.

@internalComponent
@released

@param anewVal
Capability value passed in to --capability option.
*/
void ParameterManager::SetCapability(SCapabilitySet & anewVal)
{
	iCapability = anewVal;
}

/**
This function sets the list of predefined symbols passed to --sysdef option.

@internalComponent
@released

@param aOrdinalnum
Ordinal number of the predefined symbols
@param aSymbol
Symbol Name
@param aCount
Position of the predefined symbol
*/
void ParameterManager::SetSysDefs(unsigned int aOrdinalnum, char* aSymbol, int aCount)
{
	iSysDefOption = 1;
	iSysDefSymbols[aCount].iSysDefOrdinalNum = aOrdinalnum;
	iSysDefSymbols[aCount].iSysDefSymbolName = aSymbol;
	iSysDefCount = (aCount+1);
}

/**
This function sets the output LOG file name that is passed as input through the --log option.

@internalComponent
@released

@param aSetLogFile
Name of the output LOG file if provided as input through --log or 0.
*/
void ParameterManager::SetLogFile(char * aSetLogFile)
{
	iLogFileOption = true;
	iLogFileName = aSetLogFile;
}

/**
This function sets the Message file name that is passed as input through the --messagefile option.

@internalComponent
@released

@param aMessageFile
Name of the Message file if provided as input through --messagefile or 0.
*/
void ParameterManager::SetMessageFile(char * aMessageFile)
{
	iMessageFileOption = true;
	iMessageFileName = aMessageFile;
}

/**
This function sets the Message file name that is passed as input through the --dumpmessagefile option.

@internalComponent
@released

@param aDumpMessageFile
Name of the Message file to be dumped if provided as input through --dumpmessagefile or 0.
*/
void ParameterManager::SetDumpMessageFile(char * aDumpMessageFile)
{
	iDumpMessageFileOption = true;
	iDumpMessageFileName = aDumpMessageFile;
}

/**
This function sets iFixedAddress if --fixedaddress is passed in.

@internalComponent
@released

@param aVal
True if --fixedaddress is passed in.
*/
void ParameterManager::SetFixedAddress(bool aSetFixedAddress)
{
	iFixedAddress = aSetFixedAddress;
}

/**
This function sets iCompress if --uncompressed is passed in.

@internalComponent
@released

@param aVal
True if --uncompressed is passed in.
*/
void ParameterManager::SetCompress(bool aSetCompress)
{
	iCompress = aSetCompress;
}

/**
This function sets iCompress if --uncompressed is passed in.

@internalComponent
@released

@param aVal
True if --uncompressed is passed in.
*/
void ParameterManager::SetCompressionMethod(UINT aCompressionMethod)
{
	
	iCompressionMethod = aCompressionMethod;
}


/**
This function sets iCallEntryPoint if --callentry is passed in.

@internalComponent
@released

@param aVal
True if --callentry is passed in.
*/
void ParameterManager::SetCallEntryPoint(bool aSetCallEntryPoint)
{
	iCallEntryPoint = aSetCallEntryPoint;
}

/**
This function sets the Version information passed to '--version' option.

@internalComponent
@released

@param aVersion information
Version information passed to '--version' option.
*/
void  ParameterManager::SetVersion(UINT aSetVersion)
{
	iVersionOption = true;
	iVersion = aSetVersion;
}

/**
This function sets the UID1 passed to '--uid1' option.

@internalComponent
@released

@param aUID1
UID1 passed to '--uid1' option.
*/
void  ParameterManager::SetUID1(UINT aUID1)
{
	iUid1Option = true;
	iUID1 = aUID1;
}

/**
This function sets the UID2 passed to '--uid2' option.

@internalComponent
@released

@param aUID2
UID2passed to '--uid2' option.
*/
void  ParameterManager::SetUID2(UINT aUID2)
{
	iUID2 = aUID2;
}

/**
This function sets the UID3 passed to '--uid3' option.

@internalComponent
@released

@param aUID3
UID3 passed to '--uid3' option.
*/
void  ParameterManager::SetUID3(UINT aUID3)
{
	iUID3 = aUID3;
}

/**
This function sets the Secure ID passed to '--sid' option.

@internalComponent
@released

@param aSetSecureID
Secure ID passed to '--sid' option.
*/
void  ParameterManager::SetSecureId(UINT aSetSecureID)
{
	iSecureIDOption = true;
	iSecureID = aSetSecureID;
}

/**
This function sets the Vendor ID passed to '--vid' option.

@internalComponent
@released

@param aSetVendorID
Vendor ID passed to '--vid' option.
*/
void  ParameterManager::SetVendorId(UINT aSetVendorID)
{
	iVendorIDOption = true;
	iVendorID = aSetVendorID;
}

/**
This function sets the heap committed size passed to '--heap' option.

@internalComponent
@released

@param aSetHeapCommittedSize
stack committed size passed to '--heap' option.
*/
void  ParameterManager::SetHeapCommittedSize(UINT aSetHeapCommittedSize)
{
	iHeapCommittedSize = aSetHeapCommittedSize;
}

/**
This function sets the heap reserver size passed to '--heap' option.

@internalComponent
@released

@param aSetStackReservedSize
stack reserved size passed to '--heap' option.
*/
void  ParameterManager::SetHeapReservedSize(UINT aSetHeapReservedSize)
{
	iHeapReservedSize = aSetHeapReservedSize;
}

/**
This function sets the stack committed size passed to '--stack' option.

@internalComponent
@released

@param aSetStackCommittedSize
stack committed size passed to '--stack' option.
*/
void  ParameterManager::SetStackCommittedSize(UINT aSetStackCommittedSize)
{
	iStackCommittedSize = aSetStackCommittedSize;
}

/**
This function sets iUnfrozen if --unfrozen is passed in.

@internalComponent
@released

@param aVal
True if --unfrozen is passed in.
*/
void ParameterManager::SetUnfrozen(bool aVal)
{
	iUnfrozen = aVal;
}

/**
This function sets iIgnoreNonCallable if --ignorenoncallable is passed in.

@internalComponent
@released

@param aVal
True if --ignorenoncallable is passed in.
*/
void ParameterManager::SetIgnoreNonCallable(bool aVal)
{
	iIgnoreNonCallable = aVal;
}

/**
This function sets iDllDataP if --dlldata is passed in.

@internalComponent
@released

@param anewVal
True if --dlldata is passed in.
*/
void ParameterManager::SetDllDataP(bool anewVal)
{
	iDllDataP = anewVal;
}

/**
This function sets the FPU type based on the parsed '--fpu' option.

@internalComponent
@released

@param aSetVendorID
FPU type passed to the '--fpu' option.
*/
void  ParameterManager::SetFPU(UINT aFPU)
{
	iFPUOption = true;
	iFPU = aFPU;
}


void ParameterManager::SetCodePaged(bool anewVal)
{
	iCodePaged = anewVal;
}

void ParameterManager::SetCodeUnpaged(bool anewVal)
{
	iCodeUnpaged = anewVal;
}

void ParameterManager::SetCodeDefaultPaged(bool anewVal)
{
	iCodeDefaultPaged = anewVal;
}

void ParameterManager::SetDataPaged(bool anewVal)
{
	iDataPaged = anewVal;
}

void ParameterManager::SetDataUnpaged(bool anewVal)
{
	iDataUnpaged = anewVal;
}

void ParameterManager::SetDataDefaultPaged(bool anewVal)
{
	iDataDefaultPaged = anewVal;
}

void ParameterManager::SetSymNamedLookup(bool aVal)
{
	iSymNamedLookup = aVal;
}

/**
This function sets iExcludeUnwantedExports if --excludeunwantedexports is passed in.

@internalComponent
@released

@param aVal
True if --excludeunwantedexports is passed in.
*/
void ParameterManager::SetExcludeUnwantedExports(bool aVal)
{
	iExcludeUnwantedExports = aVal;
}

/**
This function sets iIsCustomDllTarget if --customdlltarget is passed in.

@internalComponent
@released

@param aVal
True if --customdlltarget is passed in.
*/
void ParameterManager::SetCustomDllTarget(bool aVal)
{
	iCustomDllTarget = aVal;
}

/**
This function sets iDebuggable if --debuggable is passed in.

@internalComponent
@released

@param aVal
True if --debuggable is passed in.
*/
void ParameterManager::SetDebuggable(bool aVal)
{
	iDebuggable = aVal;
}


void ParameterManager::SetSmpSafe(bool aVal)
{
	iSmpSafe = aVal;
}
