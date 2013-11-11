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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "test_step_logger.h"

#include "test_step_comparison.h"

#include "stresslet.h"
#include "comparison.h"


//
// ConfigurationContext
//
CWServTestStep::CConfigurationContext* CWServTestStep::CConfigurationContext::NewLC(CTestStep& aStep, const TDesC& aContextName)
	{
	CWServTestStep::CConfigurationContext* self = new (ELeave) CWServTestStep::CConfigurationContext(aStep, aContextName);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return self;
	}

TBool CWServTestStep::CConfigurationContext::GetInt(const TDesC& aName,  TInt& aData) const
	{
	return iStep.GetIntFromConfig(iContextName, aName, aData);
	}

TBool CWServTestStep::CConfigurationContext::GetBool(const TDesC& aName, TBool& aData) const
	{
	return iStep.GetBoolFromConfig(iContextName, aName, aData);
	}

TBool CWServTestStep::CConfigurationContext::GetString(const TDesC& aName, TPtrC& aData) const
	{
	return iStep.GetStringFromConfig(iContextName, aName, aData);
	}

TBool CWServTestStep::CConfigurationContext::HasInt(const TDesC& aName) const
	{
	TInt dummy;
	return iStep.GetIntFromConfig(iContextName, aName, dummy);
	}

TBool CWServTestStep::CConfigurationContext::HasBool(const TDesC& aName) const
	{
	TBool dummy;
	return iStep.GetBoolFromConfig(iContextName, aName, dummy);
	}

TBool CWServTestStep::CConfigurationContext::HasString(const TDesC& aName) const
	{
	TPtrC dummy;
	return iStep.GetStringFromConfig(iContextName, aName, dummy);
	}

CWServTestStep::CConfigurationContext::CConfigurationContext(CTestStep& aStep, const TDesC& aContextName) :
	iStep(aStep), iContextName(aContextName)
	{
	}

void CWServTestStep::CConfigurationContext::ConstructL()
	{
	}

	
//
// TestStep
//

// string literals for configuration parameter names
_LIT(KDefaultSectionName,"default");
_LIT(KT_WservStressInit, "init_period");
_LIT(KT_WservStressPeriod, "period");
_LIT(KT_WservStressMaxRunCycles, "max_run_cycles");
_LIT(KT_WservStressRandomSeed, "random_seed");
_LIT(KT_WservStressMinNumWindows, "min_num_windows");
_LIT(KT_WservStressWindowWidth, "window_width");
_LIT(KT_WservStressWindowHeight, "window_height");
_LIT(KT_WservStressCompareBitmaps, "compare_bitmaps");
_LIT(KT_WservStressSaveOnlyDifferent, "save_only_different");
_LIT(KT_WservStressSaveDifferenceBitmap, "save_difference_bitmap");
_LIT(KT_WservStressClearAllBackground, "clear_all_background");
_LIT(KT_WservStressLoggingPath, "logging_path");

MTestStepConfigurationContext* CWServTestStep::GetConfigurationContextLC(const TDesC& aContextName)
{	
	return CWServTestStep::CConfigurationContext::NewLC(*this, aContextName);
}

CWServTestStep::~CWServTestStep()
	{
//	iStresslets.ResetAndDestroy();	
	}

CWServTestStep::CWServTestStep()
	{
	SetTestStepName(KServStressTest);
	}

/** 
 * Reads test configuration data from ini file
 * Leaves if expected data not found
 */
TVerdict CWServTestStep::doTestStepPreambleL()
	{
	TVerdict ret=CTestStep::doTestStepPreambleL();
	if (!GetIntFromConfig(KDefaultSectionName, KT_WservStressInit, iData.initPeriod)||
		!GetIntFromConfig(KDefaultSectionName, KT_WservStressPeriod, iData.period)||
		!GetIntFromConfig(KDefaultSectionName, KT_WservStressMaxRunCycles, iData.maxRunCycles))
		{
		INFO_PRINTF1(_L("Error reading ini file"));
		User::Leave(KErrNotFound);
		}
	
	//random_seed is not a mandatory parameter
	iData.randomSeed = -1;
	TPtrC strSeed;
	GetStringFromConfig(KDefaultSectionName, KT_WservStressRandomSeed, strSeed);
	if (strSeed.Ptr())
		{
		TLex seed;
		seed.Assign(strSeed);
		seed.Val(iData.randomSeed);
		}
	iData.minNumWindows = 0;	//unlimited
	GetIntFromConfig(KDefaultSectionName, KT_WservStressMinNumWindows, iData.minNumWindows);
	
	iData.windowWidth = 400;
	GetIntFromConfig(KDefaultSectionName, KT_WservStressWindowWidth, iData.windowWidth);
	iData.windowHeight = 400;
	GetIntFromConfig(KDefaultSectionName, KT_WservStressWindowHeight, iData.windowHeight);
	
	iData.compareBitmaps = ETrue;
	GetBoolFromConfig(KDefaultSectionName, KT_WservStressCompareBitmaps, iData.compareBitmaps);

	iData.saveOnlyDifferent = ETrue;
	GetBoolFromConfig(KDefaultSectionName, KT_WservStressSaveOnlyDifferent, iData.saveOnlyDifferent);

	iData.saveDifferenceBitmap = EFalse;
	GetBoolFromConfig(KDefaultSectionName, KT_WservStressSaveDifferenceBitmap, iData.saveDifferenceBitmap);

	iData.clearAllBackground = ETrue;
	GetBoolFromConfig(KDefaultSectionName, KT_WservStressClearAllBackground, iData.clearAllBackground);

	TPtrC path;
	GetStringFromConfig(KDefaultSectionName, KT_WservStressLoggingPath, path);
	RBuf loggingPath;
	loggingPath.CleanupClosePushL();
	loggingPath.CreateL(path.Length() + 16);
	if (path.Length())
		{
		loggingPath.Copy(path);
		if (path[path.Length()-1] != '\\')
			{
			loggingPath.Append('\\');
			}
		}
	else
		{
		loggingPath = _L("C:\\StressLog\\");
		}
	iData.loggingPath = loggingPath;
	CleanupStack::PopAndDestroy(&loggingPath);
	return ret;
	}

/**
 @SYMTestCaseID		GRAPHICS-PREQ1841-STRESS-0001
 @SYMPREQ 		PREQ1841
 @SYMTestType		UT
 @SYMTestPriority	Critical
 @SYMTestStatus 	Implemented
 @SYMTestCaseDesc	
 @SYMTestActions	
 @SYMTestExpectedResults 
 */
TVerdict CWServTestStep::doTestStepL()
	{
	User::LeaveIfError(iRFs.Connect());	

	CComparison * comparison = CComparison::NewLC( *this );
	comparison->SetRunDataL(iData, this);
	CComparison::LaunchL(comparison);
	CleanupStack::Pop(comparison); 
	
	return TestStepResult();
	}

/** */
TVerdict CWServTestStep::doTestStepPostambleL()
	{
	iRFs.Close();
	return CTestStep::doTestStepPostambleL();
	}

/** */
void CWServTestStep::LogCondition(TBool aCondValue, const TText8* aFile, TInt aLine )
	{
	TPtrC8 ptr(aFile);
	TBuf<KMaxFileName> buf;
	buf.Copy(ptr);
	
	TVolumeInfo info;
	iRFs.Volume(info);

	TInt biggestBlock;
	TInt freeHeapMem = User::Available(biggestBlock);
	
	testBooleanTrue ( aCondValue, aFile, aLine ); 		// not using TEST macro as we need to transfer file & line from the caller
	INFO_PRINTF6(_L("cond value: %d at %S in %d available disk space %d, available heap mem %d"), (TInt) aCondValue, &buf, aLine, info.iFree, freeHeapMem);
	}

/** */
void CWServTestStep::LogCondition(TDesC& /*aCondInformation*/, TBool /*aCondValue*/, const TText8* /*aFile*/, TInt /*aLine*/ )
	{ 
	}
