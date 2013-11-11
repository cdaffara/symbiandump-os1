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


#include <centralrepository.h>
#include "sensrvdefines.h"
#include "sensrvproxymanager.h"
#include "sensrvsession.h"
#include "sensrvserver.h"
#include "sensrvtrace.h"
#include "ssycontrol.h"
#include "sensrvtransactionqueue.h"
#include "sensrvmessage.h"
#include "sensrvclientserver.h"
#include "sensrvtransaction.h"
#include "sensrvtypes.h"
#include "sensrvprivatecrkeys.h"

#ifdef __WINS__
// Multiplier to avoid out of memory in emulator
const TInt KSensrvSsyMaxHeapWinsMultiplier = 2;
#endif // __WINS__


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvProxyManager* CSensrvProxyManager::NewL(CSensrvServer& aServer)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::CSensrvProxyManager()" ) ) );

    CSensrvProxyManager* self = new( ELeave ) CSensrvProxyManager(aServer);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::CSensrvProxyManager - return 0x%x" ), self ) );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvProxyManager::CSensrvProxyManager(CSensrvServer& aServer)
    : iServer(aServer),
      iTransactionTimeout(KSensrvDefaultTransactionTimeout),
      iSsyStackSize(KSensrvDefaultSsyStackSize),
      iSsyHeapMaxSize(KSensrvDefaultSsyHeapMaxSize),
      iThreadTerminationGracePeriod(KSensrvSsyGracefulCleanupTime),
      iSsyInactivityPeriod(KSensrvSsyInactivityPeriod),
      iBufferSizeMultiplier(KSensrvDefaultDataRateMultiplier),
      iTerminationPeriod(KSensrvDefaultTerminationPeriod)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ConstructL()" ) ) );

    User::LeaveIfError(iServerThread.Open(iServerThread.Id()));

    iHeap = &User::Heap();

    User::LeaveIfError(iProxyManagerMutex.CreateLocal());

    // Get configurable settings
    GetSettings();

    iServerShutdown = CSensrvShutdown::NewL( *this );

		// coverity[SIZECHECK]
    iWaitQueueQueryChannels = CSensrvTransactionQueue::NewL(ETrue);
    	
    // coverity[SIZECHECK]	
    iWaitQueueOpenChannel   = CSensrvTransactionQueue::NewL(ETrue);

    // Create ecom changelistener, which will make the initial check for plugins
    iEcomChangeListener = CSensrvEcomChangeListener::NewL(*this);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ConstructL - return" ) ) );
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSensrvProxyManager::~CSensrvProxyManager()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::~CSensrvProxyManager()" ) ) );

    if (iProxyCleaner)
        {
        iProxyCleaner->Cancel();
        delete iProxyCleaner;
        }

    delete iEcomChangeListener;

    // Cleanup iProxyList
    TInt count = iProxyList.Count();
    for(TInt i = 0; i < count; i++)
        {
        delete iProxyList[i];
        }

    iProxyList.Reset();

    // Cleanup iWaitQueue
    delete iWaitQueueQueryChannels;
    delete iWaitQueueOpenChannel;

    iProxyManagerMutex.Close();

    iServerThread.Close();

    if (iShutdownMessage.Handle())
        {
        iShutdownMessage.Complete(KErrCancel);
        }

    if (iSsyImplInfoArray)
        {
        iSsyImplInfoArray->ResetAndDestroy();
        delete iSsyImplInfoArray;
        }

    delete iServerShutdown;

    // iHeap not owned

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::~CSensrvProxyManager - return") ) );
    }

// -----------------------------------------------------------------------------
// Handles message according to message type
// -----------------------------------------------------------------------------
//
void CSensrvProxyManager::DispatchMessage( CSensrvMessage& aMessage )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DispatchMessage(aMessage.Function(): %d), client SID: 0x%x" ), aMessage.Function(), aMessage.SecureId().iId ) );

    // Check command code and call appropriate function
    switch ( aMessage.Function() )
        {
        case ESensrvSrvReqQueryChannels:
            {
            QueryChannels(aMessage);
            break;
            }

        case ESensrvSrvReqOpenChannel:
            {
            // Get channel ID from message
            // No need to check error value, we already made sure the message function is correct

            TSensrvChannelId channelId(0);
            aMessage.GetChannelId(channelId);

            // Determine which proxy handles this channel
            CSensrvPluginProxy* proxy = GetProxyForChannel(channelId);

            if( proxy != NULL &&
                (proxy->PluginState() == CSensrvPluginProxy::EPluginStateUnloaded ||
                 proxy->PluginState() == CSensrvPluginProxy::EPluginStateUninitialized ) )
                {
                TRAPD(err, proxy->InitializePluginL());
                if ( err != KErrNone )
                    {
                    // If proxy initialization fails, it means that proxy will not provide any channels.
                    // Since SSY thread has not been resumed yet (resume is the last thing in InitializeL), proxy
                    // can be safely deleted without risk for memory leak.
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged - ERROR: Proxy initialization failed, error %d" ),  err) );
                    TInt pos( iProxyList.Find( proxy ) );
                    delete proxy;
                    proxy = NULL;
                    if ( pos >= 0 && pos < iProxyList.Count() )
                        {
                        iProxyList.Remove(pos);
                        }
                    }
                }

            // Dispatch the message for found proxy to be handled
            if (proxy != NULL)
                {
                proxy->DispatchMessage(aMessage, channelId); // Transfer ownership of message
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DispatchMessage - ERROR: Proxy not found" )) );
                // Complete with error
                aMessage.Complete(KErrNotFound);
                }
            break;
            }

        case ESensrvSrvReqCloseChannel:
        case ESensrvSrvReqStartListening:
        case ESensrvSrvReqStopListening:
        case ESensrvSrvReqAsyncChannelData:
        case ESensrvSrvReqGetProperty:
        case ESensrvSrvReqSetProperty:
        case ESensrvSrvReqAsyncPropertyData:
        case ESensrvSrvReqStopPropertyListening:
        case ESensrvSrvReqGetAllProperties:
        case ESensrvSrvReqAddConditionSet:
        case ESensrvSrvReqRemoveConditionSet:
        case ESensrvSrvReqStartConditionListening:
        case ESensrvSrvReqStopConditionListening:
        case ESensrvSrvReqAsyncConditionMet:
            {
            // Get channel ID from message
            // No need to check error value, we already made sure the message function is correct
            TSensrvChannelId channelId(0);
            aMessage.GetChannelId(channelId);

            // Determine which proxy handles this channel
            CSensrvPluginProxy* proxy = GetProxyForChannel(channelId);

            // Dispatch the message for found proxy to be handled
            if (proxy != NULL)
                {
                proxy->DispatchMessage(aMessage, channelId); // Transfer ownership of message
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DispatchMessage - ERROR: Proxy not found" )) );
                // Complete with error
                aMessage.Complete(KErrNotFound);
                }
            break;
            }

        // Cannot identify the message
        default:
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DispatchMessage - ERROR: unknown command" )) );
            aMessage.Complete(KErrArgument);
            break;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DispatchMessage - return" )) );
    }


// ---------------------------------------------------------------------------
// Generate new unique id.
// ---------------------------------------------------------------------------
//
TInt CSensrvProxyManager::GenerateUniqueId()
    {
    iProxyManagerMutex.Wait();

    iIdCounter++;

    // Realistically, there will never be so many ids generated that the counter will roll over, but
    // check just in case. No checking for duplicate ids, as chance of that happening is truly
    // astronomical.
    if (iIdCounter == KMaxTInt)
        {
        iIdCounter = 1;
        }

    iProxyManagerMutex.Signal();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::GenerateUniqueId - generated new id: 0x%x" ), iIdCounter ) );

    return iIdCounter;
    }

// ---------------------------------------------------------------------------
// Checks waitqueue and if there are queued transactions,
// checks if all proxies have constructed successfully.
// If so, starts executing queued transactions.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::NotifyProxyInitialized()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::NotifyProxyInitialized()") ) );

    TInt count = iProxyList.Count();
    iAllProxiesInitialized = ETrue;

    for(TInt i=0; i<count; i++)
        {
        if (!iProxyList[i]->IsInitialized())
            {
            iAllProxiesInitialized = EFalse;
            break;
            }
        }

    if (iAllProxiesInitialized)
        {
        HandleTransactionsQueryChannels();
        HandleTransactionsOpenChannel();
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::NotifyProxyInitialized - All proxies not yet initialized") ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::NotifyProxyInitialized - return" )) );
    }

// ---------------------------------------------------------------------------
// Duplicates source handle to destination handle.
// Source handle must be held by Server Main Thread
// ---------------------------------------------------------------------------
//
TInt CSensrvProxyManager::DuplicateMutexHandle(const RMutex& aSrc, RMutex& aDst) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DuplicateMutexHandle(aSrc.Handle(): 0x%x, aDst.Handle(): 0x%x)"),aSrc.Handle(), aDst.Handle() ) );

    aDst.SetHandle(aSrc.Handle());
    TInt err = aDst.Duplicate(iServerThread);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::DuplicateMutexHandle - return %d" ), err) );

    return err;
    }

// ---------------------------------------------------------------------------
// shuts down server gracefully
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::ShutdownServer(const RMessage2& aMessage)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer(<RMessage2>)") ) );

    if (iShutdownMessage.Handle())
        {
        // Already shutting down, complete
        aMessage.Complete(KErrNone);
        }
    else
        {
        iShutdownMessage = aMessage;
        ShutdownServer();
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer - return" )) );
    }

// ---------------------------------------------------------------------------
// shuts down server gracefully
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::ShutdownServer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer()") ) );

    if (!iShutdown)
        {
        iShutdown = ETrue;
        iShuttingDownCount = 0;

        // Cleanup proxies and solve amount of proxies to shutdown
        TInt proxyCount = iProxyList.Count();
        for (TInt i = 0; i < proxyCount; i++)
            {
            // Set deletion flag to disable usage attempts if shutdown was requested by client.
            // Othwerwise usage attempt is allowed to cancel shutdown.
            if (iShutdownMessage.Handle())
                {
                iProxyList[i]->SetDeletionFlag();
                }

            CSensrvPluginProxy::TPluginState pluginState = iProxyList[i]->PluginState();
            if (pluginState == CSensrvPluginProxy::EPluginStateLoaded ||
                pluginState == CSensrvPluginProxy::EPluginStateThreadInitializing ||
                pluginState == CSensrvPluginProxy::EPluginStateThreadInitialized)
                {
                iProxyList[i]->CleanupPlugin();
                iShuttingDownCount++;
                }
            else if (pluginState == CSensrvPluginProxy::EPluginStateUnloading)
                {
                // Plugin already unloading
                iShuttingDownCount++;
                }
            }

        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer - proxies to shutdown %d" ),
            iShuttingDownCount ) );

        // Shutdown right away, if no proxies to unload
        if (!iShuttingDownCount)
            {
            SsyThreadTerminated();
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer - Already shutting down" )) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ShutdownServer - return" )) );
    }

// ---------------------------------------------------------------------------
// Handles proxy thread termination
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::SsyThreadTerminated()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SsyThreadTerminated() shutdown status %d"), iShutdown ) );

    if (iShutdown)
    	{
        if (iShuttingDownCount > 0)
            {
            --iShuttingDownCount;
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SsyThreadTerminated - proxies left %d "), iShuttingDownCount ) );
            }
        else
            {
            iShuttingDownCount = 0;
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SsyThreadTerminated - Proxies already shutdown") ) );
            }

        if (!iShuttingDownCount)
            {
            if (iShutdownMessage.Handle())
                {
                iShutdownMessage.Complete(KErrNone);
                }
            if (CActiveScheduler::Current())
                {
                CActiveScheduler::Stop();
                CActiveScheduler::Install(NULL);
                }
            }
    	}

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SsyThreadTerminated - return" )) );
    }

// ---------------------------------------------------------------------------
// Compares current implementation infos to new implementation infos
// and determines if new proxies need to be loaded or old ones deleted.
// ---------------------------------------------------------------------------
//
TBool CSensrvProxyManager::EcomChanged( RImplInfoPtrArray* aImplementationInfos )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged(<array>)") ) );

    TBool changes(EFalse);
    TInt i(0);
    TInt j(0);
    TInt found(KErrNotFound);
    TInt err(KErrNone);
    TBool changeOk(ETrue);

    __ASSERT_ALWAYS(aImplementationInfos, User::Panic(KSensrvPanicCategory, ESensrvPanicNullImplInfos));

    // If no array previously registered this is first registration.
    if (!iSsyImplInfoArray)
        {
        // First registration should always have some plugins or it is considered an error, although
        // there might be devices that do not have any sensors as default. Since we only trace the
        // error, this is not a problem.
        if (!aImplementationInfos->Count())
            {
            ERROR_TRACE( ( _L("Sensor Server - CSensrvProxyManager::EcomChanged - Possible ERROR: No SSY plugins found. (This might be ok for some devices with no internal sensors)" ) ) );
            }
        }
    else
        {
        // If still loading initial plugins, do not allow new changes until initial loads have completed.
        if (!iAllProxiesInitialized)
            {
            changeOk = EFalse;
            }
        }

    if (changeOk)
        {
        // Compare current implementation infos to new ones to find new plugins
        for (i = 0; i < aImplementationInfos->Count(); i++)
            {
            found = KErrNotFound;

            // Skip comparisons on the first time when there is no array to compare
            if (iSsyImplInfoArray)
                {
                for (j = 0; found == KErrNotFound && j < iSsyImplInfoArray->Count(); j++)
                    {
                    if ((*iSsyImplInfoArray)[j]->ImplementationUid() == (*aImplementationInfos)[i]->ImplementationUid())
                        {
                        found = j;
                        }
                    }
                }

            // Create and initialize new proxy
            if (found == KErrNotFound)
                {
                changes = ETrue;
                CSensrvPluginProxy* proxy = AddProxy((*aImplementationInfos)[i]->ImplementationUid());

                if (proxy)
                    {
                    CImplementationInformation* implInfo = (*aImplementationInfos)[i];
                    err = proxy->InitializeChannelData( *implInfo );
                    if ( err != KErrNone )
                        {
                        // If proxy initialization fails, it means that proxy will not provide any channels.
                        // Since SSY thread has not been resumed yet (resume is the last thing in InitializeL), proxy
                        // can be safely deleted without risk for memory leak.
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged - ERROR: Proxy initialization failed, error %d" ),  err) );
                        TInt pos( iProxyList.Find( proxy ) );
                        delete proxy;
                        proxy = NULL;
                        if ( pos >= 0 && pos < iProxyList.Count() )
                            {
                            iProxyList.Remove(pos);
                            }
                        }
                    // Note: New channels are notified to interested clients after they are registered.
                    }
                }
            }

        NotifyProxyInitialized();

        // Skip the following comparisons entirely on the first time
        if (iSsyImplInfoArray)
            {
            // Compare current implementation infos to new ones to find removed plugins
            for (i = 0; i < iSsyImplInfoArray->Count(); i++)
                {
                found = KErrNotFound;

                for (j = 0; found == KErrNotFound  && j < aImplementationInfos->Count(); j++)
                    {
                    if ((*iSsyImplInfoArray)[i]->ImplementationUid() == (*aImplementationInfos)[j]->ImplementationUid())
                        {
                        found = j;
                        }
                    }

                // Delete obsolete proxy
                if (found == KErrNotFound)
                    {
                    changes = ETrue;

                    // Find proxy from proxy list
                    for (TInt z=0; found == KErrNotFound && z < iProxyList.Count(); z++)
                        {
                        if (iProxyList[z]->ImplementationUid() == (*iSsyImplInfoArray)[i]->ImplementationUid())
                            {
                            // Mark proxy for deletion.
                            iProxyList[z]->SetDeletionFlag();

                            // Notify channel removal
                            for (TInt x = 0; x < iProxyList[z]->ChannelInfoList().Count(); x++)
                                {
                                NotifyChannelChange((iProxyList[z]->ChannelInfoList())[x], ESensrvChannelChangeTypeRemoved);
                                }

                            // Clear any channels provided by proxy, so that it will not
                            // show up in further finds.
                            iProxyList[z]->ChannelInfoList().Reset();

                            // If plugin is not unloaded, it cannot be deleted without cleaning it up properly.
                            // Actual delete is done in ProxyCleanerCallback() in this case
                            if (iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateLoaded
                                || iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateThreadInitializing
                                || iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateThreadInitialized
                                || iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateUnloading)
                                {
                                // Cleanup plugin unless it is already unloading
                                if (iProxyList[z]->PluginState() != CSensrvPluginProxy::EPluginStateUnloading)
                                    {
                                    iProxyList[z]->CleanupPlugin();
                                    }

                                if (!iProxyCleaner)
                                    {
                                    iProxyCleaner = CPeriodic::New(CActive::EPriorityStandard);

                                    if (iProxyCleaner)
                                        {
                                        iProxyCleaner->Start(TTimeIntervalMicroSeconds32( KSensrvEcomRescanTimerPeriod ),
                                                             TTimeIntervalMicroSeconds32( KSensrvEcomRescanTimerPeriod ),
                                                             TCallBack( ProxyCleanerCallback, this ));
                                        }
                                    else
                                        {
                                        // Since cleaner cannot be constructed, proxy is left alive. Since it is marked
                                        // for deletion, it will be quite unusable, however.
                                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged - Unable to create proxy cleaner" ) ) );
                                        }
                                    }
                                }
                            else
                                {
                                // Delete plugin outright, plugin is unloaded or uninitialized
                                delete iProxyList[z];
                                iProxyList.Remove(z);
                                }

                            found = z;
                            }
                        }
                    }
                }
            }

        // Update iSsyImplInfoArray
        if (!iSsyImplInfoArray)
            {
            // First time always assign the array
            iSsyImplInfoArray = aImplementationInfos;
            }
        else if (changes)
            {
            iSsyImplInfoArray->ResetAndDestroy();
            delete iSsyImplInfoArray;
            iSsyImplInfoArray = aImplementationInfos;
            }
        else
            {
            // No changes, just destroy new infos
            aImplementationInfos->ResetAndDestroy();
            delete aImplementationInfos;
            }

        // If no proxies were successfully initialized,
        // set initialized flag so that finds will not wait forever.
        if (!iProxyList.Count())
            {
            iAllProxiesInitialized = ETrue;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged - return %d" ), changeOk) );

    return changeOk;
    }

// ---------------------------------------------------------------------------
// Orders server to notify all interested client sessions. Notifications
// are not sent until all proxies have finished initializing to
// avoid notifications about initial plugin loads.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::NotifyChannelChange(const TSensrvResourceChannelInfo& aChangedChannel,
                                              TSensrvChannelChangeType aChangeType )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::NotifyChannelChange(aChangedChannel.iChannelId: %d, aChangeType: %d)"), aChangedChannel.iChannelId, aChangeType ) );

    if (iAllProxiesInitialized)
        {
        iServer.NotifyChannelChange(aChangedChannel, aChangeType);
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::NotifyChannelChange - return" )) );
    }

// ---------------------------------------------------------------------------
// Cleans up everything related to terminated session.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::SessionTerminated( CSensrvSession* aSession )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SessionTerminated(aSession: 0x%x)"),aSession ) );

    // Cleanup any waiting transactions for this session
    iWaitQueueQueryChannels->Remove(aSession);
    iWaitQueueOpenChannel->Remove(aSession);

    if (iSessionCounter > 0)
        {
        iSessionCounter--;
        }
    else
        {
        iSessionCounter = 0;
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SessionTerminated - Already no sessions") ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SessionTerminated - Amount of sessions %d"), iSessionCounter ) );

    if (!iSessionCounter)
        {
        // Shutdown server after timer since no more clients
        iServerShutdown->Start();
        }

    // Call session cleanup on each proxy
    TInt proxyCount = iProxyList.Count();
    for(TInt i = 0; i < proxyCount; i++)
        {
        iProxyList[i]->SessionTerminated(aSession);
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::SessionTerminated - return" )) );
    }


// ---------------------------------------------------------------------------
// Executes queued transactions.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::HandleTransactionsQueryChannels()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactionsQueryChannels()") ) );

    if (iWaitQueueQueryChannels)
        {
        while (!iWaitQueueQueryChannels->IsEmpty())
            {
            TInt err(KErrNone);

            CSensrvTransaction* transaction = iWaitQueueQueryChannels->First();

            if (transaction->State() == CSensrvTransaction::ETransStateQueued)
                {
                // Read required channel information from message
                TSensrvResourceChannelInfoPckgBuf pckg;

                __ASSERT_ALWAYS(transaction->Message(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullMessage));

                err = transaction->Message()->Read( KSensrvQueryChannelsQueryParametersSlot, pckg );
                if(err == KErrNone && transaction->Message()->Handle())
                    {
                    TSensrvResourceChannelInfo queryInfo = pckg();
                    const RMessage2& queryMessage(transaction->Message()->GetMessage());

                    if (LoadDynamicChannelSsysIfNeeded(queryInfo, queryMessage, EFalse))
                        {
                        // Continue wait queue processing after the required SSYs are loaded
                        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactionsQueryChannels() - Waiting for SSYs to be loaded") ) );
                        break;
                        }

                    transaction->SetState(CSensrvTransaction::ETransStateExecuting);

                    // Go through all proxies and add matches to return array.
                    RSensrvChannelInfoList matchingInfos;
                    TInt proxyCount = iProxyList.Count();
                    for (TInt i = 0; i < proxyCount; ++i)
                        {
                        CSensrvPluginProxy* proxy = iProxyList[i];
                        TInt channelCount = proxy->ChannelInfoList().Count();
                        for (TInt j = 0; j < channelCount; ++j)
                            {
                            const TSensrvResourceChannelInfo& info(proxy->ChannelInfoList()[j]);
                            if (info.IsMatch(queryInfo, queryMessage))
                                {
                                err = matchingInfos.Append(info);
                                if (err != KErrNone)
                                    {
                                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactionsQueryChannels - ERROR: Append of matching info failed, error %d" ), err) );
                                    break;
                                    }
                                }
                            }
                        }
#ifdef COMPONENT_TRACE_DEBUG
                    COMPONENT_TRACE( ( _L( "### Found channels:" )) );
                    TraceChannelInfoList(matchingInfos);
#endif
                    if (err == KErrNone)
                        {
                        // Write found infos to message
                        err = transaction->SetMessageData(&matchingInfos);
                        }

                    matchingInfos.Reset();
                    }
                else
                    {
                    transaction->SetState(CSensrvTransaction::ETransStateExecuting);
                    }

                if (err != KErrNone)
                    {
                    transaction->SetErrorCode(err);
                    }

                // Complete transaction
                iWaitQueueQueryChannels->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);
                transaction = NULL;
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactionsQueryChannels - Transaction already executing, do nothing." )) );
                }
            } // end while
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactionsQueryChannels - return" )) );
    }

// ---------------------------------------------------------------------------
// Executes queued transactions.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::HandleTransactionsOpenChannel()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactions()") ) );

    if (iWaitQueueOpenChannel)
        {
        while (!iWaitQueueOpenChannel->IsEmpty())
            {
            TInt err(KErrNone);

            CSensrvTransaction* transaction = iWaitQueueOpenChannel->First();

            if (transaction->State() == CSensrvTransaction::ETransStateQueued)
                {
                transaction->SetState(CSensrvTransaction::ETransStateExecuting);

                __ASSERT_ALWAYS(transaction->Message(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullMessage));

                // Get channel ID from message
                // No need to check error value, we already made sure the message function is correct
                TSensrvChannelId channelId(0);
                err = transaction->Message()->GetChannelId(channelId);

                if(err == KErrNone)
                    {

                    // Determine which proxy handles this channel
                    CSensrvPluginProxy* proxy = GetProxyForChannel(channelId);

                    if( proxy != NULL &&
                                ( proxy->PluginState() == CSensrvPluginProxy::EPluginStateUnloaded ||
                                  proxy->PluginState() == CSensrvPluginProxy::EPluginStateUninitialized ) )
                        {
                        TRAPD(err, proxy->InitializePluginL());
                        if ( err != KErrNone )
                            {
                            // If proxy initialization fails, it means that proxy will not provide any channels.
                            // Since SSY thread has not been resumed yet (resume is the last thing in InitializeL), proxy
                            // can be safely deleted without risk for memory leak.
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::EcomChanged - ERROR: Proxy initialization failed, error %d" ),  err) );
                            TInt pos( iProxyList.Find( proxy ) );
                            delete proxy;
                            proxy = NULL;
                            if ( pos >= 0 && pos < iProxyList.Count() )
                                {
                                iProxyList.Remove(pos);
                                }
                            }
                        }

                    // Dispatch the message for found proxy to be handled
                    if (proxy != NULL)
                        {
                        proxy->DispatchMessage(*transaction->Message(), channelId); // Transfer ownership of message
                        }
                    }
                else
                    {
                    transaction->SetErrorCode(err);
                    }
                // Complete transaction
                iWaitQueueOpenChannel->Remove(transaction, CSensrvTransactionQueue::ERemovalTypeComplete);
                transaction = NULL;
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactions - Transaction already executing, do nothing." )) );
                }
            } // end while
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::HandleTransactions - return" )) );
    }

// ---------------------------------------------------------------------------
// Checks all channels provided by proxies and returns information matching
// the query parameters.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::QueryChannels(CSensrvMessage& aMessage)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels(aMessage: 0x%x, <aQueryParams>)" ), &aMessage ) );

    // Create transaction
    CSensrvTransaction* queryTransaction = NULL;
    TRAPD(err, queryTransaction = CSensrvTransaction::NewL(
        &aMessage,
        NULL,
        NULL,
        CSensrvTransaction::ETransTypeClientQueryChannels));

    if (err == KErrNone)
        {
        // queue transaction
        queryTransaction->SetState(CSensrvTransaction::ETransStateQueued);

        err = iWaitQueueQueryChannels->Add(queryTransaction);

        if (err == KErrNone)
            {
            // Execute transaction if all proxies initialized
            if (iAllProxiesInitialized)
                {
                HandleTransactionsQueryChannels();
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - All proxies not yet initialized") ) );
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - ERROR: Transaction adding to queue failed" )) );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - ERROR: Transaction creation failed" )) );
        }

    // Handle error
    if (err != KErrNone)
        {
        if(queryTransaction)
            {
            queryTransaction->SetErrorCode(err);
            queryTransaction->Complete();
            delete queryTransaction;
            queryTransaction = NULL;
            }
        else
            {
            // Do nothing, message gets deleted and completed with KErrGeneral on transaction desctructor.
            }
        }


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - return" )) );
    }

// ---------------------------------------------------------------------------
// Checks all channels provided by proxies and returns information matching
// the query parameters.
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::OpenChannel(CSensrvMessage& aMessage)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels(aMessage: 0x%x, <aQueryParams>)" ), &aMessage ) );

    // Create transaction
    CSensrvTransaction* queryTransaction = NULL;
    TRAPD(err, queryTransaction = CSensrvTransaction::NewL(
        &aMessage,
        NULL,
        NULL,
        CSensrvTransaction::ETransTypeOpenChannel));

    if (err == KErrNone)
        {
        // queue transaction
        queryTransaction->SetState(CSensrvTransaction::ETransStateQueued);

        err = iWaitQueueOpenChannel->Add(queryTransaction);

        if (err == KErrNone)
            {
            // Execute transaction if all proxies initialized
            if (iAllProxiesInitialized)
                {
                HandleTransactionsOpenChannel();
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - All proxies not yet initialized") ) );
                }
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - ERROR: Transaction adding to queue failed" )) );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - ERROR: Transaction creation failed" )) );
        }

    // Handle error
    if (err != KErrNone)
        {
        if(queryTransaction)
            {
            queryTransaction->SetErrorCode(err);
            queryTransaction->Complete();
            delete queryTransaction;
            queryTransaction = NULL;
            }
        else
            {
            // Do nothing, message gets deleted and completed with KErrGeneral on transaction desctructor.
            }
        }


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::QueryChannels - return" )) );
    }

// ---------------------------------------------------------------------------
// Gets the proxy that handles the specified channel.
// NULL is returned if channel is not found.
// ---------------------------------------------------------------------------
//
CSensrvPluginProxy* CSensrvProxyManager::GetProxyForChannel(TSensrvChannelId aChannelId) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::GetProxyForChannel(aChannelId: %d)" ), aChannelId ) );

    CSensrvPluginProxy* returnProxy = NULL;

    TInt proxyCount = iProxyList.Count();
    for(TInt i = 0; !returnProxy && i < proxyCount; i++)
        {
        if (iProxyList[i]->IsChannelSupported(aChannelId))
            {
            returnProxy = iProxyList[i];
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::GetProxyForChannel - return 0x%x" ), returnProxy) );

    return returnProxy;
    }

// ---------------------------------------------------------------------------
// Fetches configurable settings from cenrep
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::GetSettings()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetSettings()" ) ) );

    TInt err(KErrNone);

    // Open settings repository and get necessary values
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL(KCRUidSensorServerSettings) );

    if ( err == KErrNone )
        {
        TInt value(0);
        err = repository->Get(KSensrvTransactionTimeoutKey, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iTransactionTimeout = value*KSecondsToMicros;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvTransactionTimeoutKey value getting failed: %d, using default value." ), err) );
            }

        err = repository->Get(KSensrvSsyThreadStackSizeKey, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iSsyStackSize = value;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvSsyThreadStackSizeKey value getting failed: %d, using default value." ), err) );
            }

        err = repository->Get(KSensrvSsyThreadHeapMaxSizeKey, value);
        if ( err == KErrNone )
            {
#ifdef __WINS__ 
            value *= KSensrvSsyMaxHeapWinsMultiplier;
#endif // __WINS__
            if ( value < KSensrvSsyHeapInitialSize )
                {
                iSsyHeapMaxSize = KSensrvSsyHeapInitialSize;
                }
            else if (value > 0)
                {
                iSsyHeapMaxSize = value;
                }
            else
                {
                // Do nothing, default is used
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvSsyThreadHeapMaxSizeKey value getting failed: %d, using default value." ), err) );
            }

        err = repository->Get(KSensrvThreadTerminationPeriodKey, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iThreadTerminationGracePeriod = value*KSecondsToMicros;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvThreadTerminationPeriodKey value getting failed: %d, using default value." ), err) );
            }

        err = repository->Get(KSensrvSsyInactivityPeriodKey, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iSsyInactivityPeriod = value*KSecondsToMicros;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvSsyInactivityPeriodKey value getting failed: %d, using default value." ), err) );
            }

        err = repository->Get(KSensrvBufferSizeMultiplierKey, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iBufferSizeMultiplier = value;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvBufferSizeMultiplierKey value getting failed: %d, using default value." ), err) );
            }
        err = repository->Get(KSensrvTerminationPeriodKey, value);
        if ( err == KErrNone )
            {
            if ( value >= 0 )
                {
                iTerminationPeriod = value*KSecondsToMicros;
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - KSensrvTerminationPeriodKey value getting failed: %d, using default value." ), err) );
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - Settings repository opening failed: %d" ), err) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iTransactionTimeout: %d" ), iTransactionTimeout.Int()) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iSsyStackSize: %d" ), iSsyStackSize) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iSsyHeapMaxSize: %d" ), iSsyHeapMaxSize) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iThreadTerminationGracePeriod: %d" ), iThreadTerminationGracePeriod.Int()) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iSsyInactivityPeriod: %d" ), iSsyInactivityPeriod.Int()) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvServer::GetSettings - iBufferSizeMultiplier: %d" ), iBufferSizeMultiplier) );

    // Cleanup repository
    delete repository;
    repository = NULL;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvChannel::GetSettings - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Create new proxy and add it to proxy list
// ---------------------------------------------------------------------------
//
CSensrvPluginProxy* CSensrvProxyManager::AddProxy(const TUid& aProxyUid)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddProxy(0x%x)" ), aProxyUid.iUid ) );

    // Create proxy
    TInt err(KErrNone);
    CSensrvPluginProxy* proxy =  NULL;
    TRAP(err, proxy = CSensrvPluginProxy::NewL(*this,aProxyUid));

    if ( err != KErrNone )
        {
        // Error loading a plugin is not critical, it just means
        // that channels provided by plugin are unavailable.
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddProxy - ERROR: creating proxy failed, implementationUid: 0x%x, error: %d" ), aProxyUid.iUid , err ) );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddProxy - Proxy for implementation 0x%x created successfully." ), aProxyUid.iUid ) );

        // Add proxy to list
        err = iProxyList.Append(proxy);
        if ( err != KErrNone )
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddProxy - ERROR: Saving plugin info failed, implementationUid: 0x%x, error %d" ), aProxyUid.iUid , err) );
            delete proxy;
            proxy = NULL;
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddProxy - return 0x%x" ), proxy ) );

    return proxy;
    }

// ---------------------------------------------------------------------------
// CSensrvProxyManager::ProxyCleanerCallback
// ---------------------------------------------------------------------------
//
TInt CSensrvProxyManager::ProxyCleanerCallback( TAny* aObject )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ProxyCleanerCallback(this)" ) ) );

    __ASSERT_ALWAYS(aObject, User::Panic(KSensrvPanicCategory, ESensrvPanicNullCallback));

    CSensrvProxyManager* manager = reinterpret_cast<CSensrvProxyManager*>( aObject );
    TBool noMore(ETrue);

    for (TInt z = manager->iProxyList.Count() - 1; z >= 0; z--)
        {
        if (manager->iProxyList[z]->IsMarkedForDeletion())
            {
            if (manager->iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateLoaded
                || manager->iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateThreadInitializing
                || manager->iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateThreadInitialized
                || manager->iProxyList[z]->PluginState() == CSensrvPluginProxy::EPluginStateUnloading)
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ProxyCleanerCallback - Proxy still not unloaded, try deletion again later" ) ) );
                noMore = EFalse;
                }
            else
                {
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ProxyCleanerCallback - Proxy unloaded, deleting..." ) ) );
                delete manager->iProxyList[z];
                manager->iProxyList.Remove(z);
                }
            }
        }

    // Delete iProxyCleaner if no more
    if (noMore)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ProxyCleanerCallback - No more proxies to unload, deleting cleaner..." ) ) );
        delete manager->iProxyCleaner;
        manager->iProxyCleaner = NULL;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::ProxyCleanerCallback - return " ) ) );

    return KErrNone;
    }

#ifdef COMPONENT_TRACE_DEBUG
// ---------------------------------------------------------------------------
// Traces channel info list contents
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::TraceChannelInfoList(const RSensrvResourceChannelInfoList& aInfoList)
    {
    // Debug found channels
    if (aInfoList.Count())
        {
        for(TInt z = 0; z < aInfoList.Count(); z++)
            {
            // Convert location and vendorID from 8bit to 16 bit for proper printing
            TInt len = aInfoList[z].iVendorId.Length();
            HBufC16* vidBuf16 = HBufC16::New(len);
            if( vidBuf16 )
                {
                vidBuf16->Des().Copy(aInfoList[z].iVendorId);
                }

            len = aInfoList[z].iLocation.Length();
            HBufC16* locBuf16 = HBufC16::New(len);
            if( locBuf16 )
                {
                locBuf16->Des().Copy(aInfoList[z].iLocation);
                }
            COMPONENT_TRACE( ( _L( "### Info %d: iChannelId: %d, iContextType: 0x%x, iQuantity: 0x%x, iChannelType: 0x%x, iLocation: \"%S\", iVendorId: \"%S\", iDataItemSize: %d " ),
                               z,
                               aInfoList[z].iChannelId,
                               aInfoList[z].iContextType,
                               aInfoList[z].iQuantity,
                               aInfoList[z].iChannelType,
                               locBuf16 ? locBuf16 : &KSensrvNullString,
                               vidBuf16 ? vidBuf16 : &KSensrvNullString,
                               aInfoList[z].iDataItemSize) );
            if( locBuf16 )
                delete locBuf16;
            if( vidBuf16 )
                delete vidBuf16;
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "### List is empty." ) ));
        }
    }

// ---------------------------------------------------------------------------
// Traces channel info list contents
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::TraceChannelInfoList(const RSensrvChannelInfoList& aInfoList)
    {
    // Debug found channels
    if (aInfoList.Count())
        {
        for(TInt z = 0; z < aInfoList.Count(); z++)
            {
            // Convert location and vendorID from 8bit to 16 bit for proper printing
            TInt len = aInfoList[z].iVendorId.Length();
            HBufC16* vidBuf16 = HBufC16::New(len);
            if( vidBuf16 )
                {
                vidBuf16->Des().Copy(aInfoList[z].iVendorId);
                }
            
            len = aInfoList[z].iLocation.Length();
            HBufC16* locBuf16 = HBufC16::New(len);
            if( locBuf16 )
                {
                locBuf16->Des().Copy(aInfoList[z].iLocation);
                }
             
            COMPONENT_TRACE( ( _L( "### Info %d: iChannelId: %d, iContextType: 0x%x, iQuantity: 0x%x, iChannelType: 0x%x, iLocation: \"%S\", iVendorId: \"%S\", iDataItemSize: %d " ),
                                               z,
                                               aInfoList[z].iChannelId,
                                               aInfoList[z].iContextType,
                                               aInfoList[z].iQuantity,
                                               aInfoList[z].iChannelType,
                                               locBuf16 ? locBuf16 : &KSensrvNullString,
                                               vidBuf16 ? vidBuf16 : &KSensrvNullString,
                                               aInfoList[z].iDataItemSize) );
             if( locBuf16 )
                 delete locBuf16;
             if( vidBuf16 )
                 delete vidBuf16;
                                        
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "### List is empty." ) ));
        }
    }
#endif

// ---------------------------------------------------------------------------
// Cleanup item implementation for plugin initialization
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::AddSession()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddSession" ) ) );

    if ( iServerShutdown->IsActive( ) )
    	{
    	iServerShutdown->Cancel();
    	}
	iSessionCounter++;

	// Cancel shutdown if it was not requested by client
	if ( !iShutdownMessage.Handle() )
	    {
	    iShutdown = EFalse;
	    }

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::AddSession sessions: %d - return" ), iSessionCounter ) );
    }

// ---------------------------------------------------------------------------
// Loads the dynamic channel SSYs that matches to given query if not loaded
// ---------------------------------------------------------------------------
//
TBool CSensrvProxyManager::LoadDynamicChannelSsysIfNeeded(const TSensrvResourceChannelInfo& aQueryInfo,
                                                          const RMessage2& aQueryMessage,
                                                          TBool aAddChannelChangeListener)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::LoadDynamicChannelSsysIfNeeded" ) ) );
    TBool ssyLoadPending = EFalse;
    TInt proxyCount = iProxyList.Count();
    for (TInt i = 0; i < proxyCount; ++i)
        {
        CSensrvPluginProxy* proxy = iProxyList[i];
        TInt channelCount = proxy->DynamicChannelInfoList().Count();
        for (TInt j = 0; j < channelCount; ++j)
            {
            if (proxy->DynamicChannelInfoList()[j].IsMatch(aQueryInfo, aQueryMessage))
                {
                CSensrvPluginProxy::TPluginState pluginState = proxy->PluginState();
                if (pluginState == CSensrvPluginProxy::EPluginStateUninitialized ||
                    pluginState == CSensrvPluginProxy::EPluginStateUnloaded ||
                    pluginState == CSensrvPluginProxy::EPluginStateUnloading)
                    {
                    if (!proxy->PreviousSsyLoadFailed()) // If loading failed earlier, ignore new attempt to avoid query deadlocks
                        {
                        TRAPD(err, proxy->InitializePluginL());
                        if (err == KErrNone)
                            {
                            ssyLoadPending = ETrue;
                            }
                        else
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::LoadDynamicChannelSsysIfNeeded - ERROR: SSY 0x%x initialization failed, error %d" ),
                                proxy->ImplementationUid().iUid, err ) );
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::LoadDynamicChannelSsysIfNeeded - Ignoring SSY 0x%x initialization" ),
                            proxy->ImplementationUid().iUid ) );
                        }
                    }
                else if (pluginState == CSensrvPluginProxy::EPluginStateThreadInitializing)
                    {
                    ssyLoadPending = ETrue;
                    }
                if (aAddChannelChangeListener)
                    {
                    proxy->AddChannelChangeListener();
                    }
                break; // Move on to next proxy
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::LoadDynamicChannelSsysIfNeeded - return %d" ), ssyLoadPending ) );
    return ssyLoadPending;
    }

// ---------------------------------------------------------------------------
// Removes channel change listener from the matching dynamic channel SSYs to allow unloading
// ---------------------------------------------------------------------------
//
void CSensrvProxyManager::RemoveChannelChangeListenerFromDynamicChannelSsys(const TSensrvResourceChannelInfo& aQueryInfo,
                                                                            const RMessage2& aQueryMessage)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::RemoveChannelChangeListenerFromDynamicChannelSsys" ) ) );

    TInt proxyCount = iProxyList.Count();
    for(TInt i = 0; i < proxyCount; ++i)
        {
        CSensrvPluginProxy* proxy = iProxyList[i];
        TInt channelCount = proxy->DynamicChannelInfoList().Count();
        for(TInt j = 0; j < channelCount; ++j)
            {
            if (proxy->DynamicChannelInfoList()[j].IsMatch(aQueryInfo, aQueryMessage))
                {
                proxy->RemoveChannelChangeListener();
                break; // Move on to next proxy
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvProxyManager::RemoveChannelChangeListenerFromDynamicChannelSsys - return" ) ) );
    }

// End of file
