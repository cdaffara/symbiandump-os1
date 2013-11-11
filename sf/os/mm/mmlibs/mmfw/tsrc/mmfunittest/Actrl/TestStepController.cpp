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
// TestStepContropller.cpp
//

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TS_MMF_ACTRL_TestStep.h"
#include "TS_MMF_ACTRL_TestSuite.h"

#include "ActrlTestUids.h"

// Specific includes for these test steps
#include "TestStepController.h"

// constants destined for header file
//const TInt KPlayDelayNewMail = 100000000 ;	// EABI warning removal
const TInt KMemoryAllocCmd = 0xaaaaa;

/**
*
* CTestStepResetController
*
*/
CTestStepResetController::CTestStepResetController()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0016-HP");
	}

/**
*
* ~CTestStepResetController
*
*/
CTestStepResetController::~CTestStepResetController()
	{
	}

/**
*
* DoTestStepL
* This method tests the controllers reset function
*
*/
TVerdict CTestStepResetController::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Reset Test");
	Log( KTestPlay );
	
	_LIT( KTestStepReset, "This test checks a controller reset");
	Log( KTestStepReset );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
	
   	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	
	//[ok lets reset the controller from it initial state]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	//[ search for reset event ]
	TMMFEvent resetEvent( KResetTestId, KErrNone);
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}
	
	//[ lets try a second reset ]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}
	
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	//[ok lets reset the controller from it initial state]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}
	
	//[ lets try a second reset ]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}

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
	
	//[ lets reset again in the primed state ]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}
	
	// [ get back to primed state ]
	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig());
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8);
    User::LeaveIfError( errorCode );
	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
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
	
	//[ lets reset again in the primed state ]
	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
	if( result == EFail )
		{
		return result;
		}

	errorCode = iController.Reset();
	User::LeaveIfError( errorCode );
	
	//[ search for reset event ]
	result = SearchForEvent( resetEvent );
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
	   
	// [test steps return a result]
	return result;
	}

/**
*
* CTestStepRemoveDataSink
*
*/
CTestStepRemoveDataSink::CTestStepRemoveDataSink()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0017-HP");
	}

/**
*
* ~CTestStepPauseController
*
*/ 
CTestStepRemoveDataSink::~CTestStepRemoveDataSink()
	{
	}

/**
*
* DoTestStepL
*
*/
TVerdict CTestStepRemoveDataSink::DoTestStepL( void )
	{
	 __MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Remove Data Sink Test");
	Log( KTestPlay );
	
	_LIT( KTestStepPause, "This test checks removing a data sink");
	Log( KTestStepPause );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
	
   	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );

	//[ declare handles for removal of the data source and data sink]
	TMMFMessageDestination aSourceHandleInfo;
	TMMFMessageDestination aSinkHandleInfo  ;

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig(), aSourceHandleInfo );
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8, aSinkHandleInfo );
    User::LeaveIfError( errorCode );

	//[ lets try removing the data sink with the soure handle ]
	errorCode =	iController.RemoveDataSink( aSourceHandleInfo );
    if( errorCode == KErrNone )
		{
		result = EFail;
		return result;
		}

	//[ lets remove the data sink ]
	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
    User::LeaveIfError( errorCode );
	
	//[ search for remove event ]
	TMMFEvent removeDataSinkEvent( KRemoveDataSinkTestId, KErrNone );
	TVerdict removeResult = SearchForEvent( removeDataSinkEvent );
	if( removeResult == EFail )
		{
		return removeResult ;
		}
	
	//[ try removing the data sink a second time ]
	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ no need to remove event since the message does not reach the custom controller ]

	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8, aSinkHandleInfo );
    User::LeaveIfError( errorCode );
    	
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict primeResult = SearchForEvent( primeEvent );
	if( primeResult == EFail )
		{
		return primeResult;
		}

	//[ try removing the data sink ]
	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	// [ second event is not generated on failure ]
	
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

	//[ try removing the data sink  ]
	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ try removing the data sink  ]
	errorCode =	iController.RemoveDataSink( aSinkHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	result = EPass ;

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
* CTestStepRemoveDataSource
*
*/
CTestStepRemoveDataSource::CTestStepRemoveDataSource()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0019-HP");
	}

/**
*
* ~CTestStepRemoveDataSource
*
*/
CTestStepRemoveDataSource::~CTestStepRemoveDataSource()
	{
	}

/**
*
* DoTestStepL
* This method tests removing a Data Source
*/
TVerdict CTestStepRemoveDataSource::DoTestStepL( void )
	{
	 __MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Remove Data Source Test");
	Log( KTestPlay );
	
	_LIT( KTestStepSource, "This test checks removing a data source");
	Log( KTestStepSource );
	
	// [ audio file to play ]
	_LIT(KTestWavFile, "newmail.wav");
	SetReadFileNameL( KTestWavFile );
	
   	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	//iFileConfig().iPath = KTestWavFile;

	//[ declare handles for removal of the data source and data sink]
	TMMFMessageDestination aSourceHandleInfo;
	TMMFMessageDestination aSinkHandleInfo  ;

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig(), aSourceHandleInfo );
	User::LeaveIfError( errorCode );
	
	errorCode =	iController.AddDataSink(KUidMmfAudioOutput, KNullDesC8, aSinkHandleInfo );
    User::LeaveIfError( errorCode );

	//[ lets remove the data source using the sink handle !! ]
	errorCode =	iController.RemoveDataSource( aSinkHandleInfo );
    if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ lets remove the data source ]
	errorCode =	iController.RemoveDataSource( aSourceHandleInfo );
    User::LeaveIfError( errorCode );

	//[ search for remove event ]
	TMMFEvent removeDataSourceEvent( KRemoveDataSourceTestId, KErrNone );
	TVerdict removeResult = SearchForEvent( removeDataSourceEvent );
	if( removeResult == EFail )
		{
		return removeResult ;
		}
	
	//[ try removing the data source a second time ]
	errorCode =	iController.RemoveDataSource( aSourceHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ no need to remove event since the message does not reach the custom controller ]

	//[ add data source and sink and prime the controller ]
    errorCode = iController.AddDataSource(KUidMmfFileSource, ReadFileConfig(), aSourceHandleInfo );
	User::LeaveIfError( errorCode );
    
	errorCode =	iController.Prime();
	User::LeaveIfError( errorCode );
	
	// [ wait for and process the return event from the audio output ]
	
	TMMFEvent primeEvent( KPrimeTestId, KErrNone);
	TVerdict primeResult = SearchForEvent( primeEvent );
	if( primeResult == EFail )
		{
		return primeResult;
		}

	//[ try removing the data source ]
	errorCode =	iController.RemoveDataSource( aSourceHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	// [ second event is not generated on failure ]
	
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

	//[ try removing the data sink  ]
	errorCode =	iController.RemoveDataSource( aSourceHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	//[ try removing the data sink  ]
	errorCode =	iController.RemoveDataSource( aSourceHandleInfo );
	if( errorCode == KErrNone )
		{
		result = EFail;
		return result ;
		}

	result = EPass ;

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
* CTestStepMemoryAllocation
*/
CTestStepMemoryAllocation::CTestStepMemoryAllocation()
	{
	iTestStepName = _L("MM-MMF-ACTRL-U-0020-HP");
	}

/**
*
* ~CTestStepMemoryAllocation
*/
CTestStepMemoryAllocation::~CTestStepMemoryAllocation()
	{
	}

/**
*
*DoTestStepL
*/
TVerdict CTestStepMemoryAllocation::DoTestStepL( void )
	{
	__MM_HEAP_MARK;
	
	TVerdict result = EPass;
	
	_LIT( KTestPlay, "Memory Allocation Test");
	Log( KTestPlay );
	
	_LIT( KTestStepSource, "This test checks memory allocation of the plugin");
	Log( KTestStepSource );
	
   	TInt errorCode = iController.Open(KUidCustomTestAudioPlugin, iPriority);
	User::LeaveIfError( errorCode );
	//[ send the custom command to the custom audio controller ]
	TMMFMessageDestinationPckg destinationPckg(KMemoryAllocationTestId);
	
	//[ send memory alloc test to the custom plugin ]
	//[ we have only ensured that the controller is loaded into memory
	// and that all add sources etc will be don
	errorCode = iController.CustomCommandSync(destinationPckg, 
		KMemoryAllocCmd, 
		KNullDesC8,
		KNullDesC8);
	User::LeaveIfError( errorCode );

	//[ ensure the controller is unloaded so that it does 
	// not interfere with the following test ]
	// There is error in this code and the stop followed by reset
	// ensure the controller resources are released.
	iController.Stop();
	iController.Reset();
	iController.Close();  

	return result ;
	}
