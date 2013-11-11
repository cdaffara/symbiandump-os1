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
 @file TestPlayerUtils.cpp
*/


#include "TestPlayerUtils.h"
#include <caf/caf.h>

// constant table of parameters for tests
const TRepeatParameters KTestParameters[] =
{	
	{ _S("playerAudioFile"), 1, 1},
	{ _S("Pcm16Mono12khz"), 3, 3},
	{ _S("PCM8"), 3, 3},
	{ _S("PCM8-6144bytes"), 1, 1},
	{ _S("ALAW"), 3, 3},
	{ _S("PCM16"), 1, 1},
	{ _S("PCMU8"), 2, 2},
	{ _S("PCMU16BE"), 3, 3},
	{ _S("PCMU16"), 1, 1},
	{ _S("MULAW"), 2, 2},
	{ _S("Sequence1"), 2, 2},
	{ _S("Sequence2"), 3, 3},
	{ _S("IMAD"), 3, 3},
	{ _S("GSM610"), 1, 1},
	{ _S("Pcm8Mono44khz"), 2, 2},
	{ _S("Pcm16Mono44khz"), 3, 3},
	{ _S("Pcm16Mono48khz"), 1, 1},
	{ _S("100khzSRate1KhzTone"), 2, 2},
	{ _S("Pcm16Mono24khz"), 3, 3},
	{ _S("playerAudioFile2"), 2, 2}
};

const TInt KNoOfTestFiles = 20;
TAny	GetDriveName(TDes& aFileName)
{
#ifdef __WINS__
	aFileName = _L("c:");	
#elif defined(__MARM__) || defined(__X86GCC__)
	#ifdef __TEST_FILES_ON_C_DRIVE__
		aFileName = _L("c:");
	#else
		aFileName = _L("z:");	
	#endif // __TEST_FILES_ON_C_DRIVE__
#endif	
}


#define STOP_TEST_GEN_ERROR ERR_PRINTF1(_L("State machine error")), StopTest(KErrGeneral, EFail)
/**
 * Constructor
 */
CTestMmfAclntFile::CTestMmfAclntFile(const TDesC& aTestName,const TDesC& aSectName,
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

CTestMmfAclntFile* CTestMmfAclntFile::NewL(const TDesC& aTestName, const TDesC& aSectName,
										   const TDesC& aKeyName,const TBool aPlay, 
										   const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntFile* self = new (ELeave) CTestMmfAclntFile(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

CTestMmfAclntFile* CTestMmfAclntFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,
											const TDesC& aKeyName,
											const TBool aPlay,  
											const TInt aExpectedError)
	{
	CTestMmfAclntFile* self = CTestMmfAclntFile::NewL(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntFile::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntFile::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1(_L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}



/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntFile::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName,iKeyName,filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioPlayerUtility* player = NULL;
	INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &filename);
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);

	if (iTestStepName.Compare(_L("MM-MMF-ACLNT-I-2001-HP"))==0)
		{
		// Check file size is more than 0
		RFs fs;
		TInt err = fs.Connect();
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error connecting to file session (%d)"), err);
			return EInconclusive;
			}
		CleanupClosePushL(fs);
		TEntry fileEntry;
		err = fs.Entry(filename, fileEntry);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error getting file entry (%d)"), err);
			fs.Close();
			return EInconclusive;
			}
		if (fileEntry.iSize <= 0)
			{
			INFO_PRINTF2(_L("Test file '%S' has size %d"), fileEntry.iSize);
			fs.Close();
			return EInconclusive;
			
			}
		CleanupStack::PopAndDestroy();	// fs
		}
		
#ifndef __WINS__
	if (iTestStepName.Compare(_L("MM-MMF-ACLNT-I-2014-LP")) == 0)
		{
		// this test should not be run on Lubbock
		INFO_PRINTF1(_L("Lubbock hardware is not fast enough to convert 100KHz audio."));
		INFO_PRINTF1(_L("This test has been skipped for this reason..."));
		
		delete player;
		User::After(KOneSecond); // wait for deletion to shut down devsound
		
		return EPass;
		}
#endif

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone && (iExpectedError == iError))
		{
		if (iTestStepName.Compare(_L("MM-MMF-ACLNT-I-1008-HP")) == 0)
			{
			// Test is for non-existent file - check that the file has not been created
			RFs fs;
			TInt err = fs.Connect();
			if (err != KErrNone)
				{
				ret = EInconclusive;
				}
			else
				{
				TEntry entry;
				if (fs.Entry(filename, entry) == KErrNone)
					{
					ret = EFail;	// file exists - test fails
					}
					
				else
					ret = EPass;
				fs.Close();
				}
			}
		else
			{
			ret = EPass;	// all other tests pass
			}
		delete player;
		User::After(KOneSecond); // wait for deletion to shut down devsound
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with expectable error %d"),iError );
		return ret;
		}
	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			player->Play();
			
			// Wait for init callback
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			CActiveScheduler::Start();

			//can't use iExpected Error as is doesn't distinguish between 
			//error from NewFilePlayerL or PlayL.
			if ((iTestStepName.Compare(_L("MM-MMF-ACLNT-I-2015-LP")) == 0))
				{
				if(iError == KErrNotSupported)
					{
					ret = EPass;
					}
				}
			else if(iError == KErrNone)
				ret = EPass;
			}
		else
			ret = EPass;			
		}
	
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError != KErrNone)
		{
		if ((iTestStepName.Compare(_L("MM-MMF-ACLNT-I-2014-LP")) == 0) && iError==KErrNotSupported)
			{
			//Sampling Rate greater than 96000 Hz is not expected in data , and we dont support this.
			//This test has been changed to expect KErrNotSupported due to fix in PDEF131534 
			INFO_PRINTF1( _L("CMdaAudiPlayerUtility returned KErrNotSupported as expected"));
			ret= EPass;
			}
		else
			{
			ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
			}
		}
		
	return	ret;
	}







/**
 * Constructor
 */
CTestMmfAclntSEQNFile::CTestMmfAclntSEQNFile(const TDesC& aTestName,const TDesC& aSectName,
									 const TDesC& aKeyName,
									 const TBool aPlay, 
									 const TBool aIsFile)
	:iPlay (aPlay)
	,iIsFile (aIsFile)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntSEQNFile* CTestMmfAclntSEQNFile::NewL(const TDesC& aTestName, const TDesC& aSectName,
										   const TDesC& aKeyName,
										   const TBool aPlay, 
										   const TBool aIsFile)
	{
	CTestMmfAclntSEQNFile* self = new (ELeave) CTestMmfAclntSEQNFile(aTestName,aSectName,aKeyName,aPlay,aIsFile);
	return self;
	}

CTestMmfAclntSEQNFile* CTestMmfAclntSEQNFile::NewLC(const TDesC& aTestName, const TDesC& aSectName,
											const TDesC& aKeyName,
											const TBool aPlay,  
											const TBool aIsFile)
	{
	CTestMmfAclntSEQNFile* self = CTestMmfAclntSEQNFile::NewL(aTestName,aSectName,aKeyName,aPlay,aIsFile);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntSEQNFile::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntSEQNFile::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}
 

//This method is used to stop playing the file 1 second after we started
void CTestMmfAclntSEQNFile::DelayedMethodCallL()
{
CActiveScheduler::Stop();
}



/** Load and play an audio sequence file.
 */
TVerdict CTestMmfAclntSEQNFile::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName,iKeyName,filename1))
		{
		return EInconclusive;
		}
	
	GetDriveName(filename);
	filename.Append(filename1);


	CMdaAudioPlayerUtility* player = NULL;
	HBufC8* seqnDesc = NULL;
	RFile file;
	RFs fs;

	if(iIsFile)
		{
		player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);
		}
	else
		{

		User::LeaveIfError(fs.Connect());
//TIMB
		User::LeaveIfError(file.Open(fs, filename, EFileShareReadersOnly | EFileStream));
		CleanupClosePushL(file);

		TInt size;
		User::LeaveIfError(file.Size(size));

		seqnDesc = HBufC8::NewMaxLC(size);

		TPtr8 bufferDes(seqnDesc->Des());
		User::LeaveIfError(file.Read(bufferDes));
		
		player = CMdaAudioPlayerUtility::NewDesPlayerL(bufferDes, *this);
		}

	CleanupStack::PushL(player);

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();


	if(iError == KErrNone)
		{
		TInt maxVol = player->MaxVolume();
		player->SetVolume(maxVol/2);
		TInt gotVol;
		iError = player->GetVolume(gotVol);

		if((iError != KErrNone) || (maxVol/2 != gotVol))
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed volume tests"));
			if(iError == KErrNone)
				iError = KErrGeneral;
			}
		}


	if(iError == KErrNone)
		{
		TTimeIntervalMicroSeconds t(0);
		TTimeIntervalMicroSeconds zeroUs(0);

		t = player->Duration();

		if(t != zeroUs)
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed duration test"));
			iError = KErrGeneral;
			}

		player->SetPosition(zeroUs);
		iError = player->GetPosition(t);

		if((t != zeroUs) || (iError != KErrNone))
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed duration test"));
			if(iError == KErrNone)
				iError = KErrGeneral;
			}
		}

	if(iError == KErrNone)
		{
		TInt entries;
		TInt metaErr = KErrNone;

		metaErr = player->GetNumberOfMetaDataEntries(entries);

		if(metaErr != KErrNotSupported)
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed volume tests"));
			iError = metaErr;
			}
		else
			{
			CMMFMetaDataEntry* metaEntry = NULL;	// EABI warning removal - "metaEntry was set but never used"

			TRAP(metaErr, metaEntry = player->GetMetaDataEntryL(0));	// EABI warning removal - "metaEntry was set but never used"


			if(metaErr != KErrNotSupported)
				{
				ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed volume tests"));
				iError = metaErr;
				if (metaErr==KErrNone)
					{
					delete metaEntry;
					}
				}
			}
		}
	


	if(iError == KErrNone)
		{
		player->SetBalance(KMMFBalanceMaxLeft);
		TInt bal = KMMFBalanceCenter;

		iError = player->GetBalance(bal);

		if(bal != KMMFBalanceMaxLeft)
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed balance tests"));
			if(iError == KErrNone)
				iError = KErrGeneral;
			}
		else
			player->SetBalance(KMMFBalanceCenter);
		}


	if(iError == KErrNone)
		{
		TInt priErr = player->SetPriority(EMdaPriorityMin, EMdaPriorityPreferenceNone);

		if(priErr != KErrNone)
			{
			ERR_PRINTF1( _L("CMdaAudioPlayerUtility failed priority tests"));
			iError = KErrGeneral;
			}
		else
			player->SetPriority(EMdaPriorityNormal, EMdaPriorityPreferenceNone);
		}




	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		player->Play();

		//Stop the player after one second and restart
		CDelayedMethodCall* delayedMethodCall = CDelayedMethodCall::NewL(this);
		CleanupStack::PushL(delayedMethodCall);
		delayedMethodCall->After(KOneSecond);

		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(delayedMethodCall);

		player->Stop();

		player->Play();
		CActiveScheduler::Start();
		}

	

	CleanupStack::PopAndDestroy(player);
	if(iIsFile == EFalse)
		{
		CleanupStack::PopAndDestroy(seqnDesc);
		CleanupStack::PopAndDestroy(&file);
		}

	User::After(KOneSecond); // wait for deletion to shut down devsound
	if(iError == KErrNone)
	{
		ret = EPass;
	}
		
	else
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );

	return	ret;
	}





//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDesc::CTestMmfAclntDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError)
	:iPlay (aPlay),
	iExpectedError(aExpectedError)

	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntDesc* CTestMmfAclntDesc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError)
	{
	CTestMmfAclntDesc* self = new (ELeave) CTestMmfAclntDesc(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

CTestMmfAclntDesc* CTestMmfAclntDesc::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntDesc* self = CTestMmfAclntDesc::NewLC(aTestName,aSectName,aKeyName,aPlay, aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntDesc::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntDesc::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

TVerdict  CTestMmfAclntDesc::DoTestStepPreambleL()
	{
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	RFs fs;
	RFile file;
	TInt size = 0;
    TInt returnCode = 0;
	// connect to file system and open file
	
	CleanupClosePushL(file);
	
    returnCode = fs.Connect();
    if (returnCode != KErrNone)
        {
        ERR_PRINTF2(_L("Failed to connect to file server, error %d"), returnCode);
        fs.Close();
        return EFail;
        }
        
    returnCode = file.Open(fs,filename,EFileRead);
    if (returnCode != KErrNone)
        {
        ERR_PRINTF2(_L("Failed to Open the file, error %d"), returnCode);
        file.Close();
        return EFail;
        }
    
	INFO_PRINTF2( _L("File opened successfully %S"), &filename);
	    
    // Set HBuf size
	User::LeaveIfError(file.Size(size));
	INFO_PRINTF2(_L("size of file = %d\n"),size); //Statement Changed under DEF105143
	iAudio = HBufC8::NewMaxL(size);
	// read data into Hbuf
	TPtr8 bufferDes(iAudio->Des());
	User::LeaveIfError(file.Read(bufferDes));
	
	CleanupStack::PopAndDestroy(); //file
	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntDesc::DoTestStepPostambleL()
	{
	delete iAudio;
	delete iPlayer;
    iPlayer = NULL;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Load and initialise an audio descriptor.
 */
TVerdict CTestMmfAclntDesc::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Desc"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TPtr8 bufferDes(iAudio->Des());
    iPlayer = CMdaAudioPlayerUtility::NewDesPlayerL(bufferDes, *this);
	// Wait for init callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone && iError == iExpectedError)
		{
		delete iPlayer;
		iPlayer = NULL;
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with expected error %d"),iError );
		return EPass;	
		}
	
	if (iError == KErrNone && iPlayer != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			iPlayer->Play();
			// Wait for init callback
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			CActiveScheduler::Start();
			if(iError == KErrNone)
				{
				ret = EPass;
				}
				
			}
		else
			ret = EPass;			
		}
	
	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
	return	ret;
	}

//------------------------------------------------------------------

/**
 * Load and initialise an audio descriptor - read-only
 */

CTestMmfAclntDescReadOnly::CTestMmfAclntDescReadOnly(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError)
	:CTestMmfAclntDesc(aTestName, aSectName, aKeyName, aPlay, aExpectedError)
	{
	}

CTestMmfAclntDescReadOnly* CTestMmfAclntDescReadOnly::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError)
	{
	CTestMmfAclntDescReadOnly* self = new (ELeave) CTestMmfAclntDescReadOnly(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

CTestMmfAclntDescReadOnly* CTestMmfAclntDescReadOnly::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntDescReadOnly* self = CTestMmfAclntDescReadOnly::NewLC(aTestName,aSectName,aKeyName,aPlay, aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

TVerdict CTestMmfAclntDescReadOnly::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Desc (Read-Only)"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	TPtr8 bufferDes(iAudio->Des());
	iPlayer = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(bufferDes, *this);
	// Wait for init callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if(iError != KErrNone && iError == iExpectedError)
		{
		delete iPlayer;
		iPlayer = NULL;
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with expected error %d"),iError );
		return EPass;	
		}
	
	if (iError == KErrNone && iPlayer != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			iPlayer->Play();
			// Wait for init callback
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			CActiveScheduler::Start();
			if(iError == KErrNone)
				{
				ret = EPass;
				}
			}
		else
			ret = EPass;			
		}
	
	delete iPlayer;
	iPlayer = NULL;
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
		}
		
	return	ret;
	}

//------------------------------------------------------------------

/**
 * Load and initialise an audio descriptor, using Open only
 */

CTestMmfAclntDescOpen::CTestMmfAclntDescOpen(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError)
	:CTestMmfAclntDesc(aTestName, aSectName, aKeyName, aPlay, aExpectedError)
	{
	}

CTestMmfAclntDescOpen* CTestMmfAclntDescOpen::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError)
	{
	CTestMmfAclntDescOpen* self = new (ELeave) CTestMmfAclntDescOpen(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

CTestMmfAclntDescOpen* CTestMmfAclntDescOpen::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntDescOpen* self = CTestMmfAclntDescOpen::NewLC(aTestName,aSectName,aKeyName,aPlay, aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

TVerdict CTestMmfAclntDescOpen::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Desc (Open)"));
	TVerdict ret = EFail;

	TPtr8 bufferDes(iAudio->Des());
	TRAPD(err, iPlayer = CMdaAudioPlayerUtility::NewL(*this));
	if(err != KErrNone)
		{
		delete iPlayer;
		iPlayer = NULL;
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with expected error %d"),iError );
		return EFail;	
		}
	
	if (err == KErrNone && iPlayer != NULL)
		{
			iError = KErrTimedOut;
			TRAP_IGNORE(iPlayer->OpenDesL(bufferDes));
			// Wait for init callback
			INFO_PRINTF1( _L("OpenDesL CMdaAudioPlayerUtility"));
			CActiveScheduler::Start();
			if(iError == KErrNone)
				{
				ret = EPass;
				}
		}
	
	delete iPlayer;
	iPlayer = NULL;
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
		}
		
	return	ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntUrl::CTestMmfAclntUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:iPlay (aPlay)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

CTestMmfAclntUrl* CTestMmfAclntUrl::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfAclntUrl* self = new (ELeave) CTestMmfAclntUrl(aTestName,aSectName,aKeyName,aPlay);
	return self;
	}

CTestMmfAclntUrl* CTestMmfAclntUrl::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfAclntUrl* self = CTestMmfAclntUrl::NewLC(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntUrl::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntUrl::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

/**
 * Open audio from a URL and play.
 */
TVerdict CTestMmfAclntUrl::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestPlayerUtils : URL"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;	

	TPtrC urlname;
	if(!GetStringFromConfig(iSectName,iKeyName,urlname))
		{
		return EInconclusive;
		}
		

 	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewL(*this);
	CleanupStack::PushL(player);

	TRAPD(err, player->OpenUrlL(urlname));
	if(err)
		{
		if(err != KErrNotSupported)
			{
			ERR_PRINTF2( _L("OpenUrlL left with error %d"), err);
			CleanupStack::PopAndDestroy(player);
			return EFail;
			}
		else
			{
			INFO_PRINTF2( _L("OpenUrlL left with expected error %d"), err);
			CleanupStack::PopAndDestroy(player);
			return EPass;
			}
		}
	
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if (iError == KErrNotSupported)
		{
		ret = EPass;
		}
		

	CleanupStack::PopAndDestroy(player);
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
		}
		
	return	ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntAudioPlayerUtil::CTestMmfAclntAudioPlayerUtil(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName=aTestName;
	iStop = ETrue;
	iSectName = aSectName;
	}


void CTestMmfAclntAudioPlayerUtil::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	
	}

void CTestMmfAclntAudioPlayerUtil::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	if(iStop)
		{
		CActiveScheduler::Stop();	
		}
	}



/**
 * Pause play and continue play.
 */
TVerdict CTestMmfAclntAudioPlayerUtil::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Player Util"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename1))
		{
		return EInconclusive;
		}
	
	GetDriveName(filename);
	filename.Append(filename1);
	INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &filename);
	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewFilePlayerL(filename,*this);
	CleanupStack::PushL(player);
	// wait for initilisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	// Attempt to wait out the in use error message!!!
	// It may just be that we have the file locked.
	if(iError == KErrInUse)
		{
		if(!GetStringFromConfig(iSectName, _L("playerAudioFile2"), filename1))
		{
			return EInconclusive;	
		}
			
		GetDriveName(filename);
		filename.Append(filename1);
		User::After(KTwoSeconds);
		CleanupStack::PopAndDestroy(player);
		User::After(KTwoSeconds);
		INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &filename);
		player = CMdaAudioPlayerUtility::NewFilePlayerL(filename,*this);
		CleanupStack::PushL(player);
		// wait for initilisation callback
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
		CActiveScheduler::Start();
		}

	if (iError == KErrNone)
		{
		// no errors so far, Do Test!
		ret = DoTestL(player);
		}

	CleanupStack::PopAndDestroy(player);
	User::After(KOneSecond);
	if (iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );	
		}
	
	return	ret;
	}
//=======================================================================================
/**
 * Constructor
 */
CAsyncTestMmfAclntAudioPlayerUtil::CAsyncTestMmfAclntAudioPlayerUtil ( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName=aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}


void CAsyncTestMmfAclntAudioPlayerUtil::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iClipDuration = aDuration;
	if (aError != KErrNone)
		{
		ERR_PRINTF2(_L("*MapcInitComplete* Error: %d."), aError);
		StopTest(aError, EFail);
		}
	else
		{
		INFO_PRINTF2(_L("*MapcInitComplete* Error: %d."), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateOpened;
		iPlayerState = EOpened;	
		}	
	}

void CAsyncTestMmfAclntAudioPlayerUtil::MapcPlayComplete(TInt aError)
	{
	if(aError == KErrNone)
		{
		INFO_PRINTF2(_L("*MapcPlayComplete* Error: %d."), iError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateStopTest;
		}
	else
		{
		ERR_PRINTF2(_L("*MapcPlayComplete* Error: %d."), aError);
		StopTest(aError, EFail);
		}
	}
	
/**
 * 
 */
void CAsyncTestMmfAclntAudioPlayerUtil::KickoffTestL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Player Util"));
	TPtrC			filename1; 
	TInt			repeatCount;
	TInt			silence;
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		INFO_PRINTF1(_L("GetStringFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
		
	if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("numOfRepeat"), repeatCount))
		{
		INFO_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	if(!GetIntFromConfig(_L("SectionSilence"), _L("silence"), silence))
		{
		INFO_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}	
	iNumOfRepeatsSet = 	repeatCount;
	iSilence = silence;
	
	GetDriveName(iFileName);
	iFileName.Append(filename1);
	INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &iFileName);
	iTrigger = CFsmTrigger::NewL(*this);
	iPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(iFileName,*this);
	iPlayerState = EOpening;
	}

/**
 * Time comparison utility function
 *
 * @param	"const TUint aActual"
 *			The actual timer value produced
 * @param	"const TUint aExpected"
 *			Expected timer value
 * @param	"const TUint aDeviation"
 *			Allowed deviation of the expected value
 *			from the actual value.
 * @return	"TBool"
 *			Did actual timed value fall within deviation limits
 */ 
TBool CAsyncTestMmfAclntAudioPlayerUtil::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		return ETrue;	

	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		difference = aActual - aExpected;
	else
		difference = aExpected - aActual;

	// comapare
	if(difference < aDeviation)
		return ETrue;
	return EFalse;
	}
	
void CAsyncTestMmfAclntAudioPlayerUtil::CloseTest()
	{
	if (iTrigger)
		{
		iTrigger->Cancel();
		delete iTrigger;
		}
	
	if (iPlayer)
		{
		delete iPlayer;
		}
	}
//
/**
 * Implementation of AO timer used to invoke test processing at desired intervals (i.e. State Machine)
 **/

CFsmTrigger* CFsmTrigger::NewL(CAsyncTestMmfAclntAudioPlayerUtil& aObserver)
	{
	CFsmTrigger* self = new (ELeave) CFsmTrigger(aObserver);	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	
	return self;
	}

CFsmTrigger::CFsmTrigger(CAsyncTestMmfAclntAudioPlayerUtil& aObserver)
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
	iObserver.FsmL();
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

//============================================================================================================
//------------------------------------------------------------------

CTestMmfAclntAudioPlayStopStart::CTestMmfAclntAudioPlayStopStart(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioPlayStopStart* CTestMmfAclntAudioPlayStopStart::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioPlayStopStart* self = new (ELeave) CTestMmfAclntAudioPlayStopStart(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioPlayStopStart::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Play Stop/Start"));
	TVerdict ret = EFail;

	TInt duration = I64INT(aPlayer->Duration().Int64());

	// Tests pause functionality
	aPlayer->Play();
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	User::After(KTwoSeconds);
	INFO_PRINTF1( _L("Stop CMdaAudioPlayerUtility"));
	aPlayer->Stop();

	
	User::After(KTwoSeconds);
	// only wait for second call to play.
	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();

	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());


	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), 
		duration, actualDuration);
	if((iError == KErrNone) && 
		(TimeComparison(actualDuration, duration , KExpectedDeviation)))
		{
		ret = EPass;
		}
	
	return ret;
	}

//------------------------------------------------------------------

// same as above, but with Pause instead of Stop
CTestMmfAclntAudioPlayPauseStart::CTestMmfAclntAudioPlayPauseStart(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioPlayPauseStart* CTestMmfAclntAudioPlayPauseStart::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioPlayPauseStart* self = new (ELeave) CTestMmfAclntAudioPlayPauseStart(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioPlayPauseStart::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Play Pause/Start"));
	TVerdict ret = EFail;

	TInt duration = I64INT(aPlayer->Duration().Int64());

	// Tests pause functionality
	// NB we must take the duration from the time at PAUSE, as pause (unlike stop)
	// does not reset the clip to start
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();
	TTime start;
	start.HomeTime();
	User::After(KTwoSeconds);
	INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
    INFO_PRINTF1(_L("Pause()"));//Statement Changed under DEF105143
	aPlayer->Pause();
	TTime stop;
	stop.HomeTime();
	TUint beforeDuration = I64INT(stop.MicroSecondsFrom(start).Int64());
	User::After(KTwoSeconds);
	
	// wait for second call to play.
	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	INFO_PRINTF1(_L("Play()"));//Statement Changed under DEF105143
	aPlayer->Play();
	start.HomeTime();
	CActiveScheduler::Start();
	stop.HomeTime();

	TUint afterDuration = I64INT(stop.MicroSecondsFrom(start).Int64());


	INFO_PRINTF7(_L("Error : %d Start = %u Stop = %u Duration = %u ActualDuration = %u + %u"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), 
		duration, beforeDuration, afterDuration);
	if((iError == KErrNone) && 
		(TimeComparison(beforeDuration+afterDuration, duration, KExpectedDeviation*2)))
		{
		ret = EPass;
		}
	
	return ret;
	}

//------------------------------------------------------------------

// Play, pause, play
CTestMmfAclntAudioPlayPausePlayTest::CTestMmfAclntAudioPlayPausePlayTest(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioPlayPausePlayTest* CTestMmfAclntAudioPlayPausePlayTest::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioPlayPausePlayTest* self = new (ELeave) CTestMmfAclntAudioPlayPausePlayTest(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioPlayPausePlayTest::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Play-Pause-Play"));
	TVerdict ret = EFail;

	// Play #1
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();
	User::After(KTwoSeconds);

	// Pause #1
	INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
	aPlayer->Pause();
	User::After(KTwoSeconds);

	// Play #2
	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();
	User::After(KOneSecond);

	// Pause #2
	INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
	aPlayer->Pause();
	User::After(KOneSecond);

	// Play #3
	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();
	User::After(KOneSecond);

	// Pause #3
	INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
	aPlayer->Pause();
	User::After(400000);

	// Play #4
	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();
	CActiveScheduler::Start();		// play to end

	if (iError == KErrNone)
		{
		ret = EPass;
		}
	
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioPlayVolume::CTestMmfAclntAudioPlayVolume(const TDesC& aTestName, const TDesC& aSectName, const TInt aVolume,TBool aNegativeTest)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName), iVolume(aVolume) , iNegativeTest(aNegativeTest)
	{}

CTestMmfAclntAudioPlayVolume* CTestMmfAclntAudioPlayVolume::NewL(const TDesC& aTestName, const TDesC& aSectName, const TInt aVolume,TBool aNegativeTest)
	{
	CTestMmfAclntAudioPlayVolume* self = new (ELeave) CTestMmfAclntAudioPlayVolume(aTestName, aSectName, aVolume, aNegativeTest);
	return self;
	}


TVerdict CTestMmfAclntAudioPlayVolume::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Play Volume"));
	TVerdict ret = EFail;


	// Check maxvolume function
	if(iVolume == -1)
		{
		INFO_PRINTF1( _L("Volume CHK 1"));
		iVolume = aPlayer->MaxVolume();
		INFO_PRINTF2( _L("Volume: Max Volume = %d"),iVolume);
		
		//on device & emulator KMaxVolume have different values
		//if(iVolume != KMaxVolume) // XXX: how to cross ref. this appears to be random values at the moment 
		//	{
		//	INFO_PRINTF3( _L("Expected volume: %d, received volume : %d"), 
		//		KMaxVolume, iVolume);
			
		//	return EFail;
		//	}
		}
	// Volume is truncated to maxvolume
	if(iVolume > aPlayer->MaxVolume())
		{
		INFO_PRINTF1( _L("Volume CHK 2"));
		TInt volume;
		INFO_PRINTF2( _L("Volume: SetVolume(%d)"),iVolume);
		aPlayer->SetVolume(iVolume);
		aPlayer->GetVolume(volume);
		INFO_PRINTF2( _L("Volume: GetVolume() returned"),volume);
		if(volume == aPlayer->MaxVolume())
			{
			ret = EPass;
			}
			
		}
	// Volume is truncated to 0
	else if(iVolume < 0)
		{
		INFO_PRINTF1( _L("Volume CHK 3"));
		TInt volume;
		INFO_PRINTF2( _L("Volume: SetVolume(%d)"),iVolume);
		aPlayer->SetVolume(iVolume);
		aPlayer->GetVolume(volume);
		INFO_PRINTF2( _L("Volume: GetVolume() returned"),volume);
		if(volume == 0)
			{
			ret = EPass;
			}
			
		}
	// Set volume and check
	else
		{
		INFO_PRINTF1( _L("Volume CHK 4"));
		INFO_PRINTF2( _L("Volume: SetVolume(%d)"),iVolume);
		TInt volume;
		aPlayer->SetVolume(iVolume);
		aPlayer->GetVolume(volume);
		INFO_PRINTF2( _L("Volume: GetVolume() returned"),volume);
		if(volume == iVolume)
			{
			ret = EPass;
			}
			
		else
			INFO_PRINTF3( _L("Expected volume: %d, received volume : %d"), iVolume, volume);

		}

	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioRepeat::CTestMmfAclntAudioRepeat(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioRepeat* CTestMmfAclntAudioRepeat::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioRepeat* self = new (ELeave) CTestMmfAclntAudioRepeat(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioRepeat::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Repeats"));

	TVerdict ret = EFail;
	TTimeIntervalMicroSeconds silence(0); // silence gap
	TInt duration = I64INT(aPlayer->Duration().Int64());

	aPlayer->SetRepeats(NUMBER_OF_REPEATS,silence);

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();

	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF2( _L("Repeats : %d"), NUMBER_OF_REPEATS);
	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
	if((iError == KErrNone) && (TimeComparison(actualDuration, duration * (NUMBER_OF_REPEATS + 1),
											   KExpectedDeviation * NUMBER_OF_REPEATS)))
		{
		ret = EPass;
		}
		
	return ret;
	}

//-------------------------------------------------

CTestMmfAclntAudioBalance::CTestMmfAclntAudioBalance(const TDesC& aTestName, const TDesC& aSectName,const TInt aValue, const TInt aExpectedError)
	: CTestMmfAclntAudioPlayerUtil(aTestName,aSectName), iBalance(aValue),
	iExpectedError(aExpectedError)
	{}

CTestMmfAclntAudioBalance* CTestMmfAclntAudioBalance::NewL(const TDesC& aTestName, const TDesC& aSectName,const TInt aValue, const TInt aExpectedError)
	{
	CTestMmfAclntAudioBalance* self = new (ELeave) CTestMmfAclntAudioBalance(aTestName,aSectName,aValue, aExpectedError);
	return self;
	}

TVerdict CTestMmfAclntAudioBalance::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	TVerdict verdict = EFail;
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Balance"));
	TInt	theErr = KErrNone;
	if (iBalance < KMinBalance)
		{
		theErr = aPlayer->SetBalance(iBalance);
		if(theErr != KErrNone)
			{
			return EFail;
			}
			
		theErr = aPlayer->GetBalance(iBalance);
		if(theErr != iExpectedError)
			{
			return EFail;	
			}
	
		INFO_PRINTF2( _L("Test finished with expectable error: %d"), iExpectedError);
		return EPass;
		}
	else if (iBalance > KMaxBalance)
		{
		theErr = aPlayer->SetBalance(iBalance);		
		if(theErr != KErrNone)
			{
			return EFail;
			}
			
		theErr = aPlayer->GetBalance(iBalance);
		if(theErr != iExpectedError)
			{
			return EFail;	
			}
		

		INFO_PRINTF2( _L("Test finished with expectable error: %d"), iExpectedError);
		return EPass;
		}
	else 
		{
		theErr = aPlayer->SetBalance(iBalance);		
		if(theErr != iExpectedError)
			{
			return EFail;
			}
		
		theErr = aPlayer->GetBalance(iBalance);
		if(theErr != iExpectedError)
			{
			return EFail;	
			}
		
		
		INFO_PRINTF2( _L("Test finished with expectable error: %d"), iExpectedError);
		verdict = EPass;
		}
	return verdict;
	}

CTestMmfAclntAudioDuration::CTestMmfAclntAudioDuration(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioDuration* CTestMmfAclntAudioDuration::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioDuration* self = new (ELeave) CTestMmfAclntAudioDuration(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioDuration::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Duration"));
	TVerdict ret = EFail;
	if (iDuration == aPlayer->Duration())
		{
		ret = EPass;
		}
	
	return ret;
	}

//------------------------------------------------------------------

CDelayedMethodCall* CDelayedMethodCall::NewL(MDelayedMethodCall* aThat)
		{
		__ASSERT_ALWAYS(aThat, User::Leave(KErrArgument));
		CDelayedMethodCall* self = new (ELeave) CDelayedMethodCall(aThat);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		};


void CDelayedMethodCall::RunL() 
	{
	iThat->DelayedMethodCallL();
	}

TInt CDelayedMethodCall::RunError(TInt aError) 
	{
	RDebug::Print( _L("CDelayedMethodCall::RunL Left %d\n"), aError);
	return KErrNone;
	}


CDelayedMethodCall::CDelayedMethodCall(MDelayedMethodCall* aThat) : CTimer(0), iThat(aThat)
	{
	CActiveScheduler::Add(this);
	}



//------------------------------------------------------------------
/**
 * Constructor
 */
CTestMmfAclntPriority::CTestMmfAclntPriority(const TDesC& aTestName, const TDesC& aSectName, const TMdaPriority aPriority)
	{
	// store the name of this test case
	// this is the name that is used by the script file
// Each test step initialises it's own name
	iTestStepName = aTestName;
	iPriority = aPriority;
	iPriorityManual = ETrue;
	iSectName = aSectName;
	}

CTestMmfAclntPriority* CTestMmfAclntPriority::NewL(const TDesC& aTestName, const TDesC& aSectName, const TMdaPriority aPriority)
	{
	CTestMmfAclntPriority* self = new (ELeave) CTestMmfAclntPriority(aTestName, aSectName, aPriority);
	return self;
	}

// overridden constructor for no priority setting
CTestMmfAclntPriority::CTestMmfAclntPriority(const TDesC& aTestName, const TDesC& aSectName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iPriority = EMdaPriorityNormal;
	iPriorityManual = EFalse;
	iSectName = aSectName;
	}


CTestMmfAclntPriority* CTestMmfAclntPriority::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntPriority* self = new (ELeave) CTestMmfAclntPriority(aTestName, aSectName);
	return self;
	}

void CTestMmfAclntPriority::MchoComplete(TInt aID, TInt aError)
	{
	INFO_PRINTF3( _L("Callback for CMdaAudioPlayerUtility called : ID %d, error %d"), aID, aError);

	iError = aError;

	iCallbackError[aID] = aError;


	if(iFirstCallback == -1)
		{
		iFirstCallback = aID;
		}
		
	
	if((--iCallbackCount) <= 0)
		{
		CActiveScheduler::Stop();	
		}
		
	}


//This method is used to start playing file 0 after file 1 has started
void CTestMmfAclntPriority::DelayedMethodCallL()
{
	audioUtil[0]->Play();
	if(iError == KErrNone)
		{
		INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility 0"));
		}
}




/**
 * Audio priority.
 */
TVerdict CTestMmfAclntPriority::DoTestStepL()
	{ 
	INFO_PRINTF1( _L("TestPlayerUtils : Priority"));

	TBuf<KSizeBuf>	filename0, filename1;

	TPtrC file; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), file))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename0);
	filename0.Append(file);

	if(!GetStringFromConfig(iSectName, _L("playerAudioFile2"), file))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename1);
	filename1.Append(file);

	TVerdict ret=EFail;

	CPlayerCallbackHandler* callback[2];

	iError = KErrNone;

	for(TInt i=0; i<2; i++)
		{
		callback[i] = new (ELeave) CPlayerCallbackHandler(i,this);
		CleanupStack::PushL(callback[i]);
		}


	audioUtil[0] = CMdaAudioPlayerUtility::NewFilePlayerL(filename0,*callback[0],EMdaPriorityNormal);
	CleanupStack::PushL(audioUtil[0]);
	audioUtil[1] = CMdaAudioPlayerUtility::NewFilePlayerL(filename1,*callback[1],EMdaPriorityNormal);
	CleanupStack::PushL(audioUtil[1]);

	INFO_PRINTF2( _L("Initialise CMdaAudioPlayerUtility 0 - %S"), &filename0);
	INFO_PRINTF2( _L("Initialise CMdaAudioPlayerUtility 1 - %S"), &filename1);

	iCallbackCount = 2;
	// wait for initilisation callback
	CActiveScheduler::Start();

	if(iError == KErrNone && iCallbackCount == 0)
		{
		// set manual priority on audioUtil[0] ?
		if(iPriorityManual)
			{
			TInt priErr = audioUtil[0]->SetPriority(iPriority, EMdaPriorityPreferenceNone);
			INFO_PRINTF3(_L("Priority set manually to %d - return %d"), iPriority, priErr); 
			}

		audioUtil[1]->Play();
		if(iError == KErrNone)
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility 1"));
			}

		iCallbackCount = 2;
		iFirstCallback = -1;
		
		CDelayedMethodCall* delayedMethodCall = CDelayedMethodCall::NewL(this);
		CleanupStack::PushL(delayedMethodCall);
		delayedMethodCall->After(KOneSecond);
		
		// wait for play to complete
		INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(delayedMethodCall); //delayedMethodCall
			
		if(iError == KErrNone && iCallbackCount == 0)
			{			
			if(	((iPriority <  EMdaPriorityNormal) && (iFirstCallback == 0 ) && (iCallbackError[0]==KErrInUse) && (iCallbackError[1]==KErrNone)) || 
				((iPriority ==  EMdaPriorityNormal) && (iFirstCallback == 0 ) && (iCallbackError[0]==KErrInUse) && (iCallbackError[1]==KErrNone)) || 
				((iPriority >  EMdaPriorityNormal) && (iFirstCallback == 1 ) && (iCallbackError[0]==KErrNone) && (iCallbackError[1]==KErrInUse)) 
				)
				{
				ret = EPass;
				}
			INFO_PRINTF2( _L("First callback with Id: %d"), iFirstCallback);
			INFO_PRINTF3( _L("Error codes - file 0 = %d   file 1 = %d"), iCallbackError[0], iCallbackError[1]);
			}
		}

	if(iError != 0)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
		}
		

	CleanupStack::PopAndDestroy(4); // audioUtil , callback

	User::After(KOneSecond);
	return ret;
	}



//------------------------------------------------------------------

CTestMmfAclntAudioPosition::CTestMmfAclntAudioPosition(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition)
	: CTestMmfAclntAudioPlayerUtil(aTestName,aSectName), iPosition(aPosition)
	{}

CTestMmfAclntAudioPosition* CTestMmfAclntAudioPosition::NewL(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition)
	{
	CTestMmfAclntAudioPosition* self = new (ELeave) CTestMmfAclntAudioPosition(aTestName,aSectName,aPosition);
	return self;
	}

TVerdict CTestMmfAclntAudioPosition::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Position"));
	TVerdict ret = EFail;
	TTimeIntervalMicroSeconds position;
	TBool validPosition = EFalse;
	INFO_PRINTF3(_L("Set Position %d Duration = %d"),
		I64INT(iPosition.Int64()), I64INT(aPlayer->Duration().Int64()));

	//  Set position: middle of clip.
	if (I64INT(iPosition.Int64()) == 0)
		{
		iPosition = I64INT(aPlayer->Duration().Int64())/2;
		}
		
	// Set position: end of clip.
	if (I64INT(iPosition.Int64()) == -1)
		{
		iPosition = aPlayer->Duration();
		}
	
	// Position is beyond the end of the clips duration.
	// so check that the value is clipped.
	if(aPlayer->Duration() < iPosition)
		{
		aPlayer->SetPosition(iPosition);
		
		//aPlayer->Play();           // Playing the clip will reset the position to 0 when finished.
		//CActiveScheduler::Start();

		/*TInt theRes1 = */aPlayer->GetPosition(position);
		
		
		INFO_PRINTF2(_L("Longer than duration : Setting position to %d"), I64INT(iPosition.Int64()));
		INFO_PRINTF3(_L("Set : position = %d (if clipped : %d)"), 
			I64INT(position.Int64()), I64INT(aPlayer->Duration().Int64()));

		TInt theRes = aPlayer->GetPosition(position);
		if(theRes == KErrNone && (position.Int64() == aPlayer->Duration().Int64()))
			{
			validPosition = ETrue;
			}
		}
			
		
	// Position is before the end of the clips duration.
	// so check that the value is clipped to 0
	// *** added by JW after discussion re. expected behaviour -
	// for test 1009
	else if(iPosition < TTimeIntervalMicroSeconds(0))
		{
		aPlayer->SetPosition(iPosition);
		
		aPlayer->Play();
		CActiveScheduler::Start();

		/*TInt theRes1 = */aPlayer->GetPosition(position);
		
		
		INFO_PRINTF2(_L("Shorter than 0 : Setting position to %d"), I64INT(iPosition.Int64()));
		INFO_PRINTF2(_L("Set : position = %d (if clipped : 0)"), 
			I64INT(position.Int64()));

		TInt theRes = aPlayer->GetPosition(position);
		if(theRes == KErrNone && I64INT(position.Int64()) == 0)
			{
			validPosition = ETrue;
			}
			
		}

	else
		{

		aPlayer->SetPosition(iPosition);
		aPlayer->GetPosition(position);
		INFO_PRINTF3(_L("Set : position = %d, duration = %d)"), 
			I64INT(position.Int64()), I64INT(aPlayer->Duration().Int64()));
		aPlayer->Play();

		CActiveScheduler::Start();

		// For test 1009, we check that the expected error code was returned
		// *** removed by JW after discussion re. expected behaviour
		/*
		if (iPosition < TTimeIntervalMicroSeconds(0))
			{
			TVerdict ret = EFail;
			if (iError == KErrArgument)
				{
				if (position != TTimeIntervalMicroSeconds(0))
					{
					INFO_PRINTF2(_L("ERROR: KErrArgument returned, but position was %d, expected position = 0"),position);
					ret = EFail;
					}
				else
					{
					// Test passed
					ret = EPass;
					}
				}
			else
				{
				INFO_PRINTF3(_L("Expected error not returned, expecting %d, got %d"), KErrArgument, iError);
				ret = EFail;
				}
			return ret;
			}
		*/

		aPlayer->Stop();		

		//TInt64 temp = (iPosition.Int64() / KPcm16FrameInterval) - 1; // -1 for DevSound
		aPlayer->GetPosition(position);
		// returned position should equal the original position or number of frames * frame intervals.
		// The above comment is invalid, one the clip has played the position should be reset to 0. - INC24264
		if (position.Int64() == 0)
		//if(position.Int64() == (temp * KPcm16FrameInterval) || (position == iPosition))
			{
			validPosition = ETrue;
			}
		//else
			INFO_PRINTF3( _L("Expected position : %f, received position  : %f"), 
					I64REAL(iPosition.Int64()), I64REAL(position.Int64()));
		}

	// Postion was set to a valid value.
	if(validPosition)
		{
		aPlayer->Play();
		// wait for play to complete
		INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		CActiveScheduler::Start();

		INFO_PRINTF2( _L("Error : %d"), iError);
		if (iError == KErrNone)
			ret = EPass;
		}
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioPlayWindow::CTestMmfAclntAudioPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TBool aNegative)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName), iNegative(aNegative)
	{}

CTestMmfAclntAudioPlayWindow* CTestMmfAclntAudioPlayWindow::NewL(const TDesC& aTestName, const TDesC& aSectName, const TBool aNegative)
	{
	CTestMmfAclntAudioPlayWindow* self = new (ELeave) CTestMmfAclntAudioPlayWindow(aTestName, aSectName, aNegative);
	return self;
	}

TVerdict CTestMmfAclntAudioPlayWindow::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Play Window"));
	TVerdict ret = EFail;
	TInt64 temp1, temp2;
	TInt duration = I64INT(aPlayer->Duration().Int64());

	TInt windowDuration = 0;
	if(iNegative)
		{
		INFO_PRINTF1( _L("Attempting to set an invalid play window"));
		temp1 = -KInvalidNumber;
		temp2 = aPlayer->Duration().Int64() + KInvalidNumber;
		windowDuration = I64INT(aPlayer->Duration().Int64());
		}
	else
		{
		temp1 = aPlayer->Duration().Int64()/2;
		temp2 = aPlayer->Duration().Int64();

		windowDuration = (I64INT(temp2) - I64INT(temp1));
		}


	TTimeIntervalMicroSeconds startTime(temp1);
	TTimeIntervalMicroSeconds endTime(temp2); 

	INFO_PRINTF4(_L("StartTime = %d EndTime = %d WindowDuration = %d"),
		I64INT(startTime.Int64()), I64INT(endTime.Int64()), windowDuration);
	
	TInt windowSet = aPlayer->SetPlayWindow(startTime,endTime);

	if (iNegative && (windowSet == KErrArgument))
		{
		INFO_PRINTF1( _L("Correct return from SetPlayWindow (KErrArgument) when attempting to set an invalid window range"));
		return EPass;
		}

	if (windowSet != KErrNone)
		{
		INFO_PRINTF2( _L("Unexpected error returned from SetPlayWindow - %d"),windowSet);
		return EFail;
		}

	iError = KErrTimedOut;
	INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
	aPlayer->Play();

	TTime start;
	start.HomeTime();
	CActiveScheduler::Start();
	TTime stop;
	stop.HomeTime();

	TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

	INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d WindowDuration = %d ActualDuration = %d"),
		iError, I64INT(start.Int64()), I64INT(stop.Int64()), windowDuration, actualDuration);

	if((iError == KErrNone) && (TimeComparison(actualDuration, windowDuration, KExpectedDeviation)))
		{
		aPlayer->ClearPlayWindow();

		iError = KErrTimedOut;
		INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		aPlayer->Play();

		TTime start;
		start.HomeTime();
		CActiveScheduler::Start();
		TTime stop;
		stop.HomeTime();

		TUint actualDuration = I64INT(stop.MicroSecondsFrom(start).Int64());

		INFO_PRINTF6(_L("Error : %d Start = %d Stop = %d Duration = %d ActualDuration = %d"),
			iError, I64INT(start.Int64()), I64INT(stop.Int64()), duration, actualDuration);
			ret =  EPass;
		}
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioType::CTestMmfAclntAudioType(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioType* CTestMmfAclntAudioType::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioType* self = new (ELeave) CTestMmfAclntAudioType(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioType::DoTestL(CMdaAudioPlayerUtility* /*aPlayer*/)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Type"));
	INFO_PRINTF1( _L("#######################################################"));
	INFO_PRINTF1( _L("No Audio Type API call for class CMdaAudioPlayerUtility"));
	INFO_PRINTF1( _L("This test is therefore inconclusive."));
	INFO_PRINTF1( _L("#######################################################"));
	TVerdict ret = EInconclusive;

	// what code should/might look like.
	//CMdaAudioType* type = aPlayer->TypeL();

	// CMdaAudioType class not complete:
	// XXX check data returned is valid???

	return ret;
	}

//--------------------------------------------------------------

CTestMmfAclntAudioRamp::CTestMmfAclntAudioRamp(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioRamp* CTestMmfAclntAudioRamp::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioRamp* self = new (ELeave) CTestMmfAclntAudioRamp(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioRamp::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Ramp"));
	TVerdict ret = EFail;
	TTimeIntervalMicroSeconds ramp(100);
	aPlayer->SetVolumeRamp(ramp);

	aPlayer->Play();
	// wait for open to complete
	iError = KErrTimedOut;
	CActiveScheduler::Start();
	if(iError == KErrNone)	
		ret = EPass;
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioMeta::CTestMmfAclntAudioMeta(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioMeta* CTestMmfAclntAudioMeta::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioMeta* self = new (ELeave) CTestMmfAclntAudioMeta(aTestName, aSectName);
	return self;
	}

_LIT(name,"Title");
_LIT(data,"Symbian");
TVerdict CTestMmfAclntAudioMeta::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Metadata"));
	
	TInt numEntries;
	TInt theErr = aPlayer->GetNumberOfMetaDataEntries(numEntries);
	INFO_PRINTF3(_L("Error : %d NumEntries : %d"), theErr, numEntries);

	if(theErr && theErr != KErrNotSupported) // Audio Controller doesn't support - this is ok
		{
		ERR_PRINTF2(_L("GetNumberOfMetaDataEntries returned unexpected error %d"), theErr);
		return EFail;
		}

	CMMFMetaDataEntry* testData = NULL;
			
	TRAPD(theErr2, testData = aPlayer->GetMetaDataEntryL(0));
	if(!theErr2)
		{
		if((testData->Name() == name) && (testData->Value() == data))
			return EPass;
		}
	else
		{
		if(theErr == KErrNotSupported) // Audio Controller doesn't support - this is ok
			{
			INFO_PRINTF1(_L("Note : Audio Controller does not support metadata"));
			return EPass;
			}
		else if(numEntries == 0)
			{
			INFO_PRINTF1(_L("No metadata entries found"));
			return EPass;
			}
		else
			{
			ERR_PRINTF2(_L("GetMetaDataEntryL left with unexpected error %d"), theErr2);
			return EFail;
			}
		}
	return EPass;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioCloseOpen::CTestMmfAclntAudioCloseOpen(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioCloseOpen* CTestMmfAclntAudioCloseOpen::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioCloseOpen* self = new (ELeave) CTestMmfAclntAudioCloseOpen(aTestName, aSectName);
	return self;
	}




TVerdict CTestMmfAclntAudioCloseOpen::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Close/Open"));

	TVerdict ret = EFail;
	aPlayer->Close(); 

	// get second file name
	TBuf<KSizeBuf>	filename2;
	TPtrC			filename; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename2);
	filename2.Append(filename);

	// Open second file
	aPlayer->OpenFileL(filename2);
	// wait for open to complete
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	TInt openErr = iError;

	INFO_PRINTF2( _L("OpenFileL completed with error %d"), iError);

	if (iError == KErrNone && openErr == KErrNone)
		{
		ret = EPass;
		}
	
	return ret;
	}

//------------------------------------------------------------------

CTestMmfAclntAudioBitRate::CTestMmfAclntAudioBitRate(const TDesC& aTestName,const TDesC& aKeyName1, const TDesC& aKeyName2)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iKeyFileName = aKeyName1;
	iKeyBitRateName = aKeyName2;
	}

CTestMmfAclntAudioBitRate* CTestMmfAclntAudioBitRate::NewL(const TDesC& aTestName,const TDesC& aKeyName1, const TDesC& aKeyName2)
	{													   
	CTestMmfAclntAudioBitRate* self = new (ELeave) CTestMmfAclntAudioBitRate(aTestName,aKeyName1,aKeyName2);
	return self;
	}

void CTestMmfAclntAudioBitRate::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds & /*aDuration*/)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntAudioBitRate::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}


TVerdict CTestMmfAclntAudioBitRate::DoTestStepL( void )
	{
	
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Bit Rate"));
		
	iError = KErrTimedOut;
	TUint bitRate = 0;
	TVerdict res = EFail;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionNine"),iKeyFileName,filename1))
		{
		return EInconclusive;
		}
	if(!GetIntFromConfig(_L("SectionNine"),iKeyBitRateName,iExpectedBitRate))
		{
		return EInconclusive;
		}
	
	INFO_PRINTF2(_L("Expected bit rate: %d"), iExpectedBitRate);
	
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewFilePlayerL(filename1,*this);
	CleanupStack::PushL(player);
	CActiveScheduler::Start();
	
	if (iError == KErrNone)
		{
		player->Play();
		CActiveScheduler::Start();
		if(iError == KErrNone) 
			{
			TInt err = player->GetBitRate(bitRate);
			if(err == KErrNone)
				{
				INFO_PRINTF2(_L("Retrieved bit rate: %d"), bitRate);
				if(static_cast<TInt>(bitRate) == iExpectedBitRate)
					{
					INFO_PRINTF1(_L("The Expected bit rate and retrieved bit rate matched"));
					res = EPass;
					}
				else
					{
					INFO_PRINTF1(_L("The Expected bit rate and retrieved bit rate does not match"));	
					}
				}
			else
				{
				INFO_PRINTF2(_L("Failed with error %d while getting the bit rate"), err);
				}
			}
		else
			{
			INFO_PRINTF1(_L("Failed during playing the audio"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("Failed during opening the file. Error =  %d "), iError);
		}
	CleanupStack::PopAndDestroy(player);
	return res;
	}
	

//------------------------------------------------------------------	
CTestMmfAclntAudioQuery::CTestMmfAclntAudioQuery(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntAudioQuery* CTestMmfAclntAudioQuery::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntAudioQuery* self = new (ELeave) CTestMmfAclntAudioQuery(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntAudioQuery::DoTestL(CMdaAudioPlayerUtility* /*aPlayer*/)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Query"));

	TVerdict ret = EFail;

	// enhanced - to use CMMFControllerPluginSelectionParameters

	INFO_PRINTF1(_L("Querying play formats..."));

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	RArray<TUid> mediaIds;
	mediaIds.Append(KUidMediaTypeAudio);
	cSelect->SetMediaIdsL(mediaIds, CMMFPluginSelectionParameters::EAllowOtherMediaIds);
	cSelect->SetRequiredPlayFormatSupportL(*fSelect); 

	RMMFControllerImplInfoArray controllers; // Array to hold all the controllers support the match data
	CleanupResetAndDestroyPushL(controllers);
	cSelect->ListImplementationsL(controllers);  // Populates the array with all the suitable controllers
	TInt numControllers = controllers.Count();
	if(!numControllers)
		{
		ERR_PRINTF1(_L("Could not find any controllers"));
		}
	else
		{
		INFO_PRINTF2(_L("Found %d controllers"), numControllers);
		for(int i = 0; i < numControllers; i++)
			{
			RMMFFormatImplInfoArray cFormats = controllers[i]->PlayFormats();
			const CDesC8Array* fileExtensions = &cFormats[0]->SupportedMimeTypes();
			INFO_PRINTF3(_L("- Controller: %d Extensions : %d"), i, fileExtensions->Count());
			for(int j = 0; j < fileExtensions->Count(); j++)
				{
				TBuf<24> ext16;
				ext16.Copy(fileExtensions->MdcaPoint(j));
				INFO_PRINTF2(_L("-- %S"), &ext16);
				}
			}
		ret = EPass;
		}

	CleanupStack::PopAndDestroy(3);//controllers, fSelect, cSelect

	return ret;
	}

//--------------------------------------------------------------------

// Negative tests

//--------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntNoLoad::CTestMmfAclntNoLoad(const TDesC& aTestName, const TDesC& aSectName)
	:CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{
	}

CTestMmfAclntNoLoad* CTestMmfAclntNoLoad::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntNoLoad* self = new (ELeave) CTestMmfAclntNoLoad(aTestName, aSectName);
	return self;
	}

TVerdict CTestMmfAclntNoLoad::DoTestL(CMdaAudioPlayerUtility* /*aPlayer*/)
	{
	return EPass;
	}

/**
 * Play with no loaded sound file.
 */
TVerdict CTestMmfAclntNoLoad::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestPlayerUtils : No Load"));

	iError = KErrTimedOut;
	iStop = ETrue;

	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewL(*this);
	CleanupStack::PushL(player);
 	player->Play();

	CActiveScheduler::Start();

	if(iError == KErrNotReady) 
		{
		CleanupStack::PopAndDestroy(player);
		return EPass; 
		}
	
	ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d (expected KErrNotReady)"),iError );
	CleanupStack::PopAndDestroy(player);
	return	EFail;
	}

//------------------------------------------------------------------

CTestMmfAclntDeletePlay::CTestMmfAclntDeletePlay(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName)
	{}

CTestMmfAclntDeletePlay* CTestMmfAclntDeletePlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntDeletePlay* self = new (ELeave) CTestMmfAclntDeletePlay(aTestName, aSectName);
	return self;
	}



/**
 * Audio clip - Delete object before play operation has completed.
 */
TVerdict CTestMmfAclntDeletePlay::DoTestL(CMdaAudioPlayerUtility* /*aPlayer*/)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Delete/Play"));

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	iError = KErrTimedOut;
	__MM_HEAP_MARK;

	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewFilePlayerL(filename,*this);
	CleanupStack::PushL(player);
	// wait for open to complete
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		player->Play();
		CleanupStack::PopAndDestroy(player); // destroy player before play has completed
		__MM_HEAP_MARKEND;
		User::Heap().Check();
		return EPass;
		}

	CleanupStack::PopAndDestroy(player);
	ERR_PRINTF2( _L("CMdaAudioRecorderUtility failed with error %d"),iError );
	__MM_HEAP_MARKEND;
	return EFail;
	}

//------------------------------------------------------------------

CTestMmfAclntPlayPlay::CTestMmfAclntPlayPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CTestMmfAclntAudioPlayerUtil(aTestName, aSectName), iKeyName(aKeyName)
	{}

CTestMmfAclntPlayPlay* CTestMmfAclntPlayPlay::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntPlayPlay* self = new (ELeave) CTestMmfAclntPlayPlay(aTestName, aSectName, aKeyName);
	return self;
	}


/**
 * Open new file while audio player is playing.
 */
TVerdict CTestMmfAclntPlayPlay::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Play/Play"));

	TVerdict ret = EFail;
	iError = KErrTimedOut;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	//if(!GetStringFromConfig(_L("SectionOne"), _L("playerAudioFile"), filename1))
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	aPlayer->Play();


	TInt error = KErrNone;
	TRAP(error,aPlayer->OpenFileL(filename));
	// wait for open to complete
	CActiveScheduler::Start();
	aPlayer->Play();
	CActiveScheduler::Start();

	if(error == KErrNone)
		ret = EPass;

	return ret;
	}

//------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntCorruptDes::CTestMmfAclntCorruptDes(const TDesC& aTestName,const TDesC& aSectName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName; 
	iSectName = aSectName;
	}

void CTestMmfAclntCorruptDes::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntCorruptDes::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}



TVerdict  CTestMmfAclntCorruptDes::DoTestStepPreambleL()
	{
	RFs fs;
	RFile file;
	TInt size = 0;

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(iSectName, _L("playerAudioFile"), filename1))
		{
		return EInconclusive;
		}
		
	GetDriveName(filename);
	filename.Append(filename1);

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(file.Open(fs,filename,EFileRead));
	CleanupClosePushL(file);
	
	INFO_PRINTF2( _L("File under test - %S"), &filename);
	
	User::LeaveIfError(file.Size(size));
	INFO_PRINTF2(_L("size of file = %d\n"),size);//Statement Changed under DEF105143
	iAudio = HBufC8::NewMaxL(size);
	// Buffer has not been init with data.
	CleanupStack::PopAndDestroy(); //file

	return CTestMmfAclntStep::DoTestStepPreambleL();
	}

TVerdict CTestMmfAclntCorruptDes::DoTestStepPostambleL()
	{
	delete iAudio;
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Play using a corrupt descriptor.
 */
TVerdict CTestMmfAclntCorruptDes::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Corrupt Des"));
	iError = KErrTimedOut;

	TPtr8 bufferDes(iAudio->Des());
	CMdaAudioPlayerUtility * player = CMdaAudioPlayerUtility::NewDesPlayerL(bufferDes, *this);
	CleanupStack::PushL(player);

	// Wait for init callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if (iError == KErrBadDescriptor || iError == KErrNotSupported)
		{
		CleanupStack::PopAndDestroy(player);
		return EPass;			
		}
	if (iError == KErrNone)
		{
		iError = KErrTimedOut;
		player->Play();
		// wait for play.
		if (iError == KErrNone)
			{
			CleanupStack::PopAndDestroy(player);
			return EPass;			
			}
		}
	CleanupStack::PopAndDestroy(player);
	return	EFail;
	}

//
//=============================================================================================
//

CAsyncTestMmfAclntAudioRepeatPlayPausePlay* CAsyncTestMmfAclntAudioRepeatPlayPausePlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayPausePlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatPlayPausePlay::CAsyncTestMmfAclntAudioRepeatPlayPausePlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioRepeatPlayPausePlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/Play/Pause/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
	
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Trailing silence = %ld\n"),iSilence.Int64());
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();
				iPlayerState = EPlaying;
				iStart.HomeTime();// start the timer
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KTwoSeconds); // Call Pause for this test case after two sec
				iTestState = EStatePause;
				}
			else if(iPlayerState == EPaused || iPlayerState == EStopped )
				{
				iPlayer->Play();
				iStart.HomeTime();// start the timer for resuming playback
				iPlayerState = EPlaying;	
				}
			break;
			}
		case EStatePause:
			{
			if(iPlayerState == EPlaying)
				{
				INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
				iPlayer->Pause();
				iStop.HomeTime();
				
				iPausedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KTwoSeconds); // Resume after two seconds
				iPlayerState = EPaused;
				iTestState = EStatePlay;
				}
			break;	
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iResumedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				iActualDuration = iPausedDuration + iResumedDuration;
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
					if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
													   2*KExpectedDeviation))//more because of the latency during pausing
						{
						StopTest(); // success
						break;	
						}
					else
						{
						STOP_TEST_GEN_ERROR;
						break;	
						}
				#else
					if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
													   2*iNumOfRepeatsSet*KExpectedDeviation))
						{
						StopTest(); // success
						break;	
						}
					else
						{
						STOP_TEST_GEN_ERROR;
						break;	
						}
				#endif
				}
			else // In case of .sqn file there will be no duration
				{
				StopTest();
				break;
				}
			}
			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
	
CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay* CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay( aTestName, aSectName, aKeyName, aPosition);
	}

CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTimeIntervalMicroSeconds aPosition)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName), iPosition(aPosition)
	{
	}

void CAsyncTestMmfAclntAudioRepeatPlayRepositionPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Play/Reposition/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();
				iPlayerState = EPlaying;
				iStart.HomeTime();// start the timer
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call Setposition for this test case after two sec
				iTestState = EStateSetPosition;
				}
			break;
			}
		case EStateSetPosition:
			{
			if(iPlayerState == EPlaying)
				{
				INFO_PRINTF1( _L("SetPosition CMdaAudioPlayerUtility"));
				iPlayer->SetPosition(iPosition);
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				}
			break;	
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iResumedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				iActualDuration = iPausedDuration + iResumedDuration;
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, (iDuration * (iNumOfRepeatsSet + 1))+1,
												   2*KExpectedDeviation))//more because of the latency during repositioning
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, (iDuration * (iNumOfRepeatsSet + 1))+1,
												   2*iNumOfRepeatsSet*KExpectedDeviation))//more because of the latency during repositioning
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else // In case of .sqn file there will be no duration
				{
				StopTest();
				break;
				}
		
			}
			
		default:
			STOP_TEST_GEN_ERROR;
			break;
		}
	}
	
//============================================================================================================


CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay* CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioRepeatPlayPauseRepeatPlay::FsmL()
	{
	switch (iTestState)
	{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/Play/Pause/Repeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet );
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;	
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KTwoSeconds); // Call Pause for this test case after two sec
				iTestState = EStatePause;
				}
			else if(iPlayerState == EPaused || iPlayerState == EStopped )
				{
				iPlayer->Play(); // wait till play completes
				iStart.HomeTime();// start the timer after resuming playback
				iPlayerState = EPlaying;	
				}
			break;
			}
		case EStatePause:
			{
			if(iPlayerState == EPlaying)
				{
				INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
				iPlayer->Pause();
				iStop.HomeTime();
				
				iPausedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				//Now Set New Repeat Value
				if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("newRepeatValue"), iNumOfRepeatsSet))
					{
					ERR_PRINTF1(_L("GetIntFromConfig Failed"));
					StopTest(KErrGeneral, EInconclusive);
					return;
					}
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KTwoSeconds); // Resume after two seconds
				iPlayerState = EPaused;
				iTestState = EStatePlay;
				}
			break;	
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iResumedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				iActualDuration = iPausedDuration + iResumedDuration;
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation * 2))//more because of latency during Pausing
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
					if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation * 2*iNumOfRepeatsSet))//more because of latency during Pausing
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
					#endif
				break;	
				}
			else
				{
				StopTest(); // success
				break;		
				}
			}
		
			
		default:
			ERR_PRINTF1(_L("State machine error"));
			STOP_TEST_GEN_ERROR;
			break;
		}
	}
	
//============================================================================================================

CAsyncTestMmfAclntAudioPlayPauseRepeatPlay* CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayPauseRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::CAsyncTestMmfAclntAudioPlayPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayPauseRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Play/Pause/Repeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call Pause for this test case after two sec
				iTestState = EStatePause;
				}
			else if(iPlayerState == EPaused || iPlayerState == EStopped )
				{
				iPlayer->Play(); // wait till play completes
				iStart.HomeTime();//start the timer after resuming playback
				
				iPlayerState = EPlaying;
				}
			break;
			}
		case EStatePause:
			{
			if(iPlayerState == EPlaying)
				{
				INFO_PRINTF1(_L("Pause CMdaAudioPlayerUtility"));
				iPlayer->Pause();
				iStop.HomeTime();
				
				iPausedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
				iPlayerState = EPaused;
				
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Resume after one seconds
				iTestState = EStatePlay;
				}
			break;	
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iResumedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				iActualDuration = iPausedDuration + iResumedDuration;
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation * 2))//more because of latency during pausing
					{
					StopTest(); // success
					break;	
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation *iNumOfRepeatsSet* 2))//more because of latency during pausing
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;	
				}
			}
			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
	
//============================================================================================================

CAsyncTestMmfAclntAudioPlayRepeat* CAsyncTestMmfAclntAudioPlayRepeat::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayRepeat( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayRepeat::CAsyncTestMmfAclntAudioPlayRepeat( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayRepeat::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Play/Repeat"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();// Start the timer
			
				TTimeIntervalMicroSeconds silence(0); // silence gap
				INFO_PRINTF1( _L("SetRepeats with higher value CMdaAudioPlayerUtility"));
				if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("maxRepeatValue"), iNumOfRepeatsSet))
					{
					ERR_PRINTF1(_L("GetIntFromConfig Failed"));
					StopTest(KErrGeneral, EInconclusive);
					break;
					}
				INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					if(TimeComparison(iActualDuration, (iDuration + 100000) * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation))
						{
						StopTest(KErrGeneral, EKnownFailure);
						}
					else
						{
						STOP_TEST_GEN_ERROR;
						}
					}
				#else
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
											   KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif	
				break;	
				}
			else
				{
				StopTest(); // success
				break;		
				}
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}

//============================================================================================================

CAsyncTestMmfAclntAudioRepeatBeforeOpen* CAsyncTestMmfAclntAudioRepeatBeforeOpen::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatBeforeOpen( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatBeforeOpen::CAsyncTestMmfAclntAudioRepeatBeforeOpen( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioRepeatBeforeOpen::KickoffTestL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Player Util"));
	TPtrC			filename1; 
	TInt			repeatCount;
	
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		ERR_PRINTF1(_L("GetStringFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("numOfRepeat"), repeatCount))
		{
		ERR_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	iNumOfRepeatsSet = 	repeatCount;
	GetDriveName(iFileName);
	iFileName.Append(filename1);
	INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &iFileName);
	iTrigger = CFsmTrigger::NewL(*this);
		
	iPlayer = CMdaAudioPlayerUtility::NewL(*this);
	
	TTimeIntervalMicroSeconds trailingSilence(1); 
	INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility before opening the clip"));
	iNumOfRepeatsSet = NUMBER_OF_REPEATS;
	INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
	iPlayer->SetRepeats(iNumOfRepeatsSet,trailingSilence);
	iPlayer->OpenFileL(iFileName);
	iPlayerState = EOpening;
	}

void CAsyncTestMmfAclntAudioRepeatBeforeOpen::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();// Start the timer
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;		
				}
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
		
//============================================================================================================

CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay* CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayRepeatPauseRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Play/Repeat/Pause/Repeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStateSetRepeat:
			{
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(KOneSecond);// Call Pause after two seconds  
			iTestState = EStatePause;
			break;	
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(500000);
				iTestState = EStateSetRepeat;
				}
			else if(iPlayerState == EPaused || iPlayerState == EStopped )
				{
				iPlayer->Play(); // wait till play completes
				iStart.HomeTime();
				iPlayerState = EPlaying;	
				}
			break;
			}
		case EStatePause:
			{
			if(iPlayerState == EPlaying)
				{
				INFO_PRINTF1( _L("Pause CMdaAudioPlayerUtility"));
				iPlayer->Pause();
				iStop.HomeTime();
				iPausedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				
					//Now Set New Repeat Value
				if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("newRepeatValue"), iNumOfRepeatsSet))
					{
					ERR_PRINTF1(_L("GetIntFromConfig Failed"));
					StopTest(KErrGeneral, EInconclusive);
					break;
					}
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				INFO_PRINTF2( _L("Repeats = %d"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KTwoSeconds); // Resume after two seconds
				iPlayerState = EPaused;
				iTestState = EStatePlay;
				}
			break;	
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iResumedDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				iActualDuration = iPausedDuration + iResumedDuration;
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation * 2))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}					
				#else
				if(TimeComparison(iActualDuration, iDuration * (iNumOfRepeatsSet + 1),
												   KExpectedDeviation * iNumOfRepeatsSet* 2))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;		
				}
			}
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
	
//============================================================================================================


CAsyncTestMmfAclntAudioRepeatPlayStopPlay* CAsyncTestMmfAclntAudioRepeatPlayStopPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayStopPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatPlayStopPlay::CAsyncTestMmfAclntAudioRepeatPlayStopPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioRepeatPlayStopPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/Play/Stop/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();
				iStart.HomeTime();
				
				iPlayerState = EPlaying;
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call Stop for this test case after two sec
				iTestState = EStateStop;
				}
			else if(iPlayerState == EPaused || iPlayerState == EStopped )
				{
				iPlayer->Play(); // wait till play completes
				iStart.HomeTime();
				iPlayerState = EPlaying;	
				}
			break;
			}
		case EStateStop:
			{
			INFO_PRINTF1( _L("Stop CMdaAudioPlayerUtility"));
			iPlayer->Stop();
			iPlayerState = EStopped;
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(KTwoSeconds); // Call Pause for this test case after two sec
			iTestState = EStatePlay;
			break;
			}	
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration*(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration*(iNumOfRepeatsSet+1), KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration*(iNumOfRepeatsSet+1), KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest();
				break;	
				}
			}
			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}	
	}
	
//============================================================================================================


CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay* CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay::CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioRepeatPlayRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/Play/Repeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("newRepeatValue"), iNumOfRepeatsSet))
					{
					ERR_PRINTF1(_L("GetIntFromConfig Failed"));
					StopTest(KErrGeneral, EInconclusive);
					break;
					}
				INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet );
				iPlayer->SetRepeats(iNumOfRepeatsSet, iSilence);				
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),
												   KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),
												   KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}				
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;		
				}
			}
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}		
	}
	
//============================================================================================================



CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay* CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayRepeatRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Play/Repeat/Repeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();
				iPlayerState = EPlaying;
				iStart.HomeTime();
				
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				INFO_PRINTF2( _L("Repeats = %d"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				
				INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
				if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("newRepeatValue"), iNumOfRepeatsSet))
					{
					ERR_PRINTF1(_L("GetIntFromConfig Failed"));
					StopTest(KErrGeneral, EInconclusive);
					break;
					}
				INFO_PRINTF2( _L("Repeats = %d"),iNumOfRepeatsSet);
				iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),KExpectedDeviation))
												   // since we setting repeats during first playback of first repeat, we need to take into consideration of first playback
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),KExpectedDeviation*iNumOfRepeatsSet))
												   // since we setting repeats during first playback of first repeat, we need to take into consideration of first playback
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;		
				}
		
			}
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}	
	}
	
//============================================================================================================



CAsyncTestMmfAclntAudioPlayRepeatRamp* CAsyncTestMmfAclntAudioPlayRepeatRamp::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayRepeatRamp( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayRepeatRamp::CAsyncTestMmfAclntAudioPlayRepeatRamp( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayRepeatRamp::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/SetVolumeRamp/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));

			iPlayer->Play();	
			iPlayerState = EPlaying;
			iStart.HomeTime();
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			INFO_PRINTF1( _L("SetVolumeRamp CMdaAudioPlayerUtility"));
			iPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds(5000000));	
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration *(iNumOfRepeatsSet+1));
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),
												   KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iDuration *(iNumOfRepeatsSet+1),
												   KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;
				}
			else
				{
				StopTest(); // success
				break;	
				}
			}	
		default:
				STOP_TEST_GEN_ERROR;
				break;
			}
		}
//============================================================================================================

CAsyncTestMmfAclntAudioRepeatForeverPlay* CAsyncTestMmfAclntAudioRepeatForeverPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTimeIntervalMicroSeconds aTrailingSilence)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatForeverPlay( aTestName, aSectName, aKeyName, aTrailingSilence);
	}

CAsyncTestMmfAclntAudioRepeatForeverPlay::CAsyncTestMmfAclntAudioRepeatForeverPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TTimeIntervalMicroSeconds aTrailingSilence)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	iTrailingSilence = aTrailingSilence;
	}

void CAsyncTestMmfAclntAudioRepeatForeverPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio RepeatForever/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats (KMdaRepeatForever) = %d\n"),KMdaRepeatForever);
			INFO_PRINTF2( _L("Trailing silence = %ld\n"),iTrailingSilence.Int64());
			iPlayer->SetRepeats(KMdaRepeatForever,iTrailingSilence);
			
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				if(iDuration)
					{
					iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
					iTrigger->StartTimer(iDuration * 6); // Wait till complettion of 5 repeats then issue stop	
					}
				else
					{
					iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
					iTrigger->StartTimer(KOneSecond * 40); // Wait for 40 sec of playback and then issue stop
					}
				iTestState = EStateStop;
				}
			break;
			}
		case EStateStop:
			{
			INFO_PRINTF1( _L("Stop CMdaAudioPlayerUtility"));
			iPlayer->Stop();
			iPlayerState = EStopped;
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000); // Call Pause for this test case after two sec
			iTestState = EStateStopTest;
			break;
			}
		case EStateStopTest:
			{
			StopTest(); // success
			break;	
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
	
	
//============================================================================================================

CAsyncTestMmfAclntAudioRepeatPlayGetPosition* CAsyncTestMmfAclntAudioRepeatPlayGetPosition::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioRepeatPlayGetPosition( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioRepeatPlayGetPosition::CAsyncTestMmfAclntAudioRepeatPlayGetPosition( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	
	}

void CAsyncTestMmfAclntAudioRepeatPlayGetPosition::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio Repeat/Play/GetPosition"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call GetPosition (during Playback of Second repeat) for every 2 sec
				iTestState = EStateGetPosition;
				}
			break;
			}
		case EStateGetPosition:
			{
			INFO_PRINTF1( _L("GetPosition CMdaAudioPlayerUtility"));
			TInt err = iPlayer->GetPosition(iPosition);
			if(err == KErrNone && iPosition <= iDuration * (iNumOfRepeatsSet+1) )
				{
				INFO_PRINTF2(_L("GetPosition = %d"),I64INT(iPosition.Int64()));
				INFO_PRINTF2(_L("TotalClip Duration Including Repeats = %d"),iDuration * (iNumOfRepeatsSet+1));
				
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call GetPosition (during Playback of Second repeat) for every 2 sec
				iTestState = EStateGetPosition;
				
				break;	
				}
			else
				{
				StopTest(err, EFail);
				break;	
				}			
			}
		case EStateStopTest:
			{
			StopTest(); // success
			break;	
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}	
	
//============================================================================================================


CAsyncTestMmfAclntDRMRepeatPlay* CAsyncTestMmfAclntDRMRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId, const TInt aRepeatCount)
	{
	return new (ELeave) CAsyncTestMmfAclntDRMRepeatPlay( aTestName, aSectName, aKeyName, aUniqueId, aRepeatCount);
	}

CAsyncTestMmfAclntDRMRepeatPlay::CAsyncTestMmfAclntDRMRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TDesC& aUniqueId,const TInt aRepeatCount)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	 iUniqueId = aUniqueId;
	 iNumOfRepeatsSet = aRepeatCount;
	}
	
void CAsyncTestMmfAclntDRMRepeatPlay::KickoffTestL()
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Player Util"));
	TPtrC			filename1; 
	TInt 			repeatCount;
	TInt			silence;
	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		ERR_PRINTF1(_L("GetStringFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("numOfRepeat"), repeatCount))
		{
		ERR_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	if(!GetIntFromConfig(_L("SectionSilence"), _L("silence"), silence))
		{
		ERR_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	iSilence = silence;
	iNumOfRepeatsSet = 	repeatCount;
	
	GetDriveName(iFileName);
	iFileName.Append(filename1);
	INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &iFileName);
	iTrigger = CFsmTrigger::NewL(*this);
		
	INFO_PRINTF1( _L("TestPlayerUtils : DRM Audio  Repeat/Play"));
	iPlayer = CMdaAudioPlayerUtility::NewL(*this);
	iPlayer->OpenFileL(TMMFileSource(iFileName, iUniqueId, ContentAccess::EPlay));
	iPlayerState = EOpening;
	}

void CAsyncTestMmfAclntDRMRepeatPlay::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iClipDuration = aDuration;
	if (aError == KErrNone)
		{
		INFO_PRINTF2(_L("*MapcInitComplete* Error: %d."), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateStopTest;
		}
	else if(aError == KErrCANoPermission)
		{
		INFO_PRINTF2(_L("*MapcInitComplete* Error: %d Rights expired"), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateStopTest;	
		}
	else
		{
		StopTest(aError, EFail);	
		}
	}
void CAsyncTestMmfAclntDRMRepeatPlay::MapcPlayComplete(TInt aError)
	{
	if(aError == KErrNone )
		{
		INFO_PRINTF2(_L("*MapcPlayComplete* Error: %d"), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateStopTest;
		}
	else if(aError == KErrCANoPermission)
		{
		INFO_PRINTF2(_L("*MapcPlayComplete* Error: %d Rights expired"), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateStopTest;	
		}
	else
		{
		StopTest(aError, EFail);
		}	
	}
void CAsyncTestMmfAclntDRMRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				}
			break;
			}
		case EStateStopTest:
			{
			StopTest(); // success
			break;	
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}




//=============================================================================================================
//RAsync Negative tests
//==============================================================================================================



CAsyncTestMmfAclntAudioNegativeRepeatPlay* CAsyncTestMmfAclntAudioNegativeRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioNegativeRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioNegativeRepeatPlay::CAsyncTestMmfAclntAudioNegativeRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioNegativeRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio  NegativeRepeat/Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),KNegativeRepeat);
			iPlayer->SetRepeats(KNegativeRepeat,iSilence);
		
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64());
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration);
				if(TimeComparison(iActualDuration, iDuration,
												   KExpectedDeviation))
					{
					StopTest(); // success
					break;	
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					break;	
					}						
				}
			else
				{
				StopTest();
				break;
				}
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}
	
//============================================================================================================


CAsyncTestMmfAclntAudioPlayNegativeRepeat* CAsyncTestMmfAclntAudioPlayNegativeRepeat::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioPlayNegativeRepeat( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioPlayNegativeRepeat::CAsyncTestMmfAclntAudioPlayNegativeRepeat( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioPlayNegativeRepeat::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : Audio  Play/NegativeRepeat"));
			iDuration = I64INT(iPlayer->Duration().Int64());
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
			iPlayer->Play();	
			iPlayerState = EPlaying;
			iStart.HomeTime();
				
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),KNegativeRepeat);
			iPlayer->SetRepeats(KNegativeRepeat,iSilence);

			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Expected Duration = %d "),iDuration);
				if(TimeComparison(iActualDuration, iDuration,
												   KExpectedDeviation))
					{
					StopTest(); // success
					break;	
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					break;	
					}					
				}
			else
				{
				StopTest();
				break;
				}
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}

//============================================================================================================

CAsyncTestMmfAclntRepeatMultipleFiles* CAsyncTestMmfAclntRepeatMultipleFiles::NewL( const TDesC& aTestName, const TDesC& aSectName)
	{
	return new (ELeave) CAsyncTestMmfAclntRepeatMultipleFiles( aTestName, aSectName);
	}

CAsyncTestMmfAclntRepeatMultipleFiles::CAsyncTestMmfAclntRepeatMultipleFiles( const TDesC& aTestName, const TDesC& aSectName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, KNullDesC), iCount(0)
	{
	}
void CAsyncTestMmfAclntRepeatMultipleFiles::KickoffTestL()
	{
	INFO_PRINTF1( _L("TestPlayerUtils : Audio Player Util"));
	TInt 		repeatCount;
	
	if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("numOfRepeat"), repeatCount))
		{
		ERR_PRINTF1(_L("GetIntFromConfig Failed"));
		StopTest(KErrGeneral, EInconclusive);
		return;
		}
	iTrigger = CFsmTrigger::NewL(*this);
	iPlayer = CMdaAudioPlayerUtility::NewL(*this);	
	iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
	iTrigger->StartTimer(500000);
	}
void CAsyncTestMmfAclntRepeatMultipleFiles::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpen:
			{
			INFO_PRINTF1( _L("TestPlayerUtils :Audio  Repeat/Play for multiple files"));
			TPtrC fileName;
			iFileName = KTestParameters[iCount].iFilename;
			if(!GetStringFromConfig(iSectName, iFileName, fileName))
				{
				//return EInconclusive;
				ERR_PRINTF1(_L("GetStringFromConfig Failed"));
				StopTest(KErrGeneral, EInconclusive);
				return;
				}
			
			GetDriveName(iFileName);
			iFileName.Append(fileName);
			INFO_PRINTF2(_L("CMdaAudioPlayerUtility::NewFilePlayerL() - %S"), &iFileName);
			iPlayer->OpenFileL(iFileName);
			iPlayerState = EOpening;
			break;
			}
		case EStateOpened:
			{
			iDuration = I64INT(iPlayer->Duration().Int64());
			iPlayer->SetRepeats(KTestParameters[iCount].iRepeatCount, TTimeIntervalMicroSeconds(KTestParameters[iCount].iSilence));
			iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
			iTrigger->StartTimer(500000);
			iTestState = EStatePlay;
			break;
			}
		case EStatePlay:
			{
			if(iPlayerState == EOpened)
				{
				INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
				iPlayer->Play();	
				iPlayerState = EPlaying;
				}
			break;
			}
		case EStateStopTest:
			{
			if(iCount == KNoOfTestFiles-1)
				{
				StopTest(); // success
				}
			else
				{
				iCount++;
				iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
				iTrigger->StartTimer(KOneSecond); // Call SetRepeat for this test case after two sec
				iTestState = EStateOpen;
				}
			break;
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}

// PDEF131534 SPPR_PERF: Music player crashes when opening corrupted wav file.
// WAV file of 96000 Hz is not supported.
void CAsyncTestMmfAclntRepeatMultipleFiles::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iClipDuration = aDuration;
	if (aError != KErrNone)
		{
		TInt match = iFileName.Match(_L("*100khzSRate1KhzTone.wav"));
		if (aError == KErrNotSupported && match != KErrNotFound)
			{
			INFO_PRINTF1( _L("WAV file of frequency more than 96 KHz is not supported."));
			INFO_PRINTF1( _L("*MapcInitComplete* Expected Error: KErrNotSupported(-5)"));
			INFO_PRINTF2(_L("*MapcInitComplete* Actual Error: %d."), aError);
			StopTest(aError, EPass);
			}
		else
			{
			ERR_PRINTF2(_L("*MapcInitComplete* Error: %d."), aError);
			StopTest(aError, EFail);
			}
		}
	else
		{
		INFO_PRINTF2(_L("*MapcInitComplete* Error: %d."), aError);
		iTrigger->Cancel(); // ensure assert in StartTimer always succeeds
		iTrigger->StartTimer(500000); 
		iTestState = EStateOpened;
		iPlayerState = EOpened;	
		}	
	}
//============================================================================================================


CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay* CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay::NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	return new (ELeave) CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay( aTestName, aSectName, aKeyName);
	}

CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay::CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CAsyncTestMmfAclntAudioPlayerUtil( aTestName, aSectName, aKeyName)
	{
	}

void CAsyncTestMmfAclntAudioSetPlayWindowRepeatPlay::FsmL()
	{
	switch (iTestState)
		{
		case EStateOpened:
			{
			INFO_PRINTF1( _L("TestPlayerUtils : SetPlayWindow Repeat Play"));
			iDuration = I64INT(iPlayer->Duration().Int64());			
			
			TInt64 temp1, temp2;
			temp1 = iPlayer->Duration().Int64()/2;
			temp2 = iPlayer->Duration().Int64();
			iWindowDuration = (I64INT(temp2) - I64INT(temp1));

			TTimeIntervalMicroSeconds startTime(temp1);
			TTimeIntervalMicroSeconds endTime(temp2); 

			INFO_PRINTF4(_L("StartTime = %d EndTime = %d WindowDuration = %d"),
				I64INT(startTime.Int64()), I64INT(endTime.Int64()), iWindowDuration);
			
			TInt windowSet = iPlayer->SetPlayWindow(startTime,endTime);
			if (windowSet != KErrNone)
				{
				ERR_PRINTF2( _L("Unexpected error returned from SetPlayWindow - %d"),windowSet);
				StopTest(windowSet, EFail);
				break;
				}
			
			INFO_PRINTF1( _L("SetRepeats CMdaAudioPlayerUtility"));
			INFO_PRINTF2( _L("Repeats = %d\n"),iNumOfRepeatsSet);
			iPlayer->SetRepeats(iNumOfRepeatsSet,iSilence);
			
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility"));
		
			if(iPlayerState == EOpened)
				{
				iPlayer->Play();	
				iPlayerState = EPlaying;
				iStart.HomeTime();
				}
			break;
			}
		case EStateStopTest:
			{
			if(iDuration)
				{
				iStop.HomeTime();
				iActualDuration = I64INT(iStop.MicroSecondsFrom(iStart).Int64()); 
				INFO_PRINTF2( _L("Clip Duration = %d "),iDuration);
				INFO_PRINTF2( _L("Actual Duration = %u "),iActualDuration);
				INFO_PRINTF2( _L("Window Duration = %d "),iWindowDuration);
				#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
				if(TimeComparison(iActualDuration, iWindowDuration*(iNumOfRepeatsSet+1),
												   KExpectedDeviation))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#else
				if(TimeComparison(iActualDuration, iWindowDuration*(iNumOfRepeatsSet+1),
												   KExpectedDeviation*iNumOfRepeatsSet))
					{
					StopTest(); // success
					}
				else
					{
					STOP_TEST_GEN_ERROR;
					}
				#endif
				break;			
				}
			else
				{
				StopTest();
				break;
				}
			}			
		default:
				STOP_TEST_GEN_ERROR;
				break;
		}
	}	
