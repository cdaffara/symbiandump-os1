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
// Implementation of the Header file for Class ParameterManager of the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined(SYMBIAN_PARAMETERMANAGER_H_)
#define SYMBIAN_PARAMETERMANAGER_H_

#include "parameterlistinterface.h"
#include <vector>
#include <map>
#include <string>

class UseCaseBase;
class Symbol;



/**
This class is derived from ParameterListInterface (the abstract base class). 

@internalComponent
@released
*/
class ParameterManager : public ParameterListInterface
{

public:

	typedef std::vector<char *> ParamList;

	struct OptionDesc
	{
		char * iName;
		const void * iParser;
		char * iDoc;
	};

	struct TargetTypeDesc
	{
		const char * iName;
		ETargetType iTargetType;
	};

	struct PriorityValueDesc
	{
		const char * iName;
		TProcessPriority iPriority;
	};
	
	struct CompressionMethodDesc
	{
		const char *iMethodName;
		UINT		iMethodUid;
	};

	/*struct SysDefs
	{
		int iSysDefOrdinalNum;
		char * iSysDefSymbolName;
	};

	typedef struct SysDefs Sys;*/

	typedef std::string string;
	typedef std::less<string> OptionCompare;
	typedef std::map<string, const OptionDesc *, OptionCompare> OptionMap;
	typedef vector<char *> LibSearchPaths;

	typedef void (*ParserFn)(ParameterManager *, char *, char *, const OptionDesc *);


	#define DECLARE_PARAM_PARSER(name) \
		static void name(ParameterManager* aPM, char* aOption, char* aValue, void* aDesc)
	
	#define DEFINE_PARAM_PARSER(name) \
		void name(ParameterManager * aPM, char * aOption, char* aValue, void * aDesc)
	
	#define INITIALISE_PARAM_PARSER \
		aOption = aOption; \
		aValue = aValue; \
		aDesc = aDesc;

	DECLARE_PARAM_PARSER(ParseDefInput);
	DECLARE_PARAM_PARSER(ParseDSOOutput);
	DECLARE_PARAM_PARSER(ParseTargetTypeName);
	DECLARE_PARAM_PARSER(ParseLinkAs);
	DECLARE_PARAM_PARSER(ParseDefOutput);
	DECLARE_PARAM_PARSER(ParseElfInput);
	DECLARE_PARAM_PARSER(ParseFileDump);
	DECLARE_PARAM_PARSER(ParseE32ImageInput);
	DECLARE_PARAM_PARSER(ParseOutput);
	DECLARE_PARAM_PARSER(ParseLogFile);
	DECLARE_PARAM_PARSER(ParseMessageFile);
	DECLARE_PARAM_PARSER(ParseDumpMessageFile);
	
	DECLARE_PARAM_PARSER(ParamHelp);
	DECLARE_PARAM_PARSER(ParseUID1);
	DECLARE_PARAM_PARSER(ParseUID2);
	DECLARE_PARAM_PARSER(ParseUID3);


	DECLARE_PARAM_PARSER(ParseCapability);
	DECLARE_PARAM_PARSER(ParseSecureId);
	DECLARE_PARAM_PARSER(ParseVendorId);
	DECLARE_PARAM_PARSER(ParseFixedAddress);
	DECLARE_PARAM_PARSER(ParseUncompressed);
	DECLARE_PARAM_PARSER(ParseCompressionMethod);
	DECLARE_PARAM_PARSER(ParseHeap);
	DECLARE_PARAM_PARSER(ParseStackCommitted);
	DECLARE_PARAM_PARSER(ParseUnfrozen);
	DECLARE_PARAM_PARSER(ParseIgnoreNonCallable);
	DECLARE_PARAM_PARSER(ParseLibPaths);
	DECLARE_PARAM_PARSER(ParseSysDefs);
	DECLARE_PARAM_PARSER(ParseAllowDllData);
	DECLARE_PARAM_PARSER(ParsePriority);
	DECLARE_PARAM_PARSER(ParseVersion);
	DECLARE_PARAM_PARSER(ParseCallEntryPoint);
	DECLARE_PARAM_PARSER(ParseFPU);

	DECLARE_PARAM_PARSER(ParsePaged);
	DECLARE_PARAM_PARSER(ParseUnpaged);
	DECLARE_PARAM_PARSER(ParseDefaultPaged);

	DECLARE_PARAM_PARSER(ParseCodePaging);
	DECLARE_PARAM_PARSER(ParseDataPaging);

	DECLARE_PARAM_PARSER(ParseExcludeUnwantedExports);
	DECLARE_PARAM_PARSER(ParseIsCustomDllTarget);
	DECLARE_PARAM_PARSER(ParseSymNamedLookup);
	DECLARE_PARAM_PARSER(ParseDebuggable);
	DECLARE_PARAM_PARSER(ParseSmpSafe);

	ParameterManager(int aArgc, char** aArgv);
	virtual ~ParameterManager();
	void ParameterAnalyser();
	void SetDefInput(char * aDefInputVal);
	void SetDSOOutput(char * aDSOOutputVal);
	void SetElfInput(char * aSetElfInput);
	void SetE32Input(char * aSetE32Input);
	void SetFileDumpOptions(char * aSetE32DumpOptions);
	void SetE32Output(char * aSetE32Output);
	void SetDefOutput(char * aSetDefOutput);
	void SetTargetTypeName(ETargetType  aSetTargetTypeName);
	void SetLinkDLLName(char * aSetLinkDLLName);
	void SetUID1(UINT aSetUINT1);
	void SetUID2(UINT aSetUINT2);
	void SetUID3(UINT aSetUINT3);
	void SetFixedAddress(bool aSetFixedAddress);
	void SetCompress(bool aSetCompress);
	void SetCompressionMethod(UINT aCompressionMethod);
	void SetSecureId(UINT aSetSecureID);
	void SetVendorId(UINT aSetVendorID);
	void SetHeapCommittedSize(UINT aSetHeapCommittedSize);
	void SetHeapReservedSize(UINT aSetHeapReservedSize);
	void SetStackCommittedSize(UINT aSetStackCommittedSize);
	void SetUnfrozen(bool aVal);
	void SetIgnoreNonCallable(bool aVal);
	void SetCapability(unsigned int newVal);
	void SetCapability(SCapabilitySet & newVal);
	void SetSysDefs(unsigned int aSysDefOrdinal, char * aSysDefSymbol, int aCount);
	void SetLogFile(char * aSetLogFile);
	void SetMessageFile(char *aMessageFile);
	void SetDumpMessageFile(char *aDumpMessageFile);
	void SetDllDataP(bool newVal);
	void SetPriority(TProcessPriority anewVal);
	void SetVersion(UINT aSetVersion);
	void SetCallEntryPoint(bool aCallEntryPoint);
	void SetFPU(UINT aVal);

	void SetCodePaged(bool);
	void SetCodeUnpaged(bool);
	void SetCodeDefaultPaged(bool);

	void SetDataPaged(bool);
	void SetDataUnpaged(bool);
	void SetDataDefaultPaged(bool);
	
	void SetExcludeUnwantedExports(bool aVal);
	void SetCustomDllTarget(bool aVal);
	void SetSymNamedLookup(bool aVal);
	void SetDebuggable(bool aVal);
	void SetSmpSafe(bool aVal);

	int NumOptions();
	int NumShortOptions();
	void InitParamParser();
	void ParseCommandLine();
	void RecordImageLocation();
	char * Path(char * aArg);
	ETargetType TargetTypeName();
	ETargetType ValidateTargetType(const char * aTargetType);
	LibSearchPaths& LibPath();
	char * DefInput();
	char * ElfInput();
	char * E32Input();
	bool DefFileInOption();
	bool DefFileOutOption();
	bool ElfFileInOption();
	bool E32ImageInOption();
	bool FileDumpOption();
	bool DSOFileOutOption();
	bool E32OutOption();
	bool LinkAsOption();
	bool Uid1Option();
	bool SecureIdOption();
	bool VendorIdOption();
	bool SysDefOption();
	bool LogFileOption();
	bool MessageFileOption();
	bool DumpMessageFileOption();
	bool DllDataP();
	TProcessPriority Priority();
	bool PriorityOption();
	bool VersionOption();
	bool CallEntryPoint();
	bool FPUOption();

	char * DefOutput();
	char * DSOOutput();
	char * E32ImageOutput();
	char * LinkAsDLLName();
	char * FileName(char * aArg);
	char * LogFile();
	char * MessageFile();
	char * DumpMessageFile();
	char * FileDumpOptions();
	char * FileDumpSubOptions();
	int DumpOptions();
	int SysDefCount();
	char DirectorySeparator();
	//int SysDefOrdinalNum();
	//char * SysDefSymbol();
	Sys SysDefSymbols(int count);
	UINT Uid1();
	UINT Uid2();
	UINT Uid3();
	UINT SecureId();
	UINT VendorId();
	UINT Version();
	bool FixedAddress();
	bool Compress();
	UINT CompressionMethod();
	size_t HeapCommittedSize();
	size_t HeapReservedSize();
	size_t StackCommittedSize();
	bool Unfrozen();
	bool IgnoreNonCallable();
	UseCaseBase * SelectUseCase();
	SCapabilitySet Capability();
	void ParseCapability1(const char * name, const char * end, SCapabilitySet& aCapabilities, bool invert);
	void ParseCapabilitiesArg(SCapabilitySet& aCapabilities, const char *aText);
	UINT FPU();

	bool IsCodePaged();
	bool IsCodeUnpaged();
	bool IsCodeDefaultPaged();

	bool IsDataPaged();
	bool IsDataUnpaged();
	bool IsDataDefaultPaged();

	bool ExcludeUnwantedExports();
	bool IsCustomDllTarget();
	bool SymNamedLookup();
	bool IsDebuggable();
	bool IsSmpSafe();

private:
	/** The number of command line arguments passed into the program */
	int iArgc;

	/** The listing of all the arguments */
	char ** iArgv;

	/** REVISIT */
	char * iImageLocation;

	/** REVISIT */
	char * iImageName;

	/** To check if the --targettypeoption (Option to pass the target type of the final image) is provided */
	 bool iTargetTypeOption;

	/** To check if the --definput (Option to pass the input DEF File) is passed as input */
	 bool iDefFileInOption;

	/** To check if the --defoutput (Option to pass the output DEF File name) is passed as input */
	bool iDefFileOutOption;
	
	/** To check if the --dump is passed as input */
	bool iFileDumpOption;

	/** To check if the --dso (Option to pass the output DSO File name) is passed as input */
	bool iDSOFileOutOption;
	
	/** To check if the --output (Option to pass the output image name) is passed as input */
	bool iOutFileOption;
	
	/** To check if the --elfinput (Option to pass the input Elf File) is passed as input */
	bool iElfFileInOption;
	
	/** To check if the --e32input (Option to pass the input E32 File) is passed as input */
	bool iE32ImageInOption;
	
	/** To check if the --linkas (Option to pass DLL name to be linked with) is passed as input */
	bool iLinkAsOption;

	bool iUid1Option;
	bool iSecureIDOption;
	bool iVendorIDOption;

	/** System level identifier, identifies the general type of a Symbian OS object */
	UINT iUID1;

	/** Interface identifier, distinguishes within a type (i.e.within a UID1) */
	UINT iUID2;

	/** Project identifier, identifies a particular subtype */ 
	UINT iUID3;

	UINT iSecureID;

	UINT iVendorID;

	bool iCompress;
	UINT iCompressionMethod;

	bool iFixedAddress;

	size_t iHeapCommittedSize;
	size_t iHeapReservedSize;
	size_t iStackCommittedSize;
	bool iUnfrozen;
	bool iIgnoreNonCallable;


	/** List of the parameters */
	ParamList iParamList;

	/** The short prefix '-' used for the command line options for the program */
	static const char * iParamShortPrefix;

	/** The normal prefix '--' used for the command line options for the program */
	static const char * iParamPrefix;

	/** The '=' used for passing the arguments to the command line options for the program */
	static const char iParamEquals; 

	/** The list of command line options (with normal prefix '--') that will be accepted by the program */
	static const OptionDesc iOptions[];

	/** The list of command line options (with short prefix '-') that will be accepted by the program */
	static const OptionDesc iShortOptions[];

	/** The map between the command line option (with normal prefix '--') and the corresponding function */
	OptionMap iOptionMap;

	/** The map between the command line option (with short prefix '-') and the corresponding function */
	OptionMap iShortOptionMap;

	/** The usecase that is selected, could either be LibraryTarget or DLLTarget or EXETarget */
	UseCaseBase *iUseCase;

	/** Target Type that is passed as input to the --targettype option */
	ETargetType iTargetTypeName;

	/** File name of the output DEF file passed as input to the --defoutput option */
	char * iDefOutput;

	/** File name of the output DSO file passed as input to the --dso option */
	char * iDSOOutput;

	/** File name of the output image passed as input to the --output option */
	char * iOutFileName;

	/** File name of the input DEF file passed as input to the --definput option */
	char * iDefInput;

	/** File name of the input DSO file passed as input to the --dsoin option */
	char * iElfInput;

	/** File name of the input E32 image passed as input to the --e32dump option */
	char * iE32Input;

	/** File name of the DLL to be linked with passed as input to the --linkas option */
	char * iLinkDLLName;

	/** Path name of the intermediate libraries passed as input to the --libpath option */
	char * iLibPath;

	int iDumpOptions;
	char *iFileDumpSubOptions;

	bool iSysDefOption;
	char * iLogFileName;
	bool iLogFileOption;
	char * iMessageFileName;
	bool iMessageFileOption;
	char * iDumpMessageFileName;
	bool iDumpMessageFileOption;

	bool iDllDataP;
	
	//vector<char*> iLibPathList;
	LibSearchPaths iLibPathList;
	SCapabilitySet iCapability;
	//struct SysDefs iSysDefSymbols[10];
	Sys iSysDefSymbols[10];
	int iSysDefCount;
	bool iPriorityOption;
	TProcessPriority iPriorityVal;
	UINT iVersion;
	bool iVersionOption;
	bool iCallEntryPoint;
	UINT iFPU;
	bool iFPUOption;

	int iArgumentCount;
	
	bool iCodePaged;
	bool iCodeUnpaged;
	bool iCodeDefaultPaged;

	bool iDataPaged;
	bool iDataUnpaged;
	bool iDataDefaultPaged;

	bool iExcludeUnwantedExports;
	bool iCustomDllTarget;
	bool iSymNamedLookup;
	bool iDebuggable;
	bool iSmpSafe;
};


#endif // !defined(SYMBIAN_PARAMETERMANAGER_H_)
