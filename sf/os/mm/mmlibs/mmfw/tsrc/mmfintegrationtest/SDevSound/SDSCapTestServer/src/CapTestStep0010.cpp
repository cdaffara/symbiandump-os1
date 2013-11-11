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
// CapTestStep0010.h
// 
//

#include "CapTestStep0010.h"

CSecDevSndTS0010* CSecDevSndTS0010::NewL()
	{
	CSecDevSndTS0010* self = new (ELeave) CSecDevSndTS0010;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSecDevSndTS0010::ConstructL()
	{
	}
	

void CSecDevSndTS0010::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	if ( (iVerdict = DoTestStepPreambleL() ) == EPass)
		{
		// only do this when DoTestStepPreambleL() pass, if it fails, it means 
		// devsound is not ready for play data yet.
		iVerdict = DoRecordData();
		}
	}
		
CSecDevSndTS0010::~CSecDevSndTS0010()
	{
	}

/******************************************************************************
 *
 * DevSound methods
 *
 *****************************************************************************/
 
/**
 *
 * DoTestStepL
 * @result TVerdict
 *
 */

TVerdict CSecDevSndTS0010::DoRecordData()
	{
	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateRecording);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityHigh);

	initOK = TestRecordInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	User::RequestComplete(iStatus, KErrNone);

	TestSetVolume(iMMFDevSound->MaxVolume() / 2);

	TFileName	aFilename = _L("C:\\rectest1.wav");

	return RecordDataFile(aFilename);
	}

/**
 *
 * TestDigitalPlayback
 * @param aNumSamples
 * @param aFilename
 * @param aDataType
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0010::RecordDataFile(TDesC& aFilename)
	{
	TInt		initOK = KErrNone;
	TInt		aNumSamples = 1;

	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		// Could not connect to Filesystem.
		return EInconclusive;
		}

	//Create output directory if it doesn't already exist
//	fs.MkDir(_L("C:\\TSU_MMF_DEVSOUND_SUITE\\Output\\"));

	err = file.Replace(fs, aFilename, EFileWrite);
	if (err != KErrNone)
		{
		file.Close();
		fs.Close();
		return EInconclusive;
		}

	//SetGain
	TInt vol = iMMFDevSound->MaxVolume();	// Hacked... Fix this...
	TestSetGain(vol);

	TInt bufferCount = 0;
	while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
		{
		//DevSound Record
		initOK = TestRecordData();

		//Write buffer to file
		CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
		file.Write(buffer->Data());

		bufferCount ++;
		}

	file.Close();
	fs.Close();

	if (initOK != KErrNone)
		{
		return EFail;
		}

	if (aNumSamples >= 0 && bufferCount != aNumSamples)
		{
		return EFail;
		}

	return EPass;
	}

/**
 *
 * TestInitialize
 * @param aDataType
 * @param aMode
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0010::TestInitialize(TMMFState aMode)
	{
	TFourCC pcm16(KMMFFourCCCodePCM16); //default to pcm16 data type

	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;

	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, pcm16, aMode));
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestRecordInit
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0010::TestRecordInit()
	{
	ResetCallbacks();

	//get buffer from devsound
	TRAPD(err, iMMFDevSound->RecordInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToEmpty] != 1)
			{
			return EFail;
			}
		TInt tot = GetCallbackTotal();
		if (tot > 1)
			{
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * TestRecordData
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0010::TestRecordData()
	{
	ResetCallbacks();

	iMMFDevSound->RecordData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToEmpty] != 1)
		{
		if (iCallbackArray[EBuffToEmpty] == 0 && iCallbackArray[ERecError] == 1)
			{
			// DevSound RecordError was called 1 time.
			}
		else
			{
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 1)
		{
		return EFail;
		}
	return EPass;
	}

/******************************************************************************
 *
 * DevSound mixin methods
 *
 *****************************************************************************/
 
/**
 * 
 * BufferToBeEmptied
 * @param aBuffer
 *
 */
void CSecDevSndTS0010::BufferToBeEmptied (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (aBuffer != NULL)
		{
		iCallbackError = KErrNone;
		}
	else
		{
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToEmpty] ++;
	CActiveScheduler::Stop();
	}

/**
 *
 * RecordError
 * @param aError
 *
 */
void CSecDevSndTS0010::RecordError (TInt aError)
	{
	if( aError != KErrInUse )
		{
		// should get interrupted by client
		iVerdict = EFail;
		}
	iCallbackError = aError;
	iCallbackArray[ERecError] ++;
	CActiveScheduler::Stop();
	}
