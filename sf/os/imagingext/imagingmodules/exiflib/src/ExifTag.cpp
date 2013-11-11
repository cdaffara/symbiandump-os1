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
* Description:  Exif tag wrapper API implementation
*
*/


// INCLUDE FILES
#include "ExifTagImpl.h"
#include "ExifCommon.h"

// ============================ CLASS TExifTagInfo =============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TExifTagInfo::TExifTagInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TExifTagInfo::TExifTagInfo( 
   TUint16 aTagId, 
   CExifTag::TExifTagDataType aDataType, 
   TUint32 aDataCount ):
       iId( aTagId ), 
       iDataType( aDataType ), 
       iDataCount( aDataCount )
    {
    }


// ============================ CLASS CExifTag =================================
// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CExifTag::~CExifTag()
    {
    }


// ============================ CLASS CExifTagImpl =============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExifTagImpl::CExifTagImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CExifTagImpl::CExifTagImpl( TExifTagInfo aTagInfo ): iTagInfo( aTagInfo )
    {
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CExifTagImpl::ConstructL( HBufC8* aDataBuffer, TBool aCheckValidity )
    {
    LOGTEXT2( _L( "ExifLib: CExifTagImpl::ConstructL entering aCheckValidity=%d" ), aCheckValidity);
    if ( !aDataBuffer ) 
        {
        LOGTEXT( _L( "ExifLib: CExifTagImpl::ConstructL leaving KErrGeneral" ) );
        User::Leave( KErrGeneral );
        }
    if ( aCheckValidity && !TExifCommon::IsValidTagId( iTagInfo.iId ) )
        {
        LOGTEXT( _L( "ExifLib: CExifTagImpl::ConstructL leaving KErrNotSupported" ) );
        User::Leave( KErrNotSupported );
        }

    TInt wordLength = 1;
    if ( iTagInfo.iDataType == ETagShort )
        {
        wordLength = 2;
        }
	else if ( ( iTagInfo.iDataType == ETagLong ) || 
        ( iTagInfo.iDataType == ETagSlong ) )
        {
        wordLength = 4;
        }
    else if ( ( iTagInfo.iDataType == ETagRational ) || 
        ( iTagInfo.iDataType == ETagSrational ) )
        {
        wordLength = 8;
        }
    else
        {
        // In practice, it never arrives here.
        }
    LOGTEXT2( _L( "ExifLib: CExifTagImpl::ConstructL wordLength=%d" ), wordLength );

    if ( aDataBuffer->Length() != 
        STATIC_CAST( TInt, wordLength * iTagInfo.iDataCount ) )
        {
        LOGTEXT3( _L( "ExifLib: CExifTagImpl::ConstructL leaving KErrArgument aDataBuffer->Length()=%d, iTagInfo.iDataCount=%d" ), aDataBuffer->Length(), iTagInfo.iDataCount);
        User::Leave( KErrArgument );
        }
    iData = aDataBuffer;
    LOGTEXT2( _L( "ExifLib: CExifTagImpl::ConstructL returning aDataBuffer->Length()=%d" ), aDataBuffer->Length() );
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifTagImpl* CExifTagImpl::NewL( 
    TUint16 aTagId, 
    CExifTag::TExifTagDataType aDataType, 
    TUint32 aDataCount, 
    HBufC8* aDataBuffer,
    TBool aCheckValidity )
    {
    if ( !aDataBuffer->Length() )
        {
        User::Leave( KErrArgument );
        }
    
    CExifTagImpl* self = new( ELeave ) CExifTagImpl( TExifTagInfo( aTagId, 
        aDataType, aDataCount ) );
    CleanupStack::PushL( self );
    self->ConstructL( aDataBuffer, aCheckValidity );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CExifTagImpl* CExifTagImpl::NewL( TExifTagInfo aTagInfo, HBufC8* aDataBuffer, TBool aCheckValidity )
    {
    if ( !aDataBuffer )
        {
        User::Leave( KErrGeneral );
        }
    if ( !aDataBuffer->Length() )
        {
        User::Leave( KErrArgument );
        }

    CExifTagImpl* self = new( ELeave ) CExifTagImpl( aTagInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aDataBuffer, aCheckValidity );
    CleanupStack::Pop();
    return self;
    }

// Destructor
CExifTagImpl::~CExifTagImpl()
    {
    if ( iData )
        {
        delete iData;
        }
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::DuplicateL
// Duplicate constructor. Creates an exact copy instance of the tag.
// -----------------------------------------------------------------------------
//
CExifTag* CExifTagImpl::DuplicateL() const
    {
    HBufC8* buffer = iData->AllocL();
    CleanupStack::PushL( buffer );
    CExifTagImpl* tag = CExifTagImpl::NewL( iTagInfo.iId, iTagInfo.iDataType, 
        iTagInfo.iDataCount, buffer, ETrue );
    CleanupStack::Pop( buffer );
    return tag;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::TagInfo
// Returns the informative fields of a tag.
// -----------------------------------------------------------------------------
//
TExifTagInfo CExifTagImpl::TagInfo() const
    {
    return iTagInfo;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::Id
// Returns the ID of the tag.
// -----------------------------------------------------------------------------
//
TUint16 CExifTagImpl::Id() const
    {
    return iTagInfo.iId;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::DataType
// Returns the data type of the tag.
// -----------------------------------------------------------------------------
//
CExifTag::TExifTagDataType CExifTagImpl::DataType() const
    {
    return iTagInfo.iDataType;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::DataCount
// Returns the data count of the tag.
// -----------------------------------------------------------------------------
//
TUint32 CExifTagImpl::DataCount() const
    {
    return iTagInfo.iDataCount;
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::Data
// Returns the data stored in the tag.
// -----------------------------------------------------------------------------
//
TPtrC8 CExifTagImpl::Data() const
    {
    return iData->Des();
    }

// -----------------------------------------------------------------------------
// CExifTagImpl::Size
// Returns the total size of the tag in bytes
// -----------------------------------------------------------------------------
//
TUint CExifTagImpl::Size() const
    {
    // If the real tag data size is smaller than 4 bytes, then total tag size is
    // the default tag size (12). Otherwise, it is:
    // Default tag size  + real tag data size (+ 1 if tag data size is odd).
    if ( iData->Length() < 5 )
         {
         return 12; // The default tag size
         }
     else
         {
         if ( iData->Length() % 2 )
             {
             return ( 12 + iData->Length() + 1 ); // Default tag size  + ...
             }
         else
             {
             return ( 12 + iData->Length() ); // // Default tag size  + ...
             }    
         }
    }

