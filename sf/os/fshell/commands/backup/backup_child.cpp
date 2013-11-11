// backup_child.cpp
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

#include <e32property.h>
#include <connect/sbdefs.h>
#include <baBackup.h>
#include "backup.h"

_LIT(KBackupOp,		"-b");
_LIT(KRestoreOp,	"-r");

const TInt KIdleTime = 5000000; // microseconds

using namespace conn;

class CBackupAO : public CActive
	{
public:
	static CBackupAO* NewL();
	~CBackupAO();
	void Start();
private:
	CBackupAO();
	void ConstructL();
	void CheckForRestoreFlagL();

	// from CActive
	void DoCancel();
	void RunL();
private:
	CBaBackupSessionWrapper* iBackupSession;
	RSemaphore iSem;
	TBool iRestoreOp;
	};


CBackupAO* CBackupAO::NewL()
	{
	CBackupAO* self = new (ELeave) CBackupAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBackupAO::CBackupAO():
CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

CBackupAO::~CBackupAO()
	{
	Cancel();
	delete iBackupSession; // will also issue the ETakeLock to system apps
	if (iSem.Handle() > 0)
		iSem.Close();
	}

void CBackupAO::ConstructL()
	{
	iBackupSession = CBaBackupSessionWrapper::NewL();
	User::LeaveIfError(iSem.CreateGlobal(KBackupSemaphore, 0, EOwnerProcess));
	CheckForRestoreFlagL();
	}

void CBackupAO::Start()
	{
	iBackupSession->CloseAll(MBackupObserver::EReleaseLockNoAccess, iStatus);
	SetActive();
	}

void CBackupAO::RunL()
	{
	TInt error = iStatus.Int();

	// check which instruction we must send
	conn::TBURPartType instruction = conn::EBURBackupFull;
	if (iRestoreOp)
		instruction = conn::EBURRestoreFull;
	
	// inform subscribed parties of our intentions
	RProperty bk;
	if (error == KErrNone)
		error = bk.Set(KUidSystemCategory, KUidBackupRestoreKey, instruction);
	
	// block for a little while to give apps a chance to process the backup instruction
	User::After(KIdleTime);
	
	// inform fshell's backup cmd we've put the system into backup-mode (or not!)
	RProcess().Rendezvous(error);
	if (error == KErrNone)
		{
		iSem.Wait(); // wait for signal from fshell's backup cmd that we're ok to terminate
		bk.Set(KUidSystemCategory, KUidBackupRestoreKey, conn::EBURNormal); // ignore the error in this case. what else can I do?
		}

	// the process has done it's thing
	CActiveScheduler::Stop();
	}

void CBackupAO::DoCancel()
	{
	ASSERT(iBackupSession);
	iBackupSession->RestartAll();
	}

//
// CBackupAO::CheckForRestoreFlagL
// determine whether we're issuing a backup or a restore
//
void CBackupAO::CheckForRestoreFlagL()
	{
	HBufC *buffer = HBufC::NewL(User::CommandLineLength());
	CleanupStack::PushL(buffer);
	TPtr cmdline = buffer->Des();
	User::CommandLine(cmdline);
	const TInt bPos = cmdline.Find(KBackupOp);
	const TInt rPos = cmdline.Find(KRestoreOp);
	CleanupStack::PopAndDestroy(1);
	if (bPos != KErrNotFound)
		{
		if (rPos != KErrNotFound)
			User::Leave(KErrArgument);
		iRestoreOp = EFalse;
		return;
		}
	if (rPos != KErrNotFound)
		{
		iRestoreOp = ETrue;
		return;
		}
	User::Leave(KErrArgument);
	}


static void RunL()
	{
	// Active Scheduler
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	CBackupAO* woop = CBackupAO::NewL();
	CleanupStack::PushL(woop);
	woop->Start();

	// start the active scheduler
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, s);
	}

// program entry-point
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
		TRAP(err, RunL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return err;
	}
