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
* Description:  Sensor server thread monitor 
*
*/



#include "sensrvdefines.h"
#include "sensrvthreadmonitor.h"
#include "sensrvtimer.h"
#include "sensrvpluginproxy.h"
#include "sensrvproxymanager.h"

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvThreadMonitor* CSensrvThreadMonitor::NewL(CSensrvPluginProxy& aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::NewL(aProxy: 0x%x)" ), aProxy.ImplementationUid().iUid ) );

    CSensrvThreadMonitor* self = new( ELeave ) CSensrvThreadMonitor(aProxy);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvThreadMonitor::CSensrvThreadMonitor(CSensrvPluginProxy& aProxy)
    : CActive(EPriorityHigh),
      iProxy(aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::CSensrvThreadMonitor()" ) ) );

    CActiveScheduler::Add(this);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::CSensrvThreadMonitor - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvThreadMonitor::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::ConstructL()" ) ) );

    // Nothing to do, remove?
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::ConstructL - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvThreadMonitor::~CSensrvThreadMonitor()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::~CSensrvThreadMonitor()" ) ) );

    delete iCleanupTimer;
    iCleanupTimer = NULL;

    Cancel(); 

    TerminateAndNotifyProxy();    

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::~CSensrvThreadMonitor - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Starts monitoring thread
// ---------------------------------------------------------------------------
//
TInt CSensrvThreadMonitor::StartMonitoring(const RThread& aThread)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::StartMonitoring()" ) ) );

    TInt err(KErrNone);

    if (aThread.Handle())
        {
        // If for some reason old iThread is still active, kill it.
        if ( (iThread.Handle()) && 
             (iThread.Handle() != RThread().Handle()) )
            {
            ERROR_TRACE( ( _L( "Sensor Sever - CSensrvThreadMonitor::StartMonitoring - ERROR: Old thread was still active, killing it." ) ) );
            iThread.LogonCancel(iStatus);
            iThread.Terminate(KErrCancel);
            iThread.Close();
            }
        
        // Old cleanuptimer needs to be deleted
        delete iCleanupTimer;
        iCleanupTimer = NULL;
        
        err = iThread.Open(aThread.Id());
        
        if (err == KErrNone)
            {
            iThread.Logon(iStatus);
            SetActive();

#if defined (COMPONENT_TRACE_FLAG) || defined(ERROR_TRACE_FLAG)
            iThreadName = iThread.Name();
#endif
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::StartMonitoring - ERROR: NULL handle" ) ) );
        err = KErrBadHandle;
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::StartMonitoring - return %d" ), err ) );
   
    return err;
    }
    
// ---------------------------------------------------------------------------
// Starts cleanup timer
// ---------------------------------------------------------------------------
//
void CSensrvThreadMonitor::DelayedTermination()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DelayedTermination(), monitored thread: %S" ), &iThreadName ) );

    // If thread is already terminated, notify proxy. Otherwise start 
    // a timer to kill threads forcefully.
    if (!iCleanupTimer)
        {
        if (IsActive())
            {
            TRAPD(err, iCleanupTimer = CSensrvTimer::NewL(*this, 
                                                          KSensrvTimerMaxTime,
                                                          CSensrvTimer::ETimerIdSsyCleanupTimer);)

            if (err == KErrNone)        
                {
                iCleanupTimer->Set(iProxy.ProxyManager().ThreadTerminationGracePeriod());
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DelayedTermination - ERROR: Unable to create timer: %d, Thread terminated immediately" ), err ) );
                Cancel();
                TerminateAndNotifyProxy();
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DelayedTermination - Thread already terminated." )) );
            
            // Paranoia check: Make sure thread has been terminated.
            TerminateAndNotifyProxy();
            }
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DelayedTermination - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Kill threads if they are still active
// ---------------------------------------------------------------------------
//
void CSensrvThreadMonitor::TimerFired(TInt /*aTimerId*/)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::TimerFired(), monitored thread: %S" ), &iThreadName ) );

    iProxy.Mutex().Wait();

    if (IsActive())
        {
        Cancel();
        }

    // If thread has not yet been terminated, do it now
    TerminateAndNotifyProxy();

    iProxy.Mutex().Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::TimerFired - return " ) ) );
    }

// -----------------------------------------------------------------------------
// Handle monitoring event
// -----------------------------------------------------------------------------
//
void CSensrvThreadMonitor::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::RunL(), monitored thread: %S" ), &iThreadName ) );
    
    iProxy.Mutex().Wait();

    // In case of error, monitoring was either canceled or didn't start at all.
    if (iStatus.Int() != KErrNone)
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::RunL - ERROR: %d" ), iStatus.Int() ) );

        // Since monitoring is not possible, kill the thread
        TerminateAndNotifyProxy();
        }
    else
        {
        // Thread has been terminated
        iThread.Close();
        iProxy.SsyThreadTerminated();    
        }
        
    if (iCleanupTimer)     
        {
        iCleanupTimer->Cancel();
        delete iCleanupTimer;
        iCleanupTimer = NULL;
        }

    iProxy.Mutex().Signal();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::RunL - return" ) ) );
    }
    
// -----------------------------------------------------------------------------
// Handle error in RunL
// -----------------------------------------------------------------------------
//
#ifdef COMPONENT_TRACE_DEBUG
TInt CSensrvThreadMonitor::RunError(TInt aError)
#else
TInt CSensrvThreadMonitor::RunError(TInt /*aError*/)
#endif
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::RunError(%d), monitored thread: %S" ), aError, &iThreadName) );

    // Panic this thread if there is unhandled error in RunL.
    // This should not be possible.
    User::Panic(KSensrvPanicCategory, ESensrvPanicThreadMonitorRunError);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::RunError - return %d" ), KErrNone ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CSensrvThreadMonitor::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DoCancel(), monitored thread: %S" ), &iThreadName ) );

    iThread.LogonCancel(iStatus);       
    
    // For some reason, LogonCancel doesn't actually complete the request when shutting down the server,
    // i.e. when active scheduler has been stopped, so do it explicitly here. 
    // This feels like a workaround, though...
    // Needs to be checked if works similarly in HW
    if (!CActiveScheduler::Current())
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DoCancel - Explicitly completing status. Causes cause USER-CBase 46 panic if LogonCancel works as expected." ) ) );
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrCancel);
        }
        
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::DoCancel - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Terminate thread and close handle, if handle is still valid.
// Notify proxy about thread termination.
// -----------------------------------------------------------------------------
//
void CSensrvThreadMonitor::TerminateAndNotifyProxy()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::TerminateAndNotifyProxy(), monitored thread: %S" ), &iThreadName ) );

    // If thread has not yet been terminated, do it now
    if (iThread.Handle() && iThread.Handle() != RThread().Handle())
        {
        iThread.Terminate(KErrCancel);
        iThread.Close();        
        iProxy.SsyThreadTerminated();    
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvThreadMonitor::TerminateAndNotifyProxy - return" ) ) );
    }


