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
// TestStepsGetSet
//

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

//const TInt KPlayDelayNewMail = 100000000;  //arbitrary delay

// Test system includes
#include <testframework.h>

#include "ActrlTestUids.h"

// Specific includes for this test suite
#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"

// Specific includes for these test steps
#include "TestStepsGetSet.h"

/**
*
* Test step constructor.
* Each test step initialises its own name.
*
*/
CTestStepGetSetVolumeInPrimed::CTestStepGetSetVolumeInPrimed()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACTRL-U-0001-HP");
	}

/**
*
* Test step destructor.
* 
*/
CTestStepGetSetVolumeInPrimed::~CTestStepGetSetVolumeInPrimed()
	{
	}

/**
* 
* Do the test step.
* Each test step must supply an implementation for DoTestStepL.
* This test does nothing more than exercise the rotate & scale code
* @return	"TVerdict" the status of the test
*			The result of the test step 
*/
TVerdict CTestStepGetSetVolumeInPrimed::DoTestStepL( void )
	{
    __MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Play Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks getting/setting volume in primed");
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
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

   //[ set up audio commands utility ]	
   RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

    // [ get the maximum volume ]
	TInt maxVolume = 0;
    errorCode =theCommander.GetMaxVolume( maxVolume );
	User::LeaveIfError( errorCode );

	//[ set the volume to 1/2 max volume ]
	TInt theVolume = maxVolume /2;

	//[ set the volume ]
	errorCode =theCommander.SetVolume( theVolume );
	User::LeaveIfError( errorCode );
	
	//[ get the volume ]
	TInt theNewVolume (0);
	errorCode =theCommander.GetVolume( theNewVolume );
	User::LeaveIfError( errorCode );

	// [ if set volume = got volume  and 
	//   rx'd event was ok then we are fine ]
	if( theNewVolume == theVolume )
		{
         result = EPass ;
		}
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is an error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  
	
	__MM_HEAP_MARKEND;
	   
	// test steps return a result
	return result;
	}


/**
*
*
*
*/
CTestStepGetSetVolumeInPlay::CTestStepGetSetVolumeInPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACTRL-U-0002-HP");
	}

/**
*
* ~CTestStepGetSetVolumeInPlay
*
*/
CTestStepGetSetVolumeInPlay::~CTestStepGetSetVolumeInPlay()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetVolumeInPlay::DoTestStepL( void )
	{
     __MM_HEAP_MARK;
	
	TVerdict result = EFail;
	
	_LIT( KTestPlay, "Play Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks playing an audio clip");
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
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}

	 User::LeaveIfError( iController.Play() );

	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	searchResult = SearchForEvent( playEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}
	
	   //[ set up audio commands utility ]	
   RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

    // [ get the maximum volume ]
	TInt maxVolume = 0;
    errorCode =theCommander.GetMaxVolume( maxVolume );
	User::LeaveIfError( errorCode );

	//[ set the volume to 1/2 max volume ]
	TInt theVolume = maxVolume /2;

	//[ set the volume ]
	errorCode =theCommander.SetVolume( theVolume );
	User::LeaveIfError( errorCode );
	//[ get the volume ]
	TInt theNewVolume (0);
	errorCode =theCommander.GetVolume( theNewVolume );
	User::LeaveIfError( errorCode );

	// [ if set volume = got volume, setVolume2 != got volume
	// and  rx'd event was ok then we are fine ]
    TInt overVolume  = maxVolume+1;
 
	//[ set the volume ]
	TInt overVolumeErrorCode =theCommander.SetVolume( overVolume );
	//[ get the volume ]
	TInt overVolumeResult (-100); // arbitrary initial value
	errorCode =theCommander.GetVolume( overVolumeResult );
	User::LeaveIfError( errorCode );

	TInt underVolume = -1;

	//[ set the volume ]
	TInt underVolumeErrorCode =theCommander.SetVolume( underVolume );
	//[ get the volume ]
	TInt underVolumeResult (0); //arbitrary initial value
	errorCode =theCommander.GetVolume( underVolumeResult );
	User::LeaveIfError( errorCode );

    //[ set the status of the test ]
    if( ( theVolume == theNewVolume ) &&       // volume has been set correctly
		( overVolumeErrorCode != KErrNone) &&  // over volume detected
		(theVolume == overVolumeResult) &&    // volume remains unchanged
		(underVolumeErrorCode != KErrNone) && // under volume detected
		( theVolume == underVolumeResult))    // volume remains unchanged
		{
		result = EPass;
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

/**
CTestStepGetSetVolumeInStopped

*/
CTestStepGetSetVolumeInStopped::CTestStepGetSetVolumeInStopped()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0003-HP");
	}

/**
~CTestStepGetSetVolumeInStopped
*/
CTestStepGetSetVolumeInStopped::~CTestStepGetSetVolumeInStopped()
	{
	}

/**
*
*DoTestStepL
*
*/
TVerdict CTestStepGetSetVolumeInStopped::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EFail;

	_LIT( KTestPlay, "Get/Set Volume while stopped");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks get/set volume while stopped");
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

	//[ set up audio commands utility ]	
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

	// [ get the maximum volume ]
	TInt maxVolume = 0;
    errorCode =theCommander.GetMaxVolume( maxVolume );
	User::LeaveIfError( errorCode );

	//[ set the volume to 1/2 max volume ]
	TInt theVolume = maxVolume /2;

	//[ set the volume ]
	errorCode =theCommander.SetVolume( theVolume );
	User::LeaveIfError( errorCode );
	//[ get the volume ]
	TInt theNewVolume (0);
	errorCode =theCommander.GetVolume( theNewVolume );
	User::LeaveIfError( errorCode );

	// [ if set volume = got volume, setVolume2 != got volume
	// and  rx'd event was ok then we are fine ]
    TInt overVolume  = maxVolume+1;
 
	//[ set the volume ]
	TInt overVolumeErrorCode =theCommander.SetVolume( overVolume );
	//[ get the volume ]
	TInt overVolumeResult (-100); //arbitrary initial value
	errorCode =theCommander.GetVolume( overVolumeResult );
	User::LeaveIfError( errorCode );

	TInt underVolume = -1; //arbitrary initial value

	//[ set the volume ]
	TInt underVolumeErrorCode =theCommander.SetVolume( underVolume );
	//[ get the volume ]
	TInt underVolumeResult (0);
	errorCode =theCommander.GetVolume( underVolumeResult );
	User::LeaveIfError( errorCode );

    //[ set the status of the test ]
    if( ( theVolume == theNewVolume ) &&       // volume has been set correctly
		( overVolumeErrorCode != KErrNone) &&  // over volume detected
		(theVolume == overVolumeResult) &&    // volume remains unchanged
		(underVolumeErrorCode != KErrNone) && // under volume detected
		( theVolume == underVolumeResult))    // volume remains unchanged
		{
		result = EPass;
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
* CTestStepGetSetBalanceInPrimed
*
*/
CTestStepGetSetBalanceInPrimed::CTestStepGetSetBalanceInPrimed()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0005-HP");
	}

/**
*
* ~CTestStepGetSetBalanceInPrimed
*/
CTestStepGetSetBalanceInPrimed::~CTestStepGetSetBalanceInPrimed()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict  CTestStepGetSetBalanceInPrimed::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EFail;

	_LIT( KTestPlay, "Get/Set Balance while primed");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks get/set balance while primed");
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

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}
	
	//[ set up audio commands utility ]	
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

	// [ set the balance to an arbitrary value within the range -100 , 100 ]
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
	//[ set the Balance, we are expecting failure ]
	TInt setErrorCode =theCommander.SetBalance( theBalance );

	//[ get the balance, we are expecting failure ]
	TInt theNewBalance (0);
	TInt getErrorCode =theCommander.GetBalance( theNewBalance );

	//[ set the status of the test ]
    if( ( getErrorCode ==  KErrNone) &&
		( setErrorCode ==  KErrNone) )
		{
		result = EPass;
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
* CTestStepGetSetBalanceInPlay
*/
CTestStepGetSetBalanceInPlay::CTestStepGetSetBalanceInPlay()
	{
   	iTestStepName = _L("MM-MMF-ACTRL-U-0007-HP");
	}

/**
*
* ~CTestStepGetSetBalanceInPlay
*/
CTestStepGetSetBalanceInPlay::~CTestStepGetSetBalanceInPlay()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetBalanceInPlay::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EFail;

	_LIT( KTestPlay, "Get/Set Balance while in Play");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks get/set balance while playing");
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

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}
	
	//[ set the controller to play ]
	User::LeaveIfError( iController.Play() );
	
	//[ set up audio commands utility ]	
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

	// [ set the balance to an arbitrary value between 0,100 and map to the range -100,100 ]
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;;
	//[ set the Balance ]
	TInt setErrorCode =theCommander.SetBalance( theBalance );
	
	//[ get the balance ]
	TInt theNewBalance (0);
	TInt getErrorCode =theCommander.GetBalance( theNewBalance );

	//[ set the status of the test ]
    if( 
		( getErrorCode != KErrNone) && // under volume detected
		( setErrorCode != KErrNone))    // volume remains unchanged
		{
		result = EPass;
		}

    TMMFEvent playEvent( KPlayTestId, KErrNone );
	searchResult = SearchForEvent( playEvent );
	if( searchResult != EFail )
		{
		result = EPass;
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
* CTestStepGetSetBalanceInStopped
*
*/
CTestStepGetSetBalanceInStopped::CTestStepGetSetBalanceInStopped()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0006-HP");
	}

/**
*
* CTestStepGetSetBalanceInStopped
*
*/
CTestStepGetSetBalanceInStopped::~CTestStepGetSetBalanceInStopped()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetBalanceInStopped::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EFail;

	_LIT( KTestPlay, "Get/Set Balance while stopped");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks get/set balance while stopped");
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

	//[ set up audio commands utility ]	
	RMMFAudioPlayDeviceCustomCommands theCommander( iController );	

	// [ set the balance ]
	TInt theBalance = (30 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;;
	//[ set the Balance ]
	errorCode =theCommander.SetBalance( theBalance );
	User::LeaveIfError( errorCode );

	//[ get the balance ]
	TInt theNewBalance (0);
	errorCode =theCommander.GetBalance( theNewBalance );
	User::LeaveIfError( errorCode );

	// [ if set balance = got balance, setbalance2 != got balance
	// and  rx'd event was ok then we are fine 
	// set the balance to an arbitrary value in the range 0..100 and map to the soundev range]
    TInt overBalance  = (300 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;
 
	//[ set the volume ]
	TInt overBalanceErrorCode =theCommander.SetBalance( overBalance );
	//[ get the volume ]
	TInt overBalanceResult (-106); // set to an arbitrary value
	errorCode =theCommander.GetBalance( overBalanceResult );
	User::LeaveIfError( errorCode );

	// [ set the under balance to a value < 0 and map to the correct range]
	TInt underBalance = (-50 * (KMMFBalanceMaxRight-KMMFBalanceMaxLeft))/100 + KMMFBalanceMaxLeft;

	//[ set the volume ]
	TInt underBalanceErrorCode =theCommander.SetBalance( underBalance );
	//[ get the volume ]
	TInt underBalanceResult (0);
	errorCode =theCommander.GetBalance( underBalanceResult );
	User::LeaveIfError( errorCode );

	// Set the test result
	if ((theBalance == theNewBalance) &&				// balance set correctly
		(overBalanceErrorCode == KErrNone) &&			// no error setting over balance
		(overBalanceResult == KMMFBalanceMaxRight) &&	// over balance is set to KMMFBalanceMaxRight
		(underBalanceErrorCode == KErrNone) &&			// no error setting under balance
		(underBalanceResult == KMMFBalanceMaxLeft))		// under balance is set to KMMFBalanceMaxLeft
		{
		result = EPass;
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
//
/**
*
* CTestStepGetSetPositionInPaused
*
*/
CTestStepGetSetPositionInPaused::CTestStepGetSetPositionInPaused()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0070-HP");
	}

/**
*
* ~CTestStepGetSetPositionInPaused
*
*/
CTestStepGetSetPositionInPaused::~CTestStepGetSetPositionInPaused()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetPositionInPaused::DoTestStepL( void )
	{
	 __MM_HEAP_MARK;
	
	TVerdict result = EPass;
	TTimeIntervalMicroSeconds position(0);
	
	_LIT( KTestPause, "Pause Test");
	Log( KTestPause );
	
	_LIT( KTestStepPause, "This test checks set/get position whilst paused: Play->Pause->Rewind->Play (Proper verification requires headphones)");
	Log( KTestStepPause );
	
	// [ audio file to play ]
	// At time of writing, this file is approx 8s duration.
	_LIT(KTestWavFile, "mainTst.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	User::LeaveIfError( iController.Prime() );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}

	User::LeaveIfError( iController.GetPosition( position ) );
	INFO_PRINTF2(_L("Position at start is %d"), position.Int64());


	// note this test does not wait around 
	// to detect that the audio has been played
	User::LeaveIfError( iController.Play() );

	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent1( KPlayTestId, KErrNone );
	searchResult = SearchForEvent( playEvent1 );
	if( searchResult == EFail )
		{
		return searchResult;
		}	

	// [ play for two seconds ]
	User::After(2000000); 

	User::LeaveIfError( iController.GetPosition( position ) );
	INFO_PRINTF2(_L("Position before pause is %d"), position.Int64());


    // [ The controller is in the play state and
	//   should not return an error if pause is called ]
	User::LeaveIfError( iController.Pause() );

	TMMFEvent pauseEvent( KPauseTestId, KErrNone );
	result = SearchForEvent( pauseEvent );
	if( result == EFail )
		{
		return result;
		}

	User::LeaveIfError( iController.GetPosition( position ) );
	INFO_PRINTF2(_L("Position after pause is %d"), position.Int64());

	//[ ok lets reset to the beginning ]
	User::LeaveIfError( iController.SetPosition( TTimeIntervalMicroSeconds(0) ) );
	
	User::LeaveIfError( iController.GetPosition( position ) );
	INFO_PRINTF2(_L("Position after rewind is %d"), position.Int64());


	// [ Now let's start to play again ]

	// note this test does not wait around 
	// to detect that the audio has been played
	User::LeaveIfError( iController.Play() );

	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent2( KPlayTestId, KErrNone );
	searchResult = SearchForEvent( playEvent2 );
	if( searchResult == EFail )
		{
		return searchResult;
		}	

	// [ play for 10s (approx entire file) ]
	User::After(10000000); 

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
//
/**
*
* CTestStepGetSetPositionInPlay
*
*/
CTestStepGetSetPositionInPlay::CTestStepGetSetPositionInPlay()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0008-HP");
	}

/**
*
* ~CTestStepGetSetPositionInPlay
*
*/
CTestStepGetSetPositionInPlay::~CTestStepGetSetPositionInPlay()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetPositionInPlay::DoTestStepL( void )
	{
	 __MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Play Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks set/get position whilst playing");
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
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}

	 User::LeaveIfError( iController.Play() );

	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	searchResult = SearchForEvent( playEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}	

	//[ ok lets set/get the position ]
	TTimeIntervalMicroSeconds theStartPosition(0);
	TInt posSetError = iController.SetPosition( theStartPosition );
	
	// [get the position ]
	TTimeIntervalMicroSeconds theCurrentPosition;
	TInt posGetError = iController.GetPosition( theCurrentPosition );

	// [ the test has passed if we have no errors in both cases ]
	if( (posGetError != KErrNone ) &&
		(posSetError != KErrNone ) )
		{
		result = EFail ;
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
*
* CTestStepGetSetPositionInStopped
*
*/
CTestStepGetSetPositionInStopped::CTestStepGetSetPositionInStopped()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0009-HP");
	}

/**
*
* ~CTestStepGetSetPositionInStopped
*/
CTestStepGetSetPositionInStopped::~CTestStepGetSetPositionInStopped()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetPositionInStopped::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;
	//[ In this state it should not be possible to set the positio]

	_LIT( KTestPlay, "Set/Get Position while stopped");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks get/set position while stopped");
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

    //[ set the position to the start ]
	TTimeIntervalMicroSeconds theStartPosition(0);
	TInt posSetError = iController.SetPosition( theStartPosition );
	
	// [get the position ]
	TTimeIntervalMicroSeconds theCurrentPosition;
	TInt posGetError = iController.GetPosition( theCurrentPosition );

	// [ the test has passed if we have errors in both cases ]
	if( (posGetError == KErrNone ) ||
		(posSetError == KErrNone ) )
		{
		result = EFail ;
		}

	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  
	
	__MM_HEAP_MARKEND;
	return result ;
	}

/**
*
* CTestStepGetSetPositionInPrimed
*/
CTestStepGetSetPositionInPrimed::CTestStepGetSetPositionInPrimed()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0010-HP");
	}

/**
*
* ~CTestStepGetSetPositionInStopped
*/
CTestStepGetSetPositionInPrimed::~CTestStepGetSetPositionInPrimed()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetPositionInPrimed::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;

	_LIT( KTestStepPlay, "GetSet Position In Primed");
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

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict searchResult = SearchForEvent( primeEvent );
	if( searchResult == EFail )
		{
		return searchResult;
		}

	// [get the duration of the file]
	TTimeIntervalMicroSeconds theDuration;
	TInt durationError = iController.GetDuration(theDuration);
	User::LeaveIfError( durationError );

	//[ set the position to the end ]
	TInt posError = iController.SetPosition( theDuration );
	User::LeaveIfError( posError );

	// [get the position ]
	TTimeIntervalMicroSeconds theEndPosition;
	TInt positionError = iController.GetPosition( theEndPosition );
	User::LeaveIfError( positionError );
	  
	// [ set the position beyond the end ]
    TInt64 beyondTheEnd = theDuration.Int64();
	beyondTheEnd *= 1000000;
	TTimeIntervalMicroSeconds beyondTheEndPosition( beyondTheEnd);

	// [get the position ]
	TTimeIntervalMicroSeconds theCurrentPosition;
	TInt currentPositionError = iController.GetPosition( theCurrentPosition );
	User::LeaveIfError( currentPositionError );

	//[ set the position beyond the end ]
	TInt beyondEndError = iController.SetPosition( beyondTheEndPosition );

	//[ get the position ]
	TTimeIntervalMicroSeconds theCurrentPosition2;
	currentPositionError = iController.GetPosition( theCurrentPosition2 );
	User::LeaveIfError( currentPositionError );
     
	if( (theCurrentPosition2 != theCurrentPosition ) && 
		( beyondEndError == KErrNone ))
		{
		// altered position when wrong position was sent
		// assumes this violation does not corrupt the current position
		return EFail;
		}
   
	// [ set the position before the start ]
	currentPositionError = iController.GetPosition( theCurrentPosition );
	User::LeaveIfError( currentPositionError );

	//[ set the position to a time < 0 ]
	TTimeIntervalMicroSeconds beforeTheStart( -10000 );
	//[ set the position beyond the end ]
	TInt beforeStartError = iController.SetPosition( beforeTheStart );

	//[ get the position ]
	currentPositionError = iController.GetPosition( theCurrentPosition2 );
	User::LeaveIfError( currentPositionError );

	// [determine the results of the test]
	if( (theCurrentPosition2 != theCurrentPosition ) && 
		( beforeStartError == KErrNone ))
		{
		// altered position when wrong position was sent
		// assumes this violation does not corrupt the current position
		return EFail;
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
* CTestStepGetSetNumChannelsOnSource
*
*/
CTestStepGetSetNumChannelsOnSource::CTestStepGetSetNumChannelsOnSource()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0021-HP");
	}

/**
*
* ~CTestStepGetSetNumChannelsOnSource
*
*/
CTestStepGetSetNumChannelsOnSource::~CTestStepGetSetNumChannelsOnSource()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetNumChannelsOnSource::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Num Channels on the Source");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get num channels on the source");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	// [ attempt to get the source num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	TUint theRate = 0;
	errorCode = theCommander.GetSourceNumChannels(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	theRate = 0;
	errorCode = theCommander.GetSourceNumChannels(theRate);
	User::LeaveIfError( errorCode );

	//[ get the format ]
	TUid theFormatUID;
	errorCode = theCommander.GetSourceFormat(theFormatUID);
	User::LeaveIfError( errorCode );

	//[ ensure we are in are not a raw format because 
	//  this functionality is only supported in raw format]
	if( theFormatUID == KFormatRAWRead )
		{
		result = EFail;
		return result;
		}
	
	// lets set the num of channels to 2
	errorCode = theCommander.SetSourceNumChannels(theRate);
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  
	
	__MM_HEAP_MARKEND;
	return EPass;
	}

/**
*
* CTestStepGetSetNumChannelsOnSink
*
*/
CTestStepGetSetNumChannelsOnSink::CTestStepGetSetNumChannelsOnSink()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0022-HP");
	}

/**
*
* ~CTestStepGetSetNumChannelsOnSink
*
*/
CTestStepGetSetNumChannelsOnSink::~CTestStepGetSetNumChannelsOnSink()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetNumChannelsOnSink::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Num Channels on the Sink");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get num channels on the sink");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	TUint theRate = 0;
	errorCode = theCommander.GetSinkNumChannels(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, ReadFileConfig());
	User::LeaveIfError( errorCode );

	//[ ok lets try to set the sink format to raw]
	errorCode = theCommander.SetSinkFormat(KFormatRAWWrite);
	User::LeaveIfError( errorCode );

	//[ ok lets set the number of channels ]
	// lets set the num of channels to 2
	TUint theNumChannels = 2;
	errorCode = theCommander.SetSinkNumChannels(theNumChannels);
	User::LeaveIfError( errorCode );

	// [ get the number of channels and see if it is the same ]
	TUint theNewNumChannels = 0;
	errorCode = theCommander.GetSinkNumChannels(theNewNumChannels);
	User::LeaveIfError( errorCode );

	if( theNumChannels != theNewNumChannels )
		{
		result = EFail;
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
* CTestStepGetSetSinkFormat
*
*/
CTestStepGetSetSinkFormat::CTestStepGetSetSinkFormat()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0023-HP");
	}

/**
*
* ~CTestStepGetSetSinkFormat
*
*/
CTestStepGetSetSinkFormat::~CTestStepGetSetSinkFormat()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetSinkFormat::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;

	_LIT( KTestSinkFormat, "Test Step Get/Set sink format");
	Log( KTestSinkFormat );
	
	_LIT( KTestStepSinkFormat, "This test checks the set/get sink format");
	Log( KTestStepSinkFormat );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "TempW.wav");
	SetWriteFileNameL( KTestWavFileRecord );
 	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

    RMMFAudioControllerCustomCommands theCommander( iController );


	//[ add sink format when no sink exists ]
	errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
	if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );

    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );
	
    errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
    User::LeaveIfError( errorCode );

	// [ add the format where the uid == the uid of the format ]
    errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
    User::LeaveIfError( errorCode );

	//[ ok lets try to set the sink format to au]
    errorCode = theCommander.SetSinkFormat( KFormatAUWrite );
    User::LeaveIfError( errorCode );

    errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
    User::LeaveIfError( errorCode );

	// [ wait for and process the return event from the audio output ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

    errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
    if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }
	
	//[ lets do some recording ]
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	errorCode =	iController.Play();
    User::LeaveIfError( errorCode );
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ check that we cannot set the sink format when 
	// no data sink has been added to the controller ]
   errorCode = theCommander.SetSinkFormat( KFormatWAVWrite );
   if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }

	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 1000000 ; // arbitrary delay
	myTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

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
* CTestStepGetSetSourceFormat
*
*/
CTestStepGetSetSourceFormat::CTestStepGetSetSourceFormat()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0024-HP");
	}

/**
*
* CTestStepGetSetSourceFormat
*
*/
CTestStepGetSetSourceFormat::~CTestStepGetSetSourceFormat()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetSourceFormat::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "Test Step GetSet Source Format");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get source format");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

    RMMFAudioControllerCustomCommands theCommander( iController );

	//[ add source format when no source exists ]
	errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
	if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	//errorCode = theCommander.SetSourceFormat( KWavUid );
	errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
	User::LeaveIfError( errorCode );

	// [ add the format where the uid == the uid of the format ]
	errorCode = theCommander.SetSourceFormat(  KFormatWAVRead );
	User::LeaveIfError( errorCode );

	//[ ok lets try to set the source format to raw]
	errorCode = theCommander.SetSourceFormat(KFormatRAWRead);
	User::LeaveIfError( errorCode );

	// [ add the format where the uid == the uid of the format ]
	errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
	User::LeaveIfError( errorCode );


	// [ wait for and process the return event from the audio output ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ check that we cannot set the sink format when 
	// no data sink has been added to the controller ]
   errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
   if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }
	
	// [ lets do some recording ]
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	// this is the raison detre of the test ]
	errorCode =	iController.Play();
    User::LeaveIfError( errorCode );
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}


	// [ check that we cannot set the sink format when 
	// no data sink has been added to the controller ]
   errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
   if( errorCode == KErrNone )
	   {
	   result = EFail;
	   return result;
	   }

	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 1000000 ; // arbitrary delay
	myTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

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
* CTestStepGetSetSourceSampleRate
*
*/
CTestStepGetSetSourceSampleRate::CTestStepGetSetSourceSampleRate()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0025-HP");
	}

/**
*
* ~CTestStepGetSetSourceSampleRate
*
*/
CTestStepGetSetSourceSampleRate::~CTestStepGetSetSourceSampleRate()
	{
	}

/**
*
* CTestStepGetSetSourceSampleRate
*
*/
TVerdict  CTestStepGetSetSourceSampleRate::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Source Sample Rate");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get source sample rate");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	RArray<TUint> supportedRates;
	CleanupClosePushL(supportedRates);
	TRAPD(errorCode2, theCommander.GetSupportedSourceSampleRatesL(supportedRates));
	if ((errorCode2 == KErrNone ) || supportedRates.Count())
		{
		// should have failed with either an error code or non zero supported rates
		result = EFail;
		return result;
		}

	TUint theRate = 0;
	errorCode = theCommander.GetSourceSampleRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	// Store the original sample rate
	TUint origRate = 0;
	errorCode = theCommander.GetSourceSampleRate(origRate);
	if (errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting original source sample rate %d"), errorCode);
		return EFail;
		}

	// Ensure the array is empty
	supportedRates.Reset();
	TRAPD( errorCode3,theCommander.GetSupportedSourceSampleRatesL(supportedRates));
	User::LeaveIfError( errorCode3 );

	TUint theNewRate = 0;

	for (TInt index = 0; index < supportedRates.Count(); index++)
		{
		theRate = supportedRates[index];
		// SetSourceSampleRate
		errorCode = theCommander.SetSourceSampleRate(theRate);
		if (theRate == origRate)
			{
			if (errorCode != KErrNone)
				{
				INFO_PRINTF2(_L("SetSourceSampleRate returned unexpected error %d"), errorCode);
				result = EFail;
				break;
				}
			}
		else
			{
			// Setting source sample rate to a different value 
			// is tolerated (INC038043) to preserve BC with 6.1
			if (errorCode != KErrNone)
				{
				INFO_PRINTF3(_L("SetSourceDataType returned %d, expecting %d"), errorCode, KErrAlreadyExists);
				result = EFail;
				break;
				}
			}
		// GetSourceSampleRate
		errorCode = theCommander.GetSourceSampleRate(theNewRate);
		if (theNewRate != origRate)
			{
			INFO_PRINTF3(_L("GetSourceSampleRate rate %d did not match expected %d"), theNewRate, origRate);
			result = EFail;
			break;
			}

		if (errorCode != KErrNone)
			{
			INFO_PRINTF3(_L("GetSourceSampleRate returned unexpected error %d, expected %d"), errorCode, KErrNone);
			result = EFail;
			break;
			}
		}

	CleanupStack::PopAndDestroy(1);		// supportedRates
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  
	
	__MM_HEAP_MARKEND;
	return result ;	
	}

/**
*
* CTestStepGetSetSinkSampleRate
*
*/
CTestStepGetSetSinkSampleRate::CTestStepGetSetSinkSampleRate()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0026-HP");
	}

/**
*
* ~CTestStepGetSetSinkSampleRate
*
*/
CTestStepGetSetSinkSampleRate::~CTestStepGetSetSinkSampleRate()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict  CTestStepGetSetSinkSampleRate::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Sink Sample Rate");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get sink sample rate");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );
	
	TUint theRate = 0;
	errorCode = theCommander.GetSinkSampleRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		}

	//[ ok lets add the source and sink ]
    if(result==EPass)
    	{
    	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
    
    if(result==EPass)
    	{
		errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
	
	if(result==EPass)
		{
		theRate = 0;
		errorCode = theCommander.GetSinkSampleRate(theRate);
	    if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetSinkSampleRate returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
		}
	
	if(result==EPass)
		{
		//[ set the sample rate ]
		errorCode = theCommander.SetSinkSampleRate(theRate);
		if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("SetSinkSampleRate returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
		}
	
	if(result==EPass)
		{
		TUint theSampleRate( 8000); // set sample rate to 8khz arbitrary value
		//[ set the sample rate ]
		theRate = theSampleRate; // known reasonable value
		errorCode = theCommander.SetSinkSampleRate(theRate);
		if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("SetSinkSampleRate returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
		}
	
	if(result==EPass)
		{
		theRate = 0;
		errorCode = theCommander.GetSinkSampleRate(theRate);
	    if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetSinkSampleRate returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
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
* CTestStepGetSetSourceBitRate
*
*/
CTestStepGetSetSourceBitRate::CTestStepGetSetSourceBitRate()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0027-HP");
	}

/**
*
* ~CTestStepGetSetSourceBitRate
*
*/
CTestStepGetSetSourceBitRate::~CTestStepGetSetSourceBitRate()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetSourceBitRate::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Source Bit Rate");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get source bit rate");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	TUint theRate = 0;
	errorCode = theCommander.GetSourceBitRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	theRate = 0;
	errorCode = theCommander.GetSourceBitRate(theRate);
    User::LeaveIfError( errorCode );

	// check to see if the bitrate is greater than zero
	// (it should exactly be samplerate*bitspersample)
	if (theRate == 0)
		{
		result = EFail;
		return result;
		}

	//[ this is currently not supported ]
	errorCode = theCommander.SetSourceBitRate(theRate);
	if( errorCode == KErrNone )
	{
	result = EFail;
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
* CTestStepGetSetSourceDataType
*
*/
CTestStepGetSetSourceDataType::CTestStepGetSetSourceDataType()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0028-HP");
	}

/**
*
* ~CTestStepGetSetSourceDataType
*
*/
CTestStepGetSetSourceDataType::~CTestStepGetSetSourceDataType()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict  CTestStepGetSetSourceDataType::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Source Data type");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get source data type");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	TFourCC theFCC;
	errorCode = theCommander.GetSourceDataType( theFCC );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	errorCode = theCommander.GetSourceDataType( theFCC );
	User::LeaveIfError( errorCode );

	TFourCC theNewFCC;
	errorCode = theCommander.SetSourceDataType( theFCC );
	User::LeaveIfError( errorCode );

	errorCode = theCommander.GetSourceDataType( theNewFCC );
	User::LeaveIfError( errorCode );

	if( theNewFCC != theFCC )
		{
		result = EFail;
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
* CTestStepGetSetSinkDataType
*
*/
CTestStepGetSetSinkDataType::CTestStepGetSetSinkDataType()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0029-HP");
	}

/**
*
* ~CTestStepGetSetSinkDataType
*
*/
CTestStepGetSetSinkDataType::~CTestStepGetSetSinkDataType()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSetSinkDataType::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSet Sink Data type");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the set/get sink data type");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );
	
	TFourCC theFCC;
	errorCode = theCommander.GetSinkDataType( theFCC );
	if( errorCode == KErrNone )
		{
		result = EFail;
		}

	//[ ok lets add the source and sink ]
    if(result==EPass)
    	{
    	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
    
    if(result==EPass)
    	{
		errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
    	
	if(result==EPass)
		{
		errorCode = theCommander.GetSinkDataType( theFCC );
		if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetSinkDataType returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
		}
	
	if(result==EPass)
		{
		errorCode = theCommander.SetSinkDataType( theFCC );
		if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("SetSinkDataType returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
		}
	
	if(result==EPass)
		{
		TFourCC theNewFCC;
		errorCode = theCommander.GetSinkDataType( theNewFCC );
		if(errorCode!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetSinkDataType returned %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
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
* CTestStepGetSetSinkBitRate
*
*/

CTestStepGetSetSinkBitRate::CTestStepGetSetSinkBitRate()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0030-HP");
	}

/**
*
* ~CTestStepGetSetSinkDataType
*
*/

CTestStepGetSetSinkBitRate::~CTestStepGetSetSinkBitRate()
	{
	}

/**
*
* DoTestStepL
*
*/

TVerdict CTestStepGetSetSinkBitRate::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSetSinkBitRate");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks setting and getting a sinks bit rate");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	// [ attempt to get the bit rate with out a sink]
    RMMFAudioControllerCustomCommands theCommander( iController );

	TUint theRate = 0;
	errorCode = theCommander.GetSinkBitRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	errorCode = theCommander.GetSinkBitRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );

	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ check when we are primed ]
	errorCode = theCommander.GetSinkBitRate(theRate);
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}
   
	// [ lets reset and add a data sink and source that will
	//   exercise the happy day scenario ]
	errorCode =	iController.Reset();
	User::LeaveIfError( errorCode );

	// [ check when recording ]
	_LIT(KTestWavFileRecord, "16bitmpcm.wav");
	
   	SetReadFileNameL( KTestWavFileRecord );
   	// check if record file already exists
   	TEntry fileEntry;
   	TInt fileErr = iFs.Entry(ReadFileConfig()().iPath, fileEntry);
   	if (fileErr != KErrNotFound)
   		{
   		User::LeaveIfError(fileErr);
   		}

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	//[lets get the sink bit rate]
	errorCode = theCommander.GetSinkBitRate(theRate);
	User::LeaveIfError( errorCode );

	// check to see if the bitrate is greater than zero
	// (it should exactly be samplerate*bitspersample)
	// fail if the bitrate returned is zero and the record file exists
	if ((theRate == 0) && (fileErr != KErrNotFound))
		{
		result = EFail;
		return result;
		}

	const TUint KArbitraryBitRate = 1000; // 1 Khz
	//[lets get the sink bit rate]
	errorCode = theCommander.SetSinkBitRate( KArbitraryBitRate );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );

	// [ wait for and process the return event from the audio output ]
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

    //[ get the bit rate while playing ]
	User::LeaveIfError( iController.Play() );
	TMMFEvent playEvent( KPlayTestId, KErrNone );

    result = SearchForEvent( playEvent );
	if( result == EFail )
		{
		return result;
		}

	//[lets get the sink bit rate]
	errorCode = theCommander.GetSinkBitRate(theRate);
	User::LeaveIfError( errorCode );

	// check to see if the bitrate is greater than zero
	// (it should exactly be samplerate*bitspersample)
	if (theRate == 0)
		{
		result = EFail;
		return result;
		}

    //[lets get the sink bit rate]
	errorCode = theCommander.SetSinkBitRate(1000);
	if( errorCode == KErrNone )
		{
		result = EFail;
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
	   
	// test steps return a result
	return result;
	}

/**
*
* CTestStepGetSetMetaData
*/
CTestStepGetSetMetaData::CTestStepGetSetMetaData()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0031-HP");
	}

/**
*
* ~CTestStepGetSetMetaData
*/
CTestStepGetSetMetaData::~CTestStepGetSetMetaData()
	{
	}

/**
*
* DoTestStepL
*/
TVerdict CTestStepGetSetMetaData::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EFail;
	
	_LIT( KTestPlay, "Meta data test with play scenario Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPlay, "This test checks setting/getting meta data");
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

	// [ lets get the number of meta data entries 
	// for this format the meta data is unsupported ]
	TInt numEntries = 0;
	errorCode = iController.GetNumberOfMetaDataEntries( numEntries ); 
	if( errorCode != KErrNotSupported )
		{
		User::LeaveIfError( errorCode );
		}

	// [ lets get a meta data entry ]
	const TInt KIndex = 0;
	TRAP( errorCode, iController.GetMetaDataEntryL( KIndex ));
	if( errorCode != KErrNotSupported )
		{
		User::LeaveIfError( errorCode );
		}
		
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}
	
	// [ lets get the number of meta data entries 
	// for this format the meta data is unsupported ]
	numEntries = 0;
	errorCode = iController.GetNumberOfMetaDataEntries( numEntries ); 
	if( errorCode != KErrNotSupported )
		{
		User::LeaveIfError( errorCode );
		}

	// [ lets get a meta data entry ]
	TRAP( errorCode, iController.GetMetaDataEntryL( KIndex ));
	if( errorCode != KErrNotSupported )
		{
		User::LeaveIfError( errorCode );
		}
	
    User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	result = SearchForEvent( playEvent );

	// [ lets try to get the meta data when playing ]

	errorCode = iController.GetNumberOfMetaDataEntries( numEntries ); 
	if( errorCode != KErrNotReady )
		{
		User::LeaveIfError( errorCode );
		}

	// [ lets get a meta data entry ]
	// This kills the thread and is under investigation
	TRAP( errorCode, iController.GetMetaDataEntryL( KIndex ));
	if( errorCode != KErrNotReady )
		{
		User::LeaveIfError( errorCode );
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
	return EPass;
	}

/**
*
* CTestStepGetSupportedSinkDataTypes
*
*/
CTestStepGetSupportedSinkDataTypes::CTestStepGetSupportedSinkDataTypes()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0034-HP");
	}

/**
*
* ~CTestStepGetSupportedSinkDataTypes
*
*/
CTestStepGetSupportedSinkDataTypes::~CTestStepGetSupportedSinkDataTypes()
	{
	}

/**
*
* DoTestStepL
* This test gets the supported sink data types
*
*/
TVerdict CTestStepGetSupportedSinkDataTypes::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	_LIT( KTestSourceNumChannels, "GetSupported Sink Data types");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the get supported sink data types method");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	RArray<TFourCC> supportedDataTypes;
	CleanupClosePushL(supportedDataTypes);

	TRAPD( errorCode2, theCommander.GetSupportedSinkDataTypesL( supportedDataTypes ));
	if( ( errorCode2 == KErrNone ) || supportedDataTypes.Count() )
		{
		//[ should have failed with either an error code or non zero supported types ]
		result = EFail;
		}

	//[ ok lets add the source and sink ]
    if(result==EPass)
    	{
    	errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
    
    if(result==EPass)
    	{
		errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    	if(errorCode!=KErrNone)
    		{
    		result=EFail;
    		}
    	}
    
    if(result==EPass)
    	{
    	//[ ensure the array is empty ]
		supportedDataTypes.Reset();
		TRAPD( errorCode3,theCommander.GetSupportedSinkDataTypesL( supportedDataTypes ));
		if(errorCode3!=KErrNotSupported)
	    	{
	    	INFO_PRINTF2(_L("GetSupportedSinkDataTypesL left with %d when KErrNotSupported is expected"), errorCode);
	    	result = EFail;
	    	}
    	}
	

	//[ pop supportedDataTypes ]
	CleanupStack::PopAndDestroy(1);//supportedDataTypes
	
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
* CTestStepGetSupportedSourceDataTypes
*
*/
CTestStepGetSupportedSourceDataTypes::CTestStepGetSupportedSourceDataTypes()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0035-HP");
	}

/**
*
* ~CTestStepGetSupportedSourceDataTypes
*
*/
CTestStepGetSupportedSourceDataTypes::~CTestStepGetSupportedSourceDataTypes()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepGetSupportedSourceDataTypes::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSupported Source Data types");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the get supported source data types method");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );
	
	RArray<TFourCC> supportedDataTypes;
	CleanupClosePushL(supportedDataTypes);
	
	TRAPD( errorCode2, theCommander.GetSupportedSourceDataTypesL( supportedDataTypes ));
	if( ( errorCode2 == KErrNone ) || supportedDataTypes.Count() )
		{
		//[ should have failed with either an error code or non zero supported types ]
		result = EFail;
		return result;
		}
	
	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	//[ ensure the array is empty ]
	supportedDataTypes.Reset();
	TRAPD( errorCode3,theCommander.GetSupportedSourceDataTypesL( supportedDataTypes ));
	User::LeaveIfError( errorCode3 );
	
	// Store the original FCC
	TFourCC origFCC;
	errorCode = theCommander.GetSourceDataType(origFCC);
	if (errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("GetSourceDataType returned error %d"), errorCode);
		return EFail;
		}

	TFourCC theNewFCC;
	TFourCC theFCC;
	
	//[ loop over the possible set of data types that are supported ]
	for( TInt index = 0; index < supportedDataTypes.Count(); index++ )
		{
		theFCC = supportedDataTypes[ index];
		errorCode = theCommander.SetSourceDataType( theFCC );

		if (theFCC == origFCC)
			{
			// Trying to set the correct FCC
			if (errorCode != KErrNone)
				{
				INFO_PRINTF2(_L("SetSourceDataType returned unexpected error %d"), errorCode);
				result = EFail;
				break;
				}
			}
		else
			{
			if (errorCode != KErrAlreadyExists)
				{
				INFO_PRINTF3(_L("SetSourceDataType returned %d, expecting %d"), errorCode, KErrAlreadyExists);
				result = EFail;
				break;
				}
			}

		errorCode = theCommander.GetSourceDataType( theNewFCC );
		if (theNewFCC != origFCC)
			{
			INFO_PRINTF1(_L("GetSourceDataType returned incorrect FCC"));
			result = EFail;
			break;
			}

		if (errorCode != KErrNone)
			{
			INFO_PRINTF2(_L("GetSourceDataType returned unexpected error %d"), errorCode);
			result = EFail;
			break;
			}
		}
	
	//[ pop supportedDataTypes ]
	CleanupStack::PopAndDestroy(1);//supportedDataTypes
	
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
* CTestStepGetSupportedSourceNumChannels
* 
*/
CTestStepGetSupportedSourceNumChannels::CTestStepGetSupportedSourceNumChannels()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0036-HP");
	}

/**
*
* ~CTestStepGetSupportedSourceNumChannels
*
*/
CTestStepGetSupportedSourceNumChannels::~CTestStepGetSupportedSourceNumChannels()
	{
	}

/**
*
* DoTestStepL
* This test  tests the number of channels supported by a source
*
*/
TVerdict CTestStepGetSupportedSourceNumChannels::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass;
	
	_LIT( KTestSourceNumChannels, "GetSupported Source Num Channels");
	Log( KTestSourceNumChannels);
	
	_LIT( KTestSinkBitRateReason, "This test checks the get supported source channels");
	Log( KTestSinkBitRateReason );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );
			
	RArray<TUint> supportedChannels;
	CleanupClosePushL(supportedChannels);

	TRAPD( errorCode2, theCommander.GetSupportedSourceNumChannelsL( supportedChannels));
	if( errorCode2 == KErrNone ) 
		{
		//[ should have failed with either an error code or non zero supported types ]
		result = EFail;
		return result;
		}
	
	//[ ok lets add the source and sink ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );

	//[ ensure the array is empty ]
	supportedChannels.Reset();
	TRAPD( errorCode3, theCommander.GetSupportedSourceNumChannelsL( supportedChannels));	
	User::LeaveIfError( errorCode3 );
	
	TUint orgNumChannels = 0;
	errorCode = theCommander.GetSourceNumChannels( orgNumChannels);
	User::LeaveIfError( errorCode );

	//[ loop over the possible set of data types that are supported ]
	for( TInt index = 0; index < supportedChannels.Count(); index++ )
		{
		TUint oldNumChannels = supportedChannels[ index];
		TUint numChannels    = 0;
		//[ wav files currently do not support a variable num source channels]
		errorCode = theCommander.SetSourceNumChannels( oldNumChannels);
		if (oldNumChannels == orgNumChannels)
			{
			// We tried to set the right number of channels for clip
			if (errorCode != KErrNone)
				{
				INFO_PRINTF2(_L("Error calling SetSourceNumChannels %d"), errorCode);
				result = EFail;
				break;
				}
			}
		else
			{
			// Setting source sample rate to a different value 
			// is tolerated (INC038043) to preserve BC with 6.1
			if (errorCode != KErrNone)
				{
				INFO_PRINTF2(_L("Unexpected error code returned by SetSourceNumChannels %d"), errorCode);
				result = EFail;
				break;
				}
			}
		
		//[ check the set operation resulted in no change of the
		// number of channels ]
		errorCode = theCommander.GetSourceNumChannels( numChannels);
		User::LeaveIfError( errorCode );
		if( orgNumChannels != numChannels )
			{
			INFO_PRINTF3(_L("GetSourceNumChannels returned %d, expecting %d"), numChannels, orgNumChannels);
			result = EFail;
			break;
			}
		}

	//[ pop supportedChannels ]
	CleanupStack::PopAndDestroy(1);//supportedChannels
	
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
