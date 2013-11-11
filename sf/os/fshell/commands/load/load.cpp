// load.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>

using namespace IoUtils;


class CCmdLoad : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdLoad();
private:
	CCmdLoad();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	HBufC* iDllName;
	RArray<TUint> iUids;
	TUint iModuleVersion;
	RLibrary iLibrary;
	};


//
// CCmdLoad.
//

CCommandBase* CCmdLoad::NewLC()
	{
	CCmdLoad* self = new(ELeave) CCmdLoad();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdLoad::~CCmdLoad()
	{
	iLibrary.Close();
	delete iDllName;
	iUids.Close();
	}

CCmdLoad::CCmdLoad() : CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdLoad::Name() const
	{
	_LIT(KName, "load");
	return KName;
	}

void CCmdLoad::DoRunL()
	{
	if (iUids.Count() > 3)
		{
		LeaveIfErr(KErrArgument, _L("Too many UIDs specified"));
		}

	while (iUids.Count() < 3)
		{
		iUids.AppendL(KNullUid.iUid);
		}
	TUidType type(TUid::Uid(iUids[0]), TUid::Uid(iUids[1]), TUid::Uid(iUids[2]));

	_LIT(KError, "Unable to load \"%S\"");
	if (iOptions.IsPresent(&iModuleVersion))
		{
		LeaveIfErr(iLibrary.Load(*iDllName, KNullDesC, type, iModuleVersion), KError, iDllName);
		}
	else
		{
		LeaveIfErr(iLibrary.Load(*iDllName, type), KError, iDllName);
		}

	TFileName file = iLibrary.FileName();
	TUidType uids = iLibrary.Type();
	Printf(_L("Loaded %S\r\n"), &file);
	Printf(_L("Uids: 0x%x, 0x%x, 0x%x\r\n"), uids[0].iUid, uids[1].iUid, uids[2].iUid);
	}

void CCmdLoad::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptUid, "uid");
	aOptions.AppendUintL(iUids, KCmdOptUid);

	_LIT(KCmdOptVersion, "version");
	aOptions.AppendUintL(iModuleVersion, KCmdOptVersion);
	}

void CCmdLoad::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KCmdArgFileName, "dll_name");
	aArguments.AppendStringL(iDllName, KCmdArgFileName);
	}


EXE_BOILER_PLATE(CCmdLoad)

