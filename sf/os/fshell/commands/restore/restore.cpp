// restore.cpp
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

#include "restore.h"

_LIT(KRestoreOp,	"-r");

//
// CRestoreDriver
//
CRestoreDriver* CRestoreDriver::NewL(MBackupParent& aParent)
	{
	CRestoreDriver* self = new (ELeave) CRestoreDriver(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRestoreDriver::CRestoreDriver(MBackupParent& aParent):
CActive(CActive::EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

CRestoreDriver::~CRestoreDriver()
	{
	Cancel();
	}

void CRestoreDriver::ConstructL()
	{
	}

void CRestoreDriver::SendInstructionL(MBackupObserver::TFileLockFlags aInstruction)
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
// CRestoreDriver::ChildProcessExists
// checks whether the backup_child process
// is already running
//
TBool CRestoreDriver::ChildProcessExists()
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
// CRestoreDriver::LaunchProcessL
// spawns the backup_child process
// & waits to rendezvous
//
void CRestoreDriver::LaunchProcessL()
	{
	if (ChildProcessExists())
		User::Leave(KErrInUse);
	
	_LIT(KExeExt, ".exe");
	TName name(KBackupProcessName);
	name.Append(KExeExt);
	User::LeaveIfError(iChild.Create(name, KRestoreOp, EOwnerProcess));
	iChild.Resume();
	iChild.Rendezvous(iStatus);
	SetActive();
	}

//
// CRestoreDriver::TerminateProcessL
// open the backup_child process & signal it's semaphore
// that semaphore is the signal to shut itself down cleanly
//
void CRestoreDriver::TerminateProcessL()
	{
	if (!ChildProcessExists())
		User::Leave(KErrNotFound);
	
	// signal the semaphore 
	RSemaphore sem;
	User::LeaveIfError(sem.OpenGlobal(KBackupSemaphore, EOwnerProcess));
	sem.Signal();

	SelfComplete(KErrNone);
	}

void CRestoreDriver::SelfComplete(const TInt aError)
	{
	ASSERT(!IsActive());
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aError);
	SetActive();
	}

//
// CRestoreDriver::RunL
// 
void CRestoreDriver::RunL()
	{
	iParent.Finished(iStatus.Int());
	}

//
// CRestoreDriver::DoCancel
//
void CRestoreDriver::DoCancel()
	{
	ASSERT(EFalse); // not dealing with this!
	}


//
// CCmdRestore
//
CCommandBase* CCmdRestore::NewLC()
	{
	CCmdRestore* self = new (ELeave) CCmdRestore();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdRestore::~CCmdRestore()
	{
	delete iRestore;
	}

CCmdRestore::CCmdRestore() : 
CCommandBase(CCommandBase::EManualComplete)
	{
	}

const TDesC& CCmdRestore::Name() const
	{
	_LIT(KName, "restore");	
	return KName;
	}

void CCmdRestore::DoRunL()
	{
	// spawn the stuff I need to do the task
	iRestore = CRestoreDriver::NewL(*this);

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
	iRestore->SendInstructionL(flag);
	}

void CCmdRestore::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);
	}

void CCmdRestore::OptionsL(RCommandOptionList&)
	{
	}

void CCmdRestore::Finished(const TInt aError)
	{
	Complete(aError);
	}

EXE_BOILER_PLATE(CCmdRestore)
