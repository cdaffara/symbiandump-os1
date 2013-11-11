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
//

#include <hal.h>
#include <hal_data.h> 

// Specific includes for this test code
#include <mmf/common/mmfstandardcustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#endif
#include "tsu_mmf_avictrl_teststep.h"
#include "tsu_mmf_avictrl_testsuite.h"
#include "tsu_mmf_avictrlplay.h"
#include "tsu_mmf_avictrltestids.h"
#define KExtraIterations 5

const TUint RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::KMaxFreeRamForTest;

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlPlay::RTestStepAviPlayCtrlPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
	
/**
Do the test step. This test Primes the AviPlayController and Plays an avi file to its completion
and checks for the proper play completion
*/
TVerdict RTestStepAviPlayCtrlPlay::DoTestStepL()
	{
	INFO_PRINTF1(_L("this step plays an avi file "));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	CActiveScheduler::Start();	
	return iTestStepResult;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlVolume::RTestStepAviPlayCtrlVolume(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
	
/**
Do the test step.This tests setting the volume on AviPlayController in Stopped, Primed and Playing 
states and checks if the volume set is same as the volume got.
*/
TVerdict RTestStepAviPlayCtrlVolume::DoTestStepL( void )
	{
	TInt errorCode = 0;
	RMMFVideoPlayControllerCustomCommands  thePlayCommander(iController);
	TBool enabled = EFalse;
	thePlayCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("this step tests SetVolume/GetVolume() APIs of AviPlayController"));
	RMMFAudioPlayDeviceCustomCommands theAudioDevice(iController);
	TInt maxVolume(0);
	errorCode =theAudioDevice.GetMaxVolume(maxVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetMaxVolume() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt theVolume = maxVolume /2;
	TTimeIntervalMicroSeconds ramp(100);
	 //try setting volume in Stopped State
	errorCode =theAudioDevice.SetVolume(theVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolume() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewVolume (0);
	errorCode =theAudioDevice.GetVolume(theNewVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVolume() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	 if( theNewVolume != theVolume )
	 	{
	       INFO_PRINTF1(_L("Set and Get Volumes are different during Stopped State"));
	       return EFail;
	    	}
	errorCode =theAudioDevice.SetVolumeRamp(ramp);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	    	
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting volume in Primed State
	errorCode =theAudioDevice.SetVolume(theVolume);
	if(errorCode)
		{
	    	INFO_PRINTF2(_L("SetVolume() during Primed State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	errorCode =theAudioDevice.GetVolume(theNewVolume);
	if(errorCode)
	    	{
	    	INFO_PRINTF2(_L("GetVolume() during Primed State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	if( theNewVolume != theVolume )
	    	{
		INFO_PRINTF1(_L("Set and Get Volumes are different during Primed State"));
	       return EFail;
	    	}
	 errorCode =theAudioDevice.SetVolumeRamp(ramp);
	 if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during primed State is failed with  %d "), errorCode);
		return EFail;
		}	   	
	 errorCode = iController.Play();
	 if(errorCode)
	 	{
	    	INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
	    	return EInconclusive;
	    	}
	   //try setting volume in Playing State
	 errorCode =theAudioDevice.SetVolume(theVolume);
	 if(errorCode)
	  	{
	    	INFO_PRINTF2(_L("SetVolume() during Playing State is failed with  %d "), errorCode);
	    	iController.Stop();
	    	return EFail;
	    	}
	 errorCode =theAudioDevice.GetVolume(theNewVolume);
	 if(errorCode)
	    	{
	    	INFO_PRINTF2(_L("GetVolume() during Playing State is failed with  %d "), errorCode);
	    	iController.Stop();
	    	return EFail;
	    	}
	 if(theNewVolume != theVolume)
		{
	        //stop the controller
	        INFO_PRINTF1(_L("Set and Get Volumes are different during Playing State"));
	        iController.Stop();
	        return EFail;
		}
	 errorCode =theAudioDevice.SetVolumeRamp(ramp);
	 if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during play State is failed with  %d "), errorCode);
		return EFail;
		}		
    //get the play completion event
	 CActiveScheduler::Start();
	 return iTestStepResult; 		
	}

	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlPlayBalance::RTestStepAviPlayCtrlPlayBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
	
/**
Do the test step.This tests setting the PlayBalance on AviPlayController in Stopped, Primed and Playing 
states and checks if the PlayBalance set is same as the PlayBalance got.
*/ 
TVerdict RTestStepAviPlayCtrlPlayBalance::DoTestStepL( void )
	 {
	TInt errorCode=0;
	INFO_PRINTF1(_L("this checks if audio is enabled on the sample.avi file"));
	RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
	TBool enabled = EFalse;
	thePlayCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("this step tests SetBalance/GetBalance() APIs of AviPlayController "));
	RMMFAudioPlayDeviceCustomCommands theAudioDevice( iController );
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance(theBalance);
	if(errorCode)
	    	{
	    	INFO_PRINTF2(_L("SetBalance() during Stopped State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	TInt theNewBalance (0);
	errorCode =theAudioDevice.GetBalance(theNewBalance);
	if(errorCode)
		{
	    	INFO_PRINTF2(_L("GetBalance() during Stopped State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	if( theNewBalance != theBalance )
		{
		INFO_PRINTF1(_L("Set and Get Balances are different during Stopped State"));
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theAudioDevice.SetBalance( theBalance );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theAudioDevice.GetBalance( theNewBalance );
	if(errorCode)
	       {
		INFO_PRINTF2(_L("GetBalance() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	 if( theNewBalance != theBalance )
		{
		INFO_PRINTF1(_L("Set and Get Balances are different during Primed State"));
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theAudioDevice.SetBalance(theBalance);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theAudioDevice.GetBalance(theNewBalance);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetBalance() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if(theNewBalance != theBalance)
		{
	       //stop the controller
	       INFO_PRINTF1(_L("Set and Get Balances are different during Playing State"));
	       iController.Stop();
	       return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlDuration::RTestStepAviPlayCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
		{
		
		}

/**
 Do the test step.This tests checks for the duration of the file in Stopped, Primed and Playing states. 
*/
TVerdict RTestStepAviPlayCtrlDuration::DoTestStepL(void)
	{
	const TTimeIntervalMicroSeconds  KExpectedDuration = 6300000;
	INFO_PRINTF1(_L("this step checks for duration of the file"));
	TTimeIntervalMicroSeconds expectedDuration(KExpectedDuration);
	TTimeIntervalMicroSeconds currentDuration;
	TInt errorCode = iController.GetDuration(currentDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(currentDuration != expectedDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), currentDuration. Int64());
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.GetDuration(currentDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(currentDuration != expectedDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), currentDuration. Int64());
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.GetDuration(currentDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	else if(currentDuration != expectedDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), currentDuration. Int64());
		iController.Stop();
		return EFail;
		}
	return EPass;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlPause::RTestStepAviPlayCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step. This tests the Pause functionality of the AviPlayController in Stopped, Primed and Playing 
states and checks if it returns KErrNot Supported.
*/
TVerdict RTestStepAviPlayCtrlPause::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests Pause() API of AviPlayController "));
	TInt errorCode = iController.Pause();
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Pause() during Stopped State is failed to return KErrNotSupported but returned %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Pause();
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Pause() during Primed State is failed to return KErrNotSupported but returned %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Pause();
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Pause() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("Play() after Pause() is failed with  %d "), errorCode);
		return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlStop::RTestStepAviPlayCtrlStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step. This tests the Stop functionality of the AviPlayController in Stopped, Primed and Playing 
states It expects KErrNotReady error if Stop is called during Stopped state.
*/
TVerdict RTestStepAviPlayCtrlStop::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests Stop() API of AviPlayController "));
	TInt errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Stopped State is failed with %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Primed State is failed with %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	User::After(2000000);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() after Stop() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}


/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlSetGetCropRegion::RTestStepAviPlayCtrlSetGetCropRegion(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
Do the test step.This test tries to set and get the crop region for the file.
*/
 TVerdict RTestStepAviPlayCtrlSetGetCropRegion::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetCropRegion() APIs of AviPlayController"));
	TRect cropRegion(1,3,5,7);
	TRect newCropRegion(1,3,5,7);
	RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
	TInt errorCode =thePlayCommander.SetCropRegion(cropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetCropRegion() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =thePlayCommander.GetCropRegion(newCropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetCropRegion() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =thePlayCommander.SetCropRegion(cropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetCropRegion() during primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =thePlayCommander.GetCropRegion(newCropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetCropRegion() during primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =thePlayCommander.SetCropRegion(cropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetCropRegion() during play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =thePlayCommander.GetCropRegion(newCropRegion);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetCropRegion() during play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	CActiveScheduler::Start();	
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlSetGetFrameRate::RTestStepAviPlayCtrlSetGetFrameRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step.This test tries to set and get the framerate for the file
*/
TVerdict RTestStepAviPlayCtrlSetGetFrameRate::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests SetFrameRate/GetFrameRate() APIs of AviPlayController"));
	TInt KFrameRate = 10;
	TReal32 frameRate(KFrameRate);
	TReal32 expectedFrameRate(KFrameRate) ;
	RMMFVideoControllerCustomCommands theVideoCommander(iController);
	TInt errorCode =theVideoCommander.SetFrameRate(frameRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetFrameRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoCommander.GetFrameRate(expectedFrameRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if(expectedFrameRate != KFrameRate)
		{
	        INFO_PRINTF1(_L("Set and Get Framerates are different during Stopped State"));
	        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theVideoCommander.SetFrameRate(frameRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetFrameRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoCommander.GetFrameRate(expectedFrameRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if(expectedFrameRate != KFrameRate)
		{
	       INFO_PRINTF1(_L("Set and Get Framerates are different during Primed State"));
	       return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	//try getting framerate inPlaying State
	errorCode =theVideoCommander.SetFrameRate(frameRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetFrameRate() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}	
	errorCode =theVideoCommander.GetFrameRate(expectedFrameRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameRate() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if(expectedFrameRate != KFrameRate)
		{
	        INFO_PRINTF1(_L("Set and Get Framerates are different during Playing State"));
	        return EFail;
		}
	CActiveScheduler::Start();	
	return EPass;
	}


/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlGetFrameSize::RTestStepAviPlayCtrlGetFrameSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
Do the test step.This test tries to get the framesize from the file.
*/
TVerdict RTestStepAviPlayCtrlGetFrameSize::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetFrameSize() APIs of AviPlayController"));
	TSize frameSize(0,0);
	TSize expectedFrameSize(176,144);
	RMMFVideoControllerCustomCommands theVideoCommander(iController);
	TInt errorCode =theVideoCommander.GetVideoFrameSize(frameSize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if(frameSize != expectedFrameSize)
		{
	        INFO_PRINTF1(_L("Frame sizes are different during Stopped states"));
	        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theVideoCommander.GetVideoFrameSize(frameSize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( frameSize != expectedFrameSize )
		{
	        INFO_PRINTF1(_L("Frame sizes are different during Primed state"));
	        return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	errorCode =theVideoCommander.GetVideoFrameSize(frameSize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if( frameSize != expectedFrameSize )
		{
	        INFO_PRINTF1(_L("Frame sizes are different during play state"));
	        iController.Stop();
	        return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return EPass;
	}


/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlGetAudioDataType::RTestStepAviPlayCtrlGetAudioDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{

	}

/**
Do the test step.This test tries to get the audio data type from the avi file.
*/
 TVerdict RTestStepAviPlayCtrlGetAudioDataType::DoTestStepL()
	{
	INFO_PRINTF1(_L("this step tests GetAudioType() and GetAudioBitRate() APIs of AviPlayController"));
	TFourCC audioCodec;
	TFourCC expectedAudioCodec(KMMFFourCCCodePCM16);
	TInt errorCode = 0;
	TInt bitRate = 705600;
	TInt newBitRate =0;
	RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
	TBool enabled(EFalse);
	thePlayCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}	
	RMMFVideoControllerCustomCommands theCommander(iController);
	errorCode =theCommander.GetAudioCodec(audioCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioCodec() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( audioCodec != expectedAudioCodec )
		{
	        INFO_PRINTF1(_L("Wrong audio codec was returned"));
	        return EFail;
		}
	errorCode = theCommander.GetAudioBitRate(newBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if(newBitRate != bitRate )
		{
	        INFO_PRINTF1(_L("Wrong audio bitrate was returned"));
	        return EFail;
		}	
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//Check for audio codec
	errorCode =theCommander.GetAudioCodec(audioCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioCodec() during primed  State is failed with  %d "), errorCode);
		return EFail;
		}
	if( audioCodec != expectedAudioCodec )
		{
	        INFO_PRINTF1(_L("Wrong audio codec was  returned"));
	        return EFail;
		}
	errorCode = theCommander.GetAudioBitRate(newBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during primed  State is failed with  %d "), errorCode);
		return EFail;
		}
	if(newBitRate != bitRate )
		{
	        INFO_PRINTF1(_L("Wrong audio bitrate was returned"));
	        return EFail;
		}		
	errorCode = iController.Play();						
	//Check for audio codec
	errorCode =theCommander.GetAudioCodec(audioCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioCodec() during play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if( audioCodec != expectedAudioCodec )
		{
	        INFO_PRINTF1(_L("Wrong audio codec was returned"));
	        iController.Stop();
	        return EFail;
		}
	errorCode = theCommander.GetAudioBitRate(newBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if(newBitRate != bitRate )
		{
	        INFO_PRINTF1(_L("Wrong audio bitrate was returned"));
	        iController.Stop();
	        return EFail;
		}		
	CActiveScheduler::Start();	
	return EPass;			
	}
 
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlGetVideoDataType::RTestStepAviPlayCtrlGetVideoDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}
/**
Do the test step.This test tries to get the video mime type
*/
TVerdict RTestStepAviPlayCtrlGetVideoDataType::DoTestStepL(void)
	{
	const TInt KBufferSize = 256;
	INFO_PRINTF1(_L("this step tests GetVideoMimeType() APIs of AviPlayController"));
	HBufC8* iMimeType = HBufC8::NewL(KBufferSize);
	TPtr8 des = iMimeType->Des();
	TInt errorCode = 0;
	char xv[]="XVID";
	TUint8* p = (TUint8*)xv;
	TPtrC8 temp(p);	
	RMMFVideoControllerCustomCommands theCommander(iController);
	errorCode =theCommander.GetVideoMimeType(des);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVideoMimeType() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if (des.Compare(temp))
		{
	       INFO_PRINTF1(_L("Wrong video mime type returned"));
	       return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Correct video mime type returned"));	
		}	
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theCommander.GetVideoMimeType(des);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVideoMimeType() during primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if (des.Compare(temp))
		{
	       INFO_PRINTF1(_L("Wrong video mime type returned"));
	       return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Correct video mime type returned"));	
		}	
			
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theCommander.GetVideoMimeType(des);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVideoMimeType() during play State is failed with  %d "), errorCode);
		return EFail;
		}
	if (des.Compare(temp))
		{
	       INFO_PRINTF1(_L("Wrong video mime type returned"));
	       return EFail;
		}
	else
		{
		INFO_PRINTF1(_L("Correct video mime type returned"));	
		}
	CActiveScheduler::Start();		
	return EPass;	
	}

	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlMetaData::RTestStepAviPlayCtrlMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step.This test tries to fetch the metadata from the avi file.
*/
TVerdict RTestStepAviPlayCtrlMetaData::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetNumberOfMetaDataEntries/GetMetaDataEntryL() APIs of AviPlayController "));
	TInt numEntries = 0;
	TInt errorCode = iController.GetNumberOfMetaDataEntries( numEntries );
	CMMFMetaDataEntry* metaEntry = NULL;
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetNumberOfMetaDataEntries() is failed with  %d "), errorCode);
		return EFail;
	 	}
	if(numEntries == 0)
		{
		INFO_PRINTF1(_L("File does not have meta entries "));
		}
	else
		{
		for(TInt counter = 0; counter < numEntries; ++counter)
        		{
		      	TRAP(errorCode, metaEntry =  iController.GetMetaDataEntryL(counter));
		      	if(errorCode)
		      		{
		      		INFO_PRINTF2(_L("GetMetaDataEntryL is failed with  %d "), errorCode);
		      		return EFail;
		      		}
		      	INFO_PRINTF3(_L("%S    %S"), &metaEntry->Name(), &metaEntry->Value());
        		}
		}
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlSetGetRotation::RTestStepAviPlayCtrlSetGetRotation(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step.This test tries to set and get the rotation
*/
TVerdict RTestStepAviPlayCtrlSetGetRotation::DoTestStepL(void)	
	{
	INFO_PRINTF1(_L("this step tests SetRotation/GetRotation() APIs of AviPlayController"));
	TVideoRotation aRotation;
	RMMFVideoPlayControllerCustomCommands theVideoPlayCommander(iController);
	TInt errorCode = theVideoPlayCommander.SetRotation(EVideoRotationClockwise90);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetRotation() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetRotation(aRotation);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetRotation() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theVideoPlayCommander.SetRotation(EVideoRotationClockwise90);	
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetRotation() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetRotation(aRotation);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetRotation() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theVideoPlayCommander.SetRotation(EVideoRotationClockwise90);	
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetRotation() during Play State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetRotation(aRotation);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetRotation() during Play State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	CActiveScheduler::Start();
	return EPass;					
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlSetGetScaling::RTestStepAviPlayCtrlSetGetScaling(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
* Do the test step.This test tries to do the scaling and expect KErrNotsupported
*/
TVerdict RTestStepAviPlayCtrlSetGetScaling::DoTestStepL(void)	
	{
	INFO_PRINTF1(_L("this step tests SetScaling/GetScaling() APIs of AviPlayController"));
	TReal32 aWidthPercentage = .75;
	TReal32 aHeightPercentage = .3;
	TBool aAntiAliasFiltering = EFalse;
	
	RMMFVideoPlayControllerCustomCommands theVideoPlayCommander(iController);
	TInt errorCode = theVideoPlayCommander.SetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetScaleFactor() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetScaleFactor() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theVideoPlayCommander.SetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetScaleFactor() during primed State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetScaleFactor() during primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theVideoPlayCommander.SetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetScaleFactor() during play  State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode =theVideoPlayCommander.GetScaleFactor(aWidthPercentage,aHeightPercentage,aAntiAliasFiltering);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetScaleFactor() during play  State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	CActiveScheduler::Start();
	return EPass;					
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlSetPosition::RTestStepAviPlayCtrlSetPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
		{
		
		}

/**
 Do the test step.This test tries to set the position in the file and expect KErrNotSupported
 */
TVerdict RTestStepAviPlayCtrlSetPosition::DoTestStepL(void)	
	{
	INFO_PRINTF1(_L("this step tests SetPosition() APIs of AviPlayController"));
	TTimeIntervalMicroSeconds aPosition;
	TInt errorCode = KErrNone;
	errorCode = iController.SetPosition(aPosition);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetPosition() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(aPosition);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(aPosition);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetPosition() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}	
	CActiveScheduler::Start();
	return EPass;					
	}


/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlGetVideoBitrate::RTestStepAviPlayCtrlGetVideoBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step.This test tries to get the bitrate from the file
*/
TVerdict RTestStepAviPlayCtrlGetVideoBitrate::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetVideoBitrate() APIs of AviPlayController"));
	TInt aBitRate;
	RMMFVideoControllerCustomCommands theVideoCommander(iController);
	TInt errorCode =theVideoCommander.GetVideoBitRate(aBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Stopped State returned KErrNotSupported %d "), errorCode);	
		}		
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theVideoCommander.GetVideoBitRate(aBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Primed State returned KErrNotSupported %d "), errorCode);	
		}		
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	errorCode =theVideoCommander.GetVideoBitRate(aBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}	
	else
		{
		INFO_PRINTF2(_L("GetVideoBitRate() during Play State returned KErrNotSupported %d "), errorCode);	
		}			
	CActiveScheduler::Start();	
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlGetAudioBitrate::RTestStepAviPlayCtrlGetAudioBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
Do the test step.This test tries to fetch the audio bit rate.
*/
TVerdict RTestStepAviPlayCtrlGetAudioBitrate::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetAudioBitrate() APIs of AviPlayController"));
	TInt audioBitRate;
	RMMFVideoControllerCustomCommands theVideoCommander(iController);
	TInt errorCode =theVideoCommander.GetAudioBitRate(audioBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Stopped State returned KErrNotSupported %d "), errorCode);	
		}					
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	else
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Primed State returned KErrNotSupported %d "), errorCode);	
		}						
	errorCode =theVideoCommander.GetAudioBitRate(audioBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	//try getting framerate inPlaying State
	errorCode =theVideoCommander.GetAudioBitRate(audioBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	else
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Playing State returned KErrNotSupported %d "), errorCode);	
		}								
	CActiveScheduler::Start();	
	return EPass;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviCtrlDirectScreenAccess::RTestStepAviCtrlDirectScreenAccess(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
This test code tries to check the direct screen access property.
*/	
TVerdict RTestStepAviCtrlDirectScreenAccess::DoTestStepL(void)
	{	
	INFO_PRINTF1(_L("this step tests DirectScreenAccessL of AviPlayController when audio not enabled"));
	RMMFVideoPlayControllerCustomCommands theVideoCommander(iController);
	TMMFDSAEvent  dsaEvent1 = EAbortDSA;
	TMMFDSAEvent  dsaEvent2 = EResumeDSA;
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		} 
	errorCode = theVideoCommander.DirectScreenAccessEvent(dsaEvent2);	
	if(errorCode)
		{
		INFO_PRINTF2(_L("DirectScreenAccessEvent() is failed during primed state  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
	 	{
    	INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
    	return EInconclusive;
    	}
	errorCode = theVideoCommander.DirectScreenAccessEvent(dsaEvent1);	
	if(errorCode)
		{
		INFO_PRINTF2(_L("DirectScreenAccessEvent() is failed during primed state  %d "), errorCode);
		return EInconclusive;
		}
	CActiveScheduler::Start();	
	return iTestStepResult;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviCtrlSetPlayWindow::RTestStepAviCtrlSetPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
This test code tries to set the set playwindow and expects KErrNotSupported
*/	
TVerdict RTestStepAviCtrlSetPlayWindow::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step plays an avi file "));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoPlayControllerCustomCommands theVideoCommander(iController);	
	TTimeIntervalMicroSeconds theDuration;
	errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt64 duration = theDuration. Int64();
	TTimeIntervalMicroSeconds thePlayStart(duration/2);
	TTimeIntervalMicroSeconds thePlayEnd(duration);
	errorCode = theVideoCommander.Play(thePlayStart, thePlayEnd);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() after SetPlayWindow() is failed with  %d "), errorCode);
		return EInconclusive;
		}	
	CActiveScheduler::Start();
	return iTestStepResult;
	}
	
//Negative Tests
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviCtrlPlayInvalid::RTestStepAviCtrlPlayInvalid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

TVerdict RTestStepAviCtrlPlayInvalid::DoTestStepPreambleL(void)
	{
	return EPass;
	}

/**
Do the test step(Negative Test).This test tries to open an invalid file. Expects KErrCurrupt from AviPlayController
*/
TVerdict RTestStepAviCtrlPlayInvalid::DoTestStepL(void)
    	{
    	const TUid KAviPlayControllerUid   = {0x102737A5};
	INFO_PRINTF1(_L("This test tries to open an invalid file using AviPlayController"));
	iScheduler = new(ELeave)CActiveScheduler;
         CleanupStack::PushL(iScheduler );
	CActiveScheduler::Install(iScheduler );
	CleanupStack::Pop();
	iPriority.iPriority = 11;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
	 if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		return EInconclusive;
		}
	 iControllerEventMonitor->Start();
	 SetupReadFileConfigL();
	 errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	 if(errorCode != KErrCorrupt)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		return EFail;
		}
	 else
		{
		INFO_PRINTF2(_L("AddDataSource() returned KErrCorrupt with invalid .avi file %d "), errorCode);	
		return EPass;
		}
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviCtrlNotAudioEnabled::RTestStepAviCtrlNotAudioEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
		
	}

/**
This test step tries to get the volume and balance from a file where audio is not enabled and expect KErrNotsupported
*/
TVerdict RTestStepAviCtrlNotAudioEnabled::DoTestStepL(void)
	{
	TInt theNewBalance (0);
	TTimeIntervalMicroSeconds ramp(100);
	INFO_PRINTF1(_L("this step tests SetVolume/GetVolume() APIs of AviPlayController when audio not enabled"));
	RMMFAudioPlayDeviceCustomCommands theAudioDevice(iController);
	TInt maxVolume(0);
	TInt errorCode = theAudioDevice.GetMaxVolume(maxVolume);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetMaxVolume() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt theVolume = maxVolume /2;
	 //try setting volume in Stopped State
	errorCode =theAudioDevice.SetVolume(theVolume);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetVolume() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewVolume (0);
	errorCode =theAudioDevice.GetVolume(theNewVolume);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetVolume() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance(theBalance);
	if(errorCode != KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("SetBalance() during Stopped State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	errorCode =theAudioDevice.GetBalance(theNewBalance);
	if(errorCode != KErrNotSupported)
		{
	    	INFO_PRINTF2(_L("GetBalance() during Stopped State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	 errorCode =theAudioDevice.SetVolumeRamp(ramp);
	 if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}	   		
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	  //try setting volume in Primed State
	errorCode =theAudioDevice.SetVolume(theVolume);
	if(errorCode != KErrNotSupported)
	 	{
	    	INFO_PRINTF2(_L("SetVolume() during Primed State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	errorCode =theAudioDevice.GetVolume(theNewVolume);
	if(errorCode != KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetVolume() during Primed State is failed with  %d "), errorCode);
	    	return EFail;
	    	}
	errorCode =theAudioDevice.SetVolumeRamp(ramp);
     if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during primed State is failed with  %d "), errorCode);
		return EFail;
		}    	
	theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance(theBalance);
	if(errorCode != KErrNotSupported)
    	{
    	INFO_PRINTF2(_L("SetBalance() during primed State is failed with  %d "), errorCode);
    	return EFail;
    	}
	errorCode =theAudioDevice.GetBalance(theNewBalance);
	if(errorCode != KErrNotSupported)
		{
    	INFO_PRINTF2(_L("GetBalance() during primed State is failed with  %d "), errorCode);
    	return EFail;
	    	}   	
	 errorCode = iController.Play();
	 if(errorCode)
	 	{
    	INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
    	return EInconclusive;
	    	}
	    //try setting volume in Playing State
	 errorCode =theAudioDevice.SetVolume(theVolume);
	  if(errorCode!= KErrNotSupported)
	  	{
    	INFO_PRINTF2(_L("SetVolume() during Playing State is failed with  %d "), errorCode);
    	iController.Stop();
    	return EFail;
	    	}
	 errorCode =theAudioDevice.GetVolume(theNewVolume);
	 if(errorCode!= KErrNotSupported)
    	{
    	INFO_PRINTF2(_L("GetVolume() during Playing State is failed with  %d "), errorCode);
    	iController.Stop();
    	return EFail;
    	}
	errorCode =theAudioDevice.SetVolumeRamp(ramp);
	 if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetVolumeRamp() during play State is failed with  %d "), errorCode);
		return EFail;
		}    	
	theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance(theBalance);
	if(errorCode != KErrNotSupported)
    	{
    	INFO_PRINTF2(_L("SetBalance() during playing State is failed with  %d "), errorCode);
    	return EFail;
    	}
	errorCode =theAudioDevice.GetBalance(theNewBalance);
	if(errorCode != KErrNotSupported)
		{
    	INFO_PRINTF2(_L("GetBalance() during playing State is failed with  %d "), errorCode);
    	return EFail;
    	}	
	  //get the play completion event
	  CActiveScheduler::Start();
	  return iTestStepResult;
	}
	
/**
Test step constructor. It initialises its own name, input filename and the screen number.
*/
RTestStepAviPlayCtrlSetScrnPlay::RTestStepAviPlayCtrlSetScrnPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	iScreenNumber = aScreenNumber;
	}

//Override base class Preamble
TVerdict RTestStepAviPlayCtrlSetScrnPlay::DoTestStepPreambleL(void)
	{
	RTestStepAviCtrlBase::DoTestStepPreambleL();
	return EPass;
	}

/**
This is from MMMFControllerEventMonitorObserver.which handles all the events from AviPlayController.
Base class version is overridden to avoid starting the active schedular twice.
*/
void RTestStepAviPlayCtrlSetScrnPlay::HandleEvent(const TMMFEvent& aEvent)
	{
	TInt errorCode = 0;
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF2(_L("PlayCompletion is passed with  %d "), aEvent.iErrorCode);
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PlayCompletion is failed with  %d "), aEvent.iErrorCode);
			}
		CActiveScheduler::Stop();	
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			//add code here to set display window and to update display region.	
			RMMFVideoPlayControllerCustomCommands thePlayCommander(iController);
			errorCode = thePlayCommander.SetDisplayWindow(iScreenRect,iClipRect);
			if(errorCode)
				{
				INFO_PRINTF2(_L("SetDisplayWindow failed with  %d "), errorCode);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				return;
				}
			errorCode = thePlayCommander.UpdateDisplayRegion(iRegion);
			if(errorCode)
				{
				INFO_PRINTF2(_L("UpdateDisplayRegion failed with  %d "), errorCode);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				return;
				}
			INFO_PRINTF2(_L("Open completion is passed with  %d "), aEvent.iErrorCode);	
			errorCode = thePlayCommander.Prepare();	
			if(errorCode)
				{
				INFO_PRINTF2(_L("Prepare failed with  %d "), errorCode);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}			
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("Video open complete is failed with  %d "), aEvent.iErrorCode);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
		{
		if((aEvent.iErrorCode == KErrNone) || (aEvent.iErrorCode == KErrEof))
			{
			INFO_PRINTF2(_L("Prepare completion is passed with  %d "), aEvent.iErrorCode);	
			errorCode = iController.Prime();
			if(errorCode)
				{
				INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				return;
				}
			errorCode = iController.Play();
			if(errorCode)
				{
				INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PrepareComplete is failed with  %d "),aEvent.iErrorCode);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("Unexpected Event from the Controller"));
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}			
	
	}

/**
This test step plays the video on the primary or secondary screen depending the value of screen number passed during its construction.
*/	
TVerdict RTestStepAviPlayCtrlSetScrnPlay::DoTestStepL(void)
	{
	INFO_PRINTF2(_L("this step plays an avi file on screen %d using RMMFVideoSetInitScreenCustomCommands"), iScreenNumber);
	iTestStepResult = EFail;
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	iControllerEventMonitor->Start();
	if(!SetupReadFileConfigL())
		{
		return EInconclusive;
		}
	RMMFVideoSetInitScreenCustomCommands setInitScrn(iController);
	errorCode = setInitScrn.SetInitScreenNumber(iScreenNumber);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetInitScreenNumber failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
#ifdef __EABI__ //we dont have secondary screen support on hardware. So fail gracefully.
	if(errorCode == KErrNotSupported && iScreenNumber == KSecondaryScreenNo)
		{
		INFO_PRINTF1(_L("No support for secondary display"));
		return EKnownFailure;
		}
#endif	
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	CActiveScheduler::Start();		
	return iTestStepResult;
	}
	
/**
Test step constructor. It initialises its own name, input filename and the screen number.
*/
RTestStepAviPlayCtrlSetInvalidScrn::RTestStepAviPlayCtrlSetInvalidScrn(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber)
:RTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	iScreenNumber = aScreenNumber;
	}
	
//Override base class Preamble
TVerdict RTestStepAviPlayCtrlSetInvalidScrn::DoTestStepPreambleL(void)
	{
	RTestStepAviCtrlBase::DoTestStepPreambleL();
	return EPass;
	}

/**
This test step tries to set an invalid screen on AviPlayController and expects AddDataSink to fail with KErrNotSupported..
*/
TVerdict RTestStepAviPlayCtrlSetInvalidScrn::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tries to set an invalid screen number on the AviPlayController"));
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	iControllerEventMonitor->Start();
	if(!SetupReadFileConfigL())
		{
		return EInconclusive;
		}
	RMMFVideoSetInitScreenCustomCommands setInitScrn(iController);
	errorCode = setInitScrn.SetInitScreenNumber(iScreenNumber);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetInitScreenNumber failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("AddDataSink failed to return KErrNotSupported but returned %d with an invalid screen"), errorCode);
		return EFail;
		}
	return EPass;
	}

/**
Test step constructor. It initialises its own name, input filename and the screen number.
*/
RTestStepAviPlayCtrlAddDataSinkTwice::RTestStepAviPlayCtrlAddDataSinkTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber)
:RTestStepAviPlayCtrlSetScrnPlay(aTestName, aSectName, aKeyName, aControllerUid, aScreenNumber)
	{
	}
/**
This test step tries to set an invalid screen after calling AddDataSink on AviController and tries playing.
It resets the controller after the play is successful and tries to call AddDataSink again and expects
it to fail with KErrNotSupported.
*/	
TVerdict RTestStepAviPlayCtrlAddDataSinkTwice::DoTestStepPreambleL(void)
	{
	RTestStepAviCtrlBase::DoTestStepPreambleL();
	return EPass;
	}

TVerdict RTestStepAviPlayCtrlAddDataSinkTwice::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tries to add data sink twice.Second time invalid screen"));
	iTestStepResult = EFail;
	TInt errorCode = iController.Open(KAviPlayControllerUid, iPriority);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Failed to load the controller  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	iControllerEventMonitor->Start();
	if(!SetupReadFileConfigL())
		{
		return EInconclusive;
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSink failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	RMMFVideoSetInitScreenCustomCommands setInitScrn(iController);
	errorCode = setInitScrn.SetInitScreenNumber(iScreenNumber);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetInitScreenNumber failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	if(errorCode)
		{
		INFO_PRINTF2(_L("AddDataSource failed with  %d "), errorCode);
		User::LeaveIfError( errorCode );
		}
	CActiveScheduler::Start();
	if(iTestStepResult == EFail)
		{
		return iTestStepResult;
		}
	errorCode = iController.Reset();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Reset is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.AddDataSink(KUidMmfAudioOutput,KNullDesC8);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("AddDataSink failed to return KErrNotSupported but returned %d with an invalid screen"), errorCode);
		return EFail;
		}
	return iTestStepResult;
	}


/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::
RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap(const TDesC& aTestName,
												const TDesC& aSectName,
												const TDesC& aKeyName,
												TUid aControllerUid)
	:
	RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid),
	iTestState(EStateDefault),
	iRChunk(),
	iKickoff01(NULL),
	iKickoff02(NULL),
	iKickoff03(NULL),
	iKickoff04(NULL),
	iKickoff05(NULL),
	iKickoff06(NULL)
	{
	}


/**
Do the test step. This test check that under low memory conditions the xvid
codec may leave and DevVideo will be notified by the codec and and in turn
AviPlayController will be notified too by DevVideo using the FatalError
callbacks.  Once the low memory conditions return to normal, the test case
Primes the AviPlayController and Plays an avi file to its completion and checks
for the proper play completion. This will prove that DevVideo is destroyed and
then recreated after a FatalError occurs.

The idea here is :

1.- To allocate enough memory with the aim of leaving around KMaxFreeRamForTest
bytes available for the rest of the test case.

2.- After that, the testcase Primes the AviPlayController and Plays an avi
file. Play is expected to fail with KErrTooBig (in the play completion event).

3.- Then returns memory back to the system.

4.- Then try to play the video file again, this time everything will work
fine...With this we test that DevVideo's been recreated...

*/
void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::KickoffTestL()
	{
	iTestState = EStateLoadKickoffTestLComplete;

	INFO_PRINTF1(_L("this step plays an avi file under low memory conditions for the xvid decoder "));

	// This will install two callbacks
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();

	// STEP 1: The idea here is to allocate enough memory with the aim of
	// leaving around KMaxFreeRamForTest bytes available for the rest of the
	// step. That will make the xvid decoder to fail the frame decoding as the
	// decoder heap will not be large enough.
	TCallBack callback01 (AllocChunkCallBack, this);
	delete iKickoff01;
	iKickoff01 = NULL;
	iKickoff01 = new (ELeave) CAsyncCallBack (callback01, CActive::EPriorityLow);
	

    // STEP 2: Prime the controller...
	TCallBack callback02 (PrimeControllerCallBack, this);
	delete iKickoff02;
	iKickoff02 = NULL;
	iKickoff02 = new (ELeave) CAsyncCallBack (callback02, CActive::EPriorityLow);

	// STEP 3: play is expected to fail with KErrTooBig (in the play completion event ) ...
	TCallBack callback03 (PlayControllerCallBack, this);
	delete iKickoff03;
	iKickoff03 = NULL;
	iKickoff03 = new (ELeave) CAsyncCallBack (callback03, CActive::EPriorityLow);

	// STEP 4: Return memory back to system here...
	TCallBack callback04 (ReturnChunckAndStopControllerCallBack, this);
	delete iKickoff04;
	iKickoff04 = NULL;
	iKickoff04 = new (ELeave) CAsyncCallBack (callback04, CActive::EPriorityLow);

	// Repeat prime....
	TCallBack callback05 (PrimeControllerCallBack, this);
	delete iKickoff05;
	iKickoff05 = NULL;
	iKickoff05 = new (ELeave) CAsyncCallBack (callback05, CActive::EPriorityLow);

	// ...and repeat play
	TCallBack callback06 (PlayControllerCallBack, this);
	delete iKickoff06;
	iKickoff06 = NULL;
	iKickoff06 = new (ELeave) CAsyncCallBack (callback06, CActive::EPriorityLow);

	// Start allocate chunck callback.
	iKickoff01->Call();
	}

void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::CloseTest()
	{
	delete iKickoff01; // no need to Cancel
	iKickoff01 = NULL;
	delete iKickoff02; // no need to Cancel
	iKickoff02 = NULL;
	delete iKickoff03; // no need to Cancel
	iKickoff03 = NULL;
	delete iKickoff04; // no need to Cancel
	iKickoff04 = NULL;
	delete iKickoff05; // no need to Cancel
	iKickoff05 = NULL;
	delete iKickoff06; // no need to Cancel
	iKickoff06 = NULL;

	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}


TInt RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::AllocChunkCallBack(TAny* aPtr)
	{
	RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap* self =
		static_cast<RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap*> (aPtr);
	self->DoAllocChunkCallBack();
	return KErrNone;
	}

void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::DoAllocChunkCallBack()
	{
	// Check simple FSM
	if (iTestState != EStateLoadKickoffTestLComplete)
		{
		INFO_PRINTF1(_L("FSM error in DoAllocChunkCallBack"));
		StopTest(KErrGeneral, EFail);
		return;
		}

	iTestState = EStateAllocChunkCallBackComplete;

	TInt freeRam = 0;

	TInt halError = KErrNone;
	if ((halError = HAL::Get(HALData::EMemoryRAMFree, freeRam)) != KErrNone)
		{
		INFO_PRINTF1(_L("Unable to obtain the amount of free ram available "));
		StopTest(halError, EInconclusive);
		return;
		}

	if (freeRam > KMaxFreeRamForTest)
		{
		TInt memToAllocate = freeRam - KMaxFreeRamForTest;
		INFO_PRINTF3(_L("freeRam  = %d bytes, before allocating %d bytes"),
					 freeRam, memToAllocate);
		TInt chunkError = KErrNone;
		//We have calculated the amount of memory to allocate
		//start by allocating half of that amount
		
		if((chunkError=iRChunk[0].CreateLocal(memToAllocate/2, memToAllocate/2 + 1))!=KErrNone)
			{
			INFO_PRINTF3(_L("At least %d mem bytes are needed for this test (err=%d)"),
						 KMaxFreeRamForTest, chunkError);
			StopTest(chunkError, EInconclusive);
			return;
			}
		//WDP: we are allocating on paged memory. So keep allocating chunks till you are Out of Memory
		//Now allocate smaller chunks. Only half of memToAllocate has been allocated yet
		//In every iteration we try to allocate KMaxFreeRamForTest/2
		//Calculate the max loop value till which we should run, add some extra iterations so that this loop goes on till OOM for sure
		//Number of iterations is (0.5*memToAllocate)/(0.5*KMaxFreeRamForTest)
		TInt loopValue=(memToAllocate/KMaxFreeRamForTest)+KExtraIterations;
		if(loopValue>KMaxChunks)
			{
			//We should not get here.
			INFO_PRINTF1(_L("Cannot Create so many chunks"));		
			StopTest(chunkError, EInconclusive);
			return;
			}
		for(iNumAllocatedChunks=1;iNumAllocatedChunks<KMaxChunks;iNumAllocatedChunks++)
			{
			chunkError = KErrNone;
			if ((chunkError = iRChunk[iNumAllocatedChunks].CreateLocal(KMaxFreeRamForTest/2, KMaxFreeRamForTest/2 + 1)) != KErrNone)
				{
				if(chunkError==KErrNoMemory)
					{
					//We have run out of memory
					//Close the last two chunks so now you have approximately KMaxFreeRamForTest free
					iRChunk[--iNumAllocatedChunks].Close();
					if(iNumAllocatedChunks>0)
						{
						iRChunk[--iNumAllocatedChunks].Close();
						}
					break;
					}
				else
					{
					//Unexpected error
					INFO_PRINTF1(_L("Received Unexpected error"));		
					StopTest(chunkError, EInconclusive);
					return;
					}
				}
			}
		if(chunkError!=KErrNoMemory)
			{
			//We are out of the loop and still not out of memory. This is not expected
			INFO_PRINTF2(_L("Expecting -4 by now but still not out of memory. Allocated chunks %d"),iNumAllocatedChunks);		
			StopTest(chunkError, EInconclusive);
			return;
			}
		
		iKickoff02->Call(); // Start prime
		}
	else
		{
		INFO_PRINTF2(_L("At least %d mem bytes are needed for this test "), KMaxFreeRamForTest);
		StopTest(KErrGeneral, EInconclusive);
		}
	}


TInt RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::PrimeControllerCallBack(TAny* aPtr)
	{
	RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap* self =
		static_cast<RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap*> (aPtr);
	self->DoPrimeControllerCallBack();
	return KErrNone;
	}

void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::DoPrimeControllerCallBack()
	{

	// Check simple fsm
	switch (iTestState)
		{
	case EStateAllocChunkCallBackComplete:
		{
		iTestState = EStatePrimeControllerCallBackComplete1;
		iKickoff03->Call(); // Start 1st playback
		break;
		}
	case EStateReturnChunckAndStopControllerCallBackComplete:
		{
		iTestState = EStatePrimeControllerCallBackComplete2;
		iKickoff06->Call(); // Start 2nd playback
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("FSM error in DoPrimeControllerCallBack"));
		StopTest(KErrGeneral, EFail);
		return;
		}
		};

	INFO_PRINTF1(_L("iController.Prime()"));

	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		StopTest(errorCode, EFail);
		}
	}


TInt RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::PlayControllerCallBack(TAny* aPtr)
	{
	RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap* self =
		static_cast<RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap*> (aPtr);
	self->DoPlayControllerCallBack();
	return KErrNone;
	}

void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::DoPlayControllerCallBack()
	{

	// Check simple fsm
	switch (iTestState)
		{
	case EStatePrimeControllerCallBackComplete1:
		{
		iTestState = EStatePlayControllerCallBackComplete1;
		break;
		}
	case EStatePrimeControllerCallBackComplete2:
		{
		iTestState = EStatePlayControllerCallBackComplete2;
		break;
		}
	default:
		{
		INFO_PRINTF1(_L("FSM error in DoPlayControllerCallBack"));
		StopTest(KErrGeneral, EFail);
		return;
		}
		};

	INFO_PRINTF1(_L("iController.Play()"));

	TInt errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() failed with error %d"), errorCode);
		StopTest(errorCode, EFail);
		}
	}

TInt RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::ReturnChunckAndStopControllerCallBack(TAny* aPtr)
	{
	RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap* self =
		static_cast<RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap*> (aPtr);
	self->DoReturnChunckAndStopControllerCallBack();
	return KErrNone;
	}

void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::DoReturnChunckAndStopControllerCallBack()
	{
	// Check simple fsm
	if (iTestState != EStatePlayControllerCallBackComplete1)
		{
		INFO_PRINTF1(_L("FSM error in DoReturnChunckAndStopControllerCallBack"));
		StopTest(KErrGeneral, EFail);
		return;
		}
	iTestState = EStateReturnChunckAndStopControllerCallBackComplete;

	INFO_PRINTF1(_L("Returning memory back to the system"));

	for(TInt i=0;i<iNumAllocatedChunks;i++)
		{
		iRChunk[i].Close();
		}
	// Get controller back to an initial state
	iController.Stop();
	
	iKickoff05->Call(); // call prime again
	}


/**
This is from MMMFControllerEventMonitorObserver.which handles all the events
from AviPlayController.  Base class version is overridden to avoid starting the
active schedular twice.
*/
void RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap::HandleEvent(const TMMFEvent& aEvent)
	{
	INFO_PRINTF2(_L("HandleEvent  aEvent.iEventType %d "),
				 aEvent.iEventType);
	INFO_PRINTF2(_L("::HandleEvent  aEvent.iErrorCode %d "),
				 aEvent.iErrorCode);

	if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			iTestStepResult = EPass;
			INFO_PRINTF2(_L("PlayCompletion is passed with  %d "),
						 aEvent.iErrorCode);
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("PlayCompletion is failed with  %d "),
						 aEvent.iErrorCode);
			}

		//
		// Match fsm state with play completion event just processed
		//
		switch(iTestState)
			{
		case EStatePlayControllerCallBackComplete1:
			{
			if (iTestStepResult == EPass)
				{
				INFO_PRINTF1(_L("Error: completion event was expected to be failed at this point"));
				StopTest(KErrGeneral, EFail);
				return;
				}
			
			iKickoff04->Call(); // Return memory
			break;
			}
		case EStatePlayControllerCallBackComplete2:
			{
			if (iTestStepResult == EFail)
				{
				INFO_PRINTF1(_L("Error: PlayCompletion event was expected to pass at this point"));
				StopTest(KErrGeneral, EFail);
				return;
				}
			else
				{
				// Test has finished succesfully at this point
				INFO_PRINTF1(_L("Test successfully completed"));
				StopTest();
				return;
				}
			}
		default:
			{
			INFO_PRINTF1(_L("FSM error in HandleEvent"));
			StopTest(KErrGeneral, EFail);
			return;
			}
			};
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if(aEvent.iErrorCode != KErrNone)
			{
			INFO_PRINTF2(_L("Video open complete is failed with  %d "), aEvent.iErrorCode);
			StopTest(KErrGeneral, EFail);
			return;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Unexpected Event from the Controller"));
		StopTest(KErrGeneral, EFail);
		}
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlMemAlloc1::RTestStepAviPlayCtrlMemAlloc1(const TDesC& aTestName,
															const TDesC& aSectName,
															const TDesC& aKeyName,
															TUid aControllerUid)
	: RTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This test issue a custom command to the custom AviPlay controller
and runs the OOM test within the controller.
*/
TVerdict RTestStepAviPlayCtrlMemAlloc1::DoTestStepL()
	{
	TVerdict result = EPass;
	
	Log( _L("Memory Allocation Test 1") );
	
	Log( _L("This test checks memory allocation of the plugin") );
	
   	TInt errorCode = iController.Open(KUidCustomTestAviPlayPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ send the custom command to the custom audio controller ]
	TMMFMessageDestinationPckg destinationPckg(KMemoryAllocationTestId);
	
	//[ send memory alloc test to the custom plugin ]
	//[ we have only ensured that the controller is loaded into memory
	// and that all add sources etc will be don
	errorCode = iController.CustomCommandSync(destinationPckg, 
		KMemoryAllocTest1, 
		KNullDesC8,
		KNullDesC8);
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is a bug in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	User::LeaveIfError( errorCode );
	return result ;
	}

/**
This is from MMMFControllerEventMonitorObserver.which handles all the events
from AviPlayController.  Base class version is overridden to avoid starting the
active schedular twice.
*/
void RTestStepAviPlayCtrlMemAlloc1::HandleEvent(const TMMFEvent& aEvent)
	{
	(void)aEvent;
	};

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlMemAlloc2::RTestStepAviPlayCtrlMemAlloc2(const TDesC& aTestName,
															const TDesC& aSectName,
															const TDesC& aKeyName,
															TUid aControllerUid)
	: RTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This test issue a custom command to the custom AviPlay controller
and runs the OOM test within the controller.
*/
TVerdict RTestStepAviPlayCtrlMemAlloc2::DoTestStepL()
	{
	TVerdict result = EPass;
	
	Log( _L("Memory Allocation Test 2") );
	
	Log( _L("This test checks memory allocation of the plugin") );
	
   	TInt errorCode = iController.Open(KUidCustomTestAviPlayPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ send the custom command to the custom audio controller ]
	TMMFMessageDestinationPckg destinationPckg(KMemoryAllocationTestId);
	
	//[ set the prime OOM test mode ]
	//[ we have only ensured that the controller is loaded into memory
	// and that all add sources etc will be don
	TPckgBuf<TBool> primeTestEnable(ETrue);
	errorCode = iController.CustomCommandSync(destinationPckg, 
		KOOMPrimeTestEnable, 
		primeTestEnable,
		KNullDesC8);
	if (errorCode == KErrNone)
		{
		TInt failCount = 1;
		for (;;)
			{
			TPckgBuf<TInt> primeTestFailCount(failCount);
			errorCode = iController.CustomCommandSync(destinationPckg, 
				KOOMPrimeTestFailCount, 
				primeTestFailCount,
				KNullDesC8);
			if (errorCode != KErrNone)
				{
				result = EInconclusive;
				break;
				}
			
			errorCode = iController.CustomCommandSync(destinationPckg, 
				KMemoryAllocTest2, 
				KNullDesC8,
				KNullDesC8);
			if (errorCode != KErrNone)
				{
				result = EInconclusive;
				break;
				}

			errorCode = iController.Prime();
			if (errorCode == KErrNone)
				{
				result = EPass;
				break;
				}
			else if (errorCode != KErrNoMemory)
				{
				result = EFail;
				break;
				}
			
			iController.Reset();
			failCount++;
			}
		}
	else
		{
		result = EInconclusive;
		}
	
	primeTestEnable() = EFalse;
	iController.CustomCommandSync(destinationPckg, 
		KOOMPrimeTestEnable, 
		primeTestEnable,
		KNullDesC8);
		
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is a bug in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result ;
	}

/**
This is from MMMFControllerEventMonitorObserver.which handles all the events
from AviPlayController.  Base class version is overridden to avoid starting the
active schedular twice.
*/
void RTestStepAviPlayCtrlMemAlloc2::HandleEvent(const TMMFEvent& aEvent)
	{
	(void)aEvent;
	};

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviPlayCtrlMemAlloc3::RTestStepAviPlayCtrlMemAlloc3(const TDesC& aTestName,
															const TDesC& aSectName,
															const TDesC& aKeyName,
															TUid aControllerUid)
	: RTestStepAviCtrlBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This test issue a custom command to the custom AviPlay controller
and runs the OOM test within the controller.
*/
TVerdict RTestStepAviPlayCtrlMemAlloc3::DoTestStepL()
	{
	TVerdict result = EPass;
	
	Log( _L("Memory Allocation Test 3") );
	
	Log( _L("This test checks memory allocation of the plugin") );
	
   	TInt errorCode = iController.Open(KUidCustomTestAviPlayPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ send the custom command to the custom audio controller ]
	TMMFMessageDestinationPckg destinationPckg(KMemoryAllocationTestId);
	
	TInt failCount = 1;
	for (;;)
		{
		TPckgBuf<TInt> primeTestFailCount(failCount);
		errorCode = iController.CustomCommandSync(destinationPckg, 
			KOOMPrimeTestFailCount, 
			primeTestFailCount,
			KNullDesC8);
		if (errorCode != KErrNone)
			{
			result = EInconclusive;
			break;
			}
		
		errorCode = iController.CustomCommandSync(destinationPckg, 
			KMemoryAllocTest2, 
			KNullDesC8,
			KNullDesC8);
		if (errorCode != KErrNone)
			{
			result = EInconclusive;
			break;
			}

		errorCode = iController.Prime();
		if (errorCode != KErrNone)
			{
			result = EInconclusive;
			break;
			}

		errorCode = iController.CustomCommandSync(destinationPckg, 
			KMemoryAllocTest3, 
			KNullDesC8,
			KNullDesC8);
		
		if (errorCode == KErrNone)
			{
			result = EPass;
			break;
			}
		else if (errorCode != KErrNoMemory)
			{
			result = EFail;
			break;
			}
		
		iController.Reset();
		failCount++;
		}
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is a bug in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result ;
	}

/**
This is from MMMFControllerEventMonitorObserver.which handles all the events
from AviPlayController.  Base class version is overridden to avoid starting the
active schedular twice.
*/
void RTestStepAviPlayCtrlMemAlloc3::HandleEvent(const TMMFEvent& aEvent)
	{
	(void)aEvent;
	};

#ifdef SYMBIAN_BUILD_GCE

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviPlayCtrlSurfaceSupport::RTestStepAviPlayCtrlSurfaceSupport(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), iVpsCommands(iController)
	{
	}

void RTestStepAviPlayCtrlSurfaceSupport::KickoffTestL()
	{
	// Allow parent to create some call backs for its initialization.
	// Should open the video controller, set the filename, add the source and add the sink.
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	
	iSurfaceCreated = EFalse;
	
	// Call UseSurfaces
	TCallBack callbackUseSurfaces(UseSurfacesCallBack, this);
	delete iKickoffUseSurfaces;
	iKickoffUseSurfaces = NULL;
	iKickoffUseSurfaces = new (ELeave) CAsyncCallBack(callbackUseSurfaces, CActive::EPriorityLow);
	iKickoffUseSurfaces->Call();
	
	// Call UseSurfaces again.  This should not fail or cause any other subsequent step to fail.
	delete iKickoffUseSurfaces2;
	iKickoffUseSurfaces2 = NULL;
	iKickoffUseSurfaces2 = new (ELeave) CAsyncCallBack(callbackUseSurfaces, CActive::EPriorityLow);
	iKickoffUseSurfaces2->Call();

	// Prime the controller
	TCallBack callbackPrimeController(PrimeControllerCallBack, this);
	delete iKickoffPrimeController;
	iKickoffPrimeController = NULL;
	iKickoffPrimeController = new (ELeave) CAsyncCallBack(callbackPrimeController, CActive::EPriorityLow);
	iKickoffPrimeController->Call();

	TCallBack callbackPlayController(PlayControllerCallBack, this);
	delete iKickoffPlayController;
	iKickoffPlayController = NULL;
	iKickoffPlayController = new (ELeave) CAsyncCallBack(callbackPlayController, CActive::EPriorityLow);
	iKickoffPlayController->Call();
	}

void RTestStepAviPlayCtrlSurfaceSupport::CloseTest()
	{
	// no need to Cancel before deleting.
	delete iKickoffUseSurfaces;
	iKickoffUseSurfaces = NULL;
	delete iKickoffUseSurfaces2;
	iKickoffUseSurfaces2 = NULL;
	delete iKickoffPrimeController;
	iKickoffPrimeController = NULL;
	
	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}

/**
Check if we have passed the test.  We need to make sure that we have received all the event
messages that we expect.
*/	
TVerdict RTestStepAviPlayCtrlSurfaceSupport::CheckTestResult()
	{
	// Only report that we have not received all expected events if everything else passes.
	// We don't expect the event generators to work properly if something else failed (and those
	// other error codes and reasons should be more useful).  We won't see these events if GCE is
	// not enabled.  So, if GCE is disabled, do not fail because we didn't see these events.
	if (Result() == EPass && !iSurfaceCreated && iGceEnabled)
		{
		if (!iSurfaceCreated)
			{
			ERR_PRINTF1(_L("Failed to receive surface created event"));
			}
			
		return EFail;
		}
		
	return RAsyncTestStepAviCtrlPlayBase::CheckTestResult();
	}

/**
This is from MMMFControllerEventMonitorObserver.which handles all the events
from AviPlayController.  Base class version is overridden to avoid starting the
active schedular twice.
*/
void RTestStepAviPlayCtrlSurfaceSupport::HandleEvent(const TMMFEvent& aEvent)
	{	
	if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
		{
		if(aEvent.iErrorCode != KErrNone)
			{
			ERR_PRINTF2(_L("Video open complete has failed with %d "), aEvent.iErrorCode);
			StopTest(KErrGeneral, EInconclusive);
			return;
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryVideoSurfaceCreated)
		{	
		if (!iGceEnabled)
			{
			ERR_PRINTF1(_L("Surface events should not be generated when GCE is not enabled"));
			StopTest(KErrGeneral, EFail);
			return;
			}
			
		// The error code is actually the display for which a surface has been created.
		if (aEvent.iErrorCode != KErrNone)
			{
			ERR_PRINTF2(_L("Video surface created event has error %d."), aEvent.iErrorCode);
			StopTest(KErrGeneral, EFail);
			}
		
		// Now that the surface has been created we can check if can retrieve the surface parameters
		TRect cropRect;
		TVideoAspectRatio pixelAspectRatio;
	
		TInt error = iVpsCommands.GetSurfaceParameters(iSurfaceId, cropRect, pixelAspectRatio);
		if (error)
			{
			ERR_PRINTF2(_L("GetSurfaceParameters failed with code %d."), error);
			StopTest(error, EFail);
			}
		
		iSurfaceCreated = ETrue;
		}
	else if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrNone)
			{
			if(iGceEnabled)
				{
				TInt error = iVpsCommands.SurfaceRemoved(iSurfaceId);
				if (error)
					{
					ERR_PRINTF2(_L("SurfaceRemoved failed with code %d."), error);
					StopTest(error, EFail);
					}
				}
			StopTest();
			}
		else
			{
			ERR_PRINTF2(_L("Video playback complete has failed with %d "), aEvent.iErrorCode);
			StopTest(KErrGeneral, EInconclusive);
			}
		}
	else
		{
		ERR_PRINTF3(_L("Unexpected Event from the Controller: type %d error: %d"), aEvent.iEventType, aEvent.iErrorCode);
		StopTest(KErrGeneral, EFail);
		}
	}

TInt RTestStepAviPlayCtrlSurfaceSupport::UseSurfacesCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceSupport* self =
		static_cast<RTestStepAviPlayCtrlSurfaceSupport*> (aPtr);
	self->DoUseSurfacesCallBack();
	return KErrNone;
	}
	
void RTestStepAviPlayCtrlSurfaceSupport::DoUseSurfacesCallBack()
	{
	TInt error = iVpsCommands.UseSurfaces();
	
	if (iGceEnabled && error != KErrNone)
		{
		ERR_PRINTF2(_L("UseSurfaces failed with code %d."), error);
		StopTest(error, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("UseSurfaces failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	}

TInt RTestStepAviPlayCtrlSurfaceSupport::PrimeControllerCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceSupport* self =
		static_cast<RTestStepAviPlayCtrlSurfaceSupport*> (aPtr);
	self->DoPrimeControllerCallBack();
	return KErrNone;
	}

void RTestStepAviPlayCtrlSurfaceSupport::DoPrimeControllerCallBack()
	{
	TInt error = iController.Prime();
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Prime failed with  %d "), error);
		StopTest(error, EFail);
		}
	}
	
TInt RTestStepAviPlayCtrlSurfaceSupport::PlayControllerCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceSupport* self =
		static_cast<RTestStepAviPlayCtrlSurfaceSupport*> (aPtr);
	self->DoPlayControllerCallBack();
	return KErrNone;
	}

void RTestStepAviPlayCtrlSurfaceSupport::DoPlayControllerCallBack()
	{
	TInt error = iController.Play();
	if (error != KErrNone)
		{
		ERR_PRINTF2(_L("Play failed with  %d "), error);
		StopTest(error, EFail);
		}
	}

RTestStepAviPlayCtrlSurfaceRemovedNoUS::RTestStepAviPlayCtrlSurfaceRemovedNoUS(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), iVpsCommands(iController)
	{
	}

void RTestStepAviPlayCtrlSurfaceRemovedNoUS::KickoffTestL()
	{
	// Allow parent to create some call backs for its initialization.
	// Should open the video controller, set the filename, add the source and add the sink.
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	
	// Call SurfaceRemoved
	iSurfaceId = TSurfaceId::CreateNullId();

	TCallBack callbackSurfaceRemoved(SurfaceRemovedCallBack, this);
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	iKickoffSurfaceRemoved = new (ELeave) CAsyncCallBack(callbackSurfaceRemoved, CActive::EPriorityLow);
	iKickoffSurfaceRemoved->Call();
	}

void RTestStepAviPlayCtrlSurfaceRemovedNoUS::CloseTest()
	{
	// no need to Cancel before deleting.
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	
	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}

TInt RTestStepAviPlayCtrlSurfaceRemovedNoUS::SurfaceRemovedCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceRemovedNoUS* self =
		static_cast<RTestStepAviPlayCtrlSurfaceRemovedNoUS*> (aPtr);
	self->DoSurfaceRemovedCallBack();
	return KErrNone;
	}

void RTestStepAviPlayCtrlSurfaceRemovedNoUS::DoSurfaceRemovedCallBack()
	{
	TInt error = iVpsCommands.SurfaceRemoved(iSurfaceId);
	
	if (iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return error KErrNotSupported.  Actual error was %d "), error);
		StopTest(KErrGeneral, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	else
		{
		StopTest();
		}
	}

RTestStepAviPlayCtrlGetSurfaceParametersNoUS::RTestStepAviPlayCtrlGetSurfaceParametersNoUS(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), iVpsCommands(iController)
	{
	}

void RTestStepAviPlayCtrlGetSurfaceParametersNoUS::KickoffTestL()
	{
	// Allow parent to create some call backs for its initialization.
	// Should open the video controller, set the filename, add the source and add the sink.
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	
	// Get the surface parameters.
	TCallBack callbackGetSurfaceParameters(GetSurfaceParametersCallBack, this);
	delete iKickoffGetSurfaceParameters;
	iKickoffGetSurfaceParameters = NULL;
	iKickoffGetSurfaceParameters = new (ELeave) CAsyncCallBack(callbackGetSurfaceParameters, CActive::EPriorityLow);
	iKickoffGetSurfaceParameters->Call();
	}

void RTestStepAviPlayCtrlGetSurfaceParametersNoUS::CloseTest()
	{
	// no need to Cancel before deleting.
	delete iKickoffGetSurfaceParameters;
	iKickoffGetSurfaceParameters = NULL;
	
	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}

TInt RTestStepAviPlayCtrlGetSurfaceParametersNoUS::GetSurfaceParametersCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlGetSurfaceParametersNoUS* self =
		static_cast<RTestStepAviPlayCtrlGetSurfaceParametersNoUS*> (aPtr);
	self->DoGetSurfaceParametersCallBack();
	return KErrNone;
	}
	
void RTestStepAviPlayCtrlGetSurfaceParametersNoUS::DoGetSurfaceParametersCallBack()
	{
	TSurfaceId surfaceID;	
	TRect cropRect;
	TVideoAspectRatio pixelAspectRatio;

	TInt error = iVpsCommands.GetSurfaceParameters(surfaceID, cropRect, pixelAspectRatio);
	
	if (iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("GetSurfaceParameters failed to return error KErrNotSupported.  Actual error was %d "), error);
		StopTest(KErrGeneral, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("GetSurfaceParameters failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	else
		{
		StopTest();
		}
	}

RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), iVpsCommands(iController)
	{
	}

void RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::KickoffTestL()
	{
	// Allow parent to create some call backs for its initialization.
	// Should open the video controller, set the filename, add the source and add the sink.
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	
	// Call UseSurfaces
	TCallBack callbackUseSurfaces(UseSurfacesCallBack, this);
	delete iKickoffUseSurfaces;
	iKickoffUseSurfaces = NULL;
	iKickoffUseSurfaces = new (ELeave) CAsyncCallBack(callbackUseSurfaces, CActive::EPriorityLow);
	iKickoffUseSurfaces->Call();
	
	// Call SurfaceRemoved
	iSurfaceId = TSurfaceId::CreateNullId();

	TCallBack callbackSurfaceRemoved(SurfaceRemovedCallBack, this);
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	iKickoffSurfaceRemoved = new (ELeave) CAsyncCallBack(callbackSurfaceRemoved, CActive::EPriorityLow);
	iKickoffSurfaceRemoved->Call();
	}

void RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::CloseTest()
	{
	// no need to Cancel before deleting.
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	
	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}

TInt RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::SurfaceRemovedCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId* self =
		static_cast<RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId*> (aPtr);
	self->DoSurfaceRemovedCallBack();
	return KErrNone;
	}

void RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::DoSurfaceRemovedCallBack()
	{
	TInt error = iVpsCommands.SurfaceRemoved(iSurfaceId);
	
	if (iGceEnabled && error != KErrArgument)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return error KErrArgument.  Actual error was %d "), error);
		StopTest(KErrGeneral, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	else
		{
		StopTest();
		}
	}

TInt RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::UseSurfacesCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId* self =
		static_cast<RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId*> (aPtr);
	self->DoUseSurfacesCallBack();
	return KErrNone;
	}
	
void RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId::DoUseSurfacesCallBack()
	{
	TInt error = iVpsCommands.UseSurfaces();
	
	if (iGceEnabled && error != KErrNone)
		{
		ERR_PRINTF2(_L("UseSurfaces failed with code %d."), error);
		StopTest(error, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("UseSurfaces failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	}

RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RAsyncTestStepAviCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid), iVpsCommands(iController)
	{
	}

void RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::KickoffTestL()
	{
	// Allow parent to create some call backs for its initialization.
	// Should open the video controller, set the filename, add the source and add the sink.
	RAsyncTestStepAviCtrlPlayBase::KickoffTestL();
	
	// Call UseSurfaces
	TCallBack callbackUseSurfaces(UseSurfacesCallBack, this);
	delete iKickoffUseSurfaces;
	iKickoffUseSurfaces = NULL;
	iKickoffUseSurfaces = new (ELeave) CAsyncCallBack(callbackUseSurfaces, CActive::EPriorityLow);
	iKickoffUseSurfaces->Call();
	
	// Call SurfaceRemoved
	iSurfaceId.iInternal[0] = 1111;
	iSurfaceId.iInternal[1] = 1111;
	iSurfaceId.iInternal[2] = 1111;
	iSurfaceId.iInternal[3] = 1111;
	
	TCallBack callbackSurfaceRemoved(SurfaceRemovedCallBack, this);
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	iKickoffSurfaceRemoved = new (ELeave) CAsyncCallBack(callbackSurfaceRemoved, CActive::EPriorityLow);
	iKickoffSurfaceRemoved->Call();
	}

void RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::CloseTest()
	{
	// no need to Cancel before deleting.
	delete iKickoffSurfaceRemoved;
	iKickoffSurfaceRemoved = NULL;
	
	RAsyncTestStepAviCtrlPlayBase::CloseTest();
	}

TInt RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::SurfaceRemovedCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId* self =
		static_cast<RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId*> (aPtr);
	self->DoSurfaceRemovedCallBack();
	return KErrNone;
	}

void RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::DoSurfaceRemovedCallBack()
	{
	TInt error = iVpsCommands.SurfaceRemoved(iSurfaceId);
	
	if (iGceEnabled && error != KErrNotReady)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return error KErrNotReady.  Actual error was %d "), error);
		StopTest(KErrGeneral, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("SurfaceRemoved failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	else
		{
		StopTest();
		}
	}

TInt RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::UseSurfacesCallBack(TAny* aPtr)
	{
	RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId* self =
		static_cast<RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId*> (aPtr);
	self->DoUseSurfacesCallBack();
	return KErrNone;
	}
	
void RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId::DoUseSurfacesCallBack()
	{
	TInt error = iVpsCommands.UseSurfaces();
	
	if (iGceEnabled && error != KErrNone)
		{
		ERR_PRINTF2(_L("UseSurfaces failed with code %d."), error);
		StopTest(error, EFail);
		}
	else if (!iGceEnabled && error != KErrNotSupported)
		{
		ERR_PRINTF2(_L("UseSurfaces failed to return KErrNotSupported. Actual result: %d."), error);
		StopTest(error, EFail);
		}
	}

#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

//
// RAsyncTestSubtitlePlay
//
RAsyncTestSubtitlePlay::RAsyncTestSubtitlePlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(1);
	}

void RAsyncTestSubtitlePlay::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	RArray<TLanguage> languages;
	CleanupClosePushL(languages);
	TRAPD(err, iSubtitleCommands.GetSupportedSubtitleLanguagesL(languages));
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL left with %d; expected %d"), err, KErrNotSupported);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	if (languages.Count() != 0)
		{
		ERR_PRINTF2(_L("GetSupportedSubtitleLanguagesL returned %d languages"), languages.Count());
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(); // languages
	
	TLanguage language = ELangNone;
	err = iSubtitleCommands.GetSubtitleLanguage(language);
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("GetSubtitleLanguage failed with %d; expected %d"), err, KErrNotSupported);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	if (language != ELangNone)
		{
		ERR_PRINTF3(_L("GetSubtitleLanguage returned %d, expected %d"), language, ELangNone);
		User::Leave(KErrGeneral);
		}
	
	err = iSubtitleCommands.SetSubtitleLanguage(ELangCanadianEnglish);
	
	if (err != KErrNotSupported)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguage failed with %d, expected %d"), err, KErrNotSupported);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	StartPlaybackL();
	}

void RAsyncTestSubtitlePlay::OnPlayCompleteL()
	{
	RemoveConfigDisableL();
	}

//
// RAsyncTestSubtitleUpdateConfig
//
RAsyncTestSubtitleUpdateConfig::RAsyncTestSubtitleUpdateConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(2);
	}

void RAsyncTestSubtitleUpdateConfig::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationClockwise90;
	config.iWindowClipRect = KWindowClipRectAlt1;
	TInt err = iSubtitleCommands.UpdateSubtitleConfig(config);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("UpdateSubtitleConfig failed with %d"), err);
		User::Leave(err);
		}
	
	StartPlaybackL();
	}

void RAsyncTestSubtitleUpdateConfig::OnPlayCompleteL()
	{
	RemoveConfigDisableL();
	}

//
// RAsyncTestSubtitleUpdateDuring
//

CCallBackTimer* CCallBackTimer::NewL(const TCallBack& aCallBack, TInt aPriority)
	{
	CCallBackTimer* self = new (ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CCallBackTimer::CCallBackTimer(const TCallBack& aCallBack, TInt aPriority)
	: CTimer(aPriority), iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

void CCallBackTimer::RunL()
	{
	User::LeaveIfError(iCallBack.CallBack());
	}

RAsyncTestSubtitleUpdateDuring::RAsyncTestSubtitleUpdateDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(2);
	}

void RAsyncTestSubtitleUpdateDuring::KickoffTestL()
	{
	TCallBack callBack(DuringPlaybackCallBack, this);
	iCallBackTimer = CCallBackTimer::NewL(callBack, CTimer::EPriorityStandard);
	iHasUpdated = EFalse;
	RAsyncTestStepSubtitleBase::KickoffTestL();
	}

void RAsyncTestSubtitleUpdateDuring::CloseTest()
	{
	delete iCallBackTimer;
	iCallBackTimer = NULL;
	RAsyncTestStepSubtitleBase::CloseTest();
	}

TInt RAsyncTestSubtitleUpdateDuring::DuringPlaybackCallBack(TAny* aPtr)
	{
	RAsyncTestSubtitleUpdateDuring* self = static_cast<RAsyncTestSubtitleUpdateDuring*>(aPtr);
	self->OnDuringPlayback();
	return KErrNone;
	}

void RAsyncTestSubtitleUpdateDuring::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	// Start the call back timer.  Will trigger the during playback handler.
	iCallBackTimer->After(2500000);
	
	StartPlaybackL();
	}

void RAsyncTestSubtitleUpdateDuring::OnDuringPlayback()
	{
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationClockwise90;
	config.iWindowClipRect = KWindowClipRectAlt1;
	TInt err = iSubtitleCommands.UpdateSubtitleConfig(config);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("UpdateSubtitleConfig during playback failed with %d"), err);
		StopTest(err, EFail);
		}
	else
		{
		iHasUpdated = ETrue;
		}
	}

void RAsyncTestSubtitleUpdateDuring::OnPlayCompleteL()
	{
	RemoveConfigDisableL();
	
	if (!iHasUpdated)
		{
		ERR_PRINTF1(_L("Subtitle configuration was not updated during playback."));
		User::Leave(KErrGeneral);
		}
	}

//
// RAsyncTestSubtitleRemoveAdd
//
RAsyncTestSubtitleRemoveAdd::RAsyncTestSubtitleRemoveAdd(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(2);
	}

void RAsyncTestSubtitleRemoveAdd::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	TInt err = iSubtitleCommands.RemoveSubtitleConfig(KWindowIdValid);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RemoveSubtitleConfig failed with %d"), err);
		User::Leave(err);
		}
	
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationClockwise270;
	config.iWindowClipRect = KWindowClipRectAlt2;
	err = iSubtitleCommands.AddSubtitleConfig(config);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddSubtitleConfig failed with %d"), err);
		User::Leave(err);
		}
	
	StartPlaybackL();
	}

void RAsyncTestSubtitleRemoveAdd::OnPlayCompleteL()
	{
	RemoveConfigDisableL();
	}

//
// RAsyncTestSubtitleNotEnabled
//
RAsyncTestSubtitleNotEnabled::RAsyncTestSubtitleNotEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(0);
	}

void RAsyncTestSubtitleNotEnabled::OnOpenCompleteL()
	{
	// Make sure that subtitles are available:	
	TBool available = EFalse;
	TInt err = iSubtitleCommands.GetSubtitlesAvailable(available);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSubtitlesAvailable failed with %d"), err);
		User::Leave(err);
		}
	
	if (!available)
		{
		ERR_PRINTF1(_L("Subtitles unavailable"));
		User::Leave(KErrGeneral);
		}
	
	// Perform severy subtitle related functions without enabling subtitles.
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = KWindowClipRectFull;
	
	err = iSubtitleCommands.AddSubtitleConfig(config);
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("AddSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.UpdateSubtitleConfig(config);
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("UpdateSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	RArray<TLanguage> languages;
	TRAP(err, iSubtitleCommands.GetSupportedSubtitleLanguagesL(languages));
	languages.Close();
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL left with %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	TLanguage language = ELangNone;
	err = iSubtitleCommands.GetSubtitleLanguage(language);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetSubtitleLanguage returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.SetSubtitleLanguage(ELangCanadianEnglish);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguage returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	StartPlaybackL();
	}

void RAsyncTestSubtitleNotEnabled::OnPlayCompleteL()
	{
	TInt err = iSubtitleCommands.RemoveSubtitleConfig(KWindowIdValid);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("RemoveSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.DisableSubtitles();
	
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("DisableSubtitles returned %d; expected %d"), err, KErrNone);
		User::Leave(err);
		}
	}

//
// RAsyncTestSubtitleNotAvailable
//
RAsyncTestSubtitleNotAvailable::RAsyncTestSubtitleNotAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(0);
	}

void RAsyncTestSubtitleNotAvailable::OnOpenCompleteL()
	{
	// Make sure that subtitles are available:	
	TBool available = EFalse;
	TInt err = iSubtitleCommands.GetSubtitlesAvailable(available);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSubtitlesAvailable failed with %d"), err);
		User::Leave(err);
		}
	
	if (available)
		{
		ERR_PRINTF1(_L("Subtitles available, but should not be"));
		User::Leave(KErrGeneral);
		}
	
	err = iSubtitleCommands.EnableSubtitles();
	
	if (err != KErrNotFound)
		{
		ERR_PRINTF3(_L("EnableSubtitles returned %d; expected %d"), err, KErrNotFound);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// Perform severy subtitle related functions without enabling subtitles.
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationNone;
	config.iWindowClipRect = KWindowClipRectFull;
	
	err = iSubtitleCommands.AddSubtitleConfig(config);
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("AddSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.UpdateSubtitleConfig(config);
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("UpdateSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	RArray<TLanguage> languages;
	TRAP(err, iSubtitleCommands.GetSupportedSubtitleLanguagesL(languages));
	languages.Close();
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetSupportedSubtitleLanguagesL left with %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	TLanguage language = ELangNone;
	err = iSubtitleCommands.GetSubtitleLanguage(language);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("GetSubtitleLanguage returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.SetSubtitleLanguage(ELangCanadianEnglish);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("SetSubtitleLanguage returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	StartPlaybackL();
	}

void RAsyncTestSubtitleNotAvailable::OnPlayCompleteL()
	{
	TInt err = iSubtitleCommands.RemoveSubtitleConfig(KWindowIdValid);
	
	if (err != KErrNotReady)
		{
		ERR_PRINTF3(_L("RemoveSubtitleConfig returned %d; expected %d"), err, KErrNotReady);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.DisableSubtitles();
	
	if (err != KErrNone)
		{
		ERR_PRINTF3(_L("DisableSubtitles returned %d; expected %d"), err, KErrNone);
		User::Leave(err);
		}
	}

//
// RAsyncTestSubtitleMismatchedId
//
RAsyncTestSubtitleMismatchedId::RAsyncTestSubtitleMismatchedId(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(1);
	}

void RAsyncTestSubtitleMismatchedId::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdMismatched;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationClockwise90;
	config.iWindowClipRect = KWindowClipRectAlt1;
	
	TInt err = iSubtitleCommands.UpdateSubtitleConfig(config);
	if (err != KErrNotFound)
		{
		ERR_PRINTF3(_L("UpdateSubtitleConfig returned %d; expected %d"), err, KErrNotFound);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	err = iSubtitleCommands.RemoveSubtitleConfig(KWindowIdMismatched);
	if (err != KErrNotFound)
		{
		ERR_PRINTF3(_L("RemoveSubtitleConfig returned %d; expected %d"), err, KErrNotFound);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	TWsGraphicId id(TWsGraphicId::EUninitialized);
	TRect rect;
	err = iSubtitleCommands.GetCrpParameters(KWindowIdMismatched, id, rect);
	if (err != KErrNotFound)
		{
		ERR_PRINTF3(_L("GetCrpParameters returned %d; expected %d"), err, KErrNotFound);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// If we haven't left yet then the test passes.
	StopTest();
	}

//
// RAsyncTestSubtitleDoubleCall
//
RAsyncTestSubtitleDoubleCall::RAsyncTestSubtitleDoubleCall(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(1);
	}

void RAsyncTestSubtitleDoubleCall::OnOpenCompleteL()
	{
	TBool available = EFalse;
	
	TInt err = iSubtitleCommands.GetSubtitlesAvailable(available);
	
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetSubtitlesAvailable failed with %d"), err);
		User::Leave(err);
		}
	
	if (!available)
		{
		ERR_PRINTF1(_L("Subtitles unavailable"));
		User::Leave(KErrGeneral);
		}
	
	// Disable subtitles when they are not enabled.
	err = iSubtitleCommands.DisableSubtitles();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DisableSubtitles failed with %d"), err);
		User::Leave(err);
		}
	
	err = iSubtitleCommands.EnableSubtitles();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitles failed with %d"), err);
		User::Leave(err);
		}
	
	// Enable subtitles when they are already enabled.
	err = iSubtitleCommands.EnableSubtitles();
	if (err != KErrInUse)
		{
		ERR_PRINTF3(_L("EnableSubtitles failed with %d; expecting %d when already enabled."), err, KErrInUse);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	AddDefaultSubtitleConfL();
	StartPlaybackL();
	}

void RAsyncTestSubtitleDoubleCall::OnPlayCompleteL()
	{
	RemoveConfigDisableL();
	
	// Disable subtitles when they are not enabled.
	TInt err = iSubtitleCommands.DisableSubtitles();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DisableSubtitles failed with %d"), err);
		User::Leave(err);
		}
	}

//
// RAsyncTestSubtitleMultipleConfigs
//
RAsyncTestSubtitleMultipleConfigs::RAsyncTestSubtitleMultipleConfigs(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RAsyncTestStepSubtitleBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	SetExpectedCrpReadyCount(1);
	}

void RAsyncTestSubtitleMultipleConfigs::OnOpenCompleteL()
	{
	CheckAndEnableSubtitlesL();
	AddDefaultSubtitleConfL();
	
	TMMFSubtitleWindowConfig config;
	config.iWindowId = KWindowIdValid;
	config.iDisplayMode = EColor16MA;
	config.iRotation = EVideoRotationClockwise90;
	config.iWindowClipRect = KWindowClipRectAlt1;
	
	// Add a config with the same window ID.
	TInt err = iSubtitleCommands.AddSubtitleConfig(config);
	if (err != KErrArgument)
		{
		ERR_PRINTF3(_L("AddSubtitleConfig returned %d; expected %d"), err, KErrArgument);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// Add a config with a different ID.
	config.iWindowId = KWindowIdValidAlt;
	err = iSubtitleCommands.AddSubtitleConfig(config);
	if (err != KErrInUse)
		{
		ERR_PRINTF3(_L("AddSubtitleConfig returned %d; expected %d"), err, KErrInUse);
		User::Leave(err == KErrNone ? KErrGeneral : err);
		}
	
	// If we haven't left yet then the test passes.
	StopTest();
	}

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
