/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Thread monitor implementation
*
*/


#ifndef SENSRVTHREADMONITOR_H
#define SENSRVTHREADMONITOR_H

#include <e32base.h>
#include "sensrvtimer.h"

class CSensrvPluginProxy;

/**
 * Provides lifecycle management and monitoring service for SSY threads.
 *
 * @since S60 5.0
 */
class CSensrvThreadMonitor : public CActive, 
                             public MSensrvTimerCallback
    {
    public:

        /**
        * Two phase constructor.
        *
        * @since S60 5.0
        * @param aProxy Parent proxy.
        * @return New CSensrvThreadMonitor instance
        * @exception KErrNoMemory Out of memory.
        */
        static CSensrvThreadMonitor* NewL(CSensrvPluginProxy& aProxy);

        /**
        * Destructor.
        */
        virtual ~CSensrvThreadMonitor();

        /**
        * Starts monitoring a thread
        *
        * @since S60 5.0
        * @param aThread The thread to be monitored
        * @return Normal symbian error
        */
        TInt StartMonitoring(const RThread& aThread);

        /**
        * Starts cleanup timer.
        *
        * @since S60 5.0
        */
        void DelayedTermination();

    // From base classes

        /** 
        * Called when the timer fires. 
        * Forcibly kills threads if they are still active
        *
        * @since S60 5.0
        * @param aTimerId Timer identifier to id which timer fired.
        */
        void TimerFired(TInt aTimerId);

    protected:

    // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
        
    private:

        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aProxy Parent proxy.
        */
        CSensrvThreadMonitor(CSensrvPluginProxy& aProxy);

        /**
        * 2nd phase of construction
        *
        * @since S60 5.0
        */
        void ConstructL();
        
        /**
        * Terminate thread and notify proxy.
        * Nothing is done if thread is already terminated.
        *
        * @since S60 5.0
        */
        void TerminateAndNotifyProxy();

        

    private:
    
        /**
        * Owning proxy.
        */
        CSensrvPluginProxy& iProxy;

        /**
        * Thread cleanup timer
        * Own.
        */
        CSensrvTimer* iCleanupTimer;
      
        /**
        * Monitored thread
        */
        RThread iThread;
        
#if defined (COMPONENT_TRACE_FLAG) || defined(ERROR_TRACE_FLAG)
        /**
        * Monitored thread name for debugging 
        * (to suppress "not a lvalue" warnings that come when using iThread.Name() in RDebug)
        */
        TName iThreadName;
#endif
    };



#endif // SENSRVTHREADMONITOR_H
