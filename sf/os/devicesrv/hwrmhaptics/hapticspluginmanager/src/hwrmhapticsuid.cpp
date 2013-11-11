/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics uid implementation.
*
*/


#include <s32file.h>
#include <f32file.h>
#include <pathinfo.h>
#include "hwrmhapticsuid.h"
#include "hwrmhapticstrace.h"

//drive char and colon string length, e.g. "c:"
const TInt KDriveAndColon = 2; 

// Maximum uid file line length
const TInt KMaxLineLength( 100 );

// New line character used to terminate each line in uid file
const TInt KNewLine( '\n' );


// format specifier used to match valid UIDs
_LIT8( KMatchUid, "0x????????" );

// format specifier used to match valid enabled sign
_LIT8( KMatchPlusSign, "+" );

// valid vendor id definition
_LIT8( KVid, "VID" );

// valid secure id definition
_LIT8( KSid, "SID" );



// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
TUidItem::TUidItem( const TDesC8& aUidType, TUid aUid, TBool aAllowed )
            : iUidType( aUidType ), iUid( aUid ), iAllowed( aAllowed )
    {
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsUid* CHWRMHapticsUid::NewL( const TDesC& aFilename )
    {
    CHWRMHapticsUid* self = NewLC( aFilename );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor. Leaves instance on the cleanup stack.
// ---------------------------------------------------------------------------
//
CHWRMHapticsUid* CHWRMHapticsUid::NewLC( const TDesC& aFilename )
    {
    CHWRMHapticsUid* self = new( ELeave ) CHWRMHapticsUid();
    
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsUid::~CHWRMHapticsUid()
    {
    // Delete all the array objects
    iUidArray.Close();
    }


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsUid::CHWRMHapticsUid()
    {
    }

// ---------------------------------------------------------------------------
// By default Symbian 2nd phase constructor is private.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsUid::ConstructL( const TDesC& aFilename )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Buffer to store the full path and filename
    TFileName filename;
    
    // Get private file path
    User::LeaveIfError( fs.PrivatePath( filename ) );

    // add the filename to path
    filename.Insert( 0, PathInfo::RomRootPath().Left( KDriveAndColon ) );
    filename.Append( aFilename );
    
    // Parse the file and construct the array of UIDs
    ParseUidFileL( fs, filename );

    CleanupStack::PopAndDestroy( &fs );
    }


// ---------------------------------------------------------------------------
// Checks if automated license setting is allowed for VID/SID combination
// found from aMessage. If definition for SID is found then VID checking
// is omitted.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsUid::LicenseAutoSetAllowed( const RMessage2& aMessage )
    {
    TUid sid = aMessage.SecureId();
    TUid vid = aMessage.VendorId();
    TBool allowed = EFalse;
    TBool sidFound = EFalse;

    for ( TInt n=0; n < iUidArray.Count(); n++ )
        {
        if ( iUidArray[n].iUidType == KSid &&
             sid == iUidArray[n].iUid )
            {
            allowed = iUidArray[n].iAllowed;
            sidFound = ETrue;
            }

        if ( !sidFound && 
             iUidArray[n].iUidType == KVid &&
             vid == iUidArray[n].iUid )
            {
            allowed = iUidArray[n].iAllowed;
            }
        }

    return allowed;
    }


// ---------------------------------------------------------------------------
// Method constructs the array of UIDs from the given file.
// A valid line in the file contains only UID
// ---------------------------------------------------------------------------
//
void CHWRMHapticsUid::ParseUidFileL( RFs& aFs, 
                                     const TDesC& aFilename )
    {
    TLex8 lex;
    TPtrC8 sign;
    TPtrC8 uidType;
    TPtrC8 id;
    TUid uid;
    TInt err( KErrNone );
    
    // Buffer to read each line of the file into
    TBuf8<KMaxLineLength> fileline;
    TChar newLine( KNewLine );  
    RFileReadStream stream;    
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsUid::ParseUidFileL - Opening UID file: %S" ), &aFilename ) );
    
    // Open the file and attach to stream 
    err = stream.Open( aFs, aFilename, EFileRead );
    
    // Return without error if file is not found
    if ( err != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsUid::ParseUidFileL - UID file open failed: %S, error: %d" ), &aFilename, err ) );
        }
    else
        {
        CleanupClosePushL( stream );
       
        while( err != KErrEof )
            {
            // read from the file upto the newline character
            TRAP( err, stream.ReadL( fileline, newLine ) );

            lex.Assign( fileline );
            sign.Set( lex.NextToken() );            
            uidType.Set( lex. NextToken() );
            id.Set( lex.NextToken() );
            TBool allowed = EFalse;
            if( sign.Match( KMatchPlusSign ) == 0 )
                {
                allowed = ETrue;
                }
            if( id.Match( KMatchUid ) == 0 )
                {
                // convert it
                if( ConvertId( id, uid ) != KErrCorrupt )
                    {
                    // add UID to array
                    iUidArray.AppendL( TUidItem( uidType, uid, allowed ) );
                    }
                }
            }

        // Close stream
        CleanupStack::PopAndDestroy( &stream );
        }
    }

// ---------------------------------------------------------------------------
// Helper method to convert and validate UID from a descriptor
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsUid::ConvertId( const TDesC8& aUidDes, 
                                    TUid& aUid ) const
    {
    TInt ret( KErrNone );
    TUint32 id;

    // this is a matching id
    TLex8 lex( aUidDes.Right( 8 ) );
    
    if( lex.Val( id, EHex ) != KErrNone )
        {
        // Failed to convert to int
        ret = KErrCorrupt;
        }
    else
        {
        // conversion ok so set the uid
        aUid.iUid = id;
        }

    return ret;
    }


// End of File
