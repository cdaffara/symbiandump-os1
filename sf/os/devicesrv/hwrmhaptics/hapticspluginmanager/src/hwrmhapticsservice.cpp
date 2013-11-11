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
* Description:  Haptic service implementation.
*
*/


#include <s32mem.h>              // RDesReadStream
#include <hwrmhaptics.h>
#include <hwrmhapticsobserver.h>
#include <hwrmlogicalactuators.h>
#include <hwrmhapticscommands.h> // adaptation interface
#include <hwrmhapticspacketizer.h>

#include "hwrmhapticsclientserver.h" // panic codes, service ID
#include "hwrmhapticsserver.h" // default case in ExecuteMessageL
#include "hwrmhapticsservice.h"
#include "hwrmhapticspluginmanager.h"
#include "hwrmhapticsreservationhandler.h"
#include "hwrmhapticscommondata.h"
#include "hwrmhapticstrace.h"
#include "hwrmhapticspluginrequestdata.h"

_LIT( KPanicCategory, "HWRMHapticsSService" );

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHWRMHapticsService* CHWRMHapticsService::NewL(
        CHWRMHapticsPluginManager* aPluginHandler,
        CHWRMHapticsReservationHandler* aReservationHandler,
        CHWRMHapticsCommonData& aHapticsCommonData,
        const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::NewL()" ) ) );

    CHWRMHapticsService* self = 
        new ( ELeave ) CHWRMHapticsService( aHapticsCommonData, aMessage );
    
    CleanupStack::PushL( self );
    self->ConstructL( aPluginHandler, aReservationHandler );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::NewL - return 0x%x" ), self ) );
    
    return self;
    }
   
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsService::~CHWRMHapticsService()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService()" ) ) );

    // Cleanup haptics just in case regular cleanup failed
    CleanupHaptics();

    if ( iPacketizer )
        {
        delete iPacketizer;
        iPacketizer = NULL;
        }
    
    // Complete any pending requests
    while ( iTransactionList->FirstItem() )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService() - Deleting request %d" ), iTransactionList->FirstItem()->TransactionId() ) );
        CHWRMHapticsPluginRequestData* data = 
            static_cast<CHWRMHapticsPluginRequestData*>(
                iTransactionList->RemoveFirstItem() );

        if ( data->RequestMessage().Handle() )
            {
            // Check that this request is not first one of a split request
            if ( !data->CommandSplit() || 
                 !CheckForMessage(data->RequestMessage().Handle()) )
                {
                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService() - Canceling pending message" ) ) );
                data->RequestMessage().Complete( KErrCancel );
                }
            else
                {
                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService() - Split request, not canceling message yet" ) ) );
                }
            }

        TRAPD ( err, iPluginManager->CancelCommandL( data->TransactionId() ) );
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService() - Cancelling Command (transid: %d) failed: %d" ), data->TransactionId(), err ) );
            }

        delete data;
        }
        
    // Destroy transaction list
    delete iTransactionList;

    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::~CHWRMHapticsService() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Handles Haptics requests. 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CHWRMHapticsService::ExecuteMessageL( 
                                                const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "e_HWRMHAPTICS_SERVICE_EXECUTEMESSAGEL 1" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ExecuteMessageL(0x%x)" ), aMessage.Function() ) );
    __ASSERT_ALWAYS( iPluginManager,
                     User::Panic( KPanicCategory, EPanicBadHandle ) );
    __ASSERT_ALWAYS( iReservationHandler,
                     User::Panic( KPanicCategory, EPanicBadHandle ) );
    
    if ( aMessage.IsNull() )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ExecuteMessageL - NULL message!" ) ) );
        User::Leave( KErrBadHandle );
        }

    TBool completeMessage( EFalse );
    
    switch( aMessage.Function() )
        {
        case RMessage2::EDisConnect:
            {
            // send command to close actuator, if actuator has 
            // been opened (i.e. packetizer exists)
            if ( iPacketizer )
                {
                SendMsgToPluginManagerL( aMessage );
                }

            CleanupHaptics();
            break;
            }
        case EHWRMHaptics:
        case EHWRMHapticsBridgeCommand: // flow through
            {
            // send haptics message
            SendMsgToPluginManagerL( aMessage );
            break;
            }
        case EHWRMHapticsPlayEffect:
            {
            // check if suspended
            if ( iSuspended )
                {
                // no effect playing, just complete the message
                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ExecuteMessageL - Haptics suspended; Play not executed." ) )  );
                aMessage.Complete( KErrNone );
                }
            else if ( iReservationHandler->IsReserved( this ) &&
                      iReservationHandler->ReservedPriorityHigher( iSid ) )
                {
                // haptics has not been reserved for some other client with
                // higher (or equal) priority, notify caller with an error code
                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ExecuteMessageL - Haptics Reserved!" ) ) );
                User::Leave( KErrInUse );
                }
            else
                {
                // send play effect -command forwards to plugin manager
                SendMsgToPluginManagerL( aMessage );
                }
            
            break;
            }
        case EHWRMHapticsOpenActuator:
            {
            // get actuator type
            THWRMLogicalActuators actuator = 
                    static_cast<THWRMLogicalActuators>( aMessage.Int2() );
            
            // open the plugin to send the message to appropriate actuator;
            TBool created = iPluginManager->OpenPluginToActuatorL( actuator );

            // reset packetizer instance
            if ( iPacketizer )
                {
                delete iPacketizer;
                iPacketizer = NULL;
                }
            
            iPacketizer = CHWRMHapticsPacketizer::NewL( actuator );

            // if plugin was not created, it already exists. Inform client 
            // about the last status of the plugin.
            if ( !created )
                {
                iHapticsCommonData.NotifyActuatorEvent( actuator, iSession );
                }
            
            // send the open device message;
            SendMsgToPluginManagerL( aMessage );

            break;
            }
        case EHWRMHapticsCleanup:
            {
            CleanupHaptics();

            // complete as there is no ProcessResponse for this
            aMessage.Complete( KErrNone );
            break;
            }
        case EHWRMHapticsReserve:
            {
            COMPONENT_TRACE( ( _L("CHWRMHapticsService::ExecuteMessageL - EHWRMReserveHaptics") ) );
            
            ReserveHapticsL( aMessage );

            // Since using dummy messages for freeze state restores, need complete always.
            completeMessage = ETrue;
            
            break;
            }           
        case EHWRMHapticsRelease:
            {
            COMPONENT_TRACE( ( _L("CHWRMHapticsService::ExecuteMessageL - EHWRMReleaseHaptics") ) );

            ReleaseHaptics();
            
            // Since using dummy messages for default state restores, need complete always.
            completeMessage = ETrue;

            break;
            }           
        case EHWRMHapticsSuppActuators:
            {
            // get the supported logical actuator types from 
            // the plugin manager
            TUint32 types = iPluginManager->GetSupportedActuatorInfo();
            
            // write the supported types to aMessage
            TPckg<TUint32> actuatorInfoRetPckg( types );
            TInt err = aMessage.Write( 0, actuatorInfoRetPckg, 0 );
            
            // complete the message
            aMessage.Complete( err );
            break;
            }

        case EHWRMHapticsSetLicenseProp:
            {
            // The license key is set automatically if it is given as empty
            // string and if LicenseAutoSettingAllowed check succeeds.
            // The actual license key setting occures in lower layer,
            // here it is enough to create new command message.
            
            // check license key length
            if ( aMessage.Int1() == 0 )
                {
                // check if automatic license key setting is allowed
                // for the calling client
                if ( iPluginManager->LicenseAutoSettingAllowed( aMessage ) &&
                     iPacketizer )
                    {
                    // create buffer for new request data
                    HWRMHapticsCommand::RHWRMHapticsReqData reqData;
                    
                    // client is allowed to get automatic license key setting,
                    // create new command package
                    TInt err = iPacketizer->EncSetPlatformLicenseKeyReq(
                                        iPacketizer->DeviceHandle(),
                                        reqData );
                    
                    // write new request data to request message, if creating
                    // it succeeded
                    if ( err == KErrNone )
                        {
                        aMessage.Write( 0, reqData, 0 );
                        }
                    
                    reqData.Close();
                    }
                }
            
            // send message to plugin manager
            SendMsgToPluginManagerL( aMessage );
            break;
            }
        case EHWRMHapticsStatusNotification:
            {
            // store the message. It will be completed only
            // when the haptics/actuator status for this client changes
            // (thus the command should always be asynchronous on the 
            // client side)
            iHapticsCommonData.AddStatusObserver( aMessage );
            
            break;
            }
        case EHWRMHapticsGetStatus:
            {
            // get the current status value for this client
            MHWRMHapticsObserver::THWRMHapticsStatus status =
                iHapticsCommonData.CurrentStatus( aMessage.Session() );

            // write the status to message
            TPckg<MHWRMHapticsObserver::THWRMHapticsStatus> statusPckg( status );
            TInt err = aMessage.Write( 0, statusPckg, 0 );
            
            // complete message
            aMessage.Complete( err );

            break;
            }
        default:
            {
            // Cannot identify the message, panic the client
            aMessage.Panic( KPanicCategory, EPanicIllegalFunction );
            break;
            }

        }//switch

    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ExecuteMessageL - return 0x%x" ), completeMessage ) );
    COMPONENT_TRACE( ( _L( "e_HWRMHAPTICS_SERVICE_EXECUTEMESSAGEL 0" ) ) );
    
    return completeMessage;
    }
    
// ---------------------------------------------------------------------------
// Handles Haptics requests responses.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::ProcessResponseL( TInt aCommandId,  
                                                     TUint8 aTransId,
                                                     const TDesC8& aData )
    {
    COMPONENT_TRACE( ( _L( "e_HWRMHAPTICS_SERVICE_PROCESSRESPONSEL 1" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ProcessResponseL(0x%x, 0x%x, <data>)" ), aCommandId, aTransId ) );

    if( aCommandId == HWRMHapticsCommand::EHapticsCmdId )
        {
        TInt contextErr = CompleteRequestL( aTransId, aData );
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ProcessResponseL - CompleteRequestL ret = %d" ), contextErr ) );

        // Leave if there is error in context
        User::LeaveIfError( contextErr );
        }
             
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ProcessResponseL - return" ) ) );
    COMPONENT_TRACE( ( _L( "e_HWRMHAPTICS_SERVICE_PROCESSRESPONSEL 0" ) ) );
    }

// ---------------------------------------------------------------------------
// Suspends haptics.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::SuspendResource()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::SuspendSubResource()" ) ) );
    
    iSuspended = ETrue;

    // notify client that haptics for it has been suspended
    iHapticsCommonData.NotifyStatus( 
        MHWRMHapticsObserver::EHWRMHapticsStatusSuspended, iSession );
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::SuspendSubResource - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// Resumes haptics.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::ResumeResource()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ResumeSubResource()" ) ) );

    iSuspended = EFalse;

    // notify client that haptics for it is now available
    iHapticsCommonData.NotifyStatus( 
        MHWRMHapticsObserver::EHWRMHapticsStatusAvailable, iSession );
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ResumeSubResource - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Cancels outstanding request by completing the RMessage2 and removing data
// from transaction list.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::CancelRequest( TUint8 aTransId )
    {
    CHWRMHapticsPluginRequestData* data = 
        static_cast<CHWRMHapticsPluginRequestData*>(
        iTransactionList->FindTransaction( aTransId, ETrue ) );
    
    if ( data && data->RequestMessage().Handle() )
        {
        data->RequestMessage().Complete( KErrTimedOut );
        }
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsService::CHWRMHapticsService(
                CHWRMHapticsCommonData& aHapticsCommonData, 
                const RMessage2& aMessage )
    : iHapticsCommonData( aHapticsCommonData ), iSid( aMessage.SecureId() ),
      iSession( aMessage.Session() )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CHWRMHapticsService()" ) ) );
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CHWRMHapticsService - return" ) ) );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::ConstructL(
        CHWRMHapticsPluginManager* aPluginManager,
        CHWRMHapticsReservationHandler* aReservationHandler )
    {    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ConstructL(0x%x)" ), aPluginManager ) );

    if ( !aPluginManager )
        {
        User::Leave( KErrBadHandle );
        }
        
    iPluginManager = aPluginManager;
    iReservationHandler = aReservationHandler;

    iTransactionList = new( ELeave ) CHWRMHapticsPluginTransactionList();
   
    // set this session to the common data for storing the client 
    // specific status information
    iHapticsCommonData.AddSessionL( iSession );

    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ConstructL - return " ) ) );
    }

// ---------------------------------------------------------------------------
// Completes request. Subclass calls this from ProcessResponseL
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsService::CompleteRequestL( TUint8 aTransId, 
                                            const TDesC8& aData )
    {
    // return value
    TInt contextErr( KErrNone );

    // find transaction data
    CHWRMHapticsPluginRequestData* data = 
        static_cast<CHWRMHapticsPluginRequestData*>(
        iTransactionList->FindTransaction( aTransId, ETrue ) );
    
    CleanupStack::PushL( data );

    if ( data && data->RequestMessage().Handle() )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequestL - iRequestMessage.Handle() ok." ) ) );

        // data storage for the protocol version and error code
        CHWRMHapticsRespData* respData = 
            CHWRMHapticsRespData::NewLC( KErrNone, KNullDesC8 );

        // internalize data using stream reader
        RDesReadStream reader( aData );
        CleanupClosePushL( reader );
        reader >> *respData;
        CleanupStack::PopAndDestroy( &reader );
        
        // status filled in message decoding
        TInt vibeStatus = KErrNone;
        
        // decode data, if no error
        if ( respData->ErrorCode() == KErrNone )
            {
            if ( data->RequestMessage().Function() == EHWRMHapticsBridgeCommand )
                {
                // bridge command handling
                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequest - inside EHWRMHapticsBridgeCommand case:" ) ) );
                DATADUMP_TRACE( _L("CHWRMHapticsService::CompleteRequest - (EHWRMHapticsBridgeCommand case) - data dump "), respData->Data() );

                // write error code and response data
                TPckg<TInt> vibeDummyCodePckg( KErrNone );
                data->RequestMessage().Write( 1, vibeDummyCodePckg, 0 );
                data->RequestMessage().Write( 2, respData->Data(), 0 );
                }
            else
                {
                // decode the message and send it to the callback service
                CDesC8ArraySeg* decodeArray = NULL;
                TRAPD( err, decodeArray = iPacketizer->DecodeMessageL(
                                            respData->Data(), vibeStatus ) );

                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequestL - Return msg decoding, err = %d" ), err ) );

                for ( TInt i = 0; err == KErrNone && 
                      i < decodeArray->MdcaCount(); ++i )
                    {
                    // write data back to client
                    err = data->RequestMessage().Write(
                                KHapticsMessageResponseArgsOffset + i,
                                decodeArray->MdcaPoint( i ), 0 );
                    }

                COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequestL - Data writing err = %d" ), err ) );
                }
            }
        
        CleanupStack::PopAndDestroy( respData );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequestL - calling iRequestMessage.Complete()") ) ) ;
        data->RequestMessage().Complete( vibeStatus );
        }
    else
        {
        // transaction data not found
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CompleteRequest - No transaction data found!" ) ) );
        contextErr = KErrBadHandle;
        }    

    // cleanup data
    CleanupStack::PopAndDestroy( data );
    
    return contextErr;
    }

// ---------------------------------------------------------------------------
// Checks transaction list if specified message is in any transaction.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsService::CheckForMessage( TInt aHandle )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CheckForMessage - Checking for message: 0x%x" ), aHandle ) );
    
    CHWRMHapticsPluginRequestData* data = 
        static_cast<CHWRMHapticsPluginRequestData*>(
            iTransactionList->FirstItem() );
    
    TBool retval( EFalse );
    
    while ( !retval && data )
        {
        if ( data->RequestMessage().Handle() == aHandle )
            {
            retval = ETrue;
            }
        
        data = static_cast<CHWRMHapticsPluginRequestData*>( data->NextItem() );
        }
        
    return retval;
    }
        
// ---------------------------------------------------------------------------
// Cleans up haptics. 
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::CleanupHaptics()
    {
    if ( !iCleanupDone )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CleanupHaptics()" ) ) );

        // release haptics in case this client has made the reservation
        ReleaseHaptics();

        iSuspended = EFalse;
        
        // remove priority of the client from reservation handler
        iReservationHandler->RemovePriority( iSid );

        // remove this session from the common data
        iHapticsCommonData.RemoveSession( iSession );

        iCleanupDone = ETrue;
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::CleanupHaptics - return" ) ) );
        }
    }

// ---------------------------------------------------------------------------
// Handles Haptics requests. 
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::SendMsgToPluginManagerL( const RMessage2& aMessage )
    {
    // Create new data (TransId is updated later, commandId is not important)
    CHWRMHapticsPluginRequestData* data( NULL );
    if ( aMessage.Function() == RMessage2::EDisConnect )
        {
        data = new ( ELeave ) 
            CHWRMHapticsPluginRequestData( RMessage2(), 0, 0, EFalse );
        }
    else 
        {
        data = new ( ELeave ) 
            CHWRMHapticsPluginRequestData( aMessage, 0, 0, EFalse );
        }
    
    CleanupStack::PushL( data );

    if ( aMessage.Function() == RMessage2::EDisConnect )
        {
        // form from Disconnect message actuator closing request
        RBuf8 closeBuf;
        CleanupClosePushL( closeBuf );
        
        User::LeaveIfError( iPacketizer->EncCloseDeviceReq( 
                            iPacketizer->DeviceHandle(), closeBuf ) );
        
        HBufC8* reqData = closeBuf.AllocL();
        
        CleanupStack::PopAndDestroy( &closeBuf );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::SendMsgToPluginManagerL - Disconnect actuator") ) );
        
        // created buffer is deleted in the request data object
        data->SetRequestData( reqData );

        // command to plugin manager
        TUint8 transId = iPluginManager->ProcessCommandL( 
                                            HWRMHapticsCommand::EHapticsCmdId, 
                                            *reqData, this );
        data->SetTransactionId( transId );
        }
    else        
        {
        // read message data into a heap buffer
        HBufC8* reqData = HBufC8::NewL( aMessage.GetDesLength( 0 ) );
        TPtr8 dataPtr = reqData->Des();
        TInt err = aMessage.Read( 0, dataPtr, 0 );
        
        COMPONENT_TRACE( ( _L( "CHWRMHapticsService::SendMsgToPluginManagerL - aMessageRead err = %d"), err ) );
        
        // created buffer is deleted in the request data object
        data->SetRequestData( reqData );

        // command to plugin manager
        TUint8 transId = iPluginManager->ProcessCommandL( 
                                            HWRMHapticsCommand::EHapticsCmdId, 
                                            *reqData, this);
        data->SetTransactionId( transId );
        }
    
    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );
    
    // Add data to list
    iTransactionList->AddTransaction( data );
    }

// ---------------------------------------------------------------------------
// Reserves haptics.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::ReserveHapticsL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ReserveHapticsL()" ) ) );
    
    // Reserve the haptics
    TBool noCoeEnv = aMessage.Int0();
    TBool suspended = iReservationHandler->ReserveL( iSid, noCoeEnv, this );
    
    // if reservation became the affective reservation, inform all other 
    // clients that haptics is reserved
    if ( !suspended )
        {
        iHapticsCommonData.BroadcastStatus( 
            MHWRMHapticsObserver::EHWRMHapticsStatusReserved, iSession );
        }
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ReserveHapticsL() return" ) ) );
    }

// ---------------------------------------------------------------------------
// Releases haptics.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsService::ReleaseHaptics()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ReleaseHaptics()" ) ) );

    // get info is haptics currently reserved for this client
    TBool activeReservation = iReservationHandler->ActiveReservation( this );

    // release reservation (removes, if this has a reservation)
    TBool reserved = iReservationHandler->Release( this );

    // if haptics is still reserved for another client, notify client
    if ( reserved )
        {
        iHapticsCommonData.NotifyStatus( 
            MHWRMHapticsObserver::EHWRMHapticsStatusReserved, iSession );
        }
    else if ( activeReservation )
        {
        // haptics was reserved for this client, but there are no more 
        // reservations --> inform all clients, which have been blocked
        iHapticsCommonData.BroadcastStatus( 
            MHWRMHapticsObserver::EHWRMHapticsStatusAvailable, iSession );
        }
           
    COMPONENT_TRACE( ( _L( "CHWRMHapticsService::ReleaseHaptics() return" ) ) );
    }


//  End of File  
