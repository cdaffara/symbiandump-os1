// tinteger.cpp
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

class CCmdTinteger : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTinteger();
private:
	CCmdTinteger();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TBool iDashOne;
	TInt iArg;
	TInt iDashThree;
	};

EXE_BOILER_PLATE(CCmdTinteger)

CCommandBase* CCmdTinteger::NewLC()
	{
	CCmdTinteger* self = new(ELeave) CCmdTinteger();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTinteger::~CCmdTinteger()
	{
	}

CCmdTinteger::CCmdTinteger()
	{
	}

const TDesC& CCmdTinteger::Name() const
	{
	_LIT(KName, "tinteger");	
	return KName;
	}

const TDesC& CCmdTinteger::Description() const
	{
	_LIT(KDescription, "Test of fshell's parsing of commands with negative integer arguments.");
	return KDescription;
	}

void CCmdTinteger::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendIntL(iArg, _L("arg"), _L("test arg - this must be set to -2"));
	}

void CCmdTinteger::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iDashOne, '1', _L("ignore"), _L("test option - should be specified with -1"));
	aOptions.AppendIntL(iDashThree, '3', _L("ignore2"), _L("test option - should be specified with -3 -4"));
	}

void CCmdTinteger::DoRunL()
	{
	if (!iDashOne) LeaveIfErr(KErrArgument, _L("-1 didn't get picked up"));
	if (iArg != -2) LeaveIfErr(KErrArgument, _L("arg isn't -2"));
	if (iDashThree != -4) LeaveIfErr(KErrArgument, _L("the dash3 option ('-3') isn't -4"));
	}
