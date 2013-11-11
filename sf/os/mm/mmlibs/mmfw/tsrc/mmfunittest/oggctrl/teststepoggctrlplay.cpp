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

#include "teststepoggctrlplay.h"

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPlay::RTestStepOggCtrlPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Primes the OggPlayController and Plays an Ogg file to its completion
 * and checks for the proper play completion
 */
TVerdict RTestStepOggCtrlPlay::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step plays an Ogg file "));
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
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlVolume::RTestStepOggCtrlVolume(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the volume on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the volume set is same as the volume got.
 */
TVerdict RTestStepOggCtrlVolume::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetVolume/GetVolume() APIs of OggPlayController "));
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );
	TInt maxVolume(0);
	TInt errorCode =theCommander.GetMaxVolume( maxVolume );
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetMaxVolume() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	
	TInt theVolume = maxVolume /2;
	//try setting volume in Stopped State
	errorCode =theCommander.SetVolume(theVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolume() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewVolume (0);
	errorCode =theCommander.GetVolume(theNewVolume);
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
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting volume in Primed State
	errorCode =theCommander.SetVolume(theVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolume() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetVolume(theNewVolume);
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
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting volume in Playing State
	errorCode =theCommander.SetVolume(theVolume);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVolume() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theCommander.GetVolume(theNewVolume);
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
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPlayBalance::RTestStepOggCtrlPlayBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the PlayBalance on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the PlayBalance set is same as the PlayBalance got.
 */
TVerdict RTestStepOggCtrlPlayBalance::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetBalance/GetBalance() APIs of OggPlayController "));
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	TInt errorCode = theCommander.SetBalance( theBalance );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewBalance (0);
	errorCode =theCommander.GetBalance( theNewBalance );
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVolume() during Stopped State is failed with  %d "), errorCode);
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
	errorCode = theCommander.SetBalance( theBalance );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetBalance( theNewBalance );
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
	errorCode =theCommander.SetBalance(theBalance);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Playing State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theCommander.GetBalance(theNewBalance);
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
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPosition::RTestStepOggCtrlPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the Position on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the Position set is same as the Position got. It expects KErrNotReady
 * error during Stopped state.
 */
TVerdict RTestStepOggCtrlPosition::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests SetPosition/GetPosition() APIs of OggPlayController "));
	TTimeIntervalMicroSeconds theDuration;
	TInt errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt64 duration = theDuration. Int64();
	TTimeIntervalMicroSeconds thePosition(duration/2);
	errorCode = iController.SetPosition(thePosition);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetPosition() during Stopped State is failed to return KErrNotReady but returned  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theNewPosition;
	errorCode = iController.GetPosition(theNewPosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	/* There is a limitation in the repositioning in the vorbis stream. The position passed by the 
	client is converted into granulePos that is nearer to the positon passed. So when GetPosition is called
	we may not get the exact position we have set using the setposition(). This is because granulePos 
	is same all through an oggPage and an oggPage will be of average size 6k. So the positioning is not that 
	precise. So the KDeviationInTime. 
	*/
	if(!TimeComparison(theNewPosition.Int64(), thePosition.Int64(), KDeviationInTime))
		{
		INFO_PRINTF1(_L("Set and Get Positions are different during Primed State"));
		return EFail;
		}
	//Reset back to the beginning before play
	iController.SetPosition(TTimeIntervalMicroSeconds(0));
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	User::After(KTwoSeconds);
	errorCode = iController.SetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.GetPosition(theNewPosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	if(!TimeComparison(theNewPosition.Int64(), thePosition.Int64(), KDeviationInTime))
		{
        //stop the controller
        INFO_PRINTF1(_L("Current Position is lessthan the Postion Set in Playing State"));
        iController.Stop();
        return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlDuration::RTestStepOggCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the Position on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the Position set is same as the Postion got. It expects KErrNotReady
 * error during Stopped state.
 */
TVerdict RTestStepOggCtrlDuration::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetDuration() API of OggPlayController "));
	TTimeIntervalMicroSeconds expectedtheDuration(6002426);
	TTimeIntervalMicroSeconds theDuration;
	TInt errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(theDuration != expectedtheDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), theDuration. Int64());
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(theDuration != expectedtheDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), theDuration. Int64());
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	else if(theDuration != expectedtheDuration)
		{
		INFO_PRINTF2(_L("GetDuration() returned the wrong duration %d "), theDuration. Int64());
		iController.Stop();
		return EFail;
		}
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPause::RTestStepOggCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests the Pause functionality of the OggPlayController in Stopped, Primed and Playing 
 * states and checks if the Play starts from the current position. It expects KErrNotReady
 * error if Pause is called during Stopped and Primed states.
 * 
 * This test may fail as the position after pause-play may vary due to granularity and recalculation of the buffer.
 */
TVerdict RTestStepOggCtrlPause::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests Pause() API of OggPlayController "));
	TInt errorCode = iController.Pause();
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("Pause() during Stopped State is failed to return KErrNotReady but returned %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Pause();
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("Pause() during Primed State is failed to return KErrNotReady but returned %d "), errorCode);
		return EFail;
		}
	INFO_PRINTF1(_L("Play"));
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("Pause"));
	errorCode = iController.Pause();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Pause() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	INFO_PRINTF1(_L("GetPosition - thePosition"));
	TTimeIntervalMicroSeconds thePosition;
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() after Pause is failed with  %d "), errorCode);
		return EFail;
		}
	INFO_PRINTF1(_L("Play"));
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() after Pause() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("GetPosition - theNewPosition"));
	TTimeIntervalMicroSeconds theNewPosition;
	errorCode = iController.GetPosition(theNewPosition);
    INFO_PRINTF2(_L("  Paused: %d"), I64INT(thePosition.Int64()));
    INFO_PRINTF2(_L("  Current: %d"), I64INT(theNewPosition.Int64()));

	if(theNewPosition < thePosition)
		{
        //stop the controller
        INFO_PRINTF1(_L("Current Position is lessthan the Paused Postion"));
        ERR_PRINTF2(_L("  Paused: %d"), I64INT(thePosition.Int64()));
        ERR_PRINTF2(_L("  Current: %d"), I64INT(theNewPosition.Int64()));
        iController.Stop();
        return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlStop::RTestStepOggCtrlStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests the Stop functionality of the OggPlayController in Stopped, Primed and Playing 
 * states It expects KErrNotReady error if Stop is called during Stopped state.
 */
TVerdict RTestStepOggCtrlStop::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests Stop() API of OggPlayController "));
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
	User::After(KTwoSeconds);
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
		return EInconclusive;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlMetaData::RTestStepOggCtrlMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This test tries to fetch the metadata from the Ogg file.
 * 
 */
TVerdict RTestStepOggCtrlMetaData::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests GetNumberOfMetaDataEntries/GetMetaDataEntryL() APIs of OggPlayController "));
	TInt numEntries = 0;
	TInt errorCode = iController.GetNumberOfMetaDataEntries( numEntries );
	CMMFMetaDataEntry* metaEntry = NULL;
	if(errorCode)
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
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPlayWindow::RTestStepOggCtrlPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests the PlayWindow functionality of the OggPlayController.
 * Sequence : Prime -> SetPlayWindow -> Play(check if the current position > PlayStart)
 *            Stop  -> Prime -> Play(check if the current position > PlayStart)
 *			  Stop  -> Prime -> DeletePlayWindow -> Play(check if the current position <= PlayStart)
 */
TVerdict RTestStepOggCtrlPlayWindow::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests SetPlaybackWindow/DeletePlaybackWindow() API of OggPlayController "));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
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
	RMMFAudioPlayControllerCustomCommands theCommander(iController);
	errorCode = theCommander.SetPlaybackWindow(thePlayStart, thePlayEnd);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPlaybackWindow() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() after SetPlayWindow() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TTimeIntervalMicroSeconds theNewPosition;
	errorCode = iController.GetPosition(theNewPosition);
	if(!TimeComparison(theNewPosition.Int64(), thePlayStart.Int64(), KDeviationInTime))
		{
        //stop the controller
        INFO_PRINTF1(_L("Current Position is lessthan the PlayWindowStart"));
        iController.Stop();
        return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() with PlayWindow set is failed with %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime with PlayWindow set is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() with PlayWindow is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.GetPosition(theNewPosition);
	if(!TimeComparison(theNewPosition.Int64(), thePlayStart.Int64(), KDeviationInTime))
		{
        //stop the controller
        INFO_PRINTF1(_L("Current Position is lessthan the PlayWindowStart(Second Play)"));
        iController.Stop();
        return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() with PlayWindow set is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime with PlayWindow set is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.DeletePlaybackWindow();
	if(errorCode)
		{
		INFO_PRINTF2(_L("DeletePlaybackWindow() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() after DeletePlaybackWindow is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.GetPosition(theNewPosition);
	if(theNewPosition > thePlayStart)
		{
        //stop the controller
        INFO_PRINTF1(_L("PlaybackWindow is not deleted properly"));
        iController.Stop();
        return EFail;
		}
	//get the play completion event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */	
RTestStepOggCtrlGetSetConfig::RTestStepOggCtrlGetSetConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This test tries to fetch the audio configuration from the Ogg file
 * and compares with the expected configuration.
 */
TVerdict RTestStepOggCtrlGetSetConfig::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests setting and getting audio configuration on OggPlayController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theRate = 0;
	const TUint KExpectedBitRate = 128000;
	const TUint KExpectedSampleRate = 44100;
	const TUint KExpectedSourceChannels = 2;
	TInt errorCode = theCommander.GetSourceBitRate(theRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSourceBitRate() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(theRate != KExpectedBitRate)
		{
		INFO_PRINTF2(_L("GetSourceBitRate() returned the wrong bitrate %d "), theRate);
		return EFail;
		}
	errorCode = theCommander.GetSourceSampleRate(theRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSourceSampleRate() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(theRate != KExpectedSampleRate)
		{
		INFO_PRINTF2(_L("GetSourceBitRate() returned the wrong samplerate %d "), theRate);
		return EFail;
		}
	errorCode = theCommander.GetSourceNumChannels(theRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSourceNumChannels() is failed with  %d "), errorCode);
		return EFail;
		}
	else if(theRate != KExpectedSourceChannels)
		{
		INFO_PRINTF2(_L("GetSourceNumChannels() returned the wrong value %d "), theRate);
		return EFail;
		}
	TUid formatUid;
	errorCode = theCommander.GetSourceFormat(formatUid);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSourceFormat(() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if(formatUid != KOggDecodeUid)
		{
		INFO_PRINTF2(_L("GetSourceFormat() returned the wrong value %d "), formatUid.iUid);
		return EFail;
		}
	TFourCC dataType(0);
	errorCode =theCommander.GetSourceDataType(dataType);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSourceDataType() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( dataType != TFourCC('V', 'O', 'R', 'B') )
		{
        INFO_PRINTF1(_L("GetSourceDataType() returned datatype different from VORB "));
        return EFail;
		}
	/*
	 Setting configurations during playing is not possible because the file has to be played at a configuration  
	 which is same as that of when it was recorded. However, if the client tries to set the configuration which 
	 is same as the configutation of the file, all the SetXXX() custom commands will return KErrNone. We dont check 
	 GetXXX() after SetXXX() here for this reason. 
	 */
	errorCode = theCommander.SetSourceBitRate(KExpectedBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSourceBitRate() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceSampleRate(KExpectedSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSourceSampleRate() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceNumChannels(KExpectedSourceChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSourceNumChannels() is failed with  %d "), errorCode);
		return EFail;
		}
	TFourCC theCodec('V', 'O', 'R', 'B');
	errorCode = theCommander.SetSourceDataType(theCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSourceDataType() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetCodec(theCodec, theCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetCodec() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceFormat(KOggDecodeUid);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSourceFormat() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}

//Negative Tests
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPlayInvalid::RTestStepOggCtrlPlayInvalid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

TVerdict RTestStepOggCtrlPlayInvalid::DoTestStepPreambleL(void)
	{
	return EPass;
	}

/**
 * Do the test step(Negative Test).
 * This test tries to open an invalid file. Expects KErrCurrupt from OggPlayController
 */
TVerdict RTestStepOggCtrlPlayInvalid::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("This test tries to open an invalid file using OggPlayController"));
	iScheduler = new(ELeave)CActiveScheduler;
    CleanupStack::PushL(iScheduler );
	CActiveScheduler::Install(iScheduler );
	CleanupStack::Pop();
	iPriority.iPriority = 11;  // set the priority to some default value
	iPriority.iPref = EMdaPriorityPreferenceQuality;
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	TInt errorCode = iController.Open(KOggPlayControllerUid, iPriority);
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
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlSetInvalidConfig::RTestStepOggCtrlSetInvalidConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step(Negative Test).
 * This tests setting the an invalid SourceDataType on OggPlayController 
 * checks if the OggPlayController returns KErrNotSupported
 */
TVerdict RTestStepOggCtrlSetInvalidConfig::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This tests setting an invalid SourceDataType on OggPlayController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TFourCC theCodec(KMMFFourCCCodeIMAD);
	TInt errorCode = theCommander.SetSourceDataType(theCodec);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceDataType() with non vorbis failed to return KErrNotSupported but returned %d "), errorCode);
		return EFail;
		}
	TUint newBitRate = 224000;
	TUint newSampleRate = 8000;
	TUint newSourceChannels = 1;
	errorCode = theCommander.SetSourceBitRate(newBitRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceBitRate() returned wrong error  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceSampleRate(newSampleRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceSampleRate() returned wrong error  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceNumChannels(newSourceChannels);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceNumChannels() returned wrong error  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSourceFormat(KOggEncodeUid);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceFormat() returned wrong error  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlPositionNegative::RTestStepOggCtrlPositionNegative(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlPlayBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step(Negative Test).
 * This tests setting the Negative Position on OggPlayController
 * checks if the Position set is to 0. 
 */
TVerdict RTestStepOggCtrlPositionNegative::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("This tests setting the Negative Position on OggPlayController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(-1);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theNewPosition;
	errorCode = iController.GetPosition(theNewPosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	
	if(theNewPosition != 0)
		{
		INFO_PRINTF1(_L("Get Position is failed to return 0 when the position is set to a negative value"));
		return EFail;
		}
	return EPass;
	}
