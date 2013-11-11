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



#include "cdevrecordcontrol.h"

#include <a3f/maudiocontext.h>
#include <a3f/mbuffersink.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/audioprocessingunittypeuids.h>


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevRecordControl::CDevRecordControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevRecordControl::CDevRecordControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CDevRecordControl::ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_CONTEXT(CDevRecordControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevAudioControl::ConstructL(aDevAudio, aDevSoundObserver);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// Symbian constructor
// -----------------------------------------------------------------------------
//
CDevRecordControl* CDevRecordControl::NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	DP_STATIC_CONTEXT(CDevRecordControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevRecordControl* self = new (ELeave) CDevRecordControl();
	CleanupStack::PushL(self);
	self->ConstructL(aDevAudio, aDevSoundObserver);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevRecordControl::~CDevRecordControl()
	{
	DP_CONTEXT(CDevRecordControl::~CDevRecordControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevRecordControl::Initialize
// -----------------------------------------------------------------------------
//
TInt CDevRecordControl::Initialize(TUid aFormat)
	{
	DP_CONTEXT(CDevRecordControl::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iDevAudio->iAudioSink && iDevAudio->iAudioCodec && iGainControl);
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

	if (err == KErrNone)
		{
		// Register for notifications
		// KErrAlreadyExist shouldn't occurr 
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
					err = iAudioCodecIf->RegisterAudioCodecObserver(*this);
					if(err == KErrNone)
						{
						__ASSERT_ALWAYS (iAudioCodecIf, CDevAudioControl::Panic(EAudioCodecIsNull));
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
TInt CDevRecordControl::ProcessInit()
	{
	DP_CONTEXT(CDevRecordControl::ProcessInit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_START, PRF_TIME, AA_DS_RecordInit, "");

	TInt err(KErrNone);

	switch (iDevAudio->iActiveState)
		{
		case EDevSoundAdaptorInitialised_Initialised:
			{
			TAny* interface;
			// get correct BufferSink interface of DevAudio::iAudioSink
			interface = iDevAudio->iAudioSink->Interface(KUidMmfBufferSink);
			if (interface == NULL)
				{
				DP0_RET(KErrNotSupported, "Incorrect sink type! %d");
				}
			else
				{
				iBufferSink = static_cast<MMMFBufferSink*>(interface);
				err = iBufferSink->SetDataConsumer(*this);
				}
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
// CDevRecordControl::ProcessData
// -----------------------------------------------------------------------------
//
void CDevRecordControl::ProcessData()
	{
	DP_CONTEXT(CDevRecordControl::ProcessData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	if(!iDevAudio->iStop)
		{
		err = iBufferSink->BufferEmptied(iBuffer);
		if ( err != KErrNone)
			{
			iErrorCondition = err;
			Stop();
			}
		}
		
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataConsumer
// CDevRecordControl::BufferToBeEmptied
// -----------------------------------------------------------------------------
//
void CDevRecordControl::BufferToBeEmptied(MMMFBufferSink* aSink, CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CDevRecordControl::BufferToBeEmptied *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_STOP, PRF_TIME, AA_DS_RecordInit, "");
	__ASSERT_ALWAYS(iBufferSink==aSink, CDevAudioControl::Panic(EBufferMismatch));
	iAdaptationObserver->BufferToBeEmptied(aBuffer);
	iBuffer = aBuffer;
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioDataConsumer
// CDevRecordControl::DiscardBuffers
// -----------------------------------------------------------------------------
//
void CDevRecordControl::DiscardBuffers(MMMFBufferSink* /*aSource*/)
	{
	DP_CONTEXT(CDevRecordControl::DiscardBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// CDevRecordControl::ProcessingFinished
// -----------------------------------------------------------------------------
//
void CDevRecordControl::ProcessingFinished(MAudioStream& /*aStream*/)
	{
	DP_CONTEXT(CDevRecordControl::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iDevAudio->iActiveState == EDevSoundAdaptorPausing || iDevAudio->iActiveState == EDevSoundAdaptorPaused_Primed);
	if(iCallbackFromAdaptor == KCallbackNone)
		{
		if(iDevAudio->iActiveState == EDevSoundAdaptorPausing || iDevAudio->iActiveState == EDevSoundAdaptorPaused_Primed)
			{
			iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackRecordPauseComplete, KErrNone);
			}
		}
	else
		{
		// Multiple callbacks from adaptor
		DP0(DLERR, "Multiple callbacks from adaptor");
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevRecordControl::BufferErrorEvent
// -----------------------------------------------------------------------------
//
void CDevRecordControl::BufferErrorEvent()
	{
	ProcessingUnitError(NULL,KErrNoMemory);
	}

// -----------------------------------------------------------------------------
// CDevRecordControl::FinishWithError
// -----------------------------------------------------------------------------
//
void CDevRecordControl::FinishWithError(TInt aError)
    {
    iAdaptationObserver->RecordError(aError);
    }


// ---------------------------------------------------------------------------
// CDevRecordControl::ProcessingError
// ---------------------------------------------------------------------------
TBool CDevRecordControl::ProcessingError(TBool& aAyncOperation)
    {
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
    return aAyncOperation;
    }
// End of file
