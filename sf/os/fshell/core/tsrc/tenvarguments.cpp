// tenvarguments.cpp
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
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdtenvarguments : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdtenvarguments();
private:
	CCmdtenvarguments();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iStringArg;
	TInt iIntArg;
	TUint iUintArg;
	TBool iBoolOpt;
	TInt iIntOpt;
	TUint iUintOpt;
	HBufC* iStringOpt;
	};

EXE_BOILER_PLATE(CCmdtenvarguments)

CCommandBase* CCmdtenvarguments::NewLC()
	{
	CCmdtenvarguments* self = new(ELeave) CCmdtenvarguments();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdtenvarguments::~CCmdtenvarguments()
	{
	delete iStringArg;
	delete iStringOpt;
	}

CCmdtenvarguments::CCmdtenvarguments()
	{
	}

const TDesC& CCmdtenvarguments::Name() const
	{
#ifdef USE_CIF
	_LIT(KName, "tenvarguments");
#else
	_LIT(KName, "tnoncifenvarguments");
#endif
	return KName;
	}

const TDesC& CCmdtenvarguments::Description() const
	{
#ifdef USE_CIF
	return CCommandBase::Description();
#else
	_LIT(KDescription, "Test for command-line arguments that get filled in from the environment.");
	return KDescription;
#endif
	}

void CCmdtenvarguments::ArgumentsL(RCommandArgumentList& aArguments)
	{
#ifdef USE_CIF
	aArguments.AppendStringL(iStringArg, _L("stringarg"));
	aArguments.AppendIntL(iIntArg, _L("intarg"));
	aArguments.AppendUintL(iUintArg, _L("uintarg"));
#else
	aArguments.AppendStringL(iStringArg, _L("stringarg"), _L("String argument."), 0, _L("STRINGARG"));
	aArguments.AppendIntL(iIntArg, _L("intarg"), _L("Int argument."), 0, _L("INTARG"));
	aArguments.AppendUintL(iUintArg, _L("uintarg"), _L("Uint argument."), 0, _L("UINTARG"));
#endif
	}

void CCmdtenvarguments::OptionsL(RCommandOptionList& aOptions)
	{
#ifdef USE_CIF
	aOptions.AppendBoolL(iBoolOpt, _L("boolopt"));
	aOptions.AppendIntL(iIntOpt, _L("intopt"));
	aOptions.AppendUintL(iUintOpt, _L("uintopt"));
	aOptions.AppendStringL(iStringOpt, _L("stringopt"));
#else
	aOptions.AppendBoolL(iBoolOpt, 'b', _L("boolopt"), _L("Bool option."), 0, _L("BOOLOPT"));
	aOptions.AppendIntL(iIntOpt, 'i', _L("intopt"), _L("Int option."), 0, _L("INTOPT"));
	aOptions.AppendUintL(iUintOpt, 'u', _L("uintopt"), _L("Uint option."), 0, _L("UINTOPT"));
	aOptions.AppendStringL(iStringOpt, 's', _L("stringopt"), _L("String option."), 0, _L("STRINGOPT"));
#endif
	}

#define CHECKINT(arg, val) if (arg != val) LeaveIfErr(KErrArgument, _L("%s is %d, expected %d"), L ## #arg , arg, (TInt)val)
#define CHECKSTR(arg, val) if (!arg) LeaveIfErr(KErrArgument, _L("%s is null, should be %S"), L ## #arg, &val); else if (*arg != val) LeaveIfErr(KErrArgument, _L("%s is %S, expected %S"), L ## #arg , arg, &val)

void CCmdtenvarguments::DoRunL()
	{
	_LIT(KStringArg, "StringArg");
	_LIT(KStringOpt, "StringOpt");

	CHECKINT(iIntArg, 1234);
	CHECKINT(iUintArg, 0x1234);
	CHECKSTR(iStringArg, KStringArg);
	CHECKINT(iBoolOpt, 1);
	CHECKINT(iIntOpt, 4321);
	CHECKINT(iUintOpt, 0x4321);
	CHECKSTR(iStringOpt, KStringOpt);
	}
