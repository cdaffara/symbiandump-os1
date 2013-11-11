// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\timer.cpp
// 
//

#include "variant.h"

#define KWinsTimerPanicCategory "WinsTimer"

enum TWinsTimerPanic
	{
	EReleaseSemaphoreError = 1,
	ETimeSetEventError = 2
	};

void PanicFromWinsTimer(TWinsTimerPanic aReason)
	{
	Kern::Fault(KWinsTimerPanicCategory, aReason);
	}



inline Int64 FileTime2Milliseconds(const FILETIME& aFileTime)
	{return ((Int64(aFileTime.dwHighDateTime)<<32) + aFileTime.dwLowDateTime)/10000;}


void CALLBACK WinsTimer::Tick(UINT, UINT, DWORD aPtr, DWORD, DWORD) 
	{

	WinsTimer& timer=*(WinsTimer*)aPtr;
	if (!ReleaseSemaphore(timer.iSemaphore,1,NULL))
		{
		__ASSERT_ALWAYS((GetLastError() == ERROR_TOO_MANY_POSTS), PanicFromWinsTimer(EReleaseSemaphoreError));
		timeKillEvent(timer.iTimer);
		// If WinsTimer::EventThread() has run since the above call to
		// ReleaseSemaphore() (this has been observed to happen while 
		// within the call to timeKillEvent()) then the semaphore may be 
		// signallable again. If it is, restart the timer otherwise wait 
		// for the event thread to restart it.
		if (ReleaseSemaphore(timer.iSemaphore,1,NULL))
			timer.Enable();
		else
			timer.iSuspend = timer.iMaxLagTicks;
		}
	}

DWORD WINAPI WinsTimer::Thread(LPVOID aPtr)
	{
	static_cast<WinsTimer*>(aPtr)->EventThread(*(NTimerQ*)NTimerQ::TimerAddress());
	return 0;
	}

void WinsTimer::EventThread(NTimerQ& aTimerQ)
	{
	for (;;)
		{
		WaitForSingleObject(iSemaphore,INFINITE);
		if (iSuspend > 0)
			{
			// Emulator interrupted/suspended for too long, MM-callback has been suspended
			//
			// Rather than try and catch up now, just discard the lost ticks - this improves
			// system behaviour particularly when debugging at the expense of losing sync
			// between the Windows clock and the emulator RTC.
			//
			while (--iSuspend > 0)
				WaitForSingleObject(iSemaphore, 0);	// absorb all signals
			//
			// un-nobble once we are debugging
			if (iNobbleNanos && IsDebuggerPresent())
				iNobbleNanos = 0;
			//
			// now restart the timer callbacks
			Enable();
			//
			// don't deliver a tick until next callback
			continue;
			}
		if (iNobbleNanos && iIdleThread != NKern::CurrentThread())
			Kern::NanoWait(iNobbleNanos);
		StartOfInterrupt();
		iTime += iPeriod;
		if (!iStandby)
			aTimerQ.Tick();
		EndOfInterrupt();
		}
	}


WinsTimer::WinsTimer()
	:iPeriod(0),iNobbleNanos(0),iMaxLagTicks(0),iSemaphore(NULL),iSuspend(0),iTime(0),iIdleThread(0)
	{}

void WinsTimer::Init(TUint aPeriod)
	{
	// calculate the y2k offset in seconds from Win32 'zero' FILETIME
	// This initially synchronizes EPOC time with Windows time
	const SYSTEMTIME KSystemTimeY2K = {2000,1,0,1,0,0,0,0};
	FILETIME y2k, now;
	SystemTimeToFileTime(&KSystemTimeY2K,&y2k);
	GetSystemTimeAsFileTime(&now);
	iTime = FileTime2Milliseconds(now) - FileTime2Milliseconds(y2k);

	TIMECAPS caps;
	timeGetDevCaps(&caps,sizeof(caps));

	iPeriod = min(caps.wPeriodMax, max(caps.wPeriodMin, aPeriod));
	TUint resolution = max(caps.wPeriodMin, iPeriod/2);
	iMaxLagTicks = EMaxLag / iPeriod;

	// limit 'catch-up' for when Win32 gets too busy for us to fire timer events
	// making this too large causes delays when resuming from debug
	iSemaphore = CreateSemaphoreA(NULL, 0, iMaxLagTicks, NULL);

	timeBeginPeriod(resolution);

	CreateWin32Thread(EThreadEvent, &WinsTimer::Thread, this, ETrue);
	}

void WinsTimer::Enable()
	{
	iTimer = timeSetEvent(iPeriod,0,&WinsTimer::Tick,DWORD(this),TIME_PERIODIC);
	__ASSERT_ALWAYS(iTimer != NULL, PanicFromWinsTimer(ETimeSetEventError));
	}

TInt WinsTimer::SystemTime() const
//
// Return the time in seconds since y2k
//
	{
	TInt irq = NKern::DisableAllInterrupts();
	Int64 time = iTime;
	NKern::RestoreInterrupts(irq);
	if (time < 0)
		time -= 999;	// we want rounding to -infinity for the division
	return TInt(time/1000);
	}

void WinsTimer::SetSystemTime(TInt aTime)
//
// Set the time in seconds since y2k
//
	{
	Int64 time=aTime;
	time*=1000;
	TInt irq = NKern::DisableAllInterrupts();
	iTime = time;
	NKern::RestoreInterrupts(irq);
	}

void WinsTimer::Standby()
	{
	iStandby = ETrue;
	}

void WinsTimer::Wakeup()
	{
	iStandby = EFalse;
	// Busy wait for the next timer interrupt 
	volatile Int64* t = &iTime;
	Int64 time = *t;
	while (time == *t)
		{}
	}