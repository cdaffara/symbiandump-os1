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
* Description:
*
*/


#ifndef R_LBSLOCINFOCONVERTER_H_
#define R_LBSLOCINFOCONVERTER_H_


#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationinfoconvertercommon.h>
#include "rlbslocmonitorsession.h"
#include "rsecuresessionbase.h"


// Forward declaration
class CLbsLocInfoConversionHandler;
class CLbsLocMonitorPtrHolder;
struct TLbsClientPrefs;


/**
 *  
 * This class provides services pertaining to conversion of geographical
 * area information represented as a collection of GSM/WCDMA or WLAN 
 * information to corresponding geographical coordinates. This class is
 * used to create sub-session with the location monitor server. 
 *
 * Before using the class, a primary connection must have already been
 * established with the server with RLbsLocMonitorSession::Connect().
 *
 */
class RLbsLocInfoConverter : public RSecureSubSessionBase
    {
public:

    /**
     * C++ constructor
     */
    IMPORT_C RLbsLocInfoConverter();
    
    /**
     * Destructor
     */
    IMPORT_C ~RLbsLocInfoConverter();
    
    /**
     * Opens a subsession with Location Monitor Server.
     *
     * A subsession must be opened before any other service can be used.
     * 
     * @param[in] aServer Reference to the Location Monitor Server 
     * session.
     *
     * @return KErrNone if successful. Otherwise, Symbian standard 
     * error code is returned, such as KErrNoMemory, KErrServerBusy, etc.
     */
    IMPORT_C void OpenL( RLbsLocMonitorSession& aLocMonSession );
    
    /**
     * Closes the subsession with Location Monitor Server.
     */
    IMPORT_C void Close();
    
    /**
     * Converts one area info to position info
     * @param[in] aLocationInfo location information corresponding to 
     * a geographical location is passed and on successful completion of this 
     * method,position information corresponding to the location information 
     * is returned in the observer callback.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     * @param[in] aRequestedInfo Location information that is requested. 
     * @param[in] aConverterModuleId Module ID of the converter plugin to be
     * used
     * @param[out] aStatus Contains the error code when the request is completed.
     *
     * @leave KErrArgument If the location info pointer is NULL or one of system
     * wide error codes.     
     */
     IMPORT_C void ConvertLocationInfoL( CLbsLocationInfoBase& aLocationInfo,
                                         TLbsConversionPrefs aConversionPrefs, 
                                         const TLbsConversionOutputInfoMask& aRequestedInfo,
                                         const TUid aConverterModuleId,
                                         TRequestStatus& aStatus );
    
    /**
     * Converts the area informations of a group of geographical locations
     * to its corresponding position information.
     *
     * @param[in] aLocationInfoArray Array of Location information to be 
     * converted.
     * @param[in] aConversionPrefs Conversion preferences suggested by client.
     * @param[in] aRequestedInfo Location information that is requested.
     * @param[in] aConversionModuleId Module ID of the converter plugin to be
     * used  
     * @param[out] aStatus Contains the error code when the request is completed.
     * 
     * @leave KErrArgument If the location info array is empty or if array count 
     * exceeds KPositionMaxBatchSize or one of system wide error codes.
     */
    IMPORT_C void ConvertLocationInfoL( RLbsLocationInfoBaseArray& aLocationInfoArray,
                                        TLbsConversionPrefs aConversionPrefs,
                                        TLbsConversionOutputInfoMask aRequestedInfo,
                                        const TUid aConverterModuleId,
                                        TRequestStatus& aStatus );
    
    /**
     * Cancel outstanding request to convert location information.
     */
    IMPORT_C void CancelConvertLocationInfo();
    
    /**
     * Handles buffer size passed from the server to store single converted 
     * location info. 
     */
    void HandleSingleConversionBufferSizeL(); 

    /**
     * Handles buffer size passed from the server to store multiple converted 
     * location info. 
     */
    void HandleMultipleConversionBufferSizeL(); 
    
    /**
     * Validates location information.
     */
    void ValidateLocationInfoL( CLbsLocationInfoBase* aLocationInfo );
    

private:
    /**
     * Symbian 2 phase constructor
     */
    void ConstructL();

    
private: // Member variables
    /**
     * Pointer to location info conversion handler
     */
    CLbsLocInfoConversionHandler* iConversionHandler;
    
    /** 
     * A pointer to a container that holds pointer descriptors, needed to point
     * to the clients request data during asynchronous requests 
     */
    CLbsLocMonitorPtrHolder* iPtrHolder;
    
    /**
     * Size of the buffer required to receive information
     * from server.
     */
    TInt iBufferSize;
    
    /**
     * Client preferences
     */
    TLbsClientPrefs* iClientPrefs;
    };

#endif /*R_LBSLOCINFOCONVERTER_H_*/
