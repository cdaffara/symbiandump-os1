// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// file timelegacyapis.cpp
// This executable times how long it takes to call legacy APIs. It should be used when 
// new hardware build targets are introduced. The existing performance tests can then be 
// updated for these platforms.
// Note: The times generated can be used for the following tests:
// - PrintSWVersionAPILegacyTimes					SYSLIB-BAFL-CT-4064
// - PrintLangVersionAPILegacyTimes				SYSLIB-BAFL-CT-4065
// - PrintLangSWVersionAPILegacyTimes				SYSLIB-BAFL-CT-4066
// 
//

#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>
#include <bafl/sysutil.h>
#include <hal.h>
#include "sysutilfunctions.h"
#include "sysutilpatchdata.h"

TReal FastCountToMilliseconds(const TUint32 aFastCount)
	{
	TInt freqInHz;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	TReal freqInkHz = freqInHz / 1000;
	return (TReal)aFastCount / freqInkHz;
	}

/**
Calls SysUtil::GetSWVersion and prints out how long it took for the call to complete.
*/
void PrintSWVersionAPILegacyTimes()
	{
	TBuf<KSysUtilVersionTextLength> version;
	
	TUint32 startTime = User::FastCounter();
	TInt err = SysUtil::GetSWVersion(version);
	TUint32 totalTime = User::FastCounter() - startTime;
	
	if(KErrNone != err)
		{
		RDebug::Printf("GetSWVersion return unexpected error. Error = %d", err);
		return;
		}
	RDebug::Printf("GetSWVersion took = %fms", FastCountToMilliseconds(totalTime));
	}

/**
Calls SysUtil::GetLangVersion and prints out how long it took for the call to complete.
*/
void PrintLangVersionAPILegacyTimes()
	{
	TBuf<KSysUtilVersionTextLength> version;
	
	TUint32 startTime = User::FastCounter();
	TInt err = SysUtil::GetSWVersion(version);
	TUint32 totalTime = User::FastCounter() - startTime;
	
	if(KErrNone != err)
		{
		RDebug::Printf("GetLangVersion return unexpected error. Error = %d", err);
		return;
		}
	RDebug::Printf("GetLangVersion took = %fms", FastCountToMilliseconds(totalTime));
	}

/**
Calls SysUtil::GetLangSWVersion and prints out how long it took for the call to complete.
*/
void PrintLangSWVersionAPILegacyTimes()
	{
	TBuf<KSysUtilVersionTextLength> version;
	
	TUint32 startTime = User::FastCounter();
	TInt err = SysUtil::GetLangSWVersion(version);
	TUint32 totalTime = User::FastCounter() - startTime;
	
	if(KErrNone != err)
		{
		RDebug::Printf("GetLangSWVersion return unexpected error. Error = %d", err);
		return;
		}
	RDebug::Printf("GetLangSWVersion took = %fms", FastCountToMilliseconds(totalTime));
	}

void Main()
	{
#ifndef __WINSCW__
	if(KSysUtilDisableVersionSetupExe)
		{
		SetTestPath(EFalse);
		PrintSWVersionAPILegacyTimes();
		PrintLangVersionAPILegacyTimes();
		PrintLangSWVersionAPILegacyTimes();
		}
	else
		{
		RDebug::Printf("\n\nMake sure SYMBIAN_BAFL_SYSUTIL_ENABLE_VERSION_SETUP_EXE is not defined when building a ROM.");
		}
#else
	RDebug::Printf("\n\nThis executable should only be run on hardware.");
#endif
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		return KErrNoMemory;
	
	Main();
	
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
