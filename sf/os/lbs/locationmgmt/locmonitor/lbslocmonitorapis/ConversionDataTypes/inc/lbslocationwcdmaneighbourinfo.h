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
* Description: Data class representing a geographical area in terms of 
* cell information in a Wcdma PLMN. This class may also optionally contain
* neighbouring cell information
*
*/

/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBS_WCDMA_NEIGHB_CELL_INFO_H
#define LBS_WCDMA_NEIGHB_CELL_INFO_H


#include <e32std.h>
#include <lbs/lbslocationcommon.h>


/** 
 * This class contains information related to a Wcdma neighbouring cell information
 */
NONSHARABLE_CLASS(TLbsWcdmaNeighbourCellInfo)
    {

public:

    /**
     * Allocates and constructs a new neighbour Wcdma cell info object. In the returned
     * instance all data is marked as undefined
     *
     * @return Pointer to the new neighbour Wcdma cell info object.
     */
    IMPORT_C TLbsWcdmaNeighbourCellInfo();
    
    /**
     * Allocates and constructs a new neighbour Wcdma cell info object. 
     *
     * @param[in] aUCid Cell Identity
     * @param[in] aSCode Scrambling Code
     * @param[in] aPathloss Pathloss
     * @param[in] Received Signal Strength Indicator
     *
     * @return Pointer to the new neighbour Wcdma cell info object.
     */
    IMPORT_C TLbsWcdmaNeighbourCellInfo(const TInt aUCid,
                                        const TInt aSCode,
                                        const TInt aPathloss,
                                        const TInt aRssi );
        
    /**
     * Sets the Cell Identity.
     *
     * @param[in] aUniqueCellId Cell Identity, range 0..268435455.
     */
    IMPORT_C void SetUniqueCellId( const TInt aUniqueCellId );
    
    /**
     * Retrieves the Cell Identity.
     *
     * @return Cell Identity, range 0..268435455.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt UniqueCellId() const; 
    
    /**
     * Sets the scrambling code 
     *
     * @param[in] aScramblingCode scrambling code , range 0..511.
     */    
    IMPORT_C void SetScramblingCode( const TInt aScramblingCode );
    
    /**
     * Retrieves the scrambling code .
     *
     * @return Base scrambling code, range 0..511.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt ScramblingCode() const;
    
    /**
     * Sets the Pathloss parameter 
     *
     * @param[in] aPathloss Pathloss, range 46..173.
     */    
    IMPORT_C void SetPathloss( const TInt aPathloss );
    
    /**
     * Retrieves the pathloss information.
     *
     * @return pathloss, range 46..173.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt Pathloss() const;
    
    /**
     * Sets the RSSI parameter 
     *
     * @param[in] aRssi Received Signal Strength Indicator, range 0..127.
     */    
    IMPORT_C void SetSignalStrength( const TInt aRssi );
    
    /**
     * Retrieves the RSSI information.
     *
     * @return RSSI, range 0..127.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt SignalStrength() const;    
    
protected: // attributes

    /** 
     * Cell Identity 
     */ 
    TInt32      iUniqueCellId;

    /** 
     * Scrambling Code 
     */ 
    TInt16       iScramblingCode;
    
    /**
     * Path loss 
     */
    TInt16        iPathloss;
    
    /**
     * RSSI
     */
    TInt16       iRssi;
    
    /** 
     * Reserved 
     */
    TUint8      iReserved[8];
    
    };

typedef RArray<TLbsWcdmaNeighbourCellInfo> RLbsWcdmaNeighbourCellInfoArray;


#endif //LBS_WCDMA_NEIGHB_CELL_INFO_H
