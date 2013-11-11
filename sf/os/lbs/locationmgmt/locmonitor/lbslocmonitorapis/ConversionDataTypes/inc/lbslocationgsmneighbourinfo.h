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
* cell information in a GSM PLMN. This class may also optionally contain
* neighbouring cell information
*
*/

/**
@file
@publishedPartner
@prototype
*/


#ifndef LBS_GSM_NEIGHB_CELL_INFO_H
#define LBS_GSM_NEIGHB_CELL_INFO_H

#include <e32std.h>
#include <lbs/lbslocationcommon.h>


/** 
 * This class contains information related to a GSM neighbouring cell 
 * information.
 */
NONSHARABLE_CLASS(TLbsGsmNeighbourCellInfo)
    {

public:

    /**
     * Allocates and constructs a new neighbour GSM cell info object. In the 
     * returned instance all data is marked as undefined
     *
     * @return Pointer to the new neighbour GSM cell info object.
     */
    IMPORT_C TLbsGsmNeighbourCellInfo();
    
    /**
     * Allocates and constructs a new neighbour GSM cell info object. 

     * @param[in] aARFCN Absolute Radio Frequency Channel Number
     * @param[in] aBSIC  Base station identity (colour) code.
     * @param[in] aRxLev Rx level.
     *
     * @return Pointer to the new neighbour GSM cell info object.
     */
    IMPORT_C TLbsGsmNeighbourCellInfo( const TInt aARFCN,
                                       const TInt aBSIC,
                                       const TInt aRxLev );
        
    /**
     *Sets the Absolute Radio Frequency Channel Number.
     *
     * @param[in] aARFCN Absolute Radio Frequency Channel Number, range 0..1023.
     */
    IMPORT_C void SetArfcn(const TInt aARFCN);
    
    /**
     * Retrieves the Absolute Radio Frequency Channel Number.
     *
     * @return Absolute Radio Frequency Channel Number, range 0..1023.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt Arfcn() const;
    
    /**
     * Sets the Base station identity (colour) code.
     *
     * @param[in] iBSIC Base station identity code, range 0..63.
     */    
    IMPORT_C void SetBsic( const TInt aBSIC );
    
    /**
     * Retrieves the Base station identity code.
     *
     * @return Base station identity code, range 0..63.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt Bsic() const;

    /**
     * Sets the Rx level.
     *
     * @param[in] aRxLev Rx Level, range 0..63. 
     */    
    IMPORT_C void SetRxLevel( const TInt aRxLev );  
    
    /**
     * Retrieves the Rx level.
     *
     * @return Rx Level, range 0..63. 
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt RxLevel() const;
   
protected: // attributes

    /** 
     * Absolute Radio Frequency Channel Number 
     */ 
    TInt16      iARFCN;

    /** 
     * Base station identity code 
     */ 
    TInt8       iBSIC;

    /** 
     * Rx level 
     */ 
    TInt8       iRxLev;

    /** 
     * Reserved 
     */
    TUint8      iReserved[4];
    
    };

typedef RArray<TLbsGsmNeighbourCellInfo> RLbsGsmNeighbourCellInfoArray;


#endif //LBS_GSM_NEIGHB_CELL_INFO_H
