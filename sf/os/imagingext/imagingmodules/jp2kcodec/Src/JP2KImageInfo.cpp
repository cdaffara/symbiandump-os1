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
* Description:  CJ2kImageInfo class used to collect image related
*                information such as Main Header, SIZ marker and
*                list of tiles. It's also implement the
*                MJ2kPacketHeaderReader interface for reading the
*                packet header from PPM marker.
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
// CJ2kImageInfo::CJ2kImageInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CJ2kImageInfo::CJ2kImageInfo() : 
    iLastTilePartProcessed( -1 )
    {
    }

// Destructor
CJ2kImageInfo::~CJ2kImageInfo()
    {
    delete iTile;
    iTile= 0;

    delete iPpm;
    iPpm = 0;

    // iPpmBuffer not owned.    
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendCOCL
// Verify and append COC to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendCOCL( TCOCMarker *aMarker )
    {
    for ( TUint16 index = 0; index < iMainMarker.iCoc.Count(); ++index )
        {
        if ( iMainMarker.iCoc[index]->iCcoc == aMarker->iCcoc )
            {
            // No more than one COC per component
           delete aMarker;
           aMarker = 0;

           return;
            }
        }
    User::LeaveIfError( iMainMarker.iCoc.Append( aMarker ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendQCCL
// Verify and append QCC to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendQCCL( TQCCMarker *aMarker )
    {
    for ( TUint16 index = 0; index < iMainMarker.iQcc.Count(); ++index )
        {
        if ( iMainMarker.iQcc[index]->iCqcc == aMarker->iCqcc )
            {
            // No more than one QCC per component
            delete aMarker;
            aMarker = 0;

            return;
            }
        }
    User::LeaveIfError( iMainMarker.iQcc.Append( aMarker ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendRGNL
// Verify and append RGN to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendRGNL( TRGNMarker *aMarker )
    {
    for ( TUint16 index = 0; index < iMainMarker.iRgn.Count(); ++index )
        {
        if ( iMainMarker.iRgn[index]->iCrgn == aMarker->iCrgn )
            {
            // no more than one RGN per component
            delete aMarker;
            aMarker = 0;

            return;
            }
        }
    User::LeaveIfError( iMainMarker.iRgn.Append( aMarker ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendPOCL
// Verify and append POC to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendPOCL( TPOCMarker *aMarker )
    {
    if ( iMainMarker.iPoc == 0 )
        {
        iMainMarker.iPoc = aMarker;
        }
    else
        {
        // Appending the content of the POC to the
        // existing main header POC
        for ( TInt index = 0; index < aMarker->iPpoc.Count(); ++index )
            {
            User::LeaveIfError( iMainMarker.iPoc->iRSpoc.Append( aMarker->iRSpoc[index] ) );
            User::LeaveIfError( iMainMarker.iPoc->iCSpoc.Append( aMarker->iCSpoc[index] ) );
            User::LeaveIfError( iMainMarker.iPoc->iLYEpoc.Append( aMarker->iLYEpoc[index] ) );
            User::LeaveIfError( iMainMarker.iPoc->iREpoc.Append( aMarker->iREpoc[index] ) );
            User::LeaveIfError( iMainMarker.iPoc->iCEpoc.Append( aMarker->iCEpoc[index] ) );
            User::LeaveIfError( iMainMarker.iPoc->iPpoc.Append( aMarker->iPpoc[index] ) );
            }
        // At most one POC may exist in main header
        delete aMarker;
        aMarker = 0;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendCRGL
// Verify and append CRG to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendCRGL( TCRGMarker *aMarker )
    {
    if ( iMainMarker.iCrg == 0 )
        {
        iMainMarker.iCrg = aMarker;
        }
    else
        {
        // Appending the content of the CRG to the
        // existing main header CRG
        for ( TInt index = 0; index < aMarker->iXYcrg.Count(); ++index )
            {
            User::LeaveIfError( iMainMarker.iCrg->iXYcrg.Append( aMarker->iXYcrg[index] ) );
            }
        // At most one CRG may exist in main header
        delete aMarker;
        aMarker = 0;
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AppendCOML
// Verify and append COM to the main header
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AppendCOML( const TCOMMarker *aMarker )
    {
    User::LeaveIfError( iMainMarker.iCom.Append( aMarker ) );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::GetCodingStyle
// Retrieve the right Coding Style Marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::GetCodingStyle( TCODMarker*& aCod, TCOCMarker*& aCoc,
                                    const CJ2kTileInfo& aTile, TUint16 aComponentIndex )
    {
    // Tile part COC > tile part COD > main header COC > main header COD
    aCod = 0;
    aCoc = 0;
    TUint16 index = 0;
    TUint16 cocCount = 0;
    const TTileMarker &tileMarker = aTile.TileMarker();

    cocCount = (TUint16)tileMarker.iCoc.Count();
    if ( cocCount )
        {
        for ( index = 0; index < cocCount; ++index )
            {
            if ( tileMarker.iCoc[index]->iCcoc == aComponentIndex )
                {
                aCoc = tileMarker.iCoc[index];
                return;
                }
            }
        }
    if ( tileMarker.iCod )
        {
        aCod = tileMarker.iCod;
        return;
        }

    cocCount = (TUint16)iMainMarker.iCoc.Count();
    if ( cocCount )
        {
        for ( index = 0; index < cocCount; ++index )
            {
            if ( iMainMarker.iCoc[index]->iCcoc == aComponentIndex )
                {
                aCoc = iMainMarker.iCoc[index];
                return;
                }
            }
        }
    aCod = &iMainMarker.iCod;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::GetQuantization
// Retrieve the right Quantization Marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::GetQuantization( TQCDMarker*& aQcd, TQCCMarker*& aQcc,
                                     const CJ2kTileInfo& aTile, TUint16 aComponentIndex )
    {
    // Tile part QCC > tile part QCD > main header QCC > main header QCD
    aQcd = 0;
    aQcc = 0;
    TUint16 index = 0;
    TUint16 qccCount = 0;
    const TTileMarker &tileMarker = aTile.TileMarker();

    qccCount = (TUint16)tileMarker.iQcc.Count();
    if ( qccCount )
        {
        for ( index = 0; index < qccCount; ++index )
            {
            if ( tileMarker.iQcc[index]->iCqcc == aComponentIndex )
                {
                aQcc = tileMarker.iQcc[index];
                return;
                }
            }
        }
    if ( tileMarker.iQcd )
        {
        aQcd = tileMarker.iQcd;
        return;
        }

    qccCount = (TUint16)iMainMarker.iQcc.Count();
    if ( qccCount )
        {
        for ( index = 0; index < qccCount; ++index )
            {
            if ( iMainMarker.iQcc[index]->iCqcc == aComponentIndex )
                {
                aQcc = iMainMarker.iQcc[index];
                return;
                }
            }
        }
    aQcd = &iMainMarker.iQcd;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::GetRegion
// Retrieve the right Region of Interest Marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::GetRegion( TRGNMarker*& aRgn, const CJ2kTileInfo& aTile, TUint16 aComponentIndex ) const
    {
    // Tile part RGN > main header RGN
    aRgn = 0;
    TUint16 index = 0;
    TUint16 rgnCount = 0;
    const TTileMarker &tileMarker = aTile.TileMarker();

    rgnCount = (TUint16)tileMarker.iRgn.Count();
    if ( rgnCount )
        {
        for ( index = 0; index < rgnCount; ++index )
            {
            if ( tileMarker.iRgn[index]->iCrgn == aComponentIndex )
                {
                aRgn = tileMarker.iRgn[index];
                return;
                }
            }
        }

    rgnCount = (TUint16)iMainMarker.iRgn.Count();
    if ( rgnCount )
        {
        for ( index = 0; index < rgnCount; ++index )
            {
            if ( iMainMarker.iRgn[index]->iCrgn == aComponentIndex )
                {
                aRgn = iMainMarker.iRgn[index];
                return;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::GetFromTLM
// Retrieve the tile length field from TLM marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::GetFromTLM( TSotMarker& aSotMarker ) const
    {
    TTLMMarker* tlm = 0;
    TUint16     tileIndex = 0;
    TUint16     dIndex = 0;

    for ( TInt index = 0; index < iMainMarker.iTlm.Count(); ++index )
        {
        tlm = iMainMarker.iTlm[index];
        dIndex = 0;
        if ( tlm->iTtlm.Count() == 0 )
            {
            // One tile-part per tile and the tiles are
            // in index order without omission or repetition
            while ( dIndex < tlm->iPtlm.Count() )
                {
                if ( dIndex == aSotMarker.iIsot )
                    {
                    aSotMarker.iPsot = tlm->iPtlm[dIndex];
                    return;
                    }
                ++dIndex;
                }
            }
        else
            {
            while ( dIndex < tlm->iTtlm.Count() )
                {
                // Looking for the right tile
                if ( tlm->iTtlm[dIndex] == aSotMarker.iIsot )
                    {
                    // Looking for the right tile-part
                    if ( tileIndex == aSotMarker.iTPsot )
                        {
                        aSotMarker.iPsot = tlm->iPtlm[dIndex];
                        return;
                        }
                    ++tileIndex;
                    }
                ++dIndex;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::UsePPM
// Set up to read the packet header from the PPM marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::UsePPM( CJ2kTileInfo& aTile )
    {
    if ( iPpm->iPtr == iPpm->iPtrEnd )
        {
        iPpm->iPtr = 4;
        iPpm->iPtrEnd = iPpm->iPtr + iMainMarker.iPpm[iLastTilePartProcessed]->iNppm;
        iPpmBuffer = iMainMarker.iPpm[iLastTilePartProcessed]->iIppm;
        }
    aTile.SetPacketHeaderReader( this );
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::DoCompactMainHeaderL
// Re-arrange the PPM and PLM buffers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::DoCompactMainHeaderL()
    {
    RPointerArray<TPLMMarker>& plmList = iMainMarker.iPlm;
    if ( plmList.Count() )
        {
        TPLMMarker *plmMarker = 0;
        TPLMMarker *prevMarker = 0;
        TUint8 checkNplm = ETrue;
        TUint8 continuation = EFalse;
        TUint8 length = 0;
        const TUint8 *ptr = 0;
        TUint32 remainder = 0;
        TInt index = 0;

        for ( index = 0; index < plmList.Count(); ++index )
            {
            plmMarker = plmList[index];
            if ( checkNplm )
                {
                ptr = plmMarker->iIplm->Ptr();
                length = *ptr;
                if ( length != ( TUint8 )( plmMarker->iNplm - 1 ) )
                    {
                    // Incomplete PLM marker
                    // there is a continuation of data in the next PLM marker
                    // and concatenation is needed
                    plmMarker->iIplm = plmMarker->iIplm->ReAllocL( length + 1 );
                    remainder = (TUint32)( length - ( plmMarker->iNplm - 1 ) );
                    plmMarker->iNplm = length;
                    prevMarker = plmMarker;
                    checkNplm = EFalse;
                    }
                else
                    {
                    // It is a complete PLM marker
                    plmMarker->iNplm = length;
                    }
                }
            else
                {
                // Concatenate with the previous PLM marker
                continuation = ETrue;
                TPtr8 previous = prevMarker->iIplm->Des();
                previous.Append( plmMarker->iIplm->Ptr(), plmMarker->iNplm );
                remainder = (TUint32)( remainder - plmMarker->iNplm );
                plmMarker->iNplm = 0;
                if ( remainder == 0 )
                    {
                    checkNplm = ETrue;
                    }
                }
            }
        if ( continuation )
            {
            index = 0;
            while ( index < plmList.Count() )
                {
                plmMarker = plmList[index];
                if ( plmMarker->iNplm )
                    {
                    ++index;
                    }
                else
                    {
                    // Remove PPM marker which has been concatenated
                    plmList.Remove( index );
                    delete plmMarker;
                    }
                }
            }
        // Do not use PLM segment.
        plmList.ResetAndDestroy();
        }

    RPointerArray<TPPMMarker>& ppmList = iMainMarker.iPpm;
    if ( ppmList.Count() > 0 )
        {
        iPpm = new ( ELeave ) TPPMStream;
        Mem::FillZ( iPpm, sizeof( TPPMStream ) );

        TPPMMarker *ppmMarker = 0;
        TPPMMarker *prevMarker = 0;
        TUint8  checkNppm = ETrue;
        TUint8  continuation = EFalse;
        const TUint8 *ptr = 0;
        TUint32 length = 0;
        TUint32 remainder = 0;
        TInt index = 0;
        TUint8 zppm = 0;

        for ( index = 0; index < ppmList.Count(); ++index )
            {
            ppmMarker = ppmList[index];
            if ( checkNppm )
                {
                ptr = ppmMarker->iIppm->Ptr();
                length = PtrReadUtil::ReadBigEndianUint32( ptr );
                if ( length != (TUint32)( ppmMarker->iNppm - 4 ) )
                    {
                    if ( length > (TUint32)( ppmMarker->iNppm - 4 ) )
                        {
                        // Incomplete PPM marker
                        // there is a continuation of data in the next PPM marker
                        // and concatenation is needed
                        ppmMarker->iZppm = zppm++;
                        ppmMarker->iIppm = ppmMarker->iIppm->ReAllocL( length + 4 );
                        remainder = (TUint32)( length - ( ppmMarker->iNppm - 4 ) );
                        ppmMarker->iNppm = length;
                        prevMarker = ppmMarker;
                        checkNppm = EFalse;
                        }
                    else
                        {
                        // We should split the PPM marker into smaller PPM marker
                        TInt location = 0;
                        ppmList.Remove( index );
                        remainder = ppmMarker->iNppm;
                        while ( remainder >= ( length + 4 ) )
                            {
                            prevMarker = new ( ELeave ) TPPMMarker;
                            CleanupStack::PushL( prevMarker );

                            prevMarker->iZppm = ( location ? (TUint8)0 : zppm++ );
                            prevMarker->iNppm = length + ( location ? 4 : 0 );
                            prevMarker->iIppm = HBufC8::NewL( length + 4 );
                            prevMarker->iIppm->Des().Append( ptr, length + 4 );
                            User::LeaveIfError( ppmList.Insert( prevMarker, index + location ) );
                            CleanupStack::Pop();
                            ++location;
                            ptr += ( length + 4 );
                            remainder -= ( length + 4 );
                            if ( remainder > 4 )
                                {
                                length = PtrReadUtil::ReadBigEndianUint32( ptr );
                                }
                            else
                                {
                                length = 0;
                                }
                            }
                        if ( remainder )
                            {
                            // Incomplete PPM marker
                            prevMarker = new ( ELeave ) TPPMMarker;
                            CleanupStack::PushL( prevMarker );

                            prevMarker->iNppm = remainder;
                            prevMarker->iIppm = HBufC8::NewL( remainder );
                            prevMarker->iIppm->Des().Append( ptr, remainder );
                            User::LeaveIfError( ppmList.Insert( prevMarker, index + location ) );
                            CleanupStack::Pop();
                            }
                        delete ppmMarker;
                        ppmMarker = 0;
                        }
                    }
                else
                    {
                    // It is a complete PPM marker, adjust the Nppm to length
                    ppmMarker->iZppm = zppm++;
                    ppmMarker->iNppm = length;
                    }
                }
            else
                {
                continuation = ETrue;
                if ( ppmMarker->iNppm <= remainder )
                    {
                    // Concatenate with the previous PPM marker
                    TPtr8 previous = prevMarker->iIppm->Des();
                    previous.Append( ppmMarker->iIppm->Ptr(), ppmMarker->iNppm );
                    remainder = ( TUint32 )( remainder - ppmMarker->iNppm );
                    ppmMarker->iNppm = 0;
                    if ( remainder == 0 )
                        {
                        checkNppm = ETrue;
                        }
                    }
                else
                    {
                    checkNppm = ETrue;
                    // Partially concatenate with the previous PPM marker
                    TPtr8 previous = prevMarker->iIppm->Des();
                    previous.Append( ppmMarker->iIppm->Ptr(), remainder );

                    // We should split the PPM marker into smaller PPM marker
                    TInt location = 0;
                    ppmList.Remove( index );
                    ptr = ppmMarker->iIppm->Ptr() + remainder;
                    length = PtrReadUtil::ReadBigEndianUint32( ptr );
                    remainder = ( TUint32 )( ppmMarker->iNppm - remainder );
                    while ( remainder >= ( length + 4 ) )
                        {
                        prevMarker = new ( ELeave ) TPPMMarker;
                        CleanupStack::PushL( prevMarker );

                        prevMarker->iZppm = ( location ? (TUint8)0 : zppm++ );
                        prevMarker->iNppm = length + ( location ? 4 : 0 );
                        prevMarker->iIppm = HBufC8::NewL( length + 4 );
                        prevMarker->iIppm->Des().Append( ptr, length + 4 );
                        User::LeaveIfError( ppmList.Insert( prevMarker, index + location ) );
                        CleanupStack::Pop();
                        ++location;
                        ptr += ( length + 4 );
                        remainder -= ( length + 4 );
                        if ( remainder > 4 )
                            {
                            length = PtrReadUtil::ReadBigEndianUint32( ptr );
                            }
                        else
                            {
                            length = 0;
                            }
                        }
                    if ( remainder )
                        {
                        // Incomplete PPM marker
                        prevMarker = new ( ELeave ) TPPMMarker;
                        CleanupStack::PushL( prevMarker );

                        prevMarker->iNppm = remainder;
                        prevMarker->iIppm = HBufC8::NewL( remainder );
                        prevMarker->iIppm->Des().Append( ptr, remainder );
                        User::LeaveIfError( ppmList.Insert( prevMarker, index + location ) );
                        CleanupStack::Pop();
                        }
                    delete ppmMarker;
                    ppmMarker = 0; 
                    }
                }
            }
        if ( continuation )
            {
            index = 0;
            while ( index < ppmList.Count() )
                {
                ppmMarker = ppmList[index];
                if ( ppmMarker->iNppm )
                    {
                    ++index;
                    }
                else
                    {
                    // Remove PPM marker which has been concatenated
                    ppmList.Remove( index );
                    delete ppmMarker;
                    ppmMarker = 0;
                    }
                }
            }
        }

    // Do not use the COM segment at this release
    if ( iMainMarker.iCom.Count() )
        {
        iMainMarker.iCom.ResetAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::IsPPM
// Is packet header should be read from PPM marker
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::IsPPM() const
    {
    if ( iPpm )
        {
        if ( iPpm->iPtr < iPpm->iPtrEnd )
            {
            return ETrue;
            }
        else if ( iLastTilePartProcessed < iMainMarker.iPpm.Count() )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ReadEPHMarker
// Try to consume the EPH marker if there is one.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::ReadEPHMarker()
    {
    if ( iPpm && ( iPpm->iPtrEnd - iPpm->iPtr ) >= 2 )
        {
        const TUint8 *ptr = iPpmBuffer->Ptr();
        ptr += iPpm->iPtr;
        if ( PtrReadUtil::ReadBigEndianUint16( ptr ) == KEPH )
            {
            iPpm->iPtr += 2;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ReadBit
// Read a bit from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::ReadBit( TUint8& aBit )
    {
    if ( iPpm->iPos == 0 )
        {
        if ( iPpm->iPtr < iPpm->iPtrEnd )
            {
            // Stream contains some packet header data
            iPpm->iData = ( *iPpmBuffer )[iPpm->iPtr++];
            iPpm->iPos = iPpm->iNextPos;
            if ( iPpm->iNextPos == 0x08 && iPpm->iData == 0xff )
                {
                iPpm->iNextPos = 0x07;
                }
            else
                {
                iPpm->iNextPos = 0x08;
                }
            }
        else
            {
            // No more data
            return ETrue;
            }
        }
    aBit = (TUint8)( ( iPpm->iData >> ( --iPpm->iPos ) ) & 0x01 );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::ReadBits( TUint8& aBit, TUint8 aBitLen )
    {
    aBit = (TUint8)0;
    TUint8 bit;
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
// CJ2kImageInfo::ReadBits
// Read some bits from the packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::ReadBits( TUint32& aBit, TUint8 aBitLen )
    {
    aBit = (TUint32)0;
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
// CJ2kImageInfo::StartReadBit
// Start reading from packet header stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint8 CJ2kImageInfo::StartReadBit()
    {
    if ( iPpm && iPpm->iPtr < iPpm->iPtrEnd )
        {
        // Stream contains some packet header data
        iPpm->iData = ( *iPpmBuffer )[iPpm->iPtr++];
        iPpm->iPos = iPpm->iNextPos = 0x08;
        if ( iPpm->iData == 0xff )
            {
            iPpm->iNextPos = 0x07;
            }

        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CJ2kImageInfo::AlignReader
// Align the stream to the next byte boundary if necessary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CJ2kImageInfo::AlignReader()
    {
    if ( iPpm->iNextPos == 0x07 )
        {
        if ( iPpm->iPtr < iPpm->iPtrEnd )
            {
            ++iPpm->iPtr;
            }
        }
    iPpm->iData = iPpm->iPos = iPpm->iNextPos = 0;
    }

