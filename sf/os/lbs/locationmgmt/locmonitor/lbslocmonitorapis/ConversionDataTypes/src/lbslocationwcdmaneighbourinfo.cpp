/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Data class representing a neighbouring cell
* information in a WCDMA PLMN. 
*
*/


#include <lbs/lbslocationwcdmaneighbourinfo.h>


// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::TLbsWcdmaNeighbourCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsWcdmaNeighbourCellInfo::TLbsWcdmaNeighbourCellInfo()
    {
    iUniqueCellId = -1;
    iScramblingCode = -1;
    iPathloss = -1;
    iRssi = -1;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::TLbsWcdmaNeighbourCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsWcdmaNeighbourCellInfo::TLbsWcdmaNeighbourCellInfo( const TInt aUCid,
                                                                 const TInt aSCode,
                                                                 const TInt aPathloss,
                                                                 const TInt aRssi )
    {
    iUniqueCellId = aUCid;
    iScramblingCode = aSCode;
    iPathloss = aPathloss;
    iRssi = aRssi;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::SetUniqueCellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void TLbsWcdmaNeighbourCellInfo::SetUniqueCellId( const TInt aUniqueCellId )
    {
    iUniqueCellId = aUniqueCellId;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::UniqueCellId()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLbsWcdmaNeighbourCellInfo::UniqueCellId() const
    {
    return iUniqueCellId;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::SetScramblingCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void TLbsWcdmaNeighbourCellInfo::SetScramblingCode( 
                                          const TInt aScramblingCode )
    {
    iScramblingCode = aScramblingCode;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::ScramblingCode()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLbsWcdmaNeighbourCellInfo::ScramblingCode() const 
    {
    return iScramblingCode;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::SetPathloss()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void TLbsWcdmaNeighbourCellInfo::SetPathloss( const TInt aPathloss )
    {
    iPathloss = aPathloss;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::Pathloss()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLbsWcdmaNeighbourCellInfo::Pathloss() const 
    {
    return iPathloss;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::SetSignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void TLbsWcdmaNeighbourCellInfo::SetSignalStrength( const TInt aRssi )
    {
    iRssi = aRssi;
    }

// ---------------------------------------------------------------------------
// TLbsWcdmaNeighbourCellInfo::SignalStrength()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLbsWcdmaNeighbourCellInfo::SignalStrength() const 
    {
    return iRssi;
    }
