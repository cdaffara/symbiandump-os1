// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TestStepActrlPlay
//

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

#include "ActrlTestUids.h"

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"

// Specific includes for these test steps
#include "TestStepActrlPlay.h"

// CONSTANTS
//const TInt KPlayDelayNewMail = 100000000; //arbitrary delay	// EABI warning removal

const TUint KSampleRate8000Hz  = 8000;
const TUint KSampleRate11025Hz = 11025;
const TUint KSampleRate22050Hz = 22050;
const TUint KSampleRate44100Hz = 44100;

const TUint KNone	= 0;
const TUint KMono	= 1;
const TUint KStereo = 2;

//const TUint KFormatWAV = 0;
//const TUint KFormatAU  = 1;
//const TUint KFormatRAW = 2;

// parameter structure for conversion tests
class TConvParameters
	{	
public:
	const TText* iTestName;
	TUid	iSrcFormat;
	TUid	iDestFormat;
	TUint32 iSrcEncoding;
	TUint32 iDestEncoding;
	TUint	iSrcSampleRate;
	TUint	iDestSampleRate;
	TUint	iSrcChannels;
	TUint	iDestChannels;
	const TText* iFromFilename;
	const TText* iRefFilename;
	TInt	iExpectedResult;
	};


// constant table of parameters for tests
const TConvParameters KTestParameters[] =
{	
	// straight encode test
	{ _S("MM-MMF-ACTRL-U-0040-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatAUWrite},		KMMFFourCCCodePCM16,	KMMFFourCCCodePCM16B,	KSampleRate44100Hz,		KSampleRate8000Hz,	KStereo,	KMono,		_S("44KHzPCM16Stereo1KHzTone.wav"),	_S("44KHzPCM16StereoWAVTo8KHzPCM16Mono.au"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0041-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCMU8,	KMMFFourCCCodePCM16,	KSampleRate22050Hz,		KSampleRate11025Hz,	KMono,		KStereo,	_S("22KHzPCM8Mono1KHzTone.wav"),	_S("22KHzPCM8MonoWAVTo11KHzPCM16Stereo.wav"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0042-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatRAWWrite},	KMMFFourCCCodeIMAD,		KMMFFourCCCodePCM16,	KSampleRate11025Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("11KHzADPCMMono1KHzTone.wav"),	_S("11KHzADPCMMonoWAVTo8KHzPCM16Mono.raw"),		KErrNone },
	{ _S("MM-MMF-ACTRL-U-0043-HP"), {KMmfUidFormatAURead},	{KMmfUidFormatAUWrite},		KMMFFourCCCodePCM16B,	KMMFFourCCCodePCM16B,	KSampleRate8000Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("8KHzPCM16Mono1KHzTone.au"),		_S("8KHzPCM16MonoAUTo8KHzPCM16Mono.au"),		KErrNone },
	{ _S("MM-MMF-ACTRL-U-0044-HP"), {KMmfUidFormatAURead},	{KMmfUidFormatRAWWrite},	KMMFFourCCCodePCM16B,	KMMFFourCCCodePCM16,	KSampleRate44100Hz,		KSampleRate22050Hz,	KStereo,	KMono,		_S("44KHzPCM16Stereo1KHzTone.au"),	_S("44KHzPCM16StereoAUTo22KHzPCM16Mono.raw"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0045-HP"), {KMmfUidFormatAURead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16B,	KMMFFourCCCodePCM16,	KSampleRate22050Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("22KHzPCM16Mono1KHzTone.au"),	_S("22KHzPCM16MonoAUTo8KhzPCM16Mono.wav"),		KErrNone },
	{ _S("MM-MMF-ACTRL-U-0046-HP"), {KMmfUidFormatRAWRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodeGSM610,	KMMFFourCCCodePCM16,	KSampleRate8000Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("8KHzGSM610Mono1KHzTone.raw"),	_S("8KHzGSM610MonoRAWTo8KHzPCM16Mono.wav"),		KErrNone },
	{ _S("MM-MMF-ACTRL-U-0047-HP"), {KMmfUidFormatRAWRead},	{KMmfUidFormatRAWWrite},	KMMFFourCCCodeALAW,		KMMFFourCCCodePCM16,	KSampleRate22050Hz,		KSampleRate11025Hz,	KStereo,	KMono,		_S("22KHzALAWStereo1KHzTone.raw"),	_S("22KHzALAWStereoRAWTo11KHzPCM16Mono.raw"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0048-HP"), {KMmfUidFormatRAWRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeGSM610,	KSampleRate11025Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("11KHzPCM16Mono1KHzTone.raw"),	_S("11KHzPCM16MonoRAWTo8KHzGSM610Mono.wav"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0049-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodeALAW,		KMMFFourCCCodePCM16,	KSampleRate22050Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("22KHzALAWMono1KHzTone.wav"),	_S("22KHzALAWMonoWAVTo8KHzPCM16Mono.wav"),		KErrNone },
	{ _S("MM-MMF-ACTRL-U-0050-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeMuLAW,	KSampleRate11025Hz,		KSampleRate8000Hz,	KStereo,	KMono,		_S("11KHzPCM16Stereo1KHzTone.wav"), _S("11KHzPCM16StereoWAVTo8KHzMuLAWMono.wav"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0051-HP"), {KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeGSM610,	KSampleRate8000Hz,		KSampleRate8000Hz,	KStereo,	KMono,		_S("8KHzPCM16Stereo1KHzTone.wav"),	_S("8KHzPCM16StereoWAVTo8KHzGSM610Mono.wav"),	KErrNone },
	
	// test 52 not currently supported - should return -5
	{ _S("MM-MMF-ACTRL-U-0052-HP"), {KMmfUidFormatAURead},	{KMmfUidFormatAUWrite},		KMMFFourCCCodePCM16B,	KMMFFourCCCodeIMAD,		KSampleRate22050Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("22KHzPCM16Mono1KHzTone.au"),	_S("NoReferenceFile"),	KErrNotSupported },
	
	{ _S("MM-MMF-ACTRL-U-0053-HP"), {KMmfUidFormatRAWRead},	{KMmfUidFormatAUWrite},		KMMFFourCCCodePCM16,	KMMFFourCCCodePCM16B,	KSampleRate44100Hz,		KSampleRate8000Hz,	KStereo,	KStereo,	_S("44KHzPCM16Stereo1KHzTone.raw"),	_S("44KHzPCM16StereoRAWTo8KHzPCM16Stereo.au"),	KErrNone },
	
	// test below will return -5
	{ _S("MM-MMF-ACTRL-U-0054-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodePCM16,	KSampleRate11025Hz,		KSampleRate44100Hz,	KStereo,	KStereo,	_S("11KHzPCM16Stereo1KHzTone.wav"),	_S("NoReferenceFile"),	KErrNotSupported },
	{ _S("MM-MMF-ACTRL-U-0055-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatAUWrite},		KMMFFourCCCodePCM16,	KMMFFourCCCodeIMAD,		KSampleRate22050Hz,		KSampleRate22050Hz,	KStereo,	KStereo,	_S("22KHzPCM16Stereo1KHzTone.wav"), _S("NoReferenceFile"),	KErrNotSupported },

	// replacement for test 039 - see DEF017344
	{ _S("MM-MMF-ACTRL-U-0039-HP"),	{KMmfUidFormatAURead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM8,		KMMFFourCCCodePCM16,	KSampleRate11025Hz,		KSampleRate8000Hz,	KMono,		KMono,		_S("NewMail.au"),					_S("11KHzPCMU8MonoAUTo8KHzPCM16Mono.wav"),		KErrNone },

	//DEF025783 - Convert Stereo PCM16 to IMA ADPCM
	//Sink channels are not configured, we are testing if negotiation will do this
	{ _S("MM-MMF-ACTRL-U-0056-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeIMAD,		KSampleRate11025Hz,		KSampleRate11025Hz,	KStereo,	KNone,		_S("11KHzPCM16Stereo1KHzTone.wav"),	_S("11KHzPCM16StereoWAVTo11KHzADPCMMono.wav"),	KErrNone },
	{ _S("MM-MMF-ACTRL-U-0057-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatRAWWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeIMAD,		KSampleRate11025Hz,		KSampleRate11025Hz,	KStereo,	KNone,		_S("11KHzPCM16Stereo1KHzTone.wav"),	_S("11KHzPCM16StereoWAVTo11KHzADPCMMono.raw"),	KErrNone },
	//Configure sink channels to stereo expect KErrNotSupported
	{ _S("MM-MMF-ACTRL-U-0058-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatWAVWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeIMAD,		KSampleRate11025Hz,		KSampleRate11025Hz,	KStereo,	KStereo,	_S("11KHzPCM16Stereo1KHzTone.wav"),	_S("11KHzPCM16StereoWAVTo11KHzADPCMMono.wav"),	KErrNotSupported },
	{ _S("MM-MMF-ACTRL-U-0059-HP"),	{KMmfUidFormatWAVRead},	{KMmfUidFormatRAWWrite},	KMMFFourCCCodePCM16,	KMMFFourCCCodeIMAD,		KSampleRate11025Hz,		KSampleRate11025Hz,	KStereo,	KStereo,	_S("11KHzPCM16Stereo1KHzTone.wav"),	_S("11KHzPCM16StereoWAVTo11KHzADPCMMono.raw"),	KErrNotSupported }

};


/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestStepActrlPlay::CTestStepActrlPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACTRL-U-0018-HP");
	}

/**
*
* Test step destructor.
* 
*/
CTestStepActrlPlay::~CTestStepActrlPlay()
	{
	}


/**
* @xxxx
* Do the test step.
* @return	"TVerdict" the status of the test
*			The result of the test step 
*/
TVerdict CTestStepActrlPlay::DoTestStepL( void )
	{
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Play Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks playing an audio clip");
	Log( KTestStepPlay );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
		//[ configure the filename ]
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}
	
    User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	result = SearchForEvent( playEvent );
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	// test steps return a result
	return result;
	}

/**
*
* CTestStepPlayT
* @param aTimesToPlay
*
*/
CTestStepPlayT::CTestStepPlayT( TInt aTimesToPlay )
: iTimesToPlay( aTimesToPlay )
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACTRL-U-0004-HP");
	}

/**
* 
* ~CTestStepPlayT
*/
CTestStepPlayT::~CTestStepPlayT()
	{
	}

/**
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
* This Test step will play an audio clip n 
* times
* @return	"TVerdict" the status of the test
*			The result of the test step 
*/
TVerdict CTestStepPlayT::DoTestStepL()
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Play Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks playing an audio clip n times");
	Log( KTestStepPlay );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	
	for( TInt iTimes = 0; iTimes < iTimesToPlay; iTimes++)
		{
		errorCode =	iController.Prime();
		User::LeaveIfError( errorCode );
		
		// [ wait for and process the return event from the audio output ]
		
		TMMFEvent primeEvent( KPrimeTestId, KErrNone);
		TVerdict primeResult = SearchForEvent( primeEvent );
		if( primeResult == EFail )
			{
			return primeResult;
			}
		
		User::LeaveIfError( iController.Play() );
		
		// note this test does not wait around 
		// to detect that the audio has been played
		// [ now process the confirmation event that play has reached audio output
		//   this is the raison detre of the test ]
		TMMFEvent playEvent( KPlayTestId, KErrNone );
		
		TVerdict playResult = SearchForEvent( playEvent );
		if( playResult == EFail )
			{
			return playResult ;
			}
		
		// [ lets ensure we are stopped ]
		User::LeaveIfError( iController.Stop() );
		}
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	__MM_HEAP_MARKEND;  
	// test steps return a result
	return result;
	}

/*
*
* CTestStepPause
*
*/
CTestStepPause::CTestStepPause()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0011-HP");
	}

/*
*
* ~CTestStepPause
*
*/
CTestStepPause::~CTestStepPause()
	{
	}

/*
*
* DoTestStepL
* This test checks the pause function
*
*/
TVerdict CTestStepPause::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EFail;
	
	_LIT( KTestPlay, "Pause Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks the pause function");
	Log( KTestStepPlay );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
	   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
		
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

    // [ The controller is in the stopped state 
	//   and should return an error if pause is called ]
		
	TInt pauseError = iController.Pause();
	if( pauseError == KErrNone )
		{
		// [ we should not be able to call pause while stopped ]
		result = EFail;
		return result ;
		}

	TMMFEvent pauseFailEvent( KPauseTestId, pauseError );
	result = SearchForEvent( pauseFailEvent );
	if( result == EFail )
		{
		return result;
		}
	
	// [ move the contoller to the primed state ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );

	//[ call pause in the paused state
    // should return no error ]
	pauseError = iController.Pause();
	if( pauseError == KErrNone )
		{
		//[calling paused while paused returns an error]
		result = EFail;
		return result ;
		}
	
	// [ wait for and process the return event from the audio output ]	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone );
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	result = SearchForEvent( pauseFailEvent );
	if( result == EFail )
		{
		return result;
		}
	
    User::LeaveIfError( iController.Play() );

	// [ process the play event ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ now call pause when playing ]
	TMMFEvent pausePassEvent( KPauseTestId, KErrNone );
	pauseError = iController.Pause();
	if( pauseError != KErrNone )
		{
		//[calling paused while paused returns an error]
		result = EFail;
		return result ;
		}

	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	result = SearchForEvent( pausePassEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ we should now be in the paused state 
	//   try playing ]
	User::LeaveIfError( iController.Play() );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  
	
	__MM_HEAP_MARKEND;    

	return result;
	}

/**
*
* CTestStepStop
*
*/
CTestStepStop::CTestStepStop()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0012-HP");
	}

/**
*
* ~CTestStepStop
*
*/
CTestStepStop::~CTestStepStop()
	{
	}

/*
*
* DoTestStepL
* This test checks the stop function on the controller
*
*/
TVerdict CTestStepStop::DoTestStepL()
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EFail;
	
	_LIT( KTestPlay, "Stop Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks the stop function");
	Log( KTestStepPlay );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
	   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	//[ call stop while stopped ]
	User::LeaveIfError( iController.Stop() );


	// [ go to primed and call stop ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}	

	User::LeaveIfError( iController.Stop() );

	// [ go to play and call stop ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}	

	User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	User::LeaveIfError( iController.Stop() );


	// [ go to prime from stopped via playing ]
	// [ stopped ]
    // [ prime   ]
	// [ play    ]
	// [ pause   ]
	// [ stop    ]

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}	

	User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ now call pause when playing ]
	TMMFEvent pausePassEvent( KPauseTestId, KErrNone );
	TInt pauseError = iController.Pause();
	if( pauseError != KErrNone )
		{
		//[calling paused while paused returns an error]
		result = EFail;
		return result ;
		}

	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	result = SearchForEvent( pausePassEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ now stop from paused ]
	User::LeaveIfError( iController.Stop() );
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result;
	}


/*
*
* CTestStepConvertFormat
*
*/
CTestStepConvertFormat::CTestStepConvertFormat( TUint aTestIndex )
	{
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);

	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

/*
*
* ~CTestStepConvertFormat
*
*/
CTestStepConvertFormat::~CTestStepConvertFormat()
	{


	}


// test preamble
TVerdict CTestStepConvertFormat::DoTestStepPreambleL(void)
	{
	// connect to the file server
	User::LeaveIfError(iFs.Connect());

	// call the superclass
	return CTestStepActrl::DoTestStepPreambleL();
	}

//test postamble
TVerdict CTestStepConvertFormat::DoTestStepPostambleL(void)
	{
	// close connection with the file server
	iFs.Close();

	// call the superclass
	return CTestStepActrl::DoTestStepPostambleL();
	}
/**
 *
 * Compare two binary files
 *
 * @param "const TDesC& aFile1"
 *		  The first file to compare
 * @param "const TDesC& aFile2"
 *		  The second file to compare
 * @leave "" 
 *		  Will leave with appropriate system codes if a problem is encountered reading either file 	
 * @return "TVerdict"
 *		  EPass if the files are identical
 *		  EFail otherwise
 * @xxxx
 *
 */
TVerdict CTestStepConvertFormat::CompareFilesL(const TDesC& aFile1,const TDesC& aFile2) 
	{
	RFile file1, file2;

	User::LeaveIfError(file1.Open(iFs, aFile1, EFileShareAny | EFileStream | EFileRead));
	CleanupClosePushL(file1);

	User::LeaveIfError(file2.Open(iFs, aFile2, EFileShareAny | EFileStream | EFileRead));
	CleanupClosePushL(file2);

	TInt fileSize1, fileSize2;

	TVerdict result = EPass;
	TInt blockSize = 1024;
	
	User::LeaveIfError(file1.Size(fileSize1));
	User::LeaveIfError(file2.Size(fileSize2));

	if (fileSize1 == fileSize2) 
		{
		HBufC8* buf1 = HBufC8::NewMaxLC(blockSize);
		TPtr8 buf1Ptr(buf1->Des());
		HBufC8* buf2 = HBufC8::NewMaxLC(blockSize);
		TPtr8 buf2Ptr(buf2->Des());

		TInt length = 0;

		while (length<fileSize1 && result == EPass)
			{
			TInt copySize;

			if (fileSize1 - length > blockSize)
				copySize = blockSize;
			else
				copySize = fileSize1 - length;
			User::LeaveIfError(file1.Read(buf1Ptr,copySize));
			User::LeaveIfError(file2.Read(buf2Ptr,copySize));

			TInt comp_res = Mem::Compare(buf1Ptr.Ptr(),copySize,buf2Ptr.Ptr(),copySize);
			    
			if (comp_res!=0)
			    {
			    INFO_PRINTF5(_L("Comparison Failed, comparison failed (%S, %S), result=%d at offset %d"),
			            &aFile1, &aFile2, comp_res, length);
				result = EFail;
				break;
			    }

			length += copySize;
			}
		CleanupStack::PopAndDestroy(2); // buf1, buf2
		}
	else
	    {
        INFO_PRINTF5(_L("Comparison Failed, files are different sizes (%S, %S), (%d,%d)"),
                &aFile1, &aFile2, fileSize1, fileSize2);
		result = EFail;
	    }

	file1.Close();
	file2.Close();

	CleanupStack::PopAndDestroy(2); //file1, file2

	return result;
	}
/*
*
* DoTestStepL
* This test checks the stop function on the controller
*
*/
TVerdict CTestStepConvertFormat::DoTestStepL( void )
	{
	TVerdict result = EPass;
	TInt	error;

	// do the conversion step
	TRAP(error, result = DoConvertStepL( ));

	if (error != iTestParameters->iExpectedResult )
		{
		INFO_PRINTF2(_L("Error - Test step returned %d"), error);
		result = EFail;
		}

	//[ delete output file after comparison]
	//[ here because DoTestStepL() can leave ]
	 TInt ret = iFs.Delete( WriteFileConfig()().iPath );
	 if (ret != KErrNone && ret != KErrNotFound)
		{
		INFO_PRINTF2(_L("Error deleting file - Delete returned %d"), ret);
		}
		
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result;
	}

/*
* DoConvertL( void )
* Do the conversion of the files
* @return	"TVerdict" the result of the convert operation
*/
TVerdict CTestStepConvertFormat::DoConvertStepL( void )
	{
	 TVerdict result = EPass;
	
	 _LIT( KTestConvertFormat, "Convert Formats Test");
	 Log( KTestConvertFormat );
	
	 _LIT( KTestStepPlay, "This test checks converting an audio clip");
	 Log( KTestStepPlay );
	
	 // get filename of input file
	 TInt strLen = User::StringLength(iTestParameters->iFromFilename);
	 TPtrC ptr( iTestParameters->iFromFilename, strLen );
	
	 //[ configure the filename ]
	 SetReadFileNameL( ptr );
	 
	 //[ configure reference filename ]
	 strLen = User::StringLength(iTestParameters->iRefFilename);
	 ptr.Set( iTestParameters->iRefFilename, strLen );

	 SetRefFileNameL( ptr );

	 // [ converted audio file name ]
	 //_LIT(KTestOutFile, "ConvertedOutput");
	 TBuf<32> testFileName = _L("ConvertedOutput");

	 switch ( iTestParameters->iDestFormat.iUid )
		{
		case KMmfUidFormatRAWWrite:
			testFileName.Append( _L(".raw") );
			break;

		case KMmfUidFormatWAVWrite:
			testFileName.Append( _L(".wav") );
			break;

		case KMmfUidFormatAUWrite:
			testFileName.Append( _L(".au") );
			break;

		default:
			// not a recognised output format - use .tmp
			testFileName.Append( _L(".tmp") );
			break;
		}	

	 //[ configure the filename ]
	 SetWriteFileNameL( testFileName );

	 // open the audio controller
	 TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	 User::LeaveIfError( errorCode );
	
	 //[ add data source and sink and prime the controller ]
     errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	 User::LeaveIfError( errorCode );
	
	 errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
     User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );
	
	 // if source data is RAW then need to complete the source format
	 if ( iTestParameters->iSrcFormat.iUid == KMmfUidFormatRAWRead )
		{
		
		errorCode = theCommander.SetSourceFormat( iTestParameters->iSrcFormat );
		User::LeaveIfError( errorCode );

		errorCode = theCommander.SetSourceDataType( iTestParameters->iSrcEncoding );
		User::LeaveIfError( errorCode );
		 
		errorCode = theCommander.SetSourceSampleRate( iTestParameters->iSrcSampleRate );
		User::LeaveIfError( errorCode );

		errorCode = theCommander.SetSourceNumChannels( iTestParameters->iSrcChannels );
		User::LeaveIfError( errorCode );
		}	

	 // set sink sample rate
	 errorCode = theCommander.SetSinkSampleRate( iTestParameters->iDestSampleRate );
	 User::LeaveIfError( errorCode );

	 // set sink number of channels
	 if(iTestParameters->iDestChannels != KNone)
		 {
		 errorCode = theCommander.SetSinkNumChannels( iTestParameters->iDestChannels );
		 User::LeaveIfError( errorCode );
		 }

	 //[ ok lets try to set the sink format ]
	 errorCode = theCommander.SetSinkFormat( iTestParameters->iDestFormat );
	 User::LeaveIfError( errorCode );

	 //[ set the sink data type ]
	 errorCode = theCommander.SetSinkDataType( iTestParameters->iDestEncoding );
	 User::LeaveIfError( errorCode );

 	 errorCode = iController.Prime();
	 User::LeaveIfError( errorCode );
	
	 // [ wait for and process the return event from the audio output ]
	 TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	 result = SearchForEvent( primeEvent );
	 if( result == EFail )
		{
		return result;
		}
	
	 errorCode = iController.Play();
     User::LeaveIfError( errorCode );
	
	 // note this test does not wait around 
	 // to detect that the audio has been played
	 // [ now process the confirmation event that play has reached audio output
	 //   this is the raison detre of the test ]
	 TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	 result = SearchForEvent( playEvent );
	 if( result == EFail )
		 {
		 return result;
		 }

	 //[ now wait for play complete event]
	 TMMFEvent playCompleteEvent( KPlayCompleteTestId, KErrNone);
	 result = SearchForEvent( playCompleteEvent );
	 if( result == EFail )
		{
		return result;
		}

	 //[ now compare the file produced with a reference file ]
	 INFO_PRINTF1(_L("Comparing file to reference..."));
	 result = CompareFilesL( RefFileName()().iPath, WriteFileConfig()().iPath);
	 
	 if (result == EFail)
		 {
		 INFO_PRINTF1(_L("Comparison Failed - Files are not identical !"));
		 }
	

	 return result;
	}

/*
*
* SetRefFileNameL
*
*/
void CTestStepConvertFormat::SetRefFileNameL( const TDesC& aPathNameAndExtn )
	{
	//[ append the filename relative to the default path ]
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(aPathNameAndExtn);
	iFileConfigRef().iPath = fileName ;
	}
/*
*
* RefFileName
*
*/
TMMFFileConfig& CTestStepConvertFormat::RefFileName()
	{
	return iFileConfigRef;
	}

/*
*
* CTestStepRampVolume
*/
CTestStepRampVolume::CTestStepRampVolume()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0064-HP");
	}

/*
*
* ~CTestStepRampVolume
*/
CTestStepRampVolume::~CTestStepRampVolume()
	{
	}

/*
*
* CTestStepRampVolume
*/
TVerdict CTestStepRampVolume::DoTestStepL()
	{
	TVerdict result = EPass;
	
	_LIT( KTestVolumeRamp, "Volume Ramp Test");
	Log( KTestVolumeRamp );
	
	_LIT( KTestStepPlay, "This test checks volume ramp");
	Log( KTestStepPlay );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "RampVolume.wav");
		//[ configure the filename ]
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	//[ attempt to get the sink num channels ]
    RMMFAudioPlayDeviceCustomCommands theCommander( iController );
	//[lets say 2 seconds for now ]
	TTimeIntervalMicroSeconds aRampDuration( 2000000);
	errorCode = theCommander.SetVolumeRamp( aRampDuration);
	User::LeaveIfError( errorCode );
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ lets set the volume ramp before we play ]
    
	
    User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return EFail;
		}

	//[ now wait for play complete event]
	 TMMFEvent playCompleteEvent( KPlayCompleteTestId, KErrNone);
	 result = SearchForEvent( playCompleteEvent );
	 if( result == EFail )
		{
		return EFail;
		}
		
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	// test steps return a result
	return result;
	}

/*
*
* CTestStepRampVolume
*/
TVerdict CTestStepRampVolume::DoTestStepPreambleL(void)
	{
	return EPass;
	}

/*
*
* CTestStepRampVolume
*/
TVerdict CTestStepRampVolume::DoTestStepPostambleL(void)
	{
	return CTestStepActrl::DoTestStepPostambleL();
	}

