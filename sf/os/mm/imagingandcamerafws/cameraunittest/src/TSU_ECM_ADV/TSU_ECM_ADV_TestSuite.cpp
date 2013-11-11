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
// This main DLL entry point for the TSU_ECM_ADV_TestSuite
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_ECM_ADV_TestSuite.h"

#include "ECamInfoAdvTest.h"
#include "ECamAdvTest.h"
#include "ECamAdvSetTest.h"
#include "ECamPresetsTest.h"
#include "ECamImageProcessingTest.h"
#include "../ECamUnitTestPlugin/ECamUnitTestPlugin.h"

#include "ECamEnhanceFuncTest.h"
#include "ecamdirectviewfindertest.h"
#include "ecamextendedfunctest.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_ECam_ADV* NewTestSuiteL() 
    { 
	CTestSuite_ECam_ADV* self = new(ELeave) CTestSuite_ECam_ADV;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
    }

/**
 * Destructor
 */
CTestSuite_ECam_ADV::~CTestSuite_ECam_ADV()
	{
	RemoveDependencyFile();
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuite_ECam_ADV::GetVersion()
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuite_ECam_ADV 
 */
void CTestSuite_ECam_ADV::InitialiseL()
	{
    CreateDependencyFileL();

	// store the name of this test suite 
	iSuiteName = _L("TSU_ECM_ADV");
	
	AddTestStepL(RECamInfoTest::NewL(EFalse));
	AddTestStepL(RECamInnerInfoTest::NewL(EFalse));

	AddTestStepL(RECamTest::NewL(EFalse));
	AddTestStepL(RECamInnerTest::NewL(EFalse));

	AddTestStepL(RECamAdvSetTest::NewL(EFalse));

	AddTestStepL(RECamPresetsTest::NewL(EFalse));
	
	AddTestStepL(RECamImageProcessingTest::NewL(EFalse));

	AddTestStepL(RECamPresetsNotificationTest::NewL(EFalse));
	AddTestStepL(RECamPresetsBaselineTest::NewL(EFalse));
	AddTestStepL(RECamAdvSetNotificationTest::NewL(EFalse));

	AddTestStepL(RECamAdvSetDefaultsTest::NewL(EFalse));
	AddTestStepL(RECamAdvSetGettersTest::NewL(EFalse));
	AddTestStepL(RECamImageProcDefaultsTest::NewL(EFalse));
	AddTestStepL(RECamImageProcBaselineTest::NewL(EFalse));
	AddTestStepL(RECamImageProcRangesTest::NewL(EFalse));
	AddTestStepL(RECamAdvSetTwoCamTest::NewL(EFalse));
	AddTestStepL(RECamPresetsNegTest::NewL(EFalse));
	AddTestStepL(RECamAdvSetNegTest::NewL(EFalse));
	AddTestStepL(RECamImageProcNegTest::NewL(EFalse));
	AddTestStepL(RECamAdvSetBaselineTest::NewL(EFalse));
	//Histogram
	AddTestStepL(RECamHistTest::NewL(_L("MM-ECM-ADV-U-020-HP")));	
	AddTestStepL(RECamHistTest::NewL(_L("MM-ECM-ADV-U-021-HP")));	
	AddTestStepL(RECamHistTest::NewL(_L("MM-ECM-ADV-U-022-HP")));	
	AddTestStepL(RECamHistogramNotificationTest::NewL(EFalse));
				
	//Overlay
	AddTestStepL(RECamOvrlayTest::NewL(_L("MM-ECM-ADV-U-030-HP")));	
	AddTestStepL(RECamOvrlayTest::NewL(_L("MM-ECM-ADV-U-031-HP")));	
		
	//Snapshot	
	AddTestStepL(RECamSnapTest::NewL(_L("MM-ECM-ADV-U-040-HP")));	
	AddTestStepL(RECamSnapTest::NewL(_L("MM-ECM-ADV-U-041-HP")));	
	AddTestStepL(RECamSnapTest::NewL(_L("MM-ECM-ADV-U-042-HP")));	
	AddTestStepL(RECamSnapNotificationTest::NewL(EFalse));
	
	//Direct Viewfinder
	AddTestStepL(RECamDirectViewFinderTest::NewL(_L("MM-ECM-ADV-U-044-HP")));	
	AddTestStepL(RECamDirectViewFinderTest::NewL(_L("MM-ECM-ADV-U-045-HP")));	
	
	// DEV CRs KLUO-6WFHP4 and KLUO-6VRLKY
	AddTestStepL(RECamAdvSetTest2::NewL(EFalse));
	
	//Client ViewFinder Test
	AddTestStepL(RECamClientViewFinderTest::NewL(_L("MM-ECM-ADV-U-050-HP")));
	AddTestStepL(RECamClientVFNotificationTest::NewL(EFalse));
	
	//Direct ViewFinder Test
	AddTestStepL(RECamV2DirectViewFinderTest::NewL(_L("MM-ECM-ADV-U-052-HP")));
	AddTestStepL(RECamV2DirectVFNotificationTest::NewL(EFalse));
	
	//Image Capture Test
	AddTestStepL(RECamPreImgCaptureNotificationTest::NewL(EFalse));
	AddTestStepL(RECamImgCaptureNotificationTest::NewL(EFalse));
	
	//Video Capture Control Test
	AddTestStepL(RECamVideoCaptureControlTest::NewL(_L("MM-ECM-ADV-U-056-HP")));
	AddTestStepL(RECamVideoCaptureNotificationTest::NewL(EFalse));
	
	//CR1838 - Continuous Zoom Test
	AddTestStepL(RECamContinuousZoomAllocTest::NewL(EFalse));
	
	//CR1838 - Continuous Zoom Test
	AddTestStepL(RECamContinuousZoomTest::NewL(EFalse));
	
	//CR1838 - Supported Drive Modes Test
	AddTestStepL(RECamDependantSupportedDriveModesTest::NewL(EFalse));
	}


void CTestSuite_ECam_ADV::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(ifsSession.Connect());
	
#ifdef SYMBIAN_MULTIMEDIA_THREEPLANEARCH
	ifsSession.Delete(KMMCameraPluginName);
#endif //SYMBIAN_MULTIMEDIA_THREEPLANEARCH
	ifsSession.Delete(KTestCameraPluginName);
	
	RFile file;
	TInt err = file.Replace(ifsSession, KECamUnitTestPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
		{
		User::LeaveIfError(KErrNotSupported);
		}
	}
	
void CTestSuite_ECam_ADV::RemoveDependencyFile()
	{
	// it will delete even if the file does not exist i.e. failed when creating
	ifsSession.Delete(KECamUnitTestPluginName);
	ifsSession.Close();
	}
