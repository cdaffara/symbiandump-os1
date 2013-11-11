/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Location information converter plug in API
*
*/


/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBS_LOC_INFO_CONVERSION_PLUGIN_BASE_H
#define LBS_LOC_INFO_CONVERSION_PLUGIN_BASE_H


// System wide
#include <e32base.h>

// Common LBS data types
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationinfoconvertercommon.h>

// Component specific
#include <lbs/lbslocinfoconverterplugincommon.h>
#include <lbs/lbslocinfoconverterpluginclasstypes.h>


class TLbsLocInfoConverterPluginParams;


/**
* The MLbsLocInfoConverterPluginObserver class is used in partnership with
* CLbsLocInfoConverterPluginBase. The observer interface is used by a 
* conversion plugin to inform LBS when a coordinate conversion process 
* initiated through the CLbsLocInfoConverterPluginBase class is completed. 
*
* Although all the interface methods in the MLbsLocInfoConverterPluginObserver
* class are synchronous, the LBS subsystem ensures they return quickly.
*/
class MLbsLocInfoConverterPluginObserver
    {
    
public:

	/** 
	 * Gets the current version of the observer interface.
	 * Derived classes should not change the implementation.
	 * 
	 * @return Version number (currently 1.0.0) 
     */
	IMPORT_C virtual TVersion Version() const;
	
	/**
	 * The callback is used by the conversion plugin to inform the LBS
	 * subsystem that a validation request initiated through the 
	 * CLbsLocInfoConverterPluginBase::ValidateClient method is completed. 
	 *
	 * @param aStatusCode Result of the validation operation. 
	 *         KErrNone if the access is granted.
	 *         KErrPermissionDenied due to a potential security violation.
         *         Other system wide error codes.
	 */
	IMPORT_C virtual void OnValidationComplete( TInt aStatusCode ) = 0;

    /** 
     * The callback is used by the conversion plugin to inform the LBS 
     * subsystem that a coordinate conversion process initiated through the 
     * CLbsLocInfoConverterPluginBase::ConvertLocationInfoL method is 
     * completed.
     *
     * Receiving the callback indicates that, unless the conversion was not 
     * successful, the location data structures passed through the 
     * CLbsLocInfoConverterPluginBase::ConvertLocationInfoL method have been 
     * updated with the requested information.
     *
     * @param aStatusCode Result of the conversion operation. 
     *         KErrNone if at least one conversion have been successful and 
     *         the related location data structure(s) passed through the 
     *         CLbsLocInfoConverterPluginBase::ConvertLocationInfoL method 
     *         have been updated with the requested information;
     *         KErrKErrNotFound if none of conversions have been successful
     *         Or other system wide error code otherwise.
     */
    IMPORT_C virtual void OnConversionComplete( TInt aStatusCode ) = 0;

private:
    
	/** 
	 * This methods is reserved for future expansion and should not be used or 
	 * implemented by derived classes.
	 */	
	IMPORT_C virtual TAny* ExtendedInterface( TInt aFunctionNumber, 
                                              TAny* aPtr1, 
                                              TAny* aPtr2 );
	
    };


/**
* The CLbsLocInfoConverterPluginBase is the abstract Ecom interface to a 
* coordinate conversion Plugin. All coordinate conversion Plugins must derive 
* from and implement this interface.
*
* The interface is used in partnership with MLbsLocInfoConverterPluginObserver
* and is used by the LBS subsystem to request conversion of location 
* information between different formats.
*
* Completion of conversions must be indicated by a plugin by invoking 
* callback(s) defined by the MLbsLocInfoConverterPluginObserver class.
*
* Although all the interface methods in the class are synchronous they must 
* return immediately. When the LBS subsystem makes a request via 
* CLbsLocInfoConverterPluginBase the Plugin must queue the request internally 
* and return control to the caller immediately. Later on, when plugin provides
* a response, it uses the corresponding notification method in the
* MLbsLocInfoConverterPluginObserver class.
*/
class CLbsLocInfoConverterPluginBase : public CBase
    {
    	
public:
    
    /**
     * Loads a coordinate conversion Plugin and initalises it.
     *
     * @param[in] aImplementationUid An implementation Uid of the module to be 
     * loaded.
     * @param[in] aParams Arguments sent to conversion plugin.
     *
     * @return Pointer to the new coordinate conversion plugin.
     */
    IMPORT_C static CLbsLocInfoConverterPluginBase* NewL( 
                                   TUid aImplementationUid,
                                   TLbsLocInfoConverterPluginParams& aParams );
                                                           
    /**
     * Destructor 
     */
    IMPORT_C virtual ~CLbsLocInfoConverterPluginBase();

	/** 
	 * Gets the current version of the interface implemented by a Plugin.
     * Derived classes should not change the implementation.
     *
     * @return Version number (currently 1.0.0) 
     */
	IMPORT_C virtual TVersion Version() const;
	
	/**
	 * Validates a client before a request is processed. This method is called
	 * by the Location Monitor Server, everytime, before a conversion request is 
	 * sent. 
	 * The result of validation is indicated by the plugin by invoking the 
	 * MLbsLocInfoConverterPluginObserver::OnValidationComplete callback.
	 *
	 * @param[in] aSecureId The secure ID of the client application/process
	 * @param[in] aVendorId The vendor ID of the client application/process
	 * @param{in] aRequestedInfo The requested converted information. This 
	 * bit field would be checked by the plugin to determine if the specified
	 * conversion can be supported for the requesting client. 
	 */
	IMPORT_C virtual void ValidateClient( const TSecureId& aSecureId, 
	                    const TVendorId& aVendorId, 
	                    const TLbsConversionOutputInfoMask aRequestedInfo) = 0;
	                    
	/**
	 * Cancels an ongoing validation operation.
	 * If there is no ongoing validation operation, the cancellation request 
	 * is ignored.
	 *
	 * The MLbsLocInfoConverterPluginObserver::OnValidationComplete callback is
	 * not invoked.
	 */
	IMPORT_C virtual void CancelValidateClient() = 0;
    
    /** 
     * Converts multiple location information.
     * 
     * Completion of the conversion is indicated by a Plugin by invoking the
     * MLbsLocInfoConverterPluginObserver::OnConversionComplete callback
     *
     * @param[in] aLocationInfo An array of location information for which 
     * corresponding position information is expected.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     * @param[in] aRequestedInfo Location information that is requested.
        
     * @leave KErrNoMemory if there is not enough memory available to perform 
     * the conversion.
     *   KErrArgument If the location info pointer is NULL.
     *   KErrNotSupported if the conversion type specified by aRequestedInfo is 
     *   not supported.
     *   Or any other system wide error code.
     */                                                       
    IMPORT_C virtual void ConvertLocationInfoL( 
                       RLbsLocationInfoArray& aLocationInfoArray,
                       const TLbsConversionPrefs aConversionPrefs, 
                       const TLbsConversionOutputInfoMask aRequestedInfo ) = 0;

    /** 
     * Cancels an ongoing conversion operation.
     * If there is no ongoing conversion the cancellation request is ignored.
     *
     * The MLbsLocInfoConverterPluginObserver::OnConversionComplete callback is 
     * not invoked.
     */
    IMPORT_C virtual void CancelConvertLocationInfo() = 0;

    
    /**
     * This method is used to read the conversion plugin's preferences such as
     * its capability to use wlan/gsm/wcdma/pos information to perform the requested conversion
     * or whether position that it returns after conversion should be retained in the cache.
     */
    IMPORT_C virtual void ConversionPluginPreferences(
            TLbsConversionPluginInfo& aPluginInfo ) = 0;
    
	/** 
	 * This method is reserved for future expansion and should not be used 
	 * or implemented by derived classes.
	 */								
	IMPORT_C virtual TAny* ExtendedInterface( TInt aFunctionNumber, 
                                               TAny* aPtr1, 
                                               TAny* aPtr2 );

private: // data

    /**
     * Instance identifier key. When instance of an implementation is
     * created by ECOM framework, the framework will assign UID
     * for it. The UID is used in destructor to notify framework that 
     * this instance is being destroyed and resources can be released.
     */
     TUid iDtor_ID_Key;
     
    };


/**
* This class encapsulates the construction parameters for a Coordinate 
* Conversion plugin.
*
* These parameters are passed by the LBS subsystem to the implementation
* of the Plugin module via ECom. The ECom constructor
* should take a single TAny* parameter. That parameter should be cast to
* a variable of type TLbsLocInfoConverterPluginParams.
*
* @see CLbsLocInfoConverterPluginBase::NewL
*/
class TLbsLocInfoConverterPluginParams : public TLbsLocInfoConverterPluginClassTypeBase
	{
	
public:
    
	/**
	 * This class encapsulates the construction parameters for a Coordinate 
	 * Conversion plugin implementation.
     *
	 * When created, the LBS subsystem must provide a reference to the 
	 * plugin's observer.  
     *
	 * @see MLbsLocInfoConverterPluginObserver
     * @see CLbsLocInfoConverterPluginBase::NewL
     */
    IMPORT_C TLbsLocInfoConverterPluginParams( 
		MLbsLocInfoConverterPluginObserver& aObserver );

	/**
	 * Returns a reference to the observer.
     *
	 * @see MLbsLocInfoConverterPluginObserver
	 * @see CLbsLocInfoConverterPluginBase::NewL
	 */
	IMPORT_C MLbsLocInfoConverterPluginObserver& Observer() const;
	
protected:
    
	/**
	 *  Handle to the plugin observer
	 */
    MLbsLocInfoConverterPluginObserver& iObserver;
    
    /**
     * Reserved for future use
     */
	TUint8 iReserved[20];
	
	};

#endif // LBS_LOC_INFO_CONVERSION_PLUGIN_BASE_H
