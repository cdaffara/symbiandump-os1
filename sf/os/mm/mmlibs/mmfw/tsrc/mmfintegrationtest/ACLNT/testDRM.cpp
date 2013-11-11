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
//

#include "rightsdatabase.h"
#include "TestDRM.h"
#include <caf/caf.h>

const TInt KDefaultRightsID = 1;
const TInt KDefaultRightsCount = 2;

const TInt KMaxMimeLength = 256;
/**
 * Constructor
 */
CTestMmfAclntDRMPlayerNoRights::CTestMmfAclntDRMPlayerNoRights()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9000-LP");

	}

CTestMmfAclntDRMPlayerNoRights* CTestMmfAclntDRMPlayerNoRights::NewL()
	{
	CTestMmfAclntDRMPlayerNoRights* self = new (ELeave) CTestMmfAclntDRMPlayerNoRights();
	return self;
	}

CTestMmfAclntDRMPlayerNoRights* CTestMmfAclntDRMPlayerNoRights::NewLC()
	{
	CTestMmfAclntDRMPlayerNoRights* self = CTestMmfAclntDRMPlayerNoRights::NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntDRMPlayerNoRights::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntDRMPlayerNoRights::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}



/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMPlayerNoRights::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	delete rights; // the database will now be reset and have no entries in it

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);


	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	if (iError == KErrCANoRights)
		ret = EPass;			
	else
		ret = EFail;
	
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}
//--------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMPlayerRightsCount::CTestMmfAclntDRMPlayerRightsCount()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9001-LP");

	}

CTestMmfAclntDRMPlayerRightsCount* CTestMmfAclntDRMPlayerRightsCount::NewL()
	{
	CTestMmfAclntDRMPlayerRightsCount* self = new (ELeave) CTestMmfAclntDRMPlayerRightsCount();
	return self;
	}

CTestMmfAclntDRMPlayerRightsCount* CTestMmfAclntDRMPlayerRightsCount::NewLC()
	{
	CTestMmfAclntDRMPlayerRightsCount* self = CTestMmfAclntDRMPlayerRightsCount::NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntDRMPlayerRightsCount::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntDRMPlayerRightsCount::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}



/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMPlayerRightsCount::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioPlayerUtility* player = NULL;
	player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this);


	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
	CActiveScheduler::Start();

	// Check for errors.
	if (iError == KErrNone && player != NULL)
		{
		iError = KErrTimedOut;

		for (TInt i=0;i<KDefaultRightsCount;i++)
			{
			player->Play();
			
			// Wait for play callback
			INFO_PRINTF2( _L("Play CMdaAudioPlayerUtility: DRM play number %d"),i);
			CActiveScheduler::Start();

			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Unexpected error %d"),iError);
				break;
				}
			}

		if (iError == KErrNone)
			{
			player->Play();
			
			// Wait for play callback
			INFO_PRINTF1( _L("Play CMdaAudioPlayerUtility with expired rights"));
			CActiveScheduler::Start();

			if (iError != KErrCANoPermission)
				{
				INFO_PRINTF3(_L("Unexpected error %d, expected %d"),iError,KErrCANoPermission);
				}
			else
				{
				INFO_PRINTF1(_L("Expected failure: rights denied"));
				ret = EPass;
				}

			}
		}
	
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}

//----------------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMRecorderNoRights::CTestMmfAclntDRMRecorderNoRights()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9002-LP");

	}

CTestMmfAclntDRMRecorderNoRights* CTestMmfAclntDRMRecorderNoRights::NewL()
	{
	CTestMmfAclntDRMRecorderNoRights* self = new (ELeave) CTestMmfAclntDRMRecorderNoRights();
	return self;
	}

CTestMmfAclntDRMRecorderNoRights* CTestMmfAclntDRMRecorderNoRights::NewLC()
	{
	CTestMmfAclntDRMRecorderNoRights* self = CTestMmfAclntDRMRecorderNoRights::NewL();
	CleanupStack::PushL(self);
	return self;
	}


/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMRecorderNoRights::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorderUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	delete rights; // the database will now be reset and have no entries in it

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioRecorderUtility* recorder = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recorder);

	recorder->OpenFileL(filename);

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	if (iError == KErrCANoRights)
		ret = EPass;			
	else
		ret = EFail;
	
	CleanupStack::PopAndDestroy(recorder);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}


void CTestMmfAclntDRMRecorderNoRights::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;

	INFO_PRINTF1( _L("CTestMmfAclntRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	CActiveScheduler::Stop();
	}

//--------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMRecorderRightsCount::CTestMmfAclntDRMRecorderRightsCount()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9003-LP");

	}

CTestMmfAclntDRMRecorderRightsCount* CTestMmfAclntDRMRecorderRightsCount::NewL()
	{
	CTestMmfAclntDRMRecorderRightsCount* self = new (ELeave) CTestMmfAclntDRMRecorderRightsCount();
	return self;
	}

CTestMmfAclntDRMRecorderRightsCount* CTestMmfAclntDRMRecorderRightsCount::NewLC()
	{
	CTestMmfAclntDRMRecorderRightsCount* self = CTestMmfAclntDRMRecorderRightsCount::NewL();
	CleanupStack::PushL(self);
	return self;
	}

/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMRecorderRightsCount::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorderUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioRecorderUtility* recorder = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recorder);

	recorder->OpenFileL(filename);

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	// Check for errors.
	if (iError == KErrNone && recorder != NULL)
		{
		iError = KErrTimedOut;

		for (TInt i=0;i<KDefaultRightsCount;i++)
			{
			recorder->PlayL();
			
			// Wait for play callback
			INFO_PRINTF2( _L("Play CMdaAudioRecorderUtility: DRM play number %d"),i);
			CActiveScheduler::Start();

			if (iError != KErrNone)
				{
				INFO_PRINTF2(_L("Unexpected error %d"),iError);
				break;
				}
			}

		if (iError == KErrNone)
			{
			recorder->PlayL();
			
			// Wait for play callback
			INFO_PRINTF1( _L("Play CMdaAudioRecorderUtility with expired rights"));
			CActiveScheduler::Start();

			if (iError != KErrCANoPermission)
				{
				INFO_PRINTF3(_L("Unexpected error %d, expected %d"),iError,KErrCANoPermission);
				}
			else
				{
				INFO_PRINTF1(_L("Expected failure: rights denied"));
				ret = EPass;
				}

			}
		}
	
	CleanupStack::PopAndDestroy(recorder);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}




void CTestMmfAclntDRMRecorderRightsCount::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;

	TBool busy = (iCurrentState == CMdaAudioClipUtility::ERecording || iCurrentState == CMdaAudioClipUtility::EPlaying);
	if (!busy || iError!=KErrNone)
		CActiveScheduler::Stop();
	
	INFO_PRINTF1( _L("CTestMmfAclntRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	
	}

//--------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMPlayerOpen::CTestMmfAclntDRMPlayerOpen()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9004-LP");

	}

CTestMmfAclntDRMPlayerOpen* CTestMmfAclntDRMPlayerOpen::NewL()
	{
	CTestMmfAclntDRMPlayerOpen* self = new (ELeave) CTestMmfAclntDRMPlayerOpen();
	return self;
	}

CTestMmfAclntDRMPlayerOpen* CTestMmfAclntDRMPlayerOpen::NewLC()
	{
	CTestMmfAclntDRMPlayerOpen* self = CTestMmfAclntDRMPlayerOpen::NewL();
	CleanupStack::PushL(self);
	return self;
	}

void CTestMmfAclntDRMPlayerOpen::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}

void CTestMmfAclntDRMPlayerOpen::MapcPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Play Complete"));
	CActiveScheduler::Stop();
	}

/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMPlayerOpen::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestPlayerUtils : File"));
	TVerdict ret = EFail;
	TInt err=KErrNone;
	iError = KErrTimedOut;
	
	//fectch MIME type from config file
	TBuf8<KMaxMimeLength> mimeType;
	TPtrC				  mimeTypePtr; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavMime"),mimeTypePtr))
		return EInconclusive;
	mimeType.Copy(mimeTypePtr);


	//setup file to open
	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount,mimeType); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object

	//create instance of Audio Player, which should try open the file as well
	CMdaAudioPlayerUtility* player = NULL;
	TRAP(err, player = CMdaAudioPlayerUtility::NewFilePlayerL(filename, *this));
	
	if (iError == KErrNone)//call back OK
	{
		// Wait for initialisation callback
		INFO_PRINTF1( _L("Initialise CMdaAudioPlayerUtility"));
		CActiveScheduler::Start();
	}
	else
	{
		INFO_PRINTF2(_L("Unexpected error %d"),iError);
	}


	if (err != KErrNone)
		{
			INFO_PRINTF3(_L("Unexpected error %d, expected %d"),iError,KErrCANoPermission);
		}
	else
		{
			INFO_PRINTF1(_L("Expected: PASS"));
			ret = EPass;
		}
		
	delete player;
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}

//--------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMRecorderPlay::CTestMmfAclntDRMRecorderPlay()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9005-LP");

	}

CTestMmfAclntDRMRecorderPlay* CTestMmfAclntDRMRecorderPlay::NewL()
	{
	CTestMmfAclntDRMRecorderPlay* self = new (ELeave) CTestMmfAclntDRMRecorderPlay();
	return self;
	}

CTestMmfAclntDRMRecorderPlay* CTestMmfAclntDRMRecorderPlay::NewLC()
	{
	CTestMmfAclntDRMRecorderPlay* self = CTestMmfAclntDRMRecorderPlay::NewL();
	CleanupStack::PushL(self);
	return self;
	}

/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMRecorderPlay::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestRecorderUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	//fectch MIME type from config file
	TBuf8<KMaxMimeLength> mimeType;
	TPtrC				  mimeTypePtr; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavMime"),mimeTypePtr))
		return EInconclusive;
	mimeType.Copy(mimeTypePtr);

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount,mimeType); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object


	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);

	CMdaAudioRecorderUtility* recorder = CMdaAudioRecorderUtility::NewL(*this);
	CleanupStack::PushL(recorder);

	recorder->OpenFileL(filename);

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioRecorderUtility"));
	CActiveScheduler::Start();

	// Check for errors.
	if (iError == KErrNone && recorder != NULL)
		{
		iError = KErrTimedOut;

		recorder->PlayL();
			
		// Wait for play callback
		INFO_PRINTF1( _L("Play DRM protected file with CMdaAudioRecorderUtility(with sufficient permission)"));
		CActiveScheduler::Start();

		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Unexpected error trying to play protected file %d"),iError);
			}
		else
			{
			INFO_PRINTF1(_L("DRM Protected file played successfully"));
			ret = EPass;
			}
		}
	
	CleanupStack::PopAndDestroy(recorder);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}




void CTestMmfAclntDRMRecorderPlay::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;

	TBool busy = (iCurrentState == CMdaAudioClipUtility::ERecording || iCurrentState == CMdaAudioClipUtility::EPlaying);
	if (!busy || iError!=KErrNone)
		CActiveScheduler::Stop();
	
	INFO_PRINTF1( _L("CTestMmfAclntRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	
	}


//--------------------------------------------------------------------------------

/**
 * Constructor
 */
CTestMmfAclntDRMConverterFail::CTestMmfAclntDRMConverterFail()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACLNT-I-9006-LP");

	}

CTestMmfAclntDRMConverterFail* CTestMmfAclntDRMConverterFail::NewL()
	{
	CTestMmfAclntDRMConverterFail* self = new (ELeave) CTestMmfAclntDRMConverterFail();
	return self;
	}

CTestMmfAclntDRMConverterFail* CTestMmfAclntDRMConverterFail::NewLC()
	{
	CTestMmfAclntDRMConverterFail* self = CTestMmfAclntDRMConverterFail::NewL();
	CleanupStack::PushL(self);
	return self;
	}

/** Load and initialise an audio file.
 */
TVerdict CTestMmfAclntDRMConverterFail::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("TestconverterUtils : File"));
	TVerdict ret = EFail;

	iError = KErrTimedOut;

	//fectch MIME type from config file
	TBuf8<KMaxMimeLength> mimeType;
	TPtrC				  mimeTypePtr; 
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavMime"),mimeTypePtr))
		return EInconclusive;
	mimeType.Copy(mimeTypePtr);

	//setup DRM rights
	CRightsDatabase* rights = CRightsDatabase::NewL(EFalse); // create new database overwriting current one
	CleanupStack::PushL(rights);
	rights->AddRightsL(KDefaultRightsID,KDefaultRightsCount,mimeType); // add rights count 
	CleanupStack::PopAndDestroy(rights); // cleanup rights object


	TBuf<KSizeBuf>	filename;
	TBuf<KSizeBuf>	destFilename;
	TPtrC			filename1; 
	TPtrC			filename2;
	if(!GetStringFromConfig(_L("SectionDRM"),_L("DRMWavFile"),filename1))
		return EInconclusive;
	if(!GetStringFromConfig(_L("SectionDRM"),_L("TestConvertFile"),filename2))
		return EInconclusive;
	GetDriveName(filename);
	filename.Append(filename1);
	GetDriveName(destFilename);
	destFilename.Append(filename2);

	RFs		theFs;
	theFs.Connect();
	theFs.Delete(destFilename);
	theFs.Close();


	iError = KErrTimedOut;
	TMdaFileClipLocation location(destFilename);
	TMdaWavClipFormat	format;
	TMdaPcmWavCodec		codec;

	CMdaAudioConvertUtility* converter = CMdaAudioConvertUtility::NewL(*this);
	CleanupStack::PushL(converter);

	converter->OpenL(filename, &location, &format, &codec);

	// Wait for initialisation callback
	INFO_PRINTF1( _L("Initialise CMdaAudioConvertUtility"));
	CActiveScheduler::Start();

	// Check for errors.
	if (iError == KErrNone && converter != NULL)
		{
		iError = KErrTimedOut;

		converter->ConvertL();
			
		// Wait for convert callback
		INFO_PRINTF1( _L("Convert CMdaAudioconverterUtility: DRM play number"));
		CActiveScheduler::Start();

		if (iError == KErrNotSupported)
			{
			INFO_PRINTF1(_L("Correctly received KErrNotSupported, for converting protected file"));
			ret = EPass;
			}
		else
			{
			INFO_PRINTF3(_L("Unexpected error %d, expecting %d"),iError, KErrNotSupported);
			}
		}
	
	CleanupStack::PopAndDestroy(converter);
	User::After(KOneSecond); // wait for deletion to shut down devsound
	return	ret;
	}




void CTestMmfAclntDRMConverterFail::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iError = aErrorCode;
	//iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;

	TBool busy = (iCurrentState == CMdaAudioClipUtility::ERecording || iCurrentState == CMdaAudioClipUtility::EPlaying);
	if (!busy || iError!=KErrNone)
		CActiveScheduler::Stop();

	INFO_PRINTF1( _L("CTestMmfAclntRecord : MMdaObjectStateChangeObserver Callback for CMdaAudioconverterUtility complete"));
	INFO_PRINTF4( _L("iError %d iPreviousState %d iCurrentState %d"), iError, iPreviousState, iCurrentState);
	
	}

