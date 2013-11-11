// rez.cpp
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
#include <barsc.h>
#include <bautils.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;

class CCmdRez : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdRez();
private:
	CCmdRez();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iIdentifier;
	TBool iHex;
	RResourceFile iFile;
	};


CCommandBase* CCmdRez::NewLC()
	{
	CCmdRez* self = new(ELeave) CCmdRez();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRez::~CCmdRez()
	{
	delete iIdentifier;
	iFile.Close();
	}

CCmdRez::CCmdRez()
	{
	}

const TDesC& CCmdRez::Name() const
	{
	_LIT(KName, "rez");
	return KName;
	}

void CCmdRez::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iIdentifier, _L("resource-identifier"));
	}

void CCmdRez::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iHex, _L("hex"));
	}


EXE_BOILER_PLATE(CCmdRez)

void CCmdRez::DoRunL()
	{
	if (iIdentifier->Left(2) != _L("R:"))
		{
		LeaveIfErr(KErrArgument, _L("Resource identifiers must start R: (see help for details)"));
		}

	TBool wide = ETrue;
	if ((*iIdentifier)[iIdentifier->Length()-1] == 'd') wide = EFalse;

	HBufC* errString = NULL;
	HBufC8* result = NULL;
	TRAPD(err, result = LtkUtils::Rez8L(*iIdentifier, NULL, &errString));
	if (err && errString)
		{
		PrintError(err, _L("%S"), errString);
		}
	delete errString;
	User::LeaveIfError(err);
	CleanupStack::PushL(result);

	if (iHex)
		{
		LtkUtils::HexDumpToOutput(*result, Stdout());
		}
	else if (wide)
		{
		TPtrC16 wptr((TUint16*)result->Ptr(), result->Size()/2);
		Write(wptr);
		}
	else
		{
		RBuf buf;
		buf.CreateL(result->Length());
		buf.Copy(*result);
		Write(buf);
		buf.Close();
		}
	CleanupStack::PopAndDestroy(result);
	if (Stdout().AttachedToConsole())
		{
		Printf(_L("\r\n"));
		}
	}	
