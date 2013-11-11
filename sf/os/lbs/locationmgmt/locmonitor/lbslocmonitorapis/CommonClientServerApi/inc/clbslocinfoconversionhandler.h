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
* Description: Declaration of CLbsLocInfoConversionHandler class.
*
*/
#ifndef C_LBSLOCINFOCONVERSIONHANDLER_H_
#define C_LBSLOCINFOCONVERSIONHANDLER_H_

#include <e32base.h>

// Forward declaration
class RLbsLocInfoConverter;
class CLbsLocationInfo;

/**
 * Class waits for location information completion event and 
 * notifies the observer on completion. 
 */
class CLbsLocInfoConversionHandler : public CActive
    {
public:
    enum TAyncRequestType
        {
        EGetBuffersizeForSingleArea,
        
        EGetSingleConvertedArea,
        
        EGetBuffersizeForMultipleArea,
                
        EGetMultipleConvertedArea
        };
public:
    /**
     * Allocates and construct a new CLbsLocInfoConversionHandler object
     */
    static CLbsLocInfoConversionHandler* NewL( RLbsLocInfoConverter& aLocInfoConverter );
    
    /**
     * Destructor.
     */
    ~CLbsLocInfoConversionHandler();
    
    /**
     * Starts the asyncronous operation by setting the iStatus to active.
     */
    void Start( TAyncRequestType aType);
    
    /**
     * Adds the buffer into the object.
     */
    void SetBuffer( CBufFlat* aBuffer );
    
    /**
     * Stores the client's status.
     */
    void SetStatus( TRequestStatus& aStatus );
    
    /**
     * Adds client specified requested info.
     */
    void SetClientRequestedInfo( TLbsConversionOutputInfoMask aRequestedInfo );
    
    /**
     * Sets client's location info array.
     */
    void SetClientLocationInfoArray( RLbsLocationInfoBaseArray*
                                     aLocationInfoArray );
    
    /**
     * Sets client's location info object.
     */
    void SetClientLocationInfo( CLbsLocationInfoBase& aLocationInfo );
    
    /**
     * Releases all the resources used by earlier request.
     */
    void ReleaseAllResources();
    
    
protected: // from CActive
    void RunL();
    void DoCancel();
    
private: 
    CLbsLocInfoConversionHandler( RLbsLocInfoConverter& aLocInfoConverter );
    
    void ConstructL();
    
    void HandleSingleConversionL();
    
    
    void HandleMultipleConversionL();
    
    void GetPositionInfoArrayL( RBufReadStream& readStream, 
                                CLbsLocationInfo& aDesLocationInfo );
    
    void GetGsmCellInfoArrayL( RBufReadStream& readStream, 
                               CLbsLocationInfo& aDesLocationInfo);
    
    void GetWcdmaCellInfoArrayL( RBufReadStream& readStream, 
                                 CLbsLocationInfo& aDesLocationInfo );

    void GetWlanInfoArrayL( RBufReadStream& readStream, 
                            CLbsLocationInfo& aDesLocationInfo );

    
private: // Member variables
    /**
     * Handle to loc info converter subsession.
     */
    RLbsLocInfoConverter&  iLocInfoConverter;
    
    /**
     * Pointer to buffer
     */
    CBufFlat* iBuffer; 
    
    /**
     * Pointer to client's TRequestStatus
     */
    TRequestStatus*  iClientStatus;
    
    /**
     * Pointer to client's location info list
     * Not Own.
     */
    RLbsLocationInfoBaseArray* iClientLocationInfoList;
    
    /**
     * Pointer to client's location info
     */
    CLbsLocationInfoBase* iClientLocationInfo;
    
    /**
     * Client specified request info
     */
    TLbsConversionOutputInfoMask iRequestedInfo;
    
    /**
     * Type of async operation requested 
     */ 
    TAyncRequestType iRequestType;
    };

#endif /*C_LBSLOCINFOCONVERSIONHANDLER_H_*/
