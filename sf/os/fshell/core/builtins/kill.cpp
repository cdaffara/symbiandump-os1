// kill.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "kill.h"
#include <fshell/common.mmh>

CCommandBase* CCmdKill::NewLC()
	{
	CCmdKill* self = new(ELeave) CCmdKill();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdKill::~CCmdKill()
	{
	delete iPanicCategory;
	delete iMatch;
	}

CCmdKill::CCmdKill()
	{
	}

const TDesC& CCmdKill::Name() const
	{
	_LIT(KName, "kill");
	return KName;
	}

void CCmdKill::DoRunL()
	{
	if (iTerminate && iPanicCategory)
		{
		PrintError(KErrArgument, _L("Options --terminate and --panic are mutually exclusive"));
		User::Leave(KErrArgument);
		}

	if (iMatch)
		{
		FindIdAndKillL(*iMatch);
		}
	else if (iArguments.IsPresent(&iId))
		{
		KillIdL(iId);
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("You must specify either an id or a match string"));
		}
	}

template <class T>
void CCmdKill::DoKillByHandleL(TUint aId)
	{
	T handle;
	LeaveIfErr(handle.Open(aId, EOwnerThread), _L("Unable to open handle id %u"), aId);
	TName name(handle.Name());
	
	if (handle.ExitType() != EExitPending)
		{
		Printf(_L("%S (id=%u) has already exited"), &name, aId);
		handle.Close();
		return;
		}

	if (iTerminate)
		{
		handle.Terminate(iReason);
		Printf(_L("Terminated %S (id=%u) with %d\r\n"), &name, aId, iReason);
		}
	else if (iPanicCategory)
		{
		handle.Panic(*iPanicCategory, iReason);
		Printf(_L("Panicked %S (id=%u) with %S %d\r\n"), &name, aId, iPanicCategory, iReason);
		}
	else
		{
		handle.Kill(iReason);
		Printf(_L("Killed %S (id=%u) with %d\r\n"), &name, aId, iReason);
		}

	handle.Close();
	}

void CCmdKill::FindIdAndKillL(const TDesC& aPattern)
	{
	if (iThread) DoFindIdAndKillL<RThread, TFindThread>(aPattern);
	else DoFindIdAndKillL<RProcess, TFindProcess>(aPattern);
	}

template <class RProcessOrThread, class TFindProcessOrThread>
void CCmdKill::DoFindIdAndKillL(const TDesC& aPattern)
	{
	TFullName fullName;
	TInt numFound = 0;
	TFindProcessOrThread finder(aPattern);
	while (finder.Next(fullName) == KErrNone)
		{
		RProcessOrThread handle;
		TInt err = handle.Open(finder, EOwnerThread);
		if ((err == KErrNone) && (handle.ExitType() == EExitPending))
			{
			++numFound;
			TUint id = handle.Id().Id();
			handle.Close();
			if (iAll || numFound == 1)
				{
				KillIdL(id);
				}
			}
		else if (err)
			{
			PrintWarning(_L("Couldn't open %S (err=%d)"), &fullName);
			}
		}
	if (!iAll)
		{
		if (numFound == 0)
			{
			LeaveIfErr(KErrNotFound, _L("No matches for pattern \"%S\", or all matches are zombies"), &aPattern);
			}
		else if (numFound > 1)
			{
			PrintWarning(_L("%d further matches for pattern \"%S\" found, be more specific or use --all option"), numFound-1, &aPattern);
			}
		}
	}

void CCmdKill::DoKillThreadL(TUint aId)
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();

	RThread thread;
	LeaveIfErr(iMemAccess.RThreadForceOpen(thread, aId), _L("Unable to open thread"));
	TName name(thread.FullName());
	if (thread.ExitType() != EExitPending)
		{
		Printf(_L("%S (id=%u) has already exited"), &name, aId);
		thread.Close();
		return;
		}

	CleanupClosePushL(thread);
	TExitType type = EExitKill;
	if (iTerminate)
		{
		type = EExitTerminate;
		}
	else if (iPanicCategory)
		{
		type = EExitPanic;
		}
	TInt err = iMemAccess.ObjectDie(EThread, aId, NULL, type, iReason, iPanicCategory ? *iPanicCategory : KNullDesC());
	LeaveIfErr(err, _L("Couldn't kill thread id %u"), aId);

	if (iTerminate)
		{
		Printf(_L("Terminated %S (id=%u) with %d\r\n"), &name, aId, iReason);
		}
	else if (iPanicCategory)
		{
		Printf(_L("Panicked %S (id=%u) with %S %d\r\n"), &name, aId, iPanicCategory, iReason);
		}
	else
		{
		Printf(_L("Killed %S (id=%u) with %d\r\n"), &name, aId, iReason);
		}
	CleanupStack::PopAndDestroy(&thread);

#else
	DoKillByHandleL<RThread>(aId);
#endif
	}

void CCmdKill::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptReason, "reason");
	_LIT(KOptThread, "thread");
	_LIT(KOptTerminate, "terminate");
	_LIT(KOptPanic, "panic");
	_LIT(KOptMatch, "match");
	_LIT(KOptAll, "all");
	aOptions.AppendIntL(iReason, KOptReason);
	aOptions.AppendBoolL(iTerminate, KOptTerminate);
	aOptions.AppendStringL(iPanicCategory, KOptPanic);
	aOptions.AppendBoolL(iThread, KOptThread);
	aOptions.AppendStringL(iMatch, KOptMatch);
	aOptions.AppendBoolL(iAll, KOptAll);
	}

void CCmdKill::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgId, "id");
	aArguments.AppendUintL(iId, KArgId);
	}

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdKill)
#endif

void CCmdKill::KillIdL(TUint aId)
	{
	if (iThread)
		{
		DoKillThreadL(aId);
		}
	else
		{
		DoKillByHandleL<RProcess>(aId);
		}
	}
