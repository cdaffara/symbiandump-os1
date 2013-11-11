// startup.cpp
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
#include <fshell/common.mmh>
#include <dscstore.h>
#include <dscitem.h>

using namespace IoUtils;

class CCmdStartup : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdStartup();
private:
	CCmdStartup();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TOp
		{
		EList,
		EAdd,
		EDelete,
		};
	TOp iOperation;
	TUid iDscUid;
	RDscStore iDsc;

	TUid iItemUid;
	HBufC* iItemFileName;
	HBufC* iItemArgs;
	TBool iCreate;
	};

EXE_BOILER_PLATE(CCmdStartup)

CCommandBase* CCmdStartup::NewLC()
	{
	CCmdStartup* self = new(ELeave) CCmdStartup();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdStartup::~CCmdStartup()
	{
	iDsc.Close();
	delete iItemFileName;
	delete iItemArgs;
	}

CCmdStartup::CCmdStartup()
	: iDscUid(TUid::Uid(KDefaultSymbianDsc))
	{
	}

const TDesC& CCmdStartup::Name() const
	{
	_LIT(KName, "startup");	
	return KName;
	}

void CCmdStartup::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iOperation, _L("command"));
	aArguments.AppendStringL(iItemFileName, _L("filename"));
	aArguments.AppendStringL(iItemArgs, _L("arguments"));
	}

void CCmdStartup::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendUintL((TUint&)iDscUid.iUid, _L("dsc-uid"));
	aOptions.AppendBoolL(iCreate, _L("create"));
	}

void EnumClose(TAny* aPtr)
	{
	static_cast<RDscStore*>(aPtr)->EnumClose();
	}

void CCmdStartup::DoRunL()
	{
	TRAPL(iDsc.OpenL(), _L("Couldn't open RDscStore"));

	if (iCreate && !iDsc.DscExistsL(iDscUid))
		{
		TRAPL(iDsc.CreateDscL(iDscUid, _L("[DSC created by fshell_startup.exe]")), _L("Couldn't create DSC 0x%08x"), iDscUid.iUid);
		}

	if (iOperation == EList)
		{
		TRAPL(iDsc.EnumOpenLC(iDscUid); CleanupStack::Pop(), _L("Couldn't list DSC 0x%x"), iDscUid.iUid);
		CleanupStack::PushL(TCleanupItem(&EnumClose, &iDsc)); // Can't use the EnumOpenLC's cleanupitem because we have to pop it, because we need it inside its own TRAP harness to get good error messages out
		for (;;)
			{
			CDscItem* item = iDsc.EnumReadNextL();
			if (!item) break;
			TPtrC args = item->Args();
			TPtrC fn = item->FileName();
			Printf(_L("DscId=0x%08x ItemId=0x%08x FileName=%S Args=%S\r\n"), item->DscId(), item->ItemId(), &fn, &args);
			delete item;
			}
		CleanupStack::PopAndDestroy(); // close enum
		}
	else if (iOperation == EAdd)
		{
		if (iItemFileName == NULL) LeaveIfErr(KErrArgument, _L("Must specify a file name for the 'add' operation"));
		CDscItem* item = CDscItem::NewLC(*iItemFileName, iItemArgs ? *iItemArgs : KNullDesC());
		item->SetStartMethodL(EFireAndForget);
		item->SetStartupTypeL(EStartProcess);
		item->SetDscId(iDscUid);
		TRAPL(iDsc.AddItemL(*item), _L("Couldn't add DSC item (try using --create option?)"));
		CleanupStack::PopAndDestroy(item);
		}
	else if (iOperation == EDelete)
		{
		if (iItemFileName == NULL) LeaveIfErr(KErrArgument, _L("Must specify a file name for the 'delete' operation"));
		CDscItem* item = CDscItem::NewLC(*iItemFileName, iItemArgs ? *iItemArgs : KNullDesC());
		item->SetDscId(iDscUid);
		TRAPL(iDsc.DeleteItemL(*item), _L("Couldn't delete DSC item"));
		CleanupStack::PopAndDestroy(item);
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("Unrecognised operation %d"), iOperation);
		}
	}
