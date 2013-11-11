// focus.cpp
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

#include <w32std.h>
#include <apgwgnam.h>
#include <fshell/ioutils.h>

const TInt KPriorityNeverAtFront = -999;

using namespace IoUtils;

class CCmdFocus : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFocus();
private:
	CCmdFocus();
	void PrintDetailsL();
	void FocusWindowGroupL(TInt aWindowGroupId);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RWsSession iWsSession;
	RWindowGroup iWindowGroup;
	TInt iWindowGroupId;
	HBufC* iWindowGroupPattern;
	TBool iFollow;
	TBool iVerbose;
	};


CCommandBase* CCmdFocus::NewLC()
	{
	CCmdFocus* self = new(ELeave) CCmdFocus();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFocus::~CCmdFocus()
	{
	iWindowGroup.Close();
	iWsSession.Close();
	delete iWindowGroupPattern;
	}

CCmdFocus::CCmdFocus()
	{
	}

const TDesC& CCmdFocus::Name() const
	{
	_LIT(KName, "focus");	
	return KName;
	}

void CCmdFocus::DoRunL()
	{
	LeaveIfErr(iWsSession.Connect(), _L("Couldn't connect to windowserver"));

	if (iOptions.IsPresent(&iWindowGroupId))
		{
		FocusWindowGroupL(iWindowGroupId);
		}
	else if (iWindowGroupPattern)
		{
		TInt id = 0;
		id = iWsSession.FindWindowGroupIdentifier(id, *iWindowGroupPattern);
		if (id >= 0)
			{
			FocusWindowGroupL(id);
			}
		}
	else
		{
		PrintDetailsL();
		if (iFollow)
			{
			iWindowGroup = RWindowGroup(iWsSession);
			User::LeaveIfError(iWindowGroup.Construct((TUint32)this,EFalse));
			iWindowGroup.SetOrdinalPosition(0, KPriorityNeverAtFront);
			iWindowGroup.EnableFocusChangeEvents();

			TRequestStatus status;
			FOREVER
				{
				iWsSession.EventReady(&status);
				User::WaitForRequest(status);
				User::LeaveIfError(status.Int());
				TWsEvent event;
				iWsSession.GetEvent(event);
				if (event.Type() == EEventFocusGroupChanged) 
					{
					PrintDetailsL();
					}
				}
			}
		}
	}

void CCmdFocus::PrintDetailsL()
	{
	TInt focusedWgId = iWsSession.GetFocusWindowGroup();
	CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(iWsSession, focusedWgId);
	_LIT(KUnknown, "Un-named");
	TPtrC caption(wgn->Caption());
	if (caption.Length() == 0)
		{
		caption.Set(KUnknown);
		}
	Printf(_L("Focused window group: %S\r\n"), &caption);
	CleanupStack::PopAndDestroy(wgn);

	if (iVerbose)
		{
		TThreadId focusedThreadId;
		User::LeaveIfError(iWsSession.GetWindowGroupClientThreadId(focusedWgId, focusedThreadId));
		RThread thread;
		User::LeaveIfError(thread.Open(focusedThreadId));
		TFullName name(thread.FullName());
		thread.Close();
		Printf(_L("  Window group id:    %u\r\n"), focusedWgId);
		Printf(_L("  Owning thread:      %S\r\n"), &name);
		Printf(_L("  Owning thread id:   %u\r\n"), (TUint32)focusedThreadId);
		Printf(_L("  Tick count:         %u\r\n"), User::NTickCount());
		}
	}

void CCmdFocus::FocusWindowGroupL(TInt aWindowGroupId)
	{
	LeaveIfErr(iWsSession.SetWindowGroupOrdinalPosition(aWindowGroupId, 0), _L("Unable to set window group %d to ordinal position zero"), aWindowGroupId);
	}

void CCmdFocus::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptWindowGroupId, "id");
	aOptions.AppendIntL(iWindowGroupId, KOptWindowGroupId);
	_LIT(KOptWindowGroupPattern, "match");
	aOptions.AppendStringL(iWindowGroupPattern, KOptWindowGroupPattern);
	_LIT(KOptFollow, "follow");
	aOptions.AppendBoolL(iFollow, KOptFollow);
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}


EXE_BOILER_PLATE(CCmdFocus)

