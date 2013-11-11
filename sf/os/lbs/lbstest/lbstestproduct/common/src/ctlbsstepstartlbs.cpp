// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbslocdatasrc.cpp
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// LBS includes. 
// 
//

// LBS test includes.
#include "ctlbsstepstartlbs.h"
#include <lbs/test/tlbsutils.h>
#include <lbs/test/lbstestclient.h>

#define TEST_AND_LOG(err) if(err!=KErrNone) {INFO_PRINTF2(_L("err = %d"),err);User::Leave(err);}
/**
 * Destructor
 */
CT_LbsStep_StartLbs::~CT_LbsStep_StartLbs()
	{
	}


/**
 * Constructor
 */
CT_LbsStep_StartLbs::CT_LbsStep_StartLbs(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_StartLbs);
	}


/**
Static Constructor
*/
CT_LbsStep_StartLbs* CT_LbsStep_StartLbs::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_StartLbs(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_StartLbs::doTestStepL()
	{
	// Test step used to start the location manager.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_StartLbs::doTestStepL()"));

	//*******************************************************************/
	// for diagnostics print out important Admin settings that will
	// determine how lbs bahaves
	TInt err;
	CLbsAdmin* adminApi = NULL;
	TRAP(err, adminApi = CLbsAdmin::NewL());
	TEST_AND_LOG(err);
	CleanupStack::PushL(adminApi);
	
	CLbsAdmin::TGpsMode mode;
	err = adminApi->Get(KLbsSettingHomeGpsMode, mode);	
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("KLbsSettingHomeGpsMode = %d"),mode);
		}
	else
		{
		INFO_PRINTF1(_L("KLbsSettingHomeGpsMode not in Admin"));
		}
			
	CLbsAdmin::TSpecialFeature	specialFeature;
	
	err = adminApi->Get(KLbsSpecialFeatureIgnoreAccuracy, specialFeature);
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("KLbsSpecialFeatureIgnoreAccuracy = %d"),specialFeature);
		}
	else
		{
		INFO_PRINTF1(_L("KLbsSpecialFeatureIgnoreAccuracy not in Admin"));
		}
	
	err = adminApi->Get(KLbsSpecialFeatureAllowTBFinalNetPos, specialFeature);
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("KLbsSpecialFeatureAllowTBFinalNetPos = %d"),specialFeature);
		}
		else
		{
		INFO_PRINTF1(_L("KLbsSpecialFeatureAllowTBFinalNetPos not in Admin"));
		}
		
	err = adminApi->Get(KLbsSpecialFeatureWithholdDeliveryOfReferencePositions, specialFeature);
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("KLbsSpecialFeatureWithholdDeliveryOfReferencePositions = %d"),specialFeature);
		}
		else
		{
		INFO_PRINTF1(_L("KLbsSpecialFeatureWithholdDeliveryOfReferencePositions not in Admin"));
		}
		
	err = adminApi->Get(KLbsSpecialFeatureIntermediateFutileUpdate, specialFeature);
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("KLbsSpecialFeatureIntermediateFutileUpdate = %d"),specialFeature);
		}
		else
		{
		INFO_PRINTF1(_L("KLbsSpecialFeatureIntermediateFutileUpdate not in Admin"));
		}


	CleanupStack::PopAndDestroy(adminApi);

	//*******************************************************************/
	
	if (TestStepResult() == EPass)
		{	
		// Create the test properties.
		T_LbsUtils utils;
		TRAP(err,utils.CreateTestPropertiesL());
		TEST_AND_LOG(err);
		
		// Now start the LBS system
		
		CLbsTestServer* testServer = NULL;
		TRAP(err,testServer = CLbsTestServer::NewL());
		TEST_AND_LOG(err);
		CleanupStack::PushL(testServer);
		
		err = testServer->StartLbsSystem();
		TEST_AND_LOG(err);
		CleanupStack::PopAndDestroy(testServer);
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_StartLbs::doTestStepL()"));

	return TestStepResult();
	}
