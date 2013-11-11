// backup.h
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
#include <babackup.h>

using namespace IoUtils;

_LIT(KArgStart, 			"start");
_LIT(KArgStop,	 			"stop");
_LIT(KBackupProcessName,	"backup_child");
_LIT(KBackupSemaphore,		"AutometricBackup");

class MBackupParent
	{
public:
	virtual void Finished(const TInt aError) = 0;	
	};

class CBackupDriver : public CActive
	{
public:
	static CBackupDriver* NewL(MBackupParent& aParent);
	~CBackupDriver();
	void SendInstructionL(MBackupObserver::TFileLockFlags aInstruction);
private:
	CBackupDriver(MBackupParent& aParent);
	void ConstructL();
	void SelfComplete(const TInt aError);
	void LaunchProcessL();
	void TerminateProcessL();
	TBool ChildProcessExists();

	// from CActive
	void DoCancel();
	void RunL();
private:
	MBackupParent& iParent;
	RProcess iChild;
	MBackupObserver::TFileLockFlags iInstruction;
	};

class CCmdBackup : public CCommandBase, public MBackupParent
	{
public:
	static CCommandBase* NewLC();
	~CCmdBackup();
private:
	CCmdBackup();

	// From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);

	// From MBackupParent
	virtual void Finished(const TInt aError);
private:
	CBackupDriver* iBackup;
	enum
		{
		EStart,
		EStop
		} iOperation;
	};
