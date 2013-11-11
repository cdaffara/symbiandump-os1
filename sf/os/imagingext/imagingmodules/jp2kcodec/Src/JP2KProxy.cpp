/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JPEG2000 image encoder/decoder plugin entry point.
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <icl/imageconstruct.h>
#include <icl/icl_uids.hrh>
#include <JP2KUids.hrh>
#include "JP2KFormat.h"
#include "JP2KConvert.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS
#ifdef __EABI__
#ifndef IMPLEMENTATION_PROXY_ENTRY
typedef TAny* TProxyNewLPtr;
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr) {{aUid},(TProxyNewLPtr)(aFuncPtr)}
#endif
#endif

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

/**
*  CJp2kDecodeConstruct class.
*  Implement the entry point for creating JPEG2000 decoder.
*
*  @lib ?library
*  @since 2.6
*/
class CJp2kDecodeConstruct : public CImageDecodeConstruct
{
    public:

        /**
        * Two-phased constructor.
        */
        static CJp2kDecodeConstruct* NewL();

    public:
        
        /**
        * Instantiates JPEG 2000 plug-in.
        * @since 2.6
        * @param None.
        * @return Pointer to instantiated plug-in.
        */
        CImageDecoderPlugin* NewPluginL() const;
};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJp2kDecodeConstruct::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJp2kDecodeConstruct* CJp2kDecodeConstruct::NewL()
    {
    CJp2kDecodeConstruct* self = new ( ELeave ) CJp2kDecodeConstruct;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CJp2kDecodeConstruct::NewPluginL
// Creating a new CJp2kDecodeConstruct object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CImageDecoderPlugin* CJp2kDecodeConstruct::NewPluginL() const
    {
    return CJp2kDecoder::NewL();
    }

// -----------------------------------------------------------------------------
// Global implementation uid array
// Define the Implementation UIDs for JP2K decoder.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
{
    // Decoder recognize two different formats
    // JP2 file format and JP2 codestream 
    IMPLEMENTATION_PROXY_ENTRY( KJ2KDecoderImplementationUidValueFileFormat, 
    CJp2kDecodeConstruct::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KJ2KDecoderImplementationUidValueCodeStream, 
    CJp2kDecodeConstruct::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( KJ2KDecoderImplementationUid, 
    CJp2kDecodeConstruct::NewL )
};

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy Implements proxy interface for ECom
// Exported proxy for instantiation method resolution.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(  //lint !e714 Used by ECom
    TInt& aTableCount ) // Number of tables 
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
