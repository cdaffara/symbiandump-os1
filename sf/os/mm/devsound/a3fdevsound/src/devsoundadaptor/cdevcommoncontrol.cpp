// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cdevcommoncontrol.h"
#include <a3f/maudiocontext.h> 
#include <a3f/maudiocodec.h>


CDevCommonControl::CDevCommonControl()
    {
    TRACE_CREATE();
    DP_CONTEXT(CDevCommonControl::CDevCommonControl, CtxDevSound, DPLOCAL);
    DP_IN();
    DP_OUT();
    }


CDevCommonControl::~CDevCommonControl()
    {
    DP_CONTEXT(CDevCommonControl::~CDevCommonControl, CtxDevSound, DPLOCAL);
    DP_IN();
    DP_OUT();
    }


TInt CDevCommonControl::Stop() // from CDevAudioControl
    {
    DP_CONTEXT(CDevCommonControl::Stop, CtxDevSound, DPLOCAL);
    DP_IN();

    TInt err = KErrNone;
    switch(iDevAudio->iActiveState)
        {
        case EDevSoundAdaptorActive_Active:
        case EDevSoundAdaptorPaused_Primed:
            err = iDevAudio->iAudioStream->Stop();
            if (err == KErrNone)
                {
                err = iDevAudio->CommitAudioContext();
                }
            if (err == KErrNone)
                {
                iDevAudio->iActiveState = EDevSoundAdaptorStopping;
                }
            break;
        case EDevSoundAdaptorGoingActive:
            iDevAudio->iActiveState = EDevSoundAdaptorStopping;
            break;
        case EDevSoundAdaptorInitialised_Idle:
        	{
        	//If following condition is true, then we are here because of a
        	//pre-emption clash in last Commit cycle started from
        	//CDevCommonControl::ContextEventUpdateWithStateEventNoError.
        	if(iDevAudio->iPreviousState == EDevSoundAdaptorUnloading)
        		{
				err = Unload();
				break;
        		}
        	}
        default:
            break;
        }
    
    if(err == KErrNone)
        {
        iDevAudio->iStop = ETrue;
        }
    
    DP0_RET(err,"%d");
    }


TInt CDevCommonControl::Pause() // from CDevAudioControl
    {
    DP_CONTEXT(CDevCommonControl::Pause, CtxDevSound, DPLOCAL);
    DP_IN();

    TInt err = iDevAudio->iAudioStream->Prime();
    if ( err == KErrNone)
        {
        err = iDevAudio->CommitAudioContext();
        }
    if (err == KErrNone)
        {
        iDevAudio->iActiveState = EDevSoundAdaptorPausing;
        }
    
    DP0_RET(err,"%d");
    }


TInt CDevCommonControl::Resume() // from CDevAudioControl
    {
    DP_CONTEXT(CDevCommonControl::Resume, CtxDevSound, DPLOCAL);
    DP_IN();
    
    TInt err = KErrNone;

    //If following condition is true, then we are here because of a
    //pre-emption clash in last Commit cycle started from
    //CDevCommonControl::ContextEventUpdateWithStateEventAndError.
    if(iDevAudio->iActiveState == EDevSoundAdaptorInitialised_Idle &&
    		iDevAudio->iPreviousState == EDevSoundAdaptorUnloading)
    	{
		err = Unload();
		DP0_RET(err,"%d");
    	}
    else if(iDevAudio->iActiveState != EDevSoundAdaptorPaused_Primed)
        {
        DP0_RET(KErrNotReady, "%d");
        }

    if(err == KErrNone)
        {
        // Populate gain and balance values set in the Paused state and being cached
        err = iDevAudio->RequestGainAndBalance(this);
        }
    if(err == KErrNone)
        {
        err = iDevAudio->iAudioStream->Activate();
        }
    if ( err == KErrNone)
        {
        err = iDevAudio->CommitAudioContext();
        }
    if (err == KErrNone)
        {
        iDevAudio->iActiveState = EDevSoundAdaptorActivating;
        }
    
    DP0_RET(err,"%d");
    }


void CDevCommonControl::StateEvent(MAudioStream& aStream, TInt aError,  // from MAudioProcessingUnitObserver
                                    TAudioState aNewState)
    {
    DP_CONTEXT(CDevCommonControl::StateEvent, CtxDevSound, DPLOCAL);
    DP3_IN("activeState %d -> newstate %d, (%d)",
            iDevAudio->iActiveState, aNewState, aError);
    
    __ASSERT_ALWAYS(iDevAudio->iAudioStream == &aStream, Panic(EStreamMismatch));
    
    if(aError != KErrNone || iDevAudio->iActiveStreamState != aNewState) 
        {
        iDevAudio->iActiveStreamState = aNewState;
        iStateEventReceived = ETrue;
        }
    // Since the audiostream already demoted the state for the most of the cases
    // only is need  when a error comes were the stream was at the middle of A3f two-phase transition
    else
        {
        switch (aNewState)
            {
            case EIdle:
                // Demote the stream state
                iDevAudio->iActiveStreamState = EIdle;
                break;
            default:
                break;
            }
        }
    iStateEventError = aError;
    
    DP_OUT();
    }


void CDevCommonControl::ProcessingUnitError(MAudioProcessingUnit* /*aInstance*/, // from MAudioProcessingUnitObserver
                                                                 TInt aError)
    {
    DP_CONTEXT(CDevCommonControl::ProcessingUnitError, CtxDevSound, DPLOCAL);
    DP_IN();

    if(iCallbackFromAdaptor == KCallbackNone)   
        {
        iProcessingUnitError = aError;
        iCallbackFromAdaptor = KCallbackProcessingUnitError;
        iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackProcessingUnitError, aError);
        }
    else
        {
        // Multiple callbacks from adaptor
        DP0(DLINFO, "Multiple callbacks from adaptor");
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEvent(TUid aEvent, TInt aError) 
    {
    DP_CONTEXT(CDevCommonControl::ContextEvent, CtxDevSound, DPLOCAL);
    DP3_IN("ContextEvent aEvent=%x iActiveState=%d aError=%d",aEvent, iDevAudio->iActiveState, aError);

    // Can't "switch {...}" on UIDs unfortunately:
    if (aEvent == KUidA3FContextUpdateComplete)
        {
        if(iBeingPreempted)
            {
			if(iStateEventReceived)
				{
				//use a sub state pattern to make pre-emption cycles like other cycles.
				DP1(DLERR,"Preemption error=%d", aError);
				iDevAudio->iActiveState = EDevSoundAdaptorBeingPreempted;
				if(iPreemptionClash)
					{
					// remove last request from front of queue without processing it
					iAdaptationObserver->PreemptionClashWithStateChange();
					iPreemptionClash=EFalse;
					}
				}
            else if(!iStateEventReceived && iPreemptionClash)
        		{
				iIgnoreAsyncOpComplete=ETrue;
				iPreemptionClash=EFalse;
        		}
			iBeingPreempted=EFalse;
            }
		ContextEventUpdateComplete(aError);
        }

    else if ((aEvent == KUidA3FContextCommitUpdate))
        {
        iBeingPreempted=EFalse; // clear being preempted
        iPreemptionClash=EFalse; // clear pre-emption clash flag
        TBool adaptorControlsContext = iAdaptationObserver->AdaptorControlsContext();
        iIgnoreAsyncOpComplete = !adaptorControlsContext;
            // if we don't control context, always send a PreemptionFinishedCallbackReceived()
		iStateEventReceived=EFalse;
        }

    else if (aEvent == KUidA3FContextPreEmption)
        {
        // clear iBeingPreepted - will be set in ContextEventPreEmption if req
        iBeingPreempted=EFalse;
        iPreemptionClash=EFalse; // clear pre-emption clash flag
        TBool adaptorControlsContext = iAdaptationObserver->AdaptorControlsContext();
		iStateEventReceived=EFalse;
        iIgnoreAsyncOpComplete=EFalse; // clear being iIgnoreAsyncOpComplete
        ContextEventPreEmption(aEvent, aError);
        if (!adaptorControlsContext)
            {
            iIgnoreAsyncOpComplete = ETrue; // if we don't control context never do AsyncOpComplete
            }
        }
    else if (aEvent == KUidA3FContextPreEmptedCommit)
        {
		DP0(DLINFO,"KUidA3FContextPreEmptedCommit event received, thus clash with Pre-emption");
        // clear iBeingPreepted - will be set in ContextEventPreEmption if req
        iBeingPreempted=EFalse;
        TBool adaptorControlsContext = iAdaptationObserver->AdaptorControlsContext();
        if (adaptorControlsContext)
        	{
			// push current request that was being processed onto front of queue.
        	iAdaptationObserver->PreemptionClash();
        	iPreemptionClash=ETrue;
        	}
		iStateEventReceived=EFalse;
        iIgnoreAsyncOpComplete=EFalse; // clear being iIgnoreAsyncOpComplete
        ContextEventPreEmption(aEvent, aError);
        if (!adaptorControlsContext)
            {
            iIgnoreAsyncOpComplete = ETrue; // if we don't control context never do AsyncOpComplete
            }
        }

    else if (aEvent == KUidA3FContextAbort)
        {
        ContextEventAbort(aError);
        }

    DP_OUT();
    }


void CDevCommonControl::ContextEventAsynchronousPlayCompletion(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventAsynchronousPlayCompletion, CtxDevSound, DPLOCAL);
    DP_IN();
    
	iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
		
    if (aError)
        {
        FinishWithError(aError);
        }
           
    DP_OUT();
    }


void CDevCommonControl::ContextEventAsynchronousInitializeComplete(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventAsynchronousInitializeComplete, CtxDevSound, DPLOCAL);
    DP_IN();
    
    iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
    iAdaptationObserver->InitializeComplete(aError);
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventUpdateComplete(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventUpdateComplete, CtxDevSound, DPLOCAL);
    DP_IN();
    
    if (iStateEventReceived)
        {
        iStateEventReceived = EFalse;
        DP0(DLINFO,"Context event for that does involve state change");         
        
        if (aError)
            {
            ContextEventUpdateWithStateEventAndError(aError);
            }
        else
            {
            ContextEventUpdateWithStateEventNoError();
            }   
        DP_OUT();
        return;
        }
    
    DP0(DLINFO,"Context event for that doesn't involve state change");

    if (aError)
        {
        ContextEventUpdateWithoutStateEventButWithError(aError);
        }
    else
        {
        ContextEventUpdateWithoutStateEventNoError();
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventPreEmption(TUid aEvent, TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventPreEmption, CtxDevSound, DPLOCAL);
    DP_IN();
    
    DP1(DLERR,"Preemption error=%d", aError);
    CDevAudioControl::ContextEvent(aEvent, aError);
    iBeingPreempted=ETrue;
        
    DP_OUT();
    }


void CDevCommonControl::ContextEventAbort(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventAbort, CtxDevSound, DPLOCAL);
    DP_IN();
    
    DP1(DLERR,"Abort error=%d", aError);
    FinishWithError(aError==KErrAbort ? KErrDied:aError);
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStopDevSoundNotifications() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventStopDevSoundNotifications, CtxDevSound, DPLOCAL);
    DP_IN();
    
    iDevAudio->iAudioStream->UnregisterAudioStreamObserver(*this);
    iGainControl->UnregisterAudioGainControlObserver(*this);
    iAudioCodecIf->UnregisterAudioCodecObserver(*this);
    iAudioCodecIf = NULL;
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventPauseResumeSequenceDueToEmptyBuffers(TBool aFlush) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventPauseResumeSequenceDueToEmptyBuffers, CtxDevSound, DPLOCAL);
    DP_IN();
    
    TInt err(KErrNone);

    if (iPauseResumeSequenceDueToEmptyBuffers)
        {
        if (aFlush)
            {
            err = iDevAudio->iAudioStream->Flush();
            }
        
        if ((err) or (aFlush==EFalse))
            {
            iPauseResumeSequenceDueToEmptyBuffers = EFalse;
            iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, err);
            }
        }
    else
        {
        iAdaptationObserver->AsynchronousOperationComplete(KErrNone, ETrue);    
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStateDevSoundAdaptorUnloading() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventStateDevSoundAdaptorUnloading, CtxDevSound, DPLOCAL);
    DP_IN();
    
    // Due destruction sequence or reinitialization
    if (iDevAudio->iClosing or iDevAudio->iReinitializing)
        {
        TInt err = Uninitialize();
        if (err and iDevAudio->iReinitializing)
            {
            ContextEventAsynchronousInitializeComplete(err);
    
            }
        
        DP_OUT();
        return;
        }

    // Notify the user that ProcessingFinished is complete. 
    // Stop call complete, sent callback.
    if (iCallbackFromAdaptor != KCallbackNone)
        {
        iAdaptationObserver->AsynchronousOperationComplete(KErrNone, ETrue);
        
        if (iCallbackFromAdaptor == KCallbackProcessingFinished)
            {
            FinishWithError(KErrUnderflow);
            }
        else if (iCallbackFromAdaptor == KCallbackProcessingUnitError)
            {
            FinishWithError(iProcessingUnitError);
            }
        
        iCallbackFromAdaptor = KCallbackNone;
        DP_OUT();
        return;
        }
    
    // Error condition
    if (iErrorCondition)
        {
        FinishWithError(iErrorCondition);
        iErrorCondition = KErrNone;
        }
    else
        {
    
        iAdaptationObserver->AsynchronousOperationComplete(KErrNone, ETrue);
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStateDevSoundAdaptorLoading() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventStateDevSoundAdaptorLoading, CtxDevSound, DPLOCAL);
    DP_IN();
    
    iDevAudio->RequestGainAndBalance(this); // TODO handle error

    TInt err = iDevAudio->iAudioStream->Activate();
    if (err)
        {
        DP_OUT();
        return;
        }
    
    err = iDevAudio->CommitAudioContext();
    if (err)
        {
        ContextEventAsynchronousPlayCompletion(err);
        DP_OUT();
        return;
        }
    
    iDevAudio->iActiveState = EDevSoundAdaptorActivating;
    iAdaptationObserver->AsynchronousOperationComplete(KErrNone, EFalse);
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStateDevSoundAdaptorStopping() // from CDevCommonControl
    {   
    DP_CONTEXT(CDevCommonControl::ContextEventStateDevSoundAdaptorStopping, CtxDevSound, DPLOCAL);
    DP_IN();
    
    TInt err = Unload();
    if (err)
        {
        DP0(DLINFO,"Commit failed during stopping");
        FinishWithError(err);
        }
    
    __ASSERT_DEBUG(err==KErrNone, Panic(ECommitFailedDuringStop));
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStateDevSoundAdaptorBeingPreempted() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventStateDevSoundAdaptorBeingPreempted, CtxDevSound, DPLOCAL);
    DP_IN();
    
    __ASSERT_DEBUG(iDevAudio->iActiveStreamState == EInitialized, Panic(EStreamBeingDemotedToEIdle));
    FinishWithError(KErrInUse);
    if (iIgnoreAsyncOpComplete)
        {
        iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
        iIgnoreAsyncOpComplete=EFalse;
        DP_OUT();
        return;
        }
    
    ContextEventPauseResumeSequenceDueToEmptyBuffers(EFalse);
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventStateDevSoundAdaptorUninitializing() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventStateDevSoundAdaptorUninitializing, CtxDevSound, DPLOCAL);
    DP_IN();
    TInt err = RemoveProcessingUnits();

    if (err == KErrNone)
        {
        iDevAudio->iActiveState = EDevSoundAdaptorRemovingProcessingUnits;
        }
    else if (iDevAudio->iReinitializing)
        {
        ContextEventAsynchronousInitializeComplete(err);
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventErrorStateDevSoundAdaptorActivating(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventErrorStateDevSoundAdaptorActivating, CtxDevSound, DPLOCAL);
    DP_IN();

    // If the resume operation fails as result of EmptyBuffers
    // Notify about operation complete through CallbackFromAdaptorReceived
    // and continue to allow client to receive PlayError()
    if (iPauseResumeSequenceDueToEmptyBuffers)
        {
        iPauseResumeSequenceDueToEmptyBuffers = EFalse;
        iAdaptationObserver->CallbackFromAdaptorReceived(KCallbackFlushComplete, KErrNone);
        }
    
    iErrorCondition = aError;
    
    TInt err = Unload();
    if (err)
        {
        DP0(DLINFO,"Commit failed during stopping");
        FinishWithError(err);
        }
    __ASSERT_DEBUG(err==KErrNone, Panic(ECommitFailedDuringStop));
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventErrorStateDevSoundAdaptorBeingPreempted() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventErrorStateDevSoundAdaptorBeingPreempted, CtxDevSound, DPLOCAL);
    DP_IN();
    
__ASSERT_DEBUG(iDevAudio->iActiveStreamState == EInitialized, Panic(EStreamBeingDemotedToEIdle));
    FinishWithError(KErrInUse);

    if(iIgnoreAsyncOpComplete)
        {
        iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
        }
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventUpdateWithoutStateEventNoError() // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventUpdateWithoutStateEventNoError, CtxDevSound, DPLOCAL);
    DP2_IN("iActiveState=%d iIgnoreAsyncOpComplete=%d",iDevAudio->iActiveState, iIgnoreAsyncOpComplete);
    
    if (iDevAudio->iActiveState != EDevSoundAdaptorRemovingProcessingUnits)
        {
	    if (iIgnoreAsyncOpComplete)
	      {
	      iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
	      iIgnoreAsyncOpComplete = EFalse;
	      }
	    else
	      {
	      iAdaptationObserver->AsynchronousOperationComplete(KErrNone, ETrue);
	      }
        DP_OUT();
        return;
        }

    //If the Commit cycle when going into EDevSoundAdaptorRemovingProcessingUnits fails due to pre-emption
    //clash then we re-apply the client request again.
    if (iDevAudio->iActiveState == EDevSoundAdaptorRemovingProcessingUnits && iIgnoreAsyncOpComplete)
    	{
		//Pop front of queue to re-apply the request again via CMMFDevSoundSession::DequeueRequest
		//from the callback into CMMFDevSoundSession below:
		iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
		iIgnoreAsyncOpComplete = EFalse;
		DP_OUT();
		return;
    	}

    iDevAudio->iActiveState = EDevSoundAdaptorCreated_Uninitialised;
    
    if (iDevAudio->iReinitializing)
        {
        ContextEventStopDevSoundNotifications();
        
        TInt err = iDevAudio->Initialize(iDevAudio->iTargetFormat, iDevAudio->iTargetMode);
        if(err)
            {
            ContextEventAsynchronousInitializeComplete(err);
            }

        iDevAudio->iReinitializing = EFalse;
        DP_OUT();
        return;
        }
    
    iDevAudio->iClosing = EFalse;
    iAdaptationObserver->AsynchronousOperationComplete(KErrNone, ETrue);
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventUpdateWithoutStateEventButWithError(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventUpdateWithoutStateEventButWithError, CtxDevSound, DPLOCAL);
    DP_IN();

    //If flag is true below then it must be due to a stateless normal pre-emption or
    //stateless pre-emption clash scenario.
    if(iIgnoreAsyncOpComplete)
    	{
		//If we are in pre-emption clash then callback below will retry the client request again.
		iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue); // notify client of end of cycle
		iIgnoreAsyncOpComplete = EFalse;
    	}
    else
    	{
        TDevSoundAdaptorState previousState = iDevAudio->iPreviousState;

        DP3(DLINFO,"Error with no state change, state %d, previous %d, error %d during Commit cycle",
                    iDevAudio->iActiveState, previousState, aError);

        // We can end up here for a number of reasons. For non "mid states", this is
        // a standard error scenario. For some mid-states (e.g. Activating, Loading etc),
        // when we try and "promote" the state, this happens when the promotion is rejected
        // and we handle thus accordingly. For other mid-states the situation is less clear
        // and we call AsynchronousOperationComplete() with the error assuming the parent
        // session will deal with it. Whatever we don't want to stay in a mid-state, so
        // rewind to the previous ("normal") one if we are in one.

        // Longer term TODO. If the code were refactored so that the InitializeComplete(),
        // PlayError() etc callback came from AsynchronousOperationComplete() then the
        // following code might be simpler. Most of the time (at least) we get here because
        // we are doing a session function, and we can use the function to determine what
        // to do more easily than relying on the state. As it is, for some mid-states we
        // try and work out what error code to generate. Not clear this covers 100% cases,
        // although demotion transitions should not fail, so the problem cases might not occur.
        //

		//If we reach this condition then it is because of rejection/error during Commit cycle.
		switch (iDevAudio->iActiveState)
			{
			case EDevSoundAdaptorInitialising:
				{
				iDevAudio->iActiveState = previousState;
				ContextEventAsynchronousInitializeComplete(aError);
				break;
				}
			case EDevSoundAdaptorLoading:
				{
				iDevAudio->iActiveState = previousState;
				ContextEventAsynchronousPlayCompletion(aError);
				break;
				}
			case EDevSoundAdaptorActivating:
				{
				iDevAudio->iActiveState = previousState;
				ContextEventAsynchronousPlayCompletion(aError);
				break;
				}
			case EDevSoundAdaptorRemovingProcessingUnits:
			case EDevSoundAdaptorUninitialising:
			case EDevSoundAdaptorUnloading:
			case EDevSoundAdaptorStopping:
			case EDevSoundAdaptorPausing:
			    {
                DP2(DLINFO,"Unexpected mid state [%d] when handling error [%d] during Commit cycle, workback", iDevAudio->iActiveState, aError);
			    iDevAudio->iActiveState = previousState;
                iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
			    break;
			    }
			default:
				{
				DP2(DLINFO,"Unexpected state [%d] when handling error [%d] during Commit cycle", iDevAudio->iActiveState, aError);
		        iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
				}
			}
    	}
       
    DP_OUT();
    }


void CDevCommonControl::ContextEventUpdateWithStateEventNoError() // from CDevCommonControl 
    {
    DP_CONTEXT(CDevCommonControl::ContextEventUpdateWithStateEventNoError, CtxDevSound, DPLOCAL);
    DP_IN();

    switch (iDevAudio->iActiveState)
        {
    case EDevSoundAdaptorUninitialising:
        iDevAudio->iActiveState = EDevSoundAdaptorUnitialised_Uninitialised;
        ContextEventStateDevSoundAdaptorUninitializing();
        break;

    case EDevSoundAdaptorInitialising:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Initialised;
        ContextEventAsynchronousInitializeComplete(KErrNone);
        break;

    case EDevSoundAdaptorUnloading:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Initialised;
        ContextEventStateDevSoundAdaptorUnloading();
        break;

    case EDevSoundAdaptorLoading:
        iDevAudio->iActiveState = EDevSoundAdaptorGoingActive;
        ContextEventStateDevSoundAdaptorLoading();
        break;

    case EDevSoundAdaptorStopping:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Idle;
        ContextEventStateDevSoundAdaptorStopping();
        break;

    case EDevSoundAdaptorActivating:
        iDevAudio->iActiveState = EDevSoundAdaptorActive_Active;
        ContextEventPauseResumeSequenceDueToEmptyBuffers(EFalse);
        break;
        
    case EDevSoundAdaptorPausing:
        iDevAudio->iActiveState = EDevSoundAdaptorPaused_Primed;
        ContextEventPauseResumeSequenceDueToEmptyBuffers(ETrue);
        break;

    case EDevSoundAdaptorBeingPreempted:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Initialised;
        ContextEventStateDevSoundAdaptorBeingPreempted();
        break;
        
    default:
        break;
        }   
    
    DP_OUT();
    }


void CDevCommonControl::ContextEventUpdateWithStateEventAndError(TInt aError) // from CDevCommonControl
    {
    DP_CONTEXT(CDevCommonControl::ContextEventUpdateWithStateEventAndError, CtxDevSound, DPLOCAL);
    DP_IN();

    DP1(DLERR,"ContextEventUpdateWithStateEventAndError error=%d", aError);
    
    switch(iDevAudio->iActiveState)
        {
    case EDevSoundAdaptorInitialising:
        iDevAudio->iActiveState = EDevSoundAdaptorCreated_Uninitialised;
        iAdaptationObserver->InitializeComplete(aError);
        break;
        
    case EDevSoundAdaptorLoading:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Initialised;
        FinishWithError(aError);
        break;
        
    case EDevSoundAdaptorActivating:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Idle;
        ContextEventErrorStateDevSoundAdaptorActivating(aError);
        break;
        
    case EDevSoundAdaptorBeingPreempted:
        iDevAudio->iActiveState = EDevSoundAdaptorInitialised_Initialised;
        ContextEventErrorStateDevSoundAdaptorBeingPreempted();
        break;

    default:
        break;
        }
    
    iCallbackFromAdaptor = KCallbackNone;
    
    if(!iIgnoreAsyncOpComplete)
        {
        iAdaptationObserver->AsynchronousOperationComplete(aError, ETrue);
        }
    else
        {
        iAdaptationObserver->PreemptionFinishedCallbackReceived(ETrue);
        iIgnoreAsyncOpComplete=EFalse;
        }
    
    DP_OUT();
    }


// end of file
