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
// This is the main DLL entry point for the TSU_MMF_SRSSNK.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
//#include <TestFramework.h>
#include "TSU_MMF_SRSSNKSuite.h"
#include "TSU_MMF_SRSSNK.h"



/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteBaseClassesL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuiteBaseClasses*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_SRSSNK* NewTestSuiteL() 
    { 
	CTestSuite_MMF_SRSSNK* result = new (ELeave) CTestSuite_MMF_SRSSNK;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 *
 * CTestSuiteBaseClasses destructor
 *
 * @xxxx
 * 
 */
CTestSuite_MMF_SRSSNK::~CTestSuite_MMF_SRSSNK()
	{
	}


/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 * @xxxx
 *
 */

TPtrC CTestSuite_MMF_SRSSNK::GetVersion() const
	{
	_LIT(KTxtVersion,"1.2");
	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite.
 *
 * @param	"CTestStepSelfTest* aPtrTestStep"
 *			The test step to be added.
 * @xxxx
 * 
 */
void CTestSuite_MMF_SRSSNK::AddTestStepL(CTestStep_MMF_SRSSNK* aPtrTestStep)
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this); 

	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}


/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteBaseClasses
 *
 * @xxxx
 * 
 */
void CTestSuite_MMF_SRSSNK::InitialiseL()
	{
	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_SRSSNK");

	// Add test steps:
	// Descriptor tests.
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0001);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0002);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0003);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0004);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0005);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0006);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0007);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0008);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0009);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0010);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0011);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0012);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0013);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0014);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0015);

	// File tests.
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0101);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0102);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0103);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0104);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileSourceMisc(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileFillBuffer(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0107);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileReadBuffer(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0109);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileSinkMisc(ESinkTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileEmptyBuffer(ESinkTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0112);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileWriteBuffer(ESinkTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0114);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0115);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileEvaluateIntent(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileExecuteIntent(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileIsProtected(ESourceTypeFile));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0119);

	// File Handle Tests
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileSourceMisc(ESourceTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileFillBuffer(ESourceTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileReadBuffer(ESourceTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileSinkMisc(ESinkTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileEmptyBuffer(ESinkTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileWriteBuffer(ESinkTypeFileHandle));
	
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileEvaluateIntent(ESourceTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileExecuteIntent(ESourceTypeFileHandle));
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_FileIsProtected(ESourceTypeFileHandle));

	// Audio tests.
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0201);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0202);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0203);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0204);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0205);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0206);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0207);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0208);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0209);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0210);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0211);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0212);

	// Extra tests attempting to recreate TeamTrack defect DEF001170
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0301);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0302);


	// URL Tests
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0400);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0405);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0406);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0407);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0408);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0409);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0410);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0411);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0412);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0413);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0414);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0415);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0416);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0417);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0418);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0419);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0420);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0421);

	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0500);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0505);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0506);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0507);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0508);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0509);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0510);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0511);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0512);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0513);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0514);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0515);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0516);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0517);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0518);
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0519);
	
	// multiple source stop test
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_MultipleStop);
	
	// INC085391: CMMFFile::iFilePath and iFileDrive should be initialised for KMMFileHandleSource
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0600);
	
	//CR0854 - Provide access to the file data via the CData object 
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0521);
	
	//CR0854 - Provide access to the file data via the CData object 
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0522);

	// PDEF102849 - Invalid Remaining Space value obtained for E:\ (memory card)	
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0523);
	
	//PDEF137282 - Answering VoIP call doesn't work 
	AddTestStepL(new(ELeave) CTestStep_MMF_SRSSNK_U_0524);

	}
