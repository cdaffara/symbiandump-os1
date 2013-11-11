/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Public, Accessory Policy Generic ID
*
*/


// INCLUDE FILES
#include <AccPolGenericID.h>
#include <e32std.h>
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
// TAccPolGenericID::TAccPolGenericID
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolGenericID::TAccPolGenericID()
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::TAccPolGenericID()" );

    iStaticAttributes.iAccessoryDeviceType = 0;
    iStaticAttributes.iPhysicalConnection = 0;
    iStaticAttributes.iApplicationProtocol = 0;
    iStaticAttributes.iCapabilitiesSubblocks = 0;
    iStaticAttributes.iHWModelID = KNullDesC;
    iStaticAttributes.iHWDeviceID = 0;
    iStaticAttributes.iDeviceAddress = 0;
    iStaticAttributes.iDBID = KErrNotFound;
   
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::TAccPolGenericID() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::DeviceTypeCapsL
// Tells whether the device type(s) given as parameter are suppported by 
// the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TAccPolGenericID::DeviceTypeCaps( 
    const TUint32 aDeviceTypeBitMask ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::DeviceTypeCapsL() 0x%x", aDeviceTypeBitMask );

    TUint32 result( iStaticAttributes.iAccessoryDeviceType );
    result = result & aDeviceTypeBitMask;
    
    API_TRACE_1( "[ACCPOLICY] TAccPolGenericID::DeviceTypeCapsL() - return %d", 
               ( result == aDeviceTypeBitMask ) );
               
    return ( result == aDeviceTypeBitMask );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::DeviceTypeCaps
// Returns the device type(s) of the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TAccPolGenericID::DeviceTypeCaps() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::DeviceTypeCaps()" );
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::DeviceTypeCaps() - return 0x%x", 
                  iStaticAttributes.iAccessoryDeviceType );
               
    return iStaticAttributes.iAccessoryDeviceType;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::PhysicalConnectionCapsL
// Tells whether the physical connection type(s) given as parameter are 
// suppported by the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TAccPolGenericID::PhysicalConnectionCaps( 
    const TUint32 aPhysicalConnectionBitMask ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::PhysicalConnectionCapsL() 0x%x", 
                  aPhysicalConnectionBitMask );

    TUint32 result( iStaticAttributes.iPhysicalConnection );
    result = result & aPhysicalConnectionBitMask;
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::PhysicalConnectionCapsL() - return %d", 
                  ( result == aPhysicalConnectionBitMask ) );
                  
    return ( result == aPhysicalConnectionBitMask );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::PhysicalConnectionCaps
// Returns the physical connection type(s) of the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TAccPolGenericID::PhysicalConnectionCaps() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::PhysicalConnectionCaps()" );
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::PhysicalConnectionCaps() - return 0x%x", 
                  iStaticAttributes.iPhysicalConnection );
                  
    return iStaticAttributes.iPhysicalConnection;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::ApplicationProtocolCapsL
// Tells whether the application protocol type(s) given as parameter are 
// suppported by the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TAccPolGenericID::ApplicationProtocolCaps( 
    const TUint32 aApplicationProtocolBitMask ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::ApplicationProtocolCapsL() 0x%x", 
                  aApplicationProtocolBitMask );

    TUint32 result( iStaticAttributes.iApplicationProtocol );
    result = result & aApplicationProtocolBitMask;
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::ApplicationProtocolCapsL() - return %d", 
                  ( result == aApplicationProtocolBitMask ) );
                  
    return ( result == aApplicationProtocolBitMask );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::ApplicationProtocolCaps
// Returns the application protocol type(s) of the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 TAccPolGenericID::ApplicationProtocolCaps() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::ApplicationProtocolCaps()" );
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::ApplicationProtocolCaps() - return 0x%x", 
                  iStaticAttributes.iApplicationProtocol );
                  
    return iStaticAttributes.iApplicationProtocol;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::SubblockCapsL
// Tells whether the application protocol type(s) given as parameter are 
// suppported by the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TAccPolGenericID::SubblockCaps( 
    const TUint64 aSubblockBitMask ) const
    {
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::SubblockCapsL() 0x%x", aSubblockBitMask );

    TUint64 result( iStaticAttributes.iCapabilitiesSubblocks );
    result = result & aSubblockBitMask;
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::SubblockCapsL() - return %d", 
                  ( result == aSubblockBitMask ) );
                  
    return ( result == aSubblockBitMask );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::SubblockCaps
// Returns all the defined subblocks of the accessory in question.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint64 TAccPolGenericID::SubblockCaps() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::SubblockCaps()" );
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::SubblockCaps() - return 0x%x", 
                  iStaticAttributes.iCapabilitiesSubblocks );
               
    return iStaticAttributes.iCapabilitiesSubblocks;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::HWModelID
// Returns the model identifier for this accessory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC TAccPolGenericID::HWModelID() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::HWModelID()" );
    
    API_TRACE_RAW_1( "[AccFW: ACCPOLICY] TAccPolGenericID::HWModelID() - return", 
                      iStaticAttributes.iHWModelID );
                      
    return iStaticAttributes.iHWModelID;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::HWDeviceID
// Returns the hardware device specific identifier for this accessory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint64 TAccPolGenericID::HWDeviceID() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::HWDeviceID()" );

    API_TRACE_2( "[AccFW: ACCPOLICY] TAccPolGenericID::HWDeviceID() - return H:0x%x, L:0x%x", 
                  I64HIGH( iStaticAttributes.iHWDeviceID ), 
                  I64LOW( iStaticAttributes.iHWDeviceID ) );        
                 
    return iStaticAttributes.iHWDeviceID;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericID::DeviceAddress
// Returns the device address for this accessory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TUint64 TAccPolGenericID::DeviceAddress() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::DeviceAddress()" );
    
    API_TRACE_2( "[AccFW: ACCPOLICY] TAccPolGenericID::DeviceAddress() - return H:0x%x, L:0x%x", 
                  I64HIGH( iStaticAttributes.iDeviceAddress ), 
                  I64LOW( iStaticAttributes.iDeviceAddress ) );        
                 
    return iStaticAttributes.iDeviceAddress;
    }
    
// -----------------------------------------------------------------------------
// TAccPolGenericID::UniqueID
// Returns the unique identifier assigned by Accessory Policy Database.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TAccPolGenericID::UniqueID() const
    {
    API_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericID::UniqueID()" );
    
    API_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericID::UniqueID() - return %d", iStaticAttributes.iDBID );    
    return iStaticAttributes.iDBID;
    }

//  End of File


