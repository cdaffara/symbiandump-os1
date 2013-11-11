// topfshell.cpp
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

#include <e32hashtab.h>
#include <fshell/ioutils.h>
#include <fshell/memoryaccesscmd.h>
#include "sampler.h"

using namespace IoUtils;

class CCmdTop : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTop();
private:
	CCmdTop();
	static TInt TimerCallback(TAny* aSelf);
	void TimerCallback();
	void UpdateL();
	struct SThreadData;
	static TInt SortReverse(const CCmdTop::SThreadData& aLeft, const CCmdTop::SThreadData& aRight);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RBuf8 iReadBuf;
	struct SThreadData
		{
		TUint iId;
		TInt iNumSamples;
		TFullName iName; // Saves recalculating every time
		};
	RHashMap<TUint, SThreadData*> iSeenThreads; // Saves linear searching through iThreads for each sample (Maps thread id to SThreadData
	RPointerArray<SThreadData> iThreads;
	RSampler iSampler;
	CPeriodic* iTimer;
	TInt iRate;

	CTextBuffer* iBuffer;
	CTextFormatter* iFormatter;
	TInt iNumConsoleLines;
	TInt iNumLinesInLastUpdate;
	};

CCommandBase* CCmdTop::NewLC()
	{
	CCmdTop* self = new(ELeave) CCmdTop();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTop::~CCmdTop()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	if (iSampler.Handle())
		{
		iSampler.Stop();
		iSampler.Close();
		User::FreeLogicalDevice(KSamplerName);
		}
	delete iBuffer;
	delete iFormatter;
	iReadBuf.Close();
	iThreads.ResetAndDestroy();
	iSeenThreads.Close();
	}

CCmdTop::CCmdTop()
	: CMemoryAccessCommandBase(EManualComplete), iRate(1000)
	{
	}

const TDesC& CCmdTop::Name() const
	{
	_LIT(KName, "top");	
	return KName;
	}

void CCmdTop::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CCmdTop::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iRate, _L("rate"));
	}

EXE_BOILER_PLATE(CCmdTop)

void CCmdTop::DoRunL()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();
#endif

	TInt err = User::LoadLogicalDevice(KSamplerName);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		LeaveIfErr(err, _L("Couldn't load sampler ldd %S"), &KSamplerName);
		}
	iReadBuf.CreateL(iRate * 2 * sizeof(TUint32)); // go twice as big as needed just to be safe, allows for AO not getting to run on time
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iTimer->Start(iRate*1000, iRate*1000, TCallBack(&CCmdTop::TimerCallback, this));

	LeaveIfErr(iSampler.Open(), _L("Couldn't open sampler"));
	iSampler.Reset(ETrue); // Parameter is ignored
	iSampler.Start(1000); // Always tell sampler to sample every millisecond, regardless of the frequency we update the UI

	TSize size;
	User::LeaveIfError(Stdout().GetScreenSize(size));
	iNumConsoleLines = size.iHeight;
	iBuffer = CTextBuffer::NewL(512);
	iFormatter = CTextFormatter::NewL(size.iWidth);
	Stdout().ClearScreen();
	UpdateL(); // Just so we display something on screen before the first update
	}

TInt CCmdTop::TimerCallback(TAny* aSelf)
	{
	static_cast<CCmdTop*>(aSelf)->TimerCallback();
	return 0;
	}

void CCmdTop::TimerCallback()
	{
	iReadBuf.Zero();
	TRequestStatus stat;
	iSampler.Read(iReadBuf, stat);
	User::WaitForRequest(stat); // API used to be async, it's now sync but I haven't changed the interface
	
	// First off clear all the iNumSamples (as that was for the previous sampling period)
	for (TInt i = 0; i < iThreads.Count(); i++)
		{
		iThreads[i]->iNumSamples = 0;
		}

	TInt numSamples = iReadBuf.Length() / sizeof(TUint32);
	TUint32* buf = (TUint32*)iReadBuf.Ptr();
	for (TInt i = 0; i < numSamples; i++)
		{
		TUint32 threadId = buf[i];
		SThreadData** thread = iSeenThreads.Find(threadId);
		if (thread)
			{
			(*thread)->iNumSamples++;
			}
		else
			{
			// New thread id
			SThreadData* thread = new SThreadData;
			if (!thread) continue;
			TInt err = iThreads.Append(thread);
			if (err)
				{
				delete thread;
				continue;
				}
			err = iSeenThreads.Insert(threadId, thread);
			if (err)
				{
				iThreads.Remove(iThreads.Count()-1);
				delete thread;
				continue;
				}
			thread->iNumSamples = 1;
			thread->iId = threadId;
			RThread rthread;
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			err = iMemAccess.RThreadForceOpen(rthread, threadId);
#else
			err = rthread.Open(TThreadId(threadId));
#endif
			if (err == KErrNone)
				{
				thread->iName = rthread.FullName();
				//PrettyName(EListThread, thread->iName);
				rthread.Close();
				}
			else
				{
				thread->iName = _L("Tid: ");
				thread->iName.AppendNum(threadId);
				}
			}
		}
	TRAP_IGNORE(UpdateL());
	}

TInt CCmdTop::SortReverse(const CCmdTop::SThreadData& aLeft, const CCmdTop::SThreadData& aRight)
	{
	// This sorts largest number of samples first 
	TInt res = - (aLeft.iNumSamples - aRight.iNumSamples);
	if (res == 0) return aLeft.iId - aRight.iId; // make sure there is a total ordering in event of tie break
	return res;
	}

void CCmdTop::UpdateL()
	{
	// First, sort iThreads
	iThreads.Sort(TLinearOrder<SThreadData>(&SortReverse));

	TInt numSamples = iReadBuf.Size() / sizeof(TUint32);
	
	iBuffer->Zero();
	iFormatter->Zero();
	iBuffer->AppendL(_L("Tid\tThread name\tCPU usage\r\n"));

	for (TInt i = 0; i < iThreads.Count() && i < iNumConsoleLines-2; i++) // minus one for title, one for last line of screen which I can't seem to figure out how to make use of
		{
		SThreadData& thread = *iThreads[i];
		TReal percent = 100 * thread.iNumSamples / (TReal)numSamples;
		iBuffer->AppendFormatL(_L("%d\t%S\t%00.2f%%\r\n"), thread.iId, &thread.iName, percent);
		}
	Stdout().SetCursorHeight(0);
	iFormatter->TabulateL(0, 1, iBuffer->Descriptor(), ETruncateLongestColumn);
	User::LeaveIfError(Stdout().SetCursorPosAbs(TPoint(0, 0)));
	Stdout().Write(iFormatter->Descriptor());
	TInt numLines = 1 + iThreads.Count(); // plus 1 for the title line
	TInt numOldLines = iNumLinesInLastUpdate - numLines;
	while (numOldLines > 0)
		{
		Stdout().ClearToEndOfLine();
		Stdout().SetCursorPosRel(TPoint(0, 1));
		--numOldLines;
		}
	iNumLinesInLastUpdate = numLines;
	}
