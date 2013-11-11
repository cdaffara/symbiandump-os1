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
* Description:  Implements Configuration File Parser Interface.
*
*/


// INCLUDE FILES

#include "acc_debug.h"
#include "AccPolAccessoryPolicy.h"
#include "AccPolGenericIDAccessor.h"
#include "AccPolicyDB.h"
#include "AccPolNameValueArraySerial.h"
#include "AccClientServerMessages.h"
#include <AccConfigFileParser.h>
#include <AccConGenericID.h>
#include <s32mem.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt  KAccConfigMaxCapabilityGroups = 64;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccConfigFileParser::ParseSubblocksL
// Detecting subblocks and store them to destination array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccConfigFileParser::ParseSubblocksL( const TUint64& aVal,
                                           RArray<TUint64>& aCaps )
    {
    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseSubblocksL()" );

    TUint64 sum( 0 );
    TUint64 curVal( 1 );
    TInt curBit( 0 );

    do {
        COM_TRACE_4( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) Binary for this roundtrip is HI:0x%x LO:0x%x, total sum is HI:0x%x LO:0x%x", I64HIGH( curVal ), I64LOW( curVal ), I64HIGH( sum ), I64LOW( sum ) );
        COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) & HI:0x%x LO:0x%x", I64HIGH( curVal & aVal ), I64LOW( curVal & aVal ) );

        // Check if this subblock is defined in SB-def block        
        if ( ( curVal & aVal ) == curVal )
            {
            COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::(Cap) MATCH! HI:0x%x LO:0x%x", I64HIGH( curVal ), I64LOW( curVal ) );
            aCaps.AppendL( curVal ); // Append to found caps array
            sum += curVal;
            }        
        curBit++;
        curVal = 2 * curVal;
        }
    while ( sum < aVal && curBit < KAccConfigMaxCapabilityGroups );

    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseSubblocksL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::ParseNamesL
// Detect all subblock names and append them to the gicen array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccConfigFileParser::ParseNamesL( const TUint32& aVal,
                                       RArray<TUint32>& aNames )
    {
    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL()" );
    TUint32 sum( 0 );
    TUint32 curVal( 1 );
    TInt curBit( 0 );

    do {
        COM_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL Binary for this roundtrip is 0x%x, total sum is 0x%x", curVal, sum );
        COM_TRACE_1( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL & 0x%x", curVal & aVal );
        // Check if this subblock is defined in SB-def block

        if ( ( curVal & aVal ) == curVal )
            {
            COM_TRACE_1( "[AccFW:SrvUtil] (Name) MATCH! 0x%x",curVal );
            aNames.AppendL( curVal ); // Append to found caps array
            sum += curVal;
            }
        curBit++;
        curVal = 2 * curVal;        
        }
    while ( sum < aVal && curBit < KAccConfigMaxCapabilityGroups );

    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ParseNamesL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::CAccConfigFileParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccConfigFileParser::CAccConfigFileParser()
    {
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccConfigFileParser::ConstructL( const TDesC& /*aConfFileName*/ )
    {
    COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAccConfigFileParser* CAccConfigFileParser::NewL( 
                                            const TDesC& aConfFileName )
    {
    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::NewL()" );
    CAccConfigFileParser* self = new( ELeave ) CAccConfigFileParser;

    CleanupStack::PushL( self );
    self->ConstructL( aConfFileName );
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::NewL - return" );
    return self;
    }

// Destructor
EXPORT_C CAccConfigFileParser::~CAccConfigFileParser()
    {
    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::~CAccConfigFileParser()" );

    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::~CAccConfigFileParser - return" );
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::FindL
// Find a matching Generic ID for Hardware Device ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccConfigFileParser::ParseGenericIDL( 
    CAccConGenericID* const aGenericID,
    const TAccPolGIDHeader& aGenericIDHeader,
    const RArray<TAccPolNameValueRecord>& aNameValueArray )
    {

    __ASSERT_DEBUG( aGenericID, User::Invariant() );

    TAccPolGenericID genericID = aGenericID->GenericID();

    TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID, 
                                                    KAccPolDTDeviceType, 
                                                    aGenericIDHeader.iAccessoryDeviceType );
    TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID,
                                                    KAccPolPCPhysicalConnection,
                                                    aGenericIDHeader.iPhysicalConnection );
    TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID,
                                                    KAccPolAPApplicationProtocol,
                                                    aGenericIDHeader.iApplicationProtocol );
    TAccPolGenericIDAccessor::SetFeatureAttributeL( genericID,
                                                    KAccPolSBCapabilities, 
                                                    aGenericIDHeader.iCapabilitiesSubblocks );
    TAccPolGenericIDAccessor::SetHWModelID( genericID, aGenericIDHeader.iHWModelID );
    TAccPolGenericIDAccessor::SetHWDeviceID( genericID, 
                                             aGenericIDHeader.iHWDeviceID );
    TAccPolGenericIDAccessor::SetDeviceAddress( genericID,
                                                aGenericIDHeader.iDeviceAddress );

	if ( aGenericIDHeader.iDBID > 0 )
    	{
    	TAccPolGenericIDAccessor::SetDBID( genericID, aGenericIDHeader.iDBID );
    	}
	else
    	{
    	TAccPolGenericIDAccessor::SetDBID( genericID, KAccSrvGenerateGID);		
    	}

    TAccPolGenericIDAccessor::SetGenericIDStaticAttributes( aGenericID, genericID );      

    RBufWriteStream wStrm;

    // Externalize to CAccConGenericID
    (void) wStrm.Open( *TAccPolGenericIDAccessor::NameValueBuf (aGenericID ) ); 
    wStrm.PushL();
    TAccPolNameValueArraySerial::ExternalizeL( aNameValueArray, wStrm );
    wStrm.CommitL();         
    wStrm.Close();
    wStrm.Pop();    

#ifdef _DEBUG  
    TBuf8<KTraceMaxSize> b;
    TInt size( TAccPolGenericIDAccessor::NameValueBuf (aGenericID )->Size() );
    if( size > KTraceMaxSize )
        {
        size = KTraceMaxSize;//COM_TRACE_RAW_1 supports KTraceMaxSize
        }
    TAccPolGenericIDAccessor::NameValueBuf (aGenericID )->Read( 0, b, size );
    TBuf<KTraceMaxSize> c;
    b.Copy( c );
    COM_TRACE_RAW_1( "   ", c );
#endif // _DEBUG

    // Internalize to the local variable
/*    RArray<TAccPolNameValueRecord> nameValueArray;
	RBufReadStream rStrm;
	rStrm.Open( *TAccPolGenericIDAccessor::NameValueBuf (aGenericID ) );
	rStrm.PushL();			
	TAccPolNameValueArraySerial::InternalizeL( rStrm, nameValueArray );
	rStrm.Close();
	rStrm.Pop(); 
*/
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::FindL
// Find a matching Generic ID for Hardware Device ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccConfigFileParser::FindL( CAccConGenericID* const aGenericID,
                                           TUint64 aHWDeviceID,
                                           const TDesC& aHWModelID )
    {
    API_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::FindL(Device HI:0x%x LO:0x%x)", I64HIGH( aHWDeviceID ), I64LOW( aHWDeviceID ) );

    CAccPolAccessoryPolicy* accPolicy = CAccPolAccessoryPolicy::NewL( NULL );
    // This is not used in ParseGenericIDContent since last parameter is not EAccRule
    TBuf8<1> nullGidInt;

    CleanupStack::PushL( accPolicy );
    accPolicy->ParseGenericIDContentL( 
                    aGenericID,
                    aHWDeviceID,
                    aHWModelID,
                    nullGidInt,    // We don't have GID integer
                    EAccUnknown ); // unknown type. accPolicy does not use this value in search

    CleanupStack::PopAndDestroy( accPolicy );

    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser:::FindL -  return void" );    
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::bitNumber
// Find a bit from aBitmask.
// -----------------------------------------------------------------------------
//
TInt CAccConfigFileParser::BitNumber( TUint64 aBitmask )
    {
    API_TRACE_2( "[AccFW:SrvUtil] CAccConfigFileParser::BitNumber(HI:0x%x LO:0x%x)", I64HIGH( aBitmask ), I64LOW( aBitmask ) );

#ifdef _DEBUG

    _LIT( KZeroBitNumber, "ZeroBitNumber" );

    if ( !aBitmask )
        {
        COM_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::BitNumber - Panics now" );
        User::Panic( KZeroBitNumber, KErrArgument );
        }

#endif // _DEBUG

    TInt curBit(-1);
    while( ( aBitmask>> ++curBit ) > 1 )

    API_TRACE_1( "[AccFW:SrvUtil] CAccConfigFileParser::BitNumber - return %d", curBit );
    return curBit;
    }

// -----------------------------------------------------------------------------
// CAccConfigFileParser::FindL
// Find a matching Generic ID for Hardware Device ID.
// -----------------------------------------------------------------------------
//
EXPORT_C void CAccConfigFileParser::FindL( TAccPolGenericID& /*aGenericID*/,
                                           TUint64 /*aHWDeviceID*/,
                                           const TDesC& /*aHWModelID*/ )
    {        
    API_TRACE_( "[AccFW:SrvUtil] CAccConfigFileParser::FindL() - KErrNotSupported");
    User::Leave( KErrNotSupported );    
    }

// End of File
