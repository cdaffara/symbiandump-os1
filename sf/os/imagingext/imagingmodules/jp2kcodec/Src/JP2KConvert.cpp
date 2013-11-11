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
* Description:  JPEG2000 image encoder/decoder plugin class.
*
*/


// INCLUDE FILES
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KConvert.h"
#include "JP2KStreamReader.h"
#include "JP2KImageClientMain.h"
#include "JP2KCodec.h"
#include <barsc.h>
#include <imageconversion.h>
#include <barsread.h>
#include <bautils.h>
#include <e32math.h>
#include <101F862D_extra.rsg>
#include <icl/icl_uids.hrh>
#include "JP2KUids.hrh"
#include <JP2KImageData.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KJ2KPanicCategory, "J2KConvertPlugin" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJp2kDecoder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJp2kDecoder* CJp2kDecoder::NewL()
    {
    return new ( ELeave ) CJp2kDecoder;
    }

// Destructor
CJp2kDecoder::~CJp2kDecoder()
    {
    // Call base class Cleanup(), else there is a possibility
    // of memory leak
    CImageDecoderPlugin::Cleanup();
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ImageType
// Get the image type uid of this plugin.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ImageType( TInt aFrameNumber, TUid& aImageType, TUid& aImageSubType ) const
    {
    __ASSERT_ALWAYS( aFrameNumber == 0, Panic( EFrameNumberOutOfRange ) );

    TUid uid = {KImageTypeJ2KUid};
    aImageType = uid;
    aImageSubType = KNullUid;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::NumberOfImageComments
// Get the number of image comment.
// -----------------------------------------------------------------------------
//
TInt CJp2kDecoder::NumberOfImageComments() const
    {
    __ASSERT_ALWAYS( IsImageHeaderProcessingComplete(), Panic( EHeaderProcessingNotComplete ) );

    const TUid commentUid = { KJ2KCommentUid };
    const CFrameImageData& frameImageData = FrameData( 0 );
    const TInt imageDataCount = frameImageData.ImageDataCount();
    const TImageDataBlock *imageData = 0;
    TInt imageCommentCount = 0;
    for ( TInt index = 0; index < imageDataCount; ++index )
        {
        imageData = frameImageData.GetImageData( index );
        if ( imageData->DataType() == commentUid )
            {
            imageCommentCount++;
            }
        }
    return imageCommentCount;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ImageCommentL
// Get the image comment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CJp2kDecoder::ImageCommentL( TInt aCommentNumber ) const
    {
    __ASSERT_ALWAYS( IsImageHeaderProcessingComplete(), Panic( EHeaderProcessingNotComplete ) );
    __ASSERT_ALWAYS( ( aCommentNumber >= 0 ) && ( aCommentNumber < NumberOfImageComments() ), 
                     Panic( ECommentNumberOutOfRange ) );
    
    HBufC *comment = 0;

    if( FrameData( 0 ).ImageDataCount() > 0 )
        {
        const TUid commentUid = { KJ2KCommentUid };
        const CFrameImageData& frameImageData = FrameData( 0 );
        const TInt imageDataCount = frameImageData.ImageDataCount();
        const TImageDataBlock *imageData = 0;
        TInt commentCount = 0;
        for ( TInt index = 0; index < imageDataCount; ++index )
            {
            imageData = frameImageData.GetImageData( index );
            if ( imageData->DataType() == commentUid )
                {
                if ( commentCount == aCommentNumber )
                    {
                    index = imageDataCount; // Break from the for loop
                    }
                else
                    {
                    commentCount++;
                    }
                }
            }

        if( imageData != 0 )
            {
            const TJp2kComment *jp2kComment = STATIC_CAST( const TJp2kComment*, imageData );
            comment = HBufC::NewL( jp2kComment->iComment->Length() );

            // Create 16 bit copy of the 8 bit original
            comment->Des().Copy( *( jp2kComment->iComment ) );
            }
        }
    return comment;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::NumberOfFrameComments
// Get the number of frame comment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CJp2kDecoder::NumberOfFrameComments( TInt aFrameNumber ) const
    {
    __ASSERT_ALWAYS( IsImageHeaderProcessingComplete(), Panic( EHeaderProcessingNotComplete ) );
    __ASSERT_ALWAYS( aFrameNumber == 0, Panic( EFrameNumberOutOfRange ) );
   
    return NumberOfImageComments();
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::FrameCommentL
// Get the frame comment.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CJp2kDecoder::FrameCommentL( TInt aFrameNumber, TInt aCommentNumber ) const
    {
    __ASSERT_ALWAYS( IsImageHeaderProcessingComplete(), Panic( EHeaderProcessingNotComplete ) );
    __ASSERT_ALWAYS( aFrameNumber == 0, Panic( EFrameNumberOutOfRange ) );

    return ImageCommentL( aCommentNumber );
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::FrameInfoStringsL
// Retrieve frame information and format it according to info stored in .rss file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CFrameInfoStrings* CJp2kDecoder::FrameInfoStringsL( RFs& aFs, TInt aFrameNumber )
    {
    const TUid KJp2kCodecDllUid = { KJ2KCodecDllUidValue };

    RResourceFile resourceFile;
    OpenExtraResourceFileLC( aFs, KJp2kCodecDllUid, resourceFile );

    HBufC8* resourceInfo = resourceFile.AllocReadLC( THEDECODERINFO );
    TResourceReader resourceReader;
    resourceReader.SetBuffer( resourceInfo );

    TBuf<KCodecResourceStringMax> info;
    TBuf<KCodecResourceStringMax> templte;

    const TFrameInfo& frameInfo = FrameInfo( aFrameNumber );
    CFrameInfoStrings* frameInfoStrings = CFrameInfoStrings::NewLC();

    info = resourceReader.ReadTPtrC();
    frameInfoStrings->SetDecoderL( info );

    info = resourceReader.ReadTPtrC();
    frameInfoStrings->SetFormatL( info );

    TInt width = frameInfo.iOverallSizeInPixels.iWidth;
    TInt height = frameInfo.iOverallSizeInPixels.iHeight;
    TInt depth = frameInfo.iBitsPerPixel;

    templte = resourceReader.ReadTPtrC();
    info.Format( templte, width, height );
    frameInfoStrings->SetDimensionsL( info );

    CDesCArrayFlat* resourceArray = resourceReader.ReadDesCArrayL();
    CleanupStack::PushL( resourceArray );
    TUint formatIndex = ( frameInfo.iFlags & TFrameInfo::EColor ) ? 1 : 0;
    templte = ( *resourceArray )[formatIndex];
    CleanupStack::PopAndDestroy( resourceArray );
    info.Format( templte, depth );
    frameInfoStrings->SetDepthL( info );

    resourceArray = resourceReader.ReadDesCArrayL();
    CleanupStack::PushL( resourceArray );
    formatIndex = 0;
    info = ( *resourceArray )[formatIndex];
    CleanupStack::PopAndDestroy( resourceArray );
    frameInfoStrings->SetDetailsL( info );

    CleanupStack::Pop( frameInfoStrings );
    CleanupStack::PopAndDestroy( 2 ); // resourceInfo + resourceFile
    return frameInfoStrings;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::FrameHeaderBlockSize
// Returns the frame header block size.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CJp2kDecoder::FrameHeaderBlockSize( TInt aFrameNumber ) const
    {
    __ASSERT_ALWAYS( aFrameNumber == 0, Panic( EFrameNumberOutOfRange ) );

    return KJ2kInputBufferSize;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::FrameBlockSize
// Returns the frame block size.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CJp2kDecoder::FrameBlockSize(TInt aFrameNumber) const
    {
    __ASSERT_ALWAYS( aFrameNumber == 0, Panic( EFrameNumberOutOfRange ) );

    return KJ2kInputBufferSize;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::DoConvert
// Override the default DoConvert to split out the parsing and decoding
// process into multiple steps.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::DoConvert()
    {
    TFrameState codecState = EFrameIncomplete;
    TInt errCode = KErrNone;

    CJp2kReadCodec *j2kCodec = static_cast<CJp2kReadCodec*>( ImageReadCodec() );

    if ( !j2kCodec->IsDecodeTile() )
        {
        // parsing of a tile-part
        TRAP( errCode, PrepareForProcessFrameL() );
        if ( errCode != KErrNone )
            {
            RequestComplete( errCode );
            return;
            }

        TBufPtr8& sourceData = SourceData();
        TInt sourceLength = sourceData.Length();
        if ( sourceLength != 0 )
            {
            TRAP( errCode, codecState = j2kCodec->ProcessFrameL( sourceData ) );
            
            if ( j2kCodec->IsDecodeTile() )
                {
                // tile-part contains all compressed data
                // and proceed to the decoding
                SelfComplete( KErrNone );
                return;
                }
            }
        // continue on parsing the tile-part
        HandleProcessFrameResult( errCode, codecState );
        }
    else
        {
        // decoding of the tile-part
        TRAP( errCode, codecState = j2kCodec->DecodeTileL() );

        if ( errCode == KErrNone )
            {
            if ( j2kCodec->IsDecodeTile() )
                {
                // continue on decoding the tile-part
                // if we split the decoding into multiple steps
                SelfComplete( KErrNone );
                return;
                }
            }
        // either continue on parsing the next tile-part
        // or done with the whole image
        HandleProcessFrameResult( errCode, codecState );
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::CJp2kDecoder
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJp2kDecoder::CJp2kDecoder()
    {
    }  

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadFormatL
// JP2 File Format detection state machine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadFormatL()
    {
    // Verify the JP2 file format if it exist
    iCallFromFramework = ETrue;

    while ( iState != EStateInCodestreamBox )
        {
        switch ( iState )
            {
            case EStateInSignatureBox:
                {
                ReadSignatureBoxL();
                break;
                }
            case EStateInFileTypeBox:
                {
                ReadFileTypeBoxL();
                break;
                }
            case EStateInJP2SuperBox:
                {
                ReadJp2HeaderBoxL();
                break;
                }
            case EStateInIPRBox:
                {
                ReadIPRBoxL();
                break;
                }
            case EStateInXMLBox:
                {
                ReadXMLBoxL();
                break;
                }
            case EStateInUUIDBox:
                {
                ReadUUIDBoxL();
                break;
                }
            case EStateInUUIDInfoBox:
                {
                ReadUUIDInfoBoxL();
                break;
                }
            case EStateInUnknown:
                {
                if ( iCallFromFramework )
                    {
                    // Always read the length and type of the box
                    ReadDataL( iLastRead, iBufferDes, KJ2kBoxTypeLength );

                    if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
                        {
                        if ( ( iJ2kInfo.iCSOffset != 0 ) && 
                             ( (TUint32)iBufferDes.Length() == 0 ) )
                            {
                            // We have reach the end of file/stream and 
                            // CodeStream box has been recognized
                            iState = EStateInCodestreamBox;
                            }
                        else
                            {
                            // Need more data from the ICL framework
                            User::Leave( KErrUnderflow );
                            }
                        }
                    iPtr = iBufferDes.Ptr();
                    }

                if ( iState != EStateInCodestreamBox )
                    {
                    // Length and Type of next box
                    iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
                    iBoxType = PtrReadUtil::ReadBigEndianUint32( iPtr );

                    // Update the internal state based on the iBoxType
                    UpdateStateFromBoxTypeL();

                    if ( iState != EStateInCodestreamBox )
                        {
                        // Corrupted box length
                        if ( ( (TInt32)iBoxLength ) < 0 )
                            {
                            User::Leave( KErrCorrupt );
                            }
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        iCallFromFramework = EFalse;
        }

    // When the code reach here, it means that we have found the 
    // codestream box in JP2 file format or the codestream SOC itself
    // in case of the JP2 file format, all the boxes after the
    // codestream box will not be read and parsed.
    
    // In JP2 file format, iLastRead points to codestream box
    // in JP2 codestream, iLastRead points to SOC marker
    // it is the offset that ICL framework used to read
    // in more data from the image file.
    SetStartPosition( iJ2kInfo.iCSOffset );

    // Always set the image data length to KMaxTInt32
    SetDataLength( KMaxTInt );

    // Fill up the image related information, if it is
    // in JP2 file format, else defer the setting of the
    // image info til SIZ marker in the codestream has
    // been processed.
    if ( iJ2kInfo.iOption & TJ2kInfo::EJP2file )
        {
        TFrameInfo& imageInfo = CONST_CAST( TFrameInfo&, ImageInfo() );
        imageInfo.iOverallSizeInPixels = iJ2kInfo.iImageSize;
        imageInfo.iFrameCoordsInPixels.SetRect( TPoint( 0, 0 ), iJ2kInfo.iImageSize );
        imageInfo.iFrameSizeInTwips = iJ2kInfo.iImageSizeInTwips;

        if ( iJ2kInfo.iBPCList.Count() > 0 )
            {
            // Calculate based on the bitdepth per component
            for ( TUint16 index = 0; index < iJ2kInfo.iNC; ++index )
                {
                imageInfo.iBitsPerPixel += ( ( 0x7f & iJ2kInfo.iBPCList[index] ) + 1 );
                }
            }
        else
            {            
            if ( iJ2kInfo.iBPC == KJ2kIsBPCBoxExist )
                {
                User::Leave( KErrCorrupt );
                }

            // Has the same bitdepth on each component
            imageInfo.iBitsPerPixel = ( ( 0x7f & iJ2kInfo.iBPC ) + 1 ) * iJ2kInfo.iNC;
            }
        imageInfo.iFlags |= TFrameInfo::ECanDither;

        // Decoder is able to handle scaleable resolution
        imageInfo.iFlags |= TFrameInfo::EConstantAspectRatio;

        if ( iJ2kInfo.iEnumCS != KJ2kColourSpecGrayScale )
            {
            imageInfo.iFlags |= TFrameInfo::EColor;
            }
        
        // No animation.
        imageInfo.iDelay = 0;

        switch ( imageInfo.iBitsPerPixel )
            {
            case 1:
                {
                imageInfo.iFrameDisplayMode = EGray2;
                break;
                }
            case 2:
                {
                imageInfo.iFrameDisplayMode = EGray4;
                break;
                }
            case 4:
                {
                imageInfo.iFrameDisplayMode = ( imageInfo.iFlags & TFrameInfo::EColor ) ? EColor16 : EGray16;
                break;
                }
            case 8:
                {
                imageInfo.iFrameDisplayMode = ( imageInfo.iFlags & TFrameInfo::EColor ) ? EColor16M : EGray256;
                break;
                }
            case 12:
                {
                imageInfo.iFrameDisplayMode = ( imageInfo.iFlags & TFrameInfo::EColor ) ? EColor4K : EGray256;
                break;
                }
            case 16:
                {
                imageInfo.iFrameDisplayMode = EColor64K;
                break;
                }
            case 24:
                {
                imageInfo.iFrameDisplayMode = EColor16M;
                break;
                }
            default:
                {
                imageInfo.iFrameDisplayMode = EColor64K;
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadSignatureBoxL
// Verify and process the Signature box.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadSignatureBoxL(  )
    {
    // Validate the Signature Box
    __ASSERT_ALWAYS( iLastRead == 0, Panic( EInvalidState ) );

    // Read the Signature box + the next box's Length and Type
    ReadDataL( iLastRead, iBufferDes, KJ2kFileInformationSize );

    if ( (TUint32)iBufferDes.Length() < KJ2kSigBoxLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    if ( ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kSigBoxLength ) ||
         ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kSigBoxType )   ||
         ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kSigBoxContent ) )
        {
        // Unrecognized signature box
        User::Leave( KErrCorrupt );
        }

    // File Type box shall immediately follow the Signature box
    iLastRead = KJ2kSigBoxLength;
    iState = EStateInFileTypeBox;

    // JP2 file format
    iJ2kInfo.iOption |= TJ2kInfo::EJP2file;

    if ( iBufferDes.Length() < KJ2kFileInformationSize )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadFileTypeBoxL
// Verify and process the File Type box.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadFileTypeBoxL()
    {
    // Validate the File Type Box's Length and Type
    if ( iCallFromFramework )
        {
        __ASSERT_ALWAYS( iLastRead == KJ2kSigBoxLength, Panic( EInvalidState ) );

        ReadDataL( iLastRead, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        }

    iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
    iBoxType = PtrReadUtil::ReadBigEndianUint32( iPtr );

    if ( ( (TInt32)iBoxLength ) <= 0 || iBoxType != KJ2kFileTypeBox || iBoxLength > KJ2kFileTypeBoxMaxLength )
        {
        // Unrecognized file type box or corrupted box length
        User::Leave( KErrCorrupt );
        }

    TUint8 xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    if ( ( (TInt32)iBoxLength ) <= 0 || ( ( (TInt32)iBoxLength - 16 ) / 4 ) <= 0 )
        {
        // Must include at least one CL field in the Compatibility list
        User::Leave( KErrCorrupt );
        }

    // Read the File Type box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    // Substract next box's KJ2kBoxTypeLength
    const TUint8 *ptrLimit = iPtr + iBoxLength - KJ2kBoxTypeLength;

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    // Check Brand ( BR ) or Major Version
    if ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kFileTypeBrand )
        {
        // It is ok at this point if Major Version does not match
        iPtr += 0;
        }

    // Check Minor Version ( MinV )
    if ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kFileTypeMinV )
        {
        // It is ok at this point if Minor Version does not match
        iPtr += 0;
        }

    // Check Compatibility List ( CL )
    TBool clFound = EFalse;
    TUint32 clField = 0;
    while ( ( iPtr + 4 ) <= ptrLimit  )
        {
        clField = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        if ( clField == KJ2kFileTypeBrand )
            {
            clFound = ETrue;
            }
        else if ( clField == KJ2kFileTypeProfile0 )
            {
            iJ2kInfo.iOption |= TJ2kInfo::EProfile0;
            }
        else if ( clField == KJ2kFileTypeProfile1 )
            {
            iJ2kInfo.iOption |= TJ2kInfo::EProfile1;
            }
        else
            {
            // other file types require no action
            }
        }

    if ( !clFound || iPtr != ptrLimit )
        {
        // Unrecognized compatibility list or data misaligned
        User::Leave( KErrCorrupt );
        }

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadJp2HeaderBoxL
// Verify and process the JP2 Header box ( superbox ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadJp2HeaderBoxL()
    {
    // Since we are getting data from ICL framework
    // we have no control of when will be the EOF
    if ( iBoxLength == 0 )
        {
        User::Leave( KErrCorrupt );
        }

    TUint8 xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) <= 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Validate the JP2 Header Super Box
    // Read the JP2 box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    // Make sure that JP2 Header box contains enough data
    // for Image Header box
    if ( iBoxLength - KJ2kBoxTypeLength < KJ2kImageHeaderBoxLength )
        {
        // Missing Image Header box
        User::Leave( KErrCorrupt );
        }

    iPtr = iBufferDes.Ptr();

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    // Image Header box shall be the first box within JP2 Header box
    ReadImageHeaderBoxL();

    TUint32 subBoxLength = 0;
    TUint32 subBoxType = 0;

    // Substract next box's Length and Type
    const TUint8 *ptrLimit = iPtr + iBoxLength - KJ2kBoxTypeLength - KJ2kImageHeaderBoxLength;
    TInt32 length = ptrLimit - iPtr;
    TBool  csFound = EFalse;

    while ( length > 0 )
        {
        if ( ( ptrLimit - iPtr ) < 8  )
            {
            // Not enough room for box data
            User::Leave( KErrCorrupt );
            }

        subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        subBoxType = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );

        if ( subBoxLength == 1 )
            {
            // The XLBox shall exist and contains the actual length of the box
            // XLBox is 8 bytes width
            subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            subBoxLength += PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            length -= KJ2kBoxTypeLength;
            subBoxLength -= KJ2kBoxTypeLength;
            }

        if ( ( (TInt32)subBoxLength ) < 0 || ( subBoxLength <= 8 ) || ( ( iPtr + subBoxLength - KJ2kBoxTypeLength ) > ptrLimit ) )
            {
            // Not enough room for subbox data or corrupted subbox length
            User::Leave( KErrCorrupt );
            }

        switch ( subBoxType )
            {
            case KJ2kColourSpecBoxType:
                {
                if ( !csFound )
                    {
                    // Colour Spec Box
                    ReadColorSpecBoxL( subBoxLength );
                    csFound = ETrue;
                    }
                else
                    {
                    // Ignore the second Colour Spec Box,
                    // just advance the iterator pointer
                    iPtr += subBoxLength - KJ2kBoxTypeLength;
                    }
                break;
                }
            case KJ2kResolutionBoxType:
                {
                // Resolution box ( superbox )
                ReadResolutionBoxL( ptrLimit, subBoxLength );
                break;
                }
            case KJ2kBitsPerCompBoxType:
                {
                // Bits Per Component Box
                ReadBitsPerCompBoxL( subBoxLength );
                break;
                }
            case KJ2kPaletterBoxType:
                {
                // Palette Box
                ReadPaletteBoxL();
                break;
                }
            case KJ2kComponentMapBoxType:
                {
                // Component Mapping Box
                ReadComponentMapBoxL( subBoxLength );
                break;
                }
            case KJ2kChannelDefBoxType:
                {
                // Channel Definition Box
                ReadChannelDefBoxL( subBoxLength );
                break;
                }
            default:
                {
                // Unrecognized Box
                User::Leave( KErrCorrupt );
                break;
                }
            }
        length -= subBoxLength;
        }

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    // JP2 Header processed
    iJ2kInfo.iOption |= TJ2kInfo::EJP2Header;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadImageHeaderBoxL
// Verify and process the Image Header box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadImageHeaderBoxL()
    {
    // Validate the Image Header Box
    if ( ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kImageHeaderBoxLength ) ||
         ( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) != KJ2kImageHeaderBoxType ) )
        {
        // Unrecognized Image Header box
        User::Leave( KErrCorrupt );
        }

    iJ2kInfo.iImageSize.iHeight = (TInt)PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
    iJ2kInfo.iImageSize.iWidth = (TInt)PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
    iJ2kInfo.iNC  = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    iJ2kInfo.iBPC = *iPtr++;

    if ( (iJ2kInfo.iImageSize.iHeight <= 0) || (iJ2kInfo.iImageSize.iWidth <= 0) ||
         (iJ2kInfo.iNC == 0) || (iJ2kInfo.iBPC == 0) )
        {
        // Corrupted image parameters
        User::Leave( KErrCorrupt );
        }

    if ( *iPtr++ != KJ2kImageHeaderCompressionType )
        {
        // Unrecognized compression type
        User::Leave( KErrCorrupt );
        }

    // Check color space - UnkC
    TUint8 value = *iPtr++;
    if ( value == 0 || value == 1 )
        {
        iJ2kInfo.iOption |= ( value == 0 ) ? TJ2kInfo::EColorSpec : 0;
        }
    else
        {
        // Unrecognized color space
        User::Leave( KErrCorrupt );
        }

    // Check intellectual property - IPR 
    value = *iPtr++;
    if ( value == 0 || value == 1 )
        {
        iJ2kInfo.iOption |= value;
        }
    else
        {
        // Unrecognized intellectual property
        User::Leave( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadColorSpecBoxL
// Verify and process the Colour Specification box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadColorSpecBoxL( const TUint32 aBoxLength )
    {
    // Validate the Color Specification Box
    TUint8 method = *iPtr++;

    if ( method != 1 && method != 2 )
        {
        // Unrecognized method
        User::Leave( KErrCorrupt );
        }

    // Ignore the precedence
    iPtr++;

    // Ignore the approximation
    iPtr++;

    if ( method == 1 )
        {
        // Method must be 1 if we reach this point
        iJ2kInfo.iEnumCS = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        switch ( iJ2kInfo.iEnumCS )
            {
            case 16:
            case 17:
            case 18:
            case 21:
            case 22:
                {
                break;
                }
            default:
                {
                // Unrecognized colour specification
                User::Leave( KErrNotSupported );
                break;
                }
            }
        }
    else
        {
        // ICC profile
        TUint32 length = (TUint32)( aBoxLength - KJ2kBoxTypeLength - 3 );
        iJ2kInfo.iICCProfile = HBufC8::NewL( length );
        iJ2kInfo.iICCProfile->Des().Append( iPtr, length );
        iPtr += length;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadBitsPerCompBoxL
// Verify and process the Bits Per Component box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadBitsPerCompBoxL( TUint32 aBoxLength )
    {
    if ( iJ2kInfo.iBPC != KJ2kIsBPCBoxExist )
        {
        // Should not happen but we just advance 
        // the iterator and ignore the whole box
        iPtr += aBoxLength - KJ2kBoxTypeLength;
        return;
        }

    // Validate the Bits Per Component Box
    if ( ( aBoxLength - KJ2kBoxTypeLength ) != iJ2kInfo.iNC  )
        {
        // Unrecognized number of components
        User::Leave( KErrCorrupt );
        }

    while ( aBoxLength > KJ2kBoxTypeLength )
        {
        User::LeaveIfError( iJ2kInfo.iBPCList.Append( *iPtr++ ) );

        --aBoxLength;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadResolutionBoxL
// Verify and process the Resolution box ( in JP2 Header box, 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadResolutionBoxL( const TUint8 *aPtrLimit, TUint32 aBoxLength )
    {
    TUint32 subBoxLength = 0;
    TUint32 subBoxType = 0;

    aBoxLength -= KJ2kBoxTypeLength;
    while ( aBoxLength > 0 )
        {
        if ( (TUint32)( aPtrLimit - iPtr ) < KJ2kResSubBoxLength )
            {
            // Not enough room for subbox data
            User::Leave( KErrCorrupt );
            }

        subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        subBoxType = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );

        if ( subBoxLength == 1 )
            {
            // The XLBox shall exist and contains the actual length of the box
            // XLBox is 8 bytes width
            subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            subBoxLength += PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            aBoxLength -= KJ2kBoxTypeLength;
            subBoxLength -= KJ2kBoxTypeLength;
            }

        if ( subBoxLength != KJ2kResSubBoxLength )
            {
            // Corrupted resolution box length, both sub boxes have length of 18
            User::Leave( KErrCorrupt );
            }

        if ( subBoxType == KJ2kCaptureResBoxType )
            {
            ReadCaptureResBoxL( subBoxLength );
            }
        else if ( subBoxType == KJ2kDisplayResBoxType )
            {
            ReadDisplayResBoxL(  );
            }
        else
            {
            // Unrecognized box in Resolution super box
            User::Leave( KErrCorrupt );
            }
        aBoxLength -= subBoxLength;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadDisplayResBoxL
// Verify and process the Default Display Resolution box ( in Resolution box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadDisplayResBoxL()
    {
    // Store default display resolution
    TUint16 verNum = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    TUint16 verDen = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    TUint16 horNum = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    TUint16 horDen = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    TUint8  verExp = *iPtr++;
    TUint8  horExp = *iPtr++;
    TReal   tenPow = 0.0;

    // Check  the validity of the parameters
    if((verNum == 0) || (verDen == 0) || (horNum == 0) || (horDen == 0))
        {
        // Values corrupted, do not try to compute the SizeInTwips
        return;
        }

    User::LeaveIfError( Math::Pow10( tenPow, verExp ) );

    TReal result = ( iJ2kInfo.iImageSize.iWidth / ( ( (TReal)verNum / verDen ) * tenPow ) ) / KJ2kTwipM;
    iJ2kInfo.iImageSizeInTwips.iWidth = (TInt)result;

    User::LeaveIfError( Math::Pow10( tenPow,horExp ) );

    result = ( iJ2kInfo.iImageSize.iHeight / ( ( (TReal)horNum / horDen ) * tenPow ) ) / KJ2kTwipM;
    iJ2kInfo.iImageSizeInTwips.iHeight = (TInt)result;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadCaptureResBoxL
// Verify and process the Capture Resolution box ( in Resolution box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadCaptureResBoxL( const TUint32 aBoxLength )
    {
    // Ignore the capture resolution
    iPtr += aBoxLength - KJ2kBoxTypeLength;
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadPaletteBoxL
// Verify and process the Palette box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadPaletteBoxL()
    {
    // Validate the Palette Box
    TUint16 entries = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    TUint8  npc = *iPtr++;

    if((entries > KMaxPaletteEntries) || (entries == 0))
        {
        // Number of entries must be between 1 and 1024
        User::Leave( KErrCorrupt );
        }

    TInt rowIndex = 0;
    TInt columnIndex = 0;
    RArray<TUint> bpcInBytes;
    TUint8 bpc = 0;

    for ( rowIndex = 0; rowIndex < npc; ++rowIndex )
        {
        bpc = *iPtr++;
        User::LeaveIfError( iJ2kInfo.iPalette.iBList.Append( bpc ) );

        bpc = (TUint8)( ( 0x7f & bpc ) + 1 );
        User::LeaveIfError( bpcInBytes.Append( (TUint)( ( bpc % 8 ) ? bpc / 8 + 1 : bpc / 8 ) ) );
        }

    RArray<TUint> *cList;
    CleanupClosePushL( bpcInBytes );

    for ( rowIndex = 0; rowIndex < entries; ++rowIndex )
        {
        cList = new ( ELeave ) RArray<TUint>( npc );
        CleanupStack::PushL( cList );
        CleanupClosePushL( *cList );

        for ( columnIndex = 0; columnIndex < npc; ++columnIndex )
            {
            switch ( bpcInBytes[columnIndex] )
                {
                case 1:
                    {
                    User::LeaveIfError( cList->Append( (TUint)*iPtr++ ) );
                    break;
                    }
                case 2:
                    {
                    User::LeaveIfError( cList->Append( (TUint)PtrReadUtil::ReadBigEndianUint16Inc( iPtr ) ) );
                    break;
                    }
                case 3:
                    {
                    bpc = *iPtr++;
                    User::LeaveIfError( cList->Append( ( (TUint)bpc << 16 ) | PtrReadUtil::ReadBigEndianUint16Inc( iPtr ) ) );
                    break;
                    }
                case 4:
                    {
                    User::LeaveIfError( cList->Append( PtrReadUtil::ReadBigEndianUint32Inc( iPtr ) ) );
                    break;
                    }
                default:
                    {
                    // Unrecognized index
                    User::Leave( KErrCorrupt );
                    break;
                    }
                }
            }
        User::LeaveIfError( iJ2kInfo.iPalette.iC2DArray.Append( cList ) );
        CleanupStack::Pop(2);
        }
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadComponentMapBoxL
// Verify and process the Component Mapping box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadComponentMapBoxL( TUint32 aBoxLength )
    {
    // Validate the Component Mapping Box
    if ( ( aBoxLength - KJ2kBoxTypeLength ) % 4 != 0 )
        {
        // Misaligned Component Mapping box
        User::Leave( KErrCorrupt );    
        }

    TUint16 cmp = 0;
    TUint8 mtyp = 0;
    TUint8 pcol = 0;

    while ( aBoxLength > KJ2kBoxTypeLength )
        {
        cmp = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );

        if(cmp > KMaxComponents)
            {
            // Illegal component index
            User::Leave( KErrCorrupt );    
            }

        mtyp = *iPtr++;
        pcol = *iPtr++;
        if ( mtyp == 0 || mtyp == 1 )
            {
            if ( mtyp == 0 && pcol != 0 )
                {
                // Unrecognized matching between MTYP and PCOL
                User::Leave( KErrCorrupt );
                }
            }
        else
            {
            // Unrecognized MTYP
            User::Leave( KErrCorrupt );
            }

        User::LeaveIfError( iJ2kInfo.iCMPList.Append( TComponentMap( cmp, mtyp, pcol ) ) );
        aBoxLength -= 4;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadChannelDefBoxL
// Verify and process the Channel Definition box ( in JP2 Header box ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadChannelDefBoxL( TUint32 aBoxLength )
    {
    // Validate the Channel Definition Box
    TUint16 entries = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
    if ( ( aBoxLength - KJ2kBoxTypeLength - 2 ) / 6 != entries )
        {
        // Misaligned Channel Definition box
        User::Leave( KErrCorrupt );
        }

    TUint16 cn = 0;
    TUint16 typ = 0;
    TUint16 asoc = 0;

    while ( entries != 0 )
        {
        cn   = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
        typ  = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
        asoc = PtrReadUtil::ReadBigEndianUint16Inc( iPtr );
        User::LeaveIfError( iJ2kInfo.iCNList.Append( TChannelDefinition( cn, typ, asoc ) ) );
        --entries;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadIPRBoxL
// Verify and process the IPR box .
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadIPRBoxL()
    {
    // Check whether this IPR box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This IPR box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // IPR box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    TUint xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Check whether this IPR box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This IPR box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // IPR box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    // Validate the IPR Box
    // Read the IPR box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    TInt length = iBoxLength - ( ( 1 + xlBoxExist ) * KJ2kBoxTypeLength );

    HBufC8 *iprBuffer = HBufC8::NewLC( length );
    iprBuffer->Des().Append( iPtr, length );
    iPtr += length;

    User::LeaveIfError( iJ2kInfo.iIPRList.Append( iprBuffer ) );
    CleanupStack::Pop(); // iprBuffer

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Test the end of file/stream 
        ReadDataL( iLastRead, iBufferDes, 1 );
        if ( ( (TUint32)iBufferDes.Length() == 0 ) &&
             ( iJ2kInfo.iCSOffset != 0 ) )
            {
            // CodeStream box has been recognized
            iState = EStateInCodestreamBox;
            }
        else
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadXMLBoxL
// Verify and process the XML box.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadXMLBoxL()
    {
    // Check whether this XML box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This XML box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // XML box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    TUint xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Check whether this XML box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This XML box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // XML box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    // Validate the XML Box
    // Read the XML box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    TInt length = iBoxLength - ( ( 1 + xlBoxExist ) * KJ2kBoxTypeLength );

    HBufC8 *xmlBuffer = HBufC8::NewLC( length );
    xmlBuffer->Des().Append( iPtr, length );
    iPtr += length;

    User::LeaveIfError( iJ2kInfo.iXMLList.Append( xmlBuffer ) );
    CleanupStack::Pop(); // xmlBuffer

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Test the end of file 
        ReadDataL( iLastRead, iBufferDes, 1 );
        if ( ( (TUint32)iBufferDes.Length() == 0 ) &&
             ( iJ2kInfo.iCSOffset != 0 ) )
            {
            // CodeStream box has been recognized
            iState = EStateInCodestreamBox;
            }
        else
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadUUIDBoxL
// Verify and process UUID box.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadUUIDBoxL()
    {
    // Check whether this UUID box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This UUID box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // UUID box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    TUint xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Check whether this UUID box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This UUID box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // UUID box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    // Validate the UUID Box
    // Read the UUID box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    TInt length = iBoxLength - ( ( 1 + xlBoxExist ) * KJ2kBoxTypeLength );

    HBufC8 *uuidBuffer = HBufC8::NewLC( length );
    uuidBuffer->Des().Append( iPtr, length );
    iPtr += length;

    User::LeaveIfError( iJ2kInfo.iUUIDList.Append( uuidBuffer ) );
    CleanupStack::Pop(); // uuidBuffer

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Test the end of file 
        ReadDataL( iLastRead, iBufferDes, 1 );
        if ( ( (TUint32)iBufferDes.Length() == 0 ) &&
             ( iJ2kInfo.iCSOffset != 0 ) )
            {
            // CodeStream box has been recognized
            iState = EStateInCodestreamBox;
            }
        else
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ReadUUIDInfoBoxL
// Verify and process the UUID Info box ( superbox ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ReadUUIDInfoBoxL()
    {
    // Check whether this UUIDInfo box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This UUIDInfo box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // UUIDInfo box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    TUint xlBoxExist = 0;
    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        xlBoxExist = 1;
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Check whether this UUIDInfo box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This UUIDInfo box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // UUIDInfo box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    // Validate the UUIDInfo Box
    // Read the UUIDInfo box + the next box's Length and Type
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    // Substract next box's Length and Type
    const TUint8 *ptrLimit = iPtr + iBoxLength - KJ2kBoxTypeLength;

    // Advance the pointer by 8 bytes if XLBox exists
    iPtr += ( xlBoxExist * KJ2kBoxTypeLength );

    TUint32 subBoxLength = 0;
    TUint32 subBoxType = 0;

    TInt32 length = ptrLimit - iPtr;
    TInt32 dataLength = 0;
    TInt32 order = 0;

    HBufC8 *listBuffer = 0;
    HBufC8 *urlBuffer = 0;

    while ( length > 0 )
        {
        if ( ( ptrLimit - iPtr ) < 8  )
            {
            // Not enough room for box data
            User::Leave( KErrCorrupt );
            }

        subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        subBoxType = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );

        if ( subBoxLength == 1 )
            {
            // The XLBox shall exist and contains the actual length of the box
            // XLBox is 8 bytes width
            subBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            subBoxLength += PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
            length -= KJ2kBoxTypeLength;
            subBoxLength -= KJ2kBoxTypeLength;
            }

        if ( ( (TInt32)subBoxLength < 0 ) || ( subBoxLength <= 8 ) || ( ( iPtr + subBoxLength - KJ2kBoxTypeLength ) > ptrLimit ) )
            {
            // Not enough room for subbox data or corrupted subbox length
            User::Leave( KErrCorrupt );
            }

        switch ( subBoxType )
            {
            case KJ2kUUIDListBoxType:
                {
                // Validate the UUIDInfo List box
                // NU | ID-0 | ... | ID-n
                // where NU is 2 bytes unsigned specifying the number of IDs
                //       ID is 16 bytes UUID 
                TUint16 nuField = PtrReadUtil::ReadBigEndianUint16( iPtr );
                if ( ( subBoxLength - ( 2 + KJ2kBoxTypeLength ) ) != 
                     ( (TUint32)( nuField * KJ2KUuidIDSize ) ) )
                    {
                    // UUIDInfo List box does not confirm to standard format
                    User::Leave( KErrCorrupt );
                    }

                if ( listBuffer )
                    {
                    // There should be one UUIDInfo List box
                    User::Leave( KErrCorrupt );
                    }

                dataLength = subBoxLength - KJ2kBoxTypeLength;
                listBuffer = HBufC8::NewLC( dataLength );
                listBuffer->Des().Append( iPtr, dataLength );
                iPtr += ( dataLength );

                if ( order == 0 )
                    {
                    // Increment the order if UUIDInfo List box come first
                    ++order;
                    }
                break;
                }
            case KJ2kUUIDUrlBoxType:
                {
                // Validate the UUIDInfo Data Entry Url box
                if ( urlBuffer )
                    {
                    // There should be one UUIDInfo Data Entry Url box
                    User::Leave( KErrCorrupt );
                    }

                dataLength = subBoxLength - KJ2kBoxTypeLength;
                urlBuffer = HBufC8::NewLC( dataLength );
                urlBuffer->Des().Append( iPtr, dataLength );
                iPtr += ( dataLength );

                if ( order == 0 )
                    {
                    // Decrement the order if UUIDInfo Data Entry Url box come first
                    --order;
                    }
                break;
                }
            default:
                {
                // Unrecognized Box
                User::Leave( KErrCorrupt );
                break;
                }
            }
        length -= subBoxLength;
        }

    if ( listBuffer || urlBuffer )
        {
        if ( !( listBuffer ) )
            {
            // Create a dummy place holder
            listBuffer = HBufC8::NewLC( 1 );
            }

        if ( !( urlBuffer ) )
            {
            // Create a dummy place holder
            urlBuffer = HBufC8::NewLC( 1 );
            }

        if ( order < 0 )
            {
            // UUIDInfo Data Entry Url box come first
            User::LeaveIfError( iJ2kInfo.iUUIDInfoListList.Append( listBuffer ) );
            CleanupStack::Pop( 1 ); // listBuffer

            User::LeaveIfError( iJ2kInfo.iUUIDInfoUrlList.Append( urlBuffer ) );
            CleanupStack::Pop( 1 ); // urlBuffer
            }
        else
            {
            // UUIDInfo List box come first
            User::LeaveIfError( iJ2kInfo.iUUIDInfoUrlList.Append( urlBuffer ) );
            CleanupStack::Pop( 1 ); // urlBuffer

            User::LeaveIfError( iJ2kInfo.iUUIDInfoListList.Append( listBuffer ) );
            CleanupStack::Pop( 1 ); // listBuffer
            }
        }
    else
        {
        // UUIDInfo box does not containing any
        // UUIDInfo List box or UUIDInfo Data Entry Url box
        User::Leave( KErrCorrupt );
        }

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Test the end of file 
        ReadDataL( iLastRead, iBufferDes, 1 );
        if ( ( (TUint32)iBufferDes.Length() == 0 ) &&
             ( iJ2kInfo.iCSOffset != 0 ) )
            {
            // CodeStream box has been recognized
            iState = EStateInCodestreamBox;
            }
        else
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::IgnoreBoxL
// Ignore the content of the box and advance the iterator to the next box.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::IgnoreBoxL()
    {
    // Check whether this box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    if ( iBoxLength == 1 )
        {
        // The XLBox shall exist and contains the actual length of the box
        // XLBox is 8 bytes width
        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }

        iPtr = iBufferDes.Ptr();
        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
        }

    // Corrupted box length
    if ( ( (TInt32)iBoxLength ) < 0 )
        {
        User::Leave( KErrCorrupt );
        }

    // Check whether this box is the last box in the file
    if ( iBoxLength == 0 )
        {
        // This box is the last box in the file
        if ( iJ2kInfo.iCSOffset != 0 )
            {
            // CodeStream box has been recognized
            // Since we are not able to read til the
            // end of file/stream, we will skip this
            // box
            iState = EStateInCodestreamBox;
            return;
            }
        else
            {
            // The CodeStream box is missing from the file
            // and nothing can be decoded
            User::Leave( KErrCorrupt );
            }
        }

    // Read and ignore the validation of the box
    ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, iBoxLength );

    if ( (TUint32)iBufferDes.Length() < iBoxLength - KJ2kBoxTypeLength )
        {
        // Need more data from the ICL framework
        User::Leave( KErrUnderflow );
        }

    iPtr = iBufferDes.Ptr();

    iLastRead += iBoxLength;
    iState = EStateInUnknown;

    iPtr += iBoxLength - KJ2kBoxTypeLength;
    if ( (TUint32)iBufferDes.Length() < iBoxLength )
        {
        // Test the end of file 
        ReadDataL( iLastRead, iBufferDes, 1 );
        if ( ( (TUint32)iBufferDes.Length() == 0 ) &&
             ( iJ2kInfo.iCSOffset != 0 ) )
            {
            // CodeStream box has been recognized
            iState = EStateInCodestreamBox;
            }
        else
            {
            // Need more data from the ICL framework
            User::Leave( KErrUnderflow );
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::UpdateStateFromBoxTypeL
// Update the current state according to the box type
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::UpdateStateFromBoxTypeL()
    {
    switch ( iBoxType )
        {
        case KJ2kSigBoxType:
            {
            iState = EStateInSignatureBox;
            break;
            }
        case KJ2kJP2HeaderBoxType:
            {
            iState = EStateInJP2SuperBox;
            break;
            }
        case KJ2kCodestreamBoxType:
            {
            if ( iJ2kInfo.iOption & TJ2kInfo::EJP2file )
                {
                if ( !( iJ2kInfo.iOption & TJ2kInfo::EJP2Header ) )
                    {
                    // In JP2 file format, CodeStream box
                    // can not come before JP2 Header box
                    User::Leave( KErrCorrupt );
                    }

                // Save the CodeStream box's length
                iJ2kInfo.iCSBoxLength = iBoxLength;
                iJ2kInfo.iCSOffset = iLastRead;

                if ( iBoxLength == 0 )
                    {
                    // CodeStream box is the last box in the file
                    iState = EStateInCodestreamBox;
                    }
                else
                    {
                    // Continue checking any boxes after the CodeStream box
                    if ( iBoxLength == 1 )
                        {
                        // The XLBox shall exist and contains the actual length of the box
                        // XLBox is 8 bytes width
                        ReadDataL( iLastRead + KJ2kBoxTypeLength, iBufferDes, KJ2kBoxTypeLength );

                        if ( (TUint32)iBufferDes.Length() < KJ2kBoxTypeLength )
                            {
                            // Need more data from the ICL framework
                            User::Leave( KErrUnderflow );
                            }

                        iPtr = iBufferDes.Ptr();
                        iBoxLength = PtrReadUtil::ReadBigEndianUint32Inc( iPtr );
                        iBoxLength += PtrReadUtil::ReadBigEndianUint32( iPtr );
                        }

                    // Corrupted box length
                    if ( ( (TInt32)iBoxLength ) < 0 )
                        {
                        User::Leave( KErrCorrupt );
                        }
                    
                    iLastRead += iBoxLength;
                    // Test the end of file 
                    ReadDataL( iLastRead, iBufferDes, KJ2kBoxTypeLength );
                    if ( (TUint32)iBufferDes.Length() == 0 )
                        {
                        // CodeStream box is the last box in the file
                        iState = EStateInCodestreamBox;
                        }
                    else if ( (TUint32)iBufferDes.Length() == KJ2kBoxTypeLength )
                        {
                        iPtr = iBufferDes.Ptr();
                        }
                    else
                        {
                        // Need more data from the ICL framework
                        User::Leave( KErrUnderflow );
                        }
                    }
                }
            else
                {
                // CodeStream box must exist in file format only
                User::Leave( KErrCorrupt );
                }
            break;
            }
        case KJ2kIPRBoxType:
            {
            iState = EStateInIPRBox;
            break;
            }
        case KJ2kXMLBoxType:
            {
            iState = EStateInXMLBox;
            break;
            }
        case KJ2kUUIDBoxType:
            {
            iState = EStateInUUIDBox;
            break;
            }
        case KJ2kUUIDInfoBoxType:
            {
            iState = EStateInUUIDInfoBox;
            break;
            }
        default:
            {
            // Check if the first 4 bytes read
            // is SOC + SIZ
            if ( iBoxLength == KJ2kSOCType )
                {
                if ( iJ2kInfo.iOption & TJ2kInfo::EJP2file )
                    {
                    // JP2 codestream shouldn't in JP2 file format
                    User::Leave( KErrCorrupt );
                    }

                // SOC + SIZ marker - JP2 Codestream
                iState = EStateInCodestreamBox;
                iJ2kInfo.iCSOffset = iLastRead;
                }
            else
                {
                // Unrecognized box type
                // there are two ways of handling this
                // 1. stop parsing and Leave with an KErrCorrupt error
                //    User::Leave( KErrCorrupt );
                // 2. just ignore the box and continue on parsing the 
                //    next box.
                IgnoreBoxL();
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kDecoder::ScanDataL
// Called from ICL framework to instantiate decoder and parse header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kDecoder::ScanDataL()
    {
    ReadFormatL();

    ASSERT( ImageReadCodec() == NULL );

    CJp2kReadCodec* imageReadCodec = CJp2kReadCodec::NewL( iJ2kInfo );
    SetImageReadCodec( imageReadCodec );

    ReadFrameHeadersL();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Global panic function, From JP2KImageClientMain.h
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic( TIclPanic aError ) //lint !e759 already in module
    {
    User::Panic( KJ2KPanicCategory, aError );
    }

