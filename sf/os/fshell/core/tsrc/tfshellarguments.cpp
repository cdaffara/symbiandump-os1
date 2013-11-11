// tfshellarguments.cpp
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

class CCmdTfshellArguments : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTfshellArguments();
private:
	CCmdTfshellArguments();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TUint64 iBigInt;
	TReal iReal;
	};

EXE_BOILER_PLATE(CCmdTfshellArguments)

CCommandBase* CCmdTfshellArguments::NewLC()
	{
	CCmdTfshellArguments* self = new(ELeave) CCmdTfshellArguments();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTfshellArguments::~CCmdTfshellArguments()
	{
	}

CCmdTfshellArguments::CCmdTfshellArguments()
	{
	}

const TDesC& CCmdTfshellArguments::Name() const
	{
	_LIT(KName, "tfshellarguments");	
	return KName;
	}

const TDesC& CCmdTfshellArguments::Description() const
	{
	_LIT(KDescription, "Test different fshell argument types");
	return KDescription;
	}

void CCmdTfshellArguments::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iBigInt, _L("bigint"), _L("64-bit int type. Must be set to 2^40 (ie 1099511627776 or 0x10000000000)."));
	aArguments.AppendRealL(iReal, _L("real"), _L("double-precision floating point type. Must be set to 3.1415927 or equivalent"));
	}

void CCmdTfshellArguments::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	}

void CCmdTfshellArguments::DoRunL()
	{
	const TUint64 KInt = 1099511627776ULL;
	const TReal KReal = 3.1415927;

	if (iBigInt != KInt)
		{
		LeaveIfErr(KErrArgument, _L("bigint %Ld doesn't equal %Ld"), iBigInt, KInt);
		}
	if (iReal < 3.14159265 || iReal > 3.14159275)
		{
		// Specify a range because it's floating point
		LeaveIfErr(KErrArgument, _L("real %f doesn't equal %f"), iReal, KReal);
		}
	}
