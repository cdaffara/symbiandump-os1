/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics generic timer class header file.
*
*/


#ifndef C_HWRMHAPTICSGENERICTIMER_H
#define C_HWRMHAPTICSGENERICTIMER_H

#include <e32base.h>

/**
 *  An interface for handling timer expiration events.
 * 
 * @since S60 5.1
 */
class MHWRMHapticsGenericTimerCallback
    {
public:
    /** 
     * Called when the timer fires
     *
     * @param aTimerId Timer identifier to id which timer fired.
     * @param aCutOff ETrue if timer firing occurred because maximum ontime
     *                was reached.
     */
    virtual void GenericTimerFired( TInt aTimerId, TBool aCutOff ) = 0;
    };

/**
 *  Timer class for supervising Genericting time.
 */
class CHWRMHapticsGenericTimer : public CTimer
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aCallback Callback object for informing timer firing
     * @param aMaxTime Maximum time timer is allowed to run. If 0 is used
     *        timer has no maximum time.
     * @param aTimerId Timer identifier used in timer fired callback.
     */
    static CHWRMHapticsGenericTimer* NewL(
            MHWRMHapticsGenericTimerCallback* aCallback,
            const TTimeIntervalMicroSeconds32& aMaxTime,
            TInt aTimerId );
     
    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsGenericTimer();

    /**
     * Set the timer.
     *
     * @param aInterval timer will expire after this duration.
     *        If aInterval is zero, then timer is not started unless
     *        maximum time has been specified, in which case the 
     *        maximum time is used as duration.
     */
    void Set( const TTimeIntervalMicroSeconds32& aInterval );

    /**
     * Cancels the timer and returns the remaining time.
     * Timer can be resumed with Set call.
     *
     * @return Remaining time on timer. Will be Zero if timer is not active,
     *         otherwise it is always positive.
     */
    TTimeIntervalMicroSeconds32 Freeze();

public:

    /**
     * From CTimer
     */
    void RunL();
        
protected:
    
    /**
     * Constructor
     *
     * @param aCallback Callback object for informing timer firing
     * @param aMaxTime Maximum time timer is allowed to run. If 0 is used
     *        timer has no maximum time.
     * @param aTimerId Timer identifier used in timer fired callback.
     */
    CHWRMHapticsGenericTimer( MHWRMHapticsGenericTimerCallback* aCallback, 
                              const TTimeIntervalMicroSeconds32& aMaxTime,
                              TInt aTimerId );
        
private: // Data
    
    /**
     * Callback for timer fired messages. Not owned.
     */
    MHWRMHapticsGenericTimerCallback* iCallback;
    
    /**
     * Maximum timer expiry time
     */
    TTimeIntervalMicroSeconds32 iMaximumTime;
    
    /**
     * Indicates whether the timer was cut off by max duration
     */
    TBool iCutOff;
    
    /**
     * Id of the timer
     */
    TInt iTimerId;

    /**
     * Stores the time the timer is set to activate
     */
    TTime iActivationTime;
    };

#endif // C_HWRMHAPTICSGENERICTIMER_H   
            
// End of File
