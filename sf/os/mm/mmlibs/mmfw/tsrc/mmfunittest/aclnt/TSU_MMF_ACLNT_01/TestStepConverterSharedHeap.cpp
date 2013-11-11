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
// These test steps create audio converter utilities whose
// controllers share a single heap.
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>
#include <mdaaudiosampleeditor.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepConverterSharedHeap.h"

#include "mmfclientaudiorecorder.h"

// For custom controller
#include "../../ctlfrm/TS_CMMFTestController/CmmfTestControllerUIDs.hrh"
#include "../../ctlfrm/TS_Codes.h"

// --------------------------------------------

const TInt KMaxConverters = 24;
// would definitely exceed the chunk limit if there converters were
// using separate heaps (which they shouldn't in this test)


const TInt KNoSharedConverter = 2;
// which of the created players should be given its own heap on mixed style tests
// This must always be true: KNoSharedConverter < KMaxConverters

const TUid KTstControllerUid = {KMmfTestControllerUid}; //(0x101F88D8)
// custom controller ID

// --------------------------------------------


/**
 *
 * Static constructor for CTestStepConverterSharedHeapMultiFile.
 *
 *
 * @return	"CTestStepConverterSharedHeapMultiFile*"
 *			The constructed CTestStepConverterSharedHeapMultiFile
 *
 * @xxxx
 * 
 */
CTestStepConverterSharedHeapMultiFile* CTestStepConverterSharedHeapMultiFile::NewL(const TDesC& aName, TBool aMixHeapStyle )
	{
	CTestStepConverterSharedHeapMultiFile* self = new(ELeave) CTestStepConverterSharedHeapMultiFile( aName, aMixHeapStyle );
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepConverterSharedHeapMultiFile::CTestStepConverterSharedHeapMultiFile(const TDesC& aName, TBool aMixHeapStyle )
	: iMixHeapStyle( aMixHeapStyle )
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aName;
	
	// increase heapsize to avoid KErrNoMemory
	iHeapSize =128* 1024; //512k (64k was default)
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConverterSharedHeapMultiFile::~CTestStepConverterSharedHeapMultiFile()
	{
	}
	
/**
 * Implementation of the MoscoStateChangeEvent  interface functions
 **/
 
 void CTestStepConverterSharedHeapMultiFile::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
 	{ 
 
    iError=aErrorCode;
	if (iError )
    	{
    	ERR_PRINTF2(_L("AudioRecPlay failed, unexpected error %d."), aErrorCode);
    	}
    CActiveScheduler::Stop();
    }



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConverterSharedHeapMultiFile::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1( iTestStepName );
	
	iError = KErrNone; //KErrTimedOut;

	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		INFO_PRINTF1(_L("Failed to find filename"));
		return EInconclusive;
		}

	// Delete the generated file before starting
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConverterSharedHeapMultiFile::DoTestStepPostambleL(void)
	{
	
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;
	// close array
	iConverters.Close();
	
	INFO_PRINTF1(_L("finished with this test step"));
	
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

/**
 *
 * The commands to be included in the TRAP command
 *
 * @xxxx
 * 
 */
CMdaAudioConvertUtility* CTestStepConverterSharedHeapMultiFile::CreateAndOpenConverterL( TInt aIteration )
	{
	CMdaAudioConvertUtility* converter = NULL;
	
	TRAPD( err, converter = CMdaAudioConvertUtility::NewL(*this) );
	User::LeaveIfError( err );
	CleanupStack::PushL(converter);
	
	if( ! (iMixHeapStyle && (aIteration == KNoSharedConverter)) )
		{
		INFO_PRINTF2(_L("Converter on iteration %2d is using a shared heap."), aIteration );
		converter->UseSharedHeap(); // ensure that underlying controller uses a shared heap
		}
	else
		{
		INFO_PRINTF2(_L("	Converter on iteration %2d has its own heap."), aIteration );
		}
	
	converter->OpenL( iFileName,iFileName2 ); // this creates a new controller
	
	CActiveScheduler::Start(); // wait for open to complete

	User::LeaveIfError( iError ); // if had error return from callback
	
	CleanupStack::Pop(converter);
	return converter;
	}
	

/**
 *
 * Creates KMaxConverters players, performs the desired test on one of them
 * and then closes them all and cleans up.
 * If problems are encountered the returned result will reflect this.
 *
 * @return	"TVerdict"
 *			The result of the tests
 *
 * @xxxx
 * 
 */
TVerdict CTestStepConverterSharedHeapMultiFile::DoTestL( TTestType aTestType )
	{
	iTestStepResult = EPass;
    TInt err = KErrNone;
	
	__MM_HEAP_MARK;
	
	TInt i=0;
	for( ; i < KMaxConverters; ++i )	
		{
		iError = KErrNone;
		
		CMdaAudioConvertUtility* converter = NULL;
		
		TRAPD(err, converter = CreateAndOpenConverterL(i) );
		
		if (err == KErrNone && iError == KErrNone)
			{
			// see if valid result and break if wrong - might be premature result
			TInt err2 = KErrNone;
			TMMFMessageDestination* srcHandle = NULL;
			err2 = GetAudConverterSourceHandle(converter, srcHandle);
			
			if ( err2 != KErrNone ||
				 &srcHandle == NULL ||
				 converter->State() != CMdaAudioConvertUtility::EOpen ||
				 converter->Duration() == TTimeIntervalMicroSeconds(0) || // we know the duration of the file is more than 0
				 iError != KErrNone)
				{
				// dodgy converter
				
				INFO_PRINTF2(_L("	Dodgy converter on iteration %2d"), i);
				iTestStepResult = EFail;
				
				delete converter;
				converter = NULL;
				break;
				}
			else
				{
				// no errors				
				// append converter to array
				if( (err = iConverters.Append( converter )) != KErrNone )
					{
					INFO_PRINTF2(_L("Fail to append converter on iteration %2d"), i);
					// this doesn't indicate a failure in what we're testing here
					iTestStepResult = EInconclusive;
				
					delete converter;
					converter = NULL;
					break;
					}
				}
			}
		else
			{
			INFO_PRINTF4(_L("Fail to open converter on iteration %2d - %d %d"), i, err, iError);
			break;
			}	
		}

	// check outcome of making converters
	if ( err || iError )
		{
		if ((err && err != KErrNoMemory) || (err == KErrNone && iError != KErrNoMemory))
			{
			// a genuine error
			
			if( err )
				{
				INFO_PRINTF3(_L("Error %d occured while trying to create converter #%d"), err, i );
				}
			else
				{
				INFO_PRINTF3(_L("Error %d occured while opening converter #%d"), iError, i );
				}
			
			iTestStepResult = EFail;
			}
		else
			{
			// we ran out of memory
			INFO_PRINTF2(_L("Teststep ran out of memory while creating converter #%d"), i );
			iTestStepResult = EFail;
			}
		}
	// else no errors


	// perform test on 1st converter(unless there were errors above)
	if( (iTestStepResult == EPass) && (iConverters.Count() > 0 ) )
		{
		switch( aTestType )
			{
			case EConvert:
				{		
				(iConverters[0])->ConvertL();
				CActiveScheduler::Start();
				
				if( iError )
					{
					// playback died
					INFO_PRINTF2(_L("Error %d occured while converting"), iError );
					iTestStepResult = EFail;
					}
					
				break;
				}
		
				
			case EPanic:
				{
				// panic player
				
				// load a custom controller (using the share heap)
				// that can be panicked and see if we recover OK
				TMMFPrioritySettings settings;
				TInt error = KErrNone;
				RMMFController controller;

				settings.iPriority = 1;
				settings.iPref = EMdaPriorityPreferenceTime;
				settings.iState = EMMFStateIdle;

				error = controller.Open(KTstControllerUid, settings, ETrue); // use shared heap
				INFO_PRINTF2(_L("Custom controller Open: %d"), error);

				if( ! error )
					{
					// panic the controller
					// this command will complete immediately and
					// then panic
					TUid uid;
					uid = TUid::Uid(0x101f72B4);
					TMMFMessageDestinationPckg destPckg(uid);
					//TRequestStatus status = KRequestPending;
					controller.CustomCommandSync(destPckg,
												EMMFTestCustomCommandPanic,
												KNullDesC8,
												KNullDesC8);
					
					// need to allow some time to make
					// sure the thread has actually been panicked when we close it
					User::After( TTimeIntervalMicroSeconds32(5000000) ); // 5 seconds
					}
					
				// controller should have panicked by now
				controller.Close();
				
				break;
				}


			// default: // do nothing
			}
		}
		
	// cleanup
	for( i = 0; i < iConverters.Count(); ++i )
		{
		delete iConverters[i];
		
		iConverters[i] = NULL;
		}
	iConverters.Reset();

	__MM_HEAP_MARKEND;

	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Get's the Audio Recorder Source Handle
 *
 * @xxxx
 * 
 */
TInt CTestStepConverterSharedHeapMultiFile::GetAudConverterSourceHandle(CMdaAudioConvertUtility* aConverter,
														 TMMFMessageDestination* /*aSrcHandle*/)
	{
	if (aConverter == NULL)
		return KErrNotReady;
	return KErrNone;
	}


/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepConverterSharedHeapMultiFile::DoTestStepL()
	{
	INFO_PRINTF1(_L("This is a test that several converter utilities using a shared heap can be created."));

	return DoTestL( EConvert );
	}


