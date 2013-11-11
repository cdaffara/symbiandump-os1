// sampler.h
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
#ifndef EXTRABTRACE_SAMPLER_H
#define EXTRABTRACE_SAMPLER_H

#include <fshell/extrabtracek.h>
#include <kernel.h>

NONSHARABLE_CLASS(DCpuSampler) : public DBase
	{
public:
	DCpuSampler();
	~DCpuSampler();

	void SetCpuUsageSampling(MExtraBtrace::TCpuUsageCallback aCallbackFn);
	void SetProfilingSampling(TBool aEnabled);

private:
	static void Sample(TAny* aPtr);
	void DoSample();
	void Usample(NThread* aThread);
	void SettingsChanged();
	TBool ShouldRun() const;

private:
	NThread* iContext;
	NTimer iTimer;
	TInt iTimerHeartbeat;
	TUint* iIntStackTop; // I don't understand this, but whover wrote it originally hopefully did!
	TBool iProfiling;
	MExtraBtrace::TCpuUsageCallback iCpuUsageCallbackFn;
	};

#endif
