// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "cdevtonecontrol.h"

#include <a3f/maudiocontext.h>
#include <a3f/mbuffersource.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/audioprocessingunittypeuids.h>


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevToneControl::CDevToneControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevToneControl::CDevToneControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDevToneControl::ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_CONTEXT(CDevToneControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevAudioControl::ConstructL(aDevAudio, aDevSoundObserver);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian constructor
// -----------------------------------------------------------------------------
//
CDevToneControl* CDevToneControl::NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_STATIC_CONTEXT(CDevToneControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevToneControl* self = new (ELeave) CDevToneControl();
	CleanupStack::PushL(self);
	self->ConstructL(aDevAudio, aDevSoundObserver);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevToneControl::~CDevToneControl()
	{
	DP_CONTEXT(CDevToneControl::~CDevToneControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevToneControl::Initialize
// -----------------------------------------------------------------------------
//
TInt CDevToneControl::Initialize(TUid aFormat)
	{
	DP_CONTEXT(CDevToneControl::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iDevAudio->iAudioSource && iDevAudio->iAudioCodec && iGainControl);
	TInt err(KErrNone);
	
	// Adding pUnits
	err = iDevAudio->iAudioStream->AddSource(iDevAudio->iAudioSource);
	if (err == KErrNone)
		{
		err = iDevAudio->iAudioStream->AddSink(iDevAudio->iAudioSink);
		if (err == KErrNone)
			{
			err = iDevAudio->iAudioStream->AddAudioCodec(iDevAudio->iAudioCodec);
			}
		}

	// KErrAlreadyExist shouldn't occurr 
	// It's mandatory unregister any observer when no more events are required
	// Otherwise the notifications will be sent to all previously registered observer
	// when reinitialising for different mode
	err = iDevAudio->iAudioStream->RegisterAudioStreamObserver(*this);
	if (err != KErrNone)
		{
		DP0_RET(err, "Stream observer registeration failed, err = %d");
		}
	iGainControl->RegisterAudioGainControlObserver(*this);
	if (err != KErrNone)
		{
		DP0_RET(err, "Gaincontrol observer registeration failed, err = %d");
		}

	err = CacheAudioCodecIf();
	if (err != KErrNone)
		{
		DP0_RET(err, "CacheAudioCodecIf() failed, err = %d");
		}

	err = iAudioCodecIf->RegisterAudioCodecObserver(*this);
	if(err != KErrNone)
		{
		DP0_RET(err, "Registration failed, err = %d");
		}

	__ASSERT_ALWAYS (iAudioCodecIf, CDevAudioControl::Panic(EAudioCodecIsNull));
	err = iAudioCodecIf->SetFormat(aFormat);
	if (err!=KErrNone)
		{
		DP0_RET(err, "SetFormat() failed, err = %d");
		}
			
	// Needed to get ProcessingUnitError notification 
	err = iDevAudio->iAudioCodec->RegisterProcessingUnitObserver(*this);
	if ((err != KErrAlreadyExists) && (err != KErrNone))
		{
		DP0_RET(err, "Audiocodec observer registeration failed, err = %d");
		}

	// get correct BufferSource interface of DevAudio::iAudioSource
	TAny* interface = iDevAudio->iAudioSource->Interface(KUidMmfBufferSource);
	if (interface == NULL)
		{
		DP0_RET(KErrNotSupported, "Incorrect source type! %d");
		}
	else
		{
		iBufferSource = static_cast<MMMFBufferSource*>(interface);
		err = iBufferSource->SetDataSupplier(*this);
		if (err != KErrNone )
			{
			DP0_RET(err, "Data suplier setting failed, err = %d");
			}
		}

	err = iDevAudio->iAudioStream->Initialize();
	if (err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		}
	if (err == KErrNone)
		{
		iDevAudio->iActiveState = EDevSoundAdaptorInitialising;
		}

	DP0_RET(err,"%d");
	}


// -----------------------------------------------------------------------------
// CDevToneControl::ProcessInit
// -----------------------------------------------------------------------------
//
TInt CDevToneControl::ProcessInit()
	{
	DP_CONTEXT(CDevToneControl::ProcessInit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_START, PRF_TIME, AA_DS_PlayTone, "");
	TInt err(KErrNone);

	DP1(DLINFO,"iActiveState = %d", iDevAudio->iActiveState);
	switch (iDevAudio->iActiveState)
		{
		case EDevSoundAdaptorInitialised_Initialised:
			{
			if (iDevAudio->IsPrioritySet())
				{
				TAudioTypeSettings priority;
				iDevAudio->GetPrioritySettings(priority);
				err = iDevAudio->iAudioStream->SetAudioType(priority);
				}
			if ( err == KErrNone)
				{
				err = iDevAudio->iAudioStream->Load();
				}
			if ( err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				}
			if(err == KErrNone)
				{
				iDevAudio->iActiveState = EDevSoundAdaptorLoading;
				}
			}
			break;

		case EDevSoundAdaptorGoingActive:
			{
			//If following condition is false, then we are here because of a
			//pre-emption clash in last Commit cycle started from
			//CDevCommonControl::ContextEventUpdateWithStateEventNoError.
			if(iDevAudio->iPreviousState != EDevSoundAdaptorActivating)
				{
				break;
				}
			//Fall through as required
			}
		case EDevSoundAdaptorPaused_Primed:
		case EDevSoundAdaptorInitialised_Idle:
			{
			//If following condition is true, then we are here because of a
			//pre-emption clash in last Commit cycle started from
			//CDevCommonControl::ContextEventUpdateWithStateEventAndError.
			if(iDevAudio->iPreviousState == EDevSoundAdaptorUnloading)
				{
				err = Unload();
				break;
				}
			DP0(DLINFO, "Already in loaded state");
			err = iDevAudio->RequestGainAndBalance(this);
			if (err==KErrNone)
				{
				err = iDevAudio->iAudioStream->Activate();
				}
			if (err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				}
			if (err == KErrNone)
				{
				iDevAudio->iActiveState = EDevSoundAdaptorActivating;
				}
			break;
			}
		case EDevSoundAdaptorActive_Active:
		default:
			err = KErrNotReady;
			break;
		}
	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevToneControl::ProcessData
// -----------------------------------------------------------------------------
//
void CDevToneControl::ProcessData()
	{
	DP_CONTEXT(CDevToneControl::ProcessData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataSupplier
// CDevToneControl::BufferToBeFilled
// -----------------------------------------------------------------------------
//
void CDevToneControl::BufferToBeFilled(MMMFBufferSource* aSource, CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CDevToneControl::BufferToBeFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	__ASSERT_ALWAYS(iBufferSource==aSource, CDevAudioControl::Panic(EBufferMismatch));
	TInt err = KErrNone;
	
	CMMFDataBuffer* myBuffer = static_cast<CMMFDataBuffer*> (aBuffer);
	Mem::Copy((TAny*)(myBuffer->Data().Ptr()), (TAny*)(&iToneData), sizeof(TToneData) );
	
	myBuffer->SetLastBuffer(ETrue);
	
	err = iBufferSource->BufferFilled(myBuffer); 
	if ( err != KErrNone)
		{
		iErrorCondition = err;
		Stop();
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataSupplier
// CDevToneControl::DiscardBuffers
// -----------------------------------------------------------------------------
//
void CDevToneControl::DiscardBuffers(MMMFBufferSource* /*aSource*/)
	{
	DP_CONTEXT(CDevToneControl::DiscardBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevToneControl::ProcessingFinished
// -----------------------------------------------------------------------------
//
void CDevToneControl::ProcessingFinished(MAudioStream& /*aStream*/)
	{
	DP_CONTEXT(CDevToneControl::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_STOP, PRF_TIME, AA_DS_PlayTone, "");
	
	if(iCallbackFromAdaptor == KCallbackNone)
		{
		iCallbackFromAdaptor = KCallbackProcessingFinished;
		iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackProcessingFinished, KErrNone);
		}
	else
		{
		// Multiple callbacks from adaptor
		DP0(DLERR, "Multiple callbacks from adaptor");
		}
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevToneControl::SetToneData
// -----------------------------------------------------------------------------
//
TInt CDevToneControl::SetToneData(TToneData& aToneData)
	{
	DP_CONTEXT(CDevToneControl::SetToneData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iToneData = aToneData;

	DP0_RET(KErrNone, "%d");
	}


// -----------------------------------------------------------------------------
// CDevToneControl::FinishWithError
// -----------------------------------------------------------------------------
//
void CDevToneControl::FinishWithError(TInt aError) // from CDevCommonControl
    {
    iAdaptationObserver->ToneFinished(aError);
    }


// -----------------------------------------------------------------------------
// CDevPlayControl::ProcessingFinishedReceived
// -----------------------------------------------------------------------------
//
TInt CDevToneControl::ProcessingFinishedReceived(TBool& asyncOperation)
	{
	DP_CONTEXT(CDevToneControl::ProcessingFinishedReceived *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	asyncOperation = EFalse;
	TInt err = iDevAudio->iAudioStream->Stop();
	if ( err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		if(err == KErrNone)
			{
			iDevAudio->iActiveState = EDevSoundAdaptorStopping;
			asyncOperation = ETrue;
			}
		}
	// Something was wrong, clear the flag.
	if (err != KErrNone)
		{
		iCallbackFromAdaptor = KCallbackNone;
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// CDevToneControl::ProcessingError
// ---------------------------------------------------------------------------
TInt CDevToneControl::ProcessingError(TBool& aAyncOperation)
    {
    TInt err = ProcessingFinishedReceived(aAyncOperation);
    return err;
    }
// End of file
