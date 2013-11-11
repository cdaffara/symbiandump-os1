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
#include <testframework.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepAudOutStream.h"

#include "mmfclientaudiooutputstream.h"
#include <mda/common/audio.h>
#include "mmfclientaudiostreamutils.h"

//
// --------------------------------------------
//
// base class for some audio Ouput stream tests
_LIT(KStreamRawFile,"c:\\mm\\mmf\\testfiles\\aclnt\\test.raw");
//_LIT(KStreamWavFile,"c:\\16bitmPcm.wav");
const TInt KMicroSecsInOneSec = 1000000;
//const TInt KBitsPerSample = 16;	// EABI warning removal
const TInt KBytesPerSample = 2;
const TInt KSampleRate = 8000;

const TInt KToneFrequency = 2000;
const TInt KOneSecond = 1000000;
const TInt KNumofThreads = 2;
const TInt KWaitReadTime = 500000;

// constant table of parameters for tests
const TConvParameters KTestParameters[] =
{	
	// straight encode test
	{ _S("MM-MMF-ACLNT-U-0631-CP"), KMMFFourCCCodePCMU8,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("PU8Mono1KhzTone.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0632-CP"), KMMFFourCCCodePCMU8,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsStereo, _S("PU8Stereo1KhzTone.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0633-CP"), KMMFFourCCCodePCM16,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("PCMExp500hzMono8Khz.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0634-CP"), KMMFFourCCCodePCM16,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsStereo, _S("PCMExp500hzStereo8Khz.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0635-CP"), KMMFFourCCCodeALAW,    TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("Mono500hzALw.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0636-CP"), KMMFFourCCCodeALAW,    TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsStereo, _S("Stereo500hzALw.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0637-CP"), KMMFFourCCCodeMuLAW,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("Mono500hzMuLaw.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0638-CP"), KMMFFourCCCodeMuLAW,   TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsStereo, _S("Stereo500hzMuLaw.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0639-CP"), KMMFFourCCCodeIMAD,    TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("ImaAdpcmMono8khz.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0640-CP"), KMMFFourCCCodePCM16B,  TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("Au16Be8khz500hzMono.au"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0641-CP"), KMMFFourCCCodePCM16B,  TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsStereo, _S("AuStereo8khz500hz.au"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0858-CP"), KMMFFourCCCodeIMAD,    TMdaAudioDataSettings::ESampleRate32000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("ImaAdpcmMono8khz.wav"),	KErrNone },
	{ _S("MM-MMF-ACLNT-U-0859-CP"), KMMFFourCCCodeIMAD,    TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("ImaAdpcmMono8khz.wav"),	KErrNone }
};
	
CTestStepAudOutStream* CTestStepAudOutStream::NewL()
	{
	CTestStepAudOutStream* s = new(ELeave) CTestStepAudOutStream();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudOutStream::CTestStepAudOutStream() : iError(KErrNone)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0999-CP");
	iIsAllDataRead = EFalse;
	iIsAllDataWritten = EFalse;
	}

void CTestStepAudOutStream::ConstructL()
	{
	// reset the buffer
	for(TInt i=0; i<KNumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;
	}

CTestStepAudOutStream::~CTestStepAudOutStream()
	{
	}
/*
TInt CTestStepAudOutputStream::HandleKeyEvent(TKeyCode aKeystroke)
	{
	TInt retval = KErrNotSupported;
	TInt key = aKeystroke;
	switch (key)
		{
	case 'o':
		if (!iStream)
			retval = KErrNotReady;
		else
			TRAP(retval, iAudOutStream->Open(NULL));
		break;
	case 'w':
		if (!iStream)
			retval = KErrNotReady;
		else
			{
			TInt err = iFile.Open(iFs, KStreamRawFile, EFileRead);
			iReadBufferActive.Read();
			retval = KErrNone;
			}
		break;
	case 's':
		if (!iAudOutStream)
			retval = KErrNotReady;
		else
			{
			iAudOutStream->Stop();
			retval = KErrNone;
			}
		break;
	case 'q': 
	case 'Q':
		iParent.TestComplete();
		retval = KErrNone;
		break;
	default:
		break;
		}
	return retval;
	}

void CTestStepAudOutputStream::StartL()
	{
	}

const TDesC& CTestStepAudOutputStream::Instructions()
	{
	return KTxtClientAudioOutputStreamInstructions;
	}
*/

void CTestStepAudOutStream::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStream::MaoscPlayComplete(TInt aError)
	{
	if (iIsAllDataWritten && (aError == KErrNone || aError == KErrUnderflow))
		iError = KErrNone;
	else
		iError = aError;
	
	iReadBufferActive->Cancel();
	TInt filePosition=0;
	iFile.Seek(ESeekStart, filePosition);
	iFile.Close();
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStream::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	if(aError)
		{
			//need add for deal with exception
			return;   
		}

	
	// adjust the buffer
	iBufferList[iStartBuf] = KNullDesC8;
	if(iStartBuf==KNumBuffer-1)
		iStartBuf = 0;
	else
		iStartBuf++;
 
	if (iIsAllDataRead && iStartBuf == iEndBuf)
		{
		iIsAllDataWritten = ETrue;
		}
	else if(!iReadBufferActive->IsActive())
		iReadBufferActive->Read();
	
	}


CTestStepAudOutStream::CReadBufferActive::CReadBufferActive() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudOutStream::CReadBufferActive::~CReadBufferActive()
	{
	Cancel();
	}

void CTestStepAudOutStream::CReadBufferActive::RunL()
	{
	iParent->ProcessDataL(iBuffer);
	}

void CTestStepAudOutStream::CReadBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudOutStream::CReadBufferActive::Read()
	{
	iParent->iFile.Read(iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

void CTestStepAudOutStream::CReadBufferActive::Read(TInt aPos)
	{
	iParent->iFile.Read(aPos, iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

void CTestStepAudOutStream::ProcessDataL(TBuf8<KBufferSize>& aData)
	{
	if(aData.Length())
		{
		if(iBufferList[iEndBuf]==KNullDesC8)
			{
			iBufferList[iEndBuf] = aData;
			iAudOutStream->WriteL(iBufferList[iEndBuf]);

			// adjust the buffer
			if(iEndBuf==KNumBuffer-1)
				iEndBuf=0;
			else
				iEndBuf++;
			}

		if(iBufferList[iEndBuf]==KNullDesC8)
			{
			iReadBufferActive->Read();
			}
		// else all our buffers are full
		}
	else
		iIsAllDataRead = ETrue;
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStream::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// create the active object
	iReadBufferActive = new(ELeave) CReadBufferActive();
	iReadBufferActive->iParent = this;
	iFs.Connect();

	TRAPD(err, iAudOutStream = CMMFMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL )
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
enum TVerdict CTestStepAudOutStream::DoTestStepPostambleL(void)
	{
	iFs.Close();

	delete iAudOutStream;
	iAudOutStream = NULL;
	delete iReadBufferActive;
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
TVerdict CTestStepAudOutStream::DoTestStepL()
	{
	iTestStepResult = EPass;
	//TInt err = KErrNone;

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	if (iError != KErrNone )
		 return EFail;

	TInt err = KErrNone;
	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		return EFail;

	iReadBufferActive->Read();
	CActiveScheduler::Start();

	// wait sometime to finish and then close the file
	User::After(1200000);
	iFile.Close();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//

/**
 *
 * Static constructor for CTestStepAudOutStreamNew.
 *
 *
 * @return	"CTestStepAudOutStreamNew*"
 *			The constructed CTestStepAudOutStreamNew
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamNew* CTestStepAudOutStreamNew::NewL()
	{
	CTestStepAudOutStreamNew* self = new(ELeave) CTestStepAudOutStreamNew;
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
CTestStepAudOutStreamNew::CTestStepAudOutStreamNew()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0600-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamNew::~CTestStepAudOutStreamNew()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudOutStreamNew::MaoscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudOutStreamNew::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudOutStreamNew::MaoscPlayComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamNew::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0600-CP"));
	INFO_PRINTF1(_L("this test is initialising an Audio Output Stream Utility Class"));
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudOutStreamNew::DoTestStepPostambleL(void)
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
TVerdict CTestStepAudOutStreamNew::DoTestStepL()
	{
	__MM_HEAP_MARK;

	iAudOutStream = CMMFMdaAudioOutputStream::NewL(*this);

	if ( iAudOutStream == NULL )
		 iTestStepResult = EFail;
	else 
		 iTestStepResult = EPass;

	delete iAudOutStream;
	iAudOutStream = NULL;
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
 * Static constructor for CTestStepAudOutStreamSetProp.
 *
 *
 * @return	"CTestStepAudOutStreamSetProp*"
 *			The constructed CTestStepAudOutStreamSetProp
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetProp* CTestStepAudOutStreamSetProp::NewL()
	{
	CTestStepAudOutStreamSetProp* self = new(ELeave) CTestStepAudOutStreamSetProp;
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
CTestStepAudOutStreamSetProp::CTestStepAudOutStreamSetProp()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0601-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetProp::~CTestStepAudOutStreamSetProp()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamSetProp::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0601-CP"));
	INFO_PRINTF1(_L("this test is setting the audio properties of an Audio Output Stream Utility Class"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	return verdict;
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
TVerdict CTestStepAudOutStreamSetProp::DoTestStepL()
	{
	TInt err = KErrNone;

	// test 1
	TRAP(err, iAudOutStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate8000Hz,TMdaAudioDataSettings::EChannelsMono) );
	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		 return EFail;

	// test2
	// fails with the enum values, correct?
	TRAP(err, iAudOutStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate44100Hz,TMdaAudioDataSettings::EChannelsStereo) );

	if (!(err == KErrNone || 
		  err == KErrNotSupported) )
		 return EFail;
	else 
		 return EPass;

	}


//
// This tests TUint StreamUtils::MapChannelsMdaToMMFL(TInt aMdaChannels)
//
CTestStepAudOutStreamMapChannels* CTestStepAudOutStreamMapChannels::NewL()
	{
	CTestStepAudOutStreamMapChannels* self = new(ELeave) CTestStepAudOutStreamMapChannels;
	return self;
	}

CTestStepAudOutStreamMapChannels::CTestStepAudOutStreamMapChannels()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0620-LP");
	}

TVerdict CTestStepAudOutStreamMapChannels::DoTestStepL()
	{
	TInt err = KErrNone;
	TInt mdaFlags = 0;
	TUint mmfChannels = 0;

	// Test with zero as param
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if ((err != KErrNone) || (mmfChannels != EMMFMono))
		{
		INFO_PRINTF4(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d, mdaFlags = %d, mmfChannels = %d)"),
					 err, mdaFlags, mmfChannels);
		return EFail;
		}

	// Test with MDA mono flag
	mdaFlags |= TMdaAudioDataSettings::EChannelsMono;
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if ((err != KErrNone) || (mmfChannels != EMMFMono))
		{
		INFO_PRINTF4(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d, mdaFlags = %d, mmfChannels = %d)"),
					 err, mdaFlags, mmfChannels);
		return EFail;
		}

	// Test with MDA mono and stereo flag
	mdaFlags |= TMdaAudioDataSettings::EChannelsStereo;
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if ((err != KErrNone) || (mmfChannels != EMMFStereo))
		{
		INFO_PRINTF4(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d, mdaFlags = %d, mmfChannels = %d)"),
					 err, mdaFlags, mmfChannels);
		return EFail;
		}

	// Test with just MDA stereo flag
	mdaFlags = TMdaAudioDataSettings::EChannelsStereo;
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if ((err != KErrNone) || (mmfChannels != EMMFStereo))
		{
		INFO_PRINTF4(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d, mdaFlags = %d, mmfChannels = %d)"),
					 err, mdaFlags, mmfChannels);
		return EFail;
		}

	// Test with some other flags combined
	mdaFlags |= TMdaAudioDataSettings::ESampleRate44100Hz | TMdaAudioDataSettings::ERealTime;
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if ((err != KErrNone) || (mmfChannels != EMMFStereo))
		{
		INFO_PRINTF4(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d, mdaFlags = %d, mmfChannels = %d)"),
					 err, mdaFlags, mmfChannels);
		return EFail;
		}

	// Test with invalid param
	mdaFlags = -1;
	TRAP(err, mmfChannels = StreamUtils::MapChannelsMdaToMMFL(mdaFlags));
	if (err != KErrNotSupported)
		{
		INFO_PRINTF2(_L("StreamUtils::MapChannelsMdaToMMFL failed test (err = %d)"), err);
		return EFail;
		}

	return EPass;	
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamSetPropNeg.
 *
 *
 * @return	"CTestStepAudOutStreamSetPropNeg*"
 *			The constructed CTestStepAudOutStreamSetPropNeg
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetPropNeg* CTestStepAudOutStreamSetPropNeg::NewL()
	{
	CTestStepAudOutStreamSetPropNeg* self = new(ELeave) CTestStepAudOutStreamSetPropNeg;
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
CTestStepAudOutStreamSetPropNeg::CTestStepAudOutStreamSetPropNeg()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0801-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetPropNeg::~CTestStepAudOutStreamSetPropNeg()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamSetPropNeg::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	 // Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0801-CP"));
	INFO_PRINTF1(_L("this negative test sets incorrect audio properties of an Audio Output Stream Utility Class"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	return verdict;
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
TVerdict CTestStepAudOutStreamSetPropNeg::DoTestStepL()
	{
	TInt err = KErrNone;

	// test 1
	TRAP(err, StreamUtils::MapChannelsMdaToMMFL(-1));
	if (err != KErrNotSupported)
		 return EFail;

	// test 2
	TRAP(err, StreamUtils::MapSampleRateMdaToMMFL(-1));
	if (err != KErrNotSupported)
		 return EFail;

	return EPass;

	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamOpen.
 *
 *
 * @return	"CTestStepAudOutStreamOpen*"
 *			The constructed CTestStepAudOutStreamOpen
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamOpen* CTestStepAudOutStreamOpen::NewL()
	{
	CTestStepAudOutStreamOpen* self = new(ELeave) CTestStepAudOutStreamOpen;
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
CTestStepAudOutStreamOpen::CTestStepAudOutStreamOpen()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0602-CP");
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamOpen::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler and initialises the iAudOutStream
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0602-CP"));
	INFO_PRINTF1(_L("this test is openning an Audio Output Stream Utility Class"));

	return verdict;
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
TVerdict CTestStepAudOutStreamOpen::DoTestStepL()
	{
	TInt err = KErrNone;

	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz; // ESampleRateAnyInRange
	settings.iChannels = TMdaAudioDataSettings::EChannelsStereo;
	settings.iFlags = TMdaAudioDataSettings::ENoNetworkRouting;
	settings.iVolume = 0;

	iAudOutStream->Open(&settings);
//	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	TBool stopped = EFalse;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EFail;
	else 
		 return EPass;

	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamSetVol.
 *
 *
 * @return	"CTestStepAudOutStreamSetVol*"
 *			The constructed CTestStepAudOutStreamSetVol
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetVol* CTestStepAudOutStreamSetVol::NewL()
	{
	CTestStepAudOutStreamSetVol* self = new(ELeave) CTestStepAudOutStreamSetVol;
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
CTestStepAudOutStreamSetVol::CTestStepAudOutStreamSetVol()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0603-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetVol::~CTestStepAudOutStreamSetVol()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudOutStreamSetVol::MaoscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudOutStreamSetVol::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudOutStreamSetVol::MaoscPlayComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamSetVol::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0603-CP"));
	INFO_PRINTF1(_L("this test is setting the volume of an Audio Output Stream Utility Class"));

	TRAPD(err, iAudOutStream = CMMFMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL )
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
enum TVerdict CTestStepAudOutStreamSetVol::DoTestStepPostambleL(void)
	{
	delete iAudOutStream;
	iAudOutStream = NULL;
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
TVerdict CTestStepAudOutStreamSetVol::DoTestStepL()
	{
	TInt vol = iAudOutStream->MaxVolume()/2;
	if (vol < 0)
		return EFail;
	//set the volume
	iAudOutStream->SetVolume(vol);

	//get the volume
	TInt rvol = iAudOutStream->Volume();
	if (vol != rvol ) 
		 return EFail;
	else 
		 iTestStepResult = EPass;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamSetPrior.
 *
 *
 * @return	"CTestStepAudOutStreamSetPrior*"
 *			The constructed CTestStepAudOutStreamSetPrior
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetPrior* CTestStepAudOutStreamSetPrior::NewL()
	{
	CTestStepAudOutStreamSetPrior* self = new(ELeave) CTestStepAudOutStreamSetPrior;
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
CTestStepAudOutStreamSetPrior::CTestStepAudOutStreamSetPrior()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0608-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetPrior::~CTestStepAudOutStreamSetPrior()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudOutStreamSetPrior::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0608-HP"));
	INFO_PRINTF1(_L("this test is setting the audio priorities of an Audio Output Stream Utility Class"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
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
enum TVerdict CTestStepAudOutStreamSetPrior::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamSetPrior::DoTestStepL()
	{
//	TInt err = KErrNone;

	// test 1
	iAudOutStream->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceTime);

/*	TMMFPrioritySettings prior;
	err = GetAudOutStreamPrioritySettings(iAudOutStream, prior);
	if ( err != KErrNone || 
		 prior.iPriority != EMdaPriorityMin ||
		 prior.iPref != EMdaPriorityPreferenceTime)
		 return EFail; // fails with openned and non-openned CMdaAudioOutputStream
*/
	// test2
	iAudOutStream->SetPriority(EMdaPriorityMax, EMdaPriorityPreferenceQuality);

/*	err = GetAudOutStreamPrioritySettings(iAudOutStream, prior);
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
 * Static constructor for CTestStepAudOutStreamSetBalance.
 *
 *
 * @return	"CTestStepAudOutStreamSetBalance*"
 *			The constructed CTestStepAudOutStreamSetBalance
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetBalance* CTestStepAudOutStreamSetBalance::NewL()
	{
	CTestStepAudOutStreamSetBalance* self = new(ELeave) CTestStepAudOutStreamSetBalance;
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
CTestStepAudOutStreamSetBalance::CTestStepAudOutStreamSetBalance()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0606-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamSetBalance::~CTestStepAudOutStreamSetBalance()
	{
	}

/**
 * Implementation of the MMdaAudioOuputStreamCallback interface functions
 **/
void CTestStepAudOutStreamSetBalance::MaoscOpenComplete(TInt /*aError*/)
	{
	}
void CTestStepAudOutStreamSetBalance::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}
void CTestStepAudOutStreamSetBalance::MaoscPlayComplete(TInt /*aError*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamSetBalance::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0606-HP"));
	INFO_PRINTF1(_L("this test is setting and getting the balance of an Audio Output Stream Utility Class"));

	TRAPD(err, iAudOutStream = CMMFMdaAudioOutputStream::NewL(*this) );

	if (err != KErrNone ||
		iAudOutStream == NULL )
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
enum TVerdict CTestStepAudOutStreamSetBalance::DoTestStepPostambleL(void)
	{
	delete iAudOutStream;
	iAudOutStream = NULL;

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
TVerdict CTestStepAudOutStreamSetBalance::DoTestStepL()
	{
	TInt bal = 0;

	TRAPD(err, iAudOutStream->SetBalanceL()); // default KMMFBalanceCenter
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceCenter )
		return EFail;

	TRAP(err, iAudOutStream->SetBalanceL(KMMFBalanceMaxRight));
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceMaxRight )
		return EFail;

	TRAP(err, iAudOutStream->SetBalanceL(KMMFBalanceMaxLeft));
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	//get the balance
	TRAP(err, bal = iAudOutStream->GetBalanceL());
	if (err != KErrNone)
		return EFail; // we expect this _will_ be supported for Typhoon

	if (bal != KMMFBalanceMaxLeft )
		return EFail;

	// out of bounds
	/*
	iAudOutStream->SetBalance(KMMFBalanceMaxLeft+10);
	//get the balance
	bal = iAudOutStream->Balance();
	if (bal != KMMFBalanceMaxLeft )
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
 * Static constructor for CTestStepAudOutStreamWrite.
 *
 *
 * @return	"CTestStepAudOutStreamWrite*"
 *			The constructed CTestStepAudOutStreamWrite
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamWrite* CTestStepAudOutStreamWrite::NewL()
	{
	CTestStepAudOutStreamWrite* self = new(ELeave) CTestStepAudOutStreamWrite;
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
CTestStepAudOutStreamWrite::CTestStepAudOutStreamWrite()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0609-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamWrite::~CTestStepAudOutStreamWrite()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamWrite::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	// this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0609-CP"));
	INFO_PRINTF1(_L("this test is writting data to an Audio Output Stream Utility Class. WriteL()"));

	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iChannels = TMdaAudioDataSettings::EChannelsMono;
	settings.iSampleRate = TMdaAudioDataSettings::ESampleRate8000Hz;
	settings.iVolume = 0;
	iAudOutStream->Open(&settings);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EInconclusive;

	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
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
enum TVerdict CTestStepAudOutStreamWrite::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamWrite::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TBool boolRes = EFalse;

	iReadBufferActive->Read(); // eventually calls CMdaAudioOutputStream::WriteL(...)
	
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KWaitReadTime);
	CActiveScheduler::Start();
	delete callBackTimer;
	
	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes)
		{
		INFO_PRINTF3(_L("GetAudOutStreamPlaying failed (err=%d, boolRes = %d)"), err, boolRes);
		return EFail;
		}

	CActiveScheduler::Start();
	iTestStepResult = (iError == KErrNone) ? EPass : EFail;
	if (iError != KErrNone)
		INFO_PRINTF2(_L("Test failed, iError=%d"), iError);

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result

	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamStop.
 *
 *
 * @return	"CTestStepAudOutStreamStop*"
 *			The constructed CTestStepAudOutStreamStop
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamStop* CTestStepAudOutStreamStop::NewL()
	{
	CTestStepAudOutStreamStop* self = new(ELeave) CTestStepAudOutStreamStop;
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
CTestStepAudOutStreamStop::CTestStepAudOutStreamStop()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0610-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamStop::~CTestStepAudOutStreamStop()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamStop::DoTestStepPreambleL(void)
	{
	 iTestStepResult = EPass;
	 enum TVerdict verdict;
	// this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0610-CP"));
	INFO_PRINTF1(_L("this test is stopping a playing Audio Output Stream Utility Class: Stop()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EInconclusive;

	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KWaitReadTime);
	CActiveScheduler::Start();
	delete callBackTimer;
	

	TBool boolRes;
	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes )
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
enum TVerdict CTestStepAudOutStreamStop::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

TInt stopAudOutStream(TAny* aAudOutStream)
	{
	REINTERPRET_CAST(CMMFMdaAudioOutputStream*, aAudOutStream)->Stop();
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
TVerdict CTestStepAudOutStreamStop::DoTestStepL()
	{
	TInt err = KErrNone;
	TBool stopped;

	// stream is playing now from preamble
	// wait a fraction of the stream duration before stopping
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopAudOutStream, iAudOutStream));
	callBackTimer->After(1000000);
	CActiveScheduler::Start();

	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrCancel || // KErrAbort(?KErrCancel seems to be what is returned) is supposed to be returned ny the MaoscBufferCopied
		err != KErrNone || 
		!stopped )
		{
		INFO_PRINTF4(_L("GetAudOutStreamStopped failed (iError=%d, err=%d, stopped=%d)"), iError, err, stopped);
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamPosition.
 *
 *
 * @return	"CTestStepAudOutStreamPosition*"
 *			The constructed CTestStepAudOutStreamPosition
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamPosition* CTestStepAudOutStreamPosition::NewL()
	{
	CTestStepAudOutStreamPosition* self = new(ELeave) CTestStepAudOutStreamPosition;
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
CTestStepAudOutStreamPosition::CTestStepAudOutStreamPosition()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0611-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamPosition::~CTestStepAudOutStreamPosition()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamPosition::DoTestStepPreambleL(void)
	{
	 iTestStepResult = EPass;
	 enum TVerdict verdict;
	// this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0611-CP"));
	INFO_PRINTF1(_L("this test is gettig the position of a playing Audio Output Stream Utility Class: Position()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EInconclusive;

	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KWaitReadTime);
	CActiveScheduler::Start();
	delete callBackTimer;

	TBool boolRes;
	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes )
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
enum TVerdict CTestStepAudOutStreamPosition::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamPosition::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;

	// here the purpose is to test if the Position() function returns the 
	// correct position with respect to the clip byte-stream position, i.e.
	// the bytes that have been played so far, expressed in time (via sample rate)
	// and not the actual real-time time difference between the beginning of the 
	// play and the moment we apply Position().
	// The stream might take longer actual time to be played, e.g. start delay, breaks, etc.
	// however in this case streaming from file to AudioOutput(speaker) should be consistent
	// but it's not the purpose of this unit test to check that it plays fine and within 
	// certain time limits

	// get the position
	TTimeIntervalMicroSeconds pos(0);
	TInt64 bytes = 0;
	TInt64 estimPos = 0;
	const TInt64 KExpectedDeviation = 50000; //50 milli secs
	pos = iAudOutStream->Position();
	bytes = iAudOutStream->GetBytes();
	estimPos = (bytes * TInt64(KMicroSecsInOneSec)) / (TInt64(KSampleRate) * TInt64(KBytesPerSample));
	//these two need not be same if the bytes played change continuously. So we need to consider the limit on
	//on the deviation
	TInt64 deviation = estimPos-pos.Int64();
	if (deviation > KExpectedDeviation)
		{
		INFO_PRINTF3(_L("Test1: pos (%ld) != estimPos (%ld)"), pos.Int64(), estimPos);
		return EFail;
		}

	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	const TInt KWaitTime = 1000000;
	callBackTimer->After(KWaitTime);
	CActiveScheduler::Start();
	delete callBackTimer;

	pos = iAudOutStream->Position();
	bytes = iAudOutStream->GetBytes();
	estimPos = (bytes * TInt64(KMicroSecsInOneSec)) / (TInt64(KSampleRate) * TInt64(KBytesPerSample));

	const TReal KTolerance = 0.05; // 5%
	const TInt64 KDelta = static_cast<TInt>(KTolerance * I64REAL(pos.Int64()));
	const TInt64 KMin = pos.Int64() - KDelta;
	const TInt64 KMax = pos.Int64() + KDelta;
	
	INFO_PRINTF4(_L("Position (%ld), Estimated Position (%ld), Tolerance at %d percent"), pos.Int64(), estimPos, static_cast<TInt>(KTolerance * 100));

	if ((estimPos < KMin) || (estimPos > KMax))
		{
		INFO_PRINTF3(_L("pos (%ld) != estimPos (%ld)"), pos.Int64(), estimPos);
		return EFail;
		}

	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopAudOutStream, iAudOutStream));
	CActiveScheduler::Start();
	delete idleStopper;

	TBool stopped;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrCancel || // KErrCancel (KErrAbort is supposed to be returned ny the MaoscBufferCopied??)
		err != KErrNone || 
		!stopped )
		{
		INFO_PRINTF4(_L("GetAudOutStreamStopped failed (iError=%d, err=%d, stopped=%d)"), iError, err, stopped);
		return EFail;
		}
	
	// get the position
	pos = iAudOutStream->Position();
	if (pos.Int64() != 0)
		{
		INFO_PRINTF1(_L("iAudOutStream->Position() not zero"));
		return EFail;
		}
	else 
		 iTestStepResult = EPass;

	//xxx try cotinuing playing and get positions while playing if there is more time.

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamGetBytes.
 *
 *
 * @return	"CTestStepAudOutStreamGetBytes*"
 *			The constructed CTestStepAudOutStreamGetBytes
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamGetBytes* CTestStepAudOutStreamGetBytes::NewL()
	{
	CTestStepAudOutStreamGetBytes* self = new(ELeave) CTestStepAudOutStreamGetBytes;
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
CTestStepAudOutStreamGetBytes::CTestStepAudOutStreamGetBytes()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0612-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamGetBytes::~CTestStepAudOutStreamGetBytes()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamGetBytes::DoTestStepPreambleL(void)
	{
	 iTestStepResult = EPass;
	 enum TVerdict verdict;
	// this installs the scheduler
	 verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0612-CP"));
	INFO_PRINTF1(_L("this test gets the bytes rendered by the H/W until this moment: GetBytes()"));

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error calling iAudOutStream->Open(NULL) %d"), iError);
		return EInconclusive;
		}

	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EInconclusive;

	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		return EInconclusive;

	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KWaitReadTime);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("1. Error in callback %d"), iError);
		return EInconclusive;
		}

	delete callBackTimer;

	TBool boolRes;
	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes )
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
enum TVerdict CTestStepAudOutStreamGetBytes::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
	}

void CTestStepAudOutStreamGetBytes::MaoscPlayComplete(TInt aError)
	{
	if (iIsAllDataWritten && (aError == KErrNone || aError == KErrUnderflow))
		iError = KErrNone;
	else
		iError = aError;

	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("MaoscPlayComplete error %d"), iError);
		}
	
	iReadBufferActive->Cancel();
	CActiveScheduler::Stop();
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
TVerdict CTestStepAudOutStreamGetBytes::DoTestStepL()
	{
	INFO_PRINTF1(_L("Test start"));

	TInt bytes1 = iAudOutStream->GetBytes();

	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(2000000);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("2. Error in callback %d"), iError);
		return EFail;
		}

	callBackTimer->Cancel(); // in case something else stopped the AS
	TInt bytes2 = iAudOutStream->GetBytes();

	callBackTimer->After(1000000);
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("3. Error in callback %d"), iError);
		return EFail;
		}

	delete callBackTimer;
	TInt bytes3 = iAudOutStream->GetBytes();

	INFO_PRINTF1(_L("Here 1"));

	TInt fileSize;
	if (iFile.SubSessionHandle())
		{
		User::LeaveIfError(iFile.Size(fileSize));
		}
	else
		{
		INFO_PRINTF1(_L("iFile doesn't have a handle"));
		return EInconclusive;
		}

	INFO_PRINTF1(_L("Here 2"));
	
	CActiveScheduler::Start(); // wait for sample to finish

	INFO_PRINTF1(_L("Here 3"));

	TInt bytes4 = iAudOutStream->GetBytes();

	INFO_PRINTF5(_L("Here 4:  bytes1 = %d; bytes2 = %d; bytes3 = %d; bytes4 = %d"), bytes1, bytes2, bytes3, bytes4);

	if (bytes1 >= bytes2 || bytes2 >= bytes3 || bytes3 >= bytes4)
		{
		return EFail;
		}

	INFO_PRINTF1(_L("Here 5"));

	if (bytes4 != fileSize)
		{
		return EFail;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamOpenZero.
 *
 *
 * @return	"CTestStepAudOutStreamOpenZero*"
 *			The constructed CTestStepAudOutStreamOpenZero
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamOpenZero* CTestStepAudOutStreamOpenZero::NewL()
	{
	CTestStepAudOutStreamOpenZero* self = new(ELeave) CTestStepAudOutStreamOpenZero;
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
CTestStepAudOutStreamOpenZero::CTestStepAudOutStreamOpenZero()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0615-CP");
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamOpenZero::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler and initialises the iAudOutStream
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0615-CP"));
	INFO_PRINTF1(_L("this test is openning an Audio Output Stream Utility Class with audio settings zeroed"));

	TInt err = iFile.Open(iFs, KStreamRawFile, EFileRead);
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
enum TVerdict CTestStepAudOutStreamOpenZero::DoTestStepPostambleL(void)
	{
	iFile.Close();

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamOpenZero::DoTestStepL()
	{
	TInt err = KErrNone;
	TBool boolRes = EFalse;

	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iSampleRate = 0; //TMdaAudioDataSettings::ESampleRate8000Hz; // ESampleRateAnyInRange
	settings.iChannels = 0; //TMdaAudioDataSettings::EChannelsStereo;
	settings.iFlags = 0; //TMdaAudioDataSettings::ENoNetworkRouting;
	settings.iVolume = 0;

	iAudOutStream->Open(&settings);
	CActiveScheduler::Start();

	TBool stopped = EFalse;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		{
		INFO_PRINTF4(_L("GetAudOutStreamStopped (iError=%d, err=%d, stopped=%d)"), iError, err, stopped);
		return EFail;
		}

	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume
	// start feeding the outputStream
	iReadBufferActive->Read(); // finally calls CMdaAudioOutputStream::WriteL(...)
	CActiveScheduler::Start();

	// check if it's stopped
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || boolRes)
		{
		INFO_PRINTF3(_L("GetAudOutStreamPlaying failed (err=%d, boolRes=%d)"), err, boolRes); 
		return EFail;
		}

	return EPass;
	}





//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamDataType.
 *
 *
 * @return	"CTestStepAudOutStreamDataType*"
 *			The constructed CTestStepAudOutStreamDataType
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamDataType* CTestStepAudOutStreamDataType::NewL()
	{
	CTestStepAudOutStreamDataType* self = new(ELeave) CTestStepAudOutStreamDataType;
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
CTestStepAudOutStreamDataType::CTestStepAudOutStreamDataType()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0630-CP");
	}

/**
 *
 * Test step destructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamDataType::~CTestStepAudOutStreamDataType()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamDataType::DoTestStepPreambleL(void)
	{
	CTestStepAudOutStream::DoTestStepPreambleL();

	return EPass;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepAudOutStreamDataType::DoTestStepPostambleL(void)
	{
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamDataType::DoTestStepL()
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
	
	CMdaAudioOutputStream* inStream = CMdaAudioOutputStream::NewL(*this);

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


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAudOutStreamFormats.
 *
 *
 * @return	"CTestStepAudOutStreamFormats*"
 *			The constructed CTestStepAudOutStreamFormats
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamFormats* CTestStepAudOutStreamFormats::NewL(TUint aTestIndex)
	{
	CTestStepAudOutStreamFormats* self = new(ELeave) CTestStepAudOutStreamFormats(aTestIndex);
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
CTestStepAudOutStreamFormats::CTestStepAudOutStreamFormats(TUint aTestIndex)
	{
	iTestStepResult = EPass;
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

/**
 *
 * Test step destructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamFormats::~CTestStepAudOutStreamFormats()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamFormats::DoTestStepPreambleL(void)
	{
	TInt err = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStream::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is writting data to an Audio Output Stream Utility Class."));

	TInt strLen = User::StringLength(iTestParameters->iFromFilename);
	TPtrC ptr( iTestParameters->iFromFilename, strLen );
	TFileName fileName = SetFileNameL(ptr);
	
	TMdaAudioDataSettings settings;
	settings.Query();
	settings.iChannels = iTestParameters->iChannels;
	settings.iSampleRate = iTestParameters->iSampleRate;
	settings.iVolume = 0;
	TRAP(err, iAudOutStream->SetDataTypeL(iTestParameters->iEncoding));
	if (err != KErrNone)
		 return EInconclusive;
	iAudOutStream->Open(&settings);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone ||
		err != KErrNone || 
		!stopped )
		 return EInconclusive;

	err = iFile.Open(iFs, fileName, EFileRead);
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
enum TVerdict CTestStepAudOutStreamFormats::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamFormats::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TBool boolRes = EFalse;
	TInt pos = 0;

	switch (iTestParameters->iEncoding)
		{
	case KMMFFourCCCodePCMU8:
	case KMMFFourCCCodePCM16:
	case KMMFFourCCCodeALAW:
	case KMMFFourCCCodeMuLAW:
		{
		 pos = 44;
		}
		break;
	case KMMFFourCCCodeIMAD:
	case KMMFFourCCCodeGSM610:
		{
		pos = 48;
		}
		break;
	case KMMFFourCCCodePCM16B:
		{
		pos = 24;
		}
		break;
	default:
		ASSERT(0);
		break;
		}
	
	iReadBufferActive->Read(pos);
	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KWaitReadTime);
	CActiveScheduler::Start();
	delete callBackTimer;

	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes)
		{
		INFO_PRINTF3(_L("GetAudOutStreamPlaying failed (err=%d, boolRes = %d)"), err, boolRes);
		return EFail;
		}

	CActiveScheduler::Start();
	iTestStepResult = (iError == KErrNone) ? EPass : EFail;
	if (iError != KErrNone)
		INFO_PRINTF2(_L("Test failed, iError=%d"), iError);

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result

	return iTestStepResult;
	}

/*
*
* Set FileName
*
*/
TFileName CTestStepAudOutStreamFormats::SetFileNameL( const TDesC& aPathNameAndExtn )
	{
	//[ append the filename relative to the default path ]
	User::LeaveIfError(iFs.Connect());
	TFileName fileName = DefaultPath();
	fileName.Append(aPathNameAndExtn);
	return fileName;
	//iFileName().iPath = fileName ;
	}


//

// DEF093436: Call to Configure in wrong place in CMMFImaAdpcmToPcm16CodecHwDevice::Start 
// PDEF092098: DevSound fails to update policy if error occurs in StartPlayDataL

// CTestStepAudOutStreamImadPolicyCallback
// Private class which enables callbacks to be processed according to which stream
// they occurred on

CTestStepAudOutStreamImadPolicyCallback* CTestStepAudOutStreamImadPolicyCallback::NewL(TInt aStreamId, CTestStepAudOutStreamImadPolicy* aParent)
	{
	CTestStepAudOutStreamImadPolicyCallback* s = new(ELeave) CTestStepAudOutStreamImadPolicyCallback(aStreamId, aParent);
	return s;
	}

CTestStepAudOutStreamImadPolicyCallback::CTestStepAudOutStreamImadPolicyCallback(TInt aStreamId, CTestStepAudOutStreamImadPolicy* aParent)
	{
	iStreamId = aStreamId;
	iParent = aParent;
	}
	
CTestStepAudOutStreamImadPolicyCallback::~CTestStepAudOutStreamImadPolicyCallback()
	{
	}

void CTestStepAudOutStreamImadPolicyCallback::MaoscOpenComplete(TInt aError)
	{
	iParent->OpenComplete(iStreamId, aError);
	}

void CTestStepAudOutStreamImadPolicyCallback::MaoscPlayComplete(TInt aError)
	{
	iParent->PlayComplete(iStreamId, aError);
	}
	
void CTestStepAudOutStreamImadPolicyCallback::MaoscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	iParent->BufferCopied(iStreamId, aError, aBuffer);
	}

/**
 *
 * Static constructor for CTestStepAudOutStreamImadPolicy.
 *
 *
 * @return	"CTestStepAudOutStreamImadPolicy*"
 *			The constructed CTestStepAudOutStreamImadPolicy
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamImadPolicy* CTestStepAudOutStreamImadPolicy::NewL(TUint aTestIndex)
	{
	CTestStepAudOutStreamImadPolicy* s = new(ELeave) CTestStepAudOutStreamImadPolicy(aTestIndex);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudOutStreamImadPolicy::CReadBufferActive::CReadBufferActive(TInt aStreamId)
:	CActive(EPriorityStandard),
	iStreamId(aStreamId)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudOutStreamImadPolicy::CReadBufferActive::~CReadBufferActive()
	{
	Cancel();
	}

void CTestStepAudOutStreamImadPolicy::CReadBufferActive::RunL()
	{
	iParent->ProcessDataL(iStreamId, iBuffer);
	}

void CTestStepAudOutStreamImadPolicy::CReadBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudOutStreamImadPolicy::CReadBufferActive::Read()
	{
	iParent->iFile[iStreamId].Read(iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

void CTestStepAudOutStreamImadPolicy::CReadBufferActive::Read(TInt aPos)
	{
	iParent->iFile[iStreamId].Read(aPos, iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamImadPolicy::CTestStepAudOutStreamImadPolicy(TUint aTestIndex)
	{
	iTestStepResult = EPass;
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

void CTestStepAudOutStreamImadPolicy::ConstructL()
	{
	// reset the buffers
	for (TInt s = 0; s < KImadPolicyNumberOfStreams; s++)
		{
		for (TInt i = 0; i < KNumBuffer; i++)
			{
			iBufferList[s][i] = KNullDesC8;
			}
		iStartBuf[s] = 0;
		iEndBuf[s] = 0;
	
		// set the flags
		iPlayComplete[s] = EFalse;
		}
	}

/**
 *
 * Test step destructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamImadPolicy::~CTestStepAudOutStreamImadPolicy()
	{
	}

void CTestStepAudOutStreamImadPolicy::OpenComplete(TInt aStreamId, TInt aError)
	{
	INFO_PRINTF3(_L("CTestStepAudOutStreamImadPolicy::OpenComplete, aStreamId=%d aError=%d"), aStreamId, aError);
	iError[aStreamId] = aError;
	CActiveScheduler::Stop();
	iSchedulerStopType = EOpenComplete;
	}

void CTestStepAudOutStreamImadPolicy::PlayComplete(TInt aStreamId, TInt aError)
	{
	INFO_PRINTF3(_L("CTestStepAudOutStreamImadPolicy::PlayComplete, aStreamId=%d aError=%d"), aStreamId, aError);
	iPlayComplete[aStreamId] = ETrue;
	if (iIsAllDataWritten[aStreamId] && (aError == KErrNone || aError == KErrUnderflow))
		{
		iError[aStreamId] = KErrNone;
		}
	else
		{
		iError[aStreamId] = aError;
		}
	
	iReadBufferActive[aStreamId]->Cancel();

	// Keep the file OPEN until the end of the test.
	CActiveScheduler::Stop();
	iSchedulerStopType = EPlayComplete;
	}

void CTestStepAudOutStreamImadPolicy::BufferCopied(TInt aStreamId, TInt aError, const TDesC8& /*aBuffer*/)
	{
	INFO_PRINTF3(_L("CTestStepAudOutStreamImadPolicy::BufferCopied, aStreamId=%d aError=%d"), aStreamId, aError);
	iError[aStreamId] = aError;
	if (aError)
		{
		// allow the error to drop through; anything but KErrNone or KErrUnderflow
		// will fail the test
		return;   
		}

	// adjust the buffer
	TInt startBuf = iStartBuf[aStreamId];
	TBuf8<KBufferSize>* bufferList = iBufferList[aStreamId];
	bufferList[startBuf] = KNullDesC8;
	if(iStartBuf[aStreamId] == KNumBuffer - 1)
		{
		iStartBuf[aStreamId] = 0;
		}
	else
		{
		iStartBuf[aStreamId]++;
		}
 
	if (iIsAllDataRead[aStreamId] && iStartBuf[aStreamId] == iEndBuf[aStreamId])
		{
		iIsAllDataWritten[aStreamId] = ETrue;
		}
	else if (!iReadBufferActive[aStreamId]->IsActive())
		{
		iReadBufferActive[aStreamId]->Read();
		}
	}

void CTestStepAudOutStreamImadPolicy::ProcessDataL(TInt aStreamId, TBuf8<KBufferSize>& aData)
	{
	if(aData.Length())
		{
		TInt endBuf = iEndBuf[aStreamId];
		TBuf8<KBufferSize>* bufferList = iBufferList[aStreamId];
		if (bufferList[endBuf]==KNullDesC8)
			{
			bufferList[endBuf] = aData;
			iAudOutStream[aStreamId]->WriteL(bufferList[endBuf]);

			// adjust the buffer
			if (iEndBuf[aStreamId] == KNumBuffer - 1)
				{
				iEndBuf[aStreamId] = 0;
				}
			else
				{
				iEndBuf[aStreamId]++;
				}
			}

		if(bufferList[endBuf]==KNullDesC8)
			{
			iReadBufferActive[aStreamId]->Read();
			}
		// else all our buffers are full
		}
	else
		{
		iIsAllDataRead[aStreamId] = ETrue;
		}
	}

void CTestStepAudOutStreamImadPolicy::CloseAll()
	{
	for(TInt i = 0; i < KImadPolicyNumberOfStreams; i++)
		{
		iFile[i].Close();
		delete iAudOutStream[i];
		iAudOutStream[i] = NULL;
		delete iAudOutStreamCallback[i];
		iAudOutStreamCallback[i] = NULL;
		}
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 TVerdict CTestStepAudOutStreamImadPolicy::DoTestStepPreambleL(void)
	{
 	// this installs the scheduler
 	TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// create the active object
	for (TInt i = 0; i < KImadPolicyNumberOfStreams; i++) 
		{
		iReadBufferActive[i] = new(ELeave) CReadBufferActive(i);
		iReadBufferActive[i]->iParent = this;
		}
	User::LeaveIfError(iFs.Connect());

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("DEF093436: Call to Configure in wrong place in CMMFImaAdpcmToPcm16CodecHwDevice::Start"));
	
	TInt strLen = User::StringLength(iTestParameters->iFromFilename);
	TPtrC ptr(iTestParameters->iFromFilename, strLen);
	iFsFileName = SetFileNameL(ptr);
	
    return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepAudOutStreamImadPolicy::DoTestStepPostambleL(void)
	{
	iFs.Close();
	for (TInt i = 0; i < KImadPolicyNumberOfStreams; i++) 
		{
		delete iReadBufferActive[i];
		iReadBufferActive[i] = NULL;
		}
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
TVerdict CTestStepAudOutStreamImadPolicy::DoTestStepL()
	{
	// This test is for DEF093436: Call to Configure in wrong place in CMMFImaAdpcmToPcm16CodecHwDevice::Start
	// and PDEF092098: DevSound fails to update policy if error occurs in StartPlayDataL.  However
	// at present, it is not possible to test the main part of PDEF092098, due to the fact
	// that there is no way to generate an error while playing.  So for now, this test is
	// limited to testing that a panic does not occur when we try to play a file with an
	// unsupported sample rate (DEF093436).
	//
	// The structure of the class CTestStepAudOutStreamImadPolicy is such that this test
	// could be easily modified to test the main part of PDEF092098 if a way of testing it
	// is found in the future.
	
	 
	// Run a stream. We expect it to fail with KErrArgument, as we have
	// given an unsupported sample rate (32K).
	TRAPD(err, iAudOutStreamCallback[iStreamId] = CTestStepAudOutStreamImadPolicyCallback::NewL(iStreamId, this));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't open first Imad policy callback"));
		CloseAll();	
		return EInconclusive;
		}

	TRAP(err, iAudOutStream[iStreamId] = CMMFMdaAudioOutputStream::NewL(*iAudOutStreamCallback[iStreamId]) );
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't open first stream"));
		CloseAll();	
		return EInconclusive;
		}
	
	TRAP(err, iAudOutStream[iStreamId]->SetDataTypeL(iTestParameters->iEncoding));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("SetDataTypeL failed on first stream"));
		CloseAll();	
		return EInconclusive;
		}
	
	TRAP(err, iAudOutStream[iStreamId]->SetAudioPropertiesL(iTestParameters->iSampleRate, iTestParameters->iChannels));
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("SetAudioPropertiesL failed on first stream"));
		CloseAll();	
		return EInconclusive;
		}
	iAudOutStream[iStreamId]->SetVolume(0);
	iAudOutStream[iStreamId]->Open(NULL);
	
	iSchedulerStopType = ENotStopped;
	CActiveScheduler::Start();
	if (iSchedulerStopType != EOpenComplete)
		{
		INFO_PRINTF1(_L("Active scheduler was stopped by something other than Open completing"));
		CloseAll();	
		return EInconclusive;
		}
	iAudOutStream[iStreamId]->SetVolume(iAudOutStream[iStreamId]->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	err = GetAudOutStreamStopped(iAudOutStream[iStreamId], stopped);
	if (iError[iStreamId] != KErrNone || err != KErrNone || !stopped )
		{
		INFO_PRINTF3(_L("GetAudOutStreamStopped failed on first stream (err = %d, stopped = %d)"), err, stopped);
		CloseAll();	
		return EInconclusive;
		}

	err = iFile[iStreamId].Open(iFs, iFsFileName, EFileRead | EFileShareAny);
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't open file on first stream"));
		CloseAll();	
		return EInconclusive;
		}

	iTestStepResult = EPass;
	TBool boolRes = EFalse;
	TInt pos = 48;

	iReadBufferActive[iStreamId]->Read(pos);
	CIdle* idleStopper = CIdle::NewL(CActive::EPriorityStandard);
	idleStopper->Start(TCallBack(stopActiveScheduler));
	
	// Attempt to play with sample rate set to 32K should fail, but
	// should not panic
	CActiveScheduler::Start();
	delete idleStopper;
	idleStopper = NULL;
	
	err = GetAudOutStreamPlaying(iAudOutStream[iStreamId], boolRes);
	if (err != KErrNone || boolRes)
		{
		INFO_PRINTF3(_L("GetAudOutStreamPlaying failed on first stream (err = %d, boolRes = %d)"), err, boolRes);
		CloseAll();	
		return EInconclusive;
		}	

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result

	CloseAll();	
	return iTestStepResult;

	}
	
/*
*
* Set FileName
*
*/
TFileName CTestStepAudOutStreamImadPolicy::SetFileNameL( const TDesC& aPathNameAndExtn )
	{
	//[ append the filename relative to the default path ]
	User::LeaveIfError(iFs.Connect());
	TFileName fileName = DefaultPath();
	fileName.Append(aPathNameAndExtn);
	return fileName;
	}


/**
 *
 * Static constructor for CTestStepAudOutStreamPlayError.
 *
 *
 * @return	"CTestStepAudOutStreamPlayError*"
 *			The constructed CTestStepAudOutStreamPlayError
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamPlayError* CTestStepAudOutStreamPlayError::NewL(TUint aTestIndex)
	{
	CTestStepAudOutStreamPlayError* self = new(ELeave) CTestStepAudOutStreamPlayError(aTestIndex);
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
CTestStepAudOutStreamPlayError::CTestStepAudOutStreamPlayError(TUint aTestIndex)
	{
	iTestStepResult = EPass;
	// store a pointer to the test parameters
	iTestParameters = &(KTestParameters[aTestIndex]);
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = iTestParameters->iTestName;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamPlayError::~CTestStepAudOutStreamPlayError()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepAudOutStreamPlayError::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	iTestStepResult = CTestStepAudOutStream::DoTestStepPreambleL();
	if (iTestStepResult != EPass)
		{
		return iTestStepResult;
		}

	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("INC090287 & PDEF092796: CMdaAudioOutputStream resumes incorrectly after an error"));

	TRAP(iError, iAudOutStream->SetAudioPropertiesL(TMdaAudioDataSettings::ESampleRate8000Hz,
													TMdaAudioDataSettings::EChannelsMono));
	if (iError != KErrNone)
		{
		INFO_PRINTF1(_L("Error setting audio properties"));
		return EInconclusive;
		}
	iAudOutStream->Open(NULL);

	CActiveScheduler::Start();
	if (iSchedulerStopType != EStreamOpenComplete)
		{
		INFO_PRINTF1(_L("Active scheduler was stopped by something other than stream open completing"));
		return EInconclusive;
		}
	else if (iError != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening stream"));
		return EInconclusive;		
		}	
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	TBool stopped = EFalse;
	TInt err = KErrNone;
	err = GetAudOutStreamStopped(iAudOutStream, stopped);
	if (iError != KErrNone || err != KErrNone || !stopped )
		{
		return EInconclusive;
		}

	err = iFile.Open(iFs, KStreamRawFile, EFileRead);
	if (err != KErrNone)
		{
		return EInconclusive;
		}
		
	// create a high priority tone utility
	TRAP(err, iToneUtility = CMdaAudioToneUtility::NewL(*this, NULL, EMdaPriorityMax, EMdaPriorityPreferenceTimeAndQuality));
	if (err != KErrNone)
		{
		return EInconclusive;
		}

	return iTestStepResult;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepAudOutStreamPlayError::DoTestStepPostambleL(void)
	{
	iFile.Close();
	delete iToneUtility;

	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStream::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamPlayError::DoTestStepL()
	{
	// start playing a stream
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TBool boolRes = EFalse;

	iReadBufferActive->Read(); // eventually calls CMdaAudioOutputStream::WriteL(...)
	CCallBackTimer* callBackTimer1 = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer1->After(KWaitReadTime);
	CActiveScheduler::Start();
	delete callBackTimer1;
	callBackTimer1 = NULL;

	// check if it's playing
	err = GetAudOutStreamPlaying(iAudOutStream, boolRes);
	if (err != KErrNone || !boolRes)
		{
		ERR_PRINTF3(_L("GetAudOutStreamPlaying failed (err=%d, boolRes = %d)"), err, boolRes);
		return EFail;
		}

	CCallBackTimer* callBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	callBackTimer->After(KOneSecond);
	CActiveScheduler::Start();
	delete callBackTimer;
	
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Play stream failed (err=%d)"), iError);
		return EFail;
		}
	
	// pre-empt it with a higher priority client (causing a KErrInUse)
	INFO_PRINTF1( _L("Initialize CMdaAudioToneUtility"));
	iToneUtility->PrepareToPlayTone(KToneFrequency, TTimeIntervalMicroSeconds(KOneSecond));
	CActiveScheduler::Start();
	if (iSchedulerStopType != ETonePrepareComplete)
		{
		INFO_PRINTF1(_L("Active scheduler was stopped by something other than tone prepare completing"));
		return EInconclusive;
		}
	else if (iError != KErrNone)
		{
		INFO_PRINTF1(_L("Error preparing tone"));
		return EInconclusive;		
		}	

	INFO_PRINTF1( _L("Playing tone..."));
	iExpectingBufferErrors = ETrue;
	iToneUtility->Play();
	CActiveScheduler::Start();
	if (iSchedulerStopType != EStreamPlayComplete)
		{
		INFO_PRINTF1(_L("Active scheduler was stopped by something other than play stream completing"));
		return EInconclusive;
		}

	CActiveScheduler::Start();
	if (iSchedulerStopType != ETonePlayComplete)
		{
		INFO_PRINTF1(_L("Active scheduler was stopped by something other than tone play completing"));
		return EInconclusive;
		}	

	if (iError != KErrInUse)
		{
		INFO_PRINTF1(_L("Could not provoke a KErrInUse"));
		return EInconclusive;
		}

	// check that the buffer FIFO is emptied (callbacks will have occurred)
	if (iBufferErrorCount == 0)
		{
		ERR_PRINTF1(_L("Test failed, no buffer errors reported"));
		return EFail;
		}

	return iTestStepResult;
	}

void CTestStepAudOutStreamPlayError::MaoscBufferCopied(TInt aError, const TDesC8& aBuffer)
	{
	if ((aError == KErrInUse) && (iExpectingBufferErrors))
		{
		iBufferErrorCount++;
		}
	CTestStepAudOutStream::MaoscBufferCopied(aError, aBuffer);
	}

void CTestStepAudOutStreamPlayError::MaoscPlayComplete(TInt aError)
	{
	iSchedulerStopType = EStreamPlayComplete;
	iError = aError;
	INFO_PRINTF2(_L("MaoscPlayComplete, iError=%d"), iError);
	CTestStepAudOutStream::MaoscPlayComplete(aError);
	}
	
void CTestStepAudOutStreamPlayError::MaoscOpenComplete(TInt aError)
	{
	iSchedulerStopType = EStreamOpenComplete;
	iError = aError;
	INFO_PRINTF2(_L("MaoscOpenComplete, iError=%d"), iError);
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStreamPlayError::MatoPrepareComplete(TInt aError)
	{
	iSchedulerStopType = ETonePrepareComplete;
	INFO_PRINTF2(_L("MatoPrepareComplete(): error = %d"), aError);
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStreamPlayError::MatoPlayComplete(TInt aError)
	{
	iSchedulerStopType = ETonePlayComplete;
	INFO_PRINTF2(_L("MatoPlayComplete(): error = %d"), aError);
	CActiveScheduler::Stop();
	}


/**
 *
 * Static constructor for CTestStepAudOutStreamConcurrentOpen
 *
 *
 * @return	"CTestStepAudOutStreamConcurrentOpen*"
 *			The constructed CTestStepAudOutStreamConcurrentOpen
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamConcurrentOpen* CTestStepAudOutStreamConcurrentOpen::NewL()
	{
	CTestStepAudOutStreamConcurrentOpen* self = new(ELeave) CTestStepAudOutStreamConcurrentOpen;
	return self;
	}


CTestStepAudOutStreamConcurrentOpen::CTestStepAudOutStreamConcurrentOpen()
	{
	iTestStepResult = EPass;
	iTestStepName = _L("MM-MMF-ACLNT-U-0971-CP");
	}

CTestStepAudOutStreamConcurrentOpen::~CTestStepAudOutStreamConcurrentOpen()
	{
	}
	
enum TVerdict CTestStepAudOutStreamConcurrentOpen::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	//Install the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	//If the audioserver is already running needs to be stopped for this teststep
	//So check for an existing audio server and kill the instance if it exists
	TFindProcess p(_L("MMFAudioServer*"));
	TFullName name;
	TInt err = p.Next(name);
	if(err == KErrNone)
		{
		RProcess audioserver;
		err=audioserver.Open(name,EOwnerThread);
		if(err==KErrNone)
			{
			//Kill the audio server
			audioserver.Kill(0);
			User::After(1000000);//wait for 1 second
			}
		}
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0971-CP"));
	INFO_PRINTF1(_L("This test step tries to start Audioserver from multiple threads Simultaneously"));
	return verdict;
	}

enum TVerdict CTestStepAudOutStreamConcurrentOpen::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

void CTestStepAudOutStreamConcurrentOpen::StartThreadL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	CTestAudioOutputStreamWrapper* outputStream = CTestAudioOutputStreamWrapper::NewL(0);
	CleanupStack::PushL(outputStream);	
	CleanupStack::PopAndDestroy(2,scheduler);
	}

TInt CTestStepAudOutStreamConcurrentOpen::CreateThread(TAny* /*aData*/)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	TRAPD(error, StartThreadL());
	
	delete cleanup;
	
	return error;
	}

TVerdict CTestStepAudOutStreamConcurrentOpen::DoTestStepL()
	{
	__MM_HEAP_MARK;
	RThread threadlist[KNumofThreads];
	TBufC<100> threadname[KNumofThreads]={_L("OutputStreamThread1"),_L("OutputStreamThread2")};
	TRequestStatus reqStatus[KNumofThreads];
	TInt i;

	//Create two AudioOutputStreams through multiple threads concurrently
	for(i=0;i<2;i++)
		{
		TRAPD(err,threadlist[i].Create(threadname[i],CreateThread,KDefaultStackSize, 0x100000, 0x100000, NULL));
		if(err != KErrNone)
			{
			return EFail;
			}
		}
	
	//Get the status of the Audio server for the threads
    for(i=0;i<KNumofThreads;i++)
    	{
    	threadlist[i].Logon(reqStatus[i]);
    	}
    
    for(i=0;i<KNumofThreads;i++)
    	{
    	threadlist[i].Resume();
    	}
    for(i=0;i<KNumofThreads;i++)
    	{
    	User::WaitForRequest(reqStatus[i]);
    	if(reqStatus[i] != KErrNone)
    		{
    		return EFail;
    		}
    	}
    
    //Close Threads
    for(TInt i=0;i<KNumofThreads;i++)
    	{
    	threadlist[i].Close();
    	}
	__MM_HEAP_MARKEND;
	
	return iTestStepResult;
	}
	
CTestAudioOutputStreamWrapper* CTestAudioOutputStreamWrapper::NewL(TInt aPriority)
 	{
 	CTestAudioOutputStreamWrapper* self = new(ELeave) CTestAudioOutputStreamWrapper();
 	CleanupStack::PushL(self);
 	self->ConstructL(aPriority);
 	CleanupStack::Pop();
 	return self;
 	}

CTestAudioOutputStreamWrapper::CTestAudioOutputStreamWrapper()
 	{
    }

void CTestAudioOutputStreamWrapper::ConstructL(TInt aPriority)
 	{
 	iAudioOutputStream = CMdaAudioOutputStream::NewL(*this, aPriority,EMdaPriorityPreferenceNone);
 	}

CTestAudioOutputStreamWrapper::~CTestAudioOutputStreamWrapper()
 	{
 	delete iAudioOutputStream;
 	}

void CTestAudioOutputStreamWrapper::MaoscOpenComplete(TInt /*aError*/)
	{
 	}
 
void CTestAudioOutputStreamWrapper:: MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	}

void CTestAudioOutputStreamWrapper::MaoscPlayComplete(TInt /*aError*/)
	{
	}

RAudioOPStreamRequestStop* RAudioOPStreamRequestStop::NewL(const TDesC& aStepName, TBool aKeepOpenAtEnd, TBool aAutoStop, TBool aWriteBufferAfterRequestStop, TBool aRestart)
	{
	RAudioOPStreamRequestStop* self = new (ELeave) RAudioOPStreamRequestStop(aStepName, aKeepOpenAtEnd, aAutoStop, aWriteBufferAfterRequestStop, aRestart);
	return self;
	}
	
RAudioOPStreamRequestStop::RAudioOPStreamRequestStop(const TDesC& aStepName, TBool aKeepOpenAtEnd, TBool aAutoStop, TBool aWriteBufferAfterRequestStop, TBool aRestart) :
	iPlayer(NULL),
	iTimer(NULL),
	iKeepOpenAtEnd(aKeepOpenAtEnd),
	iAutoStop(aAutoStop),
	iWriteBufferAfterRequestStop(aWriteBufferAfterRequestStop),
	iRestart(aRestart)
	{
	iTestStepName = aStepName;
	iHeapSize = 1000000; // ~1M
	}
	
// start test
void RAudioOPStreamRequestStop::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iPlayer = NULL; 
	iTimer = NULL;
	INFO_PRINTF1(_L("DEF120897: This test checks KeepOpenAtEnd and RequestStop APIs of AudioOutputStream"));
	iPlayer = CAudioOPStreamAudio::NewL(this, iKeepOpenAtEnd, iAutoStop, iWriteBufferAfterRequestStop);
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	
	iPlayer->PlayL();
	iSuite->Log(ESevrInfo, _L("Playing Started...."));
	//AutoStop means that giving the responsibility of stopping the play to CAudioOPStreamAudio
	//If it is not auto stop, stop the player explicitly after a longtime
	//to check that we dont get the PlayError in between
	if(!iAutoStop && iKeepOpenAtEnd)
		{
		TCallBack callback (TimerCallback, this);
		//giving a 5 secs delay intentionally to make sure that all buffers are already used up
		iTimer->Start(5000000, 0, callback);
		}
	else if(iRestart)
		{
		TCallBack callback (TimerCallback, this);
		iTimer->Start(1000000, 0, callback);
		}
	}
	
// cleanup at end
void RAudioOPStreamRequestStop::CloseTest()
	{
	delete iPlayer;
	iPlayer = NULL;
	delete iTimer;
	iTimer = NULL;
	}
	
// signal complete
void RAudioOPStreamRequestStop::PlayingStopped(TInt aError, TVerdict aVerdict)
	{
#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	if(aError!=KErrNotSupported && iKeepOpenAtEnd)
		{
		INFO_PRINTF2(_L("Old DevSound: Unexpected error: [%d], Expected is KErrNotSupported"), aError);
		}
	else if(!iKeepOpenAtEnd && aError!=KErrNone)
		{
		INFO_PRINTF2(_L("Old DevSound: Play is stopped with unexpected error [%d]"), aError);
		}
#else
	if(aError!=KErrNone)
		{
		INFO_PRINTF2(_L("A3F DevSound: Play is stopped with unexpected error [%d]"), aError);
		}
#endif
	StopTest (aError, aVerdict);
	}

// timer callback
TInt RAudioOPStreamRequestStop::TimerCallback(TAny* aPtr)
	{
	static_cast<RAudioOPStreamRequestStop*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RAudioOPStreamRequestStop::DoTimerCallback()
	{
	iTimer->Cancel(); // only really wanted a one-shot
	if(!iAutoStop && iKeepOpenAtEnd)
		{
		iPlayer->RequestStop();
		}
	else
		{
		iPlayer->Stop();
		iPlayer->PlayL();
		}
	}

// factory function
CAudioOPStreamAudio* CAudioOPStreamAudio::NewL(MAudioOPStreamObserver* aObserver, TBool aKeepOpenAtEnd, TBool aAutoStop, TBool aWriteBufferAfterRequestStop)
	{
	CAudioOPStreamAudio* self = new (ELeave) CAudioOPStreamAudio(aObserver, aKeepOpenAtEnd, aAutoStop, aWriteBufferAfterRequestStop);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
CAudioOPStreamAudio::~CAudioOPStreamAudio()
	{
	if (iOutputStream)
		{
		iOutputStream->Stop();
		delete iOutputStream;
		}
	}

// 2nd phase constructor
void CAudioOPStreamAudio::ConstructL()
	{
	iOutputStream = CMdaAudioOutputStream::NewL(*this);
	User::LeaveIfError(iFs.Connect());
	}
	
// constructor
CAudioOPStreamAudio::CAudioOPStreamAudio(MAudioOPStreamObserver* aObserver, TBool aKeepOpenAtEnd, TBool aAutoStop, TBool aWriteBufferAfterRequestStop) :
	iObserver (aObserver),
	iKeepOpenAtEnd(aKeepOpenAtEnd),
	iAutoStop(aAutoStop),
	iWriteBufferAfterRequestStop(aWriteBufferAfterRequestStop)
	{
	}
	
void CAudioOPStreamAudio::Complete(TInt aError, TVerdict aVerdict)
	{
	TInt err = aError;
	iState = EStateIdle;
	if(iWriteBufferAfterRequestStop && err==KErrNotReady)
		{
		err=KErrNone;
		}
	iObserver->PlayingStopped(err, aVerdict);
	}
	
void CAudioOPStreamAudio::PlayL()
	{
	if(iState==EStateIdle)
		{
		//Read the data from the file into buffers in advance
		User::LeaveIfError(iFile.Open(iFs, KStreamRawFile, EFileRead));
		for (TInt i=0; i<KNumBuffers; i++)
			{
			User::LeaveIfError(iFile.Read(iBufferList[i]));
			}
		iOutputStream->Open(NULL);
		iState = EStateOpeningOutput;
		}
	else
		{
		iBufferCount=0;
		WriteOutputStreamBuffersL();
		if(iKeepOpenAtEnd && iAutoStop)
			{
			//Stopping after all the data is sent
			User::LeaveIfError(iOutputStream->RequestStop());
			}
		}
	}

void CAudioOPStreamAudio::Stop()
	{
	iOutputStream->Stop();
	}

void CAudioOPStreamAudio::RequestStop()
	{
	iOutputStream->RequestStop();
	}
		
void CAudioOPStreamAudio::WriteOutputStreamBuffersL()
	{
	for (TInt i=0; i<KNumBuffers; i++)
		{
		iOutputStream->WriteL(iBufferList[i]);
		if(iWriteBufferAfterRequestStop)
			{
			User::LeaveIfError(iOutputStream->RequestStop());
			}
		}
	}
	
void CAudioOPStreamAudio::MaoscOpenComplete(TInt aError)
	{
	ASSERT(iState==EStateOpeningOutput);
	TInt error = aError;
	if (error==KErrNone)
		{
		iState = EStateWriting;	
		iOutputStream->SetVolume(iOutputStream->MaxVolume()/2);
		if(iKeepOpenAtEnd)
			{
			error = iOutputStream->KeepOpenAtEnd();
		#ifndef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
			if(error!=KErrNotSupported)//On old devsound, we dont support this feature
				{
				Complete(error, EFail);
				}
			else
				{
				Complete(error, EPass);
				}
			return;
		#else
			if(error!=KErrNone)
				{
				Complete(error, EFail);
				return;
				}
		#endif
			}
		TRAP(error, WriteOutputStreamBuffersL());
		}
	
	if(error==KErrNone && iKeepOpenAtEnd && iAutoStop)
		{
		//Stopping after all the data is sent
		error = iOutputStream->RequestStop();
		}
	
	//Negative test, Complete is done via MaoscPlayComplete
	if (error!=KErrNone && (iWriteBufferAfterRequestStop && error!=KErrNotReady))
		{
		Complete(error, EFail);	
		}
	}
	
void CAudioOPStreamAudio::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	if (aError!=KErrNone)
		{
		// ignore any KErrAbort returns - this would happen during a Stop() call
		// if we were playing 
		if (aError!=KErrAbort)
			{
			Complete(aError, EFail);
			}
		}
	else
		{
		iBufferCount++;
		}
	}
	
void CAudioOPStreamAudio::MaoscPlayComplete(TInt aError)
	{
	TInt error = aError;
	if(error==KErrCancel)//during Stop we get KErrCancel. Do not stop at this point. Ignore that.
		{
		return;
		}
	if (error==KErrUnderflow && iBufferCount==KNumBuffers)
		{
		error = KErrNone; // normal termination is underflow following buffer request	
		}
	else if(iWriteBufferAfterRequestStop && error==KErrUnderflow) //Negative test, we dont play all the buffers
		{
		error = KErrNone;
		}
	if(error==KErrNone)
		{
		Complete(error, EPass);
		}
	else
		{
		Complete(error, EFail);
		}
	}
