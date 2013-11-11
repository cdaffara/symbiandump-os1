// command_wrappers.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __COMMAND_WRAPPERS_H__
#define __COMMAND_WRAPPERS_H__

#include <e32base.h>
#include <fshell/iocli.h>
#include <fshell/ioutils.h>
#include "command_constructors.h"

using namespace IoUtils;


class MCommandObserver;


class MCommand
	{
public:
	virtual RIoReadHandle& CmndStdin() = 0;
	virtual RIoWriteHandle& CmndStdout() = 0;
	virtual RIoWriteHandle& CmndStderr() = 0;
	virtual TInt CmndRun(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver, RIoSession& aIoSession) = 0;
	virtual void CmndForeground() = 0;
	virtual void CmndBackground() = 0;
	virtual void CmndKill() = 0;
	virtual TInt CmndSuspend() = 0;
	virtual TInt CmndResume() = 0;
	virtual void CmndRelease() = 0;
	virtual TExitType CmndExitType() const = 0;
	virtual TExitCategoryName CmndExitCategory() const = 0;
	virtual const TDesC& CmndName() const = 0;
	virtual TInt CmndReattachStdin(RIoEndPoint& aStdinEndPoint) = 0;
	virtual TInt CmndReattachStdout(RIoEndPoint& aStdoutEndPoint) = 0;
	virtual TInt CmndReattachStderr(RIoEndPoint& aStderrEndPoint) = 0;
	virtual TBool CmndIsDisownable() const = 0;
	virtual void CmndDisown() = 0;
	};


class MCommandObserver
	{
public:
	virtual void HandleCommandComplete(MCommand& aCommand, TInt aError) = 0;
	};

class CCommandWrapperBase : public CActive, public MCommand
	{
protected:
	CCommandWrapperBase();
	~CCommandWrapperBase();
	void BaseConstructL(const TDesC& aName);
protected: // From MCommand.
	virtual RIoReadHandle& CmndStdin();
	virtual RIoWriteHandle& CmndStdout();
	virtual RIoWriteHandle& CmndStderr();
	virtual const TDesC& CmndName() const;
	virtual TInt CmndReattachStdin(RIoEndPoint& aStdinEndPoint);
	virtual TInt CmndReattachStdout(RIoEndPoint& aStdoutEndPoint);
	virtual TInt CmndReattachStderr(RIoEndPoint& aStderrEndPoint);
	virtual TBool CmndIsDisownable() const;
	virtual void CmndDisown();
private: // From MCommand.
	virtual void CmndRelease();
protected: // From CActive
	void RunL();
	void DoCancel();

private:
	HBufC* iName;	///< This is used by concrete classes as they see fit.
	RIoReadHandle iStdin;
	RIoWriteHandle iStdout;
	RIoWriteHandle iStderr;
	};


class CThreadCommand : public CCommandWrapperBase
	{
public:
	enum TFlags
		{
		EUpdateEnvironment = 0x00000001,
		ESharedHeap = 0x00000002, // Any command that accesses gShell must have this set
		};
public:
	static CThreadCommand* NewL(const TDesC& aName, TCommandConstructor aCommandConstructor, TUint aFlags, MTaskRunner* aTaskRunner);
	~CThreadCommand();
private:
	CThreadCommand(TCommandConstructor aCommandConstructor, TUint aFlags, MTaskRunner* aTaskRunner);
	void ConstructL(const TDesC& aName);
	void RunL();
	void DoCancel();
	static void DoCommandThreadStartL(TAny* aSelf);
private: // From MCommand.
	virtual TInt CmndRun(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver, RIoSession& aIoSession);
	virtual void CmndForeground();
	virtual void CmndBackground();
	virtual void CmndKill();
	virtual TInt CmndSuspend();
	virtual TInt CmndResume();
	virtual TExitType CmndExitType() const;
	virtual TExitCategoryName CmndExitCategory() const;
private:
	TUint iFlags;
	TCommandConstructor iCommandConstructor;
	MCommandObserver* iObserver;
	RThread iThread;
	MTaskRunner* iTaskRunner;
	CEnvironment* iSuppliedEnv;
	HBufC* iCommandLine;
	};


class CProcessCommand : public CCommandWrapperBase
	{
public:
	static CProcessCommand* NewL(const TDesC& aExeName);
	static CProcessCommand* NewL(const TDesC& aExeName, RProcess& aProcess);
	~CProcessCommand();
protected:
	CProcessCommand();
	void ConstructL(const TDesC& aExeName, RProcess* aProcess = NULL);
	virtual void CreateProcessL(const TDesC& aCommandLine, CEnvironment& aEnv);
private:
	void CmndRunL(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver);
protected: // From MCommand.
	virtual TInt CmndRun(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver, RIoSession& aIoSession);
	virtual void CmndForeground();
	virtual void CmndBackground();
	virtual void CmndKill();
	virtual TInt CmndSuspend();
	virtual TInt CmndResume();
	virtual TExitType CmndExitType() const;
	virtual TExitCategoryName CmndExitCategory() const;
	virtual TBool CmndIsDisownable() const;
	virtual void CmndDisown();
public:
	class CProcessWatcher : public CActive
		{
	public:
		static CProcessWatcher* NewL();
		~CProcessWatcher();
		TInt Logon(CProcessCommand& aCommand, RProcess& aProcess, MCommandObserver& aObserver);
	private:
		CProcessWatcher();
	private: // From CActive.
		virtual void RunL();
		virtual void DoCancel();
	private:
		CProcessCommand* iCommand;
		RProcess* iProcess;
		MCommandObserver* iObserver;
		};
protected:
	MCommandObserver* iObserver;
	RProcess iProcess;
	CProcessWatcher* iWatcher;
	};


class CPipsCommand : public CProcessCommand
	{
public:
	static CPipsCommand* NewL(const TDesC& aExeName);
	CPipsCommand();
	~CPipsCommand();
private: // From MCommand.
	virtual TInt CmndRun(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver, RIoSession& aIoSession);
	virtual void CmndKill();
private: // From CProcessCommand.
	virtual void CreateProcessL(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv);
private:
	TBool iUsingPipsRun;
	TInt iPipsRunChildProcessId;
	};


class CAliasCommand : public CCommandWrapperBase, public MCommandObserver
	{
public:
	static CAliasCommand* NewL(MCommand& aAliasedCommand, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments);
	~CAliasCommand();
private:
	CAliasCommand(MCommand& aAliasedCommand);
	void ConstructL(const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments);
private: // From MCommand.
	RIoReadHandle& CmndStdin();
	RIoWriteHandle& CmndStdout();
	RIoWriteHandle& CmndStderr();
	TInt CmndRun(const TDesC& aCommandLine, IoUtils::CEnvironment& aEnv, MCommandObserver& aObserver, RIoSession& aIoSession);
	void CmndForeground();
	void CmndBackground();
	void CmndKill();
	TInt CmndSuspend();
	TInt CmndResume();
	TExitType CmndExitType() const;
	TExitCategoryName CmndExitCategory() const;
private: // From MCommandObserver.
	virtual void HandleCommandComplete(MCommand& aCommand, TInt aError);
private:
	MCommand& iAliasedCommand;
	HBufC* iAdditionalArguments;
	HBufC* iReplacementArguments;
	MCommandObserver* iCommandObserver;
	};


#endif // __COMMAND_WRAPPERS_H__
