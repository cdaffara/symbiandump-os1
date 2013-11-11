// backup.cpp
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

#include "backup.h"

_LIT(KBackupOp,		"-b");

//
// CBackupDriver
//
CBackupDriver* CBackupDriver::NewL(MBackupParent& aParent)
	{
	CBackupDriver* self = new (ELeave) CBackupDriver(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBackupDriver::CBackupDriver(MBackupParent& aParent):
CActive(CActive::EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CBackupDriver::~CBackupDriver()
	{
	Cancel();
	}

void CBackupDriver::ConstructL()
	{
	}

void CBackupDriver::SendInstructionL(MBackupObserver::TFileLockFlags aInstruction)
	{
	iInstruction = aInstruction;
	switch (iInstruction)
		{
		case MBackupObserver::EReleaseLockNoAccess:
			LaunchProcessL();
		break;

		case MBackupObserver::ETakeLock:
			TerminateProcessL();
		break;

		default:
			User::Leave(KErrNotSupported);
		break;		
		};
	}

//
// CBackupDriver::ChildProcessExists
// checks whether the backup_child process
// is already running
//
TBool CBackupDriver::ChildProcessExists()
	{
	_LIT(KWildCard, "*");
	TName woop(KBackupProcessName);
	woop.Append(KWildCard);
	TFindProcess check(woop);
	TFullName name;
	TInt result = check.Next(name);
	if (result == KErrNone)
		return ETrue;	
	return EFalse;
	}

//
// CBackupDriver::LaunchProcessL
// spawns the backup_child process
// & waits to rendezvous
//
void CBackupDriver::LaunchProcessL()
	{
	if (ChildProcessExists())
		User::Leave(KErrInUse);
	
	_LIT(KExeExt, ".exe");
	TName name(KBackupProcessName);
	name.Append(KExeExt);
	User::LeaveIfError(iChild.Create(name, KBackupOp, EOwnerProcess));
	iChild.Resume();
	iChild.Rendezvous(iStatus);
	SetActive();
	}

//
// CBackupDriver::TerminateProcessL
// open the backup_child process & signal it's semaphore
// that semaphore is the signal to shut itself down cleanly
//
void CBackupDriver::TerminateProcessL()
	{
	if (!ChildProcessExists())
		User::Leave(KErrNotFound);
	
	// signal the semaphore 
	RSemaphore sem;
	User::LeaveIfError(sem.OpenGlobal(KBackupSemaphore, EOwnerProcess));
	sem.Signal();

	SelfComplete(KErrNone);
	}

void CBackupDriver::SelfComplete(const TInt aError)
	{
	ASSERT(!IsActive());
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aError);
	SetActive();
	}

//
// CBackupDriver::RunL
// 
void CBackupDriver::RunL()
	{
	iParent.Finished(iStatus.Int());
	}

//
// CBackupDriver::DoCancel
//
void CBackupDriver::DoCancel()
	{
	ASSERT(EFalse); // not dealing with this!
	}


//
// CCmdBackup
//
CCommandBase* CCmdBackup::NewLC()
	{
	CCmdBackup* self = new (ELeave) CCmdBackup();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdBackup::~CCmdBackup()
	{
	delete iBackup;
	}

CCmdBackup::CCmdBackup() : 
CCommandBase(CCommandBase::EManualComplete)
	{
	}

const TDesC& CCmdBackup::Name() const
	{
	_LIT(KName, "backup");	
	return KName;
	}

void CCmdBackup::DoRunL()
	{
	// spawn the stuff I need to do the task
	iBackup = CBackupDriver::NewL(*this);

	// process the argument
	MBackupObserver::TFileLockFlags flag = MBackupObserver::EReleaseLockNoAccess;
	switch (iOperation)
		{
	case EStart:
		flag = MBackupObserver::EReleaseLockNoAccess;
		break;
	case EStop:
		flag = MBackupObserver::ETakeLock;
		break;
	default:
		User::LeaveIfError(KErrArgument);
		}
			
	// send the instruction through
	iBackup->SendInstructionL(flag);
	}

void CCmdBackup::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);
	}

void CCmdBackup::Finished(const TInt aError)
	{
	Complete(aError);
	}

EXE_BOILER_PLATE(CCmdBackup)
