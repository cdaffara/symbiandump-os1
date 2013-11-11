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


// Specific includes for this test suite
#include "tsu_mmf_avictrl_teststep.h"
#include "tsu_mmf_avictrl_testsuite.h"
#include "tsu_mmf_avictrlrecord.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#endif
const TInt KTwoSeconds = 2000000;

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlRecord::RTestStepAviRecordCtrlRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

	
/**
Do the test step. This test Primes the AviRecordController and Records an Avi file for 2 secs
*/
TVerdict RTestStepAviRecordCtrlRecord::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Avi file with default config using AviRecordController"));
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
		iController.Stop();
		return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlGain::RTestStepAviRecordCtrlGain(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the Gain on AviRecordController in Stopped, Primed and Playing 
states and checks if the Gain set is same as the Gain got.
*/
TVerdict RTestStepAviRecordCtrlGain::DoTestStepL( void )
	{
	TInt maxGain(0);
	RMMFVideoRecordControllerCustomCommands theRecordCommander(iController);
	TBool enabled(ETrue);
	TInt errorCode =theRecordCommander.SetAudioEnabled( enabled );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	theRecordCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF1(_L("this step tests SetGain/GetGain() APIs of AviRecordController"));
	RMMFAudioRecordDeviceCustomCommands theAudioDevice( iController );
	errorCode =theAudioDevice.GetMaxGain( maxGain );
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetMaxGain() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt theGain = maxGain /2;
	//try setting gain in Stopped State
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewGain (0);
	errorCode =theAudioDevice.GetGain(theNewGain);
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
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theAudioDevice.GetGain(theNewGain);
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
		iController.Stop();
		return EInconclusive;
		}
	//try setting Gain in Playing State
	theGain = maxGain;
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetGain() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theAudioDevice.GetGain(theNewGain);
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
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlRecordBalance::RTestStepAviRecordCtrlRecordBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 Do the test step. This tests setting the RecordBalance on AviRecordController in Stopped, Primed and Recording 
 states and checks if the RecordBalance set is same as the RecordBalance got.
 */
TVerdict RTestStepAviRecordCtrlRecordBalance::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetBalance/GetBalance() APIs of AviRecordController "));
	RMMFVideoRecordControllerCustomCommands theRecordCommander(iController);
	TBool enabled(ETrue);
	TInt errorCode =theRecordCommander.SetAudioEnabled( enabled );
          if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	theRecordCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	RMMFAudioRecordDeviceCustomCommands theCommander( iController );
	TInt theBalance = (KBalanceMultiplyFactor * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theCommander.SetBalance( theBalance );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetBalance() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewBalance (0);
	errorCode =theCommander.GetBalance( theNewBalance );
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
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlSetMaxFileSize::RTestStepAviRecordCtrlSetMaxFileSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
	
/**
 Do the test step. This test Primes the AviRecordController and sets a limit on the recorded file size
 and checks for the KErrEof from the AviRecordController
 */
TVerdict RTestStepAviRecordCtrlSetMaxFileSize::DoTestStepL()
	{
	INFO_PRINTF1(_L("this step sets a limit on the recorded file size on AviRecordController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoRecordControllerCustomCommands  theVideoCommander( iController );
    	TInt theSize = 15000;
	errorCode = theVideoCommander.SetMaxFileSize( theSize );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetMaxFileSize() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	//Dont Stop! Wait for the KErrEOF event
	CActiveScheduler::Start();
	return iTestStepResult;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlRecordTimeAvailable::RTestStepAviRecordCtrlRecordTimeAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
	
/**
Do the test step. This test Primes the AviRecordController, sets a limit on the record file size
 and queries recordtime availiable
 */
TVerdict RTestStepAviRecordCtrlRecordTimeAvailable::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tries to get the RecordTimeAvailable from AviRecordController"));
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoRecordControllerCustomCommands theVideoCommander( iController );
    	TInt theSize = 15000;
	errorCode = theVideoCommander.SetMaxFileSize( theSize );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetMaxFileSize() is failed with  %d "), errorCode);
		return EFail;
		}
	TTimeIntervalMicroSeconds theTimeAvailable(0);
	errorCode = theVideoCommander.GetRecordTimeAvailable(theTimeAvailable);
	if( errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetRecordTimeAvailable() is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	//Dont Stop! Wait for the KErrEOF event
	CActiveScheduler::Start();
	return iTestStepResult;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlSetGetFrameRate::RTestStepAviRecordCtrlSetGetFrameRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the frame rate on AviRecordController in Stopped, Primed and Recording 
states and checks if the framerate set is same as the framerate got.
*/
TVerdict RTestStepAviRecordCtrlSetGetFrameRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetFrameRate/GetFrameRate() APIs of AviRecordController"));
	TReal32 framerate = 10.0;
	TReal32 newFramerate;
	RMMFVideoControllerCustomCommands theCommander(iController);
	TInt errorCode =theCommander.SetFrameRate(framerate);
	 if(errorCode)
		{
		INFO_PRINTF2(_L("SetFrameRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetFrameRate(newFramerate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("Get FrameRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newFramerate != framerate )
		{
	         INFO_PRINTF1(_L("Set and Get frame rates are different during Stopped State"));
	         return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theCommander.SetFrameRate(framerate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetFrameRate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetFrameRate(newFramerate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrame Rate() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newFramerate != framerate )
		{
	          INFO_PRINTF1(_L("Set and Get Frame rates are different during Primed State"));
	          return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		 INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		 iController.Stop();
		 return EInconclusive;
		}
	errorCode =theCommander.SetFrameRate(framerate);
	if(errorCode)
		{
		 INFO_PRINTF2(_L("SetFrameRate() during recording state is failed with  %d "), errorCode);
		 iController.Stop();
		 return EFail;
		}
	errorCode =theCommander.GetFrameRate(newFramerate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameRate() during recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if( newFramerate != framerate )
		{
	          INFO_PRINTF1(_L("Set and Get Frame Rates are different during recording State"));
	          return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}	
	return EPass;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/	
RTestStepAviRecirdCtrlSetGetFrameSize::RTestStepAviRecirdCtrlSetGetFrameSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the bitrate on AviRecordController in Stopped, Primed and Recording 
states and checks if the bitrate set is same as the bitrate got.
*/
TVerdict RTestStepAviRecirdCtrlSetGetFrameSize::DoTestStepL( void )
	{
	const TSize KFrameSize (176,144);
	const TSize KVideoFrameSize(KFrameSize);
	TSize framesize(KVideoFrameSize);
	TSize newFramesize;
	RMMFVideoRecordControllerCustomCommands  theCommander(iController);
	TInt errorCode =theCommander.SetVideoFrameSize(framesize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetFrameSize() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoControllerCustomCommands getCommander(iController);	
	errorCode =getCommander.GetVideoFrameSize(newFramesize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newFramesize != framesize )
		{
	          INFO_PRINTF1(_L("Set and Get Framesizes are different during Stopped State"));
	          return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode =theCommander.SetVideoFrameSize(framesize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetFrameSize() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =getCommander.GetVideoFrameSize(newFramesize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newFramesize != framesize )
		{
	         INFO_PRINTF1(_L("Set and Get Frame Sizes are different during Primed  State"));
	         return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	errorCode =getCommander.GetVideoFrameSize(newFramesize);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetFrameSize() during recording  State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	if( newFramesize != framesize )
		{
	        	INFO_PRINTF1(_L("Set and Get frame sizes  are different during recording  State"));
	        	return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}	
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlGetSetAudioDataType::RTestStepAviRecordCtrlGetSetAudioDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 Do the test step. This tests setting the SinkDataType on AviRecordController in Stopped, Primed and Recording 
 states and checks if the SinkDataType set is same as the SinkDataType got.
 */
TVerdict RTestStepAviRecordCtrlGetSetAudioDataType::DoTestStepL( void )
	{
	RMMFVideoRecordControllerCustomCommands theVideoRecordCommander(iController);
	TFourCC theNewCodec;
	TInt errorCode = 0;
	TBool enabled(ETrue);
	TInt bitRate = 705600;
	TInt newBitRate = 0;
	errorCode =theVideoRecordCommander.SetAudioEnabled( enabled );
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	theVideoRecordCommander.GetAudioEnabled(enabled);
	if(!enabled)
		{
		INFO_PRINTF2(_L("GetAudioEnabled() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = theVideoRecordCommander.SetAudioCodec(KMMFFourCCCodePCM8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioCodec() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoControllerCustomCommands theGetCommander(iController);
	errorCode =theGetCommander.GetAudioCodec(theNewCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioCodec() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewCodec != KMMFFourCCCodePCM8)
		{
	        INFO_PRINTF1(_L("GetAudioCodec() returned datatype different from PCM8 "));
	        return EFail;
		}
	errorCode = theVideoRecordCommander.SetAudioBitRate(bitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theGetCommander.GetAudioBitRate(newBitRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioBitRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newBitRate != bitRate)
		{
	         INFO_PRINTF1(_L("GetAudioBitRate() returned wronng bit rate "));
	         return EFail;
		}		
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	//Audio codec fail;
	errorCode = theVideoRecordCommander.SetAudioCodec(KMMFFourCCCodePCM8);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioCodec() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theGetCommander.GetAudioCodec(theNewCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioCodec() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if( theNewCodec != KMMFFourCCCodePCM8 )
		{
	         INFO_PRINTF1(_L("GetAudioCodec() returned datatype different from PCM8 "));
	         return EFail;
		}
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlGetSetVideoDataType::RTestStepAviRecordCtrlGetSetVideoDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
 Do the test step. This tests setting the SinkDataType on AviRecordController in Stopped, Primed and Recording 
 states and checks if the SinkDataType set is same as the SinkDataType got.
 */
TVerdict RTestStepAviRecordCtrlGetSetVideoDataType::DoTestStepL( void )
	{
	HBufC8* iMimeType = HBufC8::NewL(256);
	TPtr8 des = iMimeType->Des();
	char xv[]="XVID";
	TUint8* p = (TUint8*)xv;
	TPtrC8 temp(p);
	_LIT8(KAviVideoCodec,"XVID");	
	INFO_PRINTF1(_L("this step tests SetVideoMimeType/GetVideoMimeType() APIs of AviRecordController"));
	
	RMMFVideoRecordControllerCustomCommands theVideoRecordCommander(iController);
	TInt errorCode = theVideoRecordCommander.SetVideoCodec(KAviVideoCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVideoCodec() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	RMMFVideoControllerCustomCommands theGetCommander(iController);
	errorCode =theGetCommander.GetVideoMimeType(des);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVideoMimeType() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if (des.Compare(temp))
		{
	        INFO_PRINTF1(_L("GetVideoMimeType() returned datatype different from XVID "));
	        return EFail;
		}
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = theVideoRecordCommander.SetVideoCodec(KAviVideoCodec);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetVideoCodec() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	//RMMFVideoControllerCustomCommands theGetCommander(iController);
	errorCode =theGetCommander.GetVideoMimeType(des);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetVideoMimeType() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	if (des.Compare(temp))
		{
	        INFO_PRINTF1(_L("GetVideoMimeType() returned datatype different from XVID "));
	        return EFail;
		}
	return EPass;
	}
		

/**
Test step constructor. It initialises its own name and the input filename
*/	
RTestStepAviRecordCtrlGetSetMetaData::RTestStepAviRecordCtrlGetSetMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
/**
This test Adds new metadata to the Avi file and checks if the meta data is added to the clip
 */
TVerdict RTestStepAviRecordCtrlGetSetMetaData::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This test Adds metadata to the Avi file "));
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
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("AddMetaDataEntryL is failed with  %d "), errorCode);
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}
	TRAP(errorCode, theCommander.AddMetaDataEntryL(*metaAuthor));
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("AddMetaDataEntryL is failed with  %d "), errorCode);
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}                                         
	TInt numEntries = 0;
	errorCode = iController.GetNumberOfMetaDataEntries( numEntries ); 
	if(numEntries != 0)
		{
		INFO_PRINTF1(_L("GetNumberOfMetaDataEntries returned wrong number "));
		CleanupStack::PopAndDestroy(2);
		return EFail;
		}
	for(TInt counter = 0; counter < numEntries; ++counter)
        		{
	        TRAP(errorCode, metaEntry =  iController.GetMetaDataEntryL(counter));
	         if(errorCode != KErrNotSupported)
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
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlDuration::RTestStepAviRecordCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}
 
/**
This test Primes the AviRecordController, Records an Avi file for 2 secs and stops the controller
records again and checks the duration
*/  	 
TVerdict RTestStepAviRecordCtrlDuration::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step records an Avi file with default config using AviRecordController"));
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
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
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
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
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
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordNotAudioEnabled::RTestStepAviRecordNotAudioEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
This tests the gain and record balance functionality in the file when audio is not enabled.
*/  	
TVerdict RTestStepAviRecordNotAudioEnabled::DoTestStepL( void )
	{
	TInt maxGain(0);
	INFO_PRINTF1(_L("this step tests SetGain/GetGain() APIs of AviRecordControllerwithout enabling audio"));
	RMMFAudioRecordDeviceCustomCommands theAudioDevice( iController );
	TInt errorCode =theAudioDevice.GetMaxGain( maxGain );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetMaxGain() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	TInt theGain = maxGain /2;
	//try setting gain in Stopped State
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode !=KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetGain() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewGain (0);
	errorCode =theAudioDevice.GetGain(theNewGain);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetGain() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theBalance = (KBalanceMultiplyFactor * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance( theBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetBalance() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	TInt theNewBalance (0);
	errorCode =theAudioDevice.GetBalance( theNewBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetBalance() during Stopped State is failed with  %d "), errorCode);
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
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetGain() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theAudioDevice.GetGain(theNewGain);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetGain() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	 theBalance = (KBalanceMultiplyFactor * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance( theBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetBalance() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theAudioDevice.GetBalance( theNewBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetBalance() during Primed State is failed with  %d "), errorCode);
		return EFail;
		}		
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	//try setting Gain in Playing State
	theGain = maxGain;
	errorCode =theAudioDevice.SetGain(theGain);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetGain() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	errorCode =theAudioDevice.GetGain(theNewGain);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetGain() during Recording State is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	theBalance = (KBalanceMultiplyFactor * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	errorCode = theAudioDevice.SetBalance( theBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetBalance() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theAudioDevice.GetBalance( theNewBalance );
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("GetBalance() during Recording State is failed with  %d "), errorCode);
		return EFail;
		}		
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/	
RTestStepAviRecordCtrlPosition::RTestStepAviRecordCtrlPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
This tests the Set and get position functionality of the AviRecordController in Stopped, Primed and Playing
and  playing states
*/  	
TVerdict RTestStepAviRecordCtrlPosition::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("this step tests SetPosition() APIs of AviRecordController"));
	TTimeIntervalMicroSeconds aPosition(0);
	TTimeIntervalMicroSeconds thePosition(0);
	TInt errorCode = KErrNone;
	errorCode = iController.SetPosition(aPosition);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetPosition() during stopped state is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode = iController.GetPosition(thePosition);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("GetPosition() during stopped state is failed with  %d "), errorCode);
		return EFail;
		}	
	errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(aPosition);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPosition() during primed state is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.GetPosition(thePosition);
	if(errorCode )
		{
		INFO_PRINTF2(_L("GetPosition() during primed is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play()  is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.SetPosition(aPosition);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetPosition() during recording state is failed with  %d "), errorCode);
		iController.Stop();
		return EFail;
		}
	User::After(KTwoSeconds);
	errorCode = iController.GetPosition(thePosition);
	if(errorCode )
		{
		INFO_PRINTF2(_L("GetPosition() during recording is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;		
	}
	
/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlPause::RTestStepAviRecordCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
This tests the Pause functionality of the AviPlayController in Stopped, Primed and Playing 
states and checks if the Play starts from the current position. It expects KErrNotReady
error if Pause is called during Stopped and Primed states.
*/  
TVerdict RTestStepAviRecordCtrlPause::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("this step tests Pause() API of AviRecordController "));
	TInt errorCode = iController.Prime();
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
	errorCode = iController.Pause();
	if(errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("Pause() during Playing State failed with error %d "), errorCode);
		return EFail;
		}	
	//get the record completion event
	User::After(KTwoSeconds);
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;		
	}

/**
Test step constructor. It initialises its own name and the input filename
*/
RTestStepAviRecordCtrlDurationNewClip::RTestStepAviRecordCtrlDurationNewClip(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	
	}

/**
DEF088332: MVS UI Application does not update the timer during video recording
This tests the Duration functionality of the AviPlayController.
*/  
TVerdict RTestStepAviRecordCtrlDurationNewClip::DoTestStepL(void)
	{
	INFO_PRINTF1(_L("DEF088332: MVS UI Application does not update the timer during video recording "));
	INFO_PRINTF1(_L("this step tests GetDuration() API of AviRecordController "));
	TInt errorCode = iController.Prime();
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
	
	// get the duration but do not stop in the meantime.
	TTimeIntervalMicroSeconds theDuration1(0);
	TTimeIntervalMicroSeconds theDuration2(0);
	User::After(KTwoSeconds);
	TRAP(errorCode, iController.GetDuration(theDuration1));
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF2(_L("Duration is %i "), I64LOW(theDuration1.Int64()));
	User::After(KTwoSeconds);
	TRAP(errorCode, iController.GetDuration(theDuration2));
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		return EInconclusive;
		}
	INFO_PRINTF2(_L("Duration is %i "), I64LOW(theDuration2.Int64()));

	// pass if both are nonzero and not equal
	if(theDuration1 == 0 || theDuration2 == 0 || theDuration2 <= theDuration1)
		{
		return EFail;
		}
			
	errorCode = iController.Stop();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Stop() is failed with  %d "), errorCode);
		return EFail;
		}
	return EPass;		
	}


/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetGetPixelAspectRatio::RTestStepAviRecordCtrlSetGetPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the pixel aspect ratio on AviRecordController in Stopped state and checks if the PAR set is same as the PAR got (1:1).
*/
TVerdict RTestStepAviRecordCtrlSetGetPixelAspectRatio::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetPixelAspectRatio/GetPixelAspectRatio() APIs of AviRecordController"));
	TVideoAspectRatio par(1,1);
	TVideoAspectRatio newPar;
	RMMFVideoPixelAspectRatioCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetPixelAspectRatio(par);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetPixelAspectRatio() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetPixelAspectRatio(newPar);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetPixelAspectRatio() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newPar != par )
		{
	    INFO_PRINTF1(_L("Set and Get pixel aspect ratio are different during Stopped State"));
	    return EFail;
		}
		
	INFO_PRINTF1(_L("Set and Get pixel aspect ratio are identical during Stopped State: (1:1)"));
	return EPass;
	}


/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlGetSupportedPixelAspectRatios::RTestStepAviRecordCtrlGetSupportedPixelAspectRatios(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests getting the list of supported pixel aspect ratios on AviRecordController and checks the returned list contains only square PAR (1:1).
*/
TVerdict RTestStepAviRecordCtrlGetSupportedPixelAspectRatios::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests GetSupportedPixelAspectRatiosL() APIs of AviRecordController"));
	TVerdict verdict = EFail;
	RArray<TVideoAspectRatio> supportedPARs;
	RMMFVideoPixelAspectRatioCustomCommands theCommander(iController);
	TInt errorCode = 0;
	TRAP(errorCode, theCommander.GetSupportedPixelAspectRatiosL(supportedPARs));	
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedPixelAspectRatiosL() is failed with  %d "), errorCode);		
		verdict = EFail;
		}	
	if(supportedPARs.Count() == 1 && (supportedPARs[0]).iNumerator == 1 && (supportedPARs[0]).iNumerator == 1) // Only supported PAR is (1:1)
		{
	    INFO_PRINTF1(_L("Get the supported pixel aspect ratios returned the expected values."));
	    verdict = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("Get the supported pixel aspect ratios didn't return the expected values."));
	    verdict = EFail;		
		}
	
	supportedPARs.Close();
	return verdict;	
	}


/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetGetAudioSampleRate::RTestStepAviRecordCtrlSetGetAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the audio sample rate on AviRecordController in Stopped state and checks if the audio sample rate set is same as the audio sample rate got 22050Hz.
*/
TVerdict RTestStepAviRecordCtrlSetGetAudioSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioSampleRate/GetAudioSampleRate() APIs of AviRecordController"));
	TUint sampleRate = 22050; // The only supported rate is 22050Hz
	TUint newSampleRate = 0;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetAudioSampleRate(sampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetAudioSampleRate(newSampleRate);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioSampleRate() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newSampleRate != sampleRate )
		{
	    INFO_PRINTF1(_L("Set and Get audio sample rate are different during Stopped State"));
	    return EFail;
		}	
	
	INFO_PRINTF1(_L("Set and Get audio sample rate are identical during Stopped State: 22050Hz"));
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlGetSupportedAudioSampleRate::RTestStepAviRecordCtrlGetSupportedAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests getting the list of supported audio sample rates on AviRecordController and checks the returned list contains only rate 22050Hz.
*/
TVerdict RTestStepAviRecordCtrlGetSupportedAudioSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests GetSupportedAudioSampleRatesL() APIs of AviRecordController"));
	TVerdict verdict = EFail;
	RArray<TUint> supportedSampleRates;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = 0;
	TRAP(errorCode, theCommander.GetSupportedAudioSampleRatesL(supportedSampleRates));	
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() is failed with  %d "), errorCode);		
		verdict = EFail;
		}
	if(supportedSampleRates.Count() == 1 && supportedSampleRates[0] == 22050) //Only supported audio sample rate is 22050Hz
		{
	    INFO_PRINTF1(_L("Get the supported audio sample rates returned the expected values."));
	    verdict = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("Get the supported audio sample rates didn't return the expected values."));	    
	    verdict = EFail;		
		}
	
	supportedSampleRates.Close();
	return verdict;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetGetAudioChannels::RTestStepAviRecordCtrlSetGetAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the numbetr of audio channels on AviRecordController in Stopped state and checks if the number of audio channels set is same as the number of audio channels got 2 (stereo).
*/
TVerdict RTestStepAviRecordCtrlSetGetAudioChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioChannels/GetAudioChannels() APIs of AviRecordController"));
	TUint audioChannels = 2; // The only supported numb er of audio channels is stereo 2
	TUint newAudioChannels = 0;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetAudioChannels(audioChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("SetAudioChannels() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	errorCode =theCommander.GetAudioChannels(newAudioChannels);
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetAudioChannels() during Stopped State is failed with  %d "), errorCode);
		return EFail;
		}
	if( newAudioChannels != audioChannels )
		{
	    INFO_PRINTF1(_L("Set and Get number of audio channels to record are different during Stopped State"));
	    return EFail;
		}	
	
	INFO_PRINTF1(_L("Set and Get number of audio channels to record are identical during Stopped State: 2 stereo"));	
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlGetSupportedAudioChannels::RTestStepAviRecordCtrlGetSupportedAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests getting the list of supported numbers of audio channels on AviRecordController and checks if the returned list contains only stereo = 2.
*/
TVerdict RTestStepAviRecordCtrlGetSupportedAudioChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests GetSupportedAudioSampleRatesL() APIs of AviRecordController"));
	TVerdict verdict = EFail;
	RArray<TUint> supportedAudioChannels;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = 0;
	TRAP(errorCode, theCommander.GetSupportedAudioChannelsL(supportedAudioChannels));	
	if(errorCode)
		{
		INFO_PRINTF2(_L("GetSupportedAudioSampleRatesL() is failed with  %d "), errorCode);		
		verdict = EFail;
		}
	if(supportedAudioChannels.Count() == 1 && supportedAudioChannels[0] == 2) //Only supported number of audio channels to record is 2 for stereo
		{
	    INFO_PRINTF1(_L("Get the supported numbers of audio channels returned the expected values."));	    
	    verdict = EPass;
		}
	else
		{
		INFO_PRINTF1(_L("Get the supported numbers of audio channels didn't return the expected values."));
	    supportedAudioChannels.Close();
	    verdict = EFail;		
		}
		
	supportedAudioChannels.Close();
	return verdict;	
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetInvalidPixelAspectRatio::RTestStepAviRecordCtrlSetInvalidPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting an invalid pixel aspect ratio on AviRecordController in Stopped states and checks if the returned error is KErrNotSupported.
*/
TVerdict RTestStepAviRecordCtrlSetInvalidPixelAspectRatio::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetPixelAspectRatio with an invalid input (APIs of AviRecordController)"));
	TVideoAspectRatio par(16,9); // Only (1:1) is supported
	RMMFVideoPixelAspectRatioCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetPixelAspectRatio(par);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetPixelAspectRatio() with an invalid input during Stopped State did not fail as expected (returned error = %d)"), errorCode);
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetPixelAspectRatio() with an invalid input during Stopped State failed with expected error %d "), errorCode);
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetInvalidAudioSampleRate::RTestStepAviRecordCtrlSetInvalidAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting an invalid audio sample rate on AviRecordController in Stopped states and checks if the returned error is KErrNotSupported.
*/
TVerdict RTestStepAviRecordCtrlSetInvalidAudioSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioSampleRate with an invalid input (APIs of AviRecordController)"));
	TUint sampleRate = 8000; // Only 22050Hz is supported
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetAudioSampleRate(sampleRate);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetAudioSampleRate() with an invalid input during Stopped State did not fail as expected (returned error = %d) "), errorCode);
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetAudioSampleRate() with an invalid input during Stopped State failed with expected error %d "), errorCode);
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlSetInvalidAudioChannels::RTestStepAviRecordCtrlSetInvalidAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting an invalid number of audio channels on AviRecordController in Stopped state and checks if the returned error is KErrNotSupported.
*/
TVerdict RTestStepAviRecordCtrlSetInvalidAudioChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioChannels with an invalid input (APIs of AviRecordController)"));
	TUint audioChannels = 1; // for mono, only 2 stereo is supported
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = theCommander.SetAudioChannels(audioChannels);
	if(errorCode != KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetAudioChannels() with an invalid input during Stopped State did not fail as expected (returned error = %d) "), errorCode);
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetAudioChannels() with an invalid input during Stopped State failed with expected error %d "), errorCode);
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio::RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the pixel aspect ratio on AviRecordController during recording and checks if the returned error is KErrNotReady.
*/
TVerdict RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetPixelAspectRatio during an invalid state (during recording) (APIs of AviRecordController)"));
	TVideoAspectRatio par(1,1);
	RMMFVideoPixelAspectRatioCustomCommands theCommander(iController);
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	
	errorCode = theCommander.SetPixelAspectRatio(par);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetPixelAspectRatio() during recording did not fail as expected (returned error = %d)"), errorCode);
		iController.Stop();
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetPixelAspectRatio() during recording failed with expected error %d "), errorCode);
	iController.Stop();
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate::RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the audio sample rate on AviRecordController during recording and checks if the returned error is KErrNotReady.
*/
TVerdict RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioSampleRate during an invalid state (during recording) (APIs of AviRecordController)"));
	TUint sampleRate = 22050;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	
	errorCode = theCommander.SetAudioSampleRate(sampleRate);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetAudioSampleRate() during recording did not fail as expected (returned error = %d)"), errorCode);
		iController.Stop();
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetAudioSampleRate() during recording failed with expectied error %d "), errorCode);
	iController.Stop();
	return EPass;
	}

/**
Test step constructor. It initialises its own name and the input filename.
*/
RTestStepAviRecordCtrlInvalidStateSetAudioChannels::RTestStepAviRecordCtrlInvalidStateSetAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid)
:RTestStepAviCtrlRecordBase(aTestName, aSectName, aKeyName, aControllerUid)
	{
	}

/**
Do the test step. This tests setting the number of audio channels on AviRecordController during recording and checks if the returned error is KErrNotReady.
*/
TVerdict RTestStepAviRecordCtrlInvalidStateSetAudioChannels::DoTestStepL( void )
	{
	INFO_PRINTF1(_L("This step tests SetAudioChannels during an invalid state (during recording) (APIs of AviRecordController)"));
	TUint audioChannels = 2;
	RMMFVideoAudioSamplingRateAndChannelConfigCustomCommands theCommander(iController);
	TInt errorCode = iController.Prime();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Prime is failed with  %d "), errorCode);
		return EInconclusive;
		}
	errorCode = iController.Play();
	if(errorCode)
		{
		INFO_PRINTF2(_L("Play() is failed with  %d "), errorCode);
		iController.Stop();
		return EInconclusive;
		}
	
	errorCode = theCommander.SetAudioChannels(audioChannels);
	if(errorCode != KErrNotReady)
		{
		INFO_PRINTF2(_L("SetAudioChannels() during recording did not fail as expected (returned error = %d)"), errorCode);
		iController.Stop();
		return EFail;
		}
	
	INFO_PRINTF2(_L("SetAudioChannels() during recording failed with expected error %d "), errorCode);
	iController.Stop();
	return EPass;
	}
