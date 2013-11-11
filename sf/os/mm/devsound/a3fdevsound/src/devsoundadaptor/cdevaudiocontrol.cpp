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



#include "cdevaudiocontrol.h"

#include <a3f/audioprocessingunittypeuids.h>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#include <a3f/maudiocontext.h>
#include <a3f/maudiostream.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiogaincontrol.h>


const TInt KMicroSecondsInSecond = 1000000;
const TInt KDefaultBufferSize = 4096;

const TUint KMaxCallbacksExpected = 2;
const TUint KDefaultSampleRate = 8000;
const TUid KDefaultMode = {KA3FModeMonoValue};

class TSampleRateTableEntry
	{
public:
	TInt		iSampleRateValue;
	TMMFSampleRate iSampleRate;
	};

const TSampleRateTableEntry KRateTableLookup[] = { 
							{ 8000, EMMFSampleRate8000Hz },
							{ 11025, EMMFSampleRate11025Hz },
							{ 12000, EMMFSampleRate12000Hz },
							{ 16000, EMMFSampleRate16000Hz },
							{ 22050, EMMFSampleRate22050Hz },
							{ 24000, EMMFSampleRate24000Hz },
							{ 32000, EMMFSampleRate32000Hz },
							{ 44100, EMMFSampleRate44100Hz },
							{ 48000, EMMFSampleRate48000Hz },
							{ 64000, EMMFSampleRate64000Hz },
							{ 88200, EMMFSampleRate88200Hz },
							{ 96000, EMMFSampleRate96000Hz },
						};
const TInt KMaxSampleRateIndex = 11; // must agree with length of table

class TAudioModeTableEntry
	{
public:
	TMMFMonoStereo	iAudioModeValue;
	TUid			iAudioMode;
	};

const TAudioModeTableEntry KModeTableLookup[] = {
							{ EMMFMono, {KA3FModeMonoValue} },
							{ EMMFStereo, {KA3FModeStereoNonInterleavedValue} },
							};

const TInt KMaxModeIndex = 1; // must agree with length of table

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CDevAudioControl::CDevAudioControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevAudioControl::CDevAudioControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Local cache default values
	iCurrentSampleRate = KDefaultSampleRate;
	iCurrentMode = KDefaultMode; 
	iOutstandingCallbacks = KMaxCallbacksExpected; //by default we expect 2 callbacks for capabilities.
	iCallbackFromAdaptor = KCallbackNone;
	
	DP_OUT();
	}


void CDevAudioControl::ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver)
	{
	TRACE_CREATE();
	DP_CONTEXT(CDevAudioControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iDevAudio = aDevAudio; 
	iAdaptationObserver = &aDevSoundObserver;
	
	iObserverRegistered = EFalse;

	TAudioChannelGain left;
	TAudioChannelGain right;
	
	left.iLocation = TAudioChannelGain::ELeft;
	right.iLocation = TAudioChannelGain::ERight;
		
	User::LeaveIfError(iChannelGains.Append(left)); // assumed element 0 in rest of code
	User::LeaveIfError(iChannelGains.Append(right)); // assumed element 1 in rest of code

	// Note this could be delayed now (volume is internal to adaptor until later in cycle)
	// Needed to allow CMMFDevSound::MaxVolume and similar calls before CMMFDevSound::InitializeL
	TAny* interface(NULL);
	interface = iDevAudio->iGainControl->Interface(KUidAudioGainControl);
	iGainControl = static_cast<MAudioGainControl*>(interface);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CDevAudioControl::~CDevAudioControl()
	{
	DP_CONTEXT(CDevAudioControl::~CDevAudioControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iChannelGains.Close();
	iSupportedRates.Close();
	iSupportedModes.Close();
	DP_OUT();
	}
	
// -----------------------------------------------------------------------------
// CacheAudioCodecIf
//
TInt CDevAudioControl::CacheAudioCodecIf()
	{
	DP_CONTEXT(CDevAudioControl::CacheAudioCodecIf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iDevAudio->iAudioCodec==NULL)
		{
		DP0_RET(KErrNotReady,"%d");
		}
	TInt err = KErrNone;
	if (iAudioCodecIf==NULL)
		{
		MAudioCodec* codecInterface = static_cast<MAudioCodec*>(iDevAudio->iAudioCodec->Interface(KUidAudioCodec));
		__ASSERT_ALWAYS (codecInterface, CDevAudioControl::Panic(EAudioCodecIsNull));
		iAudioCodecIf = codecInterface; 
		}
	DP0_RET(err,"%d");
	}


// -----------------------------------------------------------------------------
// CDevAudioControl::Initialize
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::Initialize(TUid /*aFormat*/)
	{
	DP_CONTEXT(CDevAudioControl::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	iObserverRegistered = EFalse;
	DP0_RET(KErrNone,"%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::Uninitialize
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::Uninitialize()
	{
	DP_CONTEXT(CDevAudioControl::Uninitialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Remove pUnits only allowed when stream is uninitialized
	TInt err = iDevAudio->iAudioStream->Uninitialize();

	if (err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		}
	if (err == KErrNone)
		{
		iDevAudio->iActiveState = EDevSoundAdaptorUninitialising;
		}

	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::Unload
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::Unload()
	{
	DP_CONTEXT(CDevAudioControl::Unload *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iDevAudio->iAudioStream->Unload();
	if (err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		}
	if (err == KErrNone)
		{
		iDevAudio->iActiveState = EDevSoundAdaptorUnloading;
		}

	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::GetCapabilities
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::GetCapabilities(TMMFCapabilities& aCap)
	{
	DP_CONTEXT(CDevAudioControl::GetCapabilities *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// At this phase of CAP, we only care about current codec capabilities. 
	// This will be supported as soon as the A3F API changes to support this are ready
	TInt err = KErrNone;
		
	if (iDevAudio->iActiveState != EDevSoundAdaptorCreated_Uninitialised)
		{
		err = CacheAudioCodecIf();
		if (err != KErrNone)
			{
			DP0_RET(err, "%d");			
			}
			
		if (err == KErrNone)
			{
			err = iAudioCodecIf->GetSupportedModes(iSupportedModes);
			if (err == KErrNone)
				{
				aCap.iChannels = GetModes(iSupportedModes);
				err = iAudioCodecIf->GetSupportedSamplesRates(iSupportedRates);
				
				if (err == KErrNone)
					{
					aCap.iRate = GetSampleRates(iSupportedRates);
					}
				else
					{
					//If was a problem getting sampleRates we don´t expect callbacks and return
					iOutstandingCallbacks = 0;
					}
					
				}
			else
				{
				//If was a problem getting modes we don´t expect callbacks and return
				iOutstandingCallbacks = 0;
				}
			}
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::GetConfig
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::GetConfig(TMMFCapabilities& aConfig)
	{
	DP_CONTEXT(CDevAudioControl::GetConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	//TODO add a return error code if the sample rate or the channels are not found
	//We need to transform the local values to a supported value for the client
	aConfig.iRate = static_cast<TMMFSampleRate>(GetSampleRate(iCurrentSampleRate));
	aConfig.iChannels = static_cast<TMMFMonoStereo>(GetMode(iCurrentMode));
	aConfig.iBufferSize = KDefaultBufferSize;
	
	DP2(DLINFO, "rate 0x%x, channels 0x%x", aConfig.iRate, aConfig.iChannels);

	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::SetConfig
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::SetConfig(const TMMFCapabilities& aConfig)
	{
	DP_CONTEXT(CDevAudioControl::SetConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	// TODO need to ensure if iChannels or iSampleRate is outside known values, then
	// the code handles that gracefully and returns the appropriate error code
	
	TInt err(KErrNone);
	TUid mode = KNullUid;
	//Reset the desired values
	iDesiredSampleRate = 0;
	iDesiredMode = KNullUid;

	err = ResolveMode(aConfig.iChannels, mode);
	if (err != KErrNone)
		{
		DP0_RET(err, "%d");
		}

	err = ResolveSampleRate(aConfig.iRate, iDesiredSampleRate);
	if (err != KErrNone)
		{
		DP0_RET(err, "%d");
		}

	// At this phase of CAP, we only care about codec, which checks config against
	// its own capabilities. Verification against stream specific capabilities
	// should be added later on.
	
	err = CacheAudioCodecIf();
	if (err != KErrNone)
		{
		DP0_RET(err, "%d");
		}
	
	err = iAudioCodecIf->SetSampleRate(iDesiredSampleRate);
		
	
	if(err == KErrNone)
		{
		err = iAudioCodecIf->SetMode(mode);
		}
	
	if(err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		if (err == KErrNone)
			{
			iDesiredMode = mode;
			}
		}

	DP0_RET(err, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::ProcessInit
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::ProcessInit()
	{
	DP_CONTEXT(CDevAudioControl::ProcessInit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP0_RET(KErrNone,"%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::ProcessData
// -----------------------------------------------------------------------------
//
void CDevAudioControl::ProcessData()
	{
	DP_CONTEXT(CDevAudioControl::ProcessData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::GetSamples
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::GetSamples()
	{
	DP_CONTEXT(CDevAudioControl::GetSamples *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	TInt samplesplayed(0);
	
	TTimeIntervalMicroSeconds timeProcessed(0);
	err = iDevAudio->iAudioStream->GetStreamTime(timeProcessed);

	DP1(DLINFO,"CDevAudioControl::GetSamples iCurrentSampleRate %d",iCurrentSampleRate);
	
	if(err == KErrNone)
		{
		samplesplayed = (timeProcessed.Int64() * iCurrentSampleRate + KMicroSecondsInSecond/2) / TInt64(KMicroSecondsInSecond);
		}
	//TODO manage the error

	DP0_RET(samplesplayed, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::Stop
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::Stop()
	{
	DP_CONTEXT(CDevAudioControl::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP0_RET(KErrNone, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::Pause
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::Pause()
	{
	DP_CONTEXT(CDevAudioControl::Pause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP0_RET(KErrNone, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::CustomInterface
// -----------------------------------------------------------------------------
//
TAny* CDevAudioControl::CustomInterface(TUid aInterfaceId)
	{
	DP_CONTEXT(CDevAudioControl::CustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAny* ciPtr(NULL);
	TInt err = KErrNone;
	if(iDevAudio->iAudioStream)
		{
		TAny* ptr = iDevAudio->iAudioStream->Interface(KUidExtensionInferface);
		if(ptr)
			{
			MCustomInterfaceSupport* ciSupport  = static_cast<MCustomInterfaceSupport*>(ptr);
			if(!iObserverRegistered)
				{
				err = ciSupport->RegisterObserver(*this);
				if(err == KErrNone)
					{
					iObserverRegistered = ETrue;
					}
				}
			err = ciSupport->RequestCustomInterface(aInterfaceId, ciPtr);
			if( err != KErrNone)
				{
				ciPtr = NULL;
				}
			}
		}
	DP0_RET(ciPtr, "0x%x");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::SetGain
// -----------------------------------------------------------------------------
TInt CDevAudioControl::SetGains(TInt aDevSoundGain, TInt aDevSoundMaxGain, TInt aBalance[2], const TTimeIntervalMicroSeconds& aRampDuration, TBool aBecomingActive)
	{
	DP_CONTEXT(CDevAudioControl::SetGains *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(aDevSoundGain>=0 && aDevSoundGain<=aDevSoundMaxGain); // higher layer assumed to scale
	ASSERT(aBalance[ELeftCh]>=0 && aBalance[ELeftCh]<=100);
	ASSERT(aBalance[ERightCh]>=0 && aBalance[ERightCh]<=100);
	ASSERT(aDevSoundMaxGain>0); // assumed max gain is positive
	TInt a3fMaxGain;
	TInt err = iGainControl->GetMaxGain(a3fMaxGain);
	if (err==KErrNone)
		{
		iLegacyGain = TInt(TReal(aDevSoundGain)*TReal(a3fMaxGain)/TReal(aDevSoundMaxGain)+0.5);
		ASSERT(iLegacyGain>=0 && iLegacyGain<=a3fMaxGain);
		iLegacyLeft = aBalance[ELeftCh];
		iLegacyRight = aBalance[ERightCh];

		MapGains();

		// VolumeRamp is only applied when DevSound is becoming active
		if(err == KErrNone)
			{
			if (aRampDuration > 0 && aBecomingActive)
				{
				err = iGainControl->SetGain(iChannelGains, KUidGainSawTooth, aRampDuration);
				}
			else
				{
				err = iGainControl->SetGain(iChannelGains);
				}
			}

		// This call will result on commit only when we are already active
		// otherwise the changes will be commited by the DevAudioControl 
		// It means we're here due to RequestGainAndBalance call
		if(err == KErrNone && !aBecomingActive)
			{
			err = iDevAudio->CommitAudioContext();
			}		
		}
	DP0_RET(err,"%d");
	}


// -----------------------------------------------------------------------------
// CDevAudioControl::MapAndSetGains
// -----------------------------------------------------------------------------
//
void CDevAudioControl::MapGains()
	{
	DP_CONTEXT(CDevAudioControl::MapGains *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Map legacy values to CAP channel array.
	if ( iLegacyLeft == iLegacyRight )
		{
		iChannelGains[ELeftCh].iGain = iLegacyGain;
		iChannelGains[ERightCh].iGain = iLegacyGain;
		}
	else if ( iLegacyLeft > iLegacyRight )
		{
		iChannelGains[ELeftCh].iGain = iLegacyGain;
		iChannelGains[ERightCh].iGain =
						static_cast<TUint>((iLegacyGain*iLegacyRight)/iLegacyLeft);
		}
	else
		{
		iChannelGains[ERightCh].iGain = iLegacyGain;
		iChannelGains[ELeftCh].iGain =
						static_cast<TUint>((iLegacyGain*iLegacyLeft)/iLegacyRight);
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::DestroyChain
// -----------------------------------------------------------------------------
//
TBool CDevAudioControl::DestroyChain()
	{
	DP_CONTEXT(CDevAudioControl::DestroyChain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = KErrNone;
	TBool readyToDestroy = EFalse;
	switch(iDevAudio->iActiveState)
		{
		case EDevSoundAdaptorActive_Active:
		case EDevSoundAdaptorPaused_Primed:
			{
			err = iDevAudio->iAudioStream->Stop();
			if(err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				}
			if (err == KErrNone)
				{
				iDevAudio->iActiveState = EDevSoundAdaptorStopping;
				}
			}
			break;
		case EDevSoundAdaptorInitialised_Idle:
			{
			err = iDevAudio->iAudioStream->Unload();
			if(err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				}
			if (err == KErrNone)
				{
				iDevAudio->iActiveState = EDevSoundAdaptorUnloading;
				}
			}
			break;
		case EDevSoundAdaptorInitialised_Initialised:
			{
			err = iDevAudio->iAudioStream->Uninitialize();
			if(err == KErrNone)
				{
				err = iDevAudio->CommitAudioContext();
				}
			if (err == KErrNone)
				{
				iDevAudio->iActiveState = EDevSoundAdaptorUninitialising;
				}
			}
			break;
		case EDevSoundAdaptorCreated_Uninitialised:
			readyToDestroy = ETrue;
			break;
		case EDevSoundAdaptorUnitialised_Uninitialised:
			//If following condition is true, then we are here because of a
			//pre-emption clash in last Commit cycle started from
			//CDevCommonControl::ContextEventUpdateWithStateEventNoError.
			if(iDevAudio->iPreviousState == EDevSoundAdaptorRemovingProcessingUnits)
				{
				err = RemoveProcessingUnits();
				break;
				}
		default:
			break;
		}
	
	// Destroy sequence fail!
	if(err != KErrNone)
		{
		DP0(DLINFO, "================ Destroy sequence fail! ================");
		readyToDestroy = ETrue;
		}
	// Set the flag only when needed
	iDevAudio->iClosing = !readyToDestroy;
	DP0_RET(readyToDestroy,"%d");
	}


// -----------------------------------------------------------------------------
// CDevAudioControl::RemoveProcessingUnits
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::RemoveProcessingUnits()
	{
	DP_CONTEXT(CDevAudioControl::RemoveProcessingUnits *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Remove pUnits only allowed when stream is uninitialized
	TInt err = iDevAudio->iAudioStream->RemoveProcessingUnit(iDevAudio->iAudioSource);
		
	if(err == KErrNone)
		{
		err = iDevAudio->iAudioStream->RemoveProcessingUnit(iDevAudio->iAudioSink);	
		}
	if(err == KErrNone)
		{
		err = iDevAudio->iAudioStream->RemoveProcessingUnit(iDevAudio->iAudioCodec);
		}

	if (err == KErrNone)
		{
		err = iDevAudio->CommitAudioContext();
		}
		
	if(err == KErrNone)
		{
		iDevAudio->iActiveState = EDevSoundAdaptorRemovingProcessingUnits;		
		}
	DP0_RET(err,"%d");
	}

// -----------------------------------------------------------------------------
// From class MAudioStreamObserver
// CDevAudioControl::StateEvent
// -----------------------------------------------------------------------------
//
void CDevAudioControl::StateEvent(MAudioStream& /*aStream*/, 
									TInt /*aReason*/, 
									TAudioState /*aNewState*/)
	{
	DP_CONTEXT(CDevAudio::StateEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioStreamObserver
// CDevAudioControl::AddProcessingUnitComplete
// -----------------------------------------------------------------------------
//
void CDevAudioControl::AddProcessingUnitComplete(MAudioStream& /*aStream*/, 
												MAudioProcessingUnit* /*aInstance*/,
												TInt /*aError*/)
	{
	}

// -----------------------------------------------------------------------------
// From class MAudioStreamObserver
// CDevAudioControl::RemoveProcessingUnitComplete
// -----------------------------------------------------------------------------
//
void CDevAudioControl::RemoveProcessingUnitComplete(MAudioStream& /*aStream*/,
													MAudioProcessingUnit* /*aInstance*/,
													TInt /*aError*/)
	{
	}

// -----------------------------------------------------------------------------
// From class MAudioStreamObserver
// CDevAudioControl::ProcessingFinished
// -----------------------------------------------------------------------------
//
void CDevAudioControl::ProcessingFinished(MAudioStream& /*aStream*/)
	{
	}

// -----------------------------------------------------------------------------
// From class MAudioStreamObserver
// CDevAudioControl::FlushComplete
// -----------------------------------------------------------------------------
//
void CDevAudioControl::FlushComplete (MAudioStream& /*aStream*/, TInt aError)
	{
	// no action needed - should complete as part of the ContextEvent.
	// otherwise could callback.
	TInt err = KErrNone;
	
	if(iPauseResumeSequenceDueToEmptyBuffers)
		{
		// Flush operation failed
		if(aError != KErrNone)
			{
			iPauseResumeSequenceDueToEmptyBuffers = EFalse;
			iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, aError);
			}
		// Handle throw-off scenarios, resume is not possible from here
		// 1. ProcessingFinished has occurred
		// 2. Preemption occurred 
		else if(iCallbackFromAdaptor != KCallbackNone || 
			iDevAudio->iActiveState != EDevSoundAdaptorPaused_Primed)
			{
			iPauseResumeSequenceDueToEmptyBuffers = EFalse;
			iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, KErrNone);
			
			}
		else
			{
			err = Resume();
			if(err != KErrNone)
				{
				iPauseResumeSequenceDueToEmptyBuffers = EFalse;
				iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, aError);
				}
			// Once ContextEvent be received 
			// EmptyBuffers can be considered completed
			}
		}
	// EmptyBuffers operation has concluded here
	// we didn't go through pause - resume sequence
	else
		{
		iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, aError);
		}
	}
	
// -----------------------------------------------------------------------------
// From class MAudioGainControlObserver
// CDevAudioControl::MaxRampTimeChanged
// -----------------------------------------------------------------------------
//
void CDevAudioControl::MaxRampTimeChanged(MAudioGainControl& /*aGain*/)
	{
	DP_CONTEXT(CDevAudioControl::MaxRampTimeChanged *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// this is not cached, no actions needed
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioGainControlObserver
// CDevAudioControl::MaxGainChanged
// -----------------------------------------------------------------------------
//
void CDevAudioControl::MaxGainChanged(MAudioGainControl& /*aGain*/)
	{
	DP_CONTEXT(CDevAudioControl::MaxGainChanged *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// this is not cached, no actions needed
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioGainControlObserver
// CDevAudioControl::GainChanged
// -----------------------------------------------------------------------------
//
void CDevAudioControl::GainChanged(MAudioGainControl& aGain, TInt aError)
	{
	DP_CONTEXT(CDevAudioControl::GainChanged *CD1*, CtxDevSound, DPLOCAL);
	DP1_IN("aError=%d", aError);

	if(aError != KErrNone)
		{
		// Either our request failed, or MMRC has forced some values, and we
		// have to update local values.
		aGain.GetGain(iChannelGains);
		ASSERT(iChannelGains.Count()==2);
		// Map CAP channel array to legacy values. 
		// assumption: left%+right%=100
		if ( iChannelGains[ELeftCh].iGain == iChannelGains[ERightCh].iGain )
			{
			iLegacyGain = iChannelGains[ELeftCh].iGain;
			iLegacyLeft = 50;
			iLegacyRight = 50;
			}
		else if ( iChannelGains[ELeftCh].iGain > iChannelGains[ERightCh].iGain )
			{
			iLegacyGain = iChannelGains[ELeftCh].iGain;
			iLegacyLeft = static_cast<TUint>
				((100*iLegacyGain)/(iLegacyGain+iChannelGains[ERightCh].iGain));
			iLegacyRight = 100 - iLegacyLeft;
			//(not that accurate, but sufficient for now)
			}
		else
			{
			iLegacyGain = iChannelGains[ERightCh].iGain;
			iLegacyRight = static_cast<TUint>
				((100*iLegacyGain)/(iLegacyGain+iChannelGains[ELeftCh].iGain));
			iLegacyLeft = 100 - iLegacyRight;
			}

		DP3(DLINFO,"New values :iLegacyGain %d, iLegacyLeft %d, iLegacyRight %d",
				iLegacyGain,iLegacyLeft,iLegacyRight);
		}
	else
		{
		// our request completed succesfully, no need to update cached values
		// just print values in debug version
		#ifdef _DEBUG
		RArray<TAudioChannelGain> gains;
		TUint left;
		TUint right;
		TUint gain;
		aGain.GetGain(gains);
		ASSERT(gains.Count()==2);
		if ( iChannelGains[ELeftCh].iGain == iChannelGains[ERightCh].iGain )
			{
			gain = iChannelGains[ELeftCh].iGain;
			left = 50;
			right = 50;
			}
		else if ( iChannelGains[ELeftCh].iGain > iChannelGains[ERightCh].iGain )
			{
			gain = iChannelGains[ELeftCh].iGain;
			left = 
				static_cast<TUint>((100*gain)/(gain+iChannelGains[ERightCh].iGain));
			right = 100 - left;
			}
		else
			{
			gain = iChannelGains[ERightCh].iGain;
			right = 
				static_cast<TUint>((100*gain)/(gain+iChannelGains[ELeftCh].iGain));
			left = 100 - right;
			}
		gains.Close();
		DP3(DLINFO,"KErrNone (gain %d, left %d, right %d)", gain,left,right);
		#endif
		}

	DP_OUT();
	}

// -----------------------------------------------------------------------------
// From class MAudioCodecObserver
// CDevAudioControl::SampleRateSet
// -----------------------------------------------------------------------------
void CDevAudioControl::SampleRateSet(TInt aError)
	{
	if(aError==KErrNone)
		{
		//Review if we call SetConfig or is only the first time that we load the codec
		if (iDesiredSampleRate > 0)
			{
			iCurrentSampleRate = iDesiredSampleRate;
			}
		}
	else
	    {
        iAdaptationObserver->NotifyError(aError);
	    }
    iDesiredSampleRate = 0;
	}

// -----------------------------------------------------------------------------
// From class MAudioCodecObserver
// CDevAudioControl::ModeSet
// -----------------------------------------------------------------------------
void CDevAudioControl::ModeSet(TInt aError)
	{
	if(aError==KErrNone)
		{
		//Review if we call SetConfig or is only the first time that we load the codec
		if (iDesiredMode != KNullUid)
			{
			iCurrentMode = iDesiredMode;
			}
		}
    else
        {
        iAdaptationObserver->NotifyError(aError);
        }
    iDesiredMode = KNullUid;
	}

// -----------------------------------------------------------------------------
// From class MAudioCodecObserver
// CDevAudioControl::GetSupportedSampleRatesComplete
// -----------------------------------------------------------------------------
void CDevAudioControl::GetSupportedSampleRatesComplete(TInt aError)
	{
	iSupportedRates.Reset();
	CompleteMessageCap(aError);
	}

// -----------------------------------------------------------------------------
// From class MAudioCodecObserver
// CDevAudioControl::GetSupportedModesComplete
// -----------------------------------------------------------------------------
void CDevAudioControl::GetSupportedModesComplete(TInt aError)
	{
	iSupportedModes.Reset();
	CompleteMessageCap(aError);
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::CompleteMessageCap
// -----------------------------------------------------------------------------
void CDevAudioControl::CompleteMessageCap(TInt aError)
	{
	if (iOutstandingCallbacks > 1) //waiting until the 2 outstanding callbacks arrival.
		{
		iOutstandingCallbacks--;
		iError = aError; //keeping the error.
		}
	else
		{
		if (iError == KErrNone)
			{
			iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
			}
		else
			{
			iAdaptationObserver->AsynchronousOperationComplete(iError, ETrue);
			}
		iError = KErrNone;
		iOutstandingCallbacks = KMaxCallbacksExpected;
		}
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::SetToneData
// -----------------------------------------------------------------------------
//
TInt CDevAudioControl::SetToneData(TToneData& /*aToneData*/)
	{
	DP_CONTEXT(CDevAudioControl::SetToneData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotSupported, "%d");
	}


// -----------------------------------------------------------------------------
// From class MAudioContextObserver
// CDevAudio::ContextEvent
// -----------------------------------------------------------------------------
//
void CDevAudioControl::ContextEvent(TUid aEvent, TInt aError)
	{
	DP_CONTEXT(CDevAudioControl::ContextEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

    if(!(iAdaptationObserver->AdaptorControlsContext()))
        {
        iIgnoreAsyncOpComplete = ETrue;
        }
    
	if (aEvent == KUidA3FContextUpdateComplete)
		{
	    if(iIgnoreAsyncOpComplete)
			{
            iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
		    iIgnoreAsyncOpComplete = EFalse;
   	    	}
        else
            {
            iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
           	}
		}
	else if(aEvent == KUidA3FContextPreEmption)
		{
		//If we are in a normal pre-emption cycle, we should not be in a mid-state.
		__ASSERT_DEBUG(!iDevAudio->IsMidState(iDevAudio->iActiveState), Panic(EInvalidStateDuringPreemptionCycle));
		iIgnoreAsyncOpComplete = ETrue;
		iAdaptationObserver->PreemptionStartedCallbackReceived();
		}
	//In a clashing pre-emption cycle we must be in a commit cycle, so do nothing here - CDevCommonControl deals
	//with this case.
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// From class MCustomInterfaceSupportObserver
// CDevAudio::CustomInterfaceRemoval
// -----------------------------------------------------------------------------
//
void CDevAudioControl::CustomInterfaceRemoval(TUid aUid, TAny* /*aPtr*/)
	{
	DP_CONTEXT(CDevAudioControl::CustomInterfaceRemoval *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// TODO: Review this functionality
	iAdaptationObserver->InterfaceDeleted(aUid);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::ResolveSampleRate
// ---------------------------------------------------------------------------
TInt CDevAudioControl::ResolveSampleRate(TInt aSampleRate, TInt& aSampleRateValue)
	{
	DP_CONTEXT(CDevAudioControl::ResolveSampleRate, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrArgument);

	for (TUint i=0; i<=KMaxSampleRateIndex; i++)
		{
		if(KRateTableLookup[i].iSampleRate == aSampleRate)
			{
			aSampleRateValue = KRateTableLookup[i].iSampleRateValue;
			err = KErrNone;
			break;
			}
		}

	//To avoid the value return a non desired value.
	if (err != KErrNone)
		{
		aSampleRateValue = 0;
		}

	DP0_RET(err, "%d");
	}


// ---------------------------------------------------------------------------
// CDevAudioControl::ResolveMode
// ---------------------------------------------------------------------------
TInt CDevAudioControl::ResolveMode(TUint aModeValue, TUid& aMode)
	{
	DP_CONTEXT(CDevAudioControl::ResolveMode *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrArgument);

	
	for (TInt i=0; i<=KMaxModeIndex; i++)
		{
		const TAudioModeTableEntry& entry = KModeTableLookup[i];
		if (entry.iAudioModeValue == aModeValue)
			{
			aMode = entry.iAudioMode;
			err = KErrNone;
			break;
			}
		}
	
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::GetModes
// ---------------------------------------------------------------------------
TUint CDevAudioControl::GetModes(const RArray<TUid>& aMode)
	{
	DP_CONTEXT(CDevAudioControl::GetModes *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TUint result = 0;
	TInt count = aMode.Count();

	for (TInt i=0; i<count; i++)
		{
		result |= GetMode(aMode[i]);
		}
	DP0_RET(result,"%d");
	}


// ---------------------------------------------------------------------------
// CDevAudioControl::GetMode
// ---------------------------------------------------------------------------
TUint CDevAudioControl::GetMode(TUid aMode)
	{
	DP_CONTEXT(CDevAudioControl::GetMode *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TUint result = 0;
	
	for (TInt e=0; e<=KMaxModeIndex; e++)
		{
		if(KModeTableLookup[e].iAudioMode == aMode)
			{
			result = KModeTableLookup[e].iAudioModeValue;
			break;
			}
		}
	DP0_RET(result,"%d");
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::GetSampleRates
// ---------------------------------------------------------------------------
TUint CDevAudioControl::GetSampleRates(const RArray<TInt>& aSampleRates)
	{
	DP_CONTEXT(CDevAudioControl::GetSampleRates *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TUint result = 0;
	TInt count = aSampleRates.Count();
	
	for (TInt i=0; i<count; i++)
		{
		result |= GetSampleRate(aSampleRates[i]);
		}
	DP0_RET(result,"%d");
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::GetSampleRate
// ---------------------------------------------------------------------------
TUint CDevAudioControl::GetSampleRate(TInt aSampleRates)
	{
	DP_CONTEXT(CDevAudioControl::GetSampleRate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TUint result = 0;
	TInt position = 0;
	TInt lowerbound = 0;
	TInt upperbound = KMaxSampleRateIndex;

	if ((aSampleRates < KRateTableLookup[lowerbound].iSampleRateValue) || (aSampleRates > KRateTableLookup[upperbound].iSampleRateValue)) 
		{
		//value request not found in the array.
		DP0_RET(result,"%d");
		}

	//Binary Search
	position = ( lowerbound + upperbound) / 2;

	while((KRateTableLookup[position].iSampleRateValue != aSampleRates) && (lowerbound <= upperbound))
		{
		if (KRateTableLookup[position].iSampleRateValue > aSampleRates)
			{
			upperbound = position - 1;
			}
		else
			{
			lowerbound = position + 1;
			}
		position = (lowerbound + upperbound) / 2;
		}

	result = KRateTableLookup[position].iSampleRate;

	DP0_RET(result,"%d");
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::ProcessingFinishedReceived
// ---------------------------------------------------------------------------
TInt CDevAudioControl::ProcessingFinishedReceived(TBool& /*aAyncOperation*/)
	{
	return KErrNone;
	}
	
// ---------------------------------------------------------------------------
// CDevAudioControl::ProcessingError
// ---------------------------------------------------------------------------
TInt CDevAudioControl::ProcessingError(TBool& /*aAyncOperation*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CDevAudioControl::RequestEmptyBuffers
// ---------------------------------------------------------------------------	
TInt CDevAudioControl::RequestEmptyBuffers()
	{
	DP_CONTEXT(CDevAudioControl::RequestEmptyBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err(KErrNotReady);

	if(iDevAudio)
		{
		if(iDevAudio->iActiveState == EDevSoundAdaptorPaused_Primed)
			{
			err = iDevAudio->iAudioStream->Flush();
			}
		else if (iDevAudio->iActiveState == EDevSoundAdaptorActive_Active)
			{
			err = Pause();
			if(err == KErrNone)
				{
				iPauseResumeSequenceDueToEmptyBuffers = ETrue;
				}
			
			}
		}
	DP0_RET(err,"%d");
	};

void CDevAudioControl::Panic(TDevSoundAdaptorPanicCode aCode)
	{
	_LIT(KMMFDevSoundAdaptorPanicCategory, "DevSoundAdaptor");
	User::Panic(KMMFDevSoundAdaptorPanicCategory, aCode);
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::GetTimePlayed
// ---------------------------------------------------------------------------
TInt CDevAudioControl::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	DP_CONTEXT(CDevAudioControl::GetTimePlayed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iDevAudio->iAudioStream->GetStreamTime(aTime);
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// CDevAudioControl::Resume
// ---------------------------------------------------------------------------
TBool CDevAudioControl::Resume()
	{
	DP_CONTEXT(CDevAudioControl::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(EFalse);
	DP0_RET(KErrNone, "%d");
	}

// -----------------------------------------------------------------------------
// CDevAudioControl::BufferErrorEvent
// -----------------------------------------------------------------------------
//
void CDevAudioControl::BufferErrorEvent()
	{
	ASSERT(EFalse); //This should never happen
	}
// End of file
