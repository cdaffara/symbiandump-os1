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
 

#ifndef __WSERV_TEST_STRESS_STEP_ONE_H__
#define __WSERV_TEST_STRESS_STEP_ONE_H__

#include <test/testexecutestepbase.h>

#include "test_step_logger.h"
#include "test_step_conf.h"

_LIT(KServStressTest,"WServStressTest_One");

struct TRunData
	{
	TInt initPeriod;
	TInt period;
	TInt maxRunCycles;
	TInt64 randomSeed;
	TInt minNumWindows;
	TInt windowWidth;
	TInt windowHeight;
	TBool compareBitmaps;            // compare screen and off-screen bitmaps after drawing windows
	TBool saveOnlyDifferent;         // true saves only bitmaps with differences, otherwise all
	TBool saveDifferenceBitmap;      // save a bitmap of expected XOR screen
	TBool clearAllBackground;        // clear all background area or just the are without windows above it
	TBufC<256> loggingPath;			 // path where logging.log file should be stored
	};
class CStressletThread;

class CWServTestStep : public CTestStep, public MTestStepReporter, public MTestStepConfigurationContextFactory
	{
public:
	class CConfigurationContext : public MTestStepConfigurationContext
	/**
	CConfigurationContext exposes an interface for reading configuration data
	from an ini file for a specificed section
 	*/
		{
	public:
		static CConfigurationContext* NewLC(CTestStep& aStep, const TDesC& aContextName);
		TBool GetInt(const TDesC& aName, TInt& aData) const;
		TBool GetBool(const TDesC& aName, TBool& aData) const;
		TBool GetString(const TDesC& aName, TPtrC& aData) const;
		TBool HasInt(const TDesC& aName) const;
		TBool HasBool(const TDesC& aName) const;
		TBool HasString(const TDesC& aName) const;
	protected:
		CConfigurationContext(CTestStep& aStep, const TDesC& aContextName);
		void ConstructL();
		
		CTestStep& iStep;
		TPtrC      iContextName;
		};

	CWServTestStep();
	~CWServTestStep();
	MTestStepConfigurationContext* GetConfigurationContextLC(const TDesC& aContextName);

protected: //from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	virtual void LogCondition(TBool aCondValue, const TText8* aFile, TInt aLine );
	virtual void LogCondition(TDesC& aCondInformation, TBool aCondValue, const TText8* aFile, TInt aLine );
	
private:
	RFs iRFs;
	TRunData iData;
	};

#endif // __WSERV_TEST_STRESS_STEP_ONE_H__
