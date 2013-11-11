// tsettings.cpp
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
#include <fshell/settings.h>

using namespace IoUtils;
using namespace LtkUtils;

class CCmdTSettings : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTSettings();
private:
	CCmdTSettings();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum 
		{
		ERead,
		EValidate,
		ERewrite,
		} iTest;
	TFileName2 iFilename;
	
	CIniReader* iReader;
	CIniFile* iValidator;
	};

_LIT(KTests, "read,validate,rewrite");


CCommandBase* CCmdTSettings::NewLC()
	{
	CCmdTSettings* self = new(ELeave) CCmdTSettings();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTSettings::~CCmdTSettings()
	{
	delete iReader;
	delete iValidator;
	}

CCmdTSettings::CCmdTSettings()
	{
	}

const TDesC& CCmdTSettings::Name() const
	{
	_LIT(KName, "tsettings");	
	return KName;
	}

const TDesC& CCmdTSettings::Description() const
	{
	_LIT(KDescription, "Test command for LTK utils settings classes.");
	return KDescription;
	}

void CCmdTSettings::DoRunL()
	{
	switch (iTest)
		{
	case ERead:
		{
		iReader = CIniReader::NewL(iFilename);
		RPointerArray<CValue> values;
		CleanupClosePushL(values);
		iReader->GetValuesL(values);
		for (TInt i=0; i<values.Count(); ++i)
			{
			Printf(_L("%S=%S\n"), &values[i]->Id(), &values[i]->Value());
			}		
		CleanupStack::PopAndDestroy(&values);
		break;
		}
	case EValidate:
		{
		iValidator = CIniFile::NewL(iFilename);
		RPointerArray<CValue> values;
		CleanupClosePushL(values);
		iValidator->GetValuesL(values);
		for (TInt i=0; i<values.Count(); ++i)
			{
			Printf(_L("%S=%S\n"), &values[i]->Id(), &values[i]->Value());
			}		
		CleanupStack::PopAndDestroy(&values);
		break;
		}
	case ERewrite:
		{
		iReader = CIniReader::NewL(iFilename);
		WriteIniFileL(iFilename, *iReader);
		break;
		}
		};
	}

void CCmdTSettings::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KTest, "test");
	_LIT(KTestDescription, "Test to perform");
	aArguments.AppendEnumL((TInt&)iTest, KTest, KTestDescription, KTests);
	
	_LIT(KIniFile, "inifile");
	_LIT(KIniFileDescription, "The .ini file to test with");
	aArguments.AppendFileNameL(iFilename, KIniFile, KIniFileDescription);
	}

void CCmdTSettings::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	}


EXE_BOILER_PLATE(CCmdTSettings)

