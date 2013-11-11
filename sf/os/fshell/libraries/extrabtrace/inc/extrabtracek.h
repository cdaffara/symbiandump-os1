// extrabtracek.h
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
#ifndef EXTRABTRACEK_H
#define EXTRABTRACEK_H

#include <fshell/extrabtrace.h>
#include <kernel/kern_priv.h>

// This class avoids exporting functions from extrabtracek.ldd - something that has proved problematic on certain platforms
class MExtraBtrace
	{
public:
	typedef void (*TCpuUsageCallback)(NThread*, NThread*); // First arg is previous thread, second is current
	virtual void SetCpuUsageSampling(TCpuUsageCallback aCallbackFn)=0;
	virtual void SetProfilingSampling(TBool aEnabled)=0;

	virtual TAny* GetVersion(TInt aVersion)=0;
	virtual void MExtraBtrace_Close()=0;
	};


class DCpuSampler;
class DExtraBtraceEventHandler;

class DExtraBTraceFactory : public DLogicalDevice, public MExtraBtrace
	{
public:
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	~DExtraBTraceFactory();

public: // From MExtraBtrace
	void SetCpuUsageSampling(TCpuUsageCallback aCallbackFn);
	void SetProfilingSampling(TBool aEnabled);
	TAny* GetVersion(TInt aVersion);
	void MExtraBtrace_Close();

public:
#ifndef __SMP__
	DCpuSampler* iSampler;
#endif
	DExtraBtraceEventHandler* iEventHandler;
	};


// This function avoids using WSD in extrabtracek.ldd - something that, again, has proved problematic on certain platforms
// (And it's inline because of aforementioned exports problem)

static MExtraBtrace* OpenExtraBtrace()
	{
	__ASSERT_CRITICAL;
	// Find the extrabtrace DLogicalDevice
	DLogicalDevice* ldd = NULL;
	DObjectCon* ldds = Kern::Containers()[ELogicalDevice];
	_LIT(KLogicalDeviceName, "extrabtrace");
	TKName resultname;
#ifdef FSHELL_9_1_SUPPORT
	TInt findHandle;
#else
	TFindHandle findHandle;
#endif
	TInt found = ldds->FindByName(findHandle, KLogicalDeviceName, resultname);
	if (found == KErrNone)
		{
		ldds->Wait();
#ifdef FSHELL_9_1_SUPPORT
		ldd = static_cast<DLogicalDevice*>(ldds->At(findHandle));
#else
		if (findHandle.Index() <= ldds->Count()) ldd = static_cast<DLogicalDevice*>((*ldds)[findHandle.Index()]);
#endif
		if (ldd && ldd->Open() != KErrNone) ldd = NULL;
		ldds->Signal();

		if (ldd)
			{
			return static_cast<DExtraBTraceFactory*>(ldd);
			}
		}
	return NULL;
	}

#endif
