
// ECamAdvSetTest.cpp

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

#include <fbs.h>

#include <ecamuids.hrh>
#include "ECamUnitTestPluginUids.hrh"
#include "ECamAdvSetTest.h"
#include <ecam/ecamplugin.h>

#include <ecom/ecomresolverparams.h>
#include "ECamUnitTestPlugin.h"
#include "AdvancedSettings.h"
#include <ecam/cameraoverlay.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamconst.h>
#include <ecamadvsettingsconst.h>
#endif

const TInt KShutterSpeed = 20;
const TInt KIsoRate01  	= 320;
const TInt KIsoRate0   	= 150;
const TInt KValue       = 100;
const TInt KContinuousAutoFocusTimeoutValue1 = 10000;
//const TRgb KRgbConst = TRgb(100,124,130);
const TInt KWBColorTemperature1       = 4300;
const TInt KDefaultIsoRating    	  = 200;
const TInt KDefaultAperture					= 280;
const TInt KAllPreCaptureWarnings			= 0x3FFF;
const TInt KBaselinedFocusRanges 			= 0x1F;
const TInt KBaselinedWhiteBalanceModes 		= 0x01FF;
const TInt KFlashCompensationStep			= 2;
const TInt KFlashCompensationInSteps		= 1;
const TInt KExposureCompensationStep		= 3;
const TInt KExposureCompensationInSteps		= 2;
const TInt KAutoISOTypes					=0x1E;
/*
const TInt KDefaultValue					= 0;
const TInt KContinuousAutoFocusTimeoutValue = 20000;
const TInt KBrightnessAdjMinValue			= -50;
const TInt KBrightnessAdjMaxValue			= 50;

const TInt KMaxIsoRate						= 200;
const TInt KMinIsoRate						= 0;
const TInt KDefaultShutterSpeed				= 0;
const TInt KDefaultFocusDistance			= 0;

const TInt KBracketStartIndex				= 0;
const TInt KBracketFrames					= 2;
const TBool KDefaultRedEyeReduce			= EFalse;

const TBool KExternalFlashPresentState		= EFalse;
const TInt KManualFlashPowerLevel			= 0;
const TBool KApertureExposureLockOn         = EFalse;
const TBool KShootClickOn					= EFalse;
const TInt KTimerInterval					= 0;
const TInt KBurstImages						= 0;
const TBool KExposureLockOn         		= EFalse;
const TBool KAutoFocusLockOn         		= EFalse;
const TBool KAutomaticSizeSelectionChangeOn	= EFalse;
const TInt KWBColorTemperature				= 6000;
*/
//
// RECamAdvSetTest
//
RECamAdvSetTest* RECamAdvSetTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetTest* self = new (ELeave) RECamAdvSetTest(aAllocTest);
	return self;	
	}
	
RECamAdvSetTest::RECamAdvSetTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-005-HP");
	}
	
TVerdict RECamAdvSetTest::DoTestStepL()	
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

		TRAP(err, verdict = DoAdvTestStepL());

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
	
TVerdict RECamAdvSetTest::DoAdvTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
		
	// using observer 2 if exist 
	MCameraObserver2* observer2 = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	    	INFO_PRINTF1(_L("CCameraAdvancedSettings object was created")); 
	    	CleanupStack::PopAndDestroy(settings);
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
	
	// create a settings object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("KErrNone return from CCamera::New2L()"));
		TRAP(error, settings  = CCamera::CCameraAdvancedSettings::NewL(*camera));	
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(settings);
	  		INFO_PRINTF1(_L("CCameraAdvancedSettings object was created using NewL")); 
	    	CleanupStack::PopAndDestroy(settings);
	    	}
	    else
    		{
    		INFO_PRINTF1(_L("CCameraAdvancedSettings object was not created using NewL")); 
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

RECamAdvSetNotificationTest* RECamAdvSetNotificationTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetNotificationTest* self = new (ELeave) RECamAdvSetNotificationTest(aAllocTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
void RECamAdvSetNotificationTest::ConstructL()
	{
	iError = KErrNone;
	}
	
RECamAdvSetNotificationTest::RECamAdvSetNotificationTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-009-HP");
	}

TVerdict RECamAdvSetNotificationTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;
	
	TInt error = KErrNone;
	TInt cameraIndex = 1;
	TBool cameraPresent = EFalse;
		
	__MM_HEAP_MARK;
	TInt cameras = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("Available %d cameras"), cameras);
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		MCamManagement* extFunct = NULL;

	    // get the MCamera management inteface implementation
	    // remember it is for the purpose of this test written to 
	    // notify this camera clients... it should be a static(singlenton) 
	    // to cater for the general case, but this is just a test, not impl example. 
		extFunct = static_cast<CCamManagement*> 
			(camera->CustomInterface(KUidCameraManagementUid));
			
	    if (extFunct != NULL)
	    	{
	    	CleanupStack::PushL(extFunct);
	    	
	    	CCamera::CCameraAdvancedSettings* settings = static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	 		CleanupStack::PushL(settings);
	 		
	 		// enumerate camera types
	 		for (TInt index = 0; index < cameras; index++)
	 			{
	 			CCamera::CCameraAdvancedSettings::TCameraType type = settings->CameraType(index);
	 			INFO_PRINTF3(_L("camera with index %d type %d"), index, type);
	 			if (type == CCamera::CCameraAdvancedSettings::ECameraUnknown)
	 				{
	 				result = EFail;
	 				}
	 			}
	 			
	 		// check that indexed and non indexed versions work produce the same result
	 		INFO_PRINTF1(_L("Check that indexed and non indexed versions work in the same way"));
	 		TInt thisCameraIndex = 	settings->CameraIndex();
	 		// camera type
	       	if (settings->CameraType() != settings->CameraType(thisCameraIndex))
	       		{
	       		INFO_PRINTF2(_L("CameraType() and CameraType(%d) produce different results"),thisCameraIndex);
	       		result = EFail;
	       		}
	       	// for this camera
			settings->IsCameraPresent();
			if (settings->IsCameraPresent() != settings->IsCameraPresent(thisCameraIndex))
	       		{
	       		INFO_PRINTF2(_L("IsCameraPresent() and IsCameraPresent(%d) produce different results"),thisCameraIndex);
	       		result = EFail;
	       		}
	       		
	       	// verify that the required camera is not present - different index	
	 		cameraPresent = settings->IsCameraPresent(cameraIndex);
	 		INFO_PRINTF3(_L("camera with index %d %d"), cameraIndex, cameraPresent);
	    	if (cameraPresent)
	    		{
	    		result = EFail;
	    		}
	    		
	    	INFO_PRINTF2(_L("Plug in camera with index %d"), cameraIndex);
	    	extFunct->PlugCameraIn(cameraIndex);
	    	CheckNotification(KUidECamEventGlobalCamera01PluggedIn, result);	    	
    		
    		cameraPresent = settings->IsCameraPresent(cameraIndex);	
    		INFO_PRINTF3(_L("camera with index %d %d"), cameraIndex, cameraPresent);
    		if (!cameraPresent)
	    		{
	    		result = EFail;
	    		}
	    			
    		INFO_PRINTF2(_L("Plug out camera with index %d"), cameraIndex);
    		extFunct->PlugCameraOut(cameraIndex);
    	    CheckNotification(KUidECamEventGlobalCamera01PluggedOut, result);
    	        		
    		cameraPresent = settings->IsCameraPresent(cameraIndex);
    		INFO_PRINTF3(_L("camera with index %d %d"), cameraIndex, cameraPresent);	
    		if (cameraPresent)
	    		{
	    		result = EFail;
	    		}
	    		
	    	CleanupStack::PopAndDestroy(settings);	
	    	
	    	CleanupStack::Pop(extFunct);
	    	extFunct->Release();
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("Could not obtain camera management interface?"));
	    	result = EFail;
	    	}
	    CleanupStack::PopAndDestroy(camera);	
		}
	    			
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;
	return result;
	}
	
	
//
// RECamAdvSetDefaultsTest //
//
RECamAdvSetDefaultsTest* RECamAdvSetDefaultsTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetDefaultsTest* self = new (ELeave) RECamAdvSetDefaultsTest(aAllocTest);
	return self;	
	}

RECamAdvSetDefaultsTest::RECamAdvSetDefaultsTest(TBool)
	{
	iTestStepName = _L("MM-ECM-ADV-U-010-HP");
	}
	
TVerdict RECamAdvSetDefaultsTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	    	
	    	// Supported Focus Modes. Set default Focus Mode. Get Focus Mode.
			iInputEventUid = KUidECamEventCameraSettingFocusMode;
	    	TInt suppFocusModes = settings->SupportedFocusModes();    	
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Focus Modes 0x%x"), suppFocusModes); 
	    	CCamera::CCameraAdvancedSettings::TFocusMode defaultFocusMode = CCamera::CCameraAdvancedSettings::EFocusModeAuto;
			settings->SetFocusMode(defaultFocusMode);
			
	        CheckNotification(iInputEventUid, result);

	    	CCamera::CCameraAdvancedSettings::TFocusMode retrievedFocusMode = settings->FocusMode();  			
	    	if (defaultFocusMode != retrievedFocusMode) 
	    		{
	   		 	INFO_PRINTF3(_L("CCameraAdvancedSettings retrieved Focus Mode 0x%x is not the default 0x%x "), 
	   		 					retrievedFocusMode, defaultFocusMode); 
	    		result = EFail;
	    		}

	    	// Supported Focus Range. Set default Focus Range. Get Focus Range.
			iInputEventUid = KUidECamEventCameraSettingFocusRange;	    	
	    	TInt suppFocusRanges = settings->SupportedFocusRanges();    	
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Focus Ranges 0x%x"), suppFocusRanges); 
	    	CCamera::CCameraAdvancedSettings::TFocusRange defaultFocusRange = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
			settings->SetFocusRange(defaultFocusRange);
			
			// Check for KUidECamEventCameraSettingFocusRange.
	        CheckNotification(iInputEventUid, result);

	    	CCamera::CCameraAdvancedSettings::TFocusRange retrievedFocusRange = settings->FocusRange();
	    	if (defaultFocusRange != retrievedFocusRange) 
	    		{
	   		 	INFO_PRINTF3(_L("CCameraAdvancedSettings retrieved Focus Range 0x%x is not the default 0x%x"), 
	   		 					retrievedFocusRange, defaultFocusRange); 
	    		result = EFail;
	    		}
	    		
	        // Check for KUidECamEventCameraSettingFocusRangeTwo.
	        iInputEventUid = KUidECamEventCameraSettingFocusRange2;
			CheckNotificationTwo(iInputEventUid, result);
				    	    	
	    	// Supported Auto Focus Type. Set default Auto Focus Type. Get Auto Focus Type.
			iInputEventUid = KUidECamEventCameraSettingAutoFocusType;	    	
	    	TInt suppAutoFocusTypes = settings->SupportedAutoFocusTypes();   
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Focus Types 0x%x"), suppAutoFocusTypes); 	    		
	    	CCamera::CCameraAdvancedSettings::TAutoFocusType defaultAutoFocusType = CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff;
			settings->SetAutoFocusType(defaultAutoFocusType);

	        // Check for KUidECamEventCameraSettingAutoFocusType.
	        CheckNotification (iInputEventUid, result);
	    	
	    	CCamera::CCameraAdvancedSettings::TAutoFocusType retrievedAutoFocusType = settings->AutoFocusType() ;
	    	if (defaultAutoFocusType!=retrievedAutoFocusType) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings retrieved Auto Focus Type 0x%x is not the default 0x%x"), 
	    					retrievedAutoFocusType, defaultAutoFocusType);
 	    		result = EFail;
	    		}
	    	
	    	// Check for KUidECamEventCameraSettingAutoFocusTypeTwo
			iInputEventUid = KUidECamEventCameraSettingAutoFocusType2;	    	
	    	CheckNotificationTwo(iInputEventUid, result);	
	    				
	    	// Supported Auto Focus Area. Set default Auto Focus Area. Get Auto Focus Area.
			iInputEventUid = KUidECamEventCameraSettingAutoFocusArea;	    	
	    	TInt suppAutoFocusArea = settings->SupportedAutoFocusAreas();    	
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Focus Areas 0x%x"), suppAutoFocusArea); 	    		
	    	CCamera::CCameraAdvancedSettings::TAutoFocusArea defaultAutoFocusArea = CCamera::CCameraAdvancedSettings::EAutoFocusTypeAuto;
			settings->SetAutoFocusArea(defaultAutoFocusArea);

	        CheckNotification(iInputEventUid, result);
	        
	    	CCamera::CCameraAdvancedSettings::TAutoFocusArea retrievedAutoFocusArea = settings->AutoFocusArea() ;
	    	if (defaultAutoFocusArea!=retrievedAutoFocusArea) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings retrieved Auto Focus Area  0x%x is not the default 0x%x"), 
	    					retrievedAutoFocusArea, defaultAutoFocusArea); 
	    		result = EFail;
	    		}	
	    		
	    	// Focus distance
			iInputEventUid = KUidECamEventCameraSettingFocusDistance;	    	
	    	TInt dummyFocusDistance = 3;
			settings->SetFocusDistance(dummyFocusDistance);

	        CheckNotification(iInputEventUid, result);
	    	
	    	TInt retrievedFocusDist = settings->FocusDistance();  
	    	if (dummyFocusDistance!=retrievedFocusDist) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Focus distance  0x%x is not as expected 0x%x"), 
	    					dummyFocusDistance, retrievedFocusDist); 
	    		result = EFail;
	    		}	
	    			    			
	    	// Min Focal Length
	    	TInt minFocalLength = settings->GetMinFocalLength();    
	    	if (KErrNotSupported!=minFocalLength) 
	    		{
	    		INFO_PRINTF1(_L("Supported min Focal Length"));
	    		result = EFail;
	    		} 	    	
   		
			// Supported Iso Rates. Set default Iso Rate. Get Iso Rate. OLD API
			iInputEventUid = KUidECamEventCameraSettingIsoRate;
			RArray<TInt> isoRates;
			TInt suppIsoRates = 0;
			settings->GetSupportedIsoRatesL(isoRates);  
			suppIsoRates =  isoRates[0];
			isoRates.Close();
			INFO_PRINTF2(_L("CCameraAdvancedSettings supported Iso Rates 0x%x"), suppIsoRates); 	    		
			TInt defaultIsoRate = KDefaultIsoRating;
			settings->SetIsoRate(defaultIsoRate);    

			CheckNotification(iInputEventUid, result);
	        
	    	TInt retrievedIsoRate = settings->IsoRate();    		
	    	if (defaultIsoRate!=retrievedIsoRate) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate %d is not as expected %d"), 
	    					defaultIsoRate, retrievedIsoRate); 
	    		result = EFail;
	    		}
	    
	    	CCamera::CCameraAdvancedSettings::TISORateType isoRateType; 
			TInt param =0;
			TInt isoRate =0;
	    	settings->GetISORateL(isoRateType, param, isoRate);
	    	
	    	if (isoRateType != CCamera::CCameraAdvancedSettings::EISOManual) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings New and Old API for Iso Rate Type not consistent  0x%x is not as expected 0x%x"), 
	    					isoRateType, CCamera::CCameraAdvancedSettings::EISOManual); 
	    		result = EFail;
	    		}
	    	
	    	if (defaultIsoRate != isoRate) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings New and Old API for Iso Rate not consistent %d is not as expected %d"), 
	    					isoRate, defaultIsoRate); 
	    		result = EFail;
	    		}
	    			
	       	// Supported Iso Types. Set default Iso Type. Set Default ISO Rate. Get Iso Type and Rate. New API
	    	iInputEventUid = KUidECamEventCameraSettingIsoRateType;
			
			TInt supportedISORateTypes = 0;
			settings->GetSupportedISORateTypeL(supportedISORateTypes);  
			
			INFO_PRINTF2(_L("CCameraAdvancedSettings supported Iso Rate Types 0x%x"), supportedISORateTypes); 	    		
			defaultIsoRate = KDefaultIsoRating;
			CCamera::CCameraAdvancedSettings::TISORateType defaultIsoRateType = CCamera::CCameraAdvancedSettings::EISOManual;
			if(defaultIsoRateType & supportedISORateTypes)
				{
				settings->SetISORateL(defaultIsoRateType, defaultIsoRate);
				
				CheckNotification(iInputEventUid, result);
				
				param =0;
				isoRate =0;	
							
				settings->GetISORateL(isoRateType, param, isoRate);
				
				if(isoRateType != defaultIsoRateType)
					{
					INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate Type 0x%x is not as expected 0x%x"), 
	    					isoRateType, defaultIsoRateType); 
	    			result = EFail;
					}
					
				if(isoRate != defaultIsoRate)
					{
					INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate %d is not as expected %d"), 
	    					isoRate, defaultIsoRate); 
	    			result = EFail;
					}
				}
				    	
	    	// Supported apertures. Set and Get aperture.
	    	iInputEventUid = KUidECamEventCameraSettingAperture;
	    	RArray<TInt> suppApertures; 
	    	TValueInfo valueInfo = ENotActive;
	    	settings->GetAperturesL(suppApertures, valueInfo);   
			// should not have entries
			if (suppApertures.Count() != 0)
				{
				result = EFail;
				}	   
				 	
			TInt dummyAperture = KDefaultAperture;
			settings->SetAperture(dummyAperture);	    	 	
	        
	        CheckNotification(iInputEventUid, result);

	    	TInt aperture = settings->Aperture() ;
	    	if (dummyAperture != aperture) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Aperture  0x%x is not as expected 0x%x"), 
	    					dummyAperture, aperture); 

	    		result = EFail;
	    		}

	    	// Supported shutter speed. Set and Get shutter speed
	    	iInputEventUid = KUidECamEventCameraSettingShutterSpeed;
	    	RArray<TInt> suppShutterSpeed; 
	    	TValueInfo shutterSpeedValueInfo = ENotActive;
	    	settings->GetShutterSpeedsL(suppShutterSpeed, shutterSpeedValueInfo );   
			// should not have entries
			if (suppShutterSpeed.Count() != 0)
				{
				result = EFail;
				}
					    	
			TInt dummyShutterSpeed = KShutterSpeed;
			settings->SetShutterSpeed(dummyShutterSpeed);	    	 	
	        
	        CheckNotification(iInputEventUid, result);

	    	TInt shutterSpeed = settings->ShutterSpeed() ;
	    	if (dummyShutterSpeed != shutterSpeed) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Shutter Speed  0x%x is not as expected 0x%x"), 
	    					dummyShutterSpeed, shutterSpeed); 

	    		result = EFail;
	    		}	    	
	    		
   			// Supported Metering Modes
	    	iInputEventUid = KUidECamEventCameraSettingMeteringMode;
	    	TInt suppMeteringModes = settings->SupportedMeteringModes();
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Metering Modes 0x%x"), suppMeteringModes); 	    		
	    	CCamera::CCameraAdvancedSettings::TMeteringMode defaultMeteringMode = CCamera::CCameraAdvancedSettings::EMeteringModeAuto;
	    	settings->SetMeteringMode(defaultMeteringMode);    

	        CheckNotification (iInputEventUid, result);
	        
	    	CCamera::CCameraAdvancedSettings::TMeteringMode retrievedMeteringMode = settings->MeteringMode() ;    		
	    	if (defaultMeteringMode != retrievedMeteringMode) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Metering Mode  0x%x is not as expected 0x%x"), 
	    					defaultMeteringMode, retrievedMeteringMode); 
	    		result = EFail;
	    		}


   			// Supported Bracket Modes
	    	iInputEventUid = KUidECamEventCameraSettingBracketMode;
	    	TInt suppBracketModes = settings->SupportedBracketModes();
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Bracket Modes 0x%x"), suppBracketModes); 	    		
	    	CCamera::CCameraAdvancedSettings::TBracketMode defaultBracketMode = CCamera::CCameraAdvancedSettings::EBracketModeOff;
	    	settings->SetBracketMode(defaultBracketMode);    

	        CheckNotification(iInputEventUid, result);	
	        
	    	CCamera::CCameraAdvancedSettings::TBracketMode retrievedBracketMode = settings->BracketMode() ;    		
	    	if (defaultBracketMode != retrievedBracketMode) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings Drive Mode  0x%x is not as expected 0x%x"), 
	    					defaultBracketMode, retrievedBracketMode); 
	    		result = EFail;
	    		}    		


			// Supported AF assistant light. Set and Get AF assistant light.
			iInputEventUid = KUidECamEventCameraSettingAFAssistantLight;
	    	TInt suppAFAssistantLights =0;
	    	
	    	settings->GetSupportedAFAssistantLightL(suppAFAssistantLights);
	    	 
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported AF Assistant Lights 0x%x"), suppAFAssistantLights); 	    		
	    	CCamera::CCameraAdvancedSettings::TAFAssistantLight defaultAFAssistantLight = CCamera::CCameraAdvancedSettings::EAFAssistantLightOff;
	    	settings->SetAFAssistantLightL(defaultAFAssistantLight);    
			
	        CheckNotification(iInputEventUid, result);	
	        
	    	CCamera::CCameraAdvancedSettings::TAFAssistantLight retrievedAFAssistantLight;
	    	settings->GetAFAssistantLightL(retrievedAFAssistantLight); 
	    	   		
	    	if (defaultAFAssistantLight != retrievedAFAssistantLight) 
	    		{
		    	INFO_PRINTF3(_L("CCameraAdvancedSettings AF Assistant Light  0x%x is not as expected 0x%x"), 
	    					retrievedAFAssistantLight, defaultAFAssistantLight); 
	    		result = EFail;
	    		}  
	    	
	    	TInt preCaptureWarningSupported = KAllPreCaptureWarnings; // ( = 0x3FFF)	
	    	settings->GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::EModeIdle, preCaptureWarningSupported);	
	    	
	    	// subscribe for pre capture warnings. Poll for it. Unsubscribe for Pre capture warning.
	    	settings->SubscribeToPreCaptureWarningL(preCaptureWarningSupported);
	    	
	    	//Poll to know the status of every possible warning ...
	    	TInt warningStatus =0;
	    	settings->GetPreCaptureWarningL(warningStatus);
	    	
	    	settings->UnSubscribePreCaptureWarningL();
	    	
	    	//set reference screen 
	    	iInputEventUid = KUidECamEventCameraSettingReferenceScreen;
	    	
	    	CWsScreenDevice* screenDevice = NULL; 
	    	// prepare the screen targetted.
	    		    	
	    	settings->SetReferenceScreenL(*screenDevice);
	    	 
	        CheckNotification(iInputEventUid, result);	
	    	

		   INFO_PRINTF1(_L("API exercise"));
		   // API exercise
	       TBool lock = ETrue;
	       TInt value = KValue;
	       settings->SetApertureExposureLockOn(lock);
	       settings->ApertureExposureLockOn();
	       
	       	// Automatic selection
	       	settings->SetAutomaticSizeSelectionChangeOn(EFalse);
	       	settings->AutomaticSizeSelectionChangeOn();
	       	
			// stabilization mode
			// settings
			settings->SupportedStabilizationModes();
			settings->StabilizationMode();
			settings->SetStabilizationMode(CCamera::CCameraAdvancedSettings::EStabilizationModeVertical);

			settings->SupportedBracketParameters();
			settings->BracketParameter();
			settings->SetBracketParameter(CCamera::CCameraAdvancedSettings::BracketParameterAutoFocus);
			settings->SupportedBracketSteps();
			settings->BracketStep();
			settings->SetBracketStep(CCamera::CCameraAdvancedSettings::EBracketStepMedium); 
			
			TInt startIndex;
			TInt frames;
			
			settings->GetBracketMerge(startIndex, frames); 
			settings->SetBracketMerge(startIndex, frames);
			
			settings->SupportedFlashModes();
			settings->FlashMode();
			settings->SetFlashMode(CCamera::EFlashSlowRearSync);
			settings->RedEyeReduceOn();
			settings->SetRedEyeReduceOn(EFalse);
			
			RArray<TInt> steps;
			TValueInfo info;
			
			settings->GetFlashCompensationStepsL(steps, info);
			TInt flashCompensationStep1 = settings->FlashCompensationStep();
			if(flashCompensationStep1 != KFlashCompensationStep)
				{
				INFO_PRINTF2(_L("old method for FlashCompensationStep retrieving wrong value %d"),flashCompensationStep1); 	    
				result = EFail;
				}
				
			TInt flashCompensationStep2 = 0;
			TInt err = settings->GetFlashCompensationStep(flashCompensationStep2);
			if(flashCompensationStep1 != flashCompensationStep2)
				{
				INFO_PRINTF1(_L("old and new method for FlashCompensationStep retrieving wrong/dissimilar values")); 	    
				result = EFail;
				}
			if(err)
				{
				INFO_PRINTF2(_L("new method for FlashCompensationStep had error %d"),err); 	    
				result = EFail;
				}
			
			settings->SetFlashCompensationStep(value);
			TRAP(error, settings->GetCurrentFocusModeStepsL(steps, info));
			
			TInt negValue;
			TInt posValue;
			TBool ready = ETrue;
			
			settings->GetFlashCompensationRangeInSteps(negValue, posValue);
			TInt flashCompensation1 = settings->FlashCompensation();
			if(flashCompensation1 != KFlashCompensationInSteps)
				{
				INFO_PRINTF2(_L("old method for FlashCompensation retrieving wrong value %d"),flashCompensation1); 	    
				result = EFail;
				}
			TInt flashCompensation2 = 0;
			err = settings->GetFlashCompensation(flashCompensation2);
			
			if(flashCompensation1 != flashCompensation2)
				{
				INFO_PRINTF1(_L("old and new method for FlashCompensation retrieving wrong/dissimilar values")); 	    
				result = EFail;
				}
			if(err)
				{
				INFO_PRINTF2(_L("new method for FlashCompensation had error %d"),err); 	    
				result = EFail;
				}
			
			settings->SetFlashCompensation(value);
			
			settings->IsFlashReady(ready);
			if(ready != EFalse) // The value the pugin has been set to return
				{
				result = EFail;	
				}
			settings->IsExternalFlashPresent();
			settings->GetManualFlashPowerLevelsL(steps, info);
			settings->SetManualFlashPowerLevel(value);
			settings->ManualFlashPowerLevel();
			
			settings->SupportedExposureModes();
			settings->ExposureMode();
			settings->SetExposureMode(CCamera::EExposureAuto);
			
 			settings->GetExposureCompensationStepsL(steps, info);
 			TInt expCompensationStep1 = settings->ExposureCompensationStep();
 			if(expCompensationStep1 != KExposureCompensationStep)
				{
				INFO_PRINTF2(_L("old method for ExposureCompensationStep retrieving wrong value %d"),expCompensationStep1); 	    
				result = EFail;
				}
				
			TInt expCompensationStep2 = 0;
			err = settings->GetExposureCompensationStep(expCompensationStep2);
			if(expCompensationStep1 != expCompensationStep2)
				{
				INFO_PRINTF1(_L("old and new method for ExposureCompensationStep retrieving wrong/dissimilar values")); 	    
				result = EFail;
				}
			if(err)
				{
				INFO_PRINTF2(_L("new method for ExposureCompensationStep had error %d"),err); 	    
				result = EFail;
				}
			settings->SetExposureCompensationStep(value);

			settings->GetExposureCompensationRangeInSteps(negValue, posValue);
			TInt expCompensation1 = settings->ExposureCompensation();
			if(expCompensation1 != KExposureCompensationInSteps)
				{
				INFO_PRINTF2(_L("old method for ExposureCompensation retrieving wrong value %d"),expCompensation1); 	    
				result = EFail;
				}
				
			TInt expCompensation2 = 0;
			err = settings->GetExposureCompensation(expCompensation2);
			
			if(expCompensation1 != expCompensation2)
				{
				INFO_PRINTF1(_L("old and new method for ExposureCompensation retrieving wrong/dissimilar values")); 	    
				result = EFail;
				}
			if(err)
				{
				INFO_PRINTF2(_L("new method for ExposureCompensation had error %d"),err); 	    
				result = EFail;
				}
			settings->SetExposureCompensation(value);
		
			settings->SupportedWhiteBalanceModes();
			settings->WhiteBalanceMode();
			settings->SetWhiteBalanceMode(CCamera::EWBAuto);
			
			settings->ShootClickOn();
			settings->SetShootClickOn(EFalse);
			settings->GetTimerIntervalsL(steps, info);
			settings->TimerInterval();
			settings->SetTimerInterval(value);

			TTime start;
			TTime end;
			TTime interval;

			settings->GetTimeLapsePeriodRange(start, end);
			settings->GetTimeLapse(start, end, interval);
			settings->SetTimeLapse(start, end, interval);
			
			settings->PictureOrientation();
			settings->SetPictureOrientation(CCamera::CCameraAdvancedSettings::EPictureOrientationPortrait);
		
			settings->SupportedPixelAspectRatios();
			settings->PixelAspectRatio();
			settings->SetPixelAspectRatio(CCamera::CCameraAdvancedSettings::EPixelAspect1To1);
			settings->SupportedYuvRanges();
			settings->YuvRange();
			settings->SetYuvRange(CCamera::CCameraAdvancedSettings::EYuvRangeVideoCropped);
			settings->BurstImages();
			settings->SetBurstImages(value); 
			settings->GetOpticalZoomStepsL(steps, info);
			settings->OpticalZoom();
			settings->SetOpticalZoom(value);
			settings->GetDigitalZoomStepsL(steps, info);
			settings->DigitalZoom();
			settings->SetDigitalZoom(value);
			
			TInt sizeIndex =0;
			TBool isInfluencePossible;
			CCamera::TFormat format = CCamera::EFormatYUV420Planar;
			settings->GetDigitalZoomStepsForStillL(steps, info, sizeIndex, format, isInfluencePossible);
			settings->DigitalZoom();
			settings->SetDigitalZoom(value);
			
			TInt frameRateIndex =0;
			CCamera::TExposure exposure = CCamera::EExposureAuto;
			settings->GetDigitalZoomStepsForVideoL(steps, info, frameRateIndex, sizeIndex, format, isInfluencePossible, exposure);
			settings->DigitalZoom();
			settings->SetDigitalZoom(value);

			settings->ExposureLockOn();
			settings->SetExposureLockOn(EFalse);
			settings->AutoFocusLockOn();
			settings->SetAutoFocusLockOn(EFalse);
			
			// timeouts
			RArray<TInt> timeouts;
    		settings->GetSupportedContinuousAutoFocusTimeoutsL(timeouts, info);
    		timeouts.Close();
    		settings->ContinuousAutoFocusTimeout();
    		settings->SetContinuousAutoFocusTimeout(KContinuousAutoFocusTimeoutValue1);
    		
    		// stabilization effects
	        settings->SupportedStabilizationEffects();
    		settings->SetStabilizationEffect(CCamera::CCameraAdvancedSettings::EStabilizationAuto);
			CCamera::CCameraAdvancedSettings::TStabilizationEffect effect = settings->StabilizationEffect();
	        
	        // stabilization complexity
			settings->SupportedStabilizationComplexityValues();
			CCamera::CCameraAdvancedSettings::TStabilizationAlgorithmComplexity complexity = settings->StabilizationComplexity();
			settings->SetStabilizationComplexity(CCamera::CCameraAdvancedSettings::EStabilizationComplexityAuto);
            
            // ISO rate type
	    	iInputEventUid = KUidECamEventCameraSettingIsoRateType;
			
			supportedISORateTypes = 0;
			settings->GetSupportedISORateTypeL(supportedISORateTypes);  
			
			INFO_PRINTF2(_L("CCameraAdvancedSettings supported Iso Rate Types 0x%x"), supportedISORateTypes); 	    
			
			//List of supported ISO rates can be found using OLD API.
			RArray<TInt> supportedIsoRates;
			settings->GetSupportedIsoRatesL(supportedIsoRates);  
			
			//set to manual ISO rate
			isoRateType = CCamera::CCameraAdvancedSettings::EISOManual;
			if(isoRateType & supportedISORateTypes)
				{
				INFO_PRINTF1(_L("Testing new ISO API for CCamera::CCameraAdvancedSettings::EISOManual")); 
				TestISOTypesL(settings, isoRateType, supportedIsoRates, result);
				}
			
			//switch to unprioritised type of Auto ISO...
			isoRateType = CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised;
			if(isoRateType & supportedISORateTypes)
				{
				INFO_PRINTF1(_L("Testing new ISO API for CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised")); 
				TestISOTypesL(settings, isoRateType, supportedIsoRates, result);
				}
			
			//switch to ISO prioritised AutoISO ...	
			isoRateType = CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised;
			if(isoRateType & supportedISORateTypes)
				{
				INFO_PRINTF1(_L("Testing new ISO API for CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised")); 
				TestISOTypesL(settings, isoRateType, supportedIsoRates, result);
				}
			
			//switch to shutter speed prioritised AutoISO...	
			isoRateType = CCamera::CCameraAdvancedSettings::EISOAutoShutterSpeedPrioritised;	
			if(isoRateType & supportedISORateTypes)
				{
				INFO_PRINTF1(_L("Testing new ISO API for CCamera::CCameraAdvancedSettings::EISOAutoShutterSpeedPrioritised")); 
				TestISOTypesL(settings, isoRateType, supportedIsoRates, result);
				}
			
			//switch to aperture prioritised AutoISO...	
			isoRateType = CCamera::CCameraAdvancedSettings::EISOAutoAperturePrioritised;	
			if(isoRateType & supportedISORateTypes)
				{
				INFO_PRINTF1(_L("Testing new ISO API for CCamera::CCameraAdvancedSettings::EISOAutoAperturePrioritised")); 
				TestISOTypesL(settings, isoRateType, supportedIsoRates, result);
				}
				
            supportedIsoRates.Close();
            
            // all functions are called, irrespective of the units; in real case will be one set or the other 
            TRgb rgb(100,124,130);
		    CCamera::CCameraAdvancedSettings::TWBUnits units = settings->SupportedWBUnits();
		    settings->SetWBRgbValue(rgb);
			settings->GetWBRgbValue(rgb);
			RArray<TInt> temperatures;
			settings->GetWBSupportedColorTemperaturesL(temperatures, info);
			temperatures.Close();
			settings->SetWBColorTemperature(KWBColorTemperature1);	
			TInt temp = settings->WBColorTemperature();
			
			INFO_PRINTF1(_L("GetFocalLengthInfoL method invocation"));
			TInt currentFocalLength=0; 
			TInt maxFocalLength=0;
			TRAP(err, settings->GetFocalLengthInfoL(minFocalLength, currentFocalLength, maxFocalLength));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetFocalLengthInfoL supported!")); 
				result = EFail;
				}
			
			INFO_PRINTF1(_L("GetNumOperationPreferenceL method invocation"));	
			TUint numOperationPreferenceSupported=0;
	        TRAP(err, settings->GetNumOperationPreferenceL(numOperationPreferenceSupported));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetNumOperationPreferenceL supported!")); 
				result = EFail;
				}
			
			INFO_PRINTF1(_L("EnumerateOperationPreferenceL method invocation"));	
			
			TUint operationPreferenceIndex=0;
			CCamera::CCameraAdvancedSettings::TPerformanceLevel speedLevel = 
							CCamera::CCameraAdvancedSettings::ELevelDontCare;
			CCamera::CCameraAdvancedSettings::TPerformanceLevel qualityLevel = 
							CCamera::CCameraAdvancedSettings::ELevelDontCare;
			CCamera::CCameraAdvancedSettings::TPerformanceLevel lowMemoryConsumptionLevel = 
							CCamera::CCameraAdvancedSettings::ELevelDontCare;
			CCamera::CCameraAdvancedSettings::TPerformanceLevel lowPowerConsumptionLevel = 
							CCamera::CCameraAdvancedSettings::ELevelDontCare;
			
	        TRAP(err, settings->EnumerateOperationPreferenceL(operationPreferenceIndex, speedLevel,
	        	qualityLevel, lowMemoryConsumptionLevel, lowPowerConsumptionLevel));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("EnumerateOperationPreferenceL supported!")); 
				result = EFail;
				}
						
			iInputEventUid = KUidECamEventCameraSettingOperationPreference;
			settings->SetOperationPreferenceL(operationPreferenceIndex);
			CheckNotificationNeg(iInputEventUid, result);
	
			INFO_PRINTF1(_L("GetOperationPreferenceL method invocation"));
			TInt opPreferenceIndex;
			TRAP(err, settings->GetOperationPreferenceL(opPreferenceIndex));
			if(err != KErrNotSupported)
				{
				INFO_PRINTF1(_L("GetOperationPreferenceL supported!")); 
				result = EFail;
				}
			
			INFO_PRINTF1(_L("GetSupportedEventsL method invocation"));	
			RArray<TUid> supportedEvents;
			settings->GetSupportedEventsL(supportedEvents);
			
			if(supportedEvents.Count() != 0)
				{
				INFO_PRINTF1(_L("GetSupportedEventsL entry exists!")); 
				result = EFail;
				}
			supportedEvents.Close();
			
			INFO_PRINTF1(_L("GetIndirectFeatureChangesL method invocation"));	
			RArray<TUid> featureChangesIndirect;
			TRAP(err, settings->GetIndirectFeatureChangesL(KUidECamEventCameraSettingOperationPreference, featureChangesIndirect));
			if(err != KErrNone)
				{
				INFO_PRINTF1(_L("GetIndirectFeatureChangesL returned error!")); 
				result = EFail;
				}
				
			if(featureChangesIndirect.Count() != 0)
				{
				INFO_PRINTF1(_L("GetIndirectFeatureChangesL entry exists!")); 
				result = EFail;
				}
			featureChangesIndirect.Close();
				
	    	CleanupStack::PopAndDestroy(settings);
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


void RECamAdvSetDefaultsTest::TestISOTypesL(CCamera::CCameraAdvancedSettings* aSettings, CCamera::CCameraAdvancedSettings::TISORateType aIsoRateType, RArray<TInt> aSupportedIsoRates, TVerdict& aResult)
	{
	TInt param =0;
	
	switch(aIsoRateType)
		{
		case CCamera::CCameraAdvancedSettings::EISOManual:
			{
			param = KDefaultIsoRating;
			break;	
			}
		
		case CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised:
			{
			param = KIsoRate01;// ISO rate equal to or closest to (and less than) it should be selected by the 
							  // camera in order to get optimum exposure.
			break;	
			}
		
		case CCamera::CCameraAdvancedSettings::EISOAutoShutterSpeedPrioritised:
			{
			param = KShutterSpeed;// Shutter speed equal to or closest to (and faster than) it should be selected by the 
								  // camera in order to get optimum exposure.
			break;	
			}
		
		case CCamera::CCameraAdvancedSettings::EISOAutoAperturePrioritised:
			{
			param = KDefaultAperture;// Aperture opening equal to or closest to (and wider than) it should be selected by the 
								   	 // camera in order to get optimum exposure.
			break;	
			}
		
		default:
			{
			param =0;
			}
		}
	
	aSettings->SetISORateL(aIsoRateType, param); 
	
	CheckNotification(iInputEventUid, aResult);
	
	CCamera::CCameraAdvancedSettings::TISORateType retrievedIsoRateType;
	TInt retrievedParam =0;
	TInt retrievedIsoRate =0;	
				
	aSettings->GetISORateL(retrievedIsoRateType, retrievedParam, retrievedIsoRate);
	
	if(retrievedIsoRateType != aIsoRateType)
		{
		INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate Type 0x%x is not as expected 0x%x"), 
				retrievedIsoRateType, aIsoRateType); 
		aResult = EFail;
		}
	
	if(aIsoRateType & KAutoISOTypes)
		{
		if(retrievedParam != param)
			{
			INFO_PRINTF3(_L("CCameraAdvancedSettings Auto ISO parameter %d is not as expected %d"), 
					retrievedParam, param); 
			aResult = EFail;
			}
		
		if(retrievedIsoRate == KErrNotFound)
			{
			INFO_PRINTF2(_L("CCameraAdvancedSettings retrieved Iso Rate: %d :- Camera incapable of getting ISO under Auto ISO"), 
					retrievedIsoRate); 
			}
		else
			{//ISO set under Auto ISO has to be one of supported ISO rates .
			if (aSupportedIsoRates.Find(retrievedIsoRate) == KErrNotFound)
				{
				INFO_PRINTF2(_L("CCameraAdvancedSettings Iso Rate %d is not among the supported ISO rates"), 
						retrievedIsoRate); 
				aResult = EFail;
				}
			
			
			if(aIsoRateType == CCamera::CCameraAdvancedSettings::EISOAutoISOPrioritised)
				{
				//ISO rate set should be less than or equal to param
				if (retrievedIsoRate > param)
					{
					INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate %d is greater than %d"), 
	    					retrievedIsoRate, param); 
	    			aResult = EFail;
					}
				}
			
			//use of old API
			TInt retrievedIsoRateOld = aSettings->IsoRate();
			if (retrievedIsoRateOld != retrievedIsoRate)
				{
				INFO_PRINTF3(_L("CCameraAdvancedSettings OLD and NEW API inconsistent: Iso Rate old %d ; ISO Rate new: %d"), 
						retrievedIsoRateOld, retrievedIsoRate); 
				aResult = EFail;
				}	
			}
		
		//switch to manual ISO using OLD API
		aSettings->SetIsoRate(KDefaultIsoRating);
		
		aSettings->GetISORateL(retrievedIsoRateType, param, retrievedIsoRate);
		
		if(retrievedIsoRateType != CCamera::CCameraAdvancedSettings::EISOManual)
			{
			INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate Type 0x%x is not as expected 0x%x : OLD and NEW API inconsistent"), 
					retrievedIsoRateType, CCamera::CCameraAdvancedSettings::EISOManual); 
			aResult = EFail;
			}
	
		if (retrievedIsoRate != KDefaultIsoRating)
			{
			INFO_PRINTF3(_L("CCameraAdvancedSettings Iso Rate %d is not as expected %d"), 
					retrievedIsoRate, KDefaultIsoRating); 
			aResult = EFail;
			}
		}
	
	//use of old API
	TInt retrievedIsoRateOld = aSettings->IsoRate();
	if (retrievedIsoRateOld != retrievedIsoRate)
		{
		INFO_PRINTF3(_L("CCameraAdvancedSettings OLD and NEW API inconsistent: Iso Rate old %d ; ISO Rate new: %d"), 
				retrievedIsoRateOld, retrievedIsoRate); 
		aResult = EFail;
		}
	}

//
// RECamAdvSetGettersTest //
//	
RECamAdvSetGettersTest* RECamAdvSetGettersTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetGettersTest* self = new (ELeave) RECamAdvSetGettersTest(aAllocTest);
	return self;	
	}
	
RECamAdvSetGettersTest::RECamAdvSetGettersTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-011-HP");
	}
	
TVerdict RECamAdvSetGettersTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
		
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*this, 0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	   
     	    iInputEventUid = KUidECamEventCameraSettingIsoRate;
	    	TInt isoRateSet = KIsoRate01;
	    	settings->SetIsoRate(isoRateSet);    
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings Iso Rate set %d"), isoRateSet); 	    		

	        CheckNotification(iInputEventUid, result);
	        	                
			// Get ISO rate. It should have changed to set value.
	    	TInt isoRate = settings->IsoRate();
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings get Iso Rate %d"), isoRate); 
	    		    		
	    	if (isoRateSet!=isoRate) 
	    		{
	    		INFO_PRINTF1(_L("Got a different iso rate than it was set"));
	    		result = EFail;
	    		}
 
	    	CleanupStack::PopAndDestroy(settings);
	    	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}


//
// RECamAdvSetTwoCamTest //
//
RECamAdvSetTwoCamTest* RECamAdvSetTwoCamTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetTwoCamTest* self = new (ELeave) RECamAdvSetTwoCamTest(aAllocTest);
	return self;	
	}
	
RECamAdvSetTwoCamTest::RECamAdvSetTwoCamTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-014-HP");
	}
	
TVerdict RECamAdvSetTwoCamTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera1 = NULL;
	CCamera* camera2 = NULL;
	
	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings1 = NULL;
	CCamera::CCameraAdvancedSettings* settings2 = NULL;		
	
	// using observer 2 if exist 
	MCameraObserver2* observer2 = NULL; 
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera1 = CCamera::New2L(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera1);
		TRAP(error, camera2 = CCamera::New2L(*observer2, 1,0));

		if (error==KErrNone)
			{
			CleanupStack::PushL(camera2);
		    //Second camera
			settings1 = static_cast<CCamera::CCameraAdvancedSettings*> (camera1->CustomInterface(KECamAdvancedSettingUid));
	    	RArray<TUid> aSuppSettings1;
	    	RArray<TUid> aActSettings1;
	    	if (settings1!= NULL)
		    	{
		    	CleanupStack::PushL(settings1);
		    	
				CleanupClosePushL(aSuppSettings1);	 
				aSuppSettings1.Reset();   
			    settings1->GetSupportedSettingsL(aSuppSettings1);
				CleanupClosePushL(aActSettings1);	 
				aActSettings1.Reset();   
			    settings1->GetActiveSettingsL(aActSettings1);
				if ((aActSettings1.Count())==0)
					{
	   				INFO_PRINTF1(_L("Active camera. Non empty array expected")); 
	        		result = EFail;						
					}
					
				settings1->GetDisabledSettingsL(aActSettings1);
				}
		    	
		    //Second camera
			settings2 = static_cast<CCamera::CCameraAdvancedSettings*> (camera2->CustomInterface(KECamAdvancedSettingUid));
	    	RArray<TUid> aSuppSettings2;
	    	RArray<TUid> aActSettings2;
	    	if (settings2!= NULL)
		    	{
		    	CleanupStack::PushL(settings2);

				CleanupClosePushL(aSuppSettings2);	 
				aSuppSettings2.Reset();   
	    		settings2->GetSupportedSettingsL(aSuppSettings2);
				CleanupClosePushL(aActSettings2);	 
				aActSettings2.Reset();   
		    	settings2->GetActiveSettingsL(aActSettings2);  	
				if ((aActSettings2.Count())!=0)
					{
 					INFO_PRINTF1(_L("Empty array expected (non present camera)")); 
	       			result = EFail;						
					}
				}		
	    	
	    	CleanupStack::PopAndDestroy(2, &aSuppSettings2); //aActSettings2
			CleanupStack::PopAndDestroy(settings2);	
			
			CleanupStack::PopAndDestroy(2, &aSuppSettings1); //aActSettings1
			CleanupStack::PopAndDestroy(settings1);			

			CleanupStack::PopAndDestroy(camera2);
			
			}
		else 
			{
			INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
			result = EFail;		
			}

		CleanupStack::PopAndDestroy(camera1);

		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
	
//
// RECamAdvSetNegTest //
//

RECamAdvSetNegTest* RECamAdvSetNegTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetNegTest* self = new (ELeave) RECamAdvSetNegTest(aAllocTest);
	return self;	
	}
	
RECamAdvSetNegTest::RECamAdvSetNegTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-0102-HP");
	}
	
TVerdict RECamAdvSetNegTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*this,0,0));
	
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	    	// trying to set a value which is not supported
	    	// the action would fail to set a new value and the notification will fail
	    	// thus the current value will be different from the requested value. - double verification
     	    iInputEventUid = KUidECamEventCameraSettingIsoRate;
	    	TInt isoRateSet = KIsoRate0; // not supported rate
	    	settings->SetIsoRate(isoRateSet);    
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings Iso Rate set 0x%x"), isoRateSet); 	    		

	        CheckNotificationNeg(iInputEventUid, result);
	        	                
			// Get ISO rate. It should have changed to set value.
	    	TInt isoRate = settings->IsoRate();
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings get Iso Rate 0x%x"), isoRate); 	    		
	    	if (isoRateSet == isoRate) 
	    		{
	    		INFO_PRINTF1(_L("Got an unsupported iso rate set"));
	    		result = EFail;
	    		}
 			
 			// Check the new API for ISO setting as well.
 			iInputEventUid = KUidECamEventCameraSettingIsoRateType;
	    	
	    	settings->SetISORateL(CCamera::CCameraAdvancedSettings::EISOManual, isoRateSet);    
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings Iso Rate set 0x%x"), isoRateSet); 	    		

	        CheckNotificationNeg(iInputEventUid, result);
	        	                
			// Get ISO rate. It should have changed to set value.
	    	CCamera::CCameraAdvancedSettings::TISORateType isoRateType = CCamera::CCameraAdvancedSettings::EISOManual;
	    	TInt index =0;
	    	settings->GetISORateL(isoRateType, index, isoRate);
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings get Iso Rate 0x%x"), isoRate); 	    		
	    	if (isoRateSet == isoRate) 
	    		{
	    		INFO_PRINTF1(_L("Got an unsupported iso rate set"));
	    		result = EFail;
	    		}
 			
	    	CleanupStack::PopAndDestroy(settings);
	    	}
	    else
    		{
    		result = EFail;
    		}	
		CleanupStack::PopAndDestroy(camera);
		}
	else 
		{
		INFO_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;		
		}
	__MM_HEAP_MARKEND;

	return result;
	}
		
//
// RECamAdvSetBaselineTest //
//
RECamAdvSetBaselineTest* RECamAdvSetBaselineTest::NewL(TBool aAllocTest)
	{
	RECamAdvSetBaselineTest* self = new (ELeave) RECamAdvSetBaselineTest(aAllocTest);
	return self;	
	}

RECamAdvSetBaselineTest::RECamAdvSetBaselineTest(TBool)
	{
	iTestStepName = _L("MM-ECM-ADV-U-048-HP");
	}
	
TVerdict RECamAdvSetBaselineTest::DoTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::NewL() and MCameraObserver2"));
	TRAP(error, camera = CCamera::NewL(*this, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	    	
	    	// Supported Focus Range. Set unfamilar Focus Range. Get a known Focus Range.
			iInputEventUid = KUidECamEventCameraSettingFocusRange;	    	
	    	TInt suppFocusRanges = settings->SupportedFocusRanges();    	
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported Focus Ranges 0x%x"), suppFocusRanges); 
	    	if(suppFocusRanges > KBaselinedFocusRanges)
	    		{
	    		INFO_PRINTF1(_L("Unfamiliar focus range received")); 
	    		result = EFail;
	    		}
	    		
	    	CCamera::CCameraAdvancedSettings::TFocusRange unfamilarFocusRange = CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal;
			settings->SetFocusRange(unfamilarFocusRange);
			
			// Check for KUidECamEventCameraSettingFocusRange.
	        CheckNotification(iInputEventUid, result);

	    	CCamera::CCameraAdvancedSettings::TFocusRange retrievedFocusRange = settings->FocusRange();
	    	if (CCamera::CCameraAdvancedSettings::EFocusRangeAuto != retrievedFocusRange) 
	    		{
	   		 	INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar/wrong Focus Range")); 
	    		result = EFail;
	    		}
	    		
	        // Supported White Balance Modes. Set unfamilar White Balance. Get a known White Balance.
			iInputEventUid = KUidECamEventCameraSettingsWBValue;	    	
	    	TInt suppWhiteBalance = settings->SupportedWhiteBalanceModes();    	
	    	INFO_PRINTF2(_L("CCameraAdvancedSettings supported White Balance 0x%x"), suppWhiteBalance); 
	    	if(suppWhiteBalance > KBaselinedWhiteBalanceModes)
	    		{
	    		INFO_PRINTF1(_L("Unfamiliar White Balance received")); 
	    		result = EFail;
	    		}
	    		
	    	CCamera::TWhiteBalance unfamilarWhiteBalance = CCamera::EWBDaylightUnderWater;
			settings->SetWhiteBalanceMode(unfamilarWhiteBalance);
			
			// Check for KUidECamEventCameraSettingsWBValue.
	        CheckNotification(iInputEventUid, result);

	    	CCamera::TWhiteBalance retrievedWhiteBalance = settings->WhiteBalanceMode();
	    	if (CCamera::EWBAuto != retrievedWhiteBalance) 
	    		{
	   		 	INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar/wrong WhiteBalance")); 
	    		result = EFail;
	    		}
			
			RArray<TUid> settingsList; 
	    	CleanupClosePushL(settingsList);
	    	// check for GetSupportedSettings
	    	settings->GetSupportedSettingsL(settingsList);
	    	
	    	for(TInt index=0; index<settingsList.Count(); index++)
	    		{
	    		if(settingsList[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
	    			{
	    			INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar settings")); 
	    			result = EFail;
	    			}
	    		}
	    	settingsList.Reset();
	    	
	    	// check for GetActiveSettings
	    	settings->GetActiveSettingsL(settingsList);
	    	
	    	for(TInt index=0; index<settingsList.Count(); index++)
	    		{
	    		if(settingsList[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
	    			{
	    			INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar settings")); 
	    			result = EFail;
	    			}
	    		}
	    	settingsList.Reset();
	    	
	    	// check for GetDisabledSettings
	    	settings->GetDisabledSettingsL(settingsList);
	    	
	    	for(TInt index=0; index<settingsList.Count(); index++)
	    		{
	    		if(settingsList[index].iUid > KUidECamEventCameraSettingAutoFocusType2UidValue)
	    			{
	    			INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar settings")); 
	    			result = EFail;
	    			}
	    		}
	    	settingsList.Reset();
	    	
	    	// check for PRECaptureWarning
	    	TInt preCaptureWarningSupported = KAllPreCaptureWarnings; // ( = 0x3FFF)	
	    	settings->GetPreCaptureWarningSupportedL(CCamera::CCameraAdvancedSettings::EModeIdle, preCaptureWarningSupported);	
	    	if(preCaptureWarningSupported != ((CCamera::CCameraAdvancedSettings::EPCWGeneralWarning << 1)-1))
	    		{
	    		INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar preCaptureWarningSupported")); 
	    		result = EFail;
	    		}
	    	
	    	// subscribe for pre capture warnings. Poll for it. Unsubscribe for Pre capture warning.
	    	settings->SubscribeToPreCaptureWarningL(preCaptureWarningSupported);
	    	
	    	//Poll to know the status of every possible warning ...
	    	TInt warningStatus = preCaptureWarningSupported;
	    	settings->GetPreCaptureWarningL(warningStatus);
	    	if(warningStatus != ((CCamera::CCameraAdvancedSettings::EPCWGeneralWarning << 1)-1))
	    		{
	    		INFO_PRINTF1(_L("CCameraAdvancedSettings retrieved unfamilar PreCaptureWarning")); 
	    		result = EFail;
	    		}
	    		
	    	settings->UnSubscribePreCaptureWarningL();
	    	
			CleanupStack::PopAndDestroy(&settingsList);
	    	CleanupStack::PopAndDestroy(settings);
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

class TDummyObserver: public MCameraObserver2
    {
public:
    explicit TDummyObserver(TInt& aErrorPointer);

    virtual void HandleEvent(const TECAMEvent& aEvent);
    virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
    virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
    virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
    
private:
    TInt& iErrorPointer;
    };
    
TDummyObserver::TDummyObserver(TInt& aErrorPointer):iErrorPointer(aErrorPointer)
    {
    }

void TDummyObserver::HandleEvent(const TECAMEvent& /*aEvent*/)
    {
    return;
    }
   
void TDummyObserver::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/, TInt aError)
    {
    iErrorPointer = aError;
    }
       
void TDummyObserver::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/, TInt aError)
    {
    iErrorPointer = aError;
    }
       
void TDummyObserver::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt aError)
    {
    iErrorPointer = aError;
    }

RECamAdvSetTest2* RECamAdvSetTest2::NewL(TBool aAllocTest)
	{
	RECamAdvSetTest2* self = new (ELeave) RECamAdvSetTest2(aAllocTest);
	return self;	
	}
	
RECamAdvSetTest2::RECamAdvSetTest2(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-049-HP");
	}
	
void RECamAdvSetTest2::RatioSubStepL(CCamera::CCameraAdvancedSettings* settings, TInt aSuppRange, TInt aTestValue, TInt aExpValue)
    {
// Pixel ratio's //    
    TInt suppRatios = settings->SupportedPixelAspectRatios();
    if (suppRatios != aSuppRange)
        {
    	INFO_PRINTF2(_L("--Error: Supported pixel ratios are wrong %x "), suppRatios);
    	User::Leave(KErrGeneral);        
        }
    settings->SetPixelAspectRatio( CCamera::CCameraAdvancedSettings::TPixelAspectRatio(aTestValue) );
    
    TInt ratio = settings->PixelAspectRatio();
    
    if (ratio != aExpValue)
        {
    	INFO_PRINTF2(_L("--Error: Returned ration is wrong %x "), ratio);
    	User::Leave(KErrGeneral);                
        }    
    }
    
void RECamAdvSetTest2::FlashSubStepL(CCamera::CCameraAdvancedSettings* settings, TInt aSuppRange, TInt aTestValue, TInt aExpValue)
    {
// Flash modes //
    TInt suppFlashModes = settings->SupportedFlashModes();
        
    if (suppFlashModes != aSuppRange)
        {
    	INFO_PRINTF2(_L("--Error: Supported flash modes are wrong %x "), suppFlashModes);
    	User::Leave(KErrGeneral);        
        }
    settings->SetFlashMode( CCamera::TFlash(aTestValue) );
    
    TInt flashMode = settings->FlashMode();
    
    if (flashMode != aExpValue)
        {
    	INFO_PRINTF2(_L("--Error: Returned flash mode is wrong %x "), flashMode);
    	User::Leave(KErrGeneral);                
        }    
    }
    
void RECamAdvSetTest2::OvrSubStepL(CCamera& aCamera, TInt aSuppRange, TInt aTestValue, TInt aExpValue)
    {
    // Overlay modes //
    CCamera::CCameraOverlay* overlay = NULL;
    TInt error = KErrNone;
    TRAP(error, overlay = CCamera::CCameraOverlay::NewL(aCamera));

    if (overlay == NULL)
        {
    	INFO_PRINTF1(_L("--Error: Failed to get the CCameraOverlay"));
    	User::Leave(KErrGeneral);        
        }
    CleanupStack::PushL(overlay);
    CCamera::CCameraOverlay::TOverlaySupportInfo overlaySupInfo;
    overlay->GetOverlaySupport( overlaySupInfo );
    
    if (overlaySupInfo.iSupportedModes != aSuppRange)
        {
    	INFO_PRINTF2(_L("--Error: Supported overlay modes are wrong %x "), overlaySupInfo.iSupportedModes);
    	User::Leave(KErrGeneral);        
        }

    CCamera::CCameraOverlay::TOverlayParameters ovrParams;
    ovrParams.iCurrentModes = aTestValue;
    ovrParams.iCurrentTypes = CCamera::CCameraOverlay::EPerPixel;
    
    CFbsBitmap* bmp = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);
    TUint handle = 0;
    TRAP(error, handle = overlay->CreateOverlayL(ovrParams, bmp) );
    if (error != KErrNone)
        {
    	INFO_PRINTF2(_L("--Error: failed to create overlay %d "), error);
    	User::Leave(KErrGeneral);                
        }
        
    CCamera::CCameraOverlay::TOverlayParameters retrievedParams;
    overlay->GetOverlayParametersL(handle, retrievedParams);
    overlay->ReleaseOverlay(handle);
    
    if (retrievedParams.iCurrentModes != aExpValue)
        {
    	INFO_PRINTF2(_L("--Error: wrong overlay params retrieved %x "), retrievedParams.iCurrentModes);
    	User::Leave(KErrGeneral);        
        }
    CleanupStack::PopAndDestroy(2, overlay); 
    }

TVerdict RECamAdvSetTest2::DoTestStepL()
	{	
	const TInt KNew2PixRatio    = CCamera::CCameraAdvancedSettings::EEPixelAspect40To33;
	const TInt KNew2SuppRatios  = ( CCamera::CCameraAdvancedSettings::EEPixelAspect40To33 << 1) - 1;
	
	const TInt KOldPixRatio     = CCamera::CCameraAdvancedSettings::EEPixelAspect59To54;
	const TInt KOldSuppRatios   = ( CCamera::CCameraAdvancedSettings::EEPixelAspect59To54 << 1) - 1;	
	
	const TInt KNew2FlashModes  = (CCamera::EFlashVideoLight << 1) - 1;
	const TInt KNew2FlashMode   = CCamera::EFlashVideoLight;
	
	const TInt KOldFlashModes   = (CCamera::EFlashManual << 1) - 1;
	const TInt KOldFlashMode    = CCamera::EFlashManual;
	
	const TInt KNew2OvrSuppt    = (CCamera::CCameraOverlay::EModeStillImageBurst << 1) - 1;
	const TInt KOldOvrSuppt     = (CCamera::CCameraOverlay::EModeVideo << 1) - 1;
		
	CCamera* camera = NULL;
	TInt callbackError = KErrNone;
	TDummyObserver observer2(callbackError);

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
		
	TRAP(error, camera = CCamera::New2L(observer2, 0, 100));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("--Error creating CCamera object %d"),error);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PushL(camera);
		
    settings = static_cast<CCamera::CCameraAdvancedSettings*> 
								(camera->CustomInterface(KECamAdvancedSettingUid));
    if (settings == NULL)
    	{
    	INFO_PRINTF1(_L("--Error: Failed to get the CCameraAdvancedSettings"));
    	User::Leave(KErrGeneral);
    	}
    CleanupStack::PushL(settings);
    
    RatioSubStepL(settings, KNew2SuppRatios, KNew2PixRatio, KNew2PixRatio);
    FlashSubStepL(settings, KNew2FlashModes, KNew2FlashMode, KNew2FlashMode);
    OvrSubStepL(*camera, KNew2OvrSuppt, KNew2OvrSuppt, KNew2OvrSuppt);
        
    CleanupStack::PopAndDestroy(2, camera);
    
// now try to use it as an "old" client //		
	TRAP(error, camera = CCamera::NewL(observer2, 0, 100));
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("--Error creating CCamera object %d"),error);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PushL(camera);

    settings = static_cast<CCamera::CCameraAdvancedSettings*> 
								(camera->CustomInterface(KECamAdvancedSettingUid));
    if (settings == NULL)
    	{
    	INFO_PRINTF1(_L("--Error: Failed to get the CCameraAdvancedSettings"));
    	User::Leave(KErrGeneral);
    	}
    CleanupStack::PushL(settings);

    RatioSubStepL(settings, KOldSuppRatios, KOldPixRatio, KOldPixRatio);
    RatioSubStepL(settings, KOldSuppRatios, KNew2PixRatio, CCamera::CCameraAdvancedSettings::EPixelAspectUnknown);
    
    FlashSubStepL(settings, KOldFlashModes, KOldFlashMode, KOldFlashMode);
    FlashSubStepL(settings, KOldFlashModes, KNew2FlashMode, CCamera::EFlashAuto);
    
    OvrSubStepL(*camera, KOldOvrSuppt, KOldOvrSuppt, KOldOvrSuppt);
    OvrSubStepL(*camera, KOldOvrSuppt, CCamera::CCameraOverlay::EModeClientViewfinder, 
                        CCamera::CCameraOverlay::EModeViewfinder);
                        
    OvrSubStepL(*camera, KOldOvrSuppt, CCamera::CCameraOverlay::EModeStillImageBurst, 
                        CCamera::CCameraOverlay::EModeStillImage);
    
    CleanupStack::PopAndDestroy(2, camera);
	return EPass;
	}


RECamContinuousZoomAllocTest* RECamContinuousZoomAllocTest::NewL(TBool aAllocTest)
	{
	RECamContinuousZoomAllocTest* self = new (ELeave) RECamContinuousZoomAllocTest(aAllocTest);
	return self;	
	}
	
RECamContinuousZoomAllocTest::RECamContinuousZoomAllocTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-058-HP");
	}
	
TVerdict RECamContinuousZoomAllocTest::DoTestStepL()	
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

		TRAP(err, verdict = DoAllocTestStepL());

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
	
TVerdict RECamContinuousZoomAllocTest::DoAllocTestStepL()
	{	
	TVerdict result = EPass;
	CCamera* camera = NULL;

	TInt error = KErrNone;
	CCamera::CCameraAdvancedSettings* settings = NULL;
	CCamera::CCameraContinuousZoom* zoom = NULL;

	// using observer 2 if exist 
	MCameraObserver2* observer2 = NULL;
	
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));
	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		INFO_PRINTF1(_L("Create advanced settings from CCamera object"));
		settings = 	static_cast<CCamera::CCameraAdvancedSettings*> (camera->CustomInterface(KECamAdvancedSettingUid));
	    if (settings!= NULL)
	    	{
	    	CleanupStack::PushL(settings);
	    	INFO_PRINTF1(_L("CCameraAdvancedSettings object was created"));
	    	CCamera::CCameraAdvancedSettings::TContinuousZoomType continuousZoomType =
	    		 		CCamera::CCameraAdvancedSettings::EContinuousZoomMixed;
	    	
	    	INFO_PRINTF1(_L("Create continuous zoom from advanced settings object"));
	    	TRAP(error, settings->CreateContinuousZoomL(*this, continuousZoomType, zoom));
	    	if (error == KErrNone)
	    		{
	    		CleanupStack::PushL(zoom);
	    		}
	    	else
	    		{
	    		ERR_PRINTF2(_L("Unexpected %d return from CCamera::CreateContinuousZoomL()"), error);
	    		result = EFail;
	    		User::Leave(KErrNoMemory);
	    		}
	    	}
	    else
    		{
    		ERR_PRINTF1(_L("Could not create CCameraAdvancedSettings"));
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}
		}
	else 
		{
		ERR_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}
	
	CleanupStack::PopAndDestroy(3, camera);
	__MM_HEAP_MARKEND;
	
	// create a settings object using New2L
	__MM_HEAP_MARK;
	INFO_PRINTF1(_L("Create camera using Camera::New2L() and MCameraObserver2"));
	
	TRAP(error, camera = CCamera::New2L(*observer2, 0,0));

	if (error==KErrNone)
		{
		CleanupStack::PushL(camera);
		TRAP(error, settings  = CCamera::CCameraAdvancedSettings::NewL(*camera));	
		if (error==KErrNone)
	    	{
	    	CleanupStack::PushL(settings);
	  		INFO_PRINTF1(_L("CCameraAdvancedSettings object was created using NewL")); 
	    	CCamera::CCameraAdvancedSettings::TContinuousZoomType continuousZoomType =
	    		 		CCamera::CCameraAdvancedSettings::EContinuousZoomMixed;
	    	
	    	INFO_PRINTF1(_L("Create continuous zoom from advanced settings object"));
	    	TRAP(error, settings->CreateContinuousZoomL(*this, continuousZoomType, zoom));
	    	if (error == KErrNone)
	    		{
	    		CleanupStack::PushL(zoom);
	    		}
	    	else
	    		{
	    		ERR_PRINTF2(_L("Unexpected %d return from CCamera::CreateContinuousZoomL()"), error);
	    		result = EFail;
	    		User::Leave(KErrNoMemory);
	    		}
	    	}
	    else
    		{
    		ERR_PRINTF1(_L("CCameraAdvancedSettings object was not created using NewL")); 
    		result = EFail;
    		User::Leave(KErrNoMemory);
    		}
		}
	else 
		{
		ERR_PRINTF2(_L("Unexpected %d return from CCamera::New2L(): observer 2"), error);
		result = EFail;	
		User::Leave(KErrNoMemory);	
		}

	CleanupStack::PopAndDestroy(3, camera);
	__MM_HEAP_MARKEND;
	return result;
	}

RECamContinuousZoomTest* RECamContinuousZoomTest::NewL(TBool aAllocTest)
	{
	RECamContinuousZoomTest* self = new (ELeave) RECamContinuousZoomTest(aAllocTest);
	return self;	
	}

RECamContinuousZoomTest::RECamContinuousZoomTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-060-HP");
	}

TVerdict RECamContinuousZoomTest::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	CCamera* camera = NULL;
	TInt callbackError = KErrNone;
	TDummyObserver observer2(callbackError);

	CCamera::CCameraAdvancedSettings* settings = NULL;
		
	camera = CCamera::New2L(observer2, 0, 0);
	CleanupStack::PushL(camera);
		
    settings = static_cast<CCamera::CCameraAdvancedSettings*>(camera->CustomInterface(KECamAdvancedSettingUid));
    if (settings == NULL)
    	{
    	ERR_PRINTF1(_L("--Error: Failed to get the CCameraAdvancedSettings"));
    	User::Leave(KErrGeneral);
    	}
    CleanupStack::PushL(settings);

	TUint supportedContinuousZoomType = 100;
	settings->GetSupportedContinuousZoomTypeL(supportedContinuousZoomType);
	if(supportedContinuousZoomType != 0)
		{
		ERR_PRINTF1(_L("GetSupportedContinuousZoomTypeL supported!")); 
		result = EFail;
		CleanupStack::PopAndDestroy(2, camera);
		return result;
		}
		
	CCamera::CCameraAdvancedSettings::TContinuousZoomType continuousZoomType =
		 		CCamera::CCameraAdvancedSettings::EContinuousZoomMixed;
	
	CCamera::CCameraContinuousZoom* continuousZoom = NULL;
	settings->CreateContinuousZoomL(*this, continuousZoomType, continuousZoom);
	CleanupStack::PushL(continuousZoom);
	
	CCamera::CCameraAdvancedSettings::TContinuousZoomSupportInfo info;
	continuousZoom->GetContinuousZoomSupportInfoL(info);
	
	INFO_PRINTF1(_L("Fake supported continuous zoom settings are:"));
	INFO_PRINTF2(_L("Max speed supported = %d"), info.iMaxSpeedSupported);
	INFO_PRINTF2(_L("Min acceleration supported = %d"), info.iMinAccelerationSupported);
	INFO_PRINTF2(_L("Max acceleration supported = %d"), info.iMaxAccelerationSupported);
	INFO_PRINTF2(_L("Min continuous zoom limit = %d"), info.iContinuousZoomMinLimit);
	INFO_PRINTF2(_L("Max continuous zoom limit = %d"), info.iContinuousZoomMaxLimit);
	
	CCamera::CCameraAdvancedSettings::TContinuousZoomParameters param;
	param.iContinuousZoomType = continuousZoomType;
	param.iContinuousZoomAcceleration = 0;
	param.iContinuousZoomSpeed = 1;
	param.iContinuousZoomLimit = 5;
	param.iZoomDirection = CCamera::CCameraAdvancedSettings::EZoomDirectionWide;

	INFO_PRINTF1(_L("Calling StartContinuousZoomL()"));
	continuousZoom->StartContinuousZoomL(param);

	INFO_PRINTF1(_L("Calling StopContinuousZoomL()"));
	continuousZoom->StopContinuousZoom();
	
	INFO_PRINTF1(_L("Creating second continuous zoom object and comparing unique zoom ids"));
	TInt id = 0;
	TInt secondId = 0;
	continuousZoom->GetContinuousZoomId(id);
	
	CCamera::CCameraContinuousZoom* secondContinuousZoom = NULL;
	settings->CreateContinuousZoomL(*this, continuousZoomType, secondContinuousZoom);
	secondContinuousZoom->GetContinuousZoomId(secondId);
	
	INFO_PRINTF2(_L("Zoom Id of first object is %d"), id);
	INFO_PRINTF2(_L("Zoom Id of second object is %d"), secondId);
	if(id == secondId)
		{
		ERR_PRINTF1(_L("Zoom ids of both continuous zoom objects are the same"));
		result = EFail;
		delete secondContinuousZoom;
		CleanupStack::PopAndDestroy(3, camera);
		return result;
		}
	delete secondContinuousZoom;
	
	INFO_PRINTF1(_L("Attempting to start continuous zoom with 'unsupported' values"));
	param.iContinuousZoomSpeed = 100;
	param.iContinuousZoomAcceleration = -10;
	param.iContinuousZoomLimit = 20;
	TRAPD(err, continuousZoom->StartContinuousZoomL(param));
	if(err == KErrArgument)
		{
		INFO_PRINTF2(_L("StartContinuousZoom() correctly failed with %d"), err);
		}
	else if(err == KErrNone)
		{
		ERR_PRINTF1(_L("StartContinuousZoom() succeeded with unsupported values"));
		result = EFail;
		}
	else
		{
		ERR_PRINTF2(_L("StartContinuousZoom() failed with unexpected error %d"), err);
		result = EFail;
		}
	
	CleanupStack::PopAndDestroy(3, camera);
	__MM_HEAP_MARKEND;
	return result;
	}


RECamDependantSupportedDriveModesTest* RECamDependantSupportedDriveModesTest::NewL(TBool aAllocTest)
	{
	RECamDependantSupportedDriveModesTest* self = new (ELeave) RECamDependantSupportedDriveModesTest(aAllocTest);
	return self;	
	}

RECamDependantSupportedDriveModesTest::RECamDependantSupportedDriveModesTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-ECM-ADV-U-061-HP");
	}

TVerdict RECamDependantSupportedDriveModesTest::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	CCamera* camera = NULL;
	MCameraObserver* observer = NULL;
	MCameraObserver2* observer2 = NULL;
	CCamera::CCameraAdvancedSettings* settings = NULL;

	INFO_PRINTF1(_L("Testing drive modes returned for legacy CCamera::NewL()"));
	camera = CCamera::NewL(*observer, 0);
	CleanupStack::PushL(camera);
		
    settings = static_cast<CCamera::CCameraAdvancedSettings*>(camera->CustomInterface(KECamAdvancedSettingUid));
    if (settings == NULL)
    	{
    	ERR_PRINTF1(_L("--Error: Failed to get the CCameraAdvancedSettings"));
    	User::Leave(KErrGeneral);
    	}
    CleanupStack::PushL(settings);

    CCamera::CCameraAdvancedSettings::TDriveMode driveMode = settings->DriveMode();
    if(driveMode != CCamera::CCameraAdvancedSettings::EDriveModeAuto)
    	{
    	ERR_PRINTF1(_L("Drive mode returned for CCamera::NewL() not CCameraAdvancedSettings::EDriveModeAuto as expected"));
    	result = EFail;
    	return result;
    	}

    TInt supportedDriveModes = settings->SupportedDriveModes();
    if(supportedDriveModes & CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture)
    	{
    	ERR_PRINTF1(_L("Unexpected error - EDriveModeTimeNudgeCapture supported for CCamera::NewL()"));
    	result = EFail;
    	return result;
    	}

    settings->SetDriveMode(CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture);
    driveMode = settings->DriveMode();
    if(driveMode == CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture)
    	{
    	ERR_PRINTF1(_L("Unexpected error - drive mode is EDriveModeTimeNudgeCapture for CCamera::NewL()"));
    	result = EFail;
    	return result;
    	}
    else
    	{
    	INFO_PRINTF1(_L("Supported drive modes do not include new EDriveTimeNudgeCapture - expected for CCamera::NewL()"));
    	}
    CleanupStack::PopAndDestroy(2, camera);



    INFO_PRINTF1(_L("Now testing drive modes returned for CCamera::New2L()"));
	camera = CCamera::New2L(*observer2, 0, 0);
	CleanupStack::PushL(camera);
		
    settings = static_cast<CCamera::CCameraAdvancedSettings*>(camera->CustomInterface(KECamAdvancedSettingUid));
    if (settings == NULL)
    	{
    	ERR_PRINTF1(_L("--Error: Failed to get the CCameraAdvancedSettings"));
    	User::Leave(KErrGeneral);
    	}
    CleanupStack::PushL(settings);

    supportedDriveModes = settings->SupportedDriveModes();
    settings->SetDriveMode(CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture);
    driveMode = settings->DriveMode();
    CleanupStack::PopAndDestroy(2, camera);

    if(!(supportedDriveModes & CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture))
    	{
    	ERR_PRINTF1(_L("Unexpected error - EDriveModeTimeNudgeCapture not supported for CCamera::New2L()"));
    	result = EFail;
    	return result;
    	}
    else if(driveMode != CCamera::CCameraAdvancedSettings::EDriveModeTimeNudgeCapture)
    	{
    	ERR_PRINTF1(_L("Unexpected error - drive mode is not EDriveModeTimeNudgeCapture for CCamera::New2L()"));
    	result = EFail;
    	return result;
    	}
    else
    	{
    	INFO_PRINTF1(_L("Supported drive modes include new EDriveTimeNudgeCapture - expected for CCamera::New2L()"));
    	}

	__MM_HEAP_MARKEND;
	return result;
	}
