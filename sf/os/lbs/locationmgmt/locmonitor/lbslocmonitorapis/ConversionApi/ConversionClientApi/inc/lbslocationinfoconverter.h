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
* Description: Class used by client applications to convert location 
* information
*
*/

/**
@file
@publishedPartner
@prototype
*/

#ifndef LBS_LOCATION_INFO_CONVERTER_H
#define LBS_LOCATION_INFO_CONVERTER_H


// OS wide
#include <e32base.h>

// LBS wide
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationinfoconvertercommon.h>

class CLbsLocationInfoConverterImpl;


/**
* The MLbsLocationInfoConverterObserver class is used in partnership with
* CLbsLocationInfoConverter. The observer interface is used by the LBS
* subsystem to inform a client application when a Locality conversion
* process initiated through the CLbsLocationInfoConverter class has been
* finished. In OnResolveLocationComplete the results will be returned 
* as an argument of the function, while OnConversionComplete indicates that the results
* have been written in to the data that was passed in using ConvertLocationInfoL.
*
* Although all the interface methods in the MLbsLocationInfoConverterObserver 
* class are synchronous, the client application should ensure they return quickly
* (e.g. any extensive processing of the received data is scheduled in another 
* active object).
*/
class MLbsLocationInfoConverterObserver
    {
        
public:

    /** 
     * Gets the current version of the observer interface.
     * Derived classes should not change the implementation.
     *
     * @return Version number (currently 1.0.0) 
     */
    IMPORT_C virtual TVersion Version()  const;
    
     /** 
     * The callback is used by the LBS subsystem to inform a client application 
     * that a general purpose Locality conversion process initiated through the 
     * CLbsLocationInfoConverter::ConvertLocationInfoL method has been finished.
     *
     * Receiving the callback indicates that, unless the conversion was not successful,
     * the location data structures passed through the 
     * CLbsLocationInfoConverter::ConvertLocationInfoL method have been updated with the
     * requested information.
     *
     * @param aStatusCode Result of the conversion operation; 
     * KErrNone if at least one conversion have been successful and 
     *   the related location data structure(s) passed through the 
     *   CLbsLocationInfoConverter::ConvertLocationInfoL method 
     *   have been updated with the requested information;
     * KErrNotSupported if the conversion type specified by 
     *       aRequestedInfo is not supported by converter plugin
     * KErrNotFound if none of conversions have been successful;
     * Or other system wide error code othervise.
In here I think we should say that when the caller uses ConvertLocationInfoL( RLbsLocationInfoBaseArray& aLocationInfoArray, ..) then on  OnConversionComplete the caller should iterate around all 'n' of the  entries of the aLocationInfoArray and call GetAreaInfoL to see if there is any conversion results.  OH I see below that you say something near the ConvertLocationInfoL method. Maybe should say it here as well to ti e together
     */
    virtual void OnConversionComplete(TInt aStatusCode) = 0;

    };




/**
* The CLbsLocationInfoConverter interface is used in partnership with MLbsLocationInfoConverterObserver 
* and provides the services to convert location information between different formats.
*
* Completion of conversions is indicated by the LBS subsystem by invoking callbacks defined
* by the MLbsLocationInfoConverterObserver class.
*
* Currently the only supported conversion is from WLAN and/or GSM and/or WCDMA info into a geographic
* Locality.
*/
NONSHARABLE_CLASS(CLbsLocationInfoConverter) : public CBase
    {

public:

    /**
     * Allocates and construct a new CLbsLocationInfoConverter
     * @param[in] aObserver An observer for location conversion event.
     * @param[in] aConverterModuleId UID of the conversion plugin to be used.
     */
    IMPORT_C static CLbsLocationInfoConverter* NewL( 
                                                MLbsLocationInfoConverterObserver& aObserver,
                                                const TUid aConverterModuleId);
    
    /** 
     * Allocates and construct a new CLbsLocationInfoConverter
     * Note here to say that impementations for both  OnResolveLocationComplete and OnConversionComplete MUST be provided even though only  interested in say, ConvertLocationFingerPrintToLocalityL
     * Opens the default module. 
     * @param[in] aObserver An observer for location conversion event.
     */
    IMPORT_C static CLbsLocationInfoConverter* NewL( 
                                                MLbsLocationInfoConverterObserver& aObserver);

    /** 
     * Destructor. 
     */
    IMPORT_C ~CLbsLocationInfoConverter();
    
    /** 
     * A general purpose function that converts a single locationinfo.
     * Completion of the conversion is indicated by the LBS subsystem by invoking the
     * MLbsLocationInfoConverterObserver::OnConversionComplete 
     * and the caller can then iterate through the results returned in aLocationInfo
     *   
     * @capability Location
     *
     * @param[in] aLocationInfo location information to be converted.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     * @param[in] aRequestedInfo Location information that is requested. Currently
     * the only supported conversion is from WLAN and/or GSM and/or WCDMA info into 
     * a geographic position, hence the parameter should be set to ELbsConversionOutputPosition)
     * Need to say that can only do one of either ConvertLocationFingerPrintToLocalityL  or ConvertLocationInfoL at a time or else user is panicked

     * @leave KErrNoMemory if there is not enough memory available to perform the conversion.
     *       KErrArgument If the location info pointer is NULL.
     */
    IMPORT_C void ConvertLocationInfoL( CLbsLocationInfoBase& aLocationInfo,
               const TLbsConversionPrefs aConversionPrefs,
               const TLbsConversionOutputInfoMask aRequestedInfo );    


    /** 
     * A general purpose function that converts multiple locationinfos. 
     * The maximum allowable batch size is defined by KPositionMaxBatchSize which is 500. 
     * Exceeding KPositionMaxBatchSize will result in KErrNotSupported being reported.
     *
     * Completion of the conversion is indicated by the LBS subsystem by invoking the
     * MLbsLocationInfoConverterObserver::OnConversionComplete
     * and the caller can then iterate through the results returned in aLocationInfoArray
     *
     * @capability Location
     *
     * @param[in] aLocationInfo An array of location information to be converted.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     * @param[in] aRequestedInfo Location information that is requested. Currently
     * the only supported conversion is from WLAN and/or GSM and/or WCDMA info into 
     * a geographic position, hence the parameter should be set to ELbsConversionOutputPosition)
     *
     * @leave KErrNoMemory if there is not enough memory available to perform the conversion.
     *       KErrArgument If the location info pointer is NULL.
     *       KErrNotSupported if the conversion type specified by 
     *       aRequestedInfo is not supported or if the bitmask specified by the
     *       client has all options set. This error is also returned if the 
     *       batch size exceeds KPositionMaxBatchSize.
     */
    IMPORT_C void ConvertLocationInfoL( RLbsLocationInfoBaseArray& aLocationInfoArray,
                                       const TLbsConversionPrefs aConversionPrefs, 
                                       const TLbsConversionOutputInfoMask aRequestedInfo );
    
    /** 
     * Cancels an ongoing conversion operation.
     * If there is no ongoing conversion the cancellation request is ignored.
     *
     * Neither of the The MLbsLocationInfoConverterObserver callbacks will not be invoked if cancellation is successful.
     */
    IMPORT_C void CancelConvertLocationInfo();

private:
   
    /** 
     * Default constructor 
     */
    CLbsLocationInfoConverter();
    
    /** 
     * Symbian 2nd phase constructor. 
     */
    void ConstructL(MLbsLocationInfoConverterObserver& aObserver,
                              const TUid aConverterModuleId);

    /** 
     * Copy constructor. Restricted by default. 
     */
    CLbsLocationInfoConverter( CLbsLocationInfoConverter& );
    
    /** 
     * Overloaded assignment operator. Restricted by default. 
     */
    CLbsLocationInfoConverter& operator=( CLbsLocationInfoConverter& );
    
private:

    /** 
     * Pointer to implementation class 
     */
    CLbsLocationInfoConverterImpl* iLocationInfoConverterImpl;
    
    };



#endif // LBS_LOCATION_INFO_CONVERTER_H
