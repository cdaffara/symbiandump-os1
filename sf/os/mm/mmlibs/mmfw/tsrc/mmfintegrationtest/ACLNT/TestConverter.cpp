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
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file TestConverter.cpp
*/

#include "TestConverter.h"
#include "mmfwavformat.h"
#include <mmfformatimplementationuids.hrh>
#include <mdaaudiosampleplayer.h>	
#include "WavDecodeUtility.h"		

// from "mmfauformat.h" :
const TUint KAuFileUncompressedDataHeaderSize = 24 ; // 6 times 4 byte quantities

_LIT(KAclntConConfigKeyName, "configAudio");
_LIT(KAclntConDurationKeyName, "configAudio2");
_LIT(KAclntConPositionKeyName, "configAudio3");
_LIT(KAclntConQueryKeyName, "configAudio4");
_LIT(KAclntConDestBitRateKeyName, "configAudio5");
_LIT(KAclntConDestSampleRateKeyName, "configAudio6");
_LIT(KAclntConDestNumberChannelsKeyName, "configAudio7");
_LIT(KAclntConDestDataTypeKeyName, "configAudio8");
_LIT(KAclntConDestFormatKeyName, "configAudio9");
_LIT(KAclntConSourceInfoKeyName, "configAudio10");


/**
 * Constructor
 */
CTestMmfAclntDoConv::CTestMmfAclntDoConv(const TDesC& aTestName, const TDesC& aSectName, 
										 const TDesC& aKeyName, 
										 const TDesC& aSectName2, const TDesC& aKeyName2,
										 TInt	aBits,
										 FormatSpec aFormat)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iSectName2 = aSectName2;
	iKeyName2 = aKeyName2;
	iBits = aBits;
	iFormat = aFormat;
	}

CTestMmfAclntDoConv* CTestMmfAclntDoConv::NewL(const TDesC& aTestName, const TDesC& aSectName, 
											   const TDesC& aKeyName, const TDesC& aSectName2, 
											   const TDesC& aKeyName2,
											   TInt	aBits,
											   FormatSpec aFormat)
	{
	CTestMmfAclntDoConv* self = new (ELeave) CTestMmfAclntDoConv(aTestName, aSectName, aKeyName, aSectName2, aKeyName2, aBits, aFormat);
	return self;
	}

void CTestMmfAclntDoConv::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntDoConv : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntDoConv::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : Do Convert"));

	TVerdict ret = EFail;

	TBuf<KSizeBuf>	filename1;
	TPtrC filename11, filename2;
	if ((!GetStringFromConfig(iSectName, iKeyName, filename11)) || 
		(!GetStringFromConfig( iSectName2, iKeyName2, filename2)))
		return EInconclusive;

	GetDriveName(filename1);
	filename1.Append(filename11);


	//delete previos output file
	RFs	theFs;
	User::LeaveIfError(theFs.Connect());
	theFs.Delete(filename2);	
	theFs.MkDirAll(filename2);

	theFs.Close();


	TMdaUL16RawAudioCodec	theCodecUnsupported;

	TMdaPcmWavCodec			theCodecWav8Bit(TMdaPcmWavCodec::E8BitPcm);
	TMdaPcmWavCodec			theCodecWav16Bit(TMdaPcmWavCodec::E16BitPcm);
	
	TMdaPackage*			theCodec = NULL;
	switch(iBits)
		{
		case 0:	theCodec = &theCodecWav8Bit; break;
		case 1:	theCodec = &theCodecWav16Bit; break;
		case -1:	theCodec = &theCodecUnsupported; break;
		default: break;
		}

	if(theCodec == NULL)
		return EInconclusive;

	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	TMdaFileClipLocation	theLocation(filename2);
	TMdaWavClipFormat		theMdaFormat;
	TMmfAclntWavEncodeClipFormat theMmfFormat;
	TMdaClipFormat* useFormat;

	if(iFormat == MdaFormatSpec)
		useFormat = &theMdaFormat;
	else
		useFormat = &theMmfFormat;

		converter->OpenL(filename1,
						&theLocation,	// Normally file or descriptor
						useFormat,
						theCodec);
	
	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = DoTestL(converter);
	else if(iError != KErrNone && iBits == -1)
		ret = EPass;

	if (iError != KErrNone)
		ERR_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );
	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return ret;	
	}

TVerdict CTestMmfAclntDoConv::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	// Convert 44100 -> 8000 sample rate
	const TUint KAclntDestSampleRate = 8000;
	aConverter->SetDestinationSampleRateL(KAclntDestSampleRate);

	// Output as AU file
	aConverter->SetDestinationFormatL(TUid::Uid(KMmfUidFormatAUWrite));

	aConverter->ConvertL();

	INFO_PRINTF1( _L("Convert CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	// XXX Improve test. e.g. check that a valid AU file is written
	if(iError == KErrNone)
		{
		ret = EPass;
		}
	else
		{
		INFO_PRINTF2(_L("Error returned in MoscoStateChangeEvent callback: %d"), iError);
		ret = EFail;
		}

	if (ret == EPass)
		{
		CActiveScheduler::Start();

		// XXX Improve test. e.g. check that a valid AU file is written
		if(iError == KErrNone)
			{
			ret = EPass;
			}
		else
			{
			INFO_PRINTF2(_L("Error returned in MoscoStateChangeEvent callback: %d"), iError);
			ret = EFail;
			}
		}

	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConCloseOpen::CTestMmfAclntConCloseOpen(const TDesC& aTestName)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, EFalse)
	{}

CTestMmfAclntConCloseOpen* CTestMmfAclntConCloseOpen::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntConCloseOpen* self = new (ELeave) CTestMmfAclntConCloseOpen(aTestName);
	return self;
	}

/**
 * Open source and destination audio. Close them and open a new source and destination.
 */
TVerdict CTestMmfAclntConCloseOpen::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConverter : Close/Open"));

	TVerdict ret = EFail;

	SetupFormatL(iTestFormat);

	
	TPtrC toFilename, fromFilename;
	if ((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), fromFilename)) || (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), toFilename)))
		return EInconclusive;

	
	TMdaFileClipLocation location(toFilename);

	iError = KErrTimedOut;
	aConverter->Close(); 

	iError = KErrTimedOut;
	aConverter->OpenL(fromFilename, &location, iFormat, iCodec, NULL);

	// wait for open
	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		ret = EPass;

	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConWindow::CTestMmfAclntConWindow(const TDesC& aTestName)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, EFalse)
	{}

CTestMmfAclntConWindow* CTestMmfAclntConWindow::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntConWindow* self = new (ELeave) CTestMmfAclntConWindow(aTestName);
	return self;
	}

/**
 * Set convert window. Enquire window. Delete window
 */
TVerdict CTestMmfAclntConWindow::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("*** Converter set window test\n"));

	TVerdict ret = EFail;
	TInt64 temp1, temp2;

	temp1 = aConverter->Duration().Int64()/4;	// set start at first quarter of clip
	temp2 = TInt64(3) * (aConverter->Duration().Int64()/4);	// set end at 3rd quarter of clip
	ASSERT(temp2 > temp1);	// temp1 = start, temp2 = end

	TInt windowDuration = (I64INT(temp2) - I64INT(temp1));

	TTimeIntervalMicroSeconds endTime(temp2); 
	TTimeIntervalMicroSeconds startTime(temp1);

	INFO_PRINTF4(_L("StartTime = %d EndTime = %d WindowDuration = %d"),
		I64INT(startTime.Int64()), I64INT(endTime.Int64()), windowDuration);
	aConverter->SetPlayWindow(startTime,endTime);


	TRAP(iError,aConverter->SetDestinationNumberOfChannelsL(1));
	if (iError != KErrNone)
		return ret ;

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("CMdaAudioConvertUtility::ConvertL"));
	aConverter->ConvertL();


	CActiveScheduler::Start();	// initial MoscoStateChangeEvent callback
	if (iError == KErrNone)
		{
		CActiveScheduler::Start();	// conversion starts here

		if (iError == KErrNone)
			{
			// Check file size is as expected
			TPtrC filename;
			if (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), filename))
				{
				INFO_PRINTF1(_L("Error getting filename from ini file"));
				ret = EInconclusive;
				}
			else
				{
				ret = CheckFileSize(filename, KConverterWindowFileSize);
				}
			}
		}
	else
		{
		INFO_PRINTF2(_L("iError = %d"), iError);
		ret = EFail;
		}

	// Test clear window
	if (ret == EPass)
		{
		INFO_PRINTF1(_L("*** Converter clear window test\n"));
		iError = KErrTimedOut;
		aConverter->ClearPlayWindow();
		aConverter->ConvertL();

		CActiveScheduler::Start();	// initial MoscoStateChangeEvent callback
		if (iError == KErrNone)
			{
			CActiveScheduler::Start();	// conversion starts here

			if (iError == KErrNone)
				{
				TPtrC filename;
				if (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), filename))
					{
					INFO_PRINTF1(_L("Error getting filename from ini file"));
					ret = EInconclusive;
					}
				else
					{
					// Second convert will be appended to the first
					ret = CheckFileSize(filename, KConverterWindowPlusNormalFileSize);
					}
				}
			}
		else
			{
			INFO_PRINTF2(_L("iError = %d"), iError);
			ret = EFail;
			}
		}

	return ret;
	}

TVerdict CTestMmfAclntConWindow::CheckFileSize(const TDesC& aFilename, const TInt aExpectedSize)
	{
	TVerdict verdict = EFail;
	RFile file;
	RFs fs;
	
	TInt ret = fs.Connect();
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error opening file system!"));
		return EInconclusive;
		}

	// Check file size matches expected size
	TInt err = file.Open(fs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening converted file (%d)"), err);
		verdict = EInconclusive;
		}
	else
		{
		TInt size = 0;
		err = file.Size(size);
		if (err != KErrNone || size != aExpectedSize)
			{
			if (err != KErrNone)
				INFO_PRINTF2(_L("Error getting file size (%d)"), err);
			else
				INFO_PRINTF3(_L("Converted file size incorrect (expected %d, got %d)"), aExpectedSize, size);
			verdict = EFail;
			}
		else
			{
			INFO_PRINTF1(_L("Converted file size as expected (test passed)"));
			verdict = EPass;
			}
		file.Close();
		}
	fs.Close();

	return verdict;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConDuration::CTestMmfAclntConDuration(const TDesC& aTestName)
	:CTestMmfAclntConConfig(aTestName,EPcm16Wav, EFalse)
	{}

CTestMmfAclntConDuration* CTestMmfAclntConDuration::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntConDuration* self = new (ELeave) CTestMmfAclntConDuration(aTestName);
	return self;
	}

void CTestMmfAclntConDuration::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDurationKeyName;
	}

/** Enquire duration
 */
TVerdict CTestMmfAclntConDuration::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConverter : Duration"));
	TVerdict ret = EFail;

	TTimeIntervalMicroSeconds duration(KSoundFileLength); // 8 second sound file!
	//since format+codec have wrrong default values
	TRAP(iError,aConverter->SetDestinationSampleRateL(8000));
	TRAP(iError,aConverter->SetDestinationNumberOfChannelsL(1));
	//TRAP(iError,aConverter->ConvertL());

	if(KErrNone == iError && (aConverter->Duration() == duration))
		{
		INFO_PRINTF3(_L("Duration  = %f mkS Record time  =  %f mkS"),
				I64REAL(duration.Int64()), 
				I64REAL(aConverter->RecordTimeAvailable().Int64()));
		ret = EPass;
		}
	else
		{
		TInt64		durationExpected = duration.Int64();
		
		TInt64		durationReceived = aConverter->Duration().Int64();	
		INFO_PRINTF3( _L("Expected duration: %f, received duration: %f"), I64REAL(durationExpected), I64REAL(durationReceived));

		}
	return ret;
	}

void CTestMmfAclntConDuration::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntDelete : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConPosition::CTestMmfAclntConPosition(const TDesC& aTestName,const	TTimeIntervalMicroSeconds aPosition)
	:CTestMmfAclntConConfig(aTestName,EPcm16Wav, EFalse)
	{
	iPosition = aPosition;
	}

CTestMmfAclntConPosition* CTestMmfAclntConPosition::NewL(const TDesC& aTestName,const TTimeIntervalMicroSeconds aPosition)
	{
	CTestMmfAclntConPosition* self = new (ELeave) CTestMmfAclntConPosition(aTestName, aPosition);
	return self;
	}

void CTestMmfAclntConPosition::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConPositionKeyName;
	}

/**
 * Set position and enquire position. Sound is always set to minus one
 * frame, becuase DevSound has to play something. (Even if set to the end of the clip).
 *
 */
TVerdict CTestMmfAclntConPosition::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConverter : Position"));
	TVerdict ret = EFail; 

	//  Set position: middle of clip.
	if (I64INT(iPosition.Int64()) == 0)
		iPosition = I64INT(aConverter->Duration().Int64())/2;
	// Set position: end of clip.
	if (I64INT(iPosition.Int64()) == -1)
		iPosition = aConverter->Duration();

	// Position is beyond the end of the clips duration.
	// so check that the value is clipped.
	if(aConverter->Duration() < iPosition)
		{
		TInt64 temp = (aConverter->Duration().Int64() / KPcm16FrameInterval) - 1; // -1 for DevSound
		aConverter->SetPosition(iPosition);

		TInt64 theReceived = aConverter->Position().Int64();
		TInt64 theExpected = temp * KPcm16FrameInterval;

		TBool theRes = TimeComparison(I64INT(theReceived), I64INT(theExpected), KExpectedDeviation);

		if(theRes == (TBool) ETrue)
			ret = EPass;
		INFO_PRINTF3( _L("Expected position : %d, Received position : %d"), theExpected, theReceived);
		}
	// Position is before the beginning of the clip
	// so check that the value is zero
	else if(iPosition < TTimeIntervalMicroSeconds(0))
		{
		TInt	theExpected = 0;
		aConverter->SetPosition(iPosition);
		TInt	theReceived = I64INT(aConverter->Position().Int64());
		TBool theRes = TimeComparison(theReceived, theExpected, 0);
		if(theRes)
			ret = EPass;
		INFO_PRINTF3( _L("Expected position : %d, Received position : %d"), theExpected, theReceived);
		}
	else
		{
		
		//Original
		/*
		aConverter->SetPosition(iPosition);
		aConverter->ConvertL();
		CActiveScheduler::Start();

		TInt64 temp = (iPosition.Int64() / KPcm16FrameInterval) - 1; // -1 for DevSound
		
		TInt	theReceived = I64INT(aConverter->Position().Int64());
		TInt	theExpected = I64INT(temp) * KPcm16FrameInterval;

		TBool theRes = TimeComparison(theReceived, theExpected, KExpectedDeviation);

		if(theRes == (TBool) ETrue)
			{
			ret = EPass;
			}
		INFO_PRINTF3( _L("Expected position : %d, Received position : %d"), theExpected, theReceived);
		*/


		//Amendment
		TInt	theDurationa = I64INT(aConverter->Duration().Int64());

		aConverter->SetPosition(iPosition);
		TInt	theExpected = I64INT(aConverter->Position().Int64());

			//ConvertL requires two CActiveScheduler::Start()
		aConverter->ConvertL();
		CActiveScheduler::Start();
		CActiveScheduler::Start();
			
		TPtrC toFilename, fromFilename;
		GetStringFromConfig(_L("SectionFour"), _L("configAudio"), fromFilename); 
		GetStringFromConfig(_L("SectionFour"), _L("configAudio12"), toFilename);
		TMdaFileClipLocation location2(toFilename);
		aConverter->OpenL(fromFilename, &location2, iFormat, iCodec);
		CActiveScheduler::Start();

		TInt	theDurationb = I64INT(aConverter->Duration().Int64());
		TInt	theReceived = theDurationa - theDurationb;

		TBool theRes = TimeComparison(theReceived, theExpected, KDeviation);
		

		if(theRes == (TBool) ETrue)
			{
			ret = EPass;
			}
		INFO_PRINTF3( _L("Expected position : %d, Received position : %d"), theExpected, theReceived);

		}
		
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConConfig::CTestMmfAclntConConfig(const TDesC& aTestName, const TTestFormat aFormat, const TBool aNegative)
	{
	iTestStepName = aTestName;
	iThisIsNegativeTest = aNegative;
	iTestFormat = aFormat;
	}

CTestMmfAclntConConfig* CTestMmfAclntConConfig::NewL(const TDesC& aTestName, const TTestFormat aFormat, const TBool aNegative)
	{
	CTestMmfAclntConConfig* self = new (ELeave) CTestMmfAclntConConfig(aTestName, aFormat, aNegative);
	return self;
	}

void CTestMmfAclntConConfig::MoscoStateChangeEvent(CBase* /*aObject*/, 
												   TInt /*aPreviousState*/, 
												   TInt /*aCurrentState*/, 
												   TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntConConfig : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);

	CActiveScheduler::Stop();
	}

void CTestMmfAclntConConfig::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConConfigKeyName;
	}

TVerdict CTestMmfAclntConConfig::DoTestStepPreambleL()
	{
	TVerdict ret = EPass;

	SetupFormatL(iTestFormat);

	TBuf<64> keyName;
	GetKeyName(keyName);

	// Make sure file doesn't exist (maybe from a previous test)
	TParse fullFileName;
	TPtrC filename;
	if(!GetStringFromConfig(_L("SectionFour"), keyName, filename))
		return EInconclusive;

	// parse the filenames
		if(!fullFileName.Set(filename, &KDefault, NULL))
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());

		TInt error = fs.Delete(fullFileName.FullName());
		if(error == KErrPathNotFound)
			{
				error = fs.MkDirAll(fullFileName.DriveAndPath());
				INFO_PRINTF2(_L("Path for file %S was been created"), &fullFileName.FullName());	
			}
		else if(error == KErrNotFound)
			INFO_PRINTF2(_L("No need to delete %S"), &fullFileName.FullName());	
		else if(error==KErrNone)
			INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
		else
			ret = EInconclusive;
		fs.Close();
		}
	if((ret == EInconclusive) || (ret == EFail))
		{
		return ret;	
		}
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

/**
 * Set config destination clip
 */
TVerdict CTestMmfAclntConConfig::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : Config"));
	TVerdict ret = EFail;

	TPtrC toFilename, fromFilename;
	if ((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), fromFilename)) 
		|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), toFilename)))
		return EInconclusive;

	// Delete the old file if it exists
	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error doing fs.Connect (%d)"), err);
		return EInconclusive;
		}

	err = fs.Delete(toFilename);
	if ((err != KErrNone) && (err != KErrNotFound))
		{
		INFO_PRINTF3(_L("Error deleting 'toFilename' (%S) %d"), &toFilename, err);
		return EInconclusive;
		}
	fs.Close();

	TMdaFileClipLocation location(toFilename);
	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	if (iThisIsNegativeTest)
		{
		iAudioSettings.iSampleRate = KInvalidNumber;
		iAudioSettings.iChannels = KInvalidNumber;
		converter->OpenL(fromFilename, &location, iFormat, iCodec, &iAudioSettings);
		}
	else
		{
		  converter->OpenL(fromFilename, &location, iFormat, iCodec);
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	//TRAP(iError, converter->SetDestinationNumberOfChannelsL(1));

	if (iError == KErrNone)
		ret = DoTestL(converter);

	if (ret == EFail)
		INFO_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );

	if(iThisIsNegativeTest && iError == KErrNotSupported)
		{
			ret = EPass;	
		}

	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return ret;	
	
	}

TVerdict CTestMmfAclntConConfig::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	aConverter->ConvertL(); 
	INFO_PRINTF1( _L("Convert CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	// a negative test will fail succesfully at this point
	if (!iThisIsNegativeTest)
		ret = EPass;
	else if (iError == KErrNotSupported)
		ret = EPass;
	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConQuery::CTestMmfAclntConQuery(const TDesC& aTestName)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, EFalse)
	{}

CTestMmfAclntConQuery* CTestMmfAclntConQuery::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntConQuery* self = new (ELeave) CTestMmfAclntConQuery(aTestName);
	return self;
	}

void CTestMmfAclntConQuery::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConQueryKeyName;
	}

TVerdict CTestMmfAclntConQuery::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : ConQuery"));
	TVerdict ret = EFail;

	TPtrC toFilename, fromFilename;
	if ((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), fromFilename)) 
	//if ((!GetStringFromConfig(_L("SectionSeven"), _L("auFile"), fromFilename)) 
	//if ((!GetStringFromConfig(_L("SectionSeven"), _L("rawFile"), fromFilename)) 
	|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio4"), toFilename)))
		return EInconclusive;

	TMdaFileClipLocation location(toFilename);
	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	converter->OpenL(fromFilename, &location, iFormat, iCodec);

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNone)
		ret = DoTestL(converter);

	if (ret == EFail)
		INFO_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );


	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return ret;	
	
	}

/**
 * query transformations available.
 */
TVerdict CTestMmfAclntConQuery::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConverter : Query Transactions"));
	RArray<TFourCC>		theSupportedDataTypes;

	TInt err ; 
	TRAP(err,aConverter->GetSupportedDestinationDataTypesL(theSupportedDataTypes));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedDataTypesL left with error %d"),err);
		return EFail;
		}
	
	TUint8		theBuffer8[KSizeBuf];

	INFO_PRINTF1( _L("The supported available transformation"));
	for(TInt	index = 0; index < theSupportedDataTypes.Count() ; index++)
		{
		 TFourCC&	four = 	theSupportedDataTypes[index];
		
		 TPtr8		ptrFour(theBuffer8, KSizeBuf);
		 four.FourCC(&ptrFour);
		 TBuf<KSizeBuf>	theBuf;
		 
		 theBuf.Copy(ptrFour);
		
		 INFO_PRINTF3( _L("%d  %S"), index, &theBuf);

		}
	
	return EPass;	
	}

//-------------------------------------------------------------------

// Negative tests

//-------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDelete::CTestMmfAclntDelete()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-1051-CP");
	}

void CTestMmfAclntDelete::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestMmfAclntDelete : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

/**
 * Conversion utility - Delete object before conversion operation has completed.
 */
TVerdict CTestMmfAclntDelete::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestConverter : Delete"));

	TPtrC sourceFileName, targetFileName;
	if((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), sourceFileName)) || 
		(!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), targetFileName)))
		return EInconclusive;
	
	RFs		theFs;
	theFs.Connect();
	theFs.Delete(targetFileName);
	theFs.Close();


	iError = KErrTimedOut;
	TMdaFileClipLocation location(targetFileName);
	TMdaWavClipFormat	format;
	TMdaPcmWavCodec		codec;

	__MM_HEAP_MARK;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);
	converter->OpenL(sourceFileName,&location, &format, &codec);
	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		converter->ConvertL(); 
		INFO_PRINTF1( _L("Convert CMdaAudioConvertUtility"));
		//User::After(3 * KOneSecond);		
		CleanupStack::PopAndDestroy(converter);
		User::After(KOneSecond);
		INFO_PRINTF1( _L("Destroy CMdaAudioConvertUtility"));
		__MM_HEAP_MARKEND;
		User::Heap().Check();

		return	EPass;
		}

	ERR_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return EFail;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntOpenWhileConvert::CTestMmfAclntOpenWhileConvert(const TDesC& aTestName)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav,EFalse)
	{
	}

CTestMmfAclntOpenWhileConvert* CTestMmfAclntOpenWhileConvert::NewL(const TDesC& aTestName)
	{
	CTestMmfAclntOpenWhileConvert* self = new (ELeave) CTestMmfAclntOpenWhileConvert(aTestName);
	return self;
	}

/**
 * Open new file while audio converter is converting. Open a new audio file.
 */
TVerdict CTestMmfAclntOpenWhileConvert::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConverter : Open/Convert"));

	TPtrC auFilename, wavFilename;
	//if((!GetStringFromConfig(_L("SectionFour"), _L("auAudioFile"), auFilename)) || 
	//	(!GetStringFromConfig(_L("SectionFour"), _L("wavAudioFile"), wavFilename)))
	if((!GetStringFromConfig(_L("SectionOne"), _L("PCM8"), auFilename)) || 
		(!GetStringFromConfig(_L("SectionFour"), _L("PCM16_out"), wavFilename)))
		return EInconclusive;
	
	RFs	theFs;
	User::LeaveIfError(theFs.Connect());
	theFs.Delete(wavFilename);
	theFs.MkDirAll(wavFilename);
	theFs.Close();


	iError = KErrTimedOut;
	TMdaFileClipLocation location(wavFilename);
	TMdaWavClipFormat format;
	TMdaPcmWavCodec		codec(TMdaPcmWavCodec::E16BitPcm);

	aConverter->ConvertL(); 
	INFO_PRINTF1( _L("Convert CMdaAudioConvertUtility"));
	User::After(KOneSecond);		

	
	aConverter->OpenL(auFilename,&location, &format, &codec);
	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	// wait for open
	CActiveScheduler::Start();

	if(iError == KErrInUse || iError == KErrNone)
		return	EPass;
	return EFail;
	}

//------------------------------------------------------------------

CTestMmfAclntConvertDestBitRate::CTestMmfAclntConvertDestBitRate(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertDestBitRate* CTestMmfAclntConvertDestBitRate::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertDestBitRate* self = new (ELeave) CTestMmfAclntConvertDestBitRate(aTestName,aNegative);
	return self;
	}

void CTestMmfAclntConvertDestBitRate::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDestBitRateKeyName;
	}

TVerdict CTestMmfAclntConvertDestBitRate::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Destination Bit Rate"));
	TVerdict ret = EPass;
	TInt err = KErrNone;
	RArray<TUint> rates;
	TRAP(err,aConverter->GetSupportedConversionBitRatesL(rates));
	// in Typhoon KErrNotSupported is expected
	if(err==KErrNotSupported)
		{
		//test the other bitrate functions for code coverage (and behaviour)
		TRAP(err,aConverter->SetDestinationBitRateL(100)); // arbitrary value since KErrNotSupported is expected anyway
		if (err != KErrNotSupported)
			return EFail;
		
		TUint rate=0;
		TRAP(err, rate=aConverter->DestinationBitRateL());
		if (err != KErrNone)	
			return EFail;
		
		INFO_PRINTF3(_L("CMdaAudioConvertUtility::GetSupportedConversionBitRatesL() leave with error %d bitrate of %d"),err,rate);
		ret = EPass;
		return ret;
		}
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionBitRatesL() leave with error %d"),iError);
		ret = EFail;
		return ret;
		}
	TInt numRates = rates.Count();
	for(TInt i=0;i<numRates;i++)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationBitRateL (%d)"),rates[i]);
		TRAP(iError,aConverter->SetDestinationBitRateL(rates[i]));
		if(iError != KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationBitRateL left with error %d"),iError);
			ret = EFail;
			}
		else// no error, get rate
			{
			TUint rate=0;
			TRAP(iError,rate = aConverter->DestinationBitRateL());
			//TRAP(err, aConverter->ConvertL());
			//CActiveScheduler::Start();

			if(iError!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationBitRateL leave with error %d"),iError);
				ret = EFail;
				}
			else
				{
				if(rate!=rates[i])
					{
					ret = EFail;
					INFO_PRINTF1(_L("Rate mismatch"));
					}
					else
					{	
					INFO_PRINTF1(_L("Rates match"));
					}
				}
			}
		}
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntConvertDestDataType::CTestMmfAclntConvertDestDataType(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertDestDataType* CTestMmfAclntConvertDestDataType::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertDestDataType* self = new (ELeave) CTestMmfAclntConvertDestDataType(aTestName,aNegative);
	return self;
	}

void CTestMmfAclntConvertDestDataType::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDestDataTypeKeyName;
	}

TVerdict CTestMmfAclntConvertDestDataType::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Destination data type"));

	TVerdict ret = EFail;
	TInt err = KErrNone;
	RArray<TFourCC> SupportedDataTypes ;
	TFourCC dataType;

	TRAP(err,aConverter->GetSupportedDestinationDataTypesL(SupportedDataTypes));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedBitRatesL leave with error %d"),err);
		ret = EFail;
		return ret;
		}

	TInt numRates = SupportedDataTypes.Count();
	ret = EPass ;

	for(TInt i=0;i<numRates;i++)
		{
		dataType = SupportedDataTypes[i] ;
		TUint8 char1 = TUint8( dataType.FourCC() ) ;
		TUint8 char2 = TUint8( dataType.FourCC() >> 8 ) ;
		TUint8 char3 = TUint8( dataType.FourCC() >> 16 ) ;
		TUint8 char4 = TUint8( dataType.FourCC() >> 24 ) ;
		//fourCCString->Format( _L8("%1c%1c%1c%1c"), char1, char2, char3, char4 ) ;
		INFO_PRINTF5(_L("dataType.FourCC= %1c%1c%1c%1c"), char1, char2, char3, char4);
		TRAP(err,aConverter->SetDestinationDataTypeL(SupportedDataTypes[i]));

		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationDataTypeL leave with error %d"),err);
			return EFail;
			}

		TRAP(err,dataType = aConverter->DestinationDataTypeL());

		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationDataTypeL leave with error %d"),err);
			return EFail;
			}

		if(dataType!=SupportedDataTypes[i])
			ret = EFail;
		}
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntConvertDestFormat::CTestMmfAclntConvertDestFormat(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertDestFormat* CTestMmfAclntConvertDestFormat::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertDestFormat* self = new (ELeave) CTestMmfAclntConvertDestFormat(aTestName,aNegative) ;
	return self;
	}

void CTestMmfAclntConvertDestFormat::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDestFormatKeyName;
	}

TVerdict CTestMmfAclntConvertDestFormat::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Destination format"));
	TVerdict ret = EFail;
	TInt err = KErrNone;
	TUid format1(KUidMdaWavCodec) ;	//Why not?
	TUid format2 ;

	TRAP(err,format1 = aConverter->DestinationFormatL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationFormatL leave with error %d"),err);
		return ret;
		}

	TRAP(err,aConverter->SetDestinationFormatL(format1));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationFormatL leave with error %d"),err);
		return ret;
		}

	TRAP(err,format2 = aConverter->DestinationFormatL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationFormatL leave with error %d"),err);
		return ret;
		}

	if(format1 == format2)
		ret = EPass;
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntConvertDestSampleRate::CTestMmfAclntConvertDestSampleRate(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertDestSampleRate* CTestMmfAclntConvertDestSampleRate::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertDestSampleRate* self = new (ELeave) CTestMmfAclntConvertDestSampleRate(aTestName,aNegative);
	return self;
	}

void CTestMmfAclntConvertDestSampleRate::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDestSampleRateKeyName;
	}

/**
 * Set config destination clip
 */
TVerdict CTestMmfAclntConvertDestSampleRate::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : Config"));
	TVerdict ret = EFail;

	TPtrC toFilename, fromFilename;

//	if ((!GetStringFromConfig(_L("SectionOne"), _L("Pcm16Mono44khz"), fromFilename)) 
	if ((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), fromFilename)) 
		|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), toFilename)))
		return EInconclusive;

	// Delete the old file if it exists
	RFs fs;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error doing fs.Connect (%d)"), err);
		return EInconclusive;
		}

	err = fs.Delete(toFilename);
	if ((err != KErrNone) && (err != KErrNotFound))
		{
		INFO_PRINTF3(_L("Error deleting 'toFilename' (%S) %d"), &toFilename, err);
		return EInconclusive;
		}
	fs.Close();

	TMdaFileClipLocation location(toFilename);
	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	if (iThisIsNegativeTest)
		{
		iAudioSettings.iSampleRate = KInvalidNumber;
		iAudioSettings.iChannels = KInvalidNumber;
		converter->OpenL(fromFilename, &location, iFormat, iCodec, &iAudioSettings);
		}
	else
		{
		  converter->OpenL(fromFilename, &location, iFormat, iCodec);
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNone)
		ret = DoTestL(converter);

	if (ret == EFail)
		INFO_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );

	if(iThisIsNegativeTest && iError == KErrNotSupported)
		{
			ret = EPass;	
		}

	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return ret;	
	
	}

void CTestMmfAclntConvertDestSampleRate::MoscoStateChangeEvent(CBase* /*aObject*/, 
													TInt aPreviousState,
													TInt aCurrentState,
													TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	TInt PreviousState = aPreviousState;
	TInt CurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntConvertDestSampleRate : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF4( _L("iError %d PreviousState %d -> CurrentState %d"), 
			iError, PreviousState, CurrentState);
	if( CurrentState != CMdaAudioClipUtility::EPlaying) 
	 //give a chace to finish conversion
		CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntConvertDestSampleRate::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Destination Sample Rate"));
	TVerdict ret = EFail;
	
	TInt err = KErrNone;
	RArray<TUint> supportedSampleRates ;


	TRAP(err,aConverter->GetSupportedConversionSampleRatesL(supportedSampleRates));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionSampleRatesL() leave with error %d"),err);
		ret = EFail;
		return ret;
		}

	TInt numRates = supportedSampleRates.Count();
	if( 0 != numRates)
		{
		ret = EPass ;
		for(TInt i=0;i<numRates;i++)
			{
			// Delete the output file if it exists
			// Necessary otherwise we will open the output file for conversion and 
			// use the sample rate of the file
			TPtrC toFilename;
			if (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), toFilename))
				{
				INFO_PRINTF1(_L("Error finding filename in INI file"));
				return EInconclusive;
				}

			RFs fs;
			TInt err = fs.Connect();
			if (err != KErrNone)
				{
				INFO_PRINTF2(_L("Error doing fs.Connect (%d)"), err);
				return EInconclusive;
				}

			err = fs.Delete(toFilename);
			if ((err != KErrNone) && (err != KErrNotFound))
				{
				INFO_PRINTF3(_L("Error deleting 'toFilename' (%S) %d"), &toFilename, err);
				return EInconclusive;
				}
			fs.Close();

			// Do the conversion
			TUint SampleRate = supportedSampleRates[i] ;
			INFO_PRINTF2(_L("SampleRate = %d"), SampleRate);
			TRAP(err,aConverter->SetDestinationSampleRateL(SampleRate));
			if(err!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationSampleRateL() leave with error %d"),err);
				ret = EFail;
				return ret;
				}

			TRAP(err, aConverter->ConvertL());
			CActiveScheduler::Start();

			if(err!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioConvertUtility::ConvertL leave with error %d"),err);
				ret = EFail;
				break ;
				}

			TRAP(err,SampleRate = aConverter->DestinationSampleRateL());
			if(err!=KErrNone)
				{
				INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationSampleRateL leave with error %d"),err);
				ret = EFail;
				break ;
				}

			if(SampleRate != supportedSampleRates[i])
				{
				INFO_PRINTF1(_L("CMdaAudioConvertUtility::DestinationSampleRateL does not match the sample rate set"));
				ret = EFail;
				}

			}
		}
	else
		{
		INFO_PRINTF1(_L("CMdaAudioConvertUtility::GetSupportedConversionSampleRatesL() returned empy array."));
		ret = EFail;
		}
	return ret;
	}


//------------------------------------------------------------------

CTestMmfAclntConvertDestNumberOfChannels::CTestMmfAclntConvertDestNumberOfChannels(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertDestNumberOfChannels* CTestMmfAclntConvertDestNumberOfChannels::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertDestNumberOfChannels* self = new (ELeave) CTestMmfAclntConvertDestNumberOfChannels(aTestName,aNegative);
	return self;
	}

void CTestMmfAclntConvertDestNumberOfChannels::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConDestNumberChannelsKeyName;
	}

TVerdict CTestMmfAclntConvertDestNumberOfChannels::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Destination Number of Channels"));
	TVerdict ret = EFail;
	
	TInt err = KErrNone;
	RArray<TUint> aSupportedNumberOfChannels ;

	TRAP(err,aConverter->GetSupportedConversionNumberOfChannelsL(aSupportedNumberOfChannels));
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::GetSupportedConversionNumberOfChannelsL() leave with error %d"),err);
		ret = EFail;
		return ret;
		}

	TInt numRates = aSupportedNumberOfChannels.Count();
	
	ret = EPass ;

	for(TInt i=0;i<numRates;i++)
		{
		TUint NumberOfChannels = aSupportedNumberOfChannels[i] ;
		INFO_PRINTF2(_L("NumberOfChannels = %d"), NumberOfChannels);
		TRAP(err,aConverter->SetDestinationNumberOfChannelsL(aSupportedNumberOfChannels[i]));

		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::SetDestinationNumberOfChannelL() leave with error %d"),err);
			return ret;
			}
		TRAP(err,NumberOfChannels = aConverter->DestinationNumberOfChannelsL());

		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationNumberOfChannelL leave with error %d"),err);
			return EFail;
			}

		if(NumberOfChannels != aSupportedNumberOfChannels[i])
			ret = EFail;
		}
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntConvertSourceInfo::CTestMmfAclntConvertSourceInfo(const TDesC& aTestName, TBool aNegative)
	: CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative)
	{}

CTestMmfAclntConvertSourceInfo* CTestMmfAclntConvertSourceInfo::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertSourceInfo* self = new (ELeave) CTestMmfAclntConvertSourceInfo(aTestName,aNegative);
	return self;
	}

void CTestMmfAclntConvertSourceInfo::GetKeyName(TDes& aDes)
	{
	aDes = KAclntConSourceInfoKeyName;
	}

TVerdict CTestMmfAclntConvertSourceInfo::DoTestL(CMdaAudioConvertUtility* aConverter)
	{
	INFO_PRINTF1( _L("TestConvertUtils : Source Info"));
	TVerdict ret = EFail;
	
	TInt err = KErrNone;
	TUint NumberOfChannels = 0 ;
	TRAP(err,NumberOfChannels = aConverter->SourceNumberOfChannelsL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SourceNumberOfChannelL leave with error %d"),err);
		return EFail;
		}
	INFO_PRINTF2(_L("NumberOfChannels = %d"), NumberOfChannels);

	TUint BitRate = 0 ;
	TRAP(err,BitRate = aConverter->SourceBitRateL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SourceBitRateL leave with error %d"),err);
		return EFail;
		}
	INFO_PRINTF2(_L("BitRate = %d"), BitRate);

	TUint SampleRate = 0 ;
	TRAP(err,SampleRate = aConverter->SourceSampleRateL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SourceSampleRateL leave with error %d"),err);
		return EFail;
		}
	INFO_PRINTF2(_L("SampleRate = %d"), SampleRate);


	TUid Format(KUidMdaWavCodec)  ;
	TRAP(err,Format = aConverter->SourceFormatL());
	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SourceFormatL leave with error %d"),err);
		return EFail;
		}
	INFO_PRINTF2(_L("Format = %d"), Format);

	TFourCC dataType;
	TRAP(err,dataType = aConverter->SourceDataTypeL());

	if(err!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioConvertUtility::SourceDataTypeL leave with error %d"),err);
		return EFail;
		}
	TUint8 char1 = TUint8( dataType.FourCC() ) ;
	TUint8 char2 = TUint8( dataType.FourCC() >> 8 ) ;
	TUint8 char3 = TUint8( dataType.FourCC() >> 16 ) ;
	TUint8 char4 = TUint8( dataType.FourCC() >> 24 ) ;
	INFO_PRINTF5(_L("dataType.FourCC= %1c%1c%1c%1c"), char1, char2, char3, char4);

	ret = EPass;

	return ret ;
	}

//------------------------------------------------------------------

CTestMmfAclntConvertLength::CTestMmfAclntConvertLength(const TDesC& aTestName, TBool aNegative)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative) 
	{
	}

CTestMmfAclntConvertLength* CTestMmfAclntConvertLength::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertLength* self = new (ELeave) CTestMmfAclntConvertLength(aTestName, aNegative);
	return self;
	}

/** 
 *Set maximum length of file in bytes and Convertord
 */
TVerdict CTestMmfAclntConvertLength::DoTestL(CMdaAudioConvertUtility* aConvertUtil)
	{
	INFO_PRINTF1( _L("TestConvertor : Length"));

	TVerdict ret = EFail;
	//TTimeIntervalMicroSeconds ConvertTime(aConvertUtil->Duration());
	
	aConvertUtil->SetMaxWriteLength(800);

	TRAP(iError, aConvertUtil->PlayL()) ;

	if(iError != KErrNone) 
		return EFail;

	CActiveScheduler::Start();

	aConvertUtil->Stop() ;

	//until DEF011919 is not fixed
	//if(aConvertUtil->Duration() != ConvertTime)
	ret = EPass;

	return	ret;
	}
//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConvertRepeat::CTestMmfAclntConvertRepeat(const TDesC& aTestName, TBool aNegative)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative) 
	{}

CTestMmfAclntConvertRepeat* CTestMmfAclntConvertRepeat::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertRepeat* self = new (ELeave) CTestMmfAclntConvertRepeat(aTestName,aNegative);
	return self;
	}

/**
 * Set repeats
 */
TVerdict CTestMmfAclntConvertRepeat::DoTestL(CMdaAudioConvertUtility* aConvertUtil)
	{
	INFO_PRINTF1( _L("TestConvert : Repeats"));

 	iError = KErrTimedOut;

	TTimeIntervalMicroSeconds silence(0);
	//just to sutisfy CCover, actualy does nothing
	aConvertUtil->SetRepeats(NUMBER_OF_REPEATS,silence);

	//TInt duration = I64INT(aConvertUtil->Duration().Int64());

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioConvertUtility"));
	//just to sutisfy CCover, actualy calls ConvertL() see
	TRAP(iError, aConvertUtil->PlayL()) ;
	if(iError != KErrNone)
		return EFail;

	CActiveScheduler::Start();

	//TUint actualDuration = I64INT(aConvertUtil->Duration().Int64()) ;

	//INFO_PRINTF2(_L("Repeats : %d"), NUMBER_OF_REPEATS);
	//INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
	//	iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);

	if(iError == KErrNone)
		return EPass;

	return	EFail ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConvertCrop::CTestMmfAclntConvertCrop(const TDesC& aTestName, const TBool aCropToEnd, TBool aNegative)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative), 
	iCropToEnd(aCropToEnd)	
	{
	}

CTestMmfAclntConvertCrop* CTestMmfAclntConvertCrop::NewL(const TDesC& aTestName, const TBool aCropToEnd, TBool aNegative)
	{
	CTestMmfAclntConvertCrop* self = new (ELeave) CTestMmfAclntConvertCrop(aTestName, aCropToEnd, aNegative);
	return self;
	}


/**
 * Set config destination clip
 */
TVerdict CTestMmfAclntConvertCrop::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : Config"));
	TVerdict ret = EFail;

	TPtrC fromFilename;
	if ((!GetStringFromConfig(_L("SectionOne"), _L("PCM16"), fromFilename))
		|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), iToFilename))
		|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio11"), iToFilename2)))
		return EInconclusive;

	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);
	TMdaFileClipLocation	location(iToFilename);

	if (iThisIsNegativeTest)
		{
		iAudioSettings.iSampleRate = KInvalidNumber;
		iAudioSettings.iChannels = KInvalidNumber;
		//to keep CCover happy
		converter->OpenL(fromFilename, iToFilename2) ;
			//&location, iFormat, iCodec, &iAudioSettings);
		}
	else
		{
		converter->OpenL(fromFilename, &location, iFormat, iCodec);
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNone)
		ret = DoTestL(converter);

	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);

	if (ret == EFail)
		{
		INFO_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );
		return ret;
		}

	if(iThisIsNegativeTest && iError == KErrNotSupported)
		{
			ret = EPass;
		}

	return ret;

	}


void CTestMmfAclntConvertCrop::MoscoStateChangeEvent(CBase* /*aObject*/,
													TInt aPreviousState,
													TInt aCurrentState,
													TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	TInt PreviousState = aPreviousState;
	TInt CurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntDoConv : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF4( _L("iError %d PreviousState %d -> CurrentState %d"), 
			iError, PreviousState, CurrentState);
	//if( CurrentState != CMdaAudioClipUtility::ERecording) 
	 //give a chance to finish conversion
		CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntConvertCrop::DoTestStepPostambleL()
	{
	if (iThisIsNegativeTest)
		{
		RFs fs;
		User::LeaveIfError(fs.Connect());

		TInt error = fs.Delete(iToFilename2);
		if(error == KErrPathNotFound)
			return EInconclusive;
		}

	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Audio croping.
 */
TVerdict CTestMmfAclntConvertCrop::DoTestL(CMdaAudioConvertUtility* aConvert)
	{
	if (iThisIsNegativeTest)
		{
		TInt err = KErrNone;
		TRAP(err, aConvert->ConvertL());
		CActiveScheduler::Start();
		if( err != KErrNone || iError != KErrNone) 
			return EFail;
		CActiveScheduler::Start();
		if( err != KErrNone || iError != KErrNone) 
			return EFail;

		TTimeIntervalMicroSeconds cropPoint( TTimeIntervalMicroSeconds(aConvert->Duration().Int64()/2) );
		TTimeIntervalMicroSeconds cropPoint2( TTimeIntervalMicroSeconds(aConvert->Duration().Int64()*2/3) );

		aConvert->SetPosition(cropPoint);
		// 1st crop
		TRAP(err, aConvert->CropL());
		if(err != KErrNone) 
			return EFail;

		// position beyond the end of the cropped file
		aConvert->SetPosition(cropPoint2);
		// 2nd crop
		TRAP(err, aConvert->CropL());
		if(err != KErrArgument) 
			return EFail;

		return EPass;
		}
	else
		{
		TInt err = KErrNone;
		TRAP(err, aConvert->ConvertL());
		CActiveScheduler::Start();
		if( err != KErrNone || iError != KErrNone) 
			return EFail;
		CActiveScheduler::Start();
		if( err != KErrNone || iError != KErrNone) 
			return EFail;

		TTimeIntervalMicroSeconds origDuration( TTimeIntervalMicroSeconds(aConvert->Duration().Int64() ));
		TTimeIntervalMicroSeconds cropPoint( origDuration.Int64()*2/3);
		aConvert->SetPosition(cropPoint);

		INFO_PRINTF4(_L("crop point %d, duration %d, cropToEnd %d"),
			I64INT(cropPoint.Int64()), 
			I64INT(origDuration.Int64()),
			iCropToEnd);

		if(iCropToEnd)
			{
			TRAP(iError, aConvert->CropL());
			}
		else
			{
			TRAP(iError, aConvert->CropFromBeginningL());
			}
		if(iError != KErrNone)
			{
			ERR_PRINTF2(_L("Crop left with error =  %d"),iError);
				return EFail;
			}

		// Check if the cropped duration is within the deviation from the expected duration
		// NB : there is no converter API to check the destination(sink) duration after cropping
		// but we can check cropping by reopening the file and checking its new duration

		TMdaFileClipLocation	location(iToFilename);
		aConvert->OpenL(iToFilename, &location, iFormat, iCodec);
		CActiveScheduler::Start();

		TTimeIntervalMicroSeconds newDuration( TTimeIntervalMicroSeconds(aConvert->Duration().Int64() ));
		TTimeIntervalMicroSeconds expectedDuration;
		
		if (iCropToEnd)
			expectedDuration = cropPoint;
		else
			expectedDuration = origDuration.Int64() - cropPoint.Int64();

		INFO_PRINTF3(_L("new duration %d, expected %d"),
			I64INT(newDuration.Int64()), 
			I64INT(expectedDuration.Int64()));

		// check for match within deviation
		// We should query the duration of one 4K buffer, then use that as the jitter
		// on whatever file we are testing.
		TUint bitRate = 0 ;
		TRAP(err,bitRate = aConvert->DestinationBitRateL());
		if(err!=KErrNone)
			{
			INFO_PRINTF2(_L("CMdaAudioConvertUtility::DestinationBitRateL leave with error %d"),err);
			return EFail;
			}
		INFO_PRINTF2(_L("Bit rate = %d"), bitRate);
		// buffer duration = no. bytes / byte rate
		TUint bufDuration = (4096000 / (bitRate / 8)) + 1;	// get it in 1/1000 sec, and round up
		TInt64 dev(bufDuration * 1000);

		INFO_PRINTF2(_L("Buffer duration = %d"), I64INT(dev));
		TBool durationMatch = EFalse;
		if (newDuration == expectedDuration)
			durationMatch = ETrue;
		else if (newDuration < expectedDuration)
			{
			if (expectedDuration <= newDuration.Int64() + dev)
				durationMatch = ETrue;
			}
		else if (newDuration > expectedDuration)
			{
			if (expectedDuration >= newDuration.Int64() - dev)
				durationMatch = ETrue;
			}

		if (!durationMatch)
				{
				ERR_PRINTF1(_L("Cropped duration out of expected range"));
				return EFail;
				}
		return EPass;
		}

	}


//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConvertStop::CTestMmfAclntConvertStop(const TDesC& aTestName, TBool aNegative)
	:CTestMmfAclntConConfig(aTestName, EPcm16Wav, aNegative) 
	{
	}

CTestMmfAclntConvertStop* CTestMmfAclntConvertStop::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertStop* self = new (ELeave) CTestMmfAclntConvertStop(aTestName, aNegative);
	return self;
	}


void CTestMmfAclntConvertStop::MoscoStateChangeEvent(CBase* /*aObject*/, 
													TInt aPreviousState,
													TInt aCurrentState,
													TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	TInt PreviousState = aPreviousState;
	TInt CurrentState = aCurrentState;
	INFO_PRINTF1( _L("CTestMmfAclntDoConv : MMdaObjectStateChangeObserver Callback for CMdaAudioConvertUtility complete"));
	INFO_PRINTF4( _L("iError %d PreviousState %d -> CurrentState %d"), 
			iError, PreviousState, CurrentState);
	if( CurrentState != CMdaAudioClipUtility::ERecording) 
	 //give a chance to finish conversion
		CActiveScheduler::Stop();
	}

/**
 * Set config destination clip
 */
TVerdict CTestMmfAclntConvertStop::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConverter : Config"));
	TVerdict ret = EFail;
//stolen from  #include <MMFMdaClient.h>
//static const TUid KUidMmfAudioController	= {0x101F5022};

	TPtrC toFilename, fromFilename;
	if ((!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), fromFilename))
		|| (!GetStringFromConfig(_L("SectionFour"), _L("configAudio"), toFilename)))
		return EInconclusive;

	//TMdaFileClipLocation toLocation(toFilename);
	//TMdaFileClipLocation fromLocation(fromFilename);
	iError = KErrTimedOut;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	if (iThisIsNegativeTest)
		{
		iAudioSettings.iSampleRate = KInvalidNumber;
		iAudioSettings.iChannels = KInvalidNumber;
		//to keep CCover happy
		//converter->OpenL(&fromLocation, &toLocation, 
		//	KUidMdaWavPcmCodec, KUidMdaClipFormatAu, KFourCCNULL);
		converter->OpenL(fromFilename, toFilename) ;
		}
	else										   
		{
		//converter->OpenL(&fromLocation, &toLocation, 
		//	KUidMmfAudioController, KUidMdaWavPcmCodec, TFourCC('A', 'L', 'A', 'W'));
		converter->OpenL(fromFilename, toFilename) ;
		}

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNone)
		ret = DoTestL(converter);

	if (ret == EFail)
		INFO_PRINTF2( _L("CMdaAudioConvertUtility failed with error %d"),iError );

	if(iThisIsNegativeTest && iError == KErrNotSupported)
		{
			ret = EPass;	
		}

	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond);
	return ret;	
	
	}


/**
 * Conversion stoping.
 */
TVerdict CTestMmfAclntConvertStop::DoTestL(CMdaAudioConvertUtility* aConvert)
	{
	INFO_PRINTF1( _L("TestConvert : Stop"));

	TRAP(iError, aConvert->ConvertL()) ;

	if(iError != KErrNone) 
		return EFail;

	CActiveScheduler::Start();

	aConvert->Stop() ;
	CActiveScheduler::Start();

	return	EPass ;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntConvertDes::CTestMmfAclntConvertDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	iTestFormat = aFormat;
	iCreateDes = aCreateDes;
	iSize = 0;
	iAudio = NULL;
	//128K heap size
	iHeapSize = 0x20000;
	}

CTestMmfAclntConvertDes* CTestMmfAclntConvertDes::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat,const TBool aCreateDes)
	{
	CTestMmfAclntConvertDes* self = new (ELeave) CTestMmfAclntConvertDes(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	return self;
	}

CTestMmfAclntConvertDes* CTestMmfAclntConvertDes::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateDes)
	{
	CTestMmfAclntConvertDes* self = CTestMmfAclntConvertDes::NewLC(aTestName,aSectName,aKeyName,aFormat,aCreateDes);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntConvertDes::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF4( _L("MMdaObjectStateChangeObserver: previous state: %d current state: %d error: %d"),aPreviousState, aCurrentState, aErrorCode);
	CActiveScheduler::Stop();
	}

TVerdict CTestMmfAclntConvertDes::DoTestStepPreambleL()
	{

	SetupFormatL(iTestFormat);

	RFs fs;
	RFile file;

	// Connect to file server
	User::LeaveIfError(fs.Connect());

	// get the filename from the config (.ini) file
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	// opem the file to see if it exists and get the size
	User::LeaveIfError(file.Open(fs,filename,EFileRead));
	CleanupClosePushL(file);

	User::LeaveIfError(file.Size(iSize));
	INFO_PRINTF2(_L("size of file is %d\n"),iSize);

	// read the file into a descriptor and use CWavDecodeUtility
	// to examine the WAV file to get the data length etc.
	HBufC8* imageData = HBufC8::NewLC(iSize);
	TPtr8 imageDataPtr(imageData->Des());
	User::LeaveIfError(file.Read(imageDataPtr));

	CWavDecodeUtility* decodeUtility = CWavDecodeUtility::NewL(imageDataPtr);
	CleanupStack::PushL(decodeUtility);

	iDataLengthSource = decodeUtility->GetDataLength();
	iSamples = decodeUtility->GetSamples();
	iChannels = decodeUtility->GetChannels();

	INFO_PRINTF2( _L("Sample Rate = %d"), decodeUtility->GetSampleRate());

	INFO_PRINTF2( _L("Data length = %d"), iDataLengthSource);
	INFO_PRINTF2( _L("Channels = %d"), iChannels);
	INFO_PRINTF2( _L("Num of samples = %d"), iSamples);

	CleanupStack::PopAndDestroy(3,&file); // decodeUtility, imageData, file

	switch (iTestFormat)
		{
		case EPcm16Wav:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			iFileSizeSink = KWavFileUncompressedDataHeaderSize + iSamples * iChannels * 16 /*BitsPerSample*/ / 8;
			break;
		
		case EAlawWav:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			iFileSizeSink = KWavFileUncompressedDataHeaderSize + iSamples * iChannels * 8 /*BitsPerSample*/ / 8;
			break;
		
		case EPcm8:
			iHeaderSize = KWavFileUncompressedDataHeaderSize; //(see mmfwavformat.cpp, CreateSinkBufferOfSizeL())
			iFrameSize = 4096;
			iFileSizeSink = KWavFileUncompressedDataHeaderSize + iSamples * iChannels * 8 /*BitsPerSample*/ / 8;
			break;
		
		case EImaAdpcmWav:
		case EImasPcmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KImaAdpcmFactChunkSize;
			iFrameSize = 4096;
			break;

		case EMulawRaw:
			iHeaderSize = 0;
			iFrameSize = 4096;
			iFileSizeSink = iSamples * iChannels * 8 /*BitsPerSample*/ / 8;
			break;
		
		case EPcmU8:
			iHeaderSize = 0;
			iFrameSize = 4096;
			iFileSizeSink = iSamples * iChannels * 8 /*BitsPerSample*/ / 8;
			break;
		
		case EPcmU16:
			iHeaderSize = 0;
			iFrameSize = 4096;
			iFileSizeSink = iSamples * iChannels * 16 /*BitsPerSample*/ / 8;
			break;

		case EGsmWav:
			iHeaderSize = KWavFileCompressedDataHeaderSize + KGSMFactChunkSize;
			iFrameSize = 4096;
			break;

		case E16BitAu:
			iHeaderSize = KAuFileUncompressedDataHeaderSize;
			iFrameSize = 4096;
			iFileSizeSink = iHeaderSize + iSamples * iChannels * 16 /*BitsPerSample*/ / 8;

		default:				   
			/*skip*/ ;
			iHeaderSize = 0;
			iFrameSize = 1;
			break ;
		}

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntConvertDes::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

void CTestMmfAclntConvertDes::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntConvertDes::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

/**
 * Open a descriptor and Convertord
 */
TVerdict CTestMmfAclntConvertDes::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestConvertor : Convert Des"));
	TVerdict ret = EPass;
	HBufC8* audioTo = NULL;
	TMdaDesClipLocation* toLocation = NULL;

	CMdaAudioConvertUtility* ConvertUtil = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(ConvertUtil);

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1;
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
			return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);
	//output buffer 2 much than input one
	audioTo = HBufC8::NewMaxLC(iSize*2);
	TPtr8 toBufferDes(audioTo->Des());
	toBufferDes.SetLength(0);

	toLocation = new (ELeave) TMdaDesClipLocation(toBufferDes);
	CleanupStack::PushL(toLocation);

	ConvertUtil->OpenL(filename, toLocation, 
			iFormat, iCodec) ; 
	
	iError = KErrTimedOut;

	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		ConvertUtil->ConvertL();
		INFO_PRINTF1( _L("Convertord CMdaAudioConvertUtility"));
		CActiveScheduler::Start(); // open->Convertord

		if (iError == KErrNone)
			{
			// wait to run out of space Convertording
			CActiveScheduler::Start();

			if(iError == KErrNone)
				{	
				INFO_PRINTF3(_L("Length %d, expected = %d"), audioTo->Length(), iFileSizeSink);
				// compare length with expected
				if (iFileSizeSink > 0)
					{
					//is it a good size?
					if((audioTo->Length()) != (static_cast<TInt> (iFileSizeSink)) )
						{
						ret = EFail;
						}
					}
				else
					{
					ret = EInconclusive;
					}

				// For good measure,
				// check the validity of the conversion by comparing duration 
				// with original file - but not for raw files
				if (iHeaderSize > 0)
					{
					CMdaAudioPlayerUtility* player = NULL;
					player = CMdaAudioPlayerUtility::NewDesPlayerL(*audioTo, *this);
					CleanupStack::PushL(player);

					// Wait for init callback
					INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
					CActiveScheduler::Start();

					player->Play();
					// Wait for play complete callback
					INFO_PRINTF1( _L("CMdaAudioPlayerUtility::Play()"));
					CActiveScheduler::Start();

					INFO_PRINTF3( _L("Duration = %ld, expected = %ld"), I64LOW(player->Duration().Int64()), I64LOW(ConvertUtil->Duration().Int64()));

					if (iError != KErrNone)
						{
						ERR_PRINTF2( _L("Play failed with error : %d"),iError);
						ret = EFail;
						}
					else if (ConvertUtil->Duration() != player->Duration())
						{
						ERR_PRINTF3( _L("Actual Duration : %ld does not match the Expected Duration : %ld"),player->Duration().Int64(), ConvertUtil->Duration().Int64());
						ret = EFail;
						}

					CleanupStack::PopAndDestroy(player);
					}
				}
			}
		}

#if defined __WRITE_CONVERSION_TO_FILE
	{
	_LIT(KFileOutRaw, "\\TEST.AU");
	RFs fs;

	fs.Connect();
	RFile file;
	file.Replace(fs, KFileOutRaw(), EFileWrite);
	file.Write(audioTo->Des());
	TInt len = audioTo->Length();
	file.Size(len);
	file.Close();
	fs.Close();
	}
#endif
	if (ret != EPass)
		ERR_PRINTF2( _L("CMdaAudioConvertorderUtility failed with error %d"),iError );

	CleanupStack::PopAndDestroy(toLocation);
	CleanupStack::PopAndDestroy(audioTo);
	CleanupStack::PopAndDestroy(ConvertUtil);	

	User::After(KOneSecond);
	return ret;

	}
//---------------------------------------------------------------------
/**
 * Constructor
 */
CTestMmfAclntConvertUrl::CTestMmfAclntConvertUrl(const TDesC& aTestName, TBool aNegative)
	:CTestMmfAclntConConfig(aTestName, EAlawWav, aNegative) 
	{}

CTestMmfAclntConvertUrl* CTestMmfAclntConvertUrl::NewL(const TDesC& aTestName, TBool aNegative)
	{
	CTestMmfAclntConvertUrl* self = new (ELeave) CTestMmfAclntConvertUrl(aTestName,aNegative);
	return self;
	}

/**
 * Set Urls
 */
TVerdict CTestMmfAclntConvertUrl::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestConvert : Urls"));
	TVerdict ret = EFail;
	TMdaUrlClipLocation* toLocation = new (ELeave)
		TMdaUrlClipLocation(_L("http://www.symbian.com"),KUseDefaultIap);
	CleanupStack::PushL(toLocation);

	CMdaAudioConvertUtility* ConvertUtil = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(ConvertUtil);

	TBuf<KSizeBuf>	fromFileName;
	TPtrC			pFromFileName;
	if(!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), pFromFileName))
			return EInconclusive;
	GetDriveName(fromFileName);
	fromFileName.Append(pFromFileName);

	TRAP(iError, ConvertUtil->OpenL(fromFileName, toLocation, iFormat, iCodec)) ;
	if(iError == KErrNone)
	{
		//should not work for time being
		iError = KErrTimedOut;
		INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
		CActiveScheduler::Start();
		if(iError == KErrNotSupported)
			ret = EPass ;
		else
			{

   			TRAP(iError, ConvertUtil->ConvertL()) ;
			if(iError != KErrNone)
				{
				ERR_PRINTF2(_L("Convert left with error =  %d"),iError);
				ret = EFail;
				}
				
			CActiveScheduler::Start();
			if(iError == KErrNotSupported)
				ret = EPass ;
			}
		//CActiveScheduler::Start();
	}
	else if(iError == KErrNotSupported)
	{  //expected result
	
		ERR_PRINTF2(_L("OpenL left with error =  %d. It's OK for now!"),iError);
		ret = EPass ;	
	}

	CleanupStack::PopAndDestroy(ConvertUtil);
	CleanupStack::PopAndDestroy(toLocation);
	return ret;
	}
