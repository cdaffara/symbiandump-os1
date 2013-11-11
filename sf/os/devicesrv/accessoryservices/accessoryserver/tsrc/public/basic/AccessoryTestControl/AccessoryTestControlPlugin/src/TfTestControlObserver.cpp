/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:
 *
 */

// INCLUDE FILES
#include <tftypes.h>
#include "siftrace.h"
#include "CtfTestControlObserver.h" 
#include <AccPolGIDHeader.h>

const TInt CTFTestControlObserver::iOffset = _FOFF(CTFTestControlObserver,iSlink);

// ----------------------------------------------------------------------------------
// CTFTestControlObserver* CTFTestControlObserver::NewL()
// ----------------------------------------------------------------------------------	
CTFTestControlObserver* CTFTestControlObserver::NewL( CTFAccessoryTestControl& aTestControl )
    {
    CTFTestControlObserver* result =
        new ( ELeave ) CTFTestControlObserver( aTestControl );
    result->iObjectValueBuf = HBufC8::NewL( 256 );
    result->iObjectValue.iValue.Set( result->iObjectValueBuf->Des() );
    CleanupStack::PushL( result );
    CleanupStack::Pop();
    return result;
    }

// ----------------------------------------------------------------------------------
// CTFTestControlObserver::CTFTestControlObserver() 
// ----------------------------------------------------------------------------------	
CTFTestControlObserver::CTFTestControlObserver( CTFAccessoryTestControl& aTestControl ) :
    CActive( CActive::EPriorityHigh ),
    iAccessoryTestControl( aTestControl ),
    iObjectValue( TPtr8( NULL, 0, 0 ) )
    {

    iRequestCompleted = ReqAccRequestNone;
    iCapabilityName = 0;

    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::CTFTestControlObserver()" ) ) );
    CActiveScheduler::Add( this );
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::CTFTestControlObserver - return" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::~CTFTestControlObserver()
// ----------------------------------------------------------------------------------	
CTFTestControlObserver::~CTFTestControlObserver()
    {
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::~CTFTestControlObserver - %d" ), iRequestType ) );
    delete iObjectValueBuf;
    COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::~CTFTestControlObserver - succesful" ) ) );
    }
//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::RunL()
// ----------------------------------------------------------------------------------
void CTFTestControlObserver::RunL()
    {

    TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL - enter" ) ) );

    switch( iRequestType )
        {

        case ReqDisconnectBtAccessory:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqDisconnectBtAccessory" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqConnectBtAccessory:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqConnectBtAccessory" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyNewAccessoryConnected:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyNewAccessoryConnected" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyAccessoryDisconnected:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyAccessoryDisconnected" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyAccessoryConnectionStatusChanged:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyAccessoryConnectionStatusChanged" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyAccessoryModeChanged:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyAccessoryModeChanged" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAccMode.iAccessoryMode;
            }
            break;

        case ReqGetAccessoryMode:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqGetAccessoryMode" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAccMode.iAccessoryMode;
            }
            break;

        case ReqNotifyAccessoryAudioLinkOpened:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqConnectBtAccessory" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAudioType;
            }
            break;

        case ReqAccessoryAudioLinkOpen:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqConnectBtAccessory" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyBluetoothAudioLinkOpenReq:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyBluetoothAudioLinkOpenReq" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAudioType;
            }
            break;

        case ReqNotifyAccessoryAudioLinkClosed:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyAccessoryAudioLinkClosed" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqAccessoryAudioLinkClose:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqAccessoryAudioLinkClose" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqNotifyBluetoothAudioLinkCloseReq:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyBluetoothAudioLinkCloseReq" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAudioType;
            }
            break;

        case ReqNotifyIntegerAccessoryValueChanged:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyIntegerAccessoryValueChanged" ) ) );
            iCapabilityName = 0;
            iRequestCompleted = iRequestType;
            iValue = iIntegerValue.iValue;
            }
            break;

        case ReqNotifyBooleanAccessoryValueChanged:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqNotifyBooleanAccessoryValueChanged" ) ) );
            iCapabilityName = 0;
            iRequestCompleted = iRequestType;
            iValue = ( TInt )iBooleanValue.iValue;
            }
            break;

        case ReqNotifyObjectAccessoryValueChanged:
            {
            iCapabilityName = 0;
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqAccessoryConnection:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqAccessoryConnection" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqAccessoryDisconnection:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqAccessoryDisconnection" ) ) );
            iRequestCompleted = iRequestType;
            }
            break;

        case ReqGetAccessoryConnectionStatus:
            {
            COMPONENT_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL() - ReqGetAccessoryConnectionStatus" ) ) );
            iRequestCompleted = iRequestType;
            iValue = iAccessoryTestControl.iGenericIdArray.Count();
            }
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }

    TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::RunL - return" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::RunError()
// ----------------------------------------------------------------------------------
TInt CTFTestControlObserver::RunError( TInt aError )
    {
    return aError;
    }
//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::DoCancel()
// ----------------------------------------------------------------------------------
void CTFTestControlObserver::DoCancel()
    {
    TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::DoCancel - enter" ) ) );

    switch( iRequestType )
        {

        case ReqDisconnectBtAccessory:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryBTControl->CancelDisconnectAccessory();
                }
            }
            break;

        case ReqConnectBtAccessory:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryBTControl->CancelConnectAccessory();
                }
            }
            break;

        case ReqNotifyNewAccessoryConnected:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessorySingleConnection->CancelNotifyNewAccessoryConnected();
                }
            }
            break;

        case ReqNotifyAccessoryDisconnected:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessorySingleConnection->CancelNotifyAccessoryDisconnected();
                }
            }
            break;

        case ReqNotifyAccessoryConnectionStatusChanged:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryConnection->CancelNotifyAccessoryConnectionStatusChanged();
                }
            }
            break;

        case ReqNotifyAccessoryModeChanged:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryMode->CancelNotifyAccessoryModeChanged();
                }
            }
            break;

        case ReqNotifyAccessoryAudioLinkOpened:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryAudioControl->CancelNotifyAccessoryAudioLinkOpened();
                }
            }
            break;

        case ReqAccessoryAudioLinkOpen:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryAudioControl->CancelAccessoryAudioLinkOpen();
                }
            }
            break;

        case ReqNotifyBluetoothAudioLinkOpenReq:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryBTControl->CancelNotifyBluetoothAudioLinkOpenReq();
                }
            }
            break;

        case ReqNotifyAccessoryAudioLinkClosed:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryAudioControl->CancelNotifyAccessoryAudioLinkClosed();
                }
            }
            break;

        case ReqAccessoryAudioLinkClose:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryAudioControl->CancelAccessoryAudioLinkClose();
                }
            }
            break;

        case ReqNotifyBluetoothAudioLinkCloseReq:
            {
            if( IsActive() )
                {
                iAccessoryTestControl.iAccessoryBTControl->CancelNotifyBluetoothAudioLinkCloseReq();
                }
            }
            break;

        case ReqGetAccessoryConnectionStatus:
            {
            //if(IsActive())
            //{		
            //iAccessoryTestControl.iAccessoryConnection.CancelGetAccessoryConnectionStatus();				
            //	}
            }
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }

    TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::DoCancel - Leave" ) ) );
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyNewAccessoryConnected
// ----------------------------------------------------------------------------------  

void CTFTestControlObserver::NotifyNewAccessoryConnected( TAccPolGenericID& aGenericId )
    {

    if( !IsActive() )
        {
        SetRequestType( ReqNotifyNewAccessoryConnected );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessorySingleConnection->NotifyNewAccessoryConnected( iStatus,
            aGenericId );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyNewAccessoryConnected outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyAccessoryDisconnected
// ----------------------------------------------------------------------------------  

void CTFTestControlObserver::NotifyAccessoryDisconnected( TAccPolGenericID& aGenericId )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyAccessoryDisconnected );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessorySingleConnection->NotifyAccessoryDisconnected( iStatus,
            aGenericId );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyAccessoryDisconnected outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyAccessoryModeChanged
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyAccessoryModeChanged()
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyAccessoryModeChanged );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryMode->NotifyAccessoryModeChanged( iStatus,
            iAccMode );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyAccessoryModeChanged outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
//  CTFTestControlObserver::NotifyAccessoryConnectionStatusChanged
// ----------------------------------------------------------------------------------  

void CTFTestControlObserver::NotifyAccessoryConnectionStatusChanged( TAccPolGenericIDArray& aGenericIdArray )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyAccessoryConnectionStatusChanged );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryConnection->NotifyAccessoryConnectionStatusChanged( iStatus,
            aGenericIdArray );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyAccessoryConnectionStatusChanged outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::AccessoryAudioLinkOpen()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::AccessoryAudioLinkOpen( TAccPolGenericID& aGenericId,
    TUint32 aAudioType )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqAccessoryAudioLinkOpen );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryAudioControl->AccessoryAudioLinkOpen( iStatus,
            aGenericId,
            ( TAccAudioType )aAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::AccessoryAudioLinkOpen outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyAccessoryAudioLinkOpened
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyAccessoryAudioLinkOpened( TAccPolGenericID& aGenericId )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyAccessoryAudioLinkOpened );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryAudioControl->NotifyAccessoryAudioLinkOpened( iStatus,
            aGenericId,
            iAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyAccessoryAudioLinkOpened outstanding" ) ) );
        }
    }
//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyAccessoryAudioLinkClosed
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyAccessoryAudioLinkClosed( TAccPolGenericID& aGenericId )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyAccessoryAudioLinkClosed );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryAudioControl->NotifyAccessoryAudioLinkClosed( iStatus,
            aGenericId,
            iAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyAccessoryAudioLinkClosed outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::AccessoryAudioLinkClose
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::AccessoryAudioLinkClose( TAccPolGenericID& aGenericId,
    TUint32 aAudioType )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqAccessoryAudioLinkClose );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryAudioControl->AccessoryAudioLinkClose( iStatus,
            aGenericId,
            ( TAccAudioType )aAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::AccessoryAudioLinkClose outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyBluetoothAudioLinkOpenReq
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyBluetoothAudioLinkOpenReq()
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyBluetoothAudioLinkOpenReq );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryBTControl->NotifyBluetoothAudioLinkOpenReq( iStatus,
            iBTaddr,
            iAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyBluetoothAudioLinkOpenReq outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyBluetoothAudioLinkCloseReq
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyBluetoothAudioLinkCloseReq()
    {
    if( !IsActive() )
        {
        SetRequestType( ReqNotifyBluetoothAudioLinkCloseReq );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryBTControl->NotifyBluetoothAudioLinkCloseReq( iStatus,
            iBTaddr,
            iAudioType );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyBluetoothAudioLinkCloseReq outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::ConnectBTAccessory()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::ConnectBTAccessory( TBTDevAddr& btaddr )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqConnectBtAccessory );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryBTControl->ConnectAccessory( iStatus,
            btaddr );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::ConnectBTAccessory outstanding" ) ) );
        }

    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::DisconnectBTAccessory()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::DisconnectBTAccessory( TBTDevAddr& btaddr )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqDisconnectBtAccessory );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryBTControl->DisconnectAccessory( iStatus,
            btaddr );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::DisconnectBTAccessory outstanding" ) ) );
        }

    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyBooleanAccessoryValueChanged()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::NotifyBooleanAccessoryValueChanged( TAccPolGenericID& aGenericId,
    const TUint32& aName )
    {
    if( !IsActive() )
        {
        TAccPolNameRecord nameRecord;
        iCapabilityName = aName;
        nameRecord.SetNameL( aName );
        SetRequestType( ReqNotifyBooleanAccessoryValueChanged );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessorySingleConnection->NotifyAccessoryValueChanged( iStatus,
            aGenericId,
            nameRecord,
            iBooleanValue );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyBooleanAccessoryValueChanged outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyIntegerAccessoryValueChanged()
// ----------------------------------------------------------------------------------          
void CTFTestControlObserver::NotifyIntegerAccessoryValueChanged( TAccPolGenericID& aGenericId,
    const TUint32& aName )
    {
    if( !IsActive() )
        {
        TAccPolNameRecord nameRecord;
        iCapabilityName = aName;
        nameRecord.SetNameL( aName );
        SetRequestType( ReqNotifyIntegerAccessoryValueChanged );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessorySingleConnection->NotifyAccessoryValueChanged( iStatus,
            aGenericId,
            nameRecord,
            iIntegerValue );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyIntegerAccessoryValueChanged outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::NotifyIntegerAccessoryValueChanged()
// ----------------------------------------------------------------------------------
void CTFTestControlObserver::NotifyObjectAccessoryValueChanged( TAccPolGenericID& aGenericId,
    const TUint32& aName )
    {
    if( !IsActive() )
        {
        // Cleanup old data
        iObjectValue.iValue.Zero();
        
        TAccPolNameRecord nameRecord;
        iCapabilityName = aName;
        nameRecord.SetNameL( aName );
        SetRequestType( ReqNotifyObjectAccessoryValueChanged );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessorySingleConnection->NotifyAccessoryValueChanged( iStatus,
            aGenericId,
            nameRecord,
            iObjectValue );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::NotifyIntegerAccessoryValueChanged outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::GetAccessoryConnectionStatus()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::GetAccessoryConnectionStatus( TAccPolGenericIDArray& aGenericIdArray )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqGetAccessoryConnectionStatus );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryConnection->GetAccessoryConnectionStatus( iStatus,
            aGenericIdArray );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::GetAccessoryConnectionStatus outstanding" ) ) );
        }

    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::ConnectAccessory()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::ConnectAccessory( TAccPolGenericID& aGenericId,
    TUint64 aHWDeviceID )
    {
    CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL( _L("Configuration file") );

    TBuf<KHWModelIDMaxLength> aHWModelID;
    aHWModelID.Num( aHWDeviceID );

    accConfigFileParser->FindL( aGenericId, aHWDeviceID, aHWModelID );

    if( !IsActive() )
        {
        SetRequestType( ReqAccessoryConnection );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryControl->ConnectAccessory( iStatus,
            aGenericId );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::ConnectAccessory outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::DisconnectAccessory()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::DisconnectAccessory( TAccPolGenericID& aGenericId )
    {
    if( !IsActive() )
        {
        SetRequestType( ReqAccessoryDisconnection );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryControl->DisconnectAccessory( iStatus,
            aGenericId );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::DisconnectAccessory outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// CTFTestControlObserver::GetAccessoryMode()
// ----------------------------------------------------------------------------------  
void CTFTestControlObserver::GetAccessoryMode()
    {
    if( !IsActive() )
        {
        SetRequestType( ReqGetAccessoryMode );
        iStatus = KRequestPending;
        iAccessoryTestControl.iAccessoryMode->GetAccessoryMode( iStatus,
            iAccMode );
        SetActive();
        TEST_CASE_TRACE( ( _L( "ACCESSORYTESTCONTROL - CTFTestControlObserver::GetAccessoryMode outstanding" ) ) );
        }
    }

//
// ----------------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------------  
TBool CTFTestControlObserver::FindRequest( TTFRequestType aRequest )
    {
    TBool bFound = EFalse;

    if( aRequest == iRequestType )
        {
        bFound = ETrue;
        }
    return bFound;
    }

