// extrabtracek.cpp
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

#include <fshell/extrabtracek.h>
#include <kern_priv.h>

#include "common.h"
#ifndef __SMP__
#include "sampler.h"
#endif
#include "eventhandler.h"

class DExtraBTraceChannel : public DLogicalChannelBase
	{
public:
	DExtraBTraceChannel();
	virtual ~DExtraBTraceChannel();
	//	Inherited from DObject
	virtual TInt RequestUserHandle(DThread* aThread, TOwnerType aType);
	// Inherited from DLogicalChannelBase
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual TInt Request(TInt aReqNo, TAny* a1, TAny* a2);
private:
	DCpuSampler* Sampler();
	};

TInt DExtraBTraceFactory::Install()
	{
	TInt err = SetName(&KLogicalDeviceName);
	if (err) return err;
	err = Open();
	if (err) return err;

#ifndef __SMP__
	iSampler = new DCpuSampler();
	if (!iSampler) err = KErrNoMemory;
#endif

	if (!err) iEventHandler = new DExtraBtraceEventHandler(this);
	if (!iEventHandler) err = KErrNoMemory;

	if (err)
		{
		Close(NULL);
		}
	else
		{
		iEventHandler->Add();
		// We must NOT call anything that could fail from this point on - otherwise it is impossible to safely close the DExtraBtraceEventHandler (looks like a bug in constructing a DKernelEventHandler from DLogicalDevice::Install())
		}
	return err;
	}


void DExtraBTraceFactory::GetCaps(TDes8& /*aDes*/) const
	{
	//Kern::InfoCopy(aDes,0,0);
	}

TInt DExtraBTraceFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel=new DExtraBTraceChannel();
	if(!aChannel)
		return KErrNoMemory;
	return KErrNone;
	}

DExtraBTraceFactory::~DExtraBTraceFactory()
	{
#ifndef __SMP__
	delete iSampler;
#endif
	delete iEventHandler;
	}

void DExtraBTraceFactory::SetCpuUsageSampling(MExtraBtrace::TCpuUsageCallback aCallbackFn)
	{
#ifndef __SMP__
	iSampler->SetCpuUsageSampling(aCallbackFn);
#endif
	}

void DExtraBTraceFactory::SetProfilingSampling(TBool aEnabled)
	{
#ifndef __SMP__
	iSampler->SetProfilingSampling(aEnabled);
#endif
	}

TAny* DExtraBTraceFactory::GetVersion(TInt /*aVersion*/)
	{
	return NULL; // We don't have any versions yet - this is like CBase::_Extension, just in case
	}

void DExtraBTraceFactory::MExtraBtrace_Close()
	{
	Close(NULL);
	}

//

DExtraBTraceChannel::DExtraBTraceChannel()
	{
	}

DExtraBTraceChannel::~DExtraBTraceChannel()
	{
	}

TInt DExtraBTraceChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& /*aVer*/)
	{
	return KErrNone;
	}

#ifndef __SMP__
DCpuSampler* DExtraBTraceChannel::Sampler()
	{
	return static_cast<DExtraBTraceFactory*>(iDevice)->iSampler;
	}
#endif

TInt DExtraBTraceChannel::RequestUserHandle(DThread* /*aThread*/, TOwnerType /*aType*/)
	{
	//if (aType!=EOwnerThread || aThread!=iClient)
	//	return KErrAccessDenied;
	return KErrNone;
	}

TInt DExtraBTraceChannel::Request(TInt aReqNo, TAny* a1, TAny* /*a2*/)
	{
	switch (aReqNo)
		{
	case EControlEnableProfiling:
		{
#ifdef __SMP__
		return KErrNotSupported;
#else
		TInt enable = (a1 == 0) ? 0 : 1; // We still ignore the rate, currently
		TInt err = BTrace::SetFilter(BTrace::EProfiling, enable);
		if (err >= 0)
			{
			Sampler()->SetProfilingSampling(enable);
			}
		return err;
#endif
		}
	default:
		return KErrNotSupported;
		}
	}

DECLARE_EXTENSION_LDD()
	{
	return new DExtraBTraceFactory;
	}

DECLARE_STANDARD_EXTENSION()
	{
	return KErrNone;
	}

