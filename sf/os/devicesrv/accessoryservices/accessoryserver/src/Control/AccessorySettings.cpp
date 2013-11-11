/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client API used to set and get accessory related settings.
*
*/


// INCLUDE FILES
#include "AccessoryServer.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include <AccessorySettings.h>

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
// RAccessorySettings::RAccessorySettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessorySettings::RAccessorySettings()
    {
    API_TRACE_( "[AccFW:Client] RAccessorySettings::RAccessorySettings" );
    }

// -----------------------------------------------------------------------------
// RAccessorySettings::CreateSubSession
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::CreateSubSession(RAccessoryServer& aSession)
    {
    API_TRACE_( "[AccFW:Client] RAccessorySettings::CreateSubSession()" );

    TInt err = RSubSessionBase::CreateSubSession( aSession,
                                                  EAccSrvCreateSubSessionAccessorySettings,
                                                  TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_1( "[AccFW:Client] RAccessorySettings::CreateSubSession - return %x0x", err );

  return err;
    }

// -----------------------------------------------------------------------------
// RAccessorySettings::CloseSubSession
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::CloseSubSession()
    {
    API_TRACE_( "[AccFW:Client] RAccessorySettings::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessorySettings );

    API_TRACE_( "[AccFW:Client] RAccessorySettings::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessorySettings::GetSupportedHWDeviceSettings
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::GetSupportedHWDeviceSettings( TUint32& aSettings )
    {
    API_TRACE_1( "[AccFW:Client] RAccessorySettings::SupportedHWDeviceSettings(aSettings=%d)", aSettings );

  TPckg<TUint32> settings( aSettings );

  TInt err = SendReceive( EAccSrvGetSupportedHWDevices, TIpcArgs( &settings ) );

  API_TRACE_2( "[AccFW:Client] RAccessorySettings::SupportedHWDeviceSettings() return aSettings=%d, err=%d", aSettings, err );

  return err;
    }

// -----------------------------------------------------------------------------
// RAccessorySettings::SetHWDeviceSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::SetHWDeviceSettingsL( TUint32 aSettings,
                                                        TBool aEnable,
                                                        TBool aForce )
  {
  API_TRACE_3( "[AccFW:Client] RAccessorySettings::SetHWDeviceSettingsL(aSettings=%d, aEnable=%d, aForce=%d )", aSettings, aEnable, aForce );

  TPckgBuf<TUint32> settings( aSettings );
  TPckgBuf<TBool> enable( aEnable );
  TPckgBuf<TBool> force( aForce );

  TInt err = SendReceive( EAccSrvSetHWDeviceSettings,
              TIpcArgs( &settings, &enable, &force ) );

    if( ( err != KErrNone ) && ( err != KErrArgument  ) )
      {
      API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetHWDeviceSettingsL() leaves %d", err );
      User::Leave( err );
      }

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetHWDeviceSettingsL() return %d", err );

  return err;
  }

// -----------------------------------------------------------------------------
// RAccessorySettings::GetHWDeviceSettings
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::GetHWDeviceSettings( TUint32& aSettings ) const
  {
  API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetHWDeviceSettings(aSettings=%d)", aSettings );

  TPckg<TUint32> settings( aSettings );

  TInt err = SendReceive( EAccSrvGetHWDeviceSettings, TIpcArgs( &settings ) );

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetHWDeviceSettings() return %d", err );

  return err;
  }

// -----------------------------------------------------------------------------
// RAccessorySettings::SetAccessoryModeSettingL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::SetAccessoryModeSettingL(
                                                   const TAccMode aAccMode,
                                                   TAccModeSetting& aSetting )
  {
  API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingL(mode=%d)", aAccMode );

  TPckgBuf<TAccMode> mode( aAccMode );
  TPckgBuf<TAccModeSetting> setting( aSetting );

  TInt err = SendReceive( EAccSrvSetAccessoryModeSetting,
              TIpcArgs( &mode, &setting ) );

  if( err != KErrNone && err != KErrArgument )
      {
      API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingL() leaves %d", err );
      User::Leave( err );
      }

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingL() return %d", err );

  return err;
  }

// -----------------------------------------------------------------------------
// RAccessorySettings::SetAccessoryModeSettingsL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::SetAccessoryModeSettingsL(
                                              const TAccMode aAccMode,
                                              TAccModeSettingArray& aArray )
    {
  API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingsL(mode=%d)", aAccMode );

  TPckgBuf<TAccMode> mode( aAccMode );
  TPckgBuf<TAccModeSettingArray> settings( aArray );

  TInt err = SendReceive( EAccSrvSetAccessoryModeSettings,
              TIpcArgs( &mode, &settings ) );

  if( err != KErrNone && err != KErrArgument )
      {
      API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingsL() leaves %d", err );
      User::Leave( err );
      }

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::SetAccessoryModeSettingsL() return %d", err );

  return err;
  }

// -----------------------------------------------------------------------------
// RAccessorySettings::GetAccessoryModeSetting
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::GetAccessoryModeSetting(
                                              const TAccMode aAccMode,
                                              TAccModeSetting& aSetting ) const
    {
    API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetAccessoryModeSetting(mode=%d)", aAccMode );

  TPckgBuf<TAccMode> mode( aAccMode );
  TPckgBuf<TAccModeSetting> setting( aSetting );

  TInt err = SendReceive( EAccSrvGetAccessoryModeSetting,
              TIpcArgs( &mode, &setting ) );

  aSetting = setting();

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetAccessoryModeSetting() return %d", err );

  return err;
  }

// -----------------------------------------------------------------------------
// RAccessorySettings::GetAccessoryModeSettings
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessorySettings::GetAccessoryModeSettings(
                                           const TAccMode aAccMode,
                                           TAccModeSettingArray& aArray ) const
    {
    API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetAccessoryModeSettings(mode=%d)", aAccMode );

  TPckgBuf<TAccMode> mode( aAccMode );
  TPckgBuf<TAccModeSettingArray> settings( aArray );

  TInt err = SendReceive( EAccSrvGetAccessoryModeSettings,
              TIpcArgs( &mode, &settings ) );

  aArray = settings();

  API_TRACE_1( "[AccFW:Client] RAccessorySettings::GetAccessoryModeSettings() return %d", err );

    return err;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
//  End of File







