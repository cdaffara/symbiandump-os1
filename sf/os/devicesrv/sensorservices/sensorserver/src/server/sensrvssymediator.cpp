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
* Description:  Mediates control flow from server main thread to plugin thread
*
*/


#include "sensrvdefines.h"
#include "sensrvssymediator.h"
#include "sensrvservermediator.h"
#include "sensrvpluginproxy.h"
#include "sensrvtransaction.h"
#include "ssycontrol.h"
#include "senserverchannel.h"
#include "sensrvproxymanager.h"
#include "sensrvchannelbuffer.h"
#include "sensrvtransactionqueue.h"
#include "sensrvclientserver.h"

// For tracing channel info contents
#ifdef ERROR_TRACE_DEBUG

static void TraceChannelInfo( const TDesC& aPrefix, const TSensrvChannelInfo& aInfo )
    {
    TBuf<KSensrvLocationLength> location;
    location.Copy( aInfo.iLocation );
    TBuf<KSensrvVendorIdLength> vendor;
    vendor.Copy( aInfo.iVendorId );
    ERROR_TRACE( ( _L("%S iChannelId=%d, iContextType=%d, iQuantity=%d, iChannelType=%d, iLocation=%S, iVendorId=%S, iDataItemSize=%d, iChannelDataTypeId=%d"),
        &aPrefix,aInfo.iChannelId,aInfo.iContextType,aInfo.iQuantity,aInfo.iChannelType,&location,&vendor,aInfo.iDataItemSize,aInfo.iChannelDataTypeId ) );
    }

#define ERROR_TRACE_CHANNELINFO( aPrefix, aChannelInfo ) TraceChannelInfo( aPrefix, aChannelInfo )

#else // ERROR_TRACE_DEBUG

#define ERROR_TRACE_CHANNELINFO( aPrefix, aChannelInfo )

#endif // ERROR_TRACE_DEBUG


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvSsyMediator* CSensrvSsyMediator::NewL(CSensrvPluginProxy* aProxy)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::NewL(aProxy: 0x%x)" ), aProxy->ImplementationUid().iUid ) );

    __ASSERT_ALWAYS(aProxy, User::Panic(KSensrvPanicCategory, ESensrvPanicNullProxy));

    CSensrvSsyMediator* self = new( ELeave ) CSensrvSsyMediator();

    CleanupStack::PushL( self );
    self->BaseConstructL(aProxy);
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::NewL - return 0x%x" ), self ) );

    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvSsyMediator::CSensrvSsyMediator()
    : iUnloading(EFalse)
    {
    // Nothing to do
    }

// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ConstructL()" ) ) );

    // Create ETransTypeMediatorChannelsChanged transaction here so we can be sure
    // it is always available in server heap
    iChannelChangeTransaction = CSensrvTransaction::NewL(
        NULL,
        iProxy,
        NULL,
        CSensrvTransaction::ETransTypeMediatorChannelsChanged);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ConstructL - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvSsyMediator::~CSensrvSsyMediator()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::~CSensrvSsyMediator()" ) ) );

    // NOTE that iSsyControl cannot be deleted here, as this is called from server thread.
    if ( iSsyControl )
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::~CSensrvSsyMediator - iSsyControl still non-NULL at SSY mediator destructor" ) ) );

        // Since mediator has not been cleaned up properly, cancel at destructor will hang,
        // waiting for request completion that will never come. To avoid this,
        // complete request pre-emptively here.
        if ( IsActive() )
            {
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrCancel );
            }
        }

    Destruct();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::~CSensrvSsyMediator - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::Destruct()
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::Destruct()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Destruct()" ) ) );

    delete iChannelChangeTransaction;
    iChannelChangeTransaction = NULL;

    if (iProxy && iRawInfos)
        {
        iProxy->ProxyManager().ServerHeap()->Free( iRawInfos );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Destruct - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::Cleanup()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Cleanup(), Mediated thread: %S" ), &iThreadName ) );


    iMutex->Wait(); 
    RemoveAllTransactions();

    if (!iUnloading)
        {
        iUnloading = ETrue;

        // Complete to cause RunL execution in SSY thread, which will then stop
        // SSY scheduler, which will lead to SSY thread termination.
        CompleteRequest(KErrNone);
        }

    iMutex->Signal(); 
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Cleanup - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Open channel
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::ChannelOpened( TSensrvChannelId aChannelId,
                                        TInt aErrorCode,
                                        MSsyChannelDataProvider* aChannelDataProvider,
                                        MSsyPropertyProvider* aSensorPropertyProvider )
    {
    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelOpened(aChannelID: 0x%x, aErrorCode: %d, aChannelDataProvider: 0x%x, aSensorPropertyProvider: 0x%x )" ), aChannelId, aErrorCode, aChannelDataProvider, aSensorPropertyProvider ) );

    iMutex->Wait();

    // Get channel object
    CSensrvChannel* channel = iProxy->GetChannelForId(aChannelId);

    if ( channel )
        {
        channel->ChannelOpened(aErrorCode, aChannelDataProvider, aSensorPropertyProvider);
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelOpened - ERROR: ChannelOpened for nonexistent channel" ) ) );
        }

    iMutex->Signal();

    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelOpened - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Close channel
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::ChannelClosed( TSensrvChannelId aChannelId)
    {
    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelClosed(aChannelId: 0x%x)" ), aChannelId ) );

    iMutex->Wait();

    // Get channel object
    CSensrvChannel* channel = iProxy->GetChannelForId(aChannelId);

    if (channel)
        {
        channel->ChannelClosed();
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelOpened - ERROR: ChannelClosed for nonexistent channel" ) ) );
        }

    iMutex->Signal();

    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::ChannelClosed - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Request new block from channel buffer and notify server mediator
// that there is new data in buffer.
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::BufferFilled( TSensrvChannelId aChannelId,
                                  TInt aWriteCount,
                                  TUint8*& aNewBuffer,
                                  TInt &aNewCount)
    {
    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled(aChannelId: 0x%x, aWriteCount: %d, <newbuffer>, <newcount>), mediated thread: %S" ), aChannelId, aWriteCount, &iThreadName ) );

    iMutex->Wait();

    // Get channel object
    CSensrvChannel* channel = iProxy->GetChannelForId(aChannelId);

    // If aWriteCount is negative, an error has occurred and the channel must be closed
    if (channel)
        {
        CSensrvTransaction* transaction = NULL;

        if (aWriteCount < 0)
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled - ERROR: Negative aWriteCount, channel must be closed: %d" ), aWriteCount ) );

            transaction = channel->ForceChannelCloseTransaction();
            }
        else if (channel->Buffer())
            {
            // Validate buffer block and request new data
            channel->Buffer()->WroteBlock(aWriteCount);

            aNewCount  = channel->RecalculateBufferingCount();
            if (aNewCount)
                {
                aNewBuffer = channel->Buffer()->GetFreeBlock(aNewCount);
                transaction = channel->DataAvailableTransaction();
                }
            else
                {
                // Zero count means no clients, so channel needs to be closed.
                transaction = channel->ForceChannelCloseTransaction();
                }
            }
        else
            {
            // Do nothing, if buffer is not valid
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled - ERROR: Buffer channel doesn't exist." ) ) );
            }

        // Notify server thread that new data was written or close is needed.
        // Note that even if data count was zero, notification is needed.
        if (transaction)
            {
            // No need to add transaction to transaction monitor,
            // because if server main thread causes timeout, we probably have
            // problems that will not be fixed by SSY reloading, and
            // in any case, it is unlikely that monitor will even run.

            TInt err = iProxy->ServerMediator()->Notify(transaction);

            if ( err != KErrNone )
                {
                // Just trace, HandleNotifyFailure will take care of failure handling.
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled - ERROR: Server mediator notify failed: %d" ), err ) );
                }
            }
        }
    else
        {
        // Do nothing, if channel is not valid
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled - Channel has been cleaned up" ) ) );
        }


    iMutex->Signal();

    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::BufferFilled - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::PropertyChanged
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::PropertyChanged( const TSensrvChannelId /*aChannelId*/, // aChannelId is not needed
                                          const RSensrvChannelList& aAffectedChannels,
                                          const TSensrvProperty& aProperty )
    {
    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PropertyChanged(<aAffectedChannels>, <propertylist>)" ) ) );

    iMutex->Wait();

    //Go through all affected channels
    for( TInt i=0 ; i<aAffectedChannels.Count() ; i++ )
        {
        // Get channel object
        CSensrvChannel* channel = iProxy->GetChannelForId( aAffectedChannels.operator[]( i ) );

        // Note: It is technically possible that channel object has been deleted because plugin unload
        // due e.g. transaction timeout has just started, although it is highly unlikely.
        if ( channel )
            {
            TBool propNotifyNeeded = channel->StoreChangedProperty( aProperty, NULL );

            if ( propNotifyNeeded )
                {
                CSensrvTransaction* transaction = channel->PropertyChangedTransaction();

                // Notify server thread that a property has been changed.
                if( transaction )
                    {
                    TInt err = iProxy->ServerMediator()->Notify( transaction );

                    if ( err != KErrNone )
                        {
                        // Just trace, HandleNotifyFailure will take care of failure handling.
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PropertyChanged - ERROR: Server mediator notify failed: %d" ), err ) );
                        }
                    }
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PropertyChanged - Channel has been deleted." ) ) );
            }
        }//for

    iMutex->Signal();

    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PropertyChanged - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Fetches new unique channel id from proxy manager
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::RegisterChannelsL( RSensrvChannelInfoList& aChannelInfoList)
    {
    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL(<infolist>)" ) ) );

    iMutex->Wait();

    TInt err( KErrNone );

    if ( !iUnloading )
        {
        TInt newCount = aChannelInfoList.Count();
        TInt oldCount = iProxy->ChannelInfoList().Count();
        TInt dynamicCount = iProxy->DynamicChannelInfoList().Count();
        TInt ancientCount = iProxy->RemovedChannelsInfoList().Count();

        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - SSY: 0x%x Old count: %d, New count: %d, Ancient count %d, Dynamic count %d" ),
            iProxy->ImplementationUid().iUid, oldCount, newCount, ancientCount, dynamicCount ) );

        // Check if old array contains these channels
        for (TInt i = 0; i < newCount; ++i)
            {
            TSensrvChannelInfo& channelInfo( aChannelInfoList[i] );

            // Ensure that info is complete and the actual data item size is set
            if ( channelInfo.IsComplete() && channelInfo.iDataItemSize > 0 )
                {
                // Set channel id to zero just in case
                channelInfo.iChannelId = 0;

                // Get channel ID from old active channels
                TInt index = CSensrvPluginProxy::FindChannel( channelInfo, iProxy->ChannelInfoList() );
                if ( index >= 0 && index < oldCount )
                    {
                    channelInfo.iChannelId = iProxy->ChannelInfoList()[index].iChannelId;
                    }

                if ( !channelInfo.iChannelId )
                    {
                    index = CSensrvPluginProxy::FindChannel( channelInfo, iProxy->DynamicChannelInfoList() );
                    if ( index >= 0 && index < dynamicCount )
                        {
						channelInfo.iChannelId = iProxy->DynamicChannelInfoList()[index].iChannelId;
						}
					}

                if ( !channelInfo.iChannelId )
                    {
					index = CSensrvPluginProxy::FindChannel( channelInfo, iProxy->RemovedChannelsInfoList() );
					if ( index >= 0 && index < ancientCount )
						{
						channelInfo.iChannelId = iProxy->RemovedChannelsInfoList()[index].iChannelId;
						}
					}

				if ( !channelInfo.iChannelId )
					{
					channelInfo.iChannelId = static_cast<TSensrvChannelId>( iProxy->ProxyManager().GenerateUniqueId() );
					}

				if ( !channelInfo.iChannelId )
					{
					ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Registered channel data (SSY 0x%x channel index: %d) not present in SSY resource" ),
						iProxy->ImplementationUid().iUid, i ) );
					ERROR_TRACE_CHANNELINFO( _L( "Invalid channel info" ), channelInfo );
					err = KErrArgument;
					}
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Incomplete channel data registered (SSY 0x%x channel index: %d)" ),
                    iProxy->ImplementationUid().iUid, i ) );
                ERROR_TRACE_CHANNELINFO( _L( "Incomplete channel info" ), channelInfo );
                err = KErrArgument;
                }
            }

        if ( err == KErrNone )
            {
            // Check if all static channels are registered
            for (TInt i = 0; i < oldCount; ++i)
                {
                const TSensrvResourceChannelInfo& oldInfo(iProxy->ChannelInfoList()[i]);
                if (!oldInfo.iDynamic)
                    {
                    if (CSensrvPluginProxy::FindChannel(aChannelInfoList, oldInfo) == KErrNotFound)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Channel data (SSY 0x%x channel id: %d channel index: %d) not registered" ),
                            iProxy->ImplementationUid().iUid, oldInfo.iChannelId, i ) );
                        ERROR_TRACE_CHANNELINFO( _L( "Not registered channel info" ), oldInfo );
                        err = KErrArgument;
                        }
                    }
                }
            }

        // Ensure that SSY given channel infos were valid
        __ASSERT_DEBUG( err == KErrNone, User::Panic( KSensrvPanicCategory, ESensrvPanicUnexpectedSsyCallback ) );

        if ( err == KErrNone )
            {
            // Allocate space to proxy's heap for count and infos, and copy info count and raw info data there
            iProxy->ProxyManager().ServerHeap()->Free(iRawInfos);
            TInt rawDataSize(newCount*sizeof(TSensrvChannelInfo));
            iRawInfos = iProxy->ProxyManager().ServerHeap()->Alloc(sizeof(newCount) + rawDataSize);
            if (iRawInfos)
                {
                TUint8* contPtr = Mem::Copy(iRawInfos, &newCount, sizeof(newCount));
                Mem::Copy(contPtr, reinterpret_cast<TAny*>(&(aChannelInfoList[0])), rawDataSize);

                // Notify proxy with ETransTypeMediatorChannelsChanged transaction,
                // unless this is during load (i.e. when iSsyControl is still NULL).
                if (iSsyControl)
                    {
                    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - Notifying proxy with ETransTypeMediatorChannelsChanged" ) ) );
                    err = iProxy->ServerMediator()->Notify(iChannelChangeTransaction);
                    if ( err != KErrNone )
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Server mediator notify failed for ETransTypeMediatorChannelsChanged: %d" ), err ) );
                        }
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Failure to copy raw data" ) ) );
                err = KErrNoMemory;
                }
            }
        else
            {
            // Mark proxy as bad and notify to remove its channels before unloading it
            iProxy->SetDeletionFlag();
            if (iSsyControl)
                {
                TInt err2 = iProxy->ServerMediator()->Notify(iChannelChangeTransaction);
                if (err2 != KErrNone)
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - ERROR: Server mediator notify failed for ETransTypeMediatorChannelsChanged: %d" ), err2 ) );
                    }
                }
            }
        }
    else
        {
        // Do nothing, cleanup ongoing
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - NULL proxy" ) ) );
        }

    iMutex->Signal();

    // Now that mutex has been signaled, it is ok to leave.
    User::LeaveIfError(err);

    API_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::RegisterChannelsL - return" ) ) );

    }

// -----------------------------------------------------------------------------
// Gets callback interface
// -----------------------------------------------------------------------------
//
void CSensrvSsyMediator::GetCallbackInterfaceL( TUid aInterfaceUid,
	                                              TAny*& aInterface )
    {
    aInterface = NULL;

    if ( aInterfaceUid.iUid == KSsyCallbackInterface1.iUid )
		    {
		    aInterface = reinterpret_cast<TAny*>( this );
		    }
    }

// ---------------------------------------------------------------------------
// Initializes mediator.
// Completion is not yet enabled. It will be enabled when scheduler has
// run at least one idle loop to ensure mediator request is not completed
// before scheduler is activated.
// ---------------------------------------------------------------------------
//
TInt CSensrvSsyMediator::Initialize()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Initialize()" ) ) );

    TInt err(KErrNone);

    if ( !iInitialized )
        {
        // Init mutex pointer
        iMutex = new RMutex();

        if (iMutex)
            {
            err = iProxy->ProxyManager().DuplicateMutexHandle(iProxy->Mutex(), *iMutex);

            if ( err == KErrNone )
                {
                err = CSensrvMediatorBase::Initialize();
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Initialize - ERROR: Mutex duplication failed" ) ) );
                }
            }
        else
            {
            err = KErrNoMemory;
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Initialize - ERROR: Creating iMutex failed" ) ) );
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::Initialize - return %d" ), err ) );

    return err;
    }

// ---------------------------------------------------------------------------
// Handles transaction
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::HandleTransaction(CSensrvTransaction* aTransaction)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction(aTransaction: 0x%x), Mediated thread: %S" ), aTransaction, &iThreadName ) );

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    TInt heapSize = User::Heap().Size();
    TInt biggestBlock(0);
    TInt heapAvail = User::Heap().Available(biggestBlock);
    TInt used(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### %S - HEAP: Size: %d, Available: %d, Used: %d" ), &iThreadName, heapSize, heapAvail, used ) );
#endif

    if (aTransaction && !iUnloading)
        {
        // Handle transaction according to transaction type
        TInt err(KErrNone);

        TBool notifyServer(ETrue);

        // Grab necessary data from transaction and do whatever is necessary
        switch(aTransaction->Type())
            {
            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeLoadSsy:
                {
                __ASSERT_ALWAYS(!iSsyControl, User::Panic(KSensrvPanicCategory, ESensrvPanicSsyControlExists));

                // Create SSY control object
                iMutex->Signal();
                TRAP(err, iSsyControl = CSsyControl::NewL(*this, iProxy->ImplementationUid()));
                iMutex->Wait();

                if ( err != KErrNone )
                    {
                    // Set error to transaction, as loading failed
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Loading SSY failed: %d at thread %S" ),err, &iThreadName ) );
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeOpenChannel:
                {
                // Channel must always exist in transaction and be in closed state
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->State() == CSensrvChannel::EChannelStateClosed, User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidChannelState));
                __ASSERT_ALWAYS(iSsyControl, User::Panic(KSensrvPanicCategory, ESensrvPanicNullSsyControl));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());

                iMutex->Signal();
                TRAP(err, iSsyControl->OpenChannelL(channelId));
                iMutex->Wait();

                if ( err != KErrNone )
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Opening channel failed: %d at thread %S" ),err, &iThreadName ) );
                    // Failure to open channel means channel will be left closed
                    // Client will also get error
                    }
                else
                    {
                    // Must not notify server yet, SSY operation is asynchronous
                    notifyServer = EFalse;
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeCloseChannel:
                {
                // Channel must always exist in transaction and be in non-closed state
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->State() != CSensrvChannel::EChannelStateClosed, User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidChannelState));
                __ASSERT_ALWAYS(iSsyControl, User::Panic(KSensrvPanicCategory, ESensrvPanicNullSsyControl));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());

                iMutex->Signal();
                TRAP(err, iSsyControl->CloseChannelL(channelId));
                iMutex->Wait();

                if ( err != KErrNone )
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Closing channel failed: %d at thread %S" ),err, &iThreadName ) );
                    // Failure to close channel means channel will be left open.
                    // Client listener will however be closed and client gets no error.
                    }
                else
                    {
                    // Must not notify server yet, SSY operation is asynchronous
                    notifyServer = EFalse;
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeStartListening:
            case CSensrvTransaction::ETransTypeStartConditionListening:
                {
                // Channel must always exist in transaction and be in open state
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->State() != CSensrvChannel::EChannelStateClosed, User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidChannelState));
                __ASSERT_ALWAYS(aTransaction->Channel()->DataProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));
                __ASSERT_ALWAYS(aTransaction->Channel()->Buffer(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullBuffer));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());
                MSsyChannelDataProvider* provider = aTransaction->Channel()->DataProvider();

                // If state is open, start listening, otherwise just force buffer fill.
                if (aTransaction->Channel()->State() == CSensrvChannel::EChannelStateOpen)
                    {
                    // If channel has been previously listened to, there probably still is
                    // open free block request, so cancel it.
                    aTransaction->Channel()->Buffer()->CancelFreeBlock();

                    TInt count = aTransaction->Channel()->RecalculateBufferingCount();
                    if (count)
                        {
                        TUint8* ptr = aTransaction->Channel()->Buffer()->GetFreeBlock(count);

                        iMutex->Signal();
                        TRAP(err, provider->StartChannelDataL(channelId, ptr, count));
                        iMutex->Wait();
                        }
                    else
                        {
                        // Zero count means there are no clients. Return error to cause close.
                        err = KErrNotFound;
                        }

                    if ( err != KErrNone )
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Starting listening failed: %d at thread %S" ),err, &iThreadName ) );
                        // Failure to start channel data means just that.
                        // Client gets error as well.
                        }
                    }
                else // aTransaction->Channel()->State() == CSensrvChannel::EChannelStateListening
                    {
                    iMutex->Signal();
                    TRAP(err, provider->ForceBufferFilledL(channelId));
                    iMutex->Wait();

                    if ( err != KErrNone )
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ForceBufferFilledL failed: %d at thread %S" ),err, &iThreadName ) );
                        }
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeStopListening:
            case CSensrvTransaction::ETransTypeStopConditionListening:
                {
                // Channel must always exist in transaction and be in listening state
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->State() == CSensrvChannel::EChannelStateListening, User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidChannelState));
                __ASSERT_ALWAYS(aTransaction->Channel()->DataProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));
                __ASSERT_ALWAYS(aTransaction->Channel()->Buffer(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullBuffer));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());
                MSsyChannelDataProvider* provider = aTransaction->Channel()->DataProvider();

                iMutex->Signal();
                TRAP(err, provider->StopChannelDataL(channelId));
                iMutex->Wait();

                if ( err != KErrNone )
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Stopping listening failed: %d at thread %S" ),err, &iThreadName ) );
                    // Failure to stop channel data means listening will be left active to SSY,
                    // although client will see it as not listening.
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeMediatorForceBufferFilled:
                {
                // Channel with data provider must exists in transaction
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->DataProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());
                MSsyChannelDataProvider* provider = aTransaction->Channel()->DataProvider();

                iMutex->Signal();
                TRAP(err, provider->ForceBufferFilledL(channelId));
                iMutex->Wait();

                if ( err != KErrNone )
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Forcing buffer filled failed: %d at thread %S" ),err, &iThreadName ) );
                    // Failure to force buffer fill means there will not be coming corresponding BufferFilled either,
                    // so we need to fake it.
                    if (iQueue->First() == aTransaction)
                        {
                        CSensrvTransaction* dataAvailableTransaction = aTransaction->Channel()->DataAvailableTransaction();
                        if (dataAvailableTransaction)
                            {
                            err = iProxy->ServerMediator()->Notify(dataAvailableTransaction);

                            if ( err != KErrNone )
                                {
                                // Just trace, HandleNotifyFailure will take care of failure handling.
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Server mediator notify failed: %d" ), err ) );
                                }
                            }
                        }
                    }

                // No need to notify server, SSY will call BufferFilledL to continue
                notifyServer = EFalse;
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeGetProperty:
                {
                // Channel with property provider must exists in transaction
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->PropertyProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());
                TSensrvProperty property;
                TPropertyPckg   propertyPckg( property );
                MSsyPropertyProvider* provider = aTransaction->Channel()->PropertyProvider();

                err = aTransaction->Message()->Read( KSensrvGetPropertyPropertySlot, propertyPckg );

                if( err == KErrNone )
                    {
                    iMutex->Signal();
                    TRAP( err, provider->GetPropertyL( channelId, property ) );
                    iMutex->Wait();

                    if( err == KErrNone )
                        {
                        // If transaction has been deleted while executing SSY method, client is no longer interested.
                        if (iQueue->First() == aTransaction)
                            {
                            err = aTransaction->Message()->Write( KSensrvGetPropertyPropertySlot, propertyPckg );

                            if ( err != KErrNone )
                                {
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ETransTypeGetProperty, writing to message: %d" ), err ) );
                                }
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ETransTypeGetProperty, SSY call: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ETransTypeGetProperty, reading from message failed: %d" ), err ) );
                    }
                }
                break;

            ///////////////////////////////////////////
            case CSensrvTransaction::ETransTypeGetAllProperties:
                {
                // Channel with property provider must exist in transaction
                __ASSERT_ALWAYS(aTransaction->Channel(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(aTransaction->Channel()->PropertyProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));

                TSensrvChannelId channelId(aTransaction->Channel()->Id());
                RSensrvPropertyList channelPropertyList;
                MSsyPropertyProvider* provider = aTransaction->Channel()->PropertyProvider();

                iMutex->Signal();
                TRAP( err, provider->GetAllPropertiesL( channelId, channelPropertyList) );
                iMutex->Wait();

                if( err == KErrNone )
                    {
                    // If transaction has been deleted while executing SSY method, client is no longer interested.
                    if (iQueue->First() == aTransaction)
                        {
                        TInt propertyCount( channelPropertyList.Count() );
                        TSensrvTIntPckgBuf propertyCountPckgBuf( propertyCount );

                        err = aTransaction->Message()->Write( KSensrvGetAllPropertiesPropertyCountSlot, propertyCountPckgBuf );
                        if( KErrNone != err)
                            {
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Write to KSensrvGetAllPropertiesPropertyCountSlot failed: %d" ), err ) );
                            }
                        else
                            {
                            TInt reservedSpace = aTransaction->Message()->GetDesMaxLength( KSensrvGetAllPropertiesPropertyArraySlot );
                            TInt arraySize = ( sizeof(TSensrvProperty) ) * propertyCount;

                            if ( reservedSpace >= arraySize )
                                {
                                TPtr8 arrayPtr( reinterpret_cast<TUint8*>(&channelPropertyList[0]), arraySize, arraySize );
                                err = aTransaction->Message()->Write( KSensrvGetAllPropertiesPropertyArraySlot, arrayPtr );

                                if ( err != KErrNone )
                                    {
                                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Write to KSensrvGetAllPropertiesPropertyArraySlot failed: %d" ), err ) );
                                    }
                                }
                            else
                                {
                                // Not enough space to write.
                                // This is not actually an error but a feature, client will rerequest with more space.
                                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - Not enough space to write to KSensrvQueryChannelsChannelInfoArraySlot" ) ) );
                                err = KErrOverflow;
                                }
                            }
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ETransTypeGetAllProperties, SSY call: %d" ), err ) );
                    }

                channelPropertyList.Reset();
                }
                break;

            ///////////////////////////////////////////
             case CSensrvTransaction::ETransTypeSetProperty:
                {
                // Channel with property provider must exist in transaction
                CSensrvChannel* channel = aTransaction->Channel();
                __ASSERT_ALWAYS(channel, User::Panic(KSensrvPanicCategory, ESensrvPanicNullChannel));
                __ASSERT_ALWAYS(channel->PropertyProvider(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullProvider));

                TSensrvProperty property;
                TPropertyPckg propertyPckg( property );

                err = aTransaction->Message()->Read( KSensrvSetPropertyPropertySlot, propertyPckg );

                if( err == KErrNone )
                    {

					if ( property.GetPropertyId() == KSensrvPropIdAvailability )
						{
						err = KErrAccessDenied;
						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - Client not allowed to set KSensrvPropIdAvailability: %d" ), err ) );
						}

					else if ( property.GetPropertyId() == KSensrvPropIdPower )
						{
						err = KErrAccessDenied;
						COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - Client not allowed to set KSensrvPropIdPower: %d" ), err ) );
						}

					else
						{
						TSecurityInfo securityInfo;

						// Update client security info to property
						if (aTransaction->Message())
							{
							aTransaction->Message()->GetSecurityInfo(securityInfo);
							}
						else
							{
							// Internal transaction, use current process info
							securityInfo.SetToCurrentInfo();
							}
							property.SetSecurityInfo(securityInfo);

							CSensrvSession* session = aTransaction->Session();
							TSensrvChannelId channelId(channel->Id());
							MSsyPropertyProvider* provider = channel->PropertyProvider();

							// Get affected channels
							RSensrvChannelList affectedChannels;

							iMutex->Signal();
							TRAP( err, provider->CheckPropertyDependenciesL( channelId,
																			 property,
																			 affectedChannels ) );
							iMutex->Wait();

						if( err == KErrNone )
							{
							// If transaction has been deleted while executing SSY method, client is no longer interested.
							if (iQueue->First() == aTransaction)
								{
								TBool okToChange( EFalse );
								//Get the listener for the client who is setting the property
								CSensrvChannelListener* listener = channel->GetListener( session );

								if( listener )
									{
									okToChange = PolicyCheck( *channel, *listener, affectedChannels );

									if( okToChange )
										{
										// Take the property set control
										TBool takenEarlier( listener == channel->TakenPropertySetControl() );
										channel->TakePropertySetControl( listener );

										iMutex->Signal();
										TRAP( err, provider->SetPropertyL( channelId, property ) );
										iMutex->Wait();

										// Reset taken property set control if the first set has failed
										if ( err != KErrNone && !takenEarlier && listener == channel->TakenPropertySetControl() )
											{
											channel->TakePropertySetControl( NULL );
											}
										}
									else
										{
										err = KErrAccessDenied;
										}
									}
								else
									{
									//Listener doesn't exist anymore (for some reason).
									err = KErrNotFound;
									}

								if ( err != KErrNone )
									{
									ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: SetProperty: %d" ), err ) );
									}
								else
									{
									// If the channel of the setting client has one or more other clients,
									// those clients need to be explicitly notified of the change, as the
									// SSY won't be notifying this channel.

									// Get is needed to update min/max etc. fields of the property.
									iMutex->Signal();
									TRAP( err, provider->GetPropertyL( channelId, property ) );
									iMutex->Wait();

									if( err != KErrNone )
									    {
									    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Getting property after SetProperty failed: %d" ), err ) );
									    }
									else
									    {
                                        TBool propNotifyNeeded = channel->StoreChangedProperty( property, session );
    
                                        if ( propNotifyNeeded )
                                            {
                                            CSensrvTransaction* propTransaction = channel->PropertyChangedTransaction();
    
                                            // Notify server thread that a property has been changed.
                                           if( propTransaction )
                                               {
                                               TInt err = iProxy->ServerMediator()->Notify( propTransaction );
    
                                               if ( err != KErrNone )
                                                   {
                                                   // Just trace, HandleNotifyFailure will take care of failure handling.
                                                   ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Server mediator notify failed: %d" ), err ) );
                                                   }
                                               }
                                            }
									    }
									}
								}
							}
						else
							{
							ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: CheckPropertyDependenciesL: %d" ), err ) );
							}

						affectedChannels.Reset();
						}
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: ETransTypeGetProperty, reading from message failed: %d" ), err ) );
                    }

                }
                break;

            ///////////////////////////////////////////
            default:
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - ERROR: Invalid transaction type" ) ) );
                break;
            }

        // Make sure the cleanup has not removed the transaction while we were executing
        // CSsyControl method.
        if (iQueue->First() == aTransaction)
            {
            aTransaction->SetState(CSensrvTransaction::ETransStateHandledAtSsy);

            if (notifyServer && iProxy->ServerMediator())
                {
                // If there is no server mediator, then something is seriously wrong,

                // Notify server mediator that transaction is done
                // Since we are at SSY, error can be ignored.
                iProxy->ServerMediator()->Notify(aTransaction);
                }

            // Set error to transaction, if something failed
            if ( err != KErrNone )
                {
                aTransaction->SetErrorCode(err);
                }
            }
        else
            {
            // Transaction has been removed, no need to notify server mediator
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - Transaction removed, not notifying server mediator" ) ) );
            }
        }

    if (iUnloading)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - Stopping scheduler" ) ) );

	// Mutex must be closed from SSY thread, and this is the last chance while still there.
        if(iMutex->IsHeld())
            {
            iMutex->Signal();
            }

        // Unloading, so delete the SSY control and stop scheduler.
        delete iSsyControl;
        iSsyControl = NULL;
        CActiveScheduler::Stop();
        
        iMutex->Close();
        delete iMutex;
        iMutex = NULL;
        }

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    heapSize = User::Heap().Size();
    heapAvail = User::Heap().Available(biggestBlock);
    TInt newUsed(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### %S - HEAP: Size: %d, Available: %d, Used: %d, Change in used: %d" ), &iThreadName, heapSize, heapAvail, newUsed, newUsed - used ) );
#endif


    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleTransaction - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Handles notification failure
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::HandleNotifyFailure()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleNotifyFailure()" ) ) );

    // Nothing special to do, cleanup done by caller of Notify.

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::HandleNotifyFailure - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::PolicyCheck
// ---------------------------------------------------------------------------
//
TBool CSensrvSsyMediator::PolicyCheck( CSensrvChannel& aChannel,
                                       CSensrvChannelListener& aListener,
                                       const RSensrvChannelList& aAffectedChannels )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PolicyCheck()" ) ) );

    TBool ret( ETrue );
    TInt ownPriority( aListener.Priority() );

    // Check if the control has already been taken by other listener on current channel
    CSensrvChannelListener* controlListener = aChannel.TakenPropertySetControl();
    if ( controlListener && controlListener != &aListener && controlListener->Priority() >= ownPriority )
        {
        // Not allowed because the control has already been taken
        ret = EFalse;
        }
    if ( ret )
        {
        TInt count( aAffectedChannels.Count() );

        // Go through all affected channels
        for( TInt i( 0 ); i < count; i++ )
            {
            // Get channel object
            CSensrvChannel* channel = iProxy->GetChannelForId( aAffectedChannels[i] );
            if( channel )
                {
                // Check if own priority is high enough
                TInt highestPriority( 0 );
                TInt multiples( channel->GetHighestListenerPriority( highestPriority ) );
                if ( multiples > 0 && ownPriority < highestPriority )
                    {
                    // Own priority is too low
                    ret = EFalse;
                    break;
                    }
                }
             else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PolicyCheck - Unidentified channel" ) ) );
                }
            }
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::PolicyCheck - return %d" ), ret ) );

    return ret;
    }

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::AvailabilityAffectedChannels()
// ---------------------------------------------------------------------------
void CSensrvSsyMediator::AvailabilityAffectedChannels(
			TSensrvChannelId aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAvailabilityAffectedChannels, TSensrvAvailability aValue )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::AvailabilityAffectedChannels()" ) ) );

	GetAvailabilityAffectedChannels( aChannelId, aSensorPropertyProvider, aAvailabilityAffectedChannels, aValue );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::AvailabilityAffectedChannels() - return" ) ) );
	}

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::AffectedChannels()
// ---------------------------------------------------------------------------
void CSensrvSsyMediator::AffectedChannels(
			TSensrvChannelId aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAffectedChannels )
	{
	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::AffectedChannels()" ) ) );

	GetAffectedChannels( aChannelId, aSensorPropertyProvider, aAffectedChannels );

	COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::AffectedChannels() - return" ) ) );
	}

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::GetAvailabilityAffectedChannels()
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::GetAvailabilityAffectedChannels(
		const TSensrvChannelId& aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAvailabilityAffectedChannels, const TSensrvAvailability& aValue )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAvailabilityAffectedChannels()" ) ) );

	if ( aAvailabilityAffectedChannels )
		{
        aAvailabilityAffectedChannels->Reset();

        if ( aSensorPropertyProvider )
            {
            // Get all the affected channels for required property.
            // The affected channels are needed for the property set indicator.
            TSensrvProperty property( KSensrvPropIdAvailability, KSensrvItemIndexNone, aValue );

			TRAPD( err, aSensorPropertyProvider->CheckPropertyDependenciesL(
				aChannelId, property, *aAvailabilityAffectedChannels ) );
			if ( err != KErrNone )
				{
				ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAvailabilityAffectedChannels - ERROR: Cannot read channel list %d" ), err ) );
				}
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAvailabilityAffectedChannels - ERROR: No property provider" ) ) );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAvailabilityAffectedChannels - ERROR: No affected channel list" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAvailabilityAffectedChannels() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvSsyMediator::GetAffectedChannels()
// ---------------------------------------------------------------------------
//
void CSensrvSsyMediator::GetAffectedChannels(
        const TSensrvChannelId& aChannelId, MSsyPropertyProvider* aSensorPropertyProvider, RSensrvChannelList* aAffectedChannels )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels()" ) ) );

    if ( aAffectedChannels )
        {
        aAffectedChannels->Reset();
        if ( aSensorPropertyProvider )
            {
            // Get all the affected channels for every property.
            // The affected channels are needed for the property set indicator.
            RSensrvPropertyList channelPropertyList;
            TRAPD( err, aSensorPropertyProvider->GetAllPropertiesL( aChannelId, channelPropertyList ) );
            if ( err == KErrNone )
                {
                TInt propertyCount( channelPropertyList.Count() );
                for ( TInt i( 0 ); i < propertyCount; i++ )
                    {
                    RSensrvChannelList affectedChannels;
                    TRAP( err, aSensorPropertyProvider->CheckPropertyDependenciesL(
                        aChannelId, channelPropertyList[ i ], affectedChannels ) );
                    if ( err == KErrNone )
                        {
                        TInt channelCount( affectedChannels.Count() );
                        for ( TInt j( 0 ); j < channelCount; j++ )
                            {
                            err = aAffectedChannels->InsertInUnsignedKeyOrder( affectedChannels[ j ] );
                            if ( err != KErrNone && err != KErrAlreadyExists )
                                {
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels - ERROR: Cannot append channel %d" ), err ) );
                                }
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels - ERROR: Cannot read channel list %d" ), err ) );
                        }
                    affectedChannels.Close();
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels - ERROR: Cannot read property list %d" ), err ) );
                }
            channelPropertyList.Close();
            }
        else
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels - ERROR: No property provider" ) ) );
            }
        }
    else
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels - ERROR: No affected channel list" ) ) );
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyMediator::GetAffectedChannels() - return" ) ) );
    }
