// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the output stream formats tests
// for GSM610 wave forms.  These require a buffer size
// of modulo 65 bytes.  Hence we have put this in a 
// separate class.
// 
//

// EPOC includes
//#include <e32base.h>
//#include <e32test.h>
//#include <e32keys.h>
//#include <c32comm.h>
//#include <f32file.h>
//#include <etel.h>
//#include <etelmm.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepAudOutStreamGSM610.h"

#include "mmfclientaudiooutputstream.h"
#include <mda/common/audio.h>
#include "mmfclientaudiostreamutils.h"

//
// --------------------------------------------
//
// base class for some audio Ouput stream tests
_LIT(KStreamRawFile,"c:\\mm\\mmf\\testfiles\\aclnt\\test.raw");

// constant table of parameters for tests
const TGSM610ConvParameters KTestParameters[] =
{	
	// straight encode test
	{ _S("MM-MMF-ACLNT-U-0642-CP"), KMMFFourCCCodeGSM610,  TMdaAudioDataSettings::ESampleRate8000Hz,	TMdaAudioDataSettings::EChannelsMono, _S("Gsm610Mono8Khz.wav"),	KErrNone }
};

LOCAL_C TInt stopActiveScheduler(TAny*)
	{
	CActiveScheduler::Stop();
	return FALSE;
	}

CTestStepAudOutStreamGSM610* CTestStepAudOutStreamGSM610::NewL()
	{
	CTestStepAudOutStreamGSM610* s = new(ELeave) CTestStepAudOutStreamGSM610();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CTestStepAudOutStreamGSM610::CTestStepAudOutStreamGSM610() : iError(KErrNone)
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-0999-CP");
	iIsAllDataRead = EFalse;
	iIsAllDataWritten = EFalse;
	}

void CTestStepAudOutStreamGSM610::ConstructL()
	{
	// reset the buffer
	for(TInt i=0; i<KGSM610NumBuffer; i++)
		{
		iBufferList[i] = KNullDesC8;
		}
	iStartBuf=iEndBuf=0;
	}

CTestStepAudOutStreamGSM610::~CTestStepAudOutStreamGSM610()
	{
	}

void CTestStepAudOutStreamGSM610::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepAudOutStreamGSM610::MaoscPlayComplete(TInt aError)
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

void CTestStepAudOutStreamGSM610::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	if(aError)
		{
			//need add for deal with exception
			return;   
		}

	
	// adjust the buffer
	iBufferList[iStartBuf] = KNullDesC8;
	if(iStartBuf==KGSM610NumBuffer-1)
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


CTestStepAudOutStreamGSM610::CReadBufferActive::CReadBufferActive() : CActive(0)
	{
	CActiveScheduler::Add(this);
	}

CTestStepAudOutStreamGSM610::CReadBufferActive::~CReadBufferActive()
	{
	Cancel();
	}

void CTestStepAudOutStreamGSM610::CReadBufferActive::RunL()
	{
	iParent->ProcessDataL(iBuffer);
	}

void CTestStepAudOutStreamGSM610::CReadBufferActive::DoCancel()
	{
	// do nothing
	}

void CTestStepAudOutStreamGSM610::CReadBufferActive::Read()
	{
	iParent->iFile.Read(iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

void CTestStepAudOutStreamGSM610::CReadBufferActive::Read(TInt aPos)
	{
	iParent->iFile.Read(aPos, iBuffer, iStatus);      // read the 1st data trunk
	SetActive();		
	}

void CTestStepAudOutStreamGSM610::ProcessDataL(TBuf8<KGSM610BufferSize>& aData)
	{
	if(aData.Length())
		{
		if(iBufferList[iEndBuf]==KNullDesC8)
			{
			iBufferList[iEndBuf] = aData;
			iAudOutStream->WriteL(iBufferList[iEndBuf]);

			// adjust the buffer
			if(iEndBuf==KGSM610NumBuffer-1)
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
 enum TVerdict CTestStepAudOutStreamGSM610::DoTestStepPreambleL(void)
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
enum TVerdict CTestStepAudOutStreamGSM610::DoTestStepPostambleL(void)
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
TVerdict CTestStepAudOutStreamGSM610::DoTestStepL()
	{
	iTestStepResult = EPass;
	//TInt err = KErrNone;

	iAudOutStream->Open(NULL);
	CActiveScheduler::Start();
	iAudOutStream->SetVolume(iAudOutStream->MaxVolume()>>1); // half volume

	if (iError != KErrNone )
		 return EFail;

	TInt err = iFile.Open(iFs, KStreamRawFile, EFileRead);
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
 * Static constructor for CTestStepAudOutStreamGSM610Formats.
 *
 *
 * @return	"CTestStepAudOutStreamGSM610Formats*"
 *			The constructed CTestStepAudOutStreamGSM610Formats
 *
 * @xxxx
 * 
 */
CTestStepAudOutStreamGSM610Formats* CTestStepAudOutStreamGSM610Formats::NewL(TUint aTestIndex)
	{
	CTestStepAudOutStreamGSM610Formats* self = new(ELeave) CTestStepAudOutStreamGSM610Formats(aTestIndex);
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
CTestStepAudOutStreamGSM610Formats::CTestStepAudOutStreamGSM610Formats(TUint aTestIndex)
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
CTestStepAudOutStreamGSM610Formats::~CTestStepAudOutStreamGSM610Formats()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepAudOutStreamGSM610Formats::DoTestStepPreambleL(void)
	{
	TInt err = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepAudOutStreamGSM610::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is writting GSM610 data to an Audio Output Stream Utility Class."));

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
enum TVerdict CTestStepAudOutStreamGSM610Formats::DoTestStepPostambleL(void)
	{
	iFile.Close();
	//[ Destroy the scheduler, delete the iAudOutStream, close the file system
	return CTestStepAudOutStreamGSM610::DoTestStepPostambleL();
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
TVerdict CTestStepAudOutStreamGSM610Formats::DoTestStepL()
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
	callBackTimer->After(500000);
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
TFileName CTestStepAudOutStreamGSM610Formats::SetFileNameL( const TDesC& aPathNameAndExtn )
	{
	//[ append the filename relative to the default path ]
	User::LeaveIfError(iFs.Connect());
	TFileName fileName = DefaultPath();
	fileName.Append(aPathNameAndExtn);
	return fileName;
	//iFileName().iPath = fileName ;
	}







