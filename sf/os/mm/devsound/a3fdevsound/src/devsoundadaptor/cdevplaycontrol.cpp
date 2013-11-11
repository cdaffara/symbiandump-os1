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



#include "cdevplaycontrol.h"

#include <a3f/maudiocontext.h>
#include <a3f/mbuffersource.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/audioprocessingunittypeuids.h>


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevPlayControl::CDevPlayControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevPlayControl::CDevPlayControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDevPlayControl::ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aAdaptationObserver)
	{
	DP_CONTEXT(CDevPlayControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevAudioControl::ConstructL(aDevAudio, aAdaptationObserver);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian constructor
// -----------------------------------------------------------------------------
//
CDevPlayControl* CDevPlayControl::NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_STATIC_CONTEXT(CDevPlayControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevPlayControl* self = new (ELeave) CDevPlayControl();
	CleanupStack::PushL(self);
	self->ConstructL(aDevAudio, aDevSoundObserver);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevPlayControl::~CDevPlayControl()
	{
	DP_CONTEXT(CDevPlayControl::~CDevPlayControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevPlayControl::Initialize
// -----------------------------------------------------------------------------
//
TInt CDevPlayControl::Initialize(TUid aFormat)
	{
	DP_CONTEXT(CDevPlayControl::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iDevAudio->iAudioSource && iDevAudio->iAudioCodec && iGainControl);
	TInt err(KErrNone);

	err = iDevAudio->iAudioStream->AddSource(iDevAudio->iAudioSource);
	if (err == KErrNone)
		{
		err = iDevAudio->iAudioStream->AddSink(iDevAudio->iAudioSink);
		if (err == KErrNone)
			{
			err = iDevAudio->iAudioStream->AddAudioCodec(iDevAudio->iAudioCodec);
			}
		}
	
	if(err == KErrNone && iDevAudio->iActiveState == EDevSoundAdaptorCreated_Uninitialised)
		{
		// Register for notifications
		// Note KErrAlreadyExist is not acceptable as an error, since it shouldn't occurr 
		// It's mandatory unregister any observer when no more events are required
		// Otherwise the notifications will be sent to all previously registered observer
		// when reinitialising for different mode
		err = iDevAudio->iAudioStream->RegisterAudioStreamObserver(*this);
		if (err == KErrNone)
			{
			err = iGainControl->RegisterAudioGainControlObserver(*this);
			if (err == KErrNone)
				{
				err = CacheAudioCodecIf();
				if (err == KErrNone)
					{
					__ASSERT_ALWAYS (iAudioCodecIf,CDevAudioControl::Panic(EAudioCodecIsNull));
					err = iAudioCodecIf->RegisterAudioCodecObserver(*this);
					if(err == KErrNone)
						{
						err = iAudioCodecIf->SetFormat(aFormat);
						if (err == KErrNone)
							{
							// Needed to get ProcessingUnitError notification 
							err = iDevAudio->iAudioCodec->RegisterProcessingUnitObserver(*this);
							}
						}
					}
				}
			}

		if (err != KErrNone)
			{
			DP0(DLINFO,"Error while registering observers");
			iDevAudio->iAudioStream->UnregisterAudioStreamObserver(*this);
			iGainControl->UnregisterAudioGainControlObserver(*this);
			if(iAudioCodecIf)
				{
				iAudioCodecIf->UnregisterAudioCodecObserver(*this);
				iAudioCodecIf = NULL; // Not owned here: convenience pointer, so no need to delete
				}
			}

		if (err == KErrNone)
			{
			err = iDevAudio->iAudioStream->Initialize();
			if (err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				if (err == KErrNone)
					{
					iDevAudio->iActiveState = EDevSoundAdaptorInitialising;
					}
				}
			}
		}
	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevPlayControl::ProcessInit
// -----------------------------------------------------------------------------
//
TInt CDevPlayControl::ProcessInit()
	{
	DP_CONTEXT(CDevPlayControl::ProcessInit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_START, PRF_TIME, AA_DS_PlayInit, "");
	PRF(PRF_ID, PRF_START, PRF_LOAD, AA_DS_Play, "");
	TInt err(KErrNone);
	
	switch (iDevAudio->iActiveState)
		{
		case EDevSoundAdaptorInitialised_Initialised:
			{
			TAny* interface(NULL);
			// get correct BufferSource interface of DevAudio::iAudioSource
			interface = iDevAudio->iAudioSource->Interface(KUidMmfBufferSource);
			
			if (interface == NULL)
				{
				DP0_RET(KErrNotSupported, "Incorrect source type! %d");
				}
			else
				{
				iBufferSource = static_cast<MMMFBufferSource*>(interface);
				iBufferSource->SetDataSupplier(*this);
				if (iDevAudio->IsPrioritySet())
					{
					TAudioTypeSettings priority;
					iDevAudio->GetPrioritySettings(priority);
					err = iDevAudio->iAudioStream->SetAudioType(priority);
					}				
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
			// Deliberate fall through - set err=KErrNotReady for PlayInit when already active
		default:
			err = KErrNotReady;
			break;
		}

	if(err == KErrNone)
		{
		iDevAudio->iStop = EFalse;
		}
	
	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevPlayControl::ProcessData
// -----------------------------------------------------------------------------
//
void CDevPlayControl::ProcessData()
	{
	DP_CONTEXT(CDevPlayControl::ProcessData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	
	if(!iDevAudio->iStop)
		{
		err = iBufferSource->BufferFilled(iBuffer);
		if ( err != KErrNone)
			{
			iErrorCondition = err;
			Stop();
			}
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataSupplier
// CDevPlayControl::BufferToBeFilled
// -----------------------------------------------------------------------------
//
void CDevPlayControl::BufferToBeFilled(MMMFBufferSource* aSource, CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CDevPlayControl::BufferToBeFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_STOP, PRF_TIME, AA_DS_PlayInit, "");
	__ASSERT_ALWAYS(iBufferSource==aSource, CDevAudioControl::Panic(EBufferMismatch));
	iAdaptationObserver->BufferToBeFilled(aBuffer);
	iBuffer = aBuffer;
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataSupplier
// CDevPlayControl::DiscardBuffers
// -----------------------------------------------------------------------------
//
void CDevPlayControl::DiscardBuffers(MMMFBufferSource* /*aSource*/)
	{
	DP_CONTEXT(CDevPlayControl::DiscardBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevPlayControl::ProcessingFinished
// -----------------------------------------------------------------------------
//
void CDevPlayControl::ProcessingFinished(MAudioStream& /*aStream*/)
	{
	DP_CONTEXT(CDevPlayControl::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_STOP, PRF_LOAD, AA_DS_Play, "");
	
	if(iCallbackFromAdaptor == KCallbackNone)
		{
		iCallbackFromAdaptor = KCallbackProcessingFinished;
		iAdaptationObserver->CallbackFromAdaptorReceived(iCallbackFromAdaptor, KErrNone);
		}
	else
		{
		// Multiple callbacks from adaptor
		DP0(DLERR, "Multiple callbacks from adaptor");
		}
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// CDevPlayControl::ProcessingFinishedReceived
// -----------------------------------------------------------------------------
//
TInt CDevPlayControl::ProcessingFinishedReceived(TBool& aAyncOperation)
	{
	DP_CONTEXT(CDevPlayControl::ProcessingFinishedReceived *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aAyncOperation = EFalse;
	TInt err = iDevAudio->iAudioStream->Stop();
	if ( err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		if(err == KErrNone)
			{
			iDevAudio->iActiveState = EDevSoundAdaptorStopping;
			aAyncOperation = ETrue;
			}
		}
	// Something was wrong, clear the flag.
	if (err != KErrNone)
		{
		iCallbackFromAdaptor = KCallbackNone;
		}
	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevPlayControl::BufferErrorEvent
// -----------------------------------------------------------------------------
//
void CDevPlayControl::BufferErrorEvent()
	{
	ProcessingUnitError(NULL,KErrNoMemory);
	}


// -----------------------------------------------------------------------------
// CDevPlayControl::FinishWithError
// -----------------------------------------------------------------------------
//
void CDevPlayControl::FinishWithError(TInt aError)
    {
    iAdaptationObserver->PlayError(aError);
    }

// ---------------------------------------------------------------------------
// CDevPlayControl::ProcessingError
// ---------------------------------------------------------------------------
TInt CDevPlayControl::ProcessingError(TBool& aAyncOperation)
    {
    TInt err = ProcessingFinishedReceived(aAyncOperation);
    return err;
    }
    
// End of file

