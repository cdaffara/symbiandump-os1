// tclog.cpp
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

#include <e32base.h>
#define USE_CLOGGER
#include <fshell/clogger.h>
#include <e32math.h>
#include <f32file.h>
#include <badesca.h>
#include <flogger.h>
#include "debugrouter.h"

_LIT(KStaticTag, "tclog::Main");
_LIT(KTag, "tclog1");

#undef ASSERT
#define ASSERT(x) __ASSERT_ALWAYS(x, AssertionFail(#x, __FILE__, __LINE__))

__NORETURN__ void AssertionFail(const char* aAssertion, const char* aFile, TInt aLine)
	{
	RDebug::Printf("Assertion failed: \"%s\" in %s:%i", aAssertion, aFile, aLine);
	__DEBUGGER();
	User::Panic(_L("TCLOG"), aLine);
	}

// Don't instanciate this directly (you can't because of non-exported virtuals) but use the tclogger macro to cheat
class RTestClogger : public RClogger
	{
public:
	void FlushLog()
		{
		TUint opts = GetGlobalOptions();
		SetGlobalOptions(0); // Flush log by disabling buffer
		SetGlobalOptions(opts);
		}

	void ExpectTagChanged()
		{
		iFlags |= 0x10000000; // Magic, see KTestTagEnabled in CloggerClient.cpp
		}

	void CheckTagChanged()
		{
		ASSERT(!(iFlags & 0x10000000));
		}

	void AlignLogToFourK(TInt aLessThisAmount)
		{
		SendReceive(2000, TIpcArgs(aLessThisAmount)); // See EDebugAlignLogFile in cliserv.h
		}

	void Shutdown()
		{
		SendReceive(2001, TIpcArgs());
		Close();
		RClogger::StaticClose(); // So that our TLS isn't left set to an invalid reference
		}

	void ForceBreakpoint()
		{
		SendReceive(2003, TIpcArgs());
		}
	};

#define tclogger (*reinterpret_cast<RTestClogger*>(&clogger))

enum TTests
	{
 	EBasicTests, // 0
	ESoakTests, // 1
	ERdebugRedirect, // 2
	ESoakAndRDebug, // 3
	ECreateLogsDir, // 4
	EDoPerformanceLogging, // 5
	EDebugRouter, // 6
	ESessionWriter, // 7
	ENumTests // Must be last in list
	};

void DoBasicTestsL();
void SoakTestsL(TBool aRedirectRDebug);
void DoRDebugTestsL();
void DoCreateLogsDirTest();
void TestPerformanceLoggingL();
void DebugRouterL();
void SessionWriterL();

RFs fs;
RClogger clogger;

void RunTestsL()
	{
	TBuf<32> cmd;
	User::CommandLine(cmd);
	if (cmd.Length() == 0)
		{
		// Syntax: tclog all
		//         tclog 0 3 5 to run tests 0, 3 and 5
		User::Invariant();
		}

	// Figure out what tests to run
	TUint8 runTests[ENumTests];
	Mem::FillZ(&runTests, sizeof(runTests));
	if (cmd == _L("all"))
		{
		Mem::Fill(&runTests, sizeof(runTests), 1);
		}
	else
		{
		TLex lex(cmd);
		TInt err = KErrNone;
		while (err == KErrNone)
			{
			TInt n;
			err = lex.Val(n);
			if (!err && n < ENumTests)
				{
				runTests[n] = ETrue;
				}
			lex.SkipSpace();
			}
		}

	// Now actually run them
	User::LeaveIfError(clogger.Connect(KTag));
	User::LeaveIfError(fs.Connect());

	if (runTests[EBasicTests]) DoBasicTestsL();
	if (runTests[ESoakTests]) SoakTestsL(EFalse);
	if (runTests[ERdebugRedirect]) DoRDebugTestsL();
	if (runTests[ESoakAndRDebug]) SoakTestsL(ETrue);
	if (runTests[ECreateLogsDir]) DoCreateLogsDirTest();
	if (runTests[EDoPerformanceLogging]) TestPerformanceLoggingL();
	if (runTests[EDebugRouter]) DebugRouterL();
	if (runTests[ESessionWriter]) SessionWriterL();

	tclogger.FlushLog();
	clogger.Close();
	fs.Close();
	}



void DoBasicTestsL()
	{
	CActiveScheduler* s = NULL;

	// Try spawning multiple servers to ensure that the failed-to-initialise case is handled ok in the server
	tclogger.Shutdown();
	clogger.Close();
	RProcess proc1, proc2, proc3;
	proc1.Create(_L("CloggerServer.exe"), KNullDesC);
	proc2.Create(_L("CloggerServer.exe"), KNullDesC);
	proc3.Create(_L("CloggerServer.exe"), KNullDesC);
	proc1.Resume();
	proc2.Resume();
	proc3.Resume();
	User::LeaveIfError(clogger.Connect(KTag));

	clogger.Log(_L("1 This is %s format string"), L"my");
	// Use static methods to check that the format buffer is deallocated when we do our heapcheck at the end
	SLOG(_L("2 A 16-bit anonymous log entry - the tag at the start should be [%S]"), &KStaticTag);

	clogger.SetGlobalOptions(RClogger::EBufferLog);
	ASSERT(clogger.GetGlobalOptions() & RClogger::EBufferLog); // This will fail if the buffer allocation has messed up
	clogger.Log(_L("3 This should appear when we turn buffering off"));
	clogger.SetGlobalOptions(0);

	clogger.SetGlobalOptions(RClogger::ERDebugPrint);
	clogger.Log(_L8("4 This should appear on RDebug::Print"));
	clogger.SetGlobalOptions(RClogger::EBufferLog|RClogger::ERDebugPrint);
	clogger.Log(_L8("5 This and all future lines up until the idle timer one should also appear on RDebug::Print"));

	clogger.Log(_L("6 An entry..."));
	User::After(2000000);
	clogger.Log(_L("7 ...and another 2 seconds later"));

	clogger.SetEnabled(KStaticTag, 0);
	_LIT(KEnabled, "Enabled");
	_LIT(KDisabled, "Disabled");
	SLOG(_L("This shouldn't appear in the log file because the tag is %S"), clogger.IsEnabled(KStaticTag) ? &KEnabled : &KDisabled);

	clogger.SetEnabled(KStaticTag, RClogger::EAllEnabled);
	SLOG(_L("8 Tag is now enabled again"));

	TInt heapSize = User::Allocator().Count();
	RClogger::SetStaticLogBehaviour(RClogger::EUseHeapBuffer);
	RClogger::Slog(_L("9 This call should cause the heapsize to go up by two cells as the TLS is upgraded and the format buffer is cached"));
	if (User::Allocator().Count() == heapSize + 2)
		{
		SLOG("10 And it did!");
		}
	else
		{
		SLOG("BUT IT DIDN'T!");
		}
	RClogger::StaticClose(); // So the heaptest passes

	clogger.SetEnabled(KStaticTag, 2);
	SLOG(1, _L("This shouldn't appear because the log mask is only 0x2"));
	SLOG(3, _L8("11 Logging to multiple masks will work so long as at least one of them is enabled"));

	TInt numBuffers;
	TInt size = clogger.GetRamBufferSize(&numBuffers);

	clogger.Log("12 %i buffers of size %i", numBuffers, size);
	clogger.SetRamBufferSize(1024, 3);
	size = clogger.GetRamBufferSize(&numBuffers);
	SLOG("13 new settings %i buffers of size %i", numBuffers, size);

	s=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(s);
	clogger.SetLogBehaviour(RClogger::EMonitorTagState);
	clogger.SetEnabled(KTag, EFalse);
	tclogger.ExpectTagChanged();
	//CActiveScheduler::Start();
	SLOG("14 Disabled tag");
	clogger.Log("This shouldn't appear as we're disabled - in fact it shouldn't get as far as the server");
	tclogger.CheckTagChanged();
	clogger.SetEnabled(KTag, RClogger::EAllEnabled);

	TVolumeInfo volInfo;
	User::LeaveIfError(fs.Volume(volInfo));
	if (volInfo.iFree < 67108864) // Don't fill up the drive unless the emulator is using a C drive < 64MB
		{
		//														  ..---------- --:--:--.---: [Clogger] Log truncated due to disk full\r\n
		_LIT(KTrunc, "15 This log line should be truncated here: |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		const TInt KTimeStampLength = 36;
		tclogger.AlignLogToFourK(KTrunc().Length() + KTimeStampLength); // This is needed so that the next write forces a disk allocation (and doesn't just fill up an already-allocated cluster)
		clogger.Log(KTrunc);
		tclogger.FlushLog();

		TFileName tempName;
		RFile file;
		file.Temp(fs, _L("C:\\logs\\"), tempName, EFileShareExclusive|EFileWrite);
		User::LeaveIfError(fs.Volume(volInfo));
		TInt err = file.SetSize((TUint)volInfo.iFree);
		ASSERT(!err);

		clogger.Log("This line shouldn't appear in the log because the disk is full");
		tclogger.FlushLog();
		clogger.Log("This line shouldn't either");
		tclogger.FlushLog();

		file.Close();
		User::LeaveIfError(fs.Delete(tempName));
		}
	else
		{
		clogger.Log("15 Unable to run disk full test - run emulator with -ci:\\ -- to enable it");
		}

	CDesCArray* tags;
	RBuf8 enabled;
	clogger.GetTagStatesL(tags, enabled);
	TInt n = tags->Count();
	while (n--)
		{
		TPtrC tagName = tags->MdcaPoint(n);
		SLOG(_L("16 Tag %S is enabled with 0x%x"), &tagName, *((TInt*)&enabled[n*4]));
		}
	SLOG("17 Inverting all tags enabled attributes (and ORing with 0x1 so we can still log)");
	n = tags->Count();
	for (TInt i = 0; i < n; i++)
		{
		*((TUint32*)&enabled[i*4]) = 1 | ~ *((TUint32*)&enabled[i*4]);
		}
	clogger.SetTagStatesL(tags, enabled);
	delete tags;
	clogger.GetTagStatesL(tags, enabled);
	n = tags->Count();
	while (n--)
		{
		TPtrC tagName = tags->MdcaPoint(n);
		SLOG(_L("18 Tag %S is enabled with 0x%x"), &tagName, *((TInt*)&enabled[n*4]));
		}

	enabled.Close();
	delete tags;

	tclogger.FlushLog();
	clogger.SetGlobalOptions(RClogger::EBufferLog);
	clogger.Log("19 This line should appear when the idle timer flushes the buffer");
	User::After(1000000);
	clogger.Log(_L("This shouldn't appear in the log file because we're buffering"));
	clogger.SetGlobalOptions(RClogger::EBufferLog); // Setting the global options to be the same as before shouldn't trigger a buffer flush

	// Test cenrep persistance
	tclogger.Shutdown();
	//CActiveScheduler::Start(); //DEBUG testing ServerDied
	clogger.Connect(KTag);
	clogger.SetRamBufferSize(3000, 9);
	clogger.PersistSettings();
	tclogger.Shutdown();
	clogger.Connect(KTag);
	size = clogger.GetRamBufferSize(&numBuffers);
	ASSERT(numBuffers == 9 && size == 3000); // This test can also fail if the disk is full (and therefore cenrep couldn't save the settings).
	clogger.ResetSettings();

	const TUint32 KEnabledMask = 0xB2FA1128;
	clogger.Log(_L("20 testing persistance of tag enabled settings"));
	clogger.SetEnabled(KTag, KEnabledMask);
	clogger.PersistSettings();
	tclogger.Shutdown();
	clogger.Connect(KTag);
	ASSERT(clogger.IsEnabled(KTag) == KEnabledMask);

	TInt err;

	RClogger clog1, clog2, clog3;
	clog1.Connect(KTag);
	clog1.Log("23 Testing multiple sessions for the same tag");
	clog2.Connect(KTag);
	clog3.Connect(KTag);
	clog2.Log("24 Testing multiple sessions for the same tag");
	clog3.Log("25 Testing multiple sessions for the same tag");
	clog3.Close();
	clog1.Close();
	clog2.Close();

	_LIT8(KBin, "Newline: \n Null: \0 <EOM>"); // Useful to put a zero byte in there to test that string termination isn't an issue
	//_LIT(KBigHeader, "27 This header is deliberately more than 256 chars (512 bytes on server side) to check that crazily big headers are handled ok (truncation is fine but crashes aren't!) 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 End\0\0");
	_LIT8(KBigHeader, "27 This header is deliberately more than 256 chars (512 bytes on server side) to check that crazily big headers are handled ok (truncation is fine but crashes aren't!) 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 End");
	//RFileLogger::HexDump(KTag, KNullDesC, EFileLoggingModeAppend, _S16("26 Testing hexdump "), NULL, KBin().Ptr(), KBin().Length());
	clogger.HexDump(_L8("26 Testing hexdump"), KBin);
	//RFileLogger::HexDump(KTag, KNullDesC, EFileLoggingModeAppend, KBigHeader().Ptr(), NULL, KBin().Ptr(), KBin().Length());
	clogger.HexDump(KBigHeader, KBin);

	clogger.SetEnabled(_L("Clogger"), RClogger::EAllEnabled);
	clogger.Log("28 Testing turning on all of clogger's internal logging");
	RClogger longNameClog;
	_LIT(KBigTag, "This tag is 256 chars to check that big tag names are handled ok, especially when the internal logging is turned on. 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 12345 End");
	_LIT(KLongTag, "This tag is more than 256 chars to check that too long tag names are return errors when they should. 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 12345 End");
	err = longNameClog.Connect(KLongTag);
	ASSERT(err);
	longNameClog.Connect(KBigTag);
	longNameClog.SetEnabled(KBigTag, RClogger::EAllEnabled);
	longNameClog.Log("29 Testing logging with bigTag");
	longNameClog.Close();

	clogger.Log("29 There should be an empty line after this log statement\n");
	clogger.SetLogBehaviour(RClogger::ETrimTrailingNewlines);
	clogger.Log("30 There should be no empty line after this statement\r\n");
	clogger.Log("31 Just checking the trim doesn't fall over if there's no newline!");

	delete s;
	}

void DoRDebugTestsL()
	{
	clogger.SetGlobalOptions(RClogger::ERedirectRDebugPrintToClogger);
	RDebug::Printf("30 This should appear in clogger and on RDebug::Print (without a [tag])"); 
	clogger.Log("31 this should not appear on RDebug::Print");
	clogger.SetGlobalOptions(RClogger::ERedirectRDebugPrintToClogger | RClogger::EMirrorToRDebugPrint);
	RDebug::Printf("32 This should appear on RDebug with [tag] and in the clogger log");
	clogger.Log("33 So should this");
	clogger.SetGlobalOptions(0);
	RDebug::Printf("This should appear in the RDebug::Print log (without [tag]) and not in the clogger log");
	User::After(1000); // Give the rdebug log flusher time to run
	}

_LIT(KTag1, "Static     Thread1");
_LIT(KTag2, "Static     Thread2");
_LIT(KTag3, "Static     Thread3");
_LIT(KTag4, "Static     Thread4");
_LIT(KTag5, "Static     Thread5");
_LIT(KThreadName1, "Thread1");
_LIT(KThreadName2, "Thread2");
_LIT(KThreadName3, "Thread3");
_LIT(KThreadName4, "Thread4");
_LIT(KThreadName5, "Thread5");
TInt GoGoGadgetThread(TAny* aTag);

void SoakTestsL(TBool aRedirectRDebug)
	{
	clogger.SetRamBufferSize(512, 2);
	TUint opt = aRedirectRDebug ? RClogger::ERedirectRDebugPrintToClogger : RClogger::EMirrorToRDebugPrint;
	clogger.SetGlobalOptions(RClogger::EBufferLog | opt);

	RThread t[5];
	TRequestStatus stat[5];
	t[0].Create(KThreadName1, &GoGoGadgetThread, 8192, NULL, (TAny*)&KTag1);
	t[1].Create(KThreadName2, &GoGoGadgetThread, 8192, NULL, (TAny*)&KTag2);
	t[2].Create(KThreadName3, &GoGoGadgetThread, 8192, NULL, (TAny*)&KTag3);
	t[3].Create(KThreadName4, &GoGoGadgetThread, 8192, NULL, (TAny*)&KTag4);
	t[4].Create(KThreadName5, &GoGoGadgetThread, 8192, NULL, (TAny*)&KTag5);
	
	for (TInt i = 0; i < 5; i++)
		{
		t[i].Rendezvous(stat[i]);
		t[i].Resume();
		}

	TInt bytes = 0;
	for (TInt i = 0; i < 5; i++)
		{
		User::WaitForRequest(stat[i]);
		bytes += stat[i].Int();
		t[i].Close();
		}

	clogger.Log("Wrote %i bytes", bytes);
	clogger.SetGlobalOptions(0); // Flush log
	}


_LIT8(KLorem, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Proin ornare, odio quis rhoncus tempor, urna dolor pellentesque arcu, vel feugiat eros odio fringilla magna. Phasellus arcu sapien, malesuada blandit, mollis et, sollicitudin id, felis. Nunc pretium quam a tortor. Sed orci aliquam.");

TInt GoGoGadgetThread(TAny* aTag)
	{
	// Each thread should log 10000 bytes in as random a way as possible

	// Use a seed in case we get results we want to replicate
	TInt64 seed = User::FastCounter();

	const TDesC* tag = (const TDesC*)aTag;
	RClogger clogger;
	clogger.Connect(*tag);
	clogger.Log("Connected with random seed %Li", seed);

	RClogger::SetStaticLogBehaviour(RClogger::EUseHeapBuffer);
	SLOG(_L("Static logging started from thread %S"), tag);


	const TInt KNumBytes = 100000;
	TInt numBytes = 0;

	TBool rotated = EFalse;
	while (numBytes < KNumBytes)
		{
		TInt rand = Math::Rand(seed);
		for(TInt i = 0; i < 4; i++)
			{
			TInt whatToDo = rand & 0xFF;
			rand = rand >> 4;

			if (whatToDo < 16)
				{
				User::After(whatToDo);
				}
			else
				{
				TPtrC8 lorem = KLorem().Left(whatToDo-16);
				numBytes += 46 + lorem.Size(); // 46 is the size of the timestamp and static tag,
				if (whatToDo & 0x80)
					{
					SLOG(lorem);
					}
				else
					{
					clogger.Log(lorem);
					}
				}
			}
		if (aTag == &KTag1 && !rotated && numBytes > KNumBytes/2)
			{
			// Thread 1 is responsible for doing a rotate about halfway through
			rotated = ETrue;
			clogger.SetRotateBehaviour(4, RClogger::ECopyRotatedToExternalMedia | RClogger::ECompressRotatedLogs);
			TFileName fn;
			ASSERT(clogger.Rotate(fn) == KErrNone);
			ASSERT(fn.Length());
			//ASSERT(clogger.Rotate() == KErrNotReady); // Because the server should still be busy doing the background compress
			// ^ This assumption is not true now, Rotate is now properly synchronous
			}
		}
	clogger.Close();
	RClogger::StaticClose();
	RThread::Rendezvous(numBytes);
	return KErrNone;
	}

void DoCreateLogsDirTest()
	{
	// Test creating logs folder while server is running
	tclogger.Shutdown();
	TInt err = fs.Rename(_L("C:\\logs"), _L("C:\\logs-disabled"));
	clogger.Connect(KTag);
	clogger.SetGlobalOptions(RClogger::EBufferLog);

	if (err)
		{
		SLOG("Couldn't rename logs dir (err=%i)", err);
		}
	else
		{
		SLOG(_L("21 Started server without logs dir (this will appear in file log because of buffering)"));
		err = fs.Rename(_L("C:\\logs-disabled"), _L("C:\\logs"));
		if (!err)
			{
			SLOG(_L("22 Logs folder back in business"));
			}
		else
			{
			SLOG(_L8("Couldn't rename logs-disabled dir (err=%i)"), err);
			}
		}
	}

void TestPerformanceLoggingL()
	{
	// Can't link directly against both clogger.dll and clogger-buffered.dll, so this is a bit messy
	RLibrary perfLib;
	User::LeaveIfError(perfLib.Load(_L("clogger-buffered.dll")));

	#ifdef __WINS__
		#define KConnectOrdinal 42
		#define KSlogOrdinal 34
		#define KCloseOrdinal 41
		#define KStaticHexDumpOrdinal 50
	#else
		#define KConnectOrdinal 4
		#define KSlogOrdinal 20
		#define KCloseOrdinal 2
		#define KStaticHexDumpOrdinal 49
	#endif

	typedef TInt (*StaticConnectFn)(const TDesC&);
	StaticConnectFn StaticConnect = (StaticConnectFn)perfLib.Lookup(KConnectOrdinal);
	typedef void (*SlogFn)(TRefByValue<const TDesC8> aFmt, ...);
	SlogFn Slog = (SlogFn)perfLib.Lookup(KSlogOrdinal);
	typedef void (*StaticCloseFn)();
	StaticCloseFn StaticClose = (StaticCloseFn)perfLib.Lookup(KCloseOrdinal);
	typedef void (*StaticHexDumpFn)(const TDesC8& aHeader, const TDesC8& aData);
	StaticHexDumpFn StaticHexDump = (StaticHexDumpFn)perfLib.Lookup(KStaticHexDumpOrdinal);

	TInt err = StaticConnect(_L("PerfLog"));
	ASSERT(!err);

	Slog(_L8("This better be some %s logging"), "high performance");
	User::After(10);
	Slog(_L8("So had this"));

	_LIT8(KBin, "Newline: \n Null: xxxxx\0 <EOM>"); // Useful to put a zero byte in there to test that string termination isn't an issue
	StaticHexDump(_L8("Testing static hexdump function "), KBin);

	clogger.Log("This should appear before the high-perf logging");
	clogger.HexDump(_L8("Normal "), KBin);

	StaticClose();
	perfLib.Close();
	}

void DebugRouterL()
	{
	// Test the debug router LDD directly, to ensure its buffering code works properly
	tclogger.Shutdown(); // Otherwise the cloggerserver is still registered as the debug router's client (And only one client is allowed)
	RCloggerDebugRouter router;
	TInt err = RCloggerDebugRouter::LoadDriver();
	ASSERT(!err || err == KErrAlreadyExists);
	err = router.Open();
	ASSERT(!err);
	RChunk chunk;
	err = router.OpenChunk(chunk);
	ASSERT(!err);
	SDebugChunkHeader* chunkHeader = (SDebugChunkHeader*)chunk.Base();
#define startPtr chunkHeader->iStartOffset
#define endPtr chunkHeader->iEndOffset
#define overflows chunkHeader->iOverflows

	err = router.EnableDebugRouting(RCloggerDebugRouter::EEnableRoutingAndConsume);
	ASSERT(!err);

	// Test queuing up a bunch of prints before the client has registered
	const char* lorem = (const char*)KLorem().Ptr();
	const TInt KLoremLength = Min(256, KLorem().Length()); // RDebugs are truncated at 256 chars
	RDebug::Printf(lorem);
	RDebug::Printf(lorem);
	RDebug::Printf(lorem);
	RDebug::Printf(lorem);
	RDebug::Printf(lorem);

	TRequestStatus stat;
	router.ReceiveData(stat);
	User::WaitForRequest(stat);
	ASSERT(stat == KErrNone);
	ASSERT(startPtr == sizeof(SDebugChunkHeader)); // Offset from start of chunk
	TInt dataLen = endPtr - startPtr;
	ASSERT(dataLen == (KLoremLength + sizeof(SCloggerTraceInfo)) * 5);
	SCloggerTraceInfo* traceHeader = (SCloggerTraceInfo*)(chunk.Base() + startPtr);
	ASSERT(traceHeader->iTraceType == 'U');

	// Now do another write, which will go to the middle of the buffer as we haven't said we've handled all the KLorems yet
	_LIT(KMiddle, "Middle of buffer");
	RDebug::Print(KMiddle);

	router.ReceiveData(stat);
	User::WaitForRequest(stat);
	ASSERT(stat == KErrNone);
	ASSERT(startPtr == sizeof(SDebugChunkHeader) + dataLen);
	ASSERT(endPtr - startPtr == KMiddle().Length() + sizeof(SCloggerTraceInfo));

	// iCurrent and iClientEnd are pointing at the same place, about 1300 bytes in
	// iClientStart is at start of buffer

	// Now test wrapping
	// Need to do 250 writes of KLorem to get to the end of the buffer, 64K/(256+12) ~= 250
	// The buffer should wrap around, then overflow as we still haven't acknowledged we've completed up to 1300 bytes

	TInt loop = 265; // So we put something at the start too
	while (loop--)
		{
		RDebug::Printf(lorem);
		}

	TInt prevEnd = endPtr;
	router.ReceiveData(stat);
	User::WaitForRequest(stat);
	ASSERT(stat == KErrNone);
	ASSERT(startPtr == prevEnd);
	ASSERT(overflows == 21); // 21 is what you get from doing 265 writes
	//ASSERT(endPtr == ??); // Can't be bothered right now to figure this out

	// Simulate unclean shutdown (ie don't call EnabledDebugRouting(EFalse))

	router.Close();
	chunk.Close();

	// Try writing enough to wrap the buffer after we've disconnected (ie while there's no client)
	loop = 265;
	while (loop--)
		{
		RDebug::Printf(lorem);
		}

	// And reconnect and see what happens
	err = router.Open();
	ASSERT(!err);
	err = router.OpenChunk(chunk);
	ASSERT(!err);
	chunkHeader = (SDebugChunkHeader*)chunk.Base();

	err = router.EnableDebugRouting(RCloggerDebugRouter::EEnableRoutingAndConsume);
	ASSERT(!err);

	router.ReceiveData(stat);
	User::WaitForRequest(stat);
	ASSERT(stat == KErrNone);
	//ASSERT(startPtr == sizeof(SDebugChunkHeader); // Offset from start of chunk
	traceHeader = (SCloggerTraceInfo*)(chunk.Base() + startPtr);
	ASSERT(traceHeader->iTraceType == 'U');

	////END


	// Done!
	err = router.EnableDebugRouting(RCloggerDebugRouter::EDisable);
	ASSERT(!err);
	router.Close();
	chunk.Close();
	
	// Now test the CreateChunk API
	err = router.Open();
	ASSERT(!err);
	RThread meThread;
	err = meThread.Open(RThread().Id());
	ASSERT(!err);
	SCreateChunkParams params;
	params.iCommittedSize = 1024;
	params.iMaxSize = 1024;
	params.iHandleOfOtherThread = meThread.Handle();
	err = router.CreateChunk(params);
	ASSERT(err == KErrNone);
	RChunk myChunk, otherChunk;
	myChunk.SetHandle(params.iChunkHandle);
	otherChunk.SetHandle(params.iOtherThreadChunkHandle);
	ASSERT(myChunk.Size() >= 1024);
	ASSERT(otherChunk.Size() >= 1024);
	myChunk.Close();
	ASSERT(otherChunk.Size()); // Testing that the one handle stays valid when the other closes
	otherChunk.Close();
	meThread.Close();
	// Try again with nothing specified for otherChunk
	params.iHandleOfOtherThread = 0;
	params.iOtherThreadChunkHandle = 0;
	err = router.CreateChunk(params);
	ASSERT(!err);
	ASSERT(params.iOtherThreadChunkHandle == 0);
	myChunk.SetHandle(params.iChunkHandle);
	ASSERT(myChunk.Size() >= 1024);

	// Try the resize API
	err = router.AdjustChunk(myChunk, 512);
	ASSERT(!err);
	// We can't actually shrink a shared chunk, so this test is not valid.
	//ASSERT(myChunk.Size() == 512);
	myChunk.Close();
	router.Close();
	
	// Restart clogger
	clogger.Close();
	err = clogger.Connect(KTag);
	ASSERT(!err);
	}

static void JustPanic(TAny*)
	{
	User::Panic(_L("Crash!"), 0);
	}

static void CleanupPanicPushL()
	{
	CleanupStack::PushL(TCleanupItem(&JustPanic));
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r, CleanupPanicPushL(); RunTestsL(); CleanupStack::Pop());
		//ASSERT(!r);
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	ASSERT(!r);
	return r;
	}

void SessionWriterL()
	{
	//tclogger.Shutdown();
	//clogger.Connect(KTag);
	clogger.SetGlobalOptions(RClogger::ERedirectRDebugPrintToClogger);
	clogger.SetEnabled(_L("Clogger"), 0); // To shut up the "thread x has started debugging" messages from using the debugrouter
	
	RCloggerLogConsumer consumer;
	TInt err = consumer.Connect();
	ASSERT(!err);

	TRequestStatus stat;
	TPtrC8 logLine;

	consumer.GetNextLog(stat, logLine);
	RDebug::Printf("Shouldn't see this as will cancel session writer!"); // Don't use RClogger::Log directly as this will deadlock. Using the debug router to sidestep this is easier than spawning a separate thread in this test app
	consumer.CancelGetNextLog();
	User::WaitForRequest(stat);

	consumer.GetNextLog(stat, logLine);
	_LIT8(KSee, "Better see this\r\n");
	RDebug::Printf("Better see this");
	User::WaitForRequest(stat);
	ASSERT(stat == KErrNone);
	ASSERT(logLine.Right(KSee().Length()) == KSee);

	consumer.Close();
	}
