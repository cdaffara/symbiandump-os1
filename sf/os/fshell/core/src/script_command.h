// script_command.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/ioutils.h>
#include <fshell/stringhash.h>

// Helper class for parsing the arguments passed to a script (via "fshell script" or "source script")
class CScriptCommand : public IoUtils::CCommandBase
	{
public:
	static CScriptCommand* NewLC(const TDesC& aScriptPath, TIoHandleSet& aIoHandles);
	~CScriptCommand();
	void ParseCommandLineArgsL(const TDesC& aArgs, IoUtils::CEnvironment& aEnv, RPointerArray<HBufC>* aAdditionalPrefixArguments=NULL);
	void DisplayScriptHelpL();

	TBool ShouldDisplayHelp() const { return iDisplayHelp; }

protected: // From CCommandBase
	const TDesC& Name() const;
	void DoRunL();

private:
	CScriptCommand();
	void ConstructL(const TDesC& aScriptPath, TIoHandleSet& aIoHandles);
	void SetupArgumentL(const IoUtils::TValue& aValue);
	void SetupOptionL(const IoUtils::TValue& aValue);
	void SetEnvironmentVariableFromValueL(IoUtils::CEnvironment& aEnv, const IoUtils::TValue& aValue);

private:
	HBufC* iScriptPath;
	TPtrC iName;
	IoUtils::CCommandInfoFile* iOwnedCif; // We can't access the base class iCif or iFlags directly...

	RPointerArray<HBufC> iNonCifScriptArguments;
	TBool iDisplayHelp;
	TBuf<256> iTempStr;
	};
