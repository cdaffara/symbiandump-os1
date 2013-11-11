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
// Integration tests.
// This program is designed the test of the MMF_ACLNT.
// 
//

/**
 @file TestPlayerFileSource.cpp
*/

#include "TestPlayerUtils.h"
#include "TestPlayerFileSource.h"

//
// CTestMmfAclntFileSource
//

/**
 * Constructor
 */
CTestMmfAclntFileSource::CTestMmfAclntFileSource(const TDesC& aTestName,const TDesC& aSectName,
									 const TDesC& aKeyName,const TBool aPlay, 
									 const TInt aExpectedError)
	:iPlay (aPlay)
	,iExpectedError (aExpectedError)
	{
	// store the name of this test case
	iTestStepName = aTestName;
	// store the section-name of the ini file
	iSectName = aSectName;
	// store the key-name of the ini file
	iKeyName = aKeyName;
	}

/**
 * NewL
 */
CTestMmfAclntFileSource* CTestMmfAclntFileSource::NewL(const TDesC& aTestName, const TDesC& aSectName,
										   const TDesC& aKeyName,const TBool aPlay, 
										   const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntFileSource* self = new (ELeave) CTestMmfAclntFileSource(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

/**
 * NewLC
 */
CTestMmfAclntFileSource* CTestMmfAclntFileSource::NewLC(const TDesC& aTestName, const TDesC& aSectName,
											const TDesC& aKeyName,
											const TBool aPlay,  
											const TInt aExpectedError)
	{
	CTestMmfAclntFileSource* self = CTestMmfAclntFileSource::NewL(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

/**
 * MapcInitComplete
 */
void CTestMmfAclntFileSource::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

/**
 * MapcPlayComplete
 */
void CTestMmfAclntFileSource::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfAclntFileSource::DoTestStepL( void )
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
	
	// Create CMdaAudioPlayerUtility Object
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewL(*this);
	
	// Create TMMFileSource Object
	TMMFileSource filesource(filename);
	player->OpenFileL(filesource);
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();
	
	// Check for expected errors.
	if(iError != KErrNone && (iExpectedError == iError))
		{
		ret = EPass;	// all other tests pass
		delete player;
		User::After(KOneSecond); // wait for deletion to shut down devsound
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with expected error %d"),iError );
		return ret;
		}

	// Check for errors (after OPEN).
	if (iError == KErrNone && player != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			player->Play();
			
			// Wait for play complete callback
			INFO_PRINTF1(_L("Play CMdaAudioPlayerUtility"));
			CActiveScheduler::Start();
			
			// Check for Callback errors
			if(iError == KErrNone)
				{
				ret = EPass;
				}
			}
		else
			{
			ret = EPass;
			}
		}
	
	// Clean up activities.
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound

	// Check for errors (final check).
	if(iError != KErrNone)
		{
		ERR_PRINTF2( _L("CMdaAudioPlayerUtility failed with error %d"),iError );
		}
	return	ret;
	}

//
// CTestMmfAclntFileHandleSource
//

/**
 * Constructor
 */
CTestMmfAclntFileHandleSource::CTestMmfAclntFileHandleSource(const TDesC& aTestName,const TDesC& aSectName,
									 const TDesC& aKeyName,const TBool aPlay, 
									 const TInt aExpectedError)
	:iPlay (aPlay)
	,iExpectedError (aExpectedError)
	{
	// store the name of this test case
	iTestStepName = aTestName;
	// store the section-name of the ini file
	iSectName = aSectName;
	// store the key-name of the ini file
	iKeyName = aKeyName;
	}

/**
 * NewL
 */
CTestMmfAclntFileHandleSource* CTestMmfAclntFileHandleSource::NewL(const TDesC& aTestName, const TDesC& aSectName,
										   const TDesC& aKeyName,const TBool aPlay, 
										   const TInt aExpectedError = KErrNone)
	{
	CTestMmfAclntFileHandleSource* self = new (ELeave) CTestMmfAclntFileHandleSource(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	return self;
	}

/**
 * NewLC
 */
CTestMmfAclntFileHandleSource* CTestMmfAclntFileHandleSource::NewLC(const TDesC& aTestName, const TDesC& aSectName,
											const TDesC& aKeyName,
											const TBool aPlay,  
											const TInt aExpectedError)
	{
	CTestMmfAclntFileHandleSource* self = CTestMmfAclntFileHandleSource::NewL(aTestName,aSectName,aKeyName,aPlay,aExpectedError);
	CleanupStack::PushL(self);
	return self;
	}

/**
 * MapcInitComplete
 */
void CTestMmfAclntFileHandleSource::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

/**
 * MapcPlayComplete
 */
void CTestMmfAclntFileHandleSource::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfAclntFileHandleSource::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;
	
	// Get the file name.
	TBuf<KSizeBuf> filename;
	TPtrC filename1;
	if(!GetStringFromConfig(iSectName,iKeyName,filename1))
		{
		return EInconclusive;
		}
	GetDriveName(filename);
	filename.Append(filename1);
	
	// Create CMdaAudioPlayerUtility Object
	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewL(*this);
	
	// Create RFs and RFile Objects
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());
	
	RFile file;
	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );
	CleanupClosePushL(file);
	
	// Create TMMFileSource Object
	TMMFileHandleSource filehandlesource(file);
	player->OpenFileL(filehandlesource);
	
	// Wait for initialisation callback
	INFO_PRINTF1(_L("CMdaAudioPlayerUtility->OpenFileL(TMMFileHandleSource)"));
	CActiveScheduler::Start();
	
	// Check for expected errors.
	if((iError != KErrNone) && (iExpectedError == iError))
		{
		ERR_PRINTF2(_L("CMdaAudioPlayerUtility->OpenFileL() Returned the Expected Error : %d"),iError);
		ret=EPass;
		}
	
	// Check for errors.
	if(iError != KErrNone)
		{
		ERR_PRINTF2(_L("CMdaAudioPlayerUtility->OpenFileL() Failed with Error : %d"),iError);
		ret=EFail;
		}
	
	// Check for No errors, so as to start Playback
	if (iError == KErrNone && player != NULL)
		{
		if(iPlay)
			{
			iError = KErrTimedOut;
			player->Play();
			
			// Wait for play complete callback
			INFO_PRINTF1(_L("CMdaAudioPlayerUtility->Play()"));
			CActiveScheduler::Start();
			
			// Check for Callback errors
			if(iError == KErrNone)
				{
				ERR_PRINTF2(_L("CMdaAudioPlayerUtility->Play() completed successfully with return code : %d"),iError);
				ret = EPass;
				}
			else
				{
				ERR_PRINTF2(_L("CMdaAudioPlayerUtility->Play() Failed with Error : %d"),iError );
				ret = EFail;
				}
			}
		else
			{
			ret = EPass;
			}
		}
	
	// Clean up activities.
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	CleanupStack::PopAndDestroy(2);

	return	ret;
	}
