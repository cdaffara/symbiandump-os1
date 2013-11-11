/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class handles connect and disconnect to AccFw.
*
*/

#include <acccongenericid.h>
#include <accpolobjectcon.h>
#include <accpolnamevaluerecord.h>
#include <accconfigfileparser.h>
#include <accpolcommonnamevaluepairs.h>
#include <accessoryservicesinternalpskeys.h>
#include <autoaudiopskeys.h>

#include "cautoaudioasyservice.h"
#include "cautoaudioasypropertyobserver.h"
#include "acc_debug.h"

#define SET_STATE(state) (iState=state)
 
// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::CAutoAudioAsyService() 
// ----------------------------------------------------------------------------------
CAutoAudioAsyService::CAutoAudioAsyService ()
	: CActive ( EPriorityStandard )
    {
    CActiveScheduler::Add ( this );
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::NewL ()
// ----------------------------------------------------------------------------------
CAutoAudioAsyService* CAutoAudioAsyService::NewL ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::NewL" );
    CAutoAudioAsyService* self = new ( ELeave ) CAutoAudioAsyService ();
    CleanupStack::PushL ( self );
    self->ConstructL ();
    CleanupStack::Pop ( self );
    return self;
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::ConstructL ()
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::ConstructL ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::ConstructL" );
    User::LeaveIfError ( iAccessoryServer.Connect () );
    User::LeaveIfError ( iAccessoryControl.CreateSubSession ( iAccessoryServer ) );
    SET_STATE ( EServiceStateIdle );	
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::~CAutoAudioAsyService()
// ----------------------------------------------------------------------------------
CAutoAudioAsyService::~CAutoAudioAsyService ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::~CAutoAudioAsyService" );
    Cancel ();
	
	delete iPropertyObserver;
    iAccessoryControl.CloseSubSession ();
    iAccessoryServer.Close ();
    
	FreeResources ();
    }
	
void CAutoAudioAsyService::StartL ()
	{
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::StartL" );
	iPropertyObserver = CAutoAudioAsyPropertyObserver::NewL ( KPSUidAccessoryServices, KPSAutoKeyRTPStreamingConnectionStatus, *this );
	}

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::PropertyChange ()
// Defined in MAutoAudioAsyPropertyListener interface.
// Receives notifications about RTP Streaming channel property changes. Property changes occur
// when accessory is connected or disconnected or headunit attached or detached
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::PropertyChange ( RProperty& aProperty, TUid aUid, TUint aKey, TInt aStatus )
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::PropertyChange" );

    TInt propValue;
    TInt status = aProperty.Get ( propValue );
    COM_TRACE_1 ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::PropertyChange -- request property value %d", propValue );
    
    if ( aUid == KPSUidAccessoryServices && aKey == KPSAutoKeyRTPStreamingConnectionStatus )
        {
        // Check if audio accessory has been connected
        if ( ( status == KErrNone ) && ( aStatus == KErrNone ) )
            {
			if ( propValue == EAudioConnectionStatusUnidirectional ) // Current release supports only UniDirectional, so other key values ignored.
				{
				TInt err;
				switch ( iState )
					{
					case EServiceStateIdle:
						TRAP ( err, ConnectAccessoryL () );
						if ( err != KErrNone )
							{
							COM_TRACE_1 ("[AccFW:AutoAudioAsy] ConnectAccessoryL error %d", err );
							FreeResources (); // switch to EServiceStateIdle							
							}
						break;
						
					case EServiceStateConnecting:
					case EServiceStateConnected:
						// Nothing to do
						break;
					}
				return;
				}
            }
			
        // Otherwise assume that it's disconnected
        AccessoryDisconnected ();
        }
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::AccessoryDisconnected ()
// Called when a disconnect notification is received
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::AccessoryDisconnected ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::AccessoryDisconnected" );
    TRequestStatus status;
    switch ( iState )
        {
        case EServiceStateConnected:
            iAccessoryControl.DisconnectAccessory ( status, iGenericId->GenericID () );
            User::WaitForRequest ( status );
            FreeResources (); // sets state to EServiceStateIdle
            break;

        case EServiceStateConnecting:
            Cancel ();
            break;

        default:
            // nothing to do in this case
            break;
        }
    }

// ----------------------------------------------------------------------------------
// CAutoAudioAsyService::ConnectAccessoryL ()
// Notifies the accessory server and switches to EServiceStateConnecting state.
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::ConnectAccessoryL ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::ConnectAccessoryL" );

    RArray<TAccPolNameValueRecord> nameValueArray;
    CleanupClosePushL ( nameValueArray );

    // Set GenericID header
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] Set generic header" );
    iGenericIDHeader.iAccessoryDeviceType   = KDTCarKit;
    iGenericIDHeader.iPhysicalConnection    = KPCUSB;
    iGenericIDHeader.iApplicationProtocol   = 0x0;
    iGenericIDHeader.iCapabilitiesSubblocks = KSBAudioSubblock;
    iGenericIDHeader.iHWDeviceID            = 0x0;
    iGenericIDHeader.iHWModelID				= KNullDesC;

    iGenericId = CAccConGenericID::NewL ();

    COM_TRACE_ ( "[AccFW:AutoAudioAsy] Set capabilities" );
    
    // Set "Audio Output Type" capability
	COM_TRACE_ ( "[AccFW:AutoAudioAsy] KAccAudioOutputType " );
    nameValueArray.AppendL ( TAccPolNameValueRecord ( KAccAudioOutputType,
        EAccAudioOutPutTypePublic, EAPVInt, EAPVPolicy ) );

    // Set "Audio stereo" capability
	COM_TRACE_ ( "[AccFW:AutoAudioAsy] KAccStereoAudio " );
	nameValueArray.AppendL ( TAccPolNameValueRecord ( KAccStereoAudio ) );

    // Set "Integrated audio output"
	COM_TRACE_ ( "[AccFW:AutoAudioAsy] KAccIntegratedAudioOutput " );
	nameValueArray.AppendL ( TAccPolNameValueRecord ( KAccIntegratedAudioOutput ) );

	// Set "RTP streaming"
	COM_TRACE_ ( "[AccFW:AutoAudioAsy] KAccRTPStreaming " );
	nameValueArray.AppendL ( TAccPolNameValueRecord ( KAccRTPStreaming ) );

    // Make generic id
    CAccConfigFileParser* accConfigFileParser = CAccConfigFileParser::NewL ( KNullDesC );
    CleanupStack::PushL ( accConfigFileParser );
    accConfigFileParser->ParseGenericIDL ( iGenericId, iGenericIDHeader, nameValueArray );

    // Connect accessory
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] Connecting accessory..." );
    iAccessoryControl.ConnectAccessory ( iStatus, iGenericId, EFalse );
    SET_STATE ( EServiceStateConnecting );
    SetActive ();

    CleanupStack::PopAndDestroy ( accConfigFileParser );
    CleanupStack::PopAndDestroy ( &nameValueArray );
    }    

// ----------------------------------------------------------------------------------
// CActive methods
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::DoCancel ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::DoCancel" );
    iAccessoryControl.CancelConnectAccessory ();
    FreeResources ();
    }

TInt CAutoAudioAsyService::RunError ( TInt /*aError*/ )
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::RunError" );
    return KErrNone;
    }

void CAutoAudioAsyService::RunL ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::RunL" );
	TInt retVal ( iStatus.Int () );
	COM_TRACE_1 ( "[AccFW:AutoAudioAsy] Connected with return value %d", retVal );
	
    if ( KErrNone == retVal )
		{
			switch ( iState )
			{
			case EServiceStateConnecting:				
				SET_STATE ( EServiceStateConnected );
				break;
			
			case EServiceStateIdle:
			case EServiceStateConnected:
			default:
				COM_TRACE_ ( "State machine broken" );
				break;
			}
		}
	else
		{
		COM_TRACE_ ( "[AccFW:AutoAudioAsy] Accessory Server failed to accept connection, freeing up resources" );		
		FreeResources ();
		}
	}	
	
// ----------------------------------------------------------------------------------
// Frees resources and set state to EServiceStateIdle
// ----------------------------------------------------------------------------------
void CAutoAudioAsyService::FreeResources ()
    {
    COM_TRACE_ ( "[AccFW:AutoAudioAsy] CAutoAudioAsyService::FreeResources" );

    if ( iState != EServiceStateIdle )
        {
        SET_STATE ( EServiceStateIdle );
        }

    delete iGenericId;
    iGenericId = NULL;
    }
