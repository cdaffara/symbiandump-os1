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
* Description:  The core service class for handling Exif v2.2 File Format.
*
*/


// INCLUDE FILES
#include "ExifIfd.h"
#include "ExifCore.h"
#include "ExifReadImpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifCore::CExifCore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifCore::CExifCore()
    {
    }

// -----------------------------------------------------------------------------
// CExifCore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifCore::ConstructL()
    {
    CreateIfdL( EIfd0 );
    CreateIfdL( EIfdExif);
    }

// -----------------------------------------------------------------------------
// CExifCore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifCore* CExifCore::NewL()
    {
    CExifCore* self = new( ELeave ) CExifCore();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();    
    return self;
    }

// Destructor
CExifCore::~CExifCore()
    {
    for ( TUint i = 0; i < KIfdNo; ++i )
        {
        if ( iIfdArray[i] )
            {
            delete iIfdArray[i];
            }
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::InsertTagL
// Inserts the given tag into the specified IFD structure.
// -----------------------------------------------------------------------------
//
void CExifCore::InsertTagL( TExifIfdType aIfdType, CExifTagImpl* aExifTag, TBool aCheckValidity )
    {
    LOGTEXT3( _L( "ExifLib: CExifCore::InsertTagL() entering: aIfdType=0x%x, aCheckValidity=%d" ), aIfdType, aCheckValidity);
    switch ( aIfdType )
        {
        case EIfd0:
        case EIfdExif:
        case EIfd1:
        case EIfdGps:
        case EIfdIntOp:
            if ( aCheckValidity && ( !TagIsValid( aExifTag->TagInfo(), aIfdType ) ) )
                {
                LOGTEXT( _L( "ExifLib: CExifCore::InsertTagL() Leaving: KErrNotSupported" ));
                User::Leave( KErrNotSupported );
                }
            if ( !iIfdArray[aIfdType] )
                {
                // Since a new IFD should be created, 6 bytes more than tag
                // size is required. 
                // Check if it exceeds 64K bytes after insertion.
                if ( App1Size() + aExifTag->Size() + 6 > KMaxApp1Size )
                    {
                    LOGTEXT( _L( "ExifLib: CExifCore::InsertTagL() Leaving: KErrOverflow (>KMaxApp1Size)1" ));
                    User::Leave( KErrOverflow );
                    }
                CreateIfdL( aIfdType ); 
                TRAPD( error, iIfdArray[aIfdType]->InsertTagL( aExifTag, aCheckValidity ) );
                if ( error != KErrNone )
                    {
                    delete( iIfdArray[aIfdType] );
                    iIfdArray[aIfdType] = 0;
                    LOGTEXT2( _L( "ExifLib: CExifCore::InsertTagL() Leaving: error=%d" ), error );
                    User::Leave( error );
                    }
                }
            else
                {
                // Check if it exceeds 64K bytes after insertion.
                if ( !( TagExists( aExifTag->Id(), aIfdType ) ) && 
                    ( App1Size() + aExifTag->Size() > KMaxApp1Size ) )
                    {
                    LOGTEXT( _L( "ExifLib: CExifCore::InsertTagL() Leaving: KErrOverflow (>KMaxApp1Size)2" ));
                    User::Leave( KErrOverflow );
                    }
                iIfdArray[aIfdType]->InsertTagL( aExifTag, aCheckValidity );
                }
            break;
        default:
            {
            LOGTEXT( _L( "ExifLib: CExifCore::InsertTagL() Leaving: KErrArgument" ));
            User::Leave( KErrArgument );
            }
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given 8-bit integer to the data of tag specified by the given tag ID
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( TUint16 aTagId, TInt8 aTagData )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagUndefined;
    // Total tag data size = 1 bytes.
    TUint32 tagCount = 1;
    TExifIfdType ifdType = EIfdExif;
    HBufC8* buffer = HBufC8::NewL( 1 );
    CleanupStack::PushL( buffer );
    buffer->Des().SetLength( 1 );
    switch ( aTagId )
        {
        case KIdFileSource:
            break;
        default:
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::SetTagDataL() Leave( KErrNotSupported ) aTagId=0x%x" ), aTagId);
            User::Leave( KErrNotSupported );
            }
        }
    *REINTERPRET_CAST( TInt8*, CONST_CAST( TUint8*, buffer->Ptr() ) ) 
        = aTagData;
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given 16-bit unsigned integer to the data of a tag specified by the 
// given tag ID.
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( TUint16 aTagId, TUint16 aTagData )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagShort;
    // Total tag data size = 2 bytes.
    // Since tag data type is 2-byte type, tagCount = 1.
    TUint32 tagCount = 1;
    TExifIfdType ifdType = EIfdExif;
    HBufC8* buffer = HBufC8::NewL( 2 );
    buffer->Des().SetLength( 2 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdCompression:
        case KIdOrientation:
        case KIdResolutionUnit:
        case KIdYCbCrPositioning:
            ifdType = EIfd0;
            break;
        case KIdFlash:
        case KIdColorSpace:
        case KIdExposureMode:
        case KIdWhiteBalance:
        case KIdSceneCaptureType:
        case KIdExposureProgram:
        case KIdMeteringMode:
        case KIdLightSource:
        case KIdContrast:
        case KIdSaturation:
        case KIdSharpness:
        case KIdCustomRendered:
        case KIdGainControl:
            break;
        default:
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::SetTagDataL() Leave( KErrNotSupported ) 16 aTagId=0x%x" ), aTagId);
            User::Leave( KErrNotSupported );
            }
        }
    TExifCommon::SetUint16( CONST_CAST( TUint8*, buffer->Ptr() ), aTagData );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given 32-bit integer to the data of tag specified by given tag ID
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( TUint16 aTagId, TUint32 aTagData )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagLong;

    // Total tag data size = 4 bytes.
    // If the tag data type is one of 4-byte types, tagCount = 1.
    // If the tag data type is one of 1-byte types, tagCount = 4.
    TUint32 tagCount = 1;
    TExifIfdType ifdType = EIfdExif;
    HBufC8* buffer = HBufC8::NewL(4 ); 
    buffer->Des().SetLength(4 );
    
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdExifIfdPointer:
        case KIdGpsIfdPointer:
            ifdType = EIfd0;
            break;
        case KIdIntOpIfdPointer:
        case KIdPixelXDimension:
        case KIdPixelYDimension:
            break;
        case KIdComponentsConfiguration:
        case KIdExifVersion:
        case KIdFlashPixVersion:
            tagType = CExifTag::ETagUndefined;
            tagCount = 4;
            break;
        case KIdGpsVersion:
            tagType = CExifTag::ETagByte;
            tagCount = 4;
            ifdType = EIfdGps;
            break;
        default:
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::SetTagDataL() Leave( KErrNotSupported ) 32 aTagId=0x%x" ), aTagId);
            User::Leave( KErrNotSupported );
            }
        }
    TExifCommon::SetUint32( CONST_CAST( TUint8*, buffer->Ptr() ), aTagData );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given data buffer to the data of a tag specified by the given tag ID
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( TUint16 aTagId, const TDesC8& aTagData )
    {
    if ( !aTagData.Length() )
        {
        User::Leave( KErrArgument );
        }
    CExifTag::TExifTagDataType tagType = CExifTag::ETagAscii;
    TUint32 tagCount = aTagData.Length();
    TExifIfdType ifdType = EIfd0;
    switch ( aTagId )
        {
        case KIdImageDescription:
        case KIdMake:
        case KIdModel:
        case KIdSoftware:
        case KIdCopyright:
            break;
        case KIdTransferFunction:
            tagType = CExifTag::ETagShort;
            tagCount /= 2;
            break;
        case KIdDateTime:
            break;
        case KIdIsoSpeedRatings:
            tagType = CExifTag::ETagShort;
            tagCount /= 2;
            ifdType = EIfdExif;
            break;
        case KIdDateTimeOriginal:
        case KIdDateTimeDigitized:
        case KIdRelatedSoundFile:
            ifdType = EIfdExif;
            break;
        case KIdMakerNote:
        case KIdUserComment:
            tagType = CExifTag::ETagUndefined;
            ifdType = EIfdExif;
            break;
        default:
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::SetTagDataL() Leave( KErrNotSupported ) C8 aTagId=0x%x" ), aTagId);
            User::Leave( KErrNotSupported );
            }
        }
    // Check if ASCII string terminates with NULL character.
    // If not add NULL.
    HBufC8* buffer = NULL;
    if ( ( tagType == CExifTag::ETagAscii ) && 
        ( aTagData.Ptr()[aTagData.Length() - 1] ) )
        {
        ++tagCount;
        buffer = HBufC8::NewL( aTagData.Length() + 1 );
        buffer->Des().Copy( aTagData );
        *( CONST_CAST( TUint8*, buffer->Des().Ptr() ) + 
            aTagData.Length() ) = NULL;
        buffer->Des().SetLength( aTagData.Length() + 1 );
        }
    else
        {
        buffer = aTagData.AllocL();
        }
    CleanupStack::PushL( buffer );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop();
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given 64-bit (2 x 32-bit ) unsigned integers to the data of a tag 
// specified by the given tag ID.
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( 
    TUint16 aTagId, 
    TUint32 aNumerator, 
    TUint32 aDenominator )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagRational;

    // Total tag data size = 8 bytes.
    // Since tag data type is 8-byte type, tagCount = 1.
    TUint32 tagCount = 1;
    TExifIfdType ifdType = EIfd0;
    HBufC8* buffer = HBufC8::NewL( 8 );
    buffer->Des().SetLength( 8 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdXResolution:
        case KIdYResolution:
            break;
        case KIdExposureTime:
        case KIdApertureValue:
        case KIdDigitalZoomRatio:
            ifdType = EIfdExif;
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TExifCommon::SetUint32( CONST_CAST( TUint8*, buffer->Ptr() ), aNumerator );
    TExifCommon::SetUint32( 
        CONST_CAST( TUint8*, buffer->Ptr() + 4 ), aDenominator );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetTagDataL
// Sets the given 64-bit (2 x 32-bit ) integers to the data of a tag specified 
// by the given tag ID.
// -----------------------------------------------------------------------------
//
void CExifCore::SetTagDataL( 
    TUint16 aTagId, 
    TInt32 aNumerator, 
    TInt32 aDenominator )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagSrational;
    // Total tag data size = 8 bytes.
    // Since tag data type is 8-byte type, tagCount = 1.
    TUint32 tagCount = 1;
    TExifIfdType ifdType = EIfdExif;
    HBufC8* buffer = HBufC8::NewL( 8 );
    buffer->Des().SetLength( 8 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdExposureBiasValue:
        case KIdShutterSpeedValue:
        case KIdBrightnessValue:
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TExifCommon::SetUint32( CONST_CAST( TUint8*, buffer->Ptr() ), aNumerator );
    TExifCommon::SetUint32( 
        CONST_CAST( TUint8*, buffer->Ptr() + 4 ), aDenominator );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( ifdType, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetThumbnailTagDataL
// Sets the given 16-bit unsigned integer to the data of a thumbnail (IFD1) tag 
// specified by the given tag ID.
// -----------------------------------------------------------------------------
//
void CExifCore::SetThumbnailTagDataL( TUint16 aTagId, TUint16 aTagData )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagShort;
    TUint32 tagCount = 1;
    HBufC8* buffer = HBufC8::NewL( 2 );
    buffer->Des().SetLength( 2 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdCompression:
        case KIdResolutionUnit:
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TExifCommon::SetUint16( CONST_CAST( TUint8*, buffer->Ptr() ), aTagData );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( EIfd1, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetThumbnailTagDataL
// Sets the given 32-bit unsigned integer to the data of a thumbnail (IFD1) tag 
// specified by the given tag ID,
// -----------------------------------------------------------------------------
//
void CExifCore::SetThumbnailTagDataL( TUint16 aTagId, TUint32 aTagData )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagLong;
    TUint32 tagCount = 1;
    HBufC8* buffer = HBufC8::NewL( 4 );
    buffer->Des().SetLength( 4 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdJpegInterchangeFormat:
        case KIdJpegInterchangeFormatLength:
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TExifCommon::SetUint32( CONST_CAST( TUint8*, buffer->Ptr() ), aTagData );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( EIfd1, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::SetThumbnailTagDataL
// Sets the given 64-bit (2 x 32-bit ) unsigned integers to the data of a 
// thumbnail (IFD1) tag specified by the given tag ID.
// -----------------------------------------------------------------------------
//
void CExifCore::SetThumbnailTagDataL( 
    TUint16 aTagId, 
    TUint32 aNumerator, 
    TUint32 aDenominator )
    {
    CExifTag::TExifTagDataType tagType = CExifTag::ETagRational;
    TUint32 tagCount = 1;
    HBufC8* buffer = HBufC8::NewL( 8 );
    buffer->Des().SetLength( 8 );
    CleanupStack::PushL( buffer );
    switch ( aTagId )
        {
        case KIdXResolution:
        case KIdYResolution:
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TExifCommon::SetUint32( CONST_CAST( TUint8*, buffer->Ptr() ), aNumerator );
    TExifCommon::SetUint32( 
        CONST_CAST( TUint8*, buffer->Ptr() + 4 ), aDenominator );
    CExifTagImpl* tag = CExifTagImpl::NewL( aTagId, tagType, tagCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    CleanupStack::PushL( tag );
    InsertTagL( EIfd1, tag, ETrue );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagData
// Gets the 8-bit integer data of a tag specified by the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetTagData( TUint16 aTagId, TInt8& aTagData ) const
    {
    TExifIfdType ifdType = EIfdExif;
    switch ( aTagId )
        {
        case KIdFileSource:
            break;
        default:
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    TPtrC8 tagData = tag->Data();
    aTagData = *REINTERPRET_CAST( TInt8*, CONST_CAST( TUint8*, tagData.Ptr() ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagData
// Gets the 16-bit unsigned integer data of a tag specified by the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetTagData( TUint16 aTagId, TUint16& aTagData ) const
    {
    TExifIfdType ifdType = EIfdExif;
    switch ( aTagId )
        {
        case KIdFlash:
        case KIdColorSpace:
        case KIdExposureMode:
        case KIdWhiteBalance:
        case KIdSceneCaptureType:
        case KIdExposureProgram:
        case KIdMeteringMode:
        case KIdLightSource:
        case KIdContrast:
        case KIdSaturation:
        case KIdSharpness:
        case KIdCustomRendered:
        case KIdGainControl:
            break;
        case KIdOrientation:
        case KIdResolutionUnit:
        case KIdYCbCrPositioning:
            ifdType = EIfd0;
            break;
        default:
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    TPtrC8 tagData = tag->Data();
    TRAP( error, aTagData = TExifCommon::Uint16L( 
        CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagData
// Gets the 32-bit unsigned integer data of a tag specified by the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetTagData( TUint16 aTagId, TUint32& aTagData ) const
    {
    TExifIfdType ifdType = EIfdExif;
    switch ( aTagId )
        {
        case KIdComponentsConfiguration:
        case KIdPixelXDimension:
        case KIdPixelYDimension:
        case KIdExifVersion:
        case KIdFlashPixVersion:
        case KIdIntOpIfdPointer:
            break;
        case KIdGpsVersion:
            ifdType = EIfdGps;
            break;
        case KIdExifIfdPointer:
        case KIdGpsIfdPointer:
            ifdType = EIfd0;
            break;
        default:
            return KErrNotSupported;
        }
    CExifTagImpl* tag = NULL;
    TRAPD( error, tag = 
        CONST_CAST( CExifTagImpl*, GetTagL( ifdType, aTagId ) ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    TPtrC8 tagData = tag->Data();
    if ( tagData.Length() == 2)
        {
        TRAP( error, aTagData = STATIC_CAST( TUint32, 
            TExifCommon::Uint16L( CONST_CAST( TUint8*, tagData.Ptr() ) ) ) );
        }
    else
        {
        TRAP( error, aTagData = TExifCommon::Uint32L( 
            CONST_CAST( TUint8*, tagData.Ptr() ) ) );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagDataL
// Gets the data buffer of a tag specified by the given tag ID.
// -----------------------------------------------------------------------------
//
HBufC8* CExifCore::GetTagDataL( TUint16 aTagId ) const
    {
    TExifIfdType ifdType = EIfd0;
    switch ( aTagId )
        {
        case KIdImageDescription:
        case KIdMake:
        case KIdModel:
        case KIdTransferFunction:
        case KIdDateTime:
        case KIdSoftware:
        case KIdCopyright:
            break;
        case KIdIsoSpeedRatings:
        case KIdDateTimeOriginal:
        case KIdDateTimeDigitized:
        case KIdMakerNote:
        case KIdUserComment:
        case KIdRelatedSoundFile:
            ifdType = EIfdExif;
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    const CExifTagImpl* tag = GetTagL( ifdType, aTagId );
    
    return tag->Data().AllocL();
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagData
// Gets the 64-bit (2 x 32-bit) unsigned integer data of a tag specified by the 
// given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetTagData( 
    TUint16 aTagId, 
    TUint32& aNumerator, 
    TUint32& aDenominator ) const
    {
    TExifIfdType ifdType = EIfd0;
    switch ( aTagId )
        {
        case KIdXResolution:
        case KIdYResolution:
            break;
        case KIdExposureTime:
        case KIdApertureValue:
        case KIdDigitalZoomRatio:
            ifdType = EIfdExif;
            break;
        default:
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    
    TPtrC8 tagData = tag->Data();
    TRAP( error, aNumerator = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    if ( error )
        {
        return error;
        }
    TRAP( error, aDenominator = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, tagData.Ptr() + 4 ) ) );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagData
// Gets the 64-bit (2 x 32-bit) integer data of tag specified by the given tag 
// ID
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetTagData( 
    TUint16 aTagId, 
    TInt32& aNumerator, 
    TInt32& aDenominator ) const
    {
    TExifIfdType ifdType = EIfdExif;
    switch ( aTagId )
        {
        case KIdExposureBiasValue:
        case KIdShutterSpeedValue:
        case KIdBrightnessValue:
            break;
        default:
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    
    TPtrC8 tagData = tag->Data();
    TRAP( error, aNumerator = TExifCommon::Int32L( 
        CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    if ( error ) 
        {
        return error;
        }
    TRAP( error, aDenominator = TExifCommon::Int32L( 
        CONST_CAST( TUint8*, tagData.Ptr() + 4 ) ));
    
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetThumbnailTagData
// Gets the 16-bit unsigned integer data of a thumbnail (IFD1) tag specified by 
// the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetThumbnailTagData( TUint16 aTagId, TUint16& aTagData ) const
    {
    LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData TUint16 entering" ));
    TExifIfdType ifdType = EIfd1;
    switch ( aTagId )
        {
        case KIdResolutionUnit:
        case KIdCompression:
            break;
        default:
            LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: KErrNotSupported" ));
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: KErrNotFound" ));
        return KErrNotFound;
        }
    TPtrC8 tagData = tag->Data();
    TRAP( error, aTagData = TExifCommon::Uint16L( 
        CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    LOGTEXT2( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: error=%d" ), error);
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetThumbnailTagData
// Gets the 32-bit unsigned integer data of a thumbnail (IFD1) tag specified by 
// the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetThumbnailTagData( TUint16 aTagId, TUint32& aTagData ) const
    {
    LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData TUint32 entering" ));
    TExifIfdType ifdType = EIfd1;
    switch ( aTagId )
        {
        case KIdJpegInterchangeFormat:
        case KIdJpegInterchangeFormatLength:
            break;
        default:
            LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: KErrNotSupported" ));
            return KErrNotSupported;
        }
    CExifTagImpl* tag = NULL;
    TRAPD( error, tag = CONST_CAST( CExifTagImpl*, GetTagL( ifdType, aTagId ) ) );
    if ( ( error ) || ( !tag ) )
        {
        LOGTEXT( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: KErrNotFound" ));
        return KErrNotFound;
        }
    TPtrC8 tagData = tag->Data();
    TRAP( error, aTagData = TExifCommon::Uint32L( 
        CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    LOGTEXT2( _L( "ExifLib: CExifCore::GetThumbnailTagData returning: error=%d" ), error);
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetThumbnailTagData
// Gets the 64-bit (2 x 32-bit) unsigned integer data of a thumbnail (IFD1) tag 
// specified by the given tag ID.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetThumbnailTagData( 
    TUint16 aTagId, 
    TUint32& aNumerator, 
    TUint32& aDenominator ) const
    {
    TExifIfdType ifdType = EIfd1;
    switch ( aTagId )
        {
        case KIdXResolution:
        case KIdYResolution:
            break;
        default:
            return KErrNotSupported;
        }
    const CExifTagImpl* tag = NULL;
    TRAPD( error, tag = GetTagL( ifdType, aTagId ) );
    if ( ( error ) || ( !tag ) )
        {
        return KErrNotFound;
        }
    
    TPtrC8 tagData = tag->Data();
    TRAP( error, aNumerator = 
        TExifCommon::Uint32L( CONST_CAST( TUint8*, tagData.Ptr() ) ) );
    if ( error )
        {
        return error;
        }
    TRAP( error, aDenominator = 
        TExifCommon::Uint32L( CONST_CAST( TUint8*, tagData.Ptr() + 4 ) ) );
    
    return error;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagL
// Gets the tag instance having the given tag ID from the specified IFD 
// structure
// -----------------------------------------------------------------------------
//
const CExifTagImpl* CExifCore::GetTagL( 
    TExifIfdType aIfdType, TUint16 aTagId ) const
    {
    switch ( aIfdType )
        {
        case EIfd0:
        case EIfdExif:
        case EIfd1:
        case EIfdGps:
        case EIfdIntOp:
            if ( !iIfdArray[aIfdType] )
                {
                User::Leave( KErrNotFound );
                }
            return iIfdArray[aIfdType]->GetTagL( aTagId );
        default:
            User::Leave( KErrArgument );
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CExifCore::DeleteTag
// Removes the tag instance having the given tag ID from the specified IFD 
// structure
// -----------------------------------------------------------------------------
//
TInt CExifCore::DeleteTag( TExifIfdType aIfdType, TUint16 aTagId )
    {
    switch ( aIfdType )
        {
        case EIfd0:
        case EIfdExif:
        case EIfd1:
        case EIfdGps:
        case EIfdIntOp:
            {
            if ( !iIfdArray[aIfdType] )
                {
                return KErrNotFound;
                }
            TInt error = iIfdArray[aIfdType]->DeleteTag( aTagId );
            if ( error )
                {
                return error;
                }
            if ( iIfdArray[aIfdType]->Size() )
                {
                return KErrNone;
                }
            return DeleteIfd( aIfdType );
            }
        default:
            return KErrArgument;
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::GetTagIdsL
// Gets the tag IDs and the number of tags in the specified IFD structure.
// -----------------------------------------------------------------------------
//
TUint16* CExifCore::GetTagIdsL( TExifIfdType aIfdType, TInt& aNoTags ) const
    {
    switch ( aIfdType )
        {
        case EIfd0:
        case EIfdExif:
        case EIfd1:
        case EIfdGps:
        case EIfdIntOp:
            if ( !iIfdArray[aIfdType] )
                {
                User::Leave( KErrNotFound );
                }
            return iIfdArray[aIfdType]->GetTagIdsL( aNoTags );
        default:
            User::Leave( KErrArgument );
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CExifCore::iExifCore->CreateIfdL
// Instantiates the specified IFD structure.
// -----------------------------------------------------------------------------
//
void CExifCore::CreateIfdL( TExifIfdType aIfdType )
    {
    if ( App1Size() > ( KMaxApp1Size - 6 ) )
        {
        User::Leave( KErrOverflow );
        }
    switch ( aIfdType )
        {
        case EIfd0:
        case EIfdExif:
        case EIfd1:
        case EIfdGps:
        case EIfdIntOp:
            iIfdArray[aIfdType] = CExifIfd::NewBaseL( aIfdType );
            break;
        default:
            User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::DeleteIfd
// Removes the specified IFD structure and all its tags.
// -----------------------------------------------------------------------------
//
TInt CExifCore::DeleteIfd( TExifIfdType aIfdType )
    {
    switch ( aIfdType )
        {
        case EIfd0:
            break;
        case EIfdExif:
            break;
        case EIfd1:
            iIfdArray[EIfd0]->SetNextIfdOffset( 0 );
            break;
        case EIfdGps:
            iIfdArray[EIfd0]->DeleteTag( KIdGpsIfdPointer );
            break;
        case EIfdIntOp:
            iIfdArray[EIfdExif]->DeleteTag( KIdIntOpIfdPointer );
            break;
        default:
            return KErrArgument;
        }
    if ( iIfdArray[aIfdType] )
        {
        delete ( iIfdArray[aIfdType] );
        iIfdArray[aIfdType] = 0;
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::GetIfdTypesL
// Gets the IFD types and number of IFDs in the file format.
// -----------------------------------------------------------------------------
//
TExifIfdType* CExifCore::GetIfdTypesL( TInt& aNoIfd ) const
    {
    TInt noIfd = 0;
    TUint i = 0;
    for ( i = 0; i < KIfdNo; ++i )
        {
        if ( iIfdArray[i] )
            {
            ++noIfd;
            }
        }
    if ( !noIfd )
        {
        User::Leave( KErrNotFound );
        }

    TExifIfdType* ifdTypes = STATIC_CAST( TExifIfdType*, 
        User::AllocL( sizeof( TExifIfdType ) * noIfd ) );
    aNoIfd = noIfd;
    noIfd = 0;
    for ( i=EIfd0; i<=EIfdIntOp; ++i )
        {
        if ( iIfdArray[i] )
            {
            ifdTypes[noIfd] = STATIC_CAST( TExifIfdType, i );
            ++noIfd;
            }
        }
    return ifdTypes;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetThumbnailL
// Gets the Exif thumbnail image data from the IFD1 structure.
// -----------------------------------------------------------------------------
//
HBufC8* CExifCore::GetThumbnailL() const
    {
    if ( !iIfdArray[EIfd1] )
        {
        User::Leave( KErrNotFound );
        }

    TDesC8* thumbnailData = NULL;
    User::LeaveIfError( ( STATIC_CAST( CExifIfd1*, iIfdArray[EIfd1] ) )->
        GetThumbnailData( thumbnailData ) );

    // Next check needed when option ENoTagChecking is used
    if ( !thumbnailData )
        {
        User::Leave( KErrNotFound );
        }

    return thumbnailData->AllocL();
    }

// -----------------------------------------------------------------------------
// CExifCore::InsertThumbnailL
// Inserts/Updates the given Exif thumbnail image data in the IFD1 structure.
// -----------------------------------------------------------------------------
//
void CExifCore::InsertThumbnailL( TDesC8* aThumbnailData )
    {
    if ( STATIC_CAST( TUint, App1Size() + aThumbnailData->Length() ) > 
        KMaxApp1Size )
        {
        User::Leave( KErrOverflow );
        }

    TUint8* jpgStartPtr = CONST_CAST( TUint8*, aThumbnailData->Ptr() );
    TUint8* jpgEndPtr = jpgStartPtr + aThumbnailData->Length();

    jpgStartPtr = TExifCommon::LocateJpegMarkerPtr( 
        KSoi, jpgStartPtr, jpgEndPtr );
    if( !jpgStartPtr )
        {
        User::Leave( KErrCorrupt );
        }

    jpgStartPtr = TExifCommon::LocateJpegMarkerPtr( 
        KSof0, jpgStartPtr, jpgEndPtr );
    if( !jpgStartPtr )
        {
        User::Leave( KErrCorrupt );
        }

    jpgStartPtr = TExifCommon::LocateJpegMarkerPtr( 
        KEoi, jpgStartPtr, jpgEndPtr );
    if( !jpgStartPtr )
        {
        User::Leave( KErrCorrupt );
        }

    if ( !iIfdArray[EIfd1] )
        {
        CreateIfdL( EIfd1 );
        }
    User::LeaveIfError( ( STATIC_CAST( CExifIfd1*, iIfdArray[EIfd1] ) )->
        SetThumbnailData( aThumbnailData ) );
    }

// -----------------------------------------------------------------------------
// CExifCore::RemoveThumbnail
// Removes the Exif thumbnail image data from the IFD1 structure.
// -----------------------------------------------------------------------------
//
TInt CExifCore::RemoveThumbnail()
    {
    if ( !iIfdArray[EIfd1] )
        {
        return KErrNotFound;
        }
    return 
        ( STATIC_CAST( CExifIfd1*, iIfdArray[EIfd1] ) )->RemoveThumbnailData();
    }


// -----------------------------------------------------------------------------
// CExifCore::WriteExifDataL
// Writes the Exif data to the given descriptor starting from the specified 
// position/offset.
// -----------------------------------------------------------------------------
//
void CExifCore::WriteExifDataL( HBufC8*& aExifData, TUint& aPos )
    {
    // Ensure byte alignment
    if ( ( aExifData->Length() - aPos ) % 2 > 0 )
        {
        User::Leave( KErrGeneral );
        }
    TUint16* exifDataPtr = REINTERPRET_CAST( TUint16*, 
        CONST_CAST( TUint8*, aExifData->Ptr() ) + aPos );
    WriteExifHeaderL( exifDataPtr, aPos );
    WriteIfdL( exifDataPtr, EIfd0, aPos );
    WriteIfdL( exifDataPtr, EIfdExif, aPos );
    WriteIfdL( exifDataPtr, EIfdGps, aPos );
    WriteIfdL( exifDataPtr, EIfdIntOp, aPos );
    WriteIfdL( exifDataPtr, EIfd1, aPos );
    }

// -----------------------------------------------------------------------------
// CExifCore::WriteJpegHeaderL
// Writes the Jpeg header to the given descriptor starting from the specified 
// position/offset.
// -----------------------------------------------------------------------------
//
void CExifCore::WriteJpegHeaderL( HBufC8*& aExifData, TUint& aPos )
    {
    if ( ( aExifData->Length() - aPos ) % 2 > 0 )
        {
        User::Leave( KErrGeneral );
        }
    TUint16* exifDataPtr = REINTERPRET_CAST( TUint16*, 
        CONST_CAST( TUint8*, aExifData->Ptr() ) + aPos );
    *exifDataPtr = KSoiRev;
    aPos+=2;
    }

// -----------------------------------------------------------------------------
// CExifCore::Finalize
// Finalizes the Exif data to ensure the validity, updates the internal offsets.
// -----------------------------------------------------------------------------
//
TInt CExifCore::Finalize()
    {
    TInt error = 0;
    TUint32 ifdOffset = FindIfdOffset( EIfdExif );
    TRAP( error, SetTagDataL( KIdExifIfdPointer, ifdOffset ) );
    if ( error )
        {
        return error;
        }

    ifdOffset = FindIfdOffset( EIfdGps );
    if ( ifdOffset )
        {
        TRAP( error, SetTagDataL( KIdGpsIfdPointer, ifdOffset ) );
        if ( error )
            {
            return error;
            }
        }
    else
        {
        DeleteTag( EIfd0, KIdGpsIfdPointer );
        }
    
    ifdOffset = FindIfdOffset( EIfdIntOp );
    if ( ifdOffset )
        {
        TRAP( error, SetTagDataL( KIdIntOpIfdPointer, ifdOffset ) );
        if ( error )
            {
            return error;
            }
        }
    else
        {
        DeleteTag( EIfdExif, KIdIntOpIfdPointer );
        }
    
    if ( iIfdArray[EIfd1] )
        {
        if ( !STATIC_CAST( CExifIfd1*, iIfdArray[EIfd1] )->ThumbnailSize() )
            {
            DeleteIfd( EIfd1 );
            iIfdArray[EIfd0]->SetNextIfdOffset( 0 );
            }
        else
            {
            ifdOffset = FindIfdOffset( EIfd1 );
            iIfdArray[EIfd0]->SetNextIfdOffset( ifdOffset );
            TRAP( error, SetThumbnailTagDataL( KIdJpegInterchangeFormat, 
                STATIC_CAST( TUint32, ifdOffset + iIfdArray[EIfd1]->Size() ) ) );
            if ( error )
                {
                return error;
                }
            TRAP( error, SetThumbnailTagDataL( KIdJpegInterchangeFormatLength, 
                STATIC_CAST( TUint32, STATIC_CAST( CExifIfd1*, 
                iIfdArray[EIfd1] )->ThumbnailSize() ) ) );
            if ( error )
                {
                return error;
                }
            TRAP( error, SetThumbnailTagDataL( KIdCompression, KCompressed ) );
            if ( error )
                {
                return error;
                }
            }
        }
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifCore::TotalSize
// Returns the total size of the Exif file format in bytes.
// -----------------------------------------------------------------------------
//
TUint CExifCore::TotalSize() const
    {
    return ( App1Size() + 4 + JpegSize() );
    }

// -----------------------------------------------------------------------------
// CExifCore::JpegSize
// Returns the size of the Jpeg image in the Exif file format, excluding SOI 
// and APP markers in bytes.
// -----------------------------------------------------------------------------
//
TUint CExifCore::JpegSize() const
    {
    return iJpgEndOffset - iJpgStartOffset;
    }

// -----------------------------------------------------------------------------
// CExifCore::App1Size
// Returns the size of the APP1 marker, which includes the Exif-specific data 
// in bytes
// -----------------------------------------------------------------------------
//
TUint16 CExifCore::App1Size() const
    {
    TUint16 app1Size = 0;
    if ( iIfdArray[EIfd0] )
        {
        app1Size = STATIC_CAST( TUint16, app1Size + iIfdArray[EIfd0]->Size() );
        }
    if ( iIfdArray[EIfdExif] )
        {
        app1Size = 
            STATIC_CAST( TUint16, app1Size + iIfdArray[EIfdExif]->Size() );
        }
    if ( iIfdArray[EIfd1] )
        {
        app1Size = STATIC_CAST( TUint16, app1Size + iIfdArray[EIfd1]->Size() );
        app1Size = STATIC_CAST( TUint16, app1Size + ( STATIC_CAST( CExifIfd1*, 
            iIfdArray[EIfd1] )->ThumbnailSize() ) );
        }
    if ( iIfdArray[EIfdGps] )
        {
        app1Size  = 
            STATIC_CAST( TUint16, app1Size + iIfdArray[EIfdGps]->Size() );
        }
    if ( iIfdArray[EIfdIntOp] )
        {
        app1Size = 
            STATIC_CAST( TUint16, app1Size + iIfdArray[EIfdIntOp]->Size() );
        }
    return STATIC_CAST( TUint16, app1Size + 16 );
    }

// -----------------------------------------------------------------------------
// CExifCore::IsValid
// Checks if the Exif data is in valid Exif v2.2 format and contains all 
// mandatory information.
// -----------------------------------------------------------------------------
//
TBool CExifCore::IsValid() const
    {
    if ( (!iIfdArray[EIfd0] ) || (!iIfdArray[EIfdExif] ) )
        {
        return EFalse;
        }

    if ( !iIfdArray[EIfd0]->IsValid() )
        {
        return EFalse;
        }

    if ( !iIfdArray[EIfdExif]->IsValid() )
        {
        return EFalse;
        }

    if ( iIfdArray[EIfd1] )
        {
        if ( !iIfdArray[EIfd1]->IsValid() )
            {
            return EFalse;
            }
        }
    if ( iIfdArray[EIfdGps] )
        {
        if ( !iIfdArray[EIfdGps]->IsValid() )
            {
            return EFalse;
            }
        TRAPD( error, GetTagL( EIfd0, KIdGpsIfdPointer ) );
        if ( error )
            {
            return EFalse;
            }
        }

    if ( iIfdArray[EIfdIntOp] )
        {
        if ( !iIfdArray[EIfdIntOp]->IsValid() )
            {
            return EFalse;
            }
        TRAPD( error, GetTagL( EIfdExif, KIdIntOpIfdPointer ) );
        if ( error )
            {
            return EFalse;
            }
        }

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CExifCore::TagIsValid
// Checks if the specified tag information conforms to the Full Validity 
// characteristics (Data count is correct, data type matches tag ID and data 
// value falls into the predefined range).
// -----------------------------------------------------------------------------
//
TBool CExifCore::TagIsValid( TExifTagInfo aTagInfo, TExifIfdType aIfdType ) const
    {
    LOGTEXT3( _L( "ExifLib: CExifCore::TagIsValid() entering: aIfdType=0x%x, aTagInfo.iId=0x%x" ), aIfdType, aTagInfo.iId );
    TInt noTags = 0;
    const TReferenceTag* tags = NULL;
    switch ( aIfdType )
        {
        case EIfd0:
            noTags = KNoIfd0Tags;
            tags = ifd0Tags;
            break;
        case EIfdExif:
            noTags = KNoIfdExifTags;
            tags = ifdExifTags;
            break;
        case EIfd1:
            noTags = KNoIfd1Tags;
            tags = ifd1Tags;
            break;
        case EIfdGps:
            noTags = KNoIfdGpsTags;
            tags = ifdGpsTags;
            break;
        case EIfdIntOp:
            noTags = KNoIfdIntOpTags;
            tags = ifdIntOpTags;
            break;
        default:
            return EFalse;
        }

    TInt k = 0;
    TBool found = EFalse;
    for ( k = 0; ( k < noTags ) && ( !found ); ++k )
        {
        if ( tags[k].iId == aTagInfo.iId )
            {
            found = ETrue;
            }
        }

    if ( !found )
        {
        LOGTEXT( _L( "ExifLib: CExifCore::TagIsValid() returning EFalse (tag not found)" ));
        return EFalse;
        }
    // k locates the item next to the found item. Make it locate the found.
    --k;

    if ( tags[k].iDataType != TReferenceTag::ETagLongOrShort )
        {
        if ( tags[k].iDataType != aTagInfo.iDataType )
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::TagIsValid() returning EFalse 1 aTagInfo.iDataType=0x%x" ), aTagInfo.iDataType );
            return EFalse;
            }
        }
    else if ( ( aTagInfo.iDataType != CExifTag::ETagShort ) && 
        ( aTagInfo.iDataType != CExifTag::ETagLong ) )
        {
        LOGTEXT2( _L( "ExifLib: CExifCore::TagIsValid() returning EFalse 2 aTagInfo.iDataType=0x%x" ), aTagInfo.iDataType );
        return EFalse;
        }
    else
        {
        // Nothing to check for this case!
        }

    if ( tags[k].iDataCount != KAny )
        {
        if ( tags[k].iDataCount != aTagInfo.iDataCount )
            {
            LOGTEXT2( _L( "ExifLib: CExifCore::TagIsValid() returning EFalse aTagInfo.iDataCount=0x%x" ), aTagInfo.iDataCount );
            return EFalse;
            }
        }

    LOGTEXT( _L( "ExifLib: CExifCore::TagIsValid() returning ETrue" ));
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CExifCore::IfdExists
// Checks if the specified IFD structure exists in the Exif data.
// -----------------------------------------------------------------------------
//
TBool CExifCore::IfdExists( TExifIfdType aIfdType ) const
    {
    if ( iIfdArray[aIfdType] )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CExifCore::TagExists
// Checks if the tag having the given tag ID exists in the specified IFD 
// structure
// -----------------------------------------------------------------------------
//
TBool CExifCore::TagExists( TUint16 aTagId, TExifIfdType aIfdType ) const
    {
    if ( iIfdArray[aIfdType] )
        {
        return iIfdArray[aIfdType]->TagExists( aTagId );
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CExifCore::SetJpgOffsets
// Sets the given Jpeg data start and end offsets.
// -----------------------------------------------------------------------------
//
void CExifCore::SetJpgOffsets( TUint32 aJpgStartOffset, TUint32 aJpgEndOffset )
    {
    iJpgStartOffset = aJpgStartOffset;
    iJpgEndOffset = aJpgEndOffset;
    }

// -----------------------------------------------------------------------------
// CExifCore::GetJpegData
// Gets the pure Jpeg image data excluding the SOI and APP1 markers.
// -----------------------------------------------------------------------------
//
TInt CExifCore::GetJpegData( TPtr8 aJpgPointer ) const
    {
    if ( !iJpgStartOffset )
        {
        return KErrGeneral;
        }

    TUint8* jpgStartPtr = CONST_CAST( TUint8*, iDataStartPtr + iJpgStartOffset );
    if ( *jpgStartPtr != KMarkerStart )
        {
        return KErrGeneral;
        }
    aJpgPointer.Copy( jpgStartPtr, JpegSize() );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifCore::SetDataStartPtr
// Sets the Exif data start pointer.
// -----------------------------------------------------------------------------
//
void CExifCore::SetDataStartPtr( const TUint8* aPtr )
    {
    iDataStartPtr = aPtr;
    }

// -----------------------------------------------------------------------------
// CExifCore::WriteExifHeaderL
// Writes the Exif header to the location, which is defined by the given pointer 
// and the offset.
// -----------------------------------------------------------------------------
//
void CExifCore::WriteExifHeaderL( TUint16*& aExifDataPtr, TUint& aPos )
    {
    if ( !aExifDataPtr )
        {
        User::Leave( KErrGeneral );
        }
    *aExifDataPtr++ = KApp1Rev;
    TUint16 app1Size = App1Size();
    *REINTERPRET_CAST( TUint8*, aExifDataPtr ) = STATIC_CAST( TUint8, 
        app1Size >> 8 ); 
    *( REINTERPRET_CAST( TUint8*, aExifDataPtr ) + 1 ) = STATIC_CAST( 
        TUint8, app1Size ); 
    ++aExifDataPtr;
    TExifCommon::SetUint32( REINTERPRET_CAST( TUint8*, aExifDataPtr ), 
        KExifIdentifierRev );
    aExifDataPtr+=2;
    *aExifDataPtr++ = KExifPad;
    *aExifDataPtr++ = KLittleEndian;
    *aExifDataPtr++ = KExifDummyRev;
    TExifCommon::SetUint32( REINTERPRET_CAST( TUint8*, aExifDataPtr ), 
        KHeaderOffset );
    aExifDataPtr+=2;
    aPos = 20;
    }

// -----------------------------------------------------------------------------
// CExifCore::WriteIfdl
// Writes the specified IFD data to the location, which is defined by the given 
// pointer and the offset.
// -----------------------------------------------------------------------------
//
void CExifCore::WriteIfdL( 
    TUint16*& aExifDataPtr, 
    TExifIfdType aIfdType, 
    TUint& aPos )
    {
    if ( !aExifDataPtr )
        {
        User::Leave( KErrGeneral );
        }
    if ( iIfdArray[aIfdType] )
        {
        iIfdArray[aIfdType]->WriteTagsL( aExifDataPtr, aPos );
        if ( aIfdType == EIfd1 )
            {
            (STATIC_CAST( CExifIfd1*, iIfdArray[aIfdType] ) )->WriteThumbnailL( 
                REINTERPRET_CAST( TUint8*&, aExifDataPtr ), aPos );
            }
        }
    }

// -----------------------------------------------------------------------------
// CExifCore::FindIfdOffset
// Returns the offset of the specified IFD structure in the Exif data.
// -----------------------------------------------------------------------------
//
TUint32 CExifCore::FindIfdOffset( TExifIfdType aIfdType ) const
    {
    if ( !iIfdArray[aIfdType] )
        {
        return 0;
        }
    TUint32 offset = 8;
    switch ( aIfdType )
        {
        case EIfd0:
            break;
        case EIfdExif:
            offset += ( iIfdArray[EIfd0]->Size() );
            break;
        case EIfd1:
            offset += ( iIfdArray[EIfd0]->Size() + iIfdArray[EIfdExif]->Size() );
            if ( iIfdArray[EIfdGps] )
                {
                offset += iIfdArray[EIfdGps]->Size();
                }
            if ( iIfdArray[EIfdIntOp] )
                {
                offset += iIfdArray[EIfdIntOp]->Size();
                }
            break;
        case EIfdGps:
            offset += ( iIfdArray[EIfd0]->Size() + iIfdArray[EIfdExif]->Size() );
            break;
        case EIfdIntOp:
            offset += ( iIfdArray[EIfd0]->Size() + iIfdArray[EIfdExif]->Size() );
            if ( iIfdArray[EIfdGps] )
                {
                offset += iIfdArray[EIfdGps]->Size();
                }
            break;
        default:
            return 0;
        }
    return offset;
    }

