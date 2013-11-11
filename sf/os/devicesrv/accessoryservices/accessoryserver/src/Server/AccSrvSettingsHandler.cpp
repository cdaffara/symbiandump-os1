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
* Description:  Control for accessory related settings.
*
*/


// INCLUDE FILES
#include "AccSrvSettingsHandler.h"
#include "AccSrvConnectionController.h"
#include "acc_debug.h"
#include <AccessoryTypes.h>
#include <e32property.h>
#include <centralrepository.h>
#include <hwrmdomainpskeys.h>
#include <AccSettingsDomainCRKeys.h>
#include <f32file.h>
#include <hwrmlight.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KAccSettingIdLength = 4; // "int:" or "boo:" or "des:"
const TInt KAccSettingMaxLength = KAccSettingIdLength + KMaxAccModeSetting;
// Max size in bytes for hwdevices.ini file
const TInt KHWDevicesFileMaxSize = 4;
// Maximum number of supported accessory modes
const TInt KMaxAccessoryMode = 4096;
// Maximum number of supported settings for each accessory mode
const TInt KMaxAccessorySetting = 4096;
// First bit reserved for accessory mode when counting is started from right
const TInt KAccessoryModeStartBit = 20;
// First bit reserved for accessory mode setting when counting is started from
// right
const TInt KAccessoryModeSettingStartBit = 8;

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KAccSettingInteger, "int:" );
_LIT( KAccSettingBoolean, "boo:" );
_LIT( KAccSettingDescriptor, "des:" );
// File that stores supported HW devices
_LIT( KHWDevicesFile, "c:\\private\\1020504A\\hwdevices.ini" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::CAccSrvSettingsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSettingsHandler::CAccSrvSettingsHandler( CAccSrvConnectionController* aConCtrl,
                                                CAccSrvServerModel& aModel,
                                                CAccPolAccessoryPolicy* aPolicy )
    : iConCtrl( aConCtrl ),
      iModel( aModel ),
      iLightsOn( EAccSettingsLightsNotSet ),
      iPolicy( aPolicy )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::CAccSrvSettingsHandler()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::CAccSrvSettingsHandler - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConstructL()" );

    // Update settings in model according to CR
    TInt retval( KErrNone );
    TInt hwDevices( 0 );

    retval = RProperty::Define( KPSUidHWRM,
                                KHWRMAccessoryVibraBlocked, 
                                RProperty::EInt, 
                                KNoCapability, 
                                KWriteDeviceDataPolicy);

    if( retval != KErrAlreadyExists )
        {
        User::LeaveIfError( retval );   
        }

    CRepository* repository = CRepository::NewLC( KCRUidAccServer );
    User::LeaveIfError( repository->Get( KAccServerHWDevices, hwDevices ) );
    iModel.SetDeviceType( static_cast<TUint32>( hwDevices ) );
    CleanupStack::PopAndDestroy( repository );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSettingsHandler* CAccSrvSettingsHandler::NewL(
                                           CAccSrvConnectionController* aConCtrl,
                                           CAccSrvServerModel& aModel,
                                           CAccPolAccessoryPolicy* aPolicy )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::NewL()" );

    CAccSrvSettingsHandler* self = new( ELeave ) CAccSrvSettingsHandler(
                                                    aConCtrl,
                                                    aModel,
                                                    aPolicy );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::NewL - return self" );
    return self;
    }

// Destructor
CAccSrvSettingsHandler::~CAccSrvSettingsHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::~CAccSrvSettingsHandler()" );
    delete iLightsApi;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::~CAccSrvSettingsHandler - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::ConnectionStatusChangedL
// Accessory settings may change when connection status changes. This method
// updates settings according connection status and user given settings.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::ConnectionStatusChangedL(
    TAccPolGenericIDArray& aArray )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConnectionStatusChangedL()" );

    TAccPolGenericID genericId;
    TBool vibraAllowed( ETrue );

    // Any accessories connected
    if( aArray.Count() > 0 )
        {
        // get generic id of last connected accessory
        genericId = aArray.GetGenericIDL( 0 );
        // check if vibra is really allowed
        GetVibraAllowed( genericId, vibraAllowed );
        }

    // Get current vibra status
    TInt vibraBlocked( 0 );
    RProperty::Get( KPSUidHWRM, KHWRMAccessoryVibraBlocked, vibraBlocked );

    // Set vibra on/off, if needed
    if( !vibraAllowed && vibraBlocked == 0 )
        {
        // accessory that doesn't support vibra was connected -> set vibra of
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandlerConnectionStatusChangedL() - Vibra PS Key = VibraOff" );

        RProperty::Set( KPSUidHWRM,
                        KHWRMAccessoryVibraBlocked,
                        1 );
        }
    else if( vibraAllowed && vibraBlocked == 1 )
        {
        // accessory that doesn't support vibra was disconnected -> set vibra on
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandlerConnectionStatusChangedL() - Vibra PS Key = VibraOn" );

        RProperty::Set( KPSUidHWRM,
                        KHWRMAccessoryVibraBlocked,
                        0 );
        }

    TBool setLights( EFalse );

    // Accessory connected
    if( aArray.Count() > 0 )
        {
        // check if lights should be put on
        GetLightsAllowedL( genericId, setLights );

        // Need to set lights on
        if( setLights )
            {
            // accessory with lights on setting connected -> set lights on
            COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConnectionStatusChangedL() - Set lights on" );

            if( !iLightsApi )
                {
                iLightsApi = CHWRMLight::NewL();
                }

            iLightsApi->ReserveLightL( CHWRMLight::ESystemTarget, ETrue, ETrue );
            iLightsApi->LightOnL( CHWRMLight::ESystemTarget );

            iLightsOn = EAccSettingsLightsOn;
            }
        }
    else if( iLightsOn == EAccSettingsLightsOn )
        {
        // accessory with lights on setting disconnected -> set lights off
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConnectionStatusChangedL() - Set lights off" );

        iLightsApi->ReleaseLight( CHWRMLight::ESystemTarget );

        delete iLightsApi;
        iLightsApi = NULL;

        iLightsOn = EAccSettingsLightsOff;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::ConnectionStatusChangedL() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetSupportedHWDevicesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvSettingsHandler::GetSupportedHWDevicesL() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetSupportedHWDevicesL()" );

    TUint32 settings( 0 );

    // Read supported HW devices from file
    RFs session;
    RFile file;
    HBufC8* buf = HBufC8::NewL( KHWDevicesFileMaxSize );
    CleanupStack::PushL( buf );

    TPtr8 ptrBuf( buf->Des() );
    User::LeaveIfError( session.Connect() );
	CleanupClosePushL ( session );
    User::LeaveIfError( file.Open( session, KHWDevicesFile, EFileRead ) );
    file.Read( ptrBuf );
    file.Close();

    TLex8 l( ptrBuf );
    TInt32 temp;
    l.Val( temp );
    settings = static_cast< TUint32 >( temp );

    CleanupStack::PopAndDestroy ( &session );
    CleanupStack::PopAndDestroy ( buf );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetSupportedHWDevicesL() - return %d", settings );

    return settings;
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::SetHWDeviceSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::SetHWDeviceSettingsL( TUint32 aSettings,
                                                   TBool aEnable,
                                                   TBool aForce  ) const
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvSettingsHandler::SetHWDeviceSettingsL( %d, %d, %d  )", aSettings, aEnable, aForce );

    // Valid settings
    TUint32 settings = ResolveNewSettingsL( aSettings, aEnable, aForce );
    TUint32 iOldDeviceType = iModel.DeviceType();
    
    // Update model
    iModel.SetDeviceType( settings );

    // Update setting to CR
    CRepository* repository = CRepository::NewL( KCRUidAccServer );
    TInt crSettings = static_cast<TInt>( settings );
    repository->Set( KAccServerHWDevices, crSettings );
    delete repository;
    repository = NULL;

        //Check default selection. The newly set device-type after the model update.
        TInt defaultSelection( iModel.DeviceType() );
        TInt iReplyValue = KASNoDevice;
        TAccPolGenericID iGenericID;
       
        // get the last connected wired accessory
        if(iModel.GetLastConnectedWiredAccessory(iGenericID))
            {
            // Device Type Supplied
            if( iGenericID.DeviceTypeCaps(KDTHeadset) )
                {       
                // This is a Headset
                if( iPolicy->IsCapabilityDefinedL(iGenericID, KAccIntegratedAudioInput) ) 
                    {
                    if(KASTTY == defaultSelection)
                        {
                        iReplyValue = defaultSelection;
                        }
                    else
                        {
                        iReplyValue = KASHeadset;
                        }             
                    }
                // This is a Headphone
                else 
                    {
                    if(KASMusicStand == defaultSelection) 
                        {
                        iReplyValue = defaultSelection;
                        }
                    else
                        {
                        iReplyValue = KASHeadphones;
                        }
                    }
                }
            else if( iGenericID.DeviceTypeCaps(KDTTTY) )
                {
                if( iPolicy->IsCapabilityDefinedL(iGenericID, KAccIntegratedAudioInput) )
                    {
                    if(KASHeadset == defaultSelection)
                        {
                        iReplyValue = defaultSelection;
                        }
                    else
                        {
                        iReplyValue = KASTTY;
                        }
                    }
                }
            else if( iGenericID.DeviceTypeCaps(KDTOffice) )
                {
                if(KASHeadphones == defaultSelection)
                    {
                    iReplyValue = defaultSelection;
                    }
                else
                    {
                    iReplyValue = KASMusicStand;
                    }
                }
            // Device Type Not Supplied
            else if( iGenericID.DeviceTypeCaps() == KASNoDevice )
                {
                iReplyValue = defaultSelection;
                }

            if( (KASNoDevice != iReplyValue) && (iReplyValue != iOldDeviceType) )
                {
                //update generic id
                iPolicy->UpdateGenericIDL( iGenericID, iReplyValue);
                // Update the "iConnectionArray" of CAccSrvServerModel, 
                // to reflect the update on generic-id done earlier.
                TAccPolGenericID oldGenericId; 
                iModel.FindWithUniqueIDL( iGenericID.UniqueID(), oldGenericId );
                
                iConCtrl->HandleConnectionUpdateValidationL( 
                            iGenericID, oldGenericId, this, KErrNone );
        
                TASYCommandParamRecord asyCommandParamRecord;
                asyCommandParamRecord.iCmdValue   = 0;//Not used in update command
                asyCommandParamRecord.iGenericID  = iGenericID;     

                //Send request to ASY Proxy Handler
                COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionHandler::RunL() - Send update request");
                TInt trId = iConCtrl->HandleASYCommsL( ECmdAccessoryUpdated,
                                                    asyCommandParamRecord );
                
                iConCtrl->HandleAccessoryModeChangedL();
          
                }            
            }
       
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::SetHWDeviceSettingsL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetHWDeviceSettings
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvSettingsHandler::GetHWDeviceSettings() const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetHWDeviceSettings()" );

    TUint32 settings = iModel.DeviceType();

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::SetHWDeviceSettings() - return %d", settings );
    return settings;
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::SetAccessoryModeSettingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::SetAccessoryModeSettingL(
                                        const TAccMode aAccMode,
                                        const TAccModeSetting& aSetting ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::SetAccessoryModeSettingL()" );

    CRepository* repository = CRepository::NewL( KCRUidAccServer );
    CleanupStack::PushL( repository );

    // Static or dynamic setting
    if( StaticKey( aSetting.Id() ) )
        {
        // write INT setting to CR
        TUint32 key = static_cast<TUint32>( aSetting.Id() );
        TInt setting( 0 );
        aSetting.GetTInt( setting );
        User::LeaveIfError( repository->Set( key, setting ) );
        }
    else
        {
        // write DESCRIPTOR setting to CR
        TUint32 key = CreateCRKeyL( aAccMode, aSetting.Id() );
        HBufC* setting = CodeSettingL( aSetting );
        CleanupStack::PushL( setting );
        User::LeaveIfError( repository->Set( key, *setting ) );
        CleanupStack::PopAndDestroy( setting );
        }

    CleanupStack::PopAndDestroy( repository );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::SetAccessoryModeSettingL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetAccessoryModeSettingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::GetAccessoryModeSettingL(
                                        const TAccMode aAccMode,
                                        TAccModeSetting& aSetting ) const
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetAccessoryModeSettingL() AccMode=%d", aAccMode );

    CRepository* repository = CRepository::NewL( KCRUidAccServer );
    CleanupStack::PushL( repository );

    // static / dynamic key
    if( StaticKey( aSetting.Id() ) )
        {
        // read INT setting from CR
        TUint32 key = static_cast<TUint32>( aSetting.Id() );
        TInt setting( 0 );
        User::LeaveIfError( repository->Get( key, setting ) );
        aSetting.SetTInt( setting );
        }
    else
        {
        // read DESCRIPTOR setting from CR
        TUint32 key = CreateCRKeyL( aAccMode, aSetting.Id() );
        TBuf16< KAccSettingMaxLength > setting;
        User::LeaveIfError( repository->Get( key, setting ) );
        DecodeSettingL( setting, aSetting );
        }

    CleanupStack::PopAndDestroy( repository );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetAccessoryModeSettingL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::SetAccessoryModeSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::SetAccessoryModeSettingsL(
    const TAccMode aAccMode,
    const TAccModeSettingArray& aArray ) const
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::SetAccessoryModeSettingsL(aAccMode=%d)", aAccMode );

    for(TInt i=0; i<aArray.GetArraySize(); ++i )
        {
        TAccModeSetting setting;
        aArray.GetSetting( i, setting );
        SetAccessoryModeSettingL( aAccMode, setting );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::SetAccessoryModeSettingsL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetAccessoryModeSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::GetAccessoryModeSettingsL(
    const TAccMode aAccMode,
    TAccModeSettingArray& aArray ) const
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetAccessoryModeSettingsL(aAccMode=%d)", aAccMode );

    // Creation of partialKey and mask below has to be line with logic in
    // CAccSrvSettingsHandler::CreateCRKeyL() method.

    TUint32 mode = static_cast< TUint32 >( const_cast< TAccMode& >( aAccMode ) );
    if( mode >= KMaxAccessoryMode )
        {
        // mode value is out of range
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetAccessoryModeSettingsL() aAccMode=%d is out of range", aAccMode );
        User::Leave( KErrArgument );
        }

	// reset possible items in an array
	aArray.Reset();

	CRepository* repository = CRepository::NewL( KCRUidAccServer );
    CleanupStack::PushL( repository );
    
    // search accessory mode specific dynamic settings
    RArray< TUint32 > foundKeys;
    CleanupClosePushL( foundKeys );
    TUint32 partialKey = mode << KAccessoryModeStartBit;
    TUint32 mask( 0xfff00000 );
    repository->FindL( partialKey, mask, foundKeys );	
        	
	RArray< TUint32 > array;
	CleanupClosePushL( array );
    GetStaticKeysL( aAccMode, array );
  	TAccModeSetting setting;
    
    // get mode related static settings for client
    for ( TInt i=0; i<array.Count(); ++i )
    	{
      	TInt intSetting( 0 );
      	User::LeaveIfError( repository->Get( array[i], intSetting ) );
      	setting.SetId( array[i] );
      	setting.SetTInt( intSetting ); // sets type & value
      	aArray.AddSetting( setting );
      	}

    // get mode related dynamic settings
    for ( TInt i=0; i<foundKeys.Count(); ++i )
    	{
      	TUint32 key = foundKeys[i];
      	TBuf16< KAccSettingMaxLength > desSetting;
      	User::LeaveIfError( repository->Get( key, desSetting ) );
      	// setting id is stored key's to hexadecimal numbers marked with X:
    	// 0x000XXX00
    	TUint32 temp = key & 0x000fff00;
    	TUint32 id = temp >> KAccessoryModeSettingStartBit;
    	setting.SetId( id );
    	DecodeSettingL( desSetting, setting ); // sets type & value
    	aArray.AddSetting( setting );
    	}
	
	CleanupStack::PopAndDestroy( &array );//array.Close() is called;
	CleanupStack::PopAndDestroy( &foundKeys );//foundKeys.Close() is called;
    CleanupStack::PopAndDestroy( repository );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetAccessoryModeSettingsL() - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::ResolveNewSettingsL
// Note: This method assumes that only ONE HW DEVICE setting is ACTIVE AT THE
// TIME. If API method RAccessorySettings::SetHWDeviceSettingsL(
//                          TUint32 aSettings,
//                                                  TBool aEnable,
//                                                  TBool aForce )
//
// starts to support several active settings at the same time, this method
// has to be modified.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvSettingsHandler::ResolveNewSettingsL( TUint32 aSettings,
                                                     TBool aEnable,
                                                     TBool /*aForce*/  ) const
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSettingsHandler::ResolveNewSettingsL( %d, %d )", aSettings, aEnable );

    TUint32 settings( 0 );

    // Clear all settings
    if( aSettings == KASNoDevice || !aEnable )
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::ResolveNewSettingsL() - return %d", settings );
        return settings;
        }

    // Set new setting
    switch ( aSettings )
        {
        case KASTTY:
        case KASLoopset:
		    case KASHeadset:
		    case KASHeadphones:
		    case KASLineOut:
		    case KASTVOut:
        case KASMusicStand:
            settings = aSettings;
            break;

        default:
            // Only one setting supported at the moment
            settings = aSettings;
            break;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::ResolveNewSettingsL() - return %d", settings );
    return settings;
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetVibraAllowedL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::GetVibraAllowed( TAccPolGenericID& aId,
                                              TBool& aVibra ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetVibraAllowed()" );

    aVibra = ETrue;

    // Critical accessory (wired car kit, music stand connected )
    if ( aId.DeviceTypeCaps( KDTCarKit ) && aId.PhysicalConnectionCaps( KPCWired ) ||
        aId.DeviceTypeCaps( KDTOffice ) && aId.PhysicalConnectionCaps( KPCWired ) )
        {
        aVibra = EFalse;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetVibraAllowed() - return aVibra = %d", aVibra );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetLightsAllowedL
// Returns aLights = ETrue, if lights are set on for connected accessory.
// Returns aLights = EFalse, if lights are not set for connected accessory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::GetLightsAllowedL( TAccPolGenericID& aId,
                                                TBool& aLights ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetLightsAllowedL()" );

    TUint32 key( 0 );

    // Select CR key
    if ( aId.DeviceTypeCaps( KDTHeadset ) &&
         aId.PhysicalConnectionCaps( KPCWired ) )
        {
        key = KAccServerWiredHSLights;
        }
    else if ( aId.DeviceTypeCaps( KDTHeadset ) &&
              aId.PhysicalConnectionCaps( KPCBluetooth ) )
        {
        key = KAccServerWirelessHSLights;
        }
    else if ( aId.DeviceTypeCaps( KDTCarKit ) &&
              aId.PhysicalConnectionCaps( KPCWired ) )
        {
        key = KAccServerWiredCarKitLights;
        }
    else if ( aId.DeviceTypeCaps( KDTCarKit ) &&
              aId.PhysicalConnectionCaps( KPCBluetooth ) )
        {
        key = KAccServerWirelessCarKitLights;
        }
    else if ( aId.DeviceTypeCaps( KDTTTY ) &&
              aId.PhysicalConnectionCaps( KPCWired ) )
        {
        key = KAccServerTextDeviceLights;
        }
    else if ( aId.DeviceTypeCaps( KDTLoopset ) &&
              aId.PhysicalConnectionCaps( KPCWired ) )
        {
        key = KAccServerLoopsetLights;
        }
    else if ( aId.DeviceTypeCaps( KDTOffice ) &&
              aId.PhysicalConnectionCaps( KPCWired ) )
        {
        key = KAccServerMusicStandLights;
        }

    // Get light setting from CR
    TInt setLights( 0 );

    if ( key )
        {
        // If key is selected, get lights status from CenRep        
        CRepository* repository = CRepository::NewL( KCRUidAccServer );
        CleanupStack::PushL( repository );
        User::LeaveIfError( repository->Get( key, setLights ) );
        CleanupStack::PopAndDestroy( repository );
        }
    else
        {
        // Key not set, no need to get value from CenRep (it'll leave anyway)
        // This is the case at least with AV_Connector (TV-Out)
        }

    aLights = static_cast<TBool>( setLights );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetLightsAllowedL() - return aLights = %d", aLights );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::CreateCRKeyL
//
// CR key has to be uniq TUint32 value. It's used to set/get a single accessory
// setting.

// Key value range from 0x00100000 to 0xFFFFFF00 has been reserved for any
// accessory related setting that doesn't have influence to Accessory Server
// behaviour but needs to be stored permanently.
//
// Key value created by this method consists as follows:
//
// 1. accessory mode is stored to hexadecimal numbers marked with X: 0xXXX00000
// 2. setting id is stored to hexadecimal numbers marked with X: 0x000XXX00
//
// Thus, Accessory Server can store dynamically 4096 accessory modes and for
// each mode 4096 settings.
//
// (2 least significant hexadecimal numbers have been reserved for keys that
// have an influence to Accessory Server behaviour like KAccServerHWDevices
// key. Thus, Accessory Server can store 256 settings this kind of settings.)
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CAccSrvSettingsHandler::CreateCRKeyL(
    const TAccMode aAccMode,
    const TInt aId ) const
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSettingsHandler::CreateCRKeyL(%d,%d)", aAccMode, aId );

    TUint32 key( 0 );

    // create first part of key from accessory mode
    TUint32 mode = static_cast< TUint32 >( const_cast< TAccMode& >( aAccMode ) );
    if( mode >= KMaxAccessoryMode )
        {
        // mode value is out of range
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::CreateCRKeyL() aAccMode=%d is out of range", aAccMode );
        User::Leave( KErrArgument );
        }

    TUint32 keyPart1 = mode << KAccessoryModeStartBit;

    // Create second part of key from id
    TUint32 id = static_cast< TUint32 >( const_cast< TInt& >( aId ) );
    if( id >= KMaxAccessorySetting )
        {
        // id value is out of range
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::CreateCRKeyL() aId=%d is out of range", aId );
        User::Leave( KErrArgument );
        }

    TUint32 keyPart2 = id << KAccessoryModeSettingStartBit;

    key = keyPart1 + keyPart2;
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::CreateCRKeyL() returns key=%d", key );

    return key;
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::CodeSettingL
//
// This method is used to convert setting in client format to (TAccModeSetting)
// dynamic format that used to store setting to CR.
//
// Dynamic settings are stored to descriptor that has following structure:
// xxx:yyy where
//
//  xxx tells variable type of a setting. Possible values are "des" for
//  descriptor, "int" for integer and "boo" for boolean. Variable is stored to
//  3 characters.
//
//  ":" separates variable type information from it's value. Separation character
//  takes one character.
//
//  yyy stores value of a setting. For setting has been reserved 256 characters.
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CAccSrvSettingsHandler::CodeSettingL( const TAccModeSetting& aSetting ) const
    {
    HBufC* data = HBufC::NewL( KAccSettingMaxLength );

    // store variable type and value to data
    switch ( aSetting.Type() )
        {
        case EAccModeSettingInt:
            {
            TInt value( 0 );
            aSetting.GetTInt( value );
            TBuf< KAccSettingMaxLength > temp;
            temp.Num( value );
            data->Des().Copy( KAccSettingInteger );
            data->Des().Append( temp );
            }
            break;

        case EAccModeSettingBool:
            {
            TBool value( EFalse );
            aSetting.GetTBool( value );
            TBuf< 1 > temp;
            if ( value )
                {
                _LIT( KTrue, "1" );
                temp.Append( KTrue );
                }
            else
                {
                _LIT( KFalse, "0" );
                temp.Append( KFalse );
                }
            data->Des().Copy( KAccSettingBoolean );
            data->Des().Append( temp );
            }
            break;

        case EAccModeSettingDes:
            {
            TBuf< KAccSettingMaxLength > value;
            aSetting.GetTDes( value );
            data->Des().Copy( KAccSettingDescriptor );
            data->Des().Append( value );
            }
            break;

        default:
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::CodeSettingL() value was not set !" );
            delete data;
            data = NULL;
            User::Leave( KErrNotFound );
            }
            break;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler:.CodeSettingL() returns %d", data );
    return data;
  }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::DecodeSettingL
//
// This method is used to convert dynamically stored setting to format
// (TAccModeSetting) that client understands.
//
// Dynamic settings are stored to descriptor that has following structure:
// xxx:yyy where
//
//  xxx tells variable type of a setting. Possible values are "des" for
//  descriptor, "int" for integer and "boo" for boolean. Variable is stored to
//  3 characters.
//
//  ":" separates variable type information from it's value. Separation character
//  takes one character.
//
//  yyy stores value of a setting. For setting has been reserved 256 characters.
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::DecodeSettingL( const TDesC& aIn, TAccModeSetting& aOut ) const
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::DecodeSettingL()" );

    TAccModeSettingType type( EAccModeSettingNotSet );

    // check variable type
    if( aIn.Find( KAccSettingInteger ) == 0 )
        {
        type = EAccModeSettingInt;
        }
    else if( aIn.Find( KAccSettingBoolean ) == 0 )
        {
        type = EAccModeSettingBool;
        }
    else if( aIn.Find( KAccSettingDescriptor ) == 0 )
        {
        type = EAccModeSettingDes;
        }

    // get data out
    HBufC* data = HBufC::NewL( aIn.Length() - KAccSettingIdLength );
    data->Des().Copy( aIn.Mid( KAccSettingIdLength ) );

    // store value for client
    switch ( type )
        {
        case EAccModeSettingInt:
            {
            TLex temp( *data );
            TInt value( 0 );
            temp.Val( value );
            aOut.SetTInt( value );
            }
            break;

        case EAccModeSettingBool:
            {
            TLex temp( *data );
            TInt value( 0 );
            temp.Val( value );
            aOut.SetTBool( static_cast< TBool >( value ) );
            }
            break;

        case EAccModeSettingDes:
            {
            aOut.SetTDes( *data );
            }
            break;

        default:
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::DecodeSettingL() value was not set !" );
            delete data;
            User::Leave( KErrNotFound );
            }
            break;
        }

    delete data;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::DecodeSettingL() return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::StaticKey
//
// Returns ETrue, if id is static key. Settings stored with static key have some
// influence to Accessory Server's behaviour.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CAccSrvSettingsHandler::StaticKey( const TInt id ) const
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::StaticKey(%d)", id );

    TBool identified( EFalse );

    switch ( id )
        {
        case KAccServerWiredHSLights:
        case KAccServerWirelessHSLights:
        case KAccServerWiredCarKitLights:
        case KAccServerWirelessCarKitLights:
        case KAccServerTextDeviceLights:
        case KAccServerLoopsetLights:
        case KAccServerMusicStandLights:
        case KAccServerSyncStandLights:
            identified = ETrue;
            break;

        default:
            break;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::StaticKey() returns %d", identified );
    return identified;
    }

// -----------------------------------------------------------------------------
// CAccSrvSettingsHandler::GetStaticKeysL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSettingsHandler::GetStaticKeysL(
                    const TAccMode aAccMode,
                    RArray< TUint32 >& aArray ) const
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSettingsHandler::GetStaticKeysL(%d)", aAccMode );

    switch( aAccMode )
        {
        case EAccModeWiredHeadset:
            aArray.AppendL( KAccServerWiredHSLights );
            break;
        case EAccModeWirelessHeadset:
            aArray.AppendL( KAccServerWirelessHSLights );
            break;
        case EAccModeWiredCarKit:
            aArray.AppendL( KAccServerWiredCarKitLights );
            break;
        case EAccModeWirelessCarKit:
            aArray.AppendL( KAccServerWirelessCarKitLights );
            break;
        case EAccModeTextDevice:
            aArray.AppendL( KAccServerTextDeviceLights );
            break;
        case EAccModeLoopset:
            aArray.AppendL( KAccServerLoopsetLights );
            break;
        case EAccModeMusicStand:
            aArray.AppendL( KAccServerMusicStandLights );
            break;
        default:  // mode is not supported
            break;
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSettingsHandler::GetStaticKeysL() returns" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
