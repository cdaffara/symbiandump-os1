// tstringhash.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
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

using namespace IoUtils;
using LtkUtils::RStringHash;
using LtkUtils::TStringHashIter;

class CCmdTstringhash : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTstringhash();
private:
	CCmdTstringhash();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	};


CCommandBase* CCmdTstringhash::NewLC()
	{
	CCmdTstringhash* self = new(ELeave) CCmdTstringhash();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTstringhash::~CCmdTstringhash()
	{
	}

CCmdTstringhash::CCmdTstringhash()
	{
	}

const TDesC& CCmdTstringhash::Name() const
	{
	_LIT(KName, "tstringhash");	
	return KName;
	}

const TDesC& CCmdTstringhash::Description() const
	{
	// ToDo: Write description text.
	_LIT(KDescription, "To do.");
	return KDescription;
	}

void CCmdTstringhash::ArgumentsL(RCommandArgumentList&)
	{
	}

void CCmdTstringhash::OptionsL(RCommandOptionList&)
	{
	}


EXE_BOILER_PLATE(CCmdTstringhash)

void CCmdTstringhash::DoRunL()
	{
	RStringHash<TInt> hash;
	CleanupClosePushL(hash);

	TInt err = hash.Insert(_L("test"), 1);

	LeaveIfErr(err, _L("Failed to insert item"));

	TInt* res = hash.Find(_L("test"));

	if (!res) LeaveIfErr(KErrGeneral, _L("Failed to find item"));
	if (*res != 1) LeaveIfErr(KErrGeneral, _L("item returned %d instead of 1"), *res);

	TInt n = 100;
	while (n--)
		{
		hash.InsertL(_L("test"), n);
		}

	hash.ReserveL(100);

	hash.InsertL(_L("a"), 6);
	hash.InsertL(_L("b"), 6);
	hash.InsertL(_L("aasdfasdf"), 66666666);
	hash.InsertL(_L("lalala"), 66666);
	hash.InsertL(_L("qqq"), 666);
	hash.InsertL(_L(""), 0);
	hash.InsertL(_L("12345"), 54321);

	TStringHashIter<TInt> iter(hash);
	while (iter.NextValue() != NULL)
		{
		const TDesC* k = iter.CurrentKey();
		const TInt* v = iter.CurrentValue();
		Printf(_L("Key %S has value %d\r\n"), k, *v);
		}

	CleanupStack::PopAndDestroy(&hash);
	}


