/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory Control Interface class for HDMI Cable Status FSM.
*
*/


#include "accessorycontrolif.h"
#include "multifinitestatemachine.h"
#include "pdeconstants.h"
#include "trace.h"
#include "hdmicablestatusfsm.h"
#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include <AsyCmdTypes.h>
#include <AccConfigFileParser.h>
#include <e32cmn.h>

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CAccessoryControlIf* CAccessoryControlIf::NewL(
        MFSMForBody& aFSM,
        RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;
    CAccessoryControlIf* self = new ( ELeave ) CAccessoryControlIf(  aFSM );
    CleanupStack::PushL( self );
    self->ConstructL( aAccessoryServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAccessoryControlIf::~CAccessoryControlIf()
    {
    FUNC_LOG;
    Cancel();
    if ( iAccessoryConnected )
        {
        TRequestStatus status;                
        iAccessoryControl.DisconnectAccessory( status, iGenericIDPtr->GenericID() );
        User::WaitForRequest( status ); 
        TInt retval = status.Int();
        INFO_1( "iAccessoryControl.DisconnectAccessory() return value %i", retval );
        }
    delete iGenericIDPtr;
    iAccessoryControl.CloseSubSession();        
    }

// ---------------------------------------------------------------------------
// ConnectAccessoryL
// ---------------------------------------------------------------------------
//
void CAccessoryControlIf::ConnectAccessoryL( TBool aCopyProtectionStatus )
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        CAccConfigFileParser* accConfigFileParser =
            CAccConfigFileParser::NewL( _L("Configuration file") );
        CleanupStack::PushL( accConfigFileParser );
        if ( iGenericIDPtr )
            {
            delete iGenericIDPtr;
            iGenericIDPtr = NULL;
            }
        iGenericIDPtr = CAccConGenericID::NewL();

        RArray<TAccPolNameValueRecord> nameValueArray;
        CleanupClosePushL( nameValueArray );
        TBuf<KHWModelIDMaxLength> hwModelID( _L("hdmi"));
        TAccPolNameRecord nameRecord;
        TAccPolValueRecord valueRecord;

        TBuf<KHWModelIDMaxLength> HWModelID( _L("hdmi"));

        //Set GenericID header
        TAccPolGIDHeader GIDHeader;
        GIDHeader.iAccessoryDeviceType = KDTAVDevice;
        GIDHeader.iPhysicalConnection = KPCHDMI;
        GIDHeader.iApplicationProtocol = 0x0;    
        GIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock | KSBVideoSubblock;
        GIDHeader.iHWModelID = hwModelID;
        GIDHeader.iHWDeviceID = 0x0;
        GIDHeader.iDeviceAddress = 0x0;
        TAccPolGenericID genericID;
        GIDHeader.iDBID = genericID.UniqueID();

				//Set "Audio Output Type" capability
				TAccPolNameValueRecord nameValueRecordAudioOutputType( KAccAudioOutputType,
					  EAccAudioOutPutTypePublic,
						EAPVInt,
						EAPVPolicy );
				nameValueArray.AppendL( nameValueRecordAudioOutputType );		

        //Set "Video Output Type" capability
        TAccPolNameValueRecord nameValueRecordVideoOut( KAccVideoOut,
            EAccVideoOutEHDMI,
            EAPVInt,
            EAPVAccessory );
        nameValueArray.AppendL( nameValueRecordVideoOut );
        
        // Set "HDMI attributes" capability
        TAccPolNameValueRecord nameValueRecordHdmiAttributes( KAccVideoHdmiAttributes,
            EAPVAccessory );
        nameValueArray.AppendL( nameValueRecordHdmiAttributes );

        // Set "HDCP" capability
        TAccPolNameValueRecord nameValueRecordHdcpSupport( KAccVideoHdcpSupport,
            aCopyProtectionStatus,
            EAPVBool,
            EAPVAccessory );
        nameValueArray.AppendL( nameValueRecordHdcpSupport );
        
        // Set "HDCP status" capability
        TAccPolNameValueRecord nameValueRecordHdcpSupportStatus( KAccVideoHdcpSupportStatus,
        aCopyProtectionStatus,
        EAPVBool,
        EAPVAccessory );
        nameValueArray.AppendL( nameValueRecordHdcpSupportStatus );
    
        // Set "Video active format" capability
        TAccPolNameValueRecord nameValueRecordVideoFormat( KAccVideoFormat,
            EAPVAccessory );
        nameValueArray.AppendL( nameValueRecordVideoFormat );

        accConfigFileParser->ParseGenericIDL( iGenericIDPtr,
            GIDHeader,
            nameValueArray );
        nameValueArray.Close();


        CleanupStack::PopAndDestroy( &nameValueArray );
        CleanupStack::PopAndDestroy( accConfigFileParser );

        iActiveRequestId = EAccessoryControlIfConnectRequested;
        iAccessoryControl.ConnectAccessory( iStatus, iGenericIDPtr, EFalse );
        SetActive();        
        }
    else
        {
        if ( EAccessoryControlIfConnectRequested == iActiveRequestId )
            {            
            INFO("Could not connect, because connect request already pending")
            }
        else if ( EAccessoryControlIfDisconnectRequested == iActiveRequestId )
            {
            INFO("Could not connect, because disconnect is pending")
            }
        User::Leave(KErrCouldNotConnect);
        }
    }

// ---------------------------------------------------------------------------
// DisconnectAccessory
// ---------------------------------------------------------------------------
//
void CAccessoryControlIf::DisconnectAccessory()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        Cancel();
        }
    iActiveRequestId = EAccessoryControlIfDisconnectRequested;
    iAccessoryControl.DisconnectAccessory( iStatus, iGenericIDPtr->GenericID() );
    SetActive();        
    }

// ---------------------------------------------------------------------------
// GetUniqueID
// ---------------------------------------------------------------------------
//
TInt CAccessoryControlIf::GetUniqueID()
    {
    FUNC_LOG;
    TInt uniqueID( 0 );
    if ( iGenericIDPtr  )
        {
        uniqueID = iGenericIDPtr->GenericID().UniqueID();
        }       
    return uniqueID;
    }

// ---------------------------------------------------------------------------
// GetAccPolGenericID
// ---------------------------------------------------------------------------
//
TInt CAccessoryControlIf::GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    if ( iGenericIDPtr )
        {
        aAccPolGenericID = iGenericIDPtr->GenericID();
        }
    else
        {
        retVal = KErrNotFound;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// NotifyValueChange
// ---------------------------------------------------------------------------
//
TInt CAccessoryControlIf::NotifyValueChange(
        const TUint32& aName,
        const TAccValueTypeTBool aValue ) const
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    if ( iGenericIDPtr )
        {
        TAccPolNameRecord aNameRecord;
        aNameRecord.SetName( aName );
        TRAP( retVal, iAccessoryControl.AccessoryValueChangedNotifyL( iGenericIDPtr->GenericID(), aNameRecord, aValue ) );
        if ( KErrNone != retVal )
            {
            INFO_1( "RAccessoryControl::AccessoryValueChangedNotifyL() failed, return code: %d", retVal );
            }
        }
    else
        {
        INFO( "No Generic ID found, cannot notify");
        retVal = KErrNotFound;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// NotifyValueChange
// ---------------------------------------------------------------------------
//
TInt CAccessoryControlIf::NotifyValueChange(
        const TUint32& aName,
        const CAccPolObjectCon& aValue ) const
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    if ( iGenericIDPtr )
        {
        TAccPolNameRecord aNameRecord;
        aNameRecord.SetName( aName );
        TRAP( retVal, iAccessoryControl.AccessoryValueChangedNotifyL( iGenericIDPtr->GenericID(), aNameRecord, aValue ) );
        if ( KErrNone != retVal )
            {
            INFO_1( "RAccessoryControl::AccessoryValueChangedNotifyL() failed, return code: %d", retVal );
            }
        }
    else
        {
        INFO( "No Generic ID found, cannot notify");
        retVal = KErrNotFound;
        }
    return retVal;
    }


// ----------------------------------------------------------------------------
// RunL
//
// ----------------------------------------------------------------------------
//
void CAccessoryControlIf::RunL()
    {

    FUNC_LOG;
    TInt retVal( iStatus.Int() );
    if ( KErrNone == retVal )
        {
        if (  EAccessoryControlIfConnectRequested == iActiveRequestId )
            {
            iAccessoryConnected = ETrue;

            ((CHDMICableStatusFSM*)&iFSM)->GetPDEngine()->Input(
                    EPDEFSMIdHDMICableStatus,
                    EPDEIfAccessoryControl,
                    EPDEIfAccessoryControlEventConnected );
            }
        else if (  EAccessoryControlIfDisconnectRequested == iActiveRequestId )
            {
            iAccessoryConnected = EFalse;
 
            ((CHDMICableStatusFSM*)&iFSM)->GetPDEngine()->Input(
                    EPDEFSMIdHDMICableStatus,
                    EPDEIfAccessoryControl,
                    EPDEIfAccessoryControlEventDisconnected );
            delete iGenericIDPtr;
            iGenericIDPtr = NULL;
            }
        }
    else
        {
        if (  EAccessoryControlIfConnectRequested == iActiveRequestId )
            {
            INFO_1( "RAccessoryControl::ConnectAccessory failed, error code: %d", retVal );

            ((CHDMICableStatusFSM*)&iFSM)->GetPDEngine()->Input(
                    EPDEFSMIdHDMICableStatus,
                    EPDEIfAccessoryControl,
                    EPDEIfAccessoryControlEventConnectFailed );            
            }
        else if (  EAccessoryControlIfDisconnectRequested == iActiveRequestId )
            {
            INFO_1( "RAccessoryControl::DisconnectAccessory failed, error code: %d", retVal );
           
            ((CHDMICableStatusFSM*)&iFSM)->GetPDEngine()->Input(
                    EPDEFSMIdHDMICableStatus,
                    EPDEIfAccessoryControl,
                    EPDEIfAccessoryControlEventDisconnectFailed );            
            delete iGenericIDPtr;
            iGenericIDPtr = NULL;
            }
        }
    iActiveRequestId = EAccessoryControlIfNoRequestActive;
    }

// ----------------------------------------------------------------------------
// DoCancel
//
// ----------------------------------------------------------------------------
//
void CAccessoryControlIf::DoCancel()
    {
    FUNC_LOG;
    if (  EAccessoryControlIfConnectRequested == iActiveRequestId )
        {
        iAccessoryControl.CancelConnectAccessory();
        }
    else if (  EAccessoryControlIfDisconnectRequested == iActiveRequestId )
        {
        iAccessoryControl.CancelDisconnectAccessory();
        }
    iActiveRequestId = EAccessoryControlIfNoRequestActive;
    }

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CAccessoryControlIf::CAccessoryControlIf(
        MFSMForBody& aFSM
        ) 
    : CActive( CActive::EPriorityLow ),
    iFSM( aFSM )
    {
    FUNC_LOG;
    iGenericIDPtr = NULL;
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CAccessoryControlIf::ConstructL( RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;
    
    User::LeaveIfError( iAccessoryControl.CreateSubSession( aAccessoryServer ) );
    CActiveScheduler::Add( this );
    }

// ======== GLOBAL FUNCTIONS ========

