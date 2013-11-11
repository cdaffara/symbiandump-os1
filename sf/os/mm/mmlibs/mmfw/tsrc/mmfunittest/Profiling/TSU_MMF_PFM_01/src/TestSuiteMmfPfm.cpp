// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestSuiteMmfPerformance.cpp
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <TestFramework.h>

#include "TestStepMmfPfm.h"
#include "TestSuiteMmfPfm.h"
#include "TestStepMeasurePfm.h"


/**
*
* Create an instance of this test suite.
*
* NewL is exported at ordinal 1.
* This provides the interface to allow the Test Framework
* to create instances of this test suite.
*
* @return	"CTestSuiteMmfPerformance*"
*			The newly created test suite
*/
EXPORT_C CTestSuiteMmfPerformance* CTestSuiteMmfPerformance::NewL() 
    { 
	CTestSuiteMmfPerformance* self = new (ELeave) CTestSuiteMmfPerformance;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

/**
*
* This test suite version string
*
*
*/
_LIT(KTxtVersion,"0.1");

/**
*
* Get test suite version.
*
* @return	"TPtrC"
*			The version string.
*
*
*/
TPtrC CTestSuiteMmfPerformance::GetVersion( )
	{
	return KTxtVersion();
	}


/**
*
* Initialiser for test suite.
* This creates all the test steps and stores them
* inside CTestSuiteMmfPerformance
* 
*/
void CTestSuiteMmfPerformance::InitialiseL( )
	{
	// set the default path
	InitSystemPath();

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_PFM_01");

	AddTestStepL(new(ELeave) RMmfPfmTest001(this));

	}

/**
*
* InitSystemPath
* This sets up the default path
* 
*/
void CTestSuiteMmfPerformance::InitSystemPath()
	{

	_LIT(KAudioTestSourcePathOnE,"e:\\MmfProfileTest\\");
	_LIT(KAudioTestSourcePathOnD,"d:\\MmfProfileTest\\");
	_LIT(KAudioTestSourcePathOnC,"c:\\MmfProfileTest\\");
	_LIT(KAudioTestSourcePathOnZ,"z:\\MmfProfileTest\\");

	TUint att;
	RFs fs;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));

	if (fs.Att(KAudioTestSourcePathOnC,att) == KErrNone)
		{
		iDefaultPath = KAudioTestSourcePathOnC;
		}
	else if (fs.Att(KAudioTestSourcePathOnD,att) == KErrNone)
		{
		iDefaultPath = KAudioTestSourcePathOnD;
		}
	else if (fs.Att(KAudioTestSourcePathOnE,att) == KErrNone)
		{
		iDefaultPath = KAudioTestSourcePathOnE;
		}
	else if (fs.Att(KAudioTestSourcePathOnZ,att) == KErrNone)
		{
		iDefaultPath = KAudioTestSourcePathOnZ;
		}
	else
		{
		User::Panic(_L("Test files not found"),KErrNotFound);
		}

	fs.Close();

	}
	
const TDesC& CTestSuiteMmfPerformance::DefaultPath() const
	{
	return iDefaultPath;
	}
