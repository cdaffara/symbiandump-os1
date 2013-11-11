// iniedit.cpp
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
#include <settings.h>

using namespace IoUtils;
using namespace LtkUtils;


class CCmdIniEdit : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdIniEdit();
private:
	CCmdIniEdit();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TFileName2 iIniFile;
	TFileName2 iDescriptionFile;
	TBool iDump;
	TBool iForce;
	HBufC* iSet;
	HBufC* iGet;
	HBufC* iRemove;
	
	RBuf iBuf;
	CIniReader* iReader;
	};


CCommandBase* CCmdIniEdit::NewLC()
	{
	CCmdIniEdit* self = new(ELeave) CCmdIniEdit();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdIniEdit::~CCmdIniEdit()
	{
	delete iSet;
	delete iGet;
	delete iRemove;
	delete iReader;
	iBuf.Close();
	}

CCmdIniEdit::CCmdIniEdit()
	{
	}

const TDesC& CCmdIniEdit::Name() const
	{
	_LIT(KName, "iniedit");	
	return KName;
	}
	
_LIT(KEquals, "=");

TPtrC Trim(const TDesC& aDes)
	{
	TPtrC des(aDes);
	while (des.Length() && TChar(des[0]).IsSpace())
		{
		des.Set(des.Mid(1));
		}
	while (des.Length() && TChar(des[des.Length()-1]).IsSpace())
		{
		des.Set(des.Left(des.Length()-1));
		}
	return des;
	}

void CCmdIniEdit::DoRunL()
	{
	TBool interactive = ETrue;
	if (iForce)
		{
		iReader = CIniReader::NewL(iIniFile);
		}
	else
		{
		iReader = CIniFile::NewL(iIniFile, iDescriptionFile);
		}
	TBool updated = EFalse;
		
	if (iSet)
		{
		TInt eqPos = iSet->Find(KEquals);
		TPtrC value(KNullDesC);
		TPtrC id(KNullDesC);
		if (eqPos == KErrNotFound)
			{
			id.Set(*iSet);
			
			Stdin().SetReadModeL(RIoReadHandle::ELine);
			iBuf.CreateL(0x100);
			Stdin().ReadL(iBuf);
			value.Set(iBuf);
			}
		else
			{
			id.Set(iSet->Left(eqPos));
			value.Set(iSet->Mid(eqPos+1));
			}
		
		iReader->SetValueL(Trim(id), value);
		updated = ETrue;
		interactive = EFalse;
		}
	if (iRemove)
		{
		iReader->RemoveValueL(*iRemove);
		updated = ETrue;
		interactive = EFalse;
		}
	
	if (iGet)
		{
		const TDesC* value = iReader->GetValue(*iGet);
		if (!value) LeaveIfErr(KErrNotFound, _L("id '%S' not found in %S"), iGet, &iIniFile);
		Write(*value);
		interactive = EFalse;
		}
	else if (iDump)
		{
		RPointerArray<CValue> values;
		CleanupClosePushL(values);
		iReader->GetValuesL(values);
		for (TInt i=0; i<values.Count(); ++i)
			{
			if (iForce)
				{
				Printf(_L("%S: %S\n"), &values[i]->Id(), &values[i]->Value());
				}
			else
				{
				CSetting* setting = (CSetting*)values[i];
				Printf(_L("%S: %S\n"), &setting->Name(), &setting->Value());
				}
			}
		CleanupStack::PopAndDestroy(&values);
		interactive = EFalse;
		}
	
	if (interactive)
		{
		// TODO enter interactive mode
		LeaveIfErr(KErrNotSupported, _L("Interactive mode not implemented yet."));
		}

	if (updated)
		{
		WriteIniFileL(iIniFile, *iReader);
		}
	}

void CCmdIniEdit::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KIniFile, "inifile");
	aArguments.AppendFileNameL(iIniFile, KIniFile);
	}

void CCmdIniEdit::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptIdfFile, "idf");
	aOptions.AppendFileNameL(iDescriptionFile, KOptIdfFile);
	
	_LIT(KOptDump, "dump");
	aOptions.AppendBoolL(iDump, KOptDump);
	
	_LIT(KOptForce, "force");
	aOptions.AppendBoolL(iForce, KOptForce);
	
	_LIT(KOptGet, "get");
	aOptions.AppendStringL(iGet, KOptGet);

	_LIT(KOptSet, "set");
	aOptions.AppendStringL(iSet, KOptSet);
	
	_LIT(KOptRemove, "remove");
	aOptions.AppendStringL(iRemove, KOptRemove);
	}


EXE_BOILER_PLATE(CCmdIniEdit)

