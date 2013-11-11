// fshell.h
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


#ifndef __SHELL_H__
#define __SHELL_H__

#include <e32base.h>
#include <fshell/iocli.h>
#include <fshell/ioutils.h>
#include "job.h"
#include "error.h"
#include "console.h"
#include <fshell/line_editor.h>
#include <fshell/iocons_writer.h>


_LIT(KScriptPath, "SCRIPT_PATH");
_LIT(KScriptName, "SCRIPT_NAME");
_LIT(KScriptLine, "SCRIPT_LINE");
_LIT(KScriptArgCount, "ARG_COUNT");
_LIT(KKeepGoing, "KEEP_GOING");

class CLineCompleter;
class CCommandFactory;


class CShell : public IoUtils::CCommandBase, public MConsoleObserver, public MLineEditorObserver, public MJobObserver, public MParserObserver
	{
public:
	static CShell* NewLC();
	~CShell();
	CCommandFactory& CommandFactory();
	void ClaimJobsLockLC();
	TInt BringJobToForeground(TInt aJobId);
	void SendJobToBackground(TInt aJobId);
	const RPointerArray<CJob>& Jobs() const;
	CJob* Job(TInt aId);
	TInt DisownJob(TInt aId);
	static HBufC* ReadScriptL(const TDesC& aScriptName, const TDesC* aArguments, IoUtils::CEnvironment& aEnv, RFs& aFs, TIoHandleSet& aIoHandles, TBool& aHelpPrinted, RPointerArray<HBufC>* aAdditionalPrefixArguments=NULL);
private:
	CShell();
	void ConstructL();
	void PrintPrompt();
	void PrintErr(TInt aError);
	void ProcessLineL(const TDesC& aLine);
	TInt NextJobId();
	CJob& ForegroundJob();
	void SetToForeground();
	static void ReleaseJobsLock(TAny* aSelf);
private: // From IoUtils::CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void StdinChange(TUint aChange);
private: // From MConsoleObserver.
	virtual void CoHandleKey(TUint aKeyCode, TUint aModifiers);
	virtual void CoHandleError(TInt aError);
private: // From MLineEditorObserver.
	virtual void LeoHandleLine(const TDesC& aLine);
private: // From MJobObserver.
	virtual void HandleJobComplete(CJob& aJob, const TError& aError);
private: // From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
private:
	RFs iFs;
	CConsoleReader* iConsole;
	CLineEditor* iLineEditor;
	CLineCompleter* iLineCompleter;
	CCommandFactory* iCommandFactory;
	RPointerArray<CJob> iJobs;
	HBufC* iScriptData;
	CParser* iParser;
	TInt iForegroundJobId;
	TInt iNextJobId;
	TIoConsWriterAdaptor iWriterAdaptor;
	TFileName2 iScriptName;
	HBufC* iScriptArgs;
	HBufC* iOneLiner;
	TBool iKeepGoing;
	TBool iIgnoreNextStdinChange;
	RMutex iJobsLock;
	};

extern CShell* gShell;
extern TInt gExitValue;
void Log(TRefByValue<const TDesC8> aFmt, ...);


#endif // __SHELL_H__
