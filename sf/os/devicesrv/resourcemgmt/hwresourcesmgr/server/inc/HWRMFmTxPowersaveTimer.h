/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef HWRMFMTXPOWERSAVETIMER_H
#define HWRMFMTXPOWERSAVETIMER_H

#include <e32base.h>

/**
*  An interface for handling timer expiration events.
*
*/
NONSHARABLE_CLASS (MHWRMFmTxPowerSaveTimerObserver)
    {
public:
    /**
    * This callback method is used to notify the client about
    * the timer expiry.
    *     
    */
    virtual void TimerExpired() = 0;
    };
    
/**
*  Timer for FmTx power save
*/
NONSHARABLE_CLASS (CHWRMFmTxPowerSaveTimer) : public CTimer
    {
public:
    
    /**
    * Two-phased constructor.
    *
    * @param aTimerObserver Reference to observer class.
    */
    static CHWRMFmTxPowerSaveTimer* NewL( MHWRMFmTxPowerSaveTimerObserver& aTimerObserver );

    /**
    * Destructor.
    */
    virtual ~CHWRMFmTxPowerSaveTimer();

    /**
    * Starts timer
    *
    * @param aTimeout Timer timeout in microseconds
    */
    void Start( const TTimeIntervalMicroSeconds32 aTimeout );

    /**
    * Stops timer   
    */
    void Stop();

private:
    /**
    * C++ default constructor.
    * Not used.
    */ 
    CHWRMFmTxPowerSaveTimer();
    
    /**
    * Constructor.
    *
    * @param aTimerObserver Reference to observer class.
    */
    CHWRMFmTxPowerSaveTimer( MHWRMFmTxPowerSaveTimerObserver& aTimerObserver );

    /**
    * Symbian second phase constructor.
    */
    void ConstructL();
    
    // from CTimer
    void RunL();

private: // data

    /**
    * Reference to power save timer observer.
    */
    MHWRMFmTxPowerSaveTimerObserver& iTimerObserver;
    };

#endif  //HWRMFMTXPOWERSAVETIMER_H
