
// ECamPresetsTest.cpp

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
//

#include <ecamuids.hrh>
#include <ecom/ecomresolverparams.h>
#include "ECamPresetsTest.h"
#include "ECamUnitTestPluginUids.hrh"
#include "ECamUnitTestPlugin.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamadvsettingsuidsconst.hrh>
#include <ecamadvsettingsconst.h>
#include <ecamimageprocessingconst.h>
#endif
//
// RECamPresetsTest
//
RECamPresetsTest* RECamPresetsTest::NewL(TBool aAllocTest)
	{
	RECamPresetsTest* self = new (ELeave) RECamPresetsTest(aAllocTest);
	return self;	
	}
	
RECamPresetsTest::RECamPresetsTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-006-HP");
	}
	
TVerdict RECamPresetsTest::DoTestStepL()
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1(_L("Alloc test"));
	TInt i;
	TInt err;
	for (i = 1 ; ; i++)
		{
		__MM_HEAP_MARK;
		
		if (i % 5 == 0)
			{
			INFO_PRINTF2(_L("Fail count = %d"), i);
			}

		__UHEAP_SETFAIL(RHeap::EFailNext, i);

		TRAP(err, verdict = DoPresetsTestStepL());

		TAny* testAlloc = User::Alloc(1);
		TBool heapTestingComplete = (testAlloc == NULL) && (err==KErrNone);
		User::Free(testAlloc);

		__UHEAP_RESET;
		__MM_HEAP_MARKEND;

		if ((err != KErrNoMemory ) || heapTestingComplete)
			{
			INFO_PRINTF4(_L("err = %d, verdict = %d, Fail count = %d"), err, verdict, i);
			INFO_PRINTF1(_L("Alloc testing completed successfully"));
			verdict = EPass;
			break;
			}
		}
	return verdict;
	}
	
TVerdict RECamPresetsTest::DoPresetsTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPresets* presets = NULL;
		
	// using observer 2 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		presets = 	static_cast<CCamera::CCameraPresets*> (camera->CustomInterface(KECamPresetsUid));
	    if (presets!= NULL)
	    	{
	    	CleanupStack::PushL(presets);
	  		INFO_PRINTF1(_L("CCameraPresets object was created")); 
	    	CleanupStack::PopAndDestroy(presets);
	    	}
	    else
    		{
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	__MM_HEAP_MARKEND;
	
	// create a presets object using NewL
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, presets  = CCamera::CCameraPresets::NewL(*camera));	
		
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(presets);
	  		INFO_PRINTF1(_L("CCameraPresets object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(presets);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraPresets object was not created using NewL")); 
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;
		User::Leave(KErrNoMemory);		
		}
	__MM_HEAP_MARKEND;
   
	return result;
	}
	
//
// RECamPresetsNotificationTest
//
RECamPresetsNotificationTest* RECamPresetsNotificationTest::NewL(TBool aAllocTest)
	{
	RECamPresetsNotificationTest* self = new (ELeave) RECamPresetsNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamPresetsNotificationTest::ConstructL()
	{
	iOriginalSet.Reset();
	iResultSet.Reset();

	}
RECamPresetsNotificationTest::RECamPresetsNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-008-HP");
	}
	
TVerdict RECamPresetsNotificationTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPresets* presets = NULL;
		
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		//	CCamUnitTestPlugin* camera1 = static_cast<CCamUnitTestPlugin*>(camera);
		//  rely on public member, done for simplfication of an external 
		//  initialization as it is the simplest way
		//	camera1->iPresets = iOriginalSet;
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		presets = static_cast<CCamera::CCameraPresets*> (camera->CustomInterface(KECamPresetsUid));
	    if (presets!= NULL)
	    	{
	    	CleanupStack::PushL(presets);
	    	
	    	// get the supported set of values  
	    	presets->GetSupportedPresetsL(iResultSet);

	    	//verify that the input and output are the same
	    	INFO_PRINTF2(_L("camera supports %d presets"),  iResultSet.Count());
	       
	    	// the array should not be empty 
	        if (iResultSet.Count()==0)
	        	{
	        	result = EFail;
	        	}
        	else
        		{
        		iResultSet.Reset();
        		}
        		
        	INFO_PRINTF1(_L("Current preset shall be  KUidECamPresetNone"));	
	    	TUid currentPreset = presets->Preset();	    	
	    
	    	// Current Preset shall be KUidECamPresetNone.
	    	if(currentPreset.iUid != KUidECamPresetNoneUidValue)
	    		{
	    		INFO_PRINTF1(_L("Current preset wrongly set"));
	    		result = EFail;
	    		}
	    	
	    	// set new value
	    	iInputEventUid = KUidECamPresetBeach;
	        INFO_PRINTF2(_L("Select a new preset %d"), iInputEventUid.iUid); 
	        presets->SetPreset(iInputEventUid);
	        
	        CheckNotification(iInputEventUid, result);
    			
    		presets->GetAffectedSettingsL(iResultSet);
    		INFO_PRINTF2(_L("This preset affected %d settings"),  iResultSet.Count());
    		
    		// get the alternative function
    		RArray<TUid> associatedSettings;
    		CleanupClosePushL(associatedSettings);
    		associatedSettings.Reset();
    		INFO_PRINTF1(_L("Get associated settings for the selected preset"));
    		TRAP(error,presets->GetAssociatedSettingsL(iInputEventUid, associatedSettings));
			if(error != KErrNone)
				{
				CleanupStack::PopAndDestroy(&associatedSettings);
				}
				
			if (iResultSet.Count()!=associatedSettings.Count())
	        	{
	        	result = EFail;
	        	}
        	else
        		{
        		iResultSet.Reset();
        		}	
        	CleanupStack::PopAndDestroy(&associatedSettings);
    		
    		// set KUidECamPresetOutdoor to get range restricted settings.
	    	iInputEventUid = KUidECamPresetOutdoor;
	        INFO_PRINTF2(_L("Select the Outdoor preset %d"), iInputEventUid.iUid); 
	        presets->SetPreset(iInputEventUid);
	        
	        INFO_PRINTF1(_L("Check the RangeRestricted notification"));
    		CheckNotification(KUidECamEventRangeRestricted, result);
	        
	        if(result != EFail)
	        	{
	        	INFO_PRINTF1(_L("Retrieve the RangeRestricted settings"));
	        	// get the range restricted settings
	    		RArray<TUid> rangeRestrictedSettings;	
	    		presets->GetRangeRestrictedSettingsL(rangeRestrictedSettings);
	    		if (rangeRestrictedSettings.Count() == 0)
		        	{
		        	INFO_PRINTF1(_L("RangeRestricted settings are Zero"));
		        	result = EFail;
		        	}
	        	rangeRestrictedSettings.Close();
	        	}
	        
	        INFO_PRINTF1(_L("Check the FeatureRestricted settings"));
	        // get the feature restricted settings
	    	RArray<TUid> featureRestrictedSettings;
    		presets->GetFeatureRestrictedSettingsL(featureRestrictedSettings);
    		if (featureRestrictedSettings.Count() != 0)
	        	{
	        	INFO_PRINTF1(_L("FeatureRestricted settings are not Zero"));
	        	result = EFail;
	        	}
	        featureRestrictedSettings.Close();
	        
	        TBool unlockSupported = EFalse;
	       	presets->IsPresetUnlockSupportedL(unlockSupported);
	       	if(!unlockSupported)
	       		{
	       		INFO_PRINTF1(_L("Preset Unlock not supported"));
	       		result = EFail;
	       		}
	       	
	       	INFO_PRINTF1(_L("Unlock Preset"));	
	       	presets->UnlockPresetL();
	       	CheckNotification(KUidECamEventPresetUnlocked, result);
	       	
	       	INFO_PRINTF1(_L("Lock Preset"));	
	       	presets->LockPresetL();
	       	CheckNotification(KUidECamEventPresetLocked, result);
	       	
    		CleanupStack::PopAndDestroy(presets);
	    	}
	    else
    		{
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}
    	
    	CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);
		}
	
	__MM_HEAP_MARKEND;

	return result;
	}

//
// RECamPresetsNegTest //
//
RECamPresetsNegTest* RECamPresetsNegTest::NewL(TBool aAllocTest)
	{
	RECamPresetsNegTest* self = new (ELeave) RECamPresetsNegTest(aAllocTest);
	return self;	
	}
	
RECamPresetsNegTest::RECamPresetsNegTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-0101-HP");
	}
	
TVerdict RECamPresetsNegTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPresets* presets = NULL;		
	
	__MM_HEAP_MARK;
	
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*this,0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		presets = static_cast<CCamera::CCameraPresets*> (camera->CustomInterface(KECamPresetsUid));
	    if (presets!= NULL)
	    	{
	    	CleanupStack::PushL(presets);	   
	    	 	
			TUid aCurrentPreset = presets->Preset();
	    	// get the current value
	    	INFO_PRINTF2(_L("Current preset %x"), aCurrentPreset.iUid);
	    	
	    	// get the supported set of values      	
			RArray<TUid> aResultSet;
			CleanupClosePushL(aResultSet);
			aResultSet.Reset();
	    	presets->GetSupportedPresetsL(aResultSet);

	        // Setting a non supported preset should be rejected       
	        // In the test plugin only KUidECamPresetOutdoor, KUidECamPresetNightPartyIndoor & KUidECamPresetBeach are supported
	 	    iInputEventUid = KUidECamPresetIndoorSport;	    	
	    	presets->SetPreset(iInputEventUid);	    
   		    	
	        CheckNotificationNeg(iInputEventUid, result); 
	        
	        // Setting a non supported preset should be rejected       
	        // In the test plugin only KUidECamPresetOutdoor, KUidECamPresetNightPartyIndoor & KUidECamPresetBeach are supported
	 	    iInputEventUid = KUidECamPresetVideoTelephony;	    	
	    	presets->SetPreset(iInputEventUid);	    
   		    	
	        CheckNotificationNeg(iInputEventUid, result);
	           				    		
	    	TUid aGetPreset = presets->Preset();
	    	
	      	if ((aCurrentPreset == aGetPreset) && (aGetPreset == KUidECamPresetNone))
	        	{
	    		INFO_PRINTF2(_L("Current preset has not changed %x"), aGetPreset.iUid);	        	
	        	}
	        else
	        	{
				INFO_PRINTF1(_L("Expected no affected settings"));
				result = EFail;
	        	}	    	 	

	    	// get the affected settings	        	
			RArray<TUid> aAffectedSet;
			CleanupClosePushL(aAffectedSet);
			presets->GetAffectedSettingsL(aAffectedSet);	    	    		     
	        // the list of affected settings should be empty 
	        if (aAffectedSet.Count()!=0)
	        	{
				INFO_PRINTF1(_L("Expected no affected settings"));
				result = EFail;
	        	}
	        else
	        	{
				INFO_PRINTF1(_L("No affected settings"));	        		
	        	}
	        	
			CleanupStack::PopAndDestroy(2, &aResultSet); //aAffectedSet
	    	
	    	CleanupStack::PopAndDestroy(presets);
	    	}
	    else // preset is NULL
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else // error != KErrNone
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
	
//
// RECamPresetsBaselineTest
//
RECamPresetsBaselineTest* RECamPresetsBaselineTest::NewL(TBool aAllocTest)
	{
	RECamPresetsBaselineTest* self = new (ELeave) RECamPresetsBaselineTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamPresetsBaselineTest::ConstructL()
	{
	iOriginalSet.Reset();
	iResultSet.Reset();

	}
RECamPresetsBaselineTest::RECamPresetsBaselineTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-046-HP");
	}

TVerdict RECamPresetsBaselineTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraPresets* presets = NULL;
		
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::NewL(*this,0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		//	CCamUnitTestPlugin* camera1 = static_cast<CCamUnitTestPlugin*>(camera);
		//  rely on public member, done for simplfication of an external 
		//  initialization as it is the simplest way
		//	camera1->iPresets = iOriginalSet;
		INFO_PRINTF1(_L("KErrNone return from CCamera::NewL()"));
		presets = static_cast<CCamera::CCameraPresets*> (camera->CustomInterface(KECamPresetsUid));
	    if (presets!= NULL)
	    	{
	    	CleanupStack::PushL(presets);
	    	
	    	// get the supported set of values  
	    	presets->GetSupportedPresetsL(iResultSet);

	    	//verify that the input and output are the same
	    	INFO_PRINTF2(_L("camera supports %d presets"),  iResultSet.Count());
	       
	    	// the array should not contain any unfamiliar uid for preset(after baseline) 
	        for(TInt index =0; index < iResultSet.Count(); index++)
				{
				/** KUidECamPresetFactoryDefaultUidValue is the baseline. Any preset with greater uid value means that it has 
				been added in later versions */
				if(iResultSet[index].iUid > KUidECamPresetFactoryDefaultUidValue)
					{
					result = EFail;
					}
				}
	        	        
	        iResultSet.Reset();
        	
	    	presets->Preset();	    	
	    	// set new unfamiliar value
	    	iInputEventUid = KUidECamPresetNightPartyIndoor; 
	        INFO_PRINTF2(_L("Select a new preset %d"), iInputEventUid.iUid); 
	        presets->SetPreset(iInputEventUid);
	        
	        CheckNotification(iInputEventUid, result);
	        
	        if(presets->Preset().iUid != KUidECamPresetFactoryDefaultUidValue)
	        	{
	        	result = EFail;
	        	}
    			
    		presets->GetAffectedSettingsL(iResultSet);
    		INFO_PRINTF2(_L("This preset affected %d settings"),  iResultSet.Count());
    		
    		// the array should not contain any unfamiliar uid for settings(after baseline) 
	        for(TInt index =0; index < iResultSet.Count(); index++)
				{
				/** KUidECamEventCameraSettingAutoFocusType2UidValue is the baseline. Any settings with greater uid value means that it has 
				been added in later versions */
				if(iResultSet[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
					{
					result = EFail;
					}
				}
    		
    		// get the alternative function
    		RArray<TUid> associatedSettings;
    		
    		presets->GetAssociatedSettingsL(iInputEventUid, associatedSettings);

			if (iResultSet.Count()!=associatedSettings.Count())
	        	{
	        	result = EFail;
	        	}
        	else
        		{
        		iResultSet.Reset();
        		}	
    		associatedSettings.Close();
	    	CleanupStack::PopAndDestroy(presets);
	    	}
	    else
    		{
    		result = EFail;
    		}
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::NewL(): observer 2"), error);
		result = EFail;		
		}
	
	__MM_HEAP_MARKEND;

	return result;
	}
