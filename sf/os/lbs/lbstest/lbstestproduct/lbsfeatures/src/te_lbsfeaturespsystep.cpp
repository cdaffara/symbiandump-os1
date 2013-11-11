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
// The test for checking variability options for including/excluding PSYs and Location APIs 
// 
//

#include <e32base.h>
#include <lbs.h>

#ifdef SYMBIAN_FEATURE_MANAGER
#include <featureuids.h>
#include <featdiscovery.h>
#endif

#include "te_lbsfeaturespsystep.h"


_LIT(KGPSPositioningPluginFeature, "GPSPositioningPlugin");
_LIT(KAGPSPositioningPluginFeature, "AGPSPositioningPlugin");
_LIT(KNetworkPositioningPluginFeature, "NetworkPositioningPlugin");
_LIT(KDefaultPositioningPluginFeature, "DefaultPositioningPlugin");
_LIT(KLocationAPIVariant2Feature, "LocationAPIVariant2");
_LIT(KConfigUseGenerated, "ConfigUseGenerated");

CT_LbsFeaturesPsyStep* CT_LbsFeaturesPsyStep::New()
	{
	CT_LbsFeaturesPsyStep* testStep = new CT_LbsFeaturesPsyStep();
	if(!testStep)
		{
		return NULL;
		}
	
	TRAPD(err, testStep->ConstructL());
	if(err!=KErrNone)
		{
		delete testStep;
		return NULL;
		}
			
	return testStep;
	}

CT_LbsFeaturesPsyStep::CT_LbsFeaturesPsyStep()
	{
	SetTestStepName(KLbsFeaturesPsy);
	}

TVerdict CT_LbsFeaturesPsyStep::doTestStepL()
	{
	TESTL(GetBoolFromConfig(ConfigSection(), KConfigUseGenerated, iConfigUseGenerated));
	
	TestFeatureL(KLocationAPIVariant2Feature, NFeature::KLocationAPIVariant2);
	TestFeatureL(KGPSPositioningPluginFeature, NFeature::KGPSPositioningPlugin);
	TestFeatureL(KAGPSPositioningPluginFeature, NFeature::KAGPSPositioningPlugin);
	TestFeatureL(KNetworkPositioningPluginFeature, NFeature::KNetworkPositioningPlugin);
	TestFeatureL(KDefaultPositioningPluginFeature, NFeature::KDefaultPositioningPlugin);
	
	return TestStepResult();
	}

void CT_LbsFeaturesPsyStep::TestFeatureL(const TDesC& aFeatureName,  const TUid& aFeatureUid)
	{
	TBool featureExpected;
	TESTL(GetBoolFromConfig(ConfigSection(), aFeatureName, featureExpected));
	
	if(featureExpected)
		{
		INFO_PRINTF2(_L("The %S feature is expected to be present"), &aFeatureName);
		}
	else
		{
		INFO_PRINTF2(_L("The %S feature is expected to be absent"), &aFeatureName);
		}
		
#ifdef SYMBIAN_FEATURE_MANAGER
	TBool featureSupported = CFeatureDiscovery::IsFeatureSupportedL(aFeatureUid);
#else
	TBool featureSupported = ETrue; // the default is that the component is present
#endif	
				
	if(featureSupported)
		{
		INFO_PRINTF2(_L("The %S feature is present"), &aFeatureName);
		}
	else
		{
		INFO_PRINTF2(_L("The %S feature is absent"), &aFeatureName);
		}
	
	TESTL((featureExpected && featureSupported) || (!featureExpected && !featureSupported));
	
	TBool featureConsistent = ETrue;
	if(aFeatureUid == NFeature::KGPSPositioningPlugin)
		{
		featureConsistent =  (featureSupported==IsFilePresent(_L("z:\\resource\\lbsgpspsy_name.rsc"))) &&
							(featureSupported==IsFilePresent(_L("z:\\private\\10202be9\\101fe98a.txt")));
		}
	else if(aFeatureUid == NFeature::KAGPSPositioningPlugin)
		{
		featureConsistent =  (featureSupported==IsFilePresent(_L("z:\\resource\\lbsagpspsy_name.rsc"))) &&
							(featureSupported==IsFilePresent(_L("z:\\private\\10202be9\\101fe98c.txt")));
				
		}
	else if(aFeatureUid == NFeature::KNetworkPositioningPlugin)
		{
		featureConsistent = (featureSupported==IsFilePresent(_L("z:\\sys\\bin\\lbsnetpsy.dll"))) && 
							(featureSupported==IsFilePresent(_L("z:\\resource\\lbsnetpsy_name.rsc"))) &&
							(featureSupported==IsFilePresent(_L("z:\\private\\10202be9\\101fe98e.txt")));
				
		}
	else if(aFeatureUid == NFeature::KLocationAPIVariant2)
		{
		featureConsistent = (featureSupported==IsFilePresent(_L("z:\\sys\\bin\\lbs.dll"))) && 
							(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\eposserver.exe"))) &&
							(featureSupported==IsFilePresent(_L("z:\\private\\101f97b2\\eposserver.rsc"))) &&
							(featureSupported==IsFilePresent(_L("z:\\private\\101f97b2\\backup_registration.xml"))) &&
							(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\eposindicator.dll"))) &&
							(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\eposmodset.dll"))) &&
							(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\eposutils.dll"))) && 
							(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\epospluginfw.dll")));
		
		if(!iConfigUseGenerated)
			{
			featureConsistent = featureConsistent && 
				(featureSupported==IsFilePresent(_L("z:\\private\\10202be9\\101F500C.cre")));
			}
		}
	else if(aFeatureUid == NFeature::KDefaultPositioningPlugin)
		{
		featureConsistent = (featureSupported==IsFilePresent(_L("z:\\sys\\bin\\eposdefault.dll")));
		
		if(!iConfigUseGenerated)
			{
			featureConsistent = featureConsistent && 
				(featureSupported==IsFilePresent(_L("z:\\private\\10202be9\\101F7A7F.cre")));
			}
		}
	
	//The lbsgpsagpspsy.dll is checked separately because it implements two PSYs
	if(aFeatureUid == NFeature::KGPSPositioningPlugin || aFeatureUid == NFeature::KAGPSPositioningPlugin)
		{
#ifdef SYMBIAN_FEATURE_MANAGER
		TBool featureSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KGPSPositioningPlugin) ||
			CFeatureDiscovery::IsFeatureSupportedL(NFeature::KAGPSPositioningPlugin);
#else
		TBool featureSupported = ETrue;
#endif		
		featureConsistent = featureConsistent && 
			(featureSupported==IsFilePresent(_L("z:\\sys\\bin\\lbsgpsagpspsy.dll")));
		}
	
	
	if(featureConsistent)
		{
		INFO_PRINTF2(_L("The %S feature is consistent"), &aFeatureName);
		}
	else
		{
		INFO_PRINTF2(_L("The %S feature is not consistent"), &aFeatureName);
		}
	
	TESTL(featureConsistent);
	}



