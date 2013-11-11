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

#include "CapTestStep0004.h"

CSecDevSndTS0004* CSecDevSndTS0004::NewL()
	{
	CSecDevSndTS0004* self = new (ELeave) CSecDevSndTS0004;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSecDevSndTS0004::ConstructL()
	{
	iIsFirstPlayed = EFalse;
	}
	
void CSecDevSndTS0004::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;

	if( (iVerdict = DoTestStepPreambleL() ) == EPass )
		{
		iVerdict = DoPlayDTMFTone();
		}
	}

CSecDevSndTS0004::~CSecDevSndTS0004()
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

TVerdict CSecDevSndTS0004::DoPlayDTMFTone()
	{
	_LIT(KDTMFStringShort,"0");
	_LIT(KDTMFString,"11111, 11111, 11111111");

	//Initialize
	TVerdict initOK = TestInitialize(EMMFStateTonePlaying);
	if (initOK != EPass)
		{
		return EInconclusive;
		}

	TestSetPriority(KDevSoundPriorityHigh);
	TestSetVolume(iMMFDevSound->MaxVolume());

	iExpectedValue = KErrUnderflow;
	TestPlayDTMFString(KDTMFStringShort);
	iExpectedValue = KErrInUse;
	return TestPlayDTMFString(KDTMFString);
	}

/**
 *
 * TestPlayDTMFString
 * @param aDTMFString
 * @result TVerdict
 *
 */
TVerdict CSecDevSndTS0004::TestPlayDTMFString(const TDesC &aDTMFString)
	{
	iCallbackError = KErrNone;

	ResetCallbacks();

	TRAPD(err, iMMFDevSound->PlayDTMFStringL(aDTMFString));
	if (err)
		{
		iCallbackError = err;
		return EFail;
		}
	else
		{
		// Start the active scheduler and catch the callback
 		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			return EFail;
			}
		if (iCallbackArray[EToneFinished] != 1)
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

/******************************************************************************
 *
 * DevSound mixin methods
 *
 *****************************************************************************/
 
/**
 *
 * ToneFinished
 * @param aError
 *
 */
void CSecDevSndTS0004::ToneFinished (TInt aError)
	{
	if( !iIsFirstPlayed )
		{
		// after first tone played inform client so we can be interrupted during second play
		iIsFirstPlayed = ETrue;
		User::RequestComplete(iStatus, aError);
		}
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	CActiveScheduler::Stop();
	}
