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
* Description:  Sub session for accessory related settings.
*
*/


// INCLUDE FILES

#include "AccSrvsubSettings.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "AccSrvSettingsHandler.h"
#include "acc_debug.h"
#include "AccPolAccessoryPolicy.h"
#include <AccModeSetting.h>

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
// CAccSrvSubSettings::CAccSrvSubSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubSettings::CAccSrvSubSettings( CASYProxyAPI* aProxy,
                                        CAccSrvConnectionController* aConnectionController,
                                        CAccSrvServerModel* aServerModel )
    : CAccSrvSubBase( aProxy, aConnectionController, aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::CAccSrvSubSettings()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubSettings* CAccSrvSubSettings::NewL( CASYProxyAPI* aProxy,
                                              CAccSrvConnectionController* aConnectionController,
                                              CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::NewL()" );

    if( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubSettings* self = new (ELeave) CAccSrvSubSettings( aProxy,
                                                                aConnectionController,
                                                                aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::NewL - return self" );

    return self;
    }

// Destructor
CAccSrvSubSettings::~CAccSrvSubSettings()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::~CAccSrvSubSettings()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::DispatchMessageL
// Handles client request
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvGetSupportedHWDevices:
            {
            GetSupportedHWDevicesL( aMessage );
            break;
            }
        case EAccSrvGetHWDeviceSettings:
            {
            GetHWDeviceSettingsL( aMessage );
            break;
            }
        case EAccSrvSetHWDeviceSettings:
            {
            SetHWDeviceSettingsL( aMessage );
            break;
            }
        case EAccSrvGetAccessoryModeSetting:
            {
            GetAccessoryModeSettingL( aMessage );
            break;
            }
        case EAccSrvSetAccessoryModeSetting:
            {
            SetAccessoryModeSettingL( aMessage );
            break;
            }
        case EAccSrvGetAccessoryModeSettings:
            {
            GetAccessoryModeSettingsL( aMessage );
            break;
            }
        case EAccSrvSetAccessoryModeSettings:
            {
            SetAccessoryModeSettingsL( aMessage );
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubSettings::DispatchMessageL - unhandled function %d", function );
            User::Leave( KErrGeneral );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::GetSupportedHWDevicesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::GetSupportedHWDevicesL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetSupportedHWDevicesL()" );
    TUint32 supportedDevices( 0 );

    supportedDevices = iServerModel->SupportedDevicesL();

    // Write settings to client request
    TPckgBuf<TUint32> TPckgSettings( supportedDevices );
    aMessage.WriteL( KAccServParamPosFirst, TPckgSettings );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetSupportedHWDevicesL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::SetHWDeviceSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::SetHWDeviceSettingsL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetHWDeviceSettingsL()" );

    TUint32 settings( 0 );
    TBool enable( EFalse );
    TBool force( EFalse );
    TPckgBuf<TUint32> TPckgSettings( settings );
    TPckgBuf<TBool> TPckgEnable( enable );
    TPckgBuf<TBool> TPckgForce( force );

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, TPckgSettings );
    aMessage.ReadL( KAccServParamPosSecond, TPckgEnable );
    aMessage.ReadL( KAccServParamPosThird, TPckgForce );
    settings = TPckgSettings();
    enable = TPckgEnable();
    force = TPckgForce();

    // Update Model
    iConnectionController->SettingsHandler().SetHWDeviceSettingsL(
                                                  settings,
                                                  enable,
                                                  force );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetHWDeviceSettingsL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::GetHWDeviceSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::GetHWDeviceSettingsL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetHWDeviceSettingsL()" );

    // Get settings
    TUint32 settings = iConnectionController->
                            SettingsHandler().GetHWDeviceSettings();

    // Write settings to client request
    TPckgBuf<TUint32> TPckgSettings( settings );
    aMessage.WriteL( KAccServParamPosFirst, TPckgSettings );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetHWDeviceSettingsL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::SetAccessoryModeSettingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 void CAccSrvSubSettings::SetAccessoryModeSettingL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetAccessoryModeSettingL()" );

    TAccMode mode( EAccModeHandPortable );
    TPckgBuf<TAccMode> TPckgMode( mode );
    TAccModeSetting setting;
    TPckgBuf<TAccModeSetting> TPckgSetting( setting );

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, TPckgMode );
    aMessage.ReadL( KAccServParamPosSecond, TPckgSetting );
    mode = TPckgMode();
    setting = TPckgSetting();

    // Update Model
    iConnectionController->
        SettingsHandler().SetAccessoryModeSettingL( mode, setting );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetAccessoryModeSettingL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::GetAccessoryModeSettingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::GetAccessoryModeSettingL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetAccessoryModeSettingL()" );

    TAccMode mode( EAccModeHandPortable );
    TPckgBuf<TAccMode> TPckgMode( mode );
    TAccModeSetting setting;
    TPckgBuf<TAccModeSetting> TPckgSettingIn( setting );

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, TPckgMode );
    aMessage.ReadL( KAccServParamPosSecond, TPckgSettingIn );
    mode = TPckgMode();
    setting = TPckgSettingIn();

    // Get setting
    iConnectionController->
        SettingsHandler().GetAccessoryModeSettingL( mode, setting );

    // Write settings to client request
    TPckgBuf<TAccModeSetting> TPckgSettingOut( setting );
    aMessage.WriteL( KAccServParamPosSecond, TPckgSettingOut );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetAccessoryModeSettingL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::SetAccessoryModeSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::SetAccessoryModeSettingsL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetAccessoryModeSettingsL()" );

    TAccMode mode( EAccModeHandPortable );
    TPckgBuf<TAccMode> TPckgMode( mode );
    TAccModeSettingArray array;
    TPckgBuf<TAccModeSettingArray> TPckgSettingArray( array );


    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, TPckgMode );
    aMessage.ReadL( KAccServParamPosSecond, TPckgSettingArray );
    mode = TPckgMode();
    array = TPckgSettingArray();

    // Update Model
    iConnectionController->
        SettingsHandler().SetAccessoryModeSettingsL( mode, array );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::SetAccessoryModeSettingsL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubSettings::GetAccessoryModeSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvSubSettings::GetAccessoryModeSettingsL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetAccessoryModeSettingsL()" );

    TAccMode mode( EAccModeHandPortable );
    TPckgBuf<TAccMode> TPckgMode( mode );
    TAccModeSettingArray array;
    TPckgBuf<TAccModeSettingArray> TPckgSettingArrayIn( array );

    // Read client parameters
    aMessage.ReadL( KAccServParamPosFirst, TPckgMode );
    aMessage.ReadL( KAccServParamPosSecond, TPckgSettingArrayIn );
    mode = TPckgMode();
    array = TPckgSettingArrayIn();

    // Get setting
    iConnectionController->
        SettingsHandler().GetAccessoryModeSettingsL( mode, array );

    // Write settings to client request
    TPckgBuf<TAccModeSettingArray> TPckgSettingArrayOut( array );
    aMessage.WriteL( KAccServParamPosSecond, TPckgSettingArrayOut );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubSettings::GetAccessoryModeSettingsL - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
