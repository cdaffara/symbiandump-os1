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
* Description:  CJ2kTileInfo class used to collect tile related information 
*                such as Tile Part Header, SOT marker and list of components.
*
*/


// INCLUDE FILES
#include <icl/imagecodec.h>
#include "JP2KImageUtils.h"
#include "JP2KFormat.h"
#include "JP2KStreamReader.h"
#include "JP2KTileInfo.h"
#include "JP2KImageInfo.h"
#include "JP2KCodec.h"
#include "JP2KComponentInfo.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CJ2kTileInfo* CJ2kTileInfo::NewLC( CJ2kImageInfo& aImageInfo, TJ2kStreamReader& aReader )
    {
    CJ2kTileInfo* self = new ( ELeave ) CJ2kTileInfo( aImageInfo, aReader );

    CleanupStack::PushL( self );

    return self;
    }

// Destructor
CJ2kTileInfo::~CJ2kTileInfo()
    {
    DoReleaseUnusedMarkers();

    iComponentList.ResetAndDestroy();

    delete iPpt;
    iPpt = 0;

    delete iPptBuffer;
    iPptBuffer = 0;

    // iPOC not owned - not deleted. 
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::InitializeL
// Initialize the components in the tile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::InitializeL()
    {
    const TSizMarker &sizMarker = iImageInfo.SizMarker();
    TUint16 numOfHorizTiles = iImageInfo.NumOfHorizTilesL();

    // Calculate the tile canvas
    TDiv    tDiv = TJ2kUtils::DivL( iSot.iIsot, numOfHorizTiles );
    iTileCanvas.iTl = TPoint( Max( sizMarker.iXTOsiz + tDiv.rem * sizMarker.iXTsiz, sizMarker.iXOsiz ),
                              Max( sizMarker.iYTOsiz + tDiv.quot * sizMarker.iYTsiz, sizMarker.iYOsiz ) );
    iTileCanvas.iBr = TPoint( Min( sizMarker.iXTOsiz + ( tDiv.rem + 1 ) * sizMarker.iXTsiz, sizMarker.iXsiz ),
                              Min( sizMarker.iYTOsiz + ( tDiv.quot + 1 ) * sizMarker.iYTsiz, sizMarker.iYsiz ) );

    CJ2kComponentInfo *component = 0;
    for ( TUint16 index = 0; index < sizMarker.iCsiz; ++index )
        {
        // Instantiate the components in the tile
        component = CJ2kComponentInfo::NewLC( *this, index );
        User::LeaveIfError( iComponentList.Append( component ) );
        CleanupStack::Pop();
        }

    iLastN1 = iTileCanvas.iTl.iY;
    iLastN2 = iTileCanvas.iTl.iX;

    // Initialize the end of layer, resolution level, component to process
    iNumOfLayersPOC = NumOfLayers();
    iNumOfLevelsPOC = NumOfLevels();
    iNumOfComponentsPOC = sizMarker.iCsiz;

    if ( IsSpeedup() )
        {
        iNumOfLevelsPOC = (TUint8)( iNumOfLevelsPOC - iImageInfo.LevelDrop() );
        }

    // Let iPOC points to the POC marker in Tile Part Header
    iPOC = iTileMarker.iPoc;
    if ( !iPOC )
        {
        // Let iPOC points to the POC marker in the Main Header
        iPOC = iImageInfo.MainMarker().iPoc;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendCOD
// Verify and append COD to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendCOD( TCODMarker *aMarker, TUint32 aLength )
    {
    if ( iSot.iTPsot == 0 )
        {
        // Use only for the first tile part of a tile
        if ( iTileMarker.iCod == 0 )
            {
            iTileMarker.iCod = aMarker;
            }
        else
            {
            // No more than one COD per tile
            delete aMarker;
            aMarker = 0;
            }
        }
    else
        {
        // Read but ignore it
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendCOCL
// Verify and append COC to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendCOCL( TCOCMarker *aMarker, TUint32 aLength )
    {
    if ( iSot.iTPsot == 0 )
        {
        // Use only for the first tile part of a tile
        TUint8 found = EFalse;
        for ( TUint16 index = 0; index < iTileMarker.iCoc.Count(); ++index )
            {
            if ( iTileMarker.iCoc[index]->iCcoc == aMarker->iCcoc )
                {
                found = ETrue; 
                index = (TUint16)iTileMarker.iCoc.Count();   
                }
            }
        if ( !found )
            {
            User::LeaveIfError( iTileMarker.iCoc.Append( aMarker ) );
            }
        else
            {
            // No more than one COC per component
            delete aMarker;
            aMarker = 0;
            }
        }
    else
        {
        // Read but ignore it
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendQCD
// Verify and append QCD to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendQCD( TQCDMarker *aMarker, TUint32 aLength )
    {
    if ( iSot.iTPsot == 0 )
        {
        // Use only for the first tile part of a tile
        if ( iTileMarker.iQcd == 0 )
            {
            iTileMarker.iQcd = aMarker;
            }
        else
            {
            // No more than one QCD per tile
            delete aMarker;
            aMarker = 0;
            }
        }
    else
        {
        // Read but ignore it
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendQCCL
// Verify and append QCC to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendQCCL( TQCCMarker *aMarker, TUint32 aLength )
    {
    if ( iSot.iTPsot == 0 )
        {
        // Use only for the first tile part of a tile
        TUint8 found = EFalse;
        for ( TUint16 index = 0; index < iTileMarker.iQcc.Count(); ++index )
            {
            if ( iTileMarker.iQcc[index]->iCqcc == aMarker->iCqcc )
                {
                found = ETrue;
                index = (TUint16)iTileMarker.iQcc.Count();  
                }
            }
        if ( !found )
            {
            User::LeaveIfError( iTileMarker.iQcc.Append( aMarker ) );
            }
        else
            {
            // No more than one QCC per component
            delete aMarker;
            aMarker = 0;
            }
        }
    else
        {
        // read but ignore it
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendRGNL
// Verify and append RGN to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendRGNL( TRGNMarker *aMarker, TUint32 aLength )
    {
    if ( iSot.iTPsot == 0 )
        {
        // Use only for the first tile part of a tile
        TUint8 found = EFalse;
        for ( TUint16 index = 0; index < iTileMarker.iRgn.Count(); ++index )
            {
            if ( iTileMarker.iRgn[index]->iCrgn == aMarker->iCrgn )
                {
                found = ETrue;
                index = (TUint16)iTileMarker.iRgn.Count();   
                }
            }
        if ( !found )
            {
            User::LeaveIfError( iTileMarker.iRgn.Append( aMarker ) );
            }
        else
            {
            // No more than one RGN per component
            delete aMarker;
            aMarker = 0;
            }
        }
    else
        {
        // Read but ignore it
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendPOCL
// Verify and append POC to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendPOCL( TPOCMarker *aMarker, TUint32 aLength )
    {
    if ( iTileMarker.iPoc == 0 )
        {
        iTileMarker.iPoc = aMarker;
        }
    else
        {
        // Appending the content of the POC to the
        // existing tile part header POC
        for ( TInt index = 0; index < aMarker->iPpoc.Count(); ++index )
            {
            User::LeaveIfError( iTileMarker.iPoc->iRSpoc.Append( aMarker->iRSpoc[index] ) );
            User::LeaveIfError( iTileMarker.iPoc->iCSpoc.Append( aMarker->iCSpoc[index] ) );
            User::LeaveIfError( iTileMarker.iPoc->iLYEpoc.Append( aMarker->iLYEpoc[index] ) );
            User::LeaveIfError( iTileMarker.iPoc->iREpoc.Append( aMarker->iREpoc[index] ) );
            User::LeaveIfError( iTileMarker.iPoc->iCEpoc.Append( aMarker->iCEpoc[index] ) );
            User::LeaveIfError( iTileMarker.iPoc->iPpoc.Append( aMarker->iPpoc[index] ) );
            }
        // At most one POC may exist in tile part header
        delete aMarker;
        aMarker = 0;
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendCOML
// Verify and append COM to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendCOML( TCOMMarker *aMarker, TUint32 aLength )
    {
    User::LeaveIfError( iTileMarker.iCom.Append( aMarker ) );

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendPPTL
// Verify and append PPT to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendPPTL( TPPTMarker *aMarker, TUint32 aLength )
    {
    TUint8 inserted = EFalse;

    // Insert the new PPT marker into the right order
    for ( TUint16 index = 0; index < iTileMarker.iPpt.Count(); ++index )
        {
        // Order by iZppt of the PPT marker
        if ( iTileMarker.iPpt[index]->iZppt > aMarker->iZppt )
            {
            User::LeaveIfError( iTileMarker.iPpt.Insert( aMarker, index ) );
            inserted = ETrue;
            index = (TUint16)( iTileMarker.iPpt.Count() );   
            }
        }

    if ( !inserted )
        {
        User::LeaveIfError( iTileMarker.iPpt.Append( aMarker ) );
        }
    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::AppendPLTL
// Verify and append PLT to the tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AppendPLTL( TPLTMarker *aMarker, TUint32 aLength )
    {
    TUint8 inserted = EFalse;

    // Insert the new PLT marker into the right order
    for ( TUint16 index = 0; index < iTileMarker.iPlt.Count(); ++index )
        {
        // Order by iZplt of the PLT marker
        if ( iTileMarker.iPlt[index]->iZplt > aMarker->iZplt )
            {
            User::LeaveIfError( iTileMarker.iPlt.Insert( aMarker, index ) );
            inserted = ETrue;
            index = (TUint16)( iTileMarker.iPlt.Count() );   
            }
        }

    if ( !inserted )
        {
        User::LeaveIfError( iTileMarker.iPlt.Append( aMarker ) );
        }

    if ( iTileLength )
        {
        iTileLength -= aLength;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ReadBitStreamL
// Parsing the bitstream data based on progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::ReadBitStreamL( TUint8 aBool )
    {
    SetCheckMarker( aBool );
    if ( iPOC )
        {
        TUint8 incomplete = EFalse;
        while ( iLastPOC < iPOC->iPpoc.Count() )
            {
            if ( !iDoLoadPOC )
                {
                // Reload progression order change from POC marker
                iDoLoadPOC = ETrue;
                iLastLayer = 0;
                iLastLevelOrig = iLastLevel = (TUint8)iPOC->iRSpoc[iLastPOC];
                iLastComponentOrig = iLastComponent = (TUint16)iPOC->iCSpoc[iLastPOC];

                iNumOfLayersPOC = (TUint16)iPOC->iLYEpoc[iLastPOC];
                if ( iNumOfLayersPOC > NumOfLayers() )
                    {
                    iNumOfLayersPOC = NumOfLayers();
                    }

                iNumOfLevelsPOC = (TUint8)( iPOC->iREpoc[iLastPOC] - 1 );
                if ( iNumOfLevelsPOC > (TUint8)NumOfLevels() )
                    {
                    iNumOfLevelsPOC = (TUint8)NumOfLevels();
                    }

                iNumOfComponentsPOC = (TUint16)iPOC->iCEpoc[iLastPOC];
                if ( iNumOfComponentsPOC > NumOfComponents() )
                    {
                    iNumOfComponentsPOC = NumOfComponents();
                    }

                for ( TUint16 index = 0; index < iNumOfComponentsPOC; ++index )
                    {
                    iComponentList[index]->ResetLastPacketProcessed();
                    }

                iLastN2 = iTileCanvas.iTl.iX;
                iLastN1 = iTileCanvas.iTl.iY;
                }

            switch ( iPOC->iPpoc[iLastPOC] )
                {
                case EProgression_L_R_C_P:
                    {
                    incomplete = LRCPProgressionL();
                    break;
                    }
                case EProgression_R_L_C_P:
                    {
                    incomplete = RLCPProgressionL();
                    break;
                    }
                case EProgression_R_P_C_L:
                    {
                    incomplete = RPCLProgressionL();
                    break;
                    }
                case EProgression_P_C_R_L:
                    {
                    incomplete = PCRLProgressionL();
                    break;
                    }
                case EProgression_C_P_R_L:
                    {
                    incomplete = CPRLProgressionL();
                    break;
                    }
                default:
                    {
                    // Unrecognized progressin order
                    User::Leave( KErrCorrupt );
                    break;
                    }
                }

            if ( incomplete )
                {
                break;   //lint !e960    It is Ok to break here.
                }

            if ( iLastPOC == iPOC->iPpoc.Count() && !iDoLoadPOC )
                {
                // Try to use Main POC if it is currently using Tile POC
                if ( iTileMarker.iPoc && iPOC == iTileMarker.iPoc )
                    {
                    if ( iImageInfo.MainMarker().iPoc )
                        {
                        iPOC = iImageInfo.MainMarker().iPoc;
                        iLastPOC = 0;
                        }
                    }
                }
            }
        }
    else
        {
        // Without POC changes
        switch ( ProgressionOrder() )
            {
            case EProgression_L_R_C_P:
                {
                LRCPProgressionL();
                break;
                }
            case EProgression_R_L_C_P:
                {
                RLCPProgressionL();
                break;
                }
            case EProgression_R_P_C_L:
                {
                RPCLProgressionL();
                break;
                }
            case EProgression_P_C_R_L:
                {
                PCRLProgressionL();
                break;
                }
            case EProgression_C_P_R_L:
                {
                CPRLProgressionL();
                break;
                }
            default:
                {
                // Unrecognized progression order
                User::Leave( KErrCorrupt );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ColorTransformation
// Get the color transformation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::ColorTransformation() const
    {
    return ( iTileMarker.iCod ) ?
             iTileMarker.iCod->iColorTransformation :
             iImageInfo.MainMarker().iCod.iColorTransformation;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::ProgressionOrder
// Get the progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::ProgressionOrder() const
    {
    return ( iTileMarker.iCod ) ?
             iTileMarker.iCod->iProgressionOrder :
             iImageInfo.MainMarker().iCod.iProgressionOrder;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NumOfLayers
// Get the number of layers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint16 CJ2kTileInfo::NumOfLayers() const
    {
    return ( iTileMarker.iCod ) ?
             iTileMarker.iCod->iNumOfLayers :
             iImageInfo.MainMarker().iCod.iNumOfLayers;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::NumOfLevels
// Get the number of resolution level.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::NumOfLevels() const
    {
    TUint8 maxLevel = 0;
    for ( TUint16 index = 0; index < iImageInfo.NumOfComponents(); ++index )
        {
        maxLevel = Max( maxLevel, iComponentList[index]->Levels() );
        }

    return maxLevel;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::GetMinGrid
// Get the minimum grid among the components in the tile.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::GetMinGrid( TSize &aGrid ) const
    {
    TSize tmpGrid;
    aGrid = iComponentList[0]->MinGrid();
    for ( TUint16 index = 1; index < iImageInfo.NumOfComponents(); ++index )
        {
        tmpGrid = iComponentList[index]->MinGrid();
        aGrid.iWidth = Min( aGrid.iWidth, tmpGrid.iWidth );
        aGrid.iHeight = Min( aGrid.iHeight, tmpGrid.iHeight );
        }
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::UsePPTL
// Set up to read the packet header from the PPT marker.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::UsePPTL()
    {
    if ( !iPpt )
        {
        iPpt = new ( ELeave ) TPPTStream;
        }
    Mem::FillZ( iPpt, sizeof( TPPTStream ) );

    // Remove the previous ppt buffer
    delete iPptBuffer;
    iPptBuffer = 0;

    if ( iTileMarker.iPpt.Count() > 1  )
        {
        // Concatenate all PPT markers into one
        TInt bytes = 0;
        TInt index = 0;
        for ( index = 0; index < iTileMarker.iPpt.Count(); ++index )
            {
            bytes += iTileMarker.iPpt[index]->iIppt->Length();
            }

        iPptBuffer = HBufC8::NewL( bytes );
        iPpt->iPtrEnd = (TUint32)bytes;
        TPtr8 ptr = iPptBuffer->Des();
        for ( index = 0; index < iTileMarker.iPpt.Count(); ++index )
            {
            ptr.Append( iTileMarker.iPpt[index]->iIppt->Ptr(), iTileMarker.iPpt[index]->iIppt->Length() );
            }
        }
    else
        {
        // Use the only PPT marker
        // transfer the ownership of the buffer from PPT to iPptBuffer
        iPptBuffer = iTileMarker.iPpt[0]->iIppt;
        iPpt->iPtrEnd = iPptBuffer->Length();
        iTileMarker.iPpt[0]->iIppt = 0;
        }

    // PPT markers can be released
    iTileMarker.iPpt.ResetAndDestroy();
    iPacketHeaderReader = STATIC_CAST( MJ2kPacketHeaderReader*, this );
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::ReadEPHMarker
// Try to consume the EPH marker if there is one.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8  CJ2kTileInfo::ReadEPHMarker()
    {
    if ( iPpt && ( iPpt->iPtrEnd - iPpt->iPtr ) >= 2 )
        {
        const TUint8 *ptr = iPptBuffer->Ptr();
        ptr += iPpt->iPtr;
        if ( PtrReadUtil::ReadBigEndianUint16( ptr ) == KEPH )
            {
            iPpt->iPtr += 2;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::ReadBit
// Read a bit from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::ReadBit( TUint8& aBit )
    {
    if ( iPpt->iPos == 0 )
        {
        if ( iPpt->iPtr < iPpt->iPtrEnd )
            {
            // Stream contains some packet header data
            iPpt->iData = ( *iPptBuffer )[iPpt->iPtr++];
            iPpt->iPos = iPpt->iNextPos;
            if ( iPpt->iNextPos == 0x08 && iPpt->iData == 0xff )
                {
                iPpt->iNextPos = 0x07;
                }
            else
                {
                iPpt->iNextPos = 0x08;
                }
            }
        else
            {
            // No more data
            return ETrue;
            }
        }
    aBit = (TUint8)( ( iPpt->iData >> ( --iPpt->iPos ) ) & 0x01 );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::ReadBits( TUint8& aBit, TUint8 aBitLen )
    {
    aBit = (TUint8)0;
    TUint8 bit = 0;
    for ( TInt8 index = (TInt8)( aBitLen - 1 ); index >= 0; --index )
        {
        if ( !ReadBit( bit ) )
            {
            aBit |= ( bit << index );
            }
        else
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::ReadBits( TUint32& aBit, TUint8 aBitLen )
    {
    aBit = ( TUint32 )0;
    TUint8 bit = 0;
    for ( TInt8 index = ( TInt8 )( aBitLen - 1 ); index >= 0; --index )
        {
        if ( !ReadBit( bit ) )
            {
            aBit |= ( bit << index );
            }
        else
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::StartReadBit
// Start reading from packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::StartReadBit()
    {
    if ( iPpt && iPpt->iPtr < iPpt->iPtrEnd )
        {
         // Stream contains some packet header data
        iPpt->iData = ( *iPptBuffer )[iPpt->iPtr++];
        iPpt->iPos = iPpt->iNextPos = 0x08;
        if ( iPpt->iData == 0xff )
            {
            iPpt->iNextPos = 0x07;
            }

        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// From MJ2kPacketHeaderReader
// CJ2kTileInfo::AlignReader
// Align the stream to the next byte boundary if necessary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::AlignReader()
    {
    if ( iPpt->iNextPos == 0x07 )
        {
        if ( iPpt->iPtr < iPpt->iPtrEnd )
            {
            ++iPpt->iPtr;
            }
        }
    iPpt->iData = iPpt->iPos = iPpt->iNextPos = 0;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::DoReleaseUnusedMarkers
// Release unused markers in tile part header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kTileInfo::DoReleaseUnusedMarkers()
    {
    delete iPptBuffer;
    iPptBuffer = 0;

    iTileMarker.iPlt.ResetAndDestroy();
    iTileMarker.iCom.ResetAndDestroy();

    delete iTileMarker.iPoc;
    iTileMarker.iPoc = 0;

    delete iTileMarker.iCod;
    iTileMarker.iCod = 0;

    iTileMarker.iCoc.ResetAndDestroy();
    iTileMarker.iRgn.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::CJ2kTileInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kTileInfo::CJ2kTileInfo( CJ2kImageInfo& aImageInfo, TJ2kStreamReader& aReader ) : 
    iComponentList( 3 ), 
    iImageInfo( aImageInfo ), 
    iReader( aReader )
    {
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::LRCPProgressionL
// Parse the bitstream with LRCP progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::LRCPProgressionL()
    {
    TUint16 marker = 0;
    TUint8  compIncomplete = EFalse;

    // For each layer
    while ( iLastLayer < iNumOfLayersPOC )
        {
        // For each resolution level
        while ( iLastLevel <= iNumOfLevelsPOC )
            {
            // For each component
            while ( iLastComponent < iNumOfComponentsPOC )
                {
                compIncomplete = iComponentList[iLastComponent]->LRCPProgressionL( *this );
                if ( compIncomplete )
                    {
                    // Underflow
                    return compIncomplete;
                    }

                if ( IsPOC() )
                    {
                    // Check if we found next SOT or EOC
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                    if ( marker == KSOT || marker == KEOC )
                        {
                        compIncomplete = ETrue;
                        return compIncomplete;
                        }
                    }

                ++iLastComponent;
                }

            iLastComponent = iLastComponentOrig;
            ++iLastLevel;

            if ( IsCheckMarker() )
                {
                // Check if we found next SOT or EOC
                // it signals the end of one LEVEL
                marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                if ( marker == KSOT || marker == KEOC )
                    {
                    compIncomplete = ETrue;
                    return compIncomplete;
                    }
                }
            }

        iLastLevel = iLastLevelOrig;
        ++iLastLayer;
        }

    // Continue using the next POC
    ++iLastPOC;
    iDoLoadPOC = EFalse;

    return compIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::RLCPProgressionL
// Parse the bitstream with RLCP progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::RLCPProgressionL()
    {
    TUint16 marker = 0;
    TUint8  compIncomplete = EFalse;

    // For each resolution level
    while ( iLastLevel <= iNumOfLevelsPOC )
        {
        // For each layer
        while ( iLastLayer < iNumOfLayersPOC )
            {
            // For each component
            while ( iLastComponent < iNumOfComponentsPOC )
                {
                compIncomplete = iComponentList[iLastComponent]->LRCPProgressionL( *this );
                if ( compIncomplete )
                    {
                    // Underflow
                    return compIncomplete;
                    }

                if ( IsPOC() )
                    {
                    // Check if we found next SOT or EOC
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                    if ( marker == KSOT || marker == KEOC )
                        {
                        compIncomplete = ETrue;
                        return compIncomplete;
                        }
                    }

                ++iLastComponent;
                }

            iLastComponent = iLastComponentOrig;
            ++iLastLayer;

            if ( IsCheckMarker() )
                {
                // Check if we found next SOT or EOC
                // it signals the end of one LAYER
                marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                if ( marker == KSOT || marker == KEOC )
                    {
                    
                    // If we are at the end of layers, we have to reset 
                    // layers and move to next resolution level
                    if(iLastLayer==iNumOfLayersPOC)
                    {
                        iLastLayer = 0;
                        ++iLastLevel;
                    }
                    
                    compIncomplete = ETrue;
                    return compIncomplete;
                    }
                }
            }

        iLastLayer = 0;
        ++iLastLevel;
        }

    // Continue using the next POC
    ++iLastPOC;
    iDoLoadPOC = EFalse;

    return compIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::RPCLProgressionL
// Parse the bitstream with RPCL progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::RPCLProgressionL()
    {
    TUint16 marker = 0;
    TUint8  compIncomplete = EFalse;

    TSize   minGrid( 0, 0 );
    GetMinGrid( minGrid );
	
	// Check that no divided by zero calculation are done.
	if ( minGrid.iWidth == 0 || minGrid.iHeight == 0 )
		{
		User::Leave( KErrCorrupt );	
		}

    // For each resolution level
    while ( iLastLevel <= iNumOfLevelsPOC )
        {
        // For each position in vertical direction
        while ( iLastN1 < iTileCanvas.iBr.iY )
            {
            // For each position in horizontal direction
            while ( iLastN2 < iTileCanvas.iBr.iX )
                {
                // For each component
                while ( iLastComponent < iNumOfComponentsPOC )
                    {
                    compIncomplete = iComponentList[iLastComponent]->RPCLProgressionL( *this );
                    if ( compIncomplete )
                        {
                        // Underflow
                        return compIncomplete;
                        }

                    if ( IsPOC() )
                        {
                        // Check if we found next SOT or EOC
                        // it signals the end of one Precinct
                        marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                        if ( marker == KSOT || marker == KEOC )
                            {
                            compIncomplete = ETrue;
                            return compIncomplete;
                            }
                        }

                    ++iLastComponent;
                    }

                iLastComponent = iLastComponentOrig;
                iLastN2 += ( minGrid.iWidth - ( iLastN2 % minGrid.iWidth ) );

                if ( IsCheckMarker() )
                    {
                    // Check if we found next SOT or EOC
                    // it signals the end of one Precinct
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                    if ( marker == KSOT || marker == KEOC )
                        {
                        
                        // If we are at the end of horizontal precincts, move to 
                        // next row of vertical precincts
                        if(iLastN2 >= iTileCanvas.iBr.iX)
                        {
                            iLastN2 = iTileCanvas.iTl.iX;
                            iLastN1 += ( minGrid.iHeight - ( iLastN1 % minGrid.iHeight ) );
                            // If we are at the end of vertical precincts, move to next 
                            // resolution level
                            if(iLastN1 >= iTileCanvas.iBr.iY)
                            {
                                iLastN1 = iTileCanvas.iTl.iY;
                                // Advance to next level to avoid a situation where we would 
                                // return without reading any bytes from the frame the next 
                                // time we enter this function
                                ++iLastLevel;
                            }
                        }
                        
                        compIncomplete = ETrue;
                        return compIncomplete;
                        }
                    }
                }

            iLastN2 = iTileCanvas.iTl.iX;
            iLastN1 += ( minGrid.iHeight - ( iLastN1 % minGrid.iHeight ) );
            }

        iLastN2 = iTileCanvas.iTl.iX;
        iLastN1 = iTileCanvas.iTl.iY;
        ++iLastLevel;
        }

    // Continue using the next POC
    ++iLastPOC;
    iDoLoadPOC = EFalse;

    return compIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::PCRLProgressionL
// Parse the bitstream with PCRL progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::PCRLProgressionL()
    {
    TUint16 marker = 0;
    TUint8  compIncomplete = EFalse;

    TSize   minGrid;
    GetMinGrid( minGrid );

	// Check that no divided by zero calculation are done.
	if ( minGrid.iWidth == 0 || minGrid.iHeight == 0 )
		{
		User::Leave( KErrCorrupt );	
		}

    // For each position in vertical direction
    while ( iLastN1 < iTileCanvas.iBr.iY )
        {
        // For each position in horizontal direction
        while ( iLastN2 < iTileCanvas.iBr.iX )
            {
            // For each component
            while ( iLastComponent < iNumOfComponentsPOC )
                {
                // For each resolution level
                while ( iLastLevel <= iNumOfLevelsPOC )
                    {
                    compIncomplete = iComponentList[iLastComponent]->CPRLProgressionL( *this );
                    if ( compIncomplete )
                        {
                        // Underflow
                        return compIncomplete;
                        }

                    if ( IsPOC() )
                        {
                        // Check if we found next SOT or EOC
                        // it signals the end of one Precinct
                        marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                        if ( marker == KSOT || marker == KEOC )
                            {
                            compIncomplete = ETrue;
                            return compIncomplete;
                            }
                        }

                    ++iLastLevel;
                    }

                iLastLevel = iLastLevelOrig;
                ++iLastComponent;

                if ( IsCheckMarker() )
                    {
                    // Check if we found next SOT or EOC
                    // it signals the end of one Precinct
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                    if ( marker == KSOT || marker == KEOC )
                        {
                        compIncomplete = ETrue;
                        return compIncomplete;
                        }
                    }
                }

            iLastComponent = iLastComponentOrig;
            iLastN2 += ( minGrid.iWidth - ( iLastN2 % minGrid.iWidth ) );
            }

        iLastN2 = iTileCanvas.iTl.iX;
        iLastN1 += ( minGrid.iHeight - ( iLastN1 % minGrid.iHeight ) );
        }

    // Continue using the next POC
    ++iLastPOC;
    iDoLoadPOC = EFalse;

    return compIncomplete;
    }

// -----------------------------------------------------------------------------
// CJ2kTileInfo::CPRLProgressionL
// Parse the bitstream with CPRL progression order.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kTileInfo::CPRLProgressionL()
    {
    TUint16 marker = 0;
    TUint8  compIncomplete = EFalse;

    TSize   minGrid;
    GetMinGrid( minGrid );

	// Check that no divided by zero calculation are done.
	if ( minGrid.iWidth == 0 || minGrid.iHeight == 0 )
		{
		User::Leave( KErrCorrupt );	
		}

    // For each component
    while ( iLastComponent < iNumOfComponentsPOC )
        {
        // For each position in vertical direction
        while ( iLastN1 < iTileCanvas.iBr.iY )
            {
            // For each position in horizontal direction
            while ( iLastN2 < iTileCanvas.iBr.iX )
                {
                // For each resolution level
                while ( iLastLevel <= iNumOfLevelsPOC )
                    {
                    compIncomplete = iComponentList[iLastComponent]->CPRLProgressionL( *this );
                    if ( compIncomplete )
                        {
                        // Underflow
                        return compIncomplete;
                        }

                    if ( IsPOC() )
                        {
                        // Check if we found next SOT or EOC
                        // it signals the end of one Precinct
                        marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                        if ( marker == KSOT || marker == KEOC )
                            {
                            compIncomplete = ETrue;
                            return compIncomplete;
                            }
                        }

                    ++iLastLevel;
                    }

                iLastLevel = iLastLevelOrig;
                iLastN2 += ( minGrid.iWidth - ( iLastN2 % minGrid.iWidth ) );

                if ( IsCheckMarker() )
                    {
                    // Check if we found next SOT or EOC
                    // it signals the end of one Precinct
                    marker = PtrReadUtil::ReadBigEndianUint16( iReader.iPtr );
                    if ( marker == KSOT || marker == KEOC )
                        {
                        compIncomplete = ETrue;
                        return compIncomplete;
                        }
                    }
                }

            iLastN2 = iTileCanvas.iTl.iX;
            iLastN1 += ( minGrid.iHeight - ( iLastN1 % minGrid.iHeight ) );
            }

        iLastN2 = iTileCanvas.iTl.iX;
        iLastN1 = iTileCanvas.iTl.iY;
        ++iLastComponent;
        }

    // Continue using the next POC
    ++iLastPOC;
    iDoLoadPOC = EFalse;

    return compIncomplete;
    }

