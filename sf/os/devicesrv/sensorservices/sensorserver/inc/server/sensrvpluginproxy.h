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
* Description:  Sensor server proxy for accessing specific Ssy plugin.
*
*/


#ifndef SENSRVPLUGINPROXY_H
#define SENSRVPLUGINPROXY_H

#include <e32base.h>
#include <sensrvchannelinfo.h>
#include <ecom/implementationinformation.h>
#include "sensrvtypes.h"
#include "sensrvtimer.h"
#include "sensrvresourcechannelinfo.h"

class CSensrvProxyManager;
class CSensrvTransaction;
class CSensrvServerMediator;
class CSensrvSsyMediator;
class CSensrvThreadMonitor;
class CSensrvMessage;
class CSensrvTransactionQueue;
class CSensrvChannel;
class CSensrvSession;
class CSensrvTransactionMonitor;

/**
*  Manages single plugin and transactions related to it
*
*  @since S60 5.0
*/
class CSensrvPluginProxy : public CBase,
                           public MSensrvTimerCallback
    {
    public:


        /**
        * Enumeration from plugin states.
        */
        enum TPluginState
            {
            EPluginStateUninitialized,       ///< SSY thread not yet initalized for the first time.
            EPluginStateThreadInitializing,  ///< SSY thread started
            EPluginStateThreadInitialized,   ///< SSY thread scheduler loop active
            EPluginStateLoaded,              ///< SSY loaded and ready to serve
            EPluginStateUnloading,           ///< SSY unloading
            EPluginStateUnloaded             ///< SSY unloaded. Thread and SSY mediator are destroyed.
            };

        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @param aParent Reference to proxy manager.
        * @param aImplementationUid Implementation UID required to load plugin.
        * @return New CSensrvPluginProxy instance
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvPluginProxy* NewL(CSensrvProxyManager& aParent,
                                        TUid aImplementationUid);

        /**
        * Destructor.
        */
        virtual ~CSensrvPluginProxy();


    public:

    // public accessors

        /**
        * Gets pointer to SSY mediator.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Pointer to SSY mediator. Can be NULL if plugin thread is not started.
        */
        inline CSensrvSsyMediator* SsyMediator() { return iSsyMediator; };

        /**
        * Gets pointer to server mediator.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Pointer to server mediator. Can be NULL if plugin thread is not started.
        */
        inline CSensrvServerMediator* ServerMediator() { return iServerMediator; };

        /**
        * Gets reference to the mutex controlling access to shared resources
        * between server and plugin threads.
        *
        * @since S60 5.0
        * @return Reference to Mutex.
        */
        inline RMutex& Mutex() { return iMutex; };

        /**
        * Gets reference to the proxy manager
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Reference to proxy manager.
        */
        inline CSensrvProxyManager& ProxyManager() { return iProxyManager; };

        /**
        * Gets plugin state.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Current plugin state according to proxy.
        */
        inline TPluginState PluginState() const { return iPluginState; };

        /**
        * Gets proxy initialization status
        * Called from server thread
        *
        * @since S60 5.0
        * @return Current proxy initialization status.
        */
        inline TBool IsInitialized() const { return iIsInitialized; };

        /**
        * Gets plugin implementation UID
        * Called from SSY thread.
        *
        * @since S60 5.0
        * @return Plugin implementation UID
        */
        inline const TUid ImplementationUid() const { return iImplementationUid; };

        /**
        * Gets a reference for plugin channel info array.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Channel info array
        */
        inline RSensrvResourceChannelInfoList& ChannelInfoList() { return iChannelInfoList; };

        /**
        * Gets a reference for plugin removed channels info array.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Removed channels info array
        */
        inline RSensrvResourceChannelInfoList& RemovedChannelsInfoList() { return iRemovedChannelsInfoList; };

        /**
        * Gets a reference to transaction monitor.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Transaction monitor
        */
        inline CSensrvTransactionMonitor* TransactionMonitor() { return iTransactionMonitor; };

        /**
        * Checks if proxy is marked for deletion
        * Called from server thread
        *
        * @since S60 5.0
        * @return Current proxy deletion mark status
        */
        inline TBool IsMarkedForDeletion() const { return iDeletionFlag; };

        /**
        * Marks proxy for deletion
        * Called from server thread
        *
        * @since S60 5.0
        */
        inline void SetDeletionFlag() { iDeletionFlag = ETrue; };

        /**
        * Notifies proxy that a transaction has been handled by plugin.
        * Called only from server thread.
        *
        * @since S60 5.0
        * @param aTransaction The transaction that has been handled.
        */
        void TransactionHandledAtSsy(CSensrvTransaction* aTransaction);

        /**
        * Initializes plugin thread and mediators.
        * Also queues load transaction.
        * Called only within server thread.
        *
        * @since S60 5.0
        * @exception Normal Symbian error code
        */
        void InitializePluginL();

        /**
        * Handles client message.
        * Message completion responsibility transfers.
        * Called only within server thread.
        *
        * @since S60 5.0
        * @param aMessage The message containing the client request.
        * @param aChannelId The channel id of related channel.
        */
        void DispatchMessage( CSensrvMessage& aMessage, TSensrvChannelId aChannelId );

        /**
        * Check if this proxy supports specified channel
        * Called only within server thread.
        *
        * @since S60 5.0
        * @param aChannelId Channel id to check
        * @return ETrue if proxy does provide specified channel
        */
        TBool IsChannelSupported(TSensrvChannelId aChannelId);


        /**
        * Gets the channel object that handles the specified channel.
        *
        * @since S60 5.0
        * @param aChannelId The channel id of desired channel.
        * @return Pointer to the channel object handling the specified channel
        *         or NULL if channel is not yet initialized.
        */
        CSensrvChannel* GetChannelForId(TSensrvChannelId aChannelId) const;

        /**
        * Cleans up session related data.
        *
        * @since S60 5.0
        * @param aSession The session that was deleted
        */
        void SessionTerminated( CSensrvSession* aSession );

        /**
        * Handles notify failure at SSY
        *
        * @since S60 5.0
        */
        void HandleSsyNotifyFailure();

        /**
        * Transaction timeout handling.
        *
        * @since S60 5.0
        */
        void HandleTransactionTimeout();

        /**
        * Called by thread monitor when it detects that SSY thread
        * is no longer running or has explicitly killed it.
        *
        * @since S60 5.0
        */
        void SsyThreadTerminated();

        /**
        * Check if plugin has no clients, and start unload timer if so.
        *
        * @since S60 5.0
        */
        void StartUnloadTimerIfNeeded();

        /**
        * Stop unload timer if it is active
        *
        * @since S60 5.0
        */
        inline void StopUnloadTimer() { iUnloadTimer->Cancel(); };

        /**
        * Initializes plugin channel data.
        *
        * @since S60 5.0
        * @param aImplInfo Implementation information of the channel
        * @return Normal Symbian error code
        */
        TInt InitializeChannelData( const CImplementationInformation& aImplInfo );

        /**
        * Gets a reference for plugin dynamic channel info array.
        * Called from both threads.
        *
        * @since S60 5.0
        * @return Dynamic channel info array
        */
        inline RSensrvResourceChannelInfoList& DynamicChannelInfoList() { return iDynamicChannelInfoList; }

        /**
        * Finds channel from info list that matches to given channel info
        *
        * @since S60 5.0
        * @param aChannelInfo Channel info to match from
        * @param aChannelInfoList Channel info list to match with
        * @return Index to given list. KErrNotFound if match is not found.
        */
        static TInt FindChannel( const TSensrvChannelInfo& aChannelInfo,
                                 const RSensrvResourceChannelInfoList& aChannelInfoList );

        /**
        * Finds channel from info list that matches to given channel info
        *
        * @since S60 5.0
        * @param aChannelInfoList Channel info list to match from
        * @param aChannelInfo Channel info to match with
        * @return Index to given list. KErrNotFound if match is not found.
        */
        static TInt FindChannel( const RSensrvChannelInfoList& aChannelInfoList,
                                 const TSensrvChannelInfo& aChannelInfo );

        /**
        * Increments dynamic channel change listener count
        *
        * @since S60 5.0
        */
        void AddChannelChangeListener();

        /**
        * Decrements dynamic channel change listener count
        *
        * @since S60 5.0
        */
        void RemoveChannelChangeListener();

        /**
        * Checks SSY load fail indicator
        *
        * @since S60 5.0
        * @return ETrue if loading of previous SSY failed. Otherwise EFalse.
        */
        inline TBool PreviousSsyLoadFailed() const { return iPreviousSsyLoadFailed; }

    // From base classes

        /**
        * Called when the unload timer fires.
        * Cleanus up the SSY
        *
        * @since S60 5.0
        * @param aTimerId Timer identifier to id which timer fired.
        */
        void TimerFired(TInt aTimerId);

        /**
        * Cleans up the plugin, its thread and SSY mediator.
        * Only called from server thread.
        *
        * @since S60 5.0
        */
        void CleanupPlugin();


    private:  // Member functions

        /**
        * C++ constructor
        *
        * @param aParent Reference to proxy manager.
        * @param aImplementationUid Implementation UID required to load plugin.
        */
        CSensrvPluginProxy(CSensrvProxyManager& aParent,
                           TUid aImplementationUid);

        /**
        * Second phase of construction.
        */
        void ConstructL();

        /**
        * Queues new transaction to proxy's queue.
        * Ownership of the transaction transfers to queue.
        * If queue is empty, also calls HandleNextTransaction.
        * Only called from server thread.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to queue
        * @param aLast If ETrue, transaction is added to the end of the queue
        * @return KErrNoMemory Out of memory
        */
        TInt QueueTransaction(CSensrvTransaction* aTransaction, TBool aLast);

        /**
        * Handle next queued transaction in specified queue.
        * If there was an error, transaction is completed with error.
        * Only called from server thread.
        *
        * @since S60 5.0
        */
        void HandleNextTransaction();

        /**
        * Completes transaction after it has been handled by SSY.
        * Only called from server thread.
        *
        * @since S60 5.0
        * @param aTransaction The transaction to complete
        */
        void CompleteTransaction(CSensrvTransaction* aTransaction);

        /**
        * Gets channel info matching specified channel id
        *
        * @since S60 5.0
        * @param aChannelId Channel id
        * @return Channel info. This is valid until plugin is reloaded.
        *         NULL if not found
        */
        TSensrvResourceChannelInfo* GetChannelInfoForId( TSensrvChannelId aChannelId );

        /**
        * Sets plugin state.
        * Called only from server thread.
        *
        * @since S60 5.0
        * @param aState New state.
        */
        void SetPluginState(TPluginState aState);

        /**
        * Gets listener count of all channels
        * Called only from server thread.
        *
        * @since S60 5.0
        * @return the total listener count.
        */
        TInt GetTotalListenerCount();

        /**
        * Grabs raw channel info data from SSY mediator and
        * Updates channel info and removed channel info lists.
        *
        * @since S60 5.0
        */
        TInt UpdateChannelInfoLists();

        /**
        * SSY thread main method.
        * Only called from SSY thread.
        *
        * @since S60 5.0
        * @param aParameter CSensrvPluginProxy pointer.
        */
        static TInt SsyThreadFunction(TAny* aParameter);

        /**
        * Cleanupitem callback for plugin initialization
        *
        * @since S60 5.0
        * @param aAny The proxy pointer
        */
        static void CleanupInitializePlugin( TAny* aAny );

        /**
        * Enqueues delayd initialization if load queue is empty
        * Only called from server thread.
        *
        * @since S60 5.0
        * @return KErrNoMemory Out of memory
        */
        TInt EnqueueDelayedInitializeTransaction();

    private: // Member variables

        /**
        * Stores the state of plugin controlled by this proxy.
        */
        TPluginState iPluginState;

        /**
        * Reference to proxy manager
        * Own.
        */
        CSensrvProxyManager& iProxyManager;

        /**
        * Implementation UID needed to load plugin.
        */
        TUid iImplementationUid;

        /**
        * Active channels provided by the plugin
        */
        RSensrvResourceChannelInfoList iChannelInfoList;

        /**
        * Channels that have been provided by the plugin at one time,
        * but have subsequently been removed. Removed channels
        * are stored so that original channel ids can be recovered should they
        * be re-registered.
        */
        RSensrvResourceChannelInfoList iRemovedChannelsInfoList;

        /**
        * Holds load transaction and channel related transactions until
        * plugin has finished loading.
        * Queued transactions are owned by the queue.
        * Own.
        */
        CSensrvTransactionQueue* iLoadWaitQueue;

        /**
        * List of channels opened at least for one client.
        * Own. (the channels in list)
        */
        RPointerArray<CSensrvChannel> iChannelList;

        /**
        * Mediator used to pass transactions from main thread to plugin thread
        * Own technically, but actual deletion is done in static SsyThreadFunction,
        * after SSY active scheduler loop terminates.
        */
        CSensrvSsyMediator* iSsyMediator;

        /**
        * Mediator used to pass transactions from plugin thread to main thread
        * Own.
        */
        CSensrvServerMediator* iServerMediator;

        /**
        * Used to control access to data shared by server and plugin threads.
        */
        RMutex iMutex;

        /**
        * Indicates that this proxy has finished its initialization.
        * Note that this doesn't mean channels are actually available or
        * that plugins are loaded successfully.
        */
        TBool iIsInitialized;

        /**
        * SSY thread handle
        */
        RThread iSsyThread;

        /**
        * Monitors transactions
        * Own.
        */
        CSensrvTransactionMonitor* iTransactionMonitor;

        /**
        * Monitors plugin threads existence.
        * Own.
        */
        CSensrvThreadMonitor* iThreadMonitor;

        /**
        * Timer used to unload plugin if it has no clients
        * for a certain period of time.
        * Own.
        */
        CSensrvTimer* iUnloadTimer;

        /**
        * If true, proxy is marked for deletion and will be deleted soon.
        */
        TBool iDeletionFlag;

        /**
        * Dynamic channels provided by the plugin
        */
        RSensrvResourceChannelInfoList iDynamicChannelInfoList;

        /**
        * Channel change listener counter
        */
        TInt iChannelChangeListenerCount;

        /**
        * Indicator of previous SSY load fail. If ETrue, the load attempt was failed.
        */
        TBool iPreviousSsyLoadFailed;
    };



#endif // SENSRVPLUGINPROXY_H
