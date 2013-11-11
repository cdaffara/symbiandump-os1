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
* Description:  Defines the key event handling API of Accessory Server and
*    implements the converter plugin for Remote Connection framework.
*
*/



// INCLUDE FILES
#include    "RemConKeyEventConverter.h"
#include    "RemConKeyEventConverterImpl.h"
#include    "RemConDebug.h"
#include    <RemConExtensionApi.h>
#include    <remconcoreapi.h>

// CONSTANTS
const TInt KRemConBearerUid = 0x102069AA; // From Bearer

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::CRemConKeyEventConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConKeyEventConverter::CRemConKeyEventConverter()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::CRemConKeyEventConverter()" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRemConKeyEventConverter::ConstructL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::ConstructL()" );
    iImpl = CRemConKeyEventConverterImpl::NewL();
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::ConstructL() return void" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConKeyEventConverter* CRemConKeyEventConverter::NewL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::NewL()" );

    CRemConKeyEventConverter* self = new( ELeave ) CRemConKeyEventConverter;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::NewL() return %d", self );
    return self;
    }

    
// Destructor
CRemConKeyEventConverter::~CRemConKeyEventConverter()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::~CRemConKeyEventConverter()" );
    delete iImpl;
    iImpl = 0;
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::~CRemConKeyEventConverter() return void" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::InterfaceToBearer
// Overload of CRemConConverterPlugin -base class function definition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRemConKeyEventConverter::InterfaceToBearer( 
    TUid /*aInterfaceUid*/, 
    TUint /*aOperationId*/,
    const TDesC8& /*aData*/,
    TRemConMessageType /*aMsgType*/, 
    TDes8& /*aBearerData*/ ) const
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverter::InterfaceToBearer() return KErrNone (does nothing))" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::BearerToInterface
// Overload of CRemConConverterPlugin -base class function definition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRemConKeyEventConverter::BearerToInterface( 
    const TDesC8& aBearerData,
    TUid& aInterfaceUid,
    TUint& aOperationId,
    TRemConMessageType& /*aMsgType*/,
    TDes8& aData ) const
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEventConverter::BearerToInterface(aInterfaceUid=%d, aOperationId=%d)", aInterfaceUid, aOperationId );
    TInt ret( KErrNone );
    TRemConKeyEventDataBuf data;
    data.Copy(aBearerData);
    const TRemConKeyEventData keyData = data();
		
    ret = iImpl->ResolveOperationID( keyData, aInterfaceUid, aOperationId, aData );
        
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::BearerToInterface() returns %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::SupportedUids
// Overload of CRemConConverterPlugin -base class function definition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CRemConKeyEventConverter::SupportedUids( 
    const TUid& aInterfaceUid, 
    const TUid& aBearerUid ) const
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEventConverter::SupportedUids(aInterfaceUid=%d, aBearerUid=%d)", aInterfaceUid, aBearerUid );
    
    TBool ret(	( 
    			aInterfaceUid == TUid::Uid(KRemConCoreApiUid) || 
    			aInterfaceUid == TUid::Uid(KRemConExtPocApiUid) ||
    			aInterfaceUid == TUid::Uid(KRemConExtSynchronizationApiUid) ||
    			aInterfaceUid == TUid::Uid(KRemConExtCallHandlingApiUid)
    			)
    			
    			&&
        		
        		( 
        		aBearerUid == TUid::Uid(KRemConBearerUid)
        		)
        	 );
        		
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::SupportedUids() returns %d", ret );
    return ret;
    }    
    
// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::SupportedInterface
// Overload of CRemConConverterPlugin -base class function definition.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CRemConKeyEventConverter::SupportedInterface(
    const TDesC8& aInterfaceData, 
    const TUid& aBearerUid) const
    {
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::SupportedInterface(aBearerUid=%d)", aBearerUid.iUid );
    COM_TRACE_1( "[AccFW:RemConConverter] Compare bearerUID to %d )", KRemConBearerUid );
    (void) aInterfaceData;
    TBool ret( aBearerUid == TUid::Uid(KRemConBearerUid) );
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::SupportedInterface() returns %d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverter::GetInterface
// Returns API of this converter, if it's asked.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRemConKeyEventConverter::GetInterface( TUid aUid )
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEventConverter::GetInterface() this=%d, aUid=%d", this, aUid );

	TAny* ret = NULL;
	
	// Interface version implemented by this converter asked ?
	if ( aUid == TUid::Uid( KRemConConverterInterface1 ) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConConverterInterface*>( this ) );
		}

	COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverter::GetInterface() this=%d", ret );
	return ret;
    }        
    
//  End of File  
