// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology
*/

// From CFLOG's perspective it's always included; other components choose this by whether
// they #include "cflog.mmh" AND Flogger is active
#define __CFLOG_INCLUDED

#include "cflog.h"
#include <cfshared.h>
#include <e32debug.h>


#if defined(__CFLOG_ACTIVE)

#if defined(__WINS__)
#include <emulator.h>
#endif

const TInt KDefaultConnectionArrayLength = 64;

class TCFLogTags
	{
public:
	TCFLogTags(const TBufC8<KMaxTagLength>& aComponent,
			   const TBufC8<KMaxTagLength>& aSubSystem);
	const TBufC8<KMaxTagLength>& Component() const;
	const TBufC8<KMaxTagLength>& SubSystem() const;

	// for find method
	static TBool Matches(const TCFLogTags& aTags1, const TCFLogTags& aTags2);

private:
	TBufC8<KMaxTagLength> iComponent;
	TBufC8<KMaxTagLength> iSubSystem;
	};


NONSHARABLE_CLASS(CCFLog) : public CBase
/** Interface class implementing CFlogger and used by the CFlogger 
interface class @see CCFlogIf
@internalTechnology */
	{
private:
	CCFLog();
	void ConstructL();
public:
	~CCFLog();
 	static CCFLog *NewL();
	
	TInt DoConnect();
	TInt DoClose();

	// 
	void DoWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText);
	void DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, ...);
	void DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, VA_LIST& aList);
	void DoWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText);
	void DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, ...);
	void DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList);
	void DoHexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen);
	void DoWriteBinary(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData);

	typedef RFastLock RCFLockType;
	RCFLockType& CsLock();
	TInt UserCount();

private:
	TInt GetFloggerPtr(const TDesC8& aSubsystem, const TDesC8& aComponent, RFileLogger*& aFlogger);

private:
	typedef RAllocator RCFAllocator;
	RCFAllocator* iProcessHeap;
	RCFLockType iCsLock;
	TInt iUserCount;
	RArray<TCFLogTags> iTags;
	RArray<RFileLogger> iFloggers;

#if defined(__WINS__)
	class TCFWin32Threads
		{
	public:
		TCFWin32Threads() : iThreads(KDefaultConnectionArrayLength, 0)
			{ }
		~TCFWin32Threads()
			{
			iThreads.Close();
			}
			TBool CheckWin32Thread(TUint32& aWin32Thread, RCFAllocator* aProcessHeap);
	private:
		RArray<TUint32> iThreads;
		};

	TCFWin32Threads iWin32Threads;

#endif
	};
	
#if defined(__WINS__)
TBool CCFLog::TCFWin32Threads::CheckWin32Thread(TUint32& aWin32Thread, RCFAllocator* aProcessHeap)
	{
	TBool result = EFalse;
	RCFAllocator* threadHeap = NULL;
	
	Emulator::Lock();
	aWin32Thread = ::GetCurrentThreadId();
	Emulator::Unlock();
	if(iThreads.Find(aWin32Thread) == KErrNotFound)
		{
		threadHeap = User::SwitchHeap(aProcessHeap);
		if(iThreads.Append(aWin32Thread) == KErrNone)
			{
			result = ETrue;
			}
		User::SwitchHeap(threadHeap);
		}
	return result;
	}

#endif

EXPORT_C CCFLogIf* CCFLogIf::NewL()
/** Creates new CCFLogIf object
@return CCFLog* pointer to the new CCFLog object.
@leave KErrNoMemory 
@internalTechnology */
	{
	CCFLogIf* This = new(ELeave) CCFLogIf;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop(This);
	return This;
	}

EXPORT_C TInt CCFLogIf::Delete(CCFLogIf* aCFLogIf)
/** Deletes a CCFLogIf object
@return result KErrNone or KErrInUse
@internalTechnology */
	{
	TInt result = KErrNone;
	aCFLogIf->CFLog()->CsLock().Wait();
	TInt count = aCFLogIf->CFLog()->UserCount();
	aCFLogIf->CFLog()->CsLock().Signal();

	if (count > 0)
		{
		__FLOG_STMT(TBuf8<KMaxTagLength> tag);
		__FLOG_STMT(TBuf8<KMaxTagLength> subtag);
		__FLOG_STMT(tag.Copy(KLogCommsFw));
		__FLOG_STMT(subtag.Copy(KLogWarning));
		__FLOG_STATIC1(tag, subtag, 
			_L8("CCFLogIf::Delete **WARNING** there were still %d cflog sessions open"),
			count);
		result = KErrInUse;
		}
	delete aCFLogIf;
	return result;
	}

void CCFLogIf::ConstructL()
/** Second level constructor
@leave KErrNoMemory
@internalTechnology */
	{
	User::LeaveIfError(Dll::SetTls(this));
	iCFLog = CCFLog::NewL();
	}

CCFLogIf::CCFLogIf() :
	iCFLog(0)
	{
	}

CCFLogIf::~CCFLogIf()
	{
	Dll::SetTls(NULL);
	delete iCFLog;
	}

EXPORT_C void CCFLogIf::Panic(TCFLogPanic aPanic)
/** Panic function
@panic 
@internalTechnology */
	{
	_LIT(tcflogger, "TCFLogger");
	User::Panic(tcflogger, aPanic);	
	}

CCFLogIf* CheckContext()
/** Function added to prevent CFLOG from panicking when Write functions are called
and CFLOGGER is not installed (Other functions than Write are currently not affected).
This is currently the case for Multimedia host which uses CommsFw but not CFLOG.
Worth noting that this fix is temporary until CFLOGGER uses Utrace ;-) 
@internalTechnology */
	{
	return static_cast<CCFLogIf*>(Dll::Tls());
	}

EXPORT_C CCFLogIf* CCFLogIf::Context()
/** Used to reference the single process-wide CCFLogIf from different 
threads+DLLs. Don't try to inline these - if TLS is in use then they 
need to be in this DLL 
@internalTechnology */
	{ 
	CCFLogIf* pCFLog = static_cast<CCFLogIf*>(Dll::Tls());
	
	// Check to make sure the TLS has been initialised correctly.
	if( !pCFLog )
		{
		RDebug::Print( _L( "CCFLogIf::Context - the log interface was not set in the TLS.  This normally means that the logging version of cflog.dll has been mixed with a non-logging version of commsfw.dll/c32rootsrv.dll or logging calls have been made from a thread which has not called CCFLogIf::SetContext.  See CommsDebugUtility How-To Document FAQ section for details on enabling logging in a release build." ) );

		Panic( ECFNoLogger );
		}
		
	return pCFLog; 
	}

EXPORT_C void CCFLogIf::SetContext()
/** A new thread/DLL needs to call this when the TLS handling is in use,
ie in the thread startup function, having obtained a pointer to the 
CCFLogIf somehow (eg caller passed in). Consider it as the 
thread/dll introducing itself to the CFLogIf. "Pleased to meet you..." 
@internalTechnology */
	{ 
	Dll::SetTls(this);
	}

EXPORT_C void CCFLogIf::Connect()
/** Create a new connection cflogger.
@internalTechnology */
	{
	CCFLogIf::Context()->iCFLog->DoConnect();
	}
	
EXPORT_C void CCFLogIf::Close()
/** Close an existing connection to cflogger
@internalTechnology */
	{
	CCFLogIf::Context()->iCFLog->DoClose();
	}
	
EXPORT_C void CCFLogIf::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		context->iCFLog->DoWrite(aSubsystem, aComponent, aText);
		}
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, ...)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		context->iCFLog->DoWriteFormat(aSubsystem, aComponent, aFmt, list);
		}
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		context->iCFLog->DoWriteFormat(aSubsystem, aComponent, aFmt, aList);
		}
	}
	
EXPORT_C void CCFLogIf::Write(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		context->iCFLog->DoWrite(aSubsystem, aComponent, aText);
		}
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt,...)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		VA_LIST list;
		VA_START(list, aFmt);
		context->iCFLog->DoWriteFormat(aSubsystem, aComponent, aFmt, list);
		}
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{	
		context->iCFLog->DoWriteFormat(aSubsystem, aComponent, aFmt, aList);
		}
	}
	
EXPORT_C void CCFLogIf::WriteBinary(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	// please see CheckContext() comment
	CCFLogIf* context = CheckContext();
	if (context)
		{
		context->iCFLog->DoWriteBinary(aSubsystem, aComponent, aData);
		}
	}
	
EXPORT_C void CCFLogIf::HexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
/** Generates a log line and sends it to flogger.
@internalTechnology */
	{
	CCFLogIf::Context()->iCFLog->DoHexDump(aSubsystem, aComponent, aHeader, aMargin, aPtr, aLen);
	}

CCFLog* CCFLogIf::CFLog()
	{
	return iCFLog;
	}

//--------------------
TCFLogTags::TCFLogTags(const TBufC8<KMaxTagLength>& aComponent,
					   const TBufC8<KMaxTagLength>& aSubSystem) :
	iComponent(aComponent),
	iSubSystem(aSubSystem)
	{
	}

const TBufC8<KMaxTagLength>& TCFLogTags::Component() const 
	{ 
	return iComponent;
	}

const TBufC8<KMaxTagLength>& TCFLogTags::SubSystem() const
	{
	return iSubSystem;
	}

TBool TCFLogTags::Matches(const TCFLogTags& aTags1, const TCFLogTags& aTags2)
	{
	return aTags1.Component().Compare(aTags2.Component()) == 0 &&
	   aTags1.SubSystem().Compare(aTags2.SubSystem()) == 0;
	}


//--------------------
CCFLog::CCFLog() :
	iUserCount(0),
	iTags(KDefaultConnectionArrayLength),
	iFloggers(KDefaultConnectionArrayLength)
#ifdef __WIN32__
#endif
	{
	iProcessHeap = &User::Heap();
	}

void CCFLog::ConstructL()
	{
	User::LeaveIfError(iCsLock.CreateLocal());

	// construct initial cflogger will be first in session arrays
	RFileLogger* defaultflogger;
	User::LeaveIfError(GetFloggerPtr(KLogCommsFw, KCFLogOom, defaultflogger));
	}

CCFLog::~CCFLog()
	{
	iTags.Close();
	TInt i;
	for (i=0; i<iFloggers.Count(); i++)
		{
		iFloggers[i].Close();
		}
	iFloggers.Close();
	iCsLock.Close();
	}

CCFLog* CCFLog::NewL()
	{	
	CCFLog* This = new(ELeave) CCFLog;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop(This);
	return This;
	}

TInt CCFLog::GetFloggerPtr(const TDesC8& aSubsystem, const TDesC8& aComponent, RFileLogger*& aFlogger)
	{
	aFlogger = NULL;	// in case caller neglects error codes
	TCFLogTags tags(aSubsystem, aComponent);
	TInt error = KErrNone;

	TInt index = iTags.Find(tags, TCFLogTags::Matches);
	if (KErrNotFound == index)
		{
		// start CS
		iCsLock.Wait();

		// we must alloc stuff from our creator thread, otherwise if a thread
		// exits, other threads will crash when they try to use the connection
		RCFAllocator* threadHeap = User::SwitchHeap(iProcessHeap);

		// create a new flogger and connect it using subsys and component tags
		RFileLogger flogger;
		error = flogger.Connect();
		if (error == KErrNone)
			{
			error = iFloggers.Append(flogger);
			
			if (error == KErrNone)
				{
				index = iFloggers.Count()-1;
				error = iFloggers[index].Share();
				if (error == KErrNone)
					{
					error = iFloggers[index].SetLogTags(aSubsystem, aComponent);
					if (error == KErrNone)
						{
						error = iTags.Append(tags);
						}
					}

				// Upon error close & bin any new RFileLogger
				if(error != KErrNone)
					{
					iFloggers[index].Close();
					iFloggers.Remove(index);
					index = KErrNotFound;
					}
				}
			else
				{
				flogger.Close();
				}
			}
		// put heap back
		User::SwitchHeap(threadHeap);
		// end CS
		iCsLock.Signal();
		}
	if(index != KErrNotFound)
		{
		aFlogger = &iFloggers[index];
#if defined(__WINS__)
		TUint32 win32Thread;
		if(iWin32Threads.CheckWin32Thread(win32Thread, iProcessHeap))
			{
			_LIT8(KDebugExtra, "DebugExtra");
			_LIT8(KDebugThread, "Thread");
			// This recurses, but safely as we've already added the thread so won't log again
			RThread epocThread;
			TFullName fullName = epocThread.FullName();
			TBuf8<sizeof(TFullName) / sizeof(TText)> printableName;
			printableName.Copy(fullName);
			DoWriteFormat(KDebugExtra, KDebugThread, _L8("Win32 thread 0x%x (%d) (%S) "), win32Thread, win32Thread, &printableName);
			}
#endif
		}
	return error;
	}

TInt CCFLog::DoConnect()
	{
	return User::LockedInc(iUserCount) + 1;
	}

TInt CCFLog::DoClose()
	{
	__ASSERT_ALWAYS(iUserCount>0, CCFLogIf::Panic(ECFUserCountInvalid));
	return User::LockedDec(iUserCount) - 1;
	}

void CCFLog::DoWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC16& aText)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->Write(aText);
		}
	}

void CCFLog::DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->WriteFormat(aFmt, list);
		}
	}

void CCFLog::DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC16> aFmt, VA_LIST& aList)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->WriteFormat(aFmt, aList);
		}
	}

void CCFLog::DoWrite(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aText)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->Write(aText);
		}
	}

void CCFLog::DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list, aFmt);

	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->WriteFormat(aFmt, list);
		}
	}

void CCFLog::DoWriteFormat(const TDesC8& aSubsystem, const TDesC8& aComponent, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->WriteFormat(aFmt, aList);
		}
	}

void CCFLog::DoWriteBinary(const TDesC8& aSubsystem, const TDesC8& aComponent, const TDesC8& aData)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->WriteBinary(aData);
		}
	}

void CCFLog::DoHexDump(const TDesC8& aSubsystem, const TDesC8& aComponent, const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen)
	{
	RFileLogger *flogger;
	if(GetFloggerPtr(aSubsystem, aComponent, flogger) == KErrNone)
		{
		flogger->HexDump(aHeader, aMargin, aPtr, aLen);
		}
	}

TInt CCFLog::UserCount()
	{
	return iUserCount;
	}

CCFLog::RCFLockType& CCFLog::CsLock()
	{
	return iCsLock;		//lint !e1536
	}

#else	 // defined(__CFLOG_ACTIVE)

EXPORT_C CCFLogIf* CCFLogIf::NewL()
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	return NULL;
	}

EXPORT_C TInt CCFLogIf::Delete(CCFLogIf*)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	return 0;
	}

//EXPORT_C void CCFLogIf::Panic(TCFLogPanic aPanic)
EXPORT_C void CCFLogIf::Panic(TCFLogPanic)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	_LIT(tcflogger, "TCFLogger");
//	User::Panic(tcflogger, aPanic);	
	}

	
EXPORT_C CCFLogIf* CCFLogIf::Context()
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{ 
//	Panic(ECFNoLogger);
	return NULL;
	}

EXPORT_C void CCFLogIf::SetContext()
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{ 
//	Panic(ECFNoLogger);
	}

EXPORT_C void CCFLogIf::Connect()
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::Close()
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::Write(const TDesC8&, const TDesC8&, const TDesC16&)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8&, const TDesC8&, TRefByValue<const TDesC16>, ...)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8&, const TDesC8&, TRefByValue<const TDesC16>, VA_LIST&)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::Write(const TDesC8&, const TDesC8&, const TDesC8&)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8&, const TDesC8&, TRefByValue<const TDesC8>, ...)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::WriteFormat(const TDesC8&, const TDesC8&, TRefByValue<const TDesC8>, VA_LIST&)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::WriteBinary(const TDesC8&, const TDesC8&, const TDesC8&)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}
	
EXPORT_C void CCFLogIf::HexDump(const TDesC8&, const TDesC8&, const TText*, const TText*, const TUint8*, TInt)
/** Empty implementation to keep exports for all builds
@panic ECFNoLogger if there is no flogger present
@return NULL
@internalTechnology */
	{
//	Panic(ECFNoLogger);
	}


#endif	// defined(__CFLOG_ACTIVE)


