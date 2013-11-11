// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit test that tests tests support for the Common TSY features in Feature Manager
// 
//

/**
 @file 
*/

#include "cctsyfeatmgr.h"
#include "tmockltsydata.h"
#include <featmgr/featurecontrol.h>
#include <featureuids.h>

CTestSuite* CCTsyFeatMgr::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyFeatMgr, TestFeatMgr0001L);
	
	END_SUITE;
	}

//
// Actual test cases
//

/**
@SYMTestCaseID BA-CTSY-FEAT-FEATMGR-0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support for CommonTSY features in Feature Manager
@SYMTestPriority High
@SYMTestActions Opens connection to Feature Manager and queries support for CsVideoTelephony, EmergencyCallsEnabledInOfflineMode and DriveModeCanRestrictMtCalls features
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFeatMgr::TestFeatMgr0001L()
	{
	TInt allFeaturesSupported(KFeatureSupported);
	TBool allFeaturesFound(ETrue);
	RFeatureControl featureControl;
	TInt ret = featureControl.Open();
	if (ret != KErrNone)
		{
		ERR_PRINTF1(_L("CCTsyFeatMgr::TestFeatMgr0001 - Could not connect to Feature Manager"));
		}
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(featureControl);
	ret = featureControl.FeatureSupported(NFeature::KCsVideoTelephony); 
	if (ret != KFeatureSupported)
		{
		ERR_PRINTF1(_L("CCTsyFeatMgr::TestFeatMgr0001 - No support for CsVideoTelephony"));
		allFeaturesSupported = KFeatureUnsupported;
        if (ret == KErrNotFound)
            {
            ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - CsVideoTelephony (UID 0x%x) not found in features.dat"),NFeature::KCsVideoTelephony);
            allFeaturesFound = EFalse;
            }
		else if (ret == KFeatureUnsupported)
			{
			ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - CsVideoTelephony (UID 0x%x) is disabled according to features.dat"),NFeature::KCsVideoTelephony);
			}		
		}
	ret = featureControl.FeatureSupported(NFeature::KEmergencyCallsEnabledInOfflineMode); 
	if (ret != KFeatureSupported)
		{
		ERR_PRINTF1(_L("CCTsyFeatMgr::TestFeatMgr0001 - No support for EmergencyCallsEnabledInOfflineMode"));
		allFeaturesSupported = KFeatureUnsupported;
        if (ret == KErrNotFound)
            {
            ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - EmergencyCallsEnabledInOfflineMode (UID 0x%x) not found in features.dat"),NFeature::KEmergencyCallsEnabledInOfflineMode);
            allFeaturesFound = EFalse;
            }
		else if (ret == KFeatureUnsupported)
			{
			ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - EmergencyCallsEnabledInOfflineMode (UID 0x%x) is disabled according to features.dat"),NFeature::KEmergencyCallsEnabledInOfflineMode);
			}					
		}	
	ret = featureControl.FeatureSupported(NFeature::KDriveModeCanRestrictMtCalls); 
	if (ret != KFeatureSupported)
		{
		ERR_PRINTF1(_L("CCTsyFeatMgr::TestFeatMgr0001 - No support for DriveModeCanRestrictMtCalls"));
		allFeaturesSupported = KFeatureUnsupported;
        if (ret == KErrNotFound)
            {
            ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - DriveModeCanRestrictMtCalls (UID 0x%x) not found in features.dat"),NFeature::KDriveModeCanRestrictMtCalls);
            allFeaturesFound = EFalse;
            }
		else if (ret == KFeatureUnsupported)
			{
			ERR_PRINTF2(_L("CCTsyFeatMgr::TestFeatMgr0001 - DriveModeCanRestrictMtCalls (UID 0x%x) is disabled according to features.dat"),NFeature::KDriveModeCanRestrictMtCalls);
			}           
		}	

	// features not found by Feature Manager indicates that we are likely to be using an outdated
	// feature manager file (features.dat or featuredatabase.xml), with old or incorrect feature UIDs.
    ASSERT_TRUE(allFeaturesFound);
	ASSERT_EQUALS(KFeatureSupported, allFeaturesSupported);
	
	INFO_PRINTF1(_L("CCTsyFeatMgr::TestFeatMgr0001 - SUCCESS: All required features are supported"));
    CleanupStack::PopAndDestroy(&featureControl);
	}
