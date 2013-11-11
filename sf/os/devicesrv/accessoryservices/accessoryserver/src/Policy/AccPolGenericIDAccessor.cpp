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
* Description:  Private, Accessory Policy Generic ID Accessory
*
*/



// INCLUDE FILES
#include    <AccPolGIDHeader.h>
#include    <AccPolGenericID.h>
#include    <AccConGenericID.h>
#include    "AccPolGenericIDAccessor.h"
#include    "acc_debug.h"

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
// TAccPolGenericIDAccessor::TAccPolGenericIDAccessor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolGenericIDAccessor::TAccPolGenericIDAccessor()
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::TAccPolGenericIDAccessor()" );
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::TAccPolGenericIDAccessor() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetFeatureAttributeL
// Set matching Generic ID feature attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetFeatureAttributeL( 
    TAccPolGenericID& aGenericID,
    const TUint aGIDAttribute, 
    const TUint32 aGIDAttributeBitMask )
    {
    COM_TRACE_2( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetFeatureAttributeL() 0x%x, 0x%x", 
                  aGIDAttribute, 
                  aGIDAttributeBitMask );

    switch ( aGIDAttribute )
        {
        case KAccPolDTDeviceType:
            {
            aGenericID.iStaticAttributes.iAccessoryDeviceType = aGIDAttributeBitMask;
            }
            break;

        case KAccPolPCPhysicalConnection:
            {
            aGenericID.iStaticAttributes.iPhysicalConnection = aGIDAttributeBitMask;
            }
            break;

        case KAccPolAPApplicationProtocol:
            {
            aGenericID.iStaticAttributes.iApplicationProtocol = aGIDAttributeBitMask;
            }
            break;

        default:
            {
            // If attribute is not of known type leave with KErrGeneral
            User::Leave( KErrGeneral );
            }
        }

    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetFeatureAttributeL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetFeatureAttributeL
// Set matching Generic ID feature attribute.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetFeatureAttributeL( 
    TAccPolGenericID& aGenericID,
    const TUint aGIDAttribute, 
    const TUint64 aGIDAttributeBitMask )
    {
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetFeatureAttributeL() %d", 
                  aGIDAttribute );

    switch ( aGIDAttribute )
        {
        case KAccPolSBCapabilities:
            {
            aGenericID.iStaticAttributes.iCapabilitiesSubblocks = aGIDAttributeBitMask;
            }
            break;

        default:
            {
            // If attribute is not of known type leave with KErrGeneral
            User::Leave( KErrGeneral );
            }
        }
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetFeatureAttributeL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetHWModelID
// Set hardware model ID for Generic ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetHWModelID( 
    TAccPolGenericID& aGenericID,
    const TDesC& aHWModelID )
    {
    COM_TRACE_RAW_1( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetHWModelIDL() ", aHWModelID );
    aGenericID.iStaticAttributes.iHWModelID.Copy( aHWModelID );
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetHWModelIDL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetHWDeviceID
// Set hardware model ID for Generic ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetHWDeviceID( 
    TAccPolGenericID& aGenericID,
    const TUint64 aHWDeviceID )
    {
    COM_TRACE_2( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetHWDeviceIDL() H:0x%x, L:0x%x",  
                  I64HIGH( aHWDeviceID ), 
                  I64LOW( aHWDeviceID ) );        
    aGenericID.iStaticAttributes.iHWDeviceID = aHWDeviceID;
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetHWDeviceIDL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetDeviceAddress
// Set device address for Generic ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetDeviceAddress( 
    TAccPolGenericID& aGenericID,
    const TUint64 aDeviceAddress )
    {
    COM_TRACE_2( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetDeviceAddressL() H:0x%x, L:0x%x",
                  I64HIGH( aDeviceAddress ), 
                  I64LOW( aDeviceAddress ) );        
    aGenericID.iStaticAttributes.iDeviceAddress = aDeviceAddress;
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetDeviceAddressL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetDBIDL
// Set unique identifier (DBID) for Generic ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetDBID( 
    TAccPolGenericID& aGenericID,
    const TInt aDBID )
    {
    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetDBIDL() 0x%x", aDBID );
    aGenericID.iStaticAttributes.iDBID = aDBID;
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::SetDBIDL() - return void" );
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::CompareGenericID
// Compare two Generic ID instances.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TAccPolGenericIDAccessor::CompareGenericID( 
    const TAccPolGenericID& aLValue,
    const TAccPolGenericID& aRValue )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::CompareGenericID()" );
    TBool ret( EFalse );

    ret = ( aLValue.iStaticAttributes.iAccessoryDeviceType == 
            aRValue.iStaticAttributes.iAccessoryDeviceType &&
            aLValue.iStaticAttributes.iPhysicalConnection == 
            aRValue.iStaticAttributes.iPhysicalConnection &&
            aLValue.iStaticAttributes.iApplicationProtocol == 
            aRValue.iStaticAttributes.iApplicationProtocol &&
            aLValue.iStaticAttributes.iCapabilitiesSubblocks == 
            aRValue.iStaticAttributes.iCapabilitiesSubblocks &&
            aLValue.iStaticAttributes.iHWModelID == 
            aRValue.iStaticAttributes.iHWModelID &&
            aLValue.iStaticAttributes.iHWDeviceID == 
            aRValue.iStaticAttributes.iHWDeviceID );

    COM_TRACE_1( "[AccFW: ACCPOLICY] TAccPolGenericIDAccessor::CompareGenericID() - return %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetGenericIDStaticAttributes
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( 
    CAccConGenericID* const aLGenericID, 
    const TAccPolGenericID& aRGenericID )
    {
    aLGenericID->iGenericID = aRGenericID;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::SetGenericIDStaticAttributes
// -----------------------------------------------------------------------------
//                                   
void TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( 
    TAccPolGenericID& aGenericID, 
    const TAccPolGIDHeader &aGenericIDHeader)
    {
	aGenericID.iStaticAttributes = aGenericIDHeader;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::NameValueBuf
// -----------------------------------------------------------------------------
//
EXPORT_C CBufFlat* TAccPolGenericIDAccessor::NameValueBuf( 
    CAccConGenericID* const aGenericID )
    {
    return aGenericID->iNameValueBuf;
    }

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::GenericIDRef
// -----------------------------------------------------------------------------
//
EXPORT_C TAccPolGenericID& TAccPolGenericIDAccessor::GenericIDRef( 
    CAccConGenericID* const aGenericID )
    {
    return aGenericID->iGenericID;
    }

#ifdef _DEBUG
// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::DebugPrint
// Trace out the static header of Generic ID instance in debug build.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::DebugPrint( const TAccPolGenericID& aGenericID )
    {
    COM_TRACE_( "[AccFW: ACCPOLICY] START GenericID:" );
    COM_TRACE_1("    DeviceType:0x%x",
                     aGenericID.iStaticAttributes.iAccessoryDeviceType );
    COM_TRACE_1( "    PhysicalConnection:0x%x", 
                      aGenericID.iStaticAttributes.iPhysicalConnection );
    COM_TRACE_1( "    ApplicationProtocol :0x%x", 
                      aGenericID.iStaticAttributes.iApplicationProtocol );
    COM_TRACE_2( "    CapabilitiesSubblocks: HIGH:0x%x, LOW:0x%x" ,
                      I64HIGH( aGenericID.iStaticAttributes.iCapabilitiesSubblocks ),
                      I64LOW( aGenericID.iStaticAttributes.iCapabilitiesSubblocks ) );
    COM_TRACE_RAW_1( "    HWModelID:", 
                          aGenericID.iStaticAttributes.iHWModelID );
    COM_TRACE_2( "    HWDeviceID: HIGH:0x%x, LOW:0x%x \n" ,
                      I64HIGH( aGenericID.iStaticAttributes.iHWDeviceID ),
                      I64LOW( aGenericID.iStaticAttributes.iHWDeviceID ) );
    COM_TRACE_2( "    DeviceAddress: HIGH:0x%x, LOW:0x%x" ,
                      I64HIGH( aGenericID.iStaticAttributes.iDeviceAddress ),
                      I64LOW( aGenericID.iStaticAttributes.iDeviceAddress ));
    COM_TRACE_1( "    Unique Id: %d", 
                      aGenericID.iStaticAttributes.iDBID );
    COM_TRACE_( "[AccFW: ACCPOLICY] GenericID END" );
    }
#else

// -----------------------------------------------------------------------------
// TAccPolGenericIDAccessor::DebugPrint
// This function does nothing in non-debug mode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void TAccPolGenericIDAccessor::DebugPrint( const TAccPolGenericID& /*aGenericID*/ )
    {
    // No debug
    }

#endif // _DEBUG

//  End of File
