/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef CDEVCOMMONCONTROL_H
#define CDEVCOMMONCONTROL_H

#include "cdevaudiocontrol.h"
#include <a3f/a3f_trace_utils.h>
#include <a3f/maudioprocessingunitobserver.h>

/**
 * CDevCommonControl
 *
 * Common functionality required by:
 *      CDevPlayControl
 *      CDevRecordControl
 *      CDevToneControl
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */

NONSHARABLE_CLASS(CDevCommonControl) :  public CDevAudioControl,  
                                        public MAudioProcessingUnitObserver
	{
public:
    CDevCommonControl();
    ~CDevCommonControl();
    
    // from CDevAudioControl
    TInt Stop();
    TInt Pause();
    TInt Resume();
    
    // from MAudioProcessingUnitObserver
    void StateEvent(MAudioStream& aStream, TInt aError, TAudioState aNewState);
    void ProcessingUnitError(MAudioProcessingUnit* aInstance, TInt aError);
    
    // from MAudioContextObserver
    void ContextEvent(TUid aEvent, TInt aError);
    
protected:
    // methods introduced by this class
    virtual void FinishWithError(TInt aError) = 0;
    
private:
    //  Internal functions which together implement CDevCommonControl::ContextEvent
    void ContextEventUpdateWithoutStateEventNoError();
    void ContextEventUpdateWithoutStateEventButWithError(TInt aError);
    void ContextEventUpdateWithStateEventNoError();
    void ContextEventStateDevSoundAdaptorLoading(); 
    void ContextEventStateDevSoundAdaptorUnloading(); 
    void ContextEventStateDevSoundAdaptorStopping();
    void ContextEventStateDevSoundAdaptorUninitializing();
    void ContextEventStateDevSoundAdaptorBeingPreempted();
    void ContextEventPauseResumeSequenceDueToEmptyBuffers(TBool aFlush);
    void ContextEventUpdateWithStateEventAndError(TInt aError);
    void ContextEventUpdateComplete(TInt aError);
    void ContextEventPreEmption(TUid aEvent, TInt aError);
    void ContextEventAbort(TInt aError);
    void ContextEventErrorStateDevSoundAdaptorActivating(TInt aError);
    void ContextEventErrorStateDevSoundAdaptorBeingPreempted();
    void ContextEventAsynchronousPlayCompletion(TInt aError);
    void ContextEventAsynchronousInitializeComplete(TInt aError);
    void ContextEventStopDevSoundNotifications();
    
private:
    TBool iBeingPreempted;
    TBool iPreemptionClash;
	};

#endif // CDEVCOMMONCONTROL_H

// end of file
