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

#include "CapTestStep0008.h"

CSecDevSndTS0008* CSecDevSndTS0008::NewL()
	{
	CSecDevSndTS0008* self = new (ELeave) CSecDevSndTS0008;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSecDevSndTS0008::ConstructL()
	{
	iIsFirst = ETrue;
	}
	

void CSecDevSndTS0008::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	if ( (iVerdict = DoTestStepPreambleL() ) == EPass)
		{
		// only do this when DoTestStepPreambleL() pass, if it fails, it means 
		// devsound is not ready for play data yet.
		iVerdict = DoPlayData();
		}
	}
		
CSecDevSndTS0008::~CSecDevSndTS0008()
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

TVerdict CSecDevSndTS0008::DoPlayData()
	{
	//Initialize
	TVerdict initOK = TestInitialize(EMMFStatePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityHigh);

	initOK = TestPlayInit();
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	User::RequestComplete(iStatus, KErrNone);

	TestSetVolume(iMMFDevSound->MaxVolume() / 2);

	_LIT(KTestFileName, "C:\\sdevsoundinttestdata\\mainTst.wav");
	TFileName aFilename(KTestFileName);

	return PlayDataFile(aFilename);
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
TVerdict CSecDevSndTS0008::PlayDataFile(TDesC& aFilename)
	{
	TInt		initOK = KErrNone;
	TInt		aNumSamples = -1;

	//open file
	RFs fs;
	RFile file;
	TInt err = fs.Connect();
	if (err != KErrNone)
		{
		return EInconclusive;
		}

	err = file.Open(fs, aFilename, EFileRead);
	if (err != KErrNone)
		{
		fs.Close();
		return EInconclusive;
		}

	TInt bufferCount = 0;
	if (aNumSamples < 0)
		{// Play to EOF
		while (initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer*  buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}
		}
	else
		{
		while (bufferCount < aNumSamples && initOK == KErrNone && iCallbackError == KErrNone)
			{
			//read sizeof buffer from file
			CMMFDataBuffer* buffer = STATIC_CAST (CMMFDataBuffer*, iBuffer);
			file.Read(buffer->Data());
			if (buffer->Data().Length()!= buffer->RequestSize())
				{
				iBuffer->SetLastBuffer(ETrue);
				}

			//DevSound Play
			initOK = TestPlayData();
			bufferCount ++;
			}
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
TVerdict CSecDevSndTS0008::TestInitialize(TMMFState aMode)
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
 * TestPlayInit
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0008::TestPlayInit()
	{
	ResetCallbacks();

	//get buffer from devsound
	TRAPD(err, iMMFDevSound->PlayInitL());
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();
	if (err)
		{
		return EFail;
		}
	else
		{
		if (iCallbackArray[EBuffToFill] != 1)
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
 * TestPlayData
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0008::TestPlayData()
	{
	ResetCallbacks();

	iMMFDevSound->PlayData();
	// Start the active scheduler and catch the callback
 	CActiveScheduler::Start();

	if (iCallbackArray[EBuffToFill] != 1)
		{
		if (iCallbackArray[EBuffToFill] == 0 && iCallbackArray[EPlayError] == 1)
			{
			// DevSound PlayError was called 1 time. Must be EOF.
			}
		else
			{
			return EFail;
			}
		}

	TInt tot = GetCallbackTotal();
	if (tot > 2)
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
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void CSecDevSndTS0008::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	iBuffer = aBuffer;
	if (aBuffer != NULL) 
		{
		if( iIsFirst )
			{
			iIsFirst = EFalse;
			}
		iCallbackError = KErrNone;
		}
	else 
		{
		iCallbackError = KErrNotFound;
		}
	iCallbackArray[EBuffToFill] ++;
	CActiveScheduler::Stop();
	}

/**
 *
 * PlayError
 * @param aError
 *
 */
void CSecDevSndTS0008::PlayError (TInt aError)
	{
	if( aError != KErrInUse )
		{
		// should get interrupted by client
		iVerdict = EFail;
		}

	iCallbackError = aError;
	iCallbackArray[EPlayError] ++;
	CActiveScheduler::Stop();
	}
