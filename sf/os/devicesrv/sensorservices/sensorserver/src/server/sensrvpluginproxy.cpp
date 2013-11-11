/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server plugin proxy implementation
*
*/


#include <ecom/ecom.h>
#include "sensrvdefines.h"
#include "sensrvpluginproxy.h"
#include "sensrvproxymanager.h"
#include "sensrvtransaction.h"
#include "sensrvtransactionqueue.h"
#include "sensrvtransactionmonitor.h"
#include "senserverchannel.h"
#include "sensrvssymediator.h"
#include "sensrvservermediator.h"
#include "sensrvssyactivescheduler.h"
#include "sensrvclientserver.h"
#include "sensrvthreadmonitor.h"
#include "sensrvchanneldatareader.h"
#include <e32std.h>
#include <e32cmn.h>



// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvPluginProxy* CSensrvPluginProxy::NewL(CSensrvProxyManager& aParent,
                         TUid aImplementationUid)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::NewL(<proxymanager>, aImplementationUid: 0x%x)" ), aImplementationUid.iUid ) );

    CSensrvPluginProxy* self = new( ELeave ) CSensrvPluginProxy(aParent, aImplementationUid);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvPluginProxy::CSensrvPluginProxy(CSensrvProxyManager& aParent,
                                       TUid aImplementationUid)
        : iPluginState( EPluginStateUninitialized ),
          iProxyManager(aParent),
          iImplementationUid(aImplementationUid)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::ConstructL()" ) ) );

    // Init mutex
    User::LeaveIfError(iMutex.CreateLocal());

    iMutex.Wait();
    
		// coverity[SIZECHECK]
    iLoadWaitQueue = CSensrvTransactionQueue::NewL(ETrue);

    iTransactionMonitor = CSensrvTransactionMonitor::NewL(*this);

    // Note: Slight memory savings are possible if iThreadMonitor and iUnloadTimer
    // are created and deleted alongside SSY thread. However, since the timer events
    // in these classes initiate cleanup, it is not trivial. Maybe server mediator
    // could be utilized for delayed cleanup of these classes?

    iThreadMonitor = CSensrvThreadMonitor::NewL(*this);

    iUnloadTimer = CSensrvTimer::NewL(*this,
                                      iProxyManager.SsyInactivityPeriod(),
                                      CSensrvTimer::ETimerIdSsyInactivityTimer);

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvPluginProxy::~CSensrvPluginProxy()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::~CSensrvPluginProxy()" ) ) );

    iMutex.Wait();

    delete iThreadMonitor;
    delete iTransactionMonitor;

    delete iServerMediator;
    delete iSsyMediator;

    // Cleanup channels
    TInt count = iChannelList.Count();
    for(TInt i = 0; i < count; i ++)
        {
        delete iChannelList[i];
        }

    iChannelList.Reset();
    iChannelInfoList.Reset();
    iRemovedChannelsInfoList.Reset();
    iDynamicChannelInfoList.Reset();

    delete iLoadWaitQueue;
    delete iUnloadTimer;

    iMutex.Signal();

    iMutex.Close();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::~CSensrvPluginProxy - return" ) ) );
    }



// ---------------------------------------------------------------------------
// Completes the transaction and calls handling for next one in queue.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::TransactionHandledAtSsy(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy(aTransaction: 0x%x)" ), aTransaction ) );

    // Handle transaction according to transaction type
    if (aTransaction)
        {
        // Route transaction finalization according to transaction type
        switch(aTransaction->Type())
            {
            // Proxy level transactions handled here
            case CSensrvTransaction::ETransTypeLoadSsy:
            case CSensrvTransaction::ETransTypeMediatorChannelsChanged:
                {
                CompleteTransaction(aTransaction);
                }
                break;

            // Channel level transactions handled in corresponding channel object.
            case CSensrvTransaction::ETransTypeOpenChannel:
            case CSensrvTransaction::ETransTypeCloseChannel:
            case CSensrvTransaction::ETransTypeStartListening:
            case CSensrvTransaction::ETransTypeStopListening:
            case CSensrvTransaction::ETransTypeMediatorNewDataAvailable:
            case CSensrvTransaction::ETransTypeGetProperty:
            case CSensrvTransaction::ETransTypeGetAllProperties:
            case CSensrvTransaction::ETransTypeSetProperty:
            case CSensrvTransaction::ETransTypeStartConditionListening:
            case CSensrvTransaction::ETransTypeStopConditionListening:
            case CSensrvTransaction::ETransTypeMediatorPropertyChanged:
            case CSensrvTransaction::ETransTypeMediatorForceChannelClose:
                {
                // These transactions must always have valid channel object.
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));

                aTransaction->Channel()->CompleteTransaction(aTransaction);
                }
                break;

            default:
                // Unexpected transaction type, so we do not know what to do with it
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy - ERROR: Invalid transaction type" ) ) );
                User::Panic(KSensrvPanicCategory, ESensrvPanicUnknownTransactionType);
                break;
            }
        }
    else
        {
        // NULL transaction used to trigger transaction handling.
        if ( iPluginState == EPluginStateThreadInitializing )
            {
            // Initial notify from SSY thread.
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy - SSY thread initialization completion detected." ) ) );

            SetPluginState(EPluginStateThreadInitialized);

            // Start handling queued transactions
            HandleNextTransaction();
            }
        else if (iPluginState == EPluginStateUnloaded)
            {
            // SSY unloading has finished, handle next transaction if any
            HandleNextTransaction();
            }
        else
            {
            // Can get here if server mediator has been notified,
            // but before its RunL is executed, the only transaction in server mediator's queue
            // is deleted (because of client panic caused session termination, for example).
            // Highly unlikely case, but theoretically possible.
            // In that case we can just ignore the notification.
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy - NULL transaction in an unexpected plugin state: %d" ), iPluginState ) );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy - return" ) ) );
    }


// ---------------------------------------------------------------------------
// Initializes plugin mediators and thread,
// loads the plugin and queries for supported channels.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::InitializePluginL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL()" ) ) );

    iMutex.Wait();

    iPreviousSsyLoadFailed = EFalse; // Reset SSY load fail indicator

    if (iPluginState != EPluginStateUnloading)
        {
        // Create mutex signaling cleanup item.
        CleanupStack::PushL(
            TCleanupItem( CleanupInitializePlugin, this ) );

        // Create Ssy thread
        // Use unique identifier generated by proxy manager to name thread
        // so that name is guaranteed unique, yet recognizable.
        HBufC* buf = HBufC::NewLC(KSensrvSsyThreadNameMaxLen);
        buf->Des().Append(KSensrvSsyThreadNameBase);

        TInt err(KErrAlreadyExists);

        while (err == KErrAlreadyExists)
            {

            buf->Des().AppendNum(iProxyManager.GenerateUniqueId());
           

            err = iSsyThread.Create(*buf,
                                    SsyThreadFunction,
                                    ProxyManager().SsyStackSize(),
                                    NULL,
                                    (TAny*)this);
            if (err == KErrAlreadyExists)
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL - Thread name is already in use, recreating..." ) ) );
                }

            buf->Des().Delete(KSensrvSsyThreadNameBaseLen, KSensrvSsyThreadNameMaxLen);
            }

        if (err != KErrNone)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL - Error creating thread." ) ) );
            User::Leave(err);
            }

        CleanupStack::PopAndDestroy(buf);

        // start monitoring the thread
        User::LeaveIfError(iThreadMonitor->StartMonitoring(iSsyThread));

        // If InitializePluginL leaves, thread monitor triggered cleanup will set state again
        SetPluginState(EPluginStateThreadInitializing);

        // SSY mediator is created here but initialized by SSY thread as it needs current thread handle.
        iSsyMediator = CSensrvSsyMediator::NewL(this);

        // Create and initialize server mediator if not yet created
        if (!iServerMediator)
            {
            iServerMediator = CSensrvServerMediator::NewL(this);
            User::LeaveIfError(iServerMediator->Initialize());
            // Note: SSY mediator is initialized by SSY thread
            }

        // Initially there are no channel change listeners concerning this proxy
        iChannelChangeListenerCount = 0;

        // Create transaction for loading plugin.
        CSensrvTransaction* loadTransaction = NULL;
        loadTransaction = CSensrvTransaction::NewL(
            NULL,
            this,
            NULL,
            CSensrvTransaction::ETransTypeLoadSsy);

        // Add transaction to the beginning of the queue
        err = QueueTransaction(loadTransaction, EFalse);

        if (err == KErrNone)
            {
            // Resume Ssy thread. Must be last thing to do in this method
            // so that nothing else can leave after this.
            iSsyThread.Resume();
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL - ERROR: Failed to queue load transaction %d" ), err ) );
            // Cleanup transaction
            loadTransaction->SetErrorCode(err);
            loadTransaction->Complete();
            delete loadTransaction;
            User::Leave(err);
            }

        CleanupStack::Pop(); // CleanupInitializePlugin
        }
    else
        {
        TInt err = EnqueueDelayedInitializeTransaction();
        if (err == KErrNone)
            {
            // Handle delayed initialize immediately, if already fully unloaded
            if (iPluginState == EPluginStateUnloaded)
                {
                HandleNextTransaction();
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL - ERROR: Failed to queue delayed initialize %d" ), err ) );
            iPreviousSsyLoadFailed = ETrue;
            User::Leave(err);
            }
        }

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializePluginL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Initializes plugin channel data.
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::InitializeChannelData( const CImplementationInformation& aImplInfo )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializeChannelDataL" ) ) );

    TSensrvChannelDataReader reader;
    TInt err = reader.ReadChannels( aImplInfo, iChannelInfoList, iDynamicChannelInfoList );
    if ( err == KErrNone )
        {
        // Generate unique IDs for static channels now. IDs for dynamic channels are handled by SSY channel registration.
        TInt count( iChannelInfoList.Count() );
        for( TInt i = 0; i < count; i++)
            {
            TSensrvChannelId newId = static_cast<TSensrvChannelId>(ProxyManager().GenerateUniqueId());

            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializeChannelDataL - plugin UID: 0x%x channel: %d" ),
                aImplInfo.ImplementationUid().iUid, newId ) );

            iChannelInfoList[i].iChannelId = newId;
            }
        }
    iIsInitialized = ETrue; // even though thread not setup and plugin not laoded yet.

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::InitializeChannelDataL - err %d, return" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Handles client message.
// All messages coming from client to proxy target specific channel,
// so pass the message to correct channel object.
// Proxy manager makes sure that messages dispatched to each proxy
// actually target a channel provided by that proxy, so no rechecking is done.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::DispatchMessage(CSensrvMessage& aMessage,
                                         TSensrvChannelId aChannelId)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage(aMessage.Function(): %d, aChannelId: %d)" ), aMessage.Function(), aChannelId ) );

    iMutex.Wait();

    TInt err(KErrNone);
    TBool completeMessageOnError(ETrue);

    // Check state
    if (iDeletionFlag)
        {
        // If proxy is marked for deletion, do not allow further messages
        err = KErrNotFound;
        }
    else
        {
        if (iPluginState == EPluginStateLoaded)
            {
            // Get channel and pass message there
            CSensrvChannel* channel = GetChannelForId(aChannelId);

            // Special case: When opening channel, create channel if it is not found
            if (aMessage.Function() == ESensrvSrvReqOpenChannel && !channel)
                {
                // Get channel info
                TSensrvChannelInfo* info = GetChannelInfoForId(aChannelId);

                if (info)
                    {
                    TRAPD(err, channel = CSensrvChannel::NewL(*info, *this));
                    if (err == KErrNone)
                        {
                        // Add channel to channel list
                        err = iChannelList.Append(channel);

                        if (err != KErrNone)
                            {
                            delete channel;	// Deleting the channel that is allocated on the heap in case Append Failure.
                            channel = NULL;
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - ERROR: Failed to append channel to array: %d" ), err ) );
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - ERROR: Failed to create channel: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - ERROR: No channel info found" ) ) );
                    err = KErrNotFound;
                    }
                }

            if (err == KErrNone)
                {
                if (channel)
                    {
                    channel->DispatchMessage(aMessage);
                    }
                else
                    {
                    // If channel object doesn't exist, something is wrong.
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - ERROR: No channel object" ) ) );
                    err = KErrNotFound;
                    }
                }
            }
        else if (   aMessage.Function() == ESensrvSrvReqOpenChannel
                 && iPluginState != EPluginStateUninitialized )
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - Queuing open channel message" ) ) );

            // If unloading/unloaded, queue initialization transaction.
            if (iPluginState == EPluginStateUnloading || iPluginState == EPluginStateUnloaded)
                {
                err = EnqueueDelayedInitializeTransaction();
                }

            // Queue open transaction into wait queue until plugin can load itself.
            if (err == KErrNone)
                {
                completeMessageOnError = EFalse;

                CSensrvTransaction* transaction = NULL;

                TRAP(err, transaction = CSensrvTransaction::NewL(
                    &aMessage,
                    this,
                    NULL,
                    CSensrvTransaction::ETransTypeDelayedOpenChannel));

                if (err == KErrNone)
                    {
                    err = QueueTransaction(transaction, ETrue);

                    if(err == KErrNone)
                        {
                        // Add transaction to monitor for the duration of the wait.
                        iTransactionMonitor->AddTransaction(transaction);
                        }
                    else
                        {
                        transaction->SetErrorCode(err);
                        transaction->Complete();
                        delete transaction;
                        transaction = NULL;
                        }
                    }
                }

            // Handle delayed initialize immediately, if already fully unloaded
            if (err == KErrNone && iPluginState == EPluginStateUnloaded)
                {
                HandleNextTransaction();
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - ERROR: Plugin in invalid state: %d" ), iPluginState ) );
            err = KErrNotReady;
            }
        }

    if (err != KErrNone && completeMessageOnError)
        {
        aMessage.Complete(err);
        }

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::DispatchMessage - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Checks if this proxy supports specified channel
// ---------------------------------------------------------------------------
//
TBool CSensrvPluginProxy::IsChannelSupported(TSensrvChannelId aChannelId)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::IsChannelSupported(aChannelId: %d)" ), aChannelId ) );

    TBool isSupported(EFalse);

    iMutex.Wait();

    if (iIsInitialized)
        {
        TInt channelCount = iChannelInfoList.Count();
        if (channelCount > 0)
            {
            for(TInt j = 0; !isSupported && j < channelCount; j++)
                {
                if (iChannelInfoList[j].iChannelId == aChannelId)
                    {
                    isSupported = ETrue;
                    }
                }
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::IsChannelSupported - Proxy not yet initialized" ) ) );
        }

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::IsChannelSupported - return %d" ), isSupported) );

    return isSupported;
    }

// ---------------------------------------------------------------------------
// Gets channel object handling specified channel.
// ---------------------------------------------------------------------------
//
CSensrvChannel* CSensrvPluginProxy::GetChannelForId(TSensrvChannelId aChannelId) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetChannelForId(aChannelId: %d)" ), aChannelId ) );

    CSensrvChannel* returnChannel = NULL;

    TInt channelCount = iChannelList.Count();
    for(TInt i = 0; !returnChannel && i < channelCount; i++)
        {
        if (iChannelList[i]->Id() == aChannelId)
            {
            returnChannel = iChannelList[i];
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetChannelForId - return 0x%x" ), returnChannel ) );

    return returnChannel;
    }


// ---------------------------------------------------------------------------
// Cleans up everything related to terminated session.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::SessionTerminated( CSensrvSession* aSession )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SessionTerminated(aSession: 0x%x)"),aSession ) );

    iMutex.Wait();

    // Cleanup any waiting transactions for this session
    iLoadWaitQueue->Remove(aSession);
    iTransactionMonitor->RemoveSessionTransactions(aSession);

    // Clean up mediators
    if (iSsyMediator)
        {
        iSsyMediator->SessionTerminated(aSession);
        }

    if (iServerMediator)
        {
        iServerMediator->SessionTerminated(aSession);
        }


    // Call session cleanup on each channel
    TInt channelCount = iChannelList.Count();
    for(TInt i = 0; i < channelCount; i++)
        {
        iChannelList[i]->SessionTerminated(aSession);
        }

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SessionTerminated - return" )) );
    }

// ---------------------------------------------------------------------------
// Handles SSY notification failure.
// This means going through all channels checking where failure possibly
// occurred, and of course any proxy level transactions.
// Note that this can execute transactions out of original sequence they
// have been notified to server mediator, but this should be no problem,
// each single channel can only have single ongoing transaction at SSY.
// Transactions of different channels do not interfere with each other.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::HandleSsyNotifyFailure()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleSsyNotifyFailure()" ) ) );

    // Check queued transactions to determine if they are handled at SSY
    CSensrvTransaction* transaction = iLoadWaitQueue->First();

    if (transaction && transaction->State() == CSensrvTransaction::ETransStateNotifyFailed)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleSsyNotifyFailure - Notify failed transaction found" ) ) );
        transaction->SetState(CSensrvTransaction::ETransStateHandledAtSsy);
        TransactionHandledAtSsy(transaction);
        }

    if (   iSsyMediator
        && iSsyMediator->ChannelChangeTransaction()
        && iSsyMediator->ChannelChangeTransaction()->State() == CSensrvTransaction::ETransStateNotifyFailed)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleSsyNotifyFailure - Notify failed transaction found (ETransTypeMediatorChannelsChanged)" ) ) );
        iSsyMediator->ChannelChangeTransaction()->SetState(CSensrvTransaction::ETransStateHandledAtSsy);
        TransactionHandledAtSsy(iSsyMediator->ChannelChangeTransaction());
        }

    // Pass notification to each channel
    TInt channelCount = iChannelList.Count();
    for(TInt i = 0; i < channelCount; i++)
        {
        iChannelList[i]->HandleSsyNotifyFailure();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleSsyNotifyFailure - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Unloads SSY as any timed out transactions probably mean that
// SSY has either crashed or is deadlocked.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::HandleTransactionTimeout()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleTransactionTimeout()" ) ) );

    // Any transaction timeout results in unloading of the SSY
    CleanupPlugin();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleTransactionTimeout - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Deletes SSY mediator and closes thread.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::SsyThreadTerminated()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SsyThreadTerminated()" ) ) );

    // Instead of delete, Free is used.
    // It means that destructor of the iSsyMediator is not called.
    // Calling the destructor causes KERN-EXEC 3 in servers main thread
    // if SSY thread is not anymore alive.
    // iSsyMediator inherit from CActive and calling ~CActive() while its
    // scheduler is dead causes KERN-EXEC 3 panic.
    iSsyMediator->Destruct();
    iSsyMediator->BaseDestruct();
    iProxyManager.ServerHeap()->Free( iSsyMediator );
    iSsyMediator = NULL;

    // Note: Server mediator is not deleted until proxy is deleted,
    // as in some cases resulting in cleanup, its RunL-method is what initiated the sequence.

    iSsyThread.Close();

    TPluginState previousState = iPluginState;
    SetPluginState(EPluginStateUnloaded);

    // Cleanup needed in case SSY thread termination was uncontrolled.
    if (previousState != EPluginStateUnloading)
        {
        CleanupPlugin();
        }

    // Notify server mediator with null transaction to handle next transaction
    if (iLoadWaitQueue && iServerMediator && !iLoadWaitQueue->IsEmpty())
        {
        iServerMediator->Notify(NULL);
        }

    // Notify proxy manager that thread has terminated
    iProxyManager.SsyThreadTerminated();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SsyThreadTerminated - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Starts SSY unload timer if no clients
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::StartUnloadTimerIfNeeded()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded()" ) ) );

    if (!iUnloadTimer->IsActive() )
        {
        if (GetTotalListenerCount())
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded - Listener found, aborting" ) ) );
            }
        else if (iChannelChangeListenerCount > 0)
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded - Change listener found, aborting" ) ) );
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded - No listeners found, starting unload timer" ) ) );
            iUnloadTimer->Set(iProxyManager.SsyInactivityPeriod());
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded - Unload timer already active or scheduler stopped" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::StartUnloadTimerIfNeeded - return " ) ) );
    }


// ---------------------------------------------------------------------------
// Kill threads if they are still active
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::TimerFired(TInt /*aTimerId*/)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TimerFired()" ) ) );

    iMutex.Wait();

    // Doublecheck that there is still zero listeners, just in case
    if(!GetTotalListenerCount())
        {
        CleanupPlugin();
        }

    iMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TimerFired - return " ) ) );
    }

// ---------------------------------------------------------------------------
// Cleans up the plugin and thread
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::CleanupPlugin()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CleanupPlugin()" ) ) );

    // Something has failed, so notify proxy manager that proxy initialization is complete,
    // if this happened during the first attempt to load plugin.
    // Note that this will leave proxy with zero channels
    // but there is not much we can do about it, as it is very unlikely that reload would be
    // successfull either.

    if (!iIsInitialized ||
         iPluginState == EPluginStateUninitialized ||
         iPluginState == EPluginStateThreadInitializing ||
         iPluginState == EPluginStateThreadInitialized)
        {
        iPreviousSsyLoadFailed = ETrue;
        // If first fail, fake initialized and notify manager to continue pending activities
        // If proxy provides dynamic channels, notify manager to continue pending channel queries
        if (!iIsInitialized || iDynamicChannelInfoList.Count() > 0)
            {
            iIsInitialized = ETrue;
            iProxyManager.NotifyProxyInitialized();
            }
        }

    // Cleanup channels
    iChannelList.ResetAndDestroy();

    // Cleanup all ongoing transactions.
    if (iTransactionMonitor)
        {
        iTransactionMonitor->RemoveAllTransactions();
        }


    if (iServerMediator)
        {

        iMutex.Wait();
        iServerMediator->RemoveAllTransactions();
        iMutex.Signal();
        }

    if (iPluginState != EPluginStateUnloaded && iLoadWaitQueue)
        {
        // Load wait transactions are not cleared when cleanup is called from SsyThreadTerminated()
        iLoadWaitQueue->RemoveAll();
        }

    // Clean up SSY mediator. Actual deletion of SSY mediator is done in SsyThreadTerminated()
    if (iSsyMediator)
        {
        SetPluginState(EPluginStateUnloading);
        iSsyMediator->Cleanup();
        }
    else
        {
        SetPluginState(EPluginStateUnloaded);
        }

    // Thread monitor needs to ensure that SSY thread dies.
    // If cleanup was triggered by thread dying, this is not needed
    if (iThreadMonitor && iPluginState != EPluginStateUnloaded)
        {
        iThreadMonitor->DelayedTermination();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CleanupPlugin - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Queues transaction
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::QueueTransaction(CSensrvTransaction* aTransaction, TBool aLast)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::QueueTransaction(aTransaction: 0x%x, aLast: %d)" ), aTransaction, aLast ) );

    __ASSERT_ALWAYS(aTransaction, User::Panic(KSensrvPanicCategory, ESensrvPanicNullTransaction));

    TInt err(KErrNone);

    aTransaction->SetState(CSensrvTransaction::ETransStateQueued);

    err = iLoadWaitQueue->Add(aTransaction, aLast);
    if (err != KErrNone)
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::QueueTransaction - ERROR: Failed to add transaction to queue" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::QueueTransaction - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Executes the first transaction in given queue, unless it is already executing,
// in which case nothing is done.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::HandleNextTransaction()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction()" ) ) );

    CSensrvTransaction* transaction = iLoadWaitQueue->First();

    if (transaction && !iDeletionFlag)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - Handling transaction type: %d" ), transaction->Type() ) );
        if (transaction->State() == CSensrvTransaction::ETransStateQueued)
            {
            switch (transaction->Type())
                {
                case CSensrvTransaction::ETransTypeLoadSsy:
                    {
                    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ETransTypeLoadSsy" ) ) );
                    transaction->SetState(CSensrvTransaction::ETransStateExecuting);

                    // If SSY mediator is not yet initialized, it is an error
                    __ASSERT_ALWAYS(iSsyMediator, User::Panic(KSensrvPanicCategory, ESensrvPanicNullSsyMediator));
                  
				   Mutex().Wait();
                    // Notify SSY mediator that transaction is ready to be handled.
                    TInt err(iSsyMediator->Notify(transaction));
                   Mutex().Signal();
                    // If there was error, transaction needs to be removed from queue (which also completes it)
                    if (err == KErrNone)
                        {
                        iTransactionMonitor->AddTransaction(transaction);
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ERROR: Failed to handle next transaction: %d" ), err ) );
                        transaction->SetErrorCode(err);
                        iLoadWaitQueue->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);
                        transaction = NULL;

                        // Since proxy only handles plugin level transactions (load),
                        // failure on any of them means we need to clean up the plugin,
                        // which will also clean up any pending transactions.
                        CleanupPlugin();
                        }
                    }
                    break;
                case CSensrvTransaction::ETransTypeDelayedInitialize:
                    {
                    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ETransTypeDelayedInitialize" ) ) );

                    // Initialize only if state is unloaded
                    if (iPluginState == EPluginStateUnloaded)
                        {
                        iLoadWaitQueue->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);

                        // Request initialize
                        TRAPD(err, InitializePluginL());

                        if (err != KErrNone)
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ERROR: Failed to reinitialize plugin : %d" ), err ) );
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ERROR: Attempted to reinitialize in invalid state: %d" ), iPluginState ) );
                        }
                    }
                    break;
                case CSensrvTransaction::ETransTypeDelayedOpenChannel:
                    {
                    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ETransTypeDelayedOpenChannel" ) ) );

                    iTransactionMonitor->RemoveTransaction(transaction);

                    // Extract message from transaction and call dispatch from proper handling
                    CSensrvMessage* message = transaction->ExtractMessage();

                    iLoadWaitQueue->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);

                    // No need to check error value, we already know message type is correct.
                    TSensrvChannelId channelId(0);
                    message->GetChannelId(channelId);

                    DispatchMessage(*message, channelId);

                    // Recursively handle next transaction
                    HandleNextTransaction();
                    }
                    break;
                default:
                    // Unexpected transaction type, so we do not know what to do with it
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - ERROR: Invalid transaction type" ) ) );
                    User::Panic(KSensrvPanicCategory, ESensrvPanicUnknownTransactionType);
                    break;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - Previous transaction still executing" ) ) );
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - No transaction to execute" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::HandleNextTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Handles transaction finalization and completes transaction on proxy's queue.
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::CompleteTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CompleteTransaction(aTransaction: 0x%x)" ), aTransaction ) );

    __ASSERT_ALWAYS(aTransaction, User::Panic(KSensrvPanicCategory, ESensrvPanicNullTransaction));

    switch (aTransaction->Type())
        {
        // Proxy level transactions handled here
        case CSensrvTransaction::ETransTypeLoadSsy:
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CompleteTransaction - ETransTypeLoadSsy" ) ) );

            TInt err = aTransaction->ErrorCode();

            if ( err == KErrNone)
                {
                err = UpdateChannelInfoLists();

                // Update plugin state
                SetPluginState(EPluginStateLoaded);
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CompleteTransaction - ERROR: Load transaction failed %d" ), err ) );
                }

            if (err != KErrNone)
                {
                // Since there was an error, cleanup the plugin and thread
                CleanupPlugin();
                }
            else
                {
                // Proxy has been initialized, notify manager
                // If proxy provides dynamic channels, notify manager to handle pending channel queries
                if (!iIsInitialized || iDynamicChannelInfoList.Count() > 0)
                    {
                    iIsInitialized = ETrue;
                    iProxyManager.NotifyProxyInitialized();
                    }

                // Initially there are no clients, so startup unload timer
                StartUnloadTimerIfNeeded();
                }
            }
            break;

        case CSensrvTransaction::ETransTypeMediatorChannelsChanged:
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CompleteTransaction - ETransTypeMediatorChannelsChanged" ) ) );

            // Error is irrelevant, as we can't do anything about it anyway, and UpdateChannelInfoLists()
            // does its own tracing.
            UpdateChannelInfoLists();
            }
            break;

        default:
            // Unexpected transaction type, so we do not know what to do with it
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::TransactionHandledAtSsy - ERROR: Invalid transaction type" ) ) );
            User::Panic(KSensrvPanicCategory, ESensrvPanicUnknownTransactionType);
            break;
        }

    // Complete the transaction
    iTransactionMonitor->RemoveTransaction(aTransaction);
    iLoadWaitQueue->Remove(aTransaction, CSensrvTransactionQueue::ERemovalTypeComplete);

    // Handle next transaction.
    HandleNextTransaction();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CompleteTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Goes through channel infos and returns pointer to one that matches given id.
// ---------------------------------------------------------------------------
//
TSensrvResourceChannelInfo* CSensrvPluginProxy::GetChannelInfoForId( TSensrvChannelId aChannelId )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetChannelInfoForId(aChannelId: %d)" ), aChannelId ) );

	TSensrvResourceChannelInfo* info = NULL;;

    if (iIsInitialized)
        {
        TBool found(EFalse);
        TInt channelCount = iChannelInfoList.Count();
        if (channelCount > 0)
            {
            for(TInt j = 0; !found && j < channelCount; j++)
                {
                if (iChannelInfoList[j].iChannelId == aChannelId)
                    {
                    info = &iChannelInfoList[j];
                    found = ETrue;
                    }
                }
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetChannelInfoForId - Proxy not yet initialized" ) ) );
        }


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetChannelInfoForId - return 0x%x" ), info ) );

    return info;
    }

// ---------------------------------------------------------------------------
// Set plugin state
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::SetPluginState(TPluginState aState)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SetPluginState(aState: %d)" ), aState ) );

    iPluginState = aState;
    }

// ---------------------------------------------------------------------------
// Gets total listener count from all channels
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::GetTotalListenerCount()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetTotalListenerCount()" ) ) );

    TInt channelCount(iChannelList.Count());
    TInt listenerCount(0);

    for(TInt i = 0; i < channelCount; i ++)
        {
        listenerCount += iChannelList[i]->ListenerCount();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::GetTotalListenerCount - return: %d" ), listenerCount ) );

    return listenerCount;
    }

// ---------------------------------------------------------------------------
// Grabs raw channel info data from SSY mediator and
// Updates channel info and removed channel info lists.
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::UpdateChannelInfoLists()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists()" ) ) );

    TInt err(KErrNone);

    // Get new info into a temporary array.
    RSensrvChannelInfoList newInfos;

    TAny** rawDataPtr = iSsyMediator->RawInfos();

    if (*rawDataPtr)
        {
        // First item in raw data is count
        TInt* countPointer = reinterpret_cast<TInt*>(*rawDataPtr);

        if (*countPointer > 0)
            {
            err = newInfos.Reserve(*countPointer);

            // Data starts after count
            TSensrvChannelInfo* info = reinterpret_cast<TSensrvChannelInfo*>(countPointer + 1);

            // Loop through raw data, appending datas to info list
            for (TInt newLoop = 0; err == KErrNone && newLoop < *countPointer; newLoop++)
                {
                err = newInfos.Append(*info++);
                if (err != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - ERROR: newInfos.Append failure: %d, count: %d" ), err, newLoop ) );
                    newInfos.Reset();
                    }
                }
            }

        // Free memory allocated by SSY mediator and set mediator's pointer to raw data to NULL
        iProxyManager.ServerHeap()->Free(*rawDataPtr);
        *rawDataPtr = NULL;
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - No channels registered by SSY" ) ) );
        }

#ifdef COMPONENT_TRACE_DEBUG
    COMPONENT_TRACE( ( _L( "### Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists, initial channel lists:" )) );

    COMPONENT_TRACE( ( _L( "### newInfos contents:" )) );
    iProxyManager.TraceChannelInfoList(newInfos);

    COMPONENT_TRACE( ( _L( "### iChannelInfoList contents:" )) );
    iProxyManager.TraceChannelInfoList(iChannelInfoList);

    COMPONENT_TRACE( ( _L( "### iRemovedChannelsInfoList contents:" )) );
    iProxyManager.TraceChannelInfoList(iRemovedChannelsInfoList);
#endif

    // Compare old infos to new infos and notify removals to sessions,
    // add removed infos to iRemovedChannelsInfoList,
    // destroy CSensrvChannel objects of removed channelsm, and
    // remove old infos from iChannelInfoList.
    for (TInt oldLoop = iChannelInfoList.Count() - 1; err == KErrNone && oldLoop >= 0 ; oldLoop--)
        {
        // If match was not found, this channel has been removed
        const TSensrvChannelInfo& oldInfo(iChannelInfoList[oldLoop]);
        TInt index = FindChannel(newInfos, oldInfo);
        if (index == KErrNotFound)
            {
            // Notify interested sessions
            iProxyManager.NotifyChannelChange(oldInfo, ESensrvChannelChangeTypeRemoved);

            // Add info to removed infos.
            err = iRemovedChannelsInfoList.Append(oldInfo);
            if (err != KErrNone)
                {
                // Error is ignored as we cannot do anything about this. It results in channel id
                // being regenerated to this channel if it is readded later, which is inconvenient, but
                // not critical.
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - ERROR: iRemovedChannelsInfoList.Append failure: %d, count: %d" ), err, oldLoop ) );
                }

            // Destroy and remove from channel list the related channel object as it is obsolete.
            TInt channelCount = iChannelList.Count();
            for(TInt i = 0; i < channelCount; i++)
                {
                if (iChannelList[i]->Id() == oldInfo.iChannelId)
                    {
                    delete iChannelList[i];
                    iChannelList.Remove(i);
                    break;
                    }
                }

            // Remove channel info from iChannelInfoList
            iChannelInfoList.Remove(oldLoop);
            }
        }

    // Compare new infos to old infos and notify additions to sessions,
    // remove added infos from iRemovedChannelsInfoList if they are there, and
    // append new infos to iChannelInfoList.
    TInt newCount = newInfos.Count();
    for (TInt newLoop = 0; err == KErrNone && newLoop < newCount; newLoop++)
        {
        const TSensrvChannelInfo& newInfo(newInfos[newLoop]);
        TInt index = FindChannel(newInfo, iChannelInfoList);
        if (index >= 0 && index < iChannelInfoList.Count())
            {
            // Update silently the actual data item size from SSY channel registration if still unset
            TSensrvChannelInfo& oldInfo(iChannelInfoList[index]);
            if (oldInfo.iDataItemSize <= 0 && newInfo.iDataItemSize > 0)
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - channel %d, new data item size: %d" ),
                    oldInfo.iChannelId, newInfo.iDataItemSize ) );
                oldInfo.iDataItemSize = newInfo.iDataItemSize;
                }
            }
        else
            {
            TSensrvResourceChannelInfo newResourceInfo(newInfo);

            // Store dynamic channel status
            newResourceInfo.iDynamic = FindChannel(newInfo, iDynamicChannelInfoList) != KErrNotFound; // Store dynamic status

            err = iChannelInfoList.Append(newResourceInfo);
            if (err == KErrNone)
                {
                // Notify interested sessions
                iProxyManager.NotifyChannelChange(newInfo, ESensrvChannelChangeTypeAdded);

                // Remove from iRemovedChannelsInfoList, in case there has been this channel previously
                index = FindChannel(newInfo, iRemovedChannelsInfoList);
                if(index >= 0 && index < iRemovedChannelsInfoList.Count())
                    {
                    iRemovedChannelsInfoList.Remove(index);
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - ERROR: iChannelInfoList.Append failure: %d, count: %d" ), err, newLoop ) );
                }
            }
        }

    // Note: It might be slightly more efficient to simply reset the iChannelInfoList and
    // repopulate it with newInfos content as opposed to explicit appends and removes
    // above, but that would create a risk that iChannelInfoList was left empty in
    // case append fails to allocate new memory. Since there is no way to resurrect
    // proxy with no channels once it has been unloaded, this is not acceptable.

#ifdef COMPONENT_TRACE_DEBUG
    COMPONENT_TRACE( ( _L( "### " )) );
    COMPONENT_TRACE( ( _L( "### Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists, after changes" )) );

    COMPONENT_TRACE( ( _L( "### iChannelInfoList contents (should be same as newInfos):" )) );
    iProxyManager.TraceChannelInfoList(iChannelInfoList);

    COMPONENT_TRACE( ( _L( "### iRemovedChannelsInfoList contents (should have infos removed from iChannelInfoList added to it):" )) );
    iProxyManager.TraceChannelInfoList(iRemovedChannelsInfoList);
#endif

    // Cleanup newinfos
    newInfos.Close();

    if (iDeletionFlag)
        {
        // Cleanup bad proxy after its channels have been removed
        CleanupPlugin();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::UpdateChannelInfoLists - return: %d" ), err ) );

    return err;
    }


// ---------------------------------------------------------------------------
// SSY thread main function.
// Creates new active scheduler and cleanup stack and installs those.
// Initializes SSY mediator.
// Starts the new active scheduler.
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::SsyThreadFunction( TAny* aParameter )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SsyThreadFunction(aParameter: 0x%x)" ), aParameter ) );

    if (!aParameter)
        {
        ERROR_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - ERROR: NULL proxy, cannot start thread." ) ) );
        return KErrBadHandle;
        }

   // __UHEAP_MARK;

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    TInt heapSize = User::Heap().Size();
    TInt biggestBlock(0);
    TInt heapAvail = User::Heap().Available(biggestBlock);
    TInt used(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### Sensor Server, SSY starting - HEAP: Size: %d, Available: %d, Used: %d" ), heapSize, heapAvail, used ) );
#endif

    CSensrvPluginProxy* proxy = static_cast<CSensrvPluginProxy*>(aParameter);

    TInt err(KErrNone);

    // Create new cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    if ( cleanup )
        {
        // Create and install the active scheduler we need
        CSensrvSsyActiveScheduler* scheduler = NULL;
        TRAP(err, scheduler = CSensrvSsyActiveScheduler::NewL());

        if (err == KErrNone)
            {
            CActiveScheduler::Install( scheduler );

            proxy->Mutex().Wait();

            COMPONENT_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - Started thread for plugin UID: 0x%x" ), proxy->iImplementationUid.iUid) );

            // initialize SSY mediator
            err = proxy->SsyMediator()->Initialize();

            if (err == KErrNone)
                {
                // Initial notify to server mediator via null transaction
                // Notifying NULL transaction cannot fail, so no error handling
                proxy->ServerMediator()->Notify(NULL);

                proxy->Mutex().Signal();

                COMPONENT_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - Starting scheduler" )) );

                CActiveScheduler::Start();

                COMPONENT_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - Scheduler stopped" )) );

                // Signal final closure of ecom session for SSY thread
                REComSession::FinalClose();
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - ERROR: Failed to initialize SSY mediator, cannot start thread: %d." ),err ) );
                proxy->Mutex().Signal();
                }

            // Cleanup
            delete scheduler;
            scheduler = NULL;
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - ERROR: Failed to create scheduler, cannot start thread: %d." ),err ) );
            }

        delete cleanup;
        cleanup = NULL;
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - SsyThreadFunction() - ERROR: No memory to create cleanup stack, cannot start thread." ) ) );
        err = KErrNoMemory;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::SsyThreadFunction - return" ) ) );

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    heapSize = User::Heap().Size();
    heapAvail = User::Heap().Available(biggestBlock);
    TInt newUsed(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### Sensor Server, SSY exit - HEAP: Size: %d, Available: %d, Used: %d, Change in used: %d" ), heapSize, heapAvail, newUsed, newUsed - used ) );
#endif

   // __UHEAP_MARKEND;

    return err;
    }

// ---------------------------------------------------------------------------
// Cleanup item implementation for plugin initialization
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::CleanupInitializePlugin( TAny* aAny )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CleanupInitializePlugin()" ) ) );

    CSensrvPluginProxy* proxy = reinterpret_cast<CSensrvPluginProxy*>(aAny);

    if (proxy)
        {
        // Uninitialized RThread object also has valid handle to current thread
        if (proxy->iSsyThread.Handle() && proxy->iSsyThread.Handle() != RThread().Handle())
            {
            proxy->iSsyThread.Terminate(KErrCancel);
            proxy->iSsyThread.Close();
            }

        // Clean up any pending channel open messages
        proxy->iLoadWaitQueue->RemoveAll();
        proxy->iPreviousSsyLoadFailed = ETrue;
        proxy->Mutex().Signal();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::CleanupInitializePlugin - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Finds channel from info list that matches to given channel info
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::FindChannel( const TSensrvChannelInfo& aChannelInfo,
                                      const RSensrvResourceChannelInfoList& aChannelInfoList )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::FindChannel (RSensrvResourceChannelInfoList)" ) ) );

    TInt index = aChannelInfoList.Find(aChannelInfo, TSensrvResourceChannelInfo::CompareFindMatch);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::FindChannel - index %d, return" ), index ) );

    return index;
    }

// ---------------------------------------------------------------------------
// Finds channel from info list that matches to given channel info
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::FindChannel( const RSensrvChannelInfoList& aChannelInfoList,
                                      const TSensrvChannelInfo& aChannelInfo )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::FindChannel (RSensrvChannelInfoList) Reverse" ) ) );

    TInt index = aChannelInfoList.Find(aChannelInfo, TSensrvResourceChannelInfo::CompareFindMatchReverse);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::FindChannel - index %d, return" ), index ) );

    return index;
    }

// ---------------------------------------------------------------------------
// Increments dynamic channel change listener count
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::AddChannelChangeListener()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::AddChannelChangeListener" ) ) );

    ++iChannelChangeListenerCount;
    StopUnloadTimer();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::AddChannelChangeListener - count %d return " ),
        iChannelChangeListenerCount ) );
    }

// ---------------------------------------------------------------------------
// Decrements dynamic channel change listener count
// ---------------------------------------------------------------------------
//
void CSensrvPluginProxy::RemoveChannelChangeListener()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::RemoveChannelChangeListener" ) ) );

    if (iChannelChangeListenerCount > 0)
        {
        --iChannelChangeListenerCount;
        if (!iChannelChangeListenerCount)
            {
            StartUnloadTimerIfNeeded();
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::RemoveChannelChangeListener - ERROR: count corrupt" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::RemoveChannelChangeListener - count %d return " ),
        iChannelChangeListenerCount ) );
    }

// ---------------------------------------------------------------------------
// Enqueues delayed initialize transaction
// ---------------------------------------------------------------------------
//
TInt CSensrvPluginProxy::EnqueueDelayedInitializeTransaction()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::EnqueueDelayedInitializeTransaction" ) ) );

    TInt err = KErrNone;

    // Enqueue initialization transaction if queue is empty.
    if (iLoadWaitQueue->IsEmpty())
        {
        CSensrvTransaction* transaction = NULL;
        TRAP(err, transaction = CSensrvTransaction::NewL(
            NULL, this, NULL, CSensrvTransaction::ETransTypeDelayedInitialize));
        if (err == KErrNone)
            {
            err = QueueTransaction(transaction, EFalse);
            if (err != KErrNone)
                {
                delete transaction;
                transaction = NULL;
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::EnqueueDelayedInitializeTransaction - ERROR: Failed to create transaction err: %d" ), err ) );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvPluginProxy::EnqueueDelayedInitializeTransaction - return" ) ) );

    return err;
    }
