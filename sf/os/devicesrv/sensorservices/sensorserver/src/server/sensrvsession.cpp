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
* Description:  Sensor server session
*
*/


#include "sensrvdefines.h"
#include "sensrvsession.h"
#include "sensrvserver.h"
#include "sensrvproxymanager.h"
#include "sensrvclientserver.h"
#include "sensrvtrace.h"
#include "sensrvmessage.h"
#include "sensrvchannelchangequeue.h"


// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CSensrvSession::CSensrvSession( CSensrvServer& aServer, 
                                CSensrvProxyManager& aProxyManager,
                                TSecureId aSecureId )
    : iServer(aServer),
      iProxyManager(aProxyManager),
      iSecureId(aSecureId)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::CSensrvSession()" ) ) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::CSensrvSession - return" )) );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvSession* CSensrvSession::NewL( CSensrvServer& aServer, 
                                      CSensrvProxyManager& aProxyManager,
                                      TSecureId aSecureId )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::NewL()" ) ) );

    CSensrvSession* self = new( ELeave ) CSensrvSession(aServer, aProxyManager, aSecureId);
    
    self->ConstructL();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// SensrvSession::ConstructL()
// -----------------------------------------------------------------------------
//
void CSensrvSession::ConstructL()
    {
    iProxyManager.AddSession();
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CSensrvSession::~CSensrvSession()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::~CSensrvSession()" ) ) );

    if (iAsyncChannelChangeMessage)
        {
        iProxyManager.RemoveChannelChangeListenerFromDynamicChannelSsys(
            iChannelChangeSearchParams, iAsyncChannelChangeMessage->GetMessage());
        }

    // Kill all ongoing transactions and channel listeners initiated by this session
    // Note: This is not actually necessary for sessions that are used by channel finder,
    // but doing special case for that is not cost effective.
    iProxyManager.SessionTerminated(this);
    
    // iServer and iProxyManager are not owned and therefore not cleaned

    delete iSyncMessage;
    delete iAsyncDataMessage;
    delete iAsyncPropertyMessage;
    delete iAsyncConditionMessage;
    delete iAsyncChannelChangeMessage;
    
    delete iChannelChangeQueue;

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    TInt heapSize = User::Heap().Size();
    TInt biggestBlock(0);
    TInt heapAvail = User::Heap().Available(biggestBlock);
    TInt used(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### Sensor Server, ~CSensrvSession - HEAP: Size: %d, Available: %d, Used: %d" ), heapSize, heapAvail, used ) );
#endif

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::~CSensrvSession - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Calls request handling functions. Also traps any leaves and signals client if
// error occurs.
// -----------------------------------------------------------------------------
//
void CSensrvSession::ServiceL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL(0x%x)" ), aMessage.Handle() ) );

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    TInt heapSize = User::Heap().Size();
    TInt biggestBlock(0);
    TInt heapAvail = User::Heap().Available(biggestBlock);
    TInt used(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### Sensor Server, ServiceL - HEAP: Size: %d, Available: %d, Used: %d" ), heapSize, heapAvail, used ) );
#endif

#ifdef API_TRACE_DEBUG
    switch ( aMessage.Function() )
        {
        case ESensrvSrvReqQueryChannels:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqQueryChannels ####" )) );
            break;
        case ESensrvSrvReqOpenChannel:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqOpenChannel ####" )) );
            break;
        case ESensrvSrvReqCloseChannel:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqCloseChannel ####" )) );
            break;
        case ESensrvSrvReqStartListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStartListening ####" )) );
            break;
        case ESensrvSrvReqStopListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStopListening ####" )) );
            break;
        case ESensrvSrvReqAsyncChannelData:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqAsyncChannelData ####" )) );
            break;
        case ESensrvSrvReqShutdownServer:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqShutdownServer ####" )) );
            break;
        case ESensrvSrvReqGetProperty:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqGetProperty ####" )) );
            break;
        case ESensrvSrvReqAsyncPropertyData:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqAsyncPropertyData ####" )) );
            break;
        case ESensrvSrvReqStopPropertyListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStopPropertyListening ####" )) );
            break;
        case ESensrvSrvReqSetProperty:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqSetProperty ####" )) );
            break;
        case ESensrvSrvReqGetAllProperties:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqGetAllProperties ####" )) );
            break;
        case ESensrvSrvReqAddConditionSet:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqAddConditionSet ####" )) );
            break;
        case ESensrvSrvReqRemoveConditionSet:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqRemoveConditionSet ####" )) );
            break;
        case ESensrvSrvReqStartConditionListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStartConditionListening ####" )) );
            break;
        case ESensrvSrvReqStopConditionListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStopConditionListening ####" )) );
            break;
        case ESensrvSrvReqAsyncConditionMet:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqAsyncConditionMet ####" )) );
            break;
        case ESensrvSrvReqAsyncChannelChangeNotification:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqAsyncChannelChangeNotification ####" )) );
            break;
        case ESensrvSrvReqStopChannelChangeListening:
            API_TRACE( ( _L( "#### Sensor Server - Message from client #### ESensrvSrvReqStopChannelChangeListening ####" )) );
            break;
        default:
            break;
        }
#endif

    TInt err(KErrNone);
    CSensrvMessage* wrappedMsg = NULL;
    
    switch ( aMessage.Function() )
        {
        case ESensrvSrvReqQueryChannels:
        case ESensrvSrvReqOpenChannel:
        case ESensrvSrvReqCloseChannel:
        case ESensrvSrvReqStartListening:
        case ESensrvSrvReqStopListening:
        case ESensrvSrvReqGetProperty:
        case ESensrvSrvReqStopPropertyListening:
        case ESensrvSrvReqSetProperty:
        case ESensrvSrvReqGetAllProperties:
        case ESensrvSrvReqAddConditionSet:
        case ESensrvSrvReqRemoveConditionSet:
        case ESensrvSrvReqStartConditionListening:
        case ESensrvSrvReqStopConditionListening:
            {
            if (!iSyncMessage)
                {
                TRAP( err, iSyncMessage = CSensrvMessage::NewL() );
                }
            
            wrappedMsg = iSyncMessage;
            }
            break;
        
        case ESensrvSrvReqAsyncChannelData:
            {
            if (!iAsyncDataMessage)
                {
                TRAP( err, iAsyncDataMessage = CSensrvMessage::NewL() );
                }
            
            wrappedMsg = iAsyncDataMessage;
            }
            break;
        
        case ESensrvSrvReqAsyncPropertyData:
            {
            if (!iAsyncPropertyMessage)
                {
                TRAP( err, iAsyncPropertyMessage = CSensrvMessage::NewL() );
                }
            
            wrappedMsg = iAsyncPropertyMessage;
            }
            break;
        
        case ESensrvSrvReqAsyncConditionMet:
            {
            if (!iAsyncConditionMessage)
                {
                TRAP( err, iAsyncConditionMessage = CSensrvMessage::NewL() );
                }
            
            wrappedMsg = iAsyncConditionMessage;
            }
            break;
            
        case ESensrvSrvReqAsyncChannelChangeNotification:
            {
            // Create queue if it doesn't yet exist
            if (!iChannelChangeQueue)
                {
                TRAP(err, iChannelChangeQueue = CSensrvChannelChangeQueue::NewL());
                }

            if (err == KErrNone)
                {
                // Create async change notification message if it doesn't exist yet
                if (!iAsyncChannelChangeMessage)
                    {
                    TRAP( err, iAsyncChannelChangeMessage = CSensrvMessage::NewL() );
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - ERROR: Unable to create channel change notification message wrapper: %d" ), err ) );
                        }
                    }

                // Initialize wrapper and check if there are notifications waiting
                if (err == KErrNone)
                    {
                    iAsyncChannelChangeMessage->Initialize(aMessage);
                    
                    TSensrvChannelInfoPckgBuf searchParamsPckgBuf;
                    err = iAsyncChannelChangeMessage->Read( KSensrvAsyncChannelSearchParamsSlot, searchParamsPckgBuf );
                    if ( err == KErrNone )
                        {
                        iChannelChangeSearchParams = searchParamsPckgBuf();

                        iProxyManager.LoadDynamicChannelSsysIfNeeded(
                            iChannelChangeSearchParams, iAsyncChannelChangeMessage->GetMessage(), ETrue); // Ignore return value

                        CSensrvChannelChangeQueue::TChannelChangeNotification*
                            changeNotification = iChannelChangeQueue->First();
                        
                        if (changeNotification)
                            {
                            CompleteChangeNotificationMessage(changeNotification->iChannelInfo,
                                                              changeNotification->iChangeType);
                            iChannelChangeQueue->RemoveFirst();
                            }
                        }
                    else
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - ERROR: Unable to read channel change search params: %d" ), err ) );
                        }
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - ERROR: Unable to create channel change notification queue: %d" ), err ) );
                }

            if ( err != KErrNone )
                {
                aMessage.Complete( KErrGeneral );
                }
            }
            break;

        case ESensrvSrvReqStopChannelChangeListening:
            {
            if (iAsyncChannelChangeMessage)
                {
                iProxyManager.RemoveChannelChangeListenerFromDynamicChannelSsys(
                    iChannelChangeSearchParams, iAsyncChannelChangeMessage->GetMessage());
                }

            // Complete async message before deletion.
            if (iAsyncChannelChangeMessage && iAsyncChannelChangeMessage->Handle())
                {
                iAsyncChannelChangeMessage->Complete(KErrCancel);
                }
                
            // Delete channel notification queue and asynchronous notification message 
            delete iChannelChangeQueue;
            iChannelChangeQueue = NULL;
            delete iAsyncChannelChangeMessage;
            iAsyncChannelChangeMessage = NULL;
            
            // Complete stop message, since it won't be dispatched further.
            aMessage.Complete(KErrNone);
            }
            break;
            
        case ESensrvSrvReqShutdownServer:
            {
            // Test framework needs to be able to shutdown the server (checked in sensor server policy)
            API_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - Attempting shutdown from 0x%x" ), aMessage.SecureId().iId ) );
            iProxyManager.ShutdownServer(aMessage);
            }
            break;
        
        default:
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - ERROR: Invalid function, panicing client" ) ) );
            aMessage.Panic(KSensrvPanicCategory, ESensrvPanicInvalidFunction);
            }
            break;
        }
    
    if (   aMessage.Function() == ESensrvSrvReqShutdownServer
        || aMessage.Function() == ESensrvSrvReqStopChannelChangeListening
        || aMessage.Function() == ESensrvSrvReqAsyncChannelChangeNotification)    
        {
        // Do nothing
        }
    else if ( wrappedMsg )
        {
        wrappedMsg->Initialize(aMessage);
        iProxyManager.DispatchMessage(*wrappedMsg);
        }
    else
        {
        aMessage.Complete(KErrNoMemory);
        }

#ifdef MEMORY_TRACE_DEBUG
    // TRACE heap usage
    heapSize = User::Heap().Size();
    heapAvail = User::Heap().Available(biggestBlock);
    TInt newUsed(heapSize-heapAvail);
    MEMORY_TRACE( ( _L( "#### Sensor Server, ServiceL - HEAP: Size: %d, Available: %d, Used: %d, Change in used: %d" ), heapSize, heapAvail, newUsed, newUsed - used ) );
#endif

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::ServiceL - return" )) );
    }
    
// -----------------------------------------------------------------------------
// CSensrvSession::SecureId
// -----------------------------------------------------------------------------
//
TSecureId CSensrvSession::SecureId()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::SecureId()" ) ) );
            
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::SecureId - return 0x%x" ), iSecureId.iId ) );
    
    return iSecureId;
    }

// ---------------------------------------------------------------------------
// Adds channel change to queue if client is interested in such changes,
// unless async message is currently present, in which case will directly
// complete it without queueuing.
// ---------------------------------------------------------------------------
//
void CSensrvSession::NotifyChannelChange(const TSensrvResourceChannelInfo& aChangedChannel,
                                         TSensrvChannelChangeType aChangeType )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::NotifyChannelChange(aChangedChannel.iChannelId: %d, aChangeType: %d)"), aChangedChannel.iChannelId, aChangeType ) );

    // Client is interested if notification queue exists
    if (iChannelChangeQueue)    
        {
        // If async message is present, handle notification
        if (iAsyncChannelChangeMessage && iAsyncChannelChangeMessage->Handle())
            {
            CompleteChangeNotificationMessage(aChangedChannel, aChangeType);
            }
        else
            {
            // No async message currently, so queue notification
            iChannelChangeQueue->Append(aChangedChannel, aChangeType);
            }
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::NotifyChannelChange - return" )) );
    }

// ---------------------------------------------------------
// Completes asynchronous change notification message
// ---------------------------------------------------------
//
void CSensrvSession::CompleteChangeNotificationMessage(const TSensrvResourceChannelInfo& aChangedChannel,
                                                       TSensrvChannelChangeType aChangeType )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage(aChangedChannel.iChannelId: %d, aChangeType: %d)" ), aChangedChannel.iChannelId, aChangeType ) );
    
    __ASSERT_DEBUG(iAsyncChannelChangeMessage, User::Panic(KSensrvPanicCategory, ESensrvPanicNullMessage));
    __ASSERT_DEBUG(iAsyncChannelChangeMessage->Handle(), User::Panic(KSensrvPanicCategory, ESensrvPanicNullMessage));
    
    // Check first if this channel change is interesting and allowed
    if ( aChangedChannel.IsMatch( iChannelChangeSearchParams, iAsyncChannelChangeMessage->GetMessage() ) )
        {
        // Change is interesting and allowed
        TInt err( KErrNone );
        TSensrvChannelInfoPckgBuf infoPckg( aChangedChannel );
        TInt space = iAsyncChannelChangeMessage->GetDesMaxLength( KSensrvAsyncChannelChangeInfoSlot );
        if ( space >= infoPckg.Length() )
            {
            err = iAsyncChannelChangeMessage->Write( KSensrvAsyncChannelChangeInfoSlot, infoPckg );
            if (err == KErrNone)
                {
                TSensrvTSensrvChannelChangeTypePckgBuf changeTypePckg( aChangeType );
    
                space = iAsyncChannelChangeMessage->GetDesMaxLength( KSensrvAsyncChannelChangeTypeSlot );
                
                if ( space >= changeTypePckg.Length() )
                    {
                    err = iAsyncChannelChangeMessage->Write( KSensrvAsyncChannelChangeTypeSlot, changeTypePckg );
    
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - ERROR: Write to KSensrvAsyncChannelChangeTypeSlot failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - ERROR: Not enough space to write to KSensrvAsyncChannelChangeTypeSlot" ) ) );
                    err = KErrOverflow;
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - ERROR: Write to KSensrvAsyncChannelChangeInfoSlot failed: %d" ), err ) );
                }
            }
        else  
            {
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - ERROR: Not enough space to write to KSensrvAsyncChannelChangeInfoSlot" ) ) );
            err = KErrOverflow;
            }
        
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - Completing: %d" ), err ) );
        
        // Complete message even if there is error. 
        iAsyncChannelChangeMessage->Complete(err);
        }
    else
        {
        // Change was not interesting or allowed
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSession::CompleteChangeNotificationMessage - Skipped" ) ) );
        }
    }
