// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
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

// Test system includes
#include <testframework.h>

#include "TSU_MMFMIDICLNT.h"
#include "TS_MMFMIDICLNTsuite.h"

#include "TestStepMidiClntAllocFailOpen.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepMidiClntAllocFailOpenFile.
 *
 *
 * @return	"CTestStepMidiClntAllocFailOpenFile*"
 *			The constructed CTestStepMidiClntAllocFailOpenFile
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenFile* CTestStepMidiClntAllocFailOpenFile::NewL(const TDesC& aTestName)
	{
	CTestStepMidiClntAllocFailOpenFile* self = new(ELeave) CTestStepMidiClntAllocFailOpenFile(aTestName);
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
CTestStepMidiClntAllocFailOpenFile::CTestStepMidiClntAllocFailOpenFile(const TDesC& aTestName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = aTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenFile::~CTestStepMidiClntAllocFailOpenFile()
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenFile::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// This installs the scheduler
	verdict = CTestMmfMidiClntStep::DoTestStepPreambleL();

	//[ Printing to the console and log file ]
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("This is a memory allocation failure test of CTestStepMidiClntAllocFailOpenFile"));
	
	if(!GetStringFromConfig(_L("SectionOne"), _L("filename"), iFileName))
		{
		verdict =  EInconclusive;
		}
		
	//[ Create the MidiClientUtility ]
	if ( (iMidiClnt = CMidiClientUtility::NewL(*this)) == NULL )
		{
		verdict = EInconclusive;
		}

	//[ Connect to File System using RFs ]
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());

	//[ Open the file using RFile ]
	INFO_PRINTF2(_L("Opening the file : %S"), &iFileName);
	
	TInt theRes = iFile.Open(iFs, iFileName, EFileRead);
	if (theRes != KErrNone)
		{
		INFO_PRINTF2(_L("Errors in Opening the file : %S"), &iFileName);
		User::Leave(theRes);
		}
		
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenFile::DoTestStepPostambleL(void)
	{
	//[ Create the MidiClientUtility ]
	delete iMidiClnt;
	iMidiClnt = NULL;
	//[ Close Files ]
	iFile.Close();
	iFs.Close();
	//[ Destroy the scheduler ]
	return CTestMmfMidiClntStep::DoTestStepPostambleL();
	}

void CTestStepMidiClntAllocFailOpenFile::OpenFileAndStartSchedulerL()
	{
	iMidiClnt->OpenFile(iFileName);
	CActiveScheduler::Start();
	}

void CTestStepMidiClntAllocFailOpenFile::OpenFileByHandleAndStartSchedulerL()
	{
	iMidiClnt->OpenFile(iFile);
	CActiveScheduler::Start();
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
TVerdict CTestStepMidiClntAllocFailOpenFile::DoTestStepL()
	{
	//[ Initialise Class Variables ]
	iTestStepResult = EPass;
	
	//[ Declare Local Variables ]
    TInt err       = KErrNone;
	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	TBool useFileHandle = EFalse;
	
	if(iTestStepName.Compare(_L("MM-MMF-MIDICLNT-U-1003")) == 0)
		{
		useFileHandle = ETrue;
		}
	
	
	//[ Start of main ALLOC test loop ]
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;
		
		if(!useFileHandle)
			{
			TRAP( err, OpenFileAndStartSchedulerL() );
			}
		else 
			{
			TRAP( err, OpenFileByHandleAndStartSchedulerL() );
			}
		
		completed = EFalse;
		if ((err == KErrNone) && (iError == KErrNone))
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}
			
			// see if valid result and break if wrong - might be premature result
			if (iMidiClnt == NULL || iError != KErrNone )
				{
				badResult = ETrue;
				}
			
			if(iMidiClnt)
				{
				iMidiClnt->Close();
				}

			completed = reachedEnd || badResult;
			}
		else 
			{
				
			if (((err != KErrNone) && (err != KErrNoMemory)) || 
				((iError != KErrNone) && (iError != KErrNoMemory))) // bad error code
				{
				completed = ETrue;
				}
			}

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			{
			break; // exit loop
			}
		failCount++;
		}

	failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.
	
	if (err != KErrNone || badResult)
		{
		if (badResult)
			{
			INFO_PRINTF2(_L("Bad result with %d memory allocations tested"), failCount);
			}
		else
			{
			INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested"), err, failCount);
			}
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}


	INFO_PRINTF1(_L("finished with this test step"));
	return iTestStepResult;
	}

// from MMidiClientUtilityObserver
void CTestStepMidiClntAllocFailOpenFile::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepMidiClntAllocFailOpenFile::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aEntry*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
	{
	}
void CTestStepMidiClntAllocFailOpenFile::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
	}


// --------------------------------------------

/**
 *
 * Static constructor for CTestStepMidiClntAllocFailOpenDes.
 *
 *
 * @return	"CTestStepMidiClntAllocFailOpenDes*"
 *			The constructed CTestStepMidiClntAllocFailOpenDes
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenDes* CTestStepMidiClntAllocFailOpenDes::NewL()
	{
	CTestStepMidiClntAllocFailOpenDes* self = new(ELeave) CTestStepMidiClntAllocFailOpenDes;
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
CTestStepMidiClntAllocFailOpenDes::CTestStepMidiClntAllocFailOpenDes()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-MIDICLNT-U-0201-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenDes::~CTestStepMidiClntAllocFailOpenDes()
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenDes::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 //this installs the scheduler
	 verdict = CTestMmfMidiClntStep::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-MIDICLNT-U-0201-CP"));
	INFO_PRINTF1(_L("this is a memory allocation failure test of CTestStepMidiClntAllocFailOpenDes"));
	
	if(!GetStringFromConfig(_L("SectionOne"), _L("filename"), iFilename))
		return EInconclusive;

	RFs fs;
	RFile file;
	TInt size = 0;

	// connect to file system and open file
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,iFilename,EFileRead));
	CleanupClosePushL(file);

	// Set HBuf size
	User::LeaveIfError(file.Size(size));
	INFO_PRINTF2(_L("size of file = %d\n"),size);

	iAudio = HBufC8::NewMaxL(size);

	// read data into Hbuf
	TPtr8 bufferDes(iAudio->Des());
	User::LeaveIfError(file.Read(bufferDes));
	
	CleanupStack::PopAndDestroy(); //file

	// create the Midi utility
	if ( (iMidiClnt = CMidiClientUtility::NewL(*this)) == NULL )
		return EInconclusive;

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenDes::DoTestStepPostambleL(void)
	{
	delete iMidiClnt;
	iMidiClnt = NULL;
	delete iAudio;
	iAudio = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfMidiClntStep::DoTestStepPostambleL();
	}

void CTestStepMidiClntAllocFailOpenDes::OpenDesAndStartSchedulerL()
	{
	iMidiClnt->OpenDes(iAudio->Des());
	CActiveScheduler::Start();
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
TVerdict CTestStepMidiClntAllocFailOpenDes::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, OpenDesAndStartSchedulerL() );

		completed = EFalse;
		if ((err == KErrNone) && (iError == KErrNone))
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);

			
			// see if valid result and break if wrong - might be premature result
			if (iMidiClnt == NULL ||
				iError != KErrNone )
				badResult = ETrue;

			if(iMidiClnt)
				iMidiClnt->Close();
			
			completed = reachedEnd || badResult;
			}
		else if (((err != KErrNone) && (err != KErrNoMemory)) || 
				((iError != KErrNone) && (iError != KErrNoMemory))) // bad error code
			completed = ETrue;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	delete iMidiClnt;
	iMidiClnt = NULL;

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

	// from MMidiClientUtilityObserver
void CTestStepMidiClntAllocFailOpenDes::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepMidiClntAllocFailOpenDes::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aEntry*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
	{
	}
void CTestStepMidiClntAllocFailOpenDes::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
	}


// --------------------------------------------

/**
 *
 * Static constructor for CTestStepMidiClntAllocFailOpenUrl.
 *
 *
 * @return	"CTestStepMidiClntAllocFailOpenUrl*"
 *			The constructed CTestStepMidiClntAllocFailOpenUrl
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenUrl* CTestStepMidiClntAllocFailOpenUrl::NewL()
	{
	CTestStepMidiClntAllocFailOpenUrl* self = new(ELeave) CTestStepMidiClntAllocFailOpenUrl;
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
CTestStepMidiClntAllocFailOpenUrl::CTestStepMidiClntAllocFailOpenUrl()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-MIDICLNT-U-0202-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailOpenUrl::~CTestStepMidiClntAllocFailOpenUrl()
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenUrl::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestMmfMidiClntStep::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-MIDICLNT-U-0202-CP"));
	INFO_PRINTF1(_L("this is a memory allocation failure test of CTestStepMidiClntAllocFailOpenUrl"));
	
	if(!GetStringFromConfig(iSectName,iKeyName,iUrlname))
		return EInconclusive;

	/*
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2))
		{
		//INFO_PRINTF2(_L("file name %s not found..."), fileptr);
		return EInconclusive;
		}
	*/

	// create the Midi utility
	if ( (iMidiClnt = CMidiClientUtility::NewL(*this)) == NULL )
		verdict = EInconclusive;

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepMidiClntAllocFailOpenUrl::DoTestStepPostambleL(void)
	{
	delete iMidiClnt;
	iMidiClnt = NULL;
	//[ Destroy the scheduler ]
	return CTestMmfMidiClntStep::DoTestStepPostambleL();
	}

void CTestStepMidiClntAllocFailOpenUrl::OpenUrlAndStartSchedulerL()
	{
	iMidiClnt->OpenUrl(iUrlname);
	CActiveScheduler::Start();
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
TVerdict CTestStepMidiClntAllocFailOpenUrl::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, OpenUrlAndStartSchedulerL() );

		completed = EFalse;
		if ((err == KErrNone) && (iError == KErrNone))
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);

			
			// see if valid result and break if wrong - might be premature result
			if (iMidiClnt == NULL ||
				iError != KErrNone )
				badResult = ETrue;

			if(iMidiClnt)
				iMidiClnt->Close();
			
			completed = reachedEnd || badResult;
			}
		else if (((err != KErrNone) && (err != KErrNoMemory)) || 
				((iError != KErrNone) && (iError != KErrNoMemory))) // bad error code
			completed = ETrue;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	delete iMidiClnt;
	iMidiClnt = NULL;

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

	// from MMidiClientUtilityObserver
void CTestStepMidiClntAllocFailOpenUrl::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepMidiClntAllocFailOpenUrl::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aEntry*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
	{
	}
void CTestStepMidiClntAllocFailOpenUrl::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
	}



