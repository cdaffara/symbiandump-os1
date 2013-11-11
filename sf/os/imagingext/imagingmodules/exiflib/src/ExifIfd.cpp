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
* Description:  Exif IFD structure handling classes.
*
*/


// INCLUDE FILES
#include "ExifRead.h"
#include "ExifTagImpl.h"
#include "ExifIfd.h"
#include "ExifCommon.h"
#include "ExifValueTable.h"

// ============================ CLASS CExifIfd =================================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfd::CExifIfd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfd::CExifIfd() : iTagArray( sizeof( CExifTagImpl* ) )
    {
    }

// -----------------------------------------------------------------------------
// CExifIfd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfd::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CExifIfd::NewBaseL
// Two-phased constructor. Instantiates and returns the relevant derived class.
// -----------------------------------------------------------------------------
//
CExifIfd* CExifIfd::NewBaseL( TExifIfdType aIfdType )
    {
    switch ( aIfdType )
        {
        case EIfd0:
            return CExifIfd0::NewL();
        case EIfdExif:
            return CExifIfdExif::NewL();
        case EIfd1:
            return CExifIfd1::NewL();
        case EIfdGps:
            return CExifIfdGps::NewL();
        case EIfdIntOp:
            return CExifIfdIntOp::NewL();
        default:
            User::Leave( KErrGeneral );
        }
    return NULL;
    }

// Destructor
CExifIfd::~CExifIfd()
    {
    iTagArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CExifIfd::IsValid
// Checks if the IFD is in valid format, and contains all mandatory tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfd::IsValid() const
    {
    if ( iTagArray.Count() )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CExifIfd::TagExists
// Checks if specified tag exists in the IFD.
// -----------------------------------------------------------------------------
//
TBool CExifIfd::TagExists( TUint16 aTagId ) const
    {
    for ( TInt i = 0; i < iTagArray.Count(); ++i )
        {
        if ( aTagId == iTagArray[i]->Id() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CExifIfd::Size
// Returns the size of the IFD structure in bytes.
// -----------------------------------------------------------------------------
//
TUint16 CExifIfd::Size() const
    {
    if ( !iTagArray.Count() )
        {
        return 0;
        }
    // Size of the tags + 6 bytes (the next ifd offset and number of tags info)
    return STATIC_CAST( TUint16, iSize + 6 ); 
    }

// -----------------------------------------------------------------------------
// CExifIfd::SetNextIfdOffset
// Sets the next IFD offset.
// -----------------------------------------------------------------------------
//
void CExifIfd::SetNextIfdOffset( TUint32 aIfdOffset )
    {
    iNextIfdOffset = aIfdOffset;
    }

// -----------------------------------------------------------------------------
// CExifIfd::WriteTagsL
// Writes the tag data in the IFD structure to the location defined by the 
// given pointer and the offset.
// -----------------------------------------------------------------------------
//
void CExifIfd::WriteTagsL( TUint16*& aExifDataPtr, TUint& aPos ) const
    {
    if ( !iTagArray.Count() )
        {
        User::Leave( KErrNotReady );
        }
    *aExifDataPtr++ = STATIC_CAST( TUint16, iTagArray.Count() );
    TUint16* ifdForwardPtr = aExifDataPtr + ( iTagArray.Count() * 6 ) + 2;
    TUint32 fwdPos = ( ( iTagArray.Count() * 12 ) - 6 ) + aPos;
    for ( TInt i = 0; i < iTagArray.Count(); ++i )
        {
        CExifTagImpl* tag = iTagArray[i];
        *aExifDataPtr++ = tag->Id();
        CExifTag::TExifTagDataType tagType = tag->DataType();
        *aExifDataPtr++ = STATIC_CAST( TUint16, tagType );
        TUint32 count = tag->DataCount();
        TExifCommon::SetUint32( REINTERPRET_CAST( TUint8*, aExifDataPtr ), 
            count );
        aExifDataPtr += 2;
        TUint noBytes = 0;
        if ( ( tagType == CExifTag::ETagByte ) || 
            ( tagType == CExifTag::ETagAscii ) || 
            ( tagType == CExifTag::ETagUndefined ) )
            {
            noBytes = 1;
            }
        else if ( ( tagType == CExifTag::ETagLong ) || 
            ( tagType == CExifTag::ETagSlong ) )
            {
            noBytes = 4;
            }
        else if ( ( tagType == CExifTag::ETagRational ) || 
            ( tagType == CExifTag::ETagSrational ) )
            {
            noBytes = 8;
            }
        else
            {
            noBytes = 2;
            }
        noBytes *= count;
        TPtrC8 tagBuffer = tag->Data();
        if ( noBytes < 5 )
            {
            TUint32 tagData = 0;
            switch ( noBytes )
                {
                case 4:
                    tagData = TExifCommon::Uint32L( 
                        CONST_CAST( TUint8*, tagBuffer.Ptr() ) );
                    break;
                case 3:
                    tagData = TExifCommon::Uint32L( CONST_CAST( 
                        TUint8*, tagBuffer.Ptr() ) ) & KThreeByteMask;
                    break;
                case 2:
                    tagData = TExifCommon::Uint32L( CONST_CAST( 
                        TUint8*, tagBuffer.Ptr() ) ) & KTwoByteMask;
                    break;
                default:
                    tagData = TExifCommon::Uint32L( CONST_CAST( 
                        TUint8*, tagBuffer.Ptr() ) ) & KOneByteMask;
                }
            TExifCommon::SetUint32( 
                REINTERPRET_CAST( TUint8*, aExifDataPtr ), tagData ); 
            }
        else
            {
            TPtr8 ptr( REINTERPRET_CAST( TUint8*, ifdForwardPtr ), noBytes );
            ptr.Copy( tagBuffer.Ptr(), noBytes );
            TExifCommon::SetUint32( 
                REINTERPRET_CAST( TUint8*, aExifDataPtr ), fwdPos ); 
            if ( noBytes % 2 )
                {
                ++noBytes;
                ifdForwardPtr += ( noBytes / 2 );
                *( ( REINTERPRET_CAST( TUint8*, ifdForwardPtr ) ) - 1 ) = 0;
                }
            else
                {
                ifdForwardPtr += ( noBytes / 2 );
                }
            fwdPos += noBytes;
            }
        aExifDataPtr += 2;
        }
    TExifCommon::SetUint32( 
        REINTERPRET_CAST( TUint8*, aExifDataPtr ), iNextIfdOffset ); 
    aExifDataPtr = ifdForwardPtr;
    aPos = fwdPos + 12;
    }

// -----------------------------------------------------------------------------
// CExifIfd::InsertTagL
// Inserts/Replaces the given tag in the IFD structure.
// -----------------------------------------------------------------------------
//
void CExifIfd::InsertTagL( CExifTagImpl* aExifTag, TBool aCheckValidity )
    {
    LOGTEXT( _L( "ExifLib: CExifIfd::InsertTagL entering" ));
    if ( !aExifTag )
        {
        LOGTEXT( _L( "ExifLib: CExifIfd::InsertTagL Leaving KErrGeneral" ));
        User::Leave( KErrGeneral );
        }
    TUint16 tagId = aExifTag->Id();

    if ( aCheckValidity && ( !IsAcceptableTagId( tagId ) ) )
        {
        LOGTEXT2( _L( "ExifLib: CExifIfd::InsertTagL Leaving KErrNotSupported, tagId=0x%x" ), tagId);
        User::Leave( KErrNotSupported );
        }

    TInt index = -1;
    for ( TInt i = 0; ( i < iTagArray.Count() ) && ( index < 0 ); ++i )
        {
        if ( iTagArray[i]->Id() > tagId )
            {
            index = i;
            }
        else if ( iTagArray[i]->Id() == tagId )
            {
            iSize = STATIC_CAST( TUint16, iSize - iTagArray[i]->Size() );
            delete iTagArray[i]; 
            iTagArray[i] = aExifTag;
            iSize = STATIC_CAST( TUint16, iSize + aExifTag->Size() );
            LOGTEXT( _L( "ExifLib: CExifIfd::InsertTagL returning" ));
            return;
            }
        else
            {
            }
        }

    if ( index < 0 )
        {
        iTagArray.AppendL( aExifTag );
        }
    else
        {
        iTagArray.InsertL( index, aExifTag );
        }
    iSize = STATIC_CAST( TUint16, iSize + aExifTag->Size() );
    LOGTEXT( _L( "ExifLib: CExifIfd::InsertTagL returning" ));
    }

// -----------------------------------------------------------------------------
// CExifIfd::DeleteTag
// Removes the specified tag from the IFD structure
// -----------------------------------------------------------------------------
//
TInt CExifIfd::DeleteTag( TUint16 aTagId )
    {
    for ( TInt i = 0; i < iTagArray.Count(); ++i )
        {
        if ( iTagArray[i]->Id() == aTagId )
            {
            iSize = STATIC_CAST( TUint16, iSize - iTagArray[i]->Size() );
            delete iTagArray[i];
            iTagArray.Delete( i );
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CExifIfd::GetTagL
// Gets the unmodifiable specified tag instance from the IFD structure.
// -----------------------------------------------------------------------------
//
const CExifTagImpl* CExifIfd::GetTagL( TUint16 aTagId ) const
    {
    if ( !IsAcceptableTagId( aTagId ) )
        {
        User::Leave( KErrNotSupported );
        }
    for ( TInt i = 0; i < iTagArray.Count(); ++i )
        {
        if ( iTagArray[i]->Id() == aTagId )
            {
            return iTagArray[i];
            }
        }
    User::Leave( KErrNotFound );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CExifIfd::GetTagIdsL
// Gets the IDs and amount of the tags existing in the IFD structure.
// -----------------------------------------------------------------------------
//
TUint16* CExifIfd::GetTagIdsL( TInt& aNoTags ) const
    {
    if ( !iTagArray.Count() )
        {
        User::Leave( KErrNotFound );
        }

    TUint16* tagIds = STATIC_CAST( TUint16*, 
        User::AllocL( sizeof( TUint16 ) * iTagArray.Count() ) );
    for ( TInt i = 0; i < iTagArray.Count(); ++i )
        {
        tagIds[i] = iTagArray[i]->Id();
        }
    
    aNoTags = iTagArray.Count();
    return tagIds;
    }

// -----------------------------------------------------------------------------
// CExifIfd::IsAcceptableTagId
// Checks if the given tag ID is one of the IDs that can be stored in the IFD 
// structure.
// -----------------------------------------------------------------------------
//
TBool CExifIfd::IsAcceptableTagId( TUint16 aTagId ) const
    {
    TInt noTags = 0;
    const TReferenceTag* tags = NULL;
    switch ( iIfdType )
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
    for ( k = 0; k < noTags; ++k )
        {
        if ( tags[k].iId == aTagId )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ============================ CLASS CExifIfd0 ================================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfd0::CExifIfd0
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfd0::CExifIfd0()
    {
    }

// -----------------------------------------------------------------------------
// CExifIfd0::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifIfd0* CExifIfd0::NewL()
    {
    CExifIfd0* self = new( ELeave ) CExifIfd0();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifIfd0::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfd0::ConstructL()
    {
    }

// Destructor
CExifIfd0::~CExifIfd0()
    {
    }

// -----------------------------------------------------------------------------
// CExifIfd0::IsValid
// Checks if the IFD0 is in valid format, and contains all mandatory IFD0 tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfd0::IsValid() const
    {
    if ( !iTagArray.Count() )
        {
        return EFalse;
        }
    for ( TInt i = 0; i < KNoMandatoryIfd0Tags; ++i )
        {
        TBool found = EFalse;
        TInt j = 0;
        for ( j = 0; ( j < iTagArray.Count() ) && ( !found ); ++j )
            {
            if ( ifd0Tags[i].iId == iTagArray[j]->Id() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            return EFalse;
            }
        }
    return ETrue;
    }


// ============================ CLASS CExifIfdExif =============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfdExif::CExifIfdExif
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfdExif::CExifIfdExif ( )
    {
    iIfdType = EIfdExif;
    }

// -----------------------------------------------------------------------------
// CExifIfdExif::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifIfdExif* CExifIfdExif::NewL()
    {
    CExifIfdExif* self = new( ELeave ) CExifIfdExif();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifIfdExif::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfdExif::ConstructL()
    {
    }

// Destructor
CExifIfdExif::~CExifIfdExif ( )
    {
    }

// -----------------------------------------------------------------------------
// CExifIfdExif::IsValid
// Checks if the Exif IFD is in valid format, and contains all mandatory Exif 
// IFD tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfdExif::IsValid() const
    {
    if ( !iTagArray.Count() )
        {
        return EFalse;
        }
    for ( TInt i = 0; i < KNoMandatoryIfdExifTags; ++i )
        {
        TBool found = EFalse;
        TInt j = 0;
        for ( j = 0; ( j < iTagArray.Count() ) && ( !found ); ++j )
            {
            if ( ifdExifTags[i].iId == iTagArray[j]->Id() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            return EFalse;
            }
        }
    return ETrue;
    }


// ============================ CLASS CExifIfd1 ================================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfd1::CExifIfd1
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfd1::CExifIfd1()
    {
    iIfdType = EIfd1;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifIfd1* CExifIfd1::NewL()
    {
    CExifIfd1* self = new( ELeave ) CExifIfd1();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfd1::ConstructL()
    {
    }

// Destructor
CExifIfd1::~CExifIfd1()
    {
    if ( iThumbnailBuffer )
        {
        delete iThumbnailBuffer;
        }
    }

// -----------------------------------------------------------------------------
// CExifIfd1::IsValid
// Checks if the Exif IFD is in valid format, and contains all mandatory Exif 
// IFD tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfd1::IsValid() const
    {
    if ( !iTagArray.Count() )
        {
        return EFalse;
        }
    for ( TInt i = 0; i < KNoMandatoryIfd1Tags; ++i )
        {
        TBool found = EFalse;
        TInt j = 0;
        for ( j = 0; ( j < iTagArray.Count() ) && ( !found ); ++j )
            {
            if ( ifd1Tags[i].iId == iTagArray[j]->Id() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            return EFalse;
            }
        }
    if ( !iThumbnailBuffer )
        {
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::WriteThumbnailL
// Writes the Exif thumbnail image to the location defined by the given pointer 
// and the offset.
// -----------------------------------------------------------------------------
//
void CExifIfd1::WriteThumbnailL( TUint8*& aExifDataPtr, TUint& aPos ) const
    {
    if ( !iThumbnailBuffer )
        {
        User::Leave( KErrNotReady );
        }
    TPtr8 ptr( aExifDataPtr, iThumbnailBuffer->Length() );
    ptr.Copy( iThumbnailBuffer->Ptr(), iThumbnailBuffer->Length() );
    aExifDataPtr += iThumbnailBuffer->Length();
    aPos += iThumbnailBuffer->Length();
    }

// -----------------------------------------------------------------------------
// CExifIfd1::ThumbnailSize
// Returns the size of the Exif thumbnail image in bytes.
// -----------------------------------------------------------------------------
//
TUint16 CExifIfd1::ThumbnailSize() const
    {
    if ( !iThumbnailBuffer )
        {
        return 0;
        }
    return STATIC_CAST( TUint16, iThumbnailBuffer->Length() );
    }

// -----------------------------------------------------------------------------
// CExifIfd1::SetThumbnailData
// Inserts/Updates the given Exif thumbnail image.
// -----------------------------------------------------------------------------
//
TInt CExifIfd1::SetThumbnailData( TDesC8* aThumbnailData )
    {
    if ( !aThumbnailData )
        {
        return KErrGeneral;
        }
    if ( iThumbnailBuffer )
        {
        delete iThumbnailBuffer;
        }
    iThumbnailBuffer = aThumbnailData;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::GetThumbnailData
// Gets the Exif thumbnail image.
// -----------------------------------------------------------------------------
//
TInt CExifIfd1::GetThumbnailData( TDesC8*& aThumbnailData ) const
    {
    if ( !iThumbnailBuffer )
        {
        return KErrNotFound;
        }
    aThumbnailData = iThumbnailBuffer;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::RemoveThumbnailData
// Removes the Exif thumbnail image.
// -----------------------------------------------------------------------------
//
TInt CExifIfd1::RemoveThumbnailData()
    {
    if ( !iThumbnailBuffer )
        {
        return KErrNotFound;
        }
    delete iThumbnailBuffer;
    iThumbnailBuffer = 0;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CExifIfd1::InsertTagL
// Inserts/Replaces the given tag in the IFD structure.
// -----------------------------------------------------------------------------
//
void CExifIfd1::InsertTagL( CExifTagImpl* aExifTag, TBool aCheckValidity )
    {
    LOGTEXT( _L( "ExifLib: CExifIfd1::InsertTagL entering" ));
    if ( !aExifTag )
        {
        LOGTEXT( _L( "ExifLib: CExifIfd1::InsertTagL Leaving KErrGeneral" ));
        User::Leave( KErrGeneral );
        }
    TUint16 tagId = aExifTag->Id();
    if ( tagId == KIdCompression )
        {
        if ( aExifTag->Data().Length() != 2 )
            {
            LOGTEXT2( _L( "ExifLib: CExifIfd1::InsertTagL Leaving KErrCorrupt, aExifTag->Data().Length()=0x%x" ), aExifTag->Data().Length());
            User::Leave( KErrCorrupt );
            }
        TUint16* ptr = REINTERPRET_CAST( TUint16*, CONST_CAST( TUint8*, 
            aExifTag->Data().Ptr() ) );
        if ( *ptr != 6 )
            {
            LOGTEXT2( _L( "ExifLib: CExifIfd1::InsertTagL Leaving KErrNotSupported, *ptr=0x%x" ), *ptr);
            User::Leave( KErrNotSupported );
            }
        }
    
    if ( aCheckValidity && ( !IsAcceptableTagId( tagId ) ) )
        {
        LOGTEXT2( _L( "ExifLib: CExifIfd1::InsertTagL Leaving KErrNotSupported, tagId=0x%x" ), tagId);
        User::Leave( KErrNotSupported );
        }

    TInt index = -1;
    for ( TInt i = 0; ( i <  iTagArray.Count() ) && ( index < 0 ); ++i )
        {
        if ( iTagArray[i]->Id() > tagId )
            {
            index = i;
            }
        else if ( iTagArray[i]->Id() == tagId )
            {
            iSize = STATIC_CAST( TUint16, iSize - iTagArray[i]->Size() );
            delete iTagArray[i];
            iTagArray[i] = aExifTag;
            iSize = STATIC_CAST( TUint16, iSize + aExifTag->Size() );
            LOGTEXT( _L( "ExifLib: CExifIfd1::InsertTagL returning" ));
            return;
            }
        else
            {
            }
        }

    if ( index < 0 )
        {
        iTagArray.AppendL( aExifTag );
        }
    else
        {
        iTagArray.InsertL( index, aExifTag );
        }
    iSize = STATIC_CAST( TUint16, iSize + aExifTag->Size() );
    LOGTEXT( _L( "ExifLib: CExifIfd1::InsertTagL returning" ));
    }


// ============================ CLASS CExifIfdGps ==============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfdGps::CExifIfdGps
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfdGps::CExifIfdGps()
    {
    iIfdType = EIfdGps;
    }

// -----------------------------------------------------------------------------
// CExifIfdGps::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifIfdGps* CExifIfdGps::NewL()
    {
    CExifIfdGps* self = new( ELeave ) CExifIfdGps();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifIfdGps::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfdGps::ConstructL()
    {
    }

// Destructor
CExifIfdGps::~CExifIfdGps()
    {
    }

// -----------------------------------------------------------------------------
// CExifIfdGps::IsValid
// Checks if the Gps IFD is in valid format, and contains all Gps IFD mandatory 
// tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfdGps::IsValid() const
    {
    if ( !iTagArray.Count() )
        {
        return EFalse;
        }
    for ( TInt i = 0; i < KNoMandatoryIfdGpsTags; ++i )
        {
        TBool found = EFalse;
        TInt j = 0;
        for ( j = 0; ( j < iTagArray.Count() ) && ( !found ); ++j )
            {
            if ( ifdGpsTags[i].iId == iTagArray[j]->Id() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            return EFalse;
            }
        }
    return ETrue;
    }


// ============================ CLASS CExifIfdIntOp ============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifIfdIntOp::CExifIfdIntOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifIfdIntOp::CExifIfdIntOp()
    {
    iIfdType = EIfdIntOp;
    }

// -----------------------------------------------------------------------------
// CExifIfdIntOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifIfdIntOp* CExifIfdIntOp::NewL()
    {
    CExifIfdIntOp* self = new( ELeave ) CExifIfdIntOp();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifIfdIntOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifIfdIntOp::ConstructL()
    {
    }

// Destructor
CExifIfdIntOp::~CExifIfdIntOp()
    {
    }

// -----------------------------------------------------------------------------
// CExifIfdIntOp::IsValid
// Checks if the Interoperability IFD is in valid format, and contains all IOP 
// mandatory tags.
// -----------------------------------------------------------------------------
//
TBool CExifIfdIntOp::IsValid() const
    {
    /* KNoMandatoryIfdIntOpTags is 0, no need for this checking loop.
    if ( !iTagArray.Count() )
        {
        return EFalse;
        }
    for ( TUint i = 0; i < KNoMandatoryIfdIntOpTags; ++i )
        {
        TBool found = EFalse;
        TUint j = 0;
        for ( j = 0; ( j < iTagArray.Count() ) && ( !found ); ++j )
            {
            if ( ifdIntOpTags[i].iId == iTagArray[j]->Id() )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            return EFalse;
            }
        }
    */
    return ETrue;
    }
