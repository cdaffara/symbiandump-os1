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
//

#include "CapTestStep0012.h"

CAudPlayUtilTS0012* CAudPlayUtilTS0012::NewL()
	{
	CAudPlayUtilTS0012* self = new (ELeave) CAudPlayUtilTS0012;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CAudPlayUtilTS0012::ConstructL()
	{
	// Create the audio player
	iPlayer = CMdaAudioRecorderUtility::NewL(*this, NULL, EMdaPriorityMax);
	}
	

void CAudPlayUtilTS0012::StartProcessing(TRequestStatus& aStatus)
	{
	iStatus = &aStatus;
	
	iPlayer->OpenFileL(_L("\\AclntITestData\\8bitmPcm.wav"));
	CActiveScheduler::Start();
	}
		
		
TVerdict CAudPlayUtilTS0012::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	iPlayer->Close();
	aMessage.Copy(_L("Done"));
	return EPass;
	}
	
void CAudPlayUtilTS0012::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	if (aErrorCode == KErrNone)
		{
		if (aPreviousState == CMdaAudioClipUtility::ENotReady && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iPlayer->PlayL();
			}
		else if (aPreviousState == CMdaAudioClipUtility::EOpen && aCurrentState==CMdaAudioClipUtility::EPlaying)
			{
			User::RequestComplete(iStatus, KErrNone);
			}
		else if (aPreviousState == CMdaAudioClipUtility::EPlaying && aCurrentState==CMdaAudioClipUtility::EOpen)
			{
			iVerdict = EPass;
			CActiveScheduler::Stop();
			}
		}
	else 
		{
		iVerdict = EFail;
		CActiveScheduler::Stop();
		}
		
	}


CAudPlayUtilTS0012::~CAudPlayUtilTS0012()
	{
	delete iPlayer;
	}
