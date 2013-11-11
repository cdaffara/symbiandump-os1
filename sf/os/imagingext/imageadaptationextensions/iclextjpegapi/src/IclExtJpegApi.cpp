/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CVisualFrame, CExtJpegDecoder and CExtJpegEncoder
*
*/

// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <icl_uids_def.hrh>
#endif
#include "IclExtJpegApi.h"
#include "IclExtJpegApiFrameImpl.h"


/*****************************************************/
/* Visual Frame Data Structure in Extended ICL API   */
/*****************************************************/

// ---------------------------------------------------------
// CVisualFrame* CVisualFrame::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVisualFrame* CVisualFrame::NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat )
    {
    
    CVisualFrame* self = new ( ELeave ) CVisualFrame();
    CleanupStack::PushL( self );
    self->iVisualFrameImpl = CVisualFrameImpl::NewL( aBuffer, aDimension, aFrameFormat );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrame* CVisualFrame::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVisualFrame* CVisualFrame::NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize, 
                                    TInt aDataOffset,
                                    const TSize& aDimension, 
                                    TInt aFrameFormat )
    {
    CVisualFrame* self = new ( ELeave ) CVisualFrame();
    CleanupStack::PushL( self );
    self->iVisualFrameImpl = CVisualFrameImpl::NewL( aChunk, aSize, aMaxSize, 
                                                     aDataOffset, aDimension, aFrameFormat );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrame* CVisualFrame::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVisualFrame* CVisualFrame::NewL( 
                                    TDes8& aBuffer, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const TFrameLayout& aFrameLayout )
    {
    CVisualFrame* self = new ( ELeave ) CVisualFrame();
    CleanupStack::PushL( self );
    self->iVisualFrameImpl = CVisualFrameImpl::NewL( aBuffer, aDimension, 
                                                     aFrameFormat, aFrameLayout );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CVisualFrame* CVisualFrame::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVisualFrame* CVisualFrame::NewL( 
                                    RChunk& aChunk, 
                                    TInt aSize, 
                                    TInt aMaxSize, 
                                    const TSize& aDimension, 
                                    TInt aFrameFormat, 
                                    const TFrameLayout& aFrameLayout )
    {
    CVisualFrame* self = new ( ELeave ) CVisualFrame();
    CleanupStack::PushL( self );
    self->iVisualFrameImpl = CVisualFrameImpl::NewL( aChunk, aSize, aMaxSize, 
                                                     aDimension, aFrameFormat, 
                                                     aFrameLayout );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------
// CVisualFrame::DataChunkL
// ----------------------------------------------------------
//
EXPORT_C const RChunk& CVisualFrame::DataChunkL() const
    {
    return iVisualFrameImpl->DataChunkL();
    }

// ----------------------------------------------------------
// CVisualFrame::DataPtrL
// ----------------------------------------------------------
//
EXPORT_C TPtr8 CVisualFrame::DataPtrL() const
    {
    return iVisualFrameImpl->DataPtrL();
    }

// ----------------------------------------------------------
// CVisualFrame::Dimension
// ----------------------------------------------------------
//
EXPORT_C TSize CVisualFrame::Dimension() const
    {
    return iVisualFrameImpl->Dimension();
    }

// ----------------------------------------------------------
// CVisualFrame::FrameFormat
// ----------------------------------------------------------
//
EXPORT_C TInt CVisualFrame::FrameFormat() const
    {
    return iVisualFrameImpl->FrameFormat2();
    }
        
// ----------------------------------------------------------
// CVisualFrame::DataOffset
// ----------------------------------------------------------
//
EXPORT_C TInt CVisualFrame::DataOffset() const
    {
    return iVisualFrameImpl->DataOffset();
    }
        
// ----------------------------------------------------------
// CVisualFrame::Size
// ----------------------------------------------------------
//
EXPORT_C TInt CVisualFrame::Size() const
    {
    return iVisualFrameImpl->Size();
    }

// ----------------------------------------------------------
// CVisualFrame::SetSizeL
// ----------------------------------------------------------
//
EXPORT_C void CVisualFrame::SetSizeL( TInt aSize )
    {
    iVisualFrameImpl->SetSizeL( aSize );
    }
    
// ----------------------------------------------------------
// CVisualFrame::MaxSize
// ----------------------------------------------------------
//
EXPORT_C TInt CVisualFrame::MaxSize() const
    {
    return iVisualFrameImpl->MaxSize();
    }
         
// ----------------------------------------------------------
// CVisualFrame::DataContainer
// ----------------------------------------------------------
//
EXPORT_C CVisualFrame::TDataContainer CVisualFrame::DataContainer() const
    {
    return ( TDataContainer )iVisualFrameImpl->DataContainer();
    }
        
// ----------------------------------------------------------
// CVisualFrame::FrameLayout
// ----------------------------------------------------------
//
EXPORT_C const CVisualFrame::TFrameLayout& CVisualFrame::FrameLayout() const
    {
    return iVisualFrameImpl->FrameLayout2();
    }

// ----------------------------------------------------------
// CVisualFrame::~CVisualFrame
// Destructor
// ----------------------------------------------------------
//
EXPORT_C CVisualFrame::~CVisualFrame()
    {
    delete iVisualFrameImpl;
    } 

// ---------------------------------------------------------
// CVisualFrame* CVisualFrame::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CVisualFrame* CVisualFrame::NewL( const CImageFrame* aImageFrame )
    {
    CVisualFrame* self = new ( ELeave ) CVisualFrame();
    CleanupStack::PushL( self );
    self->iVisualFrameImpl = CVisualFrameImpl::NewL( aImageFrame );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------
// CVisualFrame::GetImageFrameL
// ----------------------------------------------------------
//
EXPORT_C CImageFrame* CVisualFrame::GetImageFrameL()
    {
    return iVisualFrameImpl->GetImageFrameL();    
    }


/*****************************************************/
/* Extended ICL Jpeg Decoder API                     */
/*****************************************************/

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::NewL()
    {
    return new (ELeave) CExtJpegDecoder();
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::FileNewL( 
                                        RFs& aFs, const TDesC& aSourceFilename, 
                                        const TDesC8& aMIMEType, 
                                        const TOptions aOptions )
    {
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::FileNewL( aFs, aSourceFilename, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::FileNewL( 
                                        RFs& aFs, 
                                        const TDesC& aSourceFilename, 
                                        const TOptions aOptions, 
                                        const TUid aImageType, 
                                        const TUid aImageSubType, 
                                        const TUid aDecoderUid )
    {
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::FileNewL( aFs, aSourceFilename, aOptions, 
                                aImageType, aImageSubType, aDecoderUid ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                                        RFs& aFs, 
                                        const TDesC8& aSourceData, 
                                        const TDesC8& aMIMEType, 
                                        const TOptions aOptions )
    {
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::DataNewL( aFs, aSourceData, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                                        RFs& aFs, 
                                        const TDesC8& aSourceData, 
                                        const TOptions aOptions, 
                                        const TUid aImageType, 
                                        const TUid aImageSubType, 
                                        const TUid aDecoderUid )
    {
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::DataNewL( aFs, aSourceData, aOptions, 
                                aImageType, aImageSubType, aDecoderUid ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                                        RFs& aFs, 
                                        const CVisualFrame* aSourceData, 
                                        const TDesC8& aMIMEType, 
                                        const TOptions aOptions )
    {
    
    TUint8* sourcePtr = NULL;
    CVisualFrame* sourceDataFrame = const_cast<CVisualFrame*>( aSourceData );
    if ( sourceDataFrame->DataContainer() == CVisualFrame::EInChunk )
        {
        sourcePtr = sourceDataFrame->DataChunkL().Base() + sourceDataFrame->DataOffset();
        }
    else if ( sourceDataFrame->DataContainer() == CVisualFrame::EInDescriptor )
        {
        sourcePtr = const_cast<TUint8*>( sourceDataFrame->DataPtrL().Ptr() ) 
                                       + sourceDataFrame->DataOffset();
        }
    else
        {
        User::Leave( KErrArgument );            
        }
    TPtrC8 sourceData( sourcePtr, sourceDataFrame->Size() );
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::DataNewL( aFs, sourceData, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ---------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                                        RFs& aFs, 
                                        const CVisualFrame* aSourceData, 
                                        const TOptions aOptions, 
                                        const TUid aImageType, 
                                        const TUid aImageSubType, 
                                        const TUid aDecoderUid )
    {
    TUint8* sourcePtr = NULL;
    CVisualFrame* sourceDataFrame = const_cast<CVisualFrame*>( aSourceData );    
    if ( sourceDataFrame->DataContainer() == CVisualFrame::EInChunk )
        {
        sourcePtr = sourceDataFrame->DataChunkL().Base() + sourceDataFrame->DataOffset();
        }
    else if ( aSourceData->DataContainer() == CVisualFrame::EInDescriptor )
        {
        sourcePtr = const_cast<TUint8*>( sourceDataFrame->DataPtrL().Ptr() ) 
                                       + sourceDataFrame->DataOffset();
        }
    else
        {
        User::Leave( KErrArgument );            
        }

    TPtrC8 sourceData( sourcePtr, sourceDataFrame->Size() );
    CExtJpegDecoder* dec = reinterpret_cast<CExtJpegDecoder*>( 
                                CImageDecoder::DataNewL( aFs, sourceData, aOptions, 
                                aImageType, aImageSubType, aDecoderUid ) );
    CleanupStack::PushL( dec );
    if ( dec->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return dec;
    }

// ----------------------------------------------------------
// CExtJpegDecoder::~CExtJpegDecoder
// Destructor
// ----------------------------------------------------------
//
EXPORT_C CExtJpegDecoder::~CExtJpegDecoder()
    {
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetCroppingL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetCroppingL( TRect aCropRect )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapCropping );
    CustomSyncL( reinterpret_cast<TInt>( &aCropRect ) );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetStreamingL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetStreamingL( TSize& aMacroBlockSize )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapStreaming );
    CustomSyncL( reinterpret_cast<TInt>( &aMacroBlockSize ) );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetRotationL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetRotationL( TInt aDegree )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapRotation );
    CustomSyncL( aDegree );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetFlippingL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetFlippingL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapFlipping );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetMirroringL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetMirroringL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapMirroring );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SetDctDecodingL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::SetDctDecodingL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapDctDecoding );
    CustomSyncL( EEnd );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::ConvertL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::ConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aDestinationFrame, 
                        TInt& aNoOfDecodedMBlocks,
                        TInt aFrameNumber )
    {
    iIsExtConvert = ETrue;
    CustomSyncL( KExtensionUID );
    CustomSyncL( EConvert );
    CustomSyncL( reinterpret_cast<TInt>( aDestinationFrame ) );
    CustomSyncL( reinterpret_cast<TInt>( &aNoOfDecodedMBlocks ) );
    CustomSyncL( aFrameNumber );
    CustomSyncL( EReadyForAsync );
    CustomAsync( aRequestStatus, 0 );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::ContinueConvertL
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::ContinueConvertL( 
                        TRequestStatus* aRequestStatus, 
                        const CVisualFrame* aDestinationFrame, 
                        TInt& aNoOfDecodedMBlocks,
                        TInt aFrameNumber )
    {
    iIsExtConvert = ETrue;
    CustomSyncL( KExtensionUID );
    CustomSyncL( EContinueConvert );
    CustomSyncL( reinterpret_cast<TInt>( aDestinationFrame ) );
    CustomSyncL( reinterpret_cast<TInt>( &aNoOfDecodedMBlocks ) );
    CustomSyncL( aFrameNumber );
    CustomSyncL( EReadyForAsync );
    CustomAsync( aRequestStatus, 0 );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::SupportedFormatsL
// ----------------------------------------------------------
//
EXPORT_C TInt CExtJpegDecoder::SupportedFormatsL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ESupportedFormats );
    TInt retVal = KErrNotFound;
    CustomSyncL( reinterpret_cast<TInt>( &retVal ) );
    CustomSyncL( EEnd );
    return retVal;
    }

// ----------------------------------------------------------
// CExtJpegDecoder::CapabilitiesL
// ----------------------------------------------------------
//
EXPORT_C TInt CExtJpegDecoder::CapabilitiesL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapabilities );
    TInt retVal = KErrNotFound;
    CustomSyncL( reinterpret_cast<TInt>( &retVal ) );
    CustomSyncL( EEnd );
    return retVal;
    }

// ----------------------------------------------------------
// CExtJpegDecoder::CExtJpegDecoder
// ----------------------------------------------------------
//
CExtJpegDecoder::CExtJpegDecoder() : CJPEGImageFrameDecoder()
    {
    }

// ----------------------------------------------------------
// CExtJpegDecoder::Convert
// ----------------------------------------------------------
//
EXPORT_C  void CExtJpegDecoder::Convert( TRequestStatus* aRequestStatus,
                                         CFbsBitmap& aDestination, TInt aFrameNumber )    
    {
    iIsExtConvert = EFalse;
    CImageDecoder::Convert( aRequestStatus, aDestination, aFrameNumber );
    }

// ----------------------------------------------------------
// CExtJpegDecoder::Convert
// ----------------------------------------------------------
//
EXPORT_C  void CExtJpegDecoder::Convert( TRequestStatus* aRequestStatus, 
                                         CFbsBitmap& aDestination, 
                                         CFbsBitmap& aDestinationMask, TInt aFrameNumber )
    {
    iIsExtConvert = EFalse;
    CImageDecoder::Convert( aRequestStatus, aDestination, aDestinationMask, aFrameNumber );
    }
    
// ----------------------------------------------------------
// CExtJpegDecoder::ContinueConvert
// ----------------------------------------------------------
//
EXPORT_C void CExtJpegDecoder::ContinueConvert( TRequestStatus* aRequestStatus )
    {
    if( iIsExtConvert )
        {
        User::RequestComplete( aRequestStatus, KErrUnknown );
        }
    else
        {
        CImageDecoder::ContinueConvert( aRequestStatus );
        }    
    } 
      
// ----------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ----------------------------------------------------------
//                             
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::FileNewL( 
                            const TDecoderType aDecoderType,
                            RFs& aFs, 
                            const TDesC& aSourceFileName, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aDecoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegDecoder::FileNewL( aFs, aSourceFileName, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );
    }

// ----------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ----------------------------------------------------------
// 
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                            const TDecoderType aDecoderType,
                            RFs& aFs, 
                            const TDesC8& aSourceData, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aDecoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegDecoder::DataNewL( aFs, aSourceData, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );

    }
                                    
// ----------------------------------------------------------
// CExtJpegDecoder* CExtJpegDecoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegDecoder
// ----------------------------------------------------------
// 
EXPORT_C CExtJpegDecoder* CExtJpegDecoder::DataNewL( 
                            const TDecoderType aDecoderType, 
                            RFs& aFs, 
                            const CVisualFrame* aSourceData, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aDecoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegDecoder::DataNewL( aFs, aSourceData, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );
    }
    
    
// ----------------------------------------------------------
// CExtJpegDecoder::GetUidByVersionRangeL
// ----------------------------------------------------------
// 
TUid CExtJpegDecoder::GetUidByVersionRangeL( TInt aMinVersion, TInt aMaxVersion )
    {
    TUid uId = KNullUid;
    TUid propertyUid = KUidSwCodec; 
    
    RUidDataArray implArray;
    // based on the version decide what kind of codec to fetch
    if( ( aMinVersion == KMinSwVersion ) && ( aMaxVersion == KMaxSwVersion ) )
        {
        propertyUid = KUidSwCodec; 
        }
    else if( ( aMinVersion == KMinHwVersion ) && ( aMaxVersion == KMaxHwVersion ) )
        {
        propertyUid = KUidHwCodec; 
        }
        
    // property array to be verified
    const TUid properties[] = { propertyUid, KImageTypeJPGUid };
    
    // Request existing plugins with the desired properties
    TRAPD( getIntErr, CImageDecoder::GetInterfaceImplementationsL( 
                                                properties, 
                                                2, 
                                                implArray ) ); 
	CleanupClosePushL( implArray );                                                
    User::LeaveIfError( getIntErr );
    
    TInt count = implArray.Count();
    for( --count; uId == KNullUid && count >= 0; --count )
        {
        // Get the Jpeg decoder UID and find out whether it is requested
        // type of decoder by version number.
        TUid tempUid = implArray[ count ];
        CImplementationInformationType* implInfo = NULL;
        TRAPD( error, implInfo = 
            CImageDecoder::GetImplementationInformationL( tempUid ) );
            
        User::LeaveIfError( error );
                
        // check the opaque data and version no
        TInt version = implInfo->Version();
        if ( ( implInfo->OpaqueData().Find( KJpgMimeType ) != KErrNotFound )
            && ( version <= aMaxVersion ) && ( version >= aMinVersion ) )
            {
            uId = tempUid;
            }
        delete implInfo;    
        } // for - count
    CleanupStack::PopAndDestroy(&implArray);
    return uId;

    }



/*****************************************************/
/* Extended ICL Jpeg Encoder API                     */
/*****************************************************/


// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::NewL()
    {
    return new (ELeave) CExtJpegEncoder();
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::FileNewL( 
                                            RFs& aFs, 
                                            const TDesC& aDestinationFilename, 
                                            const TDesC8& aMIMEType, 
                                            const TOptions aOptions )
    {
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                                CImageEncoder::FileNewL( aFs, aDestinationFilename, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::FileNewL( 
                                            RFs& aFs, 
                                            const TDesC& aDestinationFilename, 
                                            const TOptions aOptions, 
                                            const TUid aImageType, 
                                            const TUid aImageSubType, 
                                            const TUid aEncoderUid )
    {
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                                CImageEncoder::FileNewL( aFs, aDestinationFilename, 
                                aOptions, aImageType, aImageSubType, aEncoderUid ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                                            HBufC8*& aDestinationData, 
                                            const TDesC8& aMIMEType, 
                                            const TOptions aOptions )
    {
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                                CImageEncoder::DataNewL( aDestinationData, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                                            HBufC8*& aDestinationData, 
                                            const TOptions aOptions, 
                                            const TUid aImageType, 
                                            const TUid aImageSubType, 
                                            const TUid aEncoderUid )
    {
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                                CImageEncoder::DataNewL( aDestinationData, 
                                aOptions, aImageType, aImageSubType, aEncoderUid ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                                            const CVisualFrame* aDestinationData, 
                                            const TDesC8& aMIMEType, 
                                            const TOptions aOptions )
    {
    HBufC8* tmp = NULL;
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                                CImageEncoder::DataNewL( tmp, 
                                aMIMEType, aOptions ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    enc->CustomSyncL( KExtensionUID );
    enc->CustomSyncL( EDestVisualFrame );
    enc->CustomSyncL( reinterpret_cast<TInt>( aDestinationData ) );
    enc->CustomSyncL( EEnd );
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                                            const CVisualFrame* aDestinationData, 
                                            const TOptions aOptions, 
                                            const TUid aImageType, 
                                            const TUid aImageSubType, 
                                            const TUid aEncoderUid )
    {
    HBufC8* tmp = NULL;
    CExtJpegEncoder* enc = reinterpret_cast<CExtJpegEncoder*>( 
                            CImageEncoder::DataNewL( tmp, aOptions, 
                            aImageType, aImageSubType, aEncoderUid ) );
    CleanupStack::PushL( enc );
    if ( enc->CapabilitiesL() < ECapNone )
        {
        User::Leave( KErrNotFound );
        }
    enc->CustomSyncL( KExtensionUID );
    enc->CustomSyncL( EDestVisualFrame );
    enc->CustomSyncL( reinterpret_cast<TInt>( aDestinationData ) );
    enc->CustomSyncL( EEnd );
    CleanupStack::Pop();
    return enc;
    }

// ---------------------------------------------------------
// CExtJpegEncoder::~CExtJpegEncoder
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder::~CExtJpegEncoder()
    {
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetStreamingL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetStreamingL( TSize& aMacroBlockSize, 
                                              const CFrameImageData* aFrameImageData )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapStreaming );
    CustomSyncL( reinterpret_cast<TInt>( &aMacroBlockSize ) );
    CustomSyncL( reinterpret_cast<TInt>( aFrameImageData ) );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetImageReplaceL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetImageReplaceL( const CVisualFrame* aReplaceImage, 
                                                 TPoint aReplacePoint )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapImageReplacing );
    CustomSyncL( reinterpret_cast<TInt>( aReplaceImage ) );
    CustomSyncL( reinterpret_cast<TInt>( &aReplacePoint ) );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetBitmapReplaceL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetBitmapReplaceL( const CFbsBitmap& aReplaceBitmap, 
                                                  TPoint aReplacePoint )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapBitmapReplacing );
    CustomSyncL( reinterpret_cast<TInt>( &aReplaceBitmap ) );
    CustomSyncL( reinterpret_cast<TInt>( &aReplacePoint ) );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetBitmapOverlayL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetBitmapOverlayL( const CFbsBitmap& aOverlayBitmap, 
                                                  TUint aAlphaValue, TPoint aOverlayPoint )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapBitmapOverlay );
    CustomSyncL( reinterpret_cast<TInt>( &aOverlayBitmap ) );
    CustomSyncL( aAlphaValue );
    CustomSyncL( reinterpret_cast<TInt>( &aOverlayPoint ) );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetLosslessRotationL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetLosslessRotationL( TInt aDegree )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapLosslessRotation );
    CustomSyncL( aDegree );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetLosslessFlippingL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetLosslessFlippingL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapLosslessFlipping );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetLosslessMirroringL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetLosslessMirroringL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapLosslessMirroring );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SetDctEncodingL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::SetDctEncodingL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapDctEncoding );
    CustomSyncL( EEnd );
    }
    
// ---------------------------------------------------------
// CExtJpegEncoder::ProvideNewDestDataL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::ProvideNewDestDataL( const CVisualFrame* aDestinationData )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ENewDestData );
    CustomSyncL( reinterpret_cast<TInt>( aDestinationData ) );
    CustomSyncL( EEnd );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::ConvertL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::ConvertL( 
                TRequestStatus* aRequestStatus, 
                const CVisualFrame* aSourceFrame, 
                TInt& aNoOfEncodedMBlocks,
                const CFrameImageData* aFrameImageData )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( EConvert );
    CustomSyncL( reinterpret_cast<TInt>( aSourceFrame ) );
    CustomSyncL( reinterpret_cast<TInt>( &aNoOfEncodedMBlocks ) );
    CustomSyncL( reinterpret_cast<TInt>( aFrameImageData ) );
    CustomSyncL( EReadyForAsync );
    CustomAsync( aRequestStatus, 0 );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::ContinueConvertL
// ---------------------------------------------------------
//
EXPORT_C void CExtJpegEncoder::ContinueConvertL( 
                TRequestStatus* aRequestStatus, 
                const CVisualFrame* aSourceFrame,
                TInt& aNoOfEncodedMBlocks )
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( EContinueConvert );
    CustomSyncL( reinterpret_cast<TInt>( aSourceFrame ) );
    CustomSyncL( reinterpret_cast<TInt>( &aNoOfEncodedMBlocks ) );
    CustomSyncL( EReadyForAsync );
    CustomAsync( aRequestStatus, 0 );
    }

// ---------------------------------------------------------
// CExtJpegEncoder::SupportedFormatsL
// ---------------------------------------------------------
//
EXPORT_C TInt CExtJpegEncoder::SupportedFormatsL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ESupportedFormats );
    TInt retVal = KErrNotFound;
    CustomSyncL( reinterpret_cast<TInt>( &retVal ) );
    CustomSyncL( EEnd );
    return retVal;
    }

// ---------------------------------------------------------
// CExtJpegEncoder::CapabilitiesL
// ---------------------------------------------------------
//
EXPORT_C TInt CExtJpegEncoder::CapabilitiesL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( ECapabilities );
    TInt retVal = KErrNotFound;
    CustomSyncL( reinterpret_cast<TInt>( &retVal ) );
    CustomSyncL( EEnd );
    return retVal;
    }

// ---------------------------------------------------------
// CExtJpegEncoder::CExtJpegEncoder
// ---------------------------------------------------------
//
CExtJpegEncoder::CExtJpegEncoder() : CJPEGImageFrameEncoder()
    {
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::FileNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::FileNewL( 
                            const TEncoderType aEncoderType,
                            RFs& aFs, 
                            const TDesC& aDestinationFilename, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aEncoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegEncoder::FileNewL( aFs, aDestinationFilename, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );
    }

// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                            const TEncoderType aEncoderType,
                            HBufC8*& aDestinationData, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aEncoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegEncoder::DataNewL( aDestinationData, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );

    }
                                    
// ---------------------------------------------------------
// CExtJpegEncoder* CExtJpegEncoder::DataNewL
// Creates, initializes and returns a pointer to an object of 
// class CExtJpegEncoder
// ---------------------------------------------------------
//           
EXPORT_C CExtJpegEncoder* CExtJpegEncoder::DataNewL( 
                            const TEncoderType aEncoderType, 
                            const CVisualFrame* aDestinationData, 
                            const TOptions aOptions )
    {
    TInt versionMin = KMinSwVersion;
    TInt versionMax = KMaxSwVersion;
    if ( aEncoderType == EHwImplementation )
        {
        versionMin = KMinHwVersion;
        versionMax = KMaxHwVersion;
        }
    TUid uId = GetUidByVersionRangeL( versionMin, versionMax );
    if ( uId == KNullUid )
        {
        User::Leave( KErrNotFound );
        }
    return CExtJpegEncoder::DataNewL( aDestinationData, aOptions, 
                                      KImageTypeJPGUid, KNullUid, uId );
    }
    
// ---------------------------------------------------------
// CExtJpegEncoder::GetDestinationDataSizeL
// ---------------------------------------------------------
//
EXPORT_C TInt CExtJpegEncoder::GetDestinationDataSizeL()
    {
    CustomSyncL( KExtensionUID );
    CustomSyncL( EDestDataSize );
    TInt retVal = KErrNotFound;
    CustomSyncL( reinterpret_cast<TInt>( &retVal ) );
    CustomSyncL( EEnd );
    return retVal;
    }

// ---------------------------------------------------------
// CExtJpegEncoder::GetUidByVersionRangeL
// ---------------------------------------------------------
//
TUid CExtJpegEncoder::GetUidByVersionRangeL( TInt aMinVersion, TInt aMaxVersion )
    {
    TUid uId = KNullUid;
    TUid propertyUid = KUidSwCodec; 
    
    // based on the version decide what kind of codec to fetch
    if( ( aMinVersion == KMinSwVersion ) && ( aMaxVersion == KMaxSwVersion ) )
        {
        propertyUid = KUidSwCodec; 
        }
    else if( ( aMinVersion == KMinHwVersion ) && ( aMaxVersion == KMaxHwVersion ) )
        {
        propertyUid = KUidHwCodec; 
        }
        
    // property array to be verified
    const TUid properties[] = { propertyUid, KImageTypeJPGUid };
    
    RUidDataArray implArray;
    // Request existing plugins with the desired properties
    TRAPD( getIntErr, CImageEncoder::GetInterfaceImplementationsL( 
                                                properties, 
                                                2, 
                                                implArray ) ); 
    CleanupClosePushL( implArray );
    User::LeaveIfError( getIntErr );
    
    TInt count = implArray.Count();
    for( --count; uId == KNullUid && count >= 0; --count )
        {
        // Check all encoders and find the one having Jpeg mime type, 
        TUid tempUid = implArray[ count ];
        
        // Get the same encoder UID and find out more info for testing
        TUid uId2 = { KEncoderInterfaceUidValue };
        RImplInfoPtrArray implInfo;
        REComSession::ListImplementationsL( uId2, implInfo );
        TInt count2 = implInfo.Count();
        uId2 = tempUid;
        for ( --count2; uId == KNullUid && count2 >= 0; --count2 )
            {
            CImplementationInformation& implInfoCur = *implInfo[count2];
            if ( ( implInfoCur.ImplementationUid() == uId2 ) && 
                ( implInfoCur.OpaqueData().Find( KJpgMimeType ) != KErrNotFound ) ) 
                {
                uId = uId2; 
                }
			}
        implInfo.ResetAndDestroy();
        } // for - count
        
    CleanupStack::PopAndDestroy( &implArray );   
    return uId;
    }

// End of File
