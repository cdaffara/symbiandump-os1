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



#include "cdevaudio.h"
#include "cdevaudiocontrol.h"
#include "cdevplaycontrol.h"
#include "cdevrecordcontrol.h"
#include "cdevtonecontrol.h"
#include "cdevgencontrol.h"

#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/devsoundadaptationinfo.h>
#include <a3f/a3ffourcclookup.h>
#include <a3f/audiocontextfactory.h>
#include <a3f/maudioprocessingunit.h>
#include <a3f/maudiocontext.h>
#include <a3f/mcontexteffectiveclient.h>
#include <a3f/maudiostream.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/mbuffersource.h>
#include <a3f/mbuffersink.h>

#include "mglobalproperties.h"

const TInt KMidWayBalance = 50; // 50%
const TInt KMaxBalance = 100; // 100%
const TInt KLeftChannel = 0;
const TInt KRightChannel = 1;


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevAudio::CDevAudio(MDevSoundAdaptationObserver& aAdaptationObserver)
	: iAdaptationObserver(aAdaptationObserver), iMode(EMMFStateIdle), 
	iActiveState(EDevSoundAdaptorCreated_Uninitialised),
	iActiveStreamState(EUninitialized)
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevAudio::CDevAudio *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
		
	iDevSoundPlayBalance[KLeftChannel] = iDevSoundPlayBalance[KRightChannel] = KMidWayBalance;
	iDevSoundRecordBalance[KLeftChannel] = iDevSoundRecordBalance[KRightChannel] = KMidWayBalance;

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevAudio::~CDevAudio()
	{
	DP_CONTEXT(CDevAudio::~CDevAudio *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	delete iAdaptationInfo;
	
	iSupportedInputFormats.Close();
	iSupportedOutputFormats.Close();

	// This part is fine at the destructor
	// All this is syncronous
	if ( iAudioContext )
		{
		if ( iGainControl )
			{
			iAudioContext->DeleteAudioProcessingUnit(iGainControl);
			}
		if ( iAudioSink )
			{
			iAudioContext->DeleteAudioProcessingUnit(iAudioSink);
			}
		if ( iAudioCodec )
			{
			iAudioContext->DeleteAudioProcessingUnit(iAudioCodec);
			}
		if ( iAudioSource )
			{
			iAudioContext->DeleteAudioProcessingUnit(iAudioSource);
			}
		if ( iAudioStream )
			{
			iAudioContext->DeleteAudioStream(iAudioStream);
			}
		}

	iAudioContextFactory->DeleteAudioContext(iAudioContext);
	
	if ( iAudioContextFactory )
		{
		delete iAudioContextFactory;
		}

	if ( iDevPlayControl )
		{
		delete iDevPlayControl;
		}
	if ( iDevRecordControl )
		{
		delete iDevRecordControl;
		}
	if ( iDevToneControl )
		{
		delete iDevToneControl;
		}
	if ( iDevGenControl )
		{
		delete iDevGenControl;
		}
	
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDevAudio* CDevAudio::NewL(MDevSoundAdaptationObserver& aAdaptationObserver, 
									MGlobalProperties& aGlobalProperties)
	{
	DP_STATIC_CONTEXT(CDevAudio::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CDevAudio* self = new (ELeave) CDevAudio(aAdaptationObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aGlobalProperties);
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}


// -----------------------------------------------------------------------------
// CDevAudio::ConstructL
// -----------------------------------------------------------------------------
//
void CDevAudio::ConstructL(MGlobalProperties& aGlobalProperties)
	{
	DP_CONTEXT(CDevAudio::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iGlobalProperties = &aGlobalProperties;

	iAudioContextFactory = CAudioContextFactory::NewL();
	
	User::LeaveIfError(iAudioContextFactory->CreateAudioContext(iAudioContext));

	User::LeaveIfError(iAudioContext->CreateAudioStream(iAudioStream));
	
	User::LeaveIfError(iAudioContext->CreateAudioProcessingUnit(KUidAudioGainControl, iGainControl));
	User::LeaveIfError(iAudioStream->AddGainControl(iGainControl));

	iDevPlayControl = CDevPlayControl::NewL(this, iAdaptationObserver);
	iDevRecordControl = CDevRecordControl::NewL(this, iAdaptationObserver);
	iDevToneControl = CDevToneControl::NewL(this, iAdaptationObserver);
	iDevGenControl = CDevGenControl::NewL(this, iAdaptationObserver);
	
	iAdaptationInfo = CA3FDevSoundAdaptationInfo::NewL(*this, const_cast<CFourCCConvertor&>(iGlobalProperties->GetFourCCConvertor()));

	// The generic audio control is the only one receiving the callbacks at this moment
	iCurrentAudioControl = static_cast<CDevAudioControl*>(iDevGenControl);
	TInt err = iAudioContext->RegisterAudioContextObserver(*iCurrentAudioControl);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	DP_OUT();
	}
	
// -----------------------------------------------------------------------------
// CDevAudio::PostOpen
// -----------------------------------------------------------------------------
//
TInt CDevAudio::PostOpen()
	{
	DP_CONTEXT(CDevAudio::PostOpen *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iPreOpenState==EPreOpenStateIdle);
	TInt err = KErrNone;
	
	if(iPreOpenState == EPreOpenStateIdle)
		{
		err = iAdaptationInfo->RequestMaxGain(KUidAudioDecoder);
		if (err == KErrNone)
			{
			iPreOpenState = EPreOpenStateRequestingMaxVolume;
			}
		}
	
	DP0_RET(err, "%d");
	}
	

TBool CDevAudio::IsResumeSupported()
	{
	DP_CONTEXT(CDevAudio::IsResumeSupported *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TUid codecType(KNullUid);
	if(iMode == EMMFStatePlaying || iMode == EMMFStateTonePlaying)
		{
		codecType = KUidAudioDecoder;
		}
	else if (iMode == EMMFStateRecording)
		{
		codecType = KUidAudioEncoder;
		}
	TBool supported = iAdaptationInfo->IsResumeSupported(codecType, iFormat);

	DP0_RET(supported, "%d");
	}


// -----------------------------------------------------------------------------
// CDevAudio::RequestMaxGainComplete
// -----------------------------------------------------------------------------
//
void CDevAudio::RequestMaxGainComplete (TUid __DEBUG_ONLY(aCodecType), TInt aError, TInt aResult)
	{
	DP_CONTEXT(CDevAudio::RequestMaxGainComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iPreOpenState==EPreOpenStateRequestingMaxVolume && aCodecType==KUidAudioDecoder||
			iPreOpenState==EPreOpenStateRequestingMaxGain && aCodecType==KUidAudioEncoder);

	TInt err = aError;
	TBool asyncComplete = EFalse;
	
	if (err == KErrNone)
		{
		if (iPreOpenState==EPreOpenStateRequestingMaxVolume)
			{
			iDevSoundMaxVolume = aResult;
						
			err = SetDevSoundVolume((iDevSoundMaxVolume+1)/2, asyncComplete);

			__ASSERT_DEBUG(asyncComplete==EFalse, User::Invariant());
			
			if(err == KErrNone)
				{
				// first request - so request max gain
				err = iAdaptationInfo->RequestMaxGain(KUidAudioEncoder);
				if (err == KErrNone)
					{
					iPreOpenState = EPreOpenStateRequestingMaxGain;
					}
				}
			}
		else if(iPreOpenState==EPreOpenStateRequestingMaxGain)
			{
			iDevSoundMaxGain = aResult;
			
			err = SetDevSoundGain((iDevSoundMaxGain+1)/2, asyncComplete);

			__ASSERT_DEBUG(asyncComplete==EFalse, User::Invariant());
			
			if(err == KErrNone)
				{
				// second request - complete whatever - Ready for Formats
				err = iAdaptationInfo->RequestSupportedFormats(KUidAudioDecoder, iSupportedInputFormats);
				if (err == KErrNone)
					{
					iPreOpenState = EPreOpenStateRequestingInputFormats;
					}
				}
			}
		}

	if (err!=KErrNone)
		{
		iPreOpenState = EPreOpenStateIdle;
		iAdaptationObserver.AsynchronousOperationComplete(err, ETrue);
		}
	
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::RequestSupportedFormatsComplete
// -----------------------------------------------------------------------------
//
void CDevAudio::RequestSupportedFormatsComplete(TUid __DEBUG_ONLY(aCodecType), TInt aError)
	{
	DP_CONTEXT(CDevAudio::RequestSupportedFormatsComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iPreOpenState==EPreOpenStateRequestingInputFormats && aCodecType==KUidAudioDecoder||
			iPreOpenState==EPreOpenStateRequestingOutputFormats && aCodecType==KUidAudioEncoder);

	TInt err = aError;

	if (err == KErrNone)
		{
		if (iPreOpenState==EPreOpenStateRequestingInputFormats)
			{
			// first request - so request output formats
			err = iAdaptationInfo->RequestSupportedFormats(KUidAudioEncoder, iSupportedOutputFormats);
			if (err == KErrNone)
				{
				iPreOpenState = EPreOpenStateRequestingOutputFormats;
				}
			}
		else
			{
			ASSERT(iPreOpenState==EPreOpenStateRequestingOutputFormats);
			// second request - complete whatever
			iPreOpenState = EPreOpenStateIdle;
			iAdaptationObserver.AsynchronousOperationComplete(aError, ETrue);
			}
		}

	if (err!=KErrNone)
		{
		iPreOpenState = EPreOpenStateIdle;
		iAdaptationObserver.AsynchronousOperationComplete(err, ETrue);
		}
	DP_OUT();
	}



// -----------------------------------------------------------------------------
// CDevAudio::Initialize
// -----------------------------------------------------------------------------
//
TInt CDevAudio::Initialize(TUid aFormat, TMMFState aMode)
	{
	DP_CONTEXT(CDevAudio::Initialize*CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	if(iActiveState != EDevSoundAdaptorCreated_Uninitialised &&
		iActiveState != EDevSoundAdaptorInitialised_Initialised && 
		iActiveState != EDevSoundAdaptorInitialised_Idle &&
		iActiveState != EDevSoundAdaptorUnitialised_Uninitialised )
		{
		DP0_RET(KErrNotReady, "%d");
		}

	// Reinitialization 
	if (iActiveState == EDevSoundAdaptorInitialised_Initialised || 
		iActiveState == EDevSoundAdaptorInitialised_Idle)
		{
		iReinitializing = ETrue;
		iTargetFormat = aFormat;
		iTargetMode = aMode;
		if(iActiveState == EDevSoundAdaptorInitialised_Idle)
			{
			err = iAudioStream->Unload();
			if(err == KErrNone)
				{
				err = CommitAudioContext();
				}
			if(err == KErrNone)
				{
				iActiveState = EDevSoundAdaptorUnloading;
				}
			}
		else
			{
			err = iCurrentAudioControl->Uninitialize();
			}
		DP0_RET(err, "%d");
		}

	// Redo partial initialization after pre-emption clash event in
	// EDevSoundAdaptorRemovingProcessingUnits state
	if (iActiveState == EDevSoundAdaptorUnitialised_Uninitialised &&
			iPreviousState == EDevSoundAdaptorRemovingProcessingUnits)
		{
		err = iCurrentAudioControl->RemoveProcessingUnits();
		DP0_RET(err, "%d");
		}

	// Delete pUnits if already created
	if (iAudioSource)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioSource);
		iAudioSource = NULL;
		}

	if (iAudioSink)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioSink);
		iAudioSink = NULL;
		}

	if (iAudioCodec)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioCodec);
		iAudioCodec = NULL;
		}

	//	Create pUnits and select proper DevSound Adaptor Helper
	if (aMode == EMMFStatePlaying)
		{
		// create correct type sink&source&codec
		err = CreateAudioProcessingUnits(KUidMmfBufferSource, KUidAudioDeviceSink, KUidAudioDecoder);
		if (err == KErrNone)
			{
			// Now just the playcontrol should receive the only one receiving events from 
			// AudioContext
			iAudioContext->UnregisterAudioContextObserver(*iCurrentAudioControl);
			iCurrentAudioControl = static_cast<CDevAudioControl*>(iDevPlayControl);
			err = iAudioContext->RegisterAudioContextObserver(*iCurrentAudioControl);
			}
		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			DeleteAudioProcessingUnits();
			}
		}
		
	else if (aMode == EMMFStateRecording)
		{
		// create correct type sink&source&codec
		err = CreateAudioProcessingUnits(KUidAudioDeviceSource, KUidMmfBufferSink, KUidAudioEncoder);
		if (err == KErrNone)
			{
			iAudioContext->UnregisterAudioContextObserver(*iCurrentAudioControl);
			iCurrentAudioControl = static_cast<CDevAudioControl*>(iDevRecordControl);
			err = iAudioContext->RegisterAudioContextObserver(*iCurrentAudioControl);
			}
		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			DeleteAudioProcessingUnits();
			}
		}
	else if (aMode == EMMFStateTonePlaying)
		{
		// create correct type sink&source&codec
		err = CreateAudioProcessingUnits(KUidMmfBufferSource, KUidAudioDeviceSink, KUidAudioDecoder);
		if (err == KErrNone)
			{
			iAudioContext->UnregisterAudioContextObserver(*iCurrentAudioControl);
			iCurrentAudioControl =  static_cast<CDevAudioControl*>(iDevToneControl);
			err = iAudioContext->RegisterAudioContextObserver(*iCurrentAudioControl);
			}
	if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			DeleteAudioProcessingUnits();
			}
		}
	else if (aMode == EMMFStateIdle)
		{
		// Unsure about this
		iAudioContext->UnregisterAudioContextObserver(*iCurrentAudioControl);
		iCurrentAudioControl =  static_cast<CDevAudioControl*>(iDevGenControl);
		err = iAudioContext->RegisterAudioContextObserver(*iCurrentAudioControl);
		}
	else
		{
		iCurrentAudioControl = static_cast<CDevAudioControl*>(iDevGenControl);
		err = KErrNotSupported;
		}

	if (err == KErrNone)
		{
		err = iCurrentAudioControl->Initialize(aFormat);	
		}
	
	if(err == KErrNone)
		{
		iMode = aMode;
		iFormat = aFormat;
		}

	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::CancelInitialize
// -----------------------------------------------------------------------------
//
TInt CDevAudio::CancelInitialize()
	{
	DP_CONTEXT(CDevAudio::CancelInitialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	// Redo partial cancelling of initialization after pre-emption clash event in
	// EDevSoundAdaptorRemovingProcessingUnits state.
	if (iActiveState == EDevSoundAdaptorUnitialised_Uninitialised &&
			iPreviousState == EDevSoundAdaptorRemovingProcessingUnits)
		{
		err = iCurrentAudioControl->RemoveProcessingUnits();
		DP0_RET(err, "%d");
		}
	else if(iActiveState != EDevSoundAdaptorInitialised_Initialised)
		{
		DP0_RET(KErrNotReady, "%d");
		}

	err = iCurrentAudioControl->Uninitialize();
	
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::GetAudioControl
// -----------------------------------------------------------------------------
//
CDevAudioControl* CDevAudio::GetAudioControl()
	{
	DP_CONTEXT(CDevAudio::GetAudioControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	return iCurrentAudioControl;
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetPrioritySettings
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	{
	DP_CONTEXT(CDevAudio::SetPrioritySettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iPrioritySettings.iPriority = aPrioritySettings.iPriority;
	iPrioritySettings.iPref = aPrioritySettings.iPref;
	iPriorityFlag = ETrue;
	DP2(DLINFO, "Priority = 0x%x Preference = 0x%x", iPrioritySettings.iPriority,iPrioritySettings.iPref);
	DP0_RET(0, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::GetPrioritySettings
// -----------------------------------------------------------------------------
//
void CDevAudio::GetPrioritySettings(TAudioTypeSettings& aPrioritySettings)
	{
	aPrioritySettings = iPrioritySettings;
	}
// -----------------------------------------------------------------------------
// CDevAudio::IsPrioritySet
// -----------------------------------------------------------------------------
//
TBool CDevAudio::IsPrioritySet()
	{
	return iPriorityFlag;
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetClientConfig
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetClientConfig(const TProcessId& aProcessId)
	{
	DP_CONTEXT(CDevAudio::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// TODO: Check if the TClientContextSettings atributte go back since 
	// if not there is no way to send the vendor id
	TClientContextSettings context;
	context.iProcessId = aProcessId;
	TInt err = iAudioContext->SetClientSettings(context);
	if (err != KErrNone)
		{
		DP1(DLERR, "Error %d setting client context!",err);
		}
	DP0_RET(err, "%d");
	}

TInt CDevAudio::SetClientConfig(const TProcessId& aActualProcessId, const TProcessId& aProcessId)
	{
	DP_CONTEXT(CDevAudio::SetClientConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	MContextSetEffectiveClient* setEffectiveClient
        = static_cast<MContextSetEffectiveClient*>(iAudioContext->Interface(KSetClientInfoUid));
	
	TInt err;
	
	if (!setEffectiveClient)
	    {
        DP0(DLINFO, "MContextSetEffectiveClient not supported, revert to old behaviour of just passing actual client info");
        err = SetClientConfig(aActualProcessId);
	    }
	else
	    {
        TClientContextSettings context;
        context.iProcessId = aProcessId;
        err = iAudioContext->SetClientSettings(context);
        if (err != KErrNone)
            {
            DP1(DLERR, "Error %d setting client context!",err);
            }
        if (!err)
            {
            err = setEffectiveClient->SetEffectiveClientInfo(aActualProcessId);
            if (err != KErrNone)
                {
                DP1(DLERR, "Error %d setting effective client context!",err);
                }
            }
	    }

	DP0_RET(err, "%d");
	}


void CDevAudio::ContextEvent(TUid /*aEvent*/, TInt /*aError*/)
	{
	DP_CONTEXT(CDevAudio::ContextEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::ActiveState
// -----------------------------------------------------------------------------
//
TDevSoundAdaptorState CDevAudio::ActiveState() const
	{
	DP_CONTEXT(CDevAudio::ActiveState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iActiveState, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::ActiveState
// -----------------------------------------------------------------------------
//
TDevSoundAdaptorState CDevAudio::PreviousState() const
	{
	DP_CONTEXT(CDevAudio::PreviousState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iPreviousState, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetActiveState
// -----------------------------------------------------------------------------
//
void CDevAudio::SetActiveState(TDevSoundAdaptorState aAdaptorState)
	{
	DP_CONTEXT(CDevAudio::SetActiveState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iActiveState = aAdaptorState;
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetPreviousState
// -----------------------------------------------------------------------------
//
void CDevAudio::SetPreviousState(TDevSoundAdaptorState aAdaptorState)
	{
	DP_CONTEXT(CDevAudio::SetPreviousState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iPreviousState = aAdaptorState;
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetDevSoundVolume
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetDevSoundVolume(TInt aVolume, TBool& aAsyncComplete)
	{
	DP_CONTEXT(CDevAudio::SetDevSoundVolume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Values are clipped between 0 and MaxGain. 
	// because MAudioGainControl expects this to be done
	TInt volume = aVolume;
	if (volume < 0)
		{
		volume = 0;
		}
	else if (volume > iDevSoundMaxVolume)
		{
		volume = iDevSoundMaxVolume;
		}
	iDevSoundVolume = volume;
	
	TInt error = SetGainAndBalance(EFalse, aAsyncComplete);

	DP0_RET(error, "%d");
	}


// -----------------------------------------------------------------------------
// CDevAudio::SetDevSoundGain
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetDevSoundGain(TInt aGain, TBool& aAsyncComplete)
	{
	DP_CONTEXT(CDevAudio::SetDevSoundGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Values are clipped between 0 and MaxGain. 
	// because MAudioGainControl expects this to be done
	TInt gain = aGain;
	if (gain < 0)
		{
		gain = 0;
		}
	else if (gain > iDevSoundMaxGain)
		{
		gain = iDevSoundMaxGain;
		}
	iDevSoundGain = gain;

	TInt error = SetGainAndBalance(EFalse, aAsyncComplete);

	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetDevSoundPlayBalance
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetDevSoundPlayBalance(TInt aLeftBalance, TInt aRightBalance, TBool& aAsyncComplete)
	{
	DP_CONTEXT(CDevAudio::SetDevSoundPlayBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	// Clipping values
	TInt left = aLeftBalance;
	if (left < 0)
		{
		left = 0;
		}
	else if (left > KMaxBalance)
		{
		left = KMaxBalance;
		}
	TInt right = aRightBalance;
	if (right < 0)
		{
		right = 0;
		}
	else if (right > KMaxBalance)
		{
		right = KMaxBalance;
		}
	iDevSoundPlayBalance[KLeftChannel] = left;
	iDevSoundPlayBalance[KRightChannel] = right;

	TInt error = SetGainAndBalance(EFalse, aAsyncComplete);

	DP0_RET(error, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::SetDevSoundRecordBalance
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetDevSoundRecordBalance(TInt aLeftBalance, TInt aRightBalance, TBool& aAsyncComplete)
	{
	DP_CONTEXT(CDevAudio::SetDevSoundRecordBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Clipping values	
	TInt left = aLeftBalance;
	if (left < 0)
		{
		left = 0;
		}
	else if (left > KMaxBalance)
		{
		left = KMaxBalance;
		}
	TInt right = aRightBalance;
	if (right < 0)
		{
		right = 0;
		}
	else if (right > KMaxBalance)
		{
		right = KMaxBalance;
		}
	iDevSoundRecordBalance[KLeftChannel] = left;
	iDevSoundRecordBalance[KRightChannel] = right;

	TInt error = SetGainAndBalance(EFalse, aAsyncComplete);

	DP0_RET(error, "%d");
	}


// -----------------------------------------------------------------------------
// CDevAudio::SetVolumeRamp
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CDevAudioControl::SetVolumeRamp *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iRampDuration = aRampDuration;
	
	DP0_RET(KErrNone,"%d");
	}

// -----------------------------------------------------------------------------
// CDevAudio::RequestGainAndBalance
// -----------------------------------------------------------------------------
//
TInt CDevAudio::RequestGainAndBalance(CDevAudioControl* aCallingControl)
	{
	DP_CONTEXT(CDevAudio::RequestGainAndBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt error = KErrNone;
	if (aCallingControl != iCurrentAudioControl)
		{
		error = KErrNotReady;
		}
	else
		{
		TBool dummy;
		error = SetGainAndBalance(ETrue, dummy);
		}
	DP0_RET(error, "%d");
	}
	
// -----------------------------------------------------------------------------
// CDevAudio::SetGainAndBalance
// -----------------------------------------------------------------------------
//
TInt CDevAudio::SetGainAndBalance(TBool aBecomingActive, TBool& aAsyncComplete)
	{
	DP_CONTEXT(CDevAudio::SetGainAndBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt error = KErrNone;
	aAsyncComplete = EFalse; // let's assume this - makes logic easier below
	// Apply cached settings. 
	// If aBecomingActive is true it indicates this call is doing before the MAudioStream::Activate() transition, 
	// so if required will apply volume ramp and won't itself generate a Commit() call. 
	// Otherwise, when Active, the calls made here will generate a Commit() 
	// and aAsyncComplete is returned true unless the SetGain() call fails
	if (aBecomingActive || iActiveState == EDevSoundAdaptorActive_Active) 
		{
		if (iCurrentAudioControl==iDevRecordControl)
			{
			// TODO assumes we are not mid Commit cycle
			// we are recing, need to change current volume at A3F layer
			error = iCurrentAudioControl->SetGains(iDevSoundGain, iDevSoundMaxGain, iDevSoundRecordBalance, 0, aBecomingActive);
			if (error==KErrNone)
				{
				aAsyncComplete = ETrue;
				}
			}
		else if (iCurrentAudioControl==iDevPlayControl || iCurrentAudioControl==iDevToneControl)
			{
			// TODO assumes we are not mid Commit cycle
			// we are playing, need to change current volume at A3F layer
			error = iCurrentAudioControl->SetGains(iDevSoundVolume, iDevSoundMaxVolume, iDevSoundPlayBalance, iRampDuration, aBecomingActive);
			if (error==KErrNone)
				{
				aAsyncComplete = ETrue;
				}
			}
		
		} 

		
	DP0_RET(error, "%d");	
	}

// -----------------------------------------------------------------------------
// CDevAudio::GetDevSoundPlayBalance
// -----------------------------------------------------------------------------
//
void CDevAudio::GetDevSoundPlayBalance(TInt& aLeftBalance, TInt& aRightBalance)
	{
	DP_CONTEXT(CDevAudio::GetDevSoundPlayBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aLeftBalance = iDevSoundPlayBalance[KLeftChannel];
	aRightBalance = iDevSoundPlayBalance[KRightChannel];
	DP_OUT();
	}
	

// -----------------------------------------------------------------------------
// CDevAudio::GetDevSoundRecordBalance
// -----------------------------------------------------------------------------
//
void CDevAudio::GetDevSoundRecordBalance(TInt& aLeftBalance, TInt& aRightBalance)
	{
	DP_CONTEXT(CDevAudio::GetDevSoundRecordBalance *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aLeftBalance = iDevSoundRecordBalance[KLeftChannel];
	aRightBalance = iDevSoundRecordBalance[KRightChannel];
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudio::DevSoundSupportedDataTypesL
// -----------------------------------------------------------------------------
//
void CDevAudio::DevSoundSupportedDataTypesL(RArray<TFourCC>& aSupportedDataTypes, TUint aDataType)
	{
	DP_CONTEXT(CDevAudio::DevSoundSupportedDataTypesL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//Clear any existing data
	aSupportedDataTypes.Reset();
	
	if(aDataType == KDataForPlay)
		{
		ConvertToFourCcL(aSupportedDataTypes, iSupportedInputFormats);
		}
	else if(aDataType == KDataForRecord)
		{
		ConvertToFourCcL(aSupportedDataTypes, iSupportedOutputFormats);
		}
	
	DP_OUT();
	}
	
// -----------------------------------------------------------------------------
// CDevAudio::ConvertToFourCcL
// -----------------------------------------------------------------------------
//
void CDevAudio::ConvertToFourCcL(RArray<TFourCC>& aSupportedDataTypes, RArray<TUid>& aSupportedFormats)
	{
	DP_CONTEXT(CDevAudio::ConvertToFourCcL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	if(aSupportedFormats.Count() > 0)
		{
		TFourCC fourCC;
		
		for(TInt element=0; element<aSupportedFormats.Count(); element++)
			{		
			User::LeaveIfError(const_cast<CFourCCConvertor&>(iGlobalProperties->GetFourCCConvertor()).FormatToFourCC(aSupportedFormats[element],fourCC));
			aSupportedDataTypes.AppendL(fourCC);	
			}
		}
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// CDevAudio::CreateAudioProcessingUnits
// -----------------------------------------------------------------------------
//
TInt CDevAudio::CreateAudioProcessingUnits(TUid aSource, TUid aSink, TUid aCodec)
	{
	DP_CONTEXT(CDevAudio::CreateAudioProcessingUnits *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioContext->CreateAudioProcessingUnit(aSource, iAudioSource);
	if(err != KErrNone)
		{
		DP0_RET(err,"Audio source creation failed!");
		}
	err = iAudioContext->CreateAudioProcessingUnit(aSink, iAudioSink);
	if(err != KErrNone)
		{
		DP0_RET(err,"Audio sink creation failed!");
		}
	err = iAudioContext->CreateAudioProcessingUnit(aCodec, iAudioCodec);
	if(err != KErrNone)
		{
		DP0_RET(err,"Audio codec creation failed!");
		}

	DP0_RET(err,"");
	}

// -----------------------------------------------------------------------------
// CDevAudio::DeleteAudioProcessingUnits
// -----------------------------------------------------------------------------
//
void CDevAudio::DeleteAudioProcessingUnits()
	{
	DP_CONTEXT(CDevAudio::DeleteAudioProcessingUnits *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iAudioSource)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioSource);
		iAudioSource = NULL;
		}
	if (iAudioSink)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioSink);
		iAudioSink = NULL;
		}
	if (iAudioCodec)
		{
		iAudioContext->DeleteAudioProcessingUnit(iAudioCodec);
		iAudioCodec = NULL;
		}
	DP_OUT();
	}

TInt CDevAudio::CommitAudioContext()
	{
	DP_CONTEXT(CDevAudio::CommitAudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	//If we are in mid state then Panic as DevSound server-side session (CMMFDevSoundSession) is not blocking properly
	__ASSERT_DEBUG(!IsMidState(iActiveState), Panic(EValidStateBeforeCommit));

	TInt err = KErrNone;
	iPreviousState = iActiveState;
	err = iAudioContext->Commit();

	DP0_RET(err,"%d");
	}

TBool CDevAudio::IsMidState(TDevSoundAdaptorState aAdaptorState)
	{
	DP_CONTEXT(CDevAudio::IsMidState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (aAdaptorState == EDevSoundAdaptorRemovingProcessingUnits || aAdaptorState == EDevSoundAdaptorUninitialising ||
			aAdaptorState == EDevSoundAdaptorInitialising || aAdaptorState == EDevSoundAdaptorLoading ||
			aAdaptorState == EDevSoundAdaptorUnloading || aAdaptorState == EDevSoundAdaptorStopping ||
			aAdaptorState == EDevSoundAdaptorActivating || aAdaptorState == EDevSoundAdaptorPausing)
		{
		DP0_RET(ETrue,"%d");
		}
	DP0_RET(EFalse,"%d");
	}

// -----------------------------------------------------------------------------
// From MA3FDevSoundAutoPauseResume
// CDevAudio::RegisterAsClient
// -----------------------------------------------------------------------------
//
TInt CDevAudio::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(CDevAudio::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNotSupported;
	MA3FDevSoundAutoPauseResume* iAPR =  static_cast<MA3FDevSoundAutoPauseResume*>(iAudioContext->Interface(KUIdAudioResourceNotification));
	if(iAPR)
		{
		err = iAPR->RegisterAsClient(aEventType, aNotificationRegistrationData, this);
		}
	DP0_RET(err, "%d");	
	}
	

// -----------------------------------------------------------------------------
// CDevAudio::CancelRegisterAsClient
// -----------------------------------------------------------------------------
//
TInt CDevAudio::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(CDevAudio::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNotSupported;
	MA3FDevSoundAutoPauseResume* iAPR =  static_cast<MA3FDevSoundAutoPauseResume*>(iAudioContext->Interface(KUIdAudioResourceNotification));
	if(iAPR)
		{
		err = iAPR->CancelRegisterAsClient(aEventType);
		}
	DP0_RET(err, "%d");	
	}


// -----------------------------------------------------------------------------
// CDevAudio::WillResumePlay
// -----------------------------------------------------------------------------
//
TInt CDevAudio::WillResumePlay()
	{
	DP_CONTEXT(CDevAudio::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNotSupported;
	MA3FDevSoundAutoPauseResume* iAPR =  static_cast<MA3FDevSoundAutoPauseResume*>(iAudioContext->Interface(KUIdAudioResourceNotification));
	if(iAPR)
		{
		err = iAPR->WillResumePlay();
		}
	DP0_RET(err, "%d");	
	}


// -----------------------------------------------------------------------------
// CDevAudio::NotifyResume
// -----------------------------------------------------------------------------
//
void CDevAudio::NotifyResume()
	{
	DP_CONTEXT(CDevAudio::NotifyResume *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iAdaptationObserver.CallbackFromAdaptorReceived(KCallbackAutoPauseResume, KErrNone);
	DP_OUT();
	}

void CDevAudio::Panic(TMMFDevAudioPanicCodes aCode)
	{
	User::Panic(KMMFDevAudioPanicCategory, aCode);
	}

// End of file
