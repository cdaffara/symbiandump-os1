// script_command.cpp
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
#include "script_command.h"
#include "fshell.h" // For KScriptArgCount etc
#include <fshell/descriptorutils.h>

using namespace IoUtils;
using namespace LtkUtils;

CScriptCommand* CScriptCommand::NewLC(const TDesC& aScriptPath, TIoHandleSet& aIoHandles)
	{
	CScriptCommand* self = new(ELeave) CScriptCommand;
	CleanupStack::PushL(self);
	self->ConstructL(aScriptPath, aIoHandles);
	return self;
	}

const TDesC& CScriptCommand::Name() const
	{
	return iName;
	}

void CScriptCommand::DoRunL()
	{
	// Never actually runs
	__DEBUGGER();
	}

CScriptCommand::CScriptCommand()
	: CCommandBase()
	{
	}

void CScriptCommand::ConstructL(const TDesC& aScriptPath, TIoHandleSet& aIoHandles)
	{
	BaseConstructL();

	// Duplicate some handles in case we need to display the script's help text.
	IoSession().SetHandle(aIoHandles.IoSession().Handle());
	User::LeaveIfError(IoSession().Duplicate(RThread(), EOwnerThread));
	Stdout().CreateL(IoSession());
	Stdout().Duplicate(aIoHandles.Stdout());
	Stderr().CreateL(IoSession());
	Stderr().Duplicate(aIoHandles.Stderr());
	
	iScriptPath = aScriptPath.AllocL();
	iName.Set(TParsePtrC(*iScriptPath).NameAndExt());
	RFs fs; // Can't use FsL(), that needs an env set up
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	iOwnedCif = CCommandInfoFile::NewL(fs, *iScriptPath); // A script file is also its own cif

	if (iOwnedCif->Name().Length() == 0)
		{
		// No cif data - fall back to just creating a basic array of args (for converting into $1 $2 etc)
		iArguments.AppendStringL(iNonCifScriptArguments, _L("args"), _L("Arguments for the script. These are converted to the environment variables C<$1>, C<$2> etc."), KValueTypeFlagOptional);
		delete iOwnedCif;
		iOwnedCif = NULL;
		}
	else
		{
		SetCif(*iOwnedCif); // SetCif doesn't take ownership (which is what iOwnedCif is for)

		// Now go through and make up "member variables" to correspond to everything in the CIF so we can fool CCommandBase into
		// doing the parsing work for us
		const TInt argCount = iOwnedCif->Arguments().Count();
		for (TInt i = 0; i < argCount; i++)
			{
			SetupArgumentL(iOwnedCif->Arguments()[i]);
			}
		
		const TInt optCount = iOwnedCif->Options().Count();
		for (TInt i = 0; i < optCount; i++)
			{
			const TCommandOption& opt = iOwnedCif->Options()[i];
			SetupOptionL(opt);
			}

		iOwnedCif->AssignL(iArguments, iOptions);
		}

	iOptions.AppendBoolL(iDisplayHelp, 'h', _L("help"), _L("Display help.")); // Always support --help (cannot do this before the call to CCommandInfoFile::AssignL())

	// Our argument data structures should now be fully constructed
	CleanupStack::PopAndDestroy(&fs);
	}

TInt StorageSizeForType(TUint aType)
	{
	TInt size = sizeof(TInt);
	switch (aType)
		{
		case KValueTypeReal:
			size = sizeof(TReal); break;
		case KValueTypeFileName:
			size = sizeof(TFileName2); break;
		case KValueTypeUint64:
		case KValueTypeInt64:
			size = sizeof(TInt64); break;
		default:
			break;
		}
	return size;
	}

void CScriptCommand::SetupArgumentL(const TValue& aValue)
	{
	if (aValue.AcceptsMultiple()) StaticLeaveIfErr(KErrArgument, _L("Arguments with the 'multiple' attribute are not supported for scripts"));

	const TUint type = aValue.Type();
	void* valptr = User::AllocZL(StorageSizeForType(type));
	CleanupStack::PushL(valptr);

	TPtrC name = aValue.Name();
	switch (type)
		{
	case KValueTypeInt:
		iArguments.AppendIntL(*(TInt*)valptr, name);
		break;
	case KValueTypeUint:
		iArguments.AppendUintL(*(TUint*)valptr, name);
		break;
	case KValueTypeString:
		iArguments.AppendStringL(*(HBufC**)valptr, name);
		break;
	case KValueTypeFileName:
		new (valptr) TFileName2; // We only AllocZL'd, we didn't initialise it
		iArguments.AppendFileNameL(*(TFileName2*)valptr, name);
		break;
	case KValueTypeEnum:
		iArguments.AppendEnumL(*(TInt*)valptr, name);
		break;
	case KValueTypeReal:
		iArguments.AppendRealL(*(TReal*)valptr, name);
		break;
	case KValueTypeUint64:
		iArguments.AppendUintL(*(TUint64*)valptr, name);
		break;
	case KValueTypeInt64:
		iArguments.AppendIntL(*(TInt64*)valptr, name);
		break;
	default:
		StaticLeaveIfErr(KErrArgument, _L("Script argument '%S' is of unsupported type %d"), &name, aValue.Type());
		break;
		}
	CleanupStack::Pop(valptr);
	}

void CScriptCommand::SetupOptionL(const IoUtils::TValue& aValue)
	{
	if (aValue.AcceptsMultiple()) StaticLeaveIfErr(KErrArgument, _L("Options with the 'multiple' attribute are not supported for scripts"));

	const TUint type = aValue.Type();
	void* valptr = User::AllocZL(StorageSizeForType(type));
	CleanupStack::PushL(valptr);

	TPtrC name = aValue.Name();
	switch (type)
		{
	case KValueTypeBool:
		iOptions.AppendBoolL(*(TBool*)valptr, name);
		break;
	case KValueTypeInt:
		iOptions.AppendIntL(*(TInt*)valptr, name);
		break;
	case KValueTypeUint:
		iOptions.AppendUintL(*(TUint*)valptr, name);
		break;
	case KValueTypeString:
		iOptions.AppendStringL(*(HBufC**)valptr, name);
		break;
	case KValueTypeFileName:
		new (valptr) TFileName2; // We only AllocZL'd, we didn't initialise it
		iOptions.AppendFileNameL(*(TFileName2*)valptr, name);
		break;
	case KValueTypeEnum:
		iOptions.AppendEnumL(*(TInt*)valptr, name);
		break;
	case KValueTypeReal:
		iOptions.AppendRealL(*(TReal*)valptr, name);
		break;
	case KValueTypeUint64:
		iOptions.AppendUintL(*(TUint64*)valptr, name);
		break;
	case KValueTypeInt64:
		iOptions.AppendIntL(*(TInt64*)valptr, name);
		break;
	default:
		StaticLeaveIfErr(KErrArgument, _L("Script argument '%S' is of unsupported type %d"), &name, aValue.Type());
		break;
		}
	CleanupStack::Pop(valptr);
	}

CScriptCommand::~CScriptCommand()
	{
	if (iOwnedCif)
		{
		// We set all these up, we need to delete them
		for (TInt i = 0; i < iArguments.Count(); i++)
			{
			const TCommandArgument& arg = ((const RCommandArgumentList&)iArguments)[i];
			if (arg.Type() == KValueTypeString) delete *(HBufC**)arg.ValuePtr();
			delete arg.ValuePtr();
			}
		for (TInt i = 0; i < iOptions.Count(); i++)
			{
			const TCommandOption& opt = ((const RCommandOptionList&)iOptions)[i];
			if (opt.ValuePtr() != &iDisplayHelp)
				{
				if (opt.Type() == KValueTypeString) delete *(HBufC**)opt.ValuePtr();
				delete opt.ValuePtr();
				}
			}
		}

	delete iScriptPath;
	delete iOwnedCif;
	iNonCifScriptArguments.ResetAndDestroy();
	Stderr().Close();
	Stdout().Close();
	IoSession().Close();
	}

void CScriptCommand::ParseCommandLineArgsL(const TDesC& aArgs, CEnvironment& aEnv, RPointerArray<HBufC>* aAdditionalPrefixArguments)
	{
	CreateEnvironmentL(&aEnv); // This sets aEnv as our environment. Doesn't copy or take ownership. ParseCommandLineL needs access to the env for things like the escape char setting

	if (aAdditionalPrefixArguments && aAdditionalPrefixArguments->Count())
		{
		// Must be a nicer way of doing this... prefix onto aArgs some args that couldn't possibly require escaping (this way we don't have to worry if anything in aAdditionalPrefixArguments actually does need escaping)
		LtkUtils::RLtkBuf argsBuf;
		CleanupClosePushL(argsBuf);
		TInt c = aAdditionalPrefixArguments->Count();
		while (c--)
			{
			argsBuf.AppendL(_L("placeholder "));
			}
		argsBuf.AppendL(aArgs);
		ParseCommandLineL(argsBuf);
		CleanupStack::PopAndDestroy(&argsBuf);
		}
	else
		{
		ParseCommandLineL(aArgs);
		}	

	if (!iArguments.AllSet())
		{
		User::Leave(KErrArgument);
		}

	// Everything is filled in now, just have to setup aEnv with the results
	TInt argCount = iArguments.Count();
	if (iOwnedCif)
		{
		for (TInt i = 0; i < argCount; i++)
			{
			const TCommandArgument& arg = ((const RCommandArgumentList&)iArguments)[i];
			SetEnvironmentVariableFromValueL(aEnv, arg);
			}

		// Now check for overrides from aAdditionalPrefixArguments
		if (aAdditionalPrefixArguments)
			{
			for (TInt i = 0; i < aAdditionalPrefixArguments->Count(); i++)
				{
				const TCommandArgument& arg = ((const RCommandArgumentList&)iArguments)[i]; // This const cast shouldn't be needed, I don't think, to make sure that we use the const operator[]
				aEnv.SetL(arg.Name(), *(*aAdditionalPrefixArguments)[i]);
				}
			}
		for (TInt i = 0; i < iOptions.Count(); i++)
			{
			const TCommandOption& opt = ((const RCommandOptionList&)iOptions)[i];
			if (opt.ValuePtr() != &iDisplayHelp) SetEnvironmentVariableFromValueL(aEnv, opt);
			}
		}
	else
		{
		// Just set $1, $2 etc
		argCount = iNonCifScriptArguments.Count();
		for (TInt i = 0; i < argCount; i++)
			{
			TBuf<16> varname;
			varname.AppendNum(i+1);
			aEnv.SetLocalL(varname);
			if (aAdditionalPrefixArguments && i < aAdditionalPrefixArguments->Count())
				{
				aEnv.SetL(varname, *(*aAdditionalPrefixArguments)[i]);
				}
			else
				{
				aEnv.SetL(varname, *iNonCifScriptArguments[i]);
				}
			}
		}

	aEnv.SetLocalL(KScriptArgCount);
	aEnv.SetL(KScriptArgCount, argCount);
	}

void CScriptCommand::DisplayScriptHelpL()
	{
	const CTextBuffer* helpText = GetHelpTextL();
	CleanupStack::PushL(const_cast<CTextBuffer*>(helpText));
	PageL(*helpText);
	CleanupStack::PopAndDestroy(const_cast<CTextBuffer*>(helpText));
	}

void CScriptCommand::SetEnvironmentVariableFromValueL(IoUtils::CEnvironment& aEnv, const IoUtils::TValue& aValue)
	{
	const TPtrC name(aValue.Name());
	aEnv.SetLocalL(name);
	void* valptr = aValue.ValuePtr();

	switch (aValue.Type())
		{
	case KValueTypeBool:
		if (*(TBool*)valptr)
			{	
			aEnv.SetL(name, 1);
			}
		break;
	case KValueTypeInt:
		aEnv.SetL(name, *(TInt*)valptr);
		break;
	case KValueTypeUint:
		iTempStr.Num(*(TUint*)valptr, EDecimal);
		aEnv.SetL(name, iTempStr);
		break;
	case KValueTypeString:
		{
		HBufC* val = *(HBufC**)valptr;
		if (val) aEnv.SetL(name, *val);
		break;
		}
	case KValueTypeFileName:
		aEnv.SetL(name, *(TFileName2*)valptr);
		break;
	case KValueTypeEnum:
		{
		TEnum enumObj(aValue.EnumValueList());
		aEnv.SetL(name, enumObj.GetString(*(TInt*)valptr));
		break;
		}
	case KValueTypeReal:
		{
		_LIT(KRealFormat, "%g");
		iTempStr.Format(KRealFormat, *(TReal*)valptr);
		aEnv.SetL(name, iTempStr);
		break;
		}
	case KValueTypeUint64:
		iTempStr.Num(*(TUint64*)valptr, EDecimal);
		aEnv.SetL(name, iTempStr);
		break;
	case KValueTypeInt64:
		iTempStr.Num(*(TInt64*)valptr);
		aEnv.SetL(name, iTempStr);
		break;
	default:
		StaticLeaveIfErr(KErrArgument, _L("Script argument '%S' is of unsupported type %d"), &name, aValue.Type());
		break;
		}
	}

