// fdb.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/memoryaccesscmd.h>
#include <fshell/ltkutils.h>
#include <fshell/bsym.h>
#include <fshell/descriptorutils.h>
#include <fshell/iocons_writer.h>
#include <fshell/line_editor.h>
#include <fshell/qr3dll.h>
#include <u32std.h> // Is this available in all environments? If not, need to define TDesType ourselves
#include <babitflags.h>
#include <badesca.h>
_LIT(KCrLf, "\r\n");
_LIT(KPrompt, "fdb>");

using namespace IoUtils;
using namespace LtkUtils;

const TInt KMaxRegisters = 32;

class CCmdFdb : public CMemoryAccessCommandBase, public MLineEditorObserver, public MLineCompleter
	{
public:
	static CCommandBase* NewLC();
	~CCmdFdb();
private:
	struct SThreadContext;
	CCmdFdb();
	void InstallL();
	void AttachL(TUint aThreadId);
	void Detach(SThreadContext* aContext);
	void PrintThreadInfo(SThreadContext& aContext);
	void PrintRegistersL(SThreadContext& aThread);
	void PrintRegistersL(SThreadContext& aThread, TBool aUserMode);
	void PrintRegistersLRPC(SThreadContext& aThread, TBool aUserMode);
	void RefreshL(SThreadContext& aContext);
	void RefreshIfRunningL(SThreadContext& aContext);
	TPtrC LookupSymbol(TUint32 aAddress);
	void StartCommandParsingL();
	void ProcessLineL(const TDesC& aLine);
	void FocusL(TUint aThreadId);
	SThreadContext* ContextForThread(TUint aThreadId) const;
	SThreadContext& CurrentL();
	SThreadContext& CurrentNoRefreshL();
	enum TPrintMode { EJustSymbols, EAllData, EHexDump };
	void PrintMemL(TUint aThreadId, TUint32 aStart, TUint32 aEnd, TPrintMode aMode);
	void BrrrrrainsL();
	void ShowHelpTextL();
	TBool IsSymbol(TUint aAddress) const;
	void StartInteractiveViewL(TLinAddr aAddress);
	void DrawMemViewL(TBool aUpdate);
	void UpdateMemForMemViewL();
	static TInt IsDescriptorHeader(TUint8 const* aHeaderPtr, TInt aMaxLen);
	void ShowBreakpointsL();
	static TInt BreakTest(TAny* aPtr);
	static TInt BreakTestLtk(TAny* aPtr);
	static TInt BreakTestCond(TAny* aPtr);
	void ClearAllBreakpointsL();
	void BreakpointHit(const RMemoryAccess::TBreakpointNotification& aNotif);
	void CompleteLineWithSymbolL(TConsoleLine& aLine, TLex& aLex);
	void CheckForConditionL(TLex& aLex, RMemoryAccess::TPredicate& aCondition);
	
private: // From MLineCompleter
	void LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar);

private: // From MLineEditorObserver
	void LeoHandleLine(const TDesC& aLine);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	void RunL();
	void DoCancel();

private:
	// Command line stuff
	TBool iAllThreads;
	TUint iThreadId;
	TFileName2 iBsymFile;
	TFileName2 iMapFileDir;

	// Thread context
	struct SThreadContext
		{
		SThreadContext() : iUserValidRegisters(0), iSupervisorValidRegisters(0), iFlags(0) {}

		RThread iThread;
		TUint32 iUserRegisters[KMaxRegisters];
		TUint32 iUserValidRegisters;
		TUint32 iSupervisorRegisters[KMaxRegisters];
		TUint32 iSupervisorValidRegisters;
		TThreadKernelInfo iKernelInfo;
		enum TFlags { ERunning = 1, };
		TBitFlags32 iFlags;
		};
	SThreadContext* iCurrent;
	RPointerArray<SThreadContext> iThreads;

	// Other stuff
	CSymbolics* iSymbols;
	TBuf<256> iTempNameBuf;

	// Line editor support
	TIoConsWriterAdaptor iConsoleAdapter;
	CLineEditor* iLineEditor;
	TBuf<256> iCommandLineBuf;
	TBool iFinishedCommand;
	TBool iShouldExit;

	// Interactive wossname support
	enum TViewType
		{
		EUnspecified,
		EStack,
		EHeap,
		};
	TViewType iMemoryViewType;
	TLinAddr iMemStart;
	RBuf8 iMemBuf;
	/*
	class TCrumb
		{
		enum TType { EAddress, EHeapCell, };
		TType iType;
		TLinAddr iAddress;
		TInt iIndex;
		}
	RArray<TCrumb> iBreadcrumbs;
	RArray<TLinAddr> iMarks;
	RArray<TLinAddr> iLinks;
	*/

	// Breakpoint support
	RMemoryAccess::TBreakpointNotification iBreakpointNotification;
	TPckg<RMemoryAccess::TBreakpointNotification> iBreakpointNotificationPkg;
	class CBreakpointNotifier : public CActive
		{
	public:
		CBreakpointNotifier(CCmdFdb& aCmd);
		~CBreakpointNotifier();

	private:
		void Request();
		void RunL();
		void DoCancel();

	private:
		CCmdFdb& iCmd;
		};
	friend class CBreakpointNotifier; // Declaration needed for gcc 2.9
	CBreakpointNotifier* iBreakpointNotifier;
	};

EXE_BOILER_PLATE(CCmdFdb)

CCommandBase* CCmdFdb::NewLC()
	{
	CCmdFdb* self = new(ELeave) CCmdFdb();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFdb::~CCmdFdb()
	{
	Cancel();
	delete iLineEditor;
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		SThreadContext* thread = iThreads[i];
		thread->iThread.Close();
		delete thread;
		}
	iThreads.Close();
	delete iSymbols;
	iMemBuf.Close();
	delete iBreakpointNotifier;
	/*
	iLinks.Close();
	iBreadcrumbs.Close();
	iMarks.Close();
	*/
	}

CCmdFdb::CCmdFdb()
	: CMemoryAccessCommandBase(EManualComplete), iConsoleAdapter(Stdout()), iBreakpointNotificationPkg(iBreakpointNotification)
	{
	}

const TDesC& CCmdFdb::Name() const
	{
	_LIT(KName, "fdb");	
	return KName;
	}

void CCmdFdb::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iThreadId, _L("threadid"));
	}

void CCmdFdb::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iAllThreads, _L("all"));
	aOptions.AppendFileNameL(iBsymFile, _L("symbols"));
	aOptions.AppendFileNameL(iMapFileDir, _L("mapfiles"));
	}

struct STestData
	{
	STestData() : iPtr8(NULL, 0) {}

	char iPad1[12];
	TBuf8<6> iBuf8;
	TInt iPad2;
	TBufC8<9> iBufC8;
	char iPad3;
	TPtrC8 iPtrC8;
	TUint64 iPad4;
	TPtr8 iPtr8;
	char iPad5[3];
	};


void CCmdFdb::DoRunL()
	{
	LoadMemoryAccessL();
	TInt mode = iMemAccess.GetZombieDebugMode();
	LeaveIfErr(mode, _L("Couldn't get zombie mode from driver"));

	iSymbols = new(ELeave) CSymbolics(FsL());
	if (iBsymFile.Length())
		{
		//Printf(_L("Loading symbolics... ")); // It can take a while...
		iSymbols->AddBsymFileL(iBsymFile);
		//Printf(_L("done.\r\n"));
		}
	if (iMapFileDir.Length())
		{
		iSymbols->SetFallbackMapFileDirL(iMapFileDir);
		}

	// Testing
	//CMapFile* map = CMapFile::NewL(FsL(), _L("C:\\symbols\\QResources3.exe.map"));
	//iSymbols->AddMapFileL(map);
	//const TDesC& symName = iSymbols->LookupL(_L("QResources3.exe"), 0x10abc);
	//Printf(_L("Found symbol = %S\r\n"), &symName);
	
	if (Env().IsDefined(_L("FDB_TEST")))
		{
		STestData test;
		test.iBuf8 = _L8("12345");
		test.iBufC8 = _L8("abcdefg");
		test.iPtrC8.Set(test.iBufC8);
		TPckg<STestData> pkg(test);
		iMemBuf.Assign(pkg.AllocL());
		DrawMemViewL(EFalse);
		return;
		}
	
	TBool install = ETrue;
	// Figure out something sensible to do
	if (mode != 0 || iThreadId != 0) install = EFalse;

	if (iAllThreads) Printf(_L("Note that because --all-threads was specified, this and all future commands will hang rather than exiting!\r\n"));

	InstallL(); // Always do this, in case user has changed the --all-threads setting
	if (install)
		{
		// This will really mess up evalid, but the toolkit doesn't really use it so I don't feel *too* guilty
		Printf(_L8("Welcome to fdb (build " __DATE__ " " __TIME__ ").\r\nDebugger hook installed and will stay running until you type 'uninstall'.\r\nYou can exit fdb if required and it will stay running in the background.\r\nType 'help' for command info.\r\n"));
		}
	else
		{
		AttachL(iThreadId);
		}

	iBreakpointNotifier = new(ELeave) CBreakpointNotifier(*this);
	TInt err = iMemAccess.RegisterPersistantBreakpoint(LtkUtils::BreakpointAddr());
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		LeaveIfErr(err, _L("Couldn't install the LtkUtils::Breakpoint() breakpoint"));
		}
	StartCommandParsingL();
	}

void CCmdFdb::InstallL()
	{
	LeaveIfErr(iMemAccess.SetZombieDebugMode(iAllThreads ? 2 : 1), _L("Couldn't install thread monitor"));
	const TUint32 KAllThreadsSystem = 1; // Word 2, bit 0
	TUint32 dbgmask = UserSvr::DebugMask(2);
	if (dbgmask & KAllThreadsSystem)
		{
		Printf(_L("(Clearing KAllThreadsSystem attribute)\r\n"));
		User::SetDebugMask(dbgmask & ~KAllThreadsSystem, 2);
		}
	}

void CCmdFdb::AttachL(TUint aThreadId)
	{
	if (aThreadId == 0)
		{
		// Need to figure out something to attach to
		TBuf8<64> buf; // TODO handle more than this many zombies
		TInt res = iMemAccess.GetZombies(buf);
		LeaveIfErr(res, _L("Couldn't get zombie info from driver"));
		//TInt leftOver = 0;
		//if (res > buf.MaxLength()) leftOver = (res - buf.MaxLength()) / sizeof(TUint);
		RMemoryAccess::TZombieInfo* ptr = (RMemoryAccess::TZombieInfo*)buf.Ptr();
		TInt zombiecount = buf.Length() / sizeof(RMemoryAccess::TZombieInfo);
		for (TInt i = 0; i < zombiecount; i++)
			{
			TUint id = ptr[i].iThreadId;
			if (ContextForThread(id)) continue; // already attached to this thread, keep looking
			RThread thread;
			TInt err = iMemAccess.RThreadForceOpen(thread, id);
			if (err)
				{
				PrintWarning(_L("Couldn't open thread %u"), id);
				continue;
				}
			iTempNameBuf = thread.FullName();
			Printf(_L("(Found thread id %u %S)\r\n"), id, &iTempNameBuf);
			if (aThreadId == 0) aThreadId = id;
			thread.Close();
			}
		if (aThreadId == 0)
			{
			if (buf.Length())
				Printf(_L("No zombied threads found that aren't already attached.\r\n"));
			else
				PrintWarning(_L("No zombied threads found to attach to."));
			return;
			}
		else
			{
			Printf(_L("(Attaching to thread %u)\r\n"), aThreadId);
			}
		}

	SThreadContext* context = ContextForThread(aThreadId);
	if (context)
		{
		PrintWarning(_L("Already attached to thread %u, focussing it instead."), aThreadId);
		iCurrent = context;
		return;
		}

	context = new(ELeave) SThreadContext;
	CleanupStack::PushL(context);
	RThread& thread(context->iThread);
	CleanupClosePushL(thread);
	LeaveIfErr(iMemAccess.RThreadForceOpen(thread, aThreadId), _L("Couldn't open thread %u"), aThreadId);
	RefreshL(*context);
	iThreads.AppendL(context);
	CleanupStack::Pop(2, context); // context->iThread, context
	iCurrent = context;

	PrintThreadInfo(*context);
	// Don't print any more than this - it makes the output too wordy
	//PrintRegistersL(*context);
	}

void CCmdFdb::PrintThreadInfo(SThreadContext& aContext)
	{
	RThread& aThread = aContext.iThread;
	TFullName name = aThread.FullName();
	Write(_L("Name: "));
	Write(name);

	Printf(_L("\r\nThread Id: %d, exit type: "), (TUint)aThread.Id());
	TExitType exitType = aThread.ExitType();
	if (exitType == EExitPending)
		{
		Printf(_L("Still running\r\n"));
		}
	else if (exitType == EExitKill)
		{
		Printf(_L("Kill %d\r\n"), aThread.ExitReason());
		}
	else if (exitType == EExitTerminate)
		{
		Printf(_L("Terminate %d\r\n"), aThread.ExitReason());
		}
	else if (exitType == EExitPanic)
		{
		TExitCategoryName exitCategory = aThread.ExitCategory();
		Printf(_L("Panic %S %d\r\n"), &exitCategory, aThread.ExitReason());
		}

	Printf(_L("User stack: base=%08x limit=%08x\r\n"), aContext.iKernelInfo.UserStackBase(), aContext.iKernelInfo.iUserStackLimit);
	Printf(_L("Kern stack: base=%08x limit=%08x\r\n"), aContext.iKernelInfo.iSupervisorStack + aContext.iKernelInfo.iSupervisorStackSize, aContext.iKernelInfo.iSupervisorStack);
	}

void CCmdFdb::PrintRegistersL(SThreadContext& aThread)
	{
	PrintRegistersL(aThread, ETrue);
	PrintRegistersL(aThread, EFalse);
	}

void CCmdFdb::PrintRegistersL(SThreadContext& aThread, TBool aUserMode)
	{
	if (aUserMode)
		{
		Printf(_L("User mode registers:\r\n"));
		}
	else
		{
		Printf(_L("Supervisor mode registers:\r\n"));
		}

	TUint32 valid = aThread.iUserValidRegisters;
	TUint32* ptr = aThread.iUserRegisters;
	if (!aUserMode)
		{
		valid = aThread.iSupervisorValidRegisters;
		ptr = aThread.iSupervisorRegisters;
		}

	if (valid == 0) Printf(_L("(No valid registers)\r\n"));
	// From here down is ARM specific, but since no other platforms implement this API there's no pressing need to ifdef it
	// See TArmRegSet for the ordering
	for (TInt i = 0; i < 16; i++)
		{
		if (valid & (1<<i))
			{
			if (i < 10) Write(_L(" ")); // extra space for padding
			Printf(_L("  R%d: "), i);
			Printf(_L("%08x "), ptr[i]);
			Write(LookupSymbol(ptr[i]));
			Write(KCrLf);
			}
		}
	if (valid & (1<<16))
		{
		Printf(_L("Flags (%cPSR): %08x\r\n"), aUserMode ? 'C' : 'S', ptr[16]); // CPSR or SPSR
		}
	if (valid & (1<<17))
		{
		// Domain Access Control Register
		Printf(_L(" DACR: %08x\r\n"), ptr[16]);
		}
	}

void CCmdFdb::PrintRegistersLRPC(SThreadContext& aThread, TBool aUserMode)
	{
	// Print these in the format of the stack trace
	TUint32 valid = aThread.iUserValidRegisters;
	TUint32* ptr = aThread.iUserRegisters;
	if (!aUserMode)
		{
		valid = aThread.iSupervisorValidRegisters;
		ptr = aThread.iSupervisorRegisters;
		}
	
	if (valid & 1<<15)
		{
		Printf(_L("     R15: %08x "), ptr[15]);
		Write(LookupSymbol(ptr[15]));
		Write(KCrLf);
		}
	if (valid & 1<<14)
		{
		Printf(_L("     R14: %08x "), ptr[14]);
		Write(LookupSymbol(ptr[14]));
		Write(KCrLf);
		}
	}

void CCmdFdb::RefreshIfRunningL(SThreadContext& aContext)
	{
	if (iCurrent->iFlags.IsSet(SThreadContext::ERunning))
		{
		// Thread is still running, we should refresh our info
		RefreshL(aContext);
		}
	}

void CCmdFdb::RefreshL(SThreadContext& aContext)
	{
	aContext.iFlags.Assign(SThreadContext::ERunning, aContext.iThread.ExitType() == EExitPending);
	TUint tid = aContext.iThread.Id();

	// Get registers
	TPtr8 userreg((TUint8*)&aContext.iUserRegisters[0], KMaxRegisters*sizeof(TUint32), KMaxRegisters*sizeof(TUint32));
	userreg.FillZ();
	aContext.iUserValidRegisters = 0;
	TInt err = iMemAccess.GetRegisters(aContext.iThread, ETrue, userreg, aContext.iUserValidRegisters);
	if (err) PrintError(err, _L("Couldn't read user registers for thread %u"), tid);

	TPtr8 supreg((TUint8*)&aContext.iSupervisorRegisters[0], KMaxRegisters*sizeof(TUint32), KMaxRegisters*sizeof(TUint32));
	supreg.FillZ();
	aContext.iSupervisorValidRegisters = 0;
	err = iMemAccess.GetRegisters(aContext.iThread, EFalse, supreg, aContext.iSupervisorValidRegisters);
	if (err) PrintError(err, _L("Couldn't read supervisor registers for thread %u\r\n"), tid);

	// And memaccess info
	TPckg<TThreadKernelInfo> kerninfo(aContext.iKernelInfo);
	err = iMemAccess.GetObjectInfoByHandle(EThread, RThread().Id(), aContext.iThread.Handle(), kerninfo);
	if (err) PrintError(err, _L("Couldn't read thread info from memoryaccess for thread %u\r\n"), tid);
	//aContext.iFlags.Assign(SThreadContext::ESuspended, aContext.iKernelInfo.iNThreadSuspendCount != 0);
	}
	
TPtrC CCmdFdb::LookupSymbol(TUint32 aAddress)
	{
	TPtrC name;
	if (iSymbols)
		{
		// Try straight ROM address lookup
		TRAPD(err, name.Set(iSymbols->LookupL(aAddress)));
		if (err) PrintError(err, _L("Failed to lookup symbol"));
		if (name.Length()) return name;
		}
	// Try getting a codeseg from memaccess
	TFullName8 codesegname;
	TInt res = iMemAccess.FindAddressInCodeSegments(codesegname, (TAny*)aAddress);
	if (res >= 0)
		{
		iTempNameBuf.Copy(codesegname);
		if (iSymbols)
			{
			// Try codeseg lookup in CSymbolics (ie in a CMapFile)
			TParsePtrC parse(iTempNameBuf);
			TRAPD(err, name.Set(iSymbols->LookupL(parse.NameAndExt(), res)));
			if (err) PrintError(err, _L("Failed to lookup symbol"));
			if (name.Length()) return name;
			}
		// Otherwise fallback to just doing codeseg+offset
		iTempNameBuf.AppendFormat(_L(" + 0x%x"), res);
		return iTempNameBuf;
		}
	// Last ditch, check if euser thinks it's in ROM
	if (RFs::IsRomAddress((TAny*)aAddress))
		{
		_LIT(KSomewhere, "[Somewhere in ROM]");
		return KSomewhere();
		}
	return KNullDesC();
	}

void CCmdFdb::StartCommandParsingL()
	{
	TBuf<64> historyFile;
	User::LeaveIfError(FsL().PrivatePath(historyFile));
	historyFile.Insert(0, _L("c:"));
	historyFile.Append(_L("fdb_history"));
	Fs().CreatePrivatePath(EDriveC);

	iLineEditor = CLineEditor::NewL(Fs(), iConsoleAdapter, *this, *this, historyFile);
	iLineEditor->Start(KPrompt, iCommandLineBuf);
	iLineEditor->ReinstatePromptAndUserInput();
	SetErrorReported(EFalse);
	Stdin().WaitForKey(iStatus);
	SetActive();
	}

void CCmdFdb::RunL()
	{
	if (iStatus.Int() < 0)
		{
		// iosrv dead?
		Complete(iStatus.Int());
		return;
		}

	iLineEditor->HandleKey(Stdin().KeyCode(), Stdin().KeyModifiers());
	if (iShouldExit)
		{
		Complete(KErrNone);
		}
	else if (iFinishedCommand)
		{
		iLineEditor->Start(KPrompt, iCommandLineBuf);
		iLineEditor->ReinstatePromptAndUserInput();
		iFinishedCommand = EFalse;
		SetErrorReported(EFalse); // We need to clear this each time through
		}
	
	if (!iShouldExit)
		{
		Stdin().WaitForKey(iStatus);
		SetActive();
		}
	}

void CCmdFdb::DoCancel()
	{
	Stdin().WaitForKeyCancel();
	}

void CCmdFdb::LcCompleteLineL(TConsoleLine& aLine, const TChar& /*aEscapeChar*/)
	{
	TPtrC line = aLine.ContentsToCursor();
	TLex lex(line);
	TPtrC cmd = lex.NextToken();
	if (cmd != _L("b") && cmd != _L("break")) return; // We only currently do completion on the 'break' command

	CompleteLineWithSymbolL(aLine, lex);
	}

void CCmdFdb::CompleteLineWithSymbolL(TConsoleLine& aLine, TLex& aLex)
	{
	// Have we got as far as a codeseg? We don't support completing on codeseg yet
	TPtrC codeseg = aLex.NextToken();
	TPtrC ext = codeseg.Right(4);
	if (ext != _L(".dll") && ext != _L(".exe")) return;

	aLex.SkipSpace();
	TInt symbolStartPos = aLex.Offset();
	TPtrC symbol = aLex.Remainder();
	RLtkBuf buf;
	buf.CreateLC(512);
	buf.Copy(symbol);
	CDesC16Array* suggestions = new(ELeave) CDesC16ArrayFlat(32);
	CleanupStack::PushL(suggestions);
	//TODO this can be slow first time, need some UI to warn the user
	iSymbols->CompleteL(codeseg, buf, *suggestions);
	if (buf.Length() > symbol.Length())
		{
		aLine.Replace(symbolStartPos, buf);
		}
	// If the tab added any chars we don't show choices
	else if (suggestions->Count() > 1)
		{
		buf.Zero();
		for (TInt i = 0; i < suggestions->Count(); i++)
			{
			if (i > 0) buf.AppendL('\t');
			buf.AppendL((*suggestions)[i]);
			}
		aLine.PrintCompletionPossibilitiesL(buf);
		}

	CleanupStack::PopAndDestroy(suggestions);
	CleanupStack::PopAndDestroy(&buf);
	}

void CCmdFdb::LeoHandleLine(const TDesC& aLine)
	{
	iFinishedCommand = ETrue;
	if (aLine.Length())
		{
		TRAPD(err, ProcessLineL(aLine));
		if (err)
			{
			PrintError(err, _L("Error executing command"));
			}
		}
	}

void CCmdFdb::ProcessLineL(const TDesC& aLine)
	{
	TLex lex(aLine);
	TPtrC cmd = lex.NextToken();
	lex.SkipSpace();

	char ch = 0;
	if (cmd.Length() == 1)
		{
		ch = cmd[0];
		}

	_LIT(KHelp, "help");
	_LIT(KExit, "exit");
	_LIT(KAttach, "attach");
	_LIT(KDetach, "detach");
	_LIT(KFocus, "focus");
	_LIT(KRegisters, "registers");
	_LIT(KLookup, "lookup");
	_LIT(KStack, "stack");
	_LIT(KKstack, "kstack");
	_LIT(KList, "list");
	_LIT(KMem, "mem");
	_LIT(KUninstall, "uninstall");
	_LIT(KBrowse, "browse");
	_LIT(KBreak, "break");
	_LIT(KContinue, "continue");
	_LIT(KClear, "clear");
	_LIT(KLoad, "load");
	if (cmd == KHelp || ch == 'h')
		{
		ShowHelpTextL();
		}
	else if (cmd == KExit || ch == 'x')
		{
		iShouldExit = ETrue; // TODO check whether a detach is desired
		}
	else if (cmd == KUninstall || ch == 'u')
		{
		Printf(_L("Uninstalling thread hook, clearing all breakpoints, freeing all zombie threads. Undertakers will now get notified of the threads' exits.\r\n"));
		iMemAccess.SetZombieDebugMode(0);
		iShouldExit = ETrue;
		}
	else if (cmd == KAttach || ch == 'a')
		{
		TUint threadId = 0;
		lex.Val(threadId);
		AttachL(threadId);
		}
	else if (cmd == KFocus || ch == 'f')
		{
		TUint threadId = 0;
		lex.Val(threadId);
		if (threadId == 0)
			{
			PrintThreadInfo(CurrentL());
			}
		else
			{
			FocusL(threadId);
			}
		}
	else if (cmd == KRegisters || ch == 'r')
		{
		PrintRegistersL(CurrentL());
		}
	else if (cmd == KLookup || ch == '?')
		{
		TUint32 addr = LtkUtils::HexLexL(lex);
		Printf(_L("%08x: "), addr);
		Write(LookupSymbol(addr));
		Write(KCrLf);
		}
	else if (cmd == KDetach || ch == 'd')
		{
		TUint threadId = 0;
		lex.Val(threadId);
		SThreadContext* thread = NULL;
		if (threadId == 0)
			{
			// No point refreshing, all it will do is possibly complain about reading the registers (which looks confusing)
			thread = &CurrentNoRefreshL();
			}
		else
			{
			thread = ContextForThread(threadId);
			if (!thread) LeaveIfErr(KErrNotFound, _L("Thread %u is not currently attached."), threadId);
			}
		Detach(thread);
		}
	else if (cmd == KStack || ch == 't')
		{
		TBool all = lex.NextToken() == _L("all");
		SThreadContext& c = CurrentL();
		TUint32 start = c.iKernelInfo.iUserStackLimit;
		TUint32 end = c.iKernelInfo.UserStackBase();
		if (start == 0)
			{
			Printf(_L("No user stack for this thread.\r\n"));
			}
		else
			{
			PrintRegistersLRPC(c, ETrue);
			if ((c.iUserValidRegisters & (1 << 13)) && Rng(start, c.iUserRegisters[13], end)) start = c.iUserRegisters[13];
			PrintMemL(c.iThread.Id(), start, end, all ? EAllData : EJustSymbols);
			}
		}
	else if (cmd == KKstack || ch == 'k')
		{
		TBool all = lex.NextToken() == _L("all");
		SThreadContext& c = CurrentL();
		TUint32 start = c.iKernelInfo.iSupervisorStack;
		TUint32 end = start + c.iKernelInfo.iSupervisorStackSize;
		if (start == 0)
			{
			Printf(_L("Couldn't find kernel stack for this thread (!?).\r\n"));
			}
		else
			{
			PrintRegistersLRPC(c, EFalse);
			if ((c.iSupervisorValidRegisters & (1 << 13)) && Rng(start, c.iSupervisorRegisters[13], end)) start = c.iSupervisorRegisters[13];
			PrintMemL(0, start, end, all ? EAllData : EJustSymbols); // zero is the null thread, which is a kernel thread thus has the address space we're interested in
			}
		}
	else if (cmd == KList || ch == 'l')
		{
		BrrrrrainsL();
		}
	else if (cmd == KMem || ch == 'm')
		{
		TUint32 start = LtkUtils::HexLexL(lex);
		lex.SkipSpace();
		TUint32 len = LtkUtils::HexLexL(lex);
		PrintMemL(CurrentL().iThread.Id(), start, start+len, EHexDump);
		}
	else if (cmd == KBrowse)
		{
		TPtrC remainder = lex.Remainder();
		TLinAddr addr = 0;
		if (remainder.Length() == 0 || remainder == _L("heap"))
			{
			//TODO
			}
		else if (remainder == _L("stack"))
			{
			//TODO
			}
		else
			{
			addr = LtkUtils::HexLexL(lex);
			}
		StartInteractiveViewL(addr);
		}
	else if (cmd == KBreak || ch == 'b')
		{
		TInt res = KErrNone;
		TBool set = ETrue;
		TPtrC remainder = lex.Remainder();
		if (remainder.Length() == 0)
			{
			ShowBreakpointsL();
			set = EFalse;
			}
		else if (remainder == _L("test"))
			{
			// This is undocumented, for testing only
			RThread me;
			res = iMemAccess.SetBreakpoint(me, (TLinAddr)&LtkUtils::RawPrint);
			LeaveIfErr(res, _L("Couldn't set test breakpoint"));
			RThread testThread;
			LeaveIfErr(testThread.Create(_L("BreakpointTestThread"), &BreakTest, 8192, NULL, NULL), _L("Couldn't create test thread"));
			testThread.Resume();
			testThread.Close();
			}
		else if (remainder == _L("testltk"))
			{
			// This is undocumented, for testing only
			RThread testThread;
			LeaveIfErr(testThread.Create(_L("BreakpointLtkTestThread"), &BreakTestLtk, 8192, NULL, NULL), _L("Couldn't create test thread"));
			testThread.Resume();
			testThread.Close();
			set = EFalse;
			}
		else if (remainder == _L("testhw"))
			{
			// This is undocumented, for testing only
			RThread testThread;
			LeaveIfErr(testThread.Create(_L("BreakpointTestThread"), &BreakTest, 8192, NULL, NULL), _L("Couldn't create test thread"));
			res = iMemAccess.SetBreakpoint(testThread, (TLinAddr)&LtkUtils::RawPrint);
			LeaveIfErr(res, _L("Couldn't set test breakpoint"));
			testThread.Resume();
			testThread.Close();
			}
		else if (remainder == _L("testcond"))
			{
			// This is undocumented, for testing only
			RThread testThread;
			LeaveIfErr(testThread.Create(_L("BreakpointTestThread"), &BreakTestCond, 8192, NULL, NULL), _L("Couldn't create test thread"));
			RMemoryAccess::TPredicate condition;
			LeaveIfErr(condition.AddCondition(RMemoryAccess::TPredicate::ESignedEq, 0, (TUint)-5), _L("Couldn't add condition"));
			res = iMemAccess.SetBreakpoint(testThread, (TLinAddr)&User::Leave, &condition);
			LeaveIfErr(res, _L("Couldn't set test breakpoint"));
			testThread.Resume();
			testThread.Close();
			}
		else
			{
			TUint addr;
			TInt err = HexLex(lex, addr);
			if (err)
				{
				// Try codeseg and symbol name
				TPtrC codeseg = lex.NextToken();
				lex.SkipSpace();
				TPtrC name = lex.Remainder();
				// Hmm symbols can have spaces in, how to distinguish the condition? Assume if the last word starts with an 'r' it's a condition. Not very nice.
				TInt space = name.LocateReverse(' ');
				RMemoryAccess::TPredicate condition;
				if (space >= 0 && space+1 < name.Length() && name[space+1] == 'r')
					{
					name.Set(name.Left(space));
					lex.Inc(name.Length());
					while (!lex.Eos())
						{
						CheckForConditionL(lex, condition);
						}
					}

				TUint offset = 0;
				TRAPL(offset = iSymbols->CodesegOffsetFromSymbolNameL(codeseg, name), _L("Couldn't find offset of symbol '%S' in codeseg '%S'"), &name, &codeseg);
				RLtkBuf8 codeseg8;
				CleanupClosePushL(codeseg8);
				codeseg8.AppendL(codeseg);
				res = iMemAccess.SetSymbolicBreakpoint(CurrentL().iThread, codeseg8, offset, &condition);
				LeaveIfErr(res, _L("Couldn't create symbolic breakpoint"));
				CleanupStack::PopAndDestroy(&codeseg8);
				}
			else
				{
				RMemoryAccess::TPredicate condition;
				while (!lex.Eos())
					{
					CheckForConditionL(lex, condition);
					}
				res = iMemAccess.SetBreakpoint(CurrentL().iThread, addr, &condition);
				LeaveIfErr(res, _L("Couldn't create breakpoint"));
				Printf(_L("Breakpoint created at 0x%08x "), addr);
				Write(LookupSymbol(addr));
				Write(KCrLf);
				}
			}

		if (set)
			{
			if (res & RMemoryAccess::TBreakpointInfo::EHardware)
				{
				Printf(_L("Hardware breakpoint %d set.\r\n"), res & ~RMemoryAccess::TBreakpointInfo::EHardware);
				}
			else if (res == 0)
				{
				Printf(_L("Pending breakpoint set (Note these don't work yet!)\r\n"));
				}
			else
				{
				Printf(_L("Breakpoint %d set.\r\n"), res);
				}
			}
		}
	else if (cmd == KContinue || ch =='c')
		{
		TInt err = iMemAccess.ContinueFromBreakpoint(CurrentL().iThread);
		LeaveIfErr(err, _L("Couldn't continue - is the thread definitely stopped on a breakpoint?"));
		}
	else if (cmd == KClear)
		{
		TPtrC remainder = lex.Remainder();
		if (remainder.Length() == 0)
			{
			ClearAllBreakpointsL();
			}
		else
			{
			TInt n;
			User::LeaveIfError(lex.Val(n));
			LeaveIfErr(iMemAccess.ClearBreakpoint(n), _L("Couldn't clear breakpoint %d"), n);
			}
		}
	else if (cmd == KLoad)
		{
		TPtrC path = lex.Remainder();
		if (path.Right(5).CompareF(_L(".bsym")) == 0)
			{
			//Printf(_L("Loading symbolics... ")); // It can take a while...
			iSymbols->AddBsymFileL(path);
			//Printf(_L("done.\r\n"));
			}
		else
			{
			iSymbols->SetFallbackMapFileDirL(path);
			}
		}
	else
		{
		PrintError(KErrNotFound, _L("Unrecognised command '%S'. Try 'help'."), &cmd);
		}
	}

CCmdFdb::SThreadContext* CCmdFdb::ContextForThread(TUint aThreadId) const
	{
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		if ((TUint)iThreads[i]->iThread.Id() == aThreadId)
			{
			return iThreads[i];
			}
		}
	return NULL;
	}

CCmdFdb::SThreadContext& CCmdFdb::CurrentL()
	{
	CCmdFdb::SThreadContext& current = CurrentNoRefreshL();
	RefreshIfRunningL(current);
	return current;
	}

CCmdFdb::SThreadContext& CCmdFdb::CurrentNoRefreshL()
	{
	if (!iCurrent)
		{
		LeaveIfErr(KErrNotReady, _L("No currently focussed thread"));
		}
	return *iCurrent;
	}

void CCmdFdb::FocusL(TUint aThreadId)
	{
	SThreadContext* c = ContextForThread(aThreadId);
	if (c) iCurrent = c;
	else
		{
		LeaveIfErr(KErrNotFound, _L("Couldn't find thread id %u in the attached threads. Do you need to do 'attach %u'?"), aThreadId, aThreadId);
		}
	}

void CCmdFdb::Detach(SThreadContext* aContext)
	{
	TInt err = iMemAccess.ReleaseZombie(aContext->iThread);
	if (err && aContext->iFlags.IsSet(SThreadContext::ERunning))
		{
		// Don't complain about driver if the thread wasn't actually zombied
		PrintError(err, _L("Driver couldn't find zombie thread"));
		}

	TInt arrayPos = iThreads.Find(aContext);
	iThreads.Remove(arrayPos);
	if (iCurrent == aContext) iCurrent = NULL;
	aContext->iThread.Close();
	delete aContext;
	}

void CCmdFdb::PrintMemL(TUint aThreadId, TUint32 aStart, TUint32 aEnd, TPrintMode aMode)
	{
	// word-align start and end
	aStart &= ~3;
	aEnd = (aEnd+3) & (~3);

	TInt size = aEnd - aStart;
	RBuf8 mem;
	CleanupClosePushL(mem);
	mem.CreateL(size);

	TThreadMemoryAccessParamsBuf params;
	params().iId = aThreadId;
	params().iAddr = (TUint8*)aStart;
	params().iSize = size;

	TInt err = iMemAccess.GetThreadMem(params, mem);
	LeaveIfErr(err, _L("Couldn't read thread memory %08x-%08x"), aStart, aEnd);

	if (aMode == EHexDump)
		{
		TInt offset = (TInt)aStart;
		LtkUtils::HexDumpToOutput(mem, Stdout(), offset);
		}
	else
		{
		const TUint32* ptr = (const TUint32*)mem.Ptr();
		const TInt count = mem.Size() / 4;
		for (TInt i = 0; i < count; i++)
			{
			TUint32 word = ptr[i];
			TBool print = aMode == EAllData || IsSymbol(word);
			if (print)
				{
				Printf(_L("%08x: %08x "), aStart + i*4, word);
				Write(LookupSymbol(word));
				Write(KCrLf);
				}
			}
		}
	CleanupStack::PopAndDestroy(&mem);
	}

void CCmdFdb::BrrrrrainsL()
	{
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(1024);
	TInt res = iMemAccess.GetZombies(buf);
	LeaveIfErr(res, _L("Couldn't get zombie info from driver"));
	RMemoryAccess::TZombieInfo* ptr = (RMemoryAccess::TZombieInfo*)buf.Ptr();
	const TInt zombiecount = buf.Length()/sizeof(RMemoryAccess::TZombieInfo);

	// Go through all the zombies
	for (TInt i = 0; i < zombiecount; i++)
		{
		TUint id = ptr[i].iThreadId;
		SThreadContext* context = ContextForThread(id);
		char stat = '-';
		char suspended = '-';
		if (context)
			{
			stat = 'a';
			if (context == iCurrent) stat = '*';
			iTempNameBuf = context->iThread.FullName();
			}
		else
			{
			RThread thread;
			TInt err = iMemAccess.RThreadForceOpen(thread, id);
			if (err)
				{
				PrintWarning(_L("Couldn't open thread %u"), id);
				continue;
				}
			iTempNameBuf = thread.FullName();
			thread.Close();
			}
		if (ptr[i].iFlags & RMemoryAccess::TZombieInfo::ESuspended) suspended = 's';
		if (ptr[i].iFlags & RMemoryAccess::TZombieInfo::EBreakpoint) suspended = 'b';

		Printf(_L("%c%c %u "), stat, suspended, id);
		Write(iTempNameBuf);
		Write(KCrLf);
		}

	// Now do any attached threads that aren't zombied
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		SThreadContext* c = iThreads[i];
		TUint id = (TUint)c->iThread.Id();
		RMemoryAccess::TZombieInfo dummy; dummy.iThreadId = id;
		TBool foundInZombies = EFalse;
		if (zombiecount) foundInZombies = RArray<RMemoryAccess::TZombieInfo>(sizeof(RMemoryAccess::TZombieInfo), ptr, zombiecount).Find(dummy) != KErrNotFound;
		if (!foundInZombies)
			{
			char stat = 'a';
			if (c == iCurrent) stat = '*';
			iTempNameBuf = c->iThread.FullName();
			Printf(_L("%c- %u "), stat, id);
			Write(iTempNameBuf);
			Write(KCrLf);
			}
		}

	CleanupStack::PopAndDestroy(&buf);
	}

void CCmdFdb::ShowHelpTextL()
	{
	// Possible TODOs: hEap, Save, Ymodem?
	_LIT(KStartOfCommands, "SUPPORTED COMMANDS\r\n\r\n");
	CTextBuffer* helpText = const_cast<CTextBuffer*>(GetHelpTextL());
	TInt found = helpText->Descriptor().Find(KStartOfCommands);
	helpText->Delete(0, found + KStartOfCommands().Length());
	helpText->Write(Stdout());
	delete helpText;
	}

TBool CCmdFdb::IsSymbol(TUint aAddress) const
	{
	// Ranges probably not perfect, seem to be roughly ok though
	TBool okRange = Rng(0x70000000u, aAddress, 0xA0000000u) || Rng(0xC0000000u, aAddress, 0xFC000000u);
	return okRange && aAddress != 0xDEDEDEDE && aAddress != 0xAAAAAAAA && aAddress != 0xBBBBBBBB && aAddress != 0xCCCCCCCC; 
	}

void CCmdFdb::StartInteractiveViewL(TLinAddr aAddress)
	{
	iMemStart = aAddress & (~3);
	if (iMemStart >= iCurrent->iKernelInfo.iUserStackLimit && iMemStart <= iCurrent->iKernelInfo.UserStackBase())
		{
		iMemoryViewType = EStack;
		}
	else //TODO EHeap
		{
		iMemoryViewType = EUnspecified;
		}

	iMemBuf.Zero();
	TSize consoleSize(80,24);
	Stdout().GetScreenSize(consoleSize);
	iMemBuf.ReAllocL(consoleSize.iHeight * 16); // At most we display 16 bytes per line
	TInt numBytesOnLine = 16; // By default
	if (iMemoryViewType == EStack) numBytesOnLine = 4;
	TInt numBytesOnScreen = numBytesOnLine * consoleSize.iHeight;

	TLinAddr lastAddr = 0;
	for (;;)
		{
		iMemStart &= ~3; // Just checking
		TBool update = (iMemStart != lastAddr);
		lastAddr = iMemStart;
		DrawMemViewL(update);
		TUint key = Stdin().ReadKey();
		switch (key)
			{
			case EKeyPageUp:
				iMemStart -= numBytesOnScreen;
				break;
			case EKeyPageDown:
				iMemStart += numBytesOnScreen;
				break;
			case EKeyUpArrow:
				iMemStart -= numBytesOnLine;
				break;
			case EKeyDownArrow:
				iMemStart += numBytesOnLine;
				break;
			case 'q':
			case 'x':
			case EKeyEscape:
				//Stdout().SetAttributesL(ConsoleAttributes::ENone);
				return;
			default:
				break;
			}
		}
	}

void CCmdFdb::UpdateMemForMemViewL()
	{
	iMemBuf.Zero();
	
	TThreadMemoryAccessParamsBuf params;
	params().iId = iCurrent->iThread.Id();
	params().iAddr = (TUint8*)iMemStart;
	params().iSize = iMemBuf.MaxSize();

	TInt err = iMemAccess.GetThreadMem(params, iMemBuf);
	if (err) PrintError(err, _L("Couldn't read thread memory %08x-%08x"), params().iAddr, params().iAddr+params().iSize);
	}

/*
class TFocusable
	{
public:
	TLinAddr iAddr;
	enum TType
		{
		EHeapPtr,
		}
	}
*/

void CCmdFdb::DrawMemViewL(TBool aUpdate)
	{
	Stdout().ClearScreen();
	if (aUpdate) UpdateMemForMemViewL();


	// This is a really messy function. Can't seem to find a way of making it easier to read.
	const ConsoleAttributes::TAttributes KNormal(ConsoleAttributes::ENone, ConsoleAttributes::EBlack, ConsoleAttributes::EWhite);
	const ConsoleAttributes::TAttributes KSymbol(0, ConsoleAttributes::ERed, ConsoleAttributes::EUnchanged);
	const ConsoleAttributes::TAttributes KDescriptor(0, ConsoleAttributes::EUnchanged, ConsoleAttributes::EYellow);
	//const ConsoleAttributes::TAttributes KHeap(0, ConsoleAttributes::EUnchanged, ConsoleAttributes::ECyan);
	//const ConsoleAttributes::TAttributes KHeapAlternate(0, ConsoleAttributes::EUnchanged, ConsoleAttributes::EBlue);
	enum { ENoColor = 0, ESymbol = 1, EDescriptor = 2 };

	TSize consoleSize(80,24);
	Stdout().GetScreenSize(consoleSize);
	TUint8 const*const bptr = iMemBuf.Ptr();
	TUint32 const*const ptr = (TUint32 const*)bptr;
	//iLinks.Reset();
	CTextBuffer* text = CTextBuffer::NewLC(1024);
	text->SetAttributesL(KNormal);
	
	TInt numBytesOnLine = 16; // By default
	if (iMemoryViewType == EStack)
		{
		numBytesOnLine = 4;
		}
	//TInt numBytesOnScreen = numBytesOnLine * consoleSize.iHeight;
	TInt remainingInDescriptor = 0; // Not in descriptor initially
	//TInt remainingInHeapCell = 0; // Not worrying about heap cells right now

	for (TInt line = 0; line < consoleSize.iHeight - 1 && line*numBytesOnLine < iMemBuf.Length(); line++)
		{
		TBuf8<16> colorBuf; colorBuf.SetLength(colorBuf.MaxLength());
		TUint8* colorBufPtr = (TUint8*)colorBuf.Ptr();
		Mem::Fill(colorBufPtr, 16, ENoColor);
		text->AppendFormatL(_L("%08x: "), iMemStart + line*numBytesOnLine);

		const TInt idxForLine = line * numBytesOnLine;
		TInt i;
		for (i = 0; i < numBytesOnLine; i += 4, colorBufPtr += 4)
			{
			const TInt idxInBuf = idxForLine + i;
			if (idxInBuf >= iMemBuf.Length()) break;

			if (remainingInDescriptor == 0) remainingInDescriptor = IsDescriptorHeader(bptr + idxInBuf, 256);
			TInt runLen = 0;
			if (remainingInDescriptor > 0)
				{
				text->SetAttributesL(KDescriptor);
				runLen = Min(remainingInDescriptor, numBytesOnLine-i);
				if (runLen > 4) runLen = 4; // We only do up to 4 bytes at a time
				Mem::Fill(colorBufPtr, runLen, EDescriptor);
				}

			// Check for symbols
			TUint32 word = ptr[idxInBuf / 4];
			if (IsSymbol(word))
				{
				text->SetAttributesL(KSymbol);
				colorBufPtr[0] |= ESymbol;
				colorBufPtr[1] |= ESymbol;
				colorBufPtr[2] |= ESymbol;
				colorBufPtr[3] |= ESymbol;
				if (iMemoryViewType == EStack)
					{
					TPtrC symb = LookupSymbol(word);
					text->AppendFormatL(_L("%08x %S"), word, &symb);
					text->SetAttributesL(KNormal);
					text->AppendL(KCrLf);
					continue;
					}
				}

			// Actually print the hex bytes
			for (TInt ch = 0; ch < 4; ch++)
				{
				text->AppendFormatL(_L("%02X"), bptr[idxInBuf+ch]);
				if (runLen)
					{
					remainingInDescriptor--;
					runLen--;
					if (remainingInDescriptor == 0)	text->SetAttributesL(KNormal); // If we've just finished a run we clear the formatting before the space
					}
				text->AppendL(' ');
				}
			if (((i+4) % 16) == 0) text->SetAttributesL(KNormal); // In preparation for printing the ascii
			if (((i+4) % 8) == 0) text->AppendL(' '); // Extra space every 8th char
			}

		TInt rem = numBytesOnLine - i;
		if (rem > 0)
			{
			// Need to fill in spaces for the hex bytes we don't have
			_LIT(K3Space, "   ");
			while (rem--) text->AppendL(K3Space);
			// And the final 2 spaces before the ascii
			text->AppendL(' ');
			text->AppendL(' ');
			}


		// Time to print the ascii
		const TInt max = Min(numBytesOnLine, iMemBuf.Length() - idxForLine);
		for (TInt j = 0; j < max; j++)
			{
			char ch = bptr[idxForLine + j];
			if (ch < 32 || ch >= 128) ch = '.';
			text->SetAttributesL(KNormal);
			if (colorBuf[j] & EDescriptor) text->SetAttributesL(KDescriptor);
			if (colorBuf[j] & ESymbol) text->SetAttributesL(KSymbol);
			text->AppendL(ch);
			}
		text->SetAttributesL(KNormal);
		text->AppendL(KCrLf);
		}

	text->SetAttributesL(ConsoleAttributes::ENone);
	text->Write(Stdout());
	CleanupStack::PopAndDestroy(text);
	}

/*static*/ TInt CCmdFdb::IsDescriptorHeader(TUint8 const* aHeaderPtr, TInt aMaxLen)
	{
	if (((TLinAddr)aHeaderPtr & 0x3) != 0) return 0; // Not aligned
	TInt type = *(TUint32*)aHeaderPtr >> 28;

	if ((type == EPtr || type == EBufCPtr) && aMaxLen >= 12) return 12; // Len + maxlen + ptr
	else if (type == EPtrC && aMaxLen >= 8) return 8; // Len + ptr
	else if (type == EBuf || type == EBufC)
		{
		TInt len = (*(TInt32 const*)aHeaderPtr) & 0xfffffff;
		if (len > aMaxLen || (type == 0 && len == 0)) return 0;
		// Take a stab at whether it's a 16-bit descriptor
		TInt wideness = 1;
		TInt bufOffset = (type == EBuf ? 8 : 4);
		TUint16 const* wptr = (TUint16 const*)(aHeaderPtr + bufOffset);
		if (len > 4 && wptr[0] < 256 && wptr[1] < 256) wideness = 2;
		return bufOffset + len * wideness; // Add 4 so the header itself is included in the calculation
		}
	else
		{
		return 0;
		}
	}

void CCmdFdb::ShowBreakpointsL()
	{
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(1024);
	LeaveIfErr(iMemAccess.GetBreakpoints(buf), _L("Couldn't read breakpoint information"));
	RMemoryAccess::TBreakpointInfo* bs = (RMemoryAccess::TBreakpointInfo*)buf.Ptr();
	RLtkBuf desc;
	desc.CreateL(256);
	TInt count = buf.Length() / sizeof(RMemoryAccess::TBreakpointInfo);
	if (count)
		{
		for (TInt i = 0; i < count; i++)
			{
			RMemoryAccess::TBreakpointInfo& b = bs[i];
			Printf(_L("Breakpoint %d (thread id %u): "), b.iBreakpointId, b.iThreadId);
			Write(LookupSymbol(b.iAddress));
			TBool brackets = b.iFlags & (RMemoryAccess::TBreakpointInfo::EPending | b.iFlags & RMemoryAccess::TBreakpointInfo::EHardware) || !(b.iFlags & RMemoryAccess::TBreakpointInfo::EEnabled) || b.iCondition.HasConditions();
			if (brackets) Write(_L(" ("));
			desc.Zero();
			if (b.iFlags & RMemoryAccess::TBreakpointInfo::EPending) desc.Append(_L("PENDING "));
			if (!(b.iFlags & RMemoryAccess::TBreakpointInfo::EEnabled)) desc.Append(_L("DISABLED "));
			if (b.iFlags & RMemoryAccess::TBreakpointInfo::EHardware) desc.Append(_L("HARDWARE "));
			b.iCondition.Description(desc);
			if (desc.Length() && desc[desc.Length()-1] == ' ') desc.SetLength(desc.Length()-1);
			Write(desc);
			if (brackets) Write(_L(")"));
			Write(KCrLf);
			}
		}
	else
		{
		Printf(_L("No breakpoints defined.\r\n"));
		}
	CleanupStack::PopAndDestroy(&buf);
	}

void CCmdFdb::CBreakpointNotifier::RunL()
	{
	if (iStatus.Int() < 0)
		{
		iCmd.PrintError(iStatus.Int(), _L("Error returned from NotifyBreakpoint"));
		return;
		}
	
	RMemoryAccess::TBreakpointNotification notif = iCmd.iBreakpointNotification;
	iCmd.iMemAccess.NotifyBreakpoint(iCmd.iBreakpointNotificationPkg, iStatus);
	SetActive();

	iCmd.BreakpointHit(notif);
	}

void CCmdFdb::CBreakpointNotifier::DoCancel()
	{
	iCmd.iMemAccess.CancelNotifyBreakpoint();
	}

CCmdFdb::CBreakpointNotifier::CBreakpointNotifier(CCmdFdb& aCmd)
: CActive(CActive::EPriorityStandard), iCmd(aCmd)
	{
	CActiveScheduler::Add(this);
	iCmd.iMemAccess.NotifyBreakpoint(iCmd.iBreakpointNotificationPkg, iStatus);
	SetActive();
	}

CCmdFdb::CBreakpointNotifier::~CBreakpointNotifier()
	{
	Cancel();
	}

void CCmdFdb::BreakpointHit(const RMemoryAccess::TBreakpointNotification& aNotif)
	{
	iLineEditor->RemovePromptAndUserInput();
	Printf(_L("Breakpoint %d hit in thread %u: "), aNotif.iBreakpointId, aNotif.iThreadId);
	Write(LookupSymbol(aNotif.iAddress));
	Write(KCrLf);
	if (iCurrent == NULL)
		{
		Printf(_L("(Attaching to thread %u)\r\n"), aNotif.iThreadId);
		TRAP_IGNORE(AttachL(aNotif.iThreadId));
		}
	iLineEditor->ReinstatePromptAndUserInput();
	}

TInt CCmdFdb::BreakTest(TAny* /*aPtr*/)
	{
	//LtkUtils::Breakpoint();
	LtkUtils::RawPrint(_L8("Breaktest has completed\r\n"));
	return 5;
	}

TInt CCmdFdb::BreakTestLtk(TAny* /*aPtr*/)
	{
	LtkUtils::Breakpoint();
	return 6;
	}

TInt CCmdFdb::BreakTestCond(TAny* /*aPtr*/)
	{
	CTrapCleanup* trap = CTrapCleanup::New();
	TRAPD(err, User::Leave(-1)); // This shouldn't trigger the break
	TRAP(err, User::Leave(-5)); // This should
	delete trap;
	return 0;
	}

void CCmdFdb::ClearAllBreakpointsL()
	{
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(1024);
	LeaveIfErr(iMemAccess.GetBreakpoints(buf), _L("Couldn't read breakpoint information"));
	RMemoryAccess::TBreakpointInfo* bs = (RMemoryAccess::TBreakpointInfo*)buf.Ptr();
	TInt count = buf.Length() / sizeof(RMemoryAccess::TBreakpointInfo);
	for (TInt i = 0; i < count; i++)
		{
		RMemoryAccess::TBreakpointInfo& b = bs[i];
		TInt err = iMemAccess.ClearBreakpoint(b.iBreakpointId);
		if (err) PrintWarning(_L("Couldn't clear breakpoint %d, err=%d"), b.iBreakpointId, err);
		}
	CleanupStack::PopAndDestroy(&buf);
	}

void CCmdFdb::CheckForConditionL(TLex& aLex, RMemoryAccess::TPredicate& aCondition)
	{
	aLex.SkipSpace();
	if (aLex.Eos()) return;
	_LIT(KRegErr, "First argument in a conditional must be a register r0-r15");
	if (aLex.Get() != 'r') LeaveIfErr(KErrArgument, KRegErr);
	TInt reg;
	LeaveIfErr(aLex.Val(reg), KRegErr);
	if (reg < 0 || reg > 15) LeaveIfErr(KErrArgument, KRegErr);
	RMemoryAccess::TPredicate::TOp op = RMemoryAccess::TPredicate::ENothing;
	TUint opchar = aLex.Get();
	switch (opchar)
		{
		case '<':
			op = RMemoryAccess::TPredicate::ELt;
			if (aLex.Peek() == '=')
				{
				op = RMemoryAccess::TPredicate::ELe;
				aLex.Get();
				}
			break;
		case '>':
			op = RMemoryAccess::TPredicate::EGt;
			if (aLex.Peek() == '=')
				{
				op = RMemoryAccess::TPredicate::EGe;
				aLex.Get();
				}
			break;
		case '!':
			if (aLex.Get() != '=') LeaveIfErr(KErrArgument, _L("Unrecognised operand"));
			op = RMemoryAccess::TPredicate::ENe;
			break;
		case '=':
			op = RMemoryAccess::TPredicate::EEq;
			if (aLex.Peek() == '=') aLex.Get(); // We allow == as well as =
			break;
		default:
			LeaveIfErr(KErrArgument, _L("Unrecognised operand"));
			break;
		}
	TInt val;
	TInt err = LtkUtils::HexLex(aLex, (TUint&)val);
	if (err)
		{
		// Try normal - this handles signed negative numbers, which HexLex doesn't afaik
		err = aLex.Val(val);
		}
	LeaveIfErr(err, _L("Couldn't parse value"));
	TBool signedCompare = ETrue;
	if (aLex.Peek() == 'u' || aLex.Peek() == 'U')
		{
		aLex.Get();
		signedCompare = EFalse;
		}
	if (signedCompare)
		{
		op = (RMemoryAccess::TPredicate::TOp)(op + 6); // ELt -> ESignedLt etc
		//Printf(_L("Op=%d reg=%d val=%d\r\n"), op, reg, val);
		}
	//else Printf(_L("Op=%d reg=%d val=%uu\r\n"), op, reg, (TUint)val);
	if (aLex.Peek() == ',')
		{
		// Eat comma
		aLex.Get();
		}
	LeaveIfErr(aCondition.AddCondition(op, reg, (TUint32)val), _L("Couldn't add condition to TPredicate"));
	}
