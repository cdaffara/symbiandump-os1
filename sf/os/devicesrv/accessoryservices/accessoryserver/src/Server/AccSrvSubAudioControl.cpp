/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Audio Control
*
*/



// INCLUDE FILES
#include "AccSrvSubAudioControl.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CAccSrvSubAudioControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubAudioControl::CAccSrvSubAudioControl(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel,
    TUint aCallerThreadID )
    : CAccSrvSubControl ( aProxy, aConnectionController, aServerModel ),
      iCallerThreadID( aCallerThreadID )
    {
    }
// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::ConstructL()" );

    CAccSrvSubControl::ConstructL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::ConstructL() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CAccSrvSubAudioControl* CAccSrvSubAudioControl::NewL(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel,
    TUint aCallerThreadID )
    {
    if ( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubAudioControl* self = new (ELeave) CAccSrvSubAudioControl( aProxy,
                                                                        aConnectionController,
                                                                        aServerModel,
                                                                        aCallerThreadID );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAccSrvSubAudioControl::~CAccSrvSubAudioControl()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::~CAccSrvSubAudioControl()" );

    RMessage2 dummy;

    CAccSrvSubAudioControl::CancelAccessoryAudioLinkOpen( dummy );//For iAccessoryAudioLinkOpenMsg
    CAccSrvSubAudioControl::CancelAccessoryAudioLinkClose( dummy );//For iAccessoryAudioLinkCloseMsg
    CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkOpened( dummy );//For iNotifyAccessoryAudioLinkOpenedMsg
    CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkClosed( dummy );//For iNotifyAccessoryAudioLinkClosedMsg
    
    if ( iServerModel )
        {       
        iServerModel->SetAudioLinkOpenCancelled( EFalse );
        iServerModel->SetAudioLinkCloseCancelled( EFalse );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::~CAccSrvSubAudioControl - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::NotifyAccessoryAudioLinkOpenedL
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::NotifyAccessoryAudioLinkOpenedL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::NotifyAccessoryAudioLinkOpenedL()" );

    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryAudioLinkOpened,
                                                                       this );

    iNotifyAccessoryAudioLinkOpenedMsg = aMessage;//Store RMessage2 for future completion

    // If message is null, there is no pending request and no filtering needed
    if ( !iAccessoryAudioLinkOpenMsg.IsNull() )
        {
        //NotifyAccessoryAudioLinkOpenedL() notification is filtered
        //for AccessoryAudioLinkOpen() caller thread.
        //In this case NotifyAccessoryAudioLinkOpenedL() notification
        //is ordered after AccessoryAudioLinkOpen() is sent.
        iConnectionController->ServerModel().SetMsgThreadIDFilterL( iCallerThreadID,
                                                                   EAccessoryAudioLinkOpened );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::NotifyAccessoryAudioLinkOpenedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::NotifyAccessoryAudioLinkClosedL
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::NotifyAccessoryAudioLinkClosedL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::NotifyAccessoryAudioLinkClosedL()" );

    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryAudioLinkClosed,
                                                                       this );
    iNotifyAccessoryAudioLinkClosedMsg = aMessage;//Store RMessage2 for future completion

    // If message is null, there is no pending request and no filtering needed
    if ( !iAccessoryAudioLinkCloseMsg.IsNull() )
        {
        //NotifyAccessoryAudioLinkClosedL() notification is filtered
        //for AccessoryAudioLinkClose() caller thread.
        //In this case NotifyAccessoryAudioLinkClosedL() notification
        //is ordered after AccessoryAudioLinkClose() is sent.
        iConnectionController->ServerModel().SetMsgThreadIDFilterL( iCallerThreadID,
                                                                   EAccessoryAudioLinkClosed );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::NotifyAccessoryAudioLinkClosedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::AccessoryAudioLinkOpenL
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::AccessoryAudioLinkOpenL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::AccessoryAudioLinkOpenL()" );

    TAccPolGenericID     genericID;
    TAccSrvGenericIDPckg genericIDPckg( genericID );
    TAccAudioType        audioType( EAccMonoAudio ); // Initialization value
    TAccSrvAudioTypePckg audioTypePckg( audioType );

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, audioTypePckg );
    
    //Precondition
    iServerModel->IsConnectedL( genericID );//Leaves with KErrArgument if the accessory is not connected

    iServerModel->SetAudioType( audioType );//Notice, This works for single client (Server model can store only one value)

    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryAudioLinkOpen,
                                                                       this );
    
    TInt bluetoothAudioLinkOpenReqs( iConnectionController->NotificationQueue().ControlMessagesWaiting( EBluetoothAudioLinkOpenReq ) );
    if ( bluetoothAudioLinkOpenReqs > 0 )
        {
        //Send bluetooth audio link notification now.
        iServerModel->SetBluetoothAudioLinkOpenReqPostponed( EFalse );
        iConnectionController->NotificationQueue().CompleteControlMessageL( EBluetoothAudioLinkOpenReq,
                                                                            KErrNone,
                                                                            genericID.UniqueID() );
        }
    else
        {
        //There was not any bluetooth audio link notification waiting at the moment.
        //Bluetooth audio link will be opened immediately when bluetooth audio link notification is set.
        //See EAccSrvNotifyBluetoothAudioLinkOpenReq handling from the control.
        iServerModel->SetBluetoothAudioLinkOpenReqPostponed( ETrue );
        }
    
    iAccessoryAudioLinkOpenMsg = aMessage;//Store RMessage2 for future completion
    
    // If message is null, there is no pending request and no filtering needed
    if ( !iNotifyAccessoryAudioLinkOpenedMsg.IsNull() )
        {
        //NotifyAccessoryAudioLinkOpenedL notification() is filtered
        //for AccessoryAudioLinkOpen() caller thread
        iConnectionController->ServerModel().SetMsgThreadIDFilterL( iCallerThreadID,
                                                                   EAccessoryAudioLinkOpened );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::AccessoryAudioLinkOpenL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::AccessoryAudioLinkCloseL
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::AccessoryAudioLinkCloseL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBaseConnection::AccessoryAudioLinkCloseL()" );

    TAccPolGenericID     genericID;
    TAccSrvGenericIDPckg genericIDPckg( genericID );
    TAccAudioType        audioType( EAccMonoAudio ); // Initialization value
    TAccSrvAudioTypePckg audioTypePckg( audioType );

    aMessage.ReadL( KAccServParamPosFirst, genericIDPckg );
    aMessage.ReadL( KAccServParamPosSecond, audioTypePckg );

    //Precondition
    iServerModel->IsConnectedL( genericID );//Leaves with KErrArgument if the accessory is not connected

    iServerModel->SetAudioType( audioType );//Notice, This works for single client (Server model can store only one value)

    iConnectionController->NotificationQueue().RegisterControlMessage( 
                                                    EAccessoryAudioLinkClose,
                                                    this );

    iConnectionController->NotificationQueue().CompleteControlMessageL( 
                                                    EBluetoothAudioLinkCloseReq,
                                                    KErrNone,
                                                    genericID.UniqueID() );

    iAccessoryAudioLinkCloseMsg = aMessage;//Store RMessage2 for future completion

    // If message is null, there is no pending request and no filtering needed
    if ( !iNotifyAccessoryAudioLinkClosedMsg.IsNull() )
        {
        //NotifyAccessoryAudioLinkClosedL() notification is filtered
        //for AccessoryAudioLinkClose() caller thread
        iConnectionController->ServerModel().SetMsgThreadIDFilterL( 
                                                    iCallerThreadID,
                                                    EAccessoryAudioLinkClosed );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubBaseConnection::AccessoryAudioLinkCloseL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::AudioRoutingStatusNotifyL
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::AudioRoutingStatusNotifyL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::AudioRoutingStatusNotifyL()" );

    TAccPolGenericID genericID;
    TPckgBuf<TBool> activeAudioPckgBuf;
    TAccSrvGenericIDPckg genericIDPckg( genericID );

    aMessage.ReadL( KAccServParamPosFirst, activeAudioPckgBuf );
    aMessage.ReadL( KAccServParamPosSecond, genericIDPckg );

    iConnectionController->HandleAccessoryModeChangedL( genericID.UniqueID(), 
                                                        activeAudioPckgBuf() );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::AudioRoutingStatusNotifyL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CancelAccessoryAudioLinkOpen
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::CancelAccessoryAudioLinkOpen( 
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelAccessoryAudioLinkOpen()" );

    // If message is null, there is nothing to cancel
    if ( !iAccessoryAudioLinkOpenMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
                                                    EAccessoryAudioLinkOpen,
                                                    this );

        //AccessoryAudioLinkOpen() request is cancelled
        //-> NO filtering needed for NotifyAccessoryAudioLinkOpenedL()
        iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter( 
                                                    iCallerThreadID, 
                                                    EAccessoryAudioLinkOpened );

        Write( iAccessoryAudioLinkOpenMsg, KAccSrvAccessoryAudioLinkOpenBit );
        iAccessoryAudioLinkOpenMsg.Complete( KErrCancel );
        iServerModel->SetAudioLinkOpenCancelled( ETrue );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelAccessoryAudioLinkOpen - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CancelAccessoryAudioLinkClose
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::CancelAccessoryAudioLinkClose(
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelAccessoryAudioLinkClose()" );

    // If message is null, there is nothing to cancel
    if ( !iAccessoryAudioLinkCloseMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( EAccessoryAudioLinkClose,
                                                                         this );

        //AccessoryAudioLinkClose() request is cancelled
        //-> NO filtering needed for NotifyAccessoryAudioLinkClosedL()
        iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter(
            iCallerThreadID, EAccessoryAudioLinkClosed );

        Write( iAccessoryAudioLinkCloseMsg, KAccSrvAccessoryAudioLinkCloseBit );
        iAccessoryAudioLinkCloseMsg.Complete( KErrCancel );
        iServerModel->SetAudioLinkCloseCancelled( ETrue );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelAccessoryAudioLinkClose - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkOpened
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkOpened(
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkOpened()" );

    // If message is null, there is nothing to cancel
    if( !iNotifyAccessoryAudioLinkOpenedMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EAccessoryAudioLinkOpened, this );

        //NotifyAccessoryAudioLinkOpenedL() notification order is cancelled
        //-> NO filtering needed for that message
        iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter( 
            iCallerThreadID, EAccessoryAudioLinkOpened );

        Write( iNotifyAccessoryAudioLinkOpenedMsg, KAccSrvNotifyAccessoryAudioLinkOpenedBit );
        iNotifyAccessoryAudioLinkOpenedMsg.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkOpened - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkClosed
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkClosed( 
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkClosed()" );

    // If message is null, there is nothing to cancel
    if( !iNotifyAccessoryAudioLinkClosedMsg.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EAccessoryAudioLinkClosed, this );

        //NotifyAccessoryAudioLinkClosedL() notification order is cancelled
        //-> NO filtering needed for that message
        iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter(
            iCallerThreadID, EAccessoryAudioLinkClosed );
    
        Write( iNotifyAccessoryAudioLinkClosedMsg, KAccSrvNotifyAccessoryAudioLinkClosedBit );
        iNotifyAccessoryAudioLinkClosedMsg.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::CancelNotifyAccessoryAudioLinkClosed - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::DispatchMessageL
// Handles client request. If request is not handled here, it is passed to 
// parent class for handling
// -----------------------------------------------------------------------------
//
void CAccSrvSubAudioControl::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvNotifyAccessoryAudioLinkOpened:
            {
            NotifyAccessoryAudioLinkOpenedL( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryAudioLinkClosed:
            {
            NotifyAccessoryAudioLinkClosedL( aMessage );
            break;
            }
        case EAccSrvAccessoryAudioLinkOpen:
            {
            AccessoryAudioLinkOpenL( aMessage );
            break;
            }
        case EAccSrvAccessoryAudioLinkClose:
            {
            AccessoryAudioLinkCloseL( aMessage );
            break;
            }
        case EAccSrvAudioRoutingStatusNotify:
            {
            AudioRoutingStatusNotifyL( aMessage );
            break;
            }
        case EAccSrvCancelAccessoryAudioLinkOpen:
            {
            CancelAccessoryAudioLinkOpen( aMessage );
            break;
            }
        case EAccSrvCancelAccessoryAudioLinkClose:
            {
            CancelAccessoryAudioLinkClose( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryAudioLinkOpened:
            {
            CancelNotifyAccessoryAudioLinkOpened( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryAudioLinkClosed:
            {
            CancelNotifyAccessoryAudioLinkClosed( aMessage );
            break;
            }
        default:
            {
            // Not handled here, pass to base class
            CAccSrvSubControl::DispatchMessageL( aMessage );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubAudioControl::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubAudioControl::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubAudioControl::CompleteMessageL( TMsgID aMsgID,
                                                TInt aErrorCode,
                                                TInt aUniqueID )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvSubAudioControl::CompleteMessageL(MsgID=%d Err=%d UniqueID=%d)", aMsgID, aErrorCode, aUniqueID );

    TBool messageConsumption( EFalse );// Return value tells should this subsession be removed from the queue

    switch ( aMsgID )
        {
        case EAccessoryAudioLinkOpen:
            {
            if( KErrNone != aErrorCode )
                {
                //Release NotifyAccessoryAudioLinkOpenedL() filtering in error case.
                //BluetoothAudioLinkOpenedNotify() message is never sent in error case
                iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter(
                                                                    iCallerThreadID,
                                                                    EAccessoryAudioLinkOpened );
                }  
            //Complete outstanding AccessoryAudioLinkOpen request
            Write( iAccessoryAudioLinkOpenMsg, KAccSrvAccessoryAudioLinkOpenBit );
            iAccessoryAudioLinkOpenMsg.Complete( aErrorCode );
            messageConsumption = ETrue;
            }
            break;

        case EAccessoryAudioLinkClose:
            {
            if( KErrNone != aErrorCode )
                {
                //Release NotifyAccessoryAudioLinkClosedL() filtering in error case.
                //BluetoothAudioLinkClosedNotify() message is never sent in error case
                iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter(
                                                                    iCallerThreadID,
                                                                    EAccessoryAudioLinkClosed );
                }                
            //Complete outstanding AccessoryAudioLinkClose request
            Write( iAccessoryAudioLinkCloseMsg, KAccSrvAccessoryAudioLinkCloseBit );
            iAccessoryAudioLinkCloseMsg.Complete( aErrorCode );
            messageConsumption = ETrue;
            }
            break;

        case EAccessoryAudioLinkOpened:
            {
            // Nothing is done here if filter is set, i.e. NotifyAccessoryAudioLinkOpenedL()
            // notification was ordered from the same thread that AccessoryAudioLinkOpen() was called
            if ( !iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter( 
                                                                    iCallerThreadID, 
                                                                    EAccessoryAudioLinkOpened ) )
                {
                //Complete outstanding NotifyAccessoryAudioLinkOpenedL request
                //Notice, This works for single client (Server model can store only one value)
                TAccAudioType    audioType( iConnectionController->ServerModel().AudioType() );
                TAccPolGenericID genericID;
                iConnectionController->ServerModel().FindWithUniqueIDL( aUniqueID, genericID );
                TAccSrvGenericIDPckg genericIDPckg( genericID );
                TAccSrvAudioTypePckg audioTypePckg( audioType );
                
                COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAudioControl::CompleteMessageL - Opened AudioType=%d)", audioType );

                iNotifyAccessoryAudioLinkOpenedMsg.WriteL( KAccServParamPosFirst, genericIDPckg );
                iNotifyAccessoryAudioLinkOpenedMsg.WriteL( KAccServParamPosSecond, audioTypePckg );
                Write( iNotifyAccessoryAudioLinkOpenedMsg, 
                       KAccSrvNotifyAccessoryAudioLinkOpenedBit );

                iNotifyAccessoryAudioLinkOpenedMsg.Complete( aErrorCode );
                messageConsumption = ETrue;
                }
            }
            break;

        case EAccessoryAudioLinkClosed:
            {
            // Nothing is done here if filter is set, i.e. NotifyAccessoryAudioLinkClosedL() 
            // notification was ordered from the same thread that AccessoryAudioLinkClose() was called
            if ( !iConnectionController->ServerModel().FindAndDeleteMsgThreadIDFilter(
                                                                    iCallerThreadID,
                                                                    EAccessoryAudioLinkClosed ) )
                {
                //Complete outstanding NotifyAccessoryAudioLinkClosedL request
                //Notice, This works for single client (Server model can store only one value)
                TAccAudioType    audioType( iConnectionController->ServerModel().AudioType() );
                TAccPolGenericID genericID;
                iConnectionController->ServerModel().FindWithUniqueIDL( aUniqueID, genericID );
                TAccSrvGenericIDPckg genericIDPckg( genericID );
                TAccSrvAudioTypePckg audioTypePckg( audioType );

                COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAudioControl::CompleteMessageL - Closed AudioType=%d)", audioType );

                iNotifyAccessoryAudioLinkClosedMsg.WriteL( KAccServParamPosFirst, genericIDPckg );
                iNotifyAccessoryAudioLinkClosedMsg.WriteL( KAccServParamPosSecond, audioTypePckg );
                Write( iNotifyAccessoryAudioLinkClosedMsg, 
                       KAccSrvNotifyAccessoryAudioLinkClosedBit );

                iNotifyAccessoryAudioLinkClosedMsg.Complete( aErrorCode );
                messageConsumption = ETrue;
                }
            }
            break;

        default:
            {
            //Pass handling to parent class
            messageConsumption = CAccSrvSubControl::CompleteMessageL( aMsgID,
                                                                      aErrorCode,
                                                                      aUniqueID );
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubAudioControl::CompleteMessageL - return 0x%x", messageConsumption );

    return messageConsumption;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
