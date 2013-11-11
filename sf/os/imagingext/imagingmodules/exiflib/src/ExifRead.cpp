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
* Description:  Exif v2.2 file format reader/parser API implementation
*
*/


// INCLUDE FILES
#include "ExifReadImpl.h"
#include "ExifCore.h"
#include "ExifEndian.h"
#include "ExifCommon.h"
#include "ExifTagImpl.h"

// ============================ CLASS CExifRead ================================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifRead::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CExifRead* CExifRead::NewL( const TDesC8& aExifData )
    {
    return CExifReadImpl::NewL( aExifData, EFalse, CExifRead::ENoOptions );
    }

EXPORT_C CExifRead* CExifRead::NewL( const TDesC8& aExifData, TUint aExifReadOption )
    {
    return CExifReadImpl::NewL( aExifData, EFalse, aExifReadOption );
    }

// Destructor
CExifRead::~CExifRead()
    {
    }


// ============================ CLASS CExifReadImpl ============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifReadImpl::CExifReadImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifReadImpl::CExifReadImpl()
    {
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ConstructL( const TDesC8& aExifData, TBool aCreate, TUint aExifReadOption )
    {
    LOGTEXT3( _L( "ExifLib: CExifReadImpl::ConstructL() entering: aCreate=%x, aExifReadOption=0x%x" ), aCreate, aExifReadOption );
    iExifReadOption = aExifReadOption;
    iExifCore = CExifCore::NewL();
    iReader = CExifEndianBase::NewBaseL( aExifData.Ptr(), aExifData.Length(), 
            !aCreate );

    if ( aCreate )
        {
        ParseJpegDataL();
        }
    else
        {
        ParseExifFormatL();
        if ( !(iExifReadOption & CExifRead::ENoTagChecking ) &&
            !iExifCore->IsValid() )
            {
            LOGTEXT( _L( "ExifLib: CExifReadImpl::ConstructL() Leaving KErrNotSupported" ));
            User::Leave( KErrNotSupported );
            }
        }
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ConstructL() returning" ));
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ConstructL()
    {
    iExifReadOption = CExifRead::ENoOptions;
    iExifCore = CExifCore::NewL();
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifReadImpl* CExifReadImpl::NewL( const TDesC8& aExifData, TBool aCreate, TUint aExifReadOption )
    {
    CExifReadImpl* self = new( ELeave ) CExifReadImpl();
    CleanupStack::PushL( self ); 
    self->ConstructL( aExifData, aCreate, aExifReadOption );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifReadImpl* CExifReadImpl::NewL()
    {
    CExifReadImpl* self = new( ELeave ) CExifReadImpl();
    CleanupStack::PushL( self ); 
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CExifReadImpl::~CExifReadImpl()
    {
    LOGTEXT( _L( "ExifLib: CExifReadImpl::~CExifReadImpl() entering" ) );
    if ( iExifCore )
        {
        delete iExifCore;
        }
    if ( iReader )
        {
        delete iReader;
        }
    LOGTEXT( _L( "ExifLib: CExifReadImpl::~CExifReadImpl() returning" ) );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetTagL
// Returns the Tag instance that has the specified tag ID from the requested IFD
// -----------------------------------------------------------------------------
//
const CExifTag* CExifReadImpl::GetTagL( 
    const TExifIfdType aIfdType, 
    const TUint16 aTagId ) const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    if ( !TExifCommon::IsValidTagId( aTagId ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::GetTagL Leave( KErrNotSupported )" ) );
        User::Leave( KErrNotSupported );
        }
    return iExifCore->GetTagL( aIfdType, aTagId );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetTagIdsL
// Returns the IDs of all the tags that are stored in the Exif data.
// -----------------------------------------------------------------------------
//
TUint16* CExifReadImpl::GetTagIdsL( 
    const TExifIfdType aIfdType, 
    TInt& aNoTags ) const
    {
    if ( !iExifCore )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::GetTagIdsL Leave( KErrGeneral )" ) );
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagIdsL( aIfdType, aNoTags );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetIfdTypesL
// Returns the types of the IFDs stored in the Exif data.
// -----------------------------------------------------------------------------
//
TExifIfdType* CExifReadImpl::GetIfdTypesL( TInt& aNoIfd ) const
    {
    if ( !iExifCore )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::GetIfdTypesL Leave( KErrGeneral )" ) );
        User::Leave( KErrGeneral );
        }
    
    return iExifCore->GetIfdTypesL( aNoIfd );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetThumbnailL
// Returns pointer to a copy of the thumbnail image data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetThumbnailL() const
    {
    if ( !iExifCore )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::GetThumbnailL Leave( KErrGeneral )" ) );
        User::Leave( KErrGeneral );
        }
    
    return iExifCore->GetThumbnailL();
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::IfdExists
// Returns a boolean stating if the queried IFD structure exists in Exif data
// -----------------------------------------------------------------------------
//
TBool CExifReadImpl::IfdExists( const TExifIfdType aIfdType ) const
    {
    if ( !iExifCore )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::IfdExists return EFalse" ) );
        return EFalse;
        }

    return iExifCore->IfdExists( aIfdType );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::TagExists
// Returns a boolean stating if the queried tag exists in the specified IFD 
// structure
// -----------------------------------------------------------------------------
//
TBool CExifReadImpl::TagExists( 
    const TUint16 aTagId, 
    const TExifIfdType aIfdType ) const
    {
    if ( !iExifCore )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::TagExists return EFalse" ) );
        return EFalse;
        }

    return iExifCore->TagExists( aTagId, aIfdType );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ParseExifFormatL
// Parses the IFD data according to the specified IFD type. 
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ParseExifFormatL()
    {
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseExifFormatL() entering" ) );
    if ( ( !iExifCore ) || ( !iReader ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseExifFormatL() Leaving KErrGeneral" ) );
        User::Leave( KErrGeneral );
        }

    TUint32 ifd1Offset = 0;
    ParseIfdL( EIfd0, ifd1Offset );
    
    TUint32 ifdExifOffset = 0;
    User::LeaveIfError( iExifCore->GetTagData( KIdExifIfdPointer, 
        ifdExifOffset ) ); 

    User::LeaveIfError( iReader->MoveTo( ifdExifOffset ) );

    TUint32 nextIfdOffset = 0;
    ParseIfdL( EIfdExif, nextIfdOffset );

    TUint32 ifdGpsOffset = 0 ;
    if ( !iExifCore->GetTagData( KIdGpsIfdPointer, ifdGpsOffset ) )
        {
        User::LeaveIfError( iReader->MoveTo( ifdGpsOffset ) );
        iExifCore->CreateIfdL( EIfdGps );
        nextIfdOffset = 0;
        ParseIfdL( EIfdGps, nextIfdOffset );
        }

    TUint32 ifdIntOpOffset = 0;
    if ( !iExifCore->GetTagData( KIdIntOpIfdPointer, ifdIntOpOffset ) )
        {
        User::LeaveIfError( iReader->MoveTo( ifdIntOpOffset ) );
        iExifCore->CreateIfdL( EIfdIntOp );
        nextIfdOffset = 0;
        ParseIfdL( EIfdIntOp, nextIfdOffset );
        }

    if ( ifd1Offset )
        {
        User::LeaveIfError( iReader->MoveTo( ifd1Offset ) );
        TRAPD( error, ParseIfdL( EIfd1, nextIfdOffset ) );
        if ( error == KErrNotSupported )
            {
            iExifCore->DeleteIfd( EIfd1 );
            }
        else if ( error == KErrCorrupt && ( iExifReadOption & CExifRead::ENoTagChecking ) )
            {
            iExifCore->DeleteIfd( EIfd1 );
            }
        else
            {
            LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseExifFormatL() ParseIfdL error=%d" ), error );
            User::LeaveIfError( error );
            }
        }

    // Parse jpeg data only if it is read to buffer    
    if ( !(iExifReadOption & (CExifRead::ENoJpeg)) )
        {
        ParseJpegDataL();
        }
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseExifFormatL() returning" ) );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ParseIfdL
// Parses the Jpeg primary image data and initializes the Jpeg primary image 
// structures.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ParseIfdL( 
    const TExifIfdType aIfdType, 
    TUint32& aNextIfdOffset )
    {
    LOGTEXT3( _L( "ExifLib: CExifReadImpl::ParseIfdL() entering: aIfdType=0x%x, aNextIfdOffset=0x%x" ), aIfdType, aNextIfdOffset );
    if ( ( !iExifCore ) || ( !iReader ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leaving KErrGeneral" ) );
        User::Leave( KErrGeneral );
        }

    TUint16 noTagsInIfd = 0;
    User::LeaveIfError( iReader->ReadUInt16( noTagsInIfd ) );
    if ( !noTagsInIfd )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leaving KErrNotSupported" ) );
        User::Leave( KErrNotSupported );
        }
    // Read and create each tag one by one
    TUint32 tagsEndOffset = 0;
    for ( TUint16 i = 0; i < noTagsInIfd; ++i )
        {
        TUint16 tagId = 0;
        TUint16 tagType = 0;
        TUint32 tagCount = 0;
        User::LeaveIfError( ReadTagHeader( tagId, tagType, tagCount ) );
        TUint noBytes = 0;
        if ( ( tagType == CExifTag::ETagByte ) || 
            ( tagType == CExifTag::ETagAscii ) || 
            ( tagType == CExifTag::ETagUndefined ) )
            {
            noBytes = sizeof( TUint8 );
            }
        else if ( ( tagType == CExifTag::ETagLong ) || 
            ( tagType == CExifTag::ETagSlong ) )
            {
            noBytes = sizeof( TUint32 );
            }
        else if ( ( tagType == CExifTag::ETagRational ) || 
            ( tagType == CExifTag::ETagSrational ) )
            {
            noBytes = 2 * sizeof( TUint32 );
            }
        else if ( tagType == CExifTag::ETagShort )
            {
            noBytes = sizeof( TUint16 );
            }
        else
            {
            LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leaving KErrCorrupt" ) );
            User::Leave( KErrCorrupt );
            }
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() noBytes=%d" ), noBytes );

        if ( tagCount > KMaxApp1Size )
            {
            LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leaving KErrCorrupt (>KMaxApp1Size)" ) );
            User::Leave( KErrCorrupt );
            }
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() tagCount=%d" ), tagCount );

        if ( !tagCount )
            {
                if ( noBytes > 4)
                    {
                    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leaving KErrCorrupt (noBytes > 4)" ) );
                    User::Leave( KErrCorrupt );
                    }
                tagCount = 4 / noBytes;
            }
        noBytes *= tagCount;
        HBufC8* valueBuffer = HBufC8::NewL( noBytes );
        CleanupStack::PushL( valueBuffer );

        // If real tag data size is smaller than 4 bytes, the data is at the 
        // current position. Otherwise 4-byte pointer to the real data location
        // is kept. The pointer is at current location.
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() noBytes=%d" ), noBytes );
        if ( noBytes < 5 )
            {
            iReader->CopyBuffer( tagCount, 
                STATIC_CAST( TUint8, ( noBytes / tagCount ) ), valueBuffer );
            // Skip remaining bytes.
            if ( 4 - noBytes )
                {
                LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() skipping=%d" ), 4 - noBytes );
                iReader->Skip( 4 - noBytes );
                }
            }
        else
            {
            TUint32 valueOffset = 0;
            User::LeaveIfError( iReader->ReadUInt32( valueOffset ) );
            LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() valueOffset=%d" ), valueOffset );
            User::LeaveIfError( iReader->CopyBuffer( valueOffset, tagCount, 
                STATIC_CAST( TUint8, ( noBytes / tagCount ) ), valueBuffer ) );
            LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() tagCount=%d" ), tagsEndOffset );
            if ( tagsEndOffset < ( valueOffset + noBytes ) ) 
                {
                tagsEndOffset = valueOffset + noBytes;
                }
            }
            
        //
        // Test if unknown tags should just be skipped and delete
        // 
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() iExifReadOption=%x" ), iExifReadOption );
        if ( !(iExifReadOption & CExifRead::ENoTagChecking ) && !TExifCommon::IsValidTagId( tagId ) )
            {
            LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() skipping tagId=%x" ), tagId );
            CleanupStack::PopAndDestroy( valueBuffer );
            }
        else // Tag was known
            {
            LOGTEXT2( _L( "ExifLib: CExifReadImpl::ParseIfdL() create new tagId=%x" ), tagId );
            CExifTagImpl* tag = NULL;
            if ( iExifReadOption & CExifRead::ENoTagChecking )
                {
                tag = CExifTagImpl::NewL( tagId, 
                    STATIC_CAST( CExifTag::TExifTagDataType, tagType ), 
                    tagCount, valueBuffer, EFalse );
                }
            else
                {
                tag = CExifTagImpl::NewL( tagId, 
                    STATIC_CAST( CExifTag::TExifTagDataType, tagType ), 
                    tagCount, valueBuffer, ETrue );
                }
            CleanupStack::Pop( valueBuffer );
            CleanupStack::PushL( tag );
            
            // Ignore case when valid tag is in wrong IFD and ENoTagChecking is set
            if ( !iExifCore->TagIsValid( tag->TagInfo(), aIfdType ) )
                {
                if ( iExifReadOption & CExifRead::ENoTagChecking )
                    {
                    LOGTEXT3( _L( "ExifLib: CExifReadImpl::ParseIfdL() keep tagId=0x%x in wrong aIfdType=0x%x" ), tagId, aIfdType );
                    iExifCore->InsertTagL( aIfdType, tag, EFalse );
                    CleanupStack::Pop( tag );
                    }
                else
                    {
                    LOGTEXT3( _L( "ExifLib: CExifReadImpl::ParseIfdL() skipping tagId=0x%x in wrong aIfdType=0x%x" ), tagId, aIfdType );
                    CleanupStack::PopAndDestroy( tag );
                    }
                }
            else // Tag is in correct IFD
                {
                if ( iExifReadOption & CExifRead::ENoTagChecking )
                    {
                    iExifCore->InsertTagL( aIfdType, tag, EFalse );
                    }
                else
                    {
                    iExifCore->InsertTagL( aIfdType, tag, ETrue );
                    }
                CleanupStack::Pop( tag );
                }
            }
            
        }
    User::LeaveIfError( iReader->ReadUInt32( aNextIfdOffset ) );
    if ( tagsEndOffset )
        {
        User::LeaveIfError( iReader->MoveTo( tagsEndOffset ) );
        }
    if ( aIfdType == EIfd1 )
        {
        ParseThumbnailL();
        }
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() returning" ) );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ParseJpegDataL
// Parses the Jpeg primary image data and initializes the Jpeg primary image 
// structures.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ParseJpegDataL()
    {
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() entering" ) );
    if ( ( !iExifCore ) || ( !iReader ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leave( KErrGeneral )" ) );
        User::Leave( KErrGeneral );
        }

    TUint32 jpgStartOffset = 0;
    TUint32 jpgStartOffset2 = 0;
    TUint32 jpgEndOffset = 0;
    TUint32 jpgEndOffset2 = 0;
    TUint32 jpgSofOffset = 0;

    // 1. Find the last 2 EOI markers.
    if ( iReader->LocateJpegMarkerFromEnd( KEoi, jpgEndOffset ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leave( KErrCorrupt )LocateJpegMarkerFromEnd" ) );
        User::Leave( KErrCorrupt );
        }

    if ( iExifReadOption & CExifRead::EFastJpegParsing )
        {
        // Read current position after exif header parsing
    	iReader->CurrentPosOffset(jpgStartOffset);
    	
        // add start offset by 2 if exif header was not in image
        // SOI is in 0 position
        if ( !jpgStartOffset && jpgEndOffset )
            {
            jpgStartOffset += 2;
            }
        }
    else
        { // Scan the whole jpeg image data
        
        iReader->LocateJpegMarkerFromEnd( KEoi, jpgEndOffset2, 0, jpgEndOffset );

        // Assumption: There might be additional redundant EOI markers after the 
        // real EOI. Difference between two EOI marker is assumed to be 150 bytes.
        while( jpgEndOffset - jpgEndOffset2 < 150 ) 
            {
            jpgEndOffset = jpgEndOffset2;
            jpgEndOffset2 = 0;
            iReader->LocateJpegMarkerFromEnd( 
                KEoi, jpgEndOffset2, 0, jpgEndOffset );
            }

        // 2. Find the SOF0 marker between the EOI markers.
        if ( iReader->LocateJpegMarker( 
            KSof0, jpgSofOffset, jpgEndOffset2, jpgEndOffset ) )
            {
            LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leave( KErrNotSupported )LocateJpegMarker" ) );
            User::Leave( KErrNotSupported );
            }

        jpgStartOffset = jpgSofOffset;

        if ( (jpgStartOffset >= jpgEndOffset ) || 
            ( jpgStartOffset < jpgEndOffset2 ) )
            {
            LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() Leave( KErrNotSupported )jpgStartOffset" ) );
            User::Leave( KErrNotSupported );
            }

        // 3. Find other Jpeg markers between the 2nd last EOI and the last 
        // SOF0 marker.
        if ( iReader->LocateJpegMarker( 
            KDqt, jpgStartOffset2, jpgEndOffset2, jpgStartOffset ) == KErrNone)
            {
            if ( jpgStartOffset2 < jpgStartOffset )
                {
                jpgStartOffset = jpgStartOffset2;
                }
            }

        if ( iReader->LocateJpegMarker( 
            KDht, jpgStartOffset2, jpgEndOffset2, jpgStartOffset ) == KErrNone)
            {
            if ( jpgStartOffset2 < jpgStartOffset )
                {
                jpgStartOffset = jpgStartOffset2;
                }
            }

        if ( iReader->LocateJpegMarker( 
            KDri, jpgStartOffset2, jpgEndOffset2, jpgStartOffset ) == KErrNone)
            {
            if ( jpgStartOffset2 < jpgStartOffset )
                {
                jpgStartOffset = jpgStartOffset2;
                }
            }
        }
    // Should include the EOI marker.
    jpgEndOffset += 2;

    iExifCore->SetJpgOffsets( jpgStartOffset, jpgEndOffset );
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseIfdL() returning" ) );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ParseThumbnailL
// Parses the Jpeg thumbnail image data and initializes the Jpeg thumbnail 
// structures.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ParseThumbnailL()
    {
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() entering" ) );
    if ( ( !iExifCore ) || ( !iReader ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() Leave( KErrGeneral )" ) );
        User::Leave( KErrGeneral );
        }

    TUint32 thumbnailStart = 0;
    if ( iExifCore->GetThumbnailTagData( KIdJpegInterchangeFormat, 
        thumbnailStart ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() Leave( KErrCorrupt )GetThumbnailTagData1" ) );
        User::Leave( KErrCorrupt );
        }
    TUint32 thumbnailSize = 0;
    if ( iExifCore->GetThumbnailTagData( KIdJpegInterchangeFormatLength, 
        thumbnailSize ) )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() Leave( KErrCorrupt )GetThumbnailTagData2" ) );
        User::Leave( KErrCorrupt );
        }
    
    LOGTEXT3( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() thumbnailStart=%d, thumbnailSize=%d" ), thumbnailStart, thumbnailSize );
    if ( thumbnailSize > KMaxApp1Size )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() Leave( KErrCorrupt ) thumbnailSize > KMaxApp1Size" ) );
        User::Leave( KErrCorrupt );
        }

    iReader->MoveTo( thumbnailStart );
    HBufC8* buffer = HBufC8::NewLC( thumbnailSize );
    iReader->CopyBuffer( thumbnailSize, 1, buffer );
    iExifCore->InsertThumbnailL( buffer );
    CleanupStack::Pop();
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ParseThumbnailL() returning" ) );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ReadTagHeader
// Reads the tag header, and returns the header information in the parameters.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::ReadTagHeader( 
    TUint16& aTagId, 
    TUint16& aTagType, 
    TUint32& aTagCount ) const
    {
    LOGTEXT4( _L( "ExifLib: CExifReadImpl::ReadTagHeader() entering: aTagId=0x%x, aTagType=0x%x, aTagCount=0x%x" ), aTagId, aTagType, aTagCount );
    if ( !iReader )
        {
        LOGTEXT( _L( "ExifLib: CExifReadImpl::ReadTagHeader() returning KErrGeneral" ) );
        return KErrGeneral;
        }

    TInt error = iReader->ReadUInt16( aTagId );
    if ( error )
        {
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ReadTagHeader() returning aTagId error=%d" ),error );
        return error;
        }
    error = iReader->ReadUInt16( aTagType );
    if ( error )
        {
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ReadTagHeader() returning aTagType error=%d" ),error );
        return error;
        }
    error = iReader->ReadUInt32( aTagCount );
    if ( error )
        {
        LOGTEXT2( _L( "ExifLib: CExifReadImpl::ReadTagHeader() returning aTagCount error=%d" ),error );
        return error;
        }
    LOGTEXT( _L( "ExifLib: CExifReadImpl::ReadTagHeader() returning" ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetImageDescriptionL
// Gets the Image Description tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetImageDescriptionL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdImageDescription );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetMakeL
// Gets the Make tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetMakeL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdMake );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetModelL
// Gets the Model tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetModelL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdModel );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetOrientation
// Gets the Orientation tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetOrientation( TUint16& aOrientation ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdOrientation, aOrientation );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetXResolution
// Gets the X Resolution tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetXResolution( 
    TUint32& aXResolutionNumerator, 
    TUint32& aXResolutionDenumerator ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdXResolution, 
        aXResolutionNumerator, aXResolutionDenumerator );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetYResolution
// Gets the Y Resolution tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetYResolution( 
    TUint32& aYResolutionNumerator, 
    TUint32& aYResolutionDenumerator ) const        
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdYResolution, 
        aYResolutionNumerator, aYResolutionDenumerator );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetResolutionUnit
// Gets the Resolution Unit tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetResolutionUnit( TUint16& aResolutionUnit ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdResolutionUnit, aResolutionUnit );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetTransferFunctionL
// Gets the Transfer Function tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetTransferFunctionL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdTransferFunction );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetDateTimeL
// Gets the Date Time tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetDateTimeL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdDateTime );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetYCbCrPositioning
// Gets the YCbCr Positioning tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetYCbCrPositioning( TUint16& aYCbCrPositioning ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdYCbCrPositioning, aYCbCrPositioning );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetSoftwareL
// Gets the Software tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetSoftwareL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdSoftware );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetCopyrightL
// Gets the Copyright tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetCopyrightL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdCopyright );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExifIfdPointer
// Gets the Exif Ifd Pointer tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExifIfdPointer( TUint32& aExifIfdPointer ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExifIfdPointer, aExifIfdPointer );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetGpsInfoIfdPointer
// Gets the Gps Ifd Pointer tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetGpsInfoIfdPointer( TUint32& aGpsInfoIfdPointer ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdGpsIfdPointer, aGpsInfoIfdPointer );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExposureTime
// Gets the Exposure Time tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExposureTime( 
    TUint32& aExposureTime1, 
    TUint32& aExposureTime2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExposureTime, 
        aExposureTime1, aExposureTime2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetComponentsConfiguration
// Gets the Components Configuration tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetComponentsConfiguration( 
    TUint8& aFirstComponent, TUint8& aSecondComponent,
    TUint8& aThirdComponent, TUint8& aFourthComponent) const 
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    TUint32 componentsConfiguration = 0;

    TInt error = iExifCore->GetTagData( KIdComponentsConfiguration, 
        componentsConfiguration );
    if ( error )
        {
        return error;
        }
    aFirstComponent = STATIC_CAST ( TUint8, 
        ( KOneByteMask & componentsConfiguration ) );
    aSecondComponent = STATIC_CAST ( TUint8, 
        ( KOneByteMask & ( componentsConfiguration >> 8 ) ) );
    aThirdComponent = STATIC_CAST ( TUint8, 
        ( KOneByteMask & ( componentsConfiguration >> 16 ) ) );
    aFourthComponent = STATIC_CAST ( TUint8, 
        ( KOneByteMask & ( componentsConfiguration >> 24 ) ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetFlash
// Gets the Flash tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetFlash( TUint16& aFlash ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdFlash, aFlash );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetColorSpace
// Gets the Color Space tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetColorSpace( TUint16& aColorSpace ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdColorSpace, aColorSpace );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetPixelXDimension
// Gets the Pixel X Dimension tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetPixelXDimension( TUint32& aPixelXDimension ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdPixelXDimension, aPixelXDimension );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetPixelYDimension
// Gets the Pixel Y Dimension tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetPixelYDimension( TUint32& aPixelYDimension ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdPixelYDimension, aPixelYDimension );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExposureMode
// Gets the Exposure Mode tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExposureMode( TUint16& aExposureMode ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExposureMode, aExposureMode );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetWhiteBalance
// Gets the White Balance tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetWhiteBalance( TUint16& aWhiteBalance ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdWhiteBalance, aWhiteBalance );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetSceneCaptureType
// Gets the Scene Capture Type tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetSceneCaptureType( TUint16& aSceneCaptureType ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdSceneCaptureType, aSceneCaptureType );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExposureProgram
// Gets the Exposure Program tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExposureProgram( TUint16& aExposureProgram ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExposureProgram, aExposureProgram );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetIsoSpeedRatingsL
// Gets the Iso Speed Ratings tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetIsoSpeedRatingsL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdIsoSpeedRatings );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetDateTimeOriginalL
// Gets the Date Time Original tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetDateTimeOriginalL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdDateTimeOriginal );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetDateTimeDigitizedL
// Gets the Date Time Digitized tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetDateTimeDigitizedL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdDateTimeDigitized );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetApertureValue
// Gets the Aperture Value tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetApertureValue( 
    TUint32& aApertureValue1, 
    TUint32& aApertureValue2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdApertureValue, 
        aApertureValue1, aApertureValue2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExposureBiasValue
// Gets the Exposure Bias Value tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExposureBiasValue( 
    TInt32& aExposureBiasValue1, 
    TInt32& aExposureBiasValue2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExposureBiasValue, 
        aExposureBiasValue1, aExposureBiasValue2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetMeteringMode
// Gets the Metering Mode tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetMeteringMode( TUint16& aMeteringMode ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdMeteringMode, aMeteringMode );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetLightSource
// Gets the Light Source tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetLightSource( TUint16& aLightSource ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdLightSource, aLightSource );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetMakerNoteL
// Gets the Maker Note tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetMakerNoteL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdMakerNote );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetUserCommentL
// Gets the User Comment tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetUserCommentL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdUserComment );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetRelatedSoundFileL
// Gets the Related Sound File tag data.
// -----------------------------------------------------------------------------
//
HBufC8* CExifReadImpl::GetRelatedSoundFileL() const

    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    return iExifCore->GetTagDataL( KIdRelatedSoundFile );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetFileSource
// Gets the File Source tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetFileSource( TInt8& aFileSource ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdFileSource, aFileSource );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetDigitalZoomRatio
// Gets the Digital Zoom Ratio tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetDigitalZoomRatio( 
    TUint32& aDigitalZoomRatio1, 
    TUint32& aDigitalZoomRatio2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdDigitalZoomRatio, 
        aDigitalZoomRatio1, aDigitalZoomRatio2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetContrast
// Gets the Contrast tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetContrast( TUint16& aContrast ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdContrast, aContrast );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetSaturation
// Gets the Saturation tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetSaturation( TUint16& aSaturation ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdSaturation, aSaturation );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetSharpness
// Gets the Sharpness tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetSharpness( TUint16& aSharpness ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdSharpness, aSharpness );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExifVersion
// Gets the Exif Version tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetExifVersion( TUint32& aExifVersion ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdExifVersion, aExifVersion );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetFlashPixVersion
// Gets the Flash Pix Version tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetFlashPixVersion( TUint32& aFlashPixVersion ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdFlashPixVersion, aFlashPixVersion );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetInteroperabilityIfdPointer
// Gets the Interoperability Ifd Pointer tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetInteroperabilityIfdPointer( 
    TUint32& aInteroperabilityIfdPointer ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdIntOpIfdPointer, 
        aInteroperabilityIfdPointer );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetThumbnailXResolution
// Gets the X Resolution tag data of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetThumbnailXResolution( 
    TUint32& aXResolution1, 
    TUint32& aXResolution2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdXResolution, 
        aXResolution1, aXResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetThumbnailYResolution
// Gets the Y Resolution tag data of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetThumbnailYResolution( 
    TUint32& aYResolution1, 
    TUint32& aYResolution2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdYResolution, 
        aYResolution1, 
        aYResolution2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetThumbnailResolutionUnit
// Gets the ResolutionUnit tag data of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetThumbnailResolutionUnit( TUint16& aResolutionUnit ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdResolutionUnit, aResolutionUnit );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetThumbnailCompression
// Gets the Compression tag data of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetThumbnailCompression( TUint16& aCompression ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdCompression, aCompression );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetJpegInterchangeFormat
// Gets the Jpeg Interchange Format of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetJpegInterchangeFormat( 
    TUint32& aJpegInterchangeFormat ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdJpegInterchangeFormat, 
        aJpegInterchangeFormat );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetJpegInterchangeFormatLength
// Gets the Jpeg Interchange Format Length of thumbnail.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetJpegInterchangeFormatLength( 
    TUint32& aJpegInterchangeFormatLength ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetThumbnailTagData( KIdJpegInterchangeFormatLength, 
        aJpegInterchangeFormatLength );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetExifAppSegmentL
// Returns a copy of whole Exif APP1 segment in a descriptor.
// -----------------------------------------------------------------------------
HBufC8* CExifReadImpl::GetExifAppSegmentL() const
    {
    if ( !iExifCore )
        {
        User::Leave( KErrGeneral );
        }

    User::LeaveIfError( iExifCore->Finalize() );    

    if ( !iExifCore->IsValid() )
        {
        User::Leave( KErrNotReady );
        }

    TUint size = iExifCore->App1Size();
    size+=2;
    // Allocation amount is even for 2-byte alignment
    HBufC8* exifData = HBufC8::NewLC( size + ( size % 2 ) );
    exifData->Des().SetLength( size + ( size % 2 ) );
    TUint pos = 0;
    iExifCore->WriteExifDataL( exifData, pos );
    exifData->Des().SetLength( size );
    CleanupStack::Pop( exifData );
    return exifData;
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetShutterSpeedValue
// Gets the Shutter Speed Value tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetShutterSpeedValue( 
    TInt32& aShutterSpeedValue1, 
    TInt32& aShutterSpeedValue2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdShutterSpeedValue, aShutterSpeedValue1, 
        aShutterSpeedValue2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetBrightnessSpeedValue
// Gets the Brightness Value tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetBrightnessValue( 
    TInt32& aBrightnessValue1, 
    TInt32& aBrightnessValue2 ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdBrightnessValue, aBrightnessValue1, 
        aBrightnessValue2 );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetCustomRendered
// Gets the Custom Rendered tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetCustomRendered( TUint16& aCustomRendered ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdCustomRendered, aCustomRendered );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetGainControl
// Gets the Gain Control tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetGainControl( TUint16& aGainControl ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdGainControl, aGainControl );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::GetGpsVersion
// Gets the Gps Version tag data.
// -----------------------------------------------------------------------------
//
TInt CExifReadImpl::GetGpsVersion( TUint32& aGpsVersion ) const
    {
    if ( !iExifCore )
        {
        return KErrGeneral;
        }

    return iExifCore->GetTagData( KIdGpsVersion, aGpsVersion );
    }

// -----------------------------------------------------------------------------
// CExifReadImpl::ParseJpegDataL
// Parses the Jpeg primary image data and initializes the Jpeg primary image 
// structures.
// -----------------------------------------------------------------------------
//
void CExifReadImpl::ParseJpegDataL( const TDesC8& aJpegData )
    {
    if ( !iReader )
        {
        iReader = CExifEndianBase::NewBaseL( aJpegData.Ptr(), aJpegData.Length(), 
            EFalse );
        }
    ParseJpegDataL();
    delete iReader;
    iReader = NULL;
    }
