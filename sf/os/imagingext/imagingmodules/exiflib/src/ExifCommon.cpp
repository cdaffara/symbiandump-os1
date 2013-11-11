/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An internal service class including some utility functions.
*
*/


// INCLUDE FILES
#include "ExifCommon.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TExifCommon::SetUint32
// Puts the given 32-bit unsigned integer to the 4-byte location, whose start 
// point is specified with the given 8-bit unsigned integer pointer. 
// -----------------------------------------------------------------------------
//
TInt TExifCommon::SetUint32( TUint8* aPtr, TUint32 aUint32 )
    {
    if ( !aPtr )
        {
        return KErrGeneral;
        }
    *( aPtr + 3 ) = STATIC_CAST( TUint8, ( aUint32 >> 24 ) & KOneByteMask );
    *( aPtr + 2 ) = STATIC_CAST( TUint8, ( aUint32 >> 16 ) & KOneByteMask );
    *( aPtr + 1 ) = STATIC_CAST( TUint8, ( aUint32 >> 8 ) & KOneByteMask );
    *aPtr = STATIC_CAST( TUint8, aUint32 & KOneByteMask );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TExifCommon::SetUint16
// Puts the given 16-bit unsigned integer to the 2-byte location, whose start 
// point is specified with the given 8-bit unsigned integer pointer. 
// -----------------------------------------------------------------------------
//
TInt TExifCommon::SetUint16( TUint8* aPtr, TUint16 aUint16 )
    {
    if ( !aPtr )
        {
        return KErrGeneral;
        }
    *( aPtr + 1 ) = STATIC_CAST( TUint8, ( aUint16 >> 8 ) & KOneByteMask );
    *aPtr = STATIC_CAST( TUint8, aUint16 & KOneByteMask );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TExifCommon::Uint32
// Returns a 32-bit unsigned integer from the location, whose start point is 
// specified with the given 8-bit unsigned integer pointer. 
// -----------------------------------------------------------------------------
//
TUint32 TExifCommon::Uint32L( TUint8* aPtr )
    {
    if ( !aPtr )
        {
        User::Leave( KErrGeneral );
        }
    TUint32 ret = 0;
    ret = *( aPtr + 3 );
    ret = ( ret << 8 ) + *( aPtr + 2 );
    ret = ( ret << 8 ) + *( aPtr + 1 );
    ret = ( ret << 8 ) + *aPtr;
    return ret;
    }

// -----------------------------------------------------------------------------
// TExifCommon::Int32
// Returns a 32-bit integer from the location, whose start point is specified  
// with the given 8-bit integer pointer. 
// -----------------------------------------------------------------------------
//
TInt32 TExifCommon::Int32L( TUint8* aPtr )
    {
    if ( !aPtr )
        {
        User::Leave( KErrGeneral );
        }
    TUint32 ret = 0;
    ret = *( aPtr + 3 );
    ret = ( ret << 8 ) + *( aPtr + 2 );
    ret = ( ret << 8 ) + *( aPtr + 1 );
    ret = ( ret << 8 ) + *aPtr;
    return STATIC_CAST( TInt32, ret );
    }

// -----------------------------------------------------------------------------
// TExifCommon::Uint16
// Returns a 16-bit unsigned integer from the location, whose start point is 
// specified with the given 8-bit unsigned integer pointer. 
// -----------------------------------------------------------------------------
//
TUint16 TExifCommon::Uint16L( TUint8* aPtr )
    {
    if ( !aPtr )
        {
        User::Leave( KErrGeneral );
        }
    TUint16 ret = 0;
    ret = *( aPtr + 1 );
    ret = STATIC_CAST( TUint16, ( ret << 8 ) + *aPtr );
    return ret;
    }

// -----------------------------------------------------------------------------
// TExifCommon::IsValidTagId
// Checks if the given 16-bit unsigned integer is one of the defined tag IDs.
// -----------------------------------------------------------------------------
//
TBool TExifCommon::IsValidTagId( TUint16 aTagId )
    {
    TInt i = 0;
    for ( i = 0; i < KNoIfd0Tags; ++i )
        {
        if ( aTagId == ifd0Tags[i].iId )
            {
            return ETrue;
            }
        }
    for ( i = 0; i < KNoIfdExifTags; ++i )
        {
        if ( aTagId == ifdExifTags[i].iId )
            {
            return ETrue;
            }
        }
    for ( i = 0; i < KNoIfd1Tags; ++i )
        {
        if ( aTagId == ifd1Tags[i].iId )
            {
            return ETrue;
            }
        }
    for ( i = 0; i < KNoIfdGpsTags; ++i )
        {
        if ( aTagId == ifdGpsTags[i].iId )
            {
            return ETrue;
            }
        }
    for ( i = 0; i < KNoIfdIntOpTags; ++i )
        {
        if ( aTagId == ifdIntOpTags[i].iId )
            {
            return ETrue;
            }
        }
    LOGTEXT2( _L( "ExifLib: TExifCommon::IsValidTagId() returning EFalse: aTagId=0x%x" ), aTagId );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TExifCommon::LocateJpegMarkerPtr
// Locates the pointer to the first specified Jpeg Marker from the begining
// inside the given interval.
// -----------------------------------------------------------------------------
//
TUint8* TExifCommon::LocateJpegMarkerPtr( 
    TUint16 aMarker, 
    TUint8* aStartPtr,
    TUint8* aEndPtr)
    {
    if ( ( !aStartPtr ) || ( !aEndPtr ) || ( !aMarker ) )
        {
        return NULL;
        }

    TUint16 revMarker = 
        STATIC_CAST( TUint16, ( aMarker << 8 ) + ( aMarker >> 8 ) );
    --aStartPtr;
    TUint16 atHand = 0;
    TInt error = KErrNone;
    do
        {
        if ( aStartPtr >= aEndPtr )
            {
            return NULL;
            }
        if ( *( ++aStartPtr ) == KMarkerStart )
            {
            TRAP( error, atHand = TExifCommon::Uint16L( aStartPtr ) );
            }
        } 
    while ( (atHand != revMarker ) && ( error == KErrNone) );
    if ( error )
        {
        return NULL;
        }
    return aStartPtr;
    }

// -----------------------------------------------------------------------------
// TExifCommon::LocateJpegMarkerPtrFromEnd
// Locates the pointer to the first specified Jpeg Marker from the end
// inside the given interval.
// -----------------------------------------------------------------------------
//
TUint8* TExifCommon::LocateJpegMarkerPtrFromEnd( 
    TUint16 aMarker, 
    TUint8* aStartPtr,
    TUint8* aEndPtr)
    {
    if ( ( !aStartPtr ) || ( !aEndPtr ) || ( !aMarker ) )
        {
        return NULL;
        }

    TUint16 revMarker = 
        STATIC_CAST( TUint16, ( aMarker << 8 ) + ( aMarker >> 8 ) );
    --aEndPtr;
    TUint16 atHand = 0;
    TInt error = KErrNone;
    do
        {
        if ( aEndPtr <= aStartPtr )
            {
            return NULL;
            }
        if ( *( --aEndPtr ) == KMarkerStart )
            {
            TRAP( error, atHand = TExifCommon::Uint16L( aEndPtr ) );
            }
        } 
    while ( (atHand != revMarker ) && ( error == KErrNone) );
    if ( error )
        {
        return NULL;
        }
    return aEndPtr;
    }
