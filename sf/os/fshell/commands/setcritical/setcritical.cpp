// setcritical.cpp
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

#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

class CCmdSetcritical : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSetcritical();
private:
	CCmdSetcritical();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RArray<TUint> iTids;
	TBool iSystem, iPermanent, iProcess, iCritical;
	};


CCommandBase* CCmdSetcritical::NewLC()
	{
	CCmdSetcritical* self = new(ELeave) CCmdSetcritical();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSetcritical::~CCmdSetcritical()
	{
	iTids.Close();
	}

CCmdSetcritical::CCmdSetcritical()
	{
	}

const TDesC& CCmdSetcritical::Name() const
	{
	_LIT(KName, "setcritical");	
	return KName;
	}

void CCmdSetcritical::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iTids, _L("threadid"));
	}

void CCmdSetcritical::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iSystem, _L("system"));
	aOptions.AppendBoolL(iProcess, _L("process"));
	aOptions.AppendBoolL(iCritical, _L("critical"));
	aOptions.AppendBoolL(iPermanent, _L("permanent"));
	}

void CCmdSetcritical::DoRunL()
	{
	LoadMemoryAccessL();

	TUint set = 0;
	if (iSystem && iCritical)
		set = KThreadFlagSystemCritical;
	else if (iSystem && iPermanent)
		set = KThreadFlagSystemPermanent;
	else if (iProcess && iCritical)
		set = KThreadFlagProcessCritical;
	else if (iProcess && iPermanent)
		set = KThreadFlagProcessPermanent;
	else if (!iSystem && !iCritical && !iProcess && !iPermanent)
		set = 0;
	else
		{
		LeaveIfErr(KErrArgument, _L("Incompatible options specified. You must either give no options (to specify ENotCritical) or one of --system/--process and one of --critical/--permanent"));
		}

	for (TInt i = 0; i < iTids.Count(); i++)
		{
		TUint id = iTids[i];
		RThread thread;
		TInt err = iMemAccess.RThreadForceOpen(thread, id);
		if (!err) err = iMemAccess.SetThreadCriticalFlags(thread, set);
		if (err)
			{
			PrintError(err, _L("Couldn't set flags for thread %u."), id);
			}
		thread.Close();
		}
	}

EXE_BOILER_PLATE(CCmdSetcritical)
