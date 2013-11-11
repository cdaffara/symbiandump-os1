// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include "HWRMFmTxRdsTextConverter.h"
#include "HWRMPolicy.h"
#include "HWRMtrace.h"

// CONSTANTS
_LIT( KConvertFromUnicodeFileName, "ConvertFromUnicode.dat" );

const TUint8 KUnmappedRdsChar = 0xFF;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxRdsTextConverter::CHWRMFmTxRdsTextConverter
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxRdsTextConverter::CHWRMFmTxRdsTextConverter()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::CHWRMFmTxRdsTextConverter()" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxRdsTextConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxRdsTextConverter::ConstructL()
    {    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConstructL()" ) );

    // Open conversion table file
    User::LeaveIfError( iFileServer.Connect() );    

    TFileName conversionTableFilename;
    User::LeaveIfError( iFileServer.PrivatePath( conversionTableFilename ) ); // Private-folder of HWRM
    conversionTableFilename.Insert(0, KDrivename); // "Z:"
    conversionTableFilename.Append( KConvertFromUnicodeFileName ); // Actual filename
    
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConstructL(), RFile::Open(%S)" ), &conversionTableFilename );
    User::LeaveIfError( iConvertFromUnicodeFile.Open( iFileServer, conversionTableFilename, EFileRead) );   

    // Read from conversion table file
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConstructL() RFile::Read" ) );
    User::LeaveIfError( iConvertFromUnicodeFile.Read(iConvertFromUnicodeTable) );

    if ( iConvertFromUnicodeTable.Length() != KConvertFromTableLength )
        {
        COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConstructL - invalid table length %d "), iConvertFromUnicodeTable.Length() );
        User::Leave( KErrNotReady );
        }

    // Default value.  Should be overwritten with CenRep setting
    SetUnsupportedCharReplacement( 0x3F /* Question Mark ? symbol */ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxRdsTextConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmTxRdsTextConverter* CHWRMFmTxRdsTextConverter::NewL()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::NewL()" ) );

    CHWRMFmTxRdsTextConverter* self = new( ELeave ) CHWRMFmTxRdsTextConverter();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxRdsTextConverter::~CHWRMFmTxRdsTextConverter()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::~CHWRMFmTxRdsTextConverter()" ) );

	iConvertFromUnicodeFile.Close();
    iFileServer.Close();

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::~CHWRMFmTxRdsTextConverter - return" ) );
    }

// ---------------------------------------------------------
// CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL
// Converts a unicode descriptor into 8bit RDS format
// All input characters must be within U+0000 -> U+024F range
// ---------------------------------------------------------
//
void CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL(const TDesC& aInput, TDes8& aOutput)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL(%S)" ), &aInput );
    
    // empty output buffer
    aOutput.Zero();

    TInt inputLength = aInput.Length();
    // convert each character from input buffer
    for (TInt inputIndex=0; inputIndex < inputLength; inputIndex++)
        {
        // get input buffer values to index conversion table
        TUint tableIndex = aInput[inputIndex];
        COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL aInput[%d] = 0x%x" ), inputIndex, tableIndex );
        if ( tableIndex >= KConvertFromTableLength )
            {
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL() aInput outside valid range" ) );
            aOutput.Zero();
            User::Leave(KErrArgument);
            }

        // convert into 8bit RDS value
        TUint8 value = iConvertFromUnicodeTable[ tableIndex ];      
        if ( SupportedCharacter(value) == EFalse )
            {
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL() using replacement char" ) );
            value = iUnsupportedCharReplacement;
            }

        COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL() appending 0x%x" ), value );
        aOutput.Append( (TChar)value );
        }

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::ConvertFromUnicodeL - return" ) );
    }

// ---------------------------------------------------------
// CHWRMFmTxRdsTextConverter::SetUnsupportedCharReplacement
// Sets the character used, to replace unsupported characters
// ---------------------------------------------------------
//
void CHWRMFmTxRdsTextConverter::SetUnsupportedCharReplacement(const TText8 aChar)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxRdsTextConverter::SetUnsupportedCharReplacement(0x%x)" ), aChar );

    iUnsupportedCharReplacement = aChar;
    }

// ---------------------------------------------------------
// CHWRMFmTxRdsTextConverter::SupportedCharacter
// Returns whether a character returned by the conversion 
// table is actually supported
// ---------------------------------------------------------
//  
TBool CHWRMFmTxRdsTextConverter::SupportedCharacter( TUint8 aChar )
    {
    return (aChar < KUnmappedRdsChar);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
