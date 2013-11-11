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

#include "CapTestStep0014.h"

CAudPlayUtilTS0014* CAudPlayUtilTS0014::NewL()
	{
	CAudPlayUtilTS0014* self = new (ELeave) CAudPlayUtilTS0014;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CAudPlayUtilTS0014::ConstructL()
	{
	// Create the audio player
	iInternalState = EStateNone;
	iPlayer = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax);
	}
	

void CAudPlayUtilTS0014::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	iInternalState = EStatePending;

	iPlayer->OpenFileL(_L("c:\\rectest1.wav"));
	CActiveScheduler::Start();
	}
		
		
TVerdict CAudPlayUtilTS0014::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	iPlayer->Close();
	aMessage.Copy(_L("Done"));
	return iVerdict;
//	return EPass;
	}
	
void CAudPlayUtilTS0014::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	if (aErrorCode == KErrNone)
		{
		if (aPreviousState == CMdaAudioClipUtility::ENotReady && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iPlayer->RecordL();
			}
		else if (aPreviousState == CMdaAudioClipUtility::EOpen && aCurrentState==CMdaAudioClipUtility::ERecording)
			{
			if(iInternalState == EStatePending)
				{
				User::RequestComplete(iStatus, KErrNone);
				iInternalState = EStateComplete;
				}
			}
		else if (aPreviousState == CMdaAudioClipUtility::ERecording && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			// this shouldn't happen as record will continue until interrupted
			iVerdict = EPass;
			CActiveScheduler::Stop();
			if(iInternalState == EStatePending)
				{
				User::RequestComplete(iStatus, KErrCancel);
				iInternalState = EStateComplete;
				}
			}
		}
	else if (aErrorCode == KErrInUse)
		{
		// double check state to ensure behaviour is correct
		if (aPreviousState == CMdaAudioClipUtility::ERecording && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iVerdict = EPass;
			CActiveScheduler::Stop();
			if(iInternalState == EStatePending)
				{
				User::RequestComplete(iStatus, KErrNone);
				iInternalState = EStateComplete;
				}
			}
		}

	else 
		{
		iVerdict = EFail;
		CActiveScheduler::Stop();
		if(iInternalState == EStatePending)
			{
			User::RequestComplete(iStatus, aErrorCode);
			iInternalState = EStateComplete;
			}
		}

	}


CAudPlayUtilTS0014::~CAudPlayUtilTS0014()
	{
	delete iPlayer;
	}
