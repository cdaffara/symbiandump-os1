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
* Description:  J2kComponentInfo class used to collect component related
*                information such as precincts size at each resolution level and
*                list of subbands.
*
*/


#ifndef __JP2KCOMPONENTINFO_INL__
#define __JP2KCOMPONENTINFO_INL__

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::Levels
// Get the resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::Levels() const
    {
    return iNumOfLevels;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::CodeBlockStyle
// Get the codeblock style
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::CodeBlockStyle() const
    {
    return iCodeBlockStyle;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::NumGuard
// Get the number of guard
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::NumGuard() const
    {
    return (TUint8)( ( iQc & 0xe0 ) >> 5 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::QuantizationStyle
// Get the quantization style
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::QuantizationStyle() const
    {
    return (TUint8)( iQc & 0x1f );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::RoiShift
// Get the region of interest shifting
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::RoiShift() const
    {
    return iRoiShift;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::CodeBlockSize
// Get the codeblock size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TSize& CJ2kComponentInfo::CodeBlockSize() const
    {
    return iCodeBlockSiz;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::ComponentCanvas
// Get the canvas of the component
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TRect& CJ2kComponentInfo::ComponentCanvas() const
    {
    return iComponentCanvas;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::PrecinctSizeAt
// Get the precinct size at specific resolution level
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TSize& CJ2kComponentInfo::PrecinctSizeAt( TUint8 aResLevel ) const
    {
    return iPrecinctSizeList[aResLevel];
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::MinGrid
// Get the minimum grid size
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline const TSize& CJ2kComponentInfo::MinGrid() const
    {
    return iMinGrid;
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::IsSOPMarkerUsed
// Is coding style shows SOP marker may exists in codestream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::IsSOPMarkerUsed() const
    {
    return (TUint8)( iCod & 0x02 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::IsEPHMarkerUsed
// Is coding style shows EPH marker may exists in codestream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::IsEPHMarkerUsed() const
    {
    return (TUint8)( iCod & 0x04 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::IsACByPass
// Is codeblock style shows AC bypass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::IsACByPass() const
    {
    return (TUint8)( iCodeBlockStyle & 0x01 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::IsTermination
// Is codeblock style shows termination
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::IsTermination() const
    {
    return (TUint8)( iCodeBlockStyle & 0x04 );
    }

// -----------------------------------------------------------------------------
// CJ2kComponentInfo::IsReversible
// Is transformation reversible
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TUint8 CJ2kComponentInfo::IsReversible() const
    {
    return (TUint8)( iWaveletTransformation & 0x01 );
    }

#endif // __JP2KCOMPONENTINFO_INL__
