// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "tsu_ecm_ram_testsuite.h"

#include "rampluginload.h"

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_ECam_RAM* NewTestSuiteL() 
    { 
	CTestSuite_ECam_RAM* self = new(ELeave) CTestSuite_ECam_RAM;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 
	return self;
    }

/**
 * Destructor
 */
CTestSuite_ECam_RAM::~CTestSuite_ECam_RAM()
	{
	RemoveDependencyFile();
	}

/**
 * Get Test Suite version
 */
TPtrC CTestSuite_ECam_RAM::GetVersion()
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();
	}

/**
 * This function initialises the test suite
 * this creates all the test steps and stores them inside CTestSuite_ECam_RAM
 */
void CTestSuite_ECam_RAM::InitialiseL()
	{
	CreateDependencyFileL();
	// store the name of this test suite 
	iSuiteName = _L("TSU_ECM_RAM");

	AddTestStepL(RRamPluginLoadTest::NewL(EFalse));
	}

void CTestSuite_ECam_RAM::CreateDependencyFileL()
	{
	// enforce dependency
	// create the file the plugin depends on.
	User::LeaveIfError(ifsSession.Connect());
	
#ifdef SYMBIAN_MULTIMEDIA_THREEPLANEARCH
	ifsSession.Delete(KMMCameraPluginName);
#endif //SYMBIAN_MULTIMEDIA_THREEPLANEARCH
	ifsSession.Delete(KECamRamPluginName);
	ifsSession.Delete(KTestCameraPluginName);	

	RFile file;
	TInt err = file.Replace(ifsSession, KECamRamPluginName, EFileShareAny);
  	file.Close();
  	if(err != KErrNone)
		{
		User::LeaveIfError(KErrNotSupported);
		}
	}
	
void CTestSuite_ECam_RAM::RemoveDependencyFile()
	{
	// it will delete even if the file does not exist i.e. failed when creating
	ifsSession.Delete(KECamRamPluginName);
	ifsSession.Close();
	}
