
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
// This program is designed the test of the MMF_ACLNT.
// Test DEF039267
// 
//

/**
 @file TestOldCodec.cpp
*/


#include "TestOldCodec.h"


/**
 * Constructor
 */

CTestMmfAclntOldCodecFile::CTestMmfAclntOldCodecFile(const TDesC& aTestName,const TDesC& aSectName,
									 const TDesC& aKeyName,const TBool aPlay, 
									 const TInt aExpectedError)
	:iPlay (aPlay)
	,iExpectedError (aExpectedError)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntOldCodecFile* CTestMmfAclntOldCodecFile::NewL(const TDesC& aTestName, const TDesC& aSectName,
										   const TDesC& aKeyName,const TBool aPlay, 
										   const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntOldCodecFile* self = new (ELeave) CTestMmfAclntOldCodecFile(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

CTestMmfAclntOldCodecFile* CTestMmfAclntOldCodecFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,
											const TDesC& aKeyName,
											const TBool aPlay,  
											const TInt aExpectedError)
	{
	CTestMmfAclntOldCodecFile* self = CTestMmfAclntOldCodecFile::NewL(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntOldCodecFile::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	if (iError != KErrNone)
		{
		INFO_PRINTF1( _L("CTestMmfAclntRecFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
		INFO_PRINTF2( _L("Previous State %d "), aPreviousState);
		INFO_PRINTF2( _L("State %d "), aCurrentState);
		INFO_PRINTF2( _L("iError %d "), iError);
		}
	if (aCurrentState != CMdaAudioClipUtility::EPlaying)
		{//don't want to stop AS from transition EOpen->EPlaying
		CActiveScheduler::Stop();
		}
	}


/** Play and record an audio file using the UseOldCodecAudioController
 *  which uses CMMFCodecs and the 'null' pcm16->pcm16 hw device plugin
 *  This is for DEF039267
 */
TVerdict CTestMmfAclntOldCodecFile::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName,iKeyName,filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);
	TUid useCMMFCodecAudioController;

	useCMMFCodecAudioController.iUid = KMmfUidUseOldCodecAudioController;

	CMdaAudioRecorderUtility* recUtil = CMdaAudioRecorderUtility::NewL(*this);

	TRAPD(err, recUtil->OpenFileL(filename,useCMMFCodecAudioController));

	if (err)
		{
		INFO_PRINTF2( _L("error opening record to file %d "), err);
		}
	else
		{
		CActiveScheduler::Start();
		TTimeIntervalMicroSeconds duration = recUtil->Duration();
		// Wait for initialisation callback
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));

		// Check for errors.
		if (iError == KErrNone && recUtil != NULL)
			{
			if(iPlay)
				{
				iError = KErrTimedOut;
				TRAPD(err, recUtil->PlayL());
				if (err)
					{
					INFO_PRINTF2( _L("error opening record to file %d "), err);
					}
				else
					{
					//lets time how long the file plays for and check 
					//it has played for the full time
					TTime startTime(0);
					TTime endTime(0);
					startTime.HomeTime();
					// Wait for init callback
					CActiveScheduler::Start();
					endTime.HomeTime();
					//check file played for the expected time - allow margin of error + 1S -0.5S
					TTimeIntervalMicroSeconds playduration = endTime.MicroSecondsFrom(startTime);
					if ( (playduration.Int64() < (duration.Int64()-KOneSecond/2)) ||
						(playduration.Int64() > (duration.Int64()+KOneSecond)) )
						{
						INFO_PRINTF2(_L("file didn't play for the expected duration expected %ld"), duration.Int64());
						INFO_PRINTF2(_L("but actual duration was %ld"), playduration.Int64());
						}
					else if(iError == KErrNone)
						ret = EPass;
					}
				}		
			}

		recUtil->Close();
		}

	//now try recording file

	TFourCC recordedDataType;
	TUid format;
	format.iUid = KMmfUidFormatWAVWrite;

	//check that the datatype is a valid datatype to record
	if (!iKeyName.Compare(_L("ALAW ")))
		recordedDataType.Set(KMMFFourCCCodeALAW);
	else if (!iKeyName.Compare(_L("PCMU8")))
		recordedDataType.Set(KMMFFourCCCodePCMU8);
	else if (!iKeyName.Compare(_L("MULAW")))
		recordedDataType.Set(KMMFFourCCCodeMuLAW);
	else if (!iKeyName.Compare(_L("IMAD")))
		recordedDataType.Set(KMMFFourCCCodeIMAD);
	else if (!iKeyName.Compare(_L("GSM610")))
		recordedDataType.Set(KMMFFourCCCodeGSM610);

	//rename file to indicate it is recorded
	if (recordedDataType != KMMFFourCCCodeNULL)
		{
		TInt matchpos = filename.Find(_L(".wav"));//get position of extension
		filename.Insert(matchpos,_L("recorded"));//change filename to indicate it is recorded

		//delete file prior to recording
		RFs fs;
		fs.Connect();
		fs.Delete(filename);
		fs.Close();
	
		TRAPD(err,recUtil->OpenFileL(filename,useCMMFCodecAudioController,useCMMFCodecAudioController,format,recordedDataType));

		if (err)
			{
			ret = EFail;
			INFO_PRINTF2( _L("error opening record to file %d "), err);
			}
		else
			{
			INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
			CActiveScheduler::Start();	// idle -> open

			recUtil->RecordL();

			// Commented this line of code as a fix for DEF 40709 as it looks like sending a trace message
			// to the TestFramework server(for some reason)and taking a very long time on beech/lubbock 

			//INFO_PRINTF1( _L("Record CMdaAudioRecorderUtility"));
			CActiveScheduler::Start(); // open -> record
			User::After(KTwoSeconds);
			
			recUtil->Stop();
	
			TTimeIntervalMicroSeconds expectedDuration(KTwoSeconds);
			TTimeIntervalMicroSeconds duration = recUtil->Duration();
			//we'll alow +/- 0.5 variance 
			if (duration > TTimeIntervalMicroSeconds(expectedDuration.Int64() + KDeviation/2))
				{
				INFO_PRINTF3( _L("Duration too long expected %ld actual %ld "), expectedDuration.Int64(), duration.Int64());
				ret = EFail;
				}
			else if (duration < TTimeIntervalMicroSeconds(expectedDuration.Int64() - KDeviation/2))
				{
				INFO_PRINTF3( _L("Duration too short expected %ld actual %ld "), expectedDuration.Int64(), duration.Int64());
				ret = EFail;
				}

			recUtil->Close();
			}
		}

	delete recUtil;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
	return	ret;
	}





