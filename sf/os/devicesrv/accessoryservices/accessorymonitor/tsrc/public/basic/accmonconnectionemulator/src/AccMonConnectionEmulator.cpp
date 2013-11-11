/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main functionality of AccMonitor. Constructs the Accessory
*          Monitor implementation class
*
*/


#include <accessoryserver.h>
#include <AccessoryBTControl.h>
#include <e32uid.h>
#include <btsdp.h>
#include <AccMonitorCapabilities.h>
#include <AccMonitor.h>
#include <AccMonitorInfo.h>
#include <e32property.h>
#include "acc_debug.h"
#include "accmonconnectionemulator.h"
#include "AccMonConnetionConsts.h"
#include "AsyConst.h"

#define TEST_BT

// ---------------------------------------------------------------------------
// CConnectionEmulator::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CConnectionEmulator* CConnectionEmulator::NewL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::NewL()" );
    
    CConnectionEmulator* impl = CConnectionEmulator::NewLC();
    CleanupStack::Pop( impl );
    
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::NewL() - return" );
    return impl;
    }

// ---------------------------------------------------------------------------
// CConnectionEmulator::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
EXPORT_C CConnectionEmulator* CConnectionEmulator::NewLC()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::NewLC()" );
    
    CConnectionEmulator* self = new (ELeave) CConnectionEmulator();
    CleanupStack::PushL( self );
    
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::NewLC() - return" );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor.
// ---------------------------------------------------------------------------
CConnectionEmulator::CConnectionEmulator()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::CConnectionEmulator()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::CConnectionEmulator() - return" );
    }

// ---------------------------------------------------------------------------
// C++ default destructor.
// ---------------------------------------------------------------------------
CConnectionEmulator::~CConnectionEmulator()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::~CConnectionEmulator()" );
    iSubscribe.Delete( KTFConnectionEmulatorTestProperty, KTFConnectionEmulatorMethod );
    iSubscribe.Close();
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::~CConnectionEmulator() - return" );
    }

// ---------------------------------------------------------------------------
// CConnectionEmulator::ConnectAccessory
// 
// Connects an accessory of some type.
// ---------------------------------------------------------------------------
EXPORT_C TInt CConnectionEmulator::ConnectAccessoryL(
    TEnumAccConnectType aAccDefintion,
    TBool aSync )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::ConnectAccessory()" );
    
    TInt err( KErrNone );
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
    
    // Reset the values and then order the connect
    TTFASYReference method( ETFAsyNone );
    TInt64 param1( 0 );
    TBool wired( EFalse );
    TUint64 btaddress( 0 );
    RArray< TUint32 > capabilityArray;
    TUint8 deviceType;
    
    switch( aAccDefintion )
        {
          case EWiredHeadSetCon:
              {
              method = ETFAsyConnectHeadset;
              wired = ETrue;
              break;
              }
          case EWirelessHeadSetCon:
              {
#ifdef TEST_BT
              method = ETFBTConnectHeadset;
              btaddress = KBTAddrHeadsetHSP;
              deviceType = 0x1;
#else
              err = KErrHardwareNotAvailable;
#endif
              break;
              }
          case EWiredCarKitCon:
              {
              method = ETFAsyConnectECICarKit;
              wired = ETrue;
              break;
              }
          case EWirelessCarKitCon:
              {
#ifdef TEST_BT
              method = ETFBTConnectCarKit;
              btaddress = KBTAddrCarkit;
              deviceType = 0x2;
#else
              err = KErrHardwareNotAvailable;
#endif
              break;
              }
          case EMusicStandCon:
              {
              method = ETFAsyConnectECIMusicStand;
              wired = ETrue;
              break;
              }
          case ETVOutCon:
              {
              method = ETFAsyConnectECITVOut;
              wired = ETrue;
              break;
              }
          case E3PoleOpenCable:
              {
              method = ETFAsyConnectOpenCable3Poles;
              wired = ETrue;
              break;
              }
          default:
              {
              User::Leave( KErrGeneral );
              break;
              }
        }
  
    // Set the struct to the right order
    buf().iMethod     = method;
    buf().iParam1     = param1;
    buf().iCapsArray  = capabilityArray;
    if( wired )
        {
        if( aSync )
            {
            TInt some = SetPubSubL();
            //RProperty subscribe;        
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            //iStatus = KRequestPending;
            //User::WaitForRequest( iStatus );
            //iSubscribe.Cancel();            
            //iSubscribe.Close();
            }
        else
            {            
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            }
        }
    // A BT connect
    else
        {
#ifdef TEST_BT
        TBTDevAddr btaddr( btaddress );
        TRequestStatus status;
        RAccessoryServer server;
        server.Connect();
        RAccessoryBTControl accessoryBTControl;
        accessoryBTControl.CreateSubSession( server );
        accessoryBTControl.ConnectAccessory( iStatus, btaddr );
        iStatus = KRequestPending;
        User::WaitForRequest( status );
        accessoryBTControl.CloseSubSession();
        server.Disconnect();
#endif
        }
    
    User::After( 1000000 );
    
    COM_TRACE_1( "[AccFW:AccMonitor] CConnectionEmulator::ConnectAccessory() - return err %d", err );
    
    capabilityArray.Close();
    
    return err;
    }

// ---------------------------------------------------------------------------
// CConnectionEmulator::ConnectAccessory
// 
// Connects an accessory with some defined capabilities.
// ---------------------------------------------------------------------------
EXPORT_C TInt CConnectionEmulator::ConnectAccessoryL(
     RAccMonCapabilityArray& aCapabilityArray,
     TBool aSync )
    {
    COM_TRACE_( "[CConnectionEmulator] CConnectionEmulator::ConnectAccessory() - with array" );
    
    TInt err( KErrNone );
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
    
    // Reset the values and then order the connect
    TTFASYReference method;
    TInt64 param1( 0 );
    TBool wired( EFalse );
    TUint64 btaddress;
    method = ETFAsyConnectArrayOfCapabilities;
  
    aCapabilityArray.SortUnsigned();
    for( TInt i = 0; i != aCapabilityArray.Count(); i++ )
        {
        COM_TRACE_1( "[CConnectionEmulator] CConnectionEmulator::ConnectAccessory() - In array capability: %d", aCapabilityArray[ i ] );
        }
    if( aCapabilityArray.Find( KAccMonWired ) >= 0 )
        {
        wired = ETrue;
        }
    
    // Set the struct to the right order
    buf().iMethod = method;
    buf().iParam1 = param1;
    buf().iCapsArray = aCapabilityArray;
    if( wired )
        {
        if( aSync )
            {
            TInt some = SetPubSubL();
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            }
        else
            {            
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            }
        }
    else
        {
#ifdef TEST_BT
        switch( aCapabilityArray[ 0 ] )
            {
            case KAccMonHeadset:
                {
                btaddress = KBTAddrHeadsetHSP;
                break;
                }
            case KAccMonCarKit:
                {
                btaddress = KBTAddrHeadsetHSP;
                break;
                }
            default:
                {
                err = KErrNotFound;
                break;
                }
            }
        if( err == KErrNone )
            {
            TRequestStatus status;
            RAccessoryServer server;
            server.Connect();
            RAccessoryBTControl accessoryBTControl;
            accessoryBTControl.CreateSubSession( server );
            TBTDevAddr btaddr( btaddress );
            accessoryBTControl.ConnectAccessory( iStatus, btaddr );
            iStatus = KRequestPending;
            User::WaitForRequest( status );
            accessoryBTControl.CloseSubSession();
            server.Disconnect();
            }
#else
         err = KErrHardwareNotAvailable;
#endif
         }
    User::After( 1000000 ); 
    
    COM_TRACE_1( "[AccFW:AccMonitor] CConnectionEmulator::ConnectAccessory() - return err %d", err );
    
    return err;
    }

// ---------------------------------------------------------------------------
// CConnectionEmulator::DisconnectAccessory
// 
// Disconnects an accessory of some type.
// ---------------------------------------------------------------------------
EXPORT_C TInt CConnectionEmulator::DisconnectAccessoryL( 
    TEnumAccConnectType aAccDefintion,
    TBool aSync )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::DisconnectAccessory()" );
    TInt err( KErrNone );
    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
    
    // Reset the values and then order the connect
    TTFASYReference method( ETFAsyNone );
    TInt64 param1( 0 );
    TBool wired( EFalse );
    TUint64 btaddress( 0 );
    RArray< TUint32 > capabilityArray;
    
    switch( aAccDefintion )
        {
            case EWiredHeadSetCon:
                {
                method = ETFAsyDisc;
                wired = ETrue;
                break;
                }
            case EWirelessHeadSetCon:
            {
#ifdef TEST_BT
                method = ETFBTConnectHeadset;
                btaddress = KBTAddrHeadsetHSP;
#else
                err = KErrHardwareNotAvailable;
#endif
                break;
            }
            case EWiredCarKitCon:
                {
                method = ETFAsyDisc;
                wired = ETrue;
                break;
                }
          case EWirelessCarKitCon:
            {
#ifdef TEST_BT
                method = ETFBTConnectCarKit;
                btaddress = KBTAddrCarkit;
#else
                err = KErrHardwareNotAvailable;
#endif
                break;
            }
          case EMusicStandCon:
                {
                wired = ETrue;
                method = ETFAsyDisc;
                break;
                }
          case ETVOutCon:
                {
                wired = ETrue;
                method = ETFAsyDisc;
                break;
                }
          default:
                {
                User::Leave( KErrGeneral );
                break;
                }
        }
  
    // Set the struct to the right order
    buf().iMethod = method;
    buf().iParam1 = param1;
    buf().iCapsArray = capabilityArray;
    
    if( wired )
        {  
        if( aSync )
            {
            TInt some = SetPubSubL();            
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            //iStatus = KRequestPending;
            //User::WaitForRequest( iStatus );
            //iSubscribe.Cancel();
            //iSubscribe.Delete( KTFConnectionEmulatorTestProperty, KTFConnectionEmulatorMethod );
            //iSubscribe.Close();
            }
        else
            {            
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            }
        }
    
    // A BT disconnect
    else
        {
#ifdef TEST_BT
        TRequestStatus status;
        RAccessoryServer server;
        server.Connect();
        RAccessoryBTControl accessoryBTControl;
        accessoryBTControl.CreateSubSession( server );
        TBTDevAddr btaddr( btaddress );
        accessoryBTControl.DisconnectAccessory( iStatus, btaddr );
        iStatus = KRequestPending;
        User::WaitForRequest( iStatus );
        accessoryBTControl.CloseSubSession();
        server.Disconnect();
#endif
        }
    User::After( 1000000 );
    COM_TRACE_1( "[AccFW:AccMonitor] CConnectionEmulator::DisconnectAccessory() - return err %d", err );
    
    capabilityArray.Close();
    
    return err;
    }

// ---------------------------------------------------------------------------
// CConnectionEmulator::DisconnectAccessory
// 
// Connects an accessory of some accessory that is connected.
// ---------------------------------------------------------------------------
EXPORT_C TInt CConnectionEmulator::DisconnectAccessoryL( 
        CAccMonitorInfo* aAccInfo,
        TBool aSync )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::DisconnectAccessory() - return err " );
    
    TInt err( KErrNone );
    
    if( aAccInfo->AccPhysicalConnection() == KAccMonWired )
        {
        TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
        
        // Reset the values and then order the connect
        TTFASYReference method;
        TInt64 param1( 0 );
        RArray< TUint32 > capabilityArray;
        
        method = ETFAsyDisc;
        
        // Set the struct to the right order
        buf().iMethod     = method;
        buf().iParam1     = param1;
        buf().iCapsArray  = capabilityArray;
        if( aSync )
            {
            TInt some = SetPubSubL();            
            User::LeaveIfError( iSubscribe.Set( KTFAccessoryTestProperty, KTFAccessoryMethod, buf ) );
            /*iStatus = KRequestPending;
            User::WaitForRequest( iStatus );
            iSubscribe.Cancel();
            iSubscribe.Delete( KTFConnectionEmulatorTestProperty, KTFConnectionEmulatorMethod );
            iSubscribe.Close();
            */
            }
        }
    // A BT disconnect
    else
        {
#ifdef TEST_BT
        TUint64 btaddress = aAccInfo->AccDeviceAddress();
        TRequestStatus status;
        RAccessoryServer server;
        server.Connect();
        RAccessoryBTControl accessoryBTControl;
        accessoryBTControl.CreateSubSession( server );
        TBTDevAddr btaddr( btaddress );
        accessoryBTControl.DisconnectAccessory( iStatus, btaddr );
        iStatus = KRequestPending;
        User::WaitForRequest( iStatus );
        accessoryBTControl.CloseSubSession();
        server.Disconnect();
#else 
        err = KErrHardwareNotAvailable;
#endif
        }
    
    User::After( 1000000 );
    
    COM_TRACE_1( "[AccFW:AccMonitor] CConnectionEmulator::DisconnectAccessory() - return err %d", err );
    
    return err;
    }
  
  
TInt CConnectionEmulator::SetPubSubL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CConnectionEmulator::SetPubSubL()" );
    TInt retval(KErrNone);
    retval = iSubscribe.Define( KTFConnectionEmulatorTestProperty, KTFConnectionEmulatorMethod, RProperty::EByteArray );
    if(retval != KErrAlreadyExists)
        {
        User::LeaveIfError( retval );
        }    
    //retval =  iSubscribe.Attach( KTFConnectionEmulatorTestProperty, KTFConnectionEmulatorMethod, EOwnerThread );
    //iSubscribe.Subscribe( iStatus );
    COM_TRACE_1( "[AccFW:AccMonitor] CConnectionEmulator::SetPubSubL() - return retval %d", retval );
  
    return retval;
    }
  
// End of file

