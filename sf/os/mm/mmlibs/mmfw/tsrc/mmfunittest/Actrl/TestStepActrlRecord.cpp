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
//

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

#include "ActrlTestUids.h"

//const TInt KPlayDelayNewMail = 100000000; // arbitrary delay	// EABI warning removal
const TUint KSampleRate32000Hz  = 32000;
const TUint KSampleRate16000Hz  = 16000;
//const TUint KSampleRate8000Hz   = 8000;	// EABI warning removal
const TUint KMono	= 1;

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"

// Specific includes for these test steps
#include "TestStepActrlRecord.h"

//[ record test data structure ]
// parameter structure for conversion tests
class TRecParameters
	{	
public:
	const TText* iTestName;
	TUid	iDestFormat;
	TUint32 iDestEncoding;
	TUint	iDestSampleRate;
	TUint	iDestChannels;
	const TText* iOutFilename;
	TInt	iExpectedResult;
	};

// constant table of parameters for tests
const TRecParameters KTestParameters[] =
{	
	{ 
	_S("MM-MMF-ACTRL-U-0056-HP"), 
	{KMmfUidFormatWAVWrite},	
	KMMFFourCCCodeIMAD, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzImadRecording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0057-HP"), 
	{KMmfUidFormatWAVWrite},	
	KMMFFourCCCodePCM16, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzPcm16Recording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0058-HP"), 
	{KMmfUidFormatAUWrite},	
	KMMFFourCCCodeALAW, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzALawRecording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0059-HP"), 
	{KMmfUidFormatAUWrite},	
	KMMFFourCCCodePCM16B, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzPcm16BRecording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0060-HP"), 
	{KMmfUidFormatRAWWrite},	
	KMMFFourCCCodeIMAD, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzImadRecording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0061-HP"), 
	{KMmfUidFormatRAWWrite},	
	KMMFFourCCCodePCM16, 
	KSampleRate32000Hz, 
	KMono, 
	_S("32KhzPcm16Recording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0062-HP"), 
	{KMmfUidFormatRAWWrite},	
	KMMFFourCCCodePCM16, 
	KSampleRate16000Hz, 
	KMono, 
	_S("16KhzPcm16Recording"),
	KErrNone 
	},

	{ 
	_S("MM-MMF-ACTRL-U-0063-HP"), 
	{KMmfUidFormatWAVWrite},	
	KMMFFourCCCodeIMAD, 
	KSampleRate16000Hz, 
	KMono, 
	_S("16KhzImadRecording"),
	KErrNone 
	},

};


/**
*
* CTestStepActrlRecord
*
*/
CTestStepActrlRecord::CTestStepActrlRecord()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0013-HP");
	}

/**
*
* CTestStepActrlRecord
*
*/
CTestStepActrlRecord::~CTestStepActrlRecord()
	{
	}

/**
*
* DoTestStepL
* This test records audio to a file
*
*/
TVerdict CTestStepActrlRecord::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Record Test");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "This test checks recording an audio clip");
	Log( KTestStepRecord );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "16bitmpcm.wav");
   	
	SetWriteFileNameL( KTestWavFileRecord );

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
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
	
	//[ lets do some recording ]
    User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	
	result = SearchForEvent( playEvent );

	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 1000000 ;
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
	   
	// test steps return a result
	return result;
	}

/***
*
* CTestStepActrlSetGetRecordGain
*
*/
CTestStepSetGetRecordGain::CTestStepSetGetRecordGain()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0014-HP");
	}

/***
*
* CTestStepActrlSetGetRecordGain
*
*/
CTestStepSetGetRecordGain::~CTestStepSetGetRecordGain()
	{
	}

/***
*
* DoTestStepL
* This tests the record gain with set get max
*
*/
TVerdict  CTestStepSetGetRecordGain::DoTestStepL( void )
	{
	TVerdict result = EFail ;

	_LIT( KTestPlay, "Set Get Record Gain Test");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "This test checks set/get record gain");
	Log( KTestStepRecord );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "16bitmpcm.wav");

	SetWriteFileNameL( KTestWavFileRecord );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );
	
	//[ create a custom commander for record ]
	RMMFAudioRecordDeviceCustomCommands theCommander( iController  );

	TInt maxGain = 0;
	errorCode = theCommander.GetMaxGain( maxGain );
	User::LeaveIfError( errorCode );

    //[ now try setting the gain to maxGain/2  ]
	errorCode = theCommander.SetGain( maxGain/2 );
	User::LeaveIfError( errorCode );

	TInt theGain = 0;
	errorCode = theCommander.GetGain( theGain );
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

	// [ we are now primed ]
    
	//[ now try setting the gain to maxGain ]
	TInt primeGain            = -1; // arbitrary value since we are getting the gain
	TInt primeMaxGain         = -1; // arbitrary value since we are getting the gain
	TInt primeSetErrorCode    = theCommander.SetGain( maxGain );
	
	errorCode                 = theCommander.GetGain( primeGain );
	User::LeaveIfError( errorCode );
	
	errorCode = theCommander.GetMaxGain( primeMaxGain );
	User::LeaveIfError( errorCode );

	//[ proceed to playing and check the error codes on the set/get]
	//[ lets do some recording ]
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

	TInt playGain    = 0; // arbitrary initial value
	TInt playMaxGain = 0; // arbitrary initial value
	//[ ok lets try seting and getting in play state ]
	TInt playSetErrorCode    = theCommander.SetGain( maxGain );
	errorCode    = theCommander.GetGain( playGain );
	User::LeaveIfError( errorCode );
	errorCode = theCommander.GetGain( playMaxGain );
	User::LeaveIfError( errorCode );

	//[ set up the test result ]
	if( (playSetErrorCode    != KErrNone ) &&
		(primeSetErrorCode   != KErrNone ) &&
		(primeGain == maxGain/2) )
		{
		result = EPass;
		}
	else
		result = EFail;
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result;
	}

/***
*
* CTestStepSetGetRecordBalance
*
*/
CTestStepSetGetRecordBalance::CTestStepSetGetRecordBalance()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0015-HP");
	}

/*
*
* ~CTestStepSetGetRecordBalance
*
*/
CTestStepSetGetRecordBalance::~CTestStepSetGetRecordBalance()
	{
	}

/*
*
* DoTestStepL
* This teststep tests get/set record balance
*
*/
TVerdict  CTestStepSetGetRecordBalance::DoTestStepL( void )
	{
	TVerdict result = EFail ;

	_LIT( KTestPlay, "Set Get Record balance Test");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "This test checks set/get record balance");
	Log( KTestStepRecord );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "16bitmpcm.wav");

	SetWriteFileNameL( KTestWavFileRecord );
   	
	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );
	
	//[ create a custom commander for record ]
	RMMFAudioRecordDeviceCustomCommands theCommander( iController  );

	const TInt KAnyOldStoppedBalance = 10;

	//[ set get record balance in stopped ]
	TInt theBalance = KAnyOldStoppedBalance;
	errorCode = theCommander.SetBalance( theBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theNewBalance = 0; 
	errorCode = theCommander.GetBalance( theNewBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ set get lower && upper bound ]

	//[ set get record balance in stopped ]
	TInt theUpperBound = KMMFBalanceMaxRight;
	errorCode = theCommander.SetBalance( theUpperBound );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theNewUpperBound = 0; 
	errorCode = theCommander.GetBalance( theNewUpperBound );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theLowerBound = KMMFBalanceMaxLeft;
	errorCode = theCommander.SetBalance( theLowerBound );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theNewLowerBound = 0; 
	errorCode = theCommander.GetBalance( theNewLowerBound );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ set get lower bound -1 && upper bound +1 ]
	TInt theUpperBoundPlusOne = KMMFBalanceMaxRight +1;
	errorCode = theCommander.SetBalance( theUpperBoundPlusOne );
	if( errorCode != KErrArgument )
		{
		result = EFail;
		return result;
		}

	TInt theNewUpperBoundPlusOne = 0; 
	errorCode = theCommander.GetBalance( theNewUpperBoundPlusOne );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theLowerBoundMinusOne = KMMFBalanceMaxLeft-1;
	errorCode = theCommander.SetBalance( theLowerBoundMinusOne );
	if( errorCode != KErrArgument )
		{
		result = EFail;
		return result;
		}

	TInt theNewLowerBoundMinusOne = 0; 
	errorCode = theCommander.GetBalance( theNewLowerBoundMinusOne );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ lets prime ]
	// [ wait for and process the return event from the audio output ]
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );

	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	const TInt KAnyOldPrimeBalance = 20;
	TInt thePrimeBalance = KAnyOldPrimeBalance;
	errorCode = theCommander.SetBalance( thePrimeBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theNewPrimeBalance = 0; // arbitrary value which is close to zero
	errorCode = theCommander.GetBalance( theNewPrimeBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ set get record balance in playing ]
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

	const TInt KAnyOldPlayBalance = 100;
	TInt thePlayBalance = KAnyOldPlayBalance;
	errorCode = theCommander.SetBalance( thePlayBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	TInt theNewPlayBalance = 0; // arbitrary value which is close to zero
	errorCode = theCommander.GetBalance( theNewPlayBalance );
	if( errorCode != KErrNone )
		{
		result = EFail;
		return result;
		}

	result = EPass;
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result;
	}

/**
*
* CTestStepRecordMemToFile
*
*/
CTestStepRecordMemToFile::CTestStepRecordMemToFile()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0032-HP");
	iHeapSize = 300 * 1024;		// we are allocating large buffers
	}

/**
*
* ~CTestStepRecordMemToFile
*/
CTestStepRecordMemToFile::~CTestStepRecordMemToFile()
	{
	}

/**
*
* DoTestStepL
*/
TVerdict CTestStepRecordMemToFile::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;

	RFs fs;
	RFile file;
	
   	// [ open and read file into mem buffer ]
	_LIT(  KRecordSourceName, "16bitmpcm.wav" );
	
	//[ set file name ]
	User::LeaveIfError(fs.Connect());
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(KRecordSourceName); // use full name
	User::LeaveIfError(file.Open(fs,fileName,EFileRead));
	CleanupClosePushL(file);
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));
    INFO_PRINTF2(_L("size of file = %d\n"),fileSize);//Statement Changed under DEF105143
	HBufC8* audioBuffer = HBufC8::NewMaxL(fileSize);
	CleanupStack::PushL(audioBuffer);
	
	//[ Read a file into a descriptor ]
	TPtr8 inBufferDes(audioBuffer->Des());
	User::LeaveIfError(file.Read(inBufferDes));
	SetReadDescriptor(&inBufferDes);
	
	_LIT(  KRecordDestName, "16xxxxx.wav" );
	SetWriteFileNameL( KRecordDestName );
	
	//[ add data source and sink ]

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

    errorCode = iController.AddDataSource( KUidMmfDescriptorSource, ReadDescConfig() );
	User::LeaveIfError( errorCode );
	
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );
	
    RMMFAudioControllerCustomCommands theCommander( iController );

	//[ set formats explicitly ]
	errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
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
	
	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 1000000 ; // sufficient delay to write the data
	myTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );
	
	
	// [ pop and destroy file, audioBuffer ]
	CleanupStack::PopAndDestroy(2);

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
* CTestStepRecordMemToMem
*
*/
CTestStepRecordMemToMem::CTestStepRecordMemToMem()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0033-HP");
	iHeapSize = 600 * 1024;		// we are allocating two 213K buffers + other stuff
	}

/**
*
* ~CTestStepRecordNewFileMem
*
*/
CTestStepRecordMemToMem::~CTestStepRecordMemToMem()
	{
	}

/**
*
* DoTestStepL
* This test tests recording from mem to mem
*
*/
TVerdict CTestStepRecordMemToMem::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	TVerdict result = EPass ;

	RFs fs;
	RFile file;

   	// [ open and read file into mem buffer ]
	_LIT(  KRecordSourceName, "16bitmpcm.wav" );

    //[ set file name ]
	User::LeaveIfError(fs.Connect());
	TFileName fileName = GetSuite()->DefaultPath();
	fileName.Append(KRecordSourceName); // use full name
	User::LeaveIfError(file.Open(fs,fileName,EFileRead));
	CleanupClosePushL(file);
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));
	INFO_PRINTF2(_L("size of file = %d\n"),fileSize); //Statement Changed under DEF105143
	// [ set up the input mem descriptor ]
	HBufC8* audioInBuffer = HBufC8::NewMaxL(fileSize);
	CleanupStack::PushL(audioInBuffer);
	TPtr8 inBufferDes(audioInBuffer->Des());
	//[ Read a file into the descriptor ]
	User::LeaveIfError(file.Read(inBufferDes));
	SetReadDescriptor(&inBufferDes);


	// [ set up the output mem descriptor ]
	HBufC8* audioOutBuffer = HBufC8::NewMaxL(fileSize);
	CleanupStack::PushL(audioOutBuffer);
	TPtr8 outBufferDes(audioOutBuffer->Des());
	SetWriteDescriptor(&outBufferDes);


	//[ add data source and sink ]

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

    errorCode = iController.AddDataSource( KUidMmfDescriptorSource, ReadDescConfig() );
	User::LeaveIfError( errorCode );

    errorCode = iController.AddDataSink(KUidMmfDescriptorSink, WriteDescConfig() );
	User::LeaveIfError( errorCode );

    RMMFAudioControllerCustomCommands theCommander( iController );

	//[ set formats explicitly ]
	errorCode = theCommander.SetSourceFormat( KFormatWAVRead );
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

	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 1000000 ;
	myTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

	// [ pop and destroy file, audioBuffer, audioOutBuffer ]
	CleanupStack::PopAndDestroy(3);

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
* CTestStepMarcSetMaxFileSize
*/
CTestStepMarcSetMaxFileSize::CTestStepMarcSetMaxFileSize()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0037-HP");
	}

/**
*
* ~CTestStepMarcSetMaxFileSize
*
*/
CTestStepMarcSetMaxFileSize::~CTestStepMarcSetMaxFileSize()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict  CTestStepMarcSetMaxFileSize::DoTestStepL( void )
	{
	TVerdict result = EFail;
	
	_LIT( KTestPlay, "SetMaxFileSize Test while recording");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "This test checks setting the max file size for recording");
	Log( KTestStepRecord );
	
	// [ audio file to record to ]
	_LIT(KTestWavFileRecord, "MaxFileSize16bitmpcm.wav");

	//[ ensure the file does not exist
	// so that we can start with a clean sheet]
	RFs	fs;
	fs.Connect();
	fs.Delete(KTestWavFileRecord);
	fs.Close();
   	
	SetWriteFileNameL( KTestWavFileRecord );

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//< try setting the file size while stopped >
	RMMFAudioRecordControllerCustomCommands theCommander( iController );
    // try setting the file size before primed
	//< expect an error >
    TInt theSize = -1;  // illegals values -1 .. min int  
	errorCode = theCommander.SetMaxFileSize( theSize );
	if( errorCode != KErrNotReady )
		{
		result = EFail;
		return result;
		}

	TMMFEvent setMaxFileSizeErrorEvent( KMarcSetMaxFileSizeId, KErrNotReady);
	result = SearchForEvent( setMaxFileSizeErrorEvent );
	if( result == EFail )
		{
		return result;
		}

    theSize = 0;  // sentinel value ( more magic)
	errorCode = theCommander.SetMaxFileSize( theSize );
	if( errorCode != KErrNotReady )
		{
		result = EFail;
		return result;
		}

	result = SearchForEvent( setMaxFileSizeErrorEvent );
	if( result == EFail )
		{
		return result;
		}

	theSize = 200;  // sentinel value ( more magic)
	errorCode = theCommander.SetMaxFileSize( theSize );
	if( errorCode != KErrNotReady )
		{
		result = EFail;
		return result;
		}

	result = SearchForEvent( setMaxFileSizeErrorEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );

   	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );

	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	// -2 is in the unacceptable range and should fail with Kerror argument
	TMMFEvent setMaxFileSizeArgumentEvent( KMarcSetMaxFileSizeId, KErrArgument);
	theSize = -2;   
	errorCode = theCommander.SetMaxFileSize( theSize );
	if( errorCode != KErrArgument)
		{
		result = EFail;
		return result;
		}
	
	result = SearchForEvent( setMaxFileSizeArgumentEvent );
	if( result == EFail )
		{
		return result;
		}

	theSize = 0;   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );

	TMMFEvent setMaxFileSizeSuccessEvent( KMarcSetMaxFileSizeId, KErrNone);
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ lets do some recording ]
    User::LeaveIfError( iController.Play() );
	
	// note this test does not wait around 
	// to detect that the audio has been played
	// [ now process the confirmation event that play has reached audio output
	//   this is the raison detre of the test ]
	TMMFEvent playEvent( KPlayTestId, KErrNone );
	result = SearchForEvent( playEvent );

	// [ lets wait a few seconds for record to write its data ]
	RTimer  myTimer ;
	User::LeaveIfError( myTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 10000000 ;
	myTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );
	
	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	// as this is only a unit test we will not test the recorded file size
	// and compare with the maximum value

	// test steps return a result
	return result;
	}

/**
*
* CTestStepSetGetRecordTimeAvailable
*
*/

CTestStepSetGetRecordTimeAvailable::CTestStepSetGetRecordTimeAvailable()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0038-HP");
	}

/**
*
* ~CTestStepSetGetRecordTimeAvailable
*
*/

CTestStepSetGetRecordTimeAvailable::~CTestStepSetGetRecordTimeAvailable()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict  CTestStepSetGetRecordTimeAvailable::DoTestStepL( void )
	{
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Record Time Available Test");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "This test tests getting record time available");
	Log( KTestStepRecord );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "TimeAvailable16bitmpcm.wav");
   	
	SetWriteFileNameL( KTestWavFileRecord );

	//[ ensure file does not exist ]
	RFs	fs;
	fs.Connect();
	fs.Delete(KTestWavFileRecord);
	fs.Close();

	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
	User::LeaveIfError( errorCode );

	RMMFAudioRecordControllerCustomCommands theCommander( iController );

	//[ In stopped we do not expect get record time avail to work
	// and should get the error code KErrNotReady]
	TTimeIntervalMicroSeconds theTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( theTimeAvailable );
	if( errorCode != KErrNotReady )
		{
		return result;
		}

	//[ search for record time available not ready event ]
	TMMFEvent recordTimeAvailNotReadyEvent( KMarcGetRecordTimeAvailId, KErrNotReady );
	result = SearchForEvent( recordTimeAvailNotReadyEvent );
	if( result == EFail )
		{
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

	//[ set the max file size to zero  and get the record time avail
	//  assuming nothing has been written to the disk :set the max file size to 1000
	// and get the record time avail. Then set the max file size to 2k and get the reoord time
	// available. Check that if record bytes avail when max file size is zero
	// > record time when file size is 2k then check that time interval for 2k is > time 
	// interval for 1k ]

	TInt theSize = 0;   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );

	TMMFEvent setMaxFileSizeSuccessEvent( KMarcSetMaxFileSizeId, KErrNone);
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	TTimeIntervalMicroSeconds theFillDiskTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( theFillDiskTimeAvailable );
	User::LeaveIfError( errorCode );
	//[ search for record time available success event ]
	TMMFEvent recordTimeAvailSuccessEvent( KMarcGetRecordTimeAvailId, KErrNone );
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	theSize = 4000;  // arbitrary value for comparison purposes   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	TTimeIntervalMicroSeconds theFourKTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( theFourKTimeAvailable );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	theSize = 3000;  // arbitrary value for comparison purposes   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	TTimeIntervalMicroSeconds theThreeKTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( theThreeKTimeAvailable );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ finally reset the file size to any before play ]
	theSize = 0;  // arbitrary value for comparison purposes   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );

	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ lets assert that the larger file has a greater record time]
	if( theThreeKTimeAvailable > theFourKTimeAvailable )
		{
		result = EFail;
		return result;
		}

	//[ lets do some recording ]
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

	//[ ok now that we are recording check that record time available
	// on two consecutive calls have the realtion recordtimeAvail(1) >=
	// recordtimeAvail(2) where recordtimeAvail(1) is the time interval
	// returned by the first call
	
	TTimeIntervalMicroSeconds firstTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( firstTimeAvailable );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	// [ lets wait for record to write some data ]
	RTimer  waitTimer ;
	User::LeaveIfError( waitTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 10000000 ; //arbitrary delay
	waitTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

	TTimeIntervalMicroSeconds secondTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( secondTimeAvailable );
	User::LeaveIfError( errorCode );	
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ lets check the results ]
	if( (firstTimeAvailable < secondTimeAvailable ) &&
		(theFillDiskTimeAvailable <= firstTimeAvailable ))
		{
		result = EFail;
		}

	//[ ok lets stop the controller, prime it and
	// reset the max file size to some low value say 200
	// attempt to record
	// reset the file size to unlimited and record for a period
	// check the bytes free has decreased ]
	//

	// [ lets ensure we are stopped ]
	User::LeaveIfError( iController.Stop() );
		

	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	result = SearchForEvent( primeEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ now set the max file size to a low value say 1 bytes and attempt to record
	//to this ]
	theSize = 1;  // arbitrary value for comparison purposes   
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	TTimeIntervalMicroSeconds currentTimeAvailable(0);
	TTimeIntervalMicroSeconds expectedTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( currentTimeAvailable );
	User::LeaveIfError( errorCode );	
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ we expect that there is zero time available ]
	if( currentTimeAvailable != expectedTimeAvailable )
		{
		result = EFail;
		return result;
		}

	//[ ok lets reset the max file size to zero ]
    theSize = 0; // which means we can fill the disk
	errorCode = theCommander.SetMaxFileSize( theSize );
	User::LeaveIfError( errorCode );
	result = SearchForEvent( setMaxFileSizeSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	errorCode = theCommander.GetRecordTimeAvailable( currentTimeAvailable );
	User::LeaveIfError( errorCode );	
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ lets do some recording ]
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

	User::LeaveIfError( waitTimer.CreateLocal());
	waitTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

	//[check record time avail is less than previous record time available ]
	TTimeIntervalMicroSeconds lastTimeAvailable(0);
	errorCode = theCommander.GetRecordTimeAvailable( lastTimeAvailable );
	User::LeaveIfError( errorCode );	
	result = SearchForEvent( recordTimeAvailSuccessEvent );
	if( result == EFail )
		{
		return result;
		}

	//[ check the LastTimeAvailable < currentTimeAvailable ]
	if( lastTimeAvailable > currentTimeAvailable )
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
	
	return result;
	}


/*
*
* CTestStepRecordFormat
*
*/
CTestStepRecordFormat::CTestStepRecordFormat( TUint aTestIndex )
	{
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);

	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

/*
*
* ~CTestStepRecordFormat
*
*/
CTestStepRecordFormat::~CTestStepRecordFormat()
	{


	}


// test preamble
TVerdict CTestStepRecordFormat::DoTestStepPreambleL(void)
	{
	// connect to the file server
	User::LeaveIfError(iFs.Connect());

	// call the superclass
	return CTestStepActrl::DoTestStepPreambleL();
	}

//test postamble
TVerdict CTestStepRecordFormat::DoTestStepPostambleL(void)
	{
	// close connection with the file server
	iFs.Close();

	// call the superclass
	return CTestStepActrl::DoTestStepPostambleL();
	}

/*
*
* DoTestStepL
* This test checks the stop function on the controller
*
*/
TVerdict CTestStepRecordFormat::DoTestStepL( void )
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
TVerdict CTestStepRecordFormat::DoConvertStepL( void )
	{
	 TVerdict result = EPass;
	
	 _LIT( KTestRecordFormat, "Record Format Test");
	 Log( KTestRecordFormat );
	
	 _LIT( KTestStepPlay, "This test checks recording an audio clip @ 32Khz");
	 Log( KTestStepPlay );
	
	 // [ record audio file name ]
	 TBuf<40> testFileName =  iTestParameters->iOutFilename;

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
	 
	 Log( testFileName );

	 //[ configure the filename ]
	 SetWriteFileNameL( testFileName );

	 // open the audio controller
	 TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	 User::LeaveIfError( errorCode );
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	User::LeaveIfError( errorCode );
	
	 errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig());
     User::LeaveIfError( errorCode );

	//[ attempt to get the sink num channels ]
    RMMFAudioControllerCustomCommands theCommander( iController );

	errorCode = theCommander.SetSourceSampleRate( iTestParameters->iDestSampleRate );
	if(errorCode!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("SetSourceSampleRate returned %d when KErrNotSupported is expected"), errorCode);
		return EFail;
		}
		
	 // set sink sample rate
	 errorCode = theCommander.SetSinkSampleRate( iTestParameters->iDestSampleRate );
	 User::LeaveIfError( errorCode );

	 // set sink number of channels
	 errorCode = theCommander.SetSinkNumChannels( iTestParameters->iDestChannels );
	 User::LeaveIfError( errorCode );

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

	// [ lets wait for record to write some data ]
	RTimer  waitTimer ;
	User::LeaveIfError( waitTimer.CreateLocal());
	TRequestStatus timerStatus    ;
    const TInt KDelay   = 10000000 ; //arbitrary delay
	waitTimer.After( timerStatus, KDelay );
	User::WaitForRequest( timerStatus );

	//[ stop the controller and check file length ]
	User::LeaveIfError( iController.Stop() );

	 return result;
	}

/***
*
* CTestStepSetGetRecordBalanceNoSink
* INC090160: Incorrect precondition check in audio controller
*
*/
CTestStepSetGetRecordBalanceNoSink::CTestStepSetGetRecordBalanceNoSink()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0065-HP");
	}

/*
*
* ~CTestStepSetGetRecordBalanceNoSink
*
*/
CTestStepSetGetRecordBalanceNoSink::~CTestStepSetGetRecordBalanceNoSink()
	{
	}

/*
*
* DoTestStepL
* This teststep tests get/set record balance with no sink
*
*/
TVerdict  CTestStepSetGetRecordBalanceNoSink::DoTestStepL( void )
	{
	TVerdict result = EFail ;

	_LIT( KTestPlay, "Set Get Record balance Test with no sink");
	Log( KTestPlay );
	
	_LIT( KTestStepRecord, "PDEF090782: Incorrect precondition check in audio controller");
	Log( KTestStepRecord );
	
	// [ audio file to play ]
	_LIT(KTestWavFileRecord, "16bitmpcm.wav");

	SetWriteFileNameL( KTestWavFileRecord );
   	
   	// NB open the REAL MmfAudioController this time, not the custom controller
	TInt errorCode = iController.Open(KUidMmfAudioController, iPriority);
	if(errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("Open MmfAudioController failed, error %d"), errorCode);		
		User::LeaveIfError( errorCode );
		}
	
	TMMFMessageDestination aSinkHandleInfo;

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
	if(errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("AddDataSource failed, error %d"), errorCode);		
		User::LeaveIfError( errorCode );
		}
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSink(KUidMmfFileSink, WriteFileConfig(), aSinkHandleInfo);
	if(errorCode != KErrNone)
		{
		INFO_PRINTF2(_L("AddDataSink failed, error %d"), errorCode);		
		User::LeaveIfError( errorCode );
		}
	
	//[ create a custom commander for record ]
	RMMFAudioRecordDeviceCustomCommands theCommander( iController  );

	const TInt KAnyOldStoppedBalance = 10;
	result = EPass;

	// INC090160
	// delete the sink - SetBalance and GetBalance should still succeed
	// NB: this test will fail prior to the INC fix (KErrNotReady)

	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
	if( errorCode != KErrNone )
		{
		INFO_PRINTF2(_L("RemoveDataSink failed, error %d"), errorCode);		
		result = EInconclusive;
		return result;
		}
	
	//[ set get record balance in stopped ]
	TInt theBalance = KAnyOldStoppedBalance;

	errorCode = theCommander.SetBalance( theBalance );
	if( errorCode != KErrNone )
		{
		INFO_PRINTF2(_L("SetBalance failed, error %d"), errorCode);		
		result = EFail;
		}

	TInt theNewBalance = 0; 
	errorCode = theCommander.GetBalance( theNewBalance );
	if( errorCode != KErrNone )
		{
		INFO_PRINTF2(_L("GetBalance failed, error %d"), errorCode);		
		result = EFail;
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
