/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Single Connection
*
*/



// INCLUDE FILES
#include "AccSrvSubSingleConnection.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include "AccSrvServerModel.h"
#include "AccPolGenericIDAccessor.h"
#include <AccessoryTypes.h>

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
// CAccSrvSubSingleConnection::CAccSrvSubSingleConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubSingleConnection::CAccSrvSubSingleConnection(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubConnectionBase( aProxy, aConnectionController, aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CAccSrvSubSingleConnection()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CAccSrvSubSingleConnection - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::ConstructL" );

    CAccSrvSubConnectionBase::ConstructL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubSingleConnection* CAccSrvSubSingleConnection::NewL(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    {
    if( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubSingleConnection* self = new (ELeave) CAccSrvSubSingleConnection( 
        aProxy, aConnectionController, aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAccSrvSubSingleConnection::~CAccSrvSubSingleConnection()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::~CAccSrvSubSingleConnection()" );
    
    RMessage2 dummy;
    
    CAccSrvSubSingleConnection::CancelNotifyNewAccessoryConnected( dummy );//For iNotifyNewAccessoryConnected        
    CAccSrvSubSingleConnection::CancelNotifyAccessoryDisconnected( dummy );//For iNotifyAccessoryDisconnected

    if( !iNotifyAccessoryValueChangedTBool.IsNull() )
        {
        Write( iNotifyAccessoryValueChangedTBool, KAccSrvNotifyAccessoryValueChangedTBoolBit );
        iNotifyAccessoryValueChangedTBool.Complete( KErrCancel );
        }
        
    if( !iNotifyAccessoryValueChangedTInt.IsNull() )
        {
        Write( iNotifyAccessoryValueChangedTInt, KAccSrvNotifyAccessoryValueChangedTIntBit );
        iNotifyAccessoryValueChangedTInt.Complete( KErrCancel );
        }    

    if( !iNotifyAccessoryValueChangedObject.IsNull() )
        {
        Write( iNotifyAccessoryValueChangedObject, KAccSrvNotifyAccessoryValueChangedObjectBit );
        iNotifyAccessoryValueChangedObject.Complete( KErrCancel );
        }    

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::~CAccSrvSubSingleConnection - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NotifyNewAccessoryConnected
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::NotifyNewAccessoryConnected( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyNewAccessoryConnected()" );

    // Add session to queue.
    iConnectionController->NotificationQueue().RegisterControlMessage( ENewAccessoryConnected,
                                                                       this );

    iNotifyNewAccessoryConnected = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyNewAccessoryConnected - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NotifyAccessoryDisconnected
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::NotifyAccessoryDisconnected( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryDisconnected()" );

    // Add session to queue.
    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryDisconnected,
                                                                       this );

    iNotifyAccessoryDisconnected = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryDisconnected - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTBoolL
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTBoolL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTBoolL()" );
    TAccSrvGIDNameRecord     GIDNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );
    
    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    
    //Precondition
    iServerModel->IsConnectedOrDetectedL( GIDNameRecord.iGenericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    // Add session to queue.
    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryValueChangedTBool,
                                                                       this );

    iNotifyAccessoryValueChangedTBool = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTBoolL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTIntL
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTIntL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTIntL()" );
    TAccSrvGIDNameRecord     GIDNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );
    
    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    
    //Precondition
    iServerModel->IsConnectedOrDetectedL( GIDNameRecord.iGenericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    // Add session to queue.
    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryValueChangedTInt,
                                                                       this );

    iNotifyAccessoryValueChangedTInt = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedTIntL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::NotifyAccessoryValueChangedObjectL
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::NotifyAccessoryValueChangedObjectL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedObjectL()" );
    TAccSrvGIDNameRecord     GIDNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );
    
    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    
    //Precondition
    iServerModel->IsConnectedOrDetectedL( GIDNameRecord.iGenericID );//Leaves with KErrArgument if the accessory is not connected or detected
    
    // Add session to queue.
    iConnectionController->NotificationQueue().RegisterControlMessage( EAccessoryValueChangedObject,
                                                                       this );

    iNotifyAccessoryValueChangedObject = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::NotifyAccessoryValueChangedObjectL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::CancelNotifyNewAccessoryConnected
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::CancelNotifyNewAccessoryConnected( const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyNewAccessoryConnected()" );

    // if request is null there is nothign to cancel
    if( !iNotifyNewAccessoryConnected.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( ENewAccessoryConnected,
                                                                          this );
        Write( iNotifyNewAccessoryConnected, KAccSrvNotifyNewAccessoryConnectedBit );
        iNotifyNewAccessoryConnected.Complete( KErrCancel );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyNewAccessoryConnected - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::CancelNotifyAccessoryDisconnected
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::CancelNotifyAccessoryDisconnected( const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryDisconnected()" );

    // if request is null there is nothign to cancel
    if( !iNotifyAccessoryDisconnected.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( EAccessoryDisconnected,
                                                                         this );
        Write( iNotifyAccessoryDisconnected, KAccSrvNotifyAccessoryDisconnectedBit );
        iNotifyAccessoryDisconnected.Complete( KErrCancel );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryDisconnected - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL()" );

    TBool                    cancelTBoolNotification( EFalse );
    TBool                    cancelTIntNotification( EFalse );
    TBool                    cancelObjectNotification( EFalse );
    TUint32				     cancelName;
    TUint32  				 emptyName;
    TUint32  				 nameFromNotify;
    TAccPolNameRecord        nameRecord;
    TAccPolNameRecord        emptyNameRecord;
    TAccSrvGIDNameRecord     nameRecordFromNotify;
    TAccSrvNameRecordPckg    nameRecordPckg( nameRecord );
    TAccSrvGIDNameRecordPckg nameRecordFromNotifyPckg( nameRecordFromNotify );

    aMessage.ReadL( KAccServParamPosFirst, nameRecordPckg );

    nameRecord.GetName( cancelName );
    emptyNameRecord.GetName( emptyName );

    if( cancelName == emptyName )
        {
        //Cancel All
        if( !iNotifyAccessoryValueChangedTBool.IsNull() )
            {
            cancelTBoolNotification = ETrue;
            }
        if( !iNotifyAccessoryValueChangedTInt.IsNull() )
            {
            cancelTIntNotification = ETrue;
            }
        if( !iNotifyAccessoryValueChangedObject.IsNull() )
            {
            cancelObjectNotification = ETrue;
            }
        }
    else
        {
        //Cancel request specified by name
        if( !iNotifyAccessoryValueChangedTBool.IsNull() )
            {
            iNotifyAccessoryValueChangedTBool.ReadL( KAccServParamPosFirst, nameRecordFromNotifyPckg );
            nameRecordFromNotify.iNameRecord.GetName( nameFromNotify );
            COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL - TBool - notify=%d cancel=%d", nameFromNotify, cancelName );
            if( cancelName == nameFromNotify )
                {
                cancelTBoolNotification = ETrue;
                }
            }
        if( !iNotifyAccessoryValueChangedTInt.IsNull() )
            {
            iNotifyAccessoryValueChangedTInt.ReadL( KAccServParamPosFirst, nameRecordFromNotifyPckg );
            nameRecordFromNotify.iNameRecord.GetName( nameFromNotify );
            COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL - TInt - notify=%d cancel=%d", nameFromNotify, cancelName );
            if( cancelName == nameFromNotify  )
                {
                cancelTIntNotification = ETrue;
                }                
            }
        if( !iNotifyAccessoryValueChangedObject.IsNull() )
            {
            iNotifyAccessoryValueChangedObject.ReadL( KAccServParamPosFirst, nameRecordFromNotifyPckg );
            nameRecordFromNotify.iNameRecord.GetName( nameFromNotify );
            COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL - Object - notify=%d cancel=%d", nameFromNotify, cancelName );
            if( cancelName == nameFromNotify  )
                {
                cancelObjectNotification = ETrue;
                }                
            }
        }

    if( cancelTBoolNotification )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EAccessoryValueChangedTBool, this );
        if( !iNotifyAccessoryValueChangedTBool.IsNull() )
            {
            Write( iNotifyAccessoryValueChangedTBool, KAccSrvNotifyAccessoryValueChangedTBoolBit );
            iNotifyAccessoryValueChangedTBool.Complete( KErrCancel );
            }
        }

    if( cancelTIntNotification )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EAccessoryValueChangedTInt, this );
        if( !iNotifyAccessoryValueChangedTInt.IsNull() )
            {
            Write( iNotifyAccessoryValueChangedTInt, KAccSrvNotifyAccessoryValueChangedTIntBit );
            iNotifyAccessoryValueChangedTInt.Complete( KErrCancel );
            }
        }

    if( cancelObjectNotification )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
            EAccessoryValueChangedObject, this );
        if( !iNotifyAccessoryValueChangedObject.IsNull() )
            {
            Write( iNotifyAccessoryValueChangedObject, KAccSrvNotifyAccessoryValueChangedObjectBit );
            iNotifyAccessoryValueChangedObject.Complete( KErrCancel );
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::CancelNotifyAccessoryValueChangedL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::DispatchMessageL
// Handles client request. If request is not handled here, it is passed to 
// parent class for handling
// -----------------------------------------------------------------------------
//
void CAccSrvSubSingleConnection::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvNotifyNewAccessoryConnected:
            {
            NotifyNewAccessoryConnected( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryDisconnected:
            {
            NotifyAccessoryDisconnected( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryValueChangedTBool:
            {
            NotifyAccessoryValueChangedTBoolL( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryValueChangedTInt:
            {
            NotifyAccessoryValueChangedTIntL( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryValueChangedObject:
            {
            NotifyAccessoryValueChangedObjectL( aMessage );
            break;
            }
        case EAccSrvCancelNotifyNewAccessoryConnected:
            {
            CancelNotifyNewAccessoryConnected( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryDisconnected:
            {
            CancelNotifyAccessoryDisconnected( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryValueChanged:
            {
            CancelNotifyAccessoryValueChangedL( aMessage );
            break;
            }
        default:
            {
            // Not handled here, pass to base class
            CAccSrvSubConnectionBase::DispatchMessageL( aMessage );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubSingleConnection::CompleteMessageL( TMsgID aMsgID,
                                                    TInt aErrorCode,
                                                    TInt aUniqueID )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvSubSingleConnection::CompleteMessageL(msg=%d err=%d uniq=%d)", aMsgID, aErrorCode, aUniqueID );

    TBool messageConsumption ( EFalse );// Return value tells should this subsession be removed from the queue

    switch ( aMsgID )
        {
        case ENewAccessoryConnected:
            {
            //Write new generic ID and
            //complete outstanding NewAccessoryConnected notification
            TAccPolGenericID genericID;
            iServerModel->GetLastConnectedAccessoryL( genericID );
            TAccSrvGenericIDPckg genericIDPckg( genericID );
            iNotifyNewAccessoryConnected.WriteL( KAccServParamPosFirst, genericIDPckg );
            Write( iNotifyNewAccessoryConnected, KAccSrvNotifyNewAccessoryConnectedBit );
            iNotifyNewAccessoryConnected.Complete( aErrorCode );
            messageConsumption = ETrue;
            }
            break;

        case EAccessoryDisconnected:
            {
            //Complete outstanding AccessoryDisconnected notification
            TAccPolGenericID gid;
            TAccSrvGenericIDPckg genericIDPckg( gid );
            iNotifyAccessoryDisconnected.ReadL( KAccServParamPosFirst, genericIDPckg );
            if( aUniqueID == genericIDPckg().UniqueID() )
                {
                Write( iNotifyAccessoryDisconnected, KAccSrvNotifyAccessoryDisconnectedBit );
                iNotifyAccessoryDisconnected.Complete( aErrorCode );
                messageConsumption = ETrue;
                }
            else
                {
                // This notification was ordered for different gid,
                // leave this subsession to queue for further completion
                }
            }
            break;

        case EAccessoryValueChangedTInt:
            {
            //complete possible NotifyAccessoryValueChanged request
            messageConsumption = HandleValueChangedTIntCompletionL();
            }
            break;

        case EAccessoryValueChangedTBool:
            {
            //complete possible NotifyAccessoryValueChanged request
            messageConsumption = HandleValueChangedTBoolCompletionL();
            }
            break;
            
        case EAccessoryValueChangedObject:
            {
            messageConsumption = HandleValueChangedObjectCompletionL();
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            }
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubSingleConnection::CompleteMessageL - return 0x%x", messageConsumption );

    return messageConsumption;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::HandleValueChangedTIntCompletionL
//
// Check if NotifyAccessoryValueChanged() request was issued for
// Accesory Capability Name and GID in question i.e. From SetValueL().
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubSingleConnection::HandleValueChangedTIntCompletionL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedTIntCompletionL()" );

    TBool messageConsumption ( EFalse );// Return value tells should this subsession be removed from the queue

    //Read the data from notification request (i.e. NotifyAccessoryValueChanged)
    TAccSrvGIDNameRecord     GIDNameRecord;
    TAccSrvGIDNameRecordPckg GIDNameRecordPckg( GIDNameRecord );
    TAccValueTypeTInt        value;
    TAccSrvValueTypeTIntPckg valuePckg( value );
    iNotifyAccessoryValueChangedTInt.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    iNotifyAccessoryValueChangedTInt.ReadL( KAccServParamPosSecond, valuePckg );

    //Accesory Capability Name from notification request (i.e. NotifyAccessoryValueChanged() )
    TUint32 notificationName;
    GIDNameRecord.iNameRecord.GetName( notificationName );

    //Accesory Capability Name from Setter (i.e. SetValueL() )
    TUint32 setName;
    iServerModel->ValueDataNameRecord().GetName( setName );

     //Compare the notification request and changed Accesory Capability Name
    if( iServerModel->ValueDataGid().UniqueID() == GIDNameRecord.iGenericID.UniqueID()
        && notificationName == setName )
        {
        //This is a good place to prevent notification completion if value is not changed
        //i.e. Accesory Capability Name has allready the same value what is set by SetValueL().
        TAccValueTypeTInt        valueData( iServerModel->ValueDataTInt() );
        TAccSrvValueTypeTIntPckg valueDataPckg( valueData );
        iNotifyAccessoryValueChangedTInt.WriteL( KAccServParamPosSecond, valueDataPckg );
        Write( iNotifyAccessoryValueChangedTInt, KAccSrvNotifyAccessoryValueChangedTIntBit );
        iNotifyAccessoryValueChangedTInt.Complete( KErrNone );
        messageConsumption = ETrue;
        }
    else
        {
        // This notification was ordered for different gid and/or
        // Accesory Capability Name, leave this subsession
        // to queue for further completion
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedTIntCompletionL - return 0x%x", messageConsumption );
    return messageConsumption;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::HandleValueChangedTBoolCompletionL
//
// Check if NotifyAccessoryValueChanged() request was issued for
// Accesory Capability Name and GID in question i.e. From SetValueL().
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubSingleConnection::HandleValueChangedTBoolCompletionL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedTBoolCompletionL()" );

    TBool messageConsumption ( EFalse );// Return value tells should this subsession be removed from the queue

    //Read the data from notification request (i.e. NotifyAccessoryValueChanged)
    TAccSrvGIDNameRecord      GIDNameRecord;
    TAccSrvGIDNameRecordPckg  GIDNameRecordPckg( GIDNameRecord );
    TAccValueTypeTBool        value;
    TAccSrvValueTypeTBoolPckg valuePckg( value );
    iNotifyAccessoryValueChangedTBool.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );
    iNotifyAccessoryValueChangedTBool.ReadL( KAccServParamPosSecond, valuePckg );

    //Accesory Capability Name from notification request (i.e. NotifyAccessoryValueChanged() )
    TUint32 notificationName;
    GIDNameRecord.iNameRecord.GetName( notificationName );

    //Accesory Capability Name from Setter (i.e. SetValueL() )
    TUint32 setName;
    iServerModel->ValueDataNameRecord().GetName( setName );

     //Compare the notification request and changed Accesory Capability Name
    if( iServerModel->ValueDataGid().UniqueID() == GIDNameRecord.iGenericID.UniqueID()
        && notificationName == setName )
        {
        //This is a good place to prevent notification completion if value is not changed
        //i.e. Accesory Capability Name has allready the same value what is set by SetValueL().
        TAccValueTypeTBool        valueData( iServerModel->ValueDataTBool() );
        TAccSrvValueTypeTBoolPckg valueDataPckg( valueData );
        iNotifyAccessoryValueChangedTBool.WriteL( KAccServParamPosSecond, valueDataPckg );
        Write( iNotifyAccessoryValueChangedTBool, KAccSrvNotifyAccessoryValueChangedTBoolBit );
        iNotifyAccessoryValueChangedTBool.Complete( KErrNone );
        messageConsumption = ETrue;
        }
    else
        {
        // This notification was ordered for different gid and/or
        // Accesory Capability Name, leave this subsession
        // to queue for further completion
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedTBoolCompletionL - return 0x%x", messageConsumption );
    return messageConsumption;
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSingleConnection::HandleValueChangedObjectCompletionL
//
// Check if NotifyAccessoryValueChanged() request was issued for
// Accesory Capability Name and GID in question i.e. From SetValueL().
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubSingleConnection::HandleValueChangedObjectCompletionL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedObjectCompletionL()" );

    TBool messageConsumption ( EFalse );// Return value tells should this subsession be removed from the queue

    //Read the data from notification request (i.e. NotifyAccessoryValueChanged)
    TAccSrvGIDNameRecord       GIDNameRecord;
    TAccSrvGIDNameRecordPckg   GIDNameRecordPckg( GIDNameRecord );
    iNotifyAccessoryValueChangedObject.ReadL( KAccServParamPosFirst, GIDNameRecordPckg );

    //Accesory Capability Name from notification request (i.e. NotifyAccessoryValueChanged() )
    TUint32 notificationName;
    GIDNameRecord.iNameRecord.GetName( notificationName );

    //Accesory Capability Name from Setter (i.e. SetValueL() )
    TUint32 setName;
    iServerModel->ValueDataNameRecord().GetName( setName );

     //Compare the notification request and changed Accesory Capability Name
    if( iServerModel->ValueDataGid().UniqueID() == GIDNameRecord.iGenericID.UniqueID()
        && notificationName == setName )
        {
        //This is a good place to prevent notification completion if value is not changed
        //i.e. Accesory Capability Name has allready the same value what is set by SetValueL().
        TAccValueTypeObject valueData( iServerModel->ValueDataObject() );
        iNotifyAccessoryValueChangedObject.WriteL( KAccServParamPosSecond, valueData.iValue );
        Write( iNotifyAccessoryValueChangedObject, KAccSrvNotifyAccessoryValueChangedObjectBit );
        iNotifyAccessoryValueChangedObject.Complete( KErrNone );
        messageConsumption = ETrue;
        }
    else
        {
        // This notification was ordered for different gid and/or
        // Accesory Capability Name, leave this subsession
        // to queue for further completion
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubSingleConnection::HandleValueChangedObjectCompletionL - return 0x%x", messageConsumption );
    return messageConsumption;
    }

//  End of File
