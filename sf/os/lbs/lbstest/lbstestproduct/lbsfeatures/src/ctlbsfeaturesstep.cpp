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
// @file ctlbshybridmolrstep.cpp
// 
//

#include <e32std.h>
#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featureuids.h>
	#include <featdiscovery.h>
#endif
#include "ctlbsfeaturesstep.h"


CT_LbsFeaturesStep::~CT_LbsFeaturesStep()
	{
	iFs.Close();
	}


CT_LbsFeaturesStep::CT_LbsFeaturesStep()
	{
	}

void CT_LbsFeaturesStep::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsFeaturesStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPreambleL()"));

	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsFeaturesStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPostambleL()"));

	return TestStepResult();
	}

CT_LbsFeaturesStep::TFeaturePresence CT_LbsFeaturesStep::IsLocationManagementFeaturePresent(TBool aIsLocationManagementFeaturePresent)
	{
#ifdef SYMBIAN_FEATURE_MANAGER
	(void)aIsLocationManagementFeaturePresent;
	TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
	TBool locationManagementSupported(aIsLocationManagementFeaturePresent); // the default is that the component is present
#endif
	TFeaturePresence locationManagementPresence;
	locationManagementPresence = locationManagementSupported ? EFeaturePresent : EFeatureAbsent;
	if(locationManagementSupported)
		{
		INFO_PRINTF1(_L("Location Management feature is present"));
		}
	else
		{
		INFO_PRINTF1(_L("Location Management feature is absent"));
		}
	
	TBool consistent(ETrue);
	consistent = (locationManagementSupported == IsFilePresent(_L("z:\\sys\\bin\\lbslocserver.exe"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\sys\\bin\\lbsnetlocmanager.exe"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\private\\10282253\\lbs\\locmods\\networklocationmanager.ini"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\sys\\bin\\GPSClock.dll"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\sys\\bin\\GPSSetClock.dll"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\sys\\bin\\lbsgpslocmanager.exe"))) && 
					(locationManagementSupported == IsFilePresent(_L("z:\\private\\10202BE9\\10281D44.cre")));
	if(!consistent)
		{
		locationManagementPresence = EFeatureInconsistent;
		}
	
	return locationManagementPresence;
	}

CT_LbsFeaturesStep::TFeaturePresence CT_LbsFeaturesStep::IsLocationPrivacyRequestAPIsFeaturePresent(TBool aIsLocationPrivacyRequestAPIsFeaturePresent)
	{
#ifdef SYMBIAN_FEATURE_MANAGER
	(void)aIsLocationPrivacyRequestAPIsFeaturePresent;
	TBool locationPrivacyRequestAPIsSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationPrivacyRequestAPIs);
#else
	TBool locationPrivacyRequestAPIsSupported(aIsLocationPrivacyRequestAPIsFeaturePresent); // the default is that the component is not present
#endif
	TFeaturePresence locationPrivacyRequestAPIsPresence;
	locationPrivacyRequestAPIsPresence = locationPrivacyRequestAPIsSupported ? EFeaturePresent : EFeatureAbsent;

	if(locationPrivacyRequestAPIsSupported)
		{
		INFO_PRINTF1(_L("Location Privacy Request APIs feature is present"));
		}
	else
		{
		INFO_PRINTF1(_L("Location Privacy Request APIs feature is absent"));
		}

	TBool consistent(ETrue);
	consistent = (locationPrivacyRequestAPIsSupported == IsFilePresent(_L("z:\\sys\\bin\\eposnwprv.dll"))) && 
					(locationPrivacyRequestAPIsSupported == IsFilePresent(_L("z:\\sys\\bin\\lbsprivacyprotocolmodule.dll")));
	if(!consistent)
		{
		locationPrivacyRequestAPIsPresence = EFeatureInconsistent;
		}
	
	return locationPrivacyRequestAPIsPresence;
	}

CT_LbsFeaturesStep::TFeaturePresence CT_LbsFeaturesStep::IsBTPSYFeaturePresent()
	{
#ifdef SYMBIAN_FEATURE_MANAGER
	TBool btpsySupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KBluetoothGPSPositioningPlugin);
#else
	TBool btpsySupported(EFalse); // the default is that the component is not present
#endif
	TFeaturePresence locationPrivacyRequestAPIsPresence;
	locationPrivacyRequestAPIsPresence = btpsySupported ? EFeaturePresent : EFeatureAbsent;

	if(btpsySupported)
		{
		INFO_PRINTF1(_L("The BTPSY feature is present"));
		}
	else
		{
		INFO_PRINTF1(_L("The BTPSY feature is absent"));
		}

	TBool consistent(ETrue);
	consistent = (btpsySupported == IsFilePresent(_L("z:\\sys\\bin\\nlabtgpspsy.dll"))) && 
					(btpsySupported == IsFilePresent(_L("z:\\private\\10202be9\\101fe999.cre")));
	if(!consistent)
		{
		locationPrivacyRequestAPIsPresence = EFeatureInconsistent;
		}
	
	return locationPrivacyRequestAPIsPresence;
	}

CT_LbsFeaturesStep::TFeaturePresence CT_LbsFeaturesStep::IsLocationAdvancedDialogFeaturePresent(TBool aIsLocationAdvancedDialogFeaturePresent)
	{
#ifdef SYMBIAN_FEATURE_MANAGER
	(void)aIsLocationAdvancedDialogFeaturePresent;
	TBool locationAdvancedDialogSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationAdvancedDialog);
#else
	TBool locationAdvancedDialogSupported(aIsLocationAdvancedDialogFeaturePresent); // the default is that the component is not present
#endif
	TFeaturePresence locationAdvancedDialogPresence;
	locationAdvancedDialogPresence = locationAdvancedDialogSupported ? EFeaturePresent : EFeatureAbsent;
	if(locationAdvancedDialogSupported)
		{
		INFO_PRINTF1(_L("Location Advanced Dialog feature is present"));
		}
	else
		{
		INFO_PRINTF1(_L("Location Advanced Dialog feature is absent"));
		}

	TBool consistent(ETrue);
	consistent = (locationAdvancedDialogSupported == IsFilePresent(_L("z:\\sys\\bin\\eposprvtyp.dll"))) && 
					(locationAdvancedDialogSupported == IsFilePresent(_L("z:\\sys\\bin\\eposprvqnif.dll")));
	if(!consistent)
		{
		locationAdvancedDialogPresence = EFeatureInconsistent;
		}
	
	return locationAdvancedDialogPresence;
	}

TBool CT_LbsFeaturesStep::IsFilePresent(const TDesC& aFileName)
	{
	RFile file;
	TBool filePresent(KErrNone == file.Open(iFs, aFileName, EFileRead));
	file.Close();
	if(filePresent)
		{
		INFO_PRINTF2(_L("File %S is present in ROM"), &aFileName);
		}
	else
		{
		INFO_PRINTF2(_L("File %S is not present in ROM"), &aFileName);
		}
	return filePresent;
	}
