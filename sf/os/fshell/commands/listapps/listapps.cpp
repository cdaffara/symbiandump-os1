// listapps.cpp
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
#include <apgwgnam.h>
#include "listapps.h"


CCommandBase* CCmdListApps::NewLC()
	{
	CCmdListApps* self = new(ELeave) CCmdListApps();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdListApps::~CCmdListApps()
	{
	delete iFormatter;
	}

CCmdListApps::CCmdListApps()
	{
	}

const TDesC& CCmdListApps::Name() const
	{
	_LIT(KName, "listapps");
	return KName;
	}

void CCmdListApps::DoRunL()
	{
	iFormatter = CTextFormatter::NewL(Stdout());
	RWsSession wsSession;
	LeaveIfErr(wsSession.Connect(), _L("Couldn't connect to windowserver"));
	CleanupClosePushL(wsSession);
	if (iArguments.IsPresent(0))
		{
		PrintDetailsL(wsSession, iWgId);
		}
	else
		{
		CArrayFixFlat<TInt>* wgIds = new(ELeave) CArrayFixFlat<TInt>(16);
		CleanupStack::PushL(wgIds);
		TInt err = KErrNone;
		if (iShowAll)
			{
			err = wsSession.WindowGroupList(wgIds);
			}
		else
			{
			err = wsSession.WindowGroupList(0, wgIds);
			}
		LeaveIfErr(err, _L("Couldn't get window group list"));
		TInt numWgs = wgIds->Count();
		for (TInt i = 0; i < numWgs; ++i)
			{
			PrintDetailsL(wsSession, (*wgIds)[i]);
			}
		CleanupStack::PopAndDestroy(wgIds);
		}
	CleanupStack::PopAndDestroy(&wsSession);
	}

void AppendBoolL(CTextFormatter& aFormatter, const TDesC& aCaption, TBool aBool)
	{
	_LIT(KYes, "yes");
	_LIT(KNo, "no");
	aFormatter.AppendFormatL(_L("\t%S: %S\r\n"), &aCaption, aBool ? &KYes : &KNo);
	}
	
void CCmdListApps::PrintDetailsL(RWsSession& aWsSession, TInt aWindowGroupId)
	{
	CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(aWsSession, aWindowGroupId);
	_LIT(KUnknown, "not named");
	TPtrC caption(wgn->Caption());
	if (caption.Length() == 0)
		{
		caption.Set(KUnknown);
		}
	if (iVerbose)
		{
		iFormatter->AppendFormatL(_L("%S\r\n"), &caption);
		TPtrC docName(wgn->DocName());
		iFormatter->AppendFormatL(_L("\tDocument name: %S\r\n"), &docName);
		iFormatter->AppendFormatL(_L("\tWindow group id: %u\r\n"), aWindowGroupId);
		TThreadId threadId;
		TInt err = aWsSession.GetWindowGroupClientThreadId(aWindowGroupId, threadId);
		if (err)
			{
			PrintWarning(_L("Couldn't read window group client thread id: %d"), err);
			}
		else
			{
			TFullName threadName(_L("unknown"));
			RThread thread;
			if (thread.Open(threadId) == KErrNone)
				{
				thread.FullName(threadName);
				thread.Close();
				}
#ifdef EKA2
			iFormatter->AppendFormatL(_L("\tThread id: %Lu '%S'\r\n"), threadId.Id(), &threadName);
#else
			iFormatter->AppendFormatL(_L("\tThread id: %u '%S'\r\n"), threadId, &threadName);
#endif
			}
		iFormatter->AppendFormatL(_L("\tUID: 0x%08x\r\n"), wgn->AppUid().iUid);
		AppendBoolL(*iFormatter, _L("Busy"), wgn->IsBusy());
		AppendBoolL(*iFormatter, _L("System"), wgn->IsSystem());
		AppendBoolL(*iFormatter, _L("Doc name is a file"), wgn->DocNameIsAFile());
		AppendBoolL(*iFormatter, _L("Responds to shutdown event"), wgn->RespondsToShutdownEvent());
		AppendBoolL(*iFormatter, _L("Responds to switch files event"), wgn->RespondsToSwitchFilesEvent());
		AppendBoolL(*iFormatter, _L("Hidden"), wgn->Hidden());
#ifdef EKA2
		AppendBoolL(*iFormatter, _L("Ready"), wgn->IsAppReady());
#endif
		iFormatter->AppendFormatL(_L("\tOrdinal priority: %d\r\n"), aWsSession.GetWindowGroupOrdinalPriority(aWindowGroupId));
		if (aWindowGroupId == aWsSession.GetFocusWindowGroup())
			{
			iFormatter->AppendL(_L("\tFocussed\r\n"));
			}
		Write(iFormatter->Descriptor());
		iFormatter->Zero();
		}
	else
		{
		Printf(_L("%S (%d)"), &caption, aWindowGroupId);
		if (aWindowGroupId == aWsSession.GetFocusWindowGroup())
			{
			Printf(_L(" (focussed)"));
			}
		Printf(_L("\r\n"));
		}
	CleanupStack::PopAndDestroy(wgn);
	}

void CCmdListApps::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KCmdOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KCmdOptVerbose);

	_LIT(KCmdOptShowAll, "show-all");
	aOptions.AppendBoolL(iShowAll, KCmdOptShowAll);
	}

void CCmdListApps::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgWgId, "id");
	aArguments.AppendUintL(iWgId, KArgWgId);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdListApps)
#endif

