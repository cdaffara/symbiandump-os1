// extrabtrace.cpp
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
#include <fshell/extrabtrace.h>
#include "common.h"

NONSHARABLE_CLASS(RExtraBtraceImpl) : public RBusLogicalChannel
	{
public:
	TInt Connect();
	TInt EnableProfiling(TInt aSamplingPeriod);
	};

#define iImplementation (*reinterpret_cast<RExtraBtraceImpl*>(iImpl))
#define iConstImplementation (*reinterpret_cast<const RExtraBtraceImpl*>(iImpl))

EXPORT_C RExtraBtrace::RExtraBtrace()
	{
	iImplementation.SetHandle(0);
	}

EXPORT_C TInt RExtraBtrace::Connect()
	{
	return iImplementation.Connect();
	}

EXPORT_C void RExtraBtrace::Close()
	{
	iImplementation.Close();
	}

EXPORT_C TInt RExtraBtrace::Handle() const
	{
	return iConstImplementation.Handle();
	}

EXPORT_C TInt RExtraBtrace::EnableProfiling(TInt aSamplingPeriod)
	{
	return iImplementation.EnableProfiling(aSamplingPeriod);
	}

TInt RExtraBtraceImpl::Connect()
	{
	_LIT(KLddName,"extrabtracek");
	TInt r = User::LoadLogicalDevice(KLddName);
	if(r!=KErrNone && r!=KErrAlreadyExists)
		return r;

#ifdef FSHELL_CUSTOM_BTRACE_SUPPORT_LIBRARY
	// If FSHELL_TRACE_LIBRARY etc has been overridden it's possible that we need to load an additional driver to have extrabtrace function
	r = User::LoadLogicalDevice(TPtrC((const TUint16*)FSHELL_CUSTOM_BTRACE_SUPPORT_LIBRARY));
	if(r!=KErrNone && r!=KErrAlreadyExists)
		return r;
#endif

	r = DoCreate(KLogicalDeviceName, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
	return r;
	}

TInt RExtraBtraceImpl::EnableProfiling(TInt aSamplingPeriod)
	{
	return DoControl(EControlEnableProfiling, (TAny*)aSamplingPeriod);
	}
