// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file define all the common values thoughout your test project
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TE_GRAPHICSPERFORMANCE_SUITEDEFS_H__
#define __TE_GRAPHICSPERFORMANCE_SUITEDEFS_H__

#include <gdi.h>

_LIT(KProfilerLogFile,"c:\\PROFILER.DAT");
#define __MOVEFILETODRIVE (__SrcFile, __DestFile) 

// use this to start and stop sampling profiler within this teststep
//#define _USE_PROFILER 

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_graphicsperformanceSuiteString,"TheString");
_LIT(KTe_graphicsperformanceSuiteInt,"TheInt");
_LIT(KTe_graphicsperformanceSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_graphicsperformanceSuitePanic,"Te_graphicsperformanceSuite");

const TDisplayMode KValidBitmapModes[] = {EColor16MAP, EColor16MA, EColor16MU, EColor16M, EColor256, EGray256, EColor64K};
const TInt KNumValidBitmapModes = TInt(sizeof(KValidBitmapModes)/sizeof(KValidBitmapModes[0]));

const TDisplayMode KValidDisplayModes[] = {EColor16MAP, EColor16MA, EColor16MU, EColor64K};
const TInt KNumValidDisplayModes = TInt(sizeof(KValidDisplayModes)/sizeof(KValidDisplayModes[0]));

// Test macro to log Leaves.
#define TESTNOERRORL(a) \
	{\
	TInt b = a;\
	TBool c = (b == KErrNone);\
	testBooleanTrueWithErrorCodeL((c), (b), (TText8*)__FILE__, __LINE__);\
	}

// Redefine TEST/TESTE as TEST1/TESTE1 so that all errors are logged, not
// just the first error.
#ifdef TEST
#undef TEST
#define TEST(a) TEST1(a, ETrue)
#endif // TEST

#ifdef TESTE
#undef TESTE
#define TESTE(a, b) TESTE1(a, b, ETrue)
#endif // TESTE

#ifdef _USE_PROFILER
	#define __PROFILERMEMBERS RProcess iProcess; TInt iPErr; RFs iFSession; TBuf<256> iProfLog; TBuf<256> iProfLoE; CFileMan* iFileMan; 
	#define __STARTPROFILER iPErr=iProcess.Create(_L("profiler"),_L("start")); \
		if (iPErr==KErrNone) { \
			iProcess.Resume(); \
			iProcess.Close(); \
		} else { \
			INFO_PRINTF2(_L("Profiler process created return value:"),iPErr); \
		}
	#define __STOPPROFILER(__destfilename, __fnext1, __fnext2, __fnext3) 	Profiler::Stop(); \
		Profiler::Close(); \
		Profiler::Unload(); \
		User::After(500*1000); \
		iProfLog.Zero(); iProfLog.Copy(_L("c:\\Profiles\\P_")); iProfLog.Append(__destfilename); iProfLog.AppendNum(__fnext1); iProfLog.Append(_L("_")); iProfLog.AppendNum(__fnext2); iProfLog.Append(_L("_")); iProfLog.AppendNum(__fnext3); iProfLog.Append(_L(".dat")); iProfLog.ZeroTerminate(); \
		iProfLoE.Zero(); iProfLoE.Copy(_L("e:\\Profiles\\P_")); iProfLoE.Append(__destfilename); iProfLoE.AppendNum(__fnext1); iProfLoE.Append(_L("_")); iProfLoE.AppendNum(__fnext2); iProfLoE.Append(_L("_")); iProfLoE.AppendNum(__fnext3); iProfLoE.Append(_L(".dat")); iProfLoE.ZeroTerminate(); \
		iFSession.Delete(iProfLog); \
		iFSession.Delete(iProfLoE); \
		iFileMan->Copy(KProfilerLogFile,iProfLoE); \
		iPErr = iFSession.Rename(KProfilerLogFile,iProfLog);	\
		if (iPErr==KErrNone) \
			INFO_PRINTF2(_L("renamed profiler logfile to %s"), iProfLog.Ptr()); \
			else  INFO_PRINTF3(_L("ERROR %i renaming profiler logfile to %s "), iPErr, iProfLog.Ptr()); 
		
		#define __INITPROFILER User::LeaveIfError(iFSession.Connect()); iFileMan = CFileMan::NewL(iFSession);
		#define __CLEANUPPROFILER delete iFileMan; iFSession.Close(); 		
	
#else // we dont use profiler, blank dummy defines
	#define __PROFILERMEMBERS
	#define __STARTPROFILER
	#define __STOPPROFILER(__destfilename, __fnext1, __fnext2, __fnext3); 
	#define __INITPROFILER
	#define __CLEANUPPROFILER
#endif

#endif //__TE_GRAPHICSPERFORMANCE_SUITEDEFS_H__
