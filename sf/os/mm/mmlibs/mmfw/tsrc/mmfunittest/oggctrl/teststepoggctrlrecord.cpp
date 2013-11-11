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
#include <hal.h>

#include "teststepoggctrlrecord.h"

const TInt KDeviationInCrop = 1000000;
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlRecord::RTestStepOggCtrlRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Primes the OggRecordController and Records an Ogg file for 2 secs
 */
TVerdict RTestStepOggCtrlRecord::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Ogg file with default config using OggRecordController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds thePosition;
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if(thePosition != 0)
		{
		INFO_PRINTF1(_L("Get Position is failed to return 0 in primed state"));
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	User::After(KFiveSeconds);
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
	if(thePosition <= 0)
		{
		INFO_PRINTF1(_L("Get Position is failed to return positive value in recording state"));
		return EFail;
		}
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}

RTestStepOggCtrlGain::RTestStepOggCtrlGain(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the Gain on OggRecordController in Stopped, Primed and Playing 
 * states and checks if the Gain set is same as the Gain got.
 */
TVerdict RTestStepOggCtrlGain::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetGain/GetGain() APIs of OggRecordController "));
	RMMFAudioRecordDeviceCustomCommands theCommander( iController );
	TInt maxGain(0);
	TInt errorCode =theCommander.GetMaxGain( maxGain );
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetMaxGain() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	
	TInt theGain = maxGain /2;
	//try setting gain in Stopped State
	errorCode =theCommander.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewGain (0);
	errorCode =theCommander.GetGain(theNewGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetGain() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewGain != theGain )
		{
        INFO_PRINTF1(_L("Set and Get Gains are different during Stopped State"));
        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting Gain in Primed State
	theGain = maxGain /4;
	errorCode =theCommander.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetGain(theNewGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetGain() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewGain != theGain )
		{
        INFO_PRINTF1(_L("Set and Get Gains are different during Primed State"));
        return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting Gain in Playing State
	theGain = maxGain;
	errorCode =theCommander.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	
	errorCode =theCommander.GetGain(theNewGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetGain() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if( theNewGain != theGain )
		{
        INFO_PRINTF1(_L("Set and Get Gains are different during Recording State"));
        iController.Stop();
        return EFail;
		}
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlRecordBalance::RTestStepOggCtrlRecordBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the RecordBalance on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the RecordBalance set is same as the RecordBalance got.
 */
TVerdict RTestStepOggCtrlRecordBalance::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetBalance/GetBalance() APIs of OggRecordController "));
	RMMFAudioRecordDeviceCustomCommands theCommander( iController );
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	//try setting balance in Stopped State
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
	//try setting balance in Primed State
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
	//try setting balance in Recording State
	errorCode =theCommander.SetBalance(theBalance);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theCommander.GetBalance(theNewBalance);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetBalance() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if(theNewBalance != theBalance)
		{
        //stop the controller
        INFO_PRINTF1(_L("Set and Get Balances are different during Recording State"));
        iController.Stop();
        return EFail;
		}
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlSetMaxFileSize::RTestStepOggCtrlSetMaxFileSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Primes the OggRecordController and sets a limit on the recorded file size
 * and checks for the KErrEof from the OggRecordController
 */
TVerdict RTestStepOggCtrlSetMaxFileSize::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step sets a limit on the recorded file size on OggRecordController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
    TInt theSize = 15000;
	errorCode = theCommander.SetMaxFileSize( theSize );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetMaxFileSize() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	//Dont Stop! Wait for the KErrEOF event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlRecordTimeAvailable::RTestStepOggCtrlRecordTimeAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Primes the OggRecordController, sets a limit on the record file size
 * and queries recordtime availiable
 */
TVerdict RTestStepOggCtrlRecordTimeAvailable::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tries to get the RecordTimeAvailable from OggRecordController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
    TInt theSize = 15000;
	errorCode = theCommander.SetMaxFileSize( theSize );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetMaxFileSize() is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable(theTimeAvailable);
	if( errorCode)
		{
		INFO_PRINTF2(_L("GetRecordTimeAvailable() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	//Dont Stop! Wait for the KErrEOF event
	CActiveScheduler::Start();
	return iTestStepResult;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlGetSetSinkChannels::RTestStepOggCtrlGetSetSinkChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the Number Of Channels on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the Number Of Channels set is same as the Number Of Channels got.
 */
TVerdict RTestStepOggCtrlGetSetSinkChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetSinkNumChannels()/GetSinkNumChannels() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theChannels = 2;
	//try setting No.OfChannels in Stopped State
	TInt errorCode = theCommander.SetSinkNumChannels(theChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkNumChannels() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TUint theNewChannels(0);
	errorCode =theCommander.GetSinkNumChannels(theNewChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkNumChannels() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewChannels != theChannels )
		{
        INFO_PRINTF1(_L("Set and Get No. of Channels are different during Stopped State"));
        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting No.OfChannels in Primed State
	errorCode = theCommander.SetSinkNumChannels(theChannels);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkNumChannels() failed to return KErrNotReady during in Primed State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkNumChannels(theNewChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkNumChannels() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewChannels != theChannels )
		{
        INFO_PRINTF1(_L("Get No. of Channels Primed State returns wrong value"));
        return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting No.OfChannels in Recording State
	errorCode = theCommander.SetSinkNumChannels(theChannels);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkNumChannels() failed to return KErrNotReady during in Recording State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkNumChannels(theNewChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkNumChannels() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewChannels != theChannels )
		{
        INFO_PRINTF1(_L("Get No. of Channels Playing State returns wrong value"));
        return EFail;
		}
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlGetSetSinkSampleRate::RTestStepOggCtrlGetSetSinkSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the samplerate on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the samplerate set is same as the samplerate got.
 */
TVerdict RTestStepOggCtrlGetSetSinkSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetSinkSampleRate/GetSinkSampleRate() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theSampleRate = 44100;
	//try setting samplerate in Stopped State
	TInt errorCode = theCommander.SetSinkSampleRate(theSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TUint theNewSampleRate(0);
	errorCode =theCommander.GetSinkSampleRate(theNewSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewSampleRate != theSampleRate )
		{
        INFO_PRINTF1(_L("Set and Get samplerates are different during Stopped State"));
        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting samplerate in Primed State
	errorCode = theCommander.SetSinkSampleRate(theSampleRate);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() failed to return KErrNotReady during in Primed State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkSampleRate(theNewSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkSampleRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewSampleRate != theSampleRate )
		{
        INFO_PRINTF1(_L("Get samplerates returns wrong value during Primed State"));
        return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//try setting samplerate in Recording State
	errorCode = theCommander.SetSinkSampleRate(theSampleRate);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() failed to return KErrNotReady during in Recording State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkSampleRate(theNewSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkSampleRate() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewSampleRate != theSampleRate )
		{
        INFO_PRINTF1(_L("Get samplerates returns wrong value during Recording State"));
        return EFail;
		}
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlGetSetSinkDataType::RTestStepOggCtrlGetSetSinkDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the SinkDataType on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the SinkDataType set is same as the SinkDataType got.
 */
TVerdict RTestStepOggCtrlGetSetSinkDataType::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetSinkDataType/GetSinkDataType() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TFourCC theCodec(KMMFFourCCCodeIMAD);
	TInt errorCode = theCommander.SetSinkDataType(theCodec);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSinkDataType() with non vorbis failed to return KErrNotSupported but returned %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetCodec(TFourCC('V', 'O', 'R', 'B'), TFourCC('V', 'O', 'R', 'B'));
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetCodec() failed to return KErrNotSupported but returned %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.SetSinkDataType(TFourCC('V', 'O', 'R', 'B'));
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkDataType() failed with %d during stopped state"), errorCode);
		return EFail;
		}
	TFourCC theNewCodec(0);
	errorCode =theCommander.GetSinkDataType(theNewCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkSinkDataType() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewCodec != TFourCC('V', 'O', 'R', 'B') )
		{
        INFO_PRINTF1(_L("GetSinkDataType() returned datatype different from VORB "));
        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theCommander.SetSinkDataType(theCodec);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkDataType() failed to return KErrNotReady during in Primed State but returned  %d "), errorCode);
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
RTestStepOggCtrlGetSetSinkBitRate::RTestStepOggCtrlGetSetSinkBitRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tests setting the bitrate on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the bitrate set is same as the bitrate got.
 */
TVerdict RTestStepOggCtrlGetSetSinkBitRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetSinkBitRate/GetSinkBitRate() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theBitRate = 500000;
	TInt errorCode = theCommander.SetSinkBitRate(theBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TUint theNewBitRate(0);
	errorCode =theCommander.GetSinkBitRate(theNewBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewBitRate != theBitRate )
		{
        INFO_PRINTF1(_L("Set and Get bitrates are different during Stopped State"));
        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theCommander.SetSinkBitRate(theBitRate);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() failed to return KErrNotReady during in Primed State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkBitRate(theNewBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkBitRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewBitRate != theBitRate )
		{
        INFO_PRINTF1(_L("Get bitrate returned wrong bitrate during Primed State"));
        return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theCommander.SetSinkBitRate(theBitRate);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetSinkBitRate() failed to return KErrNotReady during in Recording State but returned  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetSinkBitRate(theNewBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSinkBitRate() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewBitRate != theBitRate )
		{
        INFO_PRINTF1(_L("Get bitrate returned wrong bitrate during Recording State"));
        return EFail;
		}
	User::After(KThreeSeconds);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlGetSetMetaData::RTestStepOggCtrlGetSetMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Adds new metadata to the Ogg file and checks if the meta data is added to the clip
 */
TVerdict RTestStepOggCtrlGetSetMetaData::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This test Adds metadata to the Ogg file "));
	_LIT(K1, "Title");
    _LIT(K2, "Author");
    _LIT(k1, "SampleMetaData");
    _LIT(k2, "Symbian");
    CMMFMetaDataEntry* metaEntry = NULL;
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
	TPtrC ptr1;                             
    TPtrC ptr2;                             
    ptr1.Set(K1);                           
    ptr2.Set(k1);                           
    CMMFMetaDataEntry* metaTitle = CMMFMetaDataEntry::NewL(ptr1,ptr2);
	CleanupStack::PushL(metaTitle);                                             
    ptr1.Set(K2);                           
    ptr2.Set(k2);                           
    CMMFMetaDataEntry* metaAuthor = CMMFMetaDataEntry::NewL(ptr1,ptr2);
    CleanupStack::PushL(metaAuthor);
    TInt errorCode = KErrNone;
    TRAP(errorCode, theCommander.AddMetaDataEntryL(*metaTitle));
    //Adding metadata is not supported in OggRecordController. So we expect KErrNotSupported here.
    if(errorCode == KErrNotSupported)
		{
		INFO_PRINTF1(_L("AddMetaDataEntryL() returned KErrNotSupported "));
		CleanupStack::PopAndDestroy(2);
		return EPass;
		}
    if(errorCode)
		{
		INFO_PRINTF2(_L("AddMetaDataEntryL is failed with  %d "), errorCode);
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}
    TRAP(errorCode, theCommander.AddMetaDataEntryL(*metaAuthor));
    if(errorCode)
		{
		INFO_PRINTF2(_L("AddMetaDataEntryL is failed with  %d "), errorCode);
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}                                         
    TInt numEntries = 0;
	errorCode = iController.GetNumberOfMetaDataEntries( numEntries ); 
	
	if(numEntries != 2)
		{
		INFO_PRINTF1(_L("GetNumberOfMetaDataEntries returned wrong number "));
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}
	
	for(TInt counter = 0; counter < numEntries; ++counter)
        {
        TRAP(errorCode, metaEntry =  iController.GetMetaDataEntryL(counter));
        if(errorCode)
        	{
        	INFO_PRINTF2(_L("GetMetaDataEntryL is failed with  %d "), errorCode);
        	CleanupStack::PopAndDestroy(2);
        	return EFail;
        	}
        INFO_PRINTF3(_L("%S    %S"), &metaEntry->Name(), &metaEntry->Value());
        }
	CleanupStack::PopAndDestroy(2);
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlGetSupportedSinkNoChannels::RTestStepOggCtrlGetSupportedSinkNoChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tries to get SupportedSinkChannels from OggRecordController in Stopped
 */
TVerdict RTestStepOggCtrlGetSupportedSinkNoChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests GetSupportedSinkNumChannels() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander(iController);
	RArray<TUint> supportedChannels;
	CleanupClosePushL(supportedChannels);
	TInt errorCode = KErrNone;
	TRAP(errorCode, theCommander.GetSupportedSinkNumChannelsL(supportedChannels));
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedSinkNumChannelsL() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	CleanupStack::PopAndDestroy(1);
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */	
RTestStepOggCtrlGetSupportedSinkSampleRates::RTestStepOggCtrlGetSupportedSinkSampleRates(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tries to get SupportedSinkSampleRates from OggRecordController in Stopped
 */
TVerdict RTestStepOggCtrlGetSupportedSinkSampleRates::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests GetSupportedSinkSampleRates() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander(iController);
	RArray<TUint> supportedSampleRates;
	CleanupClosePushL(supportedSampleRates);
	TInt errorCode = KErrNone;
	TRAP(errorCode, theCommander.GetSupportedSinkSampleRatesL(supportedSampleRates));
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedSinkSampleRatesL() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	CleanupStack::PopAndDestroy(1);
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */	
RTestStepOggCtrlGetSupportedSinkBitRates::RTestStepOggCtrlGetSupportedSinkBitRates(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 * Do the test step.
 * This tries to get SupportedSinkBitRates from OggRecordController in Stopped
 */
TVerdict RTestStepOggCtrlGetSupportedSinkBitRates::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests GetSupportedSinkBitRates() APIs of OggRecordController "));
	RMMFAudioControllerCustomCommands theCommander(iController);
	RArray<TUint> supportedBitRates;
	CleanupClosePushL(supportedBitRates);
	TInt errorCode = KErrNone;
	TRAP(errorCode, theCommander.GetSupportedSinkBitRatesL(supportedBitRates));
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedSinkBitRatesL() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	CleanupStack::PopAndDestroy(1);
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlRecordCrop::RTestStepOggCtrlRecordCrop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName1, const TDesC& aKeyName2, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName1, aControllerUid)
	{
	iKeyName2 = aKeyName2;
	}
/**
 *
 * Creates the file to be cropped from the sourceFile and sets the file config for AddDataSink
 *
 */
TBool RTestStepOggCtrlRecordCrop::SetupWriteFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
	TPtrC	sourceFileName;
	if(!GetStringFromConfig(iSectName, iKeyName, sourceFileName))
		{
		return EFalse;
		}
	TPtrC	cropFileName;
	if(!GetStringFromConfig(iSectName, iKeyName2, cropFileName))
		{
		return EFalse;
		}
	CopyFile(sourceFileName, cropFileName);
	iFileConfig().iPath = cropFileName;
	return ETrue;
	}

/**
 * Do the test step.
 * This step crops an already recorded file using OggRecordController
 * and checks for the proper play completion
 */
TVerdict RTestStepOggCtrlRecordCrop::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step crops an already recorded file using OggRecordController"));
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
	TInt errorCode = iController.Prime();

	// DEF127335
    // Prime functionality on an already existing clip is not supported in OggRecordController. So we expect KErrNotSupported here.
    if(errorCode == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Prime() returned KErrNotSupported "));
		return EPass;
		}
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theDuration;
	errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt64 duration = theDuration. Int64();
	TTimeIntervalMicroSeconds thePosition(duration/2);
	errorCode = iController.SetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
    errorCode = theCommander.Crop(ETrue);
    //Crop functionality is not supported in OggRecordController. So we expect KErrNotSupported here.
    if(errorCode == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Crop() returned KErrNotSupported "));
		return EPass;
		}
    if(errorCode)
		{
		INFO_PRINTF2(_L("Crop() when ETrue during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
    errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	/*This deviation is more than the KDeviationInTime because when we seek to a specified position and crop,
	the duration is calculated using the granulepos that is lessthan the granulepos we have set before. so the 
	deviation is doubled in this case*/
	if(!TimeComparison(thePosition.Int64(), theDuration.Int64(), KDeviationInCrop))
		{
		INFO_PRINTF1(_L("Clip is not cropped properly "));
		return EFail;
		}
	duration = theDuration. Int64();
	thePosition = duration/2;
	errorCode = iController.SetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theCommander.Crop(EFalse);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Crop() when EFalse during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theNewDuration;
	errorCode = iController.GetDuration(theNewDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	if(!TimeComparison(theNewDuration.Int64(), theDuration.Int64() - thePosition.Int64(), KDeviationInCrop))
		{
		INFO_PRINTF1(_L("Clip is not cropped properly "));
		return EFail;
		}
	return EPass;
	}
/**
 * DoTestStepPostambleL is the postamble function for all test steps
 * It cleans up all the initialization that is done in Preamble
 */	
TVerdict RTestStepOggCtrlRecordCrop::DoTestStepPostambleL(void)
	{
	iController.Stop();
	iController.Reset();
	iControllerEventMonitor->Cancel();
	delete iControllerEventMonitor;
	iController.Close();
	iFs.Close();
	return EPass;
	}
/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */	
RTestStepOggCtrlAppend::RTestStepOggCtrlAppend(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * This test Primes the OggRecordController, Records an Ogg file for 2 secs and stops the controller
 * records again and checks the duration
 */
TVerdict RTestStepOggCtrlAppend::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Ogg file with default config using OggRecordController"));
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
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theDuration;
	errorCode = iController.GetDuration(theDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Prime();

	// DEF127335
    // Prime functionality on an already existing clip is not supported in OggRecordController. So we expect KErrNotSupported here.
    if(errorCode == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Prime() returned KErrNotSupported "));
		return EPass;
		}
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
	User::After(KThreeSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theNewDuration;
	errorCode = iController.GetDuration(theNewDuration);
	if(errorCode)
		{
		INFO_PRINTF2(_L("DurationL() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	if(theNewDuration < theDuration)
		{
		INFO_PRINTF1(_L("Data is not appended to the Clip  "));
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
RTestStepOggCtrlFailsWithAccessDenied::RTestStepOggCtrlFailsWithAccessDenied(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

TVerdict RTestStepOggCtrlFailsWithAccessDenied::DoTestStepPostambleL()
	{
	TInt errorCode = KErrNone;
	errorCode = iFs.SetAtt(iFileConfig().iPath, KEntryAttNormal, KEntryAttReadOnly);

	if(errorCode)
		{
		INFO_PRINTF2(_L("Reset normal file attributes during Postamble failed , SetAtt() failed with  %d "), errorCode);
		RTestStepOggCtrlBase::DoTestStepPostambleL();
		return EFail;
		}
	
	// Call base class postamble to complete cleanup
	return RTestStepOggCtrlBase::DoTestStepPostambleL();
	}


TBool RTestStepOggCtrlFailsWithAccessDenied::SetupWriteFileConfigL()
	{
	User::LeaveIfError(iFs.Connect());
	TPtrC	fileName;
	if(!GetStringFromConfig(iSectName, iKeyName, fileName))
		{
		return EFalse;
		}
	TInt ret = iFs.Delete(fileName);
	if (ret != KErrNone && ret != KErrNotFound)
		{
		User::Leave(ret);	
		}
	RFile file;
	User::LeaveIfError(file.Create(iFs, fileName, EFileRead));
	file.Close();
	User::LeaveIfError(iFs.SetAtt(fileName, KEntryAttReadOnly, KEntryAttNormal));
	iFileConfig().iPath = fileName ;
	return ETrue;	
	}
	
/**
 * This is from MMMFControllerEventMonitorObserver. CMMFControllerEventMonitor
 * calls the method whenever it receives an event from OggRecordController. Basically
 * checks for the Playcompletion event and passes the error back to the test step.
 */
void RTestStepOggCtrlFailsWithAccessDenied::HandleEvent(const TMMFEvent& aEvent)
	{
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		if(aEvent.iErrorCode == KErrAccessDenied)
			{
			iTestStepResult = EPass;
			}
		else
			{
			iTestStepResult = EFail;
			INFO_PRINTF2(_L("Negative test recordCompletion is failed with  %d, expected KErrAccessDenied"), aEvent.iErrorCode);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Failed to receive RecordCompletion Event "));
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}

/**
 * Do the test step.
 * This test Primes the OggRecordController and Records an Ogg file for 2 secs
 */
TVerdict RTestStepOggCtrlFailsWithAccessDenied::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Ogg file with default config using OggRecordController"));
	
	RDebug::Printf("[RTestStepOggCtrlFailsWithAccessDenied::DoTestStepL] here  ");
	
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
    TInt theSize = 15000;
	errorCode = theCommander.SetMaxFileSize( theSize );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetMaxFileSize() is failed with  %d "), errorCode);
		return EFail;
		}		
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	//Dont Stop! Wait for the KErrAccessDenied event
	iTestStepResult = EFail;
	CActiveScheduler::Start();

	return iTestStepResult;
	}

/**
 *
 * Test step constructor.
 * It initialises its own name and the input filename
 *
 */
RTestStepOggCtrlRecord12::RTestStepOggCtrlRecord12(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * DEF116323
 * This test Primes the OggRecordController and Records an Ogg file with sample rate 12KHz for 2 secs
 */
TVerdict RTestStepOggCtrlRecord12::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Ogg file with 12KHz sameple rate using OggRecordController"));
	
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theSampleRate = 12000;
	TInt errorCode = theCommander.SetSinkSampleRate(theSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	
	errorCode = iController.Prime();
	if(errorCode)
		{
		TInt machineType;
		TInt err = HAL::Get(HALData::EMachineUid, machineType);
		if (err)
			{
			INFO_PRINTF1(_L("Error Getting Device information"));
			INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
			return EFail;
			}
		else
			{
			if (machineType==0x102864F7 && errorCode==KErrNotFound)
				{
				INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
			    INFO_PRINTF1(_L("This is a Known Failure on the naviengine"));
				return EKnownFailure;
				}
			else
				{
				INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
				return EFail;
				}
			}
		}
	TTimeIntervalMicroSeconds thePosition;
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if(thePosition != 0)
		{
		INFO_PRINTF1(_L("Get Position is failed to return 0 in primed state"));
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	User::After(KFiveSeconds);
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{

// There is a performance issue with the OggVorbis codec on H4 when it comes to recording audio. 
// This can result in overflows occuring in the device driver that can lead to GetPosition()
// failing with KErrNotReady on pre-A3FDevSound builds.		
#ifdef __MARM__
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		if(errorCode == KErrNotReady)
			{
			INFO_PRINTF1(_L("Get Position has failed with KErrNotReady, this is due to a known performance issue with the OggVorbis codec overflowing when recording on the H4 board."));
			return EPass;	//No need to call Stop() as the controller is already in this state
			}
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND			
#endif // __MARM__
		
		INFO_PRINTF2(_L("GetPosition() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
		
// There is a performance issue with the OggVorbis codec on H4 when it comes to recording audio
// at a sample rate above 8kHz.  This can result in overflows occuring in the device driver
// that can lead to GetPosition() sometimes returning a value of 0 with A3FDevSound.
	TBool positionZeroFailure = ETrue;
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
#ifdef __MARM__
	positionZeroFailure = EFalse;
#endif // __MARM__
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND

	if (thePosition <= 0)
		{
		if ((thePosition < 0) || (positionZeroFailure))
			{
			INFO_PRINTF1(_L("Get Position has failed to return positive value in recording state"));
			return EFail;			
			}
		INFO_PRINTF1(_L("Get Position has returned a value of 0, this is due to a known performance issue with the OggVorbis codec overflowing when recording at rates above 8kHz on the H4 board."));
		}


	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
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
RTestStepOggCtrlRecord24::RTestStepOggCtrlRecord24(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
	:RTestStepOggCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
 * Do the test step.
 * DEF116323
 * This test Primes the OggRecordController and Records an Ogg file with sample rate 24KHz for 2 secs
 */
TVerdict RTestStepOggCtrlRecord24::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Ogg file with 24KHz sample rate using OggRecordController"));
	
	RMMFAudioControllerCustomCommands theCommander( iController );
	TUint theSampleRate = 24000;
	TInt errorCode = theCommander.SetSinkSampleRate(theSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetSinkSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}

	errorCode = iController.Prime();
	if(errorCode)
		{
		TInt machineType;
		TInt err = HAL::Get(HALData::EMachineUid, machineType);
		if (err)
			{
			INFO_PRINTF1(_L("Error Getting Device information"));
			INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
			return EFail;
			}
		else
			{
			if (machineType==0x102864F7 && errorCode==KErrNotFound)
				{
				INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
			    INFO_PRINTF1(_L("This is a Known Failure on the naviengine"));
				return EKnownFailure;
				}
			else
				{
				INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
				return EFail;
				}
			}
		}
	TTimeIntervalMicroSeconds thePosition;
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPosition() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if(thePosition != 0)
		{
		INFO_PRINTF1(_L("Get Position is failed to return 0 in primed state"));
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EFail;
		}
	User::After(KFiveSeconds);
	errorCode = iController.GetPosition(thePosition);
	if(errorCode)
		{

// There is a performance issue with the OggVorbis codec on H4 when it comes to recording audio. 
// This can result in overflows occuring in the device driver that can lead to GetPosition()
// failing with KErrNotReady on pre-A3FDevSound builds.		
#ifdef __MARM__
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
		if(errorCode == KErrNotReady)
			{
			INFO_PRINTF1(_L("Get Position has failed with KErrNotReady, this is due to a known performance issue with the OggVorbis codec overflowing when recording on the H4 board."));
			return EPass;	//No need to call Stop() as the controller is already in this state
			}
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND			
#endif // __MARM__
		
		INFO_PRINTF2(_L("GetPosition() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
		
// There is a performance issue with the OggVorbis codec on H4 when it comes to recording audio
// at a sample rate above 8kHz.  This can result in overflows occuring in the device driver
// that can lead to GetPosition() sometimes returning a value of 0 with A3FDevSound.
	TBool positionZeroFailure = ETrue;
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
#ifdef __MARM__
	positionZeroFailure = EFalse;
#endif // __MARM__
#endif // SYMBIAN_MULTIMEDIA_A3FDEVSOUND

	if (thePosition <= 0)
		{
		if ((thePosition < 0) || (positionZeroFailure))
			{
			INFO_PRINTF1(_L("Get Position has failed to return positive value in recording state"));
			return EFail;			
			}
		INFO_PRINTF1(_L("Get Position has returned a value of 0, this is due to a known performance issue with the OggVorbis codec overflowing when recording at rates above 8kHz on the H4 board."));
		}

	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() during Playing State is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}
