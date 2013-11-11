/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CVisualFrameImpl
*
*/

#include "IclExtJpegApi.h"
#include "IclExtJpegApiFrameImpl.h"

// ---------------------------------------------------------
// TFrameFormatExt::Constructor
// ---------------------------------------------------------
//
void TFrameFormatExt::Constructor()
    {
    switch ( iFormatCode )
        {
        case CVisualFrame::EFormatMonochrome:
            {
            iColourSpace    = KUidColourSpaceYCbCr;
            iSampling       = KUidSamplingMonochrome;
            break;            
            }
        case CVisualFrame::EFormat16bitRGB444:
        case CVisualFrame::EFormat16BitRGB565:
        case CVisualFrame::EFormat32BitRGB888:
        case CVisualFrame::EFormatFbsBitmapColor4K:
        case CVisualFrame::EFormatFbsBitmapColor64K:
        case CVisualFrame::EFormatFbsBitmapColor16M:
            {
            iColourSpace    = KUidColourSpaceRGB;
            iSampling       = KUidSamplingColor444;            
            break;
            }
        case CVisualFrame::EFormatYUV420SemiPlanar:
        case CVisualFrame::EFormatYUV420Interleaved:
        case CVisualFrame::EFormatYUV420Planar:
            {
            iColourSpace    = KUidColourSpaceYCbCr;
            iSampling       = KUidSamplingColor420;
            break;
            }
        case CVisualFrame::EFormatYUV422:
        case CVisualFrame::EFormatYUV422Reversed:
        case CVisualFrame::EFormatExtYUV422Interleaved:
        case CVisualFrame::EFormatExtYUV422Planar:
            {
            iColourSpace    = KUidColourSpaceYCbCr;
            iSampling       = KUidSamplingColor422;
            break;
            } 
        case CVisualFrame::EFormatExtYUV444Planar:
        case CVisualFrame::EFormatYUV444:
            {
            iColourSpace    = KUidColourSpaceYCbCr;
            iSampling       = KUidSamplingColor444;
            break;
            }
        case CVisualFrame::EFormatJpeg:
        case CVisualFrame::EFormatExif:
        case CVisualFrame::EFormatUserDefined:
        case CVisualFrame::EFormatExtDctCoeff:
            {
            iColourSpace    = KNullUid;
            iSampling       = KNullUid;
            break;
            }
        default:
            {
            iFormatCode     = KErrNotFound;
            iColourSpace    = KNullUid;
            iSampling       = KNullUid;
            }
        }   
    }

// ----------------------------------------------------------
// TFrameFormatExt::ColourSpace
// ----------------------------------------------------------
//
TUid TFrameFormatExt::ColourSpace() const
    {
    return iColourSpace;
    }

// ----------------------------------------------------------
// TFrameFormatExt::Sampling
// ----------------------------------------------------------
//
TUid TFrameFormatExt::Sampling() const
    {
    return iSampling;
    }
    
// ----------------------------------------------------------
// TFrameFormatExt::FormatCode
// ----------------------------------------------------------
//
TInt TFrameFormatExt::FormatCode() const
    {
    return iFormatCode;
    }
    
// ----------------------------------------------------------
// TFrameFormatExt::ColourSpace
// ----------------------------------------------------------
//
void TFrameFormatExt::SetColourSpace( TUid aColourSpace )
    {
    iColourSpace = aColourSpace;
    }



/*****************************************************/
/* Visual Frame Data Structure in Extended ICL API   */
/*****************************************************/


// ----------------------------------------------------------
// TFrameFormatBase* GetFrameFormatL
// ----------------------------------------------------------
//
TFrameFormatBase* GetFrameFormatL( TInt aFrameFormatCode )
    {
    TUid formatUid = KNullUid;
    switch ( aFrameFormatCode )
        {
        case CVisualFrame::EFormatMonochrome:
            {
            formatUid = KUidFormatYUVMonochrome;
            break;
            }
        case CVisualFrame::EFormat16bitRGB444:
            {
            formatUid = KUidFormat16bitRGB444Interleaved; 
            break;
            }
        case CVisualFrame::EFormat16BitRGB565:
            {
            formatUid = KUidFormat16BitRGB565Interleaved;
            break;
            }
        case CVisualFrame::EFormat32BitRGB888:
            {
            formatUid = KUidFormat32BitRGB888Interleaved;
            break;
            }
        case CVisualFrame::EFormatYUV420Interleaved:
            {
            formatUid = KUidFormatYUV420Interleaved;
            break;
            }
        case CVisualFrame::EFormatYUV420Planar:
            {
            formatUid = KUidFormatYUV420Planar;
            break;
            }
        case CVisualFrame::EFormatYUV422:
            {
            formatUid = KUidFormatYUV422Interleaved;
            break;
            } 
        case CVisualFrame::EFormatYUV422Reversed:
            {
            formatUid = KUidFormatYUV422InterleavedReversed;
            break;
            }
        case CVisualFrame::EFormatYUV444:
            {
            formatUid = KUidFormatYUV444Interleaved;
            break;
            }
        case CVisualFrame::EFormatYUV420SemiPlanar:
            {
            formatUid = KUidFormatYUV420SemiPlanar;
            break;
            }
        case CVisualFrame::EFormatExtYUV422Interleaved:
            {
            formatUid = KUidFormatYYUV422Interleaved;
            break;
            }
        case CVisualFrame::EFormatExtYUV422Planar:
            {
            formatUid = KUidFormatYUV422Planar;
            break;
            }
        case CVisualFrame::EFormatExtYUV444Planar:
            {
            formatUid = KUidFormatYUV444Planar;
            break;
            }
        case CVisualFrame::EFormatJpeg:
        case CVisualFrame::EFormatExif:
        case CVisualFrame::EFormatFbsBitmapColor4K:
        case CVisualFrame::EFormatFbsBitmapColor64K:
        case CVisualFrame::EFormatFbsBitmapColor16M:
        case CVisualFrame::EFormatUserDefined:
        case CVisualFrame::EFormatExtDctCoeff:
            {
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    TFrameFormatBase* frameFormatBase = NULL;
    if ( formatUid == KNullUid )
        {
        frameFormatBase = new ( ELeave ) TFrameFormatExt( aFrameFormatCode );
        }
    else
        {
        frameFormatBase = new ( ELeave ) TFrameFormat( formatUid );
        }
    return frameFormatBase;
    }
    
// ----------------------------------------------------------
// GetFrameFormat
// ----------------------------------------------------------
//
TInt GetFrameFormat( const TFrameFormatBase& aFrameFormat )
    {
    TInt frameFormatCode = KErrNotFound;
    if ( aFrameFormat.Type() == KUidIclImageFrameFormat )
        {
        const TFrameFormat& frameFormat = reinterpret_cast<const TFrameFormat&>( aFrameFormat );
        switch ( frameFormat.FormatCode().iUid )
            {
            case KFormatYUVMonochromeUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatMonochrome;
                break;
                }
            case KFormat16bitRGB444InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormat16bitRGB444; 
                break;
                }
            case KFormat16BitRGB565InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormat16BitRGB565;
                break;
                }
            case KFormat32BitRGB888InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormat32BitRGB888;
                break;
                }
            case KFormatYUV420InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV420Interleaved;
                break;
                }
            case KFormatYUV420PlanarUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV420Planar;
                break;
                }
            case KFormatYUV422InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV422;
                break;
                } 
            case KFormatYUV422InterleavedReversedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV422Reversed;
                break;
                }
            case KFormatYUV444InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV444;
                break;
                }
            case KFormatYUV420SemiPlanarUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatYUV420SemiPlanar;
                break;
                }              
            case KFormatYYUV422InterleavedUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatExtYUV422Interleaved;
                break;
                }
            case KFormatYUV422PlanarUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatExtYUV422Planar;
                break;
                }
            case KFormatYUV444PlanarUidValue:
                {
                frameFormatCode = CVisualFrame::EFormatExtYUV444Planar;
                break;
                }                              
            default:
                {
                }
            }
        }
    else if ( aFrameFormat.Type() == KUidExtIclImageFrameFormat )
        {
        frameFormatCode = reinterpret_cast<const TFrameFormatExt&>( aFrameFormat ).FormatCode();
        }
    return frameFormatCode;    
    }



// ---------------------------------------------------------
// CVisualFrameImpl* CVisualFrameImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVisualFrameImpl* CVisualFrameImpl::NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat )
    {

    TFrameFormatBase* frameFormat = GetFrameFormatL( aFrameFormat );
    
    CleanupStack::PushL( frameFormat );

    TFrameLayout frameLayout = TFrameLayout( 1 );
    frameLayout.SetStart( 0, 0 );
    frameLayout.SetScanLength( 0, aBuffer.MaxLength() );
    frameLayout.SetLength( 0, aBuffer.MaxLength() );
    frameLayout.SetCurrentLength( 0, aBuffer.Length() );
    
    CVisualFrameImpl* self = new ( ELeave ) CVisualFrameImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aBuffer, aBuffer.MaxLength(), aDimension, *frameFormat, frameLayout );
    CleanupStack::Pop( self );
    CleanupStack::PopAndDestroy( frameFormat );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrameImpl* CVisualFrameImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVisualFrameImpl* CVisualFrameImpl::NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize, 
                                    TInt aDataOffset,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat )
    {

    TFrameFormatBase* frameFormat = GetFrameFormatL( aFrameFormat );
    CleanupStack::PushL( frameFormat );

    TFrameLayout frameLayout = TFrameLayout( 1 );

    frameLayout.SetStart( 0, aDataOffset );
    frameLayout.SetScanLength( 0, aMaxSize );
    frameLayout.SetLength( 0, aMaxSize );
    frameLayout.SetCurrentLength( 0, aSize );

    CVisualFrameImpl* self = new ( ELeave ) CVisualFrameImpl();
    CleanupStack::PushL( self );
    self->ConstructL( &aChunk, aMaxSize, aDataOffset, aDimension, *frameFormat, frameLayout );
    self->iSize = aSize;
    CleanupStack::Pop( self );
    CleanupStack::PopAndDestroy( frameFormat );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrameImpl* CVisualFrameImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVisualFrameImpl* CVisualFrameImpl::NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const CVisualFrame::TFrameLayout& aFrameLayout )
    {
    TFrameFormatBase* frameFormat = GetFrameFormatL( aFrameFormat );
    CleanupStack::PushL( frameFormat );

    TFrameLayout frameLayout = TFrameLayout( aFrameLayout.iNumberOfPlanes );

    for ( TInt i = 0; i < aFrameLayout.iNumberOfPlanes; ++i )
        {
        frameLayout.SetStart( i, aFrameLayout.iOffset[i] );
        frameLayout.SetScanLength( i, aFrameLayout.iScanLineLength[i] );
        frameLayout.SetLength( i, aFrameLayout.iMaxLength[i] );
        frameLayout.SetCurrentLength( i, aFrameLayout.iLength[i] );
        }
        
    CVisualFrameImpl* self = new ( ELeave ) CVisualFrameImpl();
    CleanupStack::PushL( self );
    self->ConstructL( aBuffer, aBuffer.MaxLength(), aDimension, *frameFormat, frameLayout );
    CleanupStack::Pop( self );
    CleanupStack::PopAndDestroy( frameFormat );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrameImpl* CVisualFrameImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVisualFrameImpl* CVisualFrameImpl::NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const CVisualFrame::TFrameLayout& aFrameLayout )
    {
    TFrameFormatBase* frameFormat = GetFrameFormatL( aFrameFormat );
    CleanupStack::PushL( frameFormat );

    TFrameLayout frameLayout = TFrameLayout( aFrameLayout.iNumberOfPlanes );

    for ( TInt i = 0; i < aFrameLayout.iNumberOfPlanes; ++i )
        {
        frameLayout.SetStart( i, aFrameLayout.iOffset[i] );
        frameLayout.SetScanLength( i, aFrameLayout.iScanLineLength[i] );
        frameLayout.SetLength( i, aFrameLayout.iMaxLength[i] );
        frameLayout.SetCurrentLength( i, aFrameLayout.iLength[i] );
        }
   
    CVisualFrameImpl* self = new ( ELeave ) CVisualFrameImpl();
    CleanupStack::PushL( self );
    self->ConstructL( &aChunk, aMaxSize, aFrameLayout.iOffset[0], aDimension, *frameFormat, frameLayout );
    self->iSize = aSize;
    CleanupStack::Pop( self );
    CleanupStack::PopAndDestroy( frameFormat );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrameImpl* CVisualFrameImpl::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CVisualFrameImpl* CVisualFrameImpl::NewL( const CImageFrame* aImageFrame )
    {
    if ( ( aImageFrame->FrameFormat().Type() != KUidIclImageFrameFormat ) && 
        ( aImageFrame->FrameFormat().Type() != KUidExtIclImageFrameFormat ) )
        {
        User::Leave( KErrNotSupported );
        }
    CVisualFrameImpl* self = new ( ELeave ) CVisualFrameImpl();
    CleanupStack::PushL( self );
    if ( aImageFrame->IsChunk() )
        {
        self->ConstructL( 
                    &const_cast<CImageFrame*>( aImageFrame )->DataChunk(), 
                    aImageFrame->MaxBufferSize(), 
                    aImageFrame->DataOffset(), 
                    aImageFrame->FrameSizeInPixels(), 
                    aImageFrame->FrameFormat(), 
                    aImageFrame->FrameLayout() );
        const TFrameLayout& frameLayout = reinterpret_cast<const TFrameLayout&>( aImageFrame->FrameLayout() );
        self->iSize = frameLayout.CurrentLength( frameLayout.Planes() - 1 ) + frameLayout.Start( frameLayout.Planes() - 1 ) - frameLayout.Start( 0 );
        }
    else
        {
        self->ConstructL( 
                    const_cast<CImageFrame*>( aImageFrame )->Data(), 
                    aImageFrame->MaxBufferSize(), 
                    aImageFrame->FrameSizeInPixels(), 
                    aImageFrame->FrameFormat(), 
                    aImageFrame->FrameLayout() );
        }
        
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CVisualFrameImpl::GetImageFrameL
// ---------------------------------------------------------
//
CImageFrame* CVisualFrameImpl::GetImageFrameL()
    {
    if ( FrameFormat().Type() != KUidIclImageFrameFormat )
        {
        User::Leave( KErrNotSupported );
        }
        
    if ( IsChunk() )
        {
        return CImageFrame::NewL( 
                    &DataChunk(), 
                    MaxBufferSize(), 
                    DataOffset(), 
                    FrameSizeInPixels(), 
                    FrameFormat(), 
                    FrameLayout() );
        }
    else
        {
        return CImageFrame::NewL( 
                    Data(), 
                    MaxBufferSize(), 
                    FrameSizeInPixels(), 
                    FrameFormat(), 
                    FrameLayout() );        
        }
    }
    
// ---------------------------------------------------------
// CVisualFrameImpl::DataChunkL
// ---------------------------------------------------------
//
const RChunk& CVisualFrameImpl::DataChunkL() const
    {
    if ( !IsChunk() )
        {
        User::Leave( KErrNotFound );
        }
    return const_cast<CVisualFrameImpl*>( this )->DataChunk();
    }

// ---------------------------------------------------------
// CVisualFrameImpl::DataPtrL
// ---------------------------------------------------------
//
TPtr8 CVisualFrameImpl::DataPtrL() const
    {
    if ( IsChunk() )
        {
        User::Leave( KErrNotFound );
        }    
    TDes8& des = const_cast<CVisualFrameImpl*>( this )->Data();
    return TPtr8( const_cast<TUint8*>( des.Ptr() ), des.Length(), des.MaxLength() );
    }

// ---------------------------------------------------------
// CVisualFrameImpl::Dimension
// ---------------------------------------------------------
//
TSize CVisualFrameImpl::Dimension() const
    {
    return FrameSizeInPixels();
    }

// ---------------------------------------------------------
// CVisualFrameImpl::FrameFormat2
// ---------------------------------------------------------
//
TInt CVisualFrameImpl::FrameFormat2() const
    {
    return GetFrameFormat( FrameFormat() );
    }
        
// ---------------------------------------------------------
// CVisualFrameImpl::DataOffset
// ---------------------------------------------------------
//
TInt CVisualFrameImpl::DataOffset() const
    {
    return CImageFrame::DataOffset();
    }
        
// ---------------------------------------------------------
// CVisualFrameImpl::Size
// ---------------------------------------------------------
//
TInt CVisualFrameImpl::Size() const
    {
    if ( !IsChunk() )
        {
        return Data().Length();
        }
    return iSize;
    }

// ---------------------------------------------------------
// CVisualFrameImpl::SetSizeL
// ---------------------------------------------------------
//
void CVisualFrameImpl::SetSizeL( TInt aSize )
    {
    if ( ( aSize < 0 ) || ( aSize > MaxSize() ) )
        {
        User::Leave( KErrArgument );
        }
    if ( !IsChunk() )
        {
        Data().SetLength( aSize );
        }
    iSize = aSize;
    }
    
// ---------------------------------------------------------
// CVisualFrameImpl::MaxSize
// ---------------------------------------------------------
//
TInt CVisualFrameImpl::MaxSize() const
    {
    return MaxBufferSize();
    }
    
// ---------------------------------------------------------
// CVisualFrameImpl::DataContainer
// ---------------------------------------------------------
//
TInt CVisualFrameImpl::DataContainer() const
    {
    if ( IsChunk() )
        {
        return CVisualFrame::EInChunk;
        }
    return CVisualFrame::EInDescriptor;
    }
        
// ---------------------------------------------------------
// CVisualFrameImpl::FrameLayout2
// ---------------------------------------------------------
//
const CVisualFrame::TFrameLayout& CVisualFrameImpl::FrameLayout2() const
    {
    const TFrameLayout& frameLayout = reinterpret_cast<const TFrameLayout&>( FrameLayout() );
    CVisualFrame::TFrameLayout& frameLayout2 = const_cast<CVisualFrameImpl*>( this )->iFrameLayout2;
    frameLayout2.iNumberOfPlanes = frameLayout.Planes();
    for (TInt i = 0; i < iFrameLayout2.iNumberOfPlanes; ++i )
        {
        frameLayout2.iOffset[i] = frameLayout.Start( i );
        frameLayout2.iScanLineLength[i] = frameLayout.ScanLength( i );
        frameLayout2.iLength[i] = frameLayout.CurrentLength( i );
        frameLayout2.iMaxLength[i] = frameLayout.Length( i );
        }
    return iFrameLayout2;
    }
    
// ---------------------------------------------------------
// CVisualFrameImpl::~CVisualFrameImpl
// Destructor
// ---------------------------------------------------------
//
CVisualFrameImpl::~CVisualFrameImpl()
    {
    } 

// End of File
