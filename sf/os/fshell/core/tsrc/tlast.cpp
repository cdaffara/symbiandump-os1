// tlast.cpp
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

class CCmdTlast : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTlast();
private:
	CCmdTlast();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iArg;
	};

EXE_BOILER_PLATE(CCmdTlast)

CCommandBase* CCmdTlast::NewLC()
	{
	CCmdTlast* self = new(ELeave) CCmdTlast();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTlast::~CCmdTlast()
	{
	delete iArg;
	}

CCmdTlast::CCmdTlast()
	{
	}

const TDesC& CCmdTlast::Name() const
	{
	_LIT(KName, "tlast");	
	return KName;
	}

const TDesC& CCmdTlast::Description() const
	{
	_LIT(KDescription, "Test of fshell's parsing of commands with the 'last' attribute");
	return KDescription;
	}

void CCmdTlast::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iArg, _L("arg"), _L("test arg"), KValueTypeFlagLast);
	}

void CCmdTlast::DoRunL()
	{
	Printf(_L("arg = %S\r\n"), iArg);
	}
