// testexecute.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/memoryaccesscmd.h>
#include <fshell/descriptorutils.h>
#include "proxyserver.h"
#include <test/testexecutelogger.h>

using namespace IoUtils;

class CCmdTestExecute : public CMemoryAccessCommandBase, public MMessageHandler
	{
public:
	static CCommandBase* NewLC();
	~CCmdTestExecute();
private:
	CCmdTestExecute();

private: // From MMessageHandler
	virtual TBool HandleMessageL(CProxySession* aSession, const RMessage2& aMessage);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);

private: // From CActive
	void RunL();
	void DoCancel();

	class CLogonCompleter : public CActive
		{
	public:
		CLogonCompleter(CCmdTestExecute* aCommand) : CActive(CActive::EPriorityStandard), iCommand(aCommand)
			{
			CActiveScheduler::Add(this);
			iCommand->iTefProc.Logon(iStatus);
			SetActive();
			}
		void RunL() { iCommand->Complete(iStatus.Int()); }
		void DoCancel() { iCommand->iTefProc.LogonCancel(iStatus); }
		~CLogonCompleter() { Cancel(); }

	private:
		CCmdTestExecute* iCommand;
		};

private:
	CProxyServer* iProxy;
	RTestExecuteLogServ iLogger;
	RBuf8 iTempBuf;

	TFileName2 iScriptFile;
	TBool iIgnoredT, iIgnoredC;
	HBufC* iIncludes;
	HBufC* iExcludes;
	TBool iDebug;
	CLogonCompleter* iLogonCompleter;
	RProcess iTefProc;
	};


EXE_BOILER_PLATE(CCmdTestExecute)

CCommandBase* CCmdTestExecute::NewLC()
	{
	CCmdTestExecute* self = new(ELeave) CCmdTestExecute();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTestExecute::~CCmdTestExecute()
	{
	Cancel();
	if (iProxy)
		{
		iProxy->Destroy();
		}
	iLogger.Close();
	iTempBuf.Close();
	delete iIncludes;
	delete iExcludes;
	delete iLogonCompleter;
	iTefProc.Close();
	}

CCmdTestExecute::CCmdTestExecute()
	: CMemoryAccessCommandBase(EManualComplete | ESharableIoSession)
	{
	iTefProc.SetHandle(0); // Don't default to the current process
	}

const TDesC& CCmdTestExecute::Name() const
	{
	_LIT(KName, "testexecute");	
	return KName;
	}

void CCmdTestExecute::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendFileNameL(iScriptFile, _L("script-file"));
	}

void CCmdTestExecute::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iDebug, _L("debug"));
	aOptions.AppendBoolL(iIgnoredT, _L("ignore"));
	aOptions.AppendBoolL(iIgnoredC, _L("ignore2"));
	aOptions.AppendStringL(iIncludes, _L("include"));
	aOptions.AppendStringL(iExcludes, _L("exclude"));
	}

#define CTRL(x) ((x)-'a'+1)

void CCmdTestExecute::DoRunL()
	{
	LoadMemoryAccessL();
	iTempBuf.CreateL(1024);
	User::LeaveIfError(Stdin().CaptureKey(CTRL('c'), 0, 0)); // So we can do cleanup
	Stdin().WaitForKey(iStatus);
	SetActive();


	User::LeaveIfError(iLogger.Connect()); // Keep it open
	iLogger.CreateLog(_L("\\logs\\testexecute\\fshelllovesyou"), RTestExecuteLogServ::ELogModeOverWrite); // The server won't actually stay open unless you call CreateLog!

	// Set up our proxy
	iProxy = CProxyServer::NewInSeparateThreadL(_L("TestExecuteLogEngine"), this);

	if (iScriptFile.Length())
		{
		LtkUtils::RLtkBuf cmdLine;
		CleanupClosePushL(cmdLine);
		cmdLine.AppendL(iScriptFile);
		if (iDebug) cmdLine.AppendL(_L(" -d"));
		if (iIncludes) cmdLine.AppendFormatL(_L(" -tci %S"), iIncludes);
		if (iExcludes) cmdLine.AppendFormatL(_L(" -tcx %S"), iExcludes);

		LeaveIfErr(iTefProc.Create(_L("testexecute.exe"), cmdLine), _L("Couldn't create testexecute.exe %S"), &cmdLine);
		iLogonCompleter = new(ELeave) CLogonCompleter(this);
		iTefProc.Resume();
		CleanupStack::PopAndDestroy(&cmdLine);
		}
	else
		{
		// Just do some tests
		RTestExecuteLogServ newlogger;
		LeaveIfErr(newlogger.Connect(), _L("Couldn't connect to proxied RTestExecuteLogServ"));

		newlogger.CreateLog(_L("\\logs\\testexecute\\scriptyscripty"), RTestExecuteLogServ::ELogModeOverWrite);
		newlogger.Write(_L("I AM A LOG"));
		newlogger.Close();
		}
	}

TBool CCmdTestExecute::HandleMessageL(CProxySession* /*aSession*/, const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case RTestExecuteLogServ::EWriteLog:
		{
		aMessage.ReadL(0, iTempBuf);
		Write(iTempBuf.Expand());
		break;
		}
	default:
		break;
		}

	return EFalse; // We allow everything to go through to the real TEF as well
	}

void CCmdTestExecute::RunL()
	{
	if (Stdin().KeyCode() == CTRL('c'))
		{
		Printf(_L("CTRL-C detected, cleaning up proxy server...\r\n"));
		iProxy->Destroy();
		iProxy = NULL;
		Printf(_L("Exiting...\r\n"));
		SetErrorReported(ETrue); // Supress complaints about the cancel
		Complete(KErrCancel);
		}
	else
		{
		Stdin().WaitForKey(iStatus);
		SetActive();
		}
	}

void CCmdTestExecute::DoCancel()
	{
	Stdin().WaitForKeyCancel();
	}
