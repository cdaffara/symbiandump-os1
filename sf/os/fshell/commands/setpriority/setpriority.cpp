// setpriority.cpp
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

class CCmdSetpriority;
class CNewThreadNotifier : public CActive
	{
public:
	CNewThreadNotifier(CCmdSetpriority& aCmd, RMemoryAccess& aMemAccess);
	void Start();
	~CNewThreadNotifier();
	void DoCancel();
	void RunL();

private:
	CCmdSetpriority& iCmd;
	RMemoryAccess& iMemAccess;
	};

class CCmdSetpriority : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSetpriority();
	void NewThread(TInt aHandle);
private:
	CCmdSetpriority();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RArray<TUint> iTids;
	RArray<TUint> iPids;
	TInt iPriority;
	HBufC* iName;
	TProcessKernelInfo iProcInfo;
	TThreadKernelInfo iThreadInfo;
	CNewThreadNotifier* iNotifier;
	};


CCommandBase* CCmdSetpriority::NewLC()
	{
	CCmdSetpriority* self = new(ELeave) CCmdSetpriority();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSetpriority::~CCmdSetpriority()
	{
	delete iNotifier;
	delete iName;
	iTids.Close();
	iPids.Close();
	}

CCmdSetpriority::CCmdSetpriority()
	: CMemoryAccessCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdSetpriority::Name() const
	{
	_LIT(KName, "setpriority");
	return KName;
	}

void CCmdSetpriority::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendIntL(iPriority, _L("priority"));
	}

void CCmdSetpriority::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendStringL(iName, _L("match"));
	aOptions.AppendUintL(iPids, _L("pid"));
	aOptions.AppendUintL(iTids, _L("tid"));
	}


EXE_BOILER_PLATE(CCmdSetpriority)

void CCmdSetpriority::DoRunL()
	{
	LoadMemoryAccessL();
	TInt err = KErrNone;

	// Fix up priorities that we had to make different to kernel cos fshell can't handle negative arguments
	// See enum TThrdPriority in kern_priv.h for the meaning of these negative numbers
	switch(iPriority)
		{
		case 101: iPriority = -8; break;
		case 102: iPriority = -7; break;
		case 103: iPriority = -6; break;
		case 104: iPriority = -5; break;
		case 105: iPriority = -4; break;
		case 106: iPriority = -3; break;
		case 107: iPriority = -2; break;
		default:
			break;
		}

	if (iName)
		{
		TPtrC8 name8 = iName->Des().Collapse();
		LeaveIfErr(iMemAccess.SetPriorityOverride(iPriority, name8), _L("Couldn't set priority override"));
		iNotifier = new(ELeave) CNewThreadNotifier(*this, iMemAccess);
		iNotifier->Start();
		return;
		}
	
	if (iTids.Count() && iPids.Count())
		{
		LeaveIfErr(KErrArgument, _L("You cannot specify both --tid and --pid - a single priority cannot be valid for thread and process."));
		}

	for (TInt i = 0; i < iTids.Count(); i++)
		{
		TUint id = iTids[i];
		RThread thread;
		err = iMemAccess.RThreadForceOpen(thread, id);
		if (!err) err = iMemAccess.SetThreadPriority(thread, iPriority);
		if (err) PrintError(err, _L("Couldn't set priority for thread %u"), id);
		thread.Close();
		}
	
	for (TInt i = 0; i < iPids.Count(); i++)
		{
		TUint id = iPids[i];
		// Get KProcessFlagPriorityControl flag
		RProcess process;
		err = process.Open(id);
		LeaveIfErr(err, _L("Couldn't open process with ID %u"), id);

		TBool priorityControlAllowed = EFalse;
		TPckg<TProcessKernelInfo> pkg(iProcInfo);
		err = iMemAccess.GetObjectInfoByHandle(EProcess, RThread().Id(), process.Handle(), pkg);
		if (err)
			{
			PrintWarning(_L("Couldn't get priority control flag setting (%d)\r\n"), err);
			}
		else
			{
			if (iProcInfo.iFlags & KProcessFlagPriorityControl) priorityControlAllowed = ETrue;
			}

		if (!priorityControlAllowed)
			{
			PrintError(KErrAccessDenied, _L("This process does not allow setting of its priority"));
			}
		else if (iPriority != EPriorityBackground && iPriority != EPriorityForeground)
			{
			PrintError(KErrAccessDenied, _L("The kernel will ignore requests to set a process priority that isn't Background (250) or Foreground (350). I can't see how to get round this even using memory access. Sorry."));
			}
		process.SetPriority((TProcessPriority)iPriority);
		process.Close();
		}
	Complete(KErrNone);
	}

void CCmdSetpriority::NewThread(TInt aHandle)
	{
	if (aHandle == KErrNotSupported)
		{
		PrintError(KErrNotSupported, _L("Kernel was not compiled with __DEBUGGER_SUPPORT__, can't get thread creation notifications."));
		Complete(aHandle);
		}
	else if (aHandle < 0)
		{
		PrintError(aHandle, _L("Failed to get new thread notification, or couldn't open handle to thread."));
		Complete(aHandle);
		}
	else
		{
		RThread thread;
		thread.SetHandle(aHandle);
		TFullName name(thread.FullName());
		TInt priority = 0;
		TPckg<TThreadKernelInfo> pkg(iThreadInfo);
		TInt err = iMemAccess.GetObjectInfoByHandle(EThread, RThread().Id(), aHandle, pkg);
		if (!err)
			{
			priority = iThreadInfo.iThreadPriority;
			}

		TUint tid = thread.Id();
		Printf(_L("New thread id %u name %S priority adjusted to %d\r\n"), tid, &name, priority);
		thread.Close();
		}
	}

CNewThreadNotifier::CNewThreadNotifier(CCmdSetpriority& aCmd, RMemoryAccess& aMemAccess)
	: CActive(CActive::EPriorityStandard), iCmd(aCmd), iMemAccess(aMemAccess)
	{
	CActiveScheduler::Add(this);
	}

CNewThreadNotifier::~CNewThreadNotifier()
	{
	Cancel();
	}

void CNewThreadNotifier::DoCancel()
	{
	iMemAccess.CancelNotifyThreadCreation();
	}

void CNewThreadNotifier::RunL()
	{
	TInt stat = iStatus.Int();
	if (stat >= 0)
		{
		Start();
		}
	iCmd.NewThread(stat);
	}

void CNewThreadNotifier::Start()
	{
	if (!IsActive())
		{
		iMemAccess.NotifyThreadCreation(iStatus);
		SetActive();
		}
	}
