// shutapp.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <w32std.h>
#include <apgtask.h>
#include <apgwgnam.h>
#include "shutapp.h"


CCommandBase* CCmdShutApp::NewLC()
	{
	CCmdShutApp* self = new(ELeave) CCmdShutApp();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdShutApp::~CCmdShutApp()
	{
	iWgIds.Close();
	iWgNames.ResetAndDestroy();
	}

CCmdShutApp::CCmdShutApp()
	: iTimeout(5)
	{
	}

const TDesC& CCmdShutApp::Name() const
	{
	_LIT(KName, "shutapp");
	return KName;
	}

void CCmdShutApp::DoRunL()
	{
	RWsSession wsSession;
	User::LeaveIfError(wsSession.Connect());
	CleanupClosePushL(wsSession);

	const TInt numIds = iWgIds.Count();
	for (TInt i = 0; i < numIds; ++i)
		{
		ShutL(wsSession, iWgIds[i]);
		}

	const TInt numNames = iWgNames.Count();
	for (TInt i = 0; i < numNames; ++i)
		{
		const TDesC& name = *(iWgNames[i]);
		TInt id = 0;
		while ((id = wsSession.FindWindowGroupIdentifier(id, name)) != KErrNotFound)
			{
			ShutL(wsSession, id);
			}
		}

	if (iAll)
		{
		TInt numWgs = wsSession.NumWindowGroups(0);
		if (numWgs > 0)
			{
			CArrayFixFlat<TInt>* wgIds = new(ELeave) CArrayFixFlat<TInt>(numWgs);
			CleanupStack::PushL(wgIds);
			User::LeaveIfError(wsSession.WindowGroupList(0, wgIds));
			numWgs = wgIds->Count();
			for (TInt i = 0; i < numWgs; ++i)
				{
				ShutL(wsSession, (*wgIds)[i]);
				}
			CleanupStack::PopAndDestroy(wgIds);
			}
		}
	CleanupStack::PopAndDestroy(&wsSession);
	}
	
void CCmdShutApp::ShutL(RWsSession& aWsSession, TInt aWindowGroupId)
	{
	CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(aWsSession, aWindowGroupId);
	if (iVerbose)
		{
		_LIT(KUnknown, "not named");
		TPtrC caption(wgn->Caption());
		if (caption.Length() == 0)
			{
			caption.Set(KUnknown);
			}
		Printf(_L("Shutting down %d (%S)\r\n"), aWindowGroupId, &caption);
		}

	TThreadId threadId;
	User::LeaveIfError(aWsSession.GetWindowGroupClientThreadId(aWindowGroupId, threadId));

	RThread thread;
	User::LeaveIfError(thread.Open(threadId));
	CleanupClosePushL(thread);

	TRequestStatus threadStatus;
	thread.Logon(threadStatus);
	if (threadStatus != KRequestPending)
		{
		User::WaitForRequest(threadStatus);
		User::Leave(KErrGeneral);
		}

	RTimer timer;
	User::LeaveIfError(timer.CreateLocal());
	CleanupClosePushL(timer);
	TRequestStatus timeoutStatus;
	timer.After(timeoutStatus, iTimeout * 1000000);

	TApaTask task(aWsSession);
	task.SetWgId(aWindowGroupId);
#ifdef EKA2
	if (iForce)
		{
		task.SendSystemEvent(EApaSystemEventSecureShutdown);
		}
	else
		{
		task.SendSystemEvent(EApaSystemEventShutdown);
		}
#else
	task.SendSystemEvent(EApaSystemEventShutdown);
#endif

	User::WaitForRequest(threadStatus, timeoutStatus);

	if (timeoutStatus != KRequestPending)
		{
		// Timeout occurred.
		thread.LogonCancel(threadStatus);
		User::WaitForRequest(threadStatus);
		User::Leave(KErrTimedOut);
		}
	else
		{
		timer.Cancel();
		User::WaitForRequest(timeoutStatus);
		}

	CleanupStack::PopAndDestroy(3, wgn);
	}

void CCmdShutApp::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptWgId, "id");
	aOptions.AppendUintL(iWgIds, KOptWgId);

	_LIT(KOptWgPattern, "match");
	aOptions.AppendStringL(iWgNames, KOptWgPattern);

	_LIT(KOptTimeout, "timeout");
	aOptions.AppendIntL(iTimeout, KOptTimeout);

	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptAll, "all");
	aOptions.AppendBoolL(iAll, KOptAll);

#ifdef EKA2
	_LIT(KOptForce, "force");
	aOptions.AppendBoolL(iForce, KOptForce);
#endif
	}

void CCmdShutApp::ArgumentsL(RCommandArgumentList&)
	{
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdShutApp)
#endif

