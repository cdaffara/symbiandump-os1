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
* Description:  Sensor server proxy manager
*
*/


#ifndef SENSRVPROXYMANAGER_H
#define SENSRVPROXYMANAGER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "sensrvproxymanager.h"
#include "sensrvtypes.h"
#include "sensrvpluginproxy.h"
#include "sensrvecomchangelistener.h"
#include "sensrvshutdown.h"

class CSensrvSession;
class CSensrvTransactionQueue;
class CSensrvServer;


typedef TPckgBuf<TSensrvResourceChannelInfo> TSensrvResourceChannelInfoPckgBuf;

/**
*  Manages proxies and transactions not targeting single channel.
*
*  @since S60 5.0
*/
class CSensrvProxyManager : public CBase, 
                            public MSensrvEcomChangeObserver
    {
    public:
    
        /**
        * Two phase constructor
        *
        * @since S60 5.0
        * @return New CSensrvProxyManager instance
        * @param Reference to server main class.
        * @exception KErrNoMemory Out of memory
        */
        static CSensrvProxyManager* NewL(CSensrvServer& aServer);

        /**
        * Destructor.
        */
        virtual ~CSensrvProxyManager();

        /**
        * Get pointer to server main thread heap
        *
        * @since S60 5.0
        * @return Heap pointer
        */
        inline RHeap* ServerHeap() {return iHeap;}

        /**
        * Get transaction timeout
        *
        * @since S60 5.0
        * @return Transaction timeout in microseconds
        */
        inline TTimeIntervalMicroSeconds32& TransactionTimeout() {return iTransactionTimeout;}

        /**
        * Get SSY stack size
        *
        * @since S60 5.0
        * @return SSY thread stack size in bytes
        */
        inline TInt SsyStackSize() {return iSsyStackSize;}

        /**
        * Get SSY heap max size
        *
        * @since S60 5.0
        * @return SSY thread heap max size in bytes
        */
        inline TInt SsyHeapMaxSize() {return iSsyHeapMaxSize;}

        /**
        * Get thread termination grace period
        *
        * @since S60 5.0
        * @return Thread termination grace period in microseconds
        */
        inline TTimeIntervalMicroSeconds32& ThreadTerminationGracePeriod() {return iThreadTerminationGracePeriod;}

        /**
        * Get SSY inactivity period before unloading
        *
        * @since S60 5.0
        * @return SSY inactivity period in microseconds
        */
        inline TTimeIntervalMicroSeconds32& SsyInactivityPeriod() {return iSsyInactivityPeriod;}

        /**
        * Get channel data buffer size multiplier.
        *
        * @since S60 5.0
        * @return Buffer size multiplier
        */
        inline TInt BufferSizeMultiplier() {return iBufferSizeMultiplier;}

        /**
        * Handles client message. 
        * Message completion responsibility transfers.
        *
        * @since S60 5.0
        * @param aMessage The message containing the client request
        */
        void DispatchMessage( CSensrvMessage& aMessage );
                
        /**
        * Generates new unique id (within context of this server instance).
        *
        * @since S60 5.0
        * @return New unique id.
        */
        TInt GenerateUniqueId();
        
        /**
        * Notifies that a proxy has finished its initialization.
        * This triggers wait queue handling.
        *
        * Note that the notifying proxy might or might not
        * have initialized properly.
        *
        * @since S60 5.0
        */
        void NotifyProxyInitialized();
        
        /**
        * Duplicates mutex handle to one held by main thread.
        *
        * @since S60 5.0
        * @param aSrc Source handle
        * @param aDst Destination handle
        * @return Normal Symbian errors
        */
        TInt DuplicateMutexHandle(const RMutex& aSrc, RMutex& aDst) const;

        /**
        * Cleans up session related data.
        *
        * @since S60 5.0
        * @param aSession The session that was deleted
        */
        void SessionTerminated( CSensrvSession* aSession );
        
        /**
        * Shuts server down gracefully.
        *
        * @since S60 5.0
        * @param aMessage Message related to shutdown.
        */
        void ShutdownServer(const RMessage2& aMessage);

        void ShutdownServer();

        /**
        * Notifies that proxy thread has been terminated
        *
        * @since S60 5.0
        */
        void SsyThreadTerminated();
        
        // From MSensrvEcomChangeObserver
        
        /** 
        * Called when the ecom has been scanned
        *
        * @since S60 5.0
        * @param aImplementationInfos Found SSY implementation infos
        *        The observer is responsible for cleanup of this array,
        *        if change was not blocked (return ETrue).
        * @return EFalse if changing is currently blocked. Try again later.
        */
        virtual TBool EcomChanged( RImplInfoPtrArray* aImplementationInfos );
        
        /**
        * Notifies clients that a change has happened in available channels.
        *
        * @since S60 5.0
        * @param[in] aChangedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel. 
        *                        If ESensrvChannelRemoved, channel was removed and is  
        *                        no longer available.
        */
        void NotifyChannelChange(const TSensrvResourceChannelInfo& aChangedChannel,
                                 TSensrvChannelChangeType aChangeType );
                                 
        
        void AddSession();

#ifdef COMPONENT_TRACE_DEBUG        
        /**
        * Traces a channel info list contents to component trace
        */
        void TraceChannelInfoList(const RSensrvResourceChannelInfoList& aInfoList);
        
        /**
        * Traces a channel info list contents to component trace
        */
        void TraceChannelInfoList(const RSensrvChannelInfoList& aInfoList);
#endif        

        
        inline TTimeIntervalMicroSeconds32& TerminationPeriod() {return iTerminationPeriod;}

        /**
        * Loads the dynamic channel SSYs that matches to given query if not already loaded.
        *
        * @since S60 5.0
        * @param[in] aQueryInfo Channel query info
        * @param[in] aMessage Message from client for capability checking
        * @param[in] aAddChannelChangeListener Updates the listener reference count if ETrue
        * @return ETrue if loading of any required SSY is pending. Otherwise EFalse.
        */
        TBool LoadDynamicChannelSsysIfNeeded(const TSensrvResourceChannelInfo& aQueryInfo,
                                             const RMessage2& aQueryMessage,
                                             TBool aAddChannelChangeListener);

        /**
        * Removes channel change listener from the matching dynamic channel SSYs to allow unloading.
        *
        * @since S60 5.0
        * @param[in] aQueryInfo Channel query info
        * @param[in] aMessage Message from client for capability checking
        */
        void RemoveChannelChangeListenerFromDynamicChannelSsys(const TSensrvResourceChannelInfo& aQueryInfo,
                                                               const RMessage2& aQueryMessage);


    private:  // Member functions
        
        /**
        * C++ constructor
        */
        CSensrvProxyManager(CSensrvServer& aServer);

        /**
        * Second phase of construction.
        */
        void ConstructL();

        /**
        * Handles waiting transactions.
        *
        * @since S60 5.0
        */
        void HandleTransactionsQueryChannels();

        void HandleTransactionsOpenChannel();
        
        /**
        * Requests info about channels asynchronously.
        * Any errors are indicated in message completion code.
        *
        * @since S60 5.0
        * @param aMessage The message related to this request. Ownership passed here.
        */
        void QueryChannels(CSensrvMessage& aMessage);

        void OpenChannel(CSensrvMessage& aMessage);
        
        /**
        * Gets the proxy that handles the specified channel.
        *
        * @since S60 5.0
        * @param aChannelId The channel id of desired channel.
        * @return Pointer to the proxy handling the specified channel or NULL if not found.
        */
        CSensrvPluginProxy* GetProxyForChannel(TSensrvChannelId aChannelId) const;

        /**
        * Fetches configurable settings from cenrep and
        * stores them in member variables.
        *
        * @since S60 5.0
        */
        void GetSettings();

        /**
        * Adds proxy to proxy list
        *
        * @since S60 5.0
        * @param aProxyUid Implementation UID of the proxy to add
        * @return Pointer to new proxy or NULL if failed
        */
        CSensrvPluginProxy* AddProxy(const TUid& aProxyUid);
        
        /**
        * Callback method for proxy cleaner periodic object.
        * Check which proxies are marked for deletion and deletes them if they are
        * ready for cleanup.
        * 
        * @since S60 5.0
        * @param aObject Pointer to this
        * @return Irrelevant, always zero
        */
        static TInt ProxyCleanerCallback( TAny* aObject );
        

    private:
    
        /**
        * Reference to server main class.
        * Not own
        */
        CSensrvServer& iServer;
    
        /**
        * Holds transactions while all proxies finish initializing.
        * Queued transactions are owned by queue.
        * Own.
        */
        CSensrvTransactionQueue* iWaitQueueQueryChannels;

        /**
        * Holds transactions while all proxies finish initializing.
        * Queued transactions are owned by queue.
        * Own.
        */
        CSensrvTransactionQueue* iWaitQueueOpenChannel;
        
        /**
        * Master list or proxies.
        * Own proxies contained in array.
        */
        RPointerArray<CSensrvPluginProxy> iProxyList;
        
        /**
        * Counter used to generate unique ids.
        */
        TInt iIdCounter;
        
        /**
        * Critical section to protect access to iIdCounter
        */
        RMutex iProxyManagerMutex;
        
        /**
        * Indicates all proxies have been initialized
        */
        TBool iAllProxiesInitialized;
        
        /**
        * Server thread handle
        */
        RThread iServerThread;
        
        /**
        * Pointer to server main thread heap.
        * Not own.
        */
        RHeap* iHeap;

        /**
        * Transaction timeout for plugin transactions
        */
        TTimeIntervalMicroSeconds32 iTransactionTimeout;
        
        /**
        * SSY thread stack size
        */
        TInt iSsyStackSize;
        
        /**
        * SSY thread maximum heap size
        */
        TInt iSsyHeapMaxSize;

        /**
        * Thread termination grace period        
        */
        TTimeIntervalMicroSeconds32 iThreadTerminationGracePeriod;

        /**
        * SSY inactivity period before unloading 
        */
        TTimeIntervalMicroSeconds32 iSsyInactivityPeriod;
        
        /**
        * Number of SSY threads still active during shutdown.
        */
        TInt iShuttingDownCount;
        
        /**
        * Shutting down message. 
        */
        RMessagePtr2 iShutdownMessage;
        
        /**
        * Buffer size multiplier.
        */
        TInt iBufferSizeMultiplier;
        
        /**
        * Implementation info array of SSYs
        * Own.
        */
        RImplInfoPtrArray* iSsyImplInfoArray;
        
        /**
        * Listener for SSY plugin changes
        * Own
        */
        CSensrvEcomChangeListener* iEcomChangeListener;
        
        /**
        * Used to delete uninstalled proxies asynchronously
        * Own
        */
        CPeriodic* iProxyCleaner;
        
        /**
        * Keep track of current sessions
        */
        TInt iSessionCounter;
        
        /**
        * A pointer to server shutdown, owned
        */
        CSensrvShutdown* iServerShutdown;
        
        /**
        * Automatic server termination grace period        
        */
        TTimeIntervalMicroSeconds32 iTerminationPeriod;
        
        /**
        * Indicates if automatic server termination has been started.
        */
        TBool iShutdown;
        
    };



#endif // SENSRVPROXYMANAGER_H 
