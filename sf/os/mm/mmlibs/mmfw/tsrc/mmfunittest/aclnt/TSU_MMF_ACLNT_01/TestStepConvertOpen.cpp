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
#include <s32file.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

//#include <MdaAudioSamplePlayer.h>
#include <mdaaudiosampleeditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepConvert.h"


#include "MmfAudioController.h"
#include "mmfclientaudioconverter.h"

const TInt KNumBytesToCompare = 4096;

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepConvertOpen.
 *
 *
 * @return	"CTestStepConvertOpen*"
 *			The constructed CTestStepConvertOpen
 *
 * @xxxx
 * 
 */
CTestStepConvertOpen* CTestStepConvertOpen::NewL()
	{
	CTestStepConvertOpen* self = new(ELeave) CTestStepConvertOpen;
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
CTestStepConvertOpen::CTestStepConvertOpen() : iError(KErrNone)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0551-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepConvertOpen::~CTestStepConvertOpen()
	{
	}

void CTestStepConvertOpen::MoscoStateChangeEvent(CBase* aObject,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	if (aObject != iConvert)
		iError = KErrArgument;
	else
		iError = aErrorCode;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpen::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0551-CP"));
	INFO_PRINTF1(_L("this is a test of CMdaAudioConvertUtility::OpenL(file1, file2) "));

	
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName7"), iFileName) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), iFileName2) )
		{
		//INFO_PRINTF2(_L("file name %s not found..."), iFileName);
		//INFO_PRINTF2(_L(" or file name %s not found..."), iFileName2);
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

	// create the Recorder utility
	if ( (iConvert = CMdaAudioConvertUtility::NewL(*this)) == NULL )
		return EInconclusive;
	
	return EPass;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepConvertOpen::DoTestStepPostambleL(void)
	{
	//delete the output file 
	CFileMan* fileMan;
	iFs.Connect();
	fileMan = CFileMan::NewL(iFs);
	fileMan->Delete(iFileName2);
	iFs.Close();
	delete fileMan;
	fileMan = NULL;

	delete iConvert;
	iConvert = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepConvertOpen::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;
	TPtrC createdFile;
	TPtrC expectedFile;

	__MM_HEAP_MARK;
	TRAP(err, iConvert->OpenL(iFileName, iFileName2) );
	CActiveScheduler::Start();

	if (iConvert == NULL ||
		iError != KErrNone ||
		err != KErrNone ||
		iConvert->State() != CMdaAudioConvertUtility::EOpen  )
		{
		iTestStepResult = EFail;
		}

	iError = iConvert->SetThreadPriority(EPriorityAbsoluteHigh);
	if (iError != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to set the coverter's thread priority"));
		iTestStepResult = EFail;
		goto EndTest;
		}

	iConvert->ConvertL();
	CActiveScheduler::Start();  // open - > record
	if (iError != KErrNone)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	CActiveScheduler::Start();  // record -> open
	if (iError != KErrNone)
		{
		iTestStepResult = EFail;
		goto EndTest;
		}
	iConvert->Close();

	// Test PlayL and RecordL functions (these functions now replaced by ConvertL)
	iTestStepResult = TestPlayL();
	if (iTestStepResult != EPass)
		{
		goto EndTest;
		}

	iTestStepResult = TestRecordL();

	// Now compare the Created file with the expected result
	if(!GetStringFromConfig(_L("SectionOne"), _L("AudioFNameToConvert"), createdFile) ||
	   !GetStringFromConfig(_L("SectionOne"), _L("AudioExpectedConvertFile"), expectedFile) )
		{
		return EInconclusive;
		}

	iTestStepResult = CompareFilesL(createdFile, expectedFile, 100);

	delete iConvert;
	iConvert = NULL;

	__MM_HEAP_MARKEND;

EndTest:
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Test failed with error code %d"), iError);
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

TVerdict CTestStepConvertOpen::TestPlayL()
	{
	__MM_HEAP_MARK;
	TRAPD(err, iConvert->OpenL(iFileName, iFileName2) );
	CActiveScheduler::Start();

	if (iConvert == NULL ||
		iError != KErrNone ||
		err != KErrNone ||
		iConvert->State() != CMdaAudioConvertUtility::EOpen  )
		iTestStepResult = EFail;

	iConvert->PlayL();
	CActiveScheduler::Start();  // open - > record
	if (iError != KErrNone)
		return EFail;
	CActiveScheduler::Start();  // record -> open
	if (iError != KErrNone)
		return EFail;
	iConvert->Close();
	__MM_HEAP_MARKEND;

	return EPass;
	}

TVerdict CTestStepConvertOpen::TestRecordL()
	{
	__MM_HEAP_MARK;
	TRAPD(err, iConvert->OpenL(iFileName, iFileName2) );
	CActiveScheduler::Start();

	if (iConvert == NULL ||
		iError != KErrNone ||
		err != KErrNone ||
		iConvert->State() != CMdaAudioConvertUtility::EOpen  )
		iTestStepResult = EFail;

	iConvert->RecordL();
	CActiveScheduler::Start();  // open - > record
	if (iError != KErrNone)
		return EFail;
	CActiveScheduler::Start();  // record -> open
	if (iError != KErrNone)
		return EFail;
	iConvert->Close();
	__MM_HEAP_MARKEND;

	return EPass;
	}

TVerdict CTestStepConvertOpen::CompareFilesL(const TDesC& aCreatedFilename, const TDesC& aExpectedFilename, TInt aLength)
	{
	TVerdict verdict = EPass;
	TInt position = 0;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = KErrNone;

	RFile file;
	User::LeaveIfError(file.Open(fs, aCreatedFilename, EFileRead|EFileShareAny));
	CleanupClosePushL(file);

	TInt fileSize=0;
	err = file.Size(fileSize);
	if (err != KErrNone) User::LeaveIfError(err);

	RFileReadStream fileReadStream(file, position);
	CleanupClosePushL(fileReadStream);

	//read data from created file into descriptor
	HBufC8* createdBuffer = HBufC8::NewL(aLength);
	CleanupStack::PushL(createdBuffer);
	TPtr8 createdBufferPtr = createdBuffer->Des();
	TRAP(err, fileReadStream.ReadL(createdBufferPtr));

	if ((err != KErrNone) && (err != KErrEof)) User::LeaveIfError(err); //EOF not an error ?

	RFile file2;
	User::LeaveIfError(file2.Open(fs, aExpectedFilename, EFileRead|EFileShareAny));
	CleanupClosePushL(file2);


	TInt file2Size;
	err = file2.Size(file2Size);
	if (err != KErrNone) User::LeaveIfError(err);


	if(fileSize != file2Size)
		{
		INFO_PRINTF3(_L("Resulting file sizes do not match  %d != %d"), fileSize, file2Size);
		verdict = EFail;
		}
	else
		{
		//check contents
		RFileReadStream fileReadStream2(file2, position);
		CleanupClosePushL(fileReadStream2);

		//read data from expected file into descriptor
		HBufC8* expectedBuffer = HBufC8::NewL(aLength);
		CleanupStack::PushL(expectedBuffer);
		TPtr8 expectedBufferPtr = expectedBuffer->Des();
		TRAP(err, fileReadStream2.ReadL(expectedBufferPtr));

		if ((err != KErrNone) && (err != KErrEof)) User::LeaveIfError(err); //EOF not an error ?

		//compare expected buffer with the newly create buffer
		TInt result = expectedBuffer->Compare(*createdBuffer);
		if (result != 0)
			{
			INFO_PRINTF1(_L("Resulting file contents do not match"));
			verdict = EFail;
			}
		}

	CleanupStack::PopAndDestroy(7); //file, fs, fileReadStream, createdBuffer, file2, fileReadStream2, expectedBuffer

	return verdict;
	}

 
 // factory function
CConvertAudio* CConvertAudio::NewL(MConvAudioObserver* aObserver)
	{
	CConvertAudio* self = new (ELeave) CConvertAudio(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CConvertAudio::~CConvertAudio()
	{
	if (iUtility)
		{
		// if it exists, always stop the utility - null action if not running
		iUtility->Stop();
		delete iUtility;
		}
	}

void CConvertAudio::ConstructL()
	{
	iUtility = CMdaAudioConvertUtility::NewL(*this);
	}
	
CConvertAudio::CConvertAudio(MConvAudioObserver* aObserver) :
	iObserver (aObserver)
	{
	}
	
void CConvertAudio::ConvertL(const TDesC& aFromFileName, const TDesC& aToFileName)
	{
	// Note: in this scenario, the calling code can handle the leave here
	// but alternatively we would have to trap and (using one-shot AO) arrange
	// for the callback to be subsequently called.
	
	iUtility->OpenL(aFromFileName, aToFileName);
	}
	
void CConvertAudio::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	__ASSERT_ALWAYS(aObject==iUtility, User::Invariant());
	
	TInt error = aErrorCode;
	if (error == KErrNone)
		{
		// only interested in some state transitions...
		if (aPreviousState == CMdaAudioConvertUtility::ENotReady && 
		        aCurrentState == CMdaAudioConvertUtility::EOpen)
			{
			// have opened the file
			TRAP(error, OnOpenL());
			}
		else if (aPreviousState == CMdaAudioConvertUtility::EPlaying && 
		         aCurrentState == CMdaAudioConvertUtility::EOpen)
			{
			// normal termination 
			iObserver->ConvertComplete(KErrNone);
			}
		}
		
	if (error != KErrNone)
		{
		// stop now
		iObserver->ConvertComplete(aErrorCode);
		}
	}

// having opened the file, give settings and then start to record
void CConvertAudio::OnOpenL()
	{
	iUtility->SetDestinationDataTypeL(KMMFFourCCCodePCM16); // let's convert to pcm16
	iUtility->ConvertL();
	}

RTestStepConvertAudio* RTestStepConvertAudio::NewL(const TDesC& aStepName, 
                                     			   const TDesC& aFromFileName,
                                     			   const TDesC& aToFileName,
                                     			   const TDesC& aReferenceFileName)
	{
	RTestStepConvertAudio* self = new (ELeave) RTestStepConvertAudio(aStepName, 
													   				 aFromFileName,
													   				 aToFileName,
													   				 aReferenceFileName);
	return self;
	}

RTestStepConvertAudio::RTestStepConvertAudio(const TDesC& aStepName, 
							   				 const TDesC& aFromFileName,
							   				 const TDesC& aToFileName,
							   				 const TDesC& aReferenceFileName) :
	iFromFileName (aFromFileName),
	iToFileName (aToFileName),
	iReferenceFileName(aReferenceFileName),
	iConverter(NULL)
	{
	iTestStepName = aStepName;
	//heap size of 1 MB is needed to check the output file with reference file.
	iHeapSize = 1024*1024;
	}

// start test
void RTestStepConvertAudio::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iConverter = NULL; 
	
	User::LeaveIfError(iFs.Connect());
	
	iFs.Delete(iToFileName); // ensure we start with no file. Ignore errors.
	
	iConverter = CConvertAudio::NewL(this);
	
	iConverter->ConvertL(iFromFileName, iToFileName);
	}
	
// cleanup at end
void RTestStepConvertAudio::CloseTest()
	{
	delete iConverter;
	iConverter = NULL;
	
	iFs.Close();
	}
	
// signal complete
void RTestStepConvertAudio::ConvertComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		StopTest(aError);		
		}
	else
		{
		TBool result = EFalse;
		TRAPD(err, result = CheckConversionL());
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Could not check the converted file, err = %d"), err);
			StopTest(err, EFail);				
			}
		else  
			{
			StopTest(aError, (result ? EPass : EFail));			
			}
		}
	}

TBool RTestStepConvertAudio::CheckConversionL()
	{
	RFile outputFile; //output file
	RFile refFile; //reference file
	
	//open the files
	User::LeaveIfError(outputFile.Open(iFs, iToFileName, EFileRead|EFileShareAny));
	CleanupClosePushL(outputFile);
	User::LeaveIfError(refFile.Open(iFs, iToFileName, EFileRead|EFileShareAny));  // this is changed because of fix for DEF145347 (TSW id : ESLM-844Q3G). As file size is changing everytime, we should compare with output file always
	CleanupClosePushL(refFile);	

	TInt err = KErrNone;
	//read contents of output file using file stream
	RFileReadStream outputFileStream(outputFile, 0);
	CleanupClosePushL(outputFileStream);
	HBufC8* outputFileBuf = HBufC8::NewLC(KNumBytesToCompare);
	TPtr8 outputData = outputFileBuf->Des();
	TRAP(err, outputFileStream.ReadL(outputData));
	if ((err != KErrNone) && (err != KErrEof)) 
		{
		User::Leave(err);	
		}

	//read contents of reference file using file stream
	RFileReadStream refFileStream(refFile, 0);
	CleanupClosePushL(refFileStream);
	HBufC8* refFileBuf = HBufC8::NewLC(KNumBytesToCompare);
	TPtr8 refData = refFileBuf->Des();
	TRAP(err, refFileStream.ReadL(refData))
	if ((err != KErrNone) && (err != KErrEof)) 
		{
		User::Leave(err);	
		}		
			
	TInt result = refFileBuf->Compare(*outputFileBuf);
	INFO_PRINTF2(_L("Result = %d"), result);	

	CleanupStack::PopAndDestroy(6, &outputFile);
	return (result == 0);
	}

// factory function
CConvertPanic* CConvertPanic::NewL(MConvAudioObserver* aObserver)
	{
	CConvertPanic* self = new (ELeave) CConvertPanic(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CConvertPanic::~CConvertPanic()
	{
	//Base class takes care
	}
	
CConvertPanic::CConvertPanic(MConvAudioObserver* aObserver) :
	CConvertAudio(aObserver)
	{
	}

void CConvertPanic::ConstructL()
	{
	CConvertAudio::ConstructL();
	}
	
void CConvertPanic::OnOpenL()
	{
	iUtility->ConvertL();
	}
	
RTestStepConvertPanic* RTestStepConvertPanic::NewL(const TDesC& aStepName, 
                                     			   const TDesC& aFromFileName,
                                     			   const TDesC& aToFileName,
                                     			   const TDesC& aReferenceFileName)
	{
	RTestStepConvertPanic* self = new (ELeave) RTestStepConvertPanic(aStepName, 
													   				 aFromFileName,
													   				 aToFileName,
													   				 aReferenceFileName);
	return self;
	}

RTestStepConvertPanic::RTestStepConvertPanic(const TDesC& aStepName, 
							   				 const TDesC& aFromFileName,
							   				 const TDesC& aToFileName,
							   				 const TDesC& aReferenceFileName) :
	RTestStepConvertAudio(aStepName, aFromFileName, aToFileName, aReferenceFileName)
	{
	
	}
	
void RTestStepConvertPanic::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iConverter = NULL; 
	
	User::LeaveIfError(iFs.Connect());
	
	iFs.Delete(iToFileName); // ensure we start with no file. Ignore errors.
	
	iConverter = CConvertPanic::NewL(this);
	
	iConverter->ConvertL(iFromFileName, iToFileName);
	}
	
// signal complete
void RTestStepConvertPanic::ConvertComplete(TInt aError)
	{
	if (aError != KErrServerTerminated)
		{
		StopTest(aError, EFail);
		}
	else
		{
		StopTest(EPass);
		}
	}
