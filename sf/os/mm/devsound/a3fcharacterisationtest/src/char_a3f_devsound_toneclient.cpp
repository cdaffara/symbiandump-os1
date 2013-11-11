// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// char_a3f_devsound_testclient.h
// 
//

#include "char_a3f_devsound_toneclient.h"

CA3FDevSoundToneClient::CA3FDevSoundToneClient(MA3FDevsoundToneClientObserver &aObserver)
	: 	iDevSound(NULL), 
		iObserver(aObserver), 
		iDTMFString(KNullDesC)
	{
	}

CA3FDevSoundToneClient::~CA3FDevSoundToneClient()
	{
	delete iDevSound;
	}

CA3FDevSoundToneClient* CA3FDevSoundToneClient::NewL(MA3FDevsoundToneClientObserver &aObserver)
	{
	CA3FDevSoundToneClient* self = new (ELeave) CA3FDevSoundToneClient(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CA3FDevSoundToneClient::ConstructL()
	{   
	iDevSound = CMMFDevSound::NewL();
	iDevSound->SetVolume(iDevSound->MaxVolume());
	}

void CA3FDevSoundToneClient::SetPriority(TInt priority)
	{   
	TMMFPrioritySettings settings;
	settings.iPriority = priority;
	settings.iPref = EMdaPriorityPreferenceTime;
	iDevSound->SetPrioritySettings(settings);
	}

TInt CA3FDevSoundToneClient::InitTonePlay(TInt aFrequency,TInt aDuration)
	{
	iFrequency = aFrequency;
	iDuration = aDuration;
	TRAPD(err, iDevSound->InitializeL(*this,EMMFStateTonePlaying));
	return err;
	}

TInt CA3FDevSoundToneClient::InitDualTonePlay(TInt aFrequency, TInt aFrequencyTwo,  TInt aDuration)
	{
	iFrequency = aFrequency;
	iFrequencyTwo = aFrequencyTwo;
	iDuration = aDuration;
	TRAPD(err, iDevSound->InitializeL(*this,EMMFStateTonePlaying));
	return err;
	}

TInt CA3FDevSoundToneClient::InitDTMFStringPlay(const TDesC &aDTMFString)
	{
	iDTMFString = aDTMFString;
	TRAPD(err, iDevSound->InitializeL(*this,EMMFStateTonePlaying));
	return err;
	}

TInt CA3FDevSoundToneClient::InitToneSequencePlay()
	{
	TRAPD(err, iDevSound->InitializeL(*this,EMMFStateTonePlaying));
	return err;
	}

void CA3FDevSoundToneClient::InitializeComplete(TInt aError)
	{
	iObserver.ClientInitializeCompleteCallback(aError);
	}
TInt CA3FDevSoundToneClient::PlayTone()
	{
	TRAPD(err, iDevSound->PlayToneL(iFrequency,iDuration));
	return err;
	}
TInt CA3FDevSoundToneClient::PlayDualTone()
	{
	TRAPD(err, iDevSound->PlayDualToneL(iFrequency,iFrequencyTwo,iDuration));
	return err;
	}
TInt CA3FDevSoundToneClient::PlayDTMFString()
	{
	TRAPD(err, iDevSound->PlayDTMFStringL(iDTMFString));
	return err;
	}
TInt CA3FDevSoundToneClient::PlayToneSequence()
	{
	TUint8* tablePointer = const_cast<TUint8*>( &(KFixedSequenceTestSequenceDataX[0] ) ); 
	TPtrC8 KFixedSequenceData(tablePointer, sizeof(KFixedSequenceTestSequenceDataX));
	TRAPD(err, iDevSound->PlayToneSequenceL(KFixedSequenceData));
	return err;
	}
void CA3FDevSoundToneClient::ToneFinished(TInt aError)
	{
	iObserver.ClientToneFinishedCallback(aError);
	}

void CA3FDevSoundToneClient::BufferToBeFilled(CMMFBuffer*/*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundToneClient::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundToneClient::PlayError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundToneClient::RecordError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundToneClient::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

void CA3FDevSoundToneClient::ConvertError(TInt /*aError*/)
	{
	__ASSERT_ALWAYS(0, Panic(_L("CA3FDevSoundToneClient"), EInvalidCallbackCall));
	}

