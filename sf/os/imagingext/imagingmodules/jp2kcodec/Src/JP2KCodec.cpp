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
* Description:  CJp2kReadCodec class implements the ICL read codec interface.
*
*/


// INCLUDE FILES
#include <fbs.h>
#include <JP2KImageData.h>
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KStreamReader.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KCodec.h"
#include "JP2KEntropyDecoder.h"
#include "JP2KImageWriter.h"
#include "JP2KSynthesis.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJp2kReadCodec::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJp2kReadCodec* CJp2kReadCodec::NewL( const TJ2kInfo& aJ2kInfo )
    {
    CJp2kReadCodec* self = new ( ELeave ) CJp2kReadCodec( aJ2kInfo );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 

    return self;
    }

// Destructor
CJp2kReadCodec::~CJp2kReadCodec()
    {
    delete iImageInfo;
    iImageInfo = 0;

    delete iEntropy;
    iEntropy = 0;

    delete iImageWriter;
    iImageWriter = 0;

    delete iSynthesis;
    iSynthesis = 0;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::DecodeTileL
// Decoding the current tile-part
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::DecodeTileL()
    {
    CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

    // Decode and delete the tile
    DecodeAndDeleteTileL( tile );

    // reset the codec back to the parsing state
    iDecodeTile = EFalse;
    TFrameState retCode = EFrameIncomplete;

    // Start with new tile
    iUseNewTile = ETrue;
    if ( !iUseNextTile )
        {
        iFHState = EStateInUnknown;
        retCode = EFrameIncompleteRepositionRequest;
        }
    else
        {
        if ( !iSequential )
            {
            // Must be End of Codestream EOC
            iFHState = EStateInEOC;
            retCode = EFrameComplete;
            }
        else if ( iFHState == EStateInEOC )
            {
            retCode = EFrameComplete;
            }
        else
            {
            iFHState = EStateInUnknown;
            }
        }
    return retCode;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::IsDecodeTile
// Is codec in the decoding state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CJp2kReadCodec::IsDecodeTile() const
    {
    return iDecodeTile;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::InitFrameHeader
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::InitFrameHeader( TFrameInfo& aFrameInfo, CFrameImageData& aFrameData )
    {
    // Initialize internal data structure for holding frame header
    ASSERT( aFrameInfo.CurrentFrameState() == TFrameInfo::EFrameInfoUninitialised );

    iFrame = &aFrameInfo;
    iFrameData = &aFrameData;
    aFrameInfo.SetCurrentFrameState( TFrameInfo::EFrameInfoProcessingFrameHeader );
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ProcessFrameHeaderL
// Collect the JP2 codestream Main Header information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ProcessFrameHeaderL( TBufPtr8& aData )
    {
    // Read frame header and fill up internal data structure
    TFrameState retCode = EFrameIncomplete;

    iReader.iPtr = aData.Ptr();
    iReader.iPtrStart = CONST_CAST( TUint8*, iReader.iPtr );
    iReader.iPtrEnd   = iReader.iPtrStart + aData.Length();

    while ( iFHState != EStateInSOT )
        {
        iReader.iPtrStartMarker = CONST_CAST( TUint8*, iReader.iPtr );
        switch ( iFHState )
            {
            case EStateInSOC:
                {
                retCode = ReadSOCL();
                break;
                }
            case EStateInSIZ:
                {
                retCode = ReadSIZL();
                break;
                }
            case EStateInCOD:
                {
                retCode = ReadCODL();
                break;
                }
            case EStateInCOC:
                {
                retCode = ReadCOCL();
                break;
                }
            case EStateInQCD:
                {
                retCode = ReadQCDL();
                break;
                }
            case EStateInQCC:
                {
                retCode = ReadQCCL();
                break;
                }
            case EStateInRGN:
                {
                retCode = ReadRGNL();
                break;
                }
            case EStateInPOC:
                {
                retCode = ReadPOCL();
                break;
                }
            case EStateInPPM:
                {
                retCode = ReadPPML();
                break;
                }
            case EStateInTLM:
                {
                retCode = ReadTLML();
                break;
                }
            case EStateInPLM:
                {
                retCode = ReadPLML();
                break;
                }
            case EStateInCRG:
                {
                retCode = ReadCRGL();
                break;
                }
            case EStateInCOM:
                {
                retCode = ReadCOML();
                break;
                }
            case EStateInUnknown:
                {
                // Update the internal state based on the Marker
                retCode = UpdateStateFromMarkerL(); 
                break;
                }
            default:
                {
                // Unrecognized marker
                User::Leave( KErrCorrupt );
                break;
                }
            }
        // Check for underflow
        if ( retCode != EFrameComplete ) 
            {
            aData.Shift( iReader.iPtr - iReader.iPtrStart );
            return retCode;
            }

        // Update the internal counter for data processed
        iReader.UpdateMainHeader();
        }

    // Convert the MainHeader's COM marker to TJp2kComment
    // and let the framework managing the buffer.
    TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
    for ( TInt index = 0; index < mainMarker.iCom.Count(); ++index )
        {
        TJp2kComment* jp2kComment = new ( ELeave ) TJp2kComment;
        CleanupDeletePushL( jp2kComment );

        jp2kComment->iComment = mainMarker.iCom[index]->iCcom;

        User::LeaveIfError( iFrameData->AppendImageData( jp2kComment ) );
        CleanupStack::Pop(jp2kComment);

        // Transfer the ownership of the buffer from TMainMarker to framework
        User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kComment->iComment ) );
        mainMarker.iCom[index]->iCcom = 0;
        }

    // Convert some metadata from file format to TImageDataBlock derived objects
    ConvertImageDataL();

    // Try to sort PPM and PLM if there is one
    iImageInfo->DoCompactMainHeaderL();

    aData.Shift( iReader.iPtr - iReader.iPtrStart );

    // Fill up the image related information if it is not
    // JP2 file format
    if ( !( iJ2kInfo.iOption & TJ2kInfo::EJP2file ) )
        {
        const TSizMarker &sizMarker = iImageInfo->SizMarker();

        // To get the right output image size, we must compute the size tile by tile.
        // Compute the width of the output image
        TInt32 tileCompCanvasWidth = 0;
        TInt32 numHorTiles = iImageInfo->NumOfHorizTilesL(); 
        TInt32 tileStartCanvas;
        TInt32 tileEndCanvas;
        TInt32 tileCompStartCanvas;
        for(TUint16 indexX = 0; indexX < numHorTiles; ++indexX )  
            {
            tileStartCanvas = Max( ( sizMarker.iXTOsiz + indexX * sizMarker.iXTsiz ), sizMarker.iXOsiz );
            tileEndCanvas = Min( ( sizMarker.iXTOsiz + ( indexX + 1 ) * sizMarker.iXTsiz ), sizMarker.iXsiz );

            // Add this tile's contribution to the total size
            tileCompStartCanvas = TJ2kUtils::CeilL( tileStartCanvas, sizMarker.iXRsiz[0] );
            tileCompCanvasWidth += TJ2kUtils::CeilL( tileEndCanvas, sizMarker.iXRsiz[0] ) - tileCompStartCanvas;
            }
        
        // Compute the height of the output image
        TInt32 tileCompCanvasHeight = 0;
        TInt32 numVerTiles = iImageInfo->NumOfVertTilesL(); 
        for(TUint16 indexY = 0; indexY < numVerTiles; ++indexY )  
            {
            tileStartCanvas = Max( ( sizMarker.iYTOsiz + indexY * sizMarker.iYTsiz ), sizMarker.iYOsiz );
            tileEndCanvas = Min( ( sizMarker.iYTOsiz + ( indexY + 1 ) * sizMarker.iYTsiz ), sizMarker.iYsiz );

            // Add this tile's contribution to the total size
            tileCompStartCanvas = TJ2kUtils::CeilL( tileStartCanvas, sizMarker.iYRsiz[0] );
            tileCompCanvasHeight += TJ2kUtils::CeilL( tileEndCanvas, sizMarker.iYRsiz[0] ) - tileCompStartCanvas;
            }
        
        iFrame->iOverallSizeInPixels = TSize( tileCompCanvasWidth, tileCompCanvasHeight );

        iFrame->iFrameCoordsInPixels.SetRect( TPoint( 0, 0 ), iFrame->iOverallSizeInPixels );
        iFrame->iFrameSizeInTwips.SetSize( 0, 0 );

        for ( TUint16 index = 0; index < sizMarker.iCsiz; ++index )  
            {
            iFrame->iBitsPerPixel += ( ( sizMarker.iSsiz[index] & 0x7f ) + 1 );
            }

        // We can dither.
        iFrame->iFlags |= TFrameInfo::ECanDither;

        // Decoder is able to handle scaleable resolution
        iFrame->iFlags |= TFrameInfo::EConstantAspectRatio;

        if ( sizMarker.iCsiz > 1 )
            {
            iFrame->iFlags |= TFrameInfo::EColor;
            }

        // Animation is not allowed
        iFrame->iDelay = 0;

        switch ( iFrame->iBitsPerPixel )
            {
            case 1:
                {
                iFrame->iFrameDisplayMode = EGray2;
                break;
                }
            case 2:
                {
                iFrame->iFrameDisplayMode = EGray4;
                break;
                }
            case 4:
                {
                iFrame->iFrameDisplayMode = ( iFrame->iFlags & TFrameInfo::EColor ) ? EColor16 : EGray16;
                break;
                }
            case 8:
                {
                iFrame->iFrameDisplayMode = ( iFrame->iFlags & TFrameInfo::EColor ) ? EColor16M : EGray256;
                break;
                }
            case 12:
                {
                iFrame->iFrameDisplayMode = ( iFrame->iFlags & TFrameInfo::EColor ) ? EColor4K : EGray256;
                break;
                }
            case 16:
                {
                iFrame->iFrameDisplayMode = EColor64K;
                break;
                }
            case 24:
                {
                iFrame->iFrameDisplayMode = EColor16M;
                break;
                }
            default:
                {
                iFrame->iFrameDisplayMode = EColor64K;
                break;
                }
            }
        }

    // It is the offset that ICL framework used to read in
    // more data from the image file.
    iFrame->SetFrameDataOffset( iReader.iStartSOT );

    // Frame header has been processed without error
    // the flag is used by the ICL framework to determine
    // whether it should continue processing or halt with error
    iFrame->SetCurrentFrameState( TFrameInfo::EFrameInfoProcessingComplete );

    return retCode;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::InitFrameL
// Called from ICL framework to initialise frame.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::InitFrameL( TFrameInfo& /*aFrameInfo*/, CFrameImageData& /*aFrameData*/, 
                                 TBool aDisableErrorDiffusion, CFbsBitmap& aDestination, 
                                 CFbsBitmap* /*aDestinationMask*/ )
    {
    // Always use ERgb mode unless specified in the JP2 file format
    TDisplayMode mode = ERgb;  

    TInt reductionFactor = ImageProcessorUtility::ReductionFactor( iFrame->iOverallSizeInPixels,
                                                                   aDestination.SizeInPixels() );

    iImageInfo->SetLevelDrop( (TUint8)reductionFactor );

    // If the codestream doesn't contain enough wavelet levels to do the resolution 
    // reduction required by the framework / calling application, "extraLevels" are 
    // assigned to perform the resolution reduction. The "extra" resolution drop is
    // performed at the output stage, i.e. all the samples are decoded, but only 
    // 1/4, 1/16, etc. are written to output.
    if(reductionFactor > iImageInfo->MainMarker().iCod.iNumOfLevels)
        {
        iImageInfo->SetExtraLevelDrop( (TUint8)(reductionFactor - iImageInfo->MainMarker().iCod.iNumOfLevels) );
        }
    else
        {
        iImageInfo->SetExtraLevelDrop( 0 );
        }

    CImageProcessor* imageProc = ImageProcessorUtility::NewImageProcessorL( aDestination, 
                                                                            0,
                                                                            mode,
                                                                            aDisableErrorDiffusion );
    TPoint startAt( iFrame->iFrameCoordsInPixels.iTl.iX,
                    iFrame->iFrameCoordsInPixels.iTl.iY );

    TRect imageSize( startAt, aDestination.SizeInPixels() );

    // Prepare the bitmap using the image size
    SetImageProcessor( imageProc );
    imageProc->PrepareL( aDestination, imageSize );

    // Clear bitmap so sensibly draw partial decodes
    ClearBitmapL(aDestination, KRgbWhite);

    iUseNewTile = ETrue;
    iReader.iNewDataStart = iFrame->FrameDataOffset();
    iStyleUsed = EUnknownDecoder;

    iProgressBar = EFalse;
    if ( ( iImageInfo->NumOfHorizTilesL() == 1 ) &&
         ( iImageInfo->NumOfVertTilesL() == 1 ) )
        {
        // To force a return immediately from ProcessFrameL()
        // on first entry to stimulate the occurrance of
        // the progress bar
        iProgressBar = ETrue;
        }

    if ( iFHState != EStateInSOT )
        {
        // We are in zoom in mode
        iUseNextTile = iSequential = EFalse;
        iFHState = EStateInSOT;
        iLastTileIndex = 0;
        iImageInfo->SetLastTilePartProcessed( (TUint16)0xffff );
        iImageWriter->SetNewImageProcessor( imageProc );

        if ( iJ2kInfo.iOption & TJ2kInfo::EJP2file )
            {
            iReader.iCSLength = iJ2kInfo.iCSBoxLength;
            
            // We have read in all header information, so skip it
            if ( iReader.iCSLength > iReader.iStartSOT )
                {
                iReader.iCSLength -= iReader.iStartSOT;
                }
            iReader.iDataUsed = 0;
            }
        }
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ProcessFrameL
// Collect the JP2 codestream Tile Part information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ProcessFrameL(TBufPtr8& aSrc)
    {
    // Start decoding the compressed data stream
    TFrameState retCode = EFrameIncomplete;

    iReader.iPtr = aSrc.Ptr();

    iReader.iPtrStart = CONST_CAST( TUint8*, iReader.iPtr );
    iReader.iPtrEnd   = iReader.iPtrStart + aSrc.Length();

    while ( iFHState != EStateInEOC )
        {
        iReader.iPtrStartMarker = CONST_CAST( TUint8*, iReader.iPtr );
        switch ( iFHState )
            {
            case EStateInSOT:
                {
                retCode = ReadSOTL();
                break;
                }
            case EStateInSOD:
                {
                retCode = ReadSODL();
                break;
                }
            case EStateInBITSTREAM:
                {
                retCode = ReadBitStreamL();
                break;
                }
            case EStateInCOD:
                {
                retCode = ReadCODL( EFalse );
                break;
                }
            case EStateInCOC:
                {
                retCode = ReadCOCL( EFalse );
                break;
                }
            case EStateInQCD:
                {
                retCode = ReadQCDL( EFalse );
                break;
                }
            case EStateInQCC:
                {
                retCode = ReadQCCL( EFalse );
                break;
                }
            case EStateInRGN:
                {
                retCode = ReadRGNL( EFalse );
                break;
                }
            case EStateInPOC:
                {
                retCode = ReadPOCL( EFalse );
                break;
                }
            case EStateInPPT:
                {
                retCode = ReadPPTL();
                break;
                }
            case EStateInPLT:
                {
                retCode = ReadPLTL();
                break;
                }
            case EStateInCOM:
                {
                retCode = ReadCOML( EFalse );
                break;
                }
            case EStateInUnknown:
                {
                // Update the internal state based on the Marker
                retCode = UpdateStateFromMarkerL();
                break;
                }
            case EStateInSkipTile:
                {
                retCode = ReadSkipTileL();
                break;
                }
            default:
                {
                // Unrecognized marker
                User::Leave( KErrCorrupt );
                break;
                }
            }
        // Check for underflow
        if ( retCode != EFrameComplete ) 
            {
            // Request to jump to another section of the image file
            if ( retCode == EFrameIncompleteRepositionRequest )
                {
                // Update the internal counter for data processed
                iReader.UpdateTileHeader();
                }

            aSrc.Shift( iReader.iPtr - iReader.iPtrStart );
            return retCode;
            }

        if ( iFHState != EStateInSkipTile && iFHState != EStateInSOT )
            {
            // Update the internal counter for data processed
            // only for the current tile
            iReader.UpdateTileHeader();
            }

        if ( ( iProgressBar && iFHState != EStateInEOC ) || iDecodeTile )
            {
            iProgressBar = EFalse;
            aSrc.Shift( iReader.iPtr - iReader.iPtrStart );
            return EFrameIncomplete;
            }
        }

    if ( iDecodeTile == EFalse )
        {
        ImageProcessor()->FlushPixels();
        }

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::GetNewDataPosition
// Get the new offset where data should be read from.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::GetNewDataPosition( TInt& aPosition, TInt& /*aLength*/ )
    {
    // Set new offset position so that
    // ICL framework will read in more image data for processing
    aPosition = iReader.iNewDataStart + iJ2kInfo.iCSOffset;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::CJp2kReadCodec
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJp2kReadCodec::CJp2kReadCodec( const TJ2kInfo& aJ2kInfo ) :
    iJ2kInfo( aJ2kInfo )
    {
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::ConstructL()
    {
    iImageInfo = new ( ELeave ) CJ2kImageInfo;

    // Invoke base class 2nd phase constructor.
    CImageProcessorReadCodec::ConstructL();
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadSOCL
// Verify and process Start of Codestream (SOC marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadSOCL()
    {    
    const TUint8* dataPtr = iReader.iPtr;

    if ( iJ2kInfo.iOption & TJ2kInfo::EJP2file )
        {
        // Codestream box in JP2 file format
        iReader.iCSLength = iJ2kInfo.iCSBoxLength;
        iReader.iPtr += KJ2kBoxTypeLength;

        if ( iReader.iCSLength == 1 )
            {
            // The XLBox shall exist and contains the actual length of the box
            // XLBox is 8 bytes width
            iReader.iCSLength = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
            iReader.iCSLength += PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );

            // Populate back to the iJ2kInfo
            CONST_CAST( TJ2kInfo&, iJ2kInfo ).iCSBoxLength = iReader.iCSLength;
            }
        }

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerSize )
        {
        // Underflow, backup the pointer to the beginning
        iReader.iPtr = dataPtr;
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KSOC )
        {
        // Unrecognized SOC marker
        User::Leave( KErrCorrupt );
        }

    // SIZ marker shall immediately follow the SOC marker
    iFHState = EStateInSIZ;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadSIZL
// Verify and process Image and Tile Size (SIZ marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadSIZL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KSIZ )
        {
        // Unrecognized SIZ marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TSizMarker& sizMarker = CONST_CAST( TSizMarker&, iImageInfo->SizMarker() );

    sizMarker.iRsiz   = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    sizMarker.iXsiz   = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iYsiz   = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iXOsiz  = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iYOsiz  = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iXTsiz  = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iYTsiz  = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iXTOsiz = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iYTOsiz = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sizMarker.iCsiz   = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    
    if(sizMarker.iXOsiz > sizMarker.iXsiz || sizMarker.iYOsiz > sizMarker.iYsiz)
        {
        // Image offset is larger than image size, exit
        User::Leave( KErrCorrupt );
        }

    if(sizMarker.iXTOsiz > sizMarker.iXsiz || sizMarker.iYTOsiz > sizMarker.iYsiz)
        {
        // Tile offset is larger than image size, exit
        User::Leave( KErrCorrupt );
        }

    for ( TUint16 index = 0; index < sizMarker.iCsiz; ++index )
        {
        // Component's information
        User::LeaveIfError( sizMarker.iSsiz.Append( *iReader.iPtr++ ) );
        User::LeaveIfError( sizMarker.iXRsiz.Append( *iReader.iPtr++ ) );
        User::LeaveIfError( sizMarker.iYRsiz.Append( *iReader.iPtr++ ) );
        
        if((( sizMarker.iSsiz[index] & 0x7f ) + 1) > KMaxBitdepth )
            {
            // Invalid bitdepth for this component, exit
            User::Leave( KErrCorrupt );
            }
        }

    if ( iJ2kInfo.iCMPList.Count() > sizMarker.iCsiz )
        {
        // Populate the remaining component Rsiz using component 0's Rsiz
        TUint16 indexi = (TUint16)( iJ2kInfo.iCMPList.Count() - sizMarker.iCsiz );
        while ( indexi )
            {
            User::LeaveIfError( sizMarker.iXRsiz.Append( sizMarker.iXRsiz[0] ) );
            User::LeaveIfError( sizMarker.iYRsiz.Append( sizMarker.iYRsiz[0] ) );
            --indexi;
            }
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    // Any valid marker may come after SIZ marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadCODL
// Verify and process Coding Style Default (COD marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadCODL( TBool aMain )
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KCOD )
        {
        // Unrecognized COD marker
        User::Leave(KErrCorrupt);
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TCODMarker *codMarker = NULL;

    if ( aMain )
        {
        TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
        codMarker = &mainMarker.iCod;
        }
    else
        {
        // COD in Tile Part Header
        codMarker = new ( ELeave ) TCODMarker;        
        CleanupDeletePushL( codMarker );
        }

    codMarker->iScod = *iReader.iPtr++;
    codMarker->iProgressionOrder = *iReader.iPtr++;
    codMarker->iNumOfLayers = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    codMarker->iColorTransformation = *iReader.iPtr++;
    codMarker->iNumOfLevels = *iReader.iPtr++;

    TUint8 tmp = (TUint8)( ( *iReader.iPtr++ ) + 2 );
    codMarker->iCodeBlockSiz.iWidth  = 1 << tmp;

    tmp = (TUint8)( ( *iReader.iPtr++ ) + 2 );
    codMarker->iCodeBlockSiz.iHeight = 1 << tmp;

    codMarker->iCodeBlockStyle = *iReader.iPtr++;
    codMarker->iWaveletTransformation = *iReader.iPtr++;

    if ( codMarker->iScod & 0x01 )
        {
        // Entropy coder with precincts defined below
        codMarker->iPrecinctSiz = HBufC8::NewL( codMarker->iNumOfLevels + 1 );
        for ( TUint8 index = 0; index < codMarker->iNumOfLevels + 1; ++index )
            {
            codMarker->iPrecinctSiz->Des().Append( *iReader.iPtr++ );
            }
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( !aMain )
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

        // Append COD to the current tile and decrement the tile length
        tile.AppendCOD( codMarker, markerLength + KMarkerSize );
        CleanupStack::Pop(codMarker);
        }

    // Any valid marker may come after COD marker
    iFHState = EStateInUnknown;
    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadCOCL
// Verify and process Coding Style Component (COC marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadCOCL( TBool aMain )
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KCOC )
        {
        // Unrecognized COC marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    const TSizMarker& sizMarker = iImageInfo->SizMarker();

    TCOCMarker *cocMarker = new ( ELeave ) TCOCMarker;    
    CleanupDeletePushL(cocMarker);

    if ( sizMarker.iCsiz < 257 )
        {
        // 8 bits component
        cocMarker->iCcoc = *iReader.iPtr++;
        }
    else
        {
        // 16 bits component
        cocMarker->iCcoc = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        }

    cocMarker->iScoc = *iReader.iPtr++;
    cocMarker->iNumOfLevels = *iReader.iPtr++;

    TUint8 tmp = (TUint8)( ( *iReader.iPtr++ ) + 2 );
    cocMarker->iCodeBlockSiz.iWidth  = 1 << tmp;

    tmp = (TUint8)( ( *iReader.iPtr++ ) + 2 );
    cocMarker->iCodeBlockSiz.iHeight = 1 << tmp;

    cocMarker->iCodeBlockStyle = *iReader.iPtr++;
    cocMarker->iWaveletTransformation = *iReader.iPtr++;

    if ( cocMarker->iScoc & 0x01 )
        {
        // Entropy coder with precincts defined below
        cocMarker->iPrecinctSiz = HBufC8::NewL( cocMarker->iNumOfLevels + 1 );
        for ( TUint8 index = 0; index < cocMarker->iNumOfLevels + 1; ++index )
            {
            cocMarker->iPrecinctSiz->Des().Append( *iReader.iPtr++ );
            }
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( aMain )
        {
        iImageInfo->AppendCOCL( cocMarker );
        }
    else
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

        // Append COC to the current tile and decrement the tile length
        tile.AppendCOCL( cocMarker, markerLength + KMarkerSize );
        }
    CleanupStack::Pop(cocMarker);

    // Any valid marker may come after COC marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadQCDL
// Verify and process Quantization Default (QCD marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadQCDL( TBool aMain )
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {   
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KQCD )
        {
        // Unrecognized QCD marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TQCDMarker *qcdMarker = NULL;

    if ( aMain )
        {
        TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
        qcdMarker = &mainMarker.iQcd;
        }
    else
        {
        // QCD in Tile Part Header
        qcdMarker = new ( ELeave ) TQCDMarker;
        
        CleanupDeletePushL( qcdMarker );
        }

    qcdMarker->iSqcd = *iReader.iPtr++;

    if ( qcdMarker->iSqcd & 0x01 )
        {
        // Scalar derived (values signalled for NLL subband only)
        TUint16 tmp = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        qcdMarker->iExponent = HBufC8::NewL(1);
        qcdMarker->iExponent->Des().Append( (TUint8)( (tmp >> 11) & 0x1f ) );
        qcdMarker->iMantissa = HBufC16::NewL(1);
        qcdMarker->iMantissa->Des().Append( (TUint16)( tmp & 0x07ff ) );
        }
    else
        {
        TInt entries = markerLength - ( iReader.iPtr - iReader.iPtrStartMarker ) + KMarkerSize;
        if ( qcdMarker->iSqcd & 0x1f )
            {
            // Word oriented
            TUint16 tmp;
            qcdMarker->iExponent = HBufC8::NewL( entries / 2 );
            qcdMarker->iMantissa = HBufC16::NewL( entries / 2 );

            while ( entries >= 2 )
                {
                tmp = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
                qcdMarker->iExponent->Des().Append((TUint8)( ( tmp >> 11) & 0x1f ) );
                qcdMarker->iMantissa->Des().Append((TUint16)( tmp & 0x07ff ) );
                entries -= 2;
            }
        }
        else
            {
            // No quantization
            qcdMarker->iExponent = HBufC8::NewL( entries );
            while ( entries )
                {
                qcdMarker->iExponent->Des().Append( (TUint8)( ( *iReader.iPtr++ >> 3 ) & 0x1f ) );
                --entries;
                }
            }
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( !aMain )
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

        // Append QCD to the current tile and decrement the tile length
        tile.AppendQCD( qcdMarker, markerLength + KMarkerSize );
        CleanupStack::Pop(qcdMarker);
        }

    // Any valid marker may come after QCD marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadQCCL
// Verify and process Quantization Component (QCC marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadQCCL(TBool aMain)
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KQCC )
        {
        // Unrecognized QCC marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < (markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    const TSizMarker& sizMarker = iImageInfo->SizMarker();

    TQCCMarker *qccMarker = new (ELeave) TQCCMarker;

    CleanupDeletePushL( qccMarker );

    if ( sizMarker.iCsiz < 257 )
        {
        // 8 bits component
        qccMarker->iCqcc = *iReader.iPtr++;
        }
    else
        {
        // 16 bit component
        qccMarker->iCqcc = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        }

    qccMarker->iSqcc = *iReader.iPtr++;

    if ( qccMarker->iSqcc & 0x01 )
        {
        // Scalar derived (values signalled for NLL subband only)
        TUint16 tmp = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        qccMarker->iExponent = HBufC8::NewL(1);
        qccMarker->iExponent->Des().Append( (TUint8)( ( tmp >> 11 ) & 0x1f ) );
        qccMarker->iMantissa = HBufC16::NewL(1);
        qccMarker->iMantissa->Des().Append( (TUint16)( tmp & 0x07ff ) );
        }
    else
        {
        TInt entries = markerLength - ( iReader.iPtr - iReader.iPtrStartMarker ) + KMarkerSize;
        if ( qccMarker->iSqcc & 0x1f )
            {
            // Word oriented
            TUint16 tmp;
            qccMarker->iExponent = HBufC8::NewL( entries / 2 );
            qccMarker->iMantissa = HBufC16::NewL( entries / 2 );

            while ( entries >= 2 )
                {
                tmp = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
                qccMarker->iExponent->Des().Append( (TUint8)( ( tmp >> 11 ) & 0x1f ) );
                qccMarker->iMantissa->Des().Append( (TUint16)( tmp & 0x07ff ) );
                entries -= 2;
                }
            }
        else
            {
            // No quantization
            qccMarker->iExponent = HBufC8::NewL( entries );
            while ( entries )
                {
                qccMarker->iExponent->Des().Append( (TUint8)( ( *iReader.iPtr++ >> 3 ) & 0x1f ) );
                --entries;
                }
            }
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( aMain )
        {
        iImageInfo->AppendQCCL( qccMarker );
        }
    else
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

        // Append QCC to the current tile and decrement the tile length
        tile.AppendQCCL( qccMarker, markerLength + KMarkerSize );
        }
    CleanupStack::Pop(qccMarker);

    // Any valid marker may come after QCC marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadRGNL
// Verify and process Region of Interest (RGN marker).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadRGNL( TBool aMain )
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KRGN )
        {
        // Unrecognized RGN marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    const TSizMarker& sizMarker = iImageInfo->SizMarker();

    TRGNMarker *rgnMarker = new ( ELeave ) TRGNMarker;
    CleanupDeletePushL( rgnMarker );

    if ( sizMarker.iCsiz < 257 )
        {
        // 8 bits component
        rgnMarker->iCrgn = *iReader.iPtr++;
        }
    else
        {
        // 16 bits component
        rgnMarker->iCrgn = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        }

    rgnMarker->iSrgn  = *iReader.iPtr++;
    rgnMarker->iSPrgn = *iReader.iPtr++;

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( aMain )
        {
        iImageInfo->AppendRGNL( rgnMarker );
        }
    else
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

        // Append RGN to the current tile and decrement the tile length
        tile.AppendRGNL( rgnMarker, markerLength + KMarkerSize );
        }
    CleanupStack::Pop(rgnMarker);

    // Any valid marker may come after RGN marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadPOCL
// Verify and process Progression Order Change ( POC marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadPOCL( TBool aMain )
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KPOC )
        {
        // Unrecognized COC marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    const TSizMarker& sizMarker = iImageInfo->SizMarker();

    TPOCMarker *pocMarker = new ( ELeave ) TPOCMarker;
    CleanupDeletePushL( pocMarker );

    TInt entries = markerLength - KMarkerSize;
    TInt sizEntry = ( sizMarker.iCsiz < 257 ) ? 7 : 9;

    while ( entries >= sizEntry )
        {
        User::LeaveIfError( pocMarker->iRSpoc.Append( *iReader.iPtr++ ) );

        if ( sizMarker.iCsiz < 257 )
            {
            // 8 bits component
            User::LeaveIfError( pocMarker->iCSpoc.Append( *iReader.iPtr++ ) );
            }
        else
            {
            // 16 bits component
            User::LeaveIfError( pocMarker->iCSpoc.Append( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) ) );
            }
        User::LeaveIfError( pocMarker->iLYEpoc.Append( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) ) );
        User::LeaveIfError( pocMarker->iREpoc.Append( *iReader.iPtr++ ) );
        if ( sizMarker.iCsiz < 257 )
            {
            // 8 bits component
            User::LeaveIfError( pocMarker->iCEpoc.Append( *iReader.iPtr++ ) );
            }
        else
            {
            // 16 bits component
            User::LeaveIfError( pocMarker->iCEpoc.Append( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) ) );
            }
        User::LeaveIfError( pocMarker->iPpoc.Append( *iReader.iPtr++ ) );

        entries -= sizEntry;
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( aMain )
        {
        iImageInfo->AppendPOCL( pocMarker );
        }
    else
        {
        CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );
        
        // Append POC to the current tile and decrement the tile length
        tile.AppendPOCL( pocMarker, markerLength + KMarkerSize );
        }
    CleanupStack::Pop(pocMarker);

    // Any valid marker may come after POC marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadPPML
// Verify and process Packed Packet Headers, Main Header ( PPM marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadPPML()
    {
    TUint8 isUnderflow = EFalse;
    if ( !iPreviousPPM )
        {
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
            {
            // Underflow
            return EFrameIncomplete;
            }

        if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KPPM )
            {
            // Unrecognized PPM marker
            User::Leave( KErrCorrupt );
            }

        TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( KMarkerLength + 1 ) )
            {
            // Underflow, we need Zppm and Nppm to be in the buffer
            // backup the iterator to the beginning of the marker
            iReader.iPtr -= KMarkerMinLength;
            return EFrameIncomplete;
            }

        if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
            {
            // Underflow, will keep reading
            isUnderflow = ETrue;
            }

        TPPMMarker *ppmMarker = new ( ELeave ) TPPMMarker;        
        CleanupDeletePushL( ppmMarker );

        ppmMarker->iZppm = *iReader.iPtr++;
        TUint32 entries = (TUint32)( markerLength - KMarkerSize - 1 );

        ppmMarker->iNppm = entries;
        ppmMarker->iIppm = HBufC8::NewL( entries );

        if ( !isUnderflow )
            {
            ppmMarker->iIppm->Des(  ).Append( iReader.iPtr, entries );
            iReader.iPtr += entries;
            iPreviousPPM = 0;

            // Make sure we read all the data
            if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
                {
                // We must be missing some data in the marker
                User::Leave( KErrCorrupt );
                }
            }
        else
            {
            ppmMarker->iIppm->Des().Append( iReader.iPtr, ( iReader.iPtrEnd - iReader.iPtr ) );
            ppmMarker->iRemainder = (TUint32)( entries - ( iReader.iPtrEnd - iReader.iPtr ) );
            iReader.iPtr = iReader.iPtrEnd;
            iPreviousPPM = ppmMarker;
            }

        // Insert the new PPM marker into the right order
        TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
        TUint8 tmp = ETrue;
        if ( mainMarker.iPpm.Count() > 0 &&
             mainMarker.iPpm[mainMarker.iPpm.Count() - 1]->iZppm > ppmMarker->iZppm )
            {
            for ( entries = 0; entries < (TUint32)mainMarker.iPpm.Count(); ++entries )
                {
                // Order by iZppm of the PPM marker
                if ( mainMarker.iPpm[entries]->iZppm > ppmMarker->iZppm )
                    {
                    User::LeaveIfError( mainMarker.iPpm.Insert( ppmMarker, entries ) );

                    tmp = EFalse;
                    entries = (TUint32)mainMarker.iPpm.Count();  
                    }
                }
            }

        if ( tmp )
            {
            User::LeaveIfError( mainMarker.iPpm.Append( ppmMarker ) );
            }
        CleanupStack::Pop(ppmMarker);
        }
    else
        {
        if ( (TUint32)( iReader.iPtrEnd - iReader.iPtr ) < iPreviousPPM->iRemainder )
            {
            // Continue reading incomplete COM marker
            iPreviousPPM->iIppm->Des().Append( iReader.iPtr, ( iReader.iPtrEnd - iReader.iPtr ) );
            iPreviousPPM->iRemainder = (TUint32)( iPreviousPPM->iRemainder - ( iReader.iPtrEnd - iReader.iPtr ) );
            iReader.iPtr = iReader.iPtrEnd;
            isUnderflow = ETrue;
            }
        else
            {
            // We have the complete COM marker now
            iPreviousPPM->iIppm->Des().Append( iReader.iPtr, iPreviousPPM->iRemainder );
            iReader.iPtr += iPreviousPPM->iRemainder;
            iPreviousPPM->iRemainder = 0;
            iPreviousPPM = 0;
            }
        }

    if ( !isUnderflow )
        {
        // Any valid marker may come after PPM marker
        iFHState = EStateInUnknown;
        return EFrameComplete;
        }
    else
        {
        // Underflow, stay in the same state
        iReader.UpdateMainHeader();
        return EFrameIncomplete;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadTLML
// Verify and process Tile Part Lengths, Main Header ( TLM marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadTLML()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KTLM )
        {
        // Unrecognized TLM marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TTLMMarker *tlmMarker = new ( ELeave ) TTLMMarker;    
    CleanupDeletePushL( tlmMarker );

    tlmMarker->iZtlm = *iReader.iPtr++;
    tlmMarker->iStlm = *iReader.iPtr++;

    TInt entries = markerLength - ( 2 * KMarkerSize );
    TUint8 st = (TUint8)( ( tlmMarker->iStlm >> 4 ) & 0x03 );
    TUint8 sp = (TUint8)( ( tlmMarker->iStlm >> 6 ) & 0x01 );
    TInt sizEntry = st;
    sizEntry += ( sp ? 4 : 2 );

    while ( entries >= sizEntry )
        {
        if ( st == 1 )
            {
            // 8 bits tile index
            User::LeaveIfError( tlmMarker->iTtlm.Append( *iReader.iPtr++ ) );
            }
        else if ( st == 2 )
            {
            // 16 bits tile index
            User::LeaveIfError( tlmMarker->iTtlm.Append( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) ) );
            }
        if ( sp )   //lint !e961    no else is needed here at the end of if...else if
            {
            // 32 bits length
            User::LeaveIfError( tlmMarker->iPtlm.Append( PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr ) ) );
            }
        else
            {
            // 16 bits length
            User::LeaveIfError( tlmMarker->iPtlm.Append( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) ) );
            }
        entries -= sizEntry;
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    // Insert the new TLM marker into the right order
    TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
    sp = 1;
    for ( entries = 0; entries < mainMarker.iTlm.Count(); ++entries )
        {
        // Order by iZtlm of the TLM marker
        if ( mainMarker.iTlm[entries]->iZtlm > tlmMarker->iZtlm )
            {
            User::LeaveIfError( mainMarker.iTlm.Insert( tlmMarker, entries ) );
            sp = 0;
            entries = mainMarker.iTlm.Count();
            }
        }

    if ( sp )
        {
        User::LeaveIfError( mainMarker.iTlm.Append( tlmMarker ) );
        }
    CleanupStack::Pop(tlmMarker);

    // Any valid marker may come after TLM marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadPLML
// Verify and process Packet Length, Main Header ( PLM marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadPLML()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KPLM )
        {
        // Unrecognized PLM marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TPLMMarker *plmMarker =  new ( ELeave ) TPLMMarker;    
    CleanupDeletePushL( plmMarker );

    plmMarker->iZplm = *iReader.iPtr++;
    TUint32 entries = (TUint32)( markerLength - KMarkerSize - 1 );

    plmMarker->iNplm = (TUint8)entries;
    plmMarker->iIplm = HBufC8::NewL( entries );
    plmMarker->iIplm->Des().Append( iReader.iPtr, entries );
    iReader.iPtr += entries;

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    // Insert the new PLM marker into the right order
    TMainMarker& mainMarker = CONST_CAST( TMainMarker&, iImageInfo->MainMarker() );
    TUint8 tmp = ETrue;

    if ( mainMarker.iPlm.Count() > 0 &&
         mainMarker.iPlm[mainMarker.iPlm.Count() - 1]->iZplm > plmMarker->iZplm )
        {
        for ( entries = 0; entries < (TUint32)mainMarker.iPlm.Count(); ++entries )
            {
            // Order by iZplm of the PLM marker
            if ( mainMarker.iPlm[entries]->iZplm > plmMarker->iZplm )
                {
                User::LeaveIfError( mainMarker.iPlm.Insert( plmMarker, entries ) );
                tmp = EFalse;
                entries = (TUint32)mainMarker.iPlm.Count();     
            }
        }
    }

    if ( tmp )
        {
        User::LeaveIfError( mainMarker.iPlm.Append( plmMarker ) );
        }
    CleanupStack::Pop(plmMarker);

    // Any valid marker may come after PLM marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadCRGL
// Verify and process Component Registration, Main Header ( CRG marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadCRGL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KCRG )
        {
        // Unrecognized CRG marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TCRGMarker *crgMarker = new ( ELeave ) TCRGMarker;    
    CleanupDeletePushL( crgMarker );

    TInt entries = markerLength - KMarkerSize;
    TUint16 xCrg;
    TUint16 yCrg;
    while ( entries )
        {
        xCrg = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        yCrg = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        User::LeaveIfError( crgMarker->iXYcrg.Append( TPoint( xCrg, yCrg ) ) );
        entries -= 4;
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    iImageInfo->AppendCRGL( crgMarker );

    CleanupStack::Pop(crgMarker);

    // Any valid marker may come after CRG marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadCOML
// Verify and process Comment ( COM marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadCOML( TBool aMain )
    {
    TUint8 isUnderflow = EFalse;
    if ( !iPreviousCOM )
        {
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
            {
            // Underflow
            return EFrameIncomplete;
            }

        if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KCME )
            {
            // Unrecognized COM marker
            User::Leave( KErrCorrupt );
            }

        TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerLength )
            {
            // Underflow, we need Rcom to be in the buffer
            // backup the iterator to the beginning of the marker
            iReader.iPtr -= KMarkerMinLength;
            return EFrameIncomplete;
            }

        if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
            {
            // Underflow, will keep reading
            isUnderflow = ETrue;
            }

        TCOMMarker* comMarker = new ( ELeave ) TCOMMarker;        
        CleanupDeletePushL( comMarker );

        comMarker->iRcom = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
        TInt entries = markerLength - ( 2 * KMarkerSize );

        comMarker->iCcom = HBufC8::NewL( entries );
        if ( !isUnderflow )
            {
            comMarker->iCcom->Des().Append( iReader.iPtr, entries );
            iReader.iPtr += entries;
            iPreviousCOM = 0;

            // Make sure we read all the data
            if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
                {
                // We must be missing some data in the marker
                User::Leave( KErrCorrupt );
                }
            }
        else
            {
            comMarker->iCcom->Des().Append( iReader.iPtr, ( iReader.iPtrEnd - iReader.iPtr ) );
            comMarker->iRemainder = (TUint16)( entries - ( iReader.iPtrEnd - iReader.iPtr ) );
            iReader.iPtr = iReader.iPtrEnd;
            iPreviousCOM = comMarker;
            }

        if ( aMain )
            {
            iImageInfo->AppendCOML( comMarker );
            }
        else
            {
            CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );
            
            // Append COM to the current tile and decrement the tile length
            tile.AppendCOML( comMarker, markerLength + KMarkerSize );
            }
        CleanupStack::Pop(comMarker);
        }
    else
        {
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < iPreviousCOM->iRemainder )
            {
            // Continue reading incomplete COM marker
            iPreviousCOM->iCcom->Des().Append( iReader.iPtr, ( iReader.iPtrEnd - iReader.iPtr ) );
            iPreviousCOM->iRemainder = (TUint16)( iPreviousCOM->iRemainder - ( iReader.iPtrEnd - iReader.iPtr ) );
            iReader.iPtr = iReader.iPtrEnd;
            isUnderflow = ETrue;
        }
        else
            {
            // We have the complete COM marker now
            iPreviousCOM->iCcom->Des(  ).Append( iReader.iPtr, iPreviousCOM->iRemainder );
            iReader.iPtr += iPreviousCOM->iRemainder;
            iPreviousCOM->iRemainder = 0;
            iPreviousCOM = 0;
            }
        }

    if ( !isUnderflow )
        {
        // Any valid marker may come after COM marker
        iFHState = EStateInUnknown;
        return EFrameComplete;
        }
    else
        {
        // Underflow, stay in the same state
        if ( aMain )
            {
            iReader.UpdateMainHeader();
            }
        return EFrameIncomplete;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadSOTL
// Verify and process Start of Tile Part ( SOT marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadSOTL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KSOT )
        {
        // Unrecognized SOT marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TSotMarker sotMarker;
    sotMarker.iIsot = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );
    sotMarker.iPsot = PtrReadUtil::ReadBigEndianUint32Inc( iReader.iPtr );
    sotMarker.iTPsot = *iReader.iPtr++;
    sotMarker.iTNsot = *iReader.iPtr++;
    
    if(sotMarker.iIsot >= ( iImageInfo->NumOfHorizTilesL() * iImageInfo->NumOfVertTilesL() ))
        {
        // Invalid tile index, exceeds the number of tiles, exit
        User::Leave( KErrCorrupt );
        }

    if ( sotMarker.iPsot == 0 )
        {
        // Try to look for tile part length information
        // from the TLM in the main header - if there is one
        iImageInfo->GetFromTLM( sotMarker );
        }

    iImageInfo->IncrementLastTilePartProcessed();

    if ( iUseNewTile )
        {
        if ( sotMarker.iIsot < iLastTileIndex )
            {
            // Skip all tiles with iIsot smaller than the iLastTileIndex
            iReader.iSkipLength = sotMarker.iPsot - markerLength - KMarkerSize;
            iUseNewTile = EFalse;
            iFHState = EStateInSkipTile;
            return EFrameComplete;
            }

        if(sotMarker.iTPsot >= sotMarker.iTNsot)
            {
            // Skip the tiles where tile part index is larger than 
            // the number of tile parts
            iReader.iSkipLength = sotMarker.iPsot - markerLength - KMarkerSize;
            iUseNewTile = EFalse;
            iFHState = EStateInSkipTile;
            return EFrameComplete;
            }
	
        // Start using this tile as the current tile
        iLastTileIndex = sotMarker.iIsot;

        // Save the next tile offset
        iReader.iNewDataStart += sotMarker.iPsot;
        iUseNextTile = iSequential = ETrue;
        }
    else
        {
        if ( sotMarker.iIsot != iLastTileIndex )
            {
            if ( iUseNextTile )
                {
                if ( sotMarker.iIsot > iLastTileIndex )
                    {
                    iUseNextTile = EFalse;
                    }
                }
            iSequential = EFalse;

            // Skip all tiles that are not equal to current tile
            iReader.iSkipLength = sotMarker.iPsot - markerLength - KMarkerSize;
            iFHState = EStateInSkipTile;
            return EFrameComplete;
            }
        else
            {
            // Tiles are in sequential order
            if ( iUseNextTile && iSequential )
                {
                // Save the next tile offset
                iReader.iNewDataStart += sotMarker.iPsot;
            }
        }
    }

    CJ2kTileInfo *tile = 0;
    if ( iUseNewTile )
        {
        tile = CJ2kTileInfo::NewLC( *iImageInfo, iReader );
        }
    else
        {
        tile = CONST_CAST( CJ2kTileInfo*, &iImageInfo->TileAt( iLastTileIndex ) );
        }

    // Set the SOT marker
    tile->SetSotMarker( sotMarker );

    if ( sotMarker.iPsot )
        {
        // Decrement the tile length
        tile->SetTileLength( sotMarker.iPsot - markerLength - KMarkerSize );
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    if ( iUseNewTile )
        { 
        iImageInfo->Append( tile );
        CleanupStack::Pop(tile);
        iUseNewTile = EFalse;
        }

    // Any valid marker may come after SOT marker
    iFHState = EStateInUnknown;
    return EFrameComplete;

    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadSODL
// Verify and process Start of Data ( SOD marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadSODL()
    {
    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KSOD )
        {
        // Unrecognized SOD marker
        User::Leave( KErrCorrupt );
        }

    CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

    TUint32& length = tile.TileLength();
    if ( length )
        {
        // Decrement the tile length
        length -= KMarkerSize;
        }

    // Initialize the tile only for tile part 0
    if ( tile.IsFirstTilePart() )
        {
        tile.InitializeL();
        iImageInfo->SetLastTilePartProcessed( tile.SotMarker().iIsot );
        }

    if ( tile.IsPPT() )
        {
        // Use packet header information from PPT
        tile.UsePPTL();
        }
    else if ( iImageInfo->IsPPM() )
        {
        // Use packet header information from PPM
        iImageInfo->UsePPM( tile );
        }
    else
        {
        tile.SetPacketHeaderReader( &iReader );
        }

    if ( ( iImageInfo->NumOfHorizTilesL() == 1 ) &&
         ( iImageInfo->NumOfVertTilesL() == 1 ) )
        {
        // To force a return immediately from ProcessFrameL()
        // on first entry to stimulate the occurrance of
        // the progress bar
        iProgressBar = ETrue;
        }

    // We know that bitstream will follow SOD marker
    iFHState = EStateInBITSTREAM;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadBitStreamL
// Verify and process BitStream Data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadBitStreamL()
    {
    CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );
    TUint32& length = tile.TileLength(  );

    if ( length )
        {
        if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( TInt32 )length )
            {
            // Larger bitstream will not fit into the internal
            // buffer size, so process a chunk at a time
            // we have to add some kind of state information
            // so that we know where to continue and process
            if ( tile.IsSpeedup() )
                {
                // For RLCP/RPCL progression order
                if ( tile.LastLevelProcessed() <= tile.NumOfLevelsPOC() )
                    {
                    tile.ReadBitStreamL();
                    }
                else
                    {
                    // discard any data that beyond the required resolution level
                    if ( !tile.IsPPT() )
                        {
                        if ( iImageInfo->IsPPM() )
                            {
                            // discard current packet header in PPM marker
                            iImageInfo->ResetPPM();
                            }
                        }
                    iReader.iPtr = iReader.iPtrEnd;
                    }
                }
            else
                {
                tile.ReadBitStreamL();
                }

            length -= ( iReader.iPtr - iReader.iPtrStartMarker );
            iReader.UpdateTileHeader();

            // stay in the current state
            return EFrameIncomplete;
            }

        // We are sure that all tile part data fit into the buffer
        // so just go ahead and process it
        if ( tile.IsSpeedup() )
            {
            // For RLCP/RPCL progression order
            if ( tile.LastLevelProcessed() <= tile.NumOfLevelsPOC() )
                {
                tile.ReadBitStreamL( ETrue );
                iReader.TryReAlignReader();
                }
            else
                {
                // discard any data that beyond the required resolution level
                if ( !tile.IsPPT() )
                    {
                    if ( iImageInfo->IsPPM() )
                        {
                        // discard current packet header in PPM marker
                        iImageInfo->ResetPPM();
                        }
                    }
                iReader.iPtr += length;
                }
            }
        else
            {
            tile.ReadBitStreamL( ETrue );
            iReader.TryReAlignReader();
            }

        length -= ( iReader.iPtr - iReader.iPtrStartMarker );

        if ( tile.IsSpeedup() )
            {
            // discard any data that beyond the required resolution level
            if ( !tile.IsPPT() )
                {
                if ( iImageInfo->IsPPM() )
                    {
                    // discard current packet header in PPM marker
                    iImageInfo->ResetPPM();
                    }
                }
            iReader.iPtr += length;
            length = 0;
            }

        // Sanity check
        if ( length != 0 )
            {
            User::Leave( KErrCorrupt );
            }

        if ( tile.IsLastTilePart() )
            {
            tile.DoReleaseUnusedMarkers();

            // proceed to the decoding state
            iDecodeTile = ETrue;

            // Start with new tile
            iUseNewTile = ETrue;
            if ( !iUseNextTile )
                {
                iFHState = EStateInUnknown;
                return EFrameIncompleteRepositionRequest;
                }
            else
                {
                if ( !iSequential )
                    {
                    // Must be End of Codestream EOC
                    iFHState = EStateInEOC;
                    iReader.iPtr += KMarkerSize;
                    return EFrameComplete;
                    }
                }
            }

        // We do not know what is the next marker
        // either next SOT or EOC
        iFHState = EStateInUnknown;
        }
    else
        {
        if ( iReader.iPtrEnd == iReader.iPtr )
            {
            // Assume that we have done with the image
            iFHState = EStateInEOC;
            }
        else
            {
            if ( ( iReader.iPtrEnd - iReader.iPtr ) >= 2 )
                {
                TUint16 marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                if ( marker == KSOT )
                    {
                    // Next SOT marker is found
                    iFHState = EStateInSOT;
                    }
                else if ( marker == EStateInEOC )
                    {
                    // End of Codestream is found
                    iFHState = EStateInEOC;
                    iReader.iPtr += KMarkerSize;
                    }
                else
                    {
                    tile.ReadBitStreamL();
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );

                    if ( marker == KSOT || marker == KEOC )
                        {
                        iFHState = EStateInUnknown;

                        if ( tile.IsLastTilePart() )
                            {
                            tile.DoReleaseUnusedMarkers();

                            // proceed to the decoding state
                            iDecodeTile = ETrue;

                            // Start with new tile
                            iUseNewTile = ETrue;
                            if ( !iUseNextTile )
                                {
                                return EFrameIncompleteRepositionRequest;
                                }
                            else
                                {
                                if ( !iSequential )
                                    {
                                    // Must be End of Codestream EOC
                                    iFHState = EStateInEOC;
                                    iReader.iPtr += KMarkerSize;
                                    }
                                }
                            }
                        }
                    else
                        {
                        iReader.UpdateTileHeader();

                        // Stay in the current state
                        return EFrameIncomplete;
                        }
                    }
                }
            else
                {
                iReader.UpdateTileHeader();                

                // Stay in the current state
                return EFrameIncomplete;
                }
            }
        }

    if ( ( iImageInfo->NumOfHorizTilesL() == 1 ) &&
         ( iImageInfo->NumOfVertTilesL() == 1 ) )
        {
        // To force a return immediately from ProcessFrameL()
        // on first entry to stimulate the occurrance of
        // the progress bar
        iProgressBar = ETrue;
        }

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadPPTL
// Verify and process Packed Packet Headers, Tile Part Header ( PPT marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadPPTL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KPPT )
        {
        // Unrecognized PPT marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TPPTMarker *pptMarker = new ( ELeave ) TPPTMarker;    
    CleanupDeletePushL(pptMarker);

    TInt entries = markerLength - KMarkerSize - 1;
    pptMarker->iZppt = *iReader.iPtr++;
    pptMarker->iIppt = HBufC8::NewL( entries );
    pptMarker->iIppt->Des(  ).Append( iReader.iPtr, entries );
    iReader.iPtr += entries;

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }

    CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );
    
    // Append PPT to the current tile and decrement the tile length
    tile.AppendPPTL( pptMarker, markerLength + KMarkerSize );
    CleanupStack::Pop(pptMarker);

    // Any valid marker may come after PPT marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadPLTL
// Verify and process Packet Length, Tile Part Header ( PLT marker ).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadPLTL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerMinLength )
        {
        // Underflow
        return EFrameIncomplete;
        }

    if ( PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr ) != KPLT )
        {
        // Unrecognized PLT marker
        User::Leave( KErrCorrupt );
        }

    TUint16 markerLength = PtrReadUtil::ReadBigEndianUint16Inc( iReader.iPtr );

    if ( ( iReader.iPtrEnd - iReader.iPtr ) < ( markerLength - KMarkerLength ) )
        {
        // Underflow, backup the iterator to the beginning of the marker
        iReader.iPtr -= KMarkerMinLength;
        return EFrameIncomplete;
        }

    TPLTMarker *pltMarker = new ( ELeave ) TPLTMarker;
    CleanupDeletePushL( pltMarker );

    pltMarker->iZplt = *iReader.iPtr++;

    TInt entries = markerLength - KMarkerSize - 1;
    while ( entries )
        {
        User::LeaveIfError( pltMarker->iIplt.Append( *iReader.iPtr++ ) );
        --entries;
        }

    // Make sure we read all the data
    if ( ( iReader.iPtr - iReader.iPtrStartMarker ) != ( markerLength + KMarkerSize ) )
        {
        // We must be missing some data in the marker
        User::Leave( KErrCorrupt );
        }   

    CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

    // Append PLT to the current tile and decrement the tile length
    tile.AppendPLTL( pltMarker, markerLength + KMarkerSize );
    CleanupStack::Pop(pltMarker);

    // Any valid marker may come after PLT marker
    iFHState = EStateInUnknown;

    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ReadSkipTileL
// Ignore the content and advance the iterator to the next marker.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::ReadSkipTileL()
    {
    if ( (TUint32)( iReader.iPtrEnd - iReader.iPtr ) < iReader.iSkipLength )
        {
        iReader.iSkipLength -= ( iReader.iPtrEnd - iReader.iPtr );
        iReader.iPtr = iReader.iPtrEnd;
        
        // Stay in the current state
        return EFrameIncomplete;
        }
    else
        {
        if ( iReader.iSkipLength )
            {
            iReader.iPtr += iReader.iSkipLength;
            iReader.iSkipLength = 0;
            }
        else
            {
            iFHState = EStateInUnknown;
            }
        return EFrameComplete;
        }
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::UpdateStateFromMarkerL
// Update the current state according to the marker type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFrameState CJp2kReadCodec::UpdateStateFromMarkerL()
    {
    if ( ( iReader.iPtrEnd - iReader.iPtr ) < KMarkerSize )
        {
        // Underflow
        return EFrameIncomplete;
        }

    TUint16 marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
    switch ( marker )
        {
        case KCOD:
            {
            iFHState = EStateInCOD;
            break;
            }
        case KCOC:
            {
            iFHState = EStateInCOC;
            break;
            }
        case KQCD:
            {
            iFHState = EStateInQCD;
            break;
            }
        case KQCC:
            {
            iFHState = EStateInQCC;
            break;
            }
        case KRGN:
            {
            iFHState = EStateInRGN;
            break;
            }
        case KPOC:
            {
            iFHState = EStateInPOC;
            break;
            }
        case KPPM:
            {
            iFHState = EStateInPPM;
            break;
            }
        case KTLM:
            {
            iFHState = EStateInTLM;
            break;
            }
        case KPLM:
            {
            iFHState = EStateInPLM;
            break;
            }
        case KCRG:
            {
            iFHState = EStateInCRG;
            break;
            }
        case KCME:
            {
            iFHState = EStateInCOM;
            break;
            }
        case KSOT:
            {
            iFHState = EStateInSOT;
            break;
            }
        case KPPT:
            {
            iFHState = EStateInPPT;
            break;
            }
        case KPLT:
            {
            iFHState = EStateInPLT;
            break;
            }
        case KSOD:
            {
            iFHState = EStateInSOD;
            break;
            }
        case KEOC:
            {
            if ( !iUseNewTile && iImageInfo->TileCount() )
                {
                // There is a tile which has not been decoded yet
                CJ2kTileInfo& tile = CONST_CAST( CJ2kTileInfo&, iImageInfo->TileAt( iLastTileIndex ) );

                tile.DoReleaseUnusedMarkers();

                // Decode and delete the tile
                DecodeAndDeleteTileL( tile );

                // Start with new tile
                iUseNewTile = ETrue;
                if ( !iUseNextTile )
                    {
                    iFHState = EStateInUnknown;
                    return EFrameIncompleteRepositionRequest;
                    }
                else
                    {
                    if ( !iSequential )
                        {
                        // Must be End of Codestream EOC
                        iFHState = EStateInEOC;
                        iReader.iPtr += KMarkerSize;
                        return EFrameComplete;
                        }
                    }
                }
            else
                {
                iFHState = EStateInEOC;
                
                // Have to increment the iterator
                iReader.iPtr += KMarkerSize;
                }
            break;
            }
        default:
            {
            if ( marker < KEXTS || marker > KEXTE )
                {
                // Unrecognized marker
                User::Leave( KErrCorrupt );
                }
            else
                {
                // Ignore the extension marker
                iReader.iPtr += KMarkerSize;
                }
            break;
            }
        }
    return EFrameComplete;
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::DecodeAndDeleteTileL
// Decode the tile, write to image processor and delete the tile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::DecodeAndDeleteTileL( CJ2kTileInfo& aTile )
    {
    if ( !iEntropy )
        {
        iEntropy = CJ2kEntropyDecoder::NewL( *iImageInfo );
        }

    if ( !iImageWriter )
        {
        iImageWriter = CJ2kImageWriter::NewL( ImageProcessor(  ), *iImageInfo, CONST_CAST( TJ2kInfo&,iJ2kInfo ) );
        }

    if ( !iSynthesis )
        {
        iSynthesis = new ( ELeave ) CJ2kSynthesis;
        }

    if ( iStyleUsed == EUnknownDecoder )
        {
        // Use tile-based as the default
        iStyleUsed = ETileBasedDecoder;

        // If extra levels are dropped, the resolution of the decoded image is larger than 
        // the resolution of the output image. levelsDropped here gives the resolution of 
        // the decoded image (and thus relates to the amount of memory required for decoding).
        TUint8 levelsDropped = (TUint8)(iImageInfo->LevelDrop() - iImageInfo->ExtraLevelDrop());

        TSizMarker& sizMarker = CONST_CAST( TSizMarker&, iImageInfo->SizMarker() );
        if ( ( sizMarker.iXTsiz >> levelsDropped ) > (TUint32) KWaveletBlockSize ||
             ( sizMarker.iYTsiz >> levelsDropped ) > (TUint32) KWaveletBlockSize )
            {
            iStyleUsed = EBlockBasedDecoder;
            }
        }

    if ( iStyleUsed == ETileBasedDecoder )
        {
        iSynthesis->DecodeTileL( *iImageWriter, *iEntropy, *iImageInfo, aTile );
        }
    else
        {
        iSynthesis->DecodeTileBlockL( *iImageWriter, *iEntropy, *iImageInfo, aTile );
        }

    iImageInfo->Remove( 0 );
    }

// -----------------------------------------------------------------------------
// CJp2kReadCodec::ConvertImageDataL
// Convert some metadata from file format to TImageDataBlock derived objects
// and let the framework managing the buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJp2kReadCodec::ConvertImageDataL()
    {
    TJ2kInfo& info = CONST_CAST( TJ2kInfo&, iJ2kInfo );

    TInt index;
    // Convert each IPR box to TJp2kIprBox image data
    for ( index = 0; index < info.iIPRList.Count(); ++index )
        {
        TJp2kIprBox* jp2kIPR = new (ELeave) TJp2kIprBox;
        CleanupDeletePushL( jp2kIPR );

        jp2kIPR->iIprData = info.iIPRList[index];

        User::LeaveIfError( iFrameData->AppendImageData( jp2kIPR ) );
        CleanupStack::Pop(); // jp2kIPR

        // Transfer the ownership of the buffer to the framework
        User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kIPR->iIprData ) );
        info.iIPRList[index] = 0;
        }
    info.iIPRList.Reset();

    // Convert each XML box to TJp2kXmlBox image data
    for ( index = 0; index < info.iXMLList.Count(); ++index )
        {
        TJp2kXmlBox* jp2kXML = new (ELeave) TJp2kXmlBox;
        CleanupDeletePushL( jp2kXML );

        jp2kXML->iXmlData = info.iXMLList[index];

        User::LeaveIfError( iFrameData->AppendImageData( jp2kXML ) );
        CleanupStack::Pop(); // jp2kXML

        // Transfer the ownership of the buffer to the framework
        User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kXML->iXmlData ) );
        info.iXMLList[index] = 0;
        }
    info.iXMLList.Reset();

    TInt length;
    const TUint8 *ptr = 0;
    // Convert each UUID box to TJp2kUuidBox image data
    for ( index = 0; index < info.iUUIDList.Count(); ++index )
        {
        TJp2kUuidBox* jp2kUUID = new (ELeave) TJp2kUuidBox;
        CleanupDeletePushL( jp2kUUID );

        ptr = info.iUUIDList[index]->Ptr();
        jp2kUUID->iUuidId.Copy( ptr, KJ2KUuidIDSize );

        // Advance the pointer
        ptr += KJ2KUuidIDSize;

        length = info.iUUIDList[index]->Length() - KJ2KUuidIDSize;
        jp2kUUID->iUuidData = HBufC8::NewLC( length );
        jp2kUUID->iUuidData->Des().Append( ptr, length );

        // Transfer the ownership of the buffer to the framework
        User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kUUID->iUuidData ) );
        CleanupStack::Pop( 1 ); // jp2kUUID->iUuidData

        User::LeaveIfError( iFrameData->AppendImageData( jp2kUUID ) );
        CleanupStack::Pop( 1 ); // jp2kUUID
        }
    info.iUUIDList.ResetAndDestroy();

    // Convert each UUIDInfo box to TJp2kUuidInfoBox image data
    for ( index = 0; index < info.iUUIDInfoListList.Count(); ++index )
        {
        TJp2kUuidInfoBox* jp2kUUIDInfo = new (ELeave) TJp2kUuidInfoBox;
        CleanupDeletePushL( jp2kUUIDInfo );

        if ( info.iUUIDInfoListList[index]->Length() )
            {
            ptr = info.iUUIDInfoListList[index]->Ptr();
            jp2kUUIDInfo->iUuidInfoNu = PtrReadUtil::ReadBigEndianUint16Inc( ptr );

            length = jp2kUUIDInfo->iUuidInfoNu * KJ2KUuidIDSize;
            jp2kUUIDInfo->iUuidInfoId = HBufC8::NewLC( length );
            jp2kUUIDInfo->iUuidInfoId->Des().Append( ptr, length );
            }

        if ( info.iUUIDInfoUrlList[index]->Length() )
            {
            ptr = info.iUUIDInfoUrlList[index]->Ptr();

            // 1 byte unsigned integer
            jp2kUUIDInfo->iUuidInfoVersion = *ptr++;

            // 3 bytes unsigned integer (in little endian)
            jp2kUUIDInfo->iUuidInfoFlag = *ptr++;
            jp2kUUIDInfo->iUuidInfoFlag |= ( *ptr++ << 8 );
            jp2kUUIDInfo->iUuidInfoFlag |= ( *ptr++ << 16 );

            length = info.iUUIDInfoUrlList[index]->Length() - 4;
            jp2kUUIDInfo->iUuidInfoData = HBufC8::NewLC( length );
            jp2kUUIDInfo->iUuidInfoData->Des().Append( ptr, length );
            }

        // Transfer the ownership of the buffer to the framework
        if ( jp2kUUIDInfo->iUuidInfoData )
            {
            User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kUUIDInfo->iUuidInfoData ) );
            CleanupStack::Pop( 1 ); // jp2kUUIDInfo->iUUidInfoData
            }

        // Transfer the ownership of the buffer to the framework
        if ( jp2kUUIDInfo->iUuidInfoId )
            {
            User::LeaveIfError( iFrameData->AppendImageBuffer( jp2kUUIDInfo->iUuidInfoId ) );
            CleanupStack::Pop( 1 ); // jp2kUUIDInfo->iUuidInfoId
            }

        User::LeaveIfError( iFrameData->AppendImageData( jp2kUUIDInfo ) );
        CleanupStack::Pop( 1 ); // jp2kUUIDInfo
        }
    info.iUUIDInfoListList.ResetAndDestroy();
    info.iUUIDInfoUrlList.ResetAndDestroy();
    }
