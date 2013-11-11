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
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepAudInStream.h"
#include "mmfclientaudioinputstream.h"
#include <mda/common/audio.h>

//
// --------------------------------------------
//
// base class for some audio Input stream tests
//_LIT(KStreamRawFile,"c:\\mm\\mmf\\testfiles\\aclnt\\test.raw");	// EABI warning removal
//_LIT(KStreamIMADFile,"c:\\mm\\mmf\\testfiles\\aclnt\\test.raw");	// EABI warning removal
_LIT(KRecordStreamRawFile,"c:\\mm\\mmf\\testfiles\\aclnt\\streamrecorded.raw");

const TInt KMicroSecsInOneSec = 1000000;
//const TInt KBitsPerSample = 16;	// EABI warning removal
const TInt KBytesPerSample = 2;
const TInt KSampleRate = 8000;

// constant table of parameters for tests
const TStreamInConvParameters KTestParameters[] =
{	
	// straight encode test
	
	{ _S("MM-MMF-ACLNT-U-0740-CP"), KMMFFourCCCodePCMU8},
	{ _S("MM-MMF-ACLNT-U-0741-CP"), KMMFFourCCCodePCM16},
	{ _S("MM-MMF-ACLNT-U-0742-CP"), KMMFFourCCCodeALAW},
	{ _S("MM-MMF-ACLNT-U-0743-CP"), KMMFFourCCCodeMuLAW},
	{ _S("MM-MMF-ACLNT-U-0744-CP"), KMMFFourCCCodeIMAD},
	{ _S("MM-MMF-ACLNT-U-0745-CP"), KMMFFourCCCodePCM16B},
	{ _S("MM-MMF-ACLNT-U-0746-CP"), KMMFFourCCCodeGSM610}
};

CTestStepAudInStream* CTestStepAudInStream::NewL()
	{
	CTestStepAudInStream* s = new(ELeave) CTestStepAudInStream();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudInStream::CTestStepAudInStream() : iError(KErrNone)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0999-CP");
	}

void CTestStepAudInStream::ConstructL()
	{
	// reset the buffer
	for(TInt i=0; i<KRecNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;
	}

CTestStepAudInStream::~CTestStepAudInStream()
	{
	}

void CTestStepAudInStream::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudInStream::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	
	iWriteBufferActive->Cancel();
	TInt filePosition=0;
	iFile.Seek(ESeekStart, filePosition);
	iFile.Close();
	CActiveScheduler::Stop();
	}

void CTestStepAudInStream::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	if(aError != KErrNone)
		{
		return;   
		}
	// Assume aBuffer == iBufferList[iEndBuf]

	// Increment iEndBuf
	if(iEndBuf==KRecNumBuffer-1)
		iEndBuf = 0;
	else
		iEndBuf++;

	// Starting writing out the buffers if we're not already
	if(!iWriteBufferActive->IsActive())
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
	}

void CTestStepAudInStream::StartReadL()
	{
	// Issue reads on all available buffers
	for (TInt ii=0; ii<KRecNumBuffer; ii++)
	    {
		iAudInStream->ReadL(iBufferList[ii]);
	    }
	}

CTestStepAudInStream::CWriteBufferActive::CWriteBufferActive() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudInStream::CWriteBufferActive::~CWriteBufferActive()
	{
	Cancel();
	}

void CTestStepAudInStream::CWriteBufferActive::RunL()
	{
	//ASSERT it wrote OK
	iParent->ProcessDataL();
	}

void CTestStepAudInStream::CWriteBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudInStream::CWriteBufferActive::Write(const TDesC8& aBuffer)
	{
	iParent->iFile.Write(aBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}


void CTestStepAudInStream::ProcessDataL()
	{
	// Set the next buffer to handle
	iBufferList[iStartBuf] = KNullDesC8; // done with that one
	// Issue another read to input stream
	iAudInStream->ReadL(iBufferList[iStartBuf]);
	
	// Increment iStartBuf
	if(iStartBuf==KRecNumBuffer-1)
		iStartBuf=0;
	else
		iStartBuf++;
	
	if (iBufferList[iStartBuf] != KNullDesC8)
		{
		// Still more to write
		iWriteBufferActive->Write(iBufferList[iStartBuf]);
		}
	// else just wait for more recorded data to come through
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStream::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// create the active object
	iWriteBufferActive = new(ELeave) CWriteBufferActive();
	iWriteBufferActive->iParent = this;
	iFs.Connect();

	TRAPD(err, iAudInStream = CMMFMdaAudioInputStream::NewL(*this) );
	

	if (err != KErrNone ||
		iAudInStream == NULL )
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
enum TVerdict CTestStepAudInStream::DoTestStepPostambleL(void)
	{
	iFs.Close();

	delete iAudInStream;
	iAudInStream = NULL;
	delete iWriteBufferActive;
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
TVerdict CTestStepAudInStream::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;

	if (iError != KErrNone )
		 return EFail;

	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		return EFail;

	StartReadL();
	CActiveScheduler::Start();

	iFile.Close();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepAudInStreamNew.
 *
 *
 * @return	"CTestStepAudInStreamNew*"
 *			The constructed CTestStepAudInStreamNew
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamNew* CTestStepAudInStreamNew::NewL()
	{
	CTestStepAudInStreamNew* self = new(ELeave) CTestStepAudInStreamNew;
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
CTestStepAudInStreamNew::CTestStepAudInStreamNew()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0700-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamNew::~CTestStepAudInStreamNew()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamNew::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamNew::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamNew::MaiscRecordComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamNew::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamNew::DoTestStepPostambleL(void)
	{
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
TVerdict CTestStepAudInStreamNew::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0700-CP"));
	INFO_PRINTF1(_L("this test is initialising an Audio Input Stream Utility Class"));

	iAudInStream = CMMFMdaAudioInputStream::NewL(*this);

	if ( iAudInStream == NULL )
		 iTestStepResult = EFail;
	else 
		 iTestStepResult = EPass;

	delete iAudInStream;
	iAudInStream = NULL;
	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamSetProp.
 *
 *
 * @return	"CTestStepAudInStreamSetProp*"
 *			The constructed CTestStepAudInStreamSetProp
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetProp* CTestStepAudInStreamSetProp::NewL()
	{
	CTestStepAudInStreamSetProp* self = new(ELeave) CTestStepAudInStreamSetProp;
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
CTestStepAudInStreamSetProp::CTestStepAudInStreamSetProp()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0701-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetProp::~CTestStepAudInStreamSetProp()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamSetProp::MaiscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
void CTestStepAudInStreamSetProp::MaiscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	}
void CTestStepAudInStreamSetProp::MaiscRecordComplete(TInt aError)
	{
	iError = aError;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamSetProp::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0701-CP"));
	INFO_PRINTF1(_L("this test is setting the audio properties of an Audio Input Stream Utility Class"));

	iAudInStream = CMMFMdaAudioInputStream::NewL(*this);
	if (iAudInStream == NULL)
		return EInconclusive;
	 
	iAudInStream->Open(NULL);
		CActiveScheduler::Start();
	
	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamSetProp::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
	//CActiveScheduler::Stop();
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
TVerdict CTestStepAudInStreamSetProp::DoTestStepL()
	{
	//TMdaAudioDataSettings settings;
		//	settings.Query();
		//	settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz; // ESampleRateAnyInRange
		//	settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
		//	settings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;
		//	settings.iVolume = 0;
		
	
	
	iTestStepResult = EPass;
	TInt err = KErrNone;

	// test 1
	TRAP(err, iAudInStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate8000Hz,TMdaAudioDataSettings::EChannelsMono) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		 return EFail;

	// test2
	TRAP(err, iAudInStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate44100Hz,TMdaAudioDataSettings::EChannelsStereo) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		 return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamOpen.
 *
 *
 * @return	"CTestStepAudInStreamOpen*"
 *			The constructed CTestStepAudInStreamOpen
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamOpen* CTestStepAudInStreamOpen::NewL()
	{
	CTestStepAudInStreamOpen* self = new(ELeave) CTestStepAudInStreamOpen;
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
CTestStepAudInStreamOpen::CTestStepAudInStreamOpen()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0702-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamOpen::~CTestStepAudInStreamOpen()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamOpen::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler and NEWLs the CMMFMdaAudioInputStream
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0702-CP"));
	INFO_PRINTF1(_L("this test is openning an Audio Input Stream Utility Class"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamOpen::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamOpen::DoTestStepL()
	{
	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz; // ESampleRateAnyInRange
	settings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
	settings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;
	settings.iVolume = 0;
	
	//Test Open with audio settings
	iAudInStream->Open(&settings);
	CActiveScheduler::Start();

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudInStreamStopped(iAudInStream, stopped);
	
	if (iError != KErrNone || err != KErrNone || !stopped )
		{
		INFO_PRINTF1(_L("Open with audio settings failed"));
		return EFail;
		}
	
	//Test Open with NULL
	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	stopped = EFalse;
	err = GetAudInStreamStopped(iAudInStream, stopped);
	
	if (iError != KErrNone || err != KErrNone || !stopped )
		{
		INFO_PRINTF1(_L("Open without audio settings failed"));
		return EFail;
		}
	else
		{
		return EPass;
		}
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamSetGain.
 *
 *
 * @return	"CTestStepAudInStreamSetGain*"
 *			The constructed CTestStepAudInStreamSetGain
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetGain* CTestStepAudInStreamSetGain::NewL()
	{
	CTestStepAudInStreamSetGain* self = new(ELeave) CTestStepAudInStreamSetGain;
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
CTestStepAudInStreamSetGain::CTestStepAudInStreamSetGain()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0703-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetGain::~CTestStepAudInStreamSetGain()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamSetGain::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamSetGain::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamSetGain::MaiscRecordComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamSetGain::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0703-CP"));
	INFO_PRINTF1(_L("this test is setting/getting the Gain of an Audio Input Stream Utility Class"));

	iAudInStream = CMMFMdaAudioInputStream::NewL(*this);

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamSetGain::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;
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
TVerdict CTestStepAudInStreamSetGain::DoTestStepL()
	{
	TInt gain = iAudInStream->MaxGain()/2;
	if (gain < 0)
		return EFail;
	//set the volume
	iAudInStream->SetGain(gain);

	//get the volume
	TInt rgain = iAudInStream->Gain();
	if (gain != rgain ) 
		 return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamSetPrior.
 *
 *
 * @return	"CTestStepAudInStreamSetPrior*"
 *			The constructed CTestStepAudInStreamSetPrior
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetPrior* CTestStepAudInStreamSetPrior::NewL()
	{
	CTestStepAudInStreamSetPrior* self = new(ELeave) CTestStepAudInStreamSetPrior;
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
CTestStepAudInStreamSetPrior::CTestStepAudInStreamSetPrior()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0708-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetPrior::~CTestStepAudInStreamSetPrior()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamSetPrior::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamSetPrior::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamSetPrior::MaiscRecordComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamSetPrior::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0708-HP"));
	INFO_PRINTF1(_L("this test is setting the audio priorities of an Audio Input Stream Utility Class"));

	iAudInStream = CMMFMdaAudioInputStream::NewL(*this);

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamSetPrior::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;

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
TVerdict CTestStepAudInStreamSetPrior::DoTestStepL()
	{
//	TInt err = KErrNone;

	// test 1
	iAudInStream->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTime);

/*	TMMFPrioritySettings prior;
	err = GetAudInStreamPrioritySettings(iAudInStream, prior);
	if ( err != KErrNone || 
		 prior.iPriority != EMdaPriorityMin ||
		 prior.iPref != EMdaPriorityPreferenceTime)
		 return EFail;
*/
	// test2
	iAudInStream->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceQuality);

/*	err = GetAudInStreamPrioritySettings(iAudInStream, prior);
	if ( err != KErrNone || 
		 prior.iPriority != EMdaPriorityMax ||
		 prior.iPref != EMdaPriorityPreferenceQuality)
		 return EFail;
*/
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamSetBalance.
 *
 *
 * @return	"CTestStepAudInStreamSetBalance*"
 *			The constructed CTestStepAudInStreamSetBalance
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetBalance* CTestStepAudInStreamSetBalance::NewL()
	{
	CTestStepAudInStreamSetBalance* self = new(ELeave) CTestStepAudInStreamSetBalance;
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
CTestStepAudInStreamSetBalance::CTestStepAudInStreamSetBalance()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0706-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamSetBalance::~CTestStepAudInStreamSetBalance()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamSetBalance::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamSetBalance::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamSetBalance::MaiscRecordComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamSetBalance::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	 TInt err = KErrNone;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0706-HP"));
	INFO_PRINTF1(_L("this test is setting and getting the balance of an Audio Input Stream Utility Class"));

	TRAP(err, iAudInStream = CMMFMdaAudioInputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudInStream == NULL )
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
enum TVerdict CTestStepAudInStreamSetBalance::DoTestStepPostambleL(void)
	{
	delete iAudInStream;
	iAudInStream = NULL;

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
TVerdict CTestStepAudInStreamSetBalance::DoTestStepL()
	{
	TInt bal = 0;

	TRAPD(err, iAudInStream->SetBalanceL()); // default KMMFBalanceCenter
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceCenter )
		return EFail;

	TRAP(err, iAudInStream->SetBalanceL(KMMFBalanceMaxRight));
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceMaxRight )
		return EFail;

	TRAP(err, iAudInStream->SetBalanceL(KMMFBalanceMaxLeft));
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudInStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceMaxLeft )
		return EFail;

	// In of bounds
	/*
	iAudInStream->SetBalance(KMMFBalanceMaxLeft+10);
	//check condition
	*/

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamRead.
 *
 *
 * @return	"CTestStepAudInStreamRead*"
 *			The constructed CTestStepAudInStreamRead
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamRead* CTestStepAudInStreamRead::NewL(TUint aTestIndex)
	{
	CTestStepAudInStreamRead* self = new(ELeave) CTestStepAudInStreamRead(aTestIndex);
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
CTestStepAudInStreamRead::CTestStepAudInStreamRead(TUint aTestIndex)
	{
	iTestStepResult = EPass;
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	iHeapSize = 131072;		//	~128k
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamRead::~CTestStepAudInStreamRead()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamRead::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	// this installs the scheduler, and NEWLs the input stream object
	 verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this test is reading data from an Audio Input Stream Utility Class. ReadL()"));

	TInt err = KErrNone;
	TRAP(err, iAudInStream->SetDataTypeL(iTestParameters->iEncoding));

	if (err != KErrNone)
		 return EInconclusive;

	iAudInStream->Open(NULL);
	
	CActiveScheduler::Start();

	if (iError != KErrNone)
		 return EInconclusive;

	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
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
enum TVerdict CTestStepAudInStreamRead::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
	}

LOCAL_C TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return FALSE;
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
TVerdict CTestStepAudInStreamRead::DoTestStepL()
	{
	StartReadL();

	CCallBackTimer* timer  = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	timer->After(5000000); // 5s
	CActiveScheduler::Start();
	delete timer;
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Test failed (iError=%d)"), iError);
		return EFail;
		}

	iAudInStream->Stop();
	// Now playback what we've recorded

	CMdaAudioOutputStream* outputStream = CMdaAudioOutputStream::NewL(*this);
	CleanupStack::PushL(outputStream);
	TRAPD(err, outputStream->SetDataTypeL(iTestParameters->iEncoding));
	
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(1, outputStream);
		return EInconclusive;
		}
	outputStream->Open(NULL);
	outputStream->SetVolume(outputStream->MaxVolume());

	CActiveScheduler::Start(); // wait for open to complete
	TInt size = 0;

	User::LeaveIfError(iFile.Size(size));

	// Read the entire file into a buffer - bit naughty but OK for test code
	INFO_PRINTF2(_L("HBufC8:NewLC(size):  size=%d"), size);
	HBufC8* buffer = HBufC8::NewLC(size);

	INFO_PRINTF1(_L("Buffer created"));

	TInt pos = 0;
	User::LeaveIfError(iFile.Seek(ESeekStart, pos));
	TPtr8 bufPtr = buffer->Des();
	User::LeaveIfError(iFile.Read(bufPtr, size));
	outputStream->WriteL(bufPtr);
	iIsPlaying = ETrue;
	CActiveScheduler::Start(); // wait for the play to complete
	CleanupStack::PopAndDestroy(2); // buffer, outputStream
	if (iError != KErrNone && iError != KErrUnderflow)
		 return EInconclusive; // There may be a problem with the output stream rather than the input stream

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

void CTestStepAudInStreamRead::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudInStreamRead::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	// Don't care
	}

void CTestStepAudInStreamRead::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	if (iIsPlaying)
		{
		CActiveScheduler::Stop();
		iIsPlaying = EFalse;
		}
	}

void CTestStepAudInStreamRead::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudInStreamRead::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamStop.
 *
 *
 * @return	"CTestStepAudInStreamStop*"
 *			The constructed CTestStepAudInStreamStop
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamStop* CTestStepAudInStreamStop::NewL()
	{
	CTestStepAudInStreamStop* self = new(ELeave) CTestStepAudInStreamStop;
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
CTestStepAudInStreamStop::CTestStepAudInStreamStop()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0710-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamStop::~CTestStepAudInStreamStop()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamStop::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler, and NEWLs the input stream object
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0710-CP"));
	INFO_PRINTF1(_L("this test is stopping a playing Audio Input Stream Utility Class: Stop()"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		 return EInconclusive;

	TInt err = KErrNone;
	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the input stream
	StartReadL();
	CCallBackTimer* timer  = CCallBackTimer::NewL(TCallBack(stopActiveScheduler, iAudInStream));
	timer->After(1000000); // 1s
	CActiveScheduler::Start();
	delete timer;
	if (iError != KErrNone)
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
enum TVerdict CTestStepAudInStreamStop::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamStop::DoTestStepL()
	{
	iAudInStream->Stop();

	TInt err = KErrNone;
	TBool stopped = EFalse;
	err = GetAudInStreamStopped(iAudInStream, stopped);
	if (err != KErrNone || 
		!stopped )
		 return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamPosition.
 *
 *
 * @return	"CTestStepAudInStreamPosition*"
 *			The constructed CTestStepAudInStreamPosition
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamPosition* CTestStepAudInStreamPosition::NewL()
	{
	CTestStepAudInStreamPosition* self = new(ELeave) CTestStepAudInStreamPosition;
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
CTestStepAudInStreamPosition::CTestStepAudInStreamPosition()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0711-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamPosition::~CTestStepAudInStreamPosition()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamPosition::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler, and NEWLs the input stream object
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0711-CP"));
	INFO_PRINTF1(_L("this test is gettig the position of a playing Audio Input Stream Utility Class: Position()"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		 return EInconclusive;

	TInt err = KErrNone;
	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the input stream
	StartReadL();
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;
	if (iError != KErrNone)
		return EInconclusive;

	TBool isRecording = EFalse;
	err = GetAudInStreamRecording(iAudInStream, isRecording);
	if (err != KErrNone || 
		!isRecording)
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
enum TVerdict CTestStepAudInStreamPosition::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamPosition::DoTestStepL()
	{
	TInt err = KErrNone;
	TInt bytes = 0;

	// here the purpose is to test if the Position() function returns the 
	// correct position with respect to the clip byte-stream position, i.e.
	// the bytes that have been recorded so far, expressed in time (via sample rate)
	// and not the actual real-time time difference between the beginning of the 
	// recording and the moment we apply Position().
	// The stream might take longer actual time to be recorded, e.g. start delay, breaks, etc.
	// however in this case streaming from AudioInput(microphone) should be consistent
	// but it's not the purpose of this unit test to check that it records fine and within 
	// certain time limits.

	// get the position
	TTimeIntervalMicroSeconds pos1 = iAudInStream->Position();
	bytes = iAudInStream->GetBytes();
	TInt64 estimPos = (TInt64(bytes) * KMicroSecsInOneSec) / (TInt64(KSampleRate) * TInt64(KBytesPerSample));
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	const TInt KWaitTime = 1000000;
	//const TInt KTolerence = 300000; // Should be able to do better than this!
	callBackTimer->After(KWaitTime);
	// wait a bit
	CActiveScheduler::Start();
	delete callBackTimer;
	if (iError != KErrNone)
		return EInconclusive;

	bytes = iAudInStream->GetBytes();
	estimPos = (TInt64(bytes) * KMicroSecsInOneSec) / (TInt64(KSampleRate) * TInt64(KBytesPerSample));
	// get the new position
	TTimeIntervalMicroSeconds pos2 = iAudInStream->Position();
	if (pos2 != estimPos)
		return EFail;
	TInt64 diff = pos2.Int64()-pos1.Int64();
	INFO_PRINTF2(_L("position diff: %d"), diff);
	//if (diff < (KWaitTime-KTolerence) || diff > (KWaitTime+KTolerence))
	//	return EFail;
	// stop the input stream
	iAudInStream->Stop();
	TBool isStopped;
	err = GetAudInStreamStopped(iAudInStream, isStopped);
	if (err != KErrNone || 
		!isStopped )
		 return EFail;

	// get the new position
	TTimeIntervalMicroSeconds pos = iAudInStream->Position();
	if (pos.Int64() != 0)
		 return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamGetBytes.
 *
 *
 * @return	"CTestStepAudInStreamGetBytes*"
 *			The constructed CTestStepAudInStreamGetBytes
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamGetBytes* CTestStepAudInStreamGetBytes::NewL()
	{
	CTestStepAudInStreamGetBytes* self = new(ELeave) CTestStepAudInStreamGetBytes;
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
CTestStepAudInStreamGetBytes::CTestStepAudInStreamGetBytes()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0712-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamGetBytes::~CTestStepAudInStreamGetBytes()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamGetBytes::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler, and NEWLs the input stream object
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0712-CP"));
	INFO_PRINTF1(_L("this test gets the bytes rendered by the H/W until this moment: GetBytes()"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		 return EInconclusive;

	TInt err = KErrNone;
	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the input stream
	StartReadL();
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;
	if (iError != KErrNone)
		return EInconclusive;

	TBool isRecording = EFalse;
	err = GetAudInStreamRecording(iAudInStream, isRecording);
	if (err != KErrNone || 
		!isRecording)
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
enum TVerdict CTestStepAudInStreamGetBytes::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamGetBytes::DoTestStepL()
	{
	TInt bytes1 = iAudInStream->GetBytes();
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(2000000);	
	CActiveScheduler::Start();
	callBackTimer->Cancel(); // in case something else stopped the AS
	if (iError != KErrNone)
		return EInconclusive;

	TInt bytes2 = iAudInStream->GetBytes();
	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	delete callBackTimer;
	if (iError != KErrNone)
		return EInconclusive;

	iAudInStream->Stop();
	TInt bytes3 = iAudInStream->GetBytes();
	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	
	if (bytes1 >= bytes2 || bytes2 >= bytes3)
		return EFail;

	if (bytes3 != fileSize)
		return EFail;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamDataType.
 *
 *
 * @return	"CTestStepAudInStreamGetSetDataType*"
 *			The constructed CTestStepAudInStreamDataType
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamDataType* CTestStepAudInStreamDataType::NewL()
	{
	CTestStepAudInStreamDataType* self = new(ELeave) CTestStepAudInStreamDataType;
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
CTestStepAudInStreamDataType::CTestStepAudInStreamDataType()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0730-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamDataType::~CTestStepAudInStreamDataType()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamDataType::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;

	CTestStepAudInStream::DoTestStepPreambleL();

	return iTestStepResult;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamDataType::DoTestStepPostambleL(void)
	{
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamDataType::DoTestStepL()
	{
	iTestStepResult = EPass;

	RArray<TFourCC> codecs;
	codecs.Append(KMMFFourCCCodePCMU8);
	codecs.Append(KMMFFourCCCodePCM16);
	codecs.Append(KMMFFourCCCodeALAW);
	codecs.Append(KMMFFourCCCodeMuLAW);
	codecs.Append(KMMFFourCCCodeIMAD);
	codecs.Append(KMMFFourCCCodePCM16B);
	codecs.Append(KMMFFourCCCodeGSM610);

	CMdaAudioInputStream* inStream = CMdaAudioInputStream::NewL(*this);

	TInt numCodecs = codecs.Count();
	for(TInt i=0; i<numCodecs; i++)
		{
		TRAPD(err, inStream->SetDataTypeL(codecs[i]));
		if (err != KErrNone)
			{
			iTestStepResult = EFail;
			break;
			}
		TFourCC fourCCReceived = inStream->DataType();
		if (codecs[i] != fourCCReceived)
			{
			iTestStepResult = EFail;
			break;
			}
		}
	delete inStream;
	codecs.Close();
	return iTestStepResult;
	}

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepAudInStreamMemLeak.
 *
 *
 * @return	"CTestStepAudInStreamMemLeak*"
 *			The constructed CTestStepAudInStreamMemLeak
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamMemLeak* CTestStepAudInStreamMemLeak::NewL()
	{
	CTestStepAudInStreamMemLeak* self = new(ELeave) CTestStepAudInStreamMemLeak;
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
CTestStepAudInStreamMemLeak::CTestStepAudInStreamMemLeak()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0510-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamMemLeak::~CTestStepAudInStreamMemLeak()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudInStreamMemLeak::MaiscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudInStreamMemLeak::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudInStreamMemLeak::MaiscRecordComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamMemLeak::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamMemLeak::DoTestStepPostambleL(void)
	{
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
TVerdict CTestStepAudInStreamMemLeak::DoTestStepL()
	{
	__MM_HEAP_MARK;

	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this test creates & then deletes an Audio Input Stream Utility Class"));
	INFO_PRINTF1(_L("and then checks for any memory leak"));

	iAudInStream = CMdaAudioInputStream::NewL(*this);

	if ( iAudInStream == NULL )
		 iTestStepResult = EFail;
	else 
		 iTestStepResult = EPass;

	delete iAudInStream;
	iAudInStream = NULL;
	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;

	}

// --------------------------------------------

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
RTestStepAudInStreamBitRate::RTestStepAudInStreamBitRate()
	{
	iTestVerdictSet = EFalse;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0511-HP");
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void RTestStepAudInStreamBitRate::MaiscOpenComplete(TInt aError)
	{
	TBuf<256> message;
	if (aError != KErrNone)
		{
		message.Format(_L("Open failed unexpectedly with error %d - aborting test"), aError);
		SetSectionFailed(message);
		CActiveScheduler::Stop();
		}
	else
		{
		TestBitRates();
		}
	}
void RTestStepAudInStreamBitRate::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void RTestStepAudInStreamBitRate::MaiscRecordComplete(TInt /*aError*/)
	{
	}
	
TVerdict RTestStepAudInStreamBitRate::OpenL()
	{
	// create and install the active scheduler
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	return EPass;
	}
	
TVerdict RTestStepAudInStreamBitRate::DoTestStepL()
	{
	iAudInStream = CMdaAudioInputStream::NewL(*this);		
	iAudInStream->Open(NULL);
	CActiveScheduler::Start();
	__ASSERT_ALWAYS(iTestVerdictSet, User::Panic(_L("Test verdict not set"),0));
	return iTestStepResult;
	}
	
const TInt KTestBitRate = 48000 * 16;
_LIT(KMessageNotReadyPass, "Received KErrNotReady (As Expected: H/W device has not yet been fully initialised)");
_LIT(KMessageUnexpectedErrorFail, "Unexpected error: %d");

void RTestStepAudInStreamBitRate::TestBitRates()
	{
	TBuf<256> message;
	TInt err = KErrNone;
	
	// this will invoke the bitrate plugin but will not have
	// initialised the datapath therefore we expect a KErrNotReady
	// error from each call
	
	TRAP(err, iAudInStream->BitRateL());

	if (err != KErrNotReady)
		{
		message.Format(KMessageUnexpectedErrorFail,err);
		SetSectionFailed(message);
		}
	else
		{
		message.Format(KMessageNotReadyPass);
		SetSectionPassed(message);
		}
		
	RArray<TInt> array;
	array.AppendL(KTestBitRate); // set a dummy value
	TRAP(err, iAudInStream->GetSupportedBitRatesL(array));
	if (array.Count() != 0)
		{
		SetSectionFailed(_L("array not reset after call to GetSupportedBitRates"));
		}
	array.Close();

	if (err != KErrNotReady)
		{
		message.Format(KMessageUnexpectedErrorFail,err);
		SetSectionFailed(message);
		}
	else
		{
		message.Format(KMessageNotReadyPass);
		SetSectionPassed(message);
		}

	TRAP(err, iAudInStream->SetBitRateL(KTestBitRate));
	if (err != KErrNotReady)
		{
		message.Format(KMessageUnexpectedErrorFail,err);
		SetSectionFailed(message);
		}
	else
		{
		message.Format(KMessageNotReadyPass);
		SetSectionPassed(message);
		}
	// finished with test
	CActiveScheduler::Stop();
	}


 void RTestStepAudInStreamBitRate::SetSectionPassed(const TDesC& aMessage)
	{
	INFO_PRINTF2(_L("%S"),&aMessage);
		
	if (!iTestVerdictSet)
		{
		iTestStepResult = EPass;
		iTestVerdictSet = ETrue;
		}
	if (iTestStepResult == EPass)
		{
		INFO_PRINTF1(_L("Section Passed - Overall Verdict PASS"));
		}
	else
		{
		INFO_PRINTF1(_L("Section Passed - Overall Verdict FAIL"));	
		}
	}
	
void RTestStepAudInStreamBitRate::SetSectionFailed(const TDesC& aMessage)
	{
	// A fail will always cause the whole test to fail
	iTestStepResult = EFail;
	INFO_PRINTF2(_L("%S"),&aMessage);
	INFO_PRINTF1(_L("Section Failed - Overall Verdict FAIL"));	
	iTestVerdictSet = ETrue;
	}


void RTestStepAudInStreamBitRate::Close()
	{
	delete iAudInStream;
	iAudInStream = NULL;
	}
	


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudInStreamTestBitrate.
 *
 *
 * @return	"CTestStepAudInStreamTestBitrate*"
 *			The constructed CTestStepAudInStreamTestBitrate
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamTestBitrate* CTestStepAudInStreamTestBitrate::NewL()
	{
	CTestStepAudInStreamTestBitrate* self = new(ELeave) CTestStepAudInStreamTestBitrate;
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
CTestStepAudInStreamTestBitrate::CTestStepAudInStreamTestBitrate()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0747-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudInStreamTestBitrate::~CTestStepAudInStreamTestBitrate()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudInStreamTestBitrate::DoTestStepPreambleL(void)
	{
	iTestStepResult = EPass;
	enum TVerdict verdict;
	// this installs the scheduler, and NEWLs the input stream object
	verdict = CTestStepAudInStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0747-CP"));
	INFO_PRINTF1(_L("this tests the bitrate custom interface implemented in DevSound"));

	iAudInStream->Open(NULL);
	CActiveScheduler::Start();

	if (iError != KErrNone)
		{
		return EInconclusive;
		}

	TInt err = KErrNone;
	err = iFile.Replace( iFs, KRecordStreamRawFile, EFileWrite ) ;
	if (err != KErrNone)
		{
		return EInconclusive;
		}

	// start feeding the input stream
	StartReadL();
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	CActiveScheduler::Start();
	delete idleStopper;
	if (iError != KErrNone)
		{
		return EInconclusive;
		}

	TBool isRecording = EFalse;
	err = GetAudInStreamRecording(iAudInStream, isRecording);
	if (err != KErrNone || !isRecording)
		{
		return EInconclusive;
		}

	User::After(1000000);
	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudInStreamTestBitrate::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler ]
	return CTestStepAudInStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudInStreamTestBitrate::DoTestStepL()
	{
	TBool test1 = EFalse;
	TBool test2 = EFalse;
	TBool test3 = EFalse;
	
	// fetch the bitrate
	TInt bitRate = 0;
	TRAPD(err, bitRate = iAudInStream->BitRateL());

	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("BitRateL() returned %d"), bitRate);
		test1 = ETrue;				
		}
	else
		{
		INFO_PRINTF2(_L("BitRateL() failed with error %d"), err);
		}
	
	// even if test is fail we keep going so as to test APIs
	// get the supported bitrates from the sound device
	RArray<TInt> array;
	TRAP(err, iAudInStream->GetSupportedBitRatesL(array));
	
	if ((err == KErrNone) && (array.Count() > 0))
		{
		INFO_PRINTF2(_L("GetSupportedBitRatesL() returned %d result"), array.Count());
		test2 = ETrue;
		}
	else if (err != KErrNone) 
		{
		INFO_PRINTF2(_L("GetSupportedBitRatesL() failed with error %d"), err);
		}
	else if (array.Count() == 0)
		{
		INFO_PRINTF2(_L("GetSupportedBitRatesL() returned no supported bitrates!"), err);
		}
	
	// set a required bitrate - we assume num of channels
	// has already been set as mono (1)
	TRAP(err, iAudInStream->SetBitRateL(KTestBitRate));
	
	if (err == KErrNone)
		{
		INFO_PRINTF2(_L("SetBitRateL() set successfully at %d"), KTestBitRate);
		test3 = ETrue;
		}
	else
		{
		INFO_PRINTF2(_L("SetBitRateL() failed with error %d"), err);
		}
	
	iTestStepResult = EFail;
	
	if (test1 && test2 && test3)
		{
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//

RTestStepAudInStreamBitRateOOM* RTestStepAudInStreamBitRateOOM::NewL()
	{
	RTestStepAudInStreamBitRateOOM* self = new(ELeave) RTestStepAudInStreamBitRateOOM;
	return self;
	}
	
RTestStepAudInStreamBitRateOOM::RTestStepAudInStreamBitRateOOM()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0748-CP");
	}
	
RTestStepAudInStreamBitRateOOM::~RTestStepAudInStreamBitRateOOM()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void RTestStepAudInStreamBitRateOOM::MaiscOpenComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF2(_L("Open failed unexpectedly with error %d - aborting test"), aError);
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}
void RTestStepAudInStreamBitRateOOM::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void RTestStepAudInStreamBitRateOOM::MaiscRecordComplete(TInt /*aError*/)
	{
	}
	
	
TVerdict RTestStepAudInStreamBitRateOOM::TestBitRateInterfaceL()	
	{
	// create the stream
	CMMFMdaAudioInputStream* inStream = CMMFMdaAudioInputStream::NewL(*this);
	CleanupStack::PushL(inStream);
	
	// open it and start the scheduler
	inStream->Open(NULL);
	CActiveScheduler::Start();

		
	// we trap all calls to catch the KErrNotReady error
	// now fetch the bitrate
	TRAPD(err, inStream->BitRateL());
	if (err != KErrNone && err != KErrNotReady)
		{
		User::Leave(err);
		}

	// fetch a list of bitrates
	RArray<TInt> array;
	TRAP(err, inStream->GetSupportedBitRatesL(array));
	if (err != KErrNone && err != KErrNotReady)
		{
		User::Leave(err);
		}
	
	// set a required bitrate - we assume num of channels
	// has already been set as mono (1)
	TRAP(err, inStream->SetBitRateL(KTestBitRate))
	if (err != KErrNone && err != KErrNotReady)
		{
		User::Leave(err);
		}
	
	// clean up
	CleanupStack::PopAndDestroy(inStream);
	inStream = NULL;
	
	return EPass;
	}



TVerdict RTestStepAudInStreamBitRateOOM::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0748-CP"));
	INFO_PRINTF1(_L("this tests the bitrate custom interface implemented in DevSound for OOM"));

	//__MM_HEAP_MARK;
	TVerdict testRes = EFail;
	TRAP( err, testRes = TestBitRateInterfaceL());
	//__MM_HEAP_MARKEND;

	if (err != KErrNone || testRes == EFail)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;
		
		testRes = EFail;
		TRAP( err, testRes = TestBitRateInterfaceL());

		completed = EFalse;
		if (err == KErrNone)
			{
			if (testRes == EPass)
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
				completed = reachedEnd;
				}
			else
				{
				// the test failed but we have already proved above
				// that the required plugins are present
				// therefore we assume that the interface returned
				// a NULL pointer and so we can continue to test
				// until the timeout or the fail count exceeds
				completed = EFalse;
				}
			}
		else if (err == KErrNotSupported)
			{
			// the test has returned not supported but we already
			// know that the interface *is* supported so we assume
			// that the interface creation failed due to lack of
			// memory and so we continue to test until the timeout
			// or we exceed the fail count
			completed = EFalse;
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}
				
		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		// if we have completed successfully or we have exceeded
		// a 1000 failed ALLOC attempts then quit
		if (completed || failCount > 1000)
			break; // exit loop

		failCount++;
		}

	// finished with test

	if (err != KErrNone)
		{
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
	
	

//
// RAsyncTestStep Tests
//

/**
 * Base class common implementation 
 **/
 
CTestStepAudInStreamAsyncTestStep::CTestStepAudInStreamAsyncTestStep(CTestSuite* aSuite)
// Using zero initialisation
//	iTestState (EStateOpen)
//	iBufferList
//	iBufferCount
//	iAudInStream
//	iTrigger
	{
	iSuite = aSuite;
	//Default heap size is 64K. Increased to 128K to avoid the KErrNoMemory for this test step.
	iHeapSize = 1024*128;
	}

void CTestStepAudInStreamAsyncTestStep::KickoffTestL()
	{
	iTrigger = CFsmTrigger::NewL(*this);
	iAudInStream = CMdaAudioInputStream::NewL(*this);
	
	// Reset the buffer. 
	// We need them empty as could test whether data has been placed into them to obtain some stats.
	for(TInt i=0; i<KNumBuffers; i++)
		{
		iBufferList[i] = KNullDesC8;
		}

	// start the timer
	iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
	iTrigger->StartTimer(500000);
	}
		
// cleanup at end
void CTestStepAudInStreamAsyncTestStep::CloseTest()
	{
	if (iTrigger)
		{
		iTrigger->Cancel();
		delete iTrigger;
		}
	
	if (iAudInStream)
		{
		delete iAudInStream;
		}
	}

void CTestStepAudInStreamAsyncTestStep::MaiscOpenComplete(TInt aError)
	{
	if (aError != KErrNone)
		{
		INFO_PRINTF2(_L("*MaiscOpenComplete* Error: %d."), aError);
		StopTest(aError, EFail);
		}
	}

void CTestStepAudInStreamAsyncTestStep::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{	
	if (aBuffer.Length())
		{
		++iBufferCount;
		}

	if (aError != KErrNone)
		{
		INFO_PRINTF2(_L("*MaiscBufferCopied* Error: %d."), aError);
		
		// KErrAbort when we Stop the device, so valid
		if (aError != KErrAbort)
			{
			StopTest(aError, EFail);			
			}
		}
	}

void CTestStepAudInStreamAsyncTestStep::MaiscRecordComplete(TInt aError)
	{
	INFO_PRINTF2(_L("*MaiscRecordComplete* GetBytes reports %d bytes recorded."), iAudInStream->GetBytes());

	if (aError != KErrNone)
		{
		INFO_PRINTF2(_L("*MaiscRecordComplete* Error: %d."), aError);
		StopTest(aError, EFail);
		}
	}

//
/**
 * Implementation of AO timer used to invoke test processing at desired intervals (i.e. State Machine)
 **/

CFsmTrigger* CFsmTrigger::NewL(CTestStepAudInStreamAsyncTestStep& aObserver)
	{
	CFsmTrigger* self = new (ELeave) CFsmTrigger(aObserver);	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	
	return self;
	}

CFsmTrigger::CFsmTrigger(CTestStepAudInStreamAsyncTestStep& aObserver)
:	CActive(CActive::EPriorityStandard),
	iObserver (aObserver)
	{
	// nothing to do
	}

void CFsmTrigger::ConstructL()
	{
	User::LeaveIfError (iTimer.CreateLocal());
	CActiveScheduler::Add (this);
	}
	
CFsmTrigger::~CFsmTrigger()
	{
	Cancel();
	iTimer.Close();
	}
	
void CFsmTrigger::RunL()
	{
	// need to call Fsm
	iObserver.Fsm();
	}

void CFsmTrigger::DoCancel()
	{
	iTimer.Cancel();
	}

void CFsmTrigger::StartTimer(TTimeIntervalMicroSeconds32 aDelay)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(_L("CFsmTrigger"), 1));
	iTimer.After(iStatus, aDelay);
	SetActive();
	}

//
/**
 * Implementation of RAsyncTestStep used to test the RequestStop() api
 **/

CTestStepAudInStreamTestReqStop* CTestStepAudInStreamTestReqStop::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepAudInStreamTestReqStop(aSuite);
	}

CTestStepAudInStreamTestReqStop::CTestStepAudInStreamTestReqStop(CTestSuite* aSuite)
: CTestStepAudInStreamAsyncTestStep(aSuite)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0749-CP");
	}

void CTestStepAudInStreamTestReqStop::KickoffTestL()
	{
	INFO_PRINTF1(_L("DEF095804 CMdaAudioInputStream loses not complete buffers from DevSound"));
	INFO_PRINTF1(_L("Calls RequestStop to ensure no data is lost."));

	CTestStepAudInStreamAsyncTestStep::KickoffTestL();
	}
		
// cleanup at end
void CTestStepAudInStreamTestReqStop::CloseTest()
	{
	CTestStepAudInStreamAsyncTestStep::CloseTest();
	}

void CTestStepAudInStreamTestReqStop::MaiscOpenComplete(TInt aError)
	{
	CTestStepAudInStreamAsyncTestStep::MaiscOpenComplete(aError);
	}

void CTestStepAudInStreamTestReqStop::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{	
	CTestStepAudInStreamAsyncTestStep::MaiscBufferCopied(aError, aBuffer);
	
	if (aError == KErrAbort)
		{
		INFO_PRINTF2(_L("*MaiscBufferCopied* Error EXPECTED. We have been Stopped."), aError);			
		INFO_PRINTF2(_L("*MaiscBufferCopied* Data in this buffer: %d bytes."), aBuffer.Length());
		INFO_PRINTF2(_L("*MaiscBufferCopied* GetBytes reports %d bytes recorded."), iAudInStream->GetBytes());
		}
	}

void CTestStepAudInStreamTestReqStop::MaiscRecordComplete(TInt aError)
	{
	CTestStepAudInStreamAsyncTestStep::MaiscRecordComplete(aError);
	}

void CTestStepAudInStreamTestReqStop::Fsm()
	{
	switch (iTestState)
		{
	case EStateOpen:
		{
		iAudInStream->Open(NULL);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateRecordInit;
		break;
		}
	case EStateRecordInit:
		{
		// start the recording process
		for (TInt i=0; i<KNumBuffers; i++)
			{
			iAudInStream->ReadL(iBufferList[i]);
			}
		
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);
		iTestState = EStateRecord;
		break;
		}
	case EStateRecord:
		{	
		// Just wating time here...for the record process to continue
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);
		iTestState = EStateRequestStop;
		break;
		}
	case EStateRequestStop:
		{		
		// stop the recording process
		iAudInStream->RequestStop();	

		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);

		// report the number of bytes read
		INFO_PRINTF2(_L("State: EStateRequestStop - GetBytes reports %d bytes recorded."), iAudInStream->GetBytes());
		iTestState = EStateStopTest;
		break;			
		}
	case EStateStopTest:
		{
		INFO_PRINTF3(_L("Used %d buffers of %d."), iBufferCount, KNumBuffers);
		StopTest(); // success
		break;					
		}
	default:
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrGeneral, EFail);
		break;
		}		
	}
	
//
/**
 * Implementation of RAsyncTestStep used to test the RequestStop() then Stop() apis
 **/

CTestStepAudInStreamTestReqStopThenStop* CTestStepAudInStreamTestReqStopThenStop::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepAudInStreamTestReqStopThenStop(aSuite);
	}

CTestStepAudInStreamTestReqStopThenStop::CTestStepAudInStreamTestReqStopThenStop(CTestSuite* aSuite)
: CTestStepAudInStreamAsyncTestStep(aSuite)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0750-CP");
	}

void CTestStepAudInStreamTestReqStopThenStop::KickoffTestL()
	{
	INFO_PRINTF1(_L("DEF095804 CMdaAudioInputStream loses not complete buffers from DevSound"));
	INFO_PRINTF1(_L("Calls RequestStop then Stop to ensure correct operation."));

	CTestStepAudInStreamAsyncTestStep::KickoffTestL();
	}
		
// cleanup at end
void CTestStepAudInStreamTestReqStopThenStop::CloseTest()
	{
	CTestStepAudInStreamAsyncTestStep::CloseTest();
	}

void CTestStepAudInStreamTestReqStopThenStop::MaiscOpenComplete(TInt aError)
	{
	CTestStepAudInStreamAsyncTestStep::MaiscOpenComplete(aError);
	}

void CTestStepAudInStreamTestReqStopThenStop::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{	
	CTestStepAudInStreamAsyncTestStep::MaiscBufferCopied(aError, aBuffer);
	
	if (aError == KErrAbort)
		{
		INFO_PRINTF2(_L("*MaiscBufferCopied* Error EXPECTED. We have been Stopped."), aError);			
		INFO_PRINTF2(_L("*MaiscBufferCopied* Data in this buffer: %d bytes."), aBuffer.Length());
		INFO_PRINTF2(_L("*MaiscBufferCopied* GetBytes reports %d bytes recorded."), iAudInStream->GetBytes());
		}
	}

void CTestStepAudInStreamTestReqStopThenStop::MaiscRecordComplete(TInt aError)
	{
	CTestStepAudInStreamAsyncTestStep::MaiscRecordComplete(aError);
	}

void CTestStepAudInStreamTestReqStopThenStop::Fsm()
	{
	switch (iTestState)
		{
	case EStateOpen:
		{
		iAudInStream->Open(NULL);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateRecordInit;
		break;
		}
	case EStateRecordInit:
		{
		// start the recording process
		for (TInt i=0; i<KNumBuffers; i++)
			{
			iAudInStream->ReadL(iBufferList[i]);
			}
		
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);
		iTestState = EStateRecord;
		break;
		}
	case EStateRecord:
		{	
		// Just wating time here...for the record process to continue
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);
		iTestState = EStateRequestStop;
		break;
		}
	case EStateRequestStop:
		{		
		// stop the recording process
		iAudInStream->RequestStop();	
		iAudInStream->Stop();	

		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);

		// report the number of bytes read
		INFO_PRINTF2(_L("State: EStateRequestStop - GetBytes reports %d bytes recorded."), iAudInStream->GetBytes());
		iTestState = EStateStopTest;
		break;			
		}
	case EStateStopTest:
		{
		INFO_PRINTF3(_L("Used %d buffers of %d."), iBufferCount, KNumBuffers);
		StopTest(); // success
		break;					
		}
	default:
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrGeneral, EFail);
		break;
		}		
	}
	
//

/**
 * Implementation of RAsyncTestStep used to test the RequestStop() then Stop() apis
 **/

CTestStepAudInStreamTestStopFromBufferCopied* CTestStepAudInStreamTestStopFromBufferCopied::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepAudInStreamTestStopFromBufferCopied(aSuite);
	}

CTestStepAudInStreamTestStopFromBufferCopied::CTestStepAudInStreamTestStopFromBufferCopied(CTestSuite* aSuite)
: CTestStepAudInStreamAsyncTestStep(aSuite)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0751-CP");
	}

void CTestStepAudInStreamTestStopFromBufferCopied::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{	
	CTestStepAudInStreamAsyncTestStep::MaiscBufferCopied(aError, aBuffer);
	if(iTestState == EStateRequestStop)
		{
		iTestState = EStateStopping;
		iAudInStream->Stop();
		iTestState = EStateStopTest;
		Fsm();
		}
	}

void CTestStepAudInStreamTestStopFromBufferCopied::Fsm()
	{
	switch (iTestState)
		{
	case EStateOpen:
		{
		iAudInStream->Open(NULL);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateRecordInit;
		break;
		}
	case EStateRecordInit:
		{
		// start the recording process
		for (TInt i=0; i<KNumBuffers; i++)
			{
			TRAPD(err, iAudInStream->ReadL(iBufferList[i]));
			if(err)
				{
				INFO_PRINTF2(_L("ReadL failed with error[%d]."), err);
				StopTest(err, EFail);
				return;
				}
			}
		
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000);
		iTestState = EStateRecord;
		break;
		}
	case EStateRecord:
		{	
		// Just wating time here...for the record process to continue
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTestState = EStateRequestStop;
		break;
		}
	case EStateStopTest:
		{
		INFO_PRINTF3(_L("Used %d buffers of %d."), iBufferCount, KRecNumBuffer);
		StopTest(); // success
		break;					
		}
	default:
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrGeneral, EFail);
		break;
		}		
	}


//

/**
 * 
 **/

CTestStepAudInStreamDestroy* CTestStepAudInStreamDestroy::NewL(CTestSuite* aSuite)
	{
	return new (ELeave) CTestStepAudInStreamDestroy(aSuite);
	}

CTestStepAudInStreamDestroy::CTestStepAudInStreamDestroy(CTestSuite* aSuite)
: CTestStepAudInStreamAsyncTestStep(aSuite)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0985-CP");
	}

void CTestStepAudInStreamDestroy::MaiscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{	
	INFO_PRINTF2(_L("*MaiscBufferCopied* Error: %d."), aError);
	if (aError==KErrNone && aBuffer.Length())
			{
			++iBufferCount;
			}

	else
			{
			if (aError == KErrAbort)
				{
				INFO_PRINTF1(_L("Unexpected call back Received when input stream has been deleted"));
				}
			StopTest(aError, EFail);			
			
			}	
	}

void CTestStepAudInStreamDestroy::Fsm()
	{
	switch (iTestState)
		{
	case EStateOpen:
		{
		iAudInStream->Open(NULL);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateRecordInit;
		break;
		}
	case EStateRecordInit:
		{
		// start the recording process
		for (TInt i=0; i<KNumBuffers; i++)
			{
			TRAPD(err, iAudInStream->ReadL(iBufferList[i]));
			if(err)
				{
				INFO_PRINTF2(_L("ReadL failed with error[%d]."), err);
				StopTest(err, EFail);
				return;
				}
			}
		
		// start the timer
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(1000000);
		iTestState = EStateStopTest;
		break;
		}
	
	case EStateStopTest:
		{
		//Now call delete to invoke the destructor
		INFO_PRINTF1(_L("Deleting Input Stream"));
		delete iAudInStream;
		iAudInStream=NULL;
		INFO_PRINTF3(_L("Used %d buffers of %d."), iBufferCount, KNumBuffers);
		StopTest(); // success
		break;					
		}
	default:
		INFO_PRINTF1(_L("State machine error"));
		StopTest(KErrGeneral, EFail);
		break;
		}		
	}
