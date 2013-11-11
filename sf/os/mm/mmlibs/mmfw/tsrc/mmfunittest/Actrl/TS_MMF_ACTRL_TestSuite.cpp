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
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <testframework.h>

#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"
#include "TestStepActrlPlay.h"
#include "TestStepsGetSet.h"
#include "TestStepActrlRecord.h"
#include "TestStepController.h"

const TInt KTimesToPlay = 100;



/**
*
* Create an instance of this test suite.
*
* NewTestSuiteActrlL is exported at ordinal 1.
* This provides the interface to allow the Test Framework
* to create instances of this test suite.
*
* @return	"CTestSuiteActrl*"
*			The newly created test suite
*/
EXPORT_C CTestSuiteActrl* NewTestSuiteActrlL() 
    { 
	CTestSuiteActrl* self = new (ELeave) CTestSuiteActrl;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

/**
*
* CTestSuiteActrl destructor
*
* 
*/
CTestSuiteActrl::~CTestSuiteActrl()
	{
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
TPtrC CTestSuiteActrl::GetVersion( void )
	{
	return KTxtVersion();
	}

/**
*
* Add a test step into the suite.
*
* @param	"CTestStepActrl* aPtrTestStep"
*			The test step to be added.
* 
*/
void CTestSuiteActrl::AddTestStepL( CTestStepActrl* aPtrTestStep )
	{
    // [ assert pre condition that a valid test ptr has been supplied ]
	ASSERT( aPtrTestStep );
	
	// [ set back wards reference to test suite ]
	aPtrTestStep->SetSuite(this); 
	
	//[ add the step to the test suite ]
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
*
* Initialiser for test suite.
* This creates all the test steps and stores them
* inside CTestSuiteActrl
* 
*/
void CTestSuiteActrl::InitialiseL( void )
	{
	//[ set the system path ]
	InitSystemPath();

	// store the name of this test suite
	iSuiteName = _L("Actrl");
	AddTestStepL( new(ELeave) CTestStepActrlPlay );
	AddTestStepL( new(ELeave) CTestStepGetSetVolumeInPrimed );
	AddTestStepL( new(ELeave) CTestStepGetSetVolumeInPlay );
	AddTestStepL( new(ELeave) CTestStepGetSetVolumeInStopped );
	AddTestStepL( new(ELeave) CTestStepPlayT( KTimesToPlay )  );
	AddTestStepL( new(ELeave) CTestStepGetSetBalanceInPrimed() );
	AddTestStepL( new(ELeave) CTestStepGetSetBalanceInPlay() );
	AddTestStepL( new(ELeave) CTestStepGetSetBalanceInStopped() );
	AddTestStepL( new(ELeave) CTestStepGetSetPositionInPlay );
	AddTestStepL( new(ELeave) CTestStepGetSetPositionInPaused );
	AddTestStepL( new(ELeave) CTestStepGetSetPositionInPrimed );
	AddTestStepL( new(ELeave) CTestStepGetSetPositionInStopped );
	AddTestStepL( new(ELeave) CTestStepPause );
	AddTestStepL( new(ELeave) CTestStepStop );
	AddTestStepL( new(ELeave) CTestStepActrlRecord );
	AddTestStepL( new(ELeave) CTestStepMarcSetMaxFileSize );
	AddTestStepL( new(ELeave) CTestStepSetGetRecordGain );
	AddTestStepL( new(ELeave) CTestStepSetGetRecordBalance );
	AddTestStepL( new(ELeave) CTestStepSetGetRecordBalanceNoSink );
	AddTestStepL( new(ELeave) CTestStepSetGetRecordTimeAvailable );
	AddTestStepL( new(ELeave) CTestStepResetController );
	AddTestStepL( new(ELeave) CTestStepRemoveDataSink );
	AddTestStepL( new(ELeave) CTestStepRemoveDataSource );
	AddTestStepL( new(ELeave) CTestStepMemoryAllocation );
	AddTestStepL( new(ELeave) CTestStepGetSetNumChannelsOnSource );
	AddTestStepL( new(ELeave) CTestStepGetSetNumChannelsOnSink );
	AddTestStepL( new(ELeave) CTestStepGetSetSinkFormat );
	AddTestStepL( new(ELeave) CTestStepGetSetSourceFormat );
	AddTestStepL( new(ELeave) CTestStepGetSetSourceSampleRate );
	AddTestStepL( new(ELeave) CTestStepGetSetSinkSampleRate );
	AddTestStepL( new(ELeave) CTestStepGetSetSourceBitRate );
	AddTestStepL( new(ELeave) CTestStepGetSetSourceDataType );
	AddTestStepL( new(ELeave) CTestStepGetSetSinkBitRate );
	AddTestStepL( new(ELeave) CTestStepGetSetSinkDataType );
	AddTestStepL( new(ELeave) CTestStepGetSetMetaData );
	AddTestStepL( new(ELeave) CTestStepRecordMemToFile );
	AddTestStepL( new(ELeave) CTestStepRecordMemToMem );
	AddTestStepL( new(ELeave) CTestStepGetSupportedSinkDataTypes );
	AddTestStepL( new(ELeave) CTestStepGetSupportedSourceDataTypes );
	AddTestStepL( new(ELeave) CTestStepGetSupportedSourceNumChannels );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 0 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 1 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 2 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 3 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 4 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 5 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 6 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 7 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 8 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 10 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 11 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 13 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 16 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 17 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 18 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 19 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 20 ) );
	
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 0 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 1 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 2 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 3 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 4 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 5 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 6 ) );
	AddTestStepL( new(ELeave) CTestStepRecordFormat( 7 ) );

	
	// negative tests
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 9 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 12 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 14 ) );
	AddTestStepL( new(ELeave) CTestStepConvertFormat( 15 ) );
	AddTestStepL( new(ELeave) CTestStepRampVolume() );
	}


/**
*
* InitSystemPath
* This sets up the default path
* 
*/
void CTestSuiteActrl::InitSystemPath()
	{
	_LIT(KVideoTestSourcePathOnC,"c:\\mm\\mmf\\testfiles\\actrl\\");

	RFs fs;
	TUint att;
	TInt ret = fs.Connect();
	__ASSERT_ALWAYS(ret == KErrNone,User::Panic(_L("Sys path not setup"),ret));

	if (fs.Att(KVideoTestSourcePathOnC, att) == KErrNone)
		iDefaultPath = KVideoTestSourcePathOnC;
	else
		User::Panic(_L("Test files not found"),KErrNotFound);

	fs.Close();
	}

const TDesC& CTestSuiteActrl::DefaultPath() const
	{
	return iDefaultPath;
	}
