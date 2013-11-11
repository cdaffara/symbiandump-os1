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
* information in a GSM PLMN. 
*
*/


#include <lbs/lbslocationgsmneighbourinfo.h>


// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::TLbsGsmNeighbourCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsGsmNeighbourCellInfo::TLbsGsmNeighbourCellInfo()
    {
    iARFCN = -1;
    iBSIC = -1;
    iRxLev = -1;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::TLbsGsmNeighbourCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TLbsGsmNeighbourCellInfo::TLbsGsmNeighbourCellInfo( const TInt aARFCN,
                                                             const TInt aBSIC,
                                                             const TInt aRxLev )
    {
    iARFCN = aARFCN;
    iBSIC = aBSIC;
    iRxLev = aRxLev;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::SetArfcn()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C void TLbsGsmNeighbourCellInfo::SetArfcn( const TInt aARFCN )
    {
    iARFCN = aARFCN;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::TLbsGsmNeighbourCellInfo()
// other items were commented in the header
// ---------------------------------------------------------------------------
//  
EXPORT_C TInt TLbsGsmNeighbourCellInfo::Arfcn() const
    {
    return iARFCN;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::SetBsic()
// other items were commented in the header
// ---------------------------------------------------------------------------
//  
EXPORT_C void TLbsGsmNeighbourCellInfo::SetBsic( const TInt aBSIC )
    {
    iBSIC = aBSIC;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::Bsic()
// other items were commented in the header
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLbsGsmNeighbourCellInfo::Bsic() const
    {
    return iBSIC;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::SetRxLevel()
// other items were commented in the header
// ---------------------------------------------------------------------------
//  
EXPORT_C void TLbsGsmNeighbourCellInfo::SetRxLevel( const TInt aRxLev )
    {
    iRxLev = aRxLev;
    }

// ---------------------------------------------------------------------------
// TLbsGsmNeighbourCellInfo::RxLevel()
// other items were commented in the header
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt TLbsGsmNeighbourCellInfo::RxLevel() const
    {
    return iRxLev;
    }
