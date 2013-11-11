// sampler.cpp
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
#include "sampler.h"
#include <fshell/btrace_parser_defs.h>
#include <fshell/common.mmh>

// Please note this code is NOT SMP-safe. The whole mechanism of using an interrupt doesn't work under SMP so there's no point adding TSpinLocks or similar
#ifdef __SMP__
#error "Sampler doesn't work under SMP!"
#endif

TUint IntStackPtr();
TUint IDFCRunning();

DCpuSampler::DCpuSampler()
	: iTimer(NTimer(&Sample, this))
	{
#if defined(__MARM__)
	iIntStackTop=(TUint*)IntStackPtr();
#endif
	}

DCpuSampler::~DCpuSampler()
	{
	iTimer.Cancel();
	}

void DCpuSampler::Sample(TAny* aPtr)
	{
	static_cast<DCpuSampler*>(aPtr)->DoSample();
	}

void DCpuSampler::DoSample()
	{
	TInt err = iTimer.Again(1);
	if (err == KErrArgument)
		{
		// Missed a tick, oh well
		iTimer.OneShot(1);
		}

	NThread* thread = NKern::CurrentThread();
	if (iCpuUsageCallbackFn)
		{
		// Not used by anything in fshell pkg
		(*iCpuUsageCallbackFn)(iContext, thread);
		}

	if (iProfiling)
		{
		Usample(thread);
		}
	iContext = thread;
	}

TBool DCpuSampler::ShouldRun() const
	{
	return iProfiling || iCpuUsageCallbackFn != NULL;
	}

void DCpuSampler::SettingsChanged()
	{
	// Call with interrupts disabled
	if (ShouldRun())
		{
#if FSHELL_PLATFORM_SYMTB >= 92
		if (!iTimer.IsPending())
#else
		if (iTimer.iState == NTimer::EIdle)
#endif
			{
			iContext = NULL;
			iTimerHeartbeat = 0;
			iTimer.OneShot(1);
			}
		}
	else
		{
		iTimer.Cancel();
		}
	}

void DCpuSampler::SetCpuUsageSampling(MExtraBtrace::TCpuUsageCallback aCallbackFn)
	{
	TUint irq = NKern::DisableAllInterrupts();
	iCpuUsageCallbackFn = aCallbackFn;
	SettingsChanged();
	NKern::RestoreInterrupts(irq);
	}


void DCpuSampler::SetProfilingSampling(TBool aEnabled)
	{
	TUint irq = NKern::DisableAllInterrupts();
	iProfiling = aEnabled;
	SettingsChanged();
	NKern::RestoreInterrupts(irq);
	}

void DCpuSampler::Usample(NThread* pN)
	{
	if (Kern::NThreadToDThread(pN)!=NULL)
		{
#if defined(__MARM__)
		TBool idfcRunning = IDFCRunning();
		TLinAddr pC=((iIntStackTop)[-1]) & ~1; //clear LSB bit (in case of Jazelle code)
#else
		// Can't get this info if we don't have the .cia functions
		TBool idfcRunning = EFalse;
		TLinAddr pC = 0;
#endif

		if (idfcRunning)
			{
			//no useful thread context
			BTrace4(BTrace::EProfiling,BTrace::ECpuIdfcSample,pC);
			}
		else
		    {
			if (iContext!=pN)
				{
				BTrace8(BTrace::EProfiling,BTrace::ECpuFullSample,pC,pN);
				}
			else
				{
				//optimisation - context of previous sample is assumed
				BTrace4(BTrace::EProfiling,BTrace::ECpuOptimisedSample,pC);
				}
 		    }
		}
	else
		{
		BTrace0(BTrace::EProfiling,BTrace::ECpuNonSymbianThreadSample);
		}
	}
